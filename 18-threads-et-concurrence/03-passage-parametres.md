🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 18.3 Passage de paramètres

## Table des matières
- [Introduction](#introduction)
- [Rappel : Le paramètre void*](#rappel--le-param%C3%A8tre-void)
- [Passage d'un entier](#passage-dun-entier)
- [Passage d'une chaîne de caractères](#passage-dune-cha%C3%AEne-de-caract%C3%A8res)
- [Passage de plusieurs paramètres avec une structure](#passage-de-plusieurs-param%C3%A8tres-avec-une-structure)
- [Passage de tableaux](#passage-de-tableaux)
- [Allocation dynamique des paramètres](#allocation-dynamique-des-param%C3%A8tres)
- [Passage de paramètres avec threads multiples](#passage-de-param%C3%A8tres-avec-threads-multiples)
- [Retour de valeurs multiples](#retour-de-valeurs-multiples)
- [Communication bidirectionnelle](#communication-bidirectionnelle)
- [Pièges et erreurs courantes](#pi%C3%A8ges-et-erreurs-courantes)
- [Bonnes pratiques](#bonnes-pratiques)

---

## Introduction

Le passage de paramètres aux threads est un aspect **crucial** de la programmation multi-threadée. Contrairement aux fonctions normales, les fonctions de threads ont une signature imposée :

```c
void *fonction_thread(void *arg)
```

Cette signature impose d'utiliser un pointeur générique `void*` pour passer des données. Cela offre une grande flexibilité, mais demande de la rigueur pour éviter les erreurs.

**Dans cette section**, nous allons voir toutes les techniques pour :
- Passer un seul paramètre (entier, chaîne, pointeur)
- Passer plusieurs paramètres (structures)
- Gérer la mémoire correctement
- Éviter les pièges classiques

---

## Rappel : Le paramètre void*

### Qu'est-ce qu'un void* ?

Un pointeur `void*` est un **pointeur générique** qui peut pointer vers n'importe quel type de données :

```c
int x = 42;  
char c = 'A';  
double d = 3.14;  

void *ptr1 = &x;  // Pointe vers un int  
void *ptr2 = &c;  // Pointe vers un char  
void *ptr3 = &d;  // Pointe vers un double  
```

**Avantage** : Grande flexibilité.  
**Inconvénient** : Perte d'information de type → nécessité de **caster**.  

### Utilisation dans pthread_create()

```c
int pthread_create(pthread_t *thread,
                   const pthread_attr_t *attr,
                   void *(*start_routine)(void *),
                   void *arg);              // ← Paramètre générique
```

Le 4ᵉ paramètre `arg` est de type `void*`. Vous pouvez y passer :
- L'adresse d'une variable : `&ma_variable`
- Un pointeur déjà existant : `mon_pointeur`
- Une valeur castée directement : `(void *)(long)42`

**Dans la fonction du thread**, vous devrez **caster** ce `void*` vers le type original :

```c
void *thread_func(void *arg) {
    int valeur = *(int *)arg;  // Cast vers int*, puis déréférence
    printf("Valeur : %d\n", valeur);
    return NULL;
}
```

---

## Passage d'un entier

### Méthode 1 : Passer l'adresse d'une variable (recommandé)

```c
#include <stdio.h>
#include <pthread.h>

void *afficher_nombre(void *arg) {
    int nombre = *(int *)arg;  // Cast void* → int*, puis déréférence
    printf("Thread reçu : %d\n", nombre);
    return NULL;
}

int main(void) {
    pthread_t tid;
    int valeur = 42;

    // Passer l'adresse de la variable
    pthread_create(&tid, NULL, afficher_nombre, &valeur);

    pthread_join(tid, NULL);
    return 0;
}
```

**Explication du cast** :
```c
int nombre = *(int *)arg;
            //  └────┘ Cast : void* → int*
            // └────────┘ Déréférence : *ptr pour obtenir la valeur
```

**⚠️ Attention** : La variable `valeur` doit exister tant que le thread l'utilise !

```c
// ❌ DANGER
void creer_thread(void) {
    int valeur = 42;  // Variable locale
    pthread_t tid;
    pthread_create(&tid, NULL, afficher_nombre, &valeur);
    // valeur est détruite ici → pointeur invalide !
}

// ✅ CORRECT
int valeur = 42;  // Variable globale ou statique

void creer_thread(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, afficher_nombre, &valeur);
    pthread_join(tid, NULL);  // Attendre avant que valeur ne soit détruite
}
```

### Méthode 2 : Cast direct de la valeur (pour petites valeurs)

Pour éviter de passer un pointeur, vous pouvez caster directement la valeur en `void*` :

```c
#include <stdio.h>
#include <pthread.h>

void *afficher_nombre(void *arg) {
    // Double cast pour éviter les warnings
    int nombre = (int)(long)arg;
    printf("Thread reçu : %d\n", nombre);
    return NULL;
}

int main(void) {
    pthread_t tid;
    int valeur = 42;

    // Cast direct de la valeur
    pthread_create(&tid, NULL, afficher_nombre, (void *)(long)valeur);

    pthread_join(tid, NULL);
    return 0;
}
```

**Pourquoi le double cast `(void *)(long)` ?**

```c
// ❌ Cast direct : warning du compilateur
pthread_create(&tid, NULL, func, (void *)42);
// warning: cast to pointer from integer of different size

// ✅ Double cast : pas de warning
pthread_create(&tid, NULL, func, (void *)(long)42);
```

Le double cast garantit que la taille de l'entier correspond à la taille d'un pointeur.

**⚠️ Limitations** :
- Fonctionne uniquement pour des petites valeurs (typiquement ≤ taille d'un pointeur)
- Moins lisible que passer un pointeur
- Non portable sur certaines architectures exotiques

**Quand l'utiliser ?**
- Indices de boucle (0, 1, 2, ...)
- Flags simples (0 ou 1)
- Petits identifiants

### Méthode 3 : Allocation dynamique (recommandé pour boucles)

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *afficher_nombre(void *arg) {
    int nombre = *(int *)arg;
    printf("Thread reçu : %d\n", nombre);
    free(arg);  // Libération dans le thread
    return NULL;
}

int main(void) {
    pthread_t tid;

    int *valeur = malloc(sizeof(int));
    *valeur = 42;

    pthread_create(&tid, NULL, afficher_nombre, valeur);
    pthread_join(tid, NULL);

    return 0;
}
```

**Avantages** :
- La mémoire persiste après la sortie de la fonction
- Idéal pour les boucles créant plusieurs threads

**Inconvénient** :
- Nécessite un `free()` (dans le thread ou après `pthread_join()`)

---

## Passage d'une chaîne de caractères

### Chaîne littérale (constante)

Les chaînes littérales sont stockées dans la zone **data** (read-only) et persistent pendant toute l'exécution :

```c
#include <stdio.h>
#include <pthread.h>

void *afficher_message(void *arg) {
    char *message = (char *)arg;  // Cast void* → char*
    printf("Thread : %s\n", message);
    return NULL;
}

int main(void) {
    pthread_t tid;

    // Chaîne littérale : existe pendant toute l'exécution
    pthread_create(&tid, NULL, afficher_message, "Hello, Thread!");

    pthread_join(tid, NULL);
    return 0;
}
```

**Sortie** :
```
Thread : Hello, Thread!
```

### Chaîne dans un tableau (attention aux portées)

```c
// ❌ DANGER
void creer_thread(void) {
    char message[] = "Hello";  // Tableau local
    pthread_t tid;
    pthread_create(&tid, NULL, afficher_message, message);
    // message est détruit ici !
}

// ✅ CORRECT : tableau statique
void creer_thread(void) {
    static char message[] = "Hello";  // Persiste après la fonction
    pthread_t tid;
    pthread_create(&tid, NULL, afficher_message, message);
    pthread_join(tid, NULL);  // Attendre le thread
}

// ✅ CORRECT : tableau global
char message[] = "Hello";

void creer_thread(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, afficher_message, message);
    pthread_join(tid, NULL);
}
```

### Chaîne allouée dynamiquement

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

void *afficher_message(void *arg) {
    char *message = (char *)arg;
    printf("Thread : %s\n", message);
    free(message);  // Libération dans le thread
    return NULL;
}

int main(void) {
    pthread_t tid;

    // Allocation dynamique
    char *message = malloc(50 * sizeof(char));
    if (message == NULL) {
        fprintf(stderr, "Erreur malloc\n");
        return 1;
    }

    strcpy(message, "Hello depuis malloc");

    pthread_create(&tid, NULL, afficher_message, message);
    pthread_join(tid, NULL);

    return 0;
}
```

**Alternative** : Libérer dans `main()` après `pthread_join()` :

```c
void *afficher_message(void *arg) {
    char *message = (char *)arg;
    printf("Thread : %s\n", message);
    return NULL;  // Ne libère pas
}

int main(void) {
    pthread_t tid;
    char *message = malloc(50);
    strcpy(message, "Hello");

    pthread_create(&tid, NULL, afficher_message, message);
    pthread_join(tid, NULL);

    free(message);  // Libération après join
    return 0;
}
```

---

## Passage de plusieurs paramètres avec une structure

Quand vous avez **plusieurs paramètres**, utilisez une **structure** :

### Exemple basique

```c
#include <stdio.h>
#include <pthread.h>

// Structure contenant plusieurs paramètres
typedef struct {
    int id;
    char nom[50];
    double coefficient;
} ParametresThread;

void *afficher_infos(void *arg) {
    ParametresThread *params = (ParametresThread *)arg;

    printf("Thread ID : %d\n", params->id);
    printf("Nom : %s\n", params->nom);
    printf("Coefficient : %.2f\n", params->coefficient);

    return NULL;
}

int main(void) {
    pthread_t tid;

    // Créer et initialiser la structure
    ParametresThread params = {
        .id = 1,
        .nom = "Worker",
        .coefficient = 1.5
    };

    pthread_create(&tid, NULL, afficher_infos, &params);
    pthread_join(tid, NULL);

    return 0;
}
```

**Sortie** :
```
Thread ID : 1  
Nom : Worker  
Coefficient : 1.50  
```

### Exemple avec calcul

```c
#include <stdio.h>
#include <pthread.h>

typedef struct {
    int debut;
    int fin;
    long long resultat;  // Stocke le résultat du calcul
} PlageCalcul;

void *calculer_somme(void *arg) {
    PlageCalcul *plage = (PlageCalcul *)arg;
    long long somme = 0;

    for (int i = plage->debut; i <= plage->fin; i++) {
        somme += i;
    }

    plage->resultat = somme;  // Stocker le résultat dans la structure
    return NULL;
}

int main(void) {
    pthread_t tid;
    PlageCalcul plage = {
        .debut = 1,
        .fin = 1000,
        .resultat = 0
    };

    printf("Calcul de la somme de %d à %d...\n", plage.debut, plage.fin);

    pthread_create(&tid, NULL, calculer_somme, &plage);
    pthread_join(tid, NULL);

    printf("Résultat : %lld\n", plage.resultat);

    return 0;
}
```

**Sortie** :
```
Calcul de la somme de 1 à 1000...  
Résultat : 500500  
```

### Structure avec allocation dynamique

Utile quand la structure ne doit pas persister dans la fonction appelante :

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

typedef struct {
    int id;
    char *message;
} ThreadData;

void *traiter_donnees(void *arg) {
    ThreadData *data = (ThreadData *)arg;

    printf("[Thread %d] : %s\n", data->id, data->message);

    // Libération de la mémoire
    free(data->message);
    free(data);

    return NULL;
}

int main(void) {
    pthread_t tid;

    // Allocation de la structure
    ThreadData *data = malloc(sizeof(ThreadData));
    if (data == NULL) {
        fprintf(stderr, "Erreur malloc\n");
        return 1;
    }

    data->id = 1;
    data->message = malloc(100);
    strcpy(data->message, "Données importantes");

    pthread_create(&tid, NULL, traiter_donnees, data);
    pthread_join(tid, NULL);

    return 0;
}
```

---

## Passage de tableaux

### Tableau d'entiers

Un tableau se convertit implicitement en pointeur vers son premier élément, donc vous pouvez le passer directement :

```c
#include <stdio.h>
#include <pthread.h>

#define TAILLE 5

void *afficher_tableau(void *arg) {
    int *tableau = (int *)arg;

    printf("Contenu du tableau :\n");
    for (int i = 0; i < TAILLE; i++) {
        printf("  tableau[%d] = %d\n", i, tableau[i]);
    }

    return NULL;
}

int main(void) {
    pthread_t tid;
    int nombres[TAILLE] = {10, 20, 30, 40, 50};

    pthread_create(&tid, NULL, afficher_tableau, nombres);
    pthread_join(tid, NULL);

    return 0;
}
```

**Problème** : Le thread ne connaît pas la taille du tableau !

### Solution : Structure contenant le tableau et sa taille

```c
#include <stdio.h>
#include <pthread.h>

typedef struct {
    int *tableau;
    int taille;
} TableauData;

void *calculer_somme(void *arg) {
    TableauData *data = (TableauData *)arg;
    int somme = 0;

    for (int i = 0; i < data->taille; i++) {
        somme += data->tableau[i];
    }

    printf("Somme du tableau : %d\n", somme);
    return NULL;
}

int main(void) {
    pthread_t tid;
    int nombres[] = {10, 20, 30, 40, 50};

    TableauData data = {
        .tableau = nombres,
        .taille = 5
    };

    pthread_create(&tid, NULL, calculer_somme, &data);
    pthread_join(tid, NULL);

    return 0;
}
```

### Tableau alloué dynamiquement

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int *tableau;
    int taille;
} TableauData;

void *traiter_tableau(void *arg) {
    TableauData *data = (TableauData *)arg;

    // Traitement
    for (int i = 0; i < data->taille; i++) {
        printf("Element %d : %d\n", i, data->tableau[i]);
    }

    // Libération
    free(data->tableau);
    free(data);

    return NULL;
}

int main(void) {
    pthread_t tid;

    // Allocation dynamique du tableau
    int taille = 5;
    int *tab = malloc(taille * sizeof(int));
    for (int i = 0; i < taille; i++) {
        tab[i] = i * 10;
    }

    // Allocation dynamique de la structure
    TableauData *data = malloc(sizeof(TableauData));
    data->tableau = tab;
    data->taille = taille;

    pthread_create(&tid, NULL, traiter_tableau, data);
    pthread_join(tid, NULL);

    return 0;
}
```

---

## Allocation dynamique des paramètres

### Pourquoi l'allocation dynamique ?

L'allocation dynamique est **essentielle** dans les cas suivants :

1. **Boucle créant plusieurs threads** : Les variables locales de boucle sont réutilisées
2. **Fonction retournant avant la fin du thread** : Les variables locales sont détruites
3. **Threads détachés** : Pas de `pthread_join()` pour synchroniser

### Exemple : Boucle avec allocation dynamique

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 5

void *afficher_id(void *arg) {
    int id = *(int *)arg;
    printf("Thread %d en cours d'exécution\n", id);
    free(arg);  // Libération dans le thread
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        // Allocation d'un entier pour chaque thread
        int *id = malloc(sizeof(int));
        if (id == NULL) {
            fprintf(stderr, "Erreur malloc\n");
            return 1;
        }

        *id = i;
        pthread_create(&threads[i], NULL, afficher_id, id);
    }

    // Attendre tous les threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    return 0;
}
```

### Comparaison : Sans vs Avec allocation dynamique

**❌ Sans allocation (BUG)** :

```c
// BUG : Tous les threads utilisent la MÊME adresse &i
for (int i = 0; i < 5; i++) {
    pthread_create(&threads[i], NULL, worker, &i);
}
// Résultat : i peut valoir 5 pour tous les threads !
```

**✅ Avec allocation** :

```c
// Chaque thread reçoit sa propre copie
for (int i = 0; i < 5; i++) {
    int *id = malloc(sizeof(int));
    *id = i;
    pthread_create(&threads[i], NULL, worker, id);
}
```

### Qui libère la mémoire ?

**Option 1 : Le thread libère** (recommandé pour threads détachés)

```c
void *worker(void *arg) {
    int id = *(int *)arg;
    free(arg);  // Le thread se charge de libérer

    // Travail...
    return NULL;
}
```

**Option 2 : Main libère après join** (pour threads joinables)

```c
void *worker(void *arg) {
    int id = *(int *)arg;
    // Ne libère PAS
    return NULL;
}

int main(void) {
    int *id = malloc(sizeof(int));
    *id = 1;

    pthread_t tid;
    pthread_create(&tid, NULL, worker, id);
    pthread_join(tid, NULL);

    free(id);  // Main libère après join
    return 0;
}
```

---

## Passage de paramètres avec threads multiples

### Exemple complet : Calcul parallèle

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4
#define ARRAY_SIZE 100

typedef struct {
    int thread_id;
    int *tableau;
    int debut;
    int fin;
    int somme;  // Résultat du calcul
} ThreadData;

void *calculer_somme_partielle(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    int somme = 0;

    printf("Thread %d : calcul de %d à %d\n",
           data->thread_id, data->debut, data->fin);

    for (int i = data->debut; i < data->fin; i++) {
        somme += data->tableau[i];
    }

    data->somme = somme;
    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data[NUM_THREADS];
    int tableau[ARRAY_SIZE];

    // Initialisation du tableau
    for (int i = 0; i < ARRAY_SIZE; i++) {
        tableau[i] = i + 1;  // 1, 2, 3, ..., 100
    }

    int elements_par_thread = ARRAY_SIZE / NUM_THREADS;

    // Création des threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_data[i].thread_id = i;
        thread_data[i].tableau = tableau;
        thread_data[i].debut = i * elements_par_thread;
        thread_data[i].fin = (i + 1) * elements_par_thread;
        thread_data[i].somme = 0;

        pthread_create(&threads[i], NULL,
                      calculer_somme_partielle, &thread_data[i]);
    }

    // Attendre tous les threads et collecter les résultats
    int somme_totale = 0;
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
        printf("Thread %d : somme partielle = %d\n",
               i, thread_data[i].somme);
        somme_totale += thread_data[i].somme;
    }

    printf("\nSomme totale : %d\n", somme_totale);
    printf("Vérification : %d (formule n(n+1)/2)\n",
           ARRAY_SIZE * (ARRAY_SIZE + 1) / 2);

    return 0;
}
```

**Sortie** :
```
Thread 0 : calcul de 0 à 25  
Thread 1 : calcul de 25 à 50  
Thread 2 : calcul de 50 à 75  
Thread 3 : calcul de 75 à 100  
Thread 0 : somme partielle = 325  
Thread 1 : somme partielle = 950  
Thread 2 : somme partielle = 1575  
Thread 3 : somme partielle = 2200  

Somme totale : 5050  
Vérification : 5050 (formule n(n+1)/2)  
```

---

## Retour de valeurs multiples

### Via une structure passée en paramètre

La structure peut contenir à la fois les **entrées** et les **sorties** :

```c
#include <stdio.h>
#include <pthread.h>
#include <math.h>

typedef struct {
    // Entrées
    double a;
    double b;
    double c;

    // Sorties
    double delta;
    double x1;
    double x2;
    int solutions;  // Nombre de solutions
} EquationData;

void *resoudre_equation(void *arg) {
    EquationData *eq = (EquationData *)arg;

    // Calcul du discriminant
    eq->delta = eq->b * eq->b - 4 * eq->a * eq->c;

    if (eq->delta < 0) {
        eq->solutions = 0;
    } else if (eq->delta == 0) {
        eq->solutions = 1;
        eq->x1 = -eq->b / (2 * eq->a);
    } else {
        eq->solutions = 2;
        eq->x1 = (-eq->b - sqrt(eq->delta)) / (2 * eq->a);
        eq->x2 = (-eq->b + sqrt(eq->delta)) / (2 * eq->a);
    }

    return NULL;
}

int main(void) {
    pthread_t tid;
    EquationData equation = {
        .a = 1,
        .b = -5,
        .c = 6,
        .delta = 0,
        .x1 = 0,
        .x2 = 0,
        .solutions = 0
    };

    printf("Résolution de %.0fx² + %.0fx + %.0f = 0\n",
           equation.a, equation.b, equation.c);

    pthread_create(&tid, NULL, resoudre_equation, &equation);
    pthread_join(tid, NULL);

    printf("Delta = %.2f\n", equation.delta);
    if (equation.solutions == 0) {
        printf("Pas de solution réelle\n");
    } else if (equation.solutions == 1) {
        printf("Une solution : x = %.2f\n", equation.x1);
    } else {
        printf("Deux solutions : x1 = %.2f, x2 = %.2f\n",
               equation.x1, equation.x2);
    }

    return 0;
}
```

**Sortie** :
```
Résolution de 1x² + -5x + 6 = 0  
Delta = 1.00  
Deux solutions : x1 = 2.00, x2 = 3.00  
```

### Via pthread_join() et malloc

```c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int min;
    int max;
    double moyenne;
} Statistiques;

void *calculer_stats(void *arg) {
    int *tableau = (int *)arg;
    int taille = 5;  // Taille connue

    Statistiques *stats = malloc(sizeof(Statistiques));
    if (stats == NULL) {
        return NULL;
    }

    stats->min = tableau[0];
    stats->max = tableau[0];
    int somme = 0;

    for (int i = 0; i < taille; i++) {
        if (tableau[i] < stats->min) stats->min = tableau[i];
        if (tableau[i] > stats->max) stats->max = tableau[i];
        somme += tableau[i];
    }

    stats->moyenne = (double)somme / taille;

    return stats;  // Retourne la structure allouée
}

int main(void) {
    pthread_t tid;
    int tableau[] = {10, 25, 5, 30, 15};
    void *retval;

    pthread_create(&tid, NULL, calculer_stats, tableau);
    pthread_join(tid, &retval);

    if (retval != NULL) {
        Statistiques *stats = (Statistiques *)retval;
        printf("Min : %d\n", stats->min);
        printf("Max : %d\n", stats->max);
        printf("Moyenne : %.2f\n", stats->moyenne);
        free(stats);
    }

    return 0;
}
```

---

## Communication bidirectionnelle

### Exemple : Thread qui modifie les données d'entrée

```c
#include <stdio.h>
#include <pthread.h>
#include <string.h>

typedef struct {
    char message[100];
    int longueur;
    int modifie;  // Flag indiquant si modifié
} MessageData;

void *transformer_message(void *arg) {
    MessageData *data = (MessageData *)arg;

    // Convertir en majuscules
    for (int i = 0; i < data->longueur; i++) {
        if (data->message[i] >= 'a' && data->message[i] <= 'z') {
            data->message[i] = data->message[i] - 'a' + 'A';
        }
    }

    data->modifie = 1;
    return NULL;
}

int main(void) {
    pthread_t tid;
    MessageData data;

    strcpy(data.message, "hello world");
    data.longueur = strlen(data.message);
    data.modifie = 0;

    printf("Avant : %s\n", data.message);

    pthread_create(&tid, NULL, transformer_message, &data);
    pthread_join(tid, NULL);

    printf("Après : %s\n", data.message);
    printf("Modifié : %s\n", data.modifie ? "Oui" : "Non");

    return 0;
}
```

**Sortie** :
```
Avant : hello world  
Après : HELLO WORLD  
Modifié : Oui  
```

---

## Pièges et erreurs courantes

### 1. Passer l'adresse d'une variable de boucle

**❌ ERREUR CLASSIQUE** :

```c
for (int i = 0; i < 5; i++) {
    pthread_create(&threads[i], NULL, worker, &i);
    //                                         ↑ BUG !
}
// Tous les threads reçoivent la MÊME adresse
```

**Pourquoi c'est un bug ?**

```
i = 0 → pthread_create(..., &i)  ───┐  
i = 1 → pthread_create(..., &i)  ───┤ Tous pointent  
i = 2 → pthread_create(..., &i)  ───┤ vers la même  
i = 3 → pthread_create(..., &i)  ───┤ variable i  
i = 4 → pthread_create(..., &i)  ───┤  
i = 5 (fin de boucle)                ┘  

Quand les threads lisent *arg, i peut valoir 5 !
```

**✅ SOLUTIONS** :

```c
// Solution 1 : Tableau d'arguments
int ids[5];  
for (int i = 0; i < 5; i++) {  
    ids[i] = i;
    pthread_create(&threads[i], NULL, worker, &ids[i]);
}

// Solution 2 : Allocation dynamique
for (int i = 0; i < 5; i++) {
    int *id = malloc(sizeof(int));
    *id = i;
    pthread_create(&threads[i], NULL, worker, id);
}

// Solution 3 : Cast direct (petites valeurs)
for (int i = 0; i < 5; i++) {
    pthread_create(&threads[i], NULL, worker, (void *)(long)i);
}
```

### 2. Passer un pointeur vers une variable locale

**❌ ERREUR** :

```c
void lancer_thread(void) {
    int valeur = 42;
    pthread_t tid;
    pthread_create(&tid, NULL, worker, &valeur);
    // valeur est détruite à la sortie de la fonction !
}
```

**✅ SOLUTIONS** :

```c
// Solution 1 : Variable statique
void lancer_thread(void) {
    static int valeur = 42;
    pthread_t tid;
    pthread_create(&tid, NULL, worker, &valeur);
    pthread_join(tid, NULL);  // Attendre avant la sortie
}

// Solution 2 : Variable globale
int valeur = 42;

void lancer_thread(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, worker, &valeur);
    pthread_join(tid, NULL);
}

// Solution 3 : Allocation dynamique
void lancer_thread(void) {
    int *valeur = malloc(sizeof(int));
    *valeur = 42;
    pthread_t tid;
    pthread_create(&tid, NULL, worker, valeur);
    pthread_join(tid, NULL);
    free(valeur);
}
```

### 3. Oublier de libérer la mémoire allouée

**❌ FUITE MÉMOIRE** :

```c
for (int i = 0; i < 1000; i++) {
    int *id = malloc(sizeof(int));
    *id = i;
    pthread_create(&threads[i], NULL, worker, id);
    // Jamais de free() → fuite !
}
```

**✅ SOLUTION** : Libérer dans le thread ou après join :

```c
void *worker(void *arg) {
    int id = *(int *)arg;
    free(arg);  // Libération
    // ...
    return NULL;
}
```

### 4. Mauvais cast du void*

**❌ ERREUR** :

```c
void *worker(void *arg) {
    int valeur = (int)arg;  // Cast direct : perte possible
    return NULL;
}
```

**⚠️ Warning** :
```
warning: cast from pointer to integer of different size
```

**✅ CORRECT** :

```c
// Si passage par pointeur
void *worker(void *arg) {
    int valeur = *(int *)arg;  // Déréférence
    return NULL;
}

// Si cast direct
void *worker(void *arg) {
    int valeur = (int)(long)arg;  // Double cast
    return NULL;
}
```

### 5. Modifier une chaîne littérale

**❌ ERREUR** :

```c
void *modifier_message(void *arg) {
    char *msg = (char *)arg;
    msg[0] = 'X';  // ← CRASH si msg est une chaîne littérale !
    return NULL;
}

int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, modifier_message, "Hello");
    // "Hello" est en mémoire read-only !
    pthread_join(tid, NULL);
    return 0;
}
```

**✅ SOLUTION** : Copier dans un buffer modifiable :

```c
void *modifier_message(void *arg) {
    char buffer[100];
    strcpy(buffer, (char *)arg);
    buffer[0] = 'X';  // OK, buffer est modifiable
    return NULL;
}
```

---

## Bonnes pratiques

### 1. Toujours utiliser des structures pour plusieurs paramètres

```c
// ❌ Compliqué
void *worker(void *arg) {
    int **params = (int **)arg;
    int a = *params[0];
    int b = *params[1];
    // ...
}

// ✅ Clair et lisible
typedef struct {
    int a;
    int b;
} Params;

void *worker(void *arg) {
    Params *p = (Params *)arg;
    int a = p->a;
    int b = p->b;
    // ...
}
```

### 2. Documenter la gestion de la mémoire

```c
/**
 * Thread qui traite des données.
 *
 * @param arg Pointeur vers ThreadData alloué dynamiquement
 *            Le thread est responsable de libérer cette mémoire.
 */
void *traiter_donnees(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    // Traitement...
    free(data);  // Libération documentée
    return NULL;
}
```

### 3. Vérifier les allocations

```c
int *id = malloc(sizeof(int));  
if (id == NULL) {  
    fprintf(stderr, "Erreur malloc\n");
    return 1;
}
*id = 42;
pthread_create(&tid, NULL, worker, id);
```

### 4. Utiliser typedef pour les structures

```c
// ❌ Sans typedef : lourd
struct ThreadData {
    int id;
    char name[50];
};

void *worker(void *arg) {
    struct ThreadData *data = (struct ThreadData *)arg;
    // ...
}

// ✅ Avec typedef : plus léger
typedef struct {
    int id;
    char name[50];
} ThreadData;

void *worker(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    // ...
}
```

### 5. Initialiser les structures

```c
// ✅ Initialisation désignée (C99+)
ThreadData data = {
    .id = 1,
    .name = "Worker",
    .active = 1
};

// ✅ Initialisation à zéro
ThreadData data = {0};  // Tous les champs à 0
```

---

## Récapitulatif

Dans cette section, vous avez appris :

✅ **Passage d'un seul paramètre** :
- Par adresse : `&variable`
- Par cast direct : `(void *)(long)valeur`
- Par allocation dynamique : `malloc()`

✅ **Passage de chaînes** :
- Littérales (constantes)
- Tableaux statiques
- Allocation dynamique

✅ **Passage de multiples paramètres** :
- Structures personnalisées
- Entrées et sorties dans la même structure

✅ **Passage de tableaux** :
- Avec pointeur + taille
- Structure englobante

✅ **Gestion mémoire** :
- Quand allouer dynamiquement
- Qui libère (thread ou main)
- Éviter les fuites

✅ **Pièges à éviter** :
- Variable de boucle
- Pointeurs invalides
- Mauvais casts
- Chaînes littérales

---

## Pour aller plus loin

Dans les prochaines sections :

- **18.4** : Threads détachés vs joinable (gestion avancée du cycle de vie)
- **18.5** : Race conditions (problèmes de concurrence avec données partagées)
- **18.6** : Mutex (protéger les données partagées)

**Conseil pratique** : Écrivez un programme qui crée 5 threads, chacun calculant la somme d'une portion d'un tableau. Utilisez des structures pour passer les paramètres et récupérer les résultats. Vérifiez avec Valgrind qu'il n'y a pas de fuite mémoire.

---

📘 **Ressources utiles** :
- `man pthread_create`
- `man malloc`
- Valgrind : `valgrind --leak-check=full ./programme`

💡 **Prochaine étape** : Section 18.4 - Threads détachés vs joinable

⏭️ [Threads détachés vs joinable](/18-threads-et-concurrence/04-detaches-vs-joinable.md)
