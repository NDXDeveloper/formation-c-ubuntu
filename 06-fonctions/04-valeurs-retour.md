🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 6.4 Valeurs de retour et codes d'erreur

## Introduction

Une fonction peut effectuer un calcul, une opération, ou une tâche... mais comment communique-t-elle le résultat à celui qui l'a appelée ? Et si quelque chose se passe mal, comment signaler une erreur ?

En C, contrairement à des langages plus modernes, il n'y a **pas de système d'exceptions**. Toute la communication se fait via :
- ✅ La **valeur de retour** de la fonction
- ✅ Les **codes d'erreur** (conventions)
- ✅ La variable globale **errno**
- ✅ Les **paramètres de sortie** (via pointeurs)

Comprendre ces mécanismes est essentiel pour :
- Écrire du code robuste qui gère les erreurs
- Utiliser correctement les bibliothèques C standards
- Déboguer efficacement vos programmes
- Suivre les conventions de la communauté C

---

## La valeur de retour : Concept de base

### Syntaxe

Toute fonction en C (sauf `void`) retourne une valeur via le mot-clé `return` :

```c
type_retour nom_fonction(paramètres) {
    // ... code ...
    return valeur;  // valeur doit être du type type_retour
}
```

**Exemple simple :**

```c
int additionner(int a, int b) {
    int somme = a + b;
    return somme;  // Retourne un int
}

int main(void) {
    int resultat = additionner(5, 3);  // Récupère la valeur retournée
    printf("Résultat : %d\n", resultat);  // Affiche 8
    return 0;
}
```

---

### Que se passe-t-il au retour ?

Quand une fonction exécute `return valeur;` :

1. La valeur est **copiée** dans un registre du processeur (ex: `EAX` ou `RAX`)
2. Le **stack frame** de la fonction est **détruit**
3. L'exécution reprend dans la fonction appelante
4. La valeur du registre peut être **assignée** à une variable ou **utilisée** directement

```c
int carre(int n) {
    return n * n;
}

int main(void) {
    int x = carre(5);           // Valeur assignée à x
    printf("%d\n", carre(3));   // Valeur utilisée directement
    carre(7);                   // ⚠️ Valeur ignorée (valide mais inutile)
    return 0;
}
```

---

## Types de retour possibles

### 1. Types primitifs

Tous les types de base peuvent être retournés :

```c
int retourne_entier(void) {
    return 42;
}

char retourne_caractere(void) {
    return 'A';
}

float retourne_flottant(void) {
    return 3.14f;
}

double retourne_double(void) {
    return 2.71828;
}

_Bool retourne_booleen(void) {  // Ou bool avec <stdbool.h>
    return 1;  // true
}
```

---

### 2. Pointeurs

Une fonction peut retourner un pointeur (adresse mémoire) :

```c
int* retourne_pointeur(void) {
    static int valeur = 100;  // static : persiste après la fonction
    return &valeur;
}

char* retourne_chaine(void) {
    return "Hello";  // Pointeur vers une chaîne littérale (en mémoire statique)
}
```

**⚠️ DANGER** : Ne jamais retourner un pointeur vers une variable locale non-static :

```c
int* fonction_dangereuse(void) {
    int x = 42;
    return &x;  // ❌ x est détruit en sortie, pointeur invalide !
}
```

---

### 3. Structures

Les structures peuvent être retournées par valeur (copie complète) :

```c
struct Point {
    int x;
    int y;
};

struct Point creer_point(int x, int y) {
    struct Point p;
    p.x = x;
    p.y = y;
    return p;  // Toute la structure est copiée
}

int main(void) {
    struct Point origine = creer_point(0, 0);
    printf("Point : (%d, %d)\n", origine.x, origine.y);
    return 0;
}
```

**Note** : Pour les grandes structures, préférez retourner un pointeur pour éviter la copie coûteuse.

---

### 4. void : Pas de valeur de retour

Une fonction `void` ne retourne rien :

```c
void afficher_message(const char *msg) {
    printf("%s\n", msg);
    return;  // Optionnel, peut être omis
}

void calculer_et_afficher(int a, int b) {
    int somme = a + b;
    printf("Somme : %d\n", somme);
    // return implicite en fin de fonction
}
```

---

## Retours multiples

Une fonction peut avoir **plusieurs points de retour** :

