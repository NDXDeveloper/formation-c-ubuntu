🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 5.4 Conditions et optimisation

## Introduction

Les structures conditionnelles sont au cœur de la programmation. Elles permettent à votre programme de prendre des décisions et d'exécuter différents blocs de code en fonction de conditions spécifiques. En C, comme dans tout langage de programmation, savoir écrire des conditions efficaces et lisibles est une compétence essentielle.

Dans cette section, nous allons explorer :
- Les différentes structures conditionnelles en C
- Comment optimiser vos conditions pour de meilleures performances
- Les bonnes pratiques pour écrire du code clair et maintenable
- Les pièges courants à éviter

---

## Les structures conditionnelles en C

### 1. L'instruction `if`

La structure `if` est la plus basique et la plus utilisée pour tester une condition.

**Syntaxe :**
```c
if (condition) {
    // Code exécuté si la condition est vraie
}
```

**Exemple simple :**
```c
#include <stdio.h>

int main() {
    int age = 20;

    if (age >= 18) {
        printf("Vous êtes majeur.\n");
    }

    return 0;
}
```

**💡 Note importante :** En C, une condition est considérée comme **vraie** si elle est différente de zéro, et **fausse** si elle est égale à zéro.

```c
#include <stdio.h>

int main() {
    int valeur = 5;

    // Ces trois conditions sont équivalentes
    if (valeur != 0) {
        printf("valeur est non nulle\n");
    }

    if (valeur) {  // Forme idiomatique en C
        printf("valeur est non nulle\n");
    }

    // Pour tester si une valeur est nulle
    if (!valeur) {
        printf("valeur est nulle\n");
    } else {
        printf("valeur est non nulle\n");
    }

    return 0;
}
```

---

### 2. L'instruction `if-else`

La structure `if-else` permet d'exécuter un bloc de code alternatif si la condition est fausse.

**Syntaxe :**
```c
if (condition) {
    // Code exécuté si la condition est vraie
} else {
    // Code exécuté si la condition est fausse
}
```

**Exemple :**
```c
#include <stdio.h>

int main() {
    int temperature = 22;

    if (temperature > 25) {
        printf("Il fait chaud.\n");
    } else {
        printf("La température est agréable.\n");
    }

    return 0;
}
```

---

### 3. L'instruction `if-else if-else`

Pour tester plusieurs conditions en séquence, on utilise `else if`.

**Syntaxe :**
```c
if (condition1) {
    // Code si condition1 est vraie
} else if (condition2) {
    // Code si condition1 est fausse et condition2 est vraie
} else if (condition3) {
    // Code si condition1 et condition2 sont fausses et condition3 est vraie
} else {
    // Code si toutes les conditions sont fausses
}
```

**Exemple complet :**
```c
#include <stdio.h>

int main() {
    int note = 15;

    if (note >= 16) {
        printf("Mention Très Bien\n");
    } else if (note >= 14) {
        printf("Mention Bien\n");
    } else if (note >= 12) {
        printf("Mention Assez Bien\n");
    } else if (note >= 10) {
        printf("Passable\n");
    } else {
        printf("Insuffisant\n");
    }

    return 0;
}
```

**⚠️ Important :** Dès qu'une condition est vraie, le bloc correspondant est exécuté et les autres conditions ne sont **pas testées**. C'est ce qu'on appelle l'évaluation en court-circuit.

---

### 4. L'instruction `switch`

L'instruction `switch` est utilisée pour tester une variable contre plusieurs valeurs constantes. Elle est souvent plus lisible que de multiples `if-else if` pour ce cas d'usage spécifique.

**Syntaxe :**
```c
switch (expression) {
    case valeur1:
        // Code pour valeur1
        break;
    case valeur2:
        // Code pour valeur2
        break;
    case valeur3:
        // Code pour valeur3
        break;
    default:
        // Code si aucune valeur ne correspond
        break;
}
```

**Exemple :**
```c
#include <stdio.h>

int main() {
    char choix = 'B';

    switch (choix) {
        case 'A':
            printf("Vous avez choisi A\n");
            break;
        case 'B':
            printf("Vous avez choisi B\n");
            break;
        case 'C':
            printf("Vous avez choisi C\n");
            break;
        default:
            printf("Choix non reconnu\n");
            break;
    }

    return 0;
}
```

**⚠️ Le mot-clé `break` est crucial !**

