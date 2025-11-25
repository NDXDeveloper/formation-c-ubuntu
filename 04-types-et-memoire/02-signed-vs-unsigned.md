🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 4.2 Signed vs Unsigned

## Introduction

Dans la section précédente, nous avons vu que le type `int` peut stocker des valeurs d'environ -2 milliards à +2 milliards. Mais que se passe-t-il si vous n'avez besoin que de nombres **positifs** ? C'est là qu'intervient la distinction entre types **signés** (signed) et **non signés** (unsigned).

Cette distinction est fondamentale en C, car elle double la plage positive des types entiers tout en sacrifiant les valeurs négatives.

## Qu'est-ce qu'un type signé (signed) ?

Un type **signé** peut représenter des nombres **positifs et négatifs**.

Par défaut, tous les types entiers en C sont signés :

```c
int temperature = -15;      // OK : int est signé par défaut
short altitude = -200;      // OK : short est signé par défaut
char lettre = 'A';          // char peut être signé ou non selon le compilateur
```

Le bit de poids fort (le bit le plus à gauche) sert de **bit de signe** :
- `0` = nombre positif
- `1` = nombre négatif

### Représentation en mémoire (complément à deux)

Le C utilise la méthode du **complément à deux** pour représenter les nombres négatifs.

**Exemple avec un `char` signé (8 bits)** :

| Valeur décimale | Représentation binaire | Explication |
|-----------------|------------------------|-------------|
| 0 | `0000 0000` | Zéro |
| 1 | `0000 0001` | Un positif |
| 127 | `0111 1111` | Maximum positif |
| -1 | `1111 1111` | Complément à deux de 1 |
| -2 | `1111 1110` | Complément à deux de 2 |
| -128 | `1000 0000` | Minimum négatif |

**Plage d'un `char` signé** : -128 à +127 (256 valeurs au total)

> **Note** : Le complément à deux simplifie les opérations arithmétiques en électronique. Vous n'avez pas besoin de comprendre les détails mathématiques pour utiliser le C, mais sachez que c'est la méthode standard.

## Qu'est-ce qu'un type non signé (unsigned) ?

Un type **non signé** ne peut représenter que des nombres **positifs ou nuls**.

On utilise le mot-clé `unsigned` devant le type :

```c
unsigned int population = 67000000;
unsigned short port = 8080;
unsigned char octet = 255;
```

Tous les bits servent à représenter la magnitude du nombre (pas de bit de signe).

**Exemple avec un `unsigned char` (8 bits)** :

| Valeur décimale | Représentation binaire | Explication |
|-----------------|------------------------|-------------|
| 0 | `0000 0000` | Zéro |
| 1 | `0000 0001` | Un |
| 127 | `0111 1111` | Ancien maximum signé |
| 128 | `1000 0000` | Maintenant positif ! |
| 255 | `1111 1111` | Maximum |

**Plage d'un `unsigned char`** : 0 à 255 (256 valeurs au total)

## Comparaison des plages

Voici l'impact du modificateur `unsigned` sur les types courants :

| Type | Taille | Plage (signed) | Plage (unsigned) |
|------|--------|----------------|------------------|
| `char` / `unsigned char` | 1 octet | -128 à 127 | 0 à 255 |
| `short` / `unsigned short` | 2 octets | -32 768 à 32 767 | 0 à 65 535 |
| `int` / `unsigned int` | 4 octets | -2 147 483 648 à 2 147 483 647 | 0 à 4 294 967 295 |
| `long long` / `unsigned long long` | 8 octets | ±9.2 × 10¹⁸ | 0 à 1.8 × 10¹⁹ |

**Observation clé** : Le type `unsigned` **double** la valeur positive maximale, mais ne peut plus représenter de nombres négatifs.

## Déclarations et syntaxe

Il existe plusieurs façons d'écrire les types non signés :

```c
// Forme complète (recommandée pour la lisibilité)
unsigned int compteur = 1000;
unsigned short port = 443;
unsigned long long id = 123456789012345ULL;

// Forme raccourcie (équivalente)
unsigned compteur2 = 1000;  // "int" est implicite

// Avec les types à largeur fixe (C99+)
#include <stdint.h>
uint8_t  octet = 200;        // unsigned 8 bits
uint16_t port2 = 8080;       // unsigned 16 bits
uint32_t timestamp = 1704067200U;  // unsigned 32 bits
uint64_t grande_valeur = 18446744073709551615ULL;  // unsigned 64 bits
```

**Suffixes de littéraux** :
- `U` ou `u` : indique un littéral `unsigned int`
- `UL` : `unsigned long`
- `ULL` : `unsigned long long`

## Quand utiliser `unsigned` ?

### ✅ Utilisez `unsigned` quand :

1. **Les valeurs sont toujours positives par nature**
   ```c
   unsigned int age = 25;               // Un âge ne peut pas être négatif
   unsigned short annee = 2025;         // Une année non plus
   unsigned long long taille_fichier = 5000000000ULL;  // Taille en octets
   ```

