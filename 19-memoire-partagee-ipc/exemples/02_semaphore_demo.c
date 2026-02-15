/* ============================================================================
   Section 19.2 : Semaphores System V
   Description : Mutex simple avec semaphore System V (fork parent/fils)
   Fichier source : 02-semaphores-system-v.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <errno.h>

/* Union requise pour semctl (non definie par le systeme) */
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
};

/* Fonction utilitaire : Attendre (P operation) */
void sem_wait_sysv(int semid, int semnum) {
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = -1;          /* Decrementer */
    op.sem_flg = SEM_UNDO;   /* Protection contre les crashes */

    if (semop(semid, &op, 1) == -1) {
        perror("sem_wait");
        exit(1);
    }
}

/* Fonction utilitaire : Signaler (V operation) */
void sem_post_sysv(int semid, int semnum) {
    struct sembuf op;
    op.sem_num = semnum;
    op.sem_op = +1;          /* Incrementer */
    op.sem_flg = SEM_UNDO;

    if (semop(semid, &op, 1) == -1) {
        perror("sem_post");
        exit(1);
    }
}

int main(void) {
    key_t key;
    int semid;
    union semun arg;

    /* 1. Generer une cle unique */
    key = ftok("/tmp", 'S');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    /* 2. Creer un ensemble de 1 semaphore */
    semid = semget(key, 1, IPC_CREAT | 0666);
    if (semid == -1) {
        perror("semget");
        exit(1);
    }

    /* 3. Initialiser le semaphore a 1 (mutex libre) */
    arg.val = 1;
    if (semctl(semid, 0, SETVAL, arg) == -1) {
        perror("semctl SETVAL");
        exit(1);
    }

    printf("Semaphore cree (ID: %d)\n", semid);
    fflush(stdout);

    /* 4. Creer un processus fils */
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) {
        /* ===== PROCESSUS FILS ===== */
        printf("[FILS] Tentative d'acces a la section critique...\n");

        sem_wait_sysv(semid, 0);  /* Attendre le semaphore */
        printf("[FILS] Entree en section critique\n");

        /* Simuler un traitement */
        for (int i = 0; i < 5; i++) {
            printf("[FILS] Travail... %d/5\n", i + 1);
            sleep(1);
        }

        printf("[FILS] Sortie de la section critique\n");
        sem_post_sysv(semid, 0);  /* Liberer le semaphore */

        exit(0);

    } else {
        /* ===== PROCESSUS PARENT ===== */
        sleep(1);  /* Laisser le fils demarrer */

        printf("[PARENT] Tentative d'acces a la section critique...\n");

        sem_wait_sysv(semid, 0);  /* Bloque tant que fils est dedans */
        printf("[PARENT] Entree en section critique\n");

        /* Simuler un traitement */
        for (int i = 0; i < 3; i++) {
            printf("[PARENT] Travail... %d/3\n", i + 1);
            sleep(1);
        }

        printf("[PARENT] Sortie de la section critique\n");
        sem_post_sysv(semid, 0);  /* Liberer le semaphore */

        /* Attendre la fin du fils */
        wait(NULL);

        /* 5. Supprimer le semaphore */
        printf("Suppression du semaphore\n");
        if (semctl(semid, 0, IPC_RMID) == -1) {
            perror("semctl IPC_RMID");
        }
    }

    return 0;
}
