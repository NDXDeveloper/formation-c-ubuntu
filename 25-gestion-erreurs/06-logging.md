🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 25.6 Logging

## Introduction

Le **logging** (journalisation) est la pratique d'enregistrer des messages décrivant ce qui se passe dans un programme pendant son exécution. C'est un outil **essentiel** pour :

- **Déboguer** : Comprendre ce qui s'est passé quand un bug survient
- **Surveiller** : Observer le comportement en production
- **Auditer** : Tracer les actions importantes pour la sécurité ou la conformité
- **Diagnostiquer** : Analyser les performances et les problèmes

Contrairement aux assertions qui stoppent le programme, les logs **enregistrent** l'information et permettent au programme de continuer.

---

## Logging vs Printf : Pourquoi pas juste printf() ?

### Le problème avec printf()

```c
int main(void) {
    printf("Démarrage du programme\n");

    int resultat = traiter_donnees();
    printf("Résultat : %d\n", resultat);

    if (resultat < 0) {
        printf("Erreur détectée\n");
    }

    return 0;
}
```

**Problèmes :**
- ❌ Impossible de désactiver les messages en production
- ❌ Pas de niveau de sévérité (info vs erreur vs debug)
- ❌ Pas de timestamp automatique
- ❌ Pas de fichier de log
- ❌ Tout mélangé avec la sortie normale du programme

### Avec un système de logging

```c
int main(void) {
    log_init("app.log");

    LOG_INFO("Démarrage du programme");

    int resultat = traiter_donnees();
    LOG_DEBUG("Résultat : %d", resultat);

    if (resultat < 0) {
        LOG_ERROR("Erreur détectée");
    }

    log_close();
    return 0;
}
```

**Avantages :**
- ✅ Niveaux de sévérité distincts
- ✅ Horodatage automatique
- ✅ Sortie dans un fichier
- ✅ Configuration centralisée
- ✅ Filtrage par niveau

---

## Les niveaux de log

### Hiérarchie standard

Les systèmes de logging utilisent généralement une hiérarchie de niveaux :

| Niveau | Usage | Exemple |
|--------|-------|---------|
| **TRACE** | Détails ultra-fins pour tracer l'exécution | "Entrée dans fonction X avec param Y" |
| **DEBUG** | Informations de débogage | "Variable X vaut 42" |
| **INFO** | Informations générales | "Serveur démarré sur le port 8080" |
| **WARNING** | Situations anormales mais gérables | "Fichier de config absent, utilisation des valeurs par défaut" |
| **ERROR** | Erreurs qui empêchent une opération | "Impossible d'ouvrir le fichier data.txt" |
| **FATAL** | Erreurs critiques qui terminent le programme | "Mémoire insuffisante, arrêt immédiat" |

### Principe de filtrage

En définissant un **niveau minimum**, on ne garde que les messages de ce niveau ou supérieur :

```
Niveau minimum = INFO
→ On voit : INFO, WARNING, ERROR, FATAL
→ On ne voit pas : TRACE, DEBUG
```

