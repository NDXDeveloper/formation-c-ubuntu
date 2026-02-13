🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 10.5 enum : Code lisible et gestion d'états

## Introduction

Imaginez que vous développez un jeu vidéo. Vous devez représenter les différentes directions possibles : haut, bas, gauche, droite. Vous pourriez utiliser des entiers :

```c
int direction = 0;  // 0 = haut, 1 = bas, 2 = gauche, 3 = droite
```

**Problèmes** :
- Que signifie `direction = 2` ? Il faut se référer aux commentaires
- Facile de faire une erreur : `direction = 42` compile sans erreur
- Code illisible et difficile à maintenir

**Solution** : Les **énumérations** (`enum`)

```c
enum Direction {
    HAUT,
    BAS,
    GAUCHE,
    DROITE
};

enum Direction direction = HAUT;  // ✅ Code lisible !
```

Une énumération permet de définir un ensemble de **constantes nommées**, rendant le code beaucoup plus clair et moins sujet aux erreurs.

---

## Qu'est-ce qu'une énumération ?

### Définition

Une **énumération** (ou `enum`) est un type de données qui définit un ensemble de **constantes entières nommées**.

### Syntaxe de base

```c
enum NomEnumeration {
    CONSTANTE1,
    CONSTANTE2,
    CONSTANTE3
};
```

Par défaut, les valeurs commencent à **0** et s'incrémentent de 1 :
- `CONSTANTE1` = 0
- `CONSTANTE2` = 1
- `CONSTANTE3` = 2

### Premier exemple

```c
#include <stdio.h>

enum Jour {
    LUNDI,      // 0
    MARDI,      // 1
    MERCREDI,   // 2
    JEUDI,      // 3
    VENDREDI,   // 4
    SAMEDI,     // 5
    DIMANCHE    // 6
};

int main() {
    enum Jour aujourd_hui = MERCREDI;

    printf("Aujourd'hui est le jour numéro : %d\n", aujourd_hui);

    if (aujourd_hui == MERCREDI) {
        printf("C'est mercredi !\n");
    }

    return 0;
}
```

**Sortie** :
```
Aujourd'hui est le jour numéro : 2  
C'est mercredi !  
```

---

## Valeurs personnalisées

### Définir des valeurs explicites

Vous pouvez spécifier les valeurs de chaque constante :

```c
enum CouleursRGB {
    ROUGE = 0xFF0000,
    VERT  = 0x00FF00,
    BLEU  = 0x0000FF
};
```

### Mélanger valeurs automatiques et explicites

```c
#include <stdio.h>

enum Exemple {
    A,        // 0 (automatique)
    B,        // 1 (automatique)
    C = 10,   // 10 (explicite)
    D,        // 11 (continue à partir de C)
    E = 100,  // 100 (explicite)
    F         // 101 (continue à partir de E)
};

int main() {
    printf("A = %d\n", A);  // 0
    printf("B = %d\n", B);  // 1
    printf("C = %d\n", C);  // 10
    printf("D = %d\n", D);  // 11
    printf("E = %d\n", E);  // 100
    printf("F = %d\n", F);  // 101

    return 0;
}
```

### Valeurs négatives et dupliquées

```c
enum Temperature {
    FROID = -10,
    FRAIS = 0,
    TIEDE = 10,
    CHAUD = 20,
    TRES_CHAUD = 20  // ✅ Valeurs dupliquées autorisées
};
```

---

## Utilisation avec switch/case

Les énumérations sont idéales avec `switch` car elles rendent le code très lisible :

```c
#include <stdio.h>

enum Statut {
    EN_ATTENTE,
    EN_COURS,
    TERMINE,
    ERREUR
};

void afficher_statut(enum Statut s) {
    switch (s) {
        case EN_ATTENTE:
            printf("⏳ Tâche en attente\n");
            break;
        case EN_COURS:
            printf("⚙️  Tâche en cours d'exécution\n");
            break;
        case TERMINE:
            printf("✅ Tâche terminée avec succès\n");
            break;
        case ERREUR:
            printf("❌ Erreur lors de l'exécution\n");
            break;
        default:
            printf("⚠️  Statut inconnu\n");
    }
}

int main() {
    enum Statut tache1 = EN_COURS;
    enum Statut tache2 = TERMINE;

    afficher_statut(tache1);
    afficher_statut(tache2);

    return 0;
}
```

