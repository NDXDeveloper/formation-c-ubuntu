🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 4.5 Endianness et portabilité

## Introduction

Jusqu'à présent, nous avons vu comment les types C stockent des valeurs, mais nous n'avons pas encore abordé un détail crucial : **dans quel ordre les octets sont-ils rangés en mémoire ?**

Cette question peut sembler triviale, mais elle a des implications majeures quand vous :
- Échangez des données entre différentes machines
- Lisez/écrivez des fichiers binaires
- Communiquez via des protocoles réseau
- Travaillez avec du matériel bas niveau

Cette section vous apprendra :
- Ce qu'est l'endianness (ou boutisme en français)
- Les différences entre big-endian et little-endian
- Comment détecter l'endianness de votre système
- Les pièges de portabilité et comment les éviter
- Les bonnes pratiques pour écrire du code portable

## Qu'est-ce que l'endianness ?

L'**endianness** (ou **boutisme**) désigne l'ordre dans lequel les octets d'un nombre multi-octets sont stockés en mémoire.

### Analogie : Écrire une date

Imaginez que vous voulez écrire la date **25 décembre 2025** :

- **Format français** : 25/12/2025 (jour/mois/année) — du plus petit au plus grand
- **Format américain** : 12/25/2025 (mois/jour/année) — ordre intermédiaire
- **Format ISO** : 2025-12-25 (année-mois-jour) — du plus grand au plus petit

Ces trois formats contiennent la **même information**, mais l'**ordre** diffère. C'est exactement le même principe avec l'endianness !

### Les deux ordres principaux

En informatique, il existe deux conventions principales pour stocker les octets :

1. **Little-Endian** : L'octet de **poids faible** (LSB - Least Significant Byte) est stocké en premier
2. **Big-Endian** : L'octet de **poids fort** (MSB - Most Significant Byte) est stocké en premier

## Exemple concret : Stocker le nombre 0x12345678

Prenons un entier sur 4 octets : `0x12345678` (en hexadécimal)

Décomposition en octets :
- Octet de poids fort (MSB) : `0x12`
- Octet suivant : `0x34`
- Octet suivant : `0x56`
- Octet de poids faible (LSB) : `0x78`

### Little-Endian (Intel x86/x64, ARM moderne)

**Ordre de stockage** : LSB en premier, MSB en dernier

```
Adresse mémoire :    0x1000   0x1001   0x1002   0x1003
Contenu :              78       56       34       12
                      ↑                            ↑
                     LSB                          MSB
```

**Lecture** : De gauche à droite, on lit `78 56 34 12`, mais la valeur est `0x12345678`

### Big-Endian (PowerPC, SPARC, réseaux)

**Ordre de stockage** : MSB en premier, LSB en dernier

```
Adresse mémoire :    0x1000   0x1001   0x1002   0x1003
Contenu :              12       34       56       78
                      ↑                            ↑
                     MSB                          LSB
```

**Lecture** : De gauche à droite, on lit `12 34 56 78`, ce qui correspond à `0x12345678`

### Visualisation comparative

```
Nombre : 0x12345678 (305 419 896 en décimal)

Little-Endian (x86) :
┌─────┬─────┬─────┬─────┐
│ 0x78│ 0x56│ 0x34│ 0x12│
└─────┴─────┴─────┴─────┘
  ↑                   ↑
 LSB                 MSB

Big-Endian (réseau) :
┌─────┬─────┬─────┬─────┐
│ 0x12│ 0x34│ 0x56│ 0x78│
└─────┴─────┴─────┴─────┘
  ↑                   ↑
 MSB                 LSB
```

## Systèmes courants et leur endianness

### Little-Endian (le plus répandu)

- **Processeurs Intel** (x86, x86-64)
- **AMD** (tous les processeurs)
- **ARM** moderne (Android, iOS — configurable mais généralement little)
- **RISC-V** (par défaut)

**Environ 90%+ des ordinateurs personnels et serveurs actuels**

### Big-Endian

- **Protocoles réseau** (TCP/IP) — appelé "Network Byte Order"
- **Anciens PowerPC** (Mac avant 2006)
- **SPARC** (serveurs Oracle/Sun)
- **Motorola 68k** (anciens Mac, Amiga)
- **Certains microcontrôleurs** embarqués

### Bi-Endian

Certains processeurs peuvent fonctionner dans les deux modes :
- **ARM** (configurable)
- **PowerPC** récents
- **MIPS**