**En développement** : Niveau DEBUG ou TRACE (tout voir)
**En production** : Niveau INFO ou WARNING (seulement l'essentiel)

---

## Implémentation simple d'un système de logging

### Version minimaliste

Commençons par une implémentation basique :

```c
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

// Niveaux de log
typedef enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

// Niveau minimum (configurable)
static LogLevel niveau_minimum = LOG_INFO;

// Noms des niveaux pour l'affichage
static const char *niveau_noms[] = {
    "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"
};

// Fonction principale de logging
void log_message(LogLevel niveau, const char *format, ...) {
    // Filtrage : ignorer si le niveau est inférieur au minimum
    if (niveau < niveau_minimum) {
        return;
    }

    // Horodatage
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    // Affichage du timestamp et du niveau
    printf("[%04d-%02d-%02d %02d:%02d:%02d] [%s] ",
           t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
           t->tm_hour, t->tm_min, t->tm_sec,
           niveau_noms[niveau]);

    // Affichage du message (avec support des arguments variables)
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);

    printf("\n");
}

// Macros pour simplifier l'utilisation
#define LOG_TRACE(...)   log_message(LOG_TRACE, __VA_ARGS__)
#define LOG_DEBUG(...)   log_message(LOG_DEBUG, __VA_ARGS__)
#define LOG_INFO(...)    log_message(LOG_INFO, __VA_ARGS__)
#define LOG_WARNING(...) log_message(LOG_WARNING, __VA_ARGS__)
#define LOG_ERROR(...)   log_message(LOG_ERROR, __VA_ARGS__)
#define LOG_FATAL(...)   log_message(LOG_FATAL, __VA_ARGS__)

int main(void) {
    // Configuration du niveau minimum
    niveau_minimum = LOG_DEBUG;

    LOG_TRACE("Ce message n'apparaîtra pas");
    LOG_DEBUG("Démarrage du programme");
    LOG_INFO("Serveur initialisé");
    LOG_WARNING("Fichier de config manquant");
    LOG_ERROR("Connexion échouée");
    LOG_FATAL("Erreur critique");

    return 0;
}
```

**Sortie :**
```
[2025-01-15 14:32:10] [DEBUG] Démarrage du programme
[2025-01-15 14:32:10] [INFO] Serveur initialisé
[2025-01-15 14:32:10] [WARNING] Fichier de config manquant
[2025-01-15 14:32:10] [ERROR] Connexion échouée
[2025-01-15 14:32:10] [FATAL] Erreur critique
```

---

## Logging dans des fichiers

### Version avec fichier de log

```c
#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <stdbool.h>

typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

// Configuration globale du logger
static struct {
    FILE *fichier;
    LogLevel niveau_min;
    bool console_active;
} logger = {NULL, LOG_INFO, true};

static const char *niveau_noms[] = {
    "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"
};

// Initialiser le logger
bool log_init(const char *nom_fichier, LogLevel niveau) {
    logger.fichier = fopen(nom_fichier, "a");  // Mode append
    if (logger.fichier == NULL) {
        return false;
    }

    logger.niveau_min = niveau;

    LOG_INFO("===== Démarrage du logging =====");

    return true;
}

// Fermer le logger
void log_close(void) {
    if (logger.fichier != NULL) {
        LOG_INFO("===== Arrêt du logging =====");
        fclose(logger.fichier);
        logger.fichier = NULL;
    }
}

// Activer/désactiver l'affichage console
void log_set_console(bool active) {
    logger.console_active = active;
}

// Obtenir le timestamp formaté
static void obtenir_timestamp(char *buffer, size_t taille) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    snprintf(buffer, taille, "%04d-%02d-%02d %02d:%02d:%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
}

// Fonction de logging
void log_message(LogLevel niveau, const char *fichier_source, int ligne,
                 const char *format, ...) {
    if (niveau < logger.niveau_min) {
        return;
    }

    char timestamp[32];
    obtenir_timestamp(timestamp, sizeof(timestamp));

    // Préparer le message
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    // Format : [timestamp] [NIVEAU] [fichier:ligne] message
    char log_line[2048];
    snprintf(log_line, sizeof(log_line), "[%s] [%s] [%s:%d] %s\n",
             timestamp, niveau_noms[niveau], fichier_source, ligne, message);

    // Écrire dans le fichier
    if (logger.fichier != NULL) {
        fputs(log_line, logger.fichier);
        fflush(logger.fichier);  // Forcer l'écriture immédiate
    }

    // Écrire sur la console si activé
    if (logger.console_active) {
        fputs(log_line, stderr);
    }
}

// Macros avec informations de fichier et ligne
#define LOG_DEBUG(...)   log_message(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)    log_message(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) log_message(LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)   log_message(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)   log_message(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

int main(void) {
    // Initialiser le logger
    if (!log_init("application.log", LOG_DEBUG)) {
        fprintf(stderr, "Erreur : impossible d'initialiser le logger\n");
        return 1;
    }

    LOG_INFO("Application démarrée");
    LOG_DEBUG("Mode debug activé");

    int valeur = 42;
    LOG_DEBUG("Valeur calculée : %d", valeur);

    FILE *f = fopen("inexistant.txt", "r");
    if (f == NULL) {
        LOG_ERROR("Impossible d'ouvrir le fichier inexistant.txt");
    } else {
        fclose(f);
    }

    LOG_WARNING("Cette opération est dépréciée");

    LOG_INFO("Application terminée avec succès");

    log_close();
    return 0;
}
```

**Contenu du fichier `application.log` :**
```
[2025-01-15 14:45:23] [INFO] [main.c:67] ===== Démarrage du logging =====
[2025-01-15 14:45:23] [INFO] [main.c:73] Application démarrée
[2025-01-15 14:45:23] [DEBUG] [main.c:74] Mode debug activé
[2025-01-15 14:45:23] [DEBUG] [main.c:77] Valeur calculée : 42
[2025-01-15 14:45:23] [ERROR] [main.c:81] Impossible d'ouvrir le fichier inexistant.txt
[2025-01-15 14:45:23] [WARNING] [main.c:85] Cette opération est dépréciée
[2025-01-15 14:45:23] [INFO] [main.c:87] Application terminée avec succès
[2025-01-15 14:45:23] [INFO] [main.c:51] ===== Arrêt du logging =====
```

---

## Fonctionnalités avancées

### 1. Logging avec contexte

Ajouter des informations de contexte aux logs :

```c
#include <unistd.h>

// Enrichir les logs avec le PID
void log_message_avec_contexte(LogLevel niveau, const char *format, ...) {
    if (niveau < logger.niveau_min) {
        return;
    }

    char timestamp[32];
    obtenir_timestamp(timestamp, sizeof(timestamp));

    // Message utilisateur
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    // Format avec PID
    fprintf(logger.fichier, "[%s] [PID:%d] [%s] %s\n",
            timestamp, getpid(), niveau_noms[niveau], message);
    fflush(logger.fichier);
}
```

**Sortie :**
```
[2025-01-15 14:50:12] [PID:12345] [INFO] Serveur démarré
```

### 2. Logging conditionnel

Activer le logging seulement pour certaines parties du code :

```c
// Activer/désactiver le logging par module
typedef struct {
    const char *nom;
    bool actif;
} Module;

static Module modules[] = {
    {"reseau", true},
    {"base_donnees", false},
    {"interface", true}
};

void log_module(const char *module, LogLevel niveau, const char *format, ...) {
    // Vérifier si le module est actif
    bool module_actif = false;
    for (size_t i = 0; i < sizeof(modules) / sizeof(modules[0]); i++) {
        if (strcmp(modules[i].nom, module) == 0) {
            module_actif = modules[i].actif;
            break;
        }
    }

    if (!module_actif || niveau < logger.niveau_min) {
        return;
    }

    // ... logging normal ...
}

// Utilisation
log_module("reseau", LOG_INFO, "Connexion établie");
log_module("base_donnees", LOG_DEBUG, "Requête SQL");  // Ignoré
```

### 3. Rotation des logs

Créer un nouveau fichier de log périodiquement :

```c
#include <time.h>
#include <string.h>

// Générer un nom de fichier avec timestamp
void generer_nom_fichier_log(char *buffer, size_t taille) {
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    snprintf(buffer, taille, "app_%04d%02d%02d_%02d%02d%02d.log",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
}

// Rotation manuelle
void log_rotate(void) {
    if (logger.fichier != NULL) {
        fclose(logger.fichier);
    }

    char nouveau_nom[256];
    generer_nom_fichier_log(nouveau_nom, sizeof(nouveau_nom));

    logger.fichier = fopen(nouveau_nom, "a");
    LOG_INFO("===== Rotation du log, nouveau fichier : %s =====", nouveau_nom);
}
```

### 4. Logging structuré (JSON)

Pour faciliter l'analyse automatique :

```c
void log_json(LogLevel niveau, const char *evenement, const char *format, ...) {
    if (niveau < logger.niveau_min) {
        return;
    }

    char timestamp[32];
    obtenir_timestamp(timestamp, sizeof(timestamp));

    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    fprintf(logger.fichier,
            "{\"timestamp\":\"%s\",\"level\":\"%s\",\"event\":\"%s\",\"message\":\"%s\"}\n",
            timestamp, niveau_noms[niveau], evenement, message);
    fflush(logger.fichier);
}

// Utilisation
log_json(LOG_INFO, "user_login", "Utilisateur %s connecté", "alice");
```

**Sortie :**
```json
{"timestamp":"2025-01-15 15:00:00","level":"INFO","event":"user_login","message":"Utilisateur alice connecté"}
```

---

## Logging thread-safe

Pour les applications multi-thread, le logging doit être **thread-safe** :

```c
#include <pthread.h>

// Mutex pour protéger l'accès au fichier de log
static pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;

void log_message_thread_safe(LogLevel niveau, const char *format, ...) {
    if (niveau < logger.niveau_min) {
        return;
    }

    // Verrouillage
    pthread_mutex_lock(&log_mutex);

    char timestamp[32];
    obtenir_timestamp(timestamp, sizeof(timestamp));

    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    // Écriture atomique
    fprintf(logger.fichier, "[%s] [%s] %s\n",
            timestamp, niveau_noms[niveau], message);
    fflush(logger.fichier);

    // Déverrouillage
    pthread_mutex_unlock(&log_mutex);
}
```

---

## Macros avancées

### Macros avec informations de contexte

```c
// Macros incluant fichier, ligne et fonction
#define LOG_DEBUG_CTX(...)   \
    log_message(LOG_DEBUG, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define LOG_INFO_CTX(...)    \
    log_message(LOG_INFO, __FILE__, __LINE__, __func__, __VA_ARGS__)

#define LOG_ERROR_CTX(...)   \
    log_message(LOG_ERROR, __FILE__, __LINE__, __func__, __VA_ARGS__)

// Fonction avec support du nom de fonction
void log_message(LogLevel niveau, const char *fichier, int ligne,
                 const char *fonction, const char *format, ...) {
    // ... code de logging ...
    fprintf(logger.fichier, "[%s] [%s] [%s:%s:%d] %s\n",
            timestamp, niveau_noms[niveau], fichier, fonction, ligne, message);
}
```

**Sortie :**
```
[2025-01-15 15:10:00] [DEBUG] [main.c:traiter_donnees:45] Traitement démarré
```

### Macros conditionnelles (compilation)

Désactiver complètement le logging DEBUG en production :

```c
#ifdef DEBUG_BUILD
    #define LOG_DEBUG(...)   log_message(LOG_DEBUG, __VA_ARGS__)
#else
    #define LOG_DEBUG(...)   ((void)0)  // Code vide
#endif

// LOG_INFO, LOG_ERROR, etc. restent toujours actifs
#define LOG_INFO(...)    log_message(LOG_INFO, __VA_ARGS__)
#define LOG_ERROR(...)   log_message(LOG_ERROR, __VA_ARGS__)
```

**Compilation :**
```bash
# Mode debug : tous les logs
gcc -DDEBUG_BUILD -o app main.c

# Mode release : pas de LOG_DEBUG
gcc -o app main.c
```

---

## Pattern : Logger centralisé

### Architecture modulaire

```c
// logger.h
#ifndef LOGGER_H
#define LOGGER_H

#include <stdbool.h>

typedef enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL
} LogLevel;

// API publique
bool logger_init(const char *fichier, LogLevel niveau_min);
void logger_close(void);
void logger_set_level(LogLevel niveau);
void logger_set_console(bool actif);
void logger_log(LogLevel niveau, const char *fichier, int ligne,
                const char *format, ...);

// Macros conviviales
#define LOG_TRACE(...)   logger_log(LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_DEBUG(...)   logger_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)    logger_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) logger_log(LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)   logger_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)   logger_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

#endif // LOGGER_H
```

```c
// logger.c
#include "logger.h"
#include <stdio.h>
#include <time.h>
#include <stdarg.h>

// Implémentation complète ici...
```

```c
// main.c
#include "logger.h"

int main(void) {
    logger_init("app.log", LOG_DEBUG);

    LOG_INFO("Application démarrée");
    LOG_DEBUG("Variable X = %d", 42);
    LOG_ERROR("Erreur détectée");

    logger_close();
    return 0;
}
```

---

## Bonnes pratiques

### 1. Choisir le bon niveau

```c
// ✅ BON
LOG_DEBUG("Variable x vaut %d", x);           // Debug
LOG_INFO("Serveur démarré sur le port %d", port);  // Info
LOG_WARNING("Fichier de config manquant, utilisation des valeurs par défaut");  // Warning
LOG_ERROR("Impossible de se connecter à la base de données");  // Error
LOG_FATAL("Mémoire insuffisante, arrêt");     // Fatal

// ❌ MAUVAIS
LOG_ERROR("Variable x vaut %d", x);           // Pas une erreur
LOG_INFO("Échec de connexion réseau");        // C'est une erreur
```

### 2. Messages informatifs

```c
// ❌ MAUVAIS : Message vague
LOG_ERROR("Erreur");

// ✅ BON : Message explicite
LOG_ERROR("Impossible d'ouvrir le fichier '%s': %s", nom_fichier, strerror(errno));

// ❌ MAUVAIS : Trop verbeux
LOG_DEBUG("La fonction traiter_donnees a été appelée avec les paramètres suivants...");

// ✅ BON : Concis mais informatif
LOG_DEBUG("traiter_donnees(id=%d, taille=%zu)", id, taille);
```

### 3. Éviter les informations sensibles

```c
// ❌ DANGER : Mot de passe en clair dans les logs
LOG_DEBUG("Connexion avec mot de passe: %s", password);

// ✅ BON : Masquer les informations sensibles
LOG_DEBUG("Connexion avec utilisateur: %s", username);

// Ou masquer partiellement
LOG_DEBUG("Carte bancaire: ****-****-****-%s", derniers_4_chiffres);
```

### 4. Logger les exceptions et erreurs critiques

```c
int traiter_fichier(const char *chemin) {
    FILE *f = fopen(chemin, "r");
    if (f == NULL) {
        LOG_ERROR("Impossible d'ouvrir '%s': %s", chemin, strerror(errno));
        return -1;
    }

    // ... traitement ...

    if (erreur_critique) {
        LOG_FATAL("Erreur critique dans le traitement de '%s'", chemin);
        abort();
    }

    fclose(f);
    return 0;
}
```

### 5. Ne pas logger dans les boucles intensives

```c
// ❌ MAUVAIS : Ralentit énormément
for (int i = 0; i < 1000000; i++) {
    LOG_DEBUG("Traitement élément %d", i);  // 1 million de logs !
    traiter_element(i);
}

// ✅ BON : Logger périodiquement ou en cas d'erreur
for (int i = 0; i < 1000000; i++) {
    if (i % 10000 == 0) {  // Tous les 10000 éléments
        LOG_INFO("Progression: %d/1000000", i);
    }

    if (traiter_element(i) != 0) {
        LOG_ERROR("Erreur sur élément %d", i);
    }
}
```

### 6. Flushing et buffering

```c
// Pour les logs critiques, forcer l'écriture immédiate
void log_flush(void) {
    if (logger.fichier != NULL) {
        fflush(logger.fichier);
    }
}

// Utiliser après les logs importants
LOG_FATAL("Erreur critique détectée");
log_flush();  // S'assurer que le message est écrit avant un crash
```

---

## Exemple complet : Serveur avec logging

Voici un exemple d'application complète avec un système de logging robuste :

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <stdbool.h>
#include <errno.h>

// ========== SYSTÈME DE LOGGING ==========

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
    "\033[36m",  // DEBUG: Cyan
    "\033[32m",  // INFO: Vert
    "\033[33m",  // WARNING: Jaune
    "\033[31m",  // ERROR: Rouge
    "\033[1;31m" // FATAL: Rouge gras
};

