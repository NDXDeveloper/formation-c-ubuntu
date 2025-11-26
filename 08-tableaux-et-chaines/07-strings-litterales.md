🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 8.7 Strings littérales et immutabilité

## Introduction

Les chaînes littérales (string literals) sont une notion fondamentale en C, mais souvent mal comprise. Leur comportement particulier en mémoire et leur immutabilité sont sources de confusion et de bugs. Cette section vous explique tout ce que vous devez savoir pour les utiliser correctement.

**Ce que vous apprendrez :**
- Ce qu'est exactement une chaîne littérale
- Où et comment elles sont stockées en mémoire
- Pourquoi elles sont immuables
- La différence entre `char[]` et `char*` pour les chaînes
- Les bonnes pratiques d'utilisation avec `const`
- Les pièges à éviter

## Qu'est-ce qu'une chaîne littérale ?

Une **chaîne littérale** (string literal) est une séquence de caractères écrite directement dans le code source, entre guillemets doubles.

```c
"Ceci est une chaîne littérale"
```

**Exemples de chaînes littérales :**

```c
"Bonjour"
"Hello, World!"
"123"
""           // Chaîne vide (contient uniquement '\0')
"C'est \"super\" !"  // Avec échappement
```

**Ce qui n'est PAS une chaîne littérale :**

```c
'A'          // Caractère littéral, pas une chaîne
str          // Variable, pas une littérale
```

### Type d'une chaîne littérale

En C, une chaîne littérale est de type **`char[N]`** où `N` est la longueur de la chaîne + 1 (pour le `'\0'`).

```c
"Bonjour"    // Type : char[8]  (7 caractères + '\0')
"Hi"         // Type : char[3]  (2 caractères + '\0')
```

Mais dans la plupart des contextes, elle **décroît** (array decay) en **`const char*`** (pointeur vers caractère constant).

## Stockage en mémoire : Le segment read-only

### Organisation de la mémoire d'un programme

Un programme C est divisé en plusieurs segments mémoire :

```
┌─────────────────────────┐  Adresses hautes
│                         │
│    Stack (Pile)         │  Variables locales, paramètres
│    ↓                    │
│                         │
│    ...                  │
│                         │
│    ↑                    │
│    Heap (Tas)           │  Allocation dynamique (malloc)
│                         │
├─────────────────────────┤
│    BSS                  │  Variables globales non initialisées
├─────────────────────────┤
│    Data                 │  Variables globales initialisées
├─────────────────────────┤
│    Rodata (Read-Only)   │  ⭐ CHAÎNES LITTÉRALES ICI ⭐
│                         │  Constantes, données immuables
├─────────────────────────┤
│    Text (Code)          │  Instructions du programme
└─────────────────────────┘  Adresses basses
```

**Les chaînes littérales sont stockées dans le segment `.rodata`** (read-only data) ou parfois dans `.text`. Ce segment est **en lecture seule** et toute tentative de modification provoque une erreur.

### Exemple visuel

```c
char *ptr = "Bonjour";
```

**En mémoire :**

```
Segment .rodata (lecture seule) :
Adresse 0x08040500 → [ 'B' 'o' 'n' 'j' 'o' 'u' 'r' '\0' ]
                      ↑
                      │
Pile (stack) :        │
ptr : 0x08040500 ─────┘  (stocke l'adresse de la littérale)
```

Le pointeur `ptr` est sur la pile et contient l'**adresse** de la chaîne littérale qui, elle, est en zone read-only.

## Immutabilité : Les chaînes littérales ne peuvent pas être modifiées

### ❌ Tentative de modification : Comportement indéfini

```c
char *str = "Bonjour";
str[0] = 'b';  // ❌ COMPORTEMENT INDÉFINI !
```

**Ce qui se passe :**
- Sur certains systèmes : **Segmentation fault** (crash immédiat)
- Sur d'autres : Peut sembler fonctionner mais comportement imprévisible
- En production : **Risque de sécurité et instabilité**

**Pourquoi ?** Vous tentez d'écrire dans une zone mémoire marquée comme lecture seule.

### Démonstration du danger

```c
#include <stdio.h>

int main(void) {
    char *message = "Hello";

    printf("Avant : %s\n", message);

    message[0] = 'h';  // ❌ Crash probable !

    printf("Après : %s\n", message);  // N'est jamais atteint

    return 0;
}
```

**Résultat typique :**
```
Avant : Hello
Segmentation fault (core dumped)
```

### Protection du système d'exploitation

Les systèmes modernes marquent le segment `.rodata` comme **non modifiable**. Le processeur déclenche une exception si on tente d'y écrire.

