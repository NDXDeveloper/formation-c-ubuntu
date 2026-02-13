🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 12.5 Conventions de nommage et standards

## Introduction

Le code est **lu beaucoup plus souvent qu'il n'est écrit**. Un développeur passe en moyenne 70% de son temps à **lire** du code et seulement 30% à en écrire. Des conventions de nommage cohérentes et des standards bien appliqués rendent le code :

- **Plus lisible** → Compréhension rapide
- **Plus maintenable** → Modifications facilitées
- **Plus professionnel** → Crédibilité accrue
- **Collaboratif** → Travail en équipe harmonieux

Dans cette section, nous allons explorer :
- Les différents styles de nommage
- Les conventions pour chaque type d'élément (variables, fonctions, macros, etc.)
- Les standards de codage reconnus
- Les règles de formatage
- Les bonnes pratiques de documentation

**Principe fondamental :** Choisissez des conventions et **appliquez-les partout** dans votre projet. La cohérence est plus importante que le style spécifique choisi.

---

## Les styles de nommage

### 1. snake_case (recommandé en C)

**Format :** Mots séparés par des underscores (`_`), tout en minuscules.

```c
int user_count;  
void calculate_total_price(void);  
struct database_connection;  
```

**Avantages :**
- ✅ Très lisible  
- ✅ Convention standard en C  
- ✅ Utilisé dans la bibliothèque standard C (`printf`, `strlen`, etc.)  
- ✅ Fonctionne bien avec les noms longs

**Utilisé pour :** Variables, fonctions, noms de fichiers, structures

### 2. camelCase

**Format :** Première lettre en minuscule, majuscule pour chaque nouveau mot.

```c
int userCount;  
void calculateTotalPrice(void);  
```

**Usage en C :** Peu courant, plus typique du Java/JavaScript.

**Inconvénient :** Moins cohérent avec le style C traditionnel.

### 3. PascalCase (UpperCamelCase)

**Format :** Première lettre de chaque mot en majuscule.

```c
int UserCount;  
void CalculateTotalPrice(void);  
```

**Usage en C :** Parfois utilisé pour les types (`typedef`).

```c
typedef struct {
    int x, y;
} Point2D;
```

### 4. UPPER_CASE

**Format :** Tout en majuscules avec underscores.

```c
#define MAX_BUFFER_SIZE 1024
#define PI 3.14159265359
enum { STATUS_OK, STATUS_ERROR };
```

**Usage en C :** **Exclusivement** pour les macros et constantes.

### 5. kebab-case

**Format :** Mots séparés par des tirets.

```c
// ❌ Invalide en C (le tiret n'est pas autorisé dans les identificateurs)
int user-count;  // ERREUR de compilation
```

**Usage en C :** **Uniquement** pour les noms de fichiers.

```
my-project-config.h  ✅ (nom de fichier)  
my_project_config.h  ✅ (plus courant)  
```

---

## Conventions par type d'élément

### Variables locales

**Style :** `snake_case`

**Principes :**
- Noms courts mais descriptifs
- Éviter les abréviations obscures
- Contexte clair

```c
// ✅ Bon
int user_count = 0;  
double total_price = 0.0;  
char* file_path = NULL;  
size_t buffer_size = 1024;  

// ❌ Mauvais
int uc;              // Trop court, pas clair  
int usrCnt;          // Abréviation  
int theUserCount;    // Article inutile  
int user_count_var;  // Suffixe redondant  
```

**Variables de boucle :**

```c
// ✅ Acceptable pour les boucles courtes
for (int i = 0; i < 10; i++) { }  
for (size_t j = 0; j < array_size; j++) { }  

// ✅ Meilleur pour les boucles complexes
for (size_t user_index = 0; user_index < user_count; user_index++) { }
```

### Variables globales

**Style :** `snake_case` avec un préfixe `g_`

**Raison :** Identifier immédiatement qu'il s'agit d'une variable globale.

```c
// globals.c
int g_debug_level = 0;  
char* g_config_path = "/etc/myapp/config";  
bool g_is_running = false;  

// Ou avec préfixe "global_"
int global_debug_level = 0;
```