**Sortie** :
```
⚙️  Tâche en cours d'exécution
✅ Tâche terminée avec succès
```

---

## enum vs #define

### Avec #define (ancienne méthode)

```c
#define ROUGE 0
#define VERT  1
#define BLEU  2

int couleur = ROUGE;
```

**Problèmes** :
- Pas de vérification de type
- Pollution de l'espace de noms global
- Pas de portée locale
- Difficile à déboguer

### Avec enum (méthode recommandée)

```c
enum Couleur {
    ROUGE,
    VERT,
    BLEU
};

enum Couleur couleur = ROUGE;
```

**Avantages** :
- Vérification de type par le compilateur
- Portée limitée au type enum
- Meilleure intégration avec les outils de débogage
- Documentation automatique du code

### Comparaison pratique

```c
// Avec #define
#define STATUS_OK 0
#define STATUS_ERROR 1

int status = STATUS_OK;  
status = 42;  // ✅ Compile, mais incorrect !  

// Avec enum
enum Status {
    STATUS_OK,
    STATUS_ERROR
};

enum Status status = STATUS_OK;  
status = 42;  // ⚠️ Compile sans erreur ni warning en C (enum = int)
```

---

## Cas d'usage typiques

### 1. Codes d'erreur

```c
enum CodeErreur {
    SUCCES = 0,
    ERREUR_FICHIER = -1,
    ERREUR_MEMOIRE = -2,
    ERREUR_RESEAU = -3,
    ERREUR_PERMISSION = -4
};

enum CodeErreur ouvrir_fichier(const char *nom) {
    // Simulation
    if (nom == NULL) {
        return ERREUR_FICHIER;
    }
    // ...
    return SUCCES;
}

int main() {
    enum CodeErreur resultat = ouvrir_fichier("test.txt");

    if (resultat == SUCCES) {
        printf("Fichier ouvert avec succès\n");
    } else {
        printf("Erreur : code %d\n", resultat);
    }

    return 0;
}
```

### 2. Machine à états

```c
#include <stdio.h>

enum EtatConnexion {
    DECONNECTE,
    CONNEXION_EN_COURS,
    CONNECTE,
    DECONNEXION_EN_COURS
};

struct Connexion {
    enum EtatConnexion etat;
    char serveur[256];
};

void changer_etat(struct Connexion *conn, enum EtatConnexion nouvel_etat) {
    printf("Transition : ");

    switch (conn->etat) {
        case DECONNECTE:           printf("DECONNECTE"); break;
        case CONNEXION_EN_COURS:   printf("CONNEXION_EN_COURS"); break;
        case CONNECTE:             printf("CONNECTE"); break;
        case DECONNEXION_EN_COURS: printf("DECONNEXION_EN_COURS"); break;
    }

    printf(" → ");

    switch (nouvel_etat) {
        case DECONNECTE:           printf("DECONNECTE\n"); break;
        case CONNEXION_EN_COURS:   printf("CONNEXION_EN_COURS\n"); break;
        case CONNECTE:             printf("CONNECTE\n"); break;
        case DECONNEXION_EN_COURS: printf("DECONNEXION_EN_COURS\n"); break;
    }

    conn->etat = nouvel_etat;
}

int main() {
    struct Connexion conn = {DECONNECTE, "serveur.exemple.com"};

    changer_etat(&conn, CONNEXION_EN_COURS);
    changer_etat(&conn, CONNECTE);
    changer_etat(&conn, DECONNEXION_EN_COURS);
    changer_etat(&conn, DECONNECTE);

    return 0;
}
```

**Sortie** :
```
Transition : DECONNECTE → CONNEXION_EN_COURS  
Transition : CONNEXION_EN_COURS → CONNECTE  
Transition : CONNECTE → DECONNEXION_EN_COURS  
Transition : DECONNEXION_EN_COURS → DECONNECTE  
```

### 3. Jours de la semaine

