#ifndef LOAN_H
#define LOAN_H

#include "types.h"
#include <stdbool.h>
#include <time.h>

struct Loan {
    int loan_id;
    Book* book;
    Member* member;
    time_t borrow_date;
    time_t due_date;
    bool is_returned;
};

Loan* loan_create(Book* book, Member* member);
void loan_destroy(Loan* loan);
bool loan_is_overdue(const Loan* loan);
void loan_return(Loan* loan);

#endif /* LOAN_H */
