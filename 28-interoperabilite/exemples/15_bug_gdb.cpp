/* ============================================================================
   Section 28.6 : Name Mangling
   Description : Bug intentionnel pour demonstrer le demangling dans GDB
                 (dereferencement de nullptr -> SIGSEGV)
                 Compiler avec -g pour symboles de debug :
                 g++ -g 15_bug_gdb.cpp -o bug_gdb
                 Lancer : gdb ./bug_gdb puis 'run' puis 'bt'
   Fichier source : 06-name-mangling.md
   ============================================================================ */

#include <iostream>

void fonction_problematique(int x) {
    int *ptr = nullptr;
    *ptr = x;  /* Crash intentionnel ! */
}

int main() {
    std::cout << "=== Bug intentionnel pour GDB ===" << std::endl;
    fonction_problematique(42);
    return 0;
}