```c
int maximum(int a, int b) {
    if (a > b) {
        return a;  // Retour anticipé
    } else {
        return b;
    }
}

// Version plus concise
int maximum_v2(int a, int b) {
    return (a > b) ? a : b;
}
```

**Retour anticipé (early return)** pour gérer les cas d'erreur :

```c
int diviser(int a, int b, int *resultat) {
    if (b == 0) {
        return -1;  // Code d'erreur : division par zéro
    }

    *resultat = a / b;
    return 0;  // Succès
}
```

---

## Codes d'erreur : Convention fondamentale en C

En C, il n'y a **pas d'exceptions** (try/catch). La gestion des erreurs repose sur des **conventions** :

### Convention classique : 0 = Succès, Non-zéro = Erreur

La plupart des fonctions C suivent cette règle :

```c
// Retourne 0 en cas de succès, -1 (ou autre valeur) en cas d'erreur
int fonction(void) {
    // ... opération ...

    if (/* erreur */) {
        return -1;  // Échec
    }

    return 0;  // Succès
}
```

**Exemple : Ouvrir un fichier**

```c
#include <stdio.h>
#include <stdlib.h>

int lire_fichier(const char *chemin) {
    FILE *fichier = fopen(chemin, "r");

    if (fichier == NULL) {
        printf("Erreur : impossible d'ouvrir %s\n", chemin);
        return -1;  // Code d'erreur
    }

    // ... traiter le fichier ...

    fclose(fichier);
    return 0;  // Succès
}

int main(void) {
    int status = lire_fichier("data.txt");

    if (status != 0) {
        printf("La lecture a échoué\n");
        return EXIT_FAILURE;  // Constante stdlib : 1
    }

    printf("Lecture réussie\n");
    return EXIT_SUCCESS;  // Constante stdlib : 0
}
```

---

### Codes d'erreur multiples

Vous pouvez retourner différentes valeurs pour différentes erreurs :

```c
#define ERR_FICHIER_INTROUVABLE  -1
#define ERR_MEMOIRE_INSUFFISANTE -2
#define ERR_FORMAT_INVALIDE      -3
#define SUCCESS                   0

int charger_configuration(const char *fichier) {
    FILE *f = fopen(fichier, "r");
    if (f == NULL) {
        return ERR_FICHIER_INTROUVABLE;
    }

    char *buffer = malloc(1024);
    if (buffer == NULL) {
        fclose(f);
        return ERR_MEMOIRE_INSUFFISANTE;
    }

    // ... parser le fichier ...

    if (/* format incorrect */) {
        free(buffer);
        fclose(f);
        return ERR_FORMAT_INVALIDE;
    }

    free(buffer);
    fclose(f);
    return SUCCESS;
}

int main(void) {
    int code = charger_configuration("config.ini");

    switch (code) {
        case SUCCESS:
            printf("Configuration chargée\n");
            break;
        case ERR_FICHIER_INTROUVABLE:
            printf("Erreur : fichier introuvable\n");
            break;
        case ERR_MEMOIRE_INSUFFISANTE:
            printf("Erreur : mémoire insuffisante\n");
            break;
        case ERR_FORMAT_INVALIDE:
            printf("Erreur : format invalide\n");
            break;
    }

    return 0;
}
```

---

## La variable errno

### Qu'est-ce qu'errno ?

`errno` est une **variable globale** définie dans `<errno.h>` qui contient le **code d'erreur** de la dernière fonction système ou bibliothèque qui a échoué.

```c
#include <errno.h>
#include <string.h>  // Pour strerror()

extern int errno;  // Variable globale (déjà déclarée dans errno.h)
```

---

### Fonctionnement

Quand une fonction de la bibliothèque standard échoue, elle :
1. Retourne une valeur indiquant l'échec (souvent `-1`, `NULL`, ou `EOF`)
2. Définit `errno` avec un **code d'erreur spécifique**

**Exemple : fopen**

```c
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(void) {
    FILE *fichier = fopen("inexistant.txt", "r");

    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture\n");
        printf("Code errno : %d\n", errno);
        printf("Message : %s\n", strerror(errno));  // Convertit errno en message lisible
    }

    return 0;
}
```

**Sortie possible :**
```
Erreur lors de l'ouverture
Code errno : 2
Message : No such file or directory
```

---

### Codes errno courants

Quelques codes définis dans `<errno.h>` :