Sans `break`, l'exécution continue dans les `case` suivants (comportement appelé "fall-through") :

```c
#include <stdio.h>

int main() {
    int jour = 3;

    // Sans break - Fall-through intentionnel
    switch (jour) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            printf("C'est un jour de semaine\n");
            break;
        case 6:
        case 7:
            printf("C'est le week-end\n");
            break;
        default:
            printf("Jour invalide\n");
            break;
    }

    return 0;
}
```

---

### 5. L'opérateur ternaire : `? :`

L'opérateur ternaire est une forme compacte de `if-else` qui retourne une valeur.

**Syntaxe :**
```c
condition ? valeur_si_vrai : valeur_si_faux
```

**Exemple :**
```c
#include <stdio.h>

int main() {
    int age = 20;

    // Forme traditionnelle
    char *statut1;
    if (age >= 18) {
        statut1 = "majeur";
    } else {
        statut1 = "mineur";
    }

    // Forme avec opérateur ternaire (plus concise)
    char *statut2 = (age >= 18) ? "majeur" : "mineur";

    printf("Vous êtes %s\n", statut2);

    // Autre exemple : trouver le maximum
    int a = 10, b = 20;
    int max = (a > b) ? a : b;

    printf("Le maximum est : %d\n", max);

    return 0;
}
```

**💡 Conseil :** L'opérateur ternaire est idéal pour les affectations simples, mais évitez de l'utiliser pour des expressions complexes car cela nuit à la lisibilité.

```c
// ✅ BON : Simple et lisible
int abs_value = (x >= 0) ? x : -x;

// ❌ MAUVAIS : Trop complexe
int result = (a > b) ? ((c > d) ? e : f) : ((g > h) ? i : j);  // Difficile à lire !
```

---

## Optimisation des conditions

### 1. Ordre des conditions : Court-circuit

Le C évalue les expressions logiques avec l'**évaluation en court-circuit** :
- Pour `&&` : Si la première condition est fausse, la seconde n'est pas évaluée
- Pour `||` : Si la première condition est vraie, la seconde n'est pas évaluée

**Principe d'optimisation :** Placez les conditions les plus susceptibles d'échouer (ou réussir) en premier.

```c
#include <stdio.h>

// Fonction simulant une opération coûteuse
int operation_couteuse() {
    printf("Opération coûteuse exécutée\n");
    return 1;
}

int main() {
    int valeur_simple = 0;

    // ✅ OPTIMISÉ : La condition simple est testée en premier
    if (valeur_simple != 0 && operation_couteuse()) {
        printf("Les deux conditions sont vraies\n");
    }
    // Résultat : "Opération coûteuse exécutée" ne s'affiche PAS
    // car valeur_simple est 0, donc la seconde condition n'est pas évaluée

    printf("---\n");

    valeur_simple = 1;

    // ❌ NON OPTIMISÉ : L'opération coûteuse est testée en premier
    if (operation_couteuse() && valeur_simple != 0) {
        printf("Les deux conditions sont vraies\n");
    }
    // Résultat : "Opération coûteuse exécutée" s'affiche
    // même si on aurait pu l'éviter en testant valeur_simple d'abord

    return 0;
}
```

**Règle générale :**
- Pour `&&` : Placez la condition la plus susceptible d'être **fausse** en premier
- Pour `||` : Placez la condition la plus susceptible d'être **vraie** en premier

```c
#include <stdio.h>
#include <string.h>

int main() {
    char *texte = NULL;

    // ✅ BON : Vérifier NULL avant d'utiliser strlen
    if (texte != NULL && strlen(texte) > 0) {
        printf("Le texte n'est pas vide\n");
    }
    // Si texte est NULL, strlen n'est jamais appelé (évite un crash)

    // ❌ DANGEREUX : strlen appelé même si texte est NULL
    // if (strlen(texte) > 0 && texte != NULL) {
    //     // Crash probable !
    // }

    return 0;
}
```

---

### 2. Simplification des conditions booléennes

Évitez les comparaisons redondantes avec `true` (1) ou `false` (0).

