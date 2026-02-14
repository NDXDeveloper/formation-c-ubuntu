#include <stdio.h>
#include "math.h"  /* Inclut les declarations */

int main(void) {
    int x = 10, y = 5;

    printf("%d + %d = %d\n", x, y, addition(x, y));
    printf("%d * %d = %d\n", x, y, multiplication(x, y));

    return 0;
}
