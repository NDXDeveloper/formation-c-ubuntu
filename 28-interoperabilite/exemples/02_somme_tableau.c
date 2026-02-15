/* ============================================================================
   Section 28.1 : Appeler du C depuis Python
   Description : Fonction de somme de tableau compilee en bibliotheque partagee
                 pour utilisation via ctypes (passage de pointeur + size_t)
   Fichier source : 01-c-depuis-python.md
   ============================================================================ */

#include <stddef.h>

/* Calcule la somme des elements d'un tableau */
int somme_tableau(int *tableau, size_t taille) {
    int somme = 0;
    for (size_t i = 0; i < taille; i++) {
        somme += tableau[i];
    }
    return somme;
}
