🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 26.3 Prévention des buffer overflows

## Introduction

Les **buffer overflows** (débordements de tampon) sont l'une des vulnérabilités les plus anciennes et les plus exploitées en programmation C. Malgré des décennies de sensibilisation, ils restent une menace majeure.

### Qu'est-ce qu'un buffer overflow ?

Un buffer overflow se produit lorsqu'un programme écrit des données **au-delà des limites** d'un espace mémoire alloué (un buffer ou tampon).

**Analogie simple** : Imaginez un verre (le buffer) qui ne peut contenir que 250ml d'eau. Si vous versez 500ml, l'eau déborde et se répand partout sur la table. En programmation, ce "débordement" écrase d'autres données en mémoire, ce qui peut avoir des conséquences catastrophiques.

---

## Pourquoi les buffer overflows sont-ils dangereux ?

### 1. **Corruption de données**

```
Mémoire avant overflow :
+----------------+----------------+-----------------+
|  buffer[10]    | variable_importante | autre_data |
+----------------+----------------+-----------------+

Mémoire après overflow :
+----------------+----------------+----------------+
| XXXXXXXXXXXXXXXXXXXXXXXXXXXX... | corrupted      |
+----------------+----------------+----------------+
```

Le débordement écrase les données adjacentes en mémoire.

### 2. **Crash du programme**

Quand des données critiques sont écrasées (pointeurs, adresses de retour, etc.), le programme plante.

### 3. **Exploitation par des attaquants**

Un attaquant peut utiliser un buffer overflow pour :
- **Exécuter du code arbitraire** (prendre le contrôle du système)
- **Contourner l'authentification**
- **Élever ses privilèges**
- **Voler des données sensibles**

### Statistiques

- ~20% des vulnérabilités CVE sont des buffer overflows
- Responsables de nombreuses attaques célèbres (Morris Worm 1988, Code Red 2001, etc.)
- Coût estimé : milliards de dollars en dégâts

---

## Comment se produisent les buffer overflows ?

### Exemple 1 : Débordement de tableau simple

```c
#include <stdio.h>

int main(void) {
    char buffer[8];  // Espace pour 8 caractères (incluant '\0')

    printf("Entrez votre nom : ");
    scanf("%s", buffer);  // ❌ DANGEREUX !

    printf("Bonjour %s\n", buffer);
    return 0;
}
```

**Que se passe-t-il ?**

```
Si l'utilisateur entre "Alice" (5 caractères + '\0') → OK
Buffer : ['A']['l']['i']['c']['e']['\0'][?][?]

Si l'utilisateur entre "AlexandreDumas" (14 caractères + '\0') → OVERFLOW !
Buffer : ['A']['l']['e']['x']['a']['n']['d']['r']
         ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
         déborde de 7 caractères !
```

Les 7 caractères supplémentaires écrasent la mémoire adjacente.

### Visualisation mémoire détaillée

```
Adresse    Contenu avant      Contenu après overflow
0x1000    [?] (buffer[0])     ['A']
0x1001    [?] (buffer[1])     ['l']
0x1002    [?] (buffer[2])     ['e']
0x1003    [?] (buffer[3])     ['x']
0x1004    [?] (buffer[4])     ['a']
0x1005    [?] (buffer[5])     ['n']
0x1006    [?] (buffer[6])     ['d']
0x1007    [?] (buffer[7])     ['r']
0x1008    [variable]          ['e']  ← Corruption !
0x1009    [variable]          ['D']  ← Corruption !
0x100A    [variable]          ['u']  ← Corruption !
...
```

### Exemple 2 : Débordement avec strcpy

```c
void copier_nom(const char *source) {
    char destination[10];
    strcpy(destination, source);  // ❌ Pas de vérification de taille !
    printf("Nom copié : %s\n", destination);
}

// Si source = "UnNomTresLongQuiDeborde", overflow !
```

**Problème** : `strcpy` ne vérifie pas si `destination` a assez d'espace.

---

## Les fonctions dangereuses à éviter

### Tableau des fonctions à risque

