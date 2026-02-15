/* ============================================================================
   Section 18.2 : Creation et terminaison de threads
   Description : pthread_exit dans main - le thread continue apres main
   Fichier source : 02-creation-terminaison.md
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

void *long_task(void *arg) {
    (void)arg;
    for (int i = 1; i <= 5; i++) {
        printf("Thread travaille... %d/5\n", i);
        sleep(1);
    }
    printf("Thread termine\n");
    return NULL;
}

int main(void) {
    pthread_t tid;
    pthread_create(&tid, NULL, long_task, NULL);

    printf("Main se termine avec pthread_exit\n");
    pthread_exit(NULL);
}
