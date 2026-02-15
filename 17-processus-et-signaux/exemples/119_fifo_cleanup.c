/* ============================================================================
   Section 17.9 : Named pipes (FIFO)
   Description : Nettoyage automatique du FIFO avec handler de signal
   Fichier source : 09-named-pipes.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/stat.h>

static const char *fifo_path = "/tmp/my_app_fifo_test";

void cleanup(int sig) {
    (void)sig;
    printf("\nNettoyage...\n");
    unlink(fifo_path);
    exit(0);
}

int main(void) {
    /* Installer les handlers de nettoyage */
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);

    /* Creer le FIFO */
    mkfifo(fifo_path, 0666);

    printf("FIFO cree: %s\n", fifo_path);
    printf("Appuyez sur Ctrl+C pour quitter.\n");

    /* Programme principal... */
    while (1) {
        sleep(1);
    }

    return 0;
}