| Fonction dangereuse | Risque | Alternative sécurisée |
|---------------------|--------|----------------------|
| `gets()` | Buffer overflow | `fgets()` |
| `scanf("%s")` | Buffer overflow | `scanf("%Ns")` ou `fgets()` |
| `strcpy()` | Buffer overflow | `strncpy()` ou `strcpy_s()` |
| `strcat()` | Buffer overflow | `strncat()` ou `strcat_s()` |
| `sprintf()` | Buffer overflow | `snprintf()` |
| `vsprintf()` | Buffer overflow | `vsnprintf()` |

### 1. gets() - À NE JAMAIS UTILISER

```c
// ❌ EXTRÊMEMENT DANGEREUX
char buffer[50];
gets(buffer);  // Fonction dépréciée et dangereuse !
```

**Pourquoi ?** `gets()` n'a aucun moyen de connaître la taille du buffer et lit jusqu'à trouver un '\n', peu importe la taille.

```c
// ✅ Alternative sécurisée
char buffer[50];
if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
    // Retirer le '\n' final si présent
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
}
```

### 2. scanf("%s") - Dangereux sans limite

```c
// ❌ Dangereux
char nom[20];
scanf("%s", nom);  // Pas de limite !
```

```c
// ✅ Mieux : spécifier une largeur maximale
char nom[20];
scanf("%19s", nom);  // Lit au maximum 19 caractères (+ '\0')

// Vider le buffer si nécessaire
int c;
while ((c = getchar()) != '\n' && c != EOF);
```

```c
// ✅ Encore mieux : utiliser fgets
char nom[20];
if (fgets(nom, sizeof(nom), stdin) != NULL) {
    size_t len = strlen(nom);
    if (len > 0 && nom[len - 1] == '\n') {
        nom[len - 1] = '\0';
    }
}
```

### 3. strcpy() - Copie sans vérification

```c
// ❌ Dangereux
void copier(const char *source) {
    char dest[10];
    strcpy(dest, source);  // Overflow si source > 9 caractères
}
```

```c
// ✅ Solution 1 : strncpy (avec précautions)
void copier(const char *source) {
    char dest[10];
    strncpy(dest, source, sizeof(dest) - 1);
    dest[sizeof(dest) - 1] = '\0';  // IMPORTANT : garantir '\0'
}
```

**⚠️ Piège de strncpy** : Elle ne garantit PAS la terminaison null si la source est trop longue !

```c
char dest[5];
strncpy(dest, "Bonjour", 5);  // dest = ['B']['o']['n']['j']['o'] (PAS de '\0')
printf("%s", dest);  // ❌ Comportement indéfini !
```

```c
// ✅ Solution 2 : strncpy sécurisée
void copier_secure(const char *source, char *dest, size_t dest_size) {
    if (dest_size == 0) return;

    strncpy(dest, source, dest_size - 1);
    dest[dest_size - 1] = '\0';  // Toujours terminer par '\0'
}
```

```c
// ✅ Solution 3 : strlcpy (BSD, pas standard POSIX)
#ifdef __BSD__
void copier_bsd(const char *source) {
    char dest[10];
    size_t result = strlcpy(dest, source, sizeof(dest));

    if (result >= sizeof(dest)) {
        fprintf(stderr, "Attention : chaîne tronquée\n");
    }
}
#endif
```

```c
// ✅ Solution 4 : strcpy_s (C11 Annex K)
#ifdef __STDC_LIB_EXT1__
void copier_c11(const char *source) {
    char dest[10];
    errno_t err = strcpy_s(dest, sizeof(dest), source);

    if (err != 0) {
        fprintf(stderr, "Erreur de copie\n");
    }
}
#endif
```

### 4. strcat() - Concaténation dangereuse

```c
// ❌ Dangereux
char buffer[20] = "Hello ";
strcat(buffer, "World!");  // OK
strcat(buffer, " This is a very long string");  // ❌ Overflow !
```