| Constante | Valeur | Signification |
|-----------|--------|---------------|
| `ENOENT` | 2 | No such file or directory |
| `ENOMEM` | 12 | Out of memory |
| `EACCES` | 13 | Permission denied |
| `EINVAL` | 22 | Invalid argument |
| `EEXIST` | 17 | File exists |
| `EAGAIN` | 11 | Resource temporarily unavailable |

---

### Utilisation correcte d'errno

**Règles importantes** :

1. **Vérifier d'abord la valeur de retour** de la fonction
2. **Puis consulter errno** uniquement si la fonction a échoué
3. **Réinitialiser errno** avant chaque appel (optionnel mais recommandé)

```c
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(void) {
    errno = 0;  // Réinitialisation

    FILE *f = fopen("test.txt", "r");

    if (f == NULL) {  // D'ABORD : vérifier le retour
        if (errno != 0) {  // ENSUITE : consulter errno
            printf("Erreur : %s\n", strerror(errno));
        } else {
            printf("Erreur inconnue\n");
        }
        return 1;
    }

    // ... utiliser f ...

    fclose(f);
    return 0;
}
```

---

### Fonction perror()

`perror()` est un raccourci pour afficher le message d'erreur errno :

```c
#include <stdio.h>
#include <errno.h>

int main(void) {
    FILE *f = fopen("inexistant.txt", "r");

    if (f == NULL) {
        perror("Erreur fopen");  // Affiche : "Erreur fopen: No such file or directory"
        return 1;
    }

    fclose(f);
    return 0;
}
```

**Format de perror** : `"message fourni: description errno"`

---

## Paramètres de sortie (Output Parameters)

Quand une fonction doit retourner **plusieurs valeurs** ou **réserver le code de retour pour les erreurs**, on utilise des **pointeurs comme paramètres de sortie** :

### Exemple : Division sécurisée

```c
int diviser(int a, int b, int *quotient, int *reste) {
    if (b == 0) {
        return -1;  // Erreur : division par zéro
    }

    *quotient = a / b;  // Écrit le quotient via le pointeur
    *reste = a % b;     // Écrit le reste via le pointeur

    return 0;  // Succès
}

int main(void) {
    int q, r;
    int status = diviser(17, 5, &q, &r);

    if (status != 0) {
        printf("Erreur : division par zéro\n");
        return 1;
    }

    printf("17 / 5 = %d reste %d\n", q, r);  // Affiche : 17 / 5 = 3 reste 2
    return 0;
}
```

**Avantages** :
- Le code de retour est **réservé pour signaler les erreurs**
- Plusieurs valeurs peuvent être retournées simultanément
- Pattern très courant dans l'API C standard

---

### Exemple : Fonction strtol (standard C)

La fonction `strtol` (string to long) utilise ce pattern :

```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(void) {
    const char *str = "12345abc";
    char *endptr;  // Pointeur de sortie

    errno = 0;
    long valeur = strtol(str, &endptr, 10);

    if (errno != 0) {
        perror("strtol");
        return 1;
    }

    printf("Valeur convertie : %ld\n", valeur);
    printf("Caractères non convertis : %s\n", endptr);

    return 0;
}
```

**Sortie :**
```
Valeur convertie : 12345
Caractères non convertis : abc
```

`endptr` pointe vers le premier caractère non converti, permettant de détecter une conversion partielle.

---

## Patterns de gestion d'erreur

### Pattern 1 : Retour simple 0/1 ou 0/-1

```c
int operation(void) {
    if (/* erreur */) {
        return -1;
    }

    // ... code ...

    return 0;  // Succès
}

// Utilisation
if (operation() != 0) {
    // Gérer l'erreur
}
```

**Avantage** : Simple, direct
**Inconvénient** : Un seul code d'erreur

---

### Pattern 2 : Codes d'erreur multiples avec enum

```c
typedef enum {
    SUCCESS = 0,
    ERR_INVALID_PARAM,
    ERR_OUT_OF_MEMORY,
    ERR_FILE_NOT_FOUND,
    ERR_PERMISSION_DENIED
} ErrorCode;

ErrorCode ouvrir_base_donnees(const char *chemin) {
    if (chemin == NULL) {
        return ERR_INVALID_PARAM;
    }

    // ... tentative d'ouverture ...

    if (/* fichier introuvable */) {
        return ERR_FILE_NOT_FOUND;
    }

    if (/* permission refusée */) {
        return ERR_PERMISSION_DENIED;
    }

    return SUCCESS;
}

// Utilisation
ErrorCode code = ouvrir_base_donnees("data.db");
if (code != SUCCESS) {
    switch (code) {
        case ERR_INVALID_PARAM:
            printf("Paramètre invalide\n");
            break;
        case ERR_FILE_NOT_FOUND:
            printf("Fichier introuvable\n");
            break;
        // ...
    }
}
```

