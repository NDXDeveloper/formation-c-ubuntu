🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 16.8 I/O asynchrone (AIO)

## Introduction

L'**I/O asynchrone** (Asynchronous I/O ou AIO) permet à un programme de **lancer des opérations d'entrée/sortie sans attendre** qu'elles se terminent. Le programme continue son exécution et est notifié plus tard quand l'opération est complète.

> **💡 Analogie :** Imaginez que vous êtes dans un restaurant :
> - **I/O synchrone (bloquant)** : Vous commandez, puis vous attendez debout devant le comptoir jusqu'à ce que votre plat soit prêt. Vous ne pouvez rien faire d'autre.
> - **I/O multiplexing** : Vous prenez un bipeur qui vibre quand votre commande est prête. En attendant, vous pouvez vous asseoir, mais vous devez régulièrement vérifier le bipeur.
> - **I/O asynchrone** : Vous donnez votre numéro de table. Le serveur viendra vous apporter votre plat quand il sera prêt. Vous êtes totalement libre de faire autre chose en attendant.

## Comprendre les différents modèles d'I/O

### Comparaison visuelle

```
┌────────────────────────────────────────────────────┐
│          I/O SYNCHRONE (BLOQUANT)                  │
├────────────────────────────────────────────────────┤
│                                                    │
│  Programme: read() ──────────────┐                 │
│                                   ▼                │
│  [BLOQUÉ EN ATTENTE] ████████████████              │
│                                   ▼                │
│  Données reçues ──────────────────┘                │
│  Programme continue                                │
│                                                    │
│  Temps perdu: 100%                                 │
└────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────┐
│          I/O MULTIPLEXING (select/poll/epoll)      │
├────────────────────────────────────────────────────┤
│                                                    │
│  Programme: select() ─────────────┐                │
│                                   ▼                │
│  [BLOQUÉ MAIS SURVEILLE] ███████████               │
│  PLUSIEURS descripteurs           ▼                │
│  Un fd prêt ──────────────────────┘                │
│  read() (non-bloquant)                             │
│  Programme continue                                │
│                                                    │
│  Efficace avec plusieurs fd                        │
└────────────────────────────────────────────────────┘

┌────────────────────────────────────────────────────┐
│          I/O ASYNCHRONE (AIO)                      │
├────────────────────────────────────────────────────┤
│                                                    │
│  Programme: aio_read() ┐                           │
│                        │                           │
│  Continue travail ████ │ ← Programme actif         │
│  Autre tâche █████████ │                           │
│  Calculs █████████████ │                           │
│                        │                           │
│  [Données arrivent] ───┘                           │
│  Callback/Signal notifie                           │
│  Traite les données                                │
│                                                    │
│  Temps utilisé: ~100%                              │
└────────────────────────────────────────────────────┘
```

### Tableau comparatif des modèles

| Aspect | I/O Bloquant | I/O Multiplexing | I/O Asynchrone |
|--------|--------------|------------------|----------------|
| **Blocage** | ✅ Total pendant read/write | ⚠️ Bloque sur select/poll | ❌ Aucun |
| **CPU utilisé** | ❌ 0% pendant attente | ⚠️ Faible pendant attente | ✅ ~100% |
| **Complexité** | ✅ Simple | ⚠️ Moyenne | ⚠️ Élevée |
| **Gestion multi-fd** | ❌ Difficile | ✅ Excellente | ✅ Excellente |
| **Notification** | N/A (bloque) | Polling actif | Callback/Signal |
| **Performance** | ⚠️ Faible | ✅ Bonne | ✅ Excellente |
| **Cas d'usage** | Scripts simples | Serveurs réseau | I/O disque intensif |

## POSIX AIO : L'API standard

### Présentation

POSIX AIO est l'API standard pour l'I/O asynchrone définie par POSIX.1-2001.

**Caractéristiques :**
- ✅ Standard POSIX (portable)
- ⚠️ Implémentation variable selon l'OS
- ⚠️ Performance limitée sur certains systèmes
- ✅ Supporte fichiers et sockets (sur certains OS)

**Fonctions principales :**

```c
#include <aio.h>

int aio_read(struct aiocb *aiocbp);    // Lecture asynchrone  
int aio_write(struct aiocb *aiocbp);   // Écriture asynchrone  
int aio_error(const struct aiocb *aiocbp);   // Vérifier le statut  
ssize_t aio_return(struct aiocb *aiocbp);    // Récupérer le résultat  
int aio_cancel(int fd, struct aiocb *aiocbp); // Annuler une opération  
int aio_suspend(const struct aiocb * const aiocb_list[],  
                int nitems, const struct timespec *timeout); // Attendre
```

