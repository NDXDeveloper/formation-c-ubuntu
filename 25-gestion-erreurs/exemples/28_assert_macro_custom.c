/* ============================================================================
   Section 25.4 : Assertions
   Description : Macro personnalisee ASSERT_MSG avec infos de contexte
                 Crash intentionnel pedagogique
   Fichier source : 04-assertions.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>

#ifdef NDEBUG
    #define ASSERT_MSG(condition, message) ((void)0)
#else
    #define ASSERT_MSG(condition, message) \
        do { \
            if (!(condition)) { \
                fprintf(stderr, "Assertion echouee : %s\n", message); \
                fprintf(stderr, "  Fichier : %s\n", __FILE__); \
                fprintf(stderr, "  Ligne : %d\n", __LINE__); \
                fprintf(stderr, "  Fonction : %s\n", __func__); \
                abort(); \
            } \
        } while (0)
#endif

int main(void) {
    int age = 200;

    ASSERT_MSG(age >= 0 && age < 150,
               "L'age doit etre entre 0 et 150");

    return 0;
}
