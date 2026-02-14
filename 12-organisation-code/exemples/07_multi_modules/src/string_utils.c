#include "string_utils.h"
#include <string.h>

void inverser_chaine(char* str) {
    size_t n = strlen(str);
    for (size_t i = 0; i < n / 2; i++) {
        char temp = str[i];
        str[i] = str[n - i - 1];
        str[n - i - 1] = temp;
    }
}

size_t longueur_chaine(const char* str) {
    return strlen(str);
}
