/* ============================================================================
   Section 25.6 : Logging
   Description : Logging dans un fichier avec informations de contexte
   Fichier source : 06-logging.md
   ============================================================================ */

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>
#include <string.h>

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

/* Configuration globale du logger */
static struct {
    FILE *fichier;
    LogLevel niveau_min;
    bool console_active;
} logger = {NULL, LOG_INFO, true};

static const char *niveau_noms[] = {
    "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"
};

/* Obtenir le timestamp formate */
static void obtenir_timestamp(char *buffer, size_t taille) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    snprintf(buffer, taille, "%04d-%02d-%02d %02d:%02d:%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
}

/* Fonction de logging */
void log_message(LogLevel niveau, const char *fichier_source, int ligne,
                 const char *format, ...) {
    if (niveau < logger.niveau_min) {
        return;
    }

    char timestamp[32];
    obtenir_timestamp(timestamp, sizeof(timestamp));

    /* Preparer le message */
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    /* Format : [timestamp] [NIVEAU] [fichier:ligne] message */
    char log_line[2048];
    snprintf(log_line, sizeof(log_line), "[%s] [%s] [%s:%d] %s\n",
             timestamp, niveau_noms[niveau], fichier_source, ligne, message);

    /* Ecrire dans le fichier */
    if (logger.fichier != NULL) {
        fputs(log_line, logger.fichier);
        fflush(logger.fichier);  /* Forcer l'ecriture immediate */
    }

    /* Ecrire sur la console si active */
    if (logger.console_active) {
        fputs(log_line, stderr);
    }
}

/* Macros avec informations de fichier et ligne */
#define LOG_DEBUG(...)   log_message(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)    log_message(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) log_message(LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)   log_message(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)   log_message(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/* Initialiser le logger */
bool log_init(const char *nom_fichier, LogLevel niveau) {
    logger.fichier = fopen(nom_fichier, "a");  /* Mode append */
    if (logger.fichier == NULL) {
        return false;
    }

    logger.niveau_min = niveau;

    LOG_INFO("===== Demarrage du logging =====");

    return true;
}

/* Fermer le logger */
void log_close(void) {
    if (logger.fichier != NULL) {
        LOG_INFO("===== Arret du logging =====");
        fclose(logger.fichier);
        logger.fichier = NULL;
    }
}

int main(void) {
    /* Initialiser le logger */
    if (!log_init("/tmp/application_test.log", LOG_DEBUG)) {
        fprintf(stderr, "Erreur : impossible d'initialiser le logger\n");
        return 1;
    }

    /* Desactiver la console pour un affichage propre */
    logger.console_active = false;

    LOG_INFO("Application demarree");
    LOG_DEBUG("Mode debug active");

    int valeur = 42;
    LOG_DEBUG("Valeur calculee : %d", valeur);

    FILE *f = fopen("inexistant.txt", "r");
    if (f == NULL) {
        LOG_ERROR("Impossible d'ouvrir le fichier inexistant.txt");
    } else {
        fclose(f);
    }

    LOG_WARNING("Cette operation est depreciee");

    LOG_INFO("Application terminee avec succes");

    log_close();

    /* Afficher le contenu du fichier log */
    printf("=== Contenu du fichier log ===\n");
    f = fopen("/tmp/application_test.log", "r");
    if (f != NULL) {
        char ligne[2048];
        while (fgets(ligne, (int)sizeof(ligne), f) != NULL) {
            printf("%s", ligne);
        }
        fclose(f);
    }

    /* Nettoyage */
    remove("/tmp/application_test.log");

    return 0;
}
