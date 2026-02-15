/* ============================================================================
   Section 28.5 : extern "C" pour C++
   Description : Implementation C++ de la bibliotheque d'images avec
                 classes internes (vector, string) cachees derriere API C
   Fichier source : 05-extern-c.md
   ============================================================================ */

#include "image_lib.h"
#include <vector>
#include <string>
#include <cstdint>

/* Classe C++ interne (pas visible depuis C) */
class ImageImpl {
public:
    int width, height;
    std::vector<uint32_t> pixels;

    ImageImpl(int w, int h) : width(w), height(h) {
        pixels.resize(static_cast<size_t>(w * h), 0);
    }

    void setPixel(int x, int y, uint32_t color) {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            pixels[static_cast<size_t>(y * width + x)] = color;
        }
    }

    uint32_t getPixel(int x, int y) const {
        if (x >= 0 && x < width && y >= 0 && y < height) {
            return pixels[static_cast<size_t>(y * width + x)];
        }
        return 0;
    }

    void applyFilter(const std::string &name) {
        if (name == "invert") {
            for (auto &pixel : pixels) {
                pixel = ~pixel;
            }
        } else if (name == "grayscale") {
            for (auto &pixel : pixels) {
                uint8_t r = (pixel >> 16) & 0xFF;
                uint8_t g = (pixel >> 8) & 0xFF;
                uint8_t b = pixel & 0xFF;
                uint8_t gray = static_cast<uint8_t>((r + g + b) / 3);
                pixel = (static_cast<uint32_t>(gray) << 16)
                      | (static_cast<uint32_t>(gray) << 8)
                      | gray;
            }
        }
    }
};

/* Implementation des fonctions C */
extern "C" {

Image *image_create(int width, int height) {
    return reinterpret_cast<Image *>(new ImageImpl(width, height));
}

void image_destroy(Image *img) {
    delete reinterpret_cast<ImageImpl *>(img);
}

void image_set_pixel(Image *img, int x, int y, uint32_t color) {
    reinterpret_cast<ImageImpl *>(img)->setPixel(x, y, color);
}

uint32_t image_get_pixel(Image *img, int x, int y) {
    return reinterpret_cast<ImageImpl *>(img)->getPixel(x, y);
}

void image_apply_filter(Image *img, const char *filter_name) {
    reinterpret_cast<ImageImpl *>(img)->applyFilter(filter_name);
}

}  /* extern "C" */
