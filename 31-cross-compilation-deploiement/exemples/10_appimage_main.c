/* ============================================================================
   Section 31.7 : AppImage
   Description : Programme pour demonstration packaging AppImage
   Fichier source : 07-appimage.md
   ============================================================================ */
#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("=== Mon Application ===\n");
    printf("Version: 1.0.0\n");

    if (argc > 1) {
        printf("Arguments recus:\n");
        for (int i = 1; i < argc; i++) {
            printf("  %d: %s\n", i, argv[i]);
        }
    }

    printf("Application executee avec succes!\n");
    return 0;
}
