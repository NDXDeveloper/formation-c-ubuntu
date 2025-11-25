🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 4.3 Les constantes et les littéraux

## Introduction

Dans les sections précédentes, nous avons manipulé des **variables** — des emplacements mémoire dont la valeur peut changer. Mais que se passe-t-il quand vous avez besoin de valeurs qui **ne doivent jamais changer** ? C'est là qu'interviennent les **constantes**.

De plus, lorsque vous écrivez `42` ou `"Hello"` dans votre code, ce sont des **littéraux** — des valeurs directement écrites dans le code source.

Cette section vous apprendra à :
- Distinguer variables, constantes et littéraux
- Utiliser différentes notations (décimale, hexadécimale, etc.)
- Créer des constantes avec `const` et `#define`
- Manipuler les énumérations (`enum`)

## Variables vs Constantes

### Variable : valeur modifiable

Une variable peut être modifiée après sa déclaration :

```c
int compteur = 0;
compteur = 5;        // OK : modification autorisée
compteur = compteur + 1;  // OK
```

### Constante : valeur immuable

Une constante est déclarée une fois et ne peut plus être modifiée :

```c
const int JOURS_SEMAINE = 7;
// JOURS_SEMAINE = 8;  // ERREUR de compilation !
```

**Avantages des constantes** :
- **Sécurité** : évite les modifications accidentelles
- **Lisibilité** : donne un nom significatif à une valeur magique
- **Maintenance** : modifier une seule fois au lieu de chercher toutes les occurrences
- **Optimisation** : le compilateur peut mieux optimiser

## Qu'est-ce qu'un littéral ?

Un **littéral** est une valeur écrite directement dans le code source.

```c
int age = 25;           // 25 est un littéral entier
float pi = 3.14;        // 3.14 est un littéral flottant
char lettre = 'A';      // 'A' est un littéral caractère
char* message = "Bonjour";  // "Bonjour" est un littéral chaîne
```

Il existe plusieurs types de littéraux que nous allons explorer.

## Les littéraux entiers

### Notation décimale (base 10)

C'est la notation par défaut, celle que nous utilisons au quotidien :

```c
int population = 67000000;
int temperature = -15;
int zero = 0;
```

### Notation hexadécimale (base 16)

Préfixe : `0x` ou `0X`

Utilisée pour les adresses mémoire, les couleurs RGB, les masques de bits :

```c
int couleur_rouge = 0xFF0000;      // Rouge en RGB
int masque = 0x00FF;               // Masque pour les 8 bits de poids faible
unsigned int adresse = 0xDEADBEEF; // Adresse mémoire
```

**Chiffres hexadécimaux** : 0-9, A-F (ou a-f)
- `A` = 10, `B` = 11, `C` = 12, `D` = 13, `E` = 14, `F` = 15

**Équivalences** :
- `0x10` = 16 en décimal
- `0xFF` = 255 en décimal
- `0x100` = 256 en décimal

### Notation octale (base 8)

Préfixe : `0` (juste un zéro)

⚠️ **Piège courant** : Un nombre commençant par 0 est interprété en octal !

```c
int a = 100;   // Décimal : 100
int b = 0100;  // Octal : équivaut à 64 en décimal !

printf("%d\n", a);  // Affiche : 100
printf("%d\n", b);  // Affiche : 64
```

**Usage moderne** : L'octal est devenu rare. Attention à ne pas ajouter de zéros inutiles devant vos nombres !

### Notation binaire (base 2) — C23