### La structure aiocb (AIO Control Block)

La structure `aiocb` décrit une opération d'I/O asynchrone :

```c
struct aiocb {
    int             aio_fildes;     // Descripteur de fichier
    off_t           aio_offset;     // Offset dans le fichier
    volatile void  *aio_buf;        // Buffer pour les données
    size_t          aio_nbytes;     // Nombre d'octets
    int             aio_reqprio;    // Priorité de la requête
    struct sigevent aio_sigevent;   // Notification
    int             aio_lio_opcode; // Opération (lecture/écriture)
};
```

### Méthodes de notification

POSIX AIO propose **trois méthodes** pour être notifié de la fin d'une opération :

#### 1. Polling (vérification active)

Vérifier périodiquement avec `aio_error()` :

```c
#define _XOPEN_SOURCE 600
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(void) {
    int fd = open("fichier.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Préparer la structure aiocb
    struct aiocb cb;
    memset(&cb, 0, sizeof(cb));

    char buffer[1024];
    cb.aio_fildes = fd;
    cb.aio_buf = buffer;
    cb.aio_nbytes = sizeof(buffer);
    cb.aio_offset = 0;

    // Lancer la lecture asynchrone
    if (aio_read(&cb) == -1) {
        perror("aio_read");
        return 1;
    }

    printf("Lecture lancée, programme continue...\n");

    // Faire autre chose pendant que la lecture se fait
    for (int i = 0; i < 1000000; i++) {
        // Calculs...
    }

    // Vérifier si la lecture est terminée (polling)
    int status;
    while ((status = aio_error(&cb)) == EINPROGRESS) {
        printf("Lecture en cours...\n");
        usleep(100000);  // Attendre 100ms
    }

    if (status == 0) {
        // Succès : récupérer le résultat
        ssize_t bytes = aio_return(&cb);
        printf("Lu %zd octets\n", bytes);
        buffer[bytes] = '\0';
        printf("Contenu: %s\n", buffer);
    } else {
        printf("Erreur lors de la lecture\n");
    }

    close(fd);
    return 0;
}
```

**Compilation :**
```bash
gcc -o aio_demo aio_demo.c -lrt
```

**Avantages :** Simple  
**Inconvénients :** Consomme du CPU, pas vraiment asynchrone  

#### 2. Signal

Recevoir un signal quand l'opération est terminée :

```c
#define _POSIX_C_SOURCE 200809L
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>

volatile sig_atomic_t aio_done = 0;

void aio_completion_handler(int signo, siginfo_t *info, void *context) {
    (void)signo;
    (void)context;

    // Récupérer l'aiocb depuis le signal
    struct aiocb *cb = (struct aiocb *)info->si_value.sival_ptr;

    ssize_t bytes = aio_return(cb);
    printf("Signal reçu : %zd octets lus\n", bytes);

    aio_done = 1;
}

int main(void) {
    // Configurer le handler de signal
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = aio_completion_handler;
    sigaction(SIGUSR1, &sa, NULL);

    int fd = open("fichier.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct aiocb cb;
    memset(&cb, 0, sizeof(cb));

    char buffer[1024];
    cb.aio_fildes = fd;
    cb.aio_buf = buffer;
    cb.aio_nbytes = sizeof(buffer);
    cb.aio_offset = 0;

    // Configurer la notification par signal
    cb.aio_sigevent.sigev_notify = SIGEV_SIGNAL;
    cb.aio_sigevent.sigev_signo = SIGUSR1;
    cb.aio_sigevent.sigev_value.sival_ptr = &cb;

    // Lancer la lecture
    if (aio_read(&cb) == -1) {
        perror("aio_read");
        return 1;
    }

    printf("Lecture lancée, en attente du signal...\n");

    // Faire autre chose
    while (!aio_done) {
        printf("Travail en cours...\n");
        sleep(1);
    }

    printf("Données: %s\n", (char *)buffer);

    close(fd);
    return 0;
}
```

**Avantages :** Vraiment asynchrone  
**Inconvénients :** Complexité des signaux  

#### 3. Thread callback

Lancer un thread qui exécutera une fonction de callback :