static const char *reset_couleur = "\033[0m";

bool logger_init(const char *fichier, LogLevel niveau) {
    logger.fichier = fopen(fichier, "a");
    if (logger.fichier == NULL) {
        fprintf(stderr, "ERREUR: Impossible d'ouvrir le fichier de log '%s'\n", fichier);
        return false;
    }

    logger.niveau_min = niveau;
    logger.nb_messages = 0;
    logger.nb_erreurs = 0;

    // Message de démarrage
    time_t now = time(NULL);
    fprintf(logger.fichier, "\n========================================\n");
    fprintf(logger.fichier, "Logging démarré : %s", ctime(&now));
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

    // Timestamp
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char timestamp[32];
    snprintf(timestamp, sizeof(timestamp), "%02d:%02d:%02d",
             t->tm_hour, t->tm_min, t->tm_sec);

    // Message utilisateur
    char message[1024];
    va_list args;
    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    // Écriture dans le fichier
    if (logger.fichier != NULL) {
        fprintf(logger.fichier, "[%s] [%s] [%s:%d] %s\n",
                timestamp, niveau_noms[niveau], fichier, ligne, message);

        // Flush pour les erreurs et fatals
        if (niveau >= LOG_ERROR) {
            fflush(logger.fichier);
        }
    }

    // Affichage console avec couleurs
    if (logger.console_active) {
        fprintf(stderr, "%s[%s] [%s]%s %s\n",
                niveau_couleurs[niveau], timestamp, niveau_noms[niveau],
                reset_couleur, message);
    }
}

