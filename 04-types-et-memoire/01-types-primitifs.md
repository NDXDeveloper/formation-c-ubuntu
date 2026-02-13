🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 4.1 Types primitifs et leur taille

## Introduction

En C, chaque donnée que vous manipulez doit avoir un **type**. Le type indique au compilateur comment interpréter les données en mémoire et combien d'espace elles occupent. Contrairement à des langages comme Python ou JavaScript, C est un langage **fortement typé** : vous devez déclarer explicitement le type de chaque variable.

Cette rigueur offre deux avantages majeurs :
- **Performance** : le compilateur peut optimiser le code car il connaît la taille exacte de chaque donnée
- **Sécurité** : les erreurs de type sont détectées à la compilation, pas à l'exécution

## Les types primitifs fondamentaux

Le C propose plusieurs types de base, appelés **types primitifs**. Voici les principaux :

### 1. Les entiers (`int`)

Le type `int` représente un nombre entier signé (positif ou négatif).

```c
int age = 25;  
int temperature = -10;  
```

**Taille typique** : 4 octets (32 bits) sur la plupart des systèmes modernes (Linux, Windows 64 bits)

**Plage de valeurs** : environ -2 147 483 648 à 2 147 483 647

### 2. Les caractères (`char`)

Le type `char` stocke un caractère unique ou un petit nombre entier.

```c
char lettre = 'A';  
char chiffre = '7';  
```

**Taille** : 1 octet (8 bits) — c'est le plus petit type en C

**Plage de valeurs** : -128 à 127 (signé) ou 0 à 255 (non signé)

> **Note** : En C, les caractères sont en réalité des nombres entiers correspondant à leur code ASCII. `'A'` vaut 65, `'0'` vaut 48, etc.

### 3. Les nombres à virgule flottante (`float`)

Le type `float` représente un nombre décimal en simple précision.

```c
float prix = 19.99;  
float pi = 3.14159;  
```

**Taille** : 4 octets (32 bits)

**Précision** : environ 6-7 chiffres significatifs

**Plage approximative** : ±3.4 × 10³⁸

### 4. Les nombres à virgule flottante double précision (`double`)

Le type `double` offre une précision accrue pour les calculs décimaux.

```c
double distance = 384400.5;  // Distance Terre-Lune en km  
double constante = 2.718281828459045;  
```

**Taille** : 8 octets (64 bits)

**Précision** : environ 15-16 chiffres significatifs

**Plage approximative** : ±1.7 × 10³⁰⁸

> **Conseil** : Utilisez `double` par défaut pour les calculs scientifiques ou financiers. Le `float` est surtout utile pour économiser de la mémoire dans les tableaux très volumineux.

## Les variantes des types entiers

Le C offre plusieurs modificateurs pour ajuster la taille et la plage des entiers.

### Les modificateurs de taille

| Type | Taille typique | Plage de valeurs (signée) |
|------|----------------|---------------------------|
| `short` ou `short int` | 2 octets | -32 768 à 32 767 |
| `int` | 4 octets | -2 147 483 648 à 2 147 483 647 |
| `long` ou `long int` | 4 ou 8 octets* | Dépend du système |
| `long long` | 8 octets | -9 223 372 036 854 775 808 à 9 223 372 036 854 775 807 |

*Sur Linux 64 bits, `long` fait 8 octets. Sur Windows 64 bits, il fait 4 octets.

Exemples :

```c
short compteur = 100;  
int population = 67000000;  
long long distance_etoile = 9460730472580800LL;  // 1 année-lumière en mètres  
```

> **Note** : Le suffixe `LL` indique un littéral `long long`.

### Le type `long double`

Pour une précision encore supérieure :

```c
long double precision_extreme = 3.141592653589793238L;
```

**Taille** : 8, 12 ou 16 octets selon l'architecture

**Précision** : jusqu'à 18-19 chiffres significatifs

## Vérifier la taille des types avec `sizeof`

L'opérateur `sizeof` renvoie la taille d'un type en octets :

```c
#include <stdio.h>

int main(void) {
    printf("Taille de char : %zu octets\n", sizeof(char));
    printf("Taille de int : %zu octets\n", sizeof(int));
    printf("Taille de float : %zu octets\n", sizeof(float));
    printf("Taille de double : %zu octets\n", sizeof(double));
    printf("Taille de long long : %zu octets\n", sizeof(long long));

    return 0;
}
```

**Résultat typique sur Ubuntu 64 bits** :
```
Taille de char : 1 octets  
Taille de int : 4 octets  
Taille de float : 4 octets  
Taille de double : 8 octets  
Taille de long long : 8 octets  
```

> **Important** : Le spécificateur de format `%zu` est prévu pour afficher des valeurs de type `size_t` (le type de retour de `sizeof`).

## Tableau récapitulatif complet

