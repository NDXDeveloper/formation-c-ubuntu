🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 23.7 X-Macros

## Introduction

Les **X-Macros** (ou "macros X") sont une technique avancée du préprocesseur C qui permet de générer automatiquement plusieurs représentations d'une même liste de données. Cette technique est extrêmement utile pour éviter la duplication de code et maintenir la cohérence entre différentes parties d'un programme.

Le principe est simple mais puissant : **définir une liste de données une seule fois, puis l'utiliser plusieurs fois de différentes manières**.

### Le problème sans X-Macros

Imaginons que vous voulez créer une énumération avec des couleurs et pouvoir convertir chaque couleur en chaîne de caractères :

```c
// ❌ Approche traditionnelle - Code dupliqué

// Définition de l'enum
typedef enum {
    COLOR_RED,
    COLOR_GREEN,
    COLOR_BLUE,
    COLOR_YELLOW
} Color;

// Fonction de conversion en chaîne (duplication!)
const char* color_to_string(Color c) {
    switch(c) {
        case COLOR_RED:    return "RED";
        case COLOR_GREEN:  return "GREEN";
        case COLOR_BLUE:   return "BLUE";
        case COLOR_YELLOW: return "YELLOW";
        default:           return "UNKNOWN";
    }
}
```

**Problèmes** :
- ❌ Si vous ajoutez une couleur, vous devez modifier **deux endroits**
- ❌ Risque d'oubli → bugs difficiles à détecter
- ❌ Code répétitif et difficile à maintenir

### La solution avec X-Macros

```c
// ✅ Avec X-Macros - Une seule source de vérité

// Définition de la liste (une seule fois!)
#define COLOR_LIST \
    X(COLOR_RED) \
    X(COLOR_GREEN) \
    X(COLOR_BLUE) \
    X(COLOR_YELLOW)

// Génération de l'enum
#define X(name) name,
typedef enum {
    COLOR_LIST
    COLOR_COUNT  // Bonus: compteur automatique
} Color;
#undef X

// Génération du tableau de chaînes
#define X(name) #name,
const char* color_names[] = {
    COLOR_LIST
};
#undef X

// Fonction de conversion (simple et sans duplication)
const char* color_to_string(Color c) {
    if (c >= 0 && c < COLOR_COUNT) {
        return color_names[c];
    }
    return "UNKNOWN";
}
```

**Avantages** :
- ✅ Une seule liste à maintenir
- ✅ Impossible d'oublier un cas
- ✅ Ajout/suppression facile
- ✅ Code plus court et plus sûr

---

## Principe fondamental des X-Macros

### Comment ça fonctionne ?

Le principe repose sur trois étapes :

1. **Définir une liste avec un placeholder `X`**
```c
#define MA_LISTE \
    X(ELEMENT1) \
    X(ELEMENT2) \
    X(ELEMENT3)
```

2. **Définir `X` selon ce qu'on veut générer**
```c
#define X(nom) /* ce qu'on veut faire avec nom */
```

3. **Utiliser la liste, puis nettoyer**
```c
MA_LISTE
#undef X
```

### Premier exemple complet

```c
#include <stdio.h>

// ÉTAPE 1: Définir la liste de fruits
#define FRUIT_LIST \
    X(POMME) \
    X(BANANE) \
    X(ORANGE) \
    X(RAISIN)

// ÉTAPE 2a: Générer l'énumération
#define X(fruit) FRUIT_##fruit,
typedef enum {
    FRUIT_LIST
    FRUIT_COUNT
} Fruit;
#undef X

// ÉTAPE 2b: Générer les noms
#define X(fruit) #fruit,
const char* fruit_names[] = {
    FRUIT_LIST
};
#undef X

// ÉTAPE 3: Utilisation
int main(void) {
    printf("Il y a %d fruits\n", FRUIT_COUNT);

    for (int i = 0; i < FRUIT_COUNT; i++) {
        printf("Fruit %d: %s\n", i, fruit_names[i]);
    }

    return 0;
}
```

**Sortie** :
```
Il y a 4 fruits
Fruit 0: POMME
Fruit 1: BANANE
Fruit 2: ORANGE
Fruit 3: RAISIN
```

---

## Exemples progressifs

### Exemple 1 : Codes d'erreur

