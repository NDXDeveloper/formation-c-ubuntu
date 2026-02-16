#include <stdio.h>
int main(void) {
    // Statement expression (extension GNU)
    int x = ({ int y = 5; y + 1; });
    printf("%d\n", x);
    return 0;
}
