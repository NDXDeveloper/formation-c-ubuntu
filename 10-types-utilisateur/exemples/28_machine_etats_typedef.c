/* ============================================================================
   Section 10.6 : typedef - Simplification de la syntaxe
   Description : Machine a etats avec typedef de pointeurs de fonctions
   Fichier source : 06-typedef.md
   ============================================================================ */

#include <stdio.h>

typedef enum {
    ETAT_ETEINT,
    ETAT_DEMARRAGE,
    ETAT_ACTIF,
    ETAT_PAUSE,
    ETAT_ARRET
} Etat;

typedef enum {
    EVENT_DEMARRER,
    EVENT_PAUSE,
    EVENT_REPRENDRE,
    EVENT_ARRETER
} Evenement;

typedef Etat (*GestionnaireFn)(Evenement);

Etat gerer_eteint(Evenement e) {
    if (e == EVENT_DEMARRER) {
        printf("Demarrage...\n");
        return ETAT_DEMARRAGE;
    }
    return ETAT_ETEINT;
}

Etat gerer_actif(Evenement e) {
    switch (e) {
        case EVENT_PAUSE:
            printf("Mise en pause...\n");
            return ETAT_PAUSE;
        case EVENT_ARRETER:
            printf("Arret...\n");
            return ETAT_ARRET;
        default:
            return ETAT_ACTIF;
    }
}

Etat gerer_pause(Evenement e) {
    switch (e) {
        case EVENT_REPRENDRE:
            printf("Reprise...\n");
            return ETAT_ACTIF;
        case EVENT_ARRETER:
            printf("Arret...\n");
            return ETAT_ARRET;
        default:
            return ETAT_PAUSE;
    }
}

int main(void) {
    Etat etat_actuel = ETAT_ETEINT;
    GestionnaireFn gestionnaires[] = {
        [ETAT_ETEINT] = gerer_eteint,
        [ETAT_ACTIF] = gerer_actif,
        [ETAT_PAUSE] = gerer_pause
    };

    etat_actuel = gestionnaires[etat_actuel](EVENT_DEMARRER);
    etat_actuel = ETAT_ACTIF;
    etat_actuel = gestionnaires[etat_actuel](EVENT_PAUSE);
    etat_actuel = gestionnaires[etat_actuel](EVENT_REPRENDRE);
    etat_actuel = gestionnaires[etat_actuel](EVENT_ARRETER);

    return 0;
}
