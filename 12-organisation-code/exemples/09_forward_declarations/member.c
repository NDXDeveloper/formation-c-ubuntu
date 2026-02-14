#include "member.h"
#include <stdlib.h>
#include <string.h>

Member* member_create(const char* name, int id) {
    Member* member = malloc(sizeof(Member));
    if (member == NULL) {
        return NULL;
    }

    strncpy(member->name, name, sizeof(member->name) - 1);
    member->name[sizeof(member->name) - 1] = '\0';
    member->member_id = id;
    member->loan_count = 0;

    for (int i = 0; i < MAX_LOANS; i++) {
        member->current_loans[i] = NULL;
    }

    return member;
}

void member_destroy(Member* member) {
    free(member);
}

bool member_can_borrow(const Member* member) {
    if (member == NULL) {
        return false;
    }
    return member->loan_count < MAX_LOANS;
}