**Alternative :** Certains projets utilisent un préfixe du nom du projet.

```c
// Pour le projet "myapp"
int myapp_debug_level = 0;  
int myapp_connection_count = 0;  
```

**Important :** Limitez l'usage des variables globales au strict minimum !

### Constantes et macros

**Style :** `UPPER_CASE`

```c
#define MAX_USERS 100
#define DEFAULT_PORT 8080
#define VERSION "1.0.0"
#define PI 3.14159265359

// Macros paramétrées
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
```

**Constantes `const` :**

Deux écoles :

```c
// Style 1 : Comme une variable normale
const int max_users = 100;  
const double pi = 3.14159265359;  

// Style 2 : Comme une macro (plus courant en C)
const int MAX_USERS = 100;  
const double PI = 3.14159265359;  
```

**Recommandation :** Préférez les constantes `const` aux macros `#define` quand c'est possible (type-safe).

### Fonctions

**Style :** `snake_case` avec préfixe de module

**Format :** `module_action_object()`

```c
// Module "user"
void user_create(const char* name);  
void user_destroy(User* user);  
bool user_is_valid(const User* user);  
const char* user_get_name(const User* user);  
void user_set_email(User* user, const char* email);  

// Module "database"
int database_connect(const char* host);  
void database_disconnect(void);  
bool database_execute_query(const char* query);  
```

**Verbes courants :**
- `create` / `destroy` : Création/destruction
- `init` / `cleanup` : Initialisation/nettoyage
- `get` / `set` : Accesseurs/mutateurs
- `is` / `has` : Tests booléens
- `add` / `remove` : Ajout/suppression
- `open` / `close` : Ouverture/fermeture
- `start` / `stop` : Démarrage/arrêt

**Fonctions privées (statiques) :**

```c
// Pas de préfixe nécessaire si la fonction est static
static void helper_function(void) { }  
static int calculate_checksum(const uint8_t* data, size_t len) { }  

// Ou avec préfixe underscore (certains projets)
static void _internal_helper(void) { }
```

### Structures

**Style :** `snake_case` pour le nom de la structure

```c
// ✅ Bon
struct user_account {
    char name[64];
    char email[128];
    int age;
};

struct database_connection {
    int socket_fd;
    char* host;
    int port;
};
```

**Avec `typedef` :**

Deux approches courantes :

```c
// Approche 1 : Nom simple (PascalCase)
typedef struct {
    int x, y;
} Point;

typedef struct {
    char name[64];
    int age;
} User;

// Approche 2 : Suffixe _t (POSIX style)
typedef struct {
    int x, y;
} point_t;

typedef struct {
    char name[64];
    int age;
} user_t;

// Approche 3 : Même nom pour struct et typedef
typedef struct user {
    char name[64];
    int age;
} user;
```

**Recommandation :** Choisissez **un seul style** et soyez cohérent.

**⚠️ Attention :** Le suffixe `_t` est réservé par POSIX. Pour éviter les conflits, certains projets utilisent d'autres suffixes ou évitent les typedef.

### Énumérations

**Style :** Nom en `snake_case`, valeurs en `UPPER_CASE` avec préfixe

```c
// ✅ Bon
enum connection_status {
    CONNECTION_STATUS_DISCONNECTED,
    CONNECTION_STATUS_CONNECTING,
    CONNECTION_STATUS_CONNECTED,
    CONNECTION_STATUS_ERROR
};

enum color {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE
};
```

**Avec `typedef` :**

```c
typedef enum {
    STATUS_OK = 0,
    STATUS_ERROR = -1,
    STATUS_PENDING = 1
} Status;

// Usage
Status result = STATUS_OK;
```

**Pourquoi préfixer les valeurs ?**

Sans préfixe, risque de collision :

```c
enum status { OK, ERROR };      // OK et ERROR  
enum result { OK, FAILED };     // ❌ Conflit : OK déjà défini !  
```

Avec préfixe :

