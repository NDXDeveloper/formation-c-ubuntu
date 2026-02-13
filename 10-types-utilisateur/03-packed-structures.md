🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 10.3 Packed structures

## Introduction

Dans la section précédente, nous avons vu que le compilateur ajoute automatiquement du **padding** pour respecter l'alignement mémoire. Mais que faire si nous voulons **forcer le compilateur à supprimer ce padding** ?

C'est là qu'interviennent les **packed structures** (ou structures compactes). Elles permettent de stocker les membres de manière **contiguë**, sans espace vide entre eux.

**Attention** : Cette fonctionnalité est puissante mais doit être utilisée avec précaution !

---

## Qu'est-ce qu'une packed structure ?

### Définition

Une **packed structure** est une structure dans laquelle le compilateur est **forcé de ne pas ajouter de padding** entre les membres. Les données sont stockées de manière compacte, exactement dans l'ordre de déclaration.

### Comparaison visuelle

**Structure normale** :
```c
struct Normal {
    char c;     // 1 octet
    int i;      // 4 octets
};
// Taille : 8 octets
```

```
Adresse:   0    1    2    3    4    5    6    7
          +----+----+----+----+----+----+----+----+
          | c  | XX | XX | XX | i0 | i1 | i2 | i3 |
          +----+----+----+----+----+----+----+----+
               ^-- 3 octets de padding
```

**Structure packed** :
```c
struct __attribute__((packed)) Packed {
    char c;     // 1 octet
    int i;      // 4 octets
};
// Taille : 5 octets
```

```
Adresse:   0    1    2    3    4
          +----+----+----+----+----+
          | c  | i0 | i1 | i2 | i3 |
          +----+----+----+----+----+
               ^-- Pas de padding !
```

---

## Comment créer une packed structure ?

### Méthode 1 : Attribut GCC (recommandée)

La méthode la plus portable avec GCC et Clang :

```c
struct __attribute__((packed)) MaStructure {
    char c;
    int i;
    double d;
};
```

**Syntaxe alternative** (plus lisible) :

```c
struct MaStructure {
    char c;
    int i;
    double d;
} __attribute__((packed));
```

### Méthode 2 : #pragma pack (Microsoft et GCC)

```c
#pragma pack(push, 1)  // Active le packing avec alignement de 1 octet

struct MaStructure {
    char c;
    int i;
    double d;
};

#pragma pack(pop)      // Restaure l'alignement par défaut
```

**Note** : L'argument `1` signifie "aligner sur 1 octet" (pas de padding).

### Méthode 3 : #pragma pack pour tout un fichier

```c
#pragma pack(1)  // Active le packing pour toutes les structures suivantes

struct Structure1 {
    char c;
    int i;
};

struct Structure2 {
    short s;
    double d;
};

#pragma pack()  // Restaure l'alignement par défaut
```

---

## Exemple complet : Comparaison

```c
#include <stdio.h>
#include <stddef.h>

// Structure normale
struct Normal {
    char c;
    int i;
    short s;
};

// Structure packed avec attribut GCC
struct __attribute__((packed)) PackedAttribut {
    char c;
    int i;
    short s;
};

// Structure packed avec pragma
#pragma pack(push, 1)
struct PackedPragma {
    char c;
    int i;
    short s;
};
#pragma pack(pop)

int main() {
    printf("=== COMPARAISON DES TAILLES ===\n\n");

    printf("Structure normale     : %zu octets\n", sizeof(struct Normal));
    printf("Packed (attribut)     : %zu octets\n", sizeof(struct PackedAttribut));
    printf("Packed (pragma)       : %zu octets\n", sizeof(struct PackedPragma));

    printf("\n=== DISPOSITION MÉMOIRE ===\n\n");

    struct Normal n;
    printf("Normal - Offset c : %zu\n", offsetof(struct Normal, c));
    printf("Normal - Offset i : %zu\n", offsetof(struct Normal, i));
    printf("Normal - Offset s : %zu\n", offsetof(struct Normal, s));

    printf("\n");

    struct PackedAttribut p;
    printf("Packed - Offset c : %zu\n", offsetof(struct PackedAttribut, c));
    printf("Packed - Offset i : %zu\n", offsetof(struct PackedAttribut, i));
    printf("Packed - Offset s : %zu\n", offsetof(struct PackedAttribut, s));

    return 0;
}
```