```
Processus :
1. Programme tente str[0] = 'b'
2. CPU détecte écriture dans zone read-only
3. Génère une exception (SIGSEGV sur Unix)
4. Système termine le processus
```

## Différence : char[] vs char*

### char[] : Tableau modifiable

```c
char tab[] = "Bonjour";
```

**Ce qui se passe :**
1. Le compilateur crée un **tableau** sur la pile (ou segment data si global)
2. **Copie** le contenu de la littérale dans ce tableau
3. Le tableau est **modifiable**

**En mémoire :**

```
Segment .rodata :
"Bonjour" → [ 'B' 'o' 'n' 'j' 'o' 'u' 'r' '\0' ]

Pile (stack) :
tab[] → [ 'B' 'o' 'n' 'j' 'o' 'u' 'r' '\0' ]  (COPIE modifiable)
```

**Utilisation :**

```c
char tab[] = "Bonjour";
tab[0] = 'b';  // ✅ OK : modification du tableau local
printf("%s\n", tab);  // "bonjour"
```

### char* : Pointeur vers littérale (immuable)

```c
char *ptr = "Bonjour";
```

**Ce qui se passe :**
1. `ptr` est un **pointeur** qui pointe vers la littérale
2. **Aucune copie** n'est faite
3. La littérale reste en zone read-only

**En mémoire :**

```
Segment .rodata :
"Bonjour" → [ 'B' 'o' 'n' 'j' 'o' 'u' 'r' '\0' ]
             ↑
Pile :       │
ptr ─────────┘  (pointe vers .rodata)
```

**Utilisation :**

```c
char *ptr = "Bonjour";
ptr[0] = 'b';  // ❌ ERREUR : modification d'une zone read-only
```

### Tableau comparatif

| Aspect | `char tab[] = "..."` | `char *ptr = "..."` |
|--------|---------------------|-------------------|
| **Type** | Tableau de caractères | Pointeur vers caractère |
| **Stockage** | Copie sur pile/data | Pointe vers .rodata |
| **Modifiable** | ✅ Oui | ❌ Non (comportement indéfini) |
| **Taille mémoire** | Taille de la chaîne | Taille d'un pointeur (8 octets) |
| **Réaffectation** | ❌ Impossible (`tab = ...`) | ✅ Possible (`ptr = ...`) |
| **Performance** | Copie (lent) | Pas de copie (rapide) |

### Exemple comparatif

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    // Tableau : copie modifiable
    char tab[] = "Hello";
    printf("sizeof(tab) = %zu\n", sizeof(tab));  // 6 (5 + '\0')
    tab[0] = 'h';  // ✅ OK
    printf("tab = %s\n", tab);  // "hello"

    // Pointeur : référence vers littérale
    char *ptr = "World";
    printf("sizeof(ptr) = %zu\n", sizeof(ptr));  // 8 (taille du pointeur)
    // ptr[0] = 'w';  // ❌ DANGER : ne pas faire
    ptr = "Autre";  // ✅ OK : réaffectation du pointeur
    printf("ptr = %s\n", ptr);  // "Autre"

    return 0;
}
```

## Le mot-clé const : Signaler l'intention

Pour éviter les modifications accidentelles et améliorer la sécurité du code, utilisez **`const`**.

### ✅ Bonne pratique : const char*

```c
const char *ptr = "Bonjour";
```

**Avantages :**
1. **Documentation** : signale que la chaîne ne doit pas être modifiée
2. **Protection** : le compilateur empêche les modifications accidentelles
3. **Lisibilité** : intention claire

**Tentative de modification :**

```c
const char *ptr = "Bonjour";
ptr[0] = 'b';  // ❌ ERREUR DE COMPILATION
// error: assignment of read-only location
```

### Warning du compilateur

Sans `const`, le compilateur moderne génère un warning :

```c
char *ptr = "Bonjour";  // ⚠️ Warning
// warning: conversion from string literal to 'char *' is deprecated
```

**En C moderne (depuis C11), la bonne pratique est :**

```c
const char *ptr = "Bonjour";  // ✅ Correct
```

### const dans les paramètres de fonction

```c
// ✅ Bonne pratique : const pour signaler qu'on ne modifie pas
void afficher(const char *message) {
    printf("%s\n", message);
    // message[0] = 'X';  // ❌ Erreur de compilation
}

