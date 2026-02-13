🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 4.4 Conversion de types

## Introduction

En C, il arrive fréquemment qu'on doive mélanger différents types dans une même expression : additionner un `int` et un `float`, comparer un `unsigned` avec un `signed`, ou stocker un `double` dans un `int`. Ces situations nécessitent des **conversions de types**.

Le C effectue certaines conversions **automatiquement** (conversions implicites), mais vous pouvez aussi forcer une conversion **manuellement** (conversions explicites ou **cast**).

Cette section vous apprendra :
- Les différents types de conversions
- Les règles de promotion automatique
- Comment effectuer des conversions explicites (cast)
- Les pièges et pertes de données potentielles
- Les bonnes pratiques pour éviter les bugs

## Conversion implicite vs explicite

### Conversion implicite (automatique)

Le compilateur convertit automatiquement certains types sans que vous ayez à le demander :

```c
int entier = 10;  
double reel = entier;  // int → double (conversion implicite)  

printf("%f\n", reel);  // Affiche : 10.000000
```

**Avantage** : Pratique et transparent  
**Inconvénient** : Peut masquer des erreurs ou des pertes de données  

### Conversion explicite (cast)

Vous forcez manuellement une conversion en utilisant la syntaxe du **cast** :

```c
double pi = 3.14159;  
int pi_entier = (int)pi;  // Conversion explicite : double → int  

printf("%d\n", pi_entier);  // Affiche : 3 (partie décimale perdue)
```

**Syntaxe** : `(type_cible) expression`

**Avantage** : Clarté d'intention, contrôle explicite  
**Inconvénient** : Plus verbeux  

## Hiérarchie des types

Le C a une hiérarchie implicite des types pour les conversions :

```
long double
    ↑
  double
    ↑
  float
    ↑
unsigned long long
    ↑
  long long
    ↑
unsigned long
    ↑
   long
    ↑
unsigned int
    ↑
   int
    ↑
unsigned short
    ↑
  short
    ↑
unsigned char
    ↑
   char
```

**Règle générale** : Dans une expression, les types "inférieurs" sont promus vers les types "supérieurs".

## Promotion entière (Integer Promotion)

Les types plus petits que `int` sont **automatiquement promus** à `int` (ou `unsigned int` si nécessaire) lors de leur utilisation dans une expression.

### Règle de base

Tout `char`, `short`, `signed` ou `unsigned` devient `int` dans les opérations.

```c
char a = 10;  
char b = 20;  
char resultat = a + b;  // a et b sont d'abord promus en int, puis le résultat converti en char  
```

### Exemple détaillé

```c
unsigned char x = 200;  // Type : unsigned char  
unsigned char y = 100;  // Type : unsigned char  

// Dans l'opération suivante :
// 1. x est promu en int (200)
// 2. y est promu en int (100)
// 3. Addition : 200 + 100 = 300 (type int)
// 4. Résultat stocké dans un unsigned char (300 % 256 = 44)
unsigned char somme = x + y;

printf("%u\n", somme);  // Affiche : 44 (débordement)
```

## Conversions arithmétiques usuelles

Lorsque deux types différents sont utilisés dans une opération, le compilateur applique les **conversions arithmétiques usuelles** :

### Règles de conversion

1. Si un opérande est `long double`, l'autre devient `long double`
2. Sinon, si un opérande est `double`, l'autre devient `double`
3. Sinon, si un opérande est `float`, l'autre devient `float`
4. Sinon, les promotions entières sont appliquées, puis :
   - Si les deux sont signés ou les deux unsigned, le plus petit est converti vers le plus grand
   - Si l'un est unsigned et l'autre signed avec même taille : signed → unsigned
   - Sinon, règles complexes selon les tailles

### Exemples pratiques

**Entier + Flottant** :

```c
int a = 5;  
float b = 2.5f;  
float resultat = a + b;  // a est converti en float : 5.0f + 2.5f = 7.5f  

printf("%f\n", resultat);  // Affiche : 7.500000
```

**Signed + Unsigned** :

```c
int a = -1;  
unsigned int b = 1;  

// a est converti en unsigned int
// -1 devient 4294967295 (UINT_MAX)
if (a < b) {
    printf("a < b\n");
} else {
    printf("a >= b\n");  // C'est ce qui s'affiche !
}
```