Un cas d'usage très courant : gérer des codes d'erreur avec leurs messages.

```c
#include <stdio.h>

// Liste des erreurs avec leurs messages
#define ERROR_LIST \
    X(ERR_SUCCESS,     0,  "Succès") \
    X(ERR_MEMORY,      1,  "Erreur d'allocation mémoire") \
    X(ERR_FILE,        2,  "Erreur de fichier") \
    X(ERR_NETWORK,     3,  "Erreur réseau") \
    X(ERR_PERMISSION,  4,  "Permission refusée") \
    X(ERR_TIMEOUT,     5,  "Délai d'attente dépassé")

// Génération de l'énumération
#define X(name, code, msg) name = code,
typedef enum {
    ERROR_LIST
} ErrorCode;
#undef X

// Génération du tableau de messages
#define X(name, code, msg) [code] = msg,
const char* error_messages[] = {
    ERROR_LIST
};
#undef X

// Fonction pour obtenir le message d'erreur
const char* get_error_message(ErrorCode err) {
    if (err >= 0 && err < sizeof(error_messages) / sizeof(error_messages[0])) {
        return error_messages[err];
    }
    return "Erreur inconnue";
}

// Exemple d'utilisation
int main(void) {
    ErrorCode erreurs[] = {
        ERR_SUCCESS,
        ERR_MEMORY,
        ERR_FILE,
        ERR_NETWORK
    };

    printf("Test des codes d'erreur:\n\n");
    for (size_t i = 0; i < sizeof(erreurs) / sizeof(erreurs[0]); i++) {
        printf("Code %d: %s\n", erreurs[i], get_error_message(erreurs[i]));
    }

    return 0;
}
```

**Sortie** :
```
Test des codes d'erreur:

Code 0: Succès
Code 1: Erreur d'allocation mémoire
Code 2: Erreur de fichier
Code 3: Erreur réseau
```

### Exemple 2 : États d'une machine à états

```c
#include <stdio.h>

// Liste des états
#define STATE_LIST \
    X(STATE_IDLE,       "Inactif") \
    X(STATE_CONNECTING, "Connexion en cours") \
    X(STATE_CONNECTED,  "Connecté") \
    X(STATE_SENDING,    "Envoi en cours") \
    X(STATE_RECEIVING,  "Réception en cours") \
    X(STATE_ERROR,      "Erreur")

// Génération de l'enum
#define X(name, description) name,
typedef enum {
    STATE_LIST
    STATE_COUNT
} State;
#undef X

// Génération des descriptions
#define X(name, description) description,
const char* state_descriptions[] = {
    STATE_LIST
};
#undef X

// Génération d'une fonction de validation
#define X(name, description) \
    case name: return 1;
int is_valid_state(State s) {
    switch(s) {
        STATE_LIST
        default: return 0;
    }
}
#undef X

int main(void) {
    State etat_actuel = STATE_CONNECTING;

    printf("État actuel: %s\n", state_descriptions[etat_actuel]);
    printf("État valide: %s\n", is_valid_state(etat_actuel) ? "Oui" : "Non");

    printf("\nTous les états:\n");
    for (int i = 0; i < STATE_COUNT; i++) {
        printf("  %d: %s\n", i, state_descriptions[i]);
    }

    return 0;
}
```

### Exemple 3 : Configuration avec valeurs par défaut

