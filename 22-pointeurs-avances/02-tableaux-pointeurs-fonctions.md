🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 22.2 Tableaux de pointeurs de fonctions

## Introduction

Après avoir découvert les pointeurs de fonctions dans la section précédente, nous allons maintenant explorer un concept encore plus puissant : **les tableaux de pointeurs de fonctions**. Cette technique permet de stocker plusieurs fonctions dans un tableau et de les appeler dynamiquement selon un index ou une condition.

### Qu'est-ce qu'un tableau de pointeurs de fonctions ?

Tout simplement, c'est un tableau dont chaque élément est un pointeur vers une fonction. Imaginez que vous avez une télécommande avec plusieurs boutons : chaque bouton (index du tableau) déclenche une action différente (fonction).

**Analogie** : Pensez à un distributeur automatique. Chaque touche numérique (0-9) correspond à un produit différent. Quand vous appuyez sur "3", la machine exécute la fonction "distribuer le produit en position 3". Le distributeur contient en fait un tableau de fonctions, une pour chaque produit.

### Pourquoi utiliser des tableaux de pointeurs de fonctions ?

Les tableaux de pointeurs de fonctions sont particulièrement utiles pour :

1. **Tables de dispatch** : Remplacer de longs `if-else` ou `switch-case`
2. **Machines à états** : Gérer des transitions d'états complexes
3. **Interpréteurs** : Exécuter des commandes basées sur des codes d'opération
4. **Menus interactifs** : Associer des actions à des choix utilisateur
5. **Calculatrices** : Sélectionner une opération mathématique selon un symbole

---

## Syntaxe de base

### Déclaration d'un tableau de pointeurs de fonctions

La syntaxe peut sembler intimidante, mais décomposons-la :

```c
type_retour (*nom_tableau[taille])(types_parametres);
```

**Exemple concret** :
```c
int (*operations[4])(int, int);
```

Décortiquons cette déclaration :
- `int` : type de retour des fonctions
- `(*operations[4])` : tableau de 4 pointeurs de fonctions nommé `operations`
- `(int, int)` : chaque fonction prend deux paramètres `int`

**Important** : L'ordre des symboles est crucial. Les crochets `[]` ont priorité sur l'astérisque `*`, donc les parenthèses autour de `*operations[4]` sont obligatoires.

### Exemple simple et complet

```c
#include <stdio.h>

// Définition de plusieurs fonctions avec la même signature
int addition(int a, int b) {
    return a + b;
}

int soustraction(int a, int b) {
    return a - b;
}

int multiplication(int a, int b) {
    return a * b;
}

int division(int a, int b) {
    if (b == 0) {
        printf("Erreur : division par zéro\n");
        return 0;
    }
    return a / b;
}

int main(void) {
    // Déclaration et initialisation du tableau de pointeurs de fonctions
    int (*operations[4])(int, int) = {
        addition,
        soustraction,
        multiplication,
        division
    };

    int x = 20, y = 5;

    // Appel des fonctions via le tableau
    printf("Addition : %d + %d = %d\n", x, y, operations[0](x, y));
    printf("Soustraction : %d - %d = %d\n", x, y, operations[1](x, y));
    printf("Multiplication : %d * %d = %d\n", x, y, operations[2](x, y));
    printf("Division : %d / %d = %d\n", x, y, operations[3](x, y));

    return 0;
}
```

**Sortie** :
```
Addition : 20 + 5 = 25  
Soustraction : 20 - 5 = 15  
Multiplication : 20 * 5 = 100  
Division : 20 / 5 = 4  
```

**Points clés** :
- Toutes les fonctions dans le tableau doivent avoir la **même signature**
- L'accès se fait comme pour un tableau normal : `operations[index]`
- L'appel se fait avec `operations[index](parametres)`

---

## Simplification avec typedef

Comme pour les pointeurs de fonctions simples, on peut utiliser `typedef` pour rendre la syntaxe plus lisible :