```c
#define _POSIX_C_SOURCE 200809L
#include <aio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

void aio_callback(union sigval sigval) {
    struct aiocb *cb = (struct aiocb *)sigval.sival_ptr;

    ssize_t bytes = aio_return(cb);
    printf("Callback exécuté : %zd octets lus\n", bytes);

    char *data = (char *)cb->aio_buf;
    data[bytes] = '\0';
    printf("Données: %s\n", data);
}

int main(void) {
    int fd = open("fichier.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    struct aiocb cb;
    memset(&cb, 0, sizeof(cb));

    char buffer[1024];
    cb.aio_fildes = fd;
    cb.aio_buf = buffer;
    cb.aio_nbytes = sizeof(buffer);
    cb.aio_offset = 0;

    // Configurer la notification par thread
    cb.aio_sigevent.sigev_notify = SIGEV_THREAD;
    cb.aio_sigevent.sigev_notify_function = aio_callback;
    cb.aio_sigevent.sigev_value.sival_ptr = &cb;

    // Lancer la lecture
    if (aio_read(&cb) == -1) {
        perror("aio_read");
        return 1;
    }

    printf("Lecture lancée, programme continue...\n");

    // Faire autre chose
    for (int i = 0; i < 5; i++) {
        printf("Travail en cours... %d\n", i);
        sleep(1);
    }

    // Attendre que l'opération soit terminée
    const struct aiocb *cb_list[] = { &cb };
    aio_suspend(cb_list, 1, NULL);

    close(fd);
    return 0;
}
```

**Avantages :** Propre, séparation des responsabilités  
**Inconvénients :** Overhead du thread  

### Exemple : Copie de fichier asynchrone

```c
#define _XOPEN_SOURCE 600
#include <aio.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <dest>\n", argv[0]);
        return 1;
    }

    // Ouvrir les fichiers
    int fd_in = open(argv[1], O_RDONLY);
    int fd_out = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (fd_in == -1 || fd_out == -1) {
        perror("open");
        return 1;
    }

    // Allouer deux buffers (double buffering)
    char *buffer1 = malloc(BUFFER_SIZE);
    char *buffer2 = malloc(BUFFER_SIZE);

    struct aiocb cb_read, cb_write;
    memset(&cb_read, 0, sizeof(cb_read));
    memset(&cb_write, 0, sizeof(cb_write));

    off_t offset = 0;
    int done = 0;

    // Lancer la première lecture
    cb_read.aio_fildes = fd_in;
    cb_read.aio_buf = buffer1;
    cb_read.aio_nbytes = BUFFER_SIZE;
    cb_read.aio_offset = offset;
    aio_read(&cb_read);

    while (!done) {
        // Attendre que la lecture soit terminée
        while (aio_error(&cb_read) == EINPROGRESS) {
            usleep(1000);
        }

        ssize_t bytes_read = aio_return(&cb_read);

        if (bytes_read <= 0) {
            done = 1;
            break;
        }

        // Lancer l'écriture asynchrone
        cb_write.aio_fildes = fd_out;
        cb_write.aio_buf = buffer1;
        cb_write.aio_nbytes = bytes_read;
        cb_write.aio_offset = offset;
        aio_write(&cb_write);

        offset += bytes_read;

        // Échanger les buffers
        char *temp = buffer1;
        buffer1 = buffer2;
        buffer2 = temp;

        // Lancer la prochaine lecture (pendant que l'écriture se fait)
        cb_read.aio_buf = buffer1;
        cb_read.aio_offset = offset;
        aio_read(&cb_read);

        // Attendre que l'écriture soit terminée
        while (aio_error(&cb_write) == EINPROGRESS) {
            usleep(1000);
        }
    }

    free(buffer1);
    free(buffer2);
    close(fd_in);
    close(fd_out);

    printf("Copie terminée : %ld octets\n", (long)offset);

    return 0;
}
```

**Avantage :** Les opérations de lecture et d'écriture se chevauchent (pipeline).

### Limitations de POSIX AIO

⚠️ **Problèmes de POSIX AIO :**

1. **Implémentation variable** : glibc simule avec des threads (overhead)
2. **Performance médiocre** sur Linux comparé à d'autres OS
3. **Bugs historiques** dans certaines implémentations
4. **Pas de support pour tous les types de fichiers**
5. **API complexe** avec beaucoup de configuration