```c
#include <stdio.h>
#include <stdbool.h>

int main() {
    bool est_actif = true;
    int compteur = 5;

    // ❌ REDONDANT
    if (est_actif == true) {
        printf("Actif\n");
    }

    // ✅ MEILLEUR : Plus idiomatique en C
    if (est_actif) {
        printf("Actif\n");
    }

    // ❌ REDONDANT
    if (compteur != 0) {
        printf("Compteur non nul\n");
    }

    // ✅ MEILLEUR
    if (compteur) {
        printf("Compteur non nul\n");
    }

    // Pour tester la négation
    // ❌ VERBEUX
    if (est_actif == false) {
        printf("Inactif\n");
    }

    // ✅ MEILLEUR
    if (!est_actif) {
        printf("Inactif\n");
    }

    return 0;
}
```

---

### 3. Utilisation de tables de recherche (lookup tables)

Pour certains cas avec beaucoup de conditions, une table de recherche peut être plus efficace qu'un `switch` ou des `if-else`.

**Exemple avec switch (approche traditionnelle) :**
```c
#include <stdio.h>

int obtenir_jours_dans_mois_switch(int mois) {
    switch (mois) {
        case 1: case 3: case 5: case 7: case 8: case 10: case 12:
            return 31;
        case 4: case 6: case 9: case 11:
            return 30;
        case 2:
            return 28;  // Sans tenir compte des années bissextiles
        default:
            return -1;  // Mois invalide
    }
}

int main() {
    printf("Février a %d jours\n", obtenir_jours_dans_mois_switch(2));
    return 0;
}
```

**Exemple avec table de recherche (plus rapide) :**
```c
#include <stdio.h>

// Table de recherche : plus rapide car accès direct par index
int obtenir_jours_dans_mois_table(int mois) {
    static const int jours_par_mois[] = {
        0,   // Index 0 non utilisé
        31,  // Janvier
        28,  // Février
        31,  // Mars
        30,  // Avril
        31,  // Mai
        30,  // Juin
        31,  // Juillet
        31,  // Août
        30,  // Septembre
        31,  // Octobre
        30,  // Novembre
        31   // Décembre
    };

    if (mois < 1 || mois > 12) {
        return -1;  // Mois invalide
    }

    return jours_par_mois[mois];
}

int main() {
    printf("Février a %d jours\n", obtenir_jours_dans_mois_table(2));
    printf("Juillet a %d jours\n", obtenir_jours_dans_mois_table(7));

    return 0;
}
```

**Avantages de la table de recherche :**
- Accès en temps constant O(1)
- Pas de branches conditionnelles (meilleur pour le pipeline CPU)
- Code plus compact
- Plus facile à maintenir pour de grandes tables

---

### 4. Éviter les branches avec des opérateurs bit-à-bit

Dans certains cas critiques en performance, on peut remplacer des conditions par des opérations bit-à-bit.

**Exemple : Trouver le maximum sans branchement**

```c
#include <stdio.h>

// Version avec if (branchement)
int max_avec_if(int a, int b) {
    if (a > b) {
        return a;
    } else {
        return b;
    }
}

// Version sans branchement (plus rapide sur certains processeurs)
int max_sans_branchement(int a, int b) {
    int diff = a - b;
    int sign = (diff >> 31) & 1;  // 1 si négatif, 0 si positif
    return a - (sign * diff);
}

int main() {
    printf("Max(10, 20) = %d\n", max_avec_if(10, 20));
    printf("Max(10, 20) = %d\n", max_sans_branchement(10, 20));

    return 0;
}
```

**⚠️ Note :** Ces optimisations sont rarement nécessaires. Les compilateurs modernes optimisent déjà très bien le code. N'utilisez cette technique que si le profiling montre un réel goulot d'étranglement.

---

### 5. Utilisation de `likely` et `unlikely` (GCC)

GCC offre des macros pour indiquer au compilateur quelles branches sont probables.

```c
#include <stdio.h>

// Macros GCC pour l'optimisation des branches
#define likely(x)   __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

int main() {
    int erreur = 0;  // Les erreurs sont rares

    // Indiquer que la condition est peu probable
    if (unlikely(erreur)) {
        printf("Gestion de l'erreur rare\n");
    } else {
        printf("Cas normal (fréquent)\n");
    }

    int valeur = 100;

    // Indiquer que la condition est probable
    if (likely(valeur > 0)) {
        printf("Cas fréquent : valeur positive\n");
    }

    return 0;
}
```

**Quand utiliser :**
- `likely()` : Pour les conditions qui sont vraies 95%+ du temps
- `unlikely()` : Pour les vérifications d'erreurs rares