```c
#include <stdio.h>

// Définition d'un type pour simplifier
typedef int (*OperationMath)(int, int);

int addition(int a, int b) { return a + b; }  
int soustraction(int a, int b) { return a - b; }  
int multiplication(int a, int b) { return a * b; }  
int division(int a, int b) {  
    return (b != 0) ? a / b : 0;
}

int main(void) {
    // Beaucoup plus lisible !
    OperationMath operations[4] = {
        addition,
        soustraction,
        multiplication,
        division
    };

    // Utilisation
    int resultat = operations[2](6, 7);  // Multiplication
    printf("6 * 7 = %d\n", resultat);

    return 0;
}
```

**Avantage** : Le code est beaucoup plus facile à lire et à maintenir.

---

## Table de dispatch : Remplacer switch-case

Une des utilisations les plus pratiques des tableaux de pointeurs de fonctions est de remplacer de longues structures `switch-case`.

### Avant : Avec switch-case

```c
#include <stdio.h>

int calculer_avec_switch(int a, int b, char operateur) {
    int resultat;

    switch (operateur) {
        case '+':
            resultat = a + b;
            break;
        case '-':
            resultat = a - b;
            break;
        case '*':
            resultat = a * b;
            break;
        case '/':
            resultat = (b != 0) ? a / b : 0;
            break;
        default:
            printf("Opérateur inconnu\n");
            resultat = 0;
    }

    return resultat;
}
```

### Après : Avec tableau de pointeurs de fonctions

```c
#include <stdio.h>
#include <string.h>

typedef int (*Operation)(int, int);

int addition(int a, int b) { return a + b; }  
int soustraction(int a, int b) { return a - b; }  
int multiplication(int a, int b) { return a * b; }  
int division(int a, int b) { return (b != 0) ? a / b : 0; }  

// Structure pour associer un symbole à une fonction
typedef struct {
    char symbole;
    Operation fonction;
} EntreeOperation;

int calculer_avec_tableau(int a, int b, char operateur) {
    // Table de dispatch
    EntreeOperation operations[] = {
        {'+', addition},
        {'-', soustraction},
        {'*', multiplication},
        {'/', division}
    };

    int nb_operations = sizeof(operations) / sizeof(operations[0]);

    // Recherche de l'opération correspondante
    for (int i = 0; i < nb_operations; i++) {
        if (operations[i].symbole == operateur) {
            return operations[i].fonction(a, b);
        }
    }

    printf("Opérateur inconnu\n");
    return 0;
}

int main(void) {
    printf("10 + 5 = %d\n", calculer_avec_tableau(10, 5, '+'));
    printf("10 - 5 = %d\n", calculer_avec_tableau(10, 5, '-'));
    printf("10 * 5 = %d\n", calculer_avec_tableau(10, 5, '*'));
    printf("10 / 5 = %d\n", calculer_avec_tableau(10, 5, '/'));

    return 0;
}
```

**Avantages de l'approche avec tableau** :
- ✅ Plus facile d'ajouter de nouvelles opérations
- ✅ Code plus modulaire et maintenable
- ✅ Moins de duplication
- ✅ Possibilité de charger les opérations dynamiquement

---

## Exemple pratique : Calculatrice interactive

Voici un exemple complet d'une calculatrice interactive utilisant un tableau de pointeurs de fonctions :

