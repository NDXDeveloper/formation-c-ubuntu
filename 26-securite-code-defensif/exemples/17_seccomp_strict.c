/* ============================================================================
   Section 26.6 : Principe du moindre privilege
   Description : Filtrage d'appels systeme avec seccomp mode strict
                 (seuls read, write, exit et sigreturn sont autorises)
   Fichier source : 06-moindre-privilege.md
   ============================================================================ */

#define _DEFAULT_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <linux/seccomp.h>
#include <sys/syscall.h>

int main(void) {
    printf("=== Seccomp mode strict ===\n\n");

    printf("Avant seccomp : tout fonctionne\n");
    printf("PID du processus : %d\n", getpid());

    /* Activer seccomp mode strict */
    printf("\nActivation de seccomp strict...\n");

    /* Vider le buffer stdout AVANT seccomp (apres, fflush utilise des syscalls bloques) */
    fflush(stdout);

    if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT) != 0) {
        perror("prctl");
        /* Note: peut echouer dans certains environnements conteneurises */
        printf("Seccomp non disponible, fin du programme\n");
        /* Utiliser syscall(SYS_exit, 0) au lieu de return car seccomp pourrait
           bloquer _exit si active partiellement */
        return 1;
    }

    /* Apres seccomp strict, seuls read/write/exit/sigreturn fonctionnent */

    /* write() fonctionne (stdout est fd 1) */
    const char msg1[] = "Apres seccomp : write() fonctionne encore\n";
    (void)write(STDOUT_FILENO, msg1, sizeof(msg1) - 1);

    const char msg2[] = "Seuls read, write, exit et sigreturn sont autorises\n";
    (void)write(STDOUT_FILENO, msg2, sizeof(msg2) - 1);

    const char msg3[] = "Tout autre appel systeme -> SIGKILL\n";
    (void)write(STDOUT_FILENO, msg3, sizeof(msg3) - 1);

    const char msg4[] = "\nFin du programme (via syscall exit)\n";
    (void)write(STDOUT_FILENO, msg4, sizeof(msg4) - 1);

    /* Utiliser syscall(SYS_exit) car exit() pourrait appeler d'autres syscalls */
    syscall(SYS_exit, 0);

    /* Jamais atteint */
    return 0;
}
