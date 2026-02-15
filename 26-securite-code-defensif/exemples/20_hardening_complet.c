/* ============================================================================
   Section 26.8 : Compilation avec hardening flags
   Description : Programme compile avec hardening complet (PIE, RELRO, canary)
                 et verification des protections actives
   Fichier source : 08-hardening-flags.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

/* Fonction qui utilise un buffer local (declenchera le stack canary) */
static void traitement_securise(const char *input) {
    char buffer[100];
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';
    printf("  Traitement : %s\n", buffer);
}

/* Fonction qui utilise snprintf (protegee par FORTIFY_SOURCE) */
static void formatage_securise(const char *nom, int age) {
    char buffer[100];
    int result = snprintf(buffer, sizeof(buffer), "Nom: %s, Age: %d", nom, age);

    if (result < 0 || (size_t)result >= sizeof(buffer)) {
        fprintf(stderr, "  Erreur de formatage\n");
        return;
    }

    printf("  Formate : %s\n", buffer);
}

int main(void) {
    printf("=== Programme avec hardening complet ===\n\n");

    /* Informations sur le binaire */
    printf("--- Informations ---\n");
    printf("  Ce programme devrait etre compile avec :\n");
    printf("    -fstack-protector-strong  (stack canary)\n");
    printf("    -D_FORTIFY_SOURCE=2 -O2  (fonctions verifiees)\n");
    printf("    -fPIE -pie               (adresses aleatoires)\n");
    printf("    -Wl,-z,relro,-z,now      (GOT en lecture seule)\n");
    printf("    -Wl,-z,noexecstack       (pile non-executable)\n");

    /* Tester les fonctions protegees */
    printf("\n--- Traitements securises ---\n");
    traitement_securise("Hello World");
    traitement_securise("Un texte un peu plus long pour tester");
    formatage_securise("Alice", 30);
    formatage_securise("Bob", 25);

    /* Adresse de main (change avec PIE/ASLR) */
    printf("\n--- Verification ASLR/PIE ---\n");
    printf("  Adresse de main()    : %p\n", (void *)main);
    printf("  Adresse d'une locale : %p\n", (void *)&(int){42});

    char *heap = malloc(1);
    if (heap != NULL) {
        printf("  Adresse heap         : %p\n", (void *)heap);
        free(heap);
    }

    printf("  (Les adresses changent a chaque execution avec PIE/ASLR)\n");

    /* Verification avec checksec */
    printf("\n--- Verification du hardening ---\n");
    printf("  Utilisez 'checksec --file=./20_hardening_complet'\n");
    printf("  Ou : 'readelf -l ./20_hardening_complet | grep GNU_STACK'\n");
    printf("  Ou : 'file ./20_hardening_complet' (doit contenir 'pie')\n");

    printf("\n=== Fin du programme securise ===\n");
    return 0;
}
