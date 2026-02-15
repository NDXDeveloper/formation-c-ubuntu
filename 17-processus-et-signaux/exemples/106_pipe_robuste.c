/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Gestion d'erreurs robuste avec safe_read et safe_write
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

ssize_t safe_read(int fd, void *buf, size_t count) {
    ssize_t n;

    while ((n = read(fd, buf, count)) == -1) {
        if (errno == EINTR) {
            /* Interrompu par un signal, reessayer */
            continue;
        }
        /* Autre erreur */
        return -1;
    }

    return n;
}

ssize_t safe_write(int fd, const void *buf, size_t count) {
    size_t written = 0;

    while (written < count) {
        ssize_t n = write(fd, (const char *)buf + written, count - written);

        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }

        written += (size_t)n;
    }

    return (ssize_t)written;
}

int main(void) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    if (pid == 0) {
        /* ENFANT */
        close(pipefd[1]);

        char buffer[100];
        ssize_t n = safe_read(pipefd[0], buffer, sizeof(buffer) - 1);

        if (n == -1) {
            perror("read");
            close(pipefd[0]);
            return 1;
        }

        buffer[n] = '\0';
        printf("[Enfant] Recu: %s\n", buffer);

        close(pipefd[0]);
        return 0;
    }
    else {
        /* PARENT */
        close(pipefd[0]);

        const char *msg = "Message avec gestion d'erreurs robuste";

        if (safe_write(pipefd[1], msg, strlen(msg)) == -1) {
            perror("write");
            close(pipefd[1]);
            return 1;
        }

        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}