// ❌ Mauvaise pratique : pas de const
void afficher_mauvais(char *message) {
    printf("%s\n", message);
    // Risque de modification accidentelle
}
```

### Niveaux de const

```c
// Pointeur vers const char (la donnée est constante)
const char *ptr1 = "Hello";
ptr1 = "World";  // ✅ OK : réaffectation du pointeur
// ptr1[0] = 'h';  // ❌ Erreur : modification de la donnée

// Pointeur constant vers char (le pointeur est constant)
char * const ptr2 = buffer;
ptr2[0] = 'H';  // ✅ OK : modification de la donnée
// ptr2 = autre;  // ❌ Erreur : réaffectation du pointeur

// Pointeur constant vers const char (tout est constant)
const char * const ptr3 = "Hello";
// ptr3 = "World";  // ❌ Erreur : réaffectation
// ptr3[0] = 'h';   // ❌ Erreur : modification
```

**Mnémonique :** Lire de droite à gauche
- `const char *` : pointeur vers char constant
- `char * const` : pointeur constant vers char
- `const char * const` : pointeur constant vers char constant

## Durée de vie des chaînes littérales

### Durée de vie : Toute l'exécution du programme

Les chaînes littérales ont une **durée de vie statique** : elles existent pendant toute la durée d'exécution du programme.

```c
const char *obtenir_message(void) {
    return "Message constant";  // ✅ OK : littérale existe toujours
}

int main(void) {
    const char *msg = obtenir_message();
    printf("%s\n", msg);  // ✅ Fonctionne parfaitement
    return 0;
}
```

**Contraste avec variable locale :**

```c
char *obtenir_message_dangereux(void) {
    char buffer[] = "Message";  // Variable locale
    return buffer;  // ❌ DANGEREUX : buffer est détruit !
}

int main(void) {
    char *msg = obtenir_message_dangereux();
    printf("%s\n", msg);  // ⚠️ Comportement indéfini
    return 0;
}
```

### Pourquoi c'est important ?

```c
// ✅ Correct : littérale
const char *get_error_message(int code) {
    switch (code) {
        case 1: return "Erreur mineure";
        case 2: return "Erreur grave";
        default: return "Erreur inconnue";
    }
}

// ❌ Incorrect : variable locale
char *get_error_message_bad(int code) {
    char buffer[100];

    switch (code) {
        case 1: strcpy(buffer, "Erreur mineure"); break;
        case 2: strcpy(buffer, "Erreur grave"); break;
        default: strcpy(buffer, "Erreur inconnue"); break;
    }

    return buffer;  // ❌ buffer n'existe plus après le retour !
}
```

## String pooling (Internement de chaînes)

### Optimisation du compilateur

Le compilateur peut **fusionner** les chaînes littérales identiques pour économiser de la mémoire.

```c
const char *str1 = "Hello";
const char *str2 = "Hello";

if (str1 == str2) {
    printf("Même adresse !\n");  // Peut s'afficher
}
```

**Explication :**
- Les deux pointeurs peuvent pointer vers la **même chaîne** en mémoire
- Le compilateur détecte les doublons et les partage
- C'est une optimisation, pas une garantie

**Visualisation :**

```
Sans string pooling :
Segment .rodata :
0x1000 → "Hello\0"    ← str1
0x1006 → "Hello\0"    ← str2

Avec string pooling :
Segment .rodata :
0x1000 → "Hello\0"    ← str1 ET str2 (même adresse)
```

### Implications pratiques

**❌ Ne jamais comparer des chaînes avec == :**

```c
const char *str1 = "Hello";
const char *str2 = "Hello";

if (str1 == str2) {  // ❌ Compare les adresses, pas le contenu !
    // Peut être vrai ou faux selon l'optimisation
}

if (strcmp(str1, str2) == 0) {  // ✅ Compare le contenu
    // Toujours correct
}
```

### Comportement non garanti

```c
const char *a = "Test";
const char *b = "Test";
const char *c = "Te" "st";  // Concaténation à la compilation

printf("%d\n", a == b);  // Peut être 1 ou 0
printf("%d\n", a == c);  // Peut être 1 ou 0
```

Le standard C **ne garantit pas** le pooling. C'est une optimisation du compilateur.

## Concaténation de chaînes littérales

### Concaténation automatique à la compilation

Le compilateur concatène automatiquement les littérales adjacentes :

```c
const char *message = "Hello " "World";
// Équivalent à : const char *message = "Hello World";
```

**Utilité : Découpage de longues chaînes :**

```c
const char *sql_query =
    "SELECT id, nom, prenom "
    "FROM utilisateurs "
    "WHERE age > 18 "
    "ORDER BY nom";
// Équivaut à une seule chaîne longue
```

### Utilisation avec les macros

```c
#define VERSION "1.0.2"