**Sortie** :
```
=== COMPARAISON DES TAILLES ===

Structure normale     : 12 octets  
Packed (attribut)     : 7 octets  
Packed (pragma)       : 7 octets  

=== DISPOSITION MÉMOIRE ===

Normal - Offset c : 0  
Normal - Offset i : 4  
Normal - Offset s : 8  

Packed - Offset c : 0  
Packed - Offset i : 1  
Packed - Offset s : 5  
```

### Visualisation mémoire

**Structure normale (12 octets)** :
```
Offset:    0    1    2    3    4    5    6    7    8    9   10   11
          +----+----+----+----+----+----+----+----+----+----+----+----+
          | c  | XX | XX | XX | i0 | i1 | i2 | i3 | s0 | s1 | XX | XX |
          +----+----+----+----+----+----+----+----+----+----+----+----+
```

**Structure packed (7 octets)** :
```
Offset:    0    1    2    3    4    5    6
          +----+----+----+----+----+----+----+
          | c  | i0 | i1 | i2 | i3 | s0 | s1 |
          +----+----+----+----+----+----+----+
```

**Gain** : 5 octets économisés (41% de réduction) !

---

## Les compromis : Avantages et inconvénients

### ✅ Avantages

#### 1. Économie de mémoire

Pour un grand nombre de structures, l'économie peut être significative :

```c
struct __attribute__((packed)) Donnee {
    char type;
    int valeur;
    char etat;
};
// Taille packed : 6 octets
// Taille normale : 12 octets

// Pour 1 million d'éléments :
// Packed  : 6 Mo
// Normal  : 12 Mo
// Gain    : 6 Mo (50%)
```

#### 2. Compatibilité avec formats externes

Les packed structures sont essentielles pour :
- **Protocoles réseau** (formats binaires stricts)
- **Formats de fichiers** (headers binaires)
- **Communication hardware** (registres de périphériques)
- **Interopérabilité** avec d'autres langages

```c
// En-tête d'un fichier BMP (format Windows Bitmap)
#pragma pack(push, 1)
struct BMPHeader {
    unsigned short type;        // 2 octets : "BM"
    unsigned int size;          // 4 octets : taille du fichier
    unsigned short reserved1;   // 2 octets : réservé
    unsigned short reserved2;   // 2 octets : réservé
    unsigned int offset;        // 4 octets : offset des données
};
#pragma pack(pop)
// Taille : exactement 14 octets (comme spécifié dans le format BMP)
```

### ❌ Inconvénients

#### 1. Perte de performance

Les accès à des données désalignées sont **plus lents** :

```c
struct __attribute__((packed)) Lent {
    char c;
    int i;  // Commence à l'adresse 1 (désaligné !)
};

struct Lent obj;  
obj.i = 42;  // Accès plus lent car désaligné  
```

Sur certaines architectures (ARM strictes), un accès désaligné peut :
- Nécessiter plusieurs cycles CPU
- Déclencher une exception matérielle
- Être émulé par le système (très lent)

#### 2. Portabilité limitée

Le comportement peut varier selon :
- **Le compilateur** (GCC, Clang, MSVC)
- **L'architecture** (x86 est tolérant, ARM est strict)
- **Le système d'exploitation**

#### 3. Problèmes avec les pointeurs

```c
struct __attribute__((packed)) Probleme {
    char c;
    int *ptr;  // Pointeur potentiellement désaligné !
};
```

Déréférencer un pointeur désaligné peut causer des bugs subtils.

