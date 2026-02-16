// Exemple 3 : Tableau hors limites (ligne 1014-1027)
#include <stdio.h>

int main() {
    int array[5] = {1, 2, 3, 4, 5};
    int sum = 0;

    for (int i = 0; i <= 5; i++) {  // Bug : i <= 5 au lieu de i < 5
        sum += array[i];
    }

    printf("Sum: %d\n", sum);
    return 0;
}