```c
#include <stdio.h>

// Liste des paramètres de configuration
#define CONFIG_LIST \
    X(MAX_CONNECTIONS,  100,   "Nombre maximum de connexions") \
    X(TIMEOUT_MS,       5000,  "Délai d'attente en millisecondes") \
    X(BUFFER_SIZE,      1024,  "Taille du tampon") \
    X(RETRY_COUNT,      3,     "Nombre de tentatives") \
    X(PORT,             8080,  "Port d'écoute")

// Génération de l'enum
#define X(name, value, desc) CONFIG_##name,
typedef enum {
    CONFIG_LIST
    CONFIG_COUNT
} ConfigParam;
#undef X

// Génération du tableau de valeurs par défaut
#define X(name, value, desc) value,
static int config_defaults[] = {
    CONFIG_LIST
};
#undef X

// Génération du tableau de descriptions
#define X(name, value, desc) desc,
static const char* config_descriptions[] = {
    CONFIG_LIST
};
#undef X

// Génération du tableau de noms
#define X(name, value, desc) #name,
static const char* config_names[] = {
    CONFIG_LIST
};
#undef X

// Structure de configuration
typedef struct {
    int values[CONFIG_COUNT];
} Config;

// Initialisation avec valeurs par défaut
void config_init(Config* cfg) {
    for (int i = 0; i < CONFIG_COUNT; i++) {
        cfg->values[i] = config_defaults[i];
    }
}

// Affichage de la configuration
void config_print(const Config* cfg) {
    printf("Configuration:\n");
    printf("==============\n");
    for (int i = 0; i < CONFIG_COUNT; i++) {
        printf("%-20s = %5d  (%s)\n",
               config_names[i],
               cfg->values[i],
               config_descriptions[i]);
    }
}

int main(void) {
    Config cfg;
    config_init(&cfg);
    config_print(&cfg);

    // Modifier une valeur
    printf("\nModification de MAX_CONNECTIONS...\n\n");
    cfg.values[CONFIG_MAX_CONNECTIONS] = 200;
    config_print(&cfg);

    return 0;
}
```

**Sortie** :
```
Configuration:
==============
MAX_CONNECTIONS      =   100  (Nombre maximum de connexions)
TIMEOUT_MS           =  5000  (Délai d'attente en millisecondes)
BUFFER_SIZE          =  1024  (Taille du tampon)
RETRY_COUNT          =     3  (Nombre de tentatives)
PORT                 =  8080  (Port d'écoute)

Modification de MAX_CONNECTIONS...

Configuration:
==============
MAX_CONNECTIONS      =   200  (Nombre maximum de connexions)
TIMEOUT_MS           =  5000  (Délai d'attente en millisecondes)
BUFFER_SIZE          =  1024  (Taille du tampon)
RETRY_COUNT          =     3  (Nombre de tentatives)
PORT                 =  8080  (Port d'écoute)
```

---

## Applications pratiques courantes

### Application 1 : Table de dispatch pour commandes

```c
#include <stdio.h>
#include <string.h>

// Liste des commandes
#define COMMAND_LIST \
    X(CMD_HELP,   "help",   "Affiche l'aide") \
    X(CMD_QUIT,   "quit",   "Quitte le programme") \
    X(CMD_SAVE,   "save",   "Sauvegarde les données") \
    X(CMD_LOAD,   "load",   "Charge les données") \
    X(CMD_STATUS, "status", "Affiche le statut")

// Génération de l'enum
#define X(id, cmd, desc) id,
typedef enum {
    COMMAND_LIST
    CMD_COUNT,
    CMD_UNKNOWN
} CommandId;
#undef X

// Génération du tableau de commandes
#define X(id, cmd, desc) cmd,
const char* command_strings[] = {
    COMMAND_LIST
};
#undef X

// Génération du tableau de descriptions
#define X(id, cmd, desc) desc,
const char* command_descriptions[] = {
    COMMAND_LIST
};
#undef X

// Prototypes des fonctions de traitement
void handle_help(void);
void handle_quit(void);
void handle_save(void);
void handle_load(void);
void handle_status(void);

// Génération de la table de dispatch
#define X(id, cmd, desc) handle_##cmd,
typedef void (*CommandHandler)(void);
CommandHandler command_handlers[] = {
    COMMAND_LIST
};
#undef X

// Fonction de parsing de commande
CommandId parse_command(const char* input) {
    for (int i = 0; i < CMD_COUNT; i++) {
        if (strcmp(input, command_strings[i]) == 0) {
            return i;
        }
    }
    return CMD_UNKNOWN;
}

// Exécution d'une commande
void execute_command(const char* input) {
    CommandId cmd = parse_command(input);

    if (cmd != CMD_UNKNOWN) {
        printf("Exécution de '%s': %s\n",
               command_strings[cmd],
               command_descriptions[cmd]);
        command_handlers[cmd]();
    } else {
        printf("Commande inconnue: %s\n", input);
    }
}

// Implémentation des handlers
void handle_help(void) {
    printf("\nCommandes disponibles:\n");
    for (int i = 0; i < CMD_COUNT; i++) {
        printf("  %-10s - %s\n", command_strings[i], command_descriptions[i]);
    }
}

void handle_quit(void)   { printf("  → Fermeture du programme\n"); }
void handle_save(void)   { printf("  → Sauvegarde effectuée\n"); }
void handle_load(void)   { printf("  → Chargement effectué\n"); }
void handle_status(void) { printf("  → Statut: OK\n"); }

int main(void) {
    printf("Système de commandes\n\n");

    const char* test_commands[] = {
        "help",
        "save",
        "status",
        "invalid"
    };

    for (size_t i = 0; i < sizeof(test_commands) / sizeof(test_commands[0]); i++) {
        execute_command(test_commands[i]);
        printf("\n");
    }

    return 0;
}
```