```c
#include <stdio.h>
#include <stdlib.h>

typedef double (*OperationCalc)(double, double);

// Fonctions d'opérations
double add(double a, double b) { return a + b; }  
double sub(double a, double b) { return a - b; }  
double mul(double a, double b) { return a * b; }  
double div_safe(double a, double b) {  
    if (b == 0.0) {
        printf("Erreur : division par zéro\n");
        return 0.0;
    }
    return a / b;
}
double puissance(double a, double b) {
    double resultat = 1.0;
    for (int i = 0; i < (int)b; i++) {
        resultat *= a;
    }
    return resultat;
}

// Structure pour la table de dispatch
typedef struct {
    char symbole;
    const char *nom;
    OperationCalc operation;
} CommandeCalculatrice;

void afficher_menu(CommandeCalculatrice commandes[], int nb_commandes) {
    printf("\n=== CALCULATRICE ===\n");
    printf("Opérations disponibles :\n");
    for (int i = 0; i < nb_commandes; i++) {
        printf("  %c : %s\n", commandes[i].symbole, commandes[i].nom);
    }
    printf("  q : Quitter\n");
    printf("===================\n");
}

int main(void) {
    // Table de dispatch des opérations
    CommandeCalculatrice commandes[] = {
        {'+', "Addition", add},
        {'-', "Soustraction", sub},
        {'*', "Multiplication", mul},
        {'/', "Division", div_safe},
        {'^', "Puissance", puissance}
    };

    int nb_commandes = sizeof(commandes) / sizeof(commandes[0]);

    char choix;
    double a, b, resultat;

    while (1) {
        afficher_menu(commandes, nb_commandes);

        printf("\nChoisissez une opération : ");
        scanf(" %c", &choix);

        if (choix == 'q' || choix == 'Q') {
            printf("Au revoir !\n");
            break;
        }

        // Recherche de l'opération
        int trouve = 0;
        for (int i = 0; i < nb_commandes; i++) {
            if (commandes[i].symbole == choix) {
                printf("Entrez le premier nombre : ");
                scanf("%lf", &a);
                printf("Entrez le second nombre : ");
                scanf("%lf", &b);

                resultat = commandes[i].operation(a, b);
                printf("\nRésultat : %.2f %c %.2f = %.2f\n",
                       a, choix, b, resultat);

                trouve = 1;
                break;
            }
        }

        if (!trouve) {
            printf("Opération inconnue !\n");
        }
    }

    return 0;
}
```

**Caractéristiques de cet exemple** :
- Menu dynamique généré à partir du tableau
- Recherche automatique de l'opération
- Facile d'ajouter de nouvelles opérations (il suffit d'ajouter une ligne dans le tableau)
- Structure claire et maintenable

---

## Machine à états finis (FSM)

Les tableaux de pointeurs de fonctions sont parfaits pour implémenter des machines à états. Voici un exemple simplifié d'un système de porte automatique :

```c
#include <stdio.h>
#include <unistd.h>  // Pour sleep()

// Énumération des états possibles
typedef enum {
    ETAT_FERMEE,
    ETAT_OUVERTURE,
    ETAT_OUVERTE,
    ETAT_FERMETURE,
    NB_ETATS
} EtatPorte;

// Énumération des événements
typedef enum {
    EVT_DETECTER_PERSONNE,
    EVT_TIMEOUT,
    EVT_OBSTACLE,
    EVT_AUCUN
} Evenement;

// Structure pour stocker l'état actuel
typedef struct {
    EtatPorte etat_actuel;
} Porte;

// Prototypes des fonctions d'état
void etat_fermee(Porte *p, Evenement evt);  
void etat_ouverture(Porte *p, Evenement evt);  
void etat_ouverte(Porte *p, Evenement evt);  
void etat_fermeture(Porte *p, Evenement evt);  

// Type pour les fonctions d'état
typedef void (*FonctionEtat)(Porte *, Evenement);

// Tableau de pointeurs vers les fonctions d'état
FonctionEtat table_etats[NB_ETATS] = {
    etat_fermee,
    etat_ouverture,
    etat_ouverte,
    etat_fermeture
};

// Implémentation des fonctions d'état
void etat_fermee(Porte *p, Evenement evt) {
    printf("État : FERMÉE\n");
    if (evt == EVT_DETECTER_PERSONNE) {
        printf("  → Personne détectée, ouverture...\n");
        p->etat_actuel = ETAT_OUVERTURE;
    }
}

void etat_ouverture(Porte *p, Evenement evt) {
    (void)evt;  // Non utilisé dans cet état
    printf("État : OUVERTURE EN COURS\n");
    printf("  → Moteur activé...\n");
    sleep(1);  // Simulation du temps d'ouverture
    p->etat_actuel = ETAT_OUVERTE;
}

void etat_ouverte(Porte *p, Evenement evt) {
    printf("État : OUVERTE\n");
    if (evt == EVT_TIMEOUT) {
        printf("  → Timeout, fermeture...\n");
        p->etat_actuel = ETAT_FERMETURE;
    }
}

void etat_fermeture(Porte *p, Evenement evt) {
    printf("État : FERMETURE EN COURS\n");
    if (evt == EVT_OBSTACLE) {
        printf("  → Obstacle détecté ! Réouverture...\n");
        p->etat_actuel = ETAT_OUVERTURE;
    } else {
        printf("  → Moteur activé...\n");
        sleep(1);  // Simulation du temps de fermeture
        p->etat_actuel = ETAT_FERMEE;
    }
}

// Fonction pour traiter un événement
void traiter_evenement(Porte *p, Evenement evt) {
    // Appel de la fonction d'état appropriée via le tableau
    table_etats[p->etat_actuel](p, evt);
}

int main(void) {
    Porte porte = {ETAT_FERMEE};

    printf("=== SIMULATION PORTE AUTOMATIQUE ===\n\n");

    // Simulation d'une séquence d'événements
    traiter_evenement(&porte, EVT_AUCUN);
    traiter_evenement(&porte, EVT_DETECTER_PERSONNE);
    traiter_evenement(&porte, EVT_AUCUN);
    traiter_evenement(&porte, EVT_AUCUN);
    traiter_evenement(&porte, EVT_TIMEOUT);
    traiter_evenement(&porte, EVT_OBSTACLE);
    traiter_evenement(&porte, EVT_AUCUN);
    traiter_evenement(&porte, EVT_AUCUN);
    traiter_evenement(&porte, EVT_AUCUN);

    printf("\n=== FIN DE LA SIMULATION ===\n");

    return 0;
}
```

