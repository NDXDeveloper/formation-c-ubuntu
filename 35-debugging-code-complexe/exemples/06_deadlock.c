/* ============================================================================
   Section 35.4 : Debugging des deadlocks
   Description : Bug intentionnel - deadlock classique (2 threads, 2 mutex)
   Fichier source : 04-debugging-deadlocks.md
   ============================================================================ */
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

pthread_mutex_t mutex_A = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_B = PTHREAD_MUTEX_INITIALIZER;

void *thread1_func(void *arg) {
    (void)arg;
    printf("Thread 1: Locking A...\n");
    pthread_mutex_lock(&mutex_A);
    sleep(1);  /* Simuler du travail */

    printf("Thread 1: Locking B...\n");
    pthread_mutex_lock(&mutex_B);  /* Bloque ici si thread2 a deja B */

    printf("Thread 1: Got both locks!\n");

    pthread_mutex_unlock(&mutex_B);
    pthread_mutex_unlock(&mutex_A);
    return NULL;
}

void *thread2_func(void *arg) {
    (void)arg;
    printf("Thread 2: Locking B...\n");
    pthread_mutex_lock(&mutex_B);
    sleep(1);  /* Simuler du travail */

    printf("Thread 2: Locking A...\n");
    pthread_mutex_lock(&mutex_A);  /* Bloque ici si thread1 a deja A */

    printf("Thread 2: Got both locks!\n");

    pthread_mutex_unlock(&mutex_A);
    pthread_mutex_unlock(&mutex_B);
    return NULL;
}

/* Timeout automatique pour eviter un blocage permanent */
void alarm_handler(int sig) {
    (void)sig;
    printf("\n[TIMEOUT] Deadlock detecte ! Le programme est bloque.\n");
    _exit(1);
}

int main(void) {
    setvbuf(stdout, NULL, _IONBF, 0);
    signal(SIGALRM, alarm_handler);
    alarm(5);  /* Timeout de 5 secondes */

    pthread_t t1, t2;

    pthread_create(&t1, NULL, thread1_func, NULL);
    pthread_create(&t2, NULL, thread2_func, NULL);

    pthread_join(t1, NULL);  /* Bloque indefiniment (deadlock) */
    pthread_join(t2, NULL);

    return 0;
}
