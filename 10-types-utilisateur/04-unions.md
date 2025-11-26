🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 10.4 union : Optimisation mémoire

## Introduction

Nous avons vu que les **structures** (`struct`) permettent de regrouper plusieurs variables différentes. Mais que faire si nous voulons stocker **une seule valeur à la fois**, qui peut être de différents types ?

C'est là qu'intervient l'**union**. Une union permet de stocker **plusieurs types de données dans le même emplacement mémoire**, mais un seul membre peut être actif à la fois.

### Analogie simple

Imaginez une boîte qui peut contenir :
- **Soit** un livre
- **Soit** un ordinateur portable
- **Soit** des chaussures

Mais jamais les trois en même temps. La taille de la boîte sera celle de l'objet le plus volumineux (l'ordinateur).

C'est exactement ce qu'est une union !

---

## Différence fondamentale : struct vs union

### Structure (struct)

```c
struct Point {
    int x;
    int y;
};
```

**Mémoire** : Les membres sont **stockés séparément**
```
Adresse:   0    1    2    3    4    5    6    7
          +----+----+----+----+----+----+----+----+
          | x0 | x1 | x2 | x3 | y0 | y1 | y2 | y3 |
          +----+----+----+----+----+----+----+----+
          ^-- x (4 octets) --^-- y (4 octets) --^
```
**Taille** : 8 octets (4 + 4)

### Union (union)

```c
union Data {
    int i;
    float f;
    char c;
};
```

**Mémoire** : Les membres **partagent le même emplacement**
```
Adresse:   0    1    2    3
          +----+----+----+----+
          | i0 | i1 | i2 | i3 |  <- int (4 octets)
          | f0 | f1 | f2 | f3 |  <- float (4 octets, même adresse)
          | c  |    |    |    |  <- char (1 octet, même adresse)
          +----+----+----+----+
```
**Taille** : 4 octets (la taille du **plus grand** membre)

---

## Déclaration et utilisation de base

### Syntaxe

```c
union NomUnion {
    type1 membre1;
    type2 membre2;
    type3 membre3;
};
```

### Exemple simple

```c
#include <stdio.h>

union Data {
    int i;
    float f;
    char c;
};

int main() {
    union Data data;

    printf("Taille de l'union : %zu octets\n\n", sizeof(union Data));

    // Stocker un entier
    data.i = 42;
    printf("Entier : %d\n", data.i);

    // Stocker un float (écrase l'entier !)
    data.f = 3.14;
    printf("Float  : %.2f\n", data.f);
    printf("Entier : %d (⚠️ valeur invalide !)\n\n", data.i);

    // Stocker un char (écrase le float !)
    data.c = 'A';
    printf("Char   : %c\n", data.c);
    printf("Float  : %.2f (⚠️ valeur invalide !)\n", data.f);

    return 0;
}
```

**Sortie** :
```
Taille de l'union : 4 octets

Entier : 42
Float  : 3.14
Entier : 1078523331 (⚠️ valeur invalide !)

Char   : A
Float  : 0.00 (⚠️ valeur invalide !)
```

**Leçon importante** : Quand on écrit dans un membre, les autres membres deviennent invalides !

---

## Pourquoi utiliser des unions ?

### 1. Économie de mémoire

Les unions sont utiles quand une variable peut être de différents types, mais **jamais simultanément**.

```c
// Sans union : 12 octets (en comptant le padding)
struct SansUnion {
    int type;      // 0 = int, 1 = float, 2 = char
    int i_val;
    float f_val;
    char c_val;
};

// Avec union : 8 octets
struct AvecUnion {
    int type;
    union {
        int i_val;
        float f_val;
        char c_val;
    } valeur;
};
```

**Gain** : 33% de mémoire économisée !

### 2. Interprétation de données brutes

Les unions permettent d'interpréter les mêmes octets de différentes manières :

```c
union FloatBytes {
    float f;
    unsigned char bytes[4];
};

union FloatBytes fb;
fb.f = 3.14f;

printf("Float : %.2f\n", fb.f);
printf("Bytes : ");
for (int i = 0; i < 4; i++) {
    printf("%02X ", fb.bytes[i]);
}
printf("\n");
```