```c
enum status { STATUS_OK, STATUS_ERROR };  
enum result { RESULT_OK, RESULT_FAILED };  // ✅ Pas de conflit  
```

### Types personnalisés (`typedef`)

**Style :** Dépend de la convention choisie

```c
// Style 1 : PascalCase (courant)
typedef unsigned char Byte;  
typedef int (*CallbackFunction)(void* data);  

// Style 2 : snake_case avec suffixe _t
typedef unsigned char byte_t;  
typedef int (*callback_function_t)(void* data);  

// Style 3 : snake_case sans suffixe
typedef unsigned char byte;  
typedef int (*callback_function)(void* data);  
```

**Recommandation standard C :** Évitez le suffixe `_t` (réservé POSIX). Utilisez plutôt un suffixe personnalisé :

```c
typedef struct config config_s;    // Suffixe _s pour struct  
typedef void (*cleanup_fn)(void*); // Suffixe _fn pour fonction  
```

### Pointeurs de fonction

**Style :** Suffixe `_fn` ou `_func`

```c
// ✅ Bon
typedef int (*compare_fn)(const void* a, const void* b);  
typedef void (*callback_func)(int event_id, void* user_data);  
typedef void (*cleanup_fn)(void* resource);  

// Usage
compare_fn my_comparator = &my_compare_function;
```

### Paramètres de fonction

**Style :** `snake_case`, même règles que les variables locales

```c
// ✅ Bon
void process_user(const char* user_name, int user_age, bool is_admin);

// ❌ Mauvais
void process_user(const char* n, int a, bool ia);  // Trop court  
void process_user(const char*, int, bool);         // Sans noms (acceptable dans les déclarations)  
```

**Déclarations vs définitions :**

```c
// Dans le .h (déclaration) : noms optionnels mais recommandés
void process_user(const char* user_name, int user_age, bool is_admin);

// Dans le .c (définition) : noms obligatoires
void process_user(const char* user_name, int user_age, bool is_admin) {
    // Implémentation
}
```

### Noms de fichiers

**Style :** `snake_case` ou `kebab-case`

```c
// ✅ Bon
user_manager.c / user_manager.h  
database_connector.c / database_connector.h  
config_parser.c / config_parser.h  

// Ou (moins courant)
user-manager.c / user-manager.h
```

**Correspondance `.c` ↔ `.h` :**

```c
user_manager.c  ↔  user_manager.h  
network.c       ↔  network.h  
```

**Recommandation :** Utilisez le même nom de base pour le `.c` et le `.h`.

---

## Standards de codage reconnus

### 1. Style K&R (Kernighan & Ritchie)

**Le style originel du C**, utilisé dans "The C Programming Language".

**Caractéristiques :**
- Accolade ouvrante sur la **même ligne** (sauf fonctions)
- Indentation : 8 espaces ou 1 tabulation
- Espaces autour des opérateurs

```c
// Fonctions : accolade sur la ligne suivante
int main(void)
{
    int x = 10;

    // Structures de contrôle : accolade sur la même ligne
    if (x > 0) {
        printf("Positif\n");
    } else {
        printf("Négatif ou nul\n");
    }

    for (int i = 0; i < 10; i++) {
        printf("%d ", i);
    }

    return 0;
}
```

**Variante K&R moderne (1TBS - One True Brace Style) :**

```c
// Accolades sur la même ligne PARTOUT
int main(void) {
    int x = 10;

    if (x > 0) {
        printf("Positif\n");
    } else {
        printf("Négatif ou nul\n");
    }

    return 0;
}
```

### 2. Style GNU

**Utilisé par les projets GNU** (GCC, Emacs, etc.).

**Caractéristiques :**
- Accolades **toujours** sur leur propre ligne
- Indentation : 2 espaces

```c
int  
main (void)  
{
  int x = 10;

  if (x > 0)
    {
      printf ("Positif\n");
    }
  else
    {
      printf ("Négatif ou nul\n");
    }

  return 0;
}
```

**Opinion :** Style verbeux, moins populaire aujourd'hui.

