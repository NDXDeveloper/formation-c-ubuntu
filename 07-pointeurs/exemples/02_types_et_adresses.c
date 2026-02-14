/* ============================================================================
   Section 7.1 : Concept d'adresse mémoire
   Description : Types de données et leurs adresses/tailles en mémoire
   Fichier source : 01-adresse-memoire.md
   ============================================================================ */

#include <stdio.h>

int main(void) {
    char    lettre = 'A';
    int     nombre = 42;
    float   decimal = 3.14f;
    double  grand = 2.718281828;

    printf("Adresse de lettre (char)   : %p (taille : %zu octet)\n",
           (void*)&lettre, sizeof(lettre));
    printf("Adresse de nombre (int)    : %p (taille : %zu octets)\n",
           (void*)&nombre, sizeof(nombre));
    printf("Adresse de decimal (float) : %p (taille : %zu octets)\n",
           (void*)&decimal, sizeof(decimal));
    printf("Adresse de grand (double)  : %p (taille : %zu octets)\n",
           (void*)&grand, sizeof(grand));

    printf("\nTaille d'une adresse : %zu octets\n", sizeof(&nombre));

    return 0;
}