⚠️ **Piège majeur** : Mélanger `signed` et `unsigned` peut produire des résultats inattendus !

## Conversion d'entier vers entier

### Élargissement (Widening)

Passer d'un type plus petit à un type plus grand est **sûr** :

```c
char petit = 100;  
int grand = petit;  // Conversion sûre : char → int  

printf("%d\n", grand);  // Affiche : 100
```

Pour les types signés, le **bit de signe est étendu** :

```c
char negatif = -50;  // 0b11001110 (représentation 8 bits)  
int etendu = negatif;  // 0b11111111111111111111111111001110 (32 bits)  

printf("%d\n", etendu);  // Affiche : -50
```

### Rétrécissement (Narrowing)

Passer d'un type plus grand à un type plus petit peut **perdre des données** :

```c
int grand = 300;  
char petit = grand;  // Conversion avec perte : int → char  

printf("%d\n", petit);  // Affiche : 44 (300 % 256 = 44)
```

**Explication** : Un `char` ne peut stocker que -128 à 127 (ou 0 à 255 en unsigned). Les bits de poids fort sont tronqués.

**Représentation binaire** :
```
300 en binaire (int) :   00000000 00000000 00000001 00101100
Tronqué à 8 bits (char): 00101100 = 44
```

### Conversion signed ↔ unsigned

**Signed → Unsigned** :

```c
int negatif = -1;  
unsigned int non_signe = negatif;  // Bit pattern préservé  

printf("%u\n", non_signe);  // Affiche : 4294967295 (UINT_MAX)
```

La représentation binaire est conservée, mais l'interprétation change.

**Unsigned → Signed** :

```c
unsigned int grand = 4000000000U;  // > INT_MAX (2147483647)  
int signe = grand;  // Comportement défini par l'implémentation  

printf("%d\n", signe);  // Peut afficher un nombre négatif
```

⚠️ Si la valeur unsigned dépasse `INT_MAX`, le résultat est **défini par l'implémentation** (généralement négatif).

## Conversion d'entier vers flottant

Convertir un entier en flottant est généralement sûr, mais peut perdre en **précision** pour les très grands entiers :

```c
int entier = 123456789;  
float reel = entier;  

printf("Entier : %d\n", entier);  
printf("Float : %.0f\n", reel);  
```

**Résultat possible** :
```
Entier : 123456789  
Float : 123456792  // Perte de précision !  
```

**Explication** : Un `float` a environ 7 chiffres de précision. Pour 9 chiffres, certains sont arrondis.

### Pourquoi cette perte ?

- `int` : 32 bits, toutes les valeurs exactes entre -2³¹ et 2³¹-1
- `float` : 32 bits, mais seulement ~23 bits de mantisse (précision limitée)

**Tableau des limites** :

| Type source | Type cible | Perte possible ? |
|-------------|------------|------------------|
| `int` | `float` | Oui (au-delà de ±16 777 216) |
| `int` | `double` | Non (double a 53 bits de mantisse) |
| `long long` | `float` | Oui |
| `long long` | `double` | Oui (au-delà de ±9 007 199 254 740 992) |

### Exemple de précision

```c
int exact = 16777216;     // 2^24 : dernière valeur exacte en float  
int inexact = 16777217;   // 2^24 + 1  

float f1 = exact;  
float f2 = inexact;  

printf("Exact : %.0f\n", f1);      // 16777216  
printf("Inexact : %.0f\n", f2);    // 16777216 (arrondi !)  
```

## Conversion de flottant vers entier

