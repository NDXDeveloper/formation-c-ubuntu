/* ============================================================================
   Section 4.6 : Nouveautes C23
   Description : Demo des nouveautes C23 (bool, nullptr, binaire, typeof, attributs)
   Fichier source : 06-nouveautes-c23.md
   ============================================================================ */
#include <stdio.h>

/* Fonction avec attribut [[nodiscard]] */
[[nodiscard]] bool valider_age(int age) {
    return age >= 18 && age <= 120;
}

/* Fonction avec [[maybe_unused]] */
void debug_info([[maybe_unused]] const char* message) {
    #ifdef DEBUG
        printf("DEBUG: %s\n", message);
    #endif
}

int main(void) {
    printf("=== Démo des nouveautés C23 ===\n\n");

    /* 1. Mots-cles bool, true, false (sans <stdbool.h>) */
    bool est_majeur = true;
    printf("1. Booléens natifs: %s\n", est_majeur ? "vrai" : "faux");

    /* 2. nullptr pour les pointeurs */
    int* ptr = nullptr;
    if (ptr == nullptr) {
        printf("2. Pointeur est null\n");
    }

    /* 3. Litteraux binaires */
    unsigned char flags = 0b10101010;
    printf("3. Flags binaires: 0x%02X\n", flags);

    /* 4. Separateurs de chiffres */
    long population = 67000000L;
    printf("4. Population: %ld habitants\n", population);

    /* 5. Fonction avec [[nodiscard]] */
    if (valider_age(25)) {
        printf("5. Âge valide\n");
    }

    /* 6. typeof */
    int x = 42;
    typeof(x) y = 100;
    printf("6. typeof: x=%d, y=%d\n", x, y);

    debug_info("Fin du programme");

    return 0;
}
