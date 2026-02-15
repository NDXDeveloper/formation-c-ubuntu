/* ============================================================================
   Section 18.1 : Introduction a pthread
   Description : Passage de parametre et recuperation de valeur de retour
   Fichier source : 01-introduction-pthread.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *print_number(void *arg) {
    int num = *(int *)arg;
    printf("Nombre recu : %d\n", num);
    return NULL;
}

void *compute_sum(void *arg) {
    (void)arg;
    int *result = malloc(sizeof(int));
    if (result == NULL) {
        return NULL;
    }
    *result = 10 + 20;
    return result;
}

int main(void) {
    pthread_t tid1, tid2;
    int number = 42;
    void *ret;

    /* Passage d'un parametre */
    pthread_create(&tid1, NULL, print_number, &number);
    pthread_join(tid1, NULL);

    /* Recuperation d'une valeur de retour */
    pthread_create(&tid2, NULL, compute_sum, NULL);
    pthread_join(tid2, &ret);

    if (ret != NULL) {
        int sum = *(int *)ret;
        printf("Somme calculee : %d\n", sum);
        free(ret);
    }

    return 0;
}
