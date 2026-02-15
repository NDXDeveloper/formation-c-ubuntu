/* ============================================================================
   Section 26.1 : Secure C Coding Standards (CERT C)
   Description : Demonstration des regles CERT C essentielles
                 (PRE31, DCL30, EXP33, INT30, FLP30, STR31, MEM30, FIO30, ENV33)
   Fichier source : 01-cert-c-standards.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <stdint.h>

/* ---- PRE31-C : Pas d'effets de bord dans les macros ---- */
/* Mauvais : #define CARRE(x) ((x) * (x)) avec i++ */
static inline int carre(int x) {
    return x * x;
}

/* ---- DCL30-C : Portee appropriee ---- */
static int compteur_module = 0;  /* Visible uniquement dans ce fichier */

static void incrementer(void) {
    compteur_module++;
}

/* ---- EXP33-C : Toujours initialiser les variables ---- */
static void demo_initialisation(void) {
    int valeur = 0;  /* Valeur par defaut sure */

    if (compteur_module > 0) {
        valeur = 42;
    }

    printf("[EXP33-C] Valeur initialisee : %d\n", valeur);
}

/* ---- INT30-C : Verification overflow avant allocation ---- */
static int* allouer_tableau(size_t nb_elements) {
    if (nb_elements > SIZE_MAX / sizeof(int)) {
        fprintf(stderr, "[INT30-C] Erreur : overflow dans le calcul de taille\n");
        return NULL;
    }

    size_t taille = nb_elements * sizeof(int);
    int *tableau = malloc(taille);

    if (tableau == NULL) {
        fprintf(stderr, "[INT30-C] Erreur d'allocation\n");
        return NULL;
    }

    return tableau;
}

/* ---- FLP30-C : Pas de flottants comme compteurs de boucle ---- */
static void demo_boucle_flottants(void) {
    printf("[FLP30-C] Boucle avec entiers (pas de flottants) :\n");
    for (int i = 0; i <= 10; i++) {
        float x = i * 0.1f;
        printf("  i=%d -> x=%.1f\n", i, (double)x);
    }
}

/* ---- STR31-C : Buffers de taille suffisante ---- */
static void copier_nom_securise(const char *source) {
    char nom[20];
    strncpy(nom, source, sizeof(nom) - 1);
    nom[sizeof(nom) - 1] = '\0';
    printf("[STR31-C] Nom copie (securise) : %s\n", nom);
}

/* ---- MEM30-C : Pas d'acces apres free ---- */
static void demo_free_securise(void) {
    int *ptr = malloc(sizeof(int) * 10);
    if (ptr == NULL) {
        return;
    }

    *ptr = 42;
    printf("[MEM30-C] Avant free : %d\n", *ptr);

    free(ptr);
    ptr = NULL;  /* Evite les acces accidentels */

    if (ptr != NULL) {
        printf("[MEM30-C] Apres free : %d\n", *ptr);
    } else {
        printf("[MEM30-C] Pointeur mis a NULL apres free\n");
    }
}

/* ---- FIO30-C : Format fixe pour printf ---- */
static void afficher_message_securise(const char *entree_utilisateur) {
    printf("[FIO30-C] Message : %s\n", entree_utilisateur);  /* Format controle */
}

/* ---- ENV33-C : Validation avant ouverture de fichier ---- */
static void ouvrir_fichier_securise(const char *nom_fichier) {
    /* Validation stricte du nom de fichier */
    for (size_t i = 0; nom_fichier[i] != '\0'; i++) {
        if (!isalnum((unsigned char)nom_fichier[i]) &&
            nom_fichier[i] != '.' && nom_fichier[i] != '_') {
            fprintf(stderr, "[ENV33-C] Erreur : nom de fichier invalide\n");
            return;
        }
    }

    printf("[ENV33-C] Nom de fichier valide : %s\n", nom_fichier);
}

/* ---- ARR30-C : Verification des limites de tableau ---- */
static void acces_tableau_securise(int *tableau, size_t taille, size_t index) {
    if (index < taille) {
        printf("[ARR30-C] tableau[%zu] = %d\n", index, tableau[index]);
    } else {
        fprintf(stderr, "[ARR30-C] Erreur : index %zu hors limites (max %zu)\n",
                index, taille - 1);
    }
}

int main(void) {
    printf("=== Demonstration des regles CERT C ===\n\n");

    /* PRE31-C : Fonction inline au lieu de macro */
    int i = 5;
    int resultat = carre(i++);
    printf("[PRE31-C] carre(5) avec i++ = %d, i apres = %d\n\n", resultat, i);

    /* DCL30-C : Portee limitee */
    incrementer();
    incrementer();
    printf("[DCL30-C] Compteur (portee fichier) = %d\n\n", compteur_module);

    /* EXP33-C : Initialisation */
    demo_initialisation();
    printf("\n");

    /* INT30-C : Allocation securisee */
    int *tab = allouer_tableau(10);
    if (tab != NULL) {
        for (int j = 0; j < 10; j++) {
            tab[j] = j * 10;
        }
        printf("[INT30-C] Allocation reussie, tab[5] = %d\n", tab[5]);
        free(tab);
    }

    /* Test avec valeur enorme */
    int *tab2 = allouer_tableau(SIZE_MAX);
    if (tab2 == NULL) {
        printf("[INT30-C] Allocation refusee (overflow detecte)\n");
    }
    printf("\n");

    /* FLP30-C : Boucle avec entiers */
    demo_boucle_flottants();
    printf("\n");

    /* STR31-C : Copie securisee */
    copier_nom_securise("Alice");
    copier_nom_securise("NomBeaucoupTropLongPourLeBuffer");
    printf("\n");

    /* MEM30-C : Free securise */
    demo_free_securise();
    printf("\n");

    /* FIO30-C : Printf securise */
    afficher_message_securise("Bonjour %x %x %x");  /* Les %x sont affiches, pas interpretes */
    printf("\n");

    /* ENV33-C : Validation nom de fichier */
    ouvrir_fichier_securise("config.txt");
    ouvrir_fichier_securise("fichier; rm -rf /");
    printf("\n");

    /* ARR30-C : Acces securise au tableau */
    int tableau[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    acces_tableau_securise(tableau, 10, 5);
    acces_tableau_securise(tableau, 10, 15);

    printf("\n=== Fin de la demonstration ===\n");
    return 0;
}
