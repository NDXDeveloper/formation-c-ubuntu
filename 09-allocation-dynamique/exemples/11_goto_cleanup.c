/* ============================================================================
   Section 9.4 : Gestion des erreurs d'allocation
   Description : Nettoyage automatique avec goto en cas d'erreur multiple
   Fichier source : 04-gestion-erreurs.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* nom;
    char* prenom;
    int* notes;
    size_t nb_notes;
} Etudiant;

Etudiant* creer_etudiant(const char* nom, const char* prenom, size_t nb_notes) {
    Etudiant* etudiant = NULL;
    char* nom_copie = NULL;
    char* prenom_copie = NULL;
    int* notes = NULL;

    etudiant = malloc(sizeof(Etudiant));
    if (etudiant == NULL) {
        fprintf(stderr, "Erreur : allocation de la structure\n");
        goto error;
    }

    nom_copie = malloc(strlen(nom) + 1);
    if (nom_copie == NULL) {
        fprintf(stderr, "Erreur : allocation du nom\n");
        goto error;
    }
    strcpy(nom_copie, nom);

    prenom_copie = malloc(strlen(prenom) + 1);
    if (prenom_copie == NULL) {
        fprintf(stderr, "Erreur : allocation du prenom\n");
        goto error;
    }
    strcpy(prenom_copie, prenom);

    notes = malloc(nb_notes * sizeof(int));
    if (notes == NULL) {
        fprintf(stderr, "Erreur : allocation des notes\n");
        goto error;
    }

    etudiant->nom = nom_copie;
    etudiant->prenom = prenom_copie;
    etudiant->notes = notes;
    etudiant->nb_notes = nb_notes;

    return etudiant;

error:
    free(notes);
    free(prenom_copie);
    free(nom_copie);
    free(etudiant);
    return NULL;
}

void liberer_etudiant(Etudiant* etudiant) {
    if (etudiant != NULL) {
        free(etudiant->notes);
        free(etudiant->prenom);
        free(etudiant->nom);
        free(etudiant);
    }
}

int main(void) {
    Etudiant* etudiant = creer_etudiant("Dupont", "Marie", 5);

    if (etudiant == NULL) {
        fprintf(stderr, "Impossible de creer l'etudiant\n");
        return 1;
    }

    printf("Etudiant cree : %s %s\n", etudiant->prenom, etudiant->nom);

    liberer_etudiant(etudiant);
    return 0;
}