```c
// ✅ Solution : strncat
char buffer[20] = "Hello ";
size_t espace_restant = sizeof(buffer) - strlen(buffer) - 1;
strncat(buffer, "World", espace_restant);
```

**⚠️ Attention** : `strncat` prend en troisième paramètre le nombre de caractères à ajouter, pas la taille totale !

```c
// Exemple complet sécurisé
void concatener_secure(char *dest, size_t dest_size, const char *source) {
    size_t dest_len = strlen(dest);

    if (dest_len >= dest_size - 1) {
        // Destination déjà pleine
        return;
    }

    size_t espace_restant = dest_size - dest_len - 1;
    strncat(dest, source, espace_restant);
}
```

### 5. sprintf() - Formatage dangereux

```c
// ❌ Dangereux
char buffer[20];
int age = 25;
sprintf(buffer, "L'utilisateur a %d ans et habite à %s", age, ville);
// Si la chaîne résultante > 19 caractères → overflow !
```

```c
// ✅ Solution : snprintf
char buffer[20];
int age = 25;
int resultat = snprintf(buffer, sizeof(buffer),
                        "L'utilisateur a %d ans et habite à %s",
                        age, ville);

if (resultat >= sizeof(buffer)) {
    fprintf(stderr, "Attention : sortie tronquée\n");
}
```

**Avantage de snprintf** : Retourne le nombre de caractères qui auraient été écrits (sans compter '\0'), permettant de détecter une troncature.

---

## Techniques de prévention

### 1. Toujours connaître la taille des buffers

```c
// ✅ Bon pattern
void traiter_chaine(char *buffer, size_t buffer_size) {
    // La fonction reçoit la taille en paramètre
    snprintf(buffer, buffer_size, "Traitement...");
}

// Utilisation
char mon_buffer[100];
traiter_chaine(mon_buffer, sizeof(mon_buffer));
```

### 2. Utiliser sizeof() systématiquement

```c
char buffer[50];

// ❌ Mauvais : nombre magique
fgets(buffer, 50, stdin);

// ✅ Bon : sizeof() s'adapte automatiquement
fgets(buffer, sizeof(buffer), stdin);

// Si on change la taille du buffer :
// char buffer[100];  ← Le code reste correct !
```

### 3. Vérifier les codes de retour

```c
// ✅ Vérification systématique
char buffer[100];
if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
    fprintf(stderr, "Erreur de lecture\n");
    return -1;
}

int result = snprintf(buffer, sizeof(buffer), "Format %s", data);
if (result < 0) {
    fprintf(stderr, "Erreur de formatage\n");
    return -1;
}
if (result >= sizeof(buffer)) {
    fprintf(stderr, "Attention : troncature\n");
}
```

### 4. Validation de longueur avant copie

```c
// ✅ Pattern de validation
bool copier_avec_validation(char *dest, size_t dest_size, const char *source) {
    if (dest == NULL || source == NULL || dest_size == 0) {
        return false;
    }

    size_t source_len = strlen(source);

    // Vérification avant copie
    if (source_len >= dest_size) {
        fprintf(stderr, "Erreur : source trop longue (%zu caractères, max %zu)\n",
                source_len, dest_size - 1);
        return false;
    }

    strcpy(dest, source);  // Maintenant sûr
    return true;
}
```

### 5. Allocation dynamique adaptée

```c
// ✅ Allouer la taille exacte nécessaire
void traiter_donnees(const char *input) {
    size_t taille_necessaire = strlen(input) + 1;  // +1 pour '\0'

    char *buffer = malloc(taille_necessaire);
    if (buffer == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        return;
    }

    strcpy(buffer, input);  // Sûr : taille exacte

    // Traitement...

    free(buffer);
}
```

---

## Cas particuliers et pièges

### Piège 1 : Tableaux multidimensionnels

```c
// ❌ Dangereux
void remplir_tableau(char tableau[][10], int nb_lignes) {
    for (int i = 0; i < nb_lignes; i++) {
        scanf("%s", tableau[i]);  // Risque d'overflow sur chaque ligne
    }
}
```