### Application 2 : Génération de sérialiseurs

```c
#include <stdio.h>
#include <string.h>

// Liste des champs d'une structure Personne
#define PERSON_FIELDS \
    X(int,    id,      "%d") \
    X(char,   name[50], "%s") \
    X(int,    age,     "%d") \
    X(double, salary,  "%.2f")

// Génération de la structure
#define X(type, name, fmt) type name;
typedef struct {
    PERSON_FIELDS
} Person;
#undef X

// Génération de la fonction print
void person_print(const Person* p) {
    printf("Person {\n");
#define X(type, name, fmt) \
    printf("  " #name ": " fmt "\n", p->name);
    PERSON_FIELDS
#undef X
    printf("}\n");
}

// Génération de la fonction d'initialisation par défaut
void person_init(Person* p) {
#define X(type, name, fmt) memset(&p->name, 0, sizeof(p->name));
    PERSON_FIELDS
#undef X
}

int main(void) {
    Person p;
    person_init(&p);

    // Remplir la structure
    p.id = 12345;
    strcpy(p.name, "Alice Dupont");
    p.age = 30;
    p.salary = 45000.50;

    // Afficher
    person_print(&p);

    return 0;
}
```

**Sortie** :
```
Person {
  id: 12345
  name: Alice Dupont
  age: 30
  salary: 45000.50
}
```

### Application 3 : Tests unitaires automatiques

```c
#include <stdio.h>
#include <string.h>

// Liste des tests
#define TEST_LIST \
    X(test_addition) \
    X(test_soustraction) \
    X(test_multiplication) \
    X(test_division)

// Génération des prototypes
#define X(name) void name(void);
TEST_LIST
#undef X

// Génération du tableau de tests
#define X(name) {#name, name},
struct TestCase {
    const char* name;
    void (*func)(void);
};

struct TestCase tests[] = {
    TEST_LIST
};
#undef X

// Compteur de tests
static int tests_passed = 0;
static int tests_failed = 0;

// Macro d'assertion simple
#define ASSERT_EQUAL(actual, expected) \
    do { \
        if ((actual) == (expected)) { \
            tests_passed++; \
        } else { \
            printf("  ✗ ÉCHEC: %d != %d\n", (actual), (expected)); \
            tests_failed++; \
        } \
    } while(0)

// Implémentation des tests
void test_addition(void) {
    ASSERT_EQUAL(2 + 2, 4);
    ASSERT_EQUAL(10 + 5, 15);
}

void test_soustraction(void) {
    ASSERT_EQUAL(5 - 3, 2);
    ASSERT_EQUAL(10 - 10, 0);
}

void test_multiplication(void) {
    ASSERT_EQUAL(3 * 4, 12);
    ASSERT_EQUAL(5 * 0, 0);
}

void test_division(void) {
    ASSERT_EQUAL(10 / 2, 5);
    ASSERT_EQUAL(9 / 3, 3);
}

// Exécution de tous les tests
void run_all_tests(void) {
    size_t num_tests = sizeof(tests) / sizeof(tests[0]);

    printf("Exécution de %zu tests...\n\n", num_tests);

    for (size_t i = 0; i < num_tests; i++) {
        printf("▶ %s\n", tests[i].name);
        tests[i].func();
    }

    printf("\n");
    printf("════════════════════════════\n");
    printf("Résultats:\n");
    printf("  ✓ Réussis: %d\n", tests_passed);
    printf("  ✗ Échoués: %d\n", tests_failed);
    printf("════════════════════════════\n");
}

int main(void) {
    run_all_tests();
    return tests_failed > 0 ? 1 : 0;
}
```

