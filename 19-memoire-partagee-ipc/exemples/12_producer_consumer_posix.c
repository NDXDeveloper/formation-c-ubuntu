/* ============================================================================
   Section 19.4 : POSIX IPC vs System V IPC
   Description : Producteur-consommateur complet version POSIX
   Fichier source : 04-posix-vs-system-v.md
   ============================================================================ */
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <unistd.h>

#define BUFFER_SIZE 5

typedef struct {
    int buffer[BUFFER_SIZE];
    int in, out;
} shared_buffer_t;

int main(void) {
    /* Creer semaphores */
    sem_t *empty = sem_open("/sem_empty", O_CREAT, 0666, BUFFER_SIZE);
    sem_t *full = sem_open("/sem_full", O_CREAT, 0666, 0);
    sem_t *mutex = sem_open("/sem_mutex", O_CREAT, 0666, 1);

    if (empty == SEM_FAILED || full == SEM_FAILED || mutex == SEM_FAILED) {
        perror("sem_open");
        exit(1);
    }

    /* Creer memoire partagee */
    int fd = shm_open("/my_buffer", O_CREAT | O_RDWR, 0666);
    if (fd == -1) {
        perror("shm_open");
        exit(1);
    }
    ftruncate(fd, sizeof(shared_buffer_t));
    shared_buffer_t *sb = mmap(NULL, sizeof(shared_buffer_t),
                                PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sb == MAP_FAILED) {
        perror("mmap");
        exit(1);
    }
    close(fd);
    sb->in = sb->out = 0;

    fflush(stdout);
    pid_t pid = fork();

    if (pid == 0) {
        /* PRODUCTEUR */
        for (int i = 0; i < 8; i++) {
            int item = (i + 1) * 10;
            sem_wait(empty);
            sem_wait(mutex);
            sb->buffer[sb->in] = item;
            printf("[POSIX PROD] %d -> index %d\n", item, sb->in);
            sb->in = (sb->in + 1) % BUFFER_SIZE;
            sem_post(mutex);
            sem_post(full);
            usleep(100000);
        }
        exit(0);
    } else {
        /* CONSOMMATEUR */
        for (int i = 0; i < 8; i++) {
            sem_wait(full);
            sem_wait(mutex);
            int item = sb->buffer[sb->out];
            printf("[POSIX CONS] %d <- index %d\n", item, sb->out);
            sb->out = (sb->out + 1) % BUFFER_SIZE;
            sem_post(mutex);
            sem_post(empty);
            usleep(200000);
        }
        wait(NULL);

        /* Nettoyage */
        munmap(sb, sizeof(shared_buffer_t));
        shm_unlink("/my_buffer");
        sem_close(empty);
        sem_close(full);
        sem_close(mutex);
        sem_unlink("/sem_empty");
        sem_unlink("/sem_full");
        sem_unlink("/sem_mutex");
        printf("Termine (POSIX).\n");
    }

    return 0;
}