### 3. Style Linux Kernel

**Utilisé pour le noyau Linux**, défini dans `Documentation/CodingStyle`.

**Caractéristiques :**
- Basé sur K&R
- Tabulations (8 espaces) obligatoires
- Ligne max : 80 caractères (recommandé)
- Pas d'espaces avant les parenthèses de fonction

```c
int main(void)
{
	int x = 10;

	if (x > 0) {
		printf("Positif\n");
	} else {
		printf("Négatif ou nul\n");
	}

	return 0;
}
```

**Règles spécifiques :**
- `goto` est acceptable pour le nettoyage d'erreurs
- Limiter les niveaux d'indentation (max 3-4)
- Fonctions courtes (< 40 lignes idéalement)

### 4. Style MISRA C

**Standard pour l'embarqué critique** (automobile, aéronautique, médical).

**Caractéristiques :**
- Règles très strictes de sécurité
- Interdit certaines constructions dangereuses
- Typage fort obligatoire
- Tous les chemins de code doivent être testables

**Exemples de règles :**
- Pas de récursivité
- Pas d'allocation dynamique (malloc interdit)
- Pas de pointeurs de fonction (ou usage limité)
- Toutes les variables doivent être initialisées

```c
// ✅ MISRA-compliant
int calculate_sum(int a, int b)
{
    int result = a + b;  // Variable initialisée
    return result;
}

// ❌ Non-compliant
int calculate_sum(int a, int b)
{
    int result;  // Variable non initialisée
    result = a + b;
    return result;
}
```

### 5. Style Google

**Utilisé par Google** pour les projets C/C++.

**Caractéristiques :**
- Basé sur K&R/1TBS
- Indentation : 2 espaces
- Ligne max : 80 caractères

```c
int main(void) {
  int x = 10;

  if (x > 0) {
    printf("Positif\n");
  } else {
    printf("Négatif ou nul\n");
  }

  return 0;
}
```

---

## Règles de formatage détaillées

### Indentation

**Choisir entre espaces et tabulations :**

```c
// Style 1 : Tabulations (Linux Kernel)
int main(void)
{
	if (condition) {
		printf("Hello\n");
	}
}

// Style 2 : Espaces (plus courant aujourd'hui)
int main(void)
{
    if (condition) {
        printf("Hello\n");
    }
}
```

**Recommandation :**
- **Espaces** (4 espaces) pour la cohérence multi-éditeur
- Ou **tabulations** pour les gros projets établis (Linux, Git)

**Configuration dans `.editorconfig` :**

```ini
[*.c]
indent_style = space  
indent_size = 4  
```

### Espacement

**Autour des opérateurs :**

```c
// ✅ Bon
int x = a + b;  
int y = x * 2;  
bool result = (x > 0) && (y < 100);  

// ❌ Mauvais
int x=a+b;  
int y = x*2;  
bool result=(x>0)&&(y<100);  
```

**Après les virgules :**

```c
// ✅ Bon
printf("%d, %d, %d\n", a, b, c);  
int array[] = {1, 2, 3, 4, 5};  

// ❌ Mauvais
printf("%d,%d,%d\n",a,b,c);  
int array[] = {1,2,3,4,5};  
```

**Autour des parenthèses :**

```c
// ✅ Bon
if (condition) { }  
for (int i = 0; i < 10; i++) { }  
void function(int x, int y);  

// ❌ Mauvais
if( condition ){ }  
for( int i=0;i<10;i++ ){ }  
void function (int x,int y);  
```

**Pointeur : position de `*` :**

Trois conventions :

```c
// Style 1 : Proche du type (recommandé)
int* ptr;  
char* str;  

// Style 2 : Proche du nom
int *ptr;  
char *str;  

// Style 3 : Centré
int * ptr;  
char * str;  
```

**Piège avec style 1 :**

```c
int* a, b;  // a est un pointeur, b est un int ! (pas un pointeur)

// Plus clair :
int* a;  
int b;  

// Ou style 2 :
int *a, *b;  // Les deux sont des pointeurs
```

