🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 10.7 Structures bit-fields

## Introduction

Imaginez que vous devez stocker plusieurs drapeaux booléens (vrai/faux) ou des petites valeurs numériques. Vous pourriez utiliser des variables séparées :

```c
struct Flags {
    int actif;      // 4 octets pour stocker 0 ou 1
    int visible;    // 4 octets pour stocker 0 ou 1
    int modifie;    // 4 octets pour stocker 0 ou 1
};
// Total : 12 octets pour 3 bits d'information !
```

**Problème** : Énorme gaspillage de mémoire.

**Solution** : Les **bit-fields** (champs de bits) permettent de spécifier combien de bits chaque membre d'une structure doit occuper.

```c
struct Flags {
    unsigned int actif   : 1;  // 1 bit seulement
    unsigned int visible : 1;  // 1 bit
    unsigned int modifie : 1;  // 1 bit
};
// Total : 4 octets (mais seulement 3 bits utilisés)
```

---

## Qu'est-ce qu'un bit-field ?

### Définition

Un **bit-field** est un membre d'une structure qui occupe un nombre spécifique de bits (au lieu d'octets complets).

### Syntaxe

```c
struct NomStructure {
    type nom_membre : nombre_de_bits;
};
```

- **type** : Généralement `unsigned int` ou `int` (certains compilateurs acceptent `char`)
- **nombre_de_bits** : Nombre de bits à allouer (1 à la taille du type)

### Exemple simple

```c
#include <stdio.h>

struct Drapeaux {
    unsigned int flag1 : 1;  // 1 bit : 0 ou 1
    unsigned int flag2 : 1;  // 1 bit : 0 ou 1
    unsigned int flag3 : 1;  // 1 bit : 0 ou 1
};

int main() {
    struct Drapeaux f = {0};

    printf("Taille : %zu octet(s)\n", sizeof(struct Drapeaux));

    f.flag1 = 1;
    f.flag2 = 0;
    f.flag3 = 1;

    printf("flag1 : %u\n", f.flag1);
    printf("flag2 : %u\n", f.flag2);
    printf("flag3 : %u\n", f.flag3);

    return 0;
}
```

**Sortie** :
```
Taille : 4 octet(s)  
flag1 : 1  
flag2 : 0  
flag3 : 1  
```

---

## Représentation en mémoire

### Structure normale vs bit-fields

**Structure normale** :
```c
struct Normal {
    int a;  // 4 octets
    int b;  // 4 octets
    int c;  // 4 octets
};
// Total : 12 octets
```

**Avec bit-fields** :
```c
struct BitFields {
    unsigned int a : 4;  // 4 bits
    unsigned int b : 4;  // 4 bits
    unsigned int c : 4;  // 4 bits
};
// Total : 4 octets (12 bits utilisés sur 32)
```

### Visualisation

```
Mémoire (4 octets = 32 bits) :

+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
|   c   |   b   |   a   | ... bits inutilisés ...                |
+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+---+
  4 bits  4 bits  4 bits       20 bits non utilisés
```

---

## Valeurs possibles selon le nombre de bits

### Unsigned (non signé)

| Bits | Valeurs possibles | Plage |
|------|-------------------|-------|
| 1    | 2 valeurs         | 0 à 1 |
| 2    | 4 valeurs         | 0 à 3 |
| 3    | 8 valeurs         | 0 à 7 |
| 4    | 16 valeurs        | 0 à 15 |
| 8    | 256 valeurs       | 0 à 255 |

**Formule** : Pour n bits → 2^n valeurs possibles (de 0 à 2^n - 1)

### Signed (signé)

Pour les champs signés, la plage est divisée entre négatifs et positifs :

| Bits | Plage |
|------|-------|
| 2    | -2 à 1 |
| 3    | -4 à 3 |
| 4    | -8 à 7 |
| 8    | -128 à 127 |

### Exemple

