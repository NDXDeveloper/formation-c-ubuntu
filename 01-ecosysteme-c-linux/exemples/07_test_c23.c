/* ============================================================================
   Section 1.3.1 : Focus sur C23
   Description : Exemple de programme C23 (attributs, binaire, typeof, nullptr)
   Fichier source : 03.1-focus-c23.md
   ============================================================================ */
#include <stdio.h>

[[nodiscard]] int compute(int x) {
    return x * 2;
}

int main(void) {
    int flags = 0b1010;
    typeof(flags) result = compute(flags);
    int *ptr = nullptr;

    printf("Result: %d\n", result);
    printf("Pointer is null: %s\n", ptr == nullptr ? "yes" : "no");

    return 0;
}