**Avantages de cette approche** :
- Code très structuré et lisible
- Facile d'ajouter de nouveaux états
- Transitions clairement définies
- Évite les longues chaînes de `if-else`

---

## Interpréteur de commandes simplifié

Voici un exemple d'interpréteur de commandes basique utilisant un tableau de pointeurs de fonctions :

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Type pour les fonctions de commandes
typedef void (*FonctionCommande)(void);

// Implémentation des commandes
void cmd_aide(void) {
    printf("Commandes disponibles :\n");
    printf("  help   - Affiche cette aide\n");
    printf("  date   - Affiche la date système\n");
    printf("  clear  - Efface l'écran\n");
    printf("  info   - Informations système\n");
    printf("  quit   - Quitter\n");
}

void cmd_date(void) {
    printf("Simulation : Date actuelle\n");
}

void cmd_clear(void) {
    // Sur Linux/Unix, utilisez system("clear")
    // Sur Windows, utilisez system("cls")
    printf("\n\n[Écran effacé]\n\n");
}

void cmd_info(void) {
    printf("Système : Linux\n");
    printf("Architecture : x86_64\n");
    printf("Interpréteur : SimpleSH v1.0\n");
}

void cmd_quit(void) {
    printf("Au revoir !\n");
    exit(0);
}

void cmd_inconnue(void) {
    printf("Commande inconnue. Tapez 'help' pour l'aide.\n");
}

// Structure associant une commande à sa fonction
typedef struct {
    const char *nom;
    FonctionCommande fonction;
    const char *description;
} Commande;

int main(void) {
    // Table de dispatch des commandes
    Commande commandes[] = {
        {"help",  cmd_aide,     "Affiche l'aide"},
        {"date",  cmd_date,     "Affiche la date"},
        {"clear", cmd_clear,    "Efface l'écran"},
        {"info",  cmd_info,     "Informations système"},
        {"quit",  cmd_quit,     "Quitter"}
    };

    int nb_commandes = sizeof(commandes) / sizeof(commandes[0]);
    char buffer[256];

    printf("=== SimpleSH - Interpréteur de commandes ===\n");
    printf("Tapez 'help' pour voir les commandes disponibles\n\n");

    while (1) {
        printf("$ ");
        fflush(stdout);

        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            break;
        }

        // Supprimer le retour à la ligne
        buffer[strcspn(buffer, "\n")] = 0;

        // Ignorer les lignes vides
        if (strlen(buffer) == 0) {
            continue;
        }

        // Recherche et exécution de la commande
        int trouve = 0;
        for (int i = 0; i < nb_commandes; i++) {
            if (strcmp(buffer, commandes[i].nom) == 0) {
                commandes[i].fonction();
                trouve = 1;
                break;
            }
        }

        if (!trouve) {
            cmd_inconnue();
        }

        printf("\n");
    }

    return 0;
}
```

**Points intéressants** :
- Séparation claire entre la logique de recherche et l'exécution
- Facile d'ajouter de nouvelles commandes
- Structure extensible (on pourrait ajouter des paramètres aux commandes)

---

## Initialisation partielle et tableaux creux

Vous pouvez initialiser partiellement un tableau de pointeurs de fonctions et vérifier les pointeurs NULL :

```c
#include <stdio.h>