**⚠️ Important :** N'abusez pas de ces macros. Ne les utilisez que si :
1. Vous avez profilé votre code
2. Vous connaissez vraiment la probabilité de la condition
3. Cette section de code est critique en performance

---

### 6. Regroupement de conditions

Regroupez les conditions similaires pour améliorer la lisibilité et potentiellement les performances.

```c
#include <stdio.h>

int main() {
    int age = 25;
    bool est_etudiant = false;
    bool a_reduction = true;

    // ❌ PEU LISIBLE : Conditions éparpillées
    if (age >= 18) {
        if (age <= 25) {
            if (est_etudiant || a_reduction) {
                printf("Tarif réduit applicable\n");
            }
        }
    }

    // ✅ MEILLEUR : Conditions regroupées logiquement
    if ((age >= 18 && age <= 25) && (est_etudiant || a_reduction)) {
        printf("Tarif réduit applicable\n");
    }

    // ✅ ENCORE MIEUX : Extraction en variables nommées
    bool est_jeune_adulte = (age >= 18 && age <= 25);
    bool a_droit_reduction = (est_etudiant || a_reduction);

    if (est_jeune_adulte && a_droit_reduction) {
        printf("Tarif réduit applicable\n");
    }

    return 0;
}
```

---

## Bonnes pratiques pour écrire des conditions

### 1. Privilégier la clarté à la concision

```c
#include <stdio.h>
#include <stdbool.h>

int main() {
    int x = 10;
    int y = 20;
    int z = 30;

    // ❌ TROP COMPACT : Difficile à comprendre
    if (x > 0 && y > 0 && z > 0 && x < y && y < z) {
        printf("Conditions respectées\n");
    }

    // ✅ MEILLEUR : Intention claire
    bool tous_positifs = (x > 0 && y > 0 && z > 0);
    bool ordre_croissant = (x < y && y < z);

    if (tous_positifs && ordre_croissant) {
        printf("Conditions respectées\n");
    }

    return 0;
}
```

### 2. Utiliser des constantes nommées

```c
#include <stdio.h>
#include <stdbool.h>

// ❌ MAUVAIS : Nombres magiques
int main() {
    int age = 17;

    if (age < 18) {
        printf("Mineur\n");
    }

    return 0;
}
```

```c
#include <stdio.h>

// ✅ BON : Constantes nommées
#define AGE_MAJORITE 18
#define AGE_RETRAITE 65

int main() {
    int age = 17;

    if (age < AGE_MAJORITE) {
        printf("Mineur\n");
    } else if (age >= AGE_RETRAITE) {
        printf("Retraité\n");
    } else {
        printf("Adulte actif\n");
    }

    return 0;
}
```

### 3. Inverser les conditions pour réduire l'imbrication

La technique du "**early return**" (retour anticipé) rend le code plus lisible.

```c
#include <stdio.h>
#include <stdbool.h>

// ❌ MAUVAIS : Imbrication profonde
int traiter_donnees_v1(int *data, int taille) {
    if (data != NULL) {
        if (taille > 0) {
            if (taille < 1000) {
                // Traitement principal
                printf("Traitement des données...\n");
                return 1;
            } else {
                printf("Erreur : taille trop grande\n");
                return 0;
            }
        } else {
            printf("Erreur : taille invalide\n");
            return 0;
        }
    } else {
        printf("Erreur : données NULL\n");
        return 0;
    }
}

// ✅ MEILLEUR : Early return (gestion des erreurs d'abord)
int traiter_donnees_v2(int *data, int taille) {
    // Vérifications d'erreur en premier
    if (data == NULL) {
        printf("Erreur : données NULL\n");
        return 0;
    }

    if (taille <= 0) {
        printf("Erreur : taille invalide\n");
        return 0;
    }

    if (taille >= 1000) {
        printf("Erreur : taille trop grande\n");
        return 0;
    }

    // Code principal au même niveau d'indentation
    printf("Traitement des données...\n");
    return 1;
}

int main() {
    int donnees[] = {1, 2, 3, 4, 5};
    traiter_donnees_v2(donnees, 5);

    return 0;
}
```

### 4. Éviter les négations multiples

Les doubles négations rendent le code difficile à lire.