**Sortie** :
```
Exécution de 4 tests...

▶ test_addition
▶ test_soustraction
▶ test_multiplication
▶ test_division

════════════════════════════
Résultats:
  ✓ Réussis: 8
  ✗ Échoués: 0
════════════════════════════
```

---

## X-Macros avec des données complexes

### Exemple avancé : Registres matériels

```c
#include <stdio.h>
#include <stdint.h>

// Liste des registres avec adresse, nom et description
#define REGISTER_LIST \
    X(REG_STATUS,   0x00, "RW", "Registre de statut") \
    X(REG_CONTROL,  0x04, "RW", "Registre de contrôle") \
    X(REG_DATA,     0x08, "RW", "Registre de données") \
    X(REG_ERROR,    0x0C, "RO", "Registre d'erreur") \
    X(REG_VERSION,  0x10, "RO", "Version du matériel")

// Génération de l'enum
#define X(name, addr, perm, desc) name,
typedef enum {
    REGISTER_LIST
    REG_COUNT
} Register;
#undef X

// Génération du tableau d'adresses
#define X(name, addr, perm, desc) [name] = addr,
static const uint32_t register_addresses[] = {
    REGISTER_LIST
};
#undef X

// Génération du tableau de permissions
#define X(name, addr, perm, desc) [name] = perm,
static const char* register_permissions[] = {
    REGISTER_LIST
};
#undef X

// Génération du tableau de descriptions
#define X(name, addr, perm, desc) [name] = desc,
static const char* register_descriptions[] = {
    REGISTER_LIST
};
#undef X

// Génération du tableau de noms
#define X(name, addr, perm, desc) [name] = #name,
static const char* register_names[] = {
    REGISTER_LIST
};
#undef X

// Fonction pour afficher les informations d'un registre
void register_info(Register reg) {
    printf("Registre: %s\n", register_names[reg]);
    printf("  Adresse:     0x%04X\n", register_addresses[reg]);
    printf("  Permissions: %s\n", register_permissions[reg]);
    printf("  Description: %s\n", register_descriptions[reg]);
}

// Génération de la documentation
void print_register_map(void) {
    printf("╔════════════════════════════════════════════════════════════╗\n");
    printf("║                    MAP DES REGISTRES                       ║\n");
    printf("╠════════╦═══════╦══════╦════════════════════════════════════╣\n");
    printf("║  Nom   ║ Addr  ║ Perm ║ Description                        ║\n");
    printf("╠════════╬═══════╬══════╬════════════════════════════════════╣\n");

#define X(name, addr, perm, desc) \
    printf("║ %-6s ║ 0x%04X ║  %s  ║ %-34s ║\n", \
           #name, addr, perm, desc);
    REGISTER_LIST
#undef X

    printf("╚════════╩═══════╩══════╩════════════════════════════════════╝\n");
}

int main(void) {
    print_register_map();
    printf("\n");
    register_info(REG_STATUS);

    return 0;
}
```

---

## Avantages des X-Macros

### ✅ Avantages

