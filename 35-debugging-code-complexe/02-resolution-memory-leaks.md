🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 35.2 Résolution de memory leaks

## Introduction

Une **memory leak** (fuite mémoire) se produit lorsqu'un programme alloue de la mémoire dynamique (avec `malloc`, `calloc`, etc.) mais ne la libère jamais avec `free`. C'est comme ouvrir des robinets sans jamais les fermer : l'eau (la mémoire) s'accumule jusqu'à inonder votre système.

### Pourquoi les memory leaks sont-elles critiques ?

En développement, une petite fuite peut passer inaperçue. Mais en **production**, où les applications tournent pendant des jours, des semaines ou des mois :

- **Consommation croissante de RAM** : Le processus grossit continuellement
- **Dégradation des performances** : Le système commence à swapper (utiliser le disque comme RAM)
- **OOM Killer** : Linux peut tuer votre processus pour libérer de la mémoire
- **Instabilité système** : Si plusieurs processus ont des fuites, le serveur entier peut devenir instable
- **Coûts** : Sur le cloud, plus de RAM = plus cher

**Exemple concret** : Un serveur web qui fuit 1 Ko par requête :
- 1 000 requêtes/jour = 1 Mo/jour = 365 Mo/an ✅ Acceptable
- 10 000 requêtes/jour = 10 Mo/jour = 3,6 Go/an ⚠️ Problématique
- 1 000 000 requêtes/jour = 1 Go/jour = crash en quelques jours ❌ Critique

---

## Comprendre les types de memory leaks

### 1. Fuite simple : allocation oubliée

Le cas le plus basique : on alloue mais on ne libère jamais.

```c
#include <stdlib.h>

void process_request() {
    char *buffer = malloc(1024);
    // ... traitement ...
    // ❌ OUBLI : pas de free(buffer)
}

int main() {
    for (int i = 0; i < 1000000; i++) {
        process_request();  // Fuit 1024 octets à chaque appel
    }
    return 0;
}
```

**Impact** : 1 Go de fuite après 1 million d'appels.

### 2. Fuite sur chemin d'erreur

Le `free` est présent dans le cas nominal, mais oublié sur un chemin d'erreur.

```c
char* load_config(const char *filename) {
    char *config = malloc(4096);

    FILE *f = fopen(filename, "r");
    if (!f) {
        return NULL;  // ❌ FUITE : config non libéré
    }

    fread(config, 1, 4096, f);
    fclose(f);
    return config;
}
```

**Correction** :
```c
char* load_config(const char *filename) {
    char *config = malloc(4096);

    FILE *f = fopen(filename, "r");
    if (!f) {
        free(config);  // ✅ Libération avant retour
        return NULL;
    }

    fread(config, 1, 4096, f);
    fclose(f);
    return config;
}
```

### 3. Fuite par perte de pointeur

On écrase un pointeur sans libérer la mémoire qu'il pointait.

```c
void example() {
    char *data = malloc(100);
    strcpy(data, "Hello");

    data = malloc(200);  // ❌ FUITE : les 100 premiers octets sont perdus
    strcpy(data, "World");

    free(data);  // Libère seulement les 200 derniers octets
}
```

**Correction** :
```c
void example() {
    char *data = malloc(100);
    strcpy(data, "Hello");

    free(data);  // ✅ Libérer avant de réallouer
    data = malloc(200);
    strcpy(data, "World");

    free(data);
}
```

### 4. Fuite dans une structure de données

Oublier de libérer les éléments internes d'une structure.

```c
typedef struct {
    char *name;
    char *address;
} Person;

Person* create_person(const char *n, const char *a) {
    Person *p = malloc(sizeof(Person));
    p->name = malloc(strlen(n) + 1);
    p->address = malloc(strlen(a) + 1);
    strcpy(p->name, n);
    strcpy(p->address, a);
    return p;
}

void destroy_person_bad(Person *p) {
    free(p);  // ❌ FUITE : name et address non libérés
}

void destroy_person_good(Person *p) {
    free(p->name);     // ✅ Libérer les champs d'abord
    free(p->address);
    free(p);           // ✅ Puis la structure
}
```

