🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 5.3 Opérateurs bit-à-bit

## Introduction

Les opérateurs bit-à-bit (ou bitwise operators en anglais) sont des outils puissants qui permettent de manipuler les données au niveau le plus bas : **le bit**. Contrairement aux opérateurs logiques qui travaillent sur des conditions vraies ou fausses, les opérateurs bit-à-bit manipulent directement les bits individuels des nombres.

**Pourquoi sont-ils importants ?**
- Optimisation de la mémoire et des performances
- Manipulation de matériel (embedded, drivers)
- Cryptographie et compression
- Gestion de flags et d'options
- Opérations réseau (masques IP, etc.)

Avant d'explorer ces opérateurs, nous devons comprendre comment les nombres sont représentés en binaire.

---

## Rappel : La représentation binaire

### Les bits et les octets

Un **bit** est la plus petite unité d'information en informatique. Il peut avoir deux valeurs :
- `0` (faux, éteint)
- `1` (vrai, allumé)

Un **octet** (byte) est composé de **8 bits**.

### Représentation d'un nombre en binaire

Les nombres que nous utilisons au quotidien sont en **base 10** (décimale). En informatique, les nombres sont stockés en **base 2** (binaire).

**Exemple : Le nombre 13 en binaire**

```
Décimal : 13
Binaire : 00001101

Position des bits :  7  6  5  4  3  2  1  0
Valeur de position: 128 64 32 16 8  4  2  1
Bit :                0  0  0  0  1  1  0  1
                                 ↓  ↓     ↓
                                 8 +4  +  1 = 13
```

**Comment lire un nombre binaire :**
- Chaque position de bit représente une puissance de 2
- On additionne les valeurs des positions où le bit est à `1`

**Quelques exemples :**

| Décimal | Binaire (8 bits) | Calcul |
|---------|------------------|--------|
| 0       | `00000000`       | 0 |
| 1       | `00000001`       | 1 |
| 5       | `00000101`       | 4 + 1 |
| 10      | `00001010`       | 8 + 2 |
| 15      | `00001111`       | 8 + 4 + 2 + 1 |
| 255     | `11111111`       | 128 + 64 + 32 + 16 + 8 + 4 + 2 + 1 |

---

## Les six opérateurs bit-à-bit

Le C propose six opérateurs pour manipuler les bits :

| Opérateur | Symbole | Nom | Description |
|-----------|---------|-----|-------------|
| ET        | `&`     | AND | Met à 1 si les deux bits sont à 1 |
| OU        | `\|`    | OR  | Met à 1 si au moins un bit est à 1 |
| OU exclusif | `^`   | XOR | Met à 1 si les bits sont différents |
| NON       | `~`     | NOT | Inverse tous les bits |
| Décalage gauche | `<<` | Left shift | Décale les bits vers la gauche |
| Décalage droite | `>>` | Right shift | Décale les bits vers la droite |

---

## 1. L'opérateur ET bit-à-bit : `&`

L'opérateur `&` compare deux nombres **bit par bit**. Le résultat a un bit à `1` uniquement si **les deux bits correspondants** sont à `1`.

### Table de vérité

| Bit A | Bit B | A & B |
|-------|-------|-------|
| 0     | 0     | 0     |
| 0     | 1     | 0     |
| 1     | 0     | 0     |
| 1     | 1     | **1** |

### Exemple visuel

```c
  13 :  00001101
& 10 :  00001010
-----------------
   8 :  00001000
```

**Explication :**
- Position 3 : `1 & 1 = 1` → Résultat : bit à 1 (valeur 8)
- Position 2 : `1 & 0 = 0` → Résultat : bit à 0
- Position 1 : `0 & 1 = 0` → Résultat : bit à 0
- Position 0 : `1 & 0 = 0` → Résultat : bit à 0

### Code d'exemple

```c
#include <stdio.h>

int main() {
    unsigned int a = 13;  // 00001101
    unsigned int b = 10;  // 00001010
    unsigned int resultat = a & b;

    printf("%u & %u = %u\n", a, b, resultat);  // Affiche : 13 & 10 = 8

    return 0;
}
```

### Cas d'usage pratiques

**1. Vérifier si un bit spécifique est activé (masquage)**

