/* ============================================================================
   Section 1.3.2 : Verification du support compilateur
   Description : Test des fonctionnalites C23 (attributs, binaire, nullptr)
   Fichier source : 03.2-verification-support-compilateur.md
   ============================================================================ */
#include <stdio.h>

[[nodiscard]] int compute(int x) {
    return x * 2;
}

int main(void) {
    int flags = 0b1010;
    int *ptr = nullptr;

    printf("Flags : %d\n", flags);
    printf("Pointer is null : %s\n", ptr == nullptr ? "oui" : "non");

    return 0;
}