typedef void (*Handler)(int);

void handler_0(int code) { printf("Handler 0 : code %d\n", code); }  
void handler_2(int code) { printf("Handler 2 : code %d\n", code); }  
void handler_5(int code) { printf("Handler 5 : code %d\n", code); }  

int main(void) {
    // Tableau avec initialisation partielle
    Handler handlers[10] = {
        [0] = handler_0,
        [2] = handler_2,
        [5] = handler_5
        // Les autres éléments sont automatiquement NULL
    };

    // Exécution sécurisée
    for (int i = 0; i < 10; i++) {
        if (handlers[i] != NULL) {
            printf("Appel du handler %d : ", i);
            handlers[i](i * 100);
        } else {
            printf("Handler %d : non défini\n", i);
        }
    }

    return 0;
}
```

**Sortie** :
```
Appel du handler 0 : Handler 0 : code 0  
Handler 1 : non défini  
Appel du handler 2 : Handler 2 : code 200  
Handler 3 : non défini  
Handler 4 : non défini  
Appel du handler 5 : Handler 5 : code 500  
Handler 6 : non défini  
Handler 7 : non défini  
Handler 8 : non défini  
Handler 9 : non défini  
```

**Important** : Toujours vérifier qu'un pointeur n'est pas NULL avant de l'appeler !

---

## Tableaux multidimensionnels de pointeurs de fonctions

Pour des cas complexes, vous pouvez créer des tableaux à plusieurs dimensions :

```c
#include <stdio.h>

typedef int (*Operation)(int, int);

int add(int a, int b) { return a + b; }  
int sub(int a, int b) { return a - b; }  
int mul(int a, int b) { return a * b; }  
int div_safe(int a, int b) { return (b != 0) ? a / b : 0; }  
int mod(int a, int b) { return (b != 0) ? a % b : 0; }  
int max(int a, int b) { return (a > b) ? a : b; }  

int main(void) {
    // Tableau 2D : catégories d'opérations
    Operation operations[2][3] = {
        // Catégorie 0 : Opérations arithmétiques de base
        {add, sub, mul},
        // Catégorie 1 : Opérations avancées
        {div_safe, mod, max}
    };

    printf("Catégorie 0, Opération 0 (addition) : 10 + 5 = %d\n",
           operations[0][0](10, 5));
    printf("Catégorie 0, Opération 2 (multiplication) : 10 * 5 = %d\n",
           operations[0][2](10, 5));
    printf("Catégorie 1, Opération 1 (modulo) : 10 %% 3 = %d\n",
           operations[1][1](10, 3));
    printf("Catégorie 1, Opération 2 (maximum) : max(10, 5) = %d\n",
           operations[1][2](10, 5));

    return 0;
}
```

---

## Passage de tableaux de pointeurs de fonctions

Vous pouvez passer des tableaux de pointeurs de fonctions à d'autres fonctions :

```c
#include <stdio.h>

typedef int (*OperationBinaire)(int, int);

int addition(int a, int b) { return a + b; }  
int multiplication(int a, int b) { return a * b; }  

// Fonction qui reçoit un tableau de pointeurs de fonctions
void appliquer_operations(int x, int y,
                          OperationBinaire ops[],
                          int nb_ops) {
    printf("Traitement de %d et %d :\n", x, y);
    for (int i = 0; i < nb_ops; i++) {
        if (ops[i] != NULL) {
            printf("  Opération %d : %d\n", i, ops[i](x, y));
        }
    }
}

int main(void) {
    OperationBinaire mes_operations[] = {addition, multiplication};
    int nombre_operations = sizeof(mes_operations) / sizeof(mes_operations[0]);

    appliquer_operations(7, 3, mes_operations, nombre_operations);

    return 0;
}
```

**Sortie** :
```
Traitement de 7 et 3 :
  Opération 0 : 10
  Opération 1 : 21
```

---

## Cas d'usage dans le monde réel

### 1. Gestionnaire d'interruptions (Embedded)

Dans les systèmes embarqués, on utilise souvent des tableaux de pointeurs de fonctions pour gérer les interruptions :

```c
typedef void (*ISR)(void);  // Interrupt Service Routine

