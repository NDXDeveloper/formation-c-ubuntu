/* ============================================================================
   Section 17.6 : Signaux courants
   Description : SIGSEGV avec backtrace - logging avant crash
   Fichier source : 06-signaux-courants.md
   Note : Compiler avec -rdynamic pour de meilleures backtraces
   ============================================================================ */

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>
#include <stdlib.h>

void sigsegv_handler(int signum) {
    (void)signum;
    void *array[10];
    int size;

    write(STDERR_FILENO, "\n=== CRASH DETECTE ===\n", 23);

    /* Obtenir la backtrace */
    size = backtrace(array, 10);

    write(STDERR_FILENO, "Stack trace:\n", 13);
    backtrace_symbols_fd(array, size, STDERR_FILENO);

    write(STDERR_FILENO, "\n===================\n", 21);

    /* Terminer immediatement */
    _exit(1);
}

int main(void) {
    signal(SIGSEGV, sigsegv_handler);

    /* Code avec bug... */
    int *ptr = NULL;
    *ptr = 42;

    return 0;
}
