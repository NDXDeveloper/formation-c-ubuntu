/*
 * Section  : 01.6 - Build types
 * Description : Detection du mode de compilation via NDEBUG
 * Fichier source : 03_build_type_detect.c
 */

#include <stdio.h>
#include <assert.h>

int main(void) {
#ifdef NDEBUG
    printf("Build: RELEASE (optimise)\n");
#else
    printf("Build: DEBUG (non optimise)\n");
#endif

    int x = 42;
    assert(x > 0);
    (void)x;

    return 0;
}