| Type | Taille (octets) | Plage typique | Usage recommandé |
|------|-----------------|---------------|------------------|
| `char` | 1 | -128 à 127 | Caractères, petits entiers |
| `unsigned char` | 1 | 0 à 255 | Octets bruts, pixels |
| `short` | 2 | -32 768 à 32 767 | Entiers modérés |
| `unsigned short` | 2 | 0 à 65 535 | Compteurs positifs |
| `int` | 4 | ±2 milliards | Type entier par défaut |
| `unsigned int` | 4 | 0 à 4 milliards | IDs, flags |
| `long` | 4 ou 8* | Variable | Éviter (portabilité) |
| `long long` | 8 | ±9 × 10¹⁸ | Très grands entiers |
| `float` | 4 | ±3.4 × 10³⁸ | Calculs simples, graphisme |
| `double` | 8 | ±1.7 × 10³⁰⁸ | Calculs scientifiques/financiers |
| `long double` | 8-16 | Variable | Précision extrême (rare) |

*Attention à la portabilité : préférez `int32_t`, `int64_t` (voir section suivante).

## Pourquoi les tailles sont-elles "typiques" ?

La norme C **ne garantit pas** des tailles fixes pour tous les types, seulement des minimums :
- `char` : au moins 8 bits
- `short` : au moins 16 bits
- `int` : au moins 16 bits
- `long` : au moins 32 bits
- `long long` : au moins 64 bits

En pratique, sur les systèmes modernes (Linux, Windows, macOS), les tailles sont standardisées comme indiqué ci-dessus. Mais pour du code portable nécessitant des tailles exactes, on utilise des types à largeur fixe.

## Types à largeur fixe (C99 et supérieur)

Depuis C99, le header `<stdint.h>` fournit des types garantissant une taille précise :

```c
#include <stdint.h>

int8_t  petit = -100;        // Exactement 8 bits (1 octet)  
int16_t moyen = 30000;       // Exactement 16 bits (2 octets)  
int32_t grand = 2000000000;  // Exactement 32 bits (4 octets)  
int64_t enorme = 9000000000000000000LL;  // Exactement 64 bits (8 octets)  

uint8_t  octet = 255;        // Non signé 8 bits  
uint32_t compteur = 4000000000U;  // Non signé 32 bits  
```

**Avantages** :
- Portabilité garantie
- Code plus lisible (la taille est explicite)
- Essentiels pour les protocoles réseau, formats de fichiers binaires

**Quand les utiliser** :
- Communication avec du matériel
- Sérialisation de données
- Interopérabilité entre systèmes

## Exemples pratiques de déclaration

```c
#include <stdio.h>
#include <stdint.h>

int main(void) {
    // Types classiques
    char initiale = 'J';
    int age = 30;
    float taille = 1.75;
    double salaire = 45000.50;

    // Types à largeur fixe
    uint32_t timestamp = 1704067200;  // Epoch Unix
    int64_t population_mondiale = 8000000000LL;

    // Affichage
    printf("Initiale : %c\n", initiale);
    printf("Age : %d ans\n", age);
    printf("Taille : %.2f m\n", taille);
    printf("Salaire : %.2f €\n", salaire);
    printf("Timestamp : %u\n", (unsigned)timestamp);
    printf("Population : %lld personnes\n", (long long)population_mondiale);

    return 0;
}
```

**Sortie** :
```
Initiale : J  
Age : 30 ans  
Taille : 1.75 m  
Salaire : 45000.50 €  
Timestamp : 1704067200  
Population : 8000000000 personnes  
```

## Spécificateurs de format `printf` essentiels

Lors de l'affichage avec `printf`, utilisez le bon spécificateur :

| Type | Spécificateur | Exemple |
|------|---------------|---------|
| `int` | `%d` | `printf("%d", x);` |
| `unsigned int` | `%u` | `printf("%u", x);` |
| `long` | `%ld` | `printf("%ld", x);` |
| `long long` | `%lld` | `printf("%lld", x);` |
| `float` / `double` | `%f` | `printf("%.2f", x);` |
| `char` | `%c` | `printf("%c", x);` |
| `size_t` | `%zu` | `printf("%zu", sizeof(x));` |

> **Attention** : Utiliser le mauvais spécificateur (ex: `%d` pour un `long long`) provoque un **comportement indéfini**.

## Points clés à retenir

✅ **Toujours déclarer le type** : `int nombre;` et non `nombre = 5;`

✅ **`sizeof` est votre ami** : Vérifiez les tailles sur votre système

✅ **Préférez `double` à `float`** sauf contrainte mémoire

✅ **Utilisez les types à largeur fixe** (`int32_t`, `uint64_t`) pour la portabilité

✅ **Attention aux limites** : Un `int` ne peut pas stocker 5 milliards

❌ **Ne pas mélanger les types** sans conversion explicite (nous verrons ça plus tard)

## Ce que nous verrons ensuite

Dans les prochaines sections, nous explorerons :
- **4.2 Signed vs Unsigned** : La différence entre nombres signés et non signés
- **4.3 Les constantes et les littéraux** : Définir des valeurs immuables
- **4.4 Conversion de types** : Comment et quand convertir entre types
- **4.5 Endianness et portabilité** : L'ordre des octets en mémoire

---

**🎯 Maintenant que vous connaissez les types primitifs, vous pouvez commencer à manipuler des données en C ! La suite du module vous apprendra à les combiner et les convertir efficacement.**

⏭️ [Signed vs Unsigned](/04-types-et-memoire/02-signed-vs-unsigned.md)
