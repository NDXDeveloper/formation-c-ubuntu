/* ============================================================================
   Section 19.4 : POSIX IPC vs System V IPC
   Description : Producteur-consommateur complet version System V
   Fichier source : 04-posix-vs-system-v.md
   ============================================================================ */
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 5

union semun {
    int val;
    unsigned short *array;
};

typedef struct {
    int buffer[BUFFER_SIZE];
    int in, out;
} shared_buffer_t;

void sem_wait_sysv(int semid, int num) {
    struct sembuf op = {(unsigned short)num, -1, 0};
    semop(semid, &op, 1);
}

void sem_post_sysv(int semid, int num) {
    struct sembuf op = {(unsigned short)num, +1, 0};
    semop(semid, &op, 1);
}

int main(void) {
    key_t key = ftok("/tmp", 'V');

    /* Creer ensemble de 3 semaphores (empty, full, mutex) */
    int semid = semget(key, 3, IPC_CREAT | 0666);
    union semun arg;
    unsigned short vals[3] = {BUFFER_SIZE, 0, 1};
    arg.array = vals;
    semctl(semid, 0, SETALL, arg);

    /* Creer memoire partagee */
    int shmid = shmget(key, sizeof(shared_buffer_t), IPC_CREAT | 0666);
    shared_buffer_t *sb = shmat(shmid, NULL, 0);
    sb->in = sb->out = 0;

    fflush(stdout);
    pid_t pid = fork();

    if (pid == 0) {
        /* PRODUCTEUR */
        for (int i = 0; i < 8; i++) {
            int item = (i + 1) * 10;
            sem_wait_sysv(semid, 0);  /* empty */
            sem_wait_sysv(semid, 2);  /* mutex */
            sb->buffer[sb->in] = item;
            printf("[SysV PROD] %d -> index %d\n", item, sb->in);
            sb->in = (sb->in + 1) % BUFFER_SIZE;
            sem_post_sysv(semid, 2);  /* mutex */
            sem_post_sysv(semid, 1);  /* full */
            usleep(100000);
        }
        exit(0);
    } else {
        /* CONSOMMATEUR */
        for (int i = 0; i < 8; i++) {
            sem_wait_sysv(semid, 1);  /* full */
            sem_wait_sysv(semid, 2);  /* mutex */
            int item = sb->buffer[sb->out];
            printf("[SysV CONS] %d <- index %d\n", item, sb->out);
            sb->out = (sb->out + 1) % BUFFER_SIZE;
            sem_post_sysv(semid, 2);  /* mutex */
            sem_post_sysv(semid, 0);  /* empty */
            usleep(200000);
        }
        wait(NULL);

        /* Nettoyage */
        shmdt(sb);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);
        printf("Termine (System V).\n");
    }

    return 0;
}