```c
#include <stdio.h>
#include <stdbool.h>

int main() {
    bool n_est_pas_vide = true;

    // ❌ CONFUS : Double négation
    if (!(!n_est_pas_vide)) {
        printf("Pas vide\n");
    }

    // ✅ MEILLEUR : Renommer la variable
    bool est_vide = false;

    if (!est_vide) {
        printf("Pas vide\n");
    }

    // ✅ ENCORE MIEUX : Variable positive
    bool contient_donnees = true;

    if (contient_donnees) {
        printf("Contient des données\n");
    }

    return 0;
}
```

### 5. Comparer les constantes à gauche (Yoda conditions)

Pour éviter l'erreur d'affectation accidentelle.

```c
#include <stdio.h>

int main() {
    int x = 10;

    // ❌ ERREUR POSSIBLE : Affectation au lieu de comparaison
    // if (x = 5) {  // Compile mais fait une affectation !
    //     printf("x vaut 5\n");
    // }

    // ✅ BON : Constante à gauche (Yoda condition)
    if (5 == x) {  // Si vous écrivez '=' par erreur, le compilateur lèvera une erreur
        printf("x vaut 5\n");
    } else {
        printf("x ne vaut pas 5\n");
    }

    // Forme normale (aussi valide avec les compilateurs modernes qui avertissent)
    if (x == 5) {
        printf("x vaut 5\n");
    }

    return 0;
}
```

**Note :** Les compilateurs modernes avec `-Wall` avertissent de cette erreur, rendant cette technique moins nécessaire. C'est surtout une question de style d'équipe.

---

## Erreurs courantes à éviter

### 1. Oublier les accolades

```c
#include <stdio.h>

int main() {
    int x = 10;

    // ❌ DANGEREUX : Sans accolades, seule la première instruction est conditionnelle
    if (x > 5)
        printf("x est grand\n");
        printf("Cette ligne s'exécute toujours !\n");  // Pas dans le if !

    // ✅ CORRECT : Toujours utiliser des accolades
    if (x > 5) {
        printf("x est grand\n");
        printf("Cette ligne est bien dans le if\n");
    }

    return 0;
}
```

**💡 Règle d'or :** Utilisez **toujours** des accolades pour les blocs `if`, `else`, `while`, `for`, même s'il n'y a qu'une seule instruction.

### 2. Confondre `=` et `==`

```c
#include <stdio.h>

int main() {
    int x = 10;

    // ❌ ERREUR : Affectation au lieu de comparaison
    if (x = 5) {  // x devient 5, et la condition est vraie (5 != 0)
        printf("x vaut maintenant 5 (erreur !)\n");
    }

    // ✅ CORRECT : Comparaison
    if (x == 5) {
        printf("x vaut 5\n");
    }

    return 0;
}
```

### 3. Point-virgule après le `if`

```c
#include <stdio.h>

int main() {
    int x = 10;

    // ❌ ERREUR : Point-virgule après le if
    if (x > 5);  // Ce point-virgule termine le if (instruction vide)
    {
        // Ce bloc s'exécute toujours, indépendamment de la condition !
        printf("Cette ligne s'exécute toujours\n");
    }

    // ✅ CORRECT
    if (x > 5) {
        printf("x est grand\n");
    }

    return 0;
}
```

### 4. Conditions toujours vraies ou fausses

```c
#include <stdio.h>

int main() {
    int x = 10;

    // ❌ ERREUR LOGIQUE : Condition toujours fausse
    if (x > 20 && x < 15) {  // Impossible : x ne peut pas être > 20 ET < 15
        printf("Jamais exécuté\n");
    }

    // ❌ ERREUR LOGIQUE : Condition toujours vraie
    if (x > 5 || x <= 5) {  // Toujours vrai : x est soit > 5, soit <= 5
        printf("Toujours exécuté\n");
    }

    // ✅ CORRECT
    if (x > 5 && x < 20) {
        printf("x est entre 5 et 20\n");
    }

    return 0;
}
```

### 5. Oublier le `break` dans un `switch`

```c
#include <stdio.h>

int main() {
    int jour = 2;

    // ❌ ERREUR : Oubli du break (sauf si intentionnel)
    switch (jour) {
        case 1:
            printf("Lundi\n");
            // Oubli du break !
        case 2:
            printf("Mardi\n");
            // Oubli du break !
        case 3:
            printf("Mercredi\n");
            break;
    }
    // Affichera "Mardi" ET "Mercredi"

    printf("---\n");

    // ✅ CORRECT
    switch (jour) {
        case 1:
            printf("Lundi\n");
            break;
        case 2:
            printf("Mardi\n");
            break;
        case 3:
            printf("Mercredi\n");
            break;
    }

    return 0;
}
```