---

## Cas d'usage typiques

### 1. Protocoles réseau

```c
// Paquet TCP simplifié
#pragma pack(push, 1)
struct TCPHeader {
    unsigned short source_port;      // 2 octets
    unsigned short dest_port;        // 2 octets
    unsigned int sequence_number;    // 4 octets
    unsigned int ack_number;         // 4 octets
    unsigned char flags;             // 1 octet
};
#pragma pack(pop)
// Taille : exactement 13 octets (sans padding parasite)
```

### 2. Lecture de fichiers binaires

```c
// En-tête de fichier WAV (audio)
#pragma pack(push, 1)
struct WAVHeader {
    char riff[4];           // "RIFF"
    unsigned int size;      // Taille du fichier
    char wave[4];           // "WAVE"
    char fmt[4];            // "fmt "
    unsigned int fmt_size;  // Taille de la section fmt
    // ... autres champs
};
#pragma pack(pop)

// Lecture directe depuis un fichier
FILE *f = fopen("audio.wav", "rb");  
struct WAVHeader header;  
fread(&header, sizeof(header), 1, f);  
// Les données correspondent exactement au format du fichier
```

### 3. Communication avec du matériel

```c
// Registres d'un capteur I2C
struct __attribute__((packed)) CapteurRegistres {
    unsigned char id;            // 1 octet : ID du capteur
    unsigned short temperature;  // 2 octets : température en dixièmes de degré
    unsigned char humidite;      // 1 octet : humidité en %
    unsigned char statut;        // 1 octet : flags de statut
};
// Taille : exactement 5 octets (mappe les registres du capteur)
```

### 4. Structures de données compactes

```c
// Arbre binaire avec économie de mémoire
struct __attribute__((packed)) Node {
    char valeur;           // 1 octet
    struct Node *gauche;   // 8 octets (sur 64 bits)
    struct Node *droite;   // 8 octets
};
// Taille : 17 octets au lieu de 24 (29% d'économie)
```

---

## Précautions et bonnes pratiques

### 1. Toujours documenter

```c
/*
 * ATTENTION : Structure packed !
 * Utilisée pour correspondre au format du protocole XYZ version 2.1
 * NE PAS MODIFIER sans mettre à jour la spécification
 */
#pragma pack(push, 1)
struct ProtocoleXYZ {
    unsigned char version;
    unsigned int timestamp;
    unsigned short data_length;
};
#pragma pack(pop)
```

### 2. Éviter pour les structures fréquemment accédées

```c
// ❌ Mauvais : structure accédée des millions de fois
struct __attribute__((packed)) MauvaisChoix {
    char id;
    int compteur;  // Accès lent à cause du désalignement
};

// ✅ Bon : structure normale pour les accès fréquents
struct BonChoix {
    char id;
    int compteur;  // Accès rapide grâce à l'alignement
};
```

### 3. Utiliser des types alignés quand possible

```c
// Au lieu de :
struct __attribute__((packed)) Sous_optimal {
    char c1;
    int i;
    char c2;
};

// Préférer :
struct __attribute__((packed)) Meilleur {
    int i;      // Naturellement aligné au début
    char c1;
    char c2;
};
// L'int reste aligné, seuls les char peuvent être désalignés (peu d'impact)
```

### 4. Tester sur l'architecture cible

```c
#include <stdio.h>

struct __attribute__((packed)) Test {
    char c;
    int i;
};

int main() {
    struct Test t = {'A', 42};

    // Vérifier l'adresse de l'int
    printf("Adresse de i : %p\n", (void*)&t.i);
    printf("Alignement : %zu\n", (size_t)&t.i % sizeof(int));

    if ((size_t)&t.i % sizeof(int) != 0) {
        printf("⚠️  L'int est désaligné !\n");
    }

    return 0;
}
```

### 5. Isolation dans des modules spécifiques