2. **Vous manipulez des bits ou des masques**
   ```c
   unsigned int flags = 0x00FF;         // Masques de bits
   unsigned char pixel = 0xFF;          // Valeur RGB (0-255)
   ```

3. **Vous avez besoin de la plage étendue**
   ```c
   unsigned int compteur = 3000000000U; // Dépasse les 2 milliards d'un int signé
   ```

4. **Interfaçage avec du matériel ou des protocoles**
   ```c
   uint32_t adresse_ip = 0xC0A80001;    // 192.168.0.1 en hexadécimal
   uint16_t port_reseau = 80;
   ```

### ❌ Évitez `unsigned` quand :

1. **Vous faites des soustractions pouvant être négatives**
   ```c
   // DANGER : comportement inattendu
   unsigned int a = 5;
   unsigned int b = 10;
   unsigned int resultat = a - b;  // Résultat : 4294967291 (underflow !)
   ```

2. **Vous utilisez des boucles décroissantes**
   ```c
   // ERREUR : boucle infinie !
   for (unsigned int i = 10; i >= 0; i--) {
       printf("%u\n", i);
       // i ne sera jamais < 0 car unsigned !
   }
   ```

3. **Par défaut, sans raison spécifique**
   - Utilisez `int` comme type par défaut pour les entiers
   - N'ajoutez `unsigned` que si vous en avez vraiment besoin

## Pièges courants avec les types non signés

### 1. L'underflow (débordement par le bas)

```c
unsigned int x = 5;
unsigned int y = 10;
unsigned int diff = x - y;

printf("Résultat : %u\n", diff);  // Affiche : 4294967291 (pas -5 !)
```

**Explication** : En unsigned, -5 "boucle" au maximum (`UINT_MAX + 1 - 5`).

**Solution** : Utilisez des types signés si vous pouvez avoir des résultats négatifs.

### 2. Comparaisons avec des signés

```c
int a = -1;
unsigned int b = 1;

if (a < b) {
    printf("a est plus petit\n");
} else {
    printf("a est plus grand\n");  // C'est ici qu'on arrive !
}
```

**Explication** : Le `-1` signé est converti en `unsigned`, devenant `4294967295`.

**Solution** : Évitez de mélanger signed et unsigned dans les comparaisons.

### 3. Boucles décroissantes infinies

```c
// ERREUR : boucle infinie
for (unsigned int i = 10; i >= 0; i--) {
    printf("%u\n", i);
}
// i passe de 0 à 4294967295 au lieu de devenir -1
```

**Solution** :
```c
// Version correcte
for (unsigned int i = 10; i > 0; i--) {
    printf("%u\n", i);
}
printf("0\n");

// Ou utilisez un int signé
for (int i = 10; i >= 0; i--) {
    printf("%d\n", i);
}
```

### 4. Différence de taille entre éléments

```c
size_t taille = strlen(chaine);  // size_t est unsigned

// DANGER si taille < 5
for (size_t i = 0; i < taille - 5; i++) {
    // Si taille = 3, alors taille - 5 = très grand nombre !
}
```

**Solution** : Vérifiez les bornes avant la soustraction.

```c
if (taille >= 5) {
    for (size_t i = 0; i < taille - 5; i++) {
        // Maintenant c'est sûr
    }
}
```

## Affichage avec `printf`

Utilisez les bons spécificateurs de format :

| Type | Spécificateur | Exemple |
|------|---------------|---------|
| `int` | `%d` | `printf("%d", x);` |
| `unsigned int` | `%u` | `printf("%u", x);` |
| `long` | `%ld` | `printf("%ld", x);` |
| `unsigned long` | `%lu` | `printf("%lu", x);` |
| `long long` | `%lld` | `printf("%lld", x);` |
| `unsigned long long` | `%llu` | `printf("%llu", x);` |
| En hexadécimal | `%x` ou `%X` | `printf("0x%X", x);` |

```c
#include <stdio.h>

int main(void) {
    int signe = -42;
    unsigned int non_signe = 42;

    printf("Signé : %d\n", signe);              // Signé : -42
    printf("Non signé : %u\n", non_signe);      // Non signé : 42

    // Attention : mauvais spécificateur
    printf("Signé avec %%u : %u\n", signe);     // Affiche un grand nombre !

    // Hexadécimal
    printf("Hexa : 0x%X\n", non_signe);         // Hexa : 0x2A

    return 0;
}
```

## Le cas particulier de `char`

Le type `char` est **spécial** : le standard C laisse le choix au compilateur de le rendre signé ou non signé par défaut.

```c
char c = 200;  // Peut être -56 (signé) ou 200 (non signé) selon le compilateur !
```

**Recommandation** : Soyez explicite si cela importe :
```c
signed char temperature = -10;     // Garanti signé
unsigned char pixel = 200;         // Garanti non signé
```

**Cas d'usage** :
- `char` : pour les chaînes de caractères (texte)
- `signed char` : pour de petits entiers signés (-128 à 127)
- `unsigned char` : pour des octets bruts (0 à 255), pixels, données binaires

## Types à largeur fixe : la solution moderne

