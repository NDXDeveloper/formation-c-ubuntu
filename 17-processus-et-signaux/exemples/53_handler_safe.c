/* ============================================================================
   Section 17.5 : Gestion des signaux
   Description : Handler async-signal-safe utilisant write() au lieu de printf()
   Fichier source : 05-gestion-signaux.md
   ============================================================================ */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

/* BON : utilisation de write() */
void safe_handler(int signum) {
    (void)signum;
    const char msg[] = "\n[HANDLER] Signal recu!\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

int main(void) {
    signal(SIGINT, safe_handler);

    printf("PID: %d - Appuyez Ctrl+C (10 iterations)\n", getpid());

    for (int i = 0; i < 10; i++) {
        write(STDOUT_FILENO, "En cours...\n", 12);
        sleep(1);
    }

    return 0;
}
