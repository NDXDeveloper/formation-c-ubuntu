/* ============================================================================
   Section 27.7 : Vectorisation et SIMD (SSE)
   Description : Addition de tableaux avec intrinsics SSE (4 floats a la fois)
                 vs version scalaire - utilise aligned_alloc pour alignement 16
   Fichier source : 07-vectorisation-simd.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <xmmintrin.h>  /* SSE */
#include <time.h>

#define SIZE 10000000
#define ITERATIONS 10

/* Version scalaire */
void add_scalar(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

/* Version SSE (4 floats a la fois) */
void add_sse(float *a, float *b, float *c, int n) {
    int i;

    /* Traiter 4 elements a la fois */
    for (i = 0; i <= n - 4; i += 4) {
        __m128 va = _mm_load_ps(&a[i]);
        __m128 vb = _mm_load_ps(&b[i]);
        __m128 vc = _mm_add_ps(va, vb);
        _mm_store_ps(&c[i], vc);
    }

    /* Reste */
    for (; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

int main(void) {
    /* Allouer avec alignement pour SSE (16 bytes) */
    float *a = aligned_alloc(16, SIZE * sizeof(float));
    float *b = aligned_alloc(16, SIZE * sizeof(float));
    float *c = aligned_alloc(16, SIZE * sizeof(float));
    if (!a || !b || !c) {
        fprintf(stderr, "Erreur allocation\n");
        return 1;
    }

    for (int i = 0; i < SIZE; i++) {
        a[i] = (float)i;
        b[i] = (float)i * 2.0f;
    }

    clock_t start, end;

    /* Test scalaire */
    start = clock();
    for (int k = 0; k < ITERATIONS; k++) {
        add_scalar(a, b, c, SIZE);
    }
    end = clock();
    printf("Scalaire : %.1f ms\n",
           (double)(end - start) * 1000.0 / CLOCKS_PER_SEC / ITERATIONS);

    /* Test SSE */
    start = clock();
    for (int k = 0; k < ITERATIONS; k++) {
        add_sse(a, b, c, SIZE);
    }
    end = clock();
    printf("SSE      : %.1f ms\n",
           (double)(end - start) * 1000.0 / CLOCKS_PER_SEC / ITERATIONS);

    free(a);
    free(b);
    free(c);
    return 0;
}
