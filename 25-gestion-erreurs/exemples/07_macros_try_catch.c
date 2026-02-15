/* ============================================================================
   Section 25.1 : Codes de retour vs exceptions simulees
   Description : Macros pour simuler try/catch avec setjmp/longjmp
   Fichier source : 01-codes-retour-vs-exceptions.md
   ============================================================================ */

#include <stdio.h>
#include <setjmp.h>

/* Definition des macros */
#define TRY do { jmp_buf ex_buf__; if (!setjmp(ex_buf__)) {
#define CATCH } else {
#define END_TRY } } while(0)
#define THROW longjmp(ex_buf__, 1)

int main(void) {
    printf("=== Test des macros TRY/CATCH ===\n\n");

    /* Test 1 : sans erreur */
    printf("Test 1 : code sans erreur\n");
    TRY {
        printf("  Code protege execute normalement\n");
        printf("  Fin normale\n");
    }
    CATCH {
        fprintf(stderr, "  Erreur capturee\n");
    }
    END_TRY;

    /* Test 2 : avec erreur */
    printf("\nTest 2 : code avec erreur\n");
    int erreur = 1;
    TRY {
        printf("  Code protege\n");

        if (erreur) {
            THROW;  /* "Lance" l'exception */
        }

        printf("  Cette ligne ne sera pas executee\n");
    }
    CATCH {
        fprintf(stderr, "  Erreur capturee !\n");
    }
    END_TRY;

    printf("\nProgramme termine normalement\n");

    return 0;
}