## Pourquoi l'endianness existe-t-elle ?

### Raisons historiques

**Little-Endian** :
- Plus naturel pour l'électronique (traitement des LSB en premier)
- Simplifie certaines opérations arithmétiques au niveau matériel

**Big-Endian** :
- Plus intuitif pour les humains (lecture naturelle de gauche à droite)
- Simplifie les comparaisons de nombres multi-octets

### Analogie : Lire un livre

- **Big-Endian** : comme lire un livre occidental (de gauche à droite, du plus significatif au moins significatif)
- **Little-Endian** : comme lire un livre arabe (de droite à gauche pour nous, mais naturel pour l'arabe)

## Impact sur le code C

### Les types de 1 octet ne sont pas affectés

```c
char c = 'A';
unsigned char octet = 0xFF;
```

Un seul octet = pas de problème d'ordre !

### Les types multi-octets sont affectés

```c
int nombre = 0x12345678;  // 4 octets
short valeur = 0x1234;    // 2 octets
long long grand = 0x123456789ABCDEF0LL;  // 8 octets
```

**En mémoire, l'ordre des octets dépend de votre architecture.**

## Détecter l'endianness de votre système

### Méthode 1 : Union

```c
#include <stdio.h>

int main(void) {
    union {
        int entier;
        unsigned char octets[4];
    } test;

    test.entier = 0x12345678;

    printf("Premier octet en mémoire : 0x%02X\n", test.octets[0]);

    if (test.octets[0] == 0x78) {
        printf("Système Little-Endian\n");
    } else if (test.octets[0] == 0x12) {
        printf("Système Big-Endian\n");
    } else {
        printf("Endianness inconnue\n");
    }

    return 0;
}
```

**Sur un système x86** (Little-Endian) :
```
Premier octet en mémoire : 0x78
Système Little-Endian
```

### Méthode 2 : Pointeur

```c
#include <stdio.h>

int main(void) {
    int nombre = 0x12345678;
    unsigned char* ptr = (unsigned char*)&nombre;

    printf("Octets en mémoire : ");
    for (unsigned int i = 0; i < sizeof(int); i++) {
        printf("0x%02X ", ptr[i]);
    }
    printf("\n");

    if (ptr[0] == 0x78) {
        printf("Little-Endian\n");
    } else {
        printf("Big-Endian\n");
    }

    return 0;
}
```

**Sur x86** :
```
Octets en mémoire : 0x78 0x56 0x34 0x12
Little-Endian
```

### Méthode 3 : Macro prédéfinie

Certains systèmes définissent des macros :

```c
#include <stdio.h>

int main(void) {
    #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        printf("Little-Endian (détecté à la compilation)\n");
    #elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
        printf("Big-Endian (détecté à la compilation)\n");
    #else
        printf("Endianness inconnue\n");
    #endif

    return 0;
}
```

## Problèmes de portabilité

### Problème 1 : Fichiers binaires

**Scénario** : Vous écrivez un entier dans un fichier sur un système Little-Endian, puis le lisez sur un système Big-Endian.

```c
// Écriture sur système Little-Endian (x86)
int valeur = 0x12345678;
FILE* fichier = fopen("data.bin", "wb");
fwrite(&valeur, sizeof(int), 1, fichier);  // Écrit : 78 56 34 12
fclose(fichier);

// Lecture sur système Big-Endian
FILE* fichier2 = fopen("data.bin", "rb");
int lu;
fread(&lu, sizeof(int), 1, fichier2);  // Lit : 0x78563412 (INCORRECT !)
fclose(fichier2);

printf("Valeur lue : 0x%X\n", lu);  // Affiche : 0x78563412 au lieu de 0x12345678
```

**Le problème** : Les octets sont lus dans le mauvais ordre !

### Problème 2 : Réseaux

Les protocoles réseau utilisent **big-endian** (Network Byte Order). Si votre système est little-endian, vous devez convertir.

```c
// Sur un système Little-Endian
unsigned short port = 8080;  // En mémoire : 0x1F90 → 90 1F

// Sans conversion, envoyé sur le réseau : 90 1F
// Le destinataire lit : 0x901F = 36895 (INCORRECT !)
```

### Problème 3 : Cast de pointeurs

```c
int nombre = 0x12345678;
short* ptr_short = (short*)&nombre;

printf("Premier short : 0x%X\n", *ptr_short);
// Little-Endian : 0x5678
// Big-Endian : 0x1234
```

**Le résultat dépend de l'endianness !**

## Solutions pour la portabilité

### Solution 1 : Fonctions de conversion d'endianness

La bibliothèque standard POSIX fournit des fonctions de conversion :

```c
#include <arpa/inet.h>  // Linux/Unix
// Ou : #include <winsock2.h>  // Windows

// Host to Network (little → big si nécessaire)
uint16_t htons(uint16_t hostshort);   // Host TO Network Short
uint32_t htonl(uint32_t hostlong);    // Host TO Network Long

// Network to Host (big → little si nécessaire)
uint16_t ntohs(uint16_t netshort);    // Network TO Host Short
uint32_t ntohl(uint32_t netlong);     // Network TO Host Long
```

**Exemple réseau** :

```c
#include <stdio.h>
#include <arpa/inet.h>

int main(void) {
    uint16_t port = 8080;
    uint32_t adresse_ip = 0xC0A80001;  // 192.168.0.1

    // Conversion pour envoi réseau
    uint16_t port_reseau = htons(port);
    uint32_t ip_reseau = htonl(adresse_ip);

    printf("Port hôte : %u (0x%X)\n", port, port);
    printf("Port réseau : %u (0x%X)\n", ntohs(port_reseau), port_reseau);

    return 0;
}
```

### Solution 2 : Écrire/Lire octet par octet

Pour les fichiers binaires portables :

```c
#include <stdio.h>
#include <stdint.h>

// Écriture portable d'un entier 32 bits (big-endian dans le fichier)
void ecrire_int32_be(FILE* f, uint32_t valeur) {
    unsigned char octets[4];
    octets[0] = (valeur >> 24) & 0xFF;  // MSB
    octets[1] = (valeur >> 16) & 0xFF;
    octets[2] = (valeur >> 8) & 0xFF;
    octets[3] = valeur & 0xFF;          // LSB
    fwrite(octets, 1, 4, f);
}

// Lecture portable d'un entier 32 bits (big-endian dans le fichier)
uint32_t lire_int32_be(FILE* f) {
    unsigned char octets[4];
    fread(octets, 1, 4, f);
    return ((uint32_t)octets[0] << 24) |
           ((uint32_t)octets[1] << 16) |
           ((uint32_t)octets[2] << 8) |
           ((uint32_t)octets[3]);
}

int main(void) {
    FILE* fichier = fopen("portable.bin", "wb");
    ecrire_int32_be(fichier, 0x12345678);
    fclose(fichier);

    fichier = fopen("portable.bin", "rb");
    uint32_t valeur = lire_int32_be(fichier);
    fclose(fichier);

    printf("Valeur lue : 0x%X\n", valeur);  // Toujours 0x12345678

    return 0;
}
```

### Solution 3 : Macros de conversion personnalisées

Si `<arpa/inet.h>` n'est pas disponible :

```c
#include <stdint.h>

// Détection de l'endianness à la compilation
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
    #define IS_LITTLE_ENDIAN 1
#else
    #define IS_LITTLE_ENDIAN 0
#endif

// Inversion d'octets
#define SWAP16(x) ((((x) & 0xFF00) >> 8) | (((x) & 0x00FF) << 8))

#define SWAP32(x) ((((x) & 0xFF000000) >> 24) | \
                   (((x) & 0x00FF0000) >> 8)  | \
                   (((x) & 0x0000FF00) << 8)  | \
                   (((x) & 0x000000FF) << 24))

// Conversion conditionelle
#if IS_LITTLE_ENDIAN
    #define HOST_TO_BE16(x) SWAP16(x)
    #define HOST_TO_BE32(x) SWAP32(x)
    #define BE_TO_HOST16(x) SWAP16(x)
    #define BE_TO_HOST32(x) SWAP32(x)
#else
    #define HOST_TO_BE16(x) (x)
    #define HOST_TO_BE32(x) (x)
    #define BE_TO_HOST16(x) (x)
    #define BE_TO_HOST32(x) (x)
#endif
```

**Utilisation** :

```c
uint32_t valeur = 0x12345678;
uint32_t valeur_be = HOST_TO_BE32(valeur);  // Converti en big-endian si nécessaire

// Écriture dans un fichier au format big-endian
fwrite(&valeur_be, sizeof(uint32_t), 1, fichier);
```

## Endianness et structures

L'endianness affecte aussi les structures :

```c
struct Paquet {
    uint16_t type;      // 2 octets
    uint16_t longueur;  // 2 octets
    uint32_t donnees;   // 4 octets
};

struct Paquet p = {0x1234, 0x5678, 0x9ABCDEF0};

// Représentation mémoire Little-Endian :
// 34 12 78 56 F0 DE BC 9A

// Représentation mémoire Big-Endian :
// 12 34 56 78 9A BC DE F0
```

**Pour la portabilité**, convertissez chaque champ individuellement :

```c
#include <arpa/inet.h>

struct Paquet p_hote = {0x1234, 0x5678, 0x9ABCDEF0};

// Conversion pour envoi réseau
struct Paquet p_reseau;
p_reseau.type = htons(p_hote.type);
p_reseau.longueur = htons(p_hote.longueur);
p_reseau.donnees = htonl(p_hote.donnees);

// Envoi sur le réseau
send(socket, &p_reseau, sizeof(p_reseau), 0);
```

## Cas pratiques : Protocoles et formats

### Protocole réseau (TCP/IP)

Les protocoles réseau utilisent **toujours** big-endian :

```c
// En-tête TCP simplifié
struct TCP_Header {
    uint16_t port_source;       // 2 octets
    uint16_t port_destination;  // 2 octets
    uint32_t numero_sequence;   // 4 octets
    // ...
};

// Préparation pour envoi
struct TCP_Header entete;
entete.port_source = htons(12345);
entete.port_destination = htons(80);
entete.numero_sequence = htonl(1000000);
```

### Format de fichier (BMP, WAV, etc.)

Beaucoup de formats spécifient l'endianness :

- **BMP** : Little-Endian
- **TIFF** : Peut être les deux (indiqué dans l'en-tête)
- **PNG** : Big-Endian

```c
// Lecture d'un en-tête BMP (little-endian)
struct BMP_Header {
    uint16_t signature;  // 'BM'
    uint32_t taille_fichier;
    // ...
};

// Sur un système little-endian : lecture directe
fread(&header, sizeof(struct BMP_Header), 1, fichier);

// Sur un système big-endian : conversion nécessaire
fread(&header, sizeof(struct BMP_Header), 1, fichier);
header.taille_fichier = SWAP32(header.taille_fichier);
```

## Bonnes pratiques pour la portabilité

### 1. Utilisez les types à largeur fixe

```c
#include <stdint.h>

// ✅ BON : taille garantie sur toutes les plateformes
uint32_t valeur = 0x12345678;

// ❌ MAUVAIS : taille variable selon la plateforme
unsigned long valeur = 0x12345678;  // 4 ou 8 octets ?
```

### 2. Spécifiez l'endianness des données persistantes

```c
// Format de fichier : big-endian
// Version du format : 1
struct FileHeader {
    uint32_t magic;      // 0x12345678 (big-endian)
    uint16_t version;    // 1 (big-endian)
    // ...
} __attribute__((packed));
```

### 3. Documentez vos choix

```c
// Cette structure est stockée en BIG-ENDIAN dans les fichiers
// Conversion nécessaire sur les systèmes little-endian
struct ConfigFile {
    uint32_t timestamp;  // Big-endian
    uint16_t flags;      // Big-endian
};
```

### 4. Ne faites pas confiance aux cast de pointeurs

```c
// ❌ DÉPENDANT DE L'ENDIANNESS
int nombre = 0x12345678;
char premier_octet = *((char*)&nombre);  // 0x78 ou 0x12 ?

// ✅ PORTABLE
unsigned char octets[4];
memcpy(octets, &nombre, 4);
// Ensuite traitez octet par octet selon l'endianness voulue
```

### 5. Testez sur différentes architectures

Si possible, testez votre code sur :
- **x86/x64** (little-endian)
- **ARM** big-endian si accessible
- **Émulateur QEMU** avec big-endian

## Outils de détection et débogage

### 1. Affichage hexadécimal en mémoire

```c
#include <stdio.h>

void afficher_octets(void* ptr, size_t taille) {
    unsigned char* p = (unsigned char*)ptr;
    printf("Octets en mémoire : ");
    for (size_t i = 0; i < taille; i++) {
        printf("%02X ", p[i]);
    }
    printf("\n");
}

int main(void) {
    int nombre = 0x12345678;
    afficher_octets(&nombre, sizeof(nombre));
    // Little-endian : 78 56 34 12
    // Big-endian : 12 34 56 78
    return 0;
}
```

### 2. Utilisation de `hexdump` (Linux)

```bash
# Créer un fichier avec un entier
echo -ne '\x12\x34\x56\x78' > test.bin

# Afficher en hexadécimal
hexdump -C test.bin
# Affiche : 00000000  12 34 56 78
```

### 3. GDB pour inspecter la mémoire

```bash
(gdb) x/4xb &nombre
# Affiche les 4 octets en hexadécimal
```

## Exemple complet : Portabilité réseau

```c
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

// Simulation d'un paquet réseau
struct Paquet {
    uint16_t type;
    uint16_t longueur;
    uint32_t id_session;
    uint32_t timestamp;
};

// Préparation pour envoi (host → network)
void preparer_envoi(struct Paquet* p) {
    p->type = htons(p->type);
    p->longueur = htons(p->longueur);
    p->id_session = htonl(p->id_session);
    p->timestamp = htonl(p->timestamp);
}

// Traitement après réception (network → host)
void traiter_reception(struct Paquet* p) {
    p->type = ntohs(p->type);
    p->longueur = ntohs(p->longueur);
    p->id_session = ntohl(p->id_session);
    p->timestamp = ntohl(p->timestamp);
}

int main(void) {
    // Création d'un paquet
    struct Paquet paquet_hote = {
        .type = 0x0001,
        .longueur = 100,
        .id_session = 0x12345678,
        .timestamp = 1704067200
    };

    printf("=== Paquet original (format hôte) ===\n");
    printf("Type: 0x%04X\n", paquet_hote.type);
    printf("Longueur: %u\n", paquet_hote.longueur);
    printf("ID Session: 0x%08X\n", paquet_hote.id_session);
    printf("Timestamp: %u\n\n", paquet_hote.timestamp);

    // Préparation pour envoi réseau
    struct Paquet paquet_reseau = paquet_hote;
    preparer_envoi(&paquet_reseau);

    printf("=== Paquet préparé (format réseau) ===\n");
    printf("Octets bruts : ");
    unsigned char* ptr = (unsigned char*)&paquet_reseau;
    for (size_t i = 0; i < sizeof(paquet_reseau); i++) {
        printf("%02X ", ptr[i]);
    }
    printf("\n\n");

    // Simulation de réception
    struct Paquet paquet_recu = paquet_reseau;
    traiter_reception(&paquet_recu);

    printf("=== Paquet reçu (format hôte) ===\n");
    printf("Type: 0x%04X\n", paquet_recu.type);
    printf("Longueur: %u\n", paquet_recu.longueur);
    printf("ID Session: 0x%08X\n", paquet_recu.id_session);
    printf("Timestamp: %u\n", paquet_recu.timestamp);

    return 0;
}
```

## Points clés à retenir

🔹 **Endianness** : ordre de stockage des octets en mémoire

🔹 **Little-Endian** : LSB en premier (Intel, AMD, ARM moderne)

🔹 **Big-Endian** : MSB en premier (réseau, anciens systèmes)

🔹 **Types 1 octet** : pas affectés par l'endianness

🔹 **Réseau** : toujours big-endian (Network Byte Order)

🔹 **Portabilité** : utilisez `htons()`, `htonl()`, `ntohs()`, `ntohl()`

🔹 **Fichiers binaires** : spécifiez l'endianness, lisez/écrivez octet par octet

🔹 **Test** : détectez l'endianness avec une union ou un cast de pointeur

🔹 **Types fixes** : préférez `uint32_t` à `unsigned long`

## Ce que nous verrons ensuite

Dans la prochaine section, nous explorerons :
- **4.6 Nouveautés C23** : Les dernières évolutions du standard C

Après avoir terminé le Module 4, vous passerez au **Module 5 : Opérateurs et Structures de contrôle**, où nous verrons comment manipuler et transformer ces données avec des opérations arithmétiques, logiques et bit-à-bit.

---

**🎯 Vous comprenez maintenant l'endianness et ses implications ! Cette connaissance est essentielle pour écrire du code portable et communiquer avec d'autres systèmes.**

⏭️ [Nouveautés C23](/04-types-et-memoire/06-nouveautes-c23.md)
