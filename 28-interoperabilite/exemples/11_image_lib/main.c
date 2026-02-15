/* ============================================================================
   Section 28.5 : extern "C" pour C++
   Description : Programme C utilisant la bibliotheque d'images C++ via API C
                 (pointeur opaque/PIMPL pattern)
   Fichier source : 05-extern-c.md
   ============================================================================ */

#include <stdio.h>
#include "image_lib.h"

int main(void) {
    printf("=== Bibliotheque C++ via API C (PIMPL) ===\n\n");

    /* Creer une image 800x600 */
    Image *img = image_create(800, 600);
    printf("  Image 800x600 creee\n");

    /* Definir un pixel rouge */
    image_set_pixel(img, 10, 20, 0xFF0000);
    uint32_t color = image_get_pixel(img, 10, 20);
    printf("  Pixel (10,20) = 0x%06X (rouge)\n", color);

    /* Appliquer le filtre grayscale */
    image_apply_filter(img, "grayscale");
    color = image_get_pixel(img, 10, 20);
    printf("  Apres grayscale : 0x%06X\n", color);

    /* Creer une autre image pour tester invert */
    Image *img2 = image_create(100, 100);
    image_set_pixel(img2, 0, 0, 0x00FF00);
    printf("\n  Image2 pixel (0,0) = 0x%06X (vert)\n",
           image_get_pixel(img2, 0, 0));

    image_apply_filter(img2, "invert");
    printf("  Apres invert : 0x%08X\n", image_get_pixel(img2, 0, 0));

    /* Pixel hors limites (doit retourner 0) */
    printf("\n  Pixel hors limites (-1,0) = 0x%06X\n",
           image_get_pixel(img, -1, 0));

    /* Nettoyage */
    image_destroy(img);
    image_destroy(img2);
    printf("\n  Images detruites\n");

    printf("\n=== Fin ===\n");
    return 0;
}
