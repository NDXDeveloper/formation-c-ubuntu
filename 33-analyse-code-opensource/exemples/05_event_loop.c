/* ============================================================================
   Section 33.3/33.4 : Event Loop (Redis/Nginx)
   Description : Boucle evenementielle simplifiee avec poll()
   Fichier source : 03-etude-cas-redis.md, 04-etude-cas-nginx.md
   ============================================================================ */

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

/* ============================================ */
/* Event Loop simplifie (Redis / Nginx)         */
/* ============================================ */

/*
 * Redis et Nginx sont single-threaded et gerent des milliers
 * de connexions grace a une boucle evenementielle (event loop).
 *
 * Principe :
 * 1. Enregistrer des file descriptors (sockets, pipes, timers)
 * 2. Attendre qu'un evenement arrive (poll/epoll/kqueue)
 * 3. Appeler le handler correspondant
 * 4. Recommencer
 *
 * Ce modele est plus efficace qu'un thread par connexion.
 */

/* Taille max d'evenements */
#define MAX_EVENTS 16

/* Types d'evenements */
#define EV_READABLE  1
#define EV_WRITABLE  2

/* Callback pour un evenement */
typedef void (*event_handler_t)(int fd, int events, void *data);

/* Entree dans la table d'evenements */
typedef struct {
    int fd;
    int mask;                   /* EV_READABLE | EV_WRITABLE */
    event_handler_t handler;
    void *data;
    int active;
} event_entry_t;

/* Timer callback */
typedef void (*timer_handler_t)(void *data);

typedef struct {
    struct timespec deadline;
    timer_handler_t handler;
    void *data;
    int active;
} timer_entry_t;

#define MAX_TIMERS 8

/* La boucle evenementielle */
typedef struct {
    event_entry_t events[MAX_EVENTS];
    timer_entry_t timers[MAX_TIMERS];
    int num_events;
    int num_timers;
    int running;
} event_loop_t;

/* Variable globale pour le signal handler */
static volatile sig_atomic_t got_signal = 0;

/* ============================================ */
/* Operations sur l'event loop                  */
/* ============================================ */

static event_loop_t *event_loop_create(void)
{
    event_loop_t *loop = calloc(1, sizeof(event_loop_t));
    if (!loop) return NULL;
    loop->running = 0;
    return loop;
}

static void event_loop_destroy(event_loop_t *loop)
{
    free(loop);
}

/* Ajouter un evenement fd */
static int event_add(event_loop_t *loop, int fd, int mask,
                     event_handler_t handler, void *data)
{
    if (loop->num_events >= MAX_EVENTS) return -1;

    event_entry_t *e = &loop->events[loop->num_events++];
    e->fd = fd;
    e->mask = mask;
    e->handler = handler;
    e->data = data;
    e->active = 1;

    return 0;
}

/* Ajouter un timer (en millisecondes) */
static int timer_add(event_loop_t *loop, int ms,
                     timer_handler_t handler, void *data)
{
    if (loop->num_timers >= MAX_TIMERS) return -1;

    timer_entry_t *t = &loop->timers[loop->num_timers++];
    clock_gettime(CLOCK_MONOTONIC, &t->deadline);
    t->deadline.tv_sec += ms / 1000;
    t->deadline.tv_nsec += (ms % 1000) * 1000000L;
    if (t->deadline.tv_nsec >= 1000000000L) {
        t->deadline.tv_sec++;
        t->deadline.tv_nsec -= 1000000000L;
    }
    t->handler = handler;
    t->data = data;
    t->active = 1;

    return 0;
}

/* Calculer le timeout jusqu'au prochain timer */
static int next_timer_ms(const event_loop_t *loop)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    int min_ms = 1000;  /* Par defaut: 1 seconde */

    for (int i = 0; i < loop->num_timers; i++) {
        if (!loop->timers[i].active) continue;

        const struct timespec *dl = &loop->timers[i].deadline;
        int diff_ms = (int)((dl->tv_sec - now.tv_sec) * 1000 +
                            (dl->tv_nsec - now.tv_nsec) / 1000000);
        if (diff_ms < 0) diff_ms = 0;
        if (diff_ms < min_ms) min_ms = diff_ms;
    }

    return min_ms;
}

/* Executer les timers expires */
static void process_timers(event_loop_t *loop)
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);

    for (int i = 0; i < loop->num_timers; i++) {
        timer_entry_t *t = &loop->timers[i];
        if (!t->active) continue;

        if (now.tv_sec > t->deadline.tv_sec ||
            (now.tv_sec == t->deadline.tv_sec &&
             now.tv_nsec >= t->deadline.tv_nsec)) {
            t->handler(t->data);
            t->active = 0;
        }
    }
}