**Conclusion :** Pour Linux, préférez `io_uring` (voir section suivante).

## io_uring : L'I/O asynchrone moderne de Linux

### Présentation

`io_uring` est la nouvelle interface d'I/O asynchrone de Linux (kernel 5.1+, 2019). Elle offre des performances exceptionnelles.

**Avantages :**
- ✅ **Performances exceptionnelles** : proche du matériel
- ✅ **Zéro copie** entre user-space et kernel-space
- ✅ **Batching** : soumettre plusieurs opérations en une fois
- ✅ **Support complet** : fichiers, sockets, accept, etc.
- ✅ **Polling** ou interruptions

**Inconvénient :**
- ❌ **Linux seulement** (pas portable)
- ❌ Nécessite kernel 5.1+ (5.19+ recommandé)

### Architecture de io_uring

```
┌────────────────────────────────────────────────┐
│            ESPACE UTILISATEUR                  │
│                                                │
│  ┌─────────────────┐      ┌─────────────────┐  │
│  │ Submission Queue│      │ Completion Queue│  │
│  │     (SQ)        │      │      (CQ)       │  │
│  │                 │      │                 │  │
│  │ [req1][req2]... │      │ [res1][res2]... │  │
│  └────────┬────────┘      └────────▲────────┘  │
│           │                        │           │
└───────────┼────────────────────────┼───────────┘
            │                        │
            │    Mémoire partagée    │
            │    (zero-copy)         │
            │                        │
┌───────────▼────────────────────────┼───────────┐
│           │    NOYAU LINUX         │           │
│           │                        │           │
│  ┌────────▼────────┐      ┌────────┴────────┐  │
│  │  Traite les     │      │  Écrit les      │  │
│  │  requêtes       │──────│  résultats      │  │
│  └─────────────────┘      └─────────────────┘  │
│                                                │
│           I/O asynchrone                       │
└────────────────────────────────────────────────┘
```

**Principe :**
1. L'application pousse des requêtes dans la **Submission Queue**
2. Le noyau traite les requêtes de manière asynchrone
3. Les résultats apparaissent dans la **Completion Queue**
4. Tout se fait en **mémoire partagée** (zéro copie)

### Installation de liburing

Pour utiliser io_uring facilement, on utilise la bibliothèque `liburing` :

```bash
# Ubuntu/Debian
sudo apt-get install liburing-dev

# Fedora
sudo dnf install liburing-devel

# Compiler depuis les sources
git clone https://github.com/axboe/liburing.git  
cd liburing  
./configure
make  
sudo make install  
```

### Exemple basique avec io_uring

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <liburing.h>

#define QUEUE_DEPTH 1

