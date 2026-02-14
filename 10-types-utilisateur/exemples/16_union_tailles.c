/* ============================================================================
   Section 10.4 : union - Optimisation memoire
   Description : Comparaison de tailles struct sans/avec union
   Fichier source : 04-unions.md
   ============================================================================ */

#include <stdio.h>

struct Personne1 {
    char nom[32];
    int type;
    int annee_etude;
    char universite[32];
    float salaire;
    char entreprise[32];
};

struct Personne2 {
    char nom[32];
    int type;
    union {
        struct {
            int annee_etude;
            char universite[32];
        } etudiant;
        struct {
            float salaire;
            char entreprise[32];
        } employe;
    } info;
};

int main(void) {
    printf("Taille struct Personne1 : %zu octets\n", sizeof(struct Personne1));
    printf("Taille struct Personne2 : %zu octets\n", sizeof(struct Personne2));
    printf("Gain : %zu octets\n",
           sizeof(struct Personne1) - sizeof(struct Personne2));

    return 0;
}
