/* ============================================================================
   Section 22.5 : Fonctions variadiques
   Description : Fonction somme avec nombre variable d'arguments
   Fichier source : 05-fonctions-variadiques.md
   ============================================================================ */

#include <stdio.h>
#include <stdarg.h>

/**
 * Calcule la somme d'un nombre variable d'entiers
 * @param count Nombre d'arguments a additionner
 * @param ... Les entiers a additionner
 * @return La somme
 */
int somme(int count, ...) {
    va_list args;           // Liste pour parcourir les arguments
    va_start(args, count);  // Initialisation (count est le dernier param fixe)

    int total = 0;

    // Parcours de tous les arguments
    for (int i = 0; i < count; i++) {
        int valeur = va_arg(args, int);  // Recupere le prochain int
        total += valeur;
    }

    va_end(args);  // Nettoyage obligatoire

    return total;
}

int main(void) {
    printf("Somme de 2 nombres : %d\n", somme(2, 10, 20));
    printf("Somme de 4 nombres : %d\n", somme(4, 1, 2, 3, 4));
    printf("Somme de 6 nombres : %d\n", somme(6, 5, 10, 15, 20, 25, 30));

    return 0;
}
