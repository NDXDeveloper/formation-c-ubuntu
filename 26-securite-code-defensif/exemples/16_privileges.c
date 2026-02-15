/* ============================================================================
   Section 26.6 : Principe du moindre privilege
   Description : Affichage des privileges et verification de permissions fichiers
   Fichier source : 06-moindre-privilege.md
   ============================================================================ */

#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

static void afficher_privileges(void) {
    printf("UID reel      : %d\n", getuid());
    printf("UID effectif  : %d\n", geteuid());
    printf("GID reel      : %d\n", getgid());
    printf("GID effectif  : %d\n", getegid());
}

static bool verifier_permissions_fichier(const char *chemin) {
    struct stat st;

    if (stat(chemin, &st) != 0) {
        perror("  stat");
        return false;
    }

    /* Verifier que le fichier appartient a l'utilisateur */
    if (st.st_uid != getuid()) {
        fprintf(stderr, "  Le fichier n'appartient pas a l'utilisateur courant\n");
        return false;
    }

    /* Verifier les permissions */
    mode_t mode = st.st_mode & 0777;
    printf("  Proprietaire : UID %d\n", st.st_uid);
    printf("  Permissions  : %o\n", mode);

    /* Verifier que seul le proprietaire peut lire/ecrire */
    if (mode & 0077) {  /* Bits de groupe ou autres actifs */
        fprintf(stderr, "  ATTENTION : permissions trop permissives !\n");
        fprintf(stderr, "  Recommande : 0600 ou 0400\n");
        return false;
    }

    printf("  Permissions OK (restrictives)\n");
    return true;
}

int main(void) {
    printf("=== Principe du moindre privilege ===\n\n");

    /* Afficher les privileges actuels */
    printf("--- Privileges du processus ---\n");
    afficher_privileges();

    /* Verifier si on est root */
    printf("\n--- Verification root ---\n");
    if (geteuid() == 0) {
        printf("  ATTENTION : Ce programme tourne en root !\n");
        printf("  Recommandation : ne pas lancer en root si pas necessaire\n");
    } else {
        printf("  OK : Le programme tourne sans privileges root (UID %d)\n", geteuid());
    }

    /* Creer un fichier de test avec bonnes permissions */
    printf("\n--- Verification de permissions fichier ---\n");
    const char *fichier_test = "/tmp/test_perms_ch26.txt";

    /* Fichier avec permissions restrictives (0600) */
    FILE *f = fopen(fichier_test, "w");
    if (f != NULL) {
        fprintf(f, "donnees sensibles\n");
        fclose(f);
        chmod(fichier_test, 0600);
        printf("  Fichier %s (permissions 0600) :\n", fichier_test);
        verifier_permissions_fichier(fichier_test);
    }

    /* Changer en permissions trop larges */
    printf("\n  Fichier avec permissions 0644 (trop larges) :\n");
    chmod(fichier_test, 0644);
    verifier_permissions_fichier(fichier_test);

    /* Nettoyage */
    remove(fichier_test);

    /* Regles du moindre privilege */
    printf("\n--- Regles du moindre privilege ---\n");
    printf("  1. Ne jamais executer en root sauf necessite absolue\n");
    printf("  2. Abandonner les privileges des que possible\n");
    printf("  3. Ordre : setgid() puis setuid()\n");
    printf("  4. Verifier : setuid(0) doit echouer apres abandon\n");
    printf("  5. Fichiers : utiliser 0600 pour les donnees sensibles\n");
    printf("  6. Preferer capabilities a setuid root\n");

    printf("\n=== Fin de la demonstration ===\n");
    return 0;
}