// Tableau des gestionnaires d'interruptions
ISR vecteurs_interruptions[256];

void timer_interrupt(void) {
    // Gérer l'interruption du timer
}

void uart_interrupt(void) {
    // Gérer l'interruption UART
}

// Configuration
vecteurs_interruptions[TIMER_IRQ] = timer_interrupt;  
vecteurs_interruptions[UART_IRQ] = uart_interrupt;  

// Quand une interruption survient :
void dispatcher_interruption(int numero_irq) {
    if (vecteurs_interruptions[numero_irq] != NULL) {
        vecteurs_interruptions[numero_irq]();
    }
}
```

### 2. Interpréteur de bytecode

Les machines virtuelles (JVM, Python VM) utilisent des tables de dispatch pour exécuter les opcodes :

```c
typedef void (*InstructionHandler)(void);

void op_add(void) { /* Addition */ }  
void op_sub(void) { /* Soustraction */ }  
void op_mul(void) { /* Multiplication */ }  
void op_load(void) { /* Charger une valeur */ }  
void op_store(void) { /* Stocker une valeur */ }  

InstructionHandler opcodes[256] = {
    [0x01] = op_add,
    [0x02] = op_sub,
    [0x03] = op_mul,
    [0x10] = op_load,
    [0x11] = op_store
    // ... 251 autres opcodes
};

void executer_bytecode(unsigned char instruction) {
    if (opcodes[instruction] != NULL) {
        opcodes[instruction]();
    }
}
```

### 3. Framework de test unitaire

```c
typedef void (*TestFunction)(void);

void test_addition(void) {
    assert(addition(2, 3) == 5);
    printf("✓ test_addition passed\n");
}

void test_multiplication(void) {
    assert(multiplication(4, 5) == 20);
    printf("✓ test_multiplication passed\n");
}

TestFunction suite_tests[] = {
    test_addition,
    test_multiplication,
    // Ajouter d'autres tests ici
};

void executer_tous_les_tests(void) {
    int nb_tests = sizeof(suite_tests) / sizeof(suite_tests[0]);
    for (int i = 0; i < nb_tests; i++) {
        suite_tests[i]();
    }
}
```

---

## Pièges courants et comment les éviter

### 1. Oublier de vérifier NULL

```c
// ❌ DANGEREUX : pas de vérification
operations[index](a, b);

// ✅ CORRECT : toujours vérifier
if (operations[index] != NULL) {
    operations[index](a, b);
}
```

### 2. Signatures incompatibles

```c
int func_int(int a, int b) { return a + b; }  
float func_float(float a, float b) { return a + b; }  

int (*ops[2])(int, int) = {
    func_int,      // ✅ OK
    // func_float  // ❌ ERREUR : signature différente
};
```

### 3. Débordement de tableau

```c
int (*operations[4])(int, int) = { /* ... */ };

// ❌ DANGEREUX : pas de vérification de limite
int index = 10;  
operations[index](5, 3);  // Comportement indéfini !  

// ✅ CORRECT : vérifier les limites
if (index >= 0 && index < 4 && operations[index] != NULL) {
    operations[index](5, 3);
}
```

### 4. Initialisation incomplète

```c
// ⚠️ ATTENTION : certains éléments seront NULL
int (*operations[10])(int, int) = {
    addition,
    soustraction
    // Les 8 autres pointeurs sont NULL !
};

