/* ============================================================================
   Section 22.1 : Pointeurs de fonctions (Callbacks)
   Description : Systeme simple de gestion d'evenements avec callbacks
   Fichier source : 01-pointeurs-fonctions.md
   ============================================================================ */

#include <stdio.h>

// Type pour les callbacks d'evenements
typedef void (*EventCallback)(const char *message);

// Differents handlers d'evenements
void on_error(const char *message) {
    printf("[ERREUR] %s\n", message);
}

void on_warning(const char *message) {
    printf("[ATTENTION] %s\n", message);
}

void on_info(const char *message) {
    printf("[INFO] %s\n", message);
}

// Structure pour gerer les evenements
typedef struct {
    const char *nom_evenement;
    EventCallback callback;
} Evenement;

// Fonction qui declenche un evenement
void declencher_evenement(Evenement *evt, const char *message) {
    if (evt != NULL && evt->callback != NULL) {
        printf("Evenement '%s' declenche : ", evt->nom_evenement);
        evt->callback(message);
    }
}

int main(void) {
    // Creation d'evenements avec differents callbacks
    Evenement erreur = {"erreur", on_error};
    Evenement warning = {"warning", on_warning};
    Evenement info = {"info", on_info};

    // Declenchement des evenements
    declencher_evenement(&info, "Application demarree");
    declencher_evenement(&warning, "Memoire faible");
    declencher_evenement(&erreur, "Fichier introuvable");

    return 0;
}
