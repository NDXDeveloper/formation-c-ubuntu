/* ============================================================================
   Section 22.2 : Tableaux de pointeurs de fonctions
   Description : Machine a etats finis (FSM) pour une porte automatique
   Fichier source : 02-tableaux-pointeurs-fonctions.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>  // Pour sleep()

// Enumeration des etats possibles
typedef enum {
    ETAT_FERMEE,
    ETAT_OUVERTURE,
    ETAT_OUVERTE,
    ETAT_FERMETURE,
    NB_ETATS
} EtatPorte;

// Enumeration des evenements
typedef enum {
    EVT_DETECTER_PERSONNE,
    EVT_TIMEOUT,
    EVT_OBSTACLE,
    EVT_AUCUN
} Evenement;

// Structure pour stocker l'etat actuel
typedef struct {
    EtatPorte etat_actuel;
} Porte;

// Prototypes des fonctions d'etat
void etat_fermee(Porte *p, Evenement evt);
void etat_ouverture(Porte *p, Evenement evt);
void etat_ouverte(Porte *p, Evenement evt);
void etat_fermeture(Porte *p, Evenement evt);

// Type pour les fonctions d'etat
typedef void (*FonctionEtat)(Porte *, Evenement);

// Tableau de pointeurs vers les fonctions d'etat
FonctionEtat table_etats[NB_ETATS] = {
    etat_fermee,
    etat_ouverture,
    etat_ouverte,
    etat_fermeture
};

// Implementation des fonctions d'etat
void etat_fermee(Porte *p, Evenement evt) {
    printf("Etat : FERMEE\n");
    if (evt == EVT_DETECTER_PERSONNE) {
        printf("  -> Personne detectee, ouverture...\n");
        p->etat_actuel = ETAT_OUVERTURE;
    }
}

void etat_ouverture(Porte *p, Evenement evt) {
    (void)evt;  // Non utilise dans cet etat
    printf("Etat : OUVERTURE EN COURS\n");
    printf("  -> Moteur active...\n");
    sleep(1);  // Simulation du temps d'ouverture
    p->etat_actuel = ETAT_OUVERTE;
}

void etat_ouverte(Porte *p, Evenement evt) {
    printf("Etat : OUVERTE\n");
    if (evt == EVT_TIMEOUT) {
        printf("  -> Timeout, fermeture...\n");
        p->etat_actuel = ETAT_FERMETURE;
    }
}

void etat_fermeture(Porte *p, Evenement evt) {
    printf("Etat : FERMETURE EN COURS\n");
    if (evt == EVT_OBSTACLE) {
        printf("  -> Obstacle detecte ! Reouverture...\n");
        p->etat_actuel = ETAT_OUVERTURE;
    } else {
        printf("  -> Moteur active...\n");
        sleep(1);  // Simulation du temps de fermeture
        p->etat_actuel = ETAT_FERMEE;
    }
}

// Fonction pour traiter un evenement
void traiter_evenement(Porte *p, Evenement evt) {
    // Appel de la fonction d'etat appropriee via le tableau
    table_etats[p->etat_actuel](p, evt);
}

int main(void) {
    Porte porte = {ETAT_FERMEE};

    printf("=== SIMULATION PORTE AUTOMATIQUE ===\n\n");

    // Simulation d'une sequence d'evenements
    traiter_evenement(&porte, EVT_AUCUN);
    traiter_evenement(&porte, EVT_DETECTER_PERSONNE);
    traiter_evenement(&porte, EVT_AUCUN);
    traiter_evenement(&porte, EVT_AUCUN);
    traiter_evenement(&porte, EVT_TIMEOUT);
    traiter_evenement(&porte, EVT_OBSTACLE);
    traiter_evenement(&porte, EVT_AUCUN);
    traiter_evenement(&porte, EVT_AUCUN);
    traiter_evenement(&porte, EVT_AUCUN);

    printf("\n=== FIN DE LA SIMULATION ===\n");

    return 0;
}
