/* ============================================================================
   Section 10.5 : enum - Code lisible et gestion d'etats
   Description : Machine a etats de connexion avec enum
   Fichier source : 05-enumerations.md
   ============================================================================ */

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

    printf(" -> ");

    switch (nouvel_etat) {
        case DECONNECTE:           printf("DECONNECTE\n"); break;
        case CONNEXION_EN_COURS:   printf("CONNEXION_EN_COURS\n"); break;
        case CONNECTE:             printf("CONNECTE\n"); break;
        case DECONNEXION_EN_COURS: printf("DECONNEXION_EN_COURS\n"); break;
    }

    conn->etat = nouvel_etat;
}

int main(void) {
    struct Connexion conn = {DECONNECTE, "serveur.exemple.com"};

    changer_etat(&conn, CONNEXION_EN_COURS);
    changer_etat(&conn, CONNECTE);
    changer_etat(&conn, DECONNEXION_EN_COURS);
    changer_etat(&conn, DECONNECTE);

    return 0;
}