```c
// ✅ Sécurisé
void remplir_tableau(char tableau[][10], int nb_lignes) {
    for (int i = 0; i < nb_lignes; i++) {
        scanf("%9s", tableau[i]);  // Limite à 9 caractères

        // Vider le buffer
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
    }
}
```

### Piège 2 : Buffers dans les structures

```c
typedef struct {
    char nom[50];
    char prenom[50];
    int age;
} Personne;

// ❌ Dangereux
void initialiser_personne(Personne *p, const char *nom, const char *prenom) {
    strcpy(p->nom, nom);      // Risque d'overflow
    strcpy(p->prenom, prenom);  // Risque d'overflow
}
```

```c
// ✅ Sécurisé
bool initialiser_personne(Personne *p, const char *nom, const char *prenom) {
    if (p == NULL || nom == NULL || prenom == NULL) {
        return false;
    }

    if (strlen(nom) >= sizeof(p->nom)) {
        fprintf(stderr, "Erreur : nom trop long\n");
        return false;
    }

    if (strlen(prenom) >= sizeof(p->prenom)) {
        fprintf(stderr, "Erreur : prénom trop long\n");
        return false;
    }

    strcpy(p->nom, nom);
    strcpy(p->prenom, prenom);
    return true;
}
```

### Piège 3 : Off-by-one errors

```c
// ❌ Erreur classique
char buffer[10];
for (int i = 0; i <= 10; i++) {  // ← BUG : devrait être i < 10
    buffer[i] = 'A';
}
// buffer[10] est hors limites !
```

```c
// ✅ Correct
char buffer[10];
for (int i = 0; i < 10; i++) {  // ou i < sizeof(buffer)
    buffer[i] = 'A';
}
```

### Piège 4 : Oubli du caractère null

```c
// ❌ Dangereux
char buffer[10];
strncpy(buffer, "UnTexeTresLong", 10);  // Pas de '\0' !
printf("%s", buffer);  // Comportement indéfini
```

```c
// ✅ Correct
char buffer[10];
strncpy(buffer, "UnTexeTresLong", sizeof(buffer) - 1);
buffer[sizeof(buffer) - 1] = '\0';  // Garantir '\0'
printf("%s", buffer);
```

---

## Protection au niveau du compilateur

### 1. Stack canaries

Les compilateurs modernes insèrent des "canaries" (valeurs sentinelles) sur la pile pour détecter les overflows.

```bash
# Compilation avec protection de la pile
gcc -fstack-protector-all mon_programme.c -o mon_programme
```

**Comment ça marche ?**

```
Stack layout avec canary :
+-------------------+
| Adresse de retour |
+-------------------+
| CANARY (0xDEADBEEF) | ← Valeur sentinelle
+-------------------+
| buffer[10]        |
+-------------------+
| Variables locales |
+-------------------+
```

Si un overflow écrase le canary, le programme détecte l'attaque et s'arrête :

```
*** stack smashing detected ***: ./mon_programme terminated
Aborted (core dumped)
```

### 2. FORTIFY_SOURCE

Active des vérifications supplémentaires pour les fonctions dangereuses.

```bash
gcc -D_FORTIFY_SOURCE=2 -O2 mon_programme.c -o mon_programme
```

**Effet** : Remplace automatiquement les appels dangereux par des versions sécurisées :
- `strcpy()` → `__strcpy_chk()`
- `sprintf()` → `__sprintf_chk()`

Ces versions vérifiées détectent les overflows au runtime.

### 3. AddressSanitizer (ASan)

Détecte les buffer overflows et autres erreurs mémoire pendant l'exécution.

```bash
# Compilation avec ASan
gcc -fsanitize=address -g mon_programme.c -o mon_programme

# Exécution
./mon_programme
```

**Exemple de sortie ASan** :

```
=================================================================
==12345==ERROR: AddressSanitizer: stack-buffer-overflow on address 0x7fff12345678
WRITE of size 15 at 0x7fff12345678 thread T0
    #0 0x4567890 in main mon_programme.c:10
    #1 0x7f1234567890 in __libc_start_main
```

