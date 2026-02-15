/* ============================================================================
   Section 19.4 : POSIX IPC vs System V IPC
   Description : Compteur partage avec System V shared memory
   Fichier source : 04-posix-vs-system-v.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int main(void) {
    /* Creer segment */
    key_t key = ftok("/tmp", 'C');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }
    int shmid = shmget(key, sizeof(int), IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }

    /* Attacher */
    int *counter = (int *)shmat(shmid, NULL, 0);
    if (counter == (int *)-1) {
        perror("shmat");
        exit(1);
    }

    /* Utiliser */
    *counter = 42;
    printf("Compteur (System V) : %d\n", *counter);

    /* Detacher */
    shmdt(counter);

    /* Supprimer */
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
