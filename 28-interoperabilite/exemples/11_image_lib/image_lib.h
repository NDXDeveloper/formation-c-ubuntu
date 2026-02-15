/* ============================================================================
   Section 28.5 : extern "C" pour C++
   Description : API C pour bibliotheque d'images C++ (pointeur opaque/PIMPL)
   Fichier source : 05-extern-c.md
   ============================================================================ */

#ifndef IMAGE_LIB_H
#define IMAGE_LIB_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Pointeur opaque (cache l'implementation C++) */
typedef struct Image Image;

Image *image_create(int width, int height);
void image_destroy(Image *img);
void image_set_pixel(Image *img, int x, int y, uint32_t color);
uint32_t image_get_pixel(Image *img, int x, int y);
void image_apply_filter(Image *img, const char *filter_name);

#ifdef __cplusplus
}
#endif

#endif