```c
#include <stdio.h>

struct Valeurs {
    unsigned int petit : 3;  // 0 à 7
    signed int signe   : 4;  // -8 à 7
};

int main() {
    struct Valeurs v;

    v.petit = 7;   // OK
    v.signe = -5;  // OK

    printf("petit : %u\n", v.petit);
    printf("signe : %d\n", v.signe);

    v.petit = 8;   // ⚠️ Dépassement : 8 ne tient pas sur 3 bits
    printf("petit après dépassement : %u\n", v.petit);  // 0 (8 % 8)

    return 0;
}
```

---

## Cas d'usage typiques

### 1. Drapeaux et options

```c
#include <stdio.h>

struct OptionsAffichage {
    unsigned int gras      : 1;
    unsigned int italique  : 1;
    unsigned int souligne  : 1;
    unsigned int barree    : 1;
};

void afficher_options(struct OptionsAffichage opt) {
    printf("Options actives : ");
    if (opt.gras)     printf("Gras ");
    if (opt.italique) printf("Italique ");
    if (opt.souligne) printf("Souligné ");
    if (opt.barree)   printf("Barré ");
    printf("\n");
}

int main() {
    struct OptionsAffichage texte = {
        .gras = 1,
        .italique = 1,
        .souligne = 0,
        .barree = 0
    };

    printf("Taille : %zu octet(s)\n", sizeof(struct OptionsAffichage));
    afficher_options(texte);

    // Modifier une option
    texte.souligne = 1;
    afficher_options(texte);

    return 0;
}
```

**Sortie** :
```
Taille : 4 octet(s)  
Options actives : Gras Italique  
Options actives : Gras Italique Souligné  
```

### 2. Registres de périphériques matériels

```c
// Registre de contrôle d'un capteur I2C
struct CapteurRegistre {
    unsigned int enable       : 1;  // Activer/désactiver
    unsigned int mode         : 2;  // 4 modes possibles (0-3)
    unsigned int resolution   : 3;  // 8 résolutions (0-7)
    unsigned int interruption : 1;  // Activer interruption
    unsigned int reserve      : 1;  // Réservé (inutilisé)
};

void configurer_capteur(struct CapteurRegistre *reg) {
    reg->enable = 1;
    reg->mode = 2;         // Mode "haute précision"
    reg->resolution = 5;   // Résolution maximale
    reg->interruption = 0;
}
```

### 3. Formats de protocoles réseau

```c
#include <stdio.h>

// En-tête TCP simplifié
struct TCPFlags {
    unsigned int fin : 1;  // Fin de connexion
    unsigned int syn : 1;  // Synchronisation
    unsigned int rst : 1;  // Reset
    unsigned int psh : 1;  // Push
    unsigned int ack : 1;  // Acknowledgment
    unsigned int urg : 1;  // Urgent
    unsigned int ece : 1;  // ECN-Echo
    unsigned int cwr : 1;  // Congestion Window Reduced
};

void afficher_flags(struct TCPFlags flags) {
    printf("Flags TCP : ");
    if (flags.fin) printf("FIN ");
    if (flags.syn) printf("SYN ");
    if (flags.rst) printf("RST ");
    if (flags.psh) printf("PSH ");
    if (flags.ack) printf("ACK ");
    if (flags.urg) printf("URG ");
    if (flags.ece) printf("ECE ");
    if (flags.cwr) printf("CWR ");
    printf("\n");
}

int main() {
    // Paquet SYN (début de connexion)
    struct TCPFlags syn_packet = {
        .syn = 1
    };

    // Paquet SYN-ACK (réponse)
    struct TCPFlags synack_packet = {
        .syn = 1,
        .ack = 1
    };

    afficher_flags(syn_packet);
    afficher_flags(synack_packet);

    return 0;
}
```

### 4. Compression de données

