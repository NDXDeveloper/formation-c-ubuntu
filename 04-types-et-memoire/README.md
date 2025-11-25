🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 4. Types de données et Représentation mémoire

## Introduction au Module

Bienvenue dans le **Module 4**, l'un des chapitres les plus fondamentaux de votre apprentissage du C ! Si les modules précédents vous ont familiarisé avec l'écosystème Linux et la chaîne de compilation, ce module va vous plonger au cœur du langage : **comment le C représente et manipule les données en mémoire**.

Le C est souvent qualifié de langage de **"bas niveau"**, non pas parce qu'il serait inférieur, mais parce qu'il vous donne un **contrôle direct** sur la mémoire et les ressources matérielles. Cette caractéristique, qui fait la force du C, nécessite une compréhension approfondie de la façon dont les données sont stockées et représentées.

## Pourquoi ce module est crucial ?

### 1. Le C ne cache rien

Contrairement à des langages comme Python ou JavaScript, le C ne fait pas de "magie" en arrière-plan. Quand vous déclarez une variable, vous devez **explicitement** :
- Choisir son **type** (entier, flottant, caractère...)
- Comprendre combien de **mémoire** elle occupe
- Savoir si elle peut stocker des **valeurs négatives** ou non
- Anticiper les **limites** et les **débordements** possibles

Cette transparence vous donne un contrôle total, mais aussi une responsabilité.

### 2. La mémoire est une ressource limitée

En C, la mémoire n'est pas "infinie" comme on pourrait le croire avec des langages modernes. Chaque variable occupe un espace précis :
- Un `char` : 1 octet
- Un `int` : généralement 4 octets
- Un `double` : 8 octets

Comprendre ces tailles est essentiel pour :
- **Optimiser** l'utilisation de la mémoire (crucial en embarqué)
- **Éviter les débordements** qui causent des bugs
- **Communiquer** avec du matériel ou d'autres systèmes

### 3. Les bugs liés aux types sont subtils

Les erreurs de types sont parmi les plus difficiles à détecter :
- Mélanger des types signés et non signés peut produire des résultats inattendus
- Convertir un flottant en entier perd la partie décimale
- Un débordement d'entier peut transformer un nombre positif en négatif

**Exemple concret** :

```c
int compteur = 2147483647;  // INT_MAX
compteur = compteur + 1;     // Que se passe-t-il ?
// Résultat : -2147483648 (débordement !)
```

Ce type de bug peut être **catastrophique** dans un système critique (aérospatial, médical, financier).

### 4. La portabilité entre systèmes

Le code C peut tourner sur :
- Des ordinateurs personnels (x86, ARM)
- Des serveurs (x86-64, PowerPC)
- Des microcontrôleurs embarqués (ARM Cortex, RISC-V)
- Des superordinateurs

Chaque architecture peut représenter les données différemment. Par exemple, l'ordre des octets en mémoire (endianness) varie selon le processeur. Comprendre ces différences est crucial pour écrire du **code portable**.

## Vue d'ensemble du module

Ce module est structuré en **6 sections** progressives :

### Section 4.1 : Types primitifs et leur taille
*Les briques de base du C*

Vous découvrirez :
- Les types fondamentaux : `int`, `char`, `float`, `double`
- Les variantes : `short`, `long`, `long long`
- Comment vérifier la taille des types avec `sizeof`
- Les types à largeur fixe (`int32_t`, `uint64_t`) pour la portabilité

**Pourquoi c'est important** : Choisir le bon type évite les débordements et optimise la mémoire.

### Section 4.2 : Signed vs Unsigned
*Positif, négatif, ou seulement positif ?*

