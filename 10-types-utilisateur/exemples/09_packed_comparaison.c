/* ============================================================================
   Section 10.3 : Packed structures
   Description : Comparaison tailles Normal vs Packed (attribut et pragma)
   Fichier source : 03-packed-structures.md
   Compilation : gcc -Wall -Wextra -Werror -std=c17 (sans -pedantic)
   ============================================================================ */

#include <stdio.h>
#include <stddef.h>

struct Normal {
    char c;
    int i;
    short s;
};

struct __attribute__((packed)) PackedAttribut {
    char c;
    int i;
    short s;
};

#pragma pack(push, 1)
struct PackedPragma {
    char c;
    int i;
    short s;
};
#pragma pack(pop)

int main(void) {
    printf("=== COMPARAISON DES TAILLES ===\n\n");

    printf("Structure normale     : %zu octets\n", sizeof(struct Normal));
    printf("Packed (attribut)     : %zu octets\n", sizeof(struct PackedAttribut));
    printf("Packed (pragma)       : %zu octets\n", sizeof(struct PackedPragma));

    printf("\n=== DISPOSITION MEMOIRE ===\n\n");

    printf("Normal - Offset c : %zu\n", offsetof(struct Normal, c));
    printf("Normal - Offset i : %zu\n", offsetof(struct Normal, i));
    printf("Normal - Offset s : %zu\n", offsetof(struct Normal, s));

    printf("\n");

    printf("Packed - Offset c : %zu\n", offsetof(struct PackedAttribut, c));
    printf("Packed - Offset i : %zu\n", offsetof(struct PackedAttribut, i));
    printf("Packed - Offset s : %zu\n", offsetof(struct PackedAttribut, s));

    return 0;
}