ASan indique exactement où et comment l'overflow s'est produit !

---

## Outils de détection

### 1. Valgrind

Détecte les erreurs mémoire, y compris certains overflows.

```bash
valgrind --leak-check=full --track-origins=yes ./mon_programme
```

### 2. Analyse statique

#### cppcheck

```bash
cppcheck --enable=all mon_programme.c
```

Exemple de détection :

```
[mon_programme.c:15]: (error) Buffer is accessed out of bounds: buffer[10]
```

#### clang-tidy

```bash
clang-tidy mon_programme.c -checks='clang-analyzer-*,cert-*' --
```

### 3. Fuzzing

Tester avec des entrées aléatoires pour trouver des crashs.

```bash
# AFL (American Fuzzy Lop)
afl-gcc mon_programme.c -o mon_programme
afl-fuzz -i entrees_test -o resultats ./mon_programme @@
```

Le fuzzer génère des entrées malformées pour provoquer des overflows.

---

## Fonctions sécurisées modernes

### C11 Annex K (Fonctions _s)

```c
#define __STDC_WANT_LIB_EXT1__ 1
#include <stdio.h>
#include <string.h>

#ifdef __STDC_LIB_EXT1__

// strcpy_s
errno_t strcpy_s(char *dest, rsize_t dest_size, const char *source);

// strcat_s
errno_t strcat_s(char *dest, rsize_t dest_size, const char *source);

// sprintf_s
int sprintf_s(char *buffer, rsize_t buffer_size, const char *format, ...);

// Exemple d'utilisation
void exemple_c11(void) {
    char buffer[20];

    errno_t err = strcpy_s(buffer, sizeof(buffer), "Hello");
    if (err != 0) {
        fprintf(stderr, "Erreur de copie\n");
        return;
    }

    err = strcat_s(buffer, sizeof(buffer), " World");
    if (err != 0) {
        fprintf(stderr, "Erreur de concaténation\n");
        return;
    }

    printf("%s\n", buffer);
}

#endif
```

**Problème** : Annex K n'est pas largement supporté (Microsoft l'implémente, GCC/Clang pas par défaut).

### Alternatives portables

Pour un code portable, créez vos propres wrappers :

```c
// safe_string.h
#ifndef SAFE_STRING_H
#define SAFE_STRING_H

#include <stddef.h>
#include <stdbool.h>

// Copie sécurisée
bool safe_strcpy(char *dest, size_t dest_size, const char *source);

// Concaténation sécurisée
bool safe_strcat(char *dest, size_t dest_size, const char *source);

// Formatage sécurisé
int safe_sprintf(char *buffer, size_t buffer_size, const char *format, ...);

#endif
```

```c
// safe_string.c
#include "safe_string.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

bool safe_strcpy(char *dest, size_t dest_size, const char *source) {
    if (dest == NULL || source == NULL || dest_size == 0) {
        return false;
    }

    size_t source_len = strlen(source);
    if (source_len >= dest_size) {
        return false;  // Source trop longue
    }

    strcpy(dest, source);
    return true;
}

bool safe_strcat(char *dest, size_t dest_size, const char *source) {
    if (dest == NULL || source == NULL || dest_size == 0) {
        return false;
    }

    size_t dest_len = strlen(dest);
    size_t source_len = strlen(source);

    if (dest_len + source_len >= dest_size) {
        return false;  // Pas assez d'espace
    }

    strcat(dest, source);
    return true;
}

int safe_sprintf(char *buffer, size_t buffer_size, const char *format, ...) {
    if (buffer == NULL || format == NULL || buffer_size == 0) {
        return -1;
    }

    va_list args;
    va_start(args, format);

    int result = vsnprintf(buffer, buffer_size, format, args);

    va_end(args);

    if (result < 0 || (size_t)result >= buffer_size) {
        return -1;  // Erreur ou troncature
    }

    return result;
}
```

---

## Exemples complets : Avant/Après

### Exemple 1 : Programme de saisie utilisateur