**Sortie** :
```
Float : 3.14
Bytes : C3 F5 48 40
```

---

## Pattern classique : Union avec type tag

### Problème

Comment savoir quel membre de l'union est actuellement valide ?

### Solution : Tagged union

On utilise une structure contenant :
1. Un **champ type** (tag) qui indique quel membre est actif
2. Une **union** pour la valeur

```c
#include <stdio.h>

// Définir les types possibles
enum TypeDonnee {
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING
};

// Structure avec union "tagged"
struct Valeur {
    enum TypeDonnee type;  // Indique quel membre est valide
    union {
        int i;
        float f;
        char *s;
    } data;
};

void afficher_valeur(struct Valeur v) {
    switch (v.type) {
        case TYPE_INT:
            printf("Entier : %d\n", v.data.i);
            break;
        case TYPE_FLOAT:
            printf("Float  : %.2f\n", v.data.f);
            break;
        case TYPE_STRING:
            printf("String : %s\n", v.data.s);
            break;
    }
}

int main() {
    struct Valeur v1 = {TYPE_INT, {.i = 42}};
    struct Valeur v2 = {TYPE_FLOAT, {.f = 3.14}};
    struct Valeur v3 = {TYPE_STRING, {.s = "Bonjour"}};

    afficher_valeur(v1);
    afficher_valeur(v2);
    afficher_valeur(v3);

    return 0;
}
```

**Sortie** :
```
Entier : 42
Float  : 3.14
String : Bonjour
```

---

## Cas d'usage pratiques

### 1. Représentation de différents types de messages

```c
#include <stdio.h>
#include <string.h>

enum TypeMessage {
    MSG_TEXTE,
    MSG_NOMBRE,
    MSG_ERREUR
};

struct Message {
    enum TypeMessage type;
    union {
        char texte[256];
        int nombre;
        struct {
            int code;
            char description[128];
        } erreur;
    } contenu;
};

void traiter_message(struct Message *msg) {
    switch (msg->type) {
        case MSG_TEXTE:
            printf("[TEXTE] %s\n", msg->contenu.texte);
            break;
        case MSG_NOMBRE:
            printf("[NOMBRE] %d\n", msg->contenu.nombre);
            break;
        case MSG_ERREUR:
            printf("[ERREUR %d] %s\n",
                   msg->contenu.erreur.code,
                   msg->contenu.erreur.description);
            break;
    }
}

int main() {
    struct Message msg1 = {MSG_TEXTE};
    strcpy(msg1.contenu.texte, "Connexion établie");
    traiter_message(&msg1);

    struct Message msg2 = {MSG_NOMBRE};
    msg2.contenu.nombre = 42;
    traiter_message(&msg2);

    struct Message msg3 = {MSG_ERREUR};
    msg3.contenu.erreur.code = 404;
    strcpy(msg3.contenu.erreur.description, "Fichier introuvable");
    traiter_message(&msg3);

    return 0;
}
```

**Sortie** :
```
[TEXTE] Connexion établie
[NOMBRE] 42
[ERREUR 404] Fichier introuvable
```

### 2. Conversion endianness (ordre des octets)

```c
#include <stdio.h>
#include <stdint.h>

union Endian {
    uint32_t value;
    uint8_t bytes[4];
};

void afficher_endianness() {
    union Endian test;
    test.value = 0x12345678;

    printf("Valeur : 0x%08X\n", test.value);
    printf("Octets : ");
    for (int i = 0; i < 4; i++) {
        printf("%02X ", test.bytes[i]);
    }
    printf("\n");

    if (test.bytes[0] == 0x78) {
        printf("Architecture : Little Endian\n");
    } else {
        printf("Architecture : Big Endian\n");
    }
}

int main() {
    afficher_endianness();
    return 0;
}
```

**Sortie (sur x86_64)** :
```
Valeur : 0x12345678
Octets : 78 56 34 12
Architecture : Little Endian
```