int main(void) {
    struct io_uring ring;

    // Initialiser io_uring
    if (io_uring_queue_init(QUEUE_DEPTH, &ring, 0) < 0) {
        perror("io_uring_queue_init");
        return 1;
    }

    // Ouvrir un fichier
    int fd = open("fichier.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Préparer le buffer
    char buffer[1024];
    struct iovec iov = {
        .iov_base = buffer,
        .iov_len = sizeof(buffer)
    };

    // Obtenir une Submission Queue Entry (SQE)
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    if (!sqe) {
        fprintf(stderr, "Impossible d'obtenir une SQE\n");
        return 1;
    }

    // Préparer une opération de lecture
    io_uring_prep_readv(sqe, fd, &iov, 1, 0);

    // Attacher des données utilisateur (optionnel)
    io_uring_sqe_set_data(sqe, (void *)"lecture_fichier");

    // Soumettre la requête
    if (io_uring_submit(&ring) < 0) {
        perror("io_uring_submit");
        return 1;
    }

    printf("Requête soumise, en attente du résultat...\n");

    // Attendre la complétion
    struct io_uring_cqe *cqe;
    if (io_uring_wait_cqe(&ring, &cqe) < 0) {
        perror("io_uring_wait_cqe");
        return 1;
    }

    // Traiter le résultat
    if (cqe->res < 0) {
        fprintf(stderr, "Erreur lors de la lecture: %s\n",
                strerror(-cqe->res));
    } else {
        buffer[cqe->res] = '\0';
        printf("Lu %d octets: %s\n", cqe->res, buffer);
    }

    // Récupérer les données utilisateur
    char *user_data = (char *)io_uring_cqe_get_data(cqe);
    printf("Données utilisateur: %s\n", user_data);

    // Marquer la CQE comme vue
    io_uring_cqe_seen(&ring, cqe);

    // Nettoyer
    io_uring_queue_exit(&ring);
    close(fd);

    return 0;
}
```

**Compilation :**
```bash
gcc -o uring_demo uring_demo.c -luring
```

### Exemple : Lecture multiple en parallèle

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <liburing.h>

#define QUEUE_DEPTH 32
#define BUF_SIZE 4096

typedef struct {
    int file_index;
    char buffer[BUF_SIZE];
} read_context_t;

int main(void) {
    const char *files[] = {
        "file1.txt",
        "file2.txt",
        "file3.txt",
        "file4.txt"
    };
    int num_files = 4;

    struct io_uring ring;
    io_uring_queue_init(QUEUE_DEPTH, &ring, 0);

    // Ouvrir tous les fichiers
    int fds[num_files];
    for (int i = 0; i < num_files; i++) {
        fds[i] = open(files[i], O_RDONLY);
        if (fds[i] == -1) {
            perror("open");
            continue;
        }
    }

    // Préparer les contextes de lecture
    read_context_t *contexts = malloc(sizeof(read_context_t) * num_files);

    // Soumettre toutes les lectures en parallèle
    for (int i = 0; i < num_files; i++) {
        if (fds[i] == -1) continue;

        contexts[i].file_index = i;

        struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
        if (!sqe) break;

        io_uring_prep_read(sqe, fds[i], contexts[i].buffer, BUF_SIZE, 0);
        io_uring_sqe_set_data(sqe, &contexts[i]);
    }

    // Soumettre toutes les requêtes en une fois
    int submitted = io_uring_submit(&ring);
    printf("%d requêtes soumises en parallèle\n", submitted);

    // Attendre et traiter les résultats
    for (int i = 0; i < submitted; i++) {
        struct io_uring_cqe *cqe;
        io_uring_wait_cqe(&ring, &cqe);

        read_context_t *ctx = (read_context_t *)io_uring_cqe_get_data(cqe);

        if (cqe->res >= 0) {
            ctx->buffer[cqe->res] = '\0';
            printf("Fichier %d (%s): %d octets lus\n",
                   ctx->file_index, files[ctx->file_index], cqe->res);
        } else {
            fprintf(stderr, "Erreur fichier %d: %s\n",
                   ctx->file_index, strerror(-cqe->res));
        }

        io_uring_cqe_seen(&ring, cqe);
    }

    // Nettoyer
    for (int i = 0; i < num_files; i++) {
        if (fds[i] != -1) close(fds[i]);
    }
    free(contexts);
    io_uring_queue_exit(&ring);

    return 0;
}
```

**Avantage :** Les 4 fichiers sont lus **en parallèle** de manière vraiment asynchrone !

### Exemple avancé : Serveur echo avec io_uring

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <liburing.h>

#define QUEUE_DEPTH 256
#define BUFFER_SIZE 1024
#define PORT 8080

typedef enum {
    ACCEPT,
    READ,
    WRITE
} op_type_t;

typedef struct {
    op_type_t type;
    int fd;
    char buffer[BUFFER_SIZE];
    size_t len;
} request_t;

int setup_listening_socket(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) return -1;

    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) == -1) {
        close(sock);
        return -1;
    }

    if (listen(sock, SOMAXCONN) == -1) {
        close(sock);
        return -1;
    }

    return sock;
}

void add_accept(struct io_uring *ring, int server_fd) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);

    request_t *req = malloc(sizeof(request_t));
    req->type = ACCEPT;
    req->fd = server_fd;

    io_uring_prep_accept(sqe, server_fd, NULL, NULL, 0);
    io_uring_sqe_set_data(sqe, req);
}

void add_read(struct io_uring *ring, int client_fd) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);

    request_t *req = malloc(sizeof(request_t));
    req->type = READ;
    req->fd = client_fd;

    io_uring_prep_read(sqe, client_fd, req->buffer, BUFFER_SIZE, 0);
    io_uring_sqe_set_data(sqe, req);
}

void add_write(struct io_uring *ring, int client_fd,
               const char *data, size_t len) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);

    request_t *req = malloc(sizeof(request_t));
    req->type = WRITE;
    req->fd = client_fd;
    req->len = len;
    memcpy(req->buffer, data, len);

    io_uring_prep_write(sqe, client_fd, req->buffer, len, 0);
    io_uring_sqe_set_data(sqe, req);
}