```c
// fichier: protocole.h
#pragma pack(push, 1)

struct PacketHeader {
    // ... définitions pour le protocole réseau
};

struct PacketData {
    // ...
};

#pragma pack(pop)

// Le reste du code utilise des structures normales
```

---

## Alternatives aux packed structures

### 1. Réorganisation des membres

Avant d'utiliser `packed`, essayez de réorganiser :

```c
// Au lieu de packer :
struct Avant {
    char c1;     // 1 + 3 padding
    int i1;      // 4
    char c2;     // 1 + 3 padding
    int i2;      // 4
};  // Total : 16 octets

// Réorganiser :
struct Apres {
    int i1;      // 4
    int i2;      // 4
    char c1;     // 1
    char c2;     // 1 + 2 padding
};  // Total : 12 octets (25% d'économie sans perte de performance)
```

### 2. Utiliser des types plus petits

```c
// Au lieu de :
struct Avant {
    int status;  // 0-255 suffisent
    int count;   // 0-100 suffisent
};

// Utiliser :
struct Apres {
    unsigned char status;  // 0-255
    unsigned char count;   // 0-255
};
// Pas besoin de packer, gain naturel
```

### 3. Bitfields (pour cas très spécifiques)

```c
// Pour des drapeaux booléens :
struct Flags {
    unsigned int flag1 : 1;  // 1 bit
    unsigned int flag2 : 1;  // 1 bit
    unsigned int flag3 : 1;  // 1 bit
    unsigned int value : 5;  // 5 bits
};  // Total : 1 octet (8 bits utilisés)
```

Nous verrons les bitfields en détail dans la section 10.7.

---

## Debugging et vérification

### Afficher la disposition mémoire

```c
#include <stdio.h>
#include <stddef.h>

struct Normal {
    char c;
    int i;
    short s;
};

struct __attribute__((packed)) Packed {
    char c;
    int i;
    short s;
};

void afficher_info(const char *nom, size_t taille, size_t offset_c, size_t offset_i, size_t offset_s) {
    printf("\n=== %s ===\n", nom);
    printf("Taille totale : %zu octets\n", taille);
    printf("Offset c      : %zu\n", offset_c);
    printf("Offset i      : %zu (padding : %zu)\n", offset_i, offset_i - offset_c - 1);
    printf("Offset s      : %zu (padding : %zu)\n", offset_s, offset_s - offset_i - 4);
}

int main() {
    afficher_info(
        "Structure normale",
        sizeof(struct Normal),
        offsetof(struct Normal, c),
        offsetof(struct Normal, i),
        offsetof(struct Normal, s)
    );

    afficher_info(
        "Structure packed",
        sizeof(struct Packed),
        offsetof(struct Packed, c),
        offsetof(struct Packed, i),
        offsetof(struct Packed, s)
    );

    return 0;
}
```

**Sortie** :
```
=== Structure normale ===
Taille totale : 12 octets  
Offset c      : 0  
Offset i      : 4 (padding : 3)  
Offset s      : 8 (padding : 0)  

=== Structure packed ===
Taille totale : 7 octets  
Offset c      : 0  
Offset i      : 1 (padding : 0)  
Offset s      : 5 (padding : 0)  
```

### Vérifier le support du compilateur

```c
#include <stdio.h>

struct Test {
    char c;
    int i;
} __attribute__((packed));

int main() {
    #ifdef __GNUC__
        printf("✅ GCC/Clang détecté\n");
        printf("   __attribute__((packed)) supporté\n");
    #endif

    #ifdef _MSC_VER
        printf("✅ MSVC détecté\n");
        printf("   #pragma pack supporté\n");
    #endif

    if (sizeof(struct Test) == 5) {
        printf("✅ Packing fonctionnel : %zu octets\n", sizeof(struct Test));
    } else {
        printf("❌ Packing non appliqué : %zu octets\n", sizeof(struct Test));
    }

    return 0;
}
```

---

## Erreurs courantes à éviter