**Recommandation :** Une déclaration par ligne pour éviter la confusion.

### Longueur de ligne

**Standard :** 80-120 caractères maximum.

```c
// ✅ < 80 caractères
void short_function(int x) {
    printf("Value: %d\n", x);
}

// ❌ > 120 caractères
void long_function_with_many_parameters(const char* user_name, const char* user_email, int user_age, bool is_admin, const char* department) {
    // ...
}

// ✅ Ligne coupée
void long_function_with_many_parameters(
    const char* user_name,
    const char* user_email,
    int user_age,
    bool is_admin,
    const char* department
) {
    // ...
}
```

### Accolades

**Règle générale :** Toujours utiliser des accolades, même pour une ligne.

```c
// ❌ Dangereux
if (condition)
    printf("OK\n");

// ✅ Sûr
if (condition) {
    printf("OK\n");
}
```

**Raison :** Évite les bugs lors de l'ajout de code.

```c
// Bug célèbre (Apple's goto fail)
if (condition)
    do_something();
    do_something_else();  // Toujours exécuté ! Pas dans le if
```

**Exception :** Certains projets acceptent l'omission pour des conditions très simples, mais ce n'est pas recommandé.

### Espacement vertical

**Règles :**
- Une ligne vide entre les fonctions
- Grouper les déclarations de variables
- Séparer les blocs logiques dans une fonction

```c
// ✅ Bon
#include <stdio.h>
#include <stdlib.h>

#define MAX_SIZE 100

int global_var = 0;

void function1(void)
{
    int x = 10;
    int y = 20;

    // Premier bloc logique
    x = x + y;
    printf("Sum: %d\n", x);

    // Deuxième bloc logique
    y = x * 2;
    printf("Double: %d\n", y);
}

void function2(void)
{
    // ...
}

// ❌ Mauvais : trop compact
void function1(void){int x=10;int y=20;x=x+y;printf("Sum: %d\n",x);y=x*2;printf("Double: %d\n",y);}  
void function2(void){/*...*/}  
```

---

## Documentation et commentaires

### Commentaires de documentation (Doxygen)

**Format Doxygen :** Documentation générée automatiquement.

```c
/**
 * @brief Calcule la somme de deux entiers
 *
 * Cette fonction additionne deux nombres entiers et retourne le résultat.
 *
 * @param a Premier entier
 * @param b Deuxième entier
 * @return La somme de a et b
 *
 * @note Cette fonction ne gère pas les débordements
 * @warning Peut causer un débordement pour de grandes valeurs
 *
 * @code
 * int result = add(5, 3);  // result vaut 8
 * @endcode
 */
int add(int a, int b);
```

**Tags courants :**
- `@brief` : Description courte
- `@param` : Paramètre de fonction
- `@return` : Valeur de retour
- `@note` : Note importante
- `@warning` : Avertissement
- `@see` : Référence croisée
- `@code` / `@endcode` : Exemple de code

### Commentaires en-tête de fichier

```c
/**
 * @file user_manager.c
 * @brief Gestion des utilisateurs de l'application
 * @author Votre Nom
 * @date 2025-01-15
 * @version 1.0
 *
 * Ce module gère la création, modification et suppression des utilisateurs.
 * Il fournit également des fonctions de validation et de recherche.
 */

#include "user_manager.h"
```

### Commentaires de structure

```c
/**
 * @brief Représente un utilisateur du système
 */
typedef struct {
    char name[64];      /**< Nom complet de l'utilisateur */
    char email[128];    /**< Adresse email */
    int age;            /**< Âge en années */
    bool is_admin;      /**< Indique si l'utilisateur est administrateur */
} User;
```

### Commentaires dans le code

**Règles :**
- Expliquer **pourquoi**, pas **quoi**
- Commenter les algorithmes complexes
- Signaler les pièges et subtilités
- Expliquer les hacks et workarounds

