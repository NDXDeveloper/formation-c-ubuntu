/*
 * Section  : 01.6 - Build types
 * Description : Impact des build types sur les performances - calcul de Fibonacci
 * Fichier source : 02_fibonacci.c
 */

#include <stdio.h>
#include <assert.h>

int fibonacci(int n) {
    assert(n >= 0);
    if (n <= 1) return n;
    return fibonacci(n - 1) + fibonacci(n - 2);
}

int main(void) {
    printf("Fib(40) = %d\n", fibonacci(40));
    return 0;
}