---

## Quand utiliser quelle structure ?

### Comparaison des structures conditionnelles

| Structure | Cas d'usage idéal | Avantages |
|-----------|-------------------|-----------|
| `if` simple | Une seule condition à tester | Simple et direct |
| `if-else` | Deux alternatives | Clair pour les choix binaires |
| `if-else if-else` | Conditions séquentielles, tests sur différentes variables | Flexible, lisible |
| `switch` | Tester une seule variable contre plusieurs valeurs constantes | Rapide, lisible pour beaucoup de cas |
| Opérateur ternaire | Affectation simple basée sur une condition | Compact pour les cas simples |
| Table de recherche | Beaucoup de valeurs possibles, performance critique | Très rapide (O(1)) |

### Exemples de choix

**Utiliser `if-else` pour :**
```c
if (age < 18) {
    printf("Mineur\n");
} else {
    printf("Majeur\n");
}
```

**Utiliser `if-else if-else` pour :**
```c
if (note >= 16) {
    printf("Très bien\n");
} else if (note >= 14) {
    printf("Bien\n");
} else if (note >= 12) {
    printf("Assez bien\n");
} else {
    printf("Passable\n");
}
```

**Utiliser `switch` pour :**
```c
switch (choix_menu) {
    case 1:
        afficher_profil();
        break;
    case 2:
        modifier_parametres();
        break;
    case 3:
        quitter();
        break;
    default:
        printf("Choix invalide\n");
        break;
}
```

**Utiliser l'opérateur ternaire pour :**
```c
char *statut = (connecte) ? "en ligne" : "hors ligne";
int max = (a > b) ? a : b;
```

---

## Exemples pratiques récapitulatifs

### 1. Validation d'une date

```c
#include <stdio.h>
#include <stdbool.h>

bool est_annee_bissextile(int annee) {
    return (annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0);
}

bool date_valide(int jour, int mois, int annee) {
    // Vérifications de base
    if (annee < 1 || mois < 1 || mois > 12 || jour < 1) {
        return false;
    }

    // Jours par mois
    int jours_par_mois[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Ajuster pour les années bissextiles
    if (est_annee_bissextile(annee)) {
        jours_par_mois[2] = 29;
    }

    // Vérifier le jour
    if (jour > jours_par_mois[mois]) {
        return false;
    }

    return true;
}

int main() {
    printf("29/02/2024 : %s\n", date_valide(29, 2, 2024) ? "Valide" : "Invalide");
    printf("29/02/2023 : %s\n", date_valide(29, 2, 2023) ? "Valide" : "Invalide");
    printf("31/04/2024 : %s\n", date_valide(31, 4, 2024) ? "Valide" : "Invalide");

    return 0;
}
```

### 2. Calculatrice simple avec optimisation

```c
#include <stdio.h>

double calculer(double a, double b, char operateur) {
    // Early return pour les cas d'erreur
    if (operateur == '/' && b == 0) {
        printf("Erreur : division par zéro\n");
        return 0;
    }

    // Utilisation d'un switch pour la clarté
    switch (operateur) {
        case '+':
            return a + b;
        case '-':
            return a - b;
        case '*':
            return a * b;
        case '/':
            return a / b;
        default:
            printf("Opérateur non reconnu\n");
            return 0;
    }
}

int main() {
    printf("10 + 5 = %.2f\n", calculer(10, 5, '+'));
    printf("10 - 5 = %.2f\n", calculer(10, 5, '-'));
    printf("10 * 5 = %.2f\n", calculer(10, 5, '*'));
    printf("10 / 5 = %.2f\n", calculer(10, 5, '/'));
    printf("10 / 0 = %.2f\n", calculer(10, 0, '/'));

    return 0;
}
```

### 3. Système de permissions avec flags

