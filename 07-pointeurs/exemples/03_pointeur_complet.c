/* ============================================================================
   Section 7.2 : Déclaration, déréférencement et adresse
   Description : Exemple complet : declaration, dereferencement, modification
   Fichier source : 02-declaration-dereferencement.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    int nombre = 42;
    int *ptr_nombre = &nombre;

    printf("=== Variable 'nombre' ===\n");
    printf("Valeur de nombre : %d\n", nombre);
    printf("Adresse de nombre : %p\n", (void*)&nombre);
    printf("Taille de nombre : %zu octets\n\n", sizeof(nombre));

    printf("=== Pointeur 'ptr_nombre' ===\n");
    printf("Valeur de ptr_nombre (adresse stockee) : %p\n", (void*)ptr_nombre);
    printf("Adresse de ptr_nombre lui-meme : %p\n", (void*)&ptr_nombre);
    printf("Taille de ptr_nombre : %zu octets\n", sizeof(ptr_nombre));
    printf("Valeur pointee (*ptr_nombre) : %d\n\n", *ptr_nombre);

    printf("=== Verification ===\n");
    if (&nombre == ptr_nombre) {
        printf("ptr_nombre pointe bien vers nombre\n\n");
    }

    printf("=== Modification via le pointeur ===\n");
    printf("Avant : nombre = %d\n", nombre);
    *ptr_nombre = 100;
    printf("Apres (*ptr_nombre = 100) : nombre = %d\n", nombre);

    return 0;
}
