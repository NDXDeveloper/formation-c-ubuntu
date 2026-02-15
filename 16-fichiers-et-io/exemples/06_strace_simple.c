/* ============================================================================
   Section 16.2 : Appels systeme
   Description : Programme simple pour observer les syscalls avec strace
   Fichier source : 02-appels-systeme.md
   ============================================================================ */

/* Compiler : gcc -o 06_strace_simple 06_strace_simple.c
   Utiliser : strace -e write ./06_strace_simple */

#include <stdio.h>

int main(void) {
    printf("Bonjour\n");
    return 0;
}
