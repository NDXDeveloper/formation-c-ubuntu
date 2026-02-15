/* ============================================================================
   Section 23.6 : Macros predefinies utiles
   Description : __COUNTER__ - compteur auto-incrementant (extension GCC/Clang)
   Fichier source : 06-macros-predefinies.md
   ============================================================================ */

#include <stdio.h>

// Macros d'indirection nécessaires : ## empêche l'expansion de ses opérandes
#define CONCAT_IMPL(a, b) a##b
#define CONCAT(a, b) CONCAT_IMPL(a, b)

#define DECLARE_VAR(prefix) \
    int CONCAT(prefix, __COUNTER__)

int main(void) {
    printf("Compteur simple :\n");
    printf("Compteur 1 : %d\n", __COUNTER__);  // 0
    printf("Compteur 2 : %d\n", __COUNTER__);  // 1
    printf("Compteur 3 : %d\n", __COUNTER__);  // 2
    printf("Compteur 4 : %d\n", __COUNTER__);  // 3

    printf("\nGénération de variables uniques :\n");
    DECLARE_VAR(var);  // int var4
    DECLARE_VAR(var);  // int var5
    DECLARE_VAR(var);  // int var6

    var4 = 10;
    var5 = 20;
    var6 = 30;

    printf("%d %d %d\n", var4, var5, var6);

    return 0;
}
