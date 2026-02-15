/* ============================================================================
   Section 25.2 : La variable errno
   Description : Demonstration de strerror() pour convertir errno en message
   Fichier source : 02-errno.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(void) {
    printf("ENOENT : %s\n", strerror(ENOENT));
    printf("EACCES : %s\n", strerror(EACCES));
    printf("ENOMEM : %s\n", strerror(ENOMEM));

    return 0;
}
