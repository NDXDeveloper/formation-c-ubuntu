#include <stdio.h>
#include "book.h"
#include "member.h"
#include "loan.h"

int main(void) {
    printf("=== Systeme de gestion de bibliotheque ===\n");
    printf("(Demonstration des forward declarations)\n\n");

    /* Creer des livres */
    Book* livre1 = book_create("Le Petit Prince", "Saint-Exupery", "978-2070408504");
    Book* livre2 = book_create("Les Miserables", "Victor Hugo", "978-2070409228");

    /* Creer un membre */
    Member* alice = member_create("Alice Dupont", 1001);

    printf("=== Livres ===\n");
    printf("  \"%s\" par %s - %s\n",
           livre1->title, livre1->author,
           book_is_available(livre1) ? "disponible" : "emprunte");
    printf("  \"%s\" par %s - %s\n",
           livre2->title, livre2->author,
           book_is_available(livre2) ? "disponible" : "emprunte");

    printf("\n=== Membre ===\n");
    printf("  %s (ID: %d) - peut emprunter : %s\n",
           alice->name, alice->member_id,
           member_can_borrow(alice) ? "oui" : "non");

    /* Emprunter un livre */
    printf("\n=== Emprunt ===\n");
    Loan* emprunt1 = loan_create(livre1, alice);
    if (emprunt1 != NULL) {
        printf("  Emprunt #%d : \"%s\" par %s\n",
               emprunt1->loan_id, livre1->title, alice->name);
    }

    printf("\n=== Apres emprunt ===\n");
    printf("  \"%s\" - %s\n",
           livre1->title,
           book_is_available(livre1) ? "disponible" : "emprunte");
    printf("  Emprunts de %s : %d\n", alice->name, alice->loan_count);

    /* Tenter d'emprunter le meme livre (deja emprunte) */
    Loan* emprunt_echec = loan_create(livre1, alice);
    printf("  Emprunt du meme livre : %s\n",
           emprunt_echec == NULL ? "refuse (deja emprunte)" : "accepte");

    /* Emprunter un second livre */
    Loan* emprunt2 = loan_create(livre2, alice);
    if (emprunt2 != NULL) {
        printf("  Emprunt #%d : \"%s\" par %s\n",
               emprunt2->loan_id, livre2->title, alice->name);
    }

    /* Retourner un livre */
    printf("\n=== Retour ===\n");
    loan_return(emprunt1);
    printf("  \"%s\" retourne - %s\n",
           livre1->title,
           book_is_available(livre1) ? "disponible" : "emprunte");

    /* Liberation de la memoire */
    printf("\n=== Liberation memoire ===\n");
    loan_destroy(emprunt1);
    loan_destroy(emprunt2);
    member_destroy(alice);
    book_destroy(livre1);
    book_destroy(livre2);
    printf("Memoire liberee.\n");

    return 0;
}