```c
// ❌ Mauvais : commentaire inutile
int x = 10;  // Assigne 10 à x

// ✅ Bon : explique le pourquoi
int x = 10;  // Nombre maximum de tentatives de reconnexion

// ✅ Bon : explique la logique non évidente
// On ajoute 1 car les indices commencent à 0 mais on compte à partir de 1
int total_items = last_index + 1;

// ✅ Bon : signale un workaround
// HACK : Ce code contourne un bug dans la bibliothèque libfoo v1.2
// TODO : Supprimer ce workaround quand libfoo sera mise à jour
if (workaround_needed) {
    // ...
}
```

**Tags courants :**
- `TODO` : À faire ultérieurement
- `FIXME` : Bug connu à corriger
- `HACK` : Solution temporaire non optimale
- `NOTE` : Information importante
- `XXX` : Code problématique à revoir

### Commentaires multi-lignes

```c
/*
 * Ceci est un commentaire
 * sur plusieurs lignes.
 * Chaque ligne commence par une étoile pour la lisibilité.
 */

// Ou style C++ (aussi valide en C99+)
// Commentaire sur une seule ligne
// Suite du commentaire
```

---

## Conventions spécifiques au langage C

### Retours de fonction

**Convention standard :** `0` = succès, `-1` ou valeur négative = erreur.

```c
// ✅ Bon
int open_file(const char* path)
{
    if (path == NULL) {
        return -1;  // Erreur : paramètre invalide
    }

    // ...

    return 0;  // Succès
}

// Ou utiliser des codes d'erreur
enum error_code {
    ERROR_SUCCESS = 0,
    ERROR_INVALID_PARAM = -1,
    ERROR_FILE_NOT_FOUND = -2,
    ERROR_NO_MEMORY = -3
};
```

**Alternative moderne :** Types d'erreur explicites.

```c
typedef enum {
    RESULT_OK,
    RESULT_ERROR,
    RESULT_NOT_FOUND
} Result;

Result find_user(int user_id, User** out_user);
```

### Usage de `const`

**Règle :** Utilisez `const` partout où possible.

```c
// ✅ Bon : paramètres en lecture seule
int calculate_length(const char* str);  
void print_array(const int* array, size_t size);  

// ✅ Bon : valeurs de retour constantes
const char* get_version(void);

// ✅ Bon : variables locales constantes
const double pi = 3.14159265359;  
const int max_users = 100;  
```

### Valeurs booléennes

**En C99+ :** Utilisez `<stdbool.h>`.

```c
#include <stdbool.h>

bool is_valid_user(const User* user)
{
    if (user == NULL) {
        return false;
    }

    return user->age >= 18;
}
```

**En C89 :** Définir manuellement.

```c
#define TRUE 1
#define FALSE 0

typedef int bool;  // Ou utiliser int directement
```

### NULL vs 0

**Règle :** Utilisez `NULL` pour les pointeurs, `0` pour les entiers.

```c
// ✅ Bon
int* ptr = NULL;  
int value = 0;  

if (ptr == NULL) { }  
if (value == 0) { }  

// ❌ Confus (mais valide)
int* ptr = 0;  
if (ptr == 0) { }  
```

### Taille des types

**Règle :** Utilisez les types de `<stdint.h>` pour la portabilité.

```c
#include <stdint.h>

// ✅ Bon : tailles explicites
int32_t counter;         // Exactement 32 bits  
uint64_t timestamp;      // Non-signé 64 bits  
size_t buffer_size;      // Taille mémoire (typedef standard)  

// ❌ Moins portable
int counter;             // Généralement 32 bits, mais 16 bits sur certaines plateformes embarquées  
unsigned long timestamp; // Peut être 32 ou 64 bits  
```

---

## Outils d'aide au formatage

### clang-format

**Outil automatique** qui formate le code selon un style configuré.

**Installation :**

```bash
sudo apt install clang-format  # Ubuntu/Debian
```

**Utilisation :**

```bash
# Formater un fichier
clang-format -i mon_fichier.c

# Vérifier sans modifier
clang-format mon_fichier.c | diff mon_fichier.c -
```

**Configuration (`.clang-format`) :**