```c
#include <stdio.h>

int main() {
    unsigned int flags = 13;  // 00001101
    unsigned int masque = 4;  // 00000100 (vérifie le bit position 2)

    if (flags & masque) {
        printf("Le bit 2 est activé\n");
    } else {
        printf("Le bit 2 est désactivé\n");
    }

    return 0;
}
```

**2. Extraire certains bits (partie d'un nombre)**

```c
#include <stdio.h>

int main() {
    unsigned int couleur_rgb = 0xFF3A7C;  // Couleur au format RGB (24 bits)

    // Extraire la composante rouge (bits 16-23)
    unsigned int rouge = (couleur_rgb >> 16) & 0xFF;

    // Extraire la composante verte (bits 8-15)
    unsigned int vert = (couleur_rgb >> 8) & 0xFF;

    // Extraire la composante bleue (bits 0-7)
    unsigned int bleu = couleur_rgb & 0xFF;

    printf("Rouge: %u, Vert: %u, Bleu: %u\n", rouge, vert, bleu);

    return 0;
}
```

---

## 2. L'opérateur OU bit-à-bit : `|`

L'opérateur `|` compare deux nombres bit par bit. Le résultat a un bit à `1` si **au moins un des deux bits** est à `1`.

### Table de vérité

| Bit A | Bit B | A \| B |
|-------|-------|--------|
| 0     | 0     | 0      |
| 0     | 1     | **1**  |
| 1     | 0     | **1**  |
| 1     | 1     | **1**  |

### Exemple visuel

```c
  13 :  00001101
| 10 :  00001010
-----------------
  15 :  00001111
```

### Code d'exemple

```c
#include <stdio.h>

int main() {
    unsigned int a = 13;  // 00001101
    unsigned int b = 10;  // 00001010
    unsigned int resultat = a | b;

    printf("%u | %u = %u\n", a, b, resultat);  // Affiche : 13 | 10 = 15

    return 0;
}
```

### Cas d'usage pratiques

**1. Activer des bits spécifiques (définir des flags)**

```c
#include <stdio.h>

// Définition de flags pour les permissions
#define LECTURE   0x01  // 00000001
#define ECRITURE  0x02  // 00000010
#define EXECUTION 0x04  // 00000100

int main() {
    unsigned int permissions = 0;  // Aucune permission au départ

    // Activer la lecture et l'exécution
    permissions = permissions | LECTURE | EXECUTION;

    printf("Permissions: %u\n", permissions);  // 5 (00000101)

    // Vérifier les permissions
    if (permissions & LECTURE) {
        printf("✓ Lecture autorisée\n");
    }
    if (permissions & ECRITURE) {
        printf("✓ Écriture autorisée\n");
    } else {
        printf("✗ Écriture non autorisée\n");
    }
    if (permissions & EXECUTION) {
        printf("✓ Exécution autorisée\n");
    }

    return 0;
}
```

**2. Combiner des options**

```c
#include <stdio.h>

// Options d'affichage
#define GRAS        0x01  // 00000001
#define ITALIQUE    0x02  // 00000010
#define SOULIGNE    0x04  // 00000100
#define BARRE       0x08  // 00001000

int main() {
    unsigned int style = GRAS | SOULIGNE;  // Style gras et souligné

    printf("Style actif: %u\n", style);  // 5 (00000101)

    return 0;
}
```

---

## 3. L'opérateur OU exclusif (XOR) : `^`

L'opérateur `^` (XOR) compare deux nombres bit par bit. Le résultat a un bit à `1` si **les deux bits sont différents**.

### Table de vérité

| Bit A | Bit B | A ^ B |
|-------|-------|-------|
| 0     | 0     | 0     |
| 0     | 1     | **1** |
| 1     | 0     | **1** |
| 1     | 1     | 0     |

### Exemple visuel

```c
  13 :  00001101
^  10 :  00001010
-----------------
   7 :  00000111
```

### Code d'exemple

```c
#include <stdio.h>

int main() {
    unsigned int a = 13;  // 00001101
    unsigned int b = 10;  // 00001010
    unsigned int resultat = a ^ b;

    printf("%u ^ %u = %u\n", a, b, resultat);  // Affiche : 13 ^ 10 = 7

    return 0;
}
```

### Cas d'usage pratiques

**1. Inverser (toggle) un bit spécifique**

```c
#include <stdio.h>

int main() {
    unsigned int flags = 5;  // 00000101
    unsigned int masque = 4; // 00000100 (bit position 2)

    printf("Avant : %u\n", flags);

    // Inverser le bit 2
    flags = flags ^ masque;
    printf("Après inversion : %u\n", flags);  // 1 (00000001)

    // Inverser à nouveau le même bit
    flags = flags ^ masque;
    printf("Après 2e inversion : %u\n", flags);  // 5 (00000101)

    return 0;
}
```

**2. Échanger deux variables sans variable temporaire**

```c
#include <stdio.h>

int main() {
    int a = 10;
    int b = 20;

    printf("Avant : a=%d, b=%d\n", a, b);

    // Échange sans variable temporaire
    a = a ^ b;  // a contient maintenant a XOR b
    b = a ^ b;  // b = (a XOR b) XOR b = a
    a = a ^ b;  // a = (a XOR b) XOR a = b

    printf("Après : a=%d, b=%d\n", a, b);

    return 0;
}
```

**3. Cryptographie simple (chiffrement XOR)**

```c
#include <stdio.h>

int main() {
    char message = 'A';
    char cle = 0x5A;  // Clé de chiffrement

    // Chiffrement
    char message_chiffre = message ^ cle;
    printf("Message original : %c (%d)\n", message, message);
    printf("Message chiffré : %c (%d)\n", message_chiffre, message_chiffre);

    // Déchiffrement (XOR avec la même clé)
    char message_dechiffre = message_chiffre ^ cle;
    printf("Message déchiffré : %c (%d)\n", message_dechiffre, message_dechiffre);

    return 0;
}
```

---

## 4. L'opérateur NON bit-à-bit : `~`

L'opérateur `~` (complément à un) inverse **tous les bits** d'un nombre : les `0` deviennent des `1` et vice versa.

### Table de vérité

| Bit A | ~A |
|-------|----|
| 0     | **1** |
| 1     | **0** |

### Exemple visuel (sur 8 bits)

```c
  13 :  00001101
~
-----------------
 242 :  11110010
```

**⚠️ Attention :** Le résultat dépend de la taille du type de données (8, 16, 32 bits, etc.)

### Code d'exemple

```c
#include <stdio.h>

int main() {
    unsigned char a = 13;  // 00001101 (sur 8 bits)
    unsigned char resultat = ~a;

    printf("~%u = %u\n", a, resultat);  // Affiche : ~13 = 242

    // Explication : 11110010 en binaire = 242 en décimal

    return 0;
}
```

### Cas d'usage pratiques

**1. Créer un masque inverse**

```c
#include <stdio.h>

#define BIT_2 0x04  // 00000100

int main() {
    unsigned int flags = 0xFF;  // 11111111 (tous les bits à 1)

    // Désactiver uniquement le bit 2
    flags = flags & ~BIT_2;

    printf("Flags: %u\n", flags);  // 251 (11111011)

    return 0;
}
```

**2. Obtenir le complément**

```c
#include <stdio.h>

int main() {
    unsigned char valeur = 0x0F;  // 00001111
    unsigned char complement = ~valeur;

    printf("Valeur: %u (0x%02X)\n", valeur, valeur);
    printf("Complément: %u (0x%02X)\n", complement, complement);  // 240 (0xF0)

    return 0;
}
```

---

## 5. Décalage à gauche : `<<`

L'opérateur `<<` décale tous les bits vers la **gauche** d'un certain nombre de positions. Les bits qui sortent à gauche sont perdus, et des `0` sont ajoutés à droite.

**Effet mathématique :** Décaler de `n` positions à gauche équivaut à **multiplier par 2^n**.

### Syntaxe

```c
valeur << nombre_de_positions
```

### Exemple visuel

```c
   13 :  00001101
<< 2           (décalage de 2 positions vers la gauche)
-----------------
   52 :  00110100
```

**Explication :**
- Les bits sont décalés de 2 positions vers la gauche
- Deux `0` sont ajoutés à droite
- 13 × 2² = 13 × 4 = 52

### Code d'exemple

```c
#include <stdio.h>

int main() {
    unsigned int a = 13;

    printf("%u << 1 = %u\n", a, a << 1);  // 26 (13 × 2)
    printf("%u << 2 = %u\n", a, a << 2);  // 52 (13 × 4)
    printf("%u << 3 = %u\n", a, a << 3);  // 104 (13 × 8)

    return 0;
}
```

### Cas d'usage pratiques

**1. Multiplication rapide par une puissance de 2**

```c
#include <stdio.h>

int main() {
    int nombre = 5;

    // Multiplier par 2
    int double_val = nombre << 1;  // Plus rapide que nombre * 2

    // Multiplier par 8
    int fois_huit = nombre << 3;   // Plus rapide que nombre * 8

    printf("%d × 2 = %d\n", nombre, double_val);
    printf("%d × 8 = %d\n", nombre, fois_huit);

    return 0;
}
```

**2. Définir des masques de bits**

```c
#include <stdio.h>

int main() {
    // Créer un masque avec le bit n à 1
    unsigned int bit_0 = 1 << 0;  // 00000001 (valeur 1)
    unsigned int bit_3 = 1 << 3;  // 00001000 (valeur 8)
    unsigned int bit_7 = 1 << 7;  // 10000000 (valeur 128)

    printf("Bit 0 : %u\n", bit_0);
    printf("Bit 3 : %u\n", bit_3);
    printf("Bit 7 : %u\n", bit_7);

    return 0;
}
```

**3. Créer des flags**

```c
#include <stdio.h>

// Définition propre des flags avec décalage
#define FLAG_A  (1 << 0)  // 00000001
#define FLAG_B  (1 << 1)  // 00000010
#define FLAG_C  (1 << 2)  // 00000100
#define FLAG_D  (1 << 3)  // 00001000

int main() {
    unsigned int options = FLAG_A | FLAG_C;  // Activer A et C

    if (options & FLAG_A) {
        printf("Option A activée\n");
    }
    if (options & FLAG_C) {
        printf("Option C activée\n");
    }

    return 0;
}
```

---

## 6. Décalage à droite : `>>`

L'opérateur `>>` décale tous les bits vers la **droite** d'un certain nombre de positions. Les bits qui sortent à droite sont perdus.

**Effet mathématique :** Décaler de `n` positions à droite équivaut à **diviser par 2^n** (division entière).

### Syntaxe

```c
valeur >> nombre_de_positions
```

### Exemple visuel

```c
   13 :  00001101
>> 2           (décalage de 2 positions vers la droite)
-----------------
    3 :  00000011
```

**Explication :**
- Les bits sont décalés de 2 positions vers la droite
- Les deux bits de droite (01) sont perdus
- 13 ÷ 4 = 3 (division entière)

### Code d'exemple

```c
#include <stdio.h>

int main() {
    unsigned int a = 13;

    printf("%u >> 1 = %u\n", a, a >> 1);  // 6 (13 ÷ 2)
    printf("%u >> 2 = %u\n", a, a >> 2);  // 3 (13 ÷ 4)
    printf("%u >> 3 = %u\n", a, a >> 3);  // 1 (13 ÷ 8)

    return 0;
}
```

### ⚠️ Attention : Décalage arithmétique vs logique

Pour les nombres **signés**, le comportement de `>>` dépend de l'implémentation :
- **Décalage logique** : Un `0` est ajouté à gauche
- **Décalage arithmétique** : Le bit de signe est préservé

```c
#include <stdio.h>

int main() {
    int negatif = -8;  // Nombre négatif

    // Sur la plupart des systèmes, le décalage arithmétique préserve le signe
    printf("%d >> 1 = %d\n", negatif, negatif >> 1);  // -4

    // Avec unsigned, toujours un décalage logique
    unsigned int positif = 8;
    printf("%u >> 1 = %u\n", positif, positif >> 1);  // 4

    return 0;
}
```

**💡 Bonne pratique :** Utilisez `unsigned` pour les opérations bit-à-bit afin d'éviter les comportements imprévisibles.

### Cas d'usage pratiques

**1. Division rapide par une puissance de 2**

```c
#include <stdio.h>

int main() {
    unsigned int nombre = 100;

    // Diviser par 2
    unsigned int moitie = nombre >> 1;  // Plus rapide que nombre / 2

    // Diviser par 8
    unsigned int huitieme = nombre >> 3;  // Plus rapide que nombre / 8

    printf("%u ÷ 2 = %u\n", nombre, moitie);
    printf("%u ÷ 8 = %u\n", nombre, huitieme);

    return 0;
}
```

**2. Extraire des parties d'un nombre**

```c
#include <stdio.h>

int main() {
    unsigned int valeur = 0x12345678;  // Nombre hexadécimal

    // Extraire l'octet de poids fort (bits 24-31)
    unsigned char octet_haut = (valeur >> 24) & 0xFF;

    // Extraire l'octet suivant (bits 16-23)
    unsigned char octet_2 = (valeur >> 16) & 0xFF;

    printf("Valeur complète : 0x%08X\n", valeur);
    printf("Octet haut : 0x%02X\n", octet_haut);
    printf("Octet 2 : 0x%02X\n", octet_2);

    return 0;
}
```

---

## Opérateurs d'affectation combinés

Le C permet de combiner les opérateurs bit-à-bit avec l'affectation :

| Opérateur | Équivalent à | Description |
|-----------|--------------|-------------|
| `&=`      | `a = a & b`  | ET et affecte |
| `\|=`     | `a = a \| b` | OU et affecte |
| `^=`      | `a = a ^ b`  | XOR et affecte |
| `<<=`     | `a = a << b` | Décale à gauche et affecte |
| `>>=`     | `a = a >> b` | Décale à droite et affecte |

### Exemples

```c
#include <stdio.h>

int main() {
    unsigned int flags = 5;  // 00000101

    // Activer le bit 1
    flags |= (1 << 1);  // flags = flags | (1 << 1)
    printf("Après |= : %u\n", flags);  // 7 (00000111)

    // Désactiver le bit 2
    flags &= ~(1 << 2);  // flags = flags & ~(1 << 2)
    printf("Après &= : %u\n", flags);  // 3 (00000011)

    // Inverser le bit 0
    flags ^= (1 << 0);  // flags = flags ^ (1 << 0)
    printf("Après ^= : %u\n", flags);  // 2 (00000010)

    return 0;
}
```

---

## Exemples pratiques avancés

### 1. Gestion de bits de statut (flags)

```c
#include <stdio.h>

// Définition des flags
#define STATUS_READY      (1 << 0)  // Bit 0 : 00000001
#define STATUS_BUSY       (1 << 1)  // Bit 1 : 00000010
#define STATUS_ERROR      (1 << 2)  // Bit 2 : 00000100
#define STATUS_CONNECTED  (1 << 3)  // Bit 3 : 00001000

int main() {
    unsigned int status = 0;

    // Définir plusieurs flags
    status |= STATUS_READY;
    status |= STATUS_CONNECTED;

    printf("Status initial : 0x%02X\n", status);  // 0x09 (00001001)

    // Vérifier si un flag est actif
    if (status & STATUS_READY) {
        printf("✓ Système prêt\n");
    }

    if (status & STATUS_BUSY) {
        printf("✓ Système occupé\n");
    } else {
        printf("✗ Système libre\n");
    }

    // Activer le flag BUSY
    status |= STATUS_BUSY;
    printf("Status après activation BUSY : 0x%02X\n", status);  // 0x0B

    // Désactiver le flag READY
    status &= ~STATUS_READY;
    printf("Status après désactivation READY : 0x%02X\n", status);  // 0x0A

    // Inverser le flag ERROR
    status ^= STATUS_ERROR;
    printf("Status après inversion ERROR : 0x%02X\n", status);  // 0x0E

    return 0;
}
```

### 2. Manipulation de couleurs RGB

```c
#include <stdio.h>

// Créer une couleur RGB (24 bits)
unsigned int creer_rgb(unsigned char r, unsigned char g, unsigned char b) {
    return ((r << 16) | (g << 8) | b);
}

// Extraire les composantes
unsigned char extraire_rouge(unsigned int couleur) {
    return (couleur >> 16) & 0xFF;
}

unsigned char extraire_vert(unsigned int couleur) {
    return (couleur >> 8) & 0xFF;
}

unsigned char extraire_bleu(unsigned int couleur) {
    return couleur & 0xFF;
}

int main() {
    // Créer une couleur violette (rouge=128, vert=0, bleu=255)
    unsigned int violet = creer_rgb(128, 0, 255);

    printf("Couleur : 0x%06X\n", violet);

    // Extraire les composantes
    printf("Rouge : %u\n", extraire_rouge(violet));
    printf("Vert : %u\n", extraire_vert(violet));
    printf("Bleu : %u\n", extraire_bleu(violet));

    // Modifier la composante verte
    unsigned int nouvelle_couleur = (violet & 0xFF00FF) | (100 << 8);
    printf("Nouvelle couleur : 0x%06X\n", nouvelle_couleur);

    return 0;
}
```

### 3. Vérifier si un nombre est une puissance de 2

```c
#include <stdio.h>

int est_puissance_de_deux(unsigned int n) {
    // Un nombre est une puissance de 2 si :
    // - Il n'est pas 0
    // - n & (n-1) == 0
    //
    // Exemple avec 8 (puissance de 2) :
    //   8  : 00001000
    //   7  : 00000111
    //   8&7: 00000000 → C'est une puissance de 2
    //
    // Exemple avec 6 (pas une puissance de 2) :
    //   6  : 00000110
    //   5  : 00000101
    //   6&5: 00000100 → Ce n'est pas une puissance de 2

    return (n != 0) && ((n & (n - 1)) == 0);
}

int main() {
    unsigned int nombres[] = {1, 2, 3, 4, 5, 8, 15, 16, 32, 100};
    int taille = sizeof(nombres) / sizeof(nombres[0]);

    for (int i = 0; i < taille; i++) {
        if (est_puissance_de_deux(nombres[i])) {
            printf("%u est une puissance de 2\n", nombres[i]);
        } else {
            printf("%u n'est pas une puissance de 2\n", nombres[i]);
        }
    }

    return 0;
}
```

### 4. Compter le nombre de bits à 1

```c
#include <stdio.h>

// Méthode simple mais efficace
int compter_bits_a_un(unsigned int n) {
    int compteur = 0;

    while (n) {
        compteur += n & 1;  // Ajoute 1 si le bit de droite est à 1
        n >>= 1;            // Décale vers la droite
    }

    return compteur;
}

// Méthode optimisée (algorithme de Brian Kernighan)
int compter_bits_a_un_optimise(unsigned int n) {
    int compteur = 0;

    // À chaque itération, n & (n-1) supprime le bit à 1 le plus à droite
    while (n) {
        n &= (n - 1);
        compteur++;
    }

    return compteur;
}

int main() {
    unsigned int nombre = 13;  // 00001101 (3 bits à 1)

    printf("Nombre : %u (binaire : ", nombre);

    // Afficher en binaire
    for (int i = 7; i >= 0; i--) {
        printf("%d", (nombre >> i) & 1);
    }
    printf(")\n");

    printf("Nombre de bits à 1 : %d\n", compter_bits_a_un(nombre));
    printf("Nombre de bits à 1 (optimisé) : %d\n", compter_bits_a_un_optimise(nombre));

    return 0;
}
```

---

## Différences importantes : Opérateurs logiques vs bit-à-bit

Il est crucial de ne pas confondre les opérateurs logiques (`&&`, `||`, `!`) avec les opérateurs bit-à-bit (`&`, `|`, `~`).

| Aspect | Logiques | Bit-à-bit |
|--------|----------|-----------|
| Symboles | `&&`, `\|\|`, `!` | `&`, `\|`, `~`, `^` |
| Opération | Sur des booléens | Sur chaque bit |
| Résultat | 0 ou 1 | Toutes valeurs possibles |
| Court-circuit | Oui | Non |

### Exemple de différence

```c
#include <stdio.h>

int main() {
    int a = 5;   // 00000101
    int b = 3;   // 00000011

    // Opérateurs logiques (résultat booléen)
    printf("a && b = %d\n", a && b);  // 1 (vrai ET vrai = vrai)
    printf("a || b = %d\n", a || b);  // 1 (vrai OU vrai = vrai)

    // Opérateurs bit-à-bit (opération sur les bits)
    printf("a & b = %d\n", a & b);    // 1 (00000001)
    printf("a | b = %d\n", a | b);    // 7 (00000111)

    return 0;
}
```

**⚠️ Erreur courante :**

```c
// ❌ ERREUR : Utiliser & au lieu de &&
if (x > 0 & y > 0) {
    // Cela compile mais ne fait pas ce que vous pensez !
}

// ✅ CORRECT : Utiliser && pour les conditions logiques
if (x > 0 && y > 0) {
    // Correct : teste si x ET y sont positifs
}
```

---

## Affichage de nombres en binaire

Pour mieux comprendre les opérations bit-à-bit, il est utile d'afficher les nombres en binaire.

```c
#include <stdio.h>

// Fonction pour afficher un nombre en binaire
void afficher_binaire(unsigned int n, int nb_bits) {
    for (int i = nb_bits - 1; i >= 0; i--) {
        printf("%d", (n >> i) & 1);

        // Ajouter un espace tous les 4 bits pour la lisibilité
        if (i % 4 == 0 && i != 0) {
            printf(" ");
        }
    }
    printf("\n");
}

int main() {
    unsigned int a = 13;
    unsigned int b = 10;

    printf("a = %u : ", a);
    afficher_binaire(a, 8);

    printf("b = %u : ", b);
    afficher_binaire(b, 8);

    printf("\na & b = %u : ", a & b);
    afficher_binaire(a & b, 8);

    printf("a | b = %u : ", a | b);
    afficher_binaire(a | b, 8);

    printf("a ^ b = %u : ", a ^ b);
    afficher_binaire(a ^ b, 8);

    printf("~a = %u : ", (unsigned char)~a);
    afficher_binaire((unsigned char)~a, 8);

    return 0;
}
```

**Résultat :**
```
a = 13 : 0000 1101
b = 10 : 0000 1010

a & b = 8 : 0000 1000
a | b = 15 : 0000 1111
a ^ b = 7 : 0000 0111
~a = 242 : 1111 0010
```

---

## Erreurs courantes à éviter

### 1. Confondre `&` et `&&`

```c
// ❌ ERREUR
if (flags & FLAG_A && flags & FLAG_B) {  // Mélange des deux types !
    // Intention pas claire
}

// ✅ CORRECT (vérifier deux flags)
if ((flags & FLAG_A) && (flags & FLAG_B)) {
    // Clair : vérifie si les deux flags sont actifs
}

// ✅ CORRECT (vérifier un flag combiné)
if (flags & (FLAG_A | FLAG_B)) {
    // Clair : vérifie si au moins un des deux flags est actif
}
```

### 2. Oublier les parenthèses

Les opérateurs bit-à-bit ont une priorité plus faible que les opérateurs de comparaison.

```c
// ❌ ERREUR : Mauvaise priorité
if (flags & FLAG_A == 1) {
    // Équivalent à : if (flags & (FLAG_A == 1))
    // Ce n'est probablement pas ce que vous voulez !
}

// ✅ CORRECT
if ((flags & FLAG_A) == FLAG_A) {
    // Vérifie correctement si FLAG_A est actif
}

// ✅ ENCORE MIEUX (idiome standard)
if (flags & FLAG_A) {
    // Idiome C standard pour vérifier un flag
}
```

### 3. Décalage avec des nombres signés

```c
// ⚠️ ATTENTION : Comportement imprévisible
int negatif = -1;
int resultat = negatif >> 1;  // Comportement dépendant de l'implémentation

// ✅ CORRECT : Utiliser unsigned
unsigned int positif = 10;
unsigned int resultat = positif >> 1;  // Comportement garanti
```

### 4. Décalage au-delà de la taille du type

```c
// ❌ COMPORTEMENT INDÉFINI
unsigned char c = 1;
unsigned char resultat = c << 10;  // Décalage de 10 positions sur 8 bits !

// ✅ CORRECT : Respecter la taille du type
unsigned int i = 1;
unsigned int resultat = i << 10;  // OK sur un int (au moins 16 bits)
```

### 5. Utiliser `~` sur des types signés

```c
// ⚠️ ATTENTION : Peut donner des résultats inattendus
int a = 5;
int complement = ~a;  // -6 (complément à deux)

// ✅ CORRECT : Utiliser unsigned pour les opérations bit-à-bit
unsigned int a = 5;
unsigned int complement = ~a;  // Résultat prévisible
```

---

## Bonnes pratiques

### 1. Toujours utiliser `unsigned` pour les opérations bit-à-bit

```c
// ✅ RECOMMANDÉ
unsigned int flags = 0;
unsigned char byte = 0xFF;
```

### 2. Définir des constantes pour les masques

```c
// ✅ BONNE PRATIQUE
#define MASK_READY  (1 << 0)
#define MASK_ERROR  (1 << 1)
#define MASK_DONE   (1 << 2)

unsigned int status = MASK_READY | MASK_DONE;
```

### 3. Utiliser des parenthèses pour la clarté

```c
// ✅ CLAIR
if ((status & MASK_READY) && (status & MASK_DONE)) {
    // Les deux flags sont actifs
}
```

### 4. Commenter les opérations complexes

```c
// ✅ BONNE PRATIQUE
// Extraire les 4 bits de poids faible
unsigned int lower_nibble = value & 0x0F;

// Décaler de 16 bits pour obtenir la partie haute d'une adresse 32 bits
unsigned int high_part = (address >> 16) & 0xFFFF;
```

### 5. Utiliser des fonctions helpers pour les opérations répétitives

```c
// ✅ RÉUTILISABLE
static inline void set_bit(unsigned int *flags, int bit_pos) {
    *flags |= (1 << bit_pos);
}

static inline void clear_bit(unsigned int *flags, int bit_pos) {
    *flags &= ~(1 << bit_pos);
}

static inline int test_bit(unsigned int flags, int bit_pos) {
    return (flags & (1 << bit_pos)) != 0;
}
```

---

## Applications concrètes

### Domaines d'utilisation

1. **Programmation système** : Manipulation de registres matériels, gestion de ports I/O
2. **Réseau** : Manipulation d'adresses IP, masques réseau
3. **Graphisme** : Manipulation de pixels, canaux de couleur
4. **Compression** : Algorithmes de compression de données
5. **Cryptographie** : Opérations sur les bits pour le chiffrement
6. **Embedded** : Optimisation mémoire, contrôle matériel
7. **Protocoles** : Parsing de trames, extraction de champs

---

## Résumé

| Opérateur | Symbole | Description | Usage typique |
|-----------|---------|-------------|---------------|
| ET | `&` | Bit à 1 si les deux sont à 1 | Masquage, tester des bits |
| OU | `\|` | Bit à 1 si au moins un est à 1 | Activer des bits |
| XOR | `^` | Bit à 1 si les bits sont différents | Inverser, chiffrement |
| NON | `~` | Inverse tous les bits | Créer masques inverses |
| Décalage gauche | `<<` | Décale vers la gauche | Multiplication par 2^n |
| Décalage droite | `>>` | Décale vers la droite | Division par 2^n |

**Points clés à retenir :**

- Les opérateurs bit-à-bit travaillent sur chaque bit individuellement
- Différents des opérateurs logiques (`&&`, `||`, `!`)
- Utilisez `unsigned` pour éviter les comportements imprévisibles
- Utiles pour l'optimisation et le contrôle bas niveau
- Mettez des parenthèses pour clarifier les priorités
- Décalage à gauche = multiplication ; décalage à droite = division
- XOR a des propriétés mathématiques intéressantes (inversibilité)

---

## Pour aller plus loin

Dans les prochaines sections, vous découvrirez :
- **5.4 Conditions et optimisation** : Comment optimiser les conditions avec les opérateurs bit-à-bit
- **5.5 Boucles et contrôle de flux** : Utilisation dans les structures itératives
- **Module 7** : Techniques avancées incluant les masques complexes et les bit-fields

Les opérateurs bit-à-bit sont puissants mais nécessitent de la pratique. N'hésitez pas à expérimenter et à visualiser les résultats en binaire pour bien comprendre leur fonctionnement !

⏭️ [Conditions et optimisation](/05-operateurs-et-controle/04-conditions.md)
