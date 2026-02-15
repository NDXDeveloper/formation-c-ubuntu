/* ============================================================================
   Section 25.3 : Patterns de gestion d'erreurs
   Description : Error Callback - fonctions de rappel pour gestion d'erreurs
   Fichier source : 03-patterns-gestion-erreurs.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

/* Type de callback d'erreur */
typedef void (*ErrorCallback)(int code, const char *message, void *user_data);

/* Gestionnaire d'erreurs global */
static struct {
    ErrorCallback callback;
    void *user_data;
} error_system = {NULL, NULL};

/* Enregistrer un callback */
void set_error_callback(ErrorCallback callback, void *user_data) {
    error_system.callback = callback;
    error_system.user_data = user_data;
}

/* Signaler une erreur */
void signal_error(int code, const char *format, ...) {
    char message[512];
    va_list args;

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    if (error_system.callback != NULL) {
        error_system.callback(code, message, error_system.user_data);
    } else {
        /* Comportement par defaut */
        fprintf(stderr, "Erreur %d: %s\n", code, message);
    }
}

/* Callback 1 : Affichage simple */
void error_handler_simple(int code, const char *message, void *user_data) {
    (void)user_data;  /* Non utilise */
    printf("[ERREUR %d] %s\n", code, message);
}

/* Callback 2 : Logging dans un fichier */
void error_handler_file(int code, const char *message, void *user_data) {
    FILE *log = (FILE *)user_data;

    if (log != NULL) {
        time_t now = time(NULL);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S",
                 localtime(&now));

        fprintf(log, "[%s] Erreur %d: %s\n", timestamp, code, message);
        fflush(log);
    }
}

/* Callback 3 : Compteur d'erreurs */
typedef struct {
    int nb_erreurs;
    int nb_critiques;
} StatistiquesErreurs;

void error_handler_stats(int code, const char *message, void *user_data) {
    StatistiquesErreurs *stats = (StatistiquesErreurs *)user_data;

    stats->nb_erreurs++;
    if (code >= 100) {
        stats->nb_critiques++;
    }

    fprintf(stderr, "[%d erreurs, %d critiques] Code %d: %s\n",
            stats->nb_erreurs, stats->nb_critiques, code, message);
}

/* Fonction qui peut generer des erreurs */
int traiter_donnees(int valeur) {
    if (valeur < 0) {
        signal_error(10, "Valeur negative: %d", valeur);
        return -1;
    }

    if (valeur > 100) {
        signal_error(150, "Valeur trop grande: %d", valeur);
        return -1;
    }

    return 0;
}

int main(void) {
    /* Test 1 : Sans callback (defaut) */
    printf("=== Test 1 : Comportement par defaut ===\n");
    traiter_donnees(-5);
    traiter_donnees(150);

    /* Test 2 : Callback simple */
    printf("\n=== Test 2 : Callback simple ===\n");
    set_error_callback(error_handler_simple, NULL);
    traiter_donnees(-10);

    /* Test 3 : Callback avec fichier */
    printf("\n=== Test 3 : Logging dans fichier ===\n");
    FILE *log = fopen("/tmp/errors_test.log", "w");
    set_error_callback(error_handler_file, log);
    traiter_donnees(200);
    if (log != NULL) {
        fclose(log);
    }
    printf("Erreur loggee dans le fichier\n");

    /* Test 4 : Callback avec statistiques */
    printf("\n=== Test 4 : Statistiques d'erreurs ===\n");
    StatistiquesErreurs stats = {0, 0};
    set_error_callback(error_handler_stats, &stats);
    traiter_donnees(-1);
    traiter_donnees(50);   /* OK */
    traiter_donnees(-2);
    traiter_donnees(150);
    printf("Total : %d erreurs, %d critiques\n",
           stats.nb_erreurs, stats.nb_critiques);

    /* Nettoyage */
    remove("/tmp/errors_test.log");

    return 0;
}
