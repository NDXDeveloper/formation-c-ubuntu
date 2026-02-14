#ifndef MEMBER_H
#define MEMBER_H

#include "types.h"
#include <stdbool.h>

#define MAX_LOANS 5

struct Member {
    char name[64];
    int member_id;
    Loan* current_loans[MAX_LOANS];
    int loan_count;
};

Member* member_create(const char* name, int id);
void member_destroy(Member* member);
bool member_can_borrow(const Member* member);

#endif /* MEMBER_H */