Depuis C23 (support limité pour l'instant), préfixe : `0b` ou `0B`

```c
int masque = 0b11110000;  // 240 en décimal
int flags = 0b00001111;   // 15 en décimal
```

**Note** : Si votre compilateur ne supporte pas C23, utilisez l'hexadécimal :
```c
int masque = 0xF0;  // Équivalent à 0b11110000
```

### Tableau récapitulatif des bases

| Base | Préfixe | Exemple | Valeur décimale |
|------|---------|---------|-----------------|
| Décimale (10) | Aucun | `255` | 255 |
| Hexadécimale (16) | `0x` | `0xFF` | 255 |
| Octale (8) | `0` | `0377` | 255 |
| Binaire (2) | `0b` (C23) | `0b11111111` | 255 |

## Suffixes des littéraux entiers

Vous pouvez spécifier le type d'un littéral avec des suffixes :

| Suffixe | Type | Exemple |
|---------|------|---------|
| Aucun | `int` | `42` |
| `U` ou `u` | `unsigned int` | `42U` |
| `L` ou `l` | `long` | `42L` |
| `LL` ou `ll` | `long long` | `42LL` |
| `UL` | `unsigned long` | `42UL` |
| `ULL` | `unsigned long long` | `42ULL` |

**Exemples** :

```c
int normal = 100;
unsigned int non_signe = 100U;
long grand = 5000000000L;
unsigned long long enorme = 18446744073709551615ULL;

// Sans suffixe approprié, risque de dépassement
long long x = 5000000000;    // Peut causer une erreur (littéral int trop grand)
long long y = 5000000000LL;  // Correct
```

**Recommandation** : Utilisez toujours le bon suffixe pour les grandes valeurs.

## Les littéraux flottants

### Notation décimale

```c
float prix = 19.99;
double pi = 3.14159265359;
float zero = 0.0;
```

### Notation scientifique

Pour les très grands ou très petits nombres :

```c
double vitesse_lumiere = 3.0e8;      // 3.0 × 10⁸ = 300 000 000 m/s
double charge_electron = 1.6e-19;    // 1.6 × 10⁻¹⁹ coulombs
double avogadro = 6.022e23;          // Nombre d'Avogadro
```

**Syntaxe** : `nombre e exposant` ou `nombre E exposant`

### Suffixes des littéraux flottants

| Suffixe | Type | Exemple |
|---------|------|---------|
| Aucun | `double` | `3.14` |
| `f` ou `F` | `float` | `3.14f` |
| `l` ou `L` | `long double` | `3.14L` |

**Exemples** :

```c
float  simple = 3.14f;           // float (4 octets)
double precis = 3.14;            // double (8 octets) - PAR DÉFAUT
long double tres_precis = 3.14L; // long double (variable)
```

**Important** : Par défaut, un littéral flottant comme `3.14` est de type `double`, pas `float`.

```c
float x = 3.14;   // Conversion implicite de double vers float
float y = 3.14f;  // Directement un float (meilleur)
```

## Les littéraux caractères

Un caractère est entouré d'**apostrophes simples** `' '` :

```c
char lettre = 'A';
char chiffre = '7';
char symbole = '@';
```

### Séquences d'échappement

Pour représenter des caractères spéciaux :

| Séquence | Signification | Code ASCII |
|----------|---------------|------------|
| `'\n'` | Nouvelle ligne (Line Feed) | 10 |
| `'\t'` | Tabulation horizontale | 9 |
| `'\r'` | Retour chariot (Carriage Return) | 13 |
| `'\0'` | Caractère nul (fin de chaîne) | 0 |
| `'\\'` | Backslash `\` | 92 |
| `'\''` | Apostrophe `'` | 39 |
| `'\"'` | Guillemet `"` | 34 |
| `'\a'` | Alerte (bip sonore) | 7 |
| `'\b'` | Retour arrière (Backspace) | 8 |

**Exemples** :

```c
printf("Ligne 1\nLigne 2\n");         // Saut de ligne
printf("Nom\tPrenom\tAge\n");         // Tabulations
printf("Chemin : C:\\Users\\Documents\\"); // Backslash
printf("Il a dit : \"Bonjour\"\n");   // Guillemets
```

### Notation octale et hexadécimale pour caractères

Vous pouvez aussi spécifier un caractère par son code :

```c
char newline = '\n';    // Notation d'échappement
char newline2 = '\012'; // Notation octale (12 en octal = 10 en décimal)
char newline3 = '\x0A'; // Notation hexadécimale (0A en hexa = 10 en décimal)

// Ces trois variables contiennent la même valeur
```

**Usage** : Rare en pratique, sauf pour des caractères non imprimables spécifiques.

## Les littéraux chaînes de caractères

Une chaîne est entourée de **guillemets doubles** `" "` :

```c
char* message = "Bonjour";
char* vide = "";
char* multiligne = "Cette chaîne est "
                   "automatiquement "
                   "concaténée";
```

### Caractères spéciaux dans les chaînes

Les séquences d'échappement fonctionnent aussi dans les chaînes :

```c
printf("Ligne 1\nLigne 2\n");
printf("Tabulation :\tcolonne alignée\n");
printf("Chemin Windows : C:\\Program Files\\App\n");
printf("Elle a dit : \"Salut !\"\n");
```

### Chaînes brutes (raw strings) — Futur C23

⚠️ Pas encore standardisé dans tous les compilateurs, mais certains supportent :

```c
// Syntax non-standard (GCC/Clang avec extensions)
const char* regex = R"(C:\Users\Documents\file.txt)";
```

**Astuce actuelle** : Utilisez les doubles backslashes en attendant.

## Le mot-clé `const`

Le mot-clé `const` rend une variable **non modifiable** après son initialisation.

### Syntaxe de base

```c
const int JOURS_ANNEE = 365;
const double PI = 3.14159265359;
const char NOUVELLE_LIGNE = '\n';

// JOURS_ANNEE = 366;  // ERREUR de compilation !
```

### Convention de nommage

Par convention, les constantes sont en **MAJUSCULES_AVEC_UNDERSCORES** :

```c
const int TAILLE_MAX = 100;
const float GRAVITE = 9.81f;
const char* MESSAGE_ERREUR = "Erreur critique";
```

### Avantages de `const`

1. **Sécurité** : Protection contre les modifications accidentelles
2. **Type safety** : Les constantes ont un type (contrairement à `#define`)
3. **Débogage** : Visible dans le débogueur avec son type
4. **Scope** : Respecte les règles de portée des variables

### Constantes et pointeurs

La position de `const` est importante :

```c
// Pointeur vers une constante (le contenu est constant)
const int* ptr1 = &x;
*ptr1 = 10;  // ERREUR : ne peut pas modifier *ptr1
ptr1 = &y;   // OK : peut changer vers où pointe ptr1

// Pointeur constant (le pointeur lui-même est constant)
int* const ptr2 = &x;
*ptr2 = 10;  // OK : peut modifier *ptr2
ptr2 = &y;   // ERREUR : ne peut pas changer ptr2

// Pointeur constant vers une constante (tout est constant)
const int* const ptr3 = &x;
*ptr3 = 10;  // ERREUR
ptr3 = &y;   // ERREUR
```

**Astuce mnémotechnique** : Lisez de droite à gauche.
- `const int* ptr` : "ptr est un pointeur vers un int constant"
- `int* const ptr` : "ptr est un pointeur constant vers un int"

## Les macros `#define`

Une **macro** est un remplacement textuel effectué par le préprocesseur **avant** la compilation.

### Syntaxe de base

```c
#define NOM valeur
```

**Exemples** :

```c
#include <stdio.h>

#define PI 3.14159265359
#define TAILLE_MAX 100
#define MESSAGE "Bienvenue"

int main(void) {
    printf("Pi vaut : %f\n", PI);
    printf("Taille maximale : %d\n", TAILLE_MAX);
    printf("%s\n", MESSAGE);

    return 0;
}
```

### `#define` vs `const` : Différences

| Critère | `#define` | `const` |
|---------|-----------|---------|
| **Type** | Aucun (remplacement textuel) | Typé |
| **Portée** | Globale (jusqu'au `#undef`) | Suit les règles C |
| **Débogage** | Non visible dans le débogueur | Visible avec son type |
| **Mémoire** | Pas d'emplacement mémoire | Occupe de la mémoire |
| **Sécurité** | Pas de vérification de type | Vérification de type |

**Exemple de différence** :

```c
#define TAILLE 10
const int LARGEUR = 10;

int tableau1[TAILLE];    // OK : macro remplacée par 10
int tableau2[LARGEUR];   // OK en C99+ (VLA), mais pas en C89
```

### Macros avec paramètres (aperçu)

Les macros peuvent avoir des paramètres (nous verrons ça en détail plus tard) :

```c
#define CARRE(x) ((x) * (x))
#define MAX(a, b) ((a) > (b) ? (a) : (b))

int resultat = CARRE(5);      // Devient : ((5) * (5))
int maximum = MAX(10, 20);    // Devient : ((10) > (20) ? (10) : (20))
```

⚠️ **Danger** : Les macros sont du remplacement textuel, attention aux effets de bord !

```c
#define CARRE(x) ((x) * (x))
int y = 5;
int z = CARRE(y++);  // Devient : ((y++) * (y++)) — y est incrémenté DEUX FOIS !
```

### Quand utiliser `#define` ?

✅ **Utilisez `#define` pour** :
- Compilation conditionnelle (`#ifdef`, `#ifndef`)
- Constantes de préprocesseur (gardes d'inclusion)
- Macros complexes (avec parcimonie)

✅ **Préférez `const` pour** :
- Constantes numériques et chaînes simples
- Tout ce qui bénéficie d'un type

## Les énumérations (`enum`)

Une **énumération** définit un ensemble de constantes entières nommées liées sémantiquement.

### Syntaxe de base

```c
enum nom {
    VALEUR1,
    VALEUR2,
    VALEUR3
};
```

Par défaut, les valeurs commencent à 0 et s'incrémentent :

```c
enum Couleur {
    ROUGE,   // 0
    VERT,    // 1
    BLEU     // 2
};

enum Couleur favori = ROUGE;
printf("Couleur favorite : %d\n", favori);  // Affiche : 0
```

### Valeurs explicites

Vous pouvez attribuer des valeurs spécifiques :

```c
enum Code_Erreur {
    SUCCES = 0,
    ERREUR_FICHIER = -1,
    ERREUR_MEMOIRE = -2,
    ERREUR_RESEAU = -3
};

enum Code_Erreur statut = SUCCES;
```

### Exemples pratiques

**États d'une machine** :

```c
enum Etat {
    ETEINT,
    EN_MARCHE,
    EN_PAUSE,
    EN_ERREUR
};

enum Etat etat_actuel = ETEINT;

if (etat_actuel == ETEINT) {
    printf("L'appareil est éteint\n");
}
```

**Jours de la semaine** :

```c
enum Jour {
    LUNDI = 1,
    MARDI,      // 2
    MERCREDI,   // 3
    JEUDI,      // 4
    VENDREDI,   // 5
    SAMEDI,     // 6
    DIMANCHE    // 7
};

enum Jour aujourd_hui = MERCREDI;
printf("Aujourd'hui : jour %d\n", aujourd_hui);  // Affiche : 3
```

**Permissions de fichiers** :

```c
enum Permission {
    LECTURE = 4,     // 0b100
    ECRITURE = 2,    // 0b010
    EXECUTION = 1    // 0b001
};

// Combinaison avec opérateurs bit-à-bit (nous verrons ça plus tard)
int permissions = LECTURE | ECRITURE;  // 6 (rw-)
```

### Avantages des énumérations

✅ **Lisibilité** : `etat == EN_MARCHE` est plus clair que `etat == 1`

✅ **Maintenance** : Changer les valeurs en un seul endroit

✅ **Auto-complétion** : Les IDE proposent les valeurs possibles

✅ **Type safety** (limité) : Le compilateur peut avertir en cas d'oubli dans un `switch`

### Énumérations avec `typedef`

Pour simplifier la syntaxe :

```c
typedef enum {
    ROUGE,
    VERT,
    BLEU
} Couleur;

// Utilisation simplifiée
Couleur favori = VERT;  // Au lieu de : enum Couleur favori = VERT;
```

## Constantes prédéfinies utiles

Le C et le préprocesseur fournissent des constantes prédéfinies :

### Macros du préprocesseur

| Macro | Signification |
|-------|---------------|
| `__FILE__` | Nom du fichier source actuel |
| `__LINE__` | Numéro de ligne actuel |
| `__DATE__` | Date de compilation ("Jan 15 2025") |
| `__TIME__` | Heure de compilation ("14:30:00") |
| `__func__` | Nom de la fonction actuelle (C99+) |

**Exemple d'utilisation** :

```c
#include <stdio.h>

void afficher_debug(void) {
    printf("Fichier : %s\n", __FILE__);
    printf("Ligne : %d\n", __LINE__);
    printf("Fonction : %s\n", __func__);
    printf("Compilé le : %s à %s\n", __DATE__, __TIME__);
}

int main(void) {
    afficher_debug();
    return 0;
}
```

### Limites des types (`<limits.h>`)

```c
#include <limits.h>

printf("INT_MIN : %d\n", INT_MIN);
printf("INT_MAX : %d\n", INT_MAX);
printf("UINT_MAX : %u\n", UINT_MAX);
printf("CHAR_BIT : %d bits\n", CHAR_BIT);
```

### Constantes mathématiques (`<math.h>`)

```c
#include <math.h>

printf("Pi : %f\n", M_PI);
printf("e : %f\n", M_E);
```

⚠️ **Note** : `M_PI` et `M_E` ne sont pas standards C, mais fournis par la plupart des compilateurs. Définissez-les vous-même si nécessaire :

```c
#ifndef M_PI
    #define M_PI 3.14159265358979323846
#endif
```

## Exemple complet récapitulatif

```c
#include <stdio.h>
#include <limits.h>

// Macros
#define VERSION "1.0.0"
#define TAILLE_BUFFER 256

// Constantes
const double PI = 3.14159265359;
const int SECONDES_PAR_MINUTE = 60;

// Énumération
typedef enum {
    LUNDI = 1,
    MARDI,
    MERCREDI,
    JEUDI,
    VENDREDI,
    SAMEDI,
    DIMANCHE
} Jour;

int main(void) {
    // Littéraux entiers (différentes bases)
    int decimal = 255;
    int hexa = 0xFF;
    int octal = 0377;

    printf("Décimal : %d\n", decimal);
    printf("Hexa : %d\n", hexa);
    printf("Octal : %d\n", octal);

    // Littéraux flottants
    double vitesse_lumiere = 3.0e8;
    float prix = 19.99f;

    printf("\nVitesse de la lumière : %.2e m/s\n", vitesse_lumiere);
    printf("Prix : %.2f €\n", prix);

    // Littéraux caractères
    char lettre = 'A';
    char newline = '\n';

    printf("\nLettre : %c (code ASCII : %d)\n", lettre, lettre);

    // Constantes
    printf("\nPi : %.10f\n", PI);
    printf("Version : %s\n", VERSION);

    // Énumération
    Jour aujourd_hui = MERCREDI;
    printf("\nAujourd'hui : jour %d de la semaine\n", aujourd_hui);

    // Limites système
    printf("\nINT_MAX : %d\n", INT_MAX);
    printf("UINT_MAX : %u\n", UINT_MAX);

    // Macros prédéfinies
    printf("\nCompilé le %s à %s\n", __DATE__, __TIME__);

    return 0;
}
```

## Bonnes pratiques

✅ **Utilisez `const` pour les constantes simples** (meilleures que `#define`)

✅ **Nommez les constantes en MAJUSCULES**

✅ **Utilisez `enum` pour des ensembles de valeurs liées**

✅ **Spécifiez les suffixes** pour les grands littéraux (`ULL`, `f`, etc.)

✅ **Évitez les "nombres magiques"** : remplacez-les par des constantes nommées

❌ **N'abusez pas de `#define`** pour des constantes simples

❌ **Attention aux octaux** : ne mettez pas de zéros inutiles (`0100` ≠ `100`)

## Points clés à retenir

🔹 **Littéral** : valeur écrite directement dans le code (`42`, `3.14`, `'A'`, `"Hello"`)

🔹 **Constante** : valeur qui ne change jamais (`const` ou `#define`)

🔹 **`const`** : constante typée, visible dans le débogueur, suit les règles de portée

🔹 **`#define`** : remplacement textuel par le préprocesseur, pas de type

🔹 **`enum`** : ensemble de constantes entières liées (états, options, etc.)

🔹 **Bases numériques** : décimal (10), hexa (`0x`), octal (`0`), binaire (`0b` en C23)

🔹 **Suffixes** : `U` (unsigned), `L` (long), `LL` (long long), `f` (float)

## Ce que nous verrons ensuite

Dans les prochaines sections, nous explorerons :
- **4.4 Conversion de types** : Comment passer d'un type à un autre (cast)
- **4.5 Endianness et portabilité** : L'ordre des octets en mémoire
- **4.6 Nouveautés C23** : Les dernières évolutions du standard

---

**🎯 Vous savez maintenant utiliser constantes et littéraux pour rendre votre code plus lisible, maintenable et sûr !**

⏭️ [Conversion de types](/04-types-et-memoire/04-conversion-types.md)