int main(void) {
    int server_fd = setup_listening_socket(PORT);
    if (server_fd == -1) {
        perror("setup_listening_socket");
        return 1;
    }

    printf("Serveur io_uring démarré sur le port %d\n", PORT);

    struct io_uring ring;
    io_uring_queue_init(QUEUE_DEPTH, &ring, 0);

    // Soumettre le premier accept
    add_accept(&ring, server_fd);
    io_uring_submit(&ring);

    // Boucle d'événements
    while (1) {
        struct io_uring_cqe *cqe;
        io_uring_wait_cqe(&ring, &cqe);

        request_t *req = (request_t *)io_uring_cqe_get_data(cqe);

        if (cqe->res < 0) {
            fprintf(stderr, "Erreur: %s\n", strerror(-cqe->res));
            close(req->fd);
            free(req);
            io_uring_cqe_seen(&ring, cqe);
            continue;
        }

        switch (req->type) {
            case ACCEPT: {
                int client_fd = cqe->res;
                printf("Nouvelle connexion: fd %d\n", client_fd);

                // Préparer la lecture pour ce client
                add_read(&ring, client_fd);

                // Relancer un accept pour le prochain client
                add_accept(&ring, server_fd);

                io_uring_submit(&ring);
                break;
            }

            case READ: {
                int bytes_read = cqe->res;

                if (bytes_read == 0) {
                    // Client déconnecté
                    printf("Client fd %d déconnecté\n", req->fd);
                    close(req->fd);
                } else {
                    // Écho : renvoyer les données
                    req->buffer[bytes_read] = '\0';
                    printf("Reçu de fd %d: %s", req->fd, req->buffer);

                    add_write(&ring, req->fd, req->buffer, bytes_read);
                    io_uring_submit(&ring);
                }
                break;
            }

            case WRITE: {
                // Écriture terminée, relancer une lecture
                add_read(&ring, req->fd);
                io_uring_submit(&ring);
                break;
            }
        }

        free(req);
        io_uring_cqe_seen(&ring, cqe);
    }

    io_uring_queue_exit(&ring);
    close(server_fd);

    return 0;
}
```

**Compilation :**
```bash
gcc -o uring_server uring_server.c -luring
```

**Performance :** Ce serveur peut gérer **des dizaines de milliers de connexions** avec un seul thread !

## Comparaison des technologies AIO

### Tableau récapitulatif

| Critère | POSIX AIO | io_uring |
|---------|-----------|----------|
| **Portabilité** | ✅ POSIX (Linux, BSD, macOS) | ❌ Linux seulement (5.1+) |
| **Performance** | ⚠️ Moyenne (threads en glibc) | ✅ Excellente (zéro copie) |
| **API** | ⚠️ Complexe | ✅ Moderne et propre |
| **Maturité** | ✅ Stable depuis longtemps | ⚠️ Récent (2019) |
| **Support** | ⚠️ Limité selon implémentation | ✅ Fichiers, sockets, accept, etc. |
| **Batching** | ❌ Non | ✅ Oui (soumettre plusieurs ops) |
| **Zéro copie** | ❌ Non | ✅ Oui (mémoire partagée) |
| **Polling** | ❌ Non | ✅ Oui (IORING_SETUP_IOPOLL) |
| **Cas d'usage** | Code portable | Serveurs haute perf Linux |

### Benchmark simplifié

```
Test: Lire 1000 fichiers de 4 Ko chacun

I/O bloquant :     250 ms  
POSIX AIO :        180 ms  (28% plus rapide)  
io_uring :          45 ms  (82% plus rapide) ✅  
```

**Conclusion :** `io_uring` est nettement supérieur sur Linux moderne.

## Quand utiliser l'I/O asynchrone ?

### ✅ Utilisez AIO quand :

1. **I/O disque intensif**
   - Bases de données
   - Traitement de gros fichiers
   - Serveurs de fichiers

2. **Besoin de performances maximales**
   - Latence critique
   - Haut débit (throughput)
   - Milliers de connexions simultanées

3. **Opérations indépendantes**
   - Plusieurs fichiers à traiter en parallèle
   - Pipeline de traitement
   - Batching d'opérations

### ❌ N'utilisez PAS AIO quand :

1. **Opérations simples**
   - Scripts courts
   - Peu d'I/O
   - Prototypes rapides

2. **Portabilité requise**
   - Code multi-plateforme
   - io_uring n'est pas disponible

3. **Complexité non justifiée**
   - L'I/O multiplexing suffit
   - Overhead de développement/maintenance

### Arbre de décision

```
Quel modèle d'I/O choisir ?
│
├─ Opération unique, simple
│  └─→ I/O bloquant (read/write)
│
├─ Plusieurs sockets/connexions
│  ├─ Portabilité importante
│  │  └─→ poll()
│  └─ Performance max (Linux)
│     └─→ epoll() ou io_uring
│
└─ I/O disque intensif, haute performance
   ├─ Linux moderne (kernel 5.1+)
   │  └─→ io_uring ✅
   └─ Portabilité requise
      └─→ POSIX AIO ou threads
