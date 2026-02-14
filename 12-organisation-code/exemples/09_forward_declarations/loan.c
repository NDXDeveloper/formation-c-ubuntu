#include "loan.h"
#include "book.h"    /* Inclusion complete pour acceder aux champs */
#include "member.h"  /* Inclusion complete pour acceder aux champs */
#include <stdlib.h>

static int next_loan_id = 1;

Loan* loan_create(Book* book, Member* member) {
    if (book == NULL || member == NULL) {
        return NULL;
    }

    if (!book_is_available(book)) {
        return NULL;  /* Livre non disponible */
    }

    if (!member_can_borrow(member)) {
        return NULL;  /* Membre ne peut pas emprunter */
    }

    Loan* loan = malloc(sizeof(Loan));
    if (loan == NULL) {
        return NULL;
    }

    loan->loan_id = next_loan_id++;
    loan->book = book;
    loan->member = member;
    loan->borrow_date = time(NULL);
    loan->due_date = loan->borrow_date + (14 * 24 * 60 * 60);  /* 14 jours */
    loan->is_returned = false;

    book->is_available = false;  /* Marquer comme emprunte */

    /* Enregistrer l'emprunt chez le membre */
    member->current_loans[member->loan_count] = loan;
    member->loan_count++;

    return loan;
}

void loan_destroy(Loan* loan) {
    free(loan);
}

bool loan_is_overdue(const Loan* loan) {
    if (loan == NULL || loan->is_returned) {
        return false;
    }

    time_t now = time(NULL);
    return now > loan->due_date;
}

void loan_return(Loan* loan) {
    if (loan == NULL) {
        return;
    }

    loan->is_returned = true;
    if (loan->book != NULL) {
        loan->book->is_available = true;
    }
}
