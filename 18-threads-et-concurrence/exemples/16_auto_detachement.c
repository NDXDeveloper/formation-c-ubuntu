/* ============================================================================
   Section 18.4 : Threads detaches vs joinable
   Description : Auto-detachement - le thread se detache lui-meme
   Fichier source : 04-detaches-vs-joinable.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

void *tache_autonome(void *arg) {
    int result = pthread_detach(pthread_self());
    if (result != 0) {
        fprintf(stderr, "Erreur detachement\n");
        free(arg);
        return NULL;
    }

    int id = *(int *)arg;
    free(arg);

    printf("Thread %d : Je suis autonome !\n", id);

    for (int i = 1; i <= 3; i++) {
        printf("Thread %d : etape %d\n", id, i);
        sleep(1);
    }

    printf("Thread %d : Termine (auto-nettoyage)\n", id);
    return NULL;
}

int main(void) {
    for (int i = 1; i <= 3; i++) {
        int *id = malloc(sizeof(int));
        if (id == NULL) {
            fprintf(stderr, "Erreur malloc\n");
            continue;
        }
        *id = i;

        pthread_t tid;
        pthread_create(&tid, NULL, tache_autonome, id);
    }

    printf("Main : Tous les threads lances\n");

    sleep(5);

    printf("Main : Termine\n");
    return 0;
}
