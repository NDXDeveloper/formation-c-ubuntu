/* ============================================================================
   Section 27.7 : Vectorisation et SIMD (AVX)
   Description : Addition de tableaux avec intrinsics AVX (8 floats a la fois)
                 utilise aligned_alloc pour alignement 32 bytes
   Fichier source : 07-vectorisation-simd.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <immintrin.h>  /* AVX */
#include <time.h>

#define SIZE 10000000
#define ITERATIONS 10

/* Version AVX (8 floats a la fois) */
void add_avx(float *a, float *b, float *c, int n) {
    int i;

    /* Traiter 8 elements a la fois avec AVX */
    for (i = 0; i <= n - 8; i += 8) {
        __m256 va = _mm256_load_ps(&a[i]);
        __m256 vb = _mm256_load_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);
        _mm256_store_ps(&c[i], vc);
    }

    /* Reste */
    for (; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

int main(void) {
    /* Allouer avec alignement pour AVX (32 bytes) */
    float *a = aligned_alloc(32, SIZE * sizeof(float));
    float *b = aligned_alloc(32, SIZE * sizeof(float));
    float *c = aligned_alloc(32, SIZE * sizeof(float));
    if (!a || !b || !c) {
        fprintf(stderr, "Erreur allocation\n");
        return 1;
    }

    for (int i = 0; i < SIZE; i++) {
        a[i] = (float)i;
        b[i] = (float)i * 2.0f;
    }

    clock_t start = clock();
    for (int k = 0; k < ITERATIONS; k++) {
        add_avx(a, b, c, SIZE);
    }
    clock_t end = clock();

    printf("AVX : %.1f ms (moyenne sur %d iterations)\n",
           (double)(end - start) * 1000.0 / CLOCKS_PER_SEC / ITERATIONS,
           ITERATIONS);
    printf("Verification : c[42] = %.1f (attendu: %.1f)\n",
           (double)c[42], 42.0 + 42.0 * 2.0);

    free(a);
    free(b);
    free(c);
    return 0;
}