```c
#include <stdio.h>

enum Jour {
    LUNDI,
    MARDI,
    MERCREDI,
    JEUDI,
    VENDREDI,
    SAMEDI,
    DIMANCHE
};

const char* nom_jour(enum Jour j) {
    switch (j) {
        case LUNDI:    return "Lundi";
        case MARDI:    return "Mardi";
        case MERCREDI: return "Mercredi";
        case JEUDI:    return "Jeudi";
        case VENDREDI: return "Vendredi";
        case SAMEDI:   return "Samedi";
        case DIMANCHE: return "Dimanche";
        default:       return "Jour inconnu";
    }
}

int est_weekend(enum Jour j) {
    return (j == SAMEDI || j == DIMANCHE);
}

int main() {
    enum Jour aujourd_hui = SAMEDI;

    printf("Aujourd'hui : %s\n", nom_jour(aujourd_hui));

    if (est_weekend(aujourd_hui)) {
        printf("C'est le weekend ! 🎉\n");
    } else {
        printf("C'est un jour de semaine 😴\n");
    }

    return 0;
}
```

### 4. Options de configuration

```c
#include <stdio.h>

enum Options {
    OPTION_VERBOSE      = 0x01,  // Bit 0
    OPTION_DEBUG        = 0x02,  // Bit 1
    OPTION_COULEUR      = 0x04,  // Bit 2
    OPTION_COMPRESSION  = 0x08   // Bit 3
};

int main() {
    unsigned int config = 0;

    // Activer des options (avec opérateur OR)
    config |= OPTION_VERBOSE;
    config |= OPTION_DEBUG;

    // Vérifier si une option est active (avec opérateur AND)
    if (config & OPTION_VERBOSE) {
        printf("Mode verbose activé\n");
    }

    if (config & OPTION_DEBUG) {
        printf("Mode debug activé\n");
    }

    if (config & OPTION_COULEUR) {
        printf("Couleurs activées\n");
    } else {
        printf("Couleurs désactivées\n");
    }

    return 0;
}
```

**Sortie** :
```
Mode verbose activé  
Mode debug activé  
Couleurs désactivées  
```

### 5. Priorités de tâches

```c
#include <stdio.h>

enum Priorite {
    CRITIQUE = 0,
    HAUTE    = 1,
    NORMALE  = 2,
    BASSE    = 3
};

struct Tache {
    char description[100];
    enum Priorite priorite;
};

const char* nom_priorite(enum Priorite p) {
    switch (p) {
        case CRITIQUE: return "🔴 CRITIQUE";
        case HAUTE:    return "🟠 HAUTE";
        case NORMALE:  return "🟡 NORMALE";
        case BASSE:    return "🟢 BASSE";
        default:       return "❓ INCONNUE";
    }
}

int main() {
    struct Tache taches[] = {
        {"Corriger bug de sécurité", CRITIQUE},
        {"Implémenter nouvelle fonctionnalité", HAUTE},
        {"Mettre à jour documentation", NORMALE},
        {"Nettoyer le code", BASSE}
    };

    printf("=== LISTE DES TÂCHES ===\n\n");

    for (int i = 0; i < 4; i++) {
        printf("%s - %s\n",
               nom_priorite(taches[i].priorite),
               taches[i].description);
    }

    return 0;
}
```

**Sortie** :
```
=== LISTE DES TÂCHES ===

🔴 CRITIQUE - Corriger bug de sécurité
🟠 HAUTE - Implémenter nouvelle fonctionnalité
🟡 NORMALE - Mettre à jour documentation
🟢 BASSE - Nettoyer le code
```

---

## Bonnes pratiques de nommage

### Convention MAJUSCULES

La convention la plus courante est d'utiliser des **MAJUSCULES** pour les constantes :

```c
enum Statut {
    STATUT_OK,
    STATUT_ERREUR,
    STATUT_TIMEOUT
};
```

### Préfixe pour éviter les conflits

Utilisez un préfixe pour éviter les collisions de noms :

```c
enum HttpStatus {
    HTTP_OK = 200,
    HTTP_NOT_FOUND = 404,
    HTTP_SERVER_ERROR = 500
};

enum FtpStatus {
    FTP_OK = 200,
    FTP_NOT_FOUND = 550
};

// Pas de conflit entre HTTP_OK et FTP_OK
```

### Nom de l'enum descriptif

