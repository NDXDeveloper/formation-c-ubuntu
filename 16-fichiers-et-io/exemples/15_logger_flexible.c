/* ============================================================================
   Section 16.5 : I/O bufferise vs non bufferise
   Description : Logger flexible avec 3 modes de buffering
   Fichier source : 05-io-bufferise.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

typedef enum {
    LOG_BUFFERED,    /* Performance maximale */
    LOG_LINE,        /* Balance entre performance et securite */
    LOG_IMMEDIATE    /* Securite maximale (unbuffered) */
} log_mode_t;

typedef struct {
    FILE *fp;
    log_mode_t mode;
} logger_t;

static logger_t *logger_create(const char *filename, log_mode_t mode) {
    logger_t *logger = malloc(sizeof(logger_t));
    if (!logger) return NULL;

    logger->fp = fopen(filename, "a");
    if (!logger->fp) {
        free(logger);
        return NULL;
    }

    logger->mode = mode;

    /* Configurer le buffering selon le mode */
    switch (mode) {
        case LOG_BUFFERED:
            /* Par defaut : fully buffered (rapide) */
            break;
        case LOG_LINE:
            setvbuf(logger->fp, NULL, _IOLBF, 0);
            break;
        case LOG_IMMEDIATE:
            setbuf(logger->fp, NULL);  /* Unbuffered */
            break;
    }

    return logger;
}

static void logger_log(logger_t *logger, const char *level,
                       const char *fmt, ...) {
    if (!logger || !logger->fp) return;

    /* Timestamp */
    time_t now = time(NULL);
    char *timestr = ctime(&now);
    timestr[strlen(timestr) - 1] = '\0';  /* Enlever le \n */

    /* Ecrire l'en-tete */
    fprintf(logger->fp, "[%s] [%s] ", timestr, level);

    /* Ecrire le message avec arguments variables */
    va_list args;
    va_start(args, fmt);
    vfprintf(logger->fp, fmt, args);
    va_end(args);

    fprintf(logger->fp, "\n");
}

static void logger_destroy(logger_t *logger) {
    if (logger) {
        if (logger->fp) fclose(logger->fp);
        free(logger);
    }
}

int main(void) {
    /* Logger haute performance (bufferise) */
    logger_t *fast_log = logger_create("/tmp/fast.log", LOG_BUFFERED);

    /* Logger equilibre (line buffered) */
    logger_t *normal_log = logger_create("/tmp/normal.log", LOG_LINE);

    /* Logger securise (unbuffered) */
    logger_t *critical_log = logger_create("/tmp/critical.log", LOG_IMMEDIATE);

    if (!fast_log || !normal_log || !critical_log) {
        fprintf(stderr, "Erreur creation des loggers\n");
        return 1;
    }

    /* Utilisation */
    for (int i = 0; i < 1000; i++) {
        logger_log(fast_log, "DEBUG", "Iteration %d", i);
    }

    logger_log(normal_log, "INFO", "Traitement termine");
    logger_log(critical_log, "ERROR", "Erreur critique detectee");

    printf("Logs ecrits dans /tmp/ :\n");
    printf("  fast.log     (bufferise)   : 1000 lignes DEBUG\n");
    printf("  normal.log   (line buffer) : 1 ligne INFO\n");
    printf("  critical.log (unbuffered)  : 1 ligne ERROR\n");

    /* Nettoyage */
    logger_destroy(fast_log);
    logger_destroy(normal_log);
    logger_destroy(critical_log);

    unlink("/tmp/fast.log");
    unlink("/tmp/normal.log");
    unlink("/tmp/critical.log");

    return 0;
}
