/* ============================================================================
   Section 34.1.3 : Output formate
   Description : Specificateurs de format printf (largeur, precision, alignement)
   Fichier source : 01.3-output-formate.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    int entier = 42;
    float flottant = 3.14159265f;
    char caractere = 'A';
    char *chaine = "Hello";
    void *pointeur = &entier;

    // Formats de base
    printf("=== Formats de base ===\n");
    printf("Entier décimal: %d\n", entier);
    printf("Entier hexadécimal: %x\n", entier);
    printf("Entier octal: %o\n", entier);
    printf("Flottant: %f\n", (double)flottant);
    printf("Notation scientifique: %e\n", (double)flottant);
    printf("Caractère: %c\n", caractere);
    printf("Chaîne: %s\n", chaine);
    printf("Pointeur: %p\n", pointeur);

    // Largeur et precision
    int num = 42;
    double pi = 3.14159265;

    printf("\n=== Largeur et precision ===\n");
    printf("|%5d|\n", num);      // |   42|
    printf("|%-5d|\n", num);     // |42   |
    printf("%05d\n", num);       // 00042
    printf("%.2f\n", pi);        // 3.14
    printf("%.4f\n", pi);        // 3.1416
    printf("%8.2f\n", pi);       // "    3.14"
    printf("%-8.2f\n", pi);      // "3.14    "
    printf("%.5s\n", "Hello World");  // "Hello"

    // Formatage dynamique
    printf("\n=== Formatage dynamique ===\n");
    int largeur = 10;
    int precision = 2;
    double nombre = 3.14159;
    printf("%*.*f\n", largeur, precision, nombre);

    return 0;
}