**Avantage** : Codes d'erreur clairs et typés
**Inconvénient** : Verbeux

---

### Pattern 3 : Valeur spéciale en cas d'erreur

Certaines fonctions retournent une **valeur spéciale** pour indiquer l'échec :

```c
// malloc retourne NULL en cas d'échec
int *tableau = malloc(100 * sizeof(int));
if (tableau == NULL) {
    printf("Erreur : allocation mémoire échouée\n");
    return -1;
}

// fopen retourne NULL en cas d'échec
FILE *f = fopen("fichier.txt", "r");
if (f == NULL) {
    printf("Erreur : impossible d'ouvrir le fichier\n");
    return -1;
}

// getchar retourne EOF en cas d'erreur ou fin de fichier
int c = getchar();
if (c == EOF) {
    if (feof(stdin)) {
        printf("Fin de fichier\n");
    } else if (ferror(stdin)) {
        printf("Erreur de lecture\n");
    }
}
```

---

### Pattern 4 : Goto pour nettoyage (cleanup)

Le `goto` est **controversé**, mais utile pour le nettoyage en cas d'erreur :

```c
int traiter_fichier(const char *chemin) {
    FILE *f = NULL;
    char *buffer = NULL;
    int status = -1;  // Par défaut : échec

    f = fopen(chemin, "r");
    if (f == NULL) {
        goto cleanup;  // Saute au nettoyage
    }

    buffer = malloc(1024);
    if (buffer == NULL) {
        goto cleanup;
    }

    // ... traitement ...

    status = 0;  // Succès

cleanup:
    if (buffer != NULL) {
        free(buffer);
    }
    if (f != NULL) {
        fclose(f);
    }

    return status;
}
```

**Avantage** : Code de nettoyage centralisé, évite la duplication
**Inconvénient** : Utilisation de `goto` (considéré comme mauvaise pratique par certains)

---

## Assertions : Détecter les bugs

Les **assertions** vérifient des conditions qui **doivent toujours être vraies**. Si fausses → **crash immédiat** du programme (en mode debug).

```c
#include <assert.h>

int factorielle(int n) {
    assert(n >= 0);  // Précondition : n doit être positif

    if (n == 0 || n == 1) {
        return 1;
    }

    return n * factorielle(n - 1);
}

int main(void) {
    int result = factorielle(-5);  // ❌ Crash avec message d'erreur
    return 0;
}
```

**Sortie (crash) :**
```
Assertion failed: (n >= 0), function factorielle, file test.c, line 4.
Abort trap: 6
```

---

### Quand utiliser assert ?

| Utilisation | assert | Code d'erreur |
|-------------|--------|---------------|
| **Bug de programmation** (violation de contrat) | ✅ | ❌ |
| **Erreur d'exécution** (fichier manquant, réseau down) | ❌ | ✅ |
| **Validation d'entrée utilisateur** | ❌ | ✅ |
| **Vérification d'invariants internes** | ✅ | ❌ |

**Règle** :
- `assert` pour les **bugs du développeur**
- Codes d'erreur pour les **problèmes d'exécution**

---

### Désactiver les assertions en production

Les assertions peuvent être **désactivées** en définissant `NDEBUG` :

```bash
gcc -DNDEBUG programme.c -o programme
```

Toutes les `assert()` deviennent alors des no-ops (aucune opération).

```c
#define NDEBUG  // Doit être avant #include <assert.h>
#include <assert.h>

int main(void) {
    int x = -1;
    assert(x > 0);  // Ne fait rien si NDEBUG est défini
    printf("x = %d\n", x);
    return 0;
}
```

---

## Conventions des fonctions système (POSIX)

Les fonctions POSIX (Linux/Unix) suivent des conventions strictes :

### Fonctions retournant int

```c
int open(const char *pathname, int flags);
int close(int fd);
int read(int fd, void *buf, size_t count);
```