```c
// ❌ Mauvais : trop générique
enum Status { OK, ERROR };

// ✅ Bon : descriptif
enum NetworkStatus { NETWORK_OK, NETWORK_ERROR };  
enum FileStatus { FILE_OK, FILE_ERROR };  
```

---

## typedef avec enum

Pour simplifier la syntaxe, on peut utiliser `typedef` :

### Sans typedef

```c
enum Couleur {
    ROUGE,
    VERT,
    BLEU
};

enum Couleur ma_couleur = ROUGE;  // Répétitif
```

### Avec typedef

```c
typedef enum {
    ROUGE,
    VERT,
    BLEU
} Couleur;

Couleur ma_couleur = ROUGE;  // Plus concis !
```

### Avec nom d'enum et typedef

```c
typedef enum Couleur {
    ROUGE,
    VERT,
    BLEU
} Couleur;

// Les deux syntaxes fonctionnent :
enum Couleur c1 = ROUGE;  
Couleur c2 = VERT;  
```

---

## Enumérations et boucles

On peut itérer sur les valeurs d'une énumération :

```c
#include <stdio.h>

enum Jour {
    LUNDI,
    MARDI,
    MERCREDI,
    JEUDI,
    VENDREDI,
    SAMEDI,
    DIMANCHE,
    NB_JOURS  // Astuce : nombre total de jours
};

const char* noms_jours[] = {
    "Lundi",
    "Mardi",
    "Mercredi",
    "Jeudi",
    "Vendredi",
    "Samedi",
    "Dimanche"
};

int main() {
    printf("Jours de la semaine :\n");

    for (enum Jour j = LUNDI; j < NB_JOURS; j++) {
        printf("%d : %s\n", j, noms_jours[j]);
    }

    return 0;
}
```

**Sortie** :
```
Jours de la semaine :
0 : Lundi
1 : Mardi
2 : Mercredi
3 : Jeudi
4 : Vendredi
5 : Samedi
6 : Dimanche
```

---

## Validation des valeurs

### Vérifier qu'une valeur est valide

```c
#include <stdio.h>

enum Direction {
    NORD,
    SUD,
    EST,
    OUEST,
    NB_DIRECTIONS
};

int direction_valide(enum Direction d) {
    return (d >= NORD && d < NB_DIRECTIONS);
}

void deplacer(enum Direction d) {
    if (!direction_valide(d)) {
        printf("❌ Direction invalide !\n");
        return;
    }

    switch (d) {
        case NORD:  printf("⬆️  Déplacement vers le nord\n"); break;
        case SUD:   printf("⬇️  Déplacement vers le sud\n"); break;
        case EST:   printf("➡️  Déplacement vers l'est\n"); break;
        case OUEST: printf("⬅️  Déplacement vers l'ouest\n"); break;
        default:    break;
    }
}

int main() {
    deplacer(NORD);
    deplacer(EST);
    deplacer(42);  // Invalide !

    return 0;
}
```

**Sortie** :
```
⬆️  Déplacement vers le nord
➡️  Déplacement vers l'est
❌ Direction invalide !
```

---

## Limites et précautions

### 1. Les enums sont des entiers

```c
enum Couleur {
    ROUGE,
    VERT,
    BLEU
};

enum Couleur c = ROUGE;  
c = 42;  // ⚠️ Compile, mais incorrect !  

// Le compilateur peut avertir avec -Wall -Wconversion
```

### 2. Pas de vérification automatique de type strict

```c
enum Animal { CHAT, CHIEN };  
enum Vehicule { VOITURE, VELO };  

enum Animal a = CHAT;  
enum Vehicule v = a;  // ⚠️ Compile sans erreur (tous deux sont des int)  
```

### 3. Portée globale des constantes

```c
enum Etat1 { ACTIF, INACTIF };  
enum Etat2 { ACTIF, SUSPENDU };  // ❌ Erreur : ACTIF déjà défini  

// Solution : utiliser des préfixes
enum Etat1 { ETAT1_ACTIF, ETAT1_INACTIF };  
enum Etat2 { ETAT2_ACTIF, ETAT2_SUSPENDU };  
```

### 4. Taille de stockage

La taille d'un enum est celle d'un `int` (généralement 4 octets) :

```c
enum Petit { A, B };

printf("Taille : %zu octets\n", sizeof(enum Petit));  // 4 octets
```