// Macros
#define LOG_DEBUG(...)   logger_log(LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_INFO(...)    logger_log(LOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_WARNING(...) logger_log(LOG_WARNING, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_ERROR(...)   logger_log(LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define LOG_FATAL(...)   logger_log(LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

// ========== APPLICATION SERVEUR ==========

typedef struct {
    int id;
    char nom[50];
    bool actif;
} Client;

bool connecter_client(Client *client) {
    LOG_INFO("Tentative de connexion du client '%s' (ID: %d)", client->nom, client->id);

    // Simulation d'une connexion
    if (client->id < 0) {
        LOG_ERROR("ID client invalide: %d", client->id);
        return false;
    }

    client->actif = true;
    LOG_INFO("Client '%s' connecté avec succès", client->nom);
    return true;
}

void deconnecter_client(Client *client) {
    if (client->actif) {
        LOG_INFO("Déconnexion du client '%s'", client->nom);
        client->actif = false;
    }
}

int traiter_requete(Client *client, const char *requete) {
    LOG_DEBUG("Requête reçue de '%s': %s", client->nom, requete);

    if (!client->actif) {
        LOG_ERROR("Client '%s' non actif, requête rejetée", client->nom);
        return -1;
    }

    if (strlen(requete) == 0) {
        LOG_WARNING("Requête vide reçue de '%s'", client->nom);
        return -1;
    }

    // Traitement de la requête
    LOG_DEBUG("Traitement de la requête pour '%s'...", client->nom);

    // Simulation d'une erreur occasionnelle
    if (strcmp(requete, "crash") == 0) {
        LOG_FATAL("Commande 'crash' reçue, arrêt du serveur");
        return -2;
    }

    LOG_INFO("Requête traitée avec succès pour '%s'", client->nom);
    return 0;
}

