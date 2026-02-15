/* ============================================================================
   Section 23.7 : X-Macros
   Description : Machine a etats avec X-Macros
   Fichier source : 07-x-macros.md
   ============================================================================ */

#include <stdio.h>

// Liste des états
#define STATE_LIST \
    X(STATE_IDLE,       "Inactif") \
    X(STATE_CONNECTING, "Connexion en cours") \
    X(STATE_CONNECTED,  "Connecté") \
    X(STATE_SENDING,    "Envoi en cours") \
    X(STATE_RECEIVING,  "Réception en cours") \
    X(STATE_ERROR,      "Erreur")

// Génération de l'enum
#define X(name, description) name,
typedef enum {
    STATE_LIST
    STATE_COUNT
} State;
#undef X

// Génération des descriptions
#define X(name, description) description,
const char* state_descriptions[] = {
    STATE_LIST
};
#undef X

// Génération d'une fonction de validation
#define X(name, description) \
    case name: return 1;
int is_valid_state(State s) {
    switch(s) {
        STATE_LIST
        default: return 0;
    }
}
#undef X

int main(void) {
    State etat_actuel = STATE_CONNECTING;

    printf("État actuel: %s\n", state_descriptions[etat_actuel]);
    printf("État valide: %s\n", is_valid_state(etat_actuel) ? "Oui" : "Non");

    printf("\nTous les états:\n");
    for (int i = 0; i < STATE_COUNT; i++) {
        printf("  %d: %s\n", i, state_descriptions[i]);
    }

    return 0;
}
