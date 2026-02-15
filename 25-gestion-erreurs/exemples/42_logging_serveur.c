/* ============================================================================
   Section 25.6 : Logging
   Description : Serveur simule avec logging complet (fichier + console couleur)
   Fichier source : 06-logging.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <stdbool.h>

/* ========== SYSTEME DE LOGGING ========== */

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

static struct {
    FILE *fichier;
    LogLevel niveau_min;
    bool console_active;
    unsigned long nb_messages;
    unsigned long nb_erreurs;
} logger = {NULL, LOG_INFO, true, 0, 0};

static const char *niveau_noms[] = {
    "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"
};

static const char *niveau_couleurs[] = {
    "\033[36m",  /* DEBUG: Cyan */
    "\033[32m",  /* INFO: Vert */
    "\033[33m",  /* WARNING: Jaune */
    "\033[31m",  /* ERROR: Rouge */
    "\033[1;31m" /* FATAL: Rouge gras */
};

static const char *reset_couleur = "\033[0m";

bool logger_init(const char *fichier, LogLevel niveau) {
    logger.fichier = fopen(fichier, "a");
    if (logger.fichier == NULL) {
        fprintf(stderr, "ERREUR: Impossible d'ouvrir le fichier de log '%s'\n",
                fichier);
        return false;
    }

    logger.niveau_min = niveau;
    logger.nb_messages = 0;
    logger.nb_erreurs = 0;

    /* Message de demarrage */
    time_t now = time(NULL);
    fprintf(logger.fichier, "\n========================================\n");
    fprintf(logger.fichier, "Logging demarre : %s", ctime(&now));
    fprintf(logger.fichier, "Niveau minimum : %s\n", niveau_noms[niveau]);
    fprintf(logger.fichier, "========================================\n\n");
    fflush(logger.fichier);

    return true;
}

void logger_close(void) {
    if (logger.fichier != NULL) {
        fprintf(logger.fichier, "\n========================================\n");
        fprintf(logger.fichier, "Statistiques de logging:\n");
        fprintf(logger.fichier, "  Messages totaux: %lu\n", logger.nb_messages);
        fprintf(logger.fichier, "  Erreurs: %lu\n", logger.nb_erreurs);
        fprintf(logger.fichier, "========================================\n");

        fclose(logger.fichier);
        logger.fichier = NULL;
    }
}

void logger_log(LogLevel niveau, const char *fichier, int ligne,
                const char *format, ...) {
    if (niveau < logger.niveau_min) {
        return;
    }

    logger.nb_messages++;
    if (niveau >= LOG_ERROR) {
        logger.nb_erreurs++;
    }

    /* Timestamp */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[32];
    snprintf(timestamp, sizeof(timestamp), "%02d:%02d:%02d",
             t->tm_hour, t->tm_min, t->tm_sec);

    /* Message utilisateur */
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    /* Ecriture dans le fichier */
    if (logger.fichier != NULL) {
        fprintf(logger.fichier, "[%s] [%s] [%s:%d] %s\n",
                timestamp, niveau_noms[niveau], fichier, ligne, message);

        /* Flush pour les erreurs et fatals */
        if (niveau >= LOG_ERROR) {
            fflush(logger.fichier);
        }
    }

    /* Affichage console avec couleurs */
    if (logger.console_active) {
        fprintf(stderr, "%s[%s] [%s]%s %s\n",
                niveau_couleurs[niveau], timestamp, niveau_noms[niveau],
                reset_couleur, message);
    }
}

/* Macros */
#define LOG_DEBUG(...)   logger_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)    logger_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) logger_log(LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)   logger_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)   logger_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

/* ========== APPLICATION SERVEUR ========== */

typedef struct {
    int id;
    char nom[50];
    bool actif;
} Client;

bool connecter_client(Client *client) {
    LOG_INFO("Tentative de connexion du client '%s' (ID: %d)",
             client->nom, client->id);

    /* Simulation d'une connexion */
    if (client->id < 0) {
        LOG_ERROR("ID client invalide: %d", client->id);
        return false;
    }

    client->actif = true;
    LOG_INFO("Client '%s' connecte avec succes", client->nom);
    return true;
}

void deconnecter_client(Client *client) {
    if (client->actif) {
        LOG_INFO("Deconnexion du client '%s'", client->nom);
        client->actif = false;
    }
}

int traiter_requete(Client *client, const char *requete) {
    LOG_DEBUG("Requete recue de '%s': %s", client->nom, requete);

    if (!client->actif) {
        LOG_ERROR("Client '%s' non actif, requete rejetee", client->nom);
        return -1;
    }

    if (strlen(requete) == 0) {
        LOG_WARNING("Requete vide recue de '%s'", client->nom);
        return -1;
    }

    /* Traitement de la requete */
    LOG_DEBUG("Traitement de la requete pour '%s'...", client->nom);

    LOG_INFO("Requete traitee avec succes pour '%s'", client->nom);
    return 0;
}

int main(void) {
    /* Initialiser le logger */
    if (!logger_init("/tmp/serveur_test.log", LOG_DEBUG)) {
        return EXIT_FAILURE;
    }

    /* Desactiver la console pour un affichage propre */
    logger.console_active = false;

    LOG_INFO("===== DEMARRAGE DU SERVEUR =====");

    /* Creer quelques clients */
    Client clients[] = {
        {1, "Alice", false},
        {2, "Bob", false},
        {-1, "Charlie", false},  /* ID invalide */
    };

    /* Connexion des clients */
    for (size_t i = 0; i < 3; i++) {
        if (!connecter_client(&clients[i])) {
            LOG_WARNING("Echec de connexion pour le client ID %d",
                        clients[i].id);
        }
    }

    /* Traitement de requetes */
    LOG_INFO("===== TRAITEMENT DES REQUETES =====");

    traiter_requete(&clients[0], "GET /data");
    traiter_requete(&clients[1], "POST /update");
    traiter_requete(&clients[0], "");  /* Requete vide */
    traiter_requete(&clients[2], "GET /info");  /* Client non connecte */

    /* Deconnexion */
    LOG_INFO("===== DECONNEXION DES CLIENTS =====");
    for (size_t i = 0; i < 3; i++) {
        deconnecter_client(&clients[i]);
    }

    LOG_INFO("===== ARRET DU SERVEUR =====");

    logger_close();

    /* Afficher le contenu du fichier log */
    printf("=== Contenu du fichier serveur.log ===\n");
    FILE *f = fopen("/tmp/serveur_test.log", "r");
    if (f != NULL) {
        char ligne[2048];
        while (fgets(ligne, (int)sizeof(ligne), f) != NULL) {
            printf("%s", ligne);
        }
        fclose(f);
    }

    /* Nettoyage */
    remove("/tmp/serveur_test.log");

    return EXIT_SUCCESS;
}