### 3. Manipulation de bits et d'adresses IP

```c
#include <stdio.h>
#include <stdint.h>

union IPv4 {
    uint32_t addr;       // Adresse complète
    uint8_t octets[4];   // 4 octets individuels
};

void afficher_ip(union IPv4 ip) {
    printf("Adresse IP : %d.%d.%d.%d\n",
           ip.octets[0], ip.octets[1],
           ip.octets[2], ip.octets[3]);
    printf("Valeur numérique : 0x%08X\n", ip.addr);
}

int main() {
    union IPv4 ip;

    // Méthode 1 : définir par octets
    ip.octets[0] = 192;
    ip.octets[1] = 168;
    ip.octets[2] = 1;
    ip.octets[3] = 1;

    afficher_ip(ip);

    printf("\n");

    // Méthode 2 : définir par valeur
    ip.addr = 0xC0A80101;  // 192.168.1.1 en hexa
    afficher_ip(ip);

    return 0;
}
```

### 4. Variantes de données dans un système de fichiers

```c
#include <stdio.h>
#include <string.h>

enum TypeFichier {
    TYPE_REGULIER,
    TYPE_REPERTOIRE,
    TYPE_LIEN
};

struct Fichier {
    char nom[256];
    enum TypeFichier type;
    union {
        struct {
            long taille;
            long offset;
        } regulier;
        struct {
            int nb_fichiers;
            char **liste;
        } repertoire;
        struct {
            char cible[256];
        } lien;
    } info;
};

void afficher_fichier(struct Fichier *f) {
    printf("Nom : %s\n", f->nom);

    switch (f->type) {
        case TYPE_REGULIER:
            printf("Type : Fichier régulier\n");
            printf("Taille : %ld octets\n", f->info.regulier.taille);
            break;
        case TYPE_REPERTOIRE:
            printf("Type : Répertoire\n");
            printf("Contient %d fichiers\n", f->info.repertoire.nb_fichiers);
            break;
        case TYPE_LIEN:
            printf("Type : Lien symbolique\n");
            printf("Pointe vers : %s\n", f->info.lien.cible);
            break;
    }
    printf("\n");
}

int main() {
    struct Fichier f1 = {
        .nom = "document.txt",
        .type = TYPE_REGULIER,
        .info.regulier = {.taille = 1024, .offset = 0}
    };

    struct Fichier f2 = {
        .nom = "mes_documents",
        .type = TYPE_REPERTOIRE,
        .info.repertoire = {.nb_fichiers = 5}
    };

    struct Fichier f3 = {
        .nom = "raccourci",
        .type = TYPE_LIEN
    };
    strcpy(f3.info.lien.cible, "/home/user/document.txt");

    afficher_fichier(&f1);
    afficher_fichier(&f2);
    afficher_fichier(&f3);

    return 0;
}
```

---

## Unions anonymes (C11)

Depuis C11, on peut déclarer des unions **anonymes** directement dans une structure :

```c
struct Donnee {
    int type;
    union {           // Union anonyme (pas de nom)
        int i;
        float f;
        char c;
    };                // Pas de nom de variable
};

int main() {
    struct Donnee d;
    d.type = 0;
    d.i = 42;         // Accès direct, pas besoin de d.valeur.i

    printf("%d\n", d.i);
    return 0;
}
```

**Avantage** : Syntaxe plus simple, les membres sont directement accessibles.

---

## Taille et alignement des unions

### Règle de calcul

La taille d'une union est égale à **la taille du membre le plus grand**, arrondie à l'alignement requis.

```c
#include <stdio.h>

union Exemple {
    char c;        // 1 octet
    short s;       // 2 octets
    int i;         // 4 octets
    double d;      // 8 octets
};

int main() {
    printf("Taille : %zu octets\n", sizeof(union Exemple));  // 8 octets

    union Exemple u;
    printf("Adresse de c : %p\n", (void*)&u.c);
    printf("Adresse de s : %p\n", (void*)&u.s);
    printf("Adresse de i : %p\n", (void*)&u.i);
    printf("Adresse de d : %p\n", (void*)&u.d);
    // Toutes les adresses sont identiques !

    return 0;
}
```

