/* ============================================================================
   Section 35.1 : Core dumps en production
   Description : Gestionnaire de signal avec backtrace pour diagnostic crash
   Fichier source : 01-core-dumps-production.md
   ============================================================================ */
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>
#include <stdlib.h>

void signal_handler(int sig) {
    void *array[10];
    int size = backtrace(array, 10);

    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);

    signal(sig, SIG_DFL);
    raise(sig);
}

/* Fonction qui provoque volontairement un SEGFAULT */
void cause_crash(void) {
    int *ptr = NULL;
    *ptr = 42;  /* Dereferencement de pointeur NULL */
}

int main(void) {
    signal(SIGSEGV, signal_handler);
    signal(SIGABRT, signal_handler);

    printf("Installation du gestionnaire de signal...\n");
    printf("Declenchement d'un crash volontaire...\n");
    fflush(stdout);

    cause_crash();

    return 0;
}
