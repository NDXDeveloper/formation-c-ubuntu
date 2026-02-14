#include "book.h"
#include <stdlib.h>
#include <string.h>

Book* book_create(const char* title, const char* author, const char* isbn) {
    Book* book = malloc(sizeof(Book));
    if (book == NULL) {
        return NULL;
    }

    strncpy(book->title, title, sizeof(book->title) - 1);
    book->title[sizeof(book->title) - 1] = '\0';

    strncpy(book->author, author, sizeof(book->author) - 1);
    book->author[sizeof(book->author) - 1] = '\0';

    strncpy(book->isbn, isbn, sizeof(book->isbn) - 1);
    book->isbn[sizeof(book->isbn) - 1] = '\0';

    book->is_available = true;
    return book;
}

void book_destroy(Book* book) {
    free(book);
}

bool book_is_available(const Book* book) {
    if (book == NULL) {
        return false;
    }
    return book->is_available;
}