```c
#include <stdio.h>

// Stocker les informations d'un pixel RGB565 (16 bits au lieu de 24)
struct Pixel565 {
    unsigned int rouge : 5;  // 5 bits : 0-31
    unsigned int vert  : 6;  // 6 bits : 0-63
    unsigned int bleu  : 5;  // 5 bits : 0-31
};
// 16 bits de données utiles (mais sizeof = 4 car type unsigned int)

int main() {
    struct Pixel565 pixel = {31, 63, 31};  // Blanc (valeurs max)

    printf("Taille : %zu octets\n", sizeof(struct Pixel565));  // 4
    printf("Rouge : %u, Vert : %u, Bleu : %u\n",
           pixel.rouge, pixel.vert, pixel.bleu);

    return 0;
}
```

### 5. État d'un jeu

```c
#include <stdio.h>

struct EtatJoueur {
    unsigned int vie         : 7;   // 0-127 points de vie
    unsigned int niveau      : 5;   // 0-31 niveaux
    unsigned int invincible  : 1;   // Booléen
    unsigned int ennemis_tues: 10;  // 0-1023 ennemis
    unsigned int arme        : 3;   // 8 types d'armes (0-7)
};

void afficher_joueur(struct EtatJoueur j) {
    printf("=== ÉTAT DU JOUEUR ===\n");
    printf("Vie : %u/127\n", j.vie);
    printf("Niveau : %u\n", j.niveau);
    printf("Invincible : %s\n", j.invincible ? "Oui" : "Non");
    printf("Ennemis tués : %u\n", j.ennemis_tues);
    printf("Arme : %u\n", j.arme);
    printf("Taille : %zu octets\n", sizeof(struct EtatJoueur));
}

int main() {
    struct EtatJoueur joueur = {
        .vie = 100,
        .niveau = 5,
        .invincible = 0,
        .ennemis_tues = 42,
        .arme = 3
    };

    afficher_joueur(joueur);

    return 0;
}
```

---

## Mélanger bit-fields et membres normaux

```c
#include <stdio.h>

struct InfoFichier {
    char nom[32];              // Membre normal (32 octets)
    unsigned int lecture  : 1; // Bit-field
    unsigned int ecriture : 1; // Bit-field
    unsigned int execution: 1; // Bit-field
    unsigned int archive  : 1; // Bit-field
    unsigned long taille;      // Membre normal (8 octets sur 64 bits)
};

int main() {
    struct InfoFichier fichier = {
        .nom = "document.txt",
        .lecture = 1,
        .ecriture = 1,
        .execution = 0,
        .archive = 0,
        .taille = 2048
    };

    printf("Fichier : %s\n", fichier.nom);
    printf("Permissions : %c%c%c\n",
           fichier.lecture   ? 'r' : '-',
           fichier.ecriture  ? 'w' : '-',
           fichier.execution ? 'x' : '-');
    printf("Taille : %lu octets\n", fichier.taille);
    printf("Taille structure : %zu octets\n", sizeof(struct InfoFichier));

    return 0;
}
```

---

## Bit-fields anonymes et padding

### Bit-fields anonymes

Utilisés pour créer du padding (espacement) :

```c
struct Registre {
    unsigned int actif  : 1;   // Bit 0
    unsigned int        : 3;   // Bits 1-3 inutilisés (padding)
    unsigned int mode   : 2;   // Bits 4-5
    unsigned int        : 2;   // Bits 6-7 inutilisés
};
```

### Forcer un alignement

Un bit-field de taille 0 force l'alignement sur l'unité suivante :

```c
struct Alignement {
    unsigned int a : 1;
    unsigned int   : 0;  // Force début d'une nouvelle unité
    unsigned int b : 1;
};
```

---

## Limitations et précautions

### 1. Pas d'adresse directe

On **ne peut pas** prendre l'adresse d'un bit-field :

```c
struct Test {
    unsigned int flag : 1;
};

struct Test t;
// unsigned int *ptr = &t.flag;  // ❌ ERREUR de compilation
```

### 2. Pas de tableaux de bit-fields

```c
struct Test {
    unsigned int flags[8] : 1;  // ❌ ERREUR : syntaxe invalide
};

// Solution : utiliser plusieurs champs
struct Test {
    unsigned int flag0 : 1;
    unsigned int flag1 : 1;
    unsigned int flag2 : 1;
    // ... etc
};
```

