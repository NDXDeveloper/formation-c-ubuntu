/* ============================================================================
   Section 19.4 : POSIX IPC vs System V IPC
   Description : Mutex simple avec semaphore POSIX
   Fichier source : 04-posix-vs-system-v.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <fcntl.h>

int main(void) {
    /* Creer et initialiser a 1 */
    sem_t *sem = sem_open("/my_mutex", O_CREAT, 0666, 1);
    if (sem == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    /* Acquerir */
    sem_wait(sem);

    printf("Section critique (POSIX)\n");

    /* Liberer */
    sem_post(sem);

    /* Nettoyer */
    sem_close(sem);
    sem_unlink("/my_mutex");

    return 0;
}