int main(void) {
    // Initialiser le logger
    if (!logger_init("serveur.log", LOG_DEBUG)) {
        return EXIT_FAILURE;
    }

    LOG_INFO("===== DÉMARRAGE DU SERVEUR =====");

    // Créer quelques clients
    Client clients[] = {
        {1, "Alice", false},
        {2, "Bob", false},
        {-1, "Charlie", false},  // ID invalide
    };

    // Connexion des clients
    for (size_t i = 0; i < 3; i++) {
        if (!connecter_client(&clients[i])) {
            LOG_WARNING("Échec de connexion pour le client ID %d", clients[i].id);
        }
    }

    // Traitement de requêtes
    LOG_INFO("===== TRAITEMENT DES REQUÊTES =====");

    traiter_requete(&clients[0], "GET /data");
    traiter_requete(&clients[1], "POST /update");
    traiter_requete(&clients[0], "");  // Requête vide
    traiter_requete(&clients[2], "GET /info");  // Client non connecté

    // Déconnexion
    LOG_INFO("===== DÉCONNEXION DES CLIENTS =====");
    for (size_t i = 0; i < 3; i++) {
        deconnecter_client(&clients[i]);
    }

    LOG_INFO("===== ARRÊT DU SERVEUR =====");

    logger_close();
    return EXIT_SUCCESS;
}
```

**Contenu du fichier `serveur.log` :**
```
========================================
Logging démarré : Wed Jan 15 15:30:00 2025
Niveau minimum : DEBUG
========================================