### Erreur 1 : Oublier de restaurer l'alignement

```c
// ❌ Mauvais
#pragma pack(1)

struct Packed1 {
    char c;
    int i;
};

struct Packed2 {  // ⚠️ Packed aussi !
    int x;
    int y;
};
// Toutes les structures suivantes sont packed !

// ✅ Bon
#pragma pack(push, 1)

struct Packed1 {
    char c;
    int i;
};

#pragma pack(pop)  // Restaure l'alignement par défaut

struct Normal {    // ✅ Normale
    int x;
    int y;
};
```

### Erreur 2 : Mélanger les méthodes

```c
// ❌ Confusion
#pragma pack(1)
struct Test {
    char c;
    int i;
} __attribute__((packed));  // Redondant et potentiellement problématique
#pragma pack()

// ✅ Choisir une méthode
struct Test {
    char c;
    int i;
} __attribute__((packed));  // Une seule directive suffit
```

### Erreur 3 : Ignorer les pointeurs

```c
// ❌ Dangereux
struct __attribute__((packed)) Risque {
    char c;
    int *ptr;  // Pointeur potentiellement désaligné
};

void fonction(struct Risque *r) {
    *r->ptr = 42;  // ⚠️ Accès via pointeur désaligné
}

// ✅ Être conscient du problème
struct __attribute__((packed)) PlusSur {
    char c;
    int valeur;  // Valeur directe, pas de pointeur
};
```

---

## Exemple complet : Lecture d'un format de fichier

```c
#include <stdio.h>
#include <stdint.h>

// Format simplifié d'un fichier BMP
#pragma pack(push, 1)

struct BMPFileHeader {
    uint16_t type;       // "BM" en ASCII
    uint32_t size;       // Taille du fichier
    uint16_t reserved1;
    uint16_t reserved2;
    uint32_t offset;     // Offset des données image
};

struct BMPInfoHeader {
    uint32_t size;           // Taille de ce header
    int32_t width;           // Largeur en pixels
    int32_t height;          // Hauteur en pixels
    uint16_t planes;         // Toujours 1
    uint16_t bits_per_pixel; // 1, 4, 8, 16, 24 ou 32
};

#pragma pack(pop)

int main() {
    printf("=== VÉRIFICATION DES TAILLES ===\n\n");

    // Ces tailles doivent correspondre exactement au format BMP
    printf("BMPFileHeader : %zu octets (attendu : 14)\n", sizeof(struct BMPFileHeader));
    printf("BMPInfoHeader : %zu octets (attendu : 16)\n", sizeof(struct BMPInfoHeader));

    if (sizeof(struct BMPFileHeader) == 14 && sizeof(struct BMPInfoHeader) == 16) {
        printf("\n✅ Les structures sont correctement packed !\n");
        printf("   Elles correspondent au format BMP standard.\n");
    } else {
        printf("\n❌ Problème de packing détecté !\n");
    }

    // Exemple de lecture (simplifié)
    /*
    FILE *f = fopen("image.bmp", "rb");
    struct BMPFileHeader file_header;
    struct BMPInfoHeader info_header;

    fread(&file_header, sizeof(file_header), 1, f);
    fread(&info_header, sizeof(info_header), 1, f);

    printf("Image : %d × %d pixels\n", info_header.width, info_header.height);

    fclose(f);
    */

    return 0;
}
```

---

## Performance : Benchmark

