#ifndef BOOK_H
#define BOOK_H

#include "types.h"
#include <stdbool.h>

struct Book {
    char title[128];
    char author[64];
    char isbn[20];
    bool is_available;
};

Book* book_create(const char* title, const char* author, const char* isbn);
void book_destroy(Book* book);
bool book_is_available(const Book* book);

#endif /* BOOK_H */
