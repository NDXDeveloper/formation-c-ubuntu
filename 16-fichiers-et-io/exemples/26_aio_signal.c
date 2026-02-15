/* ============================================================================
   Section 16.8 : I/O asynchrone (AIO)
   Description : Lecture asynchrone POSIX AIO avec notification par signal
   Fichier source : 08-io-asynchrone.md
   ============================================================================ */

/* Compiler avec : gcc ... -lrt */

#define _XOPEN_SOURCE 600
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

static volatile sig_atomic_t aio_done = 0;

static void aio_completion_handler(int signo, siginfo_t *info, void *context) {
    (void)signo;
    (void)context;

    /* Recuperer l'aiocb depuis le signal */
    struct aiocb *cb = (struct aiocb *)info->si_value.sival_ptr;

    ssize_t bytes = aio_return(cb);
    printf("Signal recu : %zd octets lus\n", bytes);

    aio_done = 1;
}

int main(void) {
    /* Configurer le handler de signal */
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = aio_completion_handler;
    sigaction(SIGUSR1, &sa, NULL);

    /* Creer un fichier de test */
    int fd_w = open("/tmp/aio_signal.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_w == -1) {
        perror("open write");
        return 1;
    }
    const char *contenu = "Contenu lu avec notification par signal SIGUSR1!\n";
    write(fd_w, contenu, strlen(contenu));
    close(fd_w);

    /* Ouvrir pour lecture */
    int fd = open("/tmp/aio_signal.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct aiocb cb;
    memset(&cb, 0, sizeof(cb));

    char buffer[1024];
    cb.aio_fildes = fd;
    cb.aio_buf = buffer;
    cb.aio_nbytes = sizeof(buffer) - 1;
    cb.aio_offset = 0;

    /* Configurer la notification par signal */
    cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    cb.aio_sigevent.sigev_signo = SIGUSR1;
    cb.aio_sigevent.sigev_value.sival_ptr = &cb;

    /* Lancer la lecture */
    if (aio_read(&cb) == -1) {
        perror("aio_read");
        close(fd);
        return 1;
    }

    printf("Lecture lancee, en attente du signal...\n");

    /* Faire autre chose */
    int tours = 0;
    while (!aio_done) {
        tours++;
        usleep(10000);  /* 10ms */
        if (tours > 500) {  /* Securite: max 5s */
            printf("Timeout!\n");
            break;
        }
    }

    if (aio_done) {
        buffer[strlen(buffer)] = '\0';
        printf("Donnees: %s", (char *)buffer);
    }

    close(fd);

    /* Nettoyage */
    unlink("/tmp/aio_signal.txt");

    return 0;
}