```c
// ❌ VERSION DANGEREUSE
#include <stdio.h>
#include <string.h>

int main(void) {
    char nom[20];
    char prenom[20];
    char adresse[50];

    printf("Nom : ");
    scanf("%s", nom);  // Dangereux

    printf("Prénom : ");
    scanf("%s", prenom);  // Dangereux

    printf("Adresse : ");
    scanf("%s", adresse);  // Dangereux

    char complet[100];
    sprintf(complet, "%s %s habite à %s", prenom, nom, adresse);  // Dangereux

    printf("%s\n", complet);
    return 0;
}
```

```c
// ✅ VERSION SÉCURISÉE
#include <stdio.h>
#include <string.h>

// Fonction helper pour lire une ligne
bool lire_ligne(const char *prompt, char *buffer, size_t buffer_size) {
    printf("%s", prompt);

    if (fgets(buffer, buffer_size, stdin) == NULL) {
        return false;
    }

    // Retirer le '\n' final
    size_t len = strlen(buffer);
    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }

    return true;
}

int main(void) {
    char nom[20];
    char prenom[20];
    char adresse[50];

    if (!lire_ligne("Nom : ", nom, sizeof(nom))) {
        fprintf(stderr, "Erreur de lecture\n");
        return 1;
    }

    if (!lire_ligne("Prénom : ", prenom, sizeof(prenom))) {
        fprintf(stderr, "Erreur de lecture\n");
        return 1;
    }

    if (!lire_ligne("Adresse : ", adresse, sizeof(adresse))) {
        fprintf(stderr, "Erreur de lecture\n");
        return 1;
    }

    char complet[100];
    int result = snprintf(complet, sizeof(complet),
                          "%s %s habite à %s",
                          prenom, nom, adresse);

    if (result < 0 || result >= sizeof(complet)) {
        fprintf(stderr, "Erreur : informations trop longues\n");
        return 1;
    }

    printf("%s\n", complet);
    return 0;
}
```

### Exemple 2 : Traitement de fichier de configuration

```c
// ❌ VERSION DANGEREUSE
#include <stdio.h>
#include <string.h>

typedef struct {
    char nom[30];
    char valeur[50];
} ConfigParam;

void charger_config(const char *fichier, ConfigParam params[], int *nb_params) {
    FILE *f = fopen(fichier, "r");
    if (f == NULL) return;

    char ligne[100];
    *nb_params = 0;

    while (fgets(ligne, sizeof(ligne), f) != NULL) {
        char nom[30], valeur[50];
        sscanf(ligne, "%s = %s", nom, valeur);  // Dangereux

        strcpy(params[*nb_params].nom, nom);      // Dangereux
        strcpy(params[*nb_params].valeur, valeur);  // Dangereux
        (*nb_params)++;
    }

    fclose(f);
}
```

```c
// ✅ VERSION SÉCURISÉE
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    char nom[30];
    char valeur[50];
} ConfigParam;

bool charger_config(const char *fichier, ConfigParam params[],
                    size_t max_params, size_t *nb_params) {
    if (fichier == NULL || params == NULL || nb_params == NULL) {
        return false;
    }

    FILE *f = fopen(fichier, "r");
    if (f == NULL) {
        perror("Erreur d'ouverture du fichier");
        return false;
    }

    char ligne[256];
    *nb_params = 0;
    size_t numero_ligne = 0;

    while (fgets(ligne, sizeof(ligne), f) != NULL && *nb_params < max_params) {
        numero_ligne++;

        // Ignorer les lignes vides et commentaires
        if (ligne[0] == '\n' || ligne[0] == '#') {
            continue;
        }

        char nom[30], valeur[50];
        int items = sscanf(ligne, "%29s = %49s", nom, valeur);

        if (items != 2) {
            fprintf(stderr, "Attention : ligne %zu mal formée\n", numero_ligne);
            continue;
        }

        // Copie sécurisée
        strncpy(params[*nb_params].nom, nom, sizeof(params[*nb_params].nom) - 1);
        params[*nb_params].nom[sizeof(params[*nb_params].nom) - 1] = '\0';

        strncpy(params[*nb_params].valeur, valeur, sizeof(params[*nb_params].valeur) - 1);
        params[*nb_params].valeur[sizeof(params[*nb_params].valeur) - 1] = '\0';

        (*nb_params)++;
    }

    fclose(f);
    return true;
}

// Utilisation
int main(void) {
    ConfigParam params[100];
    size_t nb_params;

    if (charger_config("config.txt", params, 100, &nb_params)) {
        printf("Configuration chargée : %zu paramètres\n", nb_params);

        for (size_t i = 0; i < nb_params; i++) {
            printf("%s = %s\n", params[i].nom, params[i].valeur);
        }
    } else {
        fprintf(stderr, "Échec du chargement\n");
        return 1;
    }

    return 0;
}
```

