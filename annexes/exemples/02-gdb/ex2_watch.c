// Exemple 2 : Watchpoint (ligne 960-979)
#include <stdio.h>

int global_counter = 0;

void mysterious_function() {
    global_counter = 99;  // Qui fait ça ?
}

void another_function() {
    global_counter++;
}

int main() {
    global_counter = 0;
    another_function();
    mysterious_function();
    printf("Counter: %d\n", global_counter);
    return 0;
}