Les types `<stdint.h>` éliminent l'ambiguïté :

```c
#include <stdint.h>

int8_t   temperature = -10;      // Signé 8 bits : -128 à 127
uint8_t  pixel = 255;            // Non signé 8 bits : 0 à 255

int32_t  balance = -50000;       // Signé 32 bits
uint32_t timestamp = 1704067200U; // Non signé 32 bits

int64_t  grand_negatif = -9000000000000000000LL;
uint64_t grand_positif = 18000000000000000000ULL;
```

**Avantages** :
- La taille est **garantie** sur toutes les plateformes
- Le signe est **explicite** dans le nom du type
- Code plus lisible et portable

## Exemple complet et comparatif

```c
#include <stdio.h>
#include <stdint.h>

int main(void) {
    // Types classiques
    int negatif = -100;
    unsigned int positif = 100;

    // Underflow demonstration
    unsigned int x = 5;
    unsigned int y = 10;
    unsigned int resultat = x - y;

    // Types à largeur fixe
    int32_t signe32 = -2000000000;
    uint32_t non_signe32 = 4000000000U;

    printf("=== Types classiques ===\n");
    printf("Négatif (int) : %d\n", negatif);
    printf("Positif (unsigned int) : %u\n", positif);

    printf("\n=== Underflow ===\n");
    printf("5 - 10 en unsigned : %u\n", resultat);
    printf("(devrait être -5, mais unsigned boucle)\n");

    printf("\n=== Types à largeur fixe ===\n");
    printf("int32_t : %d\n", signe32);
    printf("uint32_t : %u\n", non_signe32);

    printf("\n=== Limites ===\n");
    printf("INT_MAX : %d\n", INT_MAX);
    printf("UINT_MAX : %u\n", UINT_MAX);

    return 0;
}
```

**Sortie** :
```
=== Types classiques ===
Négatif (int) : -100
Positif (unsigned int) : 100

=== Underflow ===
5 - 10 en unsigned : 4294967291
(devrait être -5, mais unsigned boucle)

=== Types à largeur fixe ===
int32_t : -2000000000
uint32_t : 4000000000

=== Limites ===
INT_MAX : 2147483647
UINT_MAX : 4294967295
```

## Macros de limites (`<limits.h>`)

Le header `<limits.h>` définit les valeurs minimales et maximales :

```c
#include <limits.h>
#include <stdio.h>

int main(void) {
    printf("CHAR_MIN : %d\n", CHAR_MIN);
    printf("CHAR_MAX : %d\n", CHAR_MAX);
    printf("UCHAR_MAX : %u\n", UCHAR_MAX);

    printf("SHRT_MIN : %d\n", SHRT_MIN);
    printf("SHRT_MAX : %d\n", SHRT_MAX);
    printf("USHRT_MAX : %u\n", USHRT_MAX);

    printf("INT_MIN : %d\n", INT_MIN);
    printf("INT_MAX : %d\n", INT_MAX);
    printf("UINT_MAX : %u\n", UINT_MAX);

    return 0;
}
```

Ces macros sont utiles pour :
- Valider des entrées utilisateur
- Détecter les débordements potentiels
- Écrire du code portable

## Bonnes pratiques

✅ **Utilisez `int` par défaut** pour les entiers ordinaires

✅ **Ajoutez `unsigned` seulement si nécessaire** (compteurs, bits, tailles)

✅ **Préférez les types à largeur fixe** (`uint32_t`) pour la portabilité

✅ **Évitez de mélanger signed et unsigned** dans les comparaisons

✅ **Attention aux soustractions** avec `unsigned` (risque d'underflow)

✅ **Utilisez les bons spécificateurs** avec `printf` (`%d` vs `%u`)

❌ **N'utilisez jamais `unsigned` juste "pour être sûr"** sans raison valable

❌ **Ne faites pas de boucles décroissantes** avec `unsigned` naïvement

## Points clés à retenir

🔹 **Signed (signé)** : nombres positifs **et** négatifs (bit de signe)

🔹 **Unsigned (non signé)** : **seulement** positifs (double la plage positive)

🔹 **Underflow** : `unsigned_var - x` peut donner un très grand nombre si le résultat serait négatif

🔹 **Mélanger signed/unsigned** dans des comparaisons produit des résultats inattendus

🔹 **Types `<stdint.h>`** : solution moderne et portable (`int32_t`, `uint64_t`)

🔹 **`char` est ambigu** : utilisez `signed char` ou `unsigned char` si le signe importe

## Ce que nous verrons ensuite

Dans les prochaines sections, nous explorerons :
- **4.3 Les constantes et les littéraux** : Définir des valeurs immuables
- **4.4 Conversion de types** : Comment convertir entre signed/unsigned et autres types
- **4.5 Endianness et portabilité** : L'ordre des octets en mémoire

---

**🎯 Vous savez maintenant choisir entre types signés et non signés ! Cette distinction est cruciale pour éviter des bugs subtils liés aux débordements et comparaisons.**

⏭️ [Les constantes et les littéraux](/04-types-et-memoire/03-constantes-litteraux.md)
