/* ============================================================================
   Section 17.7 : Masquage de signaux
   Description : Pattern de section critique avec masquage complet
   Fichier source : 07-masquage-signaux.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

typedef struct {
    int compteur;
    char buffer[256];
} SharedData;

SharedData data = {0, ""};

void update_data(void) {
    sigset_t newmask, oldmask;

    /* Preparer le masque (bloquer tous les signaux sauf SIGKILL/SIGSTOP) */
    sigfillset(&newmask);

    /* Bloquer les signaux et sauvegarder l'ancien masque */
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    printf("[Section critique] Debut\n");

    /* === SECTION CRITIQUE === */
    /* Aucun signal ne peut interrompre cette partie */

    data.compteur++;
    snprintf(data.buffer, sizeof(data.buffer),
             "Donnee %d", data.compteur);

    printf("[Section critique] Modification de data.compteur = %d\n",
           data.compteur);

    sleep(2);  /* Simuler une operation longue */

    /* === FIN SECTION CRITIQUE === */

    printf("[Section critique] Fin\n");

    /* Restaurer le masque de signaux */
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

int main(void) {
    printf("Programme demarre (PID: %d)\n", getpid());
    printf("Les signaux seront bloques pendant les sections critiques\n\n");

    for (int i = 0; i < 3; i++) {
        printf("=== Iteration %d ===\n", i + 1);
        update_data();
        sleep(1);
    }

    printf("\nProgramme termine\n");

    return 0;
}