1. **DRY (Don't Repeat Yourself)**
   - Une seule source de vérité
   - Pas de duplication de code

2. **Maintenance facilitée**
   - Ajouter/supprimer un élément : un seul endroit à modifier
   - Changements propagés automatiquement

3. **Réduction des erreurs**
   - Impossible d'oublier un cas
   - Synchronisation automatique

4. **Génération automatique**
   - Tableaux de conversion
   - Tables de dispatch
   - Documentation
   - Tests

5. **Type-safe**
   - Les énumérations sont vérifiées par le compilateur
   - Pas de magic numbers

### ⚠️ Limitations

1. **Lisibilité réduite**
   - Peut être difficile à comprendre pour les débutants
   - Nécessite des commentaires explicatifs

2. **Débogage plus difficile**
   - Le code expansé n'est pas visible directement
   - Utiliser `gcc -E` pour voir le résultat

3. **Messages d'erreur cryptiques**
   - Les erreurs du préprocesseur peuvent être obscures

4. **Pas de vérification de syntaxe**
   - Les erreurs ne sont détectées qu'après expansion

5. **Limitation du préprocesseur**
   - Pas de structures de contrôle complexes
   - Pas de récursion

---

## Bonnes pratiques

### 1. Toujours commenter les X-Macros

```c
/**
 * Liste des codes d'erreur du système
 *
 * Format: X(NOM, CODE, MESSAGE)
 * - NOM: Identifiant de l'erreur
 * - CODE: Code numérique
 * - MESSAGE: Description lisible
 */
#define ERROR_LIST \
    X(ERR_SUCCESS,  0, "Succès") \
    X(ERR_MEMORY,   1, "Erreur mémoire") \
    X(ERR_FILE,     2, "Erreur fichier")
```

### 2. Utiliser des noms de macro explicites

```c
// ✅ Bon
#define COLOR_LIST \
    X(RED) \
    X(GREEN) \
    X(BLUE)

// ❌ Mauvais (nom trop générique)
#define LIST \
    X(RED) \
    X(GREEN) \
    X(BLUE)
```

### 3. Toujours faire #undef X après utilisation

```c
// Utilisation
#define X(name) name,
typedef enum {
    COLOR_LIST
} Color;
#undef X  // ← Important !

// Nouvelle utilisation possible
#define X(name) #name,
const char* color_names[] = {
    COLOR_LIST
};
#undef X
```

### 4. Isoler dans des fichiers d'en-tête

**error_codes.def** :
```c
// Fichier de définition
X(ERR_SUCCESS,  0, "Succès")
X(ERR_MEMORY,   1, "Erreur mémoire")
X(ERR_FILE,     2, "Erreur fichier")
```

**error_codes.h** :
```c
#ifndef ERROR_CODES_H
#define ERROR_CODES_H

// Génération de l'enum
#define X(name, code, msg) name = code,
typedef enum {
    #include "error_codes.def"
    ERR_COUNT
} ErrorCode;
#undef X

// Fonction de conversion
const char* error_to_string(ErrorCode err);

#endif
```

**error_codes.c** :
```c
#include "error_codes.h"

// Génération du tableau de messages
#define X(name, code, msg) [code] = msg,
const char* error_messages[] = {
    #include "error_codes.def"
};
#undef X

const char* error_to_string(ErrorCode err) {
    if (err >= 0 && err < ERR_COUNT) {
        return error_messages[err];
    }
    return "Erreur inconnue";
}
```

### 5. Prévoir un compteur automatique

```c
#define COLOR_LIST \
    X(RED) \
    X(GREEN) \
    X(BLUE)

#define X(name) COLOR_##name,
typedef enum {
    COLOR_LIST
    COLOR_COUNT  // ← Nombre automatique d'éléments
} Color;
#undef X

// Maintenant COLOR_COUNT = 3
```

---

## Debugging des X-Macros

### Voir le code expansé

```bash
# Préprocesser seulement (sans compiler)
gcc -E fichier.c

# Sauvegarder dans un fichier
gcc -E fichier.c > fichier_expanded.c

# Avec formatage lisible
gcc -E -P fichier.c
```

### Exemple de sortie

**Avant expansion** :
```c
#define COLOR_LIST X(RED) X(GREEN) X(BLUE)

#define X(name) name,
typedef enum { COLOR_LIST } Color;
#undef X
```

**Après expansion** (visible avec `gcc -E`) :
```c
typedef enum { RED, GREEN, BLUE, } Color;
```

---

## Comparaison avec les alternatives

### Alternative 1 : Code dupliqué

```c
// ❌ Sans X-Macros - Duplication
typedef enum { RED, GREEN, BLUE } Color;
const char* names[] = {"RED", "GREEN", "BLUE"};
// Risque d'incohérence si on ajoute/supprime une couleur
```

### Alternative 2 : Génération de code externe

```c
// ⚠️ Script Python/Perl qui génère du C
// + Flexible mais nécessite un outil externe
// - Complexité du build
// - Pas standard
```

### Alternative 3 : Templates C++ (si disponible)

```c++
// C++ uniquement
template<typename T>
const char* to_string(T value);
```

**Verdict** : Les X-Macros sont la solution la plus portable et intégrée pour le C pur.

---

## Exemple complet : Mini-framework de menu

```c
#include <stdio.h>
#include <string.h>

// Liste des options du menu
#define MENU_OPTIONS \
    X(1, "Nouveau fichier",    "Créer un nouveau document") \
    X(2, "Ouvrir",             "Ouvrir un document existant") \
    X(3, "Enregistrer",        "Sauvegarder le document") \
    X(4, "Enregistrer sous",   "Sauvegarder avec un nouveau nom") \
    X(5, "Imprimer",           "Imprimer le document") \
    X(0, "Quitter",            "Fermer l'application")

// Génération des handlers (prototypes)
#define X(id, label, desc) void menu_handler_##id(void);
MENU_OPTIONS
#undef X

// Structure pour les options
typedef struct {
    int id;
    const char* label;
    const char* description;
    void (*handler)(void);
} MenuOption;

// Génération du tableau d'options
#define X(id, label, desc) {id, label, desc, menu_handler_##id},
MenuOption menu_options[] = {
    MENU_OPTIONS
};
#undef X

const int menu_count = sizeof(menu_options) / sizeof(menu_options[0]);

// Affichage du menu
void display_menu(void) {
    printf("\n╔════════════════════════════════════════════════╗\n");
    printf("║              MENU PRINCIPAL                    ║\n");
    printf("╠════════════════════════════════════════════════╣\n");

    for (int i = 0; i < menu_count; i++) {
        MenuOption* opt = &menu_options[i];
        printf("║ %d. %-41s ║\n", opt->id, opt->label);
    }

    printf("╚════════════════════════════════════════════════╝\n");
    printf("\nChoisissez une option: ");
}

// Exécution d'une option
int execute_menu_option(int choice) {
    for (int i = 0; i < menu_count; i++) {
        if (menu_options[i].id == choice) {
            printf("\n→ %s\n", menu_options[i].description);
            menu_options[i].handler();
            return choice != 0;  // Continue si pas "Quitter"
        }
    }

    printf("\nOption invalide!\n");
    return 1;
}

// Implémentation des handlers
#define X(id, label, desc) \
    void menu_handler_##id(void) { \
        printf("  Exécution de: %s\n", label); \
        if (id == 0) printf("  Au revoir!\n"); \
    }
MENU_OPTIONS
#undef X

int main(void) {
    printf("Application de démonstration - X-Macros\n");

    int running = 1;
    while (running) {
        display_menu();

        int choice;
        if (scanf("%d", &choice) == 1) {
            running = execute_menu_option(choice);
        } else {
            // Nettoyer le buffer d'entrée
            while (getchar() != '\n');
            printf("\nEntrée invalide!\n");
        }
    }

    return 0;
}
```

---

## Résumé

### Points clés

🔑 **Principe** : Définir une liste une fois, l'utiliser de multiples façons

🔑 **Avantage principal** : Une seule source de vérité → pas d'incohérence

🔑 **Cas d'usage** :
- Énumérations avec noms/descriptions
- Codes d'erreur
- Tables de dispatch
- Configuration
- Génération de code répétitif

### Checklist d'utilisation

✅ **Quand utiliser les X-Macros** :
- [ ] Vous avez une liste d'éléments liés
- [ ] Cette liste doit être représentée de plusieurs façons
- [ ] La maintenance de multiples versions serait source d'erreurs
- [ ] Vous voulez de la génération automatique

❌ **Quand ne PAS utiliser** :
- [ ] Pour du code simple sans duplication
- [ ] Quand la lisibilité est critique
- [ ] Si l'équipe n'est pas familière avec la technique
- [ ] Pour des structures de données complexes

### Template de base

```c
// 1. Définir la liste
#define MA_LISTE \
    X(ELEMENT1, données1) \
    X(ELEMENT2, données2) \
    X(ELEMENT3, données3)

// 2. Générer ce dont vous avez besoin
#define X(nom, données) /* transformation */
// Utiliser MA_LISTE ici
#undef X

// 3. Répéter pour d'autres transformations
#define X(nom, données) /* autre transformation */
// Utiliser MA_LISTE ici
#undef X
```

Les X-Macros sont une technique puissante qui, bien utilisée, peut grandement améliorer la maintenabilité de votre code C !

---

**Fin du chapitre 23 : Macros et Préprocesseur avancé**

Ce module a chapitre l'ensemble des techniques de macros en C, des plus simples aux plus avancées. Vous disposez maintenant des outils pour écrire du code maintenable, portable et efficace.

⏭️ [Gestion avancée de la mémoire](/24-gestion-memoire-avancee/README.md)