Si vous avez besoin d'économiser de la mémoire, utilisez des types explicites :

```c
uint8_t etat = 0;  // 1 octet au lieu de 4
```

---

## Exemple avancé : Analyseur de commandes

```c
#include <stdio.h>
#include <string.h>

typedef enum {
    CMD_AIDE,
    CMD_QUITTER,
    CMD_LISTE,
    CMD_AJOUTER,
    CMD_SUPPRIMER,
    CMD_INCONNUE
} TypeCommande;

typedef struct {
    const char *nom;
    TypeCommande type;
    const char *description;
} Commande;

const Commande commandes[] = {
    {"aide",      CMD_AIDE,      "Affiche l'aide"},
    {"quitter",   CMD_QUITTER,   "Quitte le programme"},
    {"liste",     CMD_LISTE,     "Liste tous les éléments"},
    {"ajouter",   CMD_AJOUTER,   "Ajoute un élément"},
    {"supprimer", CMD_SUPPRIMER, "Supprime un élément"},
    {NULL,        CMD_INCONNUE,  NULL}
};

TypeCommande parser_commande(const char *input) {
    for (int i = 0; commandes[i].nom != NULL; i++) {
        if (strcmp(input, commandes[i].nom) == 0) {
            return commandes[i].type;
        }
    }
    return CMD_INCONNUE;
}

void afficher_aide() {
    printf("\n=== COMMANDES DISPONIBLES ===\n\n");
    for (int i = 0; commandes[i].nom != NULL; i++) {
        printf("  %-12s - %s\n", commandes[i].nom, commandes[i].description);
    }
    printf("\n");
}

void executer_commande(TypeCommande cmd) {
    switch (cmd) {
        case CMD_AIDE:
            afficher_aide();
            break;

        case CMD_QUITTER:
            printf("Au revoir !\n");
            break;

        case CMD_LISTE:
            printf("Liste des éléments : [élément1, élément2, élément3]\n");
            break;

        case CMD_AJOUTER:
            printf("Ajout d'un nouvel élément...\n");
            break;

        case CMD_SUPPRIMER:
            printf("Suppression d'un élément...\n");
            break;

        case CMD_INCONNUE:
            printf("❌ Commande inconnue. Tapez 'aide' pour voir les commandes.\n");
            break;
    }
}

int main() {
    const char *tests[] = {
        "aide",
        "liste",
        "ajouter",
        "blabla",
        "quitter"
    };

    for (int i = 0; i < 5; i++) {
        printf("> %s\n", tests[i]);
        TypeCommande cmd = parser_commande(tests[i]);
        executer_commande(cmd);
        printf("\n");
    }

    return 0;
}
```

**Sortie** :
```
> aide

=== COMMANDES DISPONIBLES ===

  aide         - Affiche l'aide
  quitter      - Quitte le programme
  liste        - Liste tous les éléments
  ajouter      - Ajoute un élément
  supprimer    - Supprime un élément


> liste
Liste des éléments : [élément1, élément2, élément3]

> ajouter
Ajout d'un nouvel élément...

> blabla
❌ Commande inconnue. Tapez 'aide' pour voir les commandes.

> quitter
Au revoir !
```

---

## Conversions entre enum et string

### enum vers string

```c
#include <stdio.h>

typedef enum {
    ETAT_DEMARRAGE,
    ETAT_EXECUTION,
    ETAT_PAUSE,
    ETAT_ARRET
} Etat;

const char* etat_vers_string(Etat e) {
    switch (e) {
        case ETAT_DEMARRAGE: return "Démarrage";
        case ETAT_EXECUTION: return "Exécution";
        case ETAT_PAUSE:     return "Pause";
        case ETAT_ARRET:     return "Arrêt";
        default:             return "Inconnu";
    }
}

int main() {
    Etat etats[] = {ETAT_DEMARRAGE, ETAT_EXECUTION, ETAT_PAUSE, ETAT_ARRET};

    for (int i = 0; i < 4; i++) {
        printf("État %d : %s\n", etats[i], etat_vers_string(etats[i]));
    }

    return 0;
}
```

### string vers enum