const char *about =
    "Application v" VERSION "\n"
    "Copyright 2024";
// Devient : "Application v1.0.2\nCopyright 2024"
```

### Échappement et caractères spéciaux

```c
const char *str = "Ligne 1\n"
                  "Ligne 2\n"
                  "Ligne 3";

const char *path = "C:\\Users\\Documents\\file.txt";

const char *quote = "Il a dit : \"Bonjour !\"";
```

## Chaînes brutes (Raw strings) - C++11+

**Note :** Pas disponible en C standard, uniquement en C++.

En C++11+, les raw strings évitent l'échappement :

```cpp
// C++ uniquement
const char *str = R"(C:\Users\Documents\file.txt)";
const char *json = R"({"name": "Alice", "age": 30})";
```

En C, vous devez échapper manuellement :

```c
const char *json = "{\"name\": \"Alice\", \"age\": 30}";
```

## Chaînes multi-lignes

### Avec backslash

```c
const char *texte = "Ceci est une chaîne \
qui continue sur \
plusieurs lignes";
// Devient : "Ceci est une chaîne qui continue sur plusieurs lignes"
```

**Attention :** Pas d'espaces après le `\` !

### Avec concaténation

```c
const char *texte =
    "Ligne 1\n"
    "Ligne 2\n"
    "Ligne 3\n";
```

### Tableaux de chaînes

```c
const char *lignes[] = {
    "Première ligne",
    "Deuxième ligne",
    "Troisième ligne"
};

for (int i = 0; i < 3; i++) {
    printf("%s\n", lignes[i]);
}
```

## Cas particuliers et pièges

### Piège 1 : Retourner une chaîne locale

```c
// ❌ TRÈS DANGEREUX
char *get_greeting(const char *name) {
    char buffer[100];
    snprintf(buffer, sizeof(buffer), "Hello %s", name);
    return buffer;  // ❌ buffer n'existe plus !
}

// ✅ Solution 1 : Littérale (si fixe)
const char *get_greeting_literal(void) {
    return "Hello";
}

// ✅ Solution 2 : Buffer statique (attention : partagé)
char *get_greeting_static(const char *name) {
    static char buffer[100];
    snprintf(buffer, sizeof(buffer), "Hello %s", name);
    return buffer;  // OK mais partagé entre appels
}

// ✅ Solution 3 : Allocation dynamique
char *get_greeting_malloc(const char *name) {
    char *buffer = malloc(100);
    if (buffer) {
        snprintf(buffer, 100, "Hello %s", name);
    }
    return buffer;  // Appelant doit faire free()
}
```

### Piège 2 : Confusion tableau/pointeur

```c
// Global ou static : pointeur
const char *global_ptr = "Text";  // ✅ OK

// Local : piège potentiel
void fonction(void) {
    char *local_ptr = "Text";  // ⚠️ Devrait être const
    local_ptr[0] = 't';        // ❌ Comportement indéfini
}
```

### Piège 3 : Modification via cast

```c
const char *str = "Hello";
char *modifiable = (char*)str;  // ⚠️ Cast retire const
modifiable[0] = 'h';  // ❌ TOUJOURS interdit, même avec cast !
```

**Le cast ne change pas la réalité :** La chaîne est toujours en zone read-only.

### Piège 4 : Mélanger littérales et tableaux

```c
char *strs[] = {
    "Littérale 1",  // OK : pointeurs vers littérales
    "Littérale 2",
    buffer          // OK si buffer est un char[]
};

// ❌ Erreur : tentative de modifier
for (int i = 0; i < 2; i++) {
    strs[i][0] = 'X';  // Comportement indéfini pour les littérales
}
```

## Bonnes pratiques

### ✅ 1. Toujours utiliser const pour les littérales

```c
// ❌ Ancien style (déprécié)
char *message = "Hello";

// ✅ Style moderne
const char *message = "Hello";
```

### ✅ 2. Utiliser des tableaux pour les chaînes modifiables

```c
// Si vous devez modifier
char buffer[] = "Hello";
buffer[0] = 'h';  // ✅ OK

// Si vous ne modifiez pas
const char *texte = "Hello";  // ✅ OK
```

### ✅ 3. Documenter avec const dans les fonctions

```c
// ✅ Signale clairement que la fonction ne modifie pas
void print_message(const char *msg) {
    printf("%s\n", msg);
}

// ✅ Permet de passer littérales et tableaux
char buffer[] = "Test";
print_message(buffer);      // OK
print_message("Littérale"); // OK
```

### ✅ 4. Préférer les littérales pour les constantes

```c
// ✅ Bon : pas de mémoire gaspillée
const char *error_msg = "Erreur fatale";

