/* ============================================================================
   Section 23.1 : Macros parametrees
   Description : Macros de conversion Celsius/Fahrenheit
   Fichier source : 01-macros-parametrees.md
   ============================================================================ */

#include <stdio.h>

// Conversion Celsius vers Fahrenheit
#define C_VERS_F(celsius) (((celsius) * 9.0 / 5.0) + 32.0)

// Conversion Fahrenheit vers Celsius
#define F_VERS_C(fahrenheit) (((fahrenheit) - 32.0) * 5.0 / 9.0)

int main(void) {
    double temp_c = 25.0;
    double temp_f = C_VERS_F(temp_c);

    printf("%.1f°C = %.1f°F\n", temp_c, temp_f);
    // Affiche : 25.0°C = 77.0°F

    return 0;
}
