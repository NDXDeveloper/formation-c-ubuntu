/* ============================================================================
   Section 5.1 : Operateurs arithmetiques
   Description : Conversion Celsius vers Fahrenheit
   Fichier source : 01-operateurs-arithmetiques.md
   ============================================================================ */
#include <stdio.h>

int main(void) {
    float celsius = 25.0f;
    float fahrenheit = (celsius * 9.0f / 5.0f) + 32.0f;

    printf("%.1f°C = %.1f°F\n", celsius, fahrenheit);

    return 0;
}
