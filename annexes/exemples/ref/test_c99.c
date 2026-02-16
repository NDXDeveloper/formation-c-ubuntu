// Ligne 121-129 : types C99
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

int main(void) {
    int64_t big_number = 9223372036854775807LL;
    bool is_active = true;
    printf("big_number = %ld, is_active = %d\n", big_number, is_active);
    return 0;
}