**Sortie** :
```
Taille : 8 octets
Adresse de c : 0x7ffc8b9c1e10
Adresse de s : 0x7ffc8b9c1e10
Adresse de i : 0x7ffc8b9c1e10
Adresse de d : 0x7ffc8b9c1e10
```

### Avec padding

```c
union AvecPadding {
    char c;        // 1 octet
    struct {
        char a;
        int b;
    } s;           // 8 octets (1 + 3 padding + 4)
};
// Taille : 8 octets (taille de la struct, le plus grand membre)
```

---

## Précautions et pièges

### 1. Comportement indéfini si mauvais membre lu

```c
union Data {
    int i;
    float f;
};

union Data d;
d.i = 42;
printf("%f\n", d.f);  // ⚠️ Comportement indéfini !
                      // On lit f alors que i a été écrit
```

**Solution** : Toujours utiliser un tag pour savoir quel membre est valide.

### 2. Initialisation

```c
union Data {
    int i;
    float f;
    char c;
};

// ❌ Erreur : on ne peut pas initialiser plusieurs membres
// union Data d = {42, 3.14, 'A'};

// ✅ Initialisation du premier membre uniquement
union Data d1 = {42};        // Initialise i

// ✅ Initialisation désignée (C99+)
union Data d2 = {.f = 3.14}; // Initialise f
union Data d3 = {.c = 'A'};  // Initialise c
```

### 3. Unions et pointeurs

```c
union Data {
    int i;
    float f;
};

void fonction(union Data *ptr) {
    ptr->i = 42;     // ✅ OK
    // Mais comment savoir si on doit lire i ou f ?
    // Il faut un tag séparé !
}
```

### 4. Alignement strict

Certaines architectures (ARM anciennes) peuvent planter sur des accès désalignés :

```c
union Dangereux {
    char c;
    int i;  // Potentiellement problématique selon l'usage
};
```

---

## Unions vs Structures : Tableau comparatif

| Critère | Structure | Union |
|---------|-----------|-------|
| **Mémoire** | Chaque membre a son espace | Tous partagent le même espace |
| **Taille** | Somme de tous les membres + padding | Taille du plus grand membre |
| **Accès** | Tous les membres accessibles simultanément | Un seul membre valide à la fois |
| **Usage** | Regrouper des données liées | Économiser mémoire avec types mutuellement exclusifs |

---

## Comparaison de tailles

```c
#include <stdio.h>

struct Personne1 {
    char nom[50];
    int age;
    float salaire;
    char poste[30];
};

struct Personne2 {
    char nom[50];
    int type;  // 0 = étudiant, 1 = employé
    union {
        int annee_etude;  // Si étudiant
        struct {
            float salaire;
            char poste[30];
        } employe;        // Si employé
    } info;
};

int main() {
    printf("Taille struct Personne1 : %zu octets\n", sizeof(struct Personne1));
    printf("Taille struct Personne2 : %zu octets\n", sizeof(struct Personne2));
    printf("Gain : %zu octets\n",
           sizeof(struct Personne1) - sizeof(struct Personne2));

    return 0;
}
```

**Sortie** :
```
Taille struct Personne1 : 88 octets
Taille struct Personne2 : 60 octets
Gain : 28 octets
```

---

## Exemple avancé : Système de gestion de variables