```c
#include <stdio.h>
#include <string.h>

typedef enum {
    ETAT_DEMARRAGE,
    ETAT_EXECUTION,
    ETAT_PAUSE,
    ETAT_ARRET,
    ETAT_INVALIDE
} Etat;

Etat string_vers_etat(const char *s) {
    if (strcmp(s, "demarrage") == 0) return ETAT_DEMARRAGE;
    if (strcmp(s, "execution") == 0) return ETAT_EXECUTION;
    if (strcmp(s, "pause") == 0)     return ETAT_PAUSE;
    if (strcmp(s, "arret") == 0)     return ETAT_ARRET;
    return ETAT_INVALIDE;
}

int main() {
    const char *tests[] = {"execution", "pause", "inconnu"};

    for (int i = 0; i < 3; i++) {
        Etat e = string_vers_etat(tests[i]);
        if (e == ETAT_INVALIDE) {
            printf("'%s' : état invalide\n", tests[i]);
        } else {
            printf("'%s' : état %d\n", tests[i], e);
        }
    }

    return 0;
}
```

**Sortie** :
```
'execution' : état 1
'pause' : état 2
'inconnu' : état invalide
```

---

## Comparaison avec d'autres langages

### C vs C++ (enum class)

En C++11+, il existe les **enum class** (scoped enums) qui offrent une meilleure sécurité de type :

```cpp
// C++ : enum class (pas disponible en C)
enum class Couleur {
    Rouge,
    Vert,
    Bleu
};

Couleur c = Couleur::Rouge;  // Doit qualifier avec Couleur::
```

En C, toutes les constantes sont dans l'espace de noms global, d'où l'importance des préfixes.

---

## Résumé

### Points clés

1. **enum** : Définit un ensemble de constantes entières nommées
2. **Lisibilité** : Rend le code beaucoup plus clair que des nombres magiques
3. **Valeurs par défaut** : Commencent à 0 et s'incrémentent de 1
4. **Valeurs personnalisées** : Possibilité de spécifier explicitement
5. **Idéal avec switch** : Pattern naturel pour gérer différents états
6. **typedef** : Simplifie la syntaxe d'utilisation

### Quand utiliser enum ?

✅ **Utiliser** pour :
- États d'une machine à états
- Codes d'erreur
- Options de configuration
- Ensemble fixe de valeurs (jours, mois, directions, etc.)
- Drapeaux et options (avec valeurs en puissances de 2)

❌ **Ne pas utiliser** pour :
- Valeurs qui changent fréquemment (utiliser des variables)
- Ensembles très grands (préférer des constantes ou des tables)

### Syntaxe essentielle

```c
// Déclaration simple
enum Nom {
    CONSTANTE1,      // 0
    CONSTANTE2,      // 1
    CONSTANTE3 = 10, // 10
    CONSTANTE4       // 11
};

// Avec typedef
typedef enum {
    VALEUR1,
    VALEUR2
} TypeNom;

// Utilisation
enum Nom variable = CONSTANTE1;  
TypeNom autre = VALEUR1;  

// Avec switch
switch (variable) {
    case CONSTANTE1:
        // ...
        break;
    case CONSTANTE2:
        // ...
        break;
}
```

---

## À retenir

```c
// ❌ Mauvais : nombres magiques
int status = 0;  // Que signifie 0 ?  
if (status == 2) { /* ... */ }  

// ✅ Bon : enum explicite
enum Status {
    STATUS_OK,
    STATUS_EN_COURS,
    STATUS_ERREUR
};

enum Status status = STATUS_OK;  
if (status == STATUS_ERREUR) { /* ... */ }  

// Convention de nommage recommandée
enum HttpStatus {
    HTTP_OK = 200,
    HTTP_NOT_FOUND = 404,
    HTTP_SERVER_ERROR = 500
};

// Avec typedef pour simplifier
typedef enum {
    ROUGE,
    VERT,
    BLEU
} Couleur;

Couleur ma_couleur = ROUGE;
```

**Règle d'or** : Utilisez les énumérations plutôt que des constantes `#define` ou des "nombres magiques" pour rendre votre code plus lisible et maintenable.

**Prochaine étape** : Nous allons découvrir **typedef** en détail (section 10.6), qui permet de simplifier la syntaxe et créer des alias de types.

⏭️ [typedef : Simplification de la syntaxe](/10-types-utilisateur/06-typedef.md)