```yaml
---
BasedOnStyle: LLVM  
IndentWidth: 4  
ColumnLimit: 100  
PointerAlignment: Left  
BreakBeforeBraces: Attach  
```

**Styles prédéfinis :**
- `LLVM` : Style LLVM/Clang
- `Google` : Style Google
- `Chromium` : Style Chromium
- `Mozilla` : Style Mozilla
- `WebKit` : Style WebKit
- `GNU` : Style GNU

### indent (GNU)

```bash
# Style K&R
indent -kr mon_fichier.c

# Style GNU
indent -gnu mon_fichier.c

# Style Linux
indent -linux mon_fichier.c
```

### uncrustify

Alternative à clang-format, très configurable.

```bash
uncrustify -c my_config.cfg --replace mon_fichier.c
```

---

## Checklist de style

### Avant de commiter du code

- [ ] Code formaté selon le style du projet
- [ ] Pas de lignes > 80-120 caractères
- [ ] Nommage cohérent (snake_case pour variables/fonctions)
- [ ] UPPER_CASE pour macros et constantes
- [ ] Commentaires à jour (si modifié)
- [ ] Pas de code commenté (supprimer ou expliquer pourquoi)
- [ ] Pas de TODO/FIXME non documentés
- [ ] Variables initialisées
- [ ] Pas de warnings de compilation (-Wall -Wextra)

### Revue de code

Points à vérifier lors d'une code review :

- [ ] Les noms sont-ils explicites et cohérents ?
- [ ] Le code respecte-t-il le style du projet ?
- [ ] Y a-t-il des commentaires là où c'est nécessaire ?
- [ ] Les fonctions sont-elles suffisamment courtes ?
- [ ] Les niveaux d'indentation sont-ils raisonnables (<4) ?
- [ ] Les constantes magiques sont-elles nommées ?
- [ ] Le code est-il facile à comprendre ?

---

## Exemples comparatifs : Bon vs Mauvais

### Exemple 1 : Fonction de calcul

**❌ Mauvais :**

```c
int f(int x,int y){int r=x+y;if(r>100)r=100;return r;}
```

**Problèmes :**
- Nom non descriptif (`f`)
- Pas d'espacement
- Pas de commentaires
- Logique peu claire

**✅ Bon :**

```c
/**
 * @brief Calcule la somme de deux nombres avec un plafond
 *
 * @param x Premier nombre
 * @param y Deuxième nombre
 * @return La somme, plafonnée à 100
 */
int calculate_capped_sum(int x, int y)
{
    const int MAX_SUM = 100;
    int result = x + y;

    // Plafonner le résultat pour éviter les valeurs trop élevées
    if (result > MAX_SUM) {
        result = MAX_SUM;
    }

    return result;
}
```

### Exemple 2 : Gestion d'utilisateur

**❌ Mauvais :**

```c
struct u{char n[50];int a;};  
void p(struct u *x){  
printf("%s %d\n",x->n,x->a);  
}
int main(){  
struct u usr;  
strcpy(usr.n,"John");usr.a=25;  
p(&usr);  
return 0;}  
```

**✅ Bon :**

```c
#include <stdio.h>
#include <string.h>

/**
 * @brief Représente un utilisateur
 */
typedef struct {
    char name[50];    /**< Nom de l'utilisateur */
    int age;          /**< Âge en années */
} User;

/**
 * @brief Affiche les informations d'un utilisateur
 * @param user Pointeur vers l'utilisateur à afficher
 */
void user_print(const User* user)
{
    if (user == NULL) {
        fprintf(stderr, "Erreur : pointeur NULL\n");
        return;
    }

    printf("Nom : %s, Age : %d\n", user->name, user->age);
}

int main(void)
{
    User user = {0};  // Initialisation à zéro

    strncpy(user.name, "John", sizeof(user.name) - 1);
    user.name[sizeof(user.name) - 1] = '\0';  // Assure la terminaison
    user.age = 25;

    user_print(&user);

    return 0;
}
```

### Exemple 3 : Macro

**❌ Mauvais :**

