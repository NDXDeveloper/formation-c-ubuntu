/* ============================================================================
   Section 17.6 : Signaux courants
   Description : SIGFPE - gestion de la division par zero avec recovery
   Fichier source : 06-signaux-courants.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>

static jmp_buf jump_buffer;

void sigfpe_handler(int signum) {
    (void)signum;
    write(STDERR_FILENO, "\nErreur: Division par zero!\n", 28);
    longjmp(jump_buffer, 1);
}

int safe_divide(int a, int b, int *result) {
    struct sigaction sa, old_sa;

    sa.sa_handler = sigfpe_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGFPE, &sa, &old_sa);

    if (setjmp(jump_buffer) == 0) {
        *result = a / b;
        sigaction(SIGFPE, &old_sa, NULL);
        return 0;  /* Succes */
    } else {
        sigaction(SIGFPE, &old_sa, NULL);
        return -1;  /* Erreur */
    }
}

int main(void) {
    int result;

    if (safe_divide(10, 2, &result) == 0) {
        printf("10 / 2 = %d\n", result);
    }

    if (safe_divide(10, 0, &result) == -1) {
        printf("Division par zero evitee\n");
    }

    printf("Programme continue normalement\n");

    return 0;
}