### 3. Ordre des bits non garanti

L'ordre dans lequel les bits sont stockés dépend de l'architecture :

```c
struct Ordre {
    unsigned int a : 4;
    unsigned int b : 4;
};

// Sur une architecture, 'a' peut être dans les 4 bits de poids faible
// Sur une autre, 'a' peut être dans les 4 bits de poids fort
```

### 4. Portabilité limitée

```c
// ⚠️ Comportement dépendant du compilateur et de l'architecture
struct NonPortable {
    int signe : 1;  // Certains compilateurs interdisent int signé avec 1 bit
};
```

**Recommandation** : Toujours utiliser `unsigned int` pour les bit-fields.

### 5. Performance

L'accès aux bit-fields peut être **plus lent** que l'accès à des octets complets :

```c
// Accès à un bit-field nécessite :
// 1. Lire l'octet/mot entier
// 2. Masquer les bits
// 3. Décaler si nécessaire
```

---

## Bit-fields vs masques binaires

### Avec masques binaires

```c
#define FLAG_LECTURE   0x01  // Bit 0
#define FLAG_ECRITURE  0x02  // Bit 1
#define FLAG_EXECUTION 0x04  // Bit 2

unsigned char permissions = 0;

// Activer un flag
permissions |= FLAG_LECTURE;  
permissions |= FLAG_ECRITURE;  

// Vérifier un flag
if (permissions & FLAG_LECTURE) {
    printf("Lecture autorisée\n");
}

// Désactiver un flag
permissions &= ~FLAG_ECRITURE;
```

### Avec bit-fields

```c
struct {
    unsigned char lecture   : 1;
    unsigned char ecriture  : 1;
    unsigned char execution : 1;
} permissions = {0};

// Activer un flag
permissions.lecture = 1;  
permissions.ecriture = 1;  

// Vérifier un flag
if (permissions.lecture) {
    printf("Lecture autorisée\n");
}

// Désactiver un flag
permissions.ecriture = 0;
```

### Comparaison

