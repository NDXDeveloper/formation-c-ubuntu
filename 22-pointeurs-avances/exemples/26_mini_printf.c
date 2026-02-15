/* ============================================================================
   Section 22.5 : Fonctions variadiques
   Description : Implementation simplifiee de printf
   Fichier source : 05-fonctions-variadiques.md
   ============================================================================ */

#include <stdio.h>
#include <stdarg.h>

void mini_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    for (const char *p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;  // Passer le '%'
            switch (*p) {
                case 'd': {
                    int i = va_arg(args, int);
                    printf("%d", i);
                    break;
                }
                case 's': {
                    char *s = va_arg(args, char*);
                    printf("%s", s);
                    break;
                }
                case 'c': {
                    int c = va_arg(args, int);  // char promu en int
                    putchar(c);
                    break;
                }
                case '%': {
                    putchar('%');
                    break;
                }
                default:
                    putchar('%');
                    putchar(*p);
            }
        } else {
            putchar(*p);
        }
    }

    va_end(args);
}

int main(void) {
    mini_printf("Nombre : %d, Texte : %s, Char : %c\n", 42, "Hello", 'A');
    mini_printf("Pourcentage : 100%%\n");
    mini_printf("Melange : %s a %d ans et mesure %dcm\n", "Jean", 30, 175);

    return 0;
}
