#include <stdio.h>

/* Prototypes des fonctions publiques */
void incrementer(void);
void decrementer(void);
int obtenir_valeur(void);
void afficher(void);

int main(void) {
    afficher();              /* Compteur : 0 */
    incrementer();
    incrementer();
    afficher();              /* Compteur : 2 */
    decrementer();
    afficher();              /* Compteur : 1 */

    int val = obtenir_valeur();
    printf("Valeur recuperee : %d\n", val);  /* 1 */

    /* compteur_interne = 100;  --> ERREUR : variable inaccessible ! */

    return 0;
}
