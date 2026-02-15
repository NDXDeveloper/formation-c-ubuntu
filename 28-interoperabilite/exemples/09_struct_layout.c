/* ============================================================================
   Section 28.4 : ABI et compatibilite
   Description : Demonstration du layout memoire des structures, padding,
                 alignement et impact de l'ordre des champs
   Fichier source : 04-abi-compatibilite.md
   ============================================================================ */

#include <stdio.h>
#include <stddef.h>
#include <stdint.h>

/* Structure avec padding important (ordre non optimal) */
struct Exemple1 {
    char a;      /* 1 byte */
    int b;       /* 4 bytes */
    char c;      /* 1 byte */
};
/* Layout : [a][___][bbbb][c][___] = 12 bytes */

/* Structure reorganisee (ordre optimal) */
struct Exemple2 {
    int b;       /* 4 bytes */
    char a;      /* 1 byte */
    char c;      /* 1 byte */
};
/* Layout : [bbbb][a][c][__] = 8 bytes */

/* Structure avec double (alignement 8) */
struct Exemple3 {
    char a;      /* 1 byte */
    double b;    /* 8 bytes */
    char c;      /* 1 byte */
};
/* Layout : [a][_______][bbbbbbbb][c][_______] = 24 bytes */

/* Meme structure reorganisee */
struct Exemple4 {
    double b;    /* 8 bytes */
    char a;      /* 1 byte */
    char c;      /* 1 byte */
};
/* Layout : [bbbbbbbb][a][c][______] = 16 bytes */

/* Structure packed (pas de padding) */
struct __attribute__((packed)) ExemplePacked {
    char a;
    int b;
    char c;
};

int main(void) {
    printf("=== ABI : Layout des structures ===\n\n");

    /* Tailles des types */
    printf("--- Tailles des types de base ---\n");
    printf("  char        : %zu byte\n", sizeof(char));
    printf("  short       : %zu bytes\n", sizeof(short));
    printf("  int         : %zu bytes\n", sizeof(int));
    printf("  long        : %zu bytes\n", sizeof(long));
    printf("  long long   : %zu bytes\n", sizeof(long long));
    printf("  float       : %zu bytes\n", sizeof(float));
    printf("  double      : %zu bytes\n", sizeof(double));
    printf("  void*       : %zu bytes\n", sizeof(void *));
    printf("  size_t      : %zu bytes\n", sizeof(size_t));
    printf("  int32_t     : %zu bytes\n", sizeof(int32_t));
    printf("  int64_t     : %zu bytes\n", sizeof(int64_t));

    /* Layout avec padding */
    printf("\n--- Structures avec padding ---\n");

    printf("\n  struct Exemple1 { char a; int b; char c; }\n");
    printf("    sizeof  = %zu bytes\n", sizeof(struct Exemple1));
    printf("    offset a = %zu\n", offsetof(struct Exemple1, a));
    printf("    offset b = %zu\n", offsetof(struct Exemple1, b));
    printf("    offset c = %zu\n", offsetof(struct Exemple1, c));
    printf("    => Padding : %zu bytes gaspilles\n",
           sizeof(struct Exemple1)
           - sizeof(char) - sizeof(int) - sizeof(char));

    printf("\n  struct Exemple2 { int b; char a; char c; }\n");
    printf("    sizeof  = %zu bytes\n", sizeof(struct Exemple2));
    printf("    offset b = %zu\n", offsetof(struct Exemple2, b));
    printf("    offset a = %zu\n", offsetof(struct Exemple2, a));
    printf("    offset c = %zu\n", offsetof(struct Exemple2, c));
    printf("    => Padding : %zu bytes gaspilles\n",
           sizeof(struct Exemple2)
           - sizeof(int) - sizeof(char) - sizeof(char));

    printf("\n  struct Exemple3 { char a; double b; char c; }\n");
    printf("    sizeof  = %zu bytes\n", sizeof(struct Exemple3));
    printf("    offset a = %zu\n", offsetof(struct Exemple3, a));
    printf("    offset b = %zu\n", offsetof(struct Exemple3, b));
    printf("    offset c = %zu\n", offsetof(struct Exemple3, c));

    printf("\n  struct Exemple4 { double b; char a; char c; }\n");
    printf("    sizeof  = %zu bytes\n", sizeof(struct Exemple4));

    /* Structure packed */
    printf("\n--- Structure packed (sans padding) ---\n");
    printf("  struct packed { char a; int b; char c; }\n");
    printf("    sizeof  = %zu bytes (vs %zu normal)\n",
           sizeof(struct ExemplePacked), sizeof(struct Exemple1));

    /* Types portables */
    printf("\n--- Types portables (stdint.h) ---\n");
    printf("  int32_t  : toujours %zu bytes\n", sizeof(int32_t));
    printf("  int64_t  : toujours %zu bytes\n", sizeof(int64_t));
    printf("  uint32_t : toujours %zu bytes\n", sizeof(uint32_t));
    printf("  Recommandation : preferer int32_t/int64_t a int/long\n");
    printf("  (long = %zu bytes ici, mais 4 sur Windows 64 bits !)\n",
           sizeof(long));

    printf("\n=== Fin ===\n");
    return 0;
}