```c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10000000

struct Normal {
    char c;
    int i;
};

struct __attribute__((packed)) Packed {
    char c;
    int i;
};

int main() {
    struct Normal *tab_normal = malloc(N * sizeof(struct Normal));
    struct Packed *tab_packed = malloc(N * sizeof(struct Packed));

    clock_t start, end;
    volatile int sum = 0;  // volatile pour empêcher l'optimisation

    // Benchmark accès normal
    start = clock();
    for (int i = 0; i < N; i++) {
        sum += tab_normal[i].i;
    }
    end = clock();
    double time_normal = ((double)(end - start)) / CLOCKS_PER_SEC;

    // Benchmark accès packed
    sum = 0;
    start = clock();
    for (int i = 0; i < N; i++) {
        sum += tab_packed[i].i;
    }
    end = clock();
    double time_packed = ((double)(end - start)) / CLOCKS_PER_SEC;

    printf("=== BENCHMARK (10 millions d'accès) ===\n\n");
    printf("Normal : %.3f secondes\n", time_normal);
    printf("Packed : %.3f secondes\n", time_packed);
    printf("Ratio  : %.2fx plus lent\n", time_packed / time_normal);

    printf("\n=== UTILISATION MÉMOIRE ===\n\n");
    size_t mem_normal = N * sizeof(struct Normal);
    size_t mem_packed = N * sizeof(struct Packed);

    printf("Normal : %.2f Mo\n", mem_normal / (1024.0 * 1024.0));
    printf("Packed : %.2f Mo\n", mem_packed / (1024.0 * 1024.0));
    printf("Gain   : %.2f Mo (%.1f%%)\n",
           (mem_normal - mem_packed) / (1024.0 * 1024.0),
           100.0 * (mem_normal - mem_packed) / mem_normal);

    free(tab_normal);
    free(tab_packed);

    return 0;
}
```

**Sortie typique (x86_64)** :
```
=== BENCHMARK (10 millions d'accès) ===

Normal : 0.023 secondes  
Packed : 0.034 secondes  
Ratio  : 1.48x plus lent  

=== UTILISATION MÉMOIRE ===

Normal : 76.29 Mo  
Packed : 47.68 Mo  
Gain   : 28.61 Mo (37.5%)  
```

**Conclusion** : 37% d'économie mémoire, mais 48% plus lent !

---

## Résumé

### Points clés

1. **Packed structures** éliminent le padding pour économiser de la mémoire
2. Deux méthodes principales : `__attribute__((packed))` et `#pragma pack`
3. **Compromis** : gain de mémoire vs perte de performance
4. **Cas d'usage** : formats binaires, protocoles réseau, hardware
5. **Précautions** : documenter, tester, éviter pour accès fréquents

### Quand utiliser packed structures ?

✅ **Utiliser** quand :
- Format binaire strict (fichiers, protocoles)
- Communication avec hardware
- Économie mémoire critique (systèmes embarqués)
- Données sérialisées

❌ **Éviter** quand :
- Structures accédées fréquemment
- Performance critique
- Pas de contrainte de format externe
- Réorganisation possible (alternative préférable)

### Commandes et syntaxes

```c
// Méthode 1 : Attribut GCC (recommandée)
struct __attribute__((packed)) NomStructure { ... };

// Méthode 2 : Pragma (compatible Microsoft)
#pragma pack(push, 1)
struct NomStructure { ... };
#pragma pack(pop)

// Vérification
sizeof(struct NomStructure)  
offsetof(struct NomStructure, membre)  
```

---

## À retenir

```c
// Structure normale : 8 octets avec padding
struct Normal {
    char c;    // 1 + 3 padding
    int i;     // 4
};

// Structure packed : 5 octets sans padding
struct __attribute__((packed)) Packed {
    char c;    // 1
    int i;     // 4 (désaligné !)
};

// Toujours documenter l'intention
#pragma pack(push, 1)
struct ProtocolHeader {
    /* Correspond au format du protocole XYZ */
    uint8_t version;
    uint32_t timestamp;
};
#pragma pack(pop)
```

**Règle d'or** : N'utilisez les packed structures que lorsque c'est **nécessaire** (formats externes stricts), pas pour optimiser de simples structures internes.

**Prochaine étape** : Nous allons découvrir les **unions** (section 10.4), qui permettent de partager la même zone mémoire entre plusieurs membres.

⏭️ [union : Optimisation mémoire](/10-types-utilisateur/04-unions.md)
