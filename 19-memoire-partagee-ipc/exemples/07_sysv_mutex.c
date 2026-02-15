/* ============================================================================
   Section 19.4 : POSIX IPC vs System V IPC
   Description : Mutex simple avec semaphore System V
   Fichier source : 04-posix-vs-system-v.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>

union semun {
    int val;
};

int main(void) {
    /* Creer */
    key_t key = ftok("/tmp", 'M');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }
    int semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    /* Initialiser a 1 (mutex libre) */
    union semun arg;
    arg.val = 1;
    semctl(semid, 0, SETVAL, arg);

    /* Acquerir (P) */
    struct sembuf lock = {0, -1, SEM_UNDO};
    semop(semid, &lock, 1);

    printf("Section critique (System V)\n");

    /* Liberer (V) */
    struct sembuf unlock = {0, +1, SEM_UNDO};
    semop(semid, &unlock, 1);

    /* Supprimer */
    semctl(semid, 0, IPC_RMID);

    return 0;
}