```c
#include <stdio.h>
#include <string.h>

enum TypeVariable {
    VAR_INT,
    VAR_DOUBLE,
    VAR_STRING,
    VAR_BOOL
};

struct Variable {
    char nom[32];
    enum TypeVariable type;
    union {
        int i;
        double d;
        char s[128];
        int b;  // 0 ou 1 pour bool
    } valeur;
};

void initialiser_variable(struct Variable *v, const char *nom, enum TypeVariable type) {
    strncpy(v->nom, nom, 31);
    v->nom[31] = '\0';
    v->type = type;
}

void definir_int(struct Variable *v, int valeur) {
    if (v->type == VAR_INT) {
        v->valeur.i = valeur;
    }
}

void definir_double(struct Variable *v, double valeur) {
    if (v->type == VAR_DOUBLE) {
        v->valeur.d = valeur;
    }
}

void definir_string(struct Variable *v, const char *valeur) {
    if (v->type == VAR_STRING) {
        strncpy(v->valeur.s, valeur, 127);
        v->valeur.s[127] = '\0';
    }
}

void definir_bool(struct Variable *v, int valeur) {
    if (v->type == VAR_BOOL) {
        v->valeur.b = valeur ? 1 : 0;
    }
}

void afficher_variable(struct Variable *v) {
    printf("Variable '%s' = ", v->nom);
    switch (v->type) {
        case VAR_INT:
            printf("%d (int)\n", v->valeur.i);
            break;
        case VAR_DOUBLE:
            printf("%.2f (double)\n", v->valeur.d);
            break;
        case VAR_STRING:
            printf("\"%s\" (string)\n", v->valeur.s);
            break;
        case VAR_BOOL:
            printf("%s (bool)\n", v->valeur.b ? "true" : "false");
            break;
    }
}

int main() {
    struct Variable vars[4];

    // Variable entière
    initialiser_variable(&vars[0], "compteur", VAR_INT);
    definir_int(&vars[0], 42);

    // Variable double
    initialiser_variable(&vars[1], "prix", VAR_DOUBLE);
    definir_double(&vars[1], 19.99);

    // Variable string
    initialiser_variable(&vars[2], "nom", VAR_STRING);
    definir_string(&vars[2], "Alice");

    // Variable booléenne
    initialiser_variable(&vars[3], "actif", VAR_BOOL);
    definir_bool(&vars[3], 1);

    // Affichage
    for (int i = 0; i < 4; i++) {
        afficher_variable(&vars[i]);
    }

    return 0;
}
```

**Sortie** :
```
Variable 'compteur' = 42 (int)
Variable 'prix' = 19.99 (double)
Variable 'nom' = "Alice" (string)
Variable 'actif' = true (bool)
```

---

## Unions et compatibilité C++

En C++, les unions ont des restrictions supplémentaires :

```c
// ✅ Valide en C
union EnC {
    int i;
    float f;
    char *str;
};

// ❌ Invalide en C++ (membres non-triviaux)
union EnCpp {
    int i;
    std::string str;  // Erreur : string a un constructeur
};
```

Si vous écrivez du code portable C/C++, utilisez uniquement des types simples (POD - Plain Old Data) dans les unions.

---

## Bonnes pratiques

### 1. Toujours utiliser des tagged unions

```c
// ✅ Bon : on sait toujours quel membre est valide
struct TaggedUnion {
    enum { TYPE_A, TYPE_B } type;
    union {
        int a;
        float b;
    } data;
};

// ❌ Mauvais : on ne sait pas quel membre est valide
union UntaggedUnion {
    int a;
    float b;
};
```

### 2. Documenter clairement

```c
/*
 * Type variant pouvant contenir soit un entier, soit un float.
 * Le champ 'type' indique quel membre de l'union est actuellement valide.
 * TOUJOURS vérifier 'type' avant d'accéder à 'data'.
 */
struct Variant {
    enum { INT, FLOAT } type;
    union {
        int as_int;
        float as_float;
    } data;
};
```

### 3. Encapsuler dans des fonctions

```c
struct Variant {
    enum { INT, FLOAT } type;
    union {
        int i;
        float f;
    } data;
};

// Fonctions d'accès sécurisées
int get_int(struct Variant *v) {
    if (v->type == INT) {
        return v->data.i;
    }
    // Gérer l'erreur
    return 0;
}

float get_float(struct Variant *v) {
    if (v->type == FLOAT) {
        return v->data.f;
    }
    // Gérer l'erreur
    return 0.0f;
}
```

### 4. Utiliser sizeof pour vérifier les optimisations