// ❌ Moins bon : allocation inutile
char error_msg[] = "Erreur fatale";
```

### ✅ 5. Attention aux retours de fonction

```c
// ✅ OK : littérale (durée de vie statique)
const char *get_status(int code) {
    return code == 0 ? "OK" : "ERREUR";
}

// ❌ Dangereux : variable locale
char *get_status_bad(int code) {
    char buffer[10];
    strcpy(buffer, code == 0 ? "OK" : "ERREUR");
    return buffer;  // ❌ Détruit après le retour
}
```

## Exemple complet : Utilisation correcte

```c
#include <stdio.h>
#include <string.h>

// ✅ Constantes globales : littérales
const char *APP_NAME = "MonApp";
const char *VERSION = "1.0.0";

// ✅ Tableau de messages
const char *error_messages[] = {
    "Aucune erreur",
    "Fichier non trouvé",
    "Permission refusée",
    "Mémoire insuffisante"
};

// ✅ Fonction retournant une littérale
const char *get_error_message(int code) {
    if (code < 0 || code >= sizeof(error_messages) / sizeof(error_messages[0])) {
        return "Erreur inconnue";
    }
    return error_messages[code];
}

// ✅ Fonction avec paramètre const
void log_message(const char *level, const char *message) {
    printf("[%s] %s\n", level, message);
}

// ✅ Construction de chaîne avec buffer local
void print_header(void) {
    char header[100];
    snprintf(header, sizeof(header),
             "%s v%s", APP_NAME, VERSION);
    printf("=== %s ===\n", header);
}

int main(void) {
    // ✅ Littérales avec const
    const char *greeting = "Bonjour";

    print_header();

    log_message("INFO", greeting);
    log_message("ERROR", get_error_message(2));

    // ✅ Tableau modifiable si nécessaire
    char username[50];
    snprintf(username, sizeof(username), "user_%d", 123);
    printf("Utilisateur : %s\n", username);

    // ✅ Concaténation de littérales
    const char *welcome =
        "Bienvenue dans l'application.\n"
        "Veuillez vous connecter.";
    printf("%s\n", welcome);

    return 0;
}
```

## Résumé des points clés

1. **Les chaînes littérales sont immuables**
   - Stockées dans le segment `.rodata` (read-only)
   - Toute modification → comportement indéfini (souvent crash)

2. **char[] vs char* : différence cruciale**
   - `char tab[] = "..."` : copie modifiable sur la pile
   - `char *ptr = "..."` : pointeur vers littérale immuable

3. **Toujours utiliser const**
   - `const char *ptr = "..."` : bonne pratique moderne
   - Prévient les modifications accidentelles
   - Documente l'intention

4. **Durée de vie statique**
   - Les littérales existent pendant toute l'exécution
   - ✅ OK de retourner une littérale d'une fonction
   - ❌ Dangereux de retourner un tableau local

5. **String pooling : optimisation du compilateur**
   - Littérales identiques peuvent partager la même adresse
   - Jamais comparer avec `==`, toujours avec `strcmp()`

6. **Ne pas confondre adresse et contenu**
   - `==` compare les adresses
   - `strcmp()` compare le contenu

7. **const dans les paramètres**
   - Signale qu'une fonction ne modifie pas la chaîne
   - Permet de passer littérales et tableaux
   - Améliore la sécurité du code

8. **Pièges à éviter**
   - Modifier une littérale (via cast ou non)
   - Retourner un buffer local
   - Oublier const sur les pointeurs vers littérales

## Pour aller plus loin

Dans les prochains chapitres, nous verrons :
- **Chapitre 9** : Allocation dynamique (malloc/free)
- **Chapitre 24** : Gestion avancée de la mémoire
- **Chapitre 26** : Sécurité et code défensif

La compréhension des chaînes littérales et de leur immutabilité est **fondamentale** pour écrire du code C correct et sûr. Ces concepts sont à la base de nombreuses bonnes pratiques de programmation en C !

---

**💡 Règle d'or :** Si vous ne devez pas modifier une chaîne, utilisez `const char*`. Si vous devez la modifier, utilisez `char[]` ou allouez dynamiquement.

**⚠️ Rappel important :** Une chaîne littérale n'est JAMAIS modifiable, même avec un cast. Le compilateur la place en mémoire read-only. Toute tentative de modification est un comportement indéfini qui peut causer un crash ou pire.

⏭️ [Allocation Dynamique](/09-allocation-dynamique/README.md)