```

## Bonnes pratiques

### 1. Gérer les erreurs proprement

```c
// ✅ BON
struct io_uring_cqe *cqe;  
if (io_uring_wait_cqe(&ring, &cqe) < 0) {  
    perror("io_uring_wait_cqe");
    return -1;
}

if (cqe->res < 0) {
    fprintf(stderr, "Erreur I/O: %s\n", strerror(-cqe->res));
    // Gérer l'erreur
}

// ❌ MAUVAIS
io_uring_wait_cqe(&ring, &cqe);
// Pas de vérification !
```

### 2. Toujours marquer les CQE comme vues

```c
struct io_uring_cqe *cqe;  
io_uring_wait_cqe(&ring, &cqe);  

// Traiter cqe...

io_uring_cqe_seen(&ring, cqe);  // ✅ Important !
```

### 3. Libérer les ressources

```c
// Libérer les contextes alloués
request_t *req = io_uring_cqe_get_data(cqe);
// ... utiliser req ...
free(req);  // ✅ Ne pas oublier

// Fermer les descripteurs
close(fd);

// Nettoyer io_uring
io_uring_queue_exit(&ring);
```

### 4. Utiliser le batching avec io_uring

```c
// ❌ INEFFICACE
for (int i = 0; i < 100; i++) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(...);
    io_uring_submit(&ring);  // Syscall à chaque fois !
}

// ✅ EFFICACE
for (int i = 0; i < 100; i++) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(...);
}
io_uring_submit(&ring);  // Un seul syscall pour tout !
```

### 5. Dimensionner la queue correctement

```c
// Trop petit : risque de manquer de slots
io_uring_queue_init(8, &ring, 0);  // ⚠️

// Raisonnable pour un serveur
io_uring_queue_init(256, &ring, 0);  // ✅

// Très grand (consomme plus de mémoire)
io_uring_queue_init(4096, &ring, 0);  // ⚠️ Si vraiment nécessaire
```

## Pièges courants

### 1. Oublier de soumettre les requêtes

```c
// ❌ ERREUR
struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);  
io_uring_prep_read(sqe, fd, buffer, size, 0);  
// Oubli de io_uring_submit() !
io_uring_wait_cqe(&ring, &cqe);  // Attend indéfiniment !

// ✅ CORRECT
struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);  
io_uring_prep_read(sqe, fd, buffer, size, 0);  
io_uring_submit(&ring);  // ✅  
io_uring_wait_cqe(&ring, &cqe);  
```

### 2. Buffer invalide pendant l'opération

```c
// ❌ DANGEREUX
void bad_function(struct io_uring *ring, int fd) {
    char buffer[1024];  // Sur la pile !

    struct io_uring_sqe *sqe = io_uring_get_sqe(ring);
    struct iovec iov = { .iov_base = buffer, .iov_len = 1024 };
    io_uring_prep_readv(sqe, fd, &iov, 1, 0);
    io_uring_submit(ring);

    // Fonction retourne, buffer détruit !
}  // ← buffer n'existe plus, mais l'I/O est toujours en cours !

// ✅ CORRECT
char *buffer = malloc(1024);  // Alloué sur le tas
// ... utiliser buffer ...
// Libérer seulement après complétion
```

### 3. Réutiliser un buffer avant complétion

```c
// ❌ PROBLÈME
char buffer[1024];  
io_uring_prep_read(sqe1, fd1, buffer, 1024, 0);  
io_uring_submit(&ring);  

// Réutilise le même buffer immédiatement !
io_uring_prep_read(sqe2, fd2, buffer, 1024, 0);  // ⚠️ Race !

