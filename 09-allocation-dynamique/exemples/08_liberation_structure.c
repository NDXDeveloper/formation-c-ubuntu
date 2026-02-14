/* ============================================================================
   Section 9.3 : Les fonctions malloc, calloc, realloc et free
   Description : Liberation correcte d'une structure avec pointeurs internes
   Fichier source : 03-fonctions-allocation.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* nom;
    char* prenom;
    int age;
} Personne;

Personne* creer_personne(const char* nom, const char* prenom, int age) {
    Personne* p = malloc(sizeof(Personne));
    if (p == NULL) return NULL;

    p->nom = malloc(strlen(nom) + 1);
    p->prenom = malloc(strlen(prenom) + 1);

    if (p->nom == NULL || p->prenom == NULL) {
        free(p->nom);
        free(p->prenom);
        free(p);
        return NULL;
    }

    strcpy(p->nom, nom);
    strcpy(p->prenom, prenom);
    p->age = age;

    return p;
}

void liberer_personne(Personne* p) {
    if (p != NULL) {
        free(p->nom);
        free(p->prenom);
        free(p);
    }
}

int main(void) {
    Personne* p = creer_personne("Dupont", "Marie", 25);

    if (p != NULL) {
        printf("%s %s a %d ans\n", p->prenom, p->nom, p->age);
        liberer_personne(p);
    }

    return 0;
}