| Critère | Masques binaires | Bit-fields |
|---------|------------------|------------|
| **Lisibilité** | Moyenne | ✅ Excellente |
| **Performance** | ✅ Rapide | Légèrement plus lent |
| **Portabilité** | ✅ Excellente | Moyenne |
| **Contrôle précis** | ✅ Total | Limité |
| **Adresse des bits** | ✅ Possible (de l'octet) | ❌ Impossible |

---

## Exemple avancé : Configuration système

```c
#include <stdio.h>
#include <string.h>

typedef struct {
    unsigned int verbose         : 1;  // Mode verbeux
    unsigned int debug           : 1;  // Mode debug
    unsigned int couleur         : 1;  // Activer couleurs
    unsigned int niveau_log      : 3;  // 0-7 niveaux
    unsigned int threads         : 4;  // 1-15 threads
    unsigned int cache_actif     : 1;  // Utiliser cache
    unsigned int compression     : 2;  // 0=aucune, 1=rapide, 2=max
    unsigned int protocole       : 3;  // Version de protocole (0-7)
} Configuration;

void afficher_config(Configuration *cfg) {
    printf("=== CONFIGURATION SYSTÈME ===\n");
    printf("Verbose       : %s\n", cfg->verbose ? "Oui" : "Non");
    printf("Debug         : %s\n", cfg->debug ? "Oui" : "Non");
    printf("Couleur       : %s\n", cfg->couleur ? "Oui" : "Non");
    printf("Niveau log    : %u\n", cfg->niveau_log);
    printf("Threads       : %u\n", cfg->threads);
    printf("Cache         : %s\n", cfg->cache_actif ? "Actif" : "Inactif");

    const char *compression[] = {"Aucune", "Rapide", "Maximale"};
    printf("Compression   : %s\n", compression[cfg->compression]);

    printf("Protocole     : v%u\n", cfg->protocole);
    printf("Taille config : %zu octets\n", sizeof(Configuration));
}

void config_par_defaut(Configuration *cfg) {
    memset(cfg, 0, sizeof(Configuration));
    cfg->couleur = 1;
    cfg->niveau_log = 3;
    cfg->threads = 4;
    cfg->cache_actif = 1;
    cfg->compression = 1;
    cfg->protocole = 2;
}

int main() {
    Configuration cfg;
    config_par_defaut(&cfg);
    afficher_config(&cfg);

    printf("\n=== Activation mode debug ===\n");
    cfg.debug = 1;
    cfg.verbose = 1;
    cfg.niveau_log = 7;
    afficher_config(&cfg);

    return 0;
}
```

**Sortie** :
```
=== CONFIGURATION SYSTÈME ===
Verbose       : Non  
Debug         : Non  
Couleur       : Oui  
Niveau log    : 3  
Threads       : 4  
Cache         : Actif  
Compression   : Rapide  
Protocole     : v2  
Taille config : 4 octets  

=== Activation mode debug ===
=== CONFIGURATION SYSTÈME ===
Verbose       : Oui  
Debug         : Oui  
Couleur       : Oui  
Niveau log    : 7  
Threads       : 4  
Cache         : Actif  
Compression   : Rapide  
Protocole     : v2  
Taille config : 4 octets  
```

---

## Debugging et inspection

### Afficher les valeurs en binaire

```c
#include <stdio.h>

void afficher_bits(unsigned int valeur, int nb_bits) {
    for (int i = nb_bits - 1; i >= 0; i--) {
        printf("%u", (valeur >> i) & 1);
    }
}

struct Bits {
    unsigned int a : 4;
    unsigned int b : 4;
    unsigned int c : 4;
};

int main() {
    struct Bits b = {5, 10, 15};

    printf("a = %u (binaire : ", b.a);
    afficher_bits(b.a, 4);
    printf(")\n");

    printf("b = %u (binaire : ", b.b);
    afficher_bits(b.b, 4);
    printf(")\n");

    printf("c = %u (binaire : ", b.c);
    afficher_bits(b.c, 4);
    printf(")\n");

    // Afficher la structure entière en mémoire
    printf("\nMémoire complète : ");
    unsigned char *ptr = (unsigned char *)&b;
    for (size_t i = 0; i < sizeof(b); i++) {
        afficher_bits(ptr[i], 8);
        printf(" ");
    }
    printf("\n");

    return 0;
}
```

---

## Bonnes pratiques

### 1. Toujours utiliser unsigned

```c
// ❌ Mauvais : comportement imprévisible avec signed
struct Mauvais {
    int flag : 1;  // Peut être -1 ou 0 (pas 0 ou 1 !)
};

// ✅ Bon : comportement prévisible
struct Bon {
    unsigned int flag : 1;  // 0 ou 1
};
```

### 2. Documenter les plages de valeurs

```c
struct Config {
    unsigned int mode  : 2;  // 0-3 : sleep, active, turbo, max
    unsigned int vitesse : 4;  // 0-15 : vitesse du ventilateur
};
```

### 3. Utiliser pour économiser de la mémoire critique

```c
// ✅ Bon usage : milliers d'instances en mémoire
struct Particule {
    unsigned int type    : 3;  // 8 types
    unsigned int couleur : 5;  // 32 couleurs
    unsigned int visible : 1;
    float x, y;  // Position
};

// Particule particules[100000];  // Gain de mémoire significatif
```

### 4. Ne pas utiliser pour optimisation prématurée

```c
// ❌ Mauvais : pas de gain significatif
struct PetiteStruct {
    unsigned int a : 1;
    unsigned int b : 1;
};
PetiteStruct unique;  // Une seule instance

// ✅ Mieux : structure normale plus simple
struct PetiteStruct {
    unsigned int a;
    unsigned int b;
};
```

### 5. Attention à la portabilité

```c
// ⚠️ Ordre des bits non garanti
struct NonPortable {
    unsigned int a : 4;
    unsigned int b : 4;
};

// Si portabilité critique, utiliser des masques explicites
```

---

## Alternatives aux bit-fields

### Option 1 : Opérateurs bit-à-bit

```c
#define BIT_0 (1 << 0)
#define BIT_1 (1 << 1)
#define BIT_2 (1 << 2)

unsigned char flags = 0;  
flags |= BIT_0;              // Activer bit 0  
if (flags & BIT_1) { }       // Tester bit 1  
flags &= ~BIT_2;             // Désactiver bit 2  
```

### Option 2 : Fonctions d'accès

```c
typedef unsigned int Flags;

#define FLAG_ACTIF   0
#define FLAG_VISIBLE 1

void set_flag(Flags *f, int bit) {
    *f |= (1 << bit);
}

void clear_flag(Flags *f, int bit) {
    *f &= ~(1 << bit);
}

int get_flag(Flags f, int bit) {
    return (f >> bit) & 1;
}
```

---

## Résumé

### Points clés

1. **Bit-fields** : Permettent de spécifier le nombre de bits pour chaque membre
2. **Syntaxe** : `type membre : nombre_bits;`
3. **Usage** : Économie de mémoire pour drapeaux, registres hardware, protocoles
4. **Limitation** : Pas d'adresse, portabilité limitée, performance potentiellement moindre
5. **Type recommandé** : `unsigned int`

### Quand utiliser les bit-fields ?

✅ **Utiliser** pour :
- Registres de périphériques matériels
- Protocoles réseau avec formats stricts
- Drapeaux multiples dans une structure très utilisée
- Formats de données compressés (ex: RGB565)
- Économie de mémoire critique (systèmes embarqués)

❌ **Éviter** pour :
- Code nécessitant une portabilité maximale
- Structures rarement utilisées (pas de gain significatif)
- Quand la performance est critique
- Quand vous avez besoin de l'adresse des membres

### Syntaxe essentielle

```c
// Déclaration
struct Flags {
    unsigned int actif   : 1;  // 1 bit
    unsigned int mode    : 2;  // 2 bits (0-3)
    unsigned int niveau  : 4;  // 4 bits (0-15)
};

// Utilisation
struct Flags f = {0};  
f.actif = 1;  
f.mode = 2;  
f.niveau = 10;  

// Vérification
if (f.actif) {
    printf("Actif !\n");
}

// Taille
printf("%zu octets\n", sizeof(struct Flags));  // Généralement 4
```

---

## À retenir

```c
// ❌ Gaspillage de mémoire
struct Mauvais {
    int flag1;     // 4 octets pour 0 ou 1
    int flag2;     // 4 octets pour 0 ou 1
    int flag3;     // 4 octets pour 0 ou 1
    int valeur;    // 4 octets pour 0-15
};  // Total : 16 octets

// ✅ Optimisé avec bit-fields
struct Bon {
    unsigned int flag1  : 1;  // 1 bit
    unsigned int flag2  : 1;  // 1 bit
    unsigned int flag3  : 1;  // 1 bit
    unsigned int valeur : 4;  // 4 bits (0-15)
};  // Total : 4 octets

// Gain : 75% de mémoire économisée !

// Bonnes pratiques
struct BonnePratique {
    unsigned int enable : 1;  // Toujours unsigned
    unsigned int mode   : 3;  // Documenter plage (0-7)
    unsigned int        : 0;  // Padding explicite si besoin
};
```

**Règle d'or** : Les bit-fields sont un outil d'optimisation mémoire puissant pour des cas spécifiques (hardware, protocoles, grandes collections). Pour du code général, privilégiez la clarté et la simplicité.

**Conclusion du chapitre 10** : Vous maîtrisez maintenant les types définis par l'utilisateur (struct, union, enum), l'optimisation mémoire (packed structures, bit-fields), et la simplification syntaxique (typedef). Ces outils sont essentiels pour créer des abstractions de données efficaces et maintenables en C !

⏭️ [Structures de données dynamiques](/11-structures-dynamiques/README.md)