Vous apprendrez :
- La différence entre types signés et non signés
- Comment les nombres négatifs sont représentés en mémoire (complément à deux)
- Les pièges du mélange signed/unsigned
- Quand utiliser `unsigned` (et quand l'éviter)

**Pourquoi c'est important** : Une soustraction entre `unsigned` peut donner un énorme nombre positif au lieu d'un résultat négatif !

### Section 4.3 : Les constantes et les littéraux
*Des valeurs qui ne changent jamais*

Vous maîtriserez :
- Les différentes notations : décimale, hexadécimale, octale, binaire
- Les suffixes (`U`, `L`, `LL`, `f`)
- Le mot-clé `const` vs les macros `#define`
- Les énumérations (`enum`) pour des ensembles de valeurs

**Pourquoi c'est important** : Les constantes rendent le code lisible, maintenable et évitent les "nombres magiques".

### Section 4.4 : Conversion de types
*Quand les types se mélangent*

Vous comprendrez :
- Les conversions implicites (automatiques)
- Les conversions explicites (cast)
- Les risques de perte de données
- Les règles de promotion arithmétique

**Pourquoi c'est important** : `5 / 2` vaut `2` en C (pas `2.5`) si vous ne faites pas attention !

### Section 4.5 : Endianness et portabilité
*L'ordre des octets en mémoire*

Vous découvrirez :
- Ce qu'est l'endianness (big-endian vs little-endian)
- Pourquoi c'est crucial pour les réseaux et les fichiers binaires
- Comment détecter l'endianness de votre système
- Les fonctions de conversion (`htons`, `htonl`, etc.)

**Pourquoi c'est important** : Échanger des données entre un PC (little-endian) et un réseau (big-endian) nécessite une conversion.

### Section 4.6 : Nouveautés C23
*Le C évolue !*

Vous explorerez :
- Les nouveaux mots-clés : `nullptr`, `true`, `false` natifs
- Les littéraux binaires (`0b1010`) et séparateurs de chiffres (`1'000'000`)
- Le type `_BitInt(N)` pour des entiers de taille arbitraire
- Les attributs `[[nodiscard]]` et `[[maybe_unused]]`

**Pourquoi c'est important** : Rester à jour avec les évolutions du langage vous rend plus efficace.

## Ce que vous saurez faire après ce module

À la fin de ce module, vous serez capable de :

✅ **Choisir le type approprié** pour chaque situation
- Besoin d'un entier ? → `int`
- Nombre négatif impossible ? → `unsigned`
- Très grand nombre ? → `long long`
- Nombre décimal ? → `double`

✅ **Éviter les bugs de débordement**
```c
// Vous saurez pourquoi ceci est dangereux
unsigned int a = 5;
unsigned int b = 10;
unsigned int diff = a - b;  // Pas -5, mais 4294967291 !
```

✅ **Écrire du code portable**
```c
// Au lieu de :
int valeur = 123;  // 4 ou 8 octets selon le système ?

// Vous utiliserez :
int32_t valeur = 123;  // Toujours 4 octets, garanti
```

✅ **Manipuler les données binaires**
```c
unsigned char flags = 0b1010'1010;  // C23
unsigned int masque = 0xFF00;       // Hexadécimal
```

✅ **Convertir entre types en toute connaissance**
```c
double moyenne = (double)somme / nombre;  // Division flottante
int tronque = (int)3.14;  // Vous savez que ça donne 3
```

✅ **Gérer l'endianness pour les réseaux**
```c
uint16_t port = htons(8080);  // Host to Network Short
// Conversion automatique selon votre système
```

## Lien avec le reste de la formation

### Ce que vous avez déjà vu

**Modules 1-3** : L'environnement
- Comment compiler un programme C
- La chaîne de compilation (préprocesseur, compilateur, linkeur)
- Les outils de base (GCC, Make)

**→ Vous savez COMMENT compiler, maintenant vous allez comprendre CE QUE vous compilez.**

### Ce qui vient après

**Module 5** : Opérateurs et structures de contrôle
- Manipuler les données avec `+`, `-`, `*`, `/`, `%`
- Opérations bit-à-bit : `&`, `|`, `^`, `~`, `<<`, `>>`
- Conditions et boucles

**Module 6** : La gestion de la mémoire
- Les pointeurs (qui reposent sur les types !)
- L'allocation dynamique
- Les tableaux et chaînes

**→ Impossible de comprendre les pointeurs sans maîtriser les types !**

## Conseils pour réussir ce module

### 1. Expérimentez constamment

**Ne vous contentez pas de lire** : tapez et exécutez chaque exemple.

```c
#include <stdio.h>

int main(void) {
    printf("Taille d'un int : %zu octets\n", sizeof(int));
    printf("INT_MAX : %d\n", INT_MAX);

    // Testez le débordement
    int max = 2147483647;
    printf("max + 1 = %d\n", max + 1);

    return 0;
}
```

Compilez avec :
```bash
gcc -Wall -Wextra programme.c -o programme
./programme
```

### 2. Utilisez les warnings du compilateur

Compilez **toujours** avec `-Wall -Wextra` :
```bash
gcc -Wall -Wextra test.c
```

Le compilateur détecte de nombreuses erreurs de types :
```c
int a = -1;
unsigned int b = 1;

if (a < b) {  // WARNING : comparison between signed and unsigned
    // ...
}
```

### 3. Testez les limites

Explorez les débordements, les conversions, les limites :
```c
#include <limits.h>

printf("CHAR_MIN : %d\n", CHAR_MIN);
printf("CHAR_MAX : %d\n", CHAR_MAX);
printf("INT_MIN : %d\n", INT_MIN);
printf("INT_MAX : %d\n", INT_MAX);
```

### 4. Visualisez en mémoire

Utilisez `sizeof` et affichez les adresses :
```c
int x = 42;
printf("Valeur : %d\n", x);
printf("Adresse : %p\n", (void*)&x);
printf("Taille : %zu octets\n", sizeof(x));
```

### 5. Lisez les sections dans l'ordre

Ce module est **progressif**. Chaque section s'appuie sur la précédente :
1. Types primitifs → base
2. Signed/Unsigned → nuances
3. Constantes → utilisation pratique
4. Conversions → interactions entre types
5. Endianness → représentation matérielle
6. C23 → modernité

### 6. Ne paniquez pas !

Certains concepts (endianness, conversions implicites) peuvent sembler complexes. **C'est normal**.

- Avancez à votre rythme
- Relisez si nécessaire
- Pratiquez avec des exemples simples
- Les choses deviendront claires avec la pratique

## Prérequis pour ce module

Pour tirer le meilleur parti de ce module, vous devriez :

✅ Savoir compiler un programme C basique (`gcc fichier.c`)

✅ Comprendre ce qu'est une variable

✅ Avoir écrit au moins un "Hello World" en C

✅ Être familier avec `printf()` pour afficher des valeurs

Si vous avez suivi les **Modules 1-3**, vous êtes prêt !

## Ressources complémentaires

### Pendant le module

- **Man pages** : `man printf` pour les spécificateurs de format
- **Référence C** : https://en.cppreference.com/w/c
- **`<limits.h>`** : Consultez les limites de votre système

### Pour approfondir

- **"The C Programming Language"** (K&R) - Chapitre 2
- **"Modern C"** (Jens Gustedt) - Parties sur les types
- **ISO C Standard** : Spécification officielle (avancé)

## Un dernier mot avant de commencer

Le C vous demande de **penser comme la machine**. Contrairement aux langages de haut niveau qui abstraient la mémoire, le C vous expose directement :
- Combien d'octets occupe une variable
- Comment les bits sont organisés
- Ce qui se passe en cas de débordement

Cette transparence peut sembler intimidante au début, mais c'est précisément ce qui rend le C si **puissant** et **performant**.

Après ce module, vous ne verrez plus jamais un `int` comme "juste un nombre". Vous comprendrez :
- Qu'il occupe **4 octets** (généralement)
- Qu'il peut stocker de **-2 147 483 648** à **2 147 483 647**
- Que le dépasser provoque un **débordement**
- Qu'en mémoire, c'est une **séquence de 32 bits**
- Que l'ordre de ces bits dépend de l'**architecture** (endianness)

Cette compréhension profonde est ce qui distingue un développeur C débutant d'un développeur C compétent.

---

## Prêt à commencer ?

Passons maintenant à la **Section 4.1 : Types primitifs et leur taille**, où vous découvrirez les briques fondamentales du C : les types de données primitifs, leur taille, leur plage de valeurs, et comment les vérifier sur votre système.

**Rappelez-vous** : chaque concept que vous allez apprendre est un outil qui vous servira tout au long de votre carrière en programmation système, en embarqué, ou en développement DevOps.

Bon apprentissage ! 🚀

---


⏭️ [Types primitifs et leur taille](/04-types-et-memoire/01-types-primitifs.md)