[15:30:00] [INFO] [main.c:145] ===== DÉMARRAGE DU SERVEUR =====
[15:30:00] [INFO] [main.c:115] Tentative de connexion du client 'Alice' (ID: 1)
[15:30:00] [INFO] [main.c:124] Client 'Alice' connecté avec succès
[15:30:00] [INFO] [main.c:115] Tentative de connexion du client 'Bob' (ID: 2)
[15:30:00] [INFO] [main.c:124] Client 'Bob' connecté avec succès
[15:30:00] [INFO] [main.c:115] Tentative de connexion du client 'Charlie' (ID: -1)
[15:30:00] [ERROR] [main.c:119] ID client invalide: -1
[15:30:00] [WARNING] [main.c:154] Échec de connexion pour le client ID -1
[15:30:00] [INFO] [main.c:159] ===== TRAITEMENT DES REQUÊTES =====
[15:30:00] [DEBUG] [main.c:133] Requête reçue de 'Alice': GET /data
[15:30:00] [DEBUG] [main.c:144] Traitement de la requête pour 'Alice'...
[15:30:00] [INFO] [main.c:154] Requête traitée avec succès pour 'Alice'
[15:30:00] [DEBUG] [main.c:133] Requête reçue de 'Bob': POST /update
[15:30:00] [DEBUG] [main.c:144] Traitement de la requête pour 'Bob'...
[15:30:00] [INFO] [main.c:154] Requête traitée avec succès pour 'Bob'
[15:30:00] [DEBUG] [main.c:133] Requête reçue de 'Alice':
[15:30:00] [WARNING] [main.c:141] Requête vide reçue de 'Alice'
[15:30:00] [DEBUG] [main.c:133] Requête reçue de 'Charlie': GET /info
[15:30:00] [ERROR] [main.c:136] Client 'Charlie' non actif, requête rejetée
[15:30:00] [INFO] [main.c:167] ===== DÉCONNEXION DES CLIENTS =====
[15:30:00] [INFO] [main.c:130] Déconnexion du client 'Alice'
[15:30:00] [INFO] [main.c:130] Déconnexion du client 'Bob'
[15:30:00] [INFO] [main.c:171] ===== ARRÊT DU SERVEUR =====