**Convention** :
- Retourne `-1` en cas d'erreur
- Définit `errno` avec le code d'erreur
- Retourne une valeur ≥ 0 en cas de succès

---

### Fonctions retournant des pointeurs

```c
void *malloc(size_t size);
FILE *fopen(const char *path, const char *mode);
```

**Convention** :
- Retourne `NULL` en cas d'erreur
- Définit `errno` avec le code d'erreur
- Retourne un pointeur valide en cas de succès

---

## Exemple complet : Gestionnaire de configuration

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define MAX_LINE 256

typedef enum {
    CONFIG_SUCCESS = 0,
    CONFIG_ERR_FILE_NOT_FOUND,
    CONFIG_ERR_INVALID_FORMAT,
    CONFIG_ERR_OUT_OF_MEMORY
} ConfigError;

// Structure pour stocker la configuration
typedef struct {
    char nom[64];
    int port;
    char host[128];
} Config;

// Fonction de chargement avec gestion d'erreur complète
ConfigError charger_configuration(const char *fichier, Config *config) {
    // Validation des paramètres
    if (fichier == NULL || config == NULL) {
        return CONFIG_ERR_INVALID_FORMAT;
    }

    // Tentative d'ouverture
    FILE *f = fopen(fichier, "r");
    if (f == NULL) {
        if (errno == ENOENT) {
            return CONFIG_ERR_FILE_NOT_FOUND;
        }
        return CONFIG_ERR_INVALID_FORMAT;
    }

    // Lecture ligne par ligne
    char ligne[MAX_LINE];
    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        // Enlever le retour à la ligne
        ligne[strcspn(ligne, "\n")] = 0;

        // Parser "clé=valeur"
        char *equal = strchr(ligne, '=');
        if (equal == NULL) {
            continue;  // Ignorer les lignes mal formées
        }

        *equal = '\0';
        char *key = ligne;
        char *value = equal + 1;

        // Remplir la structure
        if (strcmp(key, "nom") == 0) {
            strncpy(config->nom, value, sizeof(config->nom) - 1);
        } else if (strcmp(key, "port") == 0) {
            config->port = atoi(value);
        } else if (strcmp(key, "host") == 0) {
            strncpy(config->host, value, sizeof(config->host) - 1);
        }
    }

    fclose(f);
    return CONFIG_SUCCESS;
}

// Fonction d'affichage des erreurs
void afficher_erreur(ConfigError err) {
    switch (err) {
        case CONFIG_SUCCESS:
            printf("Configuration chargée avec succès\n");
            break;
        case CONFIG_ERR_FILE_NOT_FOUND:
            printf("Erreur : fichier de configuration introuvable\n");
            break;
        case CONFIG_ERR_INVALID_FORMAT:
            printf("Erreur : format de configuration invalide\n");
            break;
        case CONFIG_ERR_OUT_OF_MEMORY:
            printf("Erreur : mémoire insuffisante\n");
            break;
    }
}

int main(void) {
    Config config = {0};  // Initialisation à zéro

    ConfigError result = charger_configuration("config.txt", &config);

    if (result != CONFIG_SUCCESS) {
        afficher_erreur(result);
        return EXIT_FAILURE;
    }

    // Utiliser la configuration
    printf("Configuration:\n");
    printf("  Nom: %s\n", config.nom);
    printf("  Host: %s\n", config.host);
    printf("  Port: %d\n", config.port);

    return EXIT_SUCCESS;
}
```

---

## Bonnes pratiques

### 1. Toujours vérifier les valeurs de retour

```c
// ❌ Mauvais : ignore la valeur de retour
malloc(1024);
fopen("fichier.txt", "r");

// ✅ Bon : vérifie et gère l'erreur
int *tab = malloc(1024);
if (tab == NULL) {
    printf("Erreur d'allocation\n");
    return -1;
}

FILE *f = fopen("fichier.txt", "r");
if (f == NULL) {
    perror("fopen");
    return -1;
}
```

---

### 2. Documenter les codes d'erreur

```c
/**
 * Ouvre une connexion à la base de données
 *
 * @param chemin Chemin vers le fichier de base de données
 * @return 0 en cas de succès
 *         -1 si le fichier est introuvable
 *         -2 si les permissions sont insuffisantes
 *         -3 si le format est invalide
 */