### 5. Fuite dans une liste chaînée

Ne pas parcourir toute la liste lors de la libération.

```c
typedef struct Node {
    int data;
    struct Node *next;
} Node;

void free_list_bad(Node *head) {
    while (head) {
        free(head);
        head = head->next;  // ❌ Use-after-free ET fuite des autres nœuds
    }
}

void free_list_good(Node *head) {
    while (head) {
        Node *temp = head;
        head = head->next;  // Sauvegarder next AVANT de free
        free(temp);         // ✅ Libération correcte
    }
}
```

---

## Détection des memory leaks avec Valgrind

**Valgrind** est l'outil de référence pour détecter les fuites mémoire en C. Il intercepte tous les appels à `malloc` et `free` et vérifie que chaque allocation est bien libérée.

### Installation

```bash
# Ubuntu/Debian
sudo apt install valgrind

# Vérification
valgrind --version
```

### Utilisation basique

```bash
# Compiler avec les symboles de débogage
gcc -g -o myapp myapp.c

# Lancer avec Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./myapp
```

### Exemple d'analyse

**Code avec fuites** :
```c
// leak_example.c
#include <stdlib.h>
#include <string.h>

char* duplicate_string(const char *str) {
    char *dup = malloc(strlen(str) + 1);
    strcpy(dup, str);
    return dup;  // Appelant doit free
}

int main() {
    // Fuite 1 : allocation directe
    char *buffer = malloc(100);
    buffer[0] = 'A';
    // Pas de free(buffer)

    // Fuite 2 : valeur de retour non libérée
    duplicate_string("Hello World");

    // Fuite 3 : perte de pointeur
    char *data = malloc(50);
    data = NULL;  // Pointeur écrasé

    return 0;
}
```

**Compilation et exécution** :
```bash
gcc -g -o leak_example leak_example.c  
valgrind --leak-check=full --show-leak-kinds=all ./leak_example  
```

**Sortie Valgrind** :
```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 162 bytes in 3 blocks
==12345==   total heap usage: 3 allocs, 0 frees, 162 bytes allocated
==12345==
==12345== 50 bytes in 1 blocks are definitely lost in loss record 1 of 3
==12345==    at 0x4C2FB0F: malloc (vg_replace_malloc.c:299)
==12345==    by 0x108751: main (leak_example.c:18)
==12345==
==12345== 12 bytes in 1 blocks are definitely lost in loss record 2 of 3
==12345==    at 0x4C2FB0F: malloc (vg_replace_malloc.c:299)
==12345==    by 0x108712: duplicate_string (leak_example.c:5)
==12345==    by 0x108765: main (leak_example.c:15)
==12345==
==12345== 100 bytes in 1 blocks are definitely lost in loss record 3 of 3
==12345==    at 0x4C2FB0F: malloc (vg_replace_malloc.c:299)
==12345==    by 0x10873D: main (leak_example.c:11)
==12345==
==12345== LEAK SUMMARY:
==12345==    definitely lost: 162 bytes in 3 blocks
==12345==    indirectly lost: 0 bytes in 0 blocks
==12345==      possibly lost: 0 bytes in 0 blocks
==12345==    still reachable: 0 bytes in 0 blocks
==12345==         suppressed: 0 bytes in 0 blocks
```

### Interprétation des résultats Valgrind

| Catégorie | Signification | Action |
|-----------|---------------|--------|
| **Definitely lost** | Fuite avérée, pointeur perdu | ❌ À corriger immédiatement |
| **Indirectly lost** | Mémoire perdue via un pointeur perdu | ❌ À corriger (liée à definitely lost) |
| **Possibly lost** | Pointeur interne à un bloc | ⚠️ À investiguer |
| **Still reachable** | Mémoire accessible à la fin du programme | ✅ Acceptable (souvent) |
| **Suppressed** | Fuite connue (bibliothèque système) | ℹ️ Ignoré via suppression file |