========================================
Statistiques de logging:
  Messages totaux: 24
  Erreurs: 2
========================================
```

---

## Outils et bibliothèques existantes

Pour les projets plus importants, considérez des bibliothèques dédiées :

### 1. syslog (POSIX)

Système de logging standard UNIX/Linux :

```c
#include <syslog.h>

int main(void) {
    // Ouvrir la connexion syslog
    openlog("mon_app", LOG_PID | LOG_CONS, LOG_USER);

    syslog(LOG_INFO, "Application démarrée");
    syslog(LOG_ERR, "Erreur: %s", strerror(errno));

    // Fermer
    closelog();
    return 0;
}
```

**Avantages :** Standard, centralisé, rotation automatique
**Inconvénients :** Configuration système, moins de contrôle

### 2. log.c

Bibliothèque C simple et légère :

```bash
# Installation
wget https://raw.githubusercontent.com/rxi/log.c/master/src/log.c
wget https://raw.githubusercontent.com/rxi/log.c/master/src/log.h
```

```c
#include "log.h"

int main(void) {
    log_set_level(LOG_TRACE);
    log_trace("Trace message");
    log_debug("Debug message");
    log_info("Info message");
    log_warn("Warning message");
    log_error("Error message");
    log_fatal("Fatal message");

    return 0;
}
```

### 3. zlog

Bibliothèque de logging haute performance :

- Configuration par fichier
- Rotation automatique
- Formats multiples

---

## Checklist pour un bon système de logging

Votre système de logging devrait :

- [ ] Supporter plusieurs **niveaux** (DEBUG, INFO, WARNING, ERROR, FATAL)
- [ ] Inclure un **timestamp** dans chaque message
- [ ] Permettre de **filtrer** par niveau minimum
- [ ] Écrire dans un **fichier** (pas seulement la console)
- [ ] Être **thread-safe** si nécessaire
- [ ] Inclure des **informations de contexte** (fichier, ligne, fonction)
- [ ] Permettre le **formatage** comme printf
- [ ] **Flusher** les messages critiques immédiatement
- [ ] Être **désactivable** en compilation pour les logs DEBUG
- [ ] **Ne pas crasher** le programme en cas d'erreur de logging

---

## Résumé

### 🎯 Points clés

1. **Le logging ≠ printf()**
   - Niveaux de sévérité
   - Horodatage automatique
   - Fichiers de log persistants

2. **Hiérarchie des niveaux**
   - TRACE/DEBUG : Développement
   - INFO : Événements importants
   - WARNING : Situations anormales
   - ERROR/FATAL : Erreurs critiques

3. **Configuration par environnement**
   - Développement : LOG_DEBUG
   - Production : LOG_INFO ou LOG_WARNING

4. **Thread-safety**
   - Mutex pour protéger les écritures
   - Important en multi-thread

5. **Bonnes pratiques**
   - Messages clairs et informatifs
   - Pas d'informations sensibles
   - Flush sur les erreurs critiques
   - Ne pas logger dans les boucles intensives

### 💡 Règle d'or

> **Le logging est votre fenêtre sur ce qui se passe dans votre programme. Loggez intelligemment : ni trop, ni trop peu.**

### Quand logger ?

```
DEBUG    → Détails d'implémentation, valeurs de variables
INFO     → Événements importants (démarrage, arrêt, connexion)
WARNING  → Situation anormale mais gérable
ERROR    → Erreur qui empêche une opération
FATAL    → Erreur critique qui termine le programme
```

Le logging est un outil **indispensable** pour comprendre, déboguer et surveiller vos applications C en production. Un bon système de logging peut vous faire gagner des heures de débogage !

---

**Fin du chapitre 25 : Gestion des erreurs robuste**

⏭️ [Sécurité et Code défensif](/26-securite-code-defensif/README.md)
