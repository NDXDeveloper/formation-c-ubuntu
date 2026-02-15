/* ============================================================================
   Section 19.2 : Semaphores System V
   Description : Producteur-consommateur complet avec semaphores et shm System V
   Fichier source : 02-semaphores-system-v.md
   ============================================================================ */
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>

#define BUFFER_SIZE 5
#define SEM_EMPTY 0    /* Places vides */
#define SEM_FULL 1     /* Elements disponibles */
#define SEM_MUTEX 2    /* Mutex pour acces buffer */

union semun {
    int val;
    unsigned short *array;
};

typedef struct {
    int buffer[BUFFER_SIZE];
    int in;   /* Index d'insertion */
    int out;  /* Index d'extraction */
} shared_buffer_t;

void sem_wait_sysv(int semid, int semnum) {
    struct sembuf op = {(unsigned short)semnum, -1, 0};
    semop(semid, &op, 1);
}

void sem_post_sysv(int semid, int semnum) {
    struct sembuf op = {(unsigned short)semnum, +1, 0};
    semop(semid, &op, 1);
}

int main(void) {
    key_t key = ftok("/tmp", 'P');

    /* Creer ensemble de 3 semaphores */
    int semid = semget(key, 3, IPC_CREAT | 0666);

    /* Initialiser */
    union semun arg;
    unsigned short init_vals[3] = {BUFFER_SIZE, 0, 1};  /* empty=5, full=0, mutex=1 */
    arg.array = init_vals;
    semctl(semid, 0, SETALL, arg);

    /* Creer memoire partagee pour le buffer */
    int shmid = shmget(key, sizeof(shared_buffer_t), IPC_CREAT | 0666);
    shared_buffer_t *sb = shmat(shmid, NULL, 0);
    sb->in = 0;
    sb->out = 0;

    fflush(stdout);
    pid_t pid = fork();

    if (pid == 0) {
        /* ===== PRODUCTEUR ===== */
        for (int i = 0; i < 10; i++) {
            int item = i * 10;

            sem_wait_sysv(semid, SEM_EMPTY);  /* Attendre place vide */
            sem_wait_sysv(semid, SEM_MUTEX);  /* Exclusion mutuelle */

            /* Section critique */
            sb->buffer[sb->in] = item;
            printf("[PROD] Produit : %d a l'index %d\n", item, sb->in);
            sb->in = (sb->in + 1) % BUFFER_SIZE;

            sem_post_sysv(semid, SEM_MUTEX);  /* Liberer mutex */
            sem_post_sysv(semid, SEM_FULL);   /* Signaler element disponible */

            usleep(200000);  /* 200ms */
        }
        exit(0);

    } else {
        /* ===== CONSOMMATEUR ===== */
        for (int i = 0; i < 10; i++) {
            sem_wait_sysv(semid, SEM_FULL);   /* Attendre element */
            sem_wait_sysv(semid, SEM_MUTEX);  /* Exclusion mutuelle */

            /* Section critique */
            int item = sb->buffer[sb->out];
            printf("[CONS] Consomme : %d de l'index %d\n", item, sb->out);
            sb->out = (sb->out + 1) % BUFFER_SIZE;

            sem_post_sysv(semid, SEM_MUTEX);  /* Liberer mutex */
            sem_post_sysv(semid, SEM_EMPTY);  /* Signaler place vide */

            usleep(400000);  /* 400ms */
        }

        wait(NULL);

        /* Nettoyage */
        shmdt(sb);
        shmctl(shmid, IPC_RMID, NULL);
        semctl(semid, 0, IPC_RMID);

        printf("Termine.\n");
    }

    return 0;
}