// Solution : initialiser explicitement ou vérifier
```

---

## Optimisation et performance

### Comparaison avec switch-case

**Switch-case** :
- ➕ Peut être optimisé par le compilateur (jump table)
- ➕ Syntaxe plus familière
- ➖ Code répétitif
- ➖ Moins flexible

**Tableau de pointeurs** :
- ➕ Très flexible (modification dynamique possible)
- ➕ Code plus modulaire
- ➕ Facile d'ajouter/retirer des éléments
- ➖ Indirection supplémentaire (impact minimal sur les CPU modernes)
- ➖ Risque de pointeurs NULL

**Verdict** : Pour des performances critiques avec un nombre fixe de cas, `switch-case` peut être légèrement plus rapide. Pour de la flexibilité et de la maintenabilité, les tableaux de pointeurs sont supérieurs.

---

## Bonnes pratiques

### ✅ À FAIRE

1. **Utiliser typedef pour la lisibilité**
```c
typedef void (*EventHandler)(int);  
EventHandler handlers[MAX_EVENTS];  
```

2. **Toujours vérifier les pointeurs NULL**
```c
if (handlers[event_id] != NULL) {
    handlers[event_id](data);
}
```

3. **Documenter la table**
```c
// Table de dispatch des commandes réseau
// Index 0: PING, Index 1: CONNECT, Index 2: DISCONNECT
CommandHandler network_commands[3] = { /* ... */ };
```

4. **Initialiser explicitement**
```c
Handler handlers[MAX] = {
    [CMD_START] = handle_start,
    [CMD_STOP] = handle_stop
};
```

5. **Utiliser des constantes pour les index**
```c
#define OP_ADD 0
#define OP_SUB 1
operations[OP_ADD](10, 5);  // Plus clair que operations[0]
```

### ❌ À ÉVITER

1. **Index magiques**
```c
// ❌ Mauvais : que représente l'index 3 ?
operations[3](x, y);

// ✅ Bon : utiliser une constante ou enum
operations[OP_DIVIDE](x, y);
```

2. **Tableaux trop grands**
```c
// ❌ Gaspillage de mémoire si peu utilisé
Handler handlers[10000];
```

3. **Pas de documentation**
```c
// ❌ Quel handler fait quoi ?
handlers[0] = func1;  
handlers[1] = func2;  
```

---

## Comparaison avec d'autres langages

| Concept C | Python | JavaScript | Rust |
|-----------|--------|------------|------|
| `int (*ops[4])(int, int)` | `ops = [add, sub, mul, div]` | `const ops = [add, sub, mul, div]` | `let ops: [fn(i32, i32) -> i32; 4]` |
| Appel : `ops[0](5, 3)` | `ops[0](5, 3)` | `ops[0](5, 3)` | `ops[0](5, 3)` |
| Typage : Statique strict | Dynamique | Dynamique | Statique strict |

**Note** : En C, contrairement aux langages de plus haut niveau, vous devez gérer manuellement la sécurité (vérification NULL, limites de tableau).

---

## Aller plus loin

Maintenant que vous maîtrisez les tableaux de pointeurs de fonctions, vous pouvez explorer :

- **Pointeurs multi-niveaux** (section 22.3) : Tableaux de tableaux de pointeurs de fonctions
- **Pointeurs opaques** (section 22.4) : Cacher l'implémentation
- **Fonctions variadiques** (section 22.6) : Callbacks avec nombre variable de paramètres
- **Plugin systems** : Chargement dynamique de fonctions depuis des bibliothèques partagées

### Projets suggérés pour pratiquer

1. **Mini-shell** : Interpréteur de commandes avec historique
2. **Machine à états** : Simulateur de feu tricolore ou distributeur automatique
3. **Calculatrice RPN** : Utilise une pile et un tableau d'opérations
4. **Framework de jeu simple** : Gestionnaire d'événements pour un jeu texte

---

## Résumé

Les tableaux de pointeurs de fonctions sont un outil puissant pour :
- ✅ Créer des **tables de dispatch** efficaces
- ✅ Implémenter des **machines à états**
- ✅ Construire des **interpréteurs** et des **parsers**
- ✅ Gérer des **événements** et des **callbacks** multiples
- ✅ Écrire du code **modulaire** et **extensible**

**Points clés** :
- Syntaxe : `type_retour (*nom[taille])(parametres)`
- Utilisez `typedef` pour simplifier
- Toujours vérifier les pointeurs NULL
- Vérifier les limites du tableau
- Documenter l'usage de chaque index

**Quand utiliser** :
- Remplacement de longs `switch-case`
- Gestion d'événements multiples
- Systèmes configurables/extensibles
- Machines à états complexes

**Quand éviter** :
- Cas simples avec 2-3 options (un `if-else` suffit)
- Quand la performance est absolument critique (préférer `switch-case` optimisé)
- Quand la clarté du code en souffre

---


⏭️ [Pointeurs multi-niveaux](/22-pointeurs-avances/03-pointeurs-multi-niveaux.md)