int ouvrir_db(const char *chemin);
```

---

### 3. Utiliser des constantes ou enum pour les codes

```c
// ❌ Mauvais : valeurs magiques
if (resultat == -1) { /* ... */ }
if (resultat == -2) { /* ... */ }

// ✅ Bon : constantes explicites
#define ERR_NOT_FOUND -1
#define ERR_PERMISSION -2

if (resultat == ERR_NOT_FOUND) { /* ... */ }
```

---

### 4. Préférer errno pour les fonctions système

```c
int copier_fichier(const char *src, const char *dest) {
    FILE *source = fopen(src, "rb");
    if (source == NULL) {
        perror("fopen source");  // Utilise errno
        return -1;
    }

    // ...
}
```

---

### 5. Nettoyer les ressources en cas d'erreur

```c
int fonction(void) {
    FILE *f = fopen("file.txt", "r");
    if (f == NULL) return -1;

    char *buffer = malloc(1024);
    if (buffer == NULL) {
        fclose(f);  // ✅ Nettoyer avant de retourner
        return -1;
    }

    // ... utiliser f et buffer ...

    free(buffer);
    fclose(f);
    return 0;
}
```

---

## Tableau récapitulatif

| Mécanisme | Usage | Exemple |
|-----------|-------|---------|
| **Valeur de retour 0/-1** | Succès/Échec simple | `if (fonction() != 0) { }` |
| **Valeur de retour NULL** | Pointeurs invalides | `if (ptr == NULL) { }` |
| **errno** | Fonctions système | `if (f == NULL) perror("fopen");` |
| **Codes enum** | Erreurs multiples | `switch (code) { case ERR_... }` |
| **Paramètres de sortie** | Valeurs multiples | `diviser(a, b, &q, &r)` |
| **assert** | Bugs de programmation | `assert(ptr != NULL)` |

---

## Récapitulatif

### Points clés à retenir

1. ✅ Toute fonction non-void **doit retourner une valeur** avec `return`
2. ✅ Convention C : **0 = succès**, valeur non-nulle = erreur
3. ✅ **Toujours vérifier** les valeurs de retour des fonctions
4. ✅ `errno` est utilisé par les fonctions système pour détailler les erreurs
5. ✅ Les **paramètres de sortie** (pointeurs) permettent de retourner plusieurs valeurs
6. ✅ Les **assertions** détectent les bugs en mode debug
7. ✅ `NULL` et `-1` sont les valeurs d'erreur les plus courantes
8. ✅ **Nettoyer les ressources** avant de retourner en cas d'erreur

---

## Erreurs courantes à éviter

### 1. Ignorer les valeurs de retour

```c
// ❌ Dangereux
malloc(1024);  // Et si l'allocation échoue ?

// ✅ Correct
void *ptr = malloc(1024);
if (ptr == NULL) {
    fprintf(stderr, "Erreur d'allocation\n");
    exit(EXIT_FAILURE);
}
```

---

### 2. Ne pas initialiser les paramètres de sortie

```c
int diviser(int a, int b, int *quotient) {
    if (b == 0) {
        // ❌ quotient n'est pas initialisé !
        return -1;
    }
    *quotient = a / b;
    return 0;
}

// Correction
int diviser(int a, int b, int *quotient) {
    if (b == 0) {
        *quotient = 0;  // ✅ Valeur par défaut
        return -1;
    }
    *quotient = a / b;
    return 0;
}
```

---

### 3. Confondre assert et gestion d'erreur

```c
// ❌ Mauvais : assert pour une erreur d'exécution
FILE *f = fopen("config.txt", "r");
assert(f != NULL);  // Crash en production si le fichier manque !

// ✅ Bon : gestion d'erreur
FILE *f = fopen("config.txt", "r");
if (f == NULL) {
    perror("fopen");
    return -1;
}
```

---

## Pour aller plus loin

La gestion des erreurs est un pilier de la robustesse en C. Dans les prochaines sections :

- **Section 6.5** : La fonction main et ses paramètres (argc, argv)
- **Module 7 (Section 25)** : Gestion avancée des erreurs (patterns, logging, design by contract)
- **Module 6** : Programmation système (codes d'erreur POSIX, signaux)

Vous êtes maintenant équipé pour écrire des fonctions robustes et gérer les erreurs comme un professionnel ! 🚀

⏭️ [La fonction main et ses paramètres](/06-fonctions/05-fonction-main.md)
