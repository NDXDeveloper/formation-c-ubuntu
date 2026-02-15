/* ============================================================================
   Section 22.4 : Pointeurs opaques
   Description : Implementation privee du type opaque Personne
   Fichier source : 04-pointeurs-opaques.md
   ============================================================================ */

#include "personne.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definition complete : visible UNIQUEMENT dans ce fichier
struct Personne {
    char nom[50];
    char prenom[50];
    int age;
    float salaire;
    int id_employe;  // Peut changer sans affecter les clients !
};

Personne* creer_personne(const char *nom, const char *prenom, int age) {
    Personne *p = malloc(sizeof(Personne));
    if (p == NULL) {
        return NULL;
    }

    strncpy(p->nom, nom, sizeof(p->nom) - 1);
    p->nom[sizeof(p->nom) - 1] = '\0';

    strncpy(p->prenom, prenom, sizeof(p->prenom) - 1);
    p->prenom[sizeof(p->prenom) - 1] = '\0';

    p->age = age;
    p->salaire = 0.0f;
    p->id_employe = 0;

    return p;
}

void definir_salaire(Personne *p, float salaire) {
    if (p != NULL && salaire >= 0) {
        p->salaire = salaire;
        printf("[LOG] Salaire modifie pour %s %s\n", p->prenom, p->nom);
    }
}

float obtenir_salaire(const Personne *p) {
    if (p == NULL) {
        return 0.0f;
    }
    return p->salaire;
}

int obtenir_age(const Personne *p) {
    return (p != NULL) ? p->age : 0;
}

void afficher_personne(const Personne *p) {
    if (p != NULL) {
        printf("Personne : %s %s, %d ans\n", p->prenom, p->nom, p->age);
    }
}

void liberer_personne(Personne *p) {
    free(p);
}