/* Boucle principale */
static void event_loop_run(event_loop_t *loop)
{
    loop->running = 1;
    printf("  Event loop demarre\n");

    while (loop->running && !got_signal) {
        /* Construire le tableau pollfd */
        struct pollfd pfds[MAX_EVENTS];
        int nfds = 0;

        for (int i = 0; i < loop->num_events; i++) {
            if (!loop->events[i].active) continue;
            pfds[nfds].fd = loop->events[i].fd;
            pfds[nfds].events = 0;
            if (loop->events[i].mask & EV_READABLE) pfds[nfds].events |= POLLIN;
            if (loop->events[i].mask & EV_WRITABLE) pfds[nfds].events |= POLLOUT;
            pfds[nfds].revents = 0;
            nfds++;
        }

        /* Calculer le timeout */
        int timeout_ms = next_timer_ms(loop);

        /* Attendre un evenement (le coeur de la boucle) */
        int ready = poll(pfds, (nfds_t)nfds, timeout_ms);

        if (ready < 0) {
            if (errno == EINTR) continue;  /* Signal recu */
            break;
        }

        /* Traiter les evenements fd */
        if (ready > 0) {
            int pfd_idx = 0;
            for (int i = 0; i < loop->num_events; i++) {
                if (!loop->events[i].active) continue;
                if (pfds[pfd_idx].revents & (POLLIN | POLLHUP | POLLERR)) {
                    int fired = 0;
                    if (pfds[pfd_idx].revents & POLLIN) fired |= EV_READABLE;
                    loop->events[i].handler(
                        loop->events[i].fd, fired, loop->events[i].data);
                }
                pfd_idx++;
            }
        }

        /* Traiter les timers */
        process_timers(loop);
    }

    printf("  Event loop arrete\n");
}

static void event_loop_stop(event_loop_t *loop)
{
    loop->running = 0;
}

/* ============================================ */
/* Demonstration                                */
/* ============================================ */

/* Compteur de messages recus */
static int message_count = 0;

/* Handler pour les donnees lisibles sur le pipe */
static void on_pipe_readable(int fd, int events, void *data)
{
    (void)events;
    event_loop_t *loop = data;

    char buf[256];
    ssize_t n = read(fd, buf, sizeof(buf) - 1);
    if (n <= 0) {
        printf("  [event] Pipe ferme\n");
        event_loop_stop(loop);
        return;
    }

    buf[n] = '\0';

    /* Traiter chaque ligne comme une commande separee */
    char *line = buf;
    char *newline;
    while ((newline = strchr(line, '\n')) != NULL) {
        *newline = '\0';
        if (*line != '\0') {
            message_count++;
            printf("  [event] Commande #%d: \"%s\"\n", message_count, line);
        }
        line = newline + 1;
    }
    /* Derniere ligne sans newline */
    if (*line != '\0') {
        message_count++;
        printf("  [event] Commande #%d: \"%s\"\n", message_count, line);
    }
}

/* Handler pour le timer */
static void on_timer(void *data)
{
    (void)data;
    printf("  [timer] Timer expire! (%d messages recus jusqu'ici)\n",
           message_count);
}

int main(void)
{
    printf("=== Event Loop (Redis / Nginx) ===\n\n");

    printf("--- Principe ---\n");
    printf("Un seul thread gere plusieurs sources d'I/O grace a poll().\n");
    printf("Redis gere 100K+ connexions ainsi. Nginx fait pareil.\n\n");

    /* Creer un pipe pour simuler des connexions */
    int pipefd[2];
    if (pipe(pipefd) < 0) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    /* Creer la boucle */
    event_loop_t *loop = event_loop_create();
    if (!loop) {
        fprintf(stderr, "Erreur creation event loop\n");
        close(pipefd[0]);
        close(pipefd[1]);
        return EXIT_FAILURE;
    }

    /* Enregistrer le pipe en lecture */
    event_add(loop, pipefd[0], EV_READABLE, on_pipe_readable, loop);

    /* Ajouter un timer a 200ms */
    timer_add(loop, 200, on_timer, NULL);

    /* Simuler des donnees arrivant sur le pipe (ecritures non-bloquantes) */
    printf("--- Simulation ---\n");
    const char *messages[] = {
        "SET key1 value1\n",
        "GET key1\n",
        "SET key2 value2\n",
        "DEL key1\n",
    };
    int num_msgs = (int)(sizeof(messages) / sizeof(messages[0]));

    /* Ecrire tous les messages dans le pipe */
    for (int i = 0; i < num_msgs; i++) {
        ssize_t w = write(pipefd[1], messages[i], strlen(messages[i]));
        (void)w;
    }

    /* Fermer le cote ecriture pour signaler la fin */
    close(pipefd[1]);

    /* Lancer la boucle */
    event_loop_run(loop);

    /* Nettoyage */
    close(pipefd[0]);
    event_loop_destroy(loop);

    printf("\n--- Resultats ---\n");
    printf("  Messages traites: %d\n", message_count);

    printf("\n--- Architecture event loop ---\n");
    printf("1. Enregistrer les fd et handlers\n");
    printf("2. poll()/epoll_wait() attend les evenements\n");
    printf("3. Dispatcher vers le handler correspondant\n");
    printf("4. Traiter les timers expires\n");
    printf("5. Recommencer (single-threaded, pas de locks!)\n");

    return EXIT_SUCCESS;
}