// ✅ SOLUTION
char buffer1[1024], buffer2[1024];  
io_uring_prep_read(sqe1, fd1, buffer1, 1024, 0);  
io_uring_prep_read(sqe2, fd2, buffer2, 1024, 0);  
```

### 4. Ne pas vérifier la disponibilité des SQE

```c
// ⚠️ RISQUE
for (int i = 0; i < 1000; i++) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    // sqe peut être NULL si la queue est pleine !
}

// ✅ CORRECT
for (int i = 0; i < 1000; i++) {
    struct io_uring_sqe *sqe = io_uring_get_sqe(&ring);
    if (!sqe) {
        // Queue pleine, soumettre et réessayer
        io_uring_submit(&ring);
        sqe = io_uring_get_sqe(&ring);
    }
    // ...
}
```

## Cas d'usage réels

### 1. Base de données (RocksDB, PostgreSQL)

```c
// Lecture de plusieurs blocs en parallèle
for (int i = 0; i < num_blocks; i++) {
    io_uring_prep_read(sqe, fd, blocks[i].buffer,
                      BLOCK_SIZE, blocks[i].offset);
}
io_uring_submit(&ring);

// Tous les blocs sont lus en parallèle
```

### 2. Serveur web (Nginx avec io_uring)

```c
// Accept, read, write tous via io_uring
io_uring_prep_accept(sqe_accept, server_fd, ...);  
io_uring_prep_recv(sqe_read, client_fd, buffer, ...);  
io_uring_prep_send(sqe_write, client_fd, response, ...);  
```

### 3. Traitement de logs

```c
// Lire plusieurs fichiers de log en parallèle
for (int i = 0; i < num_log_files; i++) {
    io_uring_prep_read(sqe, log_fds[i], buffers[i], ...);
}
io_uring_submit(&ring);

// Analyser les logs pendant que d'autres se chargent
```

### 4. Backup / Archivage

```c
// Lire depuis source et écrire vers destination en parallèle
io_uring_prep_read(sqe_read, src_fd, buffer, SIZE, offset);  
io_uring_prep_write(sqe_write, dst_fd, buffer, SIZE, offset);  

// Pipeline : lecture et écriture simultanées
```

## Ressources et apprentissage

### Documentation officielle

- **POSIX AIO** : `man 7 aio`, `man 3 aio_read`
- **io_uring** : https://kernel.dk/io_uring.pdf
- **liburing** : https://github.com/axboe/liburing

### Outils de mesure

```bash
# Installer fio (benchmark I/O)
sudo apt-get install fio

# Tester io_uring vs autres engines
fio --name=test --ioengine=io_uring --rw=read --bs=4k
```

### Livres et articles

- "Efficient I/O with io_uring" (Jens Axboe, 2019)
- "The Linux Programming Interface" (Michael Kerrisk)
- Articles sur lwn.net sur io_uring

## Points clés à retenir

✅ **I/O asynchrone** = Lancer des opérations sans attendre leur complétion

✅ **POSIX AIO** = Standard portable mais performance limitée

✅ **io_uring** = Moderne, ultra-performant, Linux seulement

✅ **Notification** : Polling, signaux, ou callbacks (threads)

✅ **Batching** avec io_uring = Soumettre plusieurs opérations en une fois

✅ **Zéro copie** avec io_uring = Mémoire partagée user/kernel

✅ **Cas d'usage** : Bases de données, serveurs haute perf, I/O disque intensif

✅ **Complexité** : Plus élevée que I/O bloquant ou multiplexing

✅ **Performance** : io_uring >> POSIX AIO >> I/O synchrone

✅ **Choix** : io_uring pour Linux haute perf, POSIX AIO pour portabilité

---

## Prochaines étapes

Maintenant que vous comprenez l'I/O asynchrone, explorez :
- **Chapitre 17** : Processus et signaux (pour comprendre les signaux AIO)
- **Chapitre 18** : Threads et concurrence (alternative à AIO)
- **Chapitre 20** : Programmation réseau (combiné avec io_uring)
- **Chapitre 27** : Optimisation et performance (profiling d'I/O)

L'I/O asynchrone représente le summum de l'efficacité pour les opérations d'entrée/sortie. C'est une technique avancée qui, lorsqu'elle est bien maîtrisée, permet de créer des systèmes extrêmement performants capables de gérer des charges massives !

⏭️ [Processus et Signaux](/17-processus-et-signaux/README.md)