```c
#define max(a,b) a>b?a:b

int x = 2 * max(3, 5);
// Erreur : évalue 2*3>5?3:5 → (2*3)>5 ? 3 : 5 → 6>5 ? 3 : 5 → 3
// Résultat attendu : 2 * 5 = 10, résultat obtenu : 3 !
```

**✅ Bon :**

```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int x = 2 * MAX(3, 5);
// Correct : 2 * ((3) > (5) ? (3) : (5)) → 2 * 5 → 10
```

**Encore mieux :** Fonction inline (C99+)

```c
static inline int max(int a, int b)
{
    return (a > b) ? a : b;
}
```

---

## Standards par projet : Exemples réels

### Linux Kernel

```c
int do_something(void)
{
	int ret = 0;

	if (condition) {
		ret = -EINVAL;
		goto out;
	}

	// Traitement normal

out:
	return ret;
}
```

**Spécificités :**
- Tabulations obligatoires
- `goto` pour nettoyage d'erreurs
- Codes d'erreur négatifs (errno)

### Git

```c
int do_something(void)
{
	int result;

	if (!condition)
		return -1;

	result = process();
	if (result < 0)
		return result;

	return 0;
}
```

**Spécificités :**
- Pas d'accolades si une seule instruction (controverse)
- Préfixes de fonction par module

### GNU (GCC, Emacs)

```c
int  
do_something (void)  
{
  int result = 0;

  if (condition)
    {
      result = process ();
    }

  return result;
}
```

**Spécificités :**
- Type de retour sur ligne séparée
- Espaces avant parenthèses de fonction
- Accolades toujours sur leur ligne

---

## Créer son propre style guide

### Template de style guide

```markdown
# Style Guide - [Nom du Projet]

## Nommage

- **Variables** : snake_case
- **Fonctions** : snake_case avec préfixe module
- **Macros** : UPPER_CASE
- **Structures** : snake_case ou PascalCase
- **Constantes** : UPPER_CASE

## Formatage

- **Indentation** : 4 espaces (pas de tabs)
- **Longueur de ligne** : 100 caractères max
- **Accolades** : Style K&R (même ligne sauf fonctions)
- **Espaces** : Autour des opérateurs, après virgules

## Commentaires

- Documentation Doxygen pour toutes les fonctions publiques
- Commentaires inline pour logique complexe
- En-tête de fichier avec auteur, date, description

## Conventions

- Toujours utiliser `const` quand possible
- `NULL` pour pointeurs, `0` pour entiers
- Utiliser `<stdbool.h>` pour booléens
- Retour : 0 = succès, -1 = erreur

## Outils

- Formater avec : `clang-format -style=file`
- Vérifier avec : `cppcheck --enable=all`
```

---

## Conclusion

Les conventions de nommage et les standards ne sont pas une contrainte arbitraire, mais un **investissement** dans la qualité et la maintenabilité du code. Un code bien nommé et bien formaté :

- Se lit comme de la prose
- Se comprend intuitivement
- Se maintient facilement
- Inspire confiance

**Points clés à retenir :**

1. **Cohérence** > Style particulier → Choisissez un style et appliquez-le partout
2. **Clarté** > Concision → `user_count` est meilleur que `uc`
3. **snake_case** pour variables et fonctions en C
4. **UPPER_CASE** pour macros et constantes
5. **Commenter le pourquoi**, pas le quoi
6. **Utilisez des outils** de formatage automatique
7. **Inspirez-vous** des grands projets (Linux, Git, Redis)

**Conseil final :** Avant de choisir votre style, regardez les conventions des projets auxquels vous voulez contribuer. Si vous travaillez sur le noyau Linux, adoptez leur style. Si vous créez votre propre projet, choisissez ce qui vous semble le plus lisible et soyez cohérent.

Un code propre est un code qui se fait oublier : on se concentre sur **ce qu'il fait**, pas sur **comment il est écrit**.

⏭️ [Forward declarations et dépendances circulaires](/12-organisation-code/06-forward-declarations.md)
