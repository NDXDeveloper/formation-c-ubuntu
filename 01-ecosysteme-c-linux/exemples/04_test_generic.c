/* ============================================================================
   Section 1.3.2 : Verification du support compilateur
   Description : Test du support de _Generic (C11)
   Fichier source : 03.2-verification-support-compilateur.md
   ============================================================================ */
#include <stdio.h>

#define print_type(x) _Generic((x), \
    int: "int", \
    float: "float", \
    double: "double", \
    char *: "string", \
    default: "unknown" \
)

int main(void) {
    printf("Type de 42 : %s\n", print_type(42));
    printf("Type de 3.14f : %s\n", print_type(3.14f));
    printf("Type de \"Hello\" : %s\n", print_type("Hello"));
    return 0;
}