---

## Checklist de prévention

Avant de valider du code, vérifiez :

### ✅ Entrées utilisateur
- [ ] Aucun usage de `gets()`
- [ ] `scanf("%s")` a une limite de largeur ou remplacé par `fgets()`
- [ ] Toutes les entrées sont validées (longueur, format)

### ✅ Manipulation de chaînes
- [ ] Aucun `strcpy()` sans validation préalable
- [ ] `strncpy()` utilisé avec terminaison null garantie
- [ ] `sprintf()` remplacé par `snprintf()`
- [ ] `strcat()` remplacé par `strncat()` avec calcul correct

### ✅ Tableaux et buffers
- [ ] Utilisation de `sizeof()` au lieu de nombres magiques
- [ ] Vérification des indices avant accès
- [ ] Boucles correctes (pas de off-by-one)

### ✅ Compilation
- [ ] Flags de sécurité activés (`-fstack-protector-all`, `-D_FORTIFY_SOURCE=2`)
- [ ] Tests avec AddressSanitizer
- [ ] Analyse statique effectuée (cppcheck, clang-tidy)

### ✅ Tests
- [ ] Tests avec des entrées longues
- [ ] Tests avec des caractères spéciaux
- [ ] Fuzzing effectué si possible

---

## Résumé : Les règles d'or

### 🎯 Principes fondamentaux

1. **Toujours connaître la taille** des buffers
2. **Utiliser sizeof()** plutôt que des nombres en dur
3. **Préférer les fonctions sûres** (snprintf, strncpy + '\0', fgets)
4. **Valider avant de copier** (longueur, format, contenu)
5. **Compiler avec protections** (stack canaries, FORTIFY_SOURCE, ASan)
6. **Tester avec rigueur** (fuzzing, entrées malveillantes)

### 📚 Hiérarchie de sécurité

```
Niveau 1 (Débutant) :
└─ Remplacer gets() par fgets()
└─ Ajouter des limites à scanf()
└─ Utiliser snprintf() au lieu de sprintf()

Niveau 2 (Intermédiaire) :
└─ Valider toutes les longueurs avant copie
└─ Créer des wrappers sécurisés
└─ Activer les flags de compilation

Niveau 3 (Avancé) :
└─ Analyse statique systématique
└─ Tests avec sanitizers
└─ Fuzzing régulier
```

### 💡 Message clé

> **Un buffer overflow n'est jamais "juste un bug" - c'est une faille de sécurité potentielle**

Les buffer overflows sont :
- ✅ **Évitables** avec les bonnes pratiques
- ✅ **Détectables** avec les bons outils
- ❌ **Inacceptables** en code de production

---

**🔒 En résumé** : La prévention des buffer overflows repose sur trois piliers :
1. **Code défensif** (validation, fonctions sûres)
2. **Outils de détection** (compilateur, sanitizers, analyse statique)
3. **Tests rigoureux** (entrées malveillantes, fuzzing)

Suivez ces principes et vous éliminerez la grande majorité des risques de buffer overflow dans votre code C !

⏭️ [Format string vulnerabilities](/26-securite-code-defensif/04-format-string-vulnerabilities.md)