### Options Valgrind essentielles

```bash
# Rapport détaillé avec numéros de lignes
valgrind --leak-check=full --show-leak-kinds=all \
         --track-origins=yes ./myapp

# Sauvegarder le rapport dans un fichier
valgrind --leak-check=full --log-file=valgrind.log ./myapp

# Ignorer les fuites connues des bibliothèques système
valgrind --leak-check=full --suppressions=/usr/lib/valgrind/default.supp ./myapp

# Arrêter au premier problème détecté
valgrind --leak-check=full --error-exitcode=1 ./myapp
```

---

## Détection avec AddressSanitizer (ASan)

**AddressSanitizer** est une alternative moderne et plus rapide à Valgrind (environ 2x plus lent que l'exécution normale, contre 20-30x pour Valgrind).

### Compilation avec ASan

```bash
gcc -g -fsanitize=address -o myapp myapp.c

# Exécution normale, ASan est intégré au binaire
./myapp
```

### Exemple avec ASan

Reprenant le même code avec fuites :

```bash
$ gcc -g -fsanitize=address -o leak_example leak_example.c
$ ./leak_example

=================================================================
==12346==ERROR: LeakSanitizer: detected memory leaks

Direct leak of 100 bytes in 1 object(s) allocated from:
    #0 0x7f8b4c in malloc (/usr/lib/x86_64-linux-gnu/libasan.so.5+0x10d7cf)
    #1 0x40087d in main /path/to/leak_example.c:11
    #2 0x7f8b3d in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x270b2)

Direct leak of 50 bytes in 1 object(s) allocated from:
    #0 0x7f8b4c in malloc (/usr/lib/x86_64-linux-gnu/libasan.so.5+0x10d7cf)
    #1 0x4008b1 in main /path/to/leak_example.c:18
    #2 0x7f8b3d in __libc_start_main (/lib/x86_64-linux-gnu/libc.so.6+0x270b2)

Direct leak of 12 bytes in 1 object(s) allocated from:
    #0 0x7f8b4c in malloc (/usr/lib/x86_64-linux-gnu/libasan.so.5+0x10d7cf)
    #1 0x400827 in duplicate_string /path/to/leak_example.c:5
    #2 0x400895 in main /path/to/leak_example.c:15

SUMMARY: AddressSanitizer: 162 byte(s) leaked in 3 allocation(s).
```

### ASan vs Valgrind : Quand utiliser lequel ?

| Critère | Valgrind | AddressSanitizer |
|---------|----------|------------------|
| **Vitesse** | Très lent (20-30x) | Rapide (2x) |
| **Recompilation** | Non requise | Requise (`-fsanitize=address`) |
| **Détection** | Exhaustive | Très bonne |
| **Use-after-free** | ✅ | ✅ |
| **Buffer overflow** | ✅ | ✅ (meilleur) |
| **Memory leaks** | ✅ (excellent) | ✅ |
| **Utilisation** | Débogage final | Développement quotidien |

**Recommandation** :
- **Développement** : ASan pour les tests rapides
- **CI/CD** : ASan dans la pipeline
- **Production debug** : Valgrind pour analyse approfondie
- **Combinaison** : Utilisez les deux !

---

## Méthodologie de résolution

### Étape 1 : Reproduction du problème

Pour corriger une fuite, il faut d'abord la reproduire de manière fiable.

```bash
# Test simple avec Valgrind
valgrind --leak-check=full ./myapp

# Si l'app est interactive, tester un scénario précis
valgrind --leak-check=full ./myapp < test_scenario.txt
```

### Étape 2 : Identifier la source

Valgrind vous donne la **stack trace** de l'allocation qui fuit.

```
Direct leak of 1024 bytes in 1 object(s) allocated from:
    #0 malloc (vg_replace_malloc.c:299)
    #1 load_config (config.c:45)        ← Allocation
    #2 init_app (main.c:123)
    #3 main (main.c:200)
```

**Question à se poser** : Où devrait-on libérer cette mémoire ?

### Étape 3 : Tracer le cycle de vie

Pour chaque pointeur qui fuit, suivez son cycle de vie :

1. **Où est-il alloué ?** (fourni par Valgrind)
2. **Où est-il utilisé ?**
3. **Où devrait-il être libéré ?**
4. **Pourquoi n'est-il pas libéré ?**
   - Oubli simple
   - Chemin d'erreur non géré
   - Responsabilité ambiguë (qui doit free ?)

### Étape 4 : Appliquer la correction

**Principe de propriété** : Toujours définir clairement **qui** est responsable de la libération.

```c
// ❌ AMBIGU : Qui doit free le retour de get_data() ?
char* get_data();

// ✅ CLAIR : Le retour doit être libéré par l'appelant
char* get_data_alloc();  // Suffixe _alloc indique l'allocation

// ✅ ENCORE MIEUX : Documentation explicite
/**
 * Récupère les données de configuration.
 * @return Pointeur vers les données. L'appelant DOIT appeler free().
 */
char* get_data();
```

### Étape 5 : Vérification

Après correction, **toujours** revérifier avec Valgrind :

```bash
valgrind --leak-check=full --error-exitcode=1 ./myapp  
echo $?  # Doit retourner 0 si aucune fuite  
```

---

## Cas pratiques de résolution

### Cas 1 : Fuite dans un serveur TCP

**Symptôme** : Un serveur TCP voit sa mémoire augmenter de 10 Mo par heure.

**Code suspect** :
```c
void handle_client(int client_fd) {
    char *buffer = malloc(BUFFER_SIZE);

    while (1) {
        int n = read(client_fd, buffer, BUFFER_SIZE);
        if (n <= 0) break;

        process_data(buffer, n);
    }

    close(client_fd);
    // ❌ FUITE : buffer jamais libéré
}
```

**Diagnostic avec Valgrind** :
```bash
# Lancer le serveur sous Valgrind
valgrind --leak-check=full ./server &

# Envoyer quelques requêtes test
echo "test" | nc localhost 8080

# Arrêter le serveur (Ctrl+C) et voir le rapport
```

**Correction** :
```c
void handle_client(int client_fd) {
    char *buffer = malloc(BUFFER_SIZE);

    while (1) {
        int n = read(client_fd, buffer, BUFFER_SIZE);
        if (n <= 0) break;

        process_data(buffer, n);
    }

    close(client_fd);
    free(buffer);  // ✅ Libération ajoutée
}
```

### Cas 2 : Fuite dans une fonction récursive

**Symptôme** : Consommation exponentielle de mémoire lors du parsing d'un arbre.

**Code problématique** :
```c
typedef struct Node {
    char *value;
    struct Node *left;
    struct Node *right;
} Node;

void print_tree(Node *node) {
    if (!node) return;

    char *formatted = malloc(256);
    sprintf(formatted, "Node: %s", node->value);
    printf("%s\n", formatted);
    // ❌ FUITE : formatted jamais libéré, appelé récursivement

    print_tree(node->left);
    print_tree(node->right);
}
```

**Correction** :
```c
void print_tree(Node *node) {
    if (!node) return;

    char *formatted = malloc(256);
    sprintf(formatted, "Node: %s", node->value);
    printf("%s\n", formatted);
    free(formatted);  // ✅ Libération immédiate

    print_tree(node->left);
    print_tree(node->right);
}
```

**Amélioration** (éviter malloc en boucle) :
```c
void print_tree(Node *node) {
    if (!node) return;

    char formatted[256];  // ✅ Stack allocation, pas de free nécessaire
    snprintf(formatted, 256, "Node: %s", node->value);
    printf("%s\n", formatted);

    print_tree(node->left);
    print_tree(node->right);
}
```

### Cas 3 : Fuite liée à realloc

**Code problématique** :
```c
void grow_buffer(char **buf, size_t new_size) {
    *buf = realloc(*buf, new_size);  // ❌ FUITE si realloc échoue
}

int main() {
    char *data = malloc(100);
    grow_buffer(&data, 1000000000000);  // Allocation énorme qui va échouer
    // data est maintenant NULL, les 100 octets initiaux sont perdus
    free(data);  // free(NULL) est sûr mais ne libère rien
    return 0;
}
```

**Correction** :
```c
int grow_buffer(char **buf, size_t new_size) {
    char *new_buf = realloc(*buf, new_size);
    if (!new_buf) {
        return -1;  // ✅ Échec mais *buf est toujours valide
    }
    *buf = new_buf;
    return 0;
}

int main() {
    char *data = malloc(100);
    if (grow_buffer(&data, 1000000000000) != 0) {
        fprintf(stderr, "Realloc failed\n");
        free(data);  // ✅ Libération de l'ancien buffer
        return 1;
    }
    free(data);
    return 0;
}
```

### Cas 4 : Fuite dans un plugin dynamique

**Symptôme** : Un système de plugins voit sa mémoire croître à chaque chargement/déchargement.

**Code problématique** :
```c
typedef struct {
    void *handle;
    char *name;
    PluginAPI *api;
} Plugin;

Plugin* load_plugin(const char *path) {
    Plugin *p = malloc(sizeof(Plugin));
    p->name = strdup(path);
    p->handle = dlopen(path, RTLD_NOW);
    p->api = dlsym(p->handle, "plugin_api");
    return p;
}

void unload_plugin(Plugin *p) {
    dlclose(p->handle);
    free(p);
    // ❌ FUITE : p->name jamais libéré
}
```

**Correction** :
```c
void unload_plugin(Plugin *p) {
    if (!p) return;

    dlclose(p->handle);
    free(p->name);  // ✅ Libérer le nom alloué par strdup
    free(p);
}
```

---

## Outils complémentaires

### 1. LeakSanitizer (LSan)

Fait partie de ASan, mais peut être utilisé seul :

```bash
gcc -g -fsanitize=leak -o myapp myapp.c
./myapp
```

Plus léger que ASan complet, se concentre uniquement sur les fuites.

### 2. Massif (profiler de tas Valgrind)

Analyse la **croissance** de la mémoire au fil du temps :

```bash
valgrind --tool=massif ./myapp  
ms_print massif.out.12345  
```

Génère un graphique textuel de l'utilisation mémoire :

```
    MB
25.00 ^                                               #
      |                                             #::
      |                                           #::: :
20.00 +                                         #::::: ::
      |                                       @#::::::: :::
      |                                     @@#:::::::: ::::
15.00 +                                   @@@#::::::::: :::::
      |                                 @@@@#:::::::::: ::::::
      |                               @@@@@#::::::::::: :::::::
10.00 +                             @@@@@@#:::::::::::: ::::::::
      |                           @@@@@@@#::::::::::::: :::::::::
      |                         @@@@@@@@#:::::::::::::: ::::::::::
 5.00 +                       @@@@@@@@@#::::::::::::::: :::::::::::
      |                     @@@@@@@@@@#:::::::::::::::: ::::::::::::
      |                   @@@@@@@@@@@#::::::::::::::::: :::::::::::::
 0.00 +----------------------------------------------------------------------->s
      0                                                                    10.00
```

Permet d'identifier **où** la mémoire est consommée.

### 3. mtrace (GNU C Library)

Mécanisme de traçage intégré à la glibc :

```c
#include <mcheck.h>

int main() {
    mtrace();  // Activer le traçage

    char *leak = malloc(100);
    // ... code ...

    muntrace();  // Désactiver le traçage
    return 0;
}
```

```bash
# Compiler et exécuter
gcc -o myapp myapp.c  
MALLOC_TRACE=mtrace.log ./myapp  

# Analyser
mtrace myapp mtrace.log
```

**Avantage** : Pas besoin d'outils externes.  
**Inconvénient** : Moins puissant que Valgrind.  

### 4. Electric Fence

Détecte les débordements de buffer en plaçant chaque allocation sur sa propre page mémoire :

```bash
# Installation
sudo apt install electric-fence

# Utilisation
gcc -o myapp myapp.c -lefence
./myapp
```

Provoque un segfault immédiat lors d'un buffer overflow.

---

## Prévention des memory leaks

### 1. Patterns de gestion mémoire

#### Pattern : RAII-like avec cleanup attribute (GCC)

```c
#define AUTO_FREE __attribute__((cleanup(cleanup_free)))

void cleanup_free(void *p) {
    free(*(void**)p);
}

void example() {
    AUTO_FREE char *data = malloc(100);
    // ... utilisation ...
    // ✅ Libération automatique en sortie de scope
}
```

#### Pattern : Paires alloc/free dans la même fonction

```c
// ❌ MAL : Allocation et libération séparées
char* create_buffer() {
    return malloc(1024);
}
void use_buffer(char *buf) { /* ... */ }  
void destroy_buffer(char *buf) {  
    free(buf);
}

// ✅ MIEUX : Tout dans une fonction
void process_with_buffer() {
    char *buf = malloc(1024);
    if (!buf) return;

    use_buffer(buf);

    free(buf);  // Garanti dans la même fonction
}
```

### 2. Conventions de nommage

Indiquez clairement qui est responsable de la libération :

```c
// Suffixes explicites
char* string_alloc();     // Appelant doit free  
char* string_get();       // Pointeur vers donnée statique/partagée, NE PAS free  
char* string_dup();       // Duplique, appelant doit free  

// Préfixes
void take_ownership(char *data);   // Prend la responsabilité, libérera  
void borrow(const char *data);     // Emprunte, ne libérera pas  
```

### 3. Documentation Doxygen

```c
/**
 * Charge la configuration depuis un fichier.
 *
 * @param filename Chemin du fichier de configuration
 * @return Pointeur vers la structure Config allouée dynamiquement.
 *         L'appelant DOIT libérer avec config_free().
 *         Retourne NULL en cas d'erreur.
 */
Config* config_load(const char *filename);

/**
 * Libère une structure Config.
 *
 * @param cfg Pointeur vers la Config à libérer (peut être NULL)
 */
void config_free(Config *cfg);
```

### 4. Vérifications systématiques

#### Checklist avant commit

```bash
#!/bin/bash
# pre-commit-leak-check.sh

echo "🔍 Vérification des memory leaks..."

# Compiler avec ASan
gcc -g -fsanitize=address -o test_app test_app.c

# Exécuter les tests
./test_app

if [ $? -ne 0 ]; then
    echo "❌ Memory leaks détectées ! Commit bloqué."
    exit 1
fi

echo "✅ Aucune fuite détectée."  
exit 0  
```

#### Intégration dans le CI/CD

```yaml
# .github/workflows/memory-check.yml
name: Memory Leak Check

on: [push, pull_request]

jobs:
  valgrind:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4

      - name: Install Valgrind
        run: sudo apt-get install -y valgrind

      - name: Build with debug symbols
        run: gcc -g -o myapp myapp.c

      - name: Run Valgrind
        run: |
          valgrind --leak-check=full --error-exitcode=1 ./myapp

      - name: Upload Valgrind report
        if: failure()
        uses: actions/upload-artifact@v4
        with:
          name: valgrind-report
          path: valgrind.log
```

### 5. Wrappers de débogage

Créez des wrappers pour tracer les allocations :

```c
#ifdef DEBUG_MALLOC
#include <stdio.h>

void* debug_malloc(size_t size, const char *file, int line) {
    void *ptr = malloc(size);
    fprintf(stderr, "[MALLOC] %p : %zu bytes at %s:%d\n", ptr, size, file, line);
    return ptr;
}

void debug_free(void *ptr, const char *file, int line) {
    fprintf(stderr, "[FREE] %p at %s:%d\n", ptr, file, line);
    free(ptr);
}

#define malloc(size) debug_malloc(size, __FILE__, __LINE__)
#define free(ptr) debug_free(ptr, __FILE__, __LINE__)
#endif
```

Permet de suivre manuellement les allocations/libérations.

---

## Analyse de fuites dans un code legacy

### Stratégie progressive

Vous héritez d'un code avec des dizaines de fuites. Comment procéder ?

#### 1. Établir une baseline

```bash
# Exécuter Valgrind et sauvegarder le rapport initial
valgrind --leak-check=full ./legacy_app 2>&1 | tee baseline.txt

# Compter les fuites
grep "definitely lost" baseline.txt
# Exemple : definitely lost: 45,678 bytes in 234 blocks
```

#### 2. Prioriser les fuites

Toutes les fuites ne sont pas égales :

| Priorité | Type | Exemple |
|----------|------|---------|
| 🔴 **P0** | Fuites dans des boucles | Fuite de 1 Ko par requête |
| 🟠 **P1** | Fuites à l'initialisation | Fuite de 10 Mo au démarrage |
| 🟡 **P2** | Fuites sur chemins rares | Fuite lors d'erreurs |
| 🟢 **P3** | Still reachable | Mémoire non libérée à la fin |

#### 3. Corriger par itérations

```bash
# Itération 1 : Corriger les fuites P0 (boucles)
# ... modifications code ...

valgrind --leak-check=full ./legacy_app 2>&1 | tee iteration1.txt
# Résultat : definitely lost: 12,345 bytes in 67 blocks (-72% ✅)

# Itération 2 : Corriger les fuites P1 (initialisation)
# ... modifications code ...

# Continuer jusqu'à 0 fuite critique
```

#### 4. Ajouter des tests de non-régression

```c
// test_no_leaks.c
void test_request_handling() {
    // Simuler 1000 requêtes
    for (int i = 0; i < 1000; i++) {
        handle_request("test");
    }
}

int main() {
    test_request_handling();
    return 0;
}
```

```bash
# Vérifier automatiquement
valgrind --leak-check=full --error-exitcode=1 ./test_no_leaks
```

---

## Fuites dans les bibliothèques externes

### Identifier les fausses fuites

Certaines bibliothèques (OpenSSL, X11, etc.) ont des "fuites" normales : elles libèrent la mémoire uniquement lors de l'appel à une fonction de cleanup.

**Exemple avec OpenSSL** :
```c
#include <openssl/ssl.h>
#include <openssl/evp.h>

int main() {
    // OpenSSL 1.1.0+ : initialisation et cleanup automatiques
    SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());

    // ... utilisation SSL ...

    SSL_CTX_free(ctx);

    // Certaines allocations internes d'OpenSSL ne sont libérées
    // qu'à la terminaison du processus. Valgrind les signale comme
    // "still reachable" : ce sont des allocations intentionnelles,
    // pas de vraies fuites.
    return 0;
}
```

**Valgrind signalera ces allocations internes. Pour éviter le bruit dans les rapports, utilisez un fichier de suppression.**

### Créer un fichier de suppression

Pour ignorer les fuites connues :

```bash
# Générer les suppressions
valgrind --leak-check=full --gen-suppressions=all ./myapp 2>&1 | grep -A 5 "insert_a_suppression_name_here" > openssl.supp
```

Éditer `openssl.supp` :
```
{
   OpenSSL_init_leak
   Memcheck:Leak
   fun:malloc
   fun:CRYPTO_malloc
   fun:sk_new
}
```

Utiliser :
```bash
valgrind --suppressions=openssl.supp --leak-check=full ./myapp
```

---

## Monitoring de la mémoire en production

### 1. Surveillance système

```bash
# Voir l'utilisation mémoire d'un processus
ps aux | grep myapp  
top -p $(pidof myapp)  

# Détails RSS (Resident Set Size)
cat /proc/$(pidof myapp)/status | grep VmRSS
```

### 2. Script de monitoring

```bash
#!/bin/bash
# monitor_memory.sh

PID=$(pidof myapp)  
LOG="/var/log/myapp_memory.log"  

while true; do
    MEM=$(ps -o rss= -p $PID)
    TIMESTAMP=$(date "+%Y-%m-%d %H:%M:%S")
    echo "$TIMESTAMP - RSS: $MEM KB" >> $LOG

    # Alerte si > 1 GB
    if [ $MEM -gt 1048576 ]; then
        echo "⚠️ ALERT: Memory usage > 1GB" | mail -s "Memory Alert" admin@example.com
    fi

    sleep 60
done
```

### 3. Intégration avec Prometheus

```c
// Exposer une métrique Prometheus
#include <prometheus-client-c/prometheus.h>

prom_gauge_t *memory_usage_gauge = prom_gauge_new("app_memory_bytes", "Current memory usage");

void update_memory_metric() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    prom_gauge_set(memory_usage_gauge, usage.ru_maxrss * 1024);
}
```

---

## Checklist de résolution de memory leaks

### Avant de coder

- [ ] Définir clairement la propriété de chaque allocation (qui doit `free` ?)
- [ ] Documenter les fonctions qui retournent de la mémoire allouée
- [ ] Utiliser des conventions de nommage explicites

### Pendant le développement

- [ ] Compiler avec `-Wall -Wextra -Werror`
- [ ] Tester régulièrement avec ASan (`-fsanitize=address`)
- [ ] Chaque `malloc` doit avoir un `free` correspondant

### Avant de merger

- [ ] Exécuter Valgrind sur tous les chemins d'exécution
- [ ] Vérifier les chemins d'erreur (sont-ils testés ?)
- [ ] Code review axé sur la gestion mémoire

### En production

- [ ] Monitoring de l'utilisation mémoire
- [ ] Alertes sur croissance anormale
- [ ] Core dumps activés pour analyse post-mortem

---

## Résumé

La résolution de memory leaks est une compétence essentielle pour tout développeur C. Les points clés :

### Outils indispensables
- **Valgrind** : Analyse exhaustive, référence pour la détection
- **AddressSanitizer** : Rapide, idéal pour le développement quotidien
- **Massif** : Profiling du tas pour identifier les croissances

### Méthodologie
1. **Détecter** : Valgrind ou ASan
2. **Localiser** : Stack trace de l'allocation
3. **Comprendre** : Tracer le cycle de vie du pointeur
4. **Corriger** : Ajouter le `free` au bon endroit
5. **Vérifier** : Re-tester avec les outils

### Prévention
- Définir clairement la **propriété** de chaque allocation
- Utiliser des **patterns** (RAII-like, alloc/free dans même fonction)
- **Documenter** les responsabilités de libération
- **Tester** automatiquement dans le CI/CD

### En production
- **Monitorer** l'utilisation mémoire
- **Analyser** les tendances (croissance = fuite probable)
- **Réagir** rapidement aux alertes

Une application sans fuite mémoire est :
- ✅ Stable sur la durée
- ✅ Prévisible en consommation de ressources
- ✅ Économique (moins de RAM = moins de coûts)
- ✅ Professionnelle

**La gestion de la mémoire n'est pas optionnelle en C : c'est votre responsabilité !**

---


⏭️ [Investigation de race conditions](/35-debugging-code-complexe/03-investigation-race-conditions.md)