```c
#include <stdio.h>

// Définition des permissions avec des flags
#define PERM_LECTURE   (1 << 0)  // 0001
#define PERM_ECRITURE  (1 << 1)  // 0010
#define PERM_EXECUTION (1 << 2)  // 0100
#define PERM_ADMIN     (1 << 3)  // 1000

void afficher_permissions(unsigned int permissions) {
    printf("Permissions : ");

    // Vérification optimisée avec des opérateurs bit-à-bit
    if (permissions & PERM_LECTURE) {
        printf("Lecture ");
    }

    if (permissions & PERM_ECRITURE) {
        printf("Écriture ");
    }

    if (permissions & PERM_EXECUTION) {
        printf("Exécution ");
    }

    if (permissions & PERM_ADMIN) {
        printf("Admin");
    }

    printf("\n");
}

int main() {
    unsigned int permissions_utilisateur = PERM_LECTURE | PERM_ECRITURE;
    unsigned int permissions_admin = PERM_LECTURE | PERM_ECRITURE | PERM_EXECUTION | PERM_ADMIN;

    printf("Utilisateur normal :\n");
    afficher_permissions(permissions_utilisateur);

    printf("\nAdministrateur :\n");
    afficher_permissions(permissions_admin);

    // Vérifier une permission spécifique
    if (permissions_utilisateur & PERM_ADMIN) {
        printf("L'utilisateur a les droits admin\n");
    } else {
        printf("L'utilisateur n'a PAS les droits admin\n");
    }

    return 0;
}
```

### 4. Gestion d'un menu avec validation robuste

```c
#include <stdio.h>
#include <stdbool.h>

void afficher_menu() {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1. Nouvelle partie\n");
    printf("2. Charger une partie\n");
    printf("3. Options\n");
    printf("4. Quitter\n");
    printf("Votre choix : ");
}

bool traiter_choix(int choix) {
    // Vérification de validité en premier (early return)
    if (choix < 1 || choix > 4) {
        printf("Choix invalide. Veuillez choisir entre 1 et 4.\n");
        return true;  // Continue la boucle
    }

    // Traitement selon le choix
    switch (choix) {
        case 1:
            printf("Démarrage d'une nouvelle partie...\n");
            break;
        case 2:
            printf("Chargement d'une partie...\n");
            break;
        case 3:
            printf("Ouverture des options...\n");
            break;
        case 4:
            printf("Au revoir !\n");
            return false;  // Sortir de la boucle
    }

    return true;  // Continue la boucle
}

int main() {
    int choix;
    bool continuer = true;

    while (continuer) {
        afficher_menu();

        // Vérification de la validité de l'entrée
        if (scanf("%d", &choix) != 1) {
            printf("Erreur de saisie.\n");
            // Nettoyer le buffer
            while (getchar() != '\n');
            continue;
        }

        continuer = traiter_choix(choix);
    }

    return 0;
}
```

---

## Résumé

**Structures conditionnelles disponibles :**
- `if` : Test simple
- `if-else` : Deux alternatives
- `if-else if-else` : Conditions multiples séquentielles
- `switch` : Test d'une variable contre plusieurs valeurs constantes
- Opérateur ternaire `? :` : Forme compacte pour affectations simples

**Principes d'optimisation :**
1. **Court-circuit** : Placez les conditions les plus probables en premier
2. **Tables de recherche** : Pour beaucoup de valeurs, plus rapide que switch
3. **Early return** : Sortez tôt des fonctions pour réduire l'imbrication
4. **Simplification** : Évitez les comparaisons redondantes avec true/false
5. **Nommage** : Utilisez des variables booléennes bien nommées

**Bonnes pratiques :**
- ✅ Toujours utiliser des accolades
- ✅ Privilégier la clarté à la concision
- ✅ Utiliser des constantes nommées
- ✅ Éviter les négations multiples
- ✅ Commenter les conditions complexes

**Erreurs à éviter :**
- ❌ Confondre `=` et `==`
- ❌ Oublier les accolades
- ❌ Point-virgule après le `if`
- ❌ Oublier le `break` dans un `switch`
- ❌ Conditions toujours vraies ou fausses

---

## Pour aller plus loin

Dans les sections suivantes, vous découvrirez :
- **5.5 Boucles et contrôle de flux** : Comment utiliser les conditions dans les boucles
- **5.6 Bonnes pratiques de formatage** : Conventions de style pour le code
- **Module 7** : Techniques avancées incluant les optimisations de niveau expert

Les structures conditionnelles sont le fondement de la logique de programmation. Maîtriser leur utilisation optimale vous permettra d'écrire du code plus rapide, plus lisible et plus maintenable !

⏭️ [Boucles et contrôle de flux](/05-operateurs-et-controle/05-boucles.md)