```c
// Avant optimisation
struct Avant {
    int type;
    int val_int;
    float val_float;
    char val_char;
};

// Après optimisation avec union
struct Apres {
    int type;
    union {
        int val_int;
        float val_float;
        char val_char;
    } valeur;
};

// Vérifier le gain
printf("Avant : %zu octets\n", sizeof(struct Avant));
printf("Après : %zu octets\n", sizeof(struct Apres));
printf("Gain  : %zu octets (%.1f%%)\n",
       sizeof(struct Avant) - sizeof(struct Apres),
       100.0 * (sizeof(struct Avant) - sizeof(struct Apres)) / sizeof(struct Avant));
```

---

## Debugging des unions

### Afficher le contenu brut

```c
#include <stdio.h>
#include <string.h>

union Debug {
    int i;
    float f;
    char c;
};

void afficher_union_brut(union Debug *u) {
    unsigned char *bytes = (unsigned char *)u;
    printf("Contenu brut : ");
    for (size_t i = 0; i < sizeof(union Debug); i++) {
        printf("%02X ", bytes[i]);
    }
    printf("\n");
}

int main() {
    union Debug u;

    u.i = 0x12345678;
    printf("Après u.i = 0x12345678 :\n");
    afficher_union_brut(&u);

    u.f = 3.14f;
    printf("\nAprès u.f = 3.14f :\n");
    afficher_union_brut(&u);

    u.c = 'A';
    printf("\nAprès u.c = 'A' :\n");
    afficher_union_brut(&u);

    return 0;
}
```

**Sortie** :
```
Après u.i = 0x12345678 :
Contenu brut : 78 56 34 12

Après u.f = 3.14f :
Contenu brut : C3 F5 48 40

Après u.c = 'A' :
Contenu brut : 41 F5 48 40
```

---

## Résumé

### Points clés

1. **Union** : Tous les membres partagent le **même emplacement mémoire**
2. **Taille** : Égale au membre le plus grand (+ alignement)
3. **Utilisation** : Un seul membre valide à la fois
4. **Avantage** : Économie de mémoire significative
5. **Obligation** : Utiliser des **tagged unions** pour suivre le membre actif

### Quand utiliser les unions ?

✅ **Utiliser** quand :
- Types de données mutuellement exclusifs
- Besoin d'interpréter des données brutes
- Économie de mémoire importante
- Protocoles ou formats de fichiers

❌ **Éviter** quand :
- Données indépendantes (utiliser struct)
- Besoin de plusieurs valeurs simultanément
- Code doit être portable vers C++

### Syntaxe essentielle

```c
// Déclaration
union NomUnion {
    int i;
    float f;
};

// Tagged union (recommandé)
struct TaggedUnion {
    enum { TYPE_INT, TYPE_FLOAT } type;
    union {
        int i;
        float f;
    } data;
};

// Union anonyme (C11)
struct Donnee {
    int type;
    union {
        int i;
        float f;
    };  // Membres accessibles directement
};
```

---

## À retenir

```c
// ❌ Mauvais : union sans tag
union Mauvais {
    int i;
    float f;
};
// Comment savoir si i ou f est valide ?

// ✅ Bon : tagged union
struct Bon {
    enum { TYPE_INT, TYPE_FLOAT } type;
    union {
        int i;
        float f;
    } data;
};
// On vérifie 'type' avant de lire 'data'

// Utilisation sécurisée
struct Bon valeur;
valeur.type = TYPE_INT;
valeur.data.i = 42;

if (valeur.type == TYPE_INT) {
    printf("%d\n", valeur.data.i);  // ✅ Sûr
}
```

**Règle d'or** : Les unions sont un outil d'optimisation mémoire puissant, mais nécessitent une discipline stricte pour éviter les bugs. Toujours utiliser un tag pour tracer le membre actif !

**Prochaine étape** : Nous allons découvrir les **enums** (section 10.5), qui améliorent la lisibilité et la gestion des états dans le code.

⏭️ [enum : Code lisible et gestion d'états](/10-types-utilisateur/05-enumerations.md)
