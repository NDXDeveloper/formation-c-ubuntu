/* ============================================================================
   Section 4.3 : Les constantes et les litteraux
   Description : Exemple complet : litteraux, constantes, enums, limites, macros
   Fichier source : 03-constantes-litteraux.md
   ============================================================================ */
#include <stdio.h>
#include <limits.h>

/* Macros */
#define VERSION "1.0.0"
#define TAILLE_BUFFER 256

/* Constantes */
const double PI = 3.14159265359;
const int SECONDES_PAR_MINUTE = 60;

/* Enumeration */
typedef enum {
    LUNDI = 1,
    MARDI,
    MERCREDI,
    JEUDI,
    VENDREDI,
    SAMEDI,
    DIMANCHE
} Jour;

int main(void) {
    /* Litteraux entiers (differentes bases) */
    int decimal = 255;
    int hexa = 0xFF;
    int octal = 0377;

    printf("Décimal : %d\n", decimal);
    printf("Hexa : %d\n", hexa);
    printf("Octal : %d\n", octal);

    /* Litteraux flottants */
    double vitesse_lumiere = 3.0e8;
    float prix = 19.99f;

    printf("\nVitesse de la lumière : %.2e m/s\n", vitesse_lumiere);
    printf("Prix : %.2f €\n", prix);

    /* Litteraux caracteres */
    char lettre = 'A';

    printf("\nLettre : %c (code ASCII : %d)\n", lettre, lettre);

    /* Constantes */
    printf("\nPi : %.10f\n", PI);
    printf("Version : %s\n", VERSION);

    /* Enumeration */
    Jour aujourd_hui = MERCREDI;
    printf("\nAujourd'hui : jour %d de la semaine\n", aujourd_hui);

    /* Limites systeme */
    printf("\nINT_MAX : %d\n", INT_MAX);
    printf("UINT_MAX : %u\n", UINT_MAX);

    /* Macros predefinies */
    printf("\nCompilé le %s à %s\n", __DATE__, __TIME__);

    /* Utilisation de TAILLE_BUFFER et SECONDES_PAR_MINUTE pour eviter warning unused */
    (void)TAILLE_BUFFER;
    (void)SECONDES_PAR_MINUTE;

    return 0;
}