Convertir un flottant en entier **tronque** la partie décimale (pas d'arrondi) :

```c
double pi = 3.14159;  
int pi_tronque = pi;  // Conversion implicite  

printf("%d\n", pi_tronque);  // Affiche : 3
```

### Troncature vs Arrondi

```c
double x = 7.9;

int tronque = (int)x;              // Troncature : 7  
int arrondi = (int)(x + 0.5);      // Arrondi manuel : 8  

printf("Tronqué : %d\n", tronque);  
printf("Arrondi : %d\n", arrondi);  
```

Pour un arrondi correct (y compris pour les négatifs), utilisez `round()` de `<math.h>` :

```c
#include <math.h>

double valeur = 7.9;  
int arrondi = (int)round(valeur);  // 8  

double negatif = -7.9;  
int arrondi_neg = (int)round(negatif);  // -8  
```

### Débordement

Si le flottant est trop grand pour le type entier cible, le comportement est **indéfini** :

```c
double enorme = 1e100;  
int resultat = (int)enorme;  // Comportement indéfini !  
```

**Solution** : Vérifiez les limites avant la conversion.

```c
#include <limits.h>

double valeur = 3000000000.0;

if (valeur > INT_MAX || valeur < INT_MIN) {
    printf("Valeur hors limites pour int\n");
} else {
    int resultat = (int)valeur;
    printf("%d\n", resultat);
}
```

## Conversion explicite : le cast

### Syntaxe

```c
(type_cible) expression
```

### Quand utiliser un cast ?

1. **Forcer une division flottante** :

```c
int a = 5;  
int b = 2;  

// Division entière
int resultat1 = a / b;           // 2

// Division flottante (avec cast)
double resultat2 = (double)a / b;  // 2.5

printf("Entière : %d\n", resultat1);  
printf("Flottante : %f\n", resultat2);  
```

2. **Clarifier une conversion intentionnelle** :

```c
unsigned int x = 100;  
int y = (int)x;  // Cast explicite pour montrer l'intention  
```

3. **Conversion de pointeurs** (avancé, nous verrons ça plus tard) :

```c
void* ptr_generique = malloc(100);  
int* ptr_entier = (int*)ptr_generique;  // Cast optionnel en C (nécessaire en C++)  
```

> **Note** : En C, `void*` est implicitement convertible vers tout type pointeur. Le cast n'est donc pas nécessaire, mais peut améliorer la lisibilité. En C++, le cast est obligatoire.

4. **Éviter les avertissements du compilateur** :

```c
long long grand = 5000000000LL;  
int petit = (int)grand;  // Le cast montre que vous savez ce que vous faites  
```

### Attention aux cast inutiles

Ne castez pas systématiquement. Les conversions implicites sont souvent suffisantes :

```c
// Inutile
int x = (int)5;  // 5 est déjà un int

// Inutile
double y = (double)3.14;  // 3.14 est déjà un double
```

## Exemples de conversions courantes

### Exemple 1 : Calcul de moyenne

```c
int somme = 85;  
int nombre = 3;  

// ERREUR : division entière
double moyenne1 = somme / nombre;  // 28.0 (et non 28.333...)

// CORRECT : cast pour division flottante
double moyenne2 = (double)somme / nombre;  // 28.333333

printf("Moyenne incorrecte : %f\n", moyenne1);  
printf("Moyenne correcte : %f\n", moyenne2);  
```

### Exemple 2 : Pourcentage

```c
int reussis = 42;  
int total = 50;  

// Division flottante pour le pourcentage
double pourcentage = (double)reussis / total * 100.0;

printf("Taux de réussite : %.2f%%\n", pourcentage);  // 84.00%
```

### Exemple 3 : Conversion avec vérification

```c
#include <limits.h>

double valeur_flottante = 2500.75;

// Vérification avant conversion
if (valeur_flottante > INT_MAX || valeur_flottante < INT_MIN) {
    printf("Erreur : valeur hors limites\n");
} else {
    int valeur_entiere = (int)valeur_flottante;
    printf("Conversion réussie : %d\n", valeur_entiere);  // 2500
}
```

### Exemple 4 : Éviter le mélange signed/unsigned

```c
int compte = -5;  
unsigned int taille = 10;  

// DANGER : conversion implicite de compte en unsigned
if (compte < taille) {
    printf("compte < taille\n");  // N'est PAS affiché !
} else {
    printf("compte >= taille\n");  // Affiché (car -5 → très grand nombre)
}

// SOLUTION : conversion explicite
if (compte < (int)taille) {
    printf("Maintenant c'est correct\n");
}
```

## Pièges et erreurs courantes

### 1. Division entière inattendue

```c
int a = 5, b = 2;  
double resultat = a / b;  // Attention : division entière PUIS conversion !  

printf("%f\n", resultat);  // Affiche : 2.000000 (pas 2.5)
```

**Solution** :
```c
double resultat = (double)a / b;  // 2.500000
// OU
double resultat = a / (double)b;  // 2.500000
// OU
double resultat = (double)a / (double)b;  // 2.500000
```

### 2. Débordement silencieux

```c
int grand = 2147483647;  // INT_MAX  
int resultat = grand + 1;  // Débordement !  

printf("%d\n", resultat);  // Affiche : -2147483648 (comportement indéfini)
```

**Solution** : Utilisez un type plus grand ou vérifiez les limites.

```c
long long resultat = (long long)grand + 1;  // Correct
```

### 3. Perte de précision non détectée

```c
long long tres_grand = 9000000000000000000LL;  
float petit = tres_grand;  // Perte de précision massive  

printf("Original : %lld\n", tres_grand);  
printf("Converti : %.0f\n", petit);  // Différent !  
```

### 4. Confusion avec les pointeurs

```c
int x = 65;  
char c = x;  // Conversion int → char (OK)  

// vs

int* ptr = &x;
// char* ptr2 = ptr;  // ERREUR : types de pointeurs incompatibles !
char* ptr2 = (char*)ptr;  // Cast explicite nécessaire (dangereux)
```

## Tableau récapitulatif des conversions

| Conversion | Sûreté | Remarques |
|------------|--------|-----------|
| `char` → `int` | ✅ Sûr | Extension de signe |
| `int` → `char` | ⚠️ Risqué | Troncature si > 127 ou < -128 |
| `int` → `float` | ⚠️ Risqué | Perte de précision si > 16 777 216 |
| `int` → `double` | ✅ Sûr | Aucune perte |
| `float` → `int` | ⚠️ Risqué | Troncature + débordement possible |
| `double` → `int` | ⚠️ Risqué | Troncature + débordement possible |
| `signed` → `unsigned` | ⚠️ Risqué | Valeurs négatives deviennent grandes |
| `unsigned` → `signed` | ⚠️ Risqué | Débordement si > INT_MAX |
| `float` → `double` | ✅ Sûr | Aucune perte |
| `double` → `float` | ⚠️ Risqué | Perte de précision |

## Conversions avec `<stdint.h>`

Les types à largeur fixe rendent les conversions plus prévisibles :

```c
#include <stdint.h>

int32_t petit = 100;  
int64_t grand = petit;  // Extension sûre : 32 bits → 64 bits  

uint32_t non_signe = 4000000000U;  
int32_t signe = (int32_t)non_signe;  // Cast explicite (débordement)  

printf("Non signé : %u\n", non_signe);  
printf("Signé : %d\n", signe);  // Nombre négatif  
```

## Fonctions de conversion (`<stdlib.h>`)

Pour convertir des chaînes en nombres :

```c
#include <stdlib.h>

const char* chaine1 = "12345";  
const char* chaine2 = "3.14159";  

int entier = atoi(chaine1);           // String → int  
long long grand = atoll(chaine1);     // String → long long  
double reel = atof(chaine2);          // String → double  

printf("Entier : %d\n", entier);  
printf("Réel : %f\n", reel);  
```

**Fonctions plus robustes** (recommandées) :

```c
#include <stdlib.h>

const char* chaine = "42";  
char* fin;  

long valeur = strtol(chaine, &fin, 10);  // Base 10

if (*fin == '\0') {
    printf("Conversion réussie : %ld\n", valeur);
} else {
    printf("Erreur de conversion\n");
}
```

## Bonnes pratiques

✅ **Préférez les conversions explicites** (cast) pour la clarté

✅ **Utilisez `double` par défaut** pour les calculs flottants

✅ **Castez au moins un opérande** dans les divisions si vous voulez un résultat flottant

✅ **Évitez de mélanger `signed` et `unsigned`** dans les comparaisons

✅ **Vérifiez les limites** avant les conversions rétrécissantes

✅ **Utilisez les types à largeur fixe** (`int32_t`) pour la portabilité

✅ **Commentez les conversions non évidentes**

❌ **N'ignorez pas les avertissements** du compilateur sur les conversions

❌ **Ne supposez pas qu'une conversion est toujours sûre**

❌ **N'abusez pas des cast** — ils peuvent masquer des bugs

## Exemple complet et pédagogique

```c
#include <stdio.h>
#include <limits.h>
#include <math.h>

int main(void) {
    printf("=== Conversions de types ===\n\n");

    // 1. Division entière vs flottante
    int a = 7, b = 2;
    printf("1. Division\n");
    printf("   Entière : %d / %d = %d\n", a, b, a / b);
    printf("   Flottante : %d / %d = %.2f\n\n", a, b, (double)a / b);

    // 2. Promotion automatique
    char petit = 100;
    int grand = petit;
    printf("2. Promotion char → int\n");
    printf("   char %d → int %d\n\n", petit, grand);

    // 3. Troncature
    double pi = 3.14159;
    int pi_tronque = (int)pi;
    int pi_arrondi = (int)round(pi);
    printf("3. Troncature double → int\n");
    printf("   Original : %.5f\n", pi);
    printf("   Tronqué : %d\n", pi_tronque);
    printf("   Arrondi : %d\n\n", pi_arrondi);

    // 4. Perte de précision
    int grand_entier = 123456789;
    float flottant = grand_entier;
    printf("4. Perte de précision int → float\n");
    printf("   Original (int) : %d\n", grand_entier);
    printf("   Converti (float) : %.0f\n\n", flottant);

    // 5. Signed vs Unsigned
    int negatif = -1;
    unsigned int positif = negatif;
    printf("5. Conversion signed → unsigned\n");
    printf("   Signé : %d\n", negatif);
    printf("   Non signé : %u\n\n", positif);

    // 6. Vérification des limites
    double valeur = 3000000000.0;
    printf("6. Vérification des limites\n");
    if (valeur > INT_MAX) {
        printf("   %.0f dépasse INT_MAX (%d)\n", valeur, INT_MAX);
        printf("   Conversion en int non recommandée\n\n");
    }

    // 7. Calcul de pourcentage
    int reussis = 42;
    int total = 50;
    double pourcentage = (double)reussis / total * 100.0;
    printf("7. Calcul de pourcentage\n");
    printf("   %d / %d = %.2f%%\n", reussis, total, pourcentage);

    return 0;
}
```

**Sortie** :
```
=== Conversions de types ===

1. Division
   Entière : 7 / 2 = 3
   Flottante : 7 / 2 = 3.50

2. Promotion char → int
   char 100 → int 100

3. Troncature double → int
   Original : 3.14159
   Tronqué : 3
   Arrondi : 3

4. Perte de précision int → float
   Original (int) : 123456789
   Converti (float) : 123456792

5. Conversion signed → unsigned
   Signé : -1
   Non signé : 4294967295

6. Vérification des limites
   3000000000 dépasse INT_MAX (2147483647)
   Conversion en int non recommandée

7. Calcul de pourcentage
   42 / 50 = 84.00%
```

## Points clés à retenir

🔹 **Conversion implicite** : automatique, peut être dangereuse

🔹 **Conversion explicite (cast)** : `(type)expression`, montre l'intention

🔹 **Hiérarchie des types** : char < short < int < long < float < double

🔹 **Division entière** : `5 / 2 = 2`, pas `2.5` — castez si besoin

🔹 **Troncature** : double → int perd la partie décimale

🔹 **Perte de précision** : int → float peut arrondir les grandes valeurs

🔹 **Signed ↔ Unsigned** : dangereux, peut donner des résultats surprenants

🔹 **Vérifiez les limites** avant les conversions rétrécissantes

## Ce que nous verrons ensuite

Dans la prochaine section, nous explorerons :
- **4.5 Endianness et portabilité** : L'ordre des octets en mémoire et son impact sur les conversions
- **4.6 Nouveautés C23** : Les dernières évolutions du standard

---

**🎯 Vous maîtrisez maintenant les conversions de types ! Utilisez-les avec précaution et vérifiez toujours les limites pour éviter les bugs subtils.**

⏭️ [Endianness et portabilité](/04-types-et-memoire/05-endianness.md)
