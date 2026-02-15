🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 17.6 Signaux courants

## Introduction

Dans la section précédente, nous avons appris à gérer les signaux de manière générale. Cette section se concentre sur les **signaux les plus couramment utilisés** sous Linux, avec des exemples pratiques et des cas d'usage concrets pour chacun.

Comprendre ces signaux est essentiel pour :
- Écrire des applications qui se terminent proprement
- Déboguer des programmes
- Gérer des processus système
- Créer des services et daemons robustes

## Classification des signaux

Les signaux peuvent être classés en plusieurs catégories :

### 1. Signaux de terminaison
Demandent au processus de se terminer : `SIGTERM`, `SIGINT`, `SIGQUIT`, `SIGKILL`

### 2. Signaux d'erreur
Indiquent une erreur fatale : `SIGSEGV`, `SIGFPE`, `SIGILL`, `SIGBUS`

### 3. Signaux de contrôle de job
Gèrent l'exécution : `SIGSTOP`, `SIGCONT`, `SIGTSTP`

### 4. Signaux utilisateur
Librement utilisables : `SIGUSR1`, `SIGUSR2`

### 5. Signaux de communication
Notifications diverses : `SIGCHLD`, `SIGPIPE`, `SIGHUP`, `SIGALRM`

## SIGTERM (15) : Terminaison propre

### Description

**`SIGTERM`** est le signal de **terminaison standard**. C'est la méthode recommandée pour demander à un processus de se terminer proprement.

**Caractéristiques :**
- Numéro : 15
- Action par défaut : Terminer le processus
- Capturable : Oui
- Utilisation : Arrêt propre d'un service

### Quand utiliser SIGTERM ?

`SIGTERM` est le signal par défaut de la commande `kill` :

```bash
kill PID        # Envoie SIGTERM  
kill -15 PID    # Équivalent  
kill -TERM PID  # Équivalent  
```

### Exemple : Gestion de SIGTERM

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

volatile sig_atomic_t termination_requested = 0;

void sigterm_handler(int signum) {
    (void)signum;
    termination_requested = 1;
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigterm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sa, NULL);

    printf("Service démarré (PID: %d)\n", getpid());
    printf("Utilisez 'kill %d' pour l'arrêter proprement\n\n", getpid());

    // Ouvrir une ressource (fichier, connexion DB, etc.)
    FILE *log = fopen("service.log", "a");
    if (log == NULL) {
        perror("fopen");
        return 1;
    }

    fprintf(log, "Service démarré\n");
    fflush(log);

    int request_count = 0;

    while (!termination_requested) {
        // Simuler du travail
        printf("Traitement requête #%d\n", ++request_count);
        fprintf(log, "Requête #%d traitée\n", request_count);
        fflush(log);
        sleep(2);
    }

    // Nettoyage propre
    printf("\nSIGTERM reçu, arrêt en cours...\n");
    fprintf(log, "Arrêt propre après %d requêtes\n", request_count);

    fclose(log);

    printf("Nettoyage terminé, au revoir!\n");

    return 0;
}
```

**Points clés :**
- Le processus termine sa tâche en cours
- Les ressources sont libérées proprement
- Les fichiers sont fermés correctement
- Les logs sont écrits avant l'arrêt

## SIGINT (2) : Interruption clavier

### Description

**`SIGINT`** est généré quand l'utilisateur appuie sur **Ctrl+C** dans le terminal. Il demande l'interruption d'un programme.

**Caractéristiques :**
- Numéro : 2
- Action par défaut : Terminer le processus
- Capturable : Oui
- Génération : Ctrl+C au clavier

### Différence avec SIGTERM

| Aspect | SIGTERM | SIGINT |
|--------|---------|--------|
| Origine | Commande `kill` ou système | Clavier (Ctrl+C) |
| Usage | Scripts, services, automation | Interaction utilisateur |
| Intention | Arrêt planifié | Interruption interactive |

### Exemple : Confirmer avant de quitter

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t interrupt_count = 0;

void sigint_handler(int signum) {
    (void)signum;
    interrupt_count++;
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    printf("Programme démarré\n");
    printf("Appuyez 2 fois sur Ctrl+C pour quitter\n\n");

    int work_count = 0;

    while (1) {
        if (interrupt_count == 1) {
            printf("\nCtrl+C détecté. Appuyez encore une fois pour confirmer la sortie.\n");
            interrupt_count++;  // Pour éviter les répétitions du message
        }
        else if (interrupt_count >= 3) {
            printf("\nConfirmation reçue, sortie...\n");
            break;
        }

        printf("Travail en cours... (%d)\n", ++work_count);
        sleep(2);
    }

    printf("Nettoyage et sortie\n");

    return 0;
}
```

### Usage typique : Script interruptible

```c
#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t keep_running = 1;

void sigint_handler(int signum) {
    (void)signum;
    write(STDOUT_FILENO, "\nInterruption demandée...\n", 27);
    keep_running = 0;
}

int main(void) {
    signal(SIGINT, sigint_handler);

    printf("Traitement de 1000 éléments\n");
    printf("Ctrl+C pour interrompre proprement\n\n");

    for (int i = 1; i <= 1000 && keep_running; i++) {
        printf("\rTraitement: %d/1000", i);
        fflush(stdout);
        usleep(10000);  // 10ms par élément
    }

    printf("\n\nTraitement %s\n",
           keep_running ? "terminé" : "interrompu");

    return 0;
}
```

## SIGKILL (9) : Terminaison forcée

### Description

**`SIGKILL`** est le signal de **terminaison immédiate**. Il ne peut **pas** être intercepté, bloqué ou ignoré.

**Caractéristiques :**
- Numéro : 9
- Action : Tuer immédiatement
- Capturable : **NON** (impossible d'installer un handler)
- Utilisation : Dernier recours

### Quand utiliser SIGKILL ?

⚠️ **Utilisez SIGKILL uniquement en dernier recours !**

```bash
# D'abord, essayer SIGTERM (propre)
kill PID

# Attendre quelques secondes...
sleep 5

# Si le processus ne répond pas, SIGKILL
kill -9 PID  
kill -KILL PID  
```

### Conséquences de SIGKILL

❌ **Problèmes causés par SIGKILL :**
- Pas de nettoyage possible
- Fichiers peuvent rester ouverts
- Verrous non libérés
- Transactions interrompues
- Données potentiellement corrompues
- Processus enfants deviennent orphelins

### Exemple : Détection impossible

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int signum) {
    printf("Signal %d reçu\n", signum);
}

int main(void) {
    printf("PID: %d\n", getpid());

    // Installer un handler pour plusieurs signaux
    signal(SIGTERM, signal_handler);
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);
    signal(SIGUSR1, signal_handler);

    // Tenter d'installer un handler pour SIGKILL (IMPOSSIBLE)
    if (signal(SIGKILL, signal_handler) == SIG_ERR) {
        printf("Impossible d'intercepter SIGKILL (normal)\n");
    }

    // Tenter d'installer un handler pour SIGSTOP (IMPOSSIBLE)
    if (signal(SIGSTOP, signal_handler) == SIG_ERR) {
        printf("Impossible d'intercepter SIGSTOP (normal)\n");
    }

    printf("\nTestez les signaux:\n");
    printf("  kill -TERM %d  (sera intercepté)\n", getpid());
    printf("  kill -KILL %d  (tuera immédiatement)\n\n", getpid());

    while (1) {
        printf("En cours...\n");
        sleep(2);
    }

    return 0;
}
```

### Bonne pratique : Timeout avec escalade

```bash
#!/bin/bash
PID=$1

# Envoyer SIGTERM
echo "Envoi de SIGTERM à $PID..."  
kill -TERM $PID  

# Attendre jusqu'à 10 secondes
for i in {1..10}; do
    if ! kill -0 $PID 2>/dev/null; then
        echo "Processus terminé proprement"
        exit 0
    fi
    sleep 1
done

# Si toujours actif, SIGKILL
echo "Processus ne répond pas, envoi de SIGKILL..."  
kill -9 $PID  
```

## SIGQUIT (3) : Quitter avec core dump

### Description

**`SIGQUIT`** est généré par **Ctrl+\\** et demande au processus de quitter en générant un **core dump**.

**Caractéristiques :**
- Numéro : 3
- Génération : Ctrl+\\ au clavier
- Action par défaut : Terminer + core dump
- Capturable : Oui

### Core dump : Qu'est-ce que c'est ?

Un **core dump** est une image de la mémoire du processus au moment de sa terminaison, utile pour le débogage.

```bash
# Activer les core dumps
ulimit -c unlimited

# Après un SIGQUIT, le fichier core est créé
./mon_programme
# Ctrl+\
# Fichier 'core' créé

# Analyser avec gdb
gdb ./mon_programme core
```

### Exemple : Différence SIGINT vs SIGQUIT

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void signal_handler(int signum) {
    char msg[128];
    int len;

    if (signum == SIGINT) {
        len = snprintf(msg, sizeof(msg),
                      "\n[SIGINT] Ctrl+C détecté, mais je continue!\n");
    } else if (signum == SIGQUIT) {
        len = snprintf(msg, sizeof(msg),
                      "\n[SIGQUIT] Ctrl+\\ détecté, je quitte avec core dump!\n");
    } else {
        len = snprintf(msg, sizeof(msg),
                      "\n[Signal %d] reçu\n", signum);
    }

    write(STDOUT_FILENO, msg, len);

    if (signum == SIGQUIT) {
        // Restaurer le comportement par défaut et renvoyer le signal
        signal(SIGQUIT, SIG_DFL);
        raise(SIGQUIT);
    }
}

int main(void) {
    signal(SIGINT, signal_handler);
    signal(SIGQUIT, signal_handler);

    printf("Programme actif (PID: %d)\n", getpid());
    printf("  Ctrl+C  : Intercepté, continue\n");
    printf("  Ctrl+\\ : Quitte avec core dump\n\n");

    while (1) {
        printf("En cours...\n");
        sleep(2);
    }

    return 0;
}
```

## SIGUSR1 (10) et SIGUSR2 (12) : Signaux utilisateur

### Description

**`SIGUSR1`** et **`SIGUSR2`** sont des signaux **définis par l'utilisateur**. Leur signification dépend entièrement de l'application.

**Caractéristiques :**
- Numéros : 10 et 12
- Action par défaut : Terminer
- Capturable : Oui
- Usage : Communication personnalisée entre processus

### Cas d'usage courants

1. **Recharger la configuration** (SIGHUP est aussi utilisé)
2. **Afficher des statistiques**
3. **Changer le niveau de verbosité**
4. **Basculer entre modes de fonctionnement**
5. **Déclencher des actions personnalisées**

### Exemple : Statistiques et configuration

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

// Statistiques globales
volatile sig_atomic_t request_count = 0;  
volatile sig_atomic_t debug_mode = 0;  

void sigusr_handler(int signum) {
    char msg[256];
    int len;

    if (signum == SIGUSR1) {
        // SIGUSR1 : Afficher les statistiques
        time_t now = time(NULL);
        len = snprintf(msg, sizeof(msg),
                      "\n=== Statistiques ===\n"
                      "Requêtes traitées: %d\n"
                      "Mode debug: %s\n"
                      "Timestamp: %ld\n"
                      "====================\n\n",
                      request_count,
                      debug_mode ? "ON" : "OFF",
                      (long)now);
        write(STDOUT_FILENO, msg, len);
    }
    else if (signum == SIGUSR2) {
        // SIGUSR2 : Basculer le mode debug
        debug_mode = !debug_mode;
        len = snprintf(msg, sizeof(msg),
                      "\n[Config] Mode debug: %s\n\n",
                      debug_mode ? "ACTIVÉ" : "DÉSACTIVÉ");
        write(STDOUT_FILENO, msg, len);
    }
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigusr_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);

    printf("Service démarré (PID: %d)\n\n", getpid());
    printf("Commandes disponibles:\n");
    printf("  kill -USR1 %d  : Afficher les statistiques\n", getpid());
    printf("  kill -USR2 %d  : Basculer le mode debug\n\n", getpid());

    while (1) {
        request_count++;

        if (debug_mode) {
            printf("[DEBUG] Traitement requête #%d\n", request_count);
        } else {
            printf("Requête #%d\n", request_count);
        }

        sleep(1);
    }

    return 0;
}
```

**Test :**
```bash
# Terminal 1
$ ./service
Service démarré (PID: 12345)  
Requête #1  
Requête #2  

# Terminal 2
$ kill -USR2 12345  # Activer debug
$ kill -USR1 12345  # Voir les stats

# Terminal 1 (suite)
[Config] Mode debug: ACTIVÉ
[DEBUG] Traitement requête #3
[DEBUG] Traitement requête #4

=== Statistiques ===
Requêtes traitées: 4  
Mode debug: ON  
Timestamp: 1234567890  
====================
```

## SIGHUP (1) : Hangup (Déconnexion)

### Description

**`SIGHUP`** était historiquement envoyé quand un terminal se déconnectait. Aujourd'hui, il est couramment utilisé pour **recharger la configuration** d'un service.

**Caractéristiques :**
- Numéro : 1
- Origine historique : Déconnexion du terminal
- Usage moderne : Rechargement de configuration
- Capturable : Oui

### Usage moderne : Rechargement de configuration

De nombreux daemons Linux utilisent `SIGHUP` pour recharger leur configuration :

```bash
# Nginx
sudo kill -HUP $(cat /var/run/nginx.pid)

# Apache
sudo kill -HUP $(cat /var/run/apache2.pid)

# Votre service personnalisé
kill -HUP <PID>
```

### Exemple : Service avec rechargement

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

typedef struct {
    int port;
    char log_level[16];
    int max_connections;
} Config;

Config global_config;  
volatile sig_atomic_t reload_requested = 0;  

void load_configuration(void) {
    printf("\n[Config] Chargement de la configuration...\n");

    // Simuler la lecture d'un fichier de config
    global_config.port = 8080;
    strcpy(global_config.log_level, "INFO");
    global_config.max_connections = 100;

    printf("[Config] Port: %d\n", global_config.port);
    printf("[Config] Log level: %s\n", global_config.log_level);
    printf("[Config] Max connections: %d\n", global_config.max_connections);
    printf("[Config] Configuration chargée\n\n");
}

void sighup_handler(int signum) {
    (void)signum;
    reload_requested = 1;
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sighup_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGHUP, &sa, NULL);

    printf("Daemon démarré (PID: %d)\n", getpid());

    load_configuration();

    printf("Pour recharger la config: kill -HUP %d\n\n", getpid());

    while (1) {
        if (reload_requested) {
            reload_requested = 0;
            load_configuration();
        }

        printf("Service en cours (port %d)...\n", global_config.port);
        sleep(3);
    }

    return 0;
}
```

### SIGHUP et nohup

La commande `nohup` protège un processus de `SIGHUP` :

```bash
# Sans nohup : le processus se termine à la déconnexion
./mon_programme &
# Se déconnecter → processus tué

# Avec nohup : le processus continue
nohup ./mon_programme &
# Se déconnecter → processus continue
```

## SIGCHLD (17) : Enfant terminé

### Description

**`SIGCHLD`** est envoyé au processus parent quand un de ses enfants se termine ou change d'état.

**Caractéristiques :**
- Numéro : 17 (peut varier selon architecture)
- Envoyé par : Le noyau
- Action par défaut : Ignorer
- Capturable : Oui

### Usage principal : Éviter les zombies

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

void sigchld_handler(int signum) {
    (void)signum;

    int status;
    pid_t pid;

    // Récupérer tous les enfants terminés
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        char msg[128];
        int len;

        if (WIFEXITED(status)) {
            len = snprintf(msg, sizeof(msg),
                          "[SIGCHLD] Enfant %d terminé (code: %d)\n",
                          pid, WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            len = snprintf(msg, sizeof(msg),
                          "[SIGCHLD] Enfant %d tué par signal %d\n",
                          pid, WTERMSIG(status));
        } else {
            len = snprintf(msg, sizeof(msg),
                          "[SIGCHLD] Enfant %d changement d'état\n", pid);
        }

        write(STDOUT_FILENO, msg, len);
    }
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    printf("Serveur avec gestion automatique des enfants\n");
    printf("PID: %d\n\n", getpid());

    // Créer des processus enfants régulièrement
    for (int i = 1; i <= 5; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // ENFANT
            printf("[Enfant %d] Démarré (PID: %d)\n", i, getpid());
            sleep(i * 2);  // Durée variable
            printf("[Enfant %d] Terminé\n", i);
            exit(i);
        }

        printf("[Parent] Enfant %d créé (PID: %d)\n", i, pid);
    }

    printf("\n[Parent] Tous les enfants créés, en attente...\n\n");

    // Parent continue son travail
    sleep(15);

    printf("[Parent] Terminé\n");

    return 0;
}
```

### Flags utiles pour SIGCHLD

```c
// Ne pas recevoir SIGCHLD pour les enfants stoppés (SIGSTOP)
sa.sa_flags = SA_NOCLDSTOP;

// Ne pas créer de zombies du tout
sa.sa_flags = SA_NOCLDWAIT;

// Combiner les deux
sa.sa_flags = SA_RESTART | SA_NOCLDSTOP | SA_NOCLDWAIT;
```

## SIGALRM (14) : Alarme (timer)

### Description

**`SIGALRM`** est déclenché par la fonction `alarm()` après un délai spécifié.

**Caractéristiques :**
- Numéro : 14
- Déclenchement : Fonction `alarm()`
- Action par défaut : Terminer
- Capturable : Oui

### Exemple : Timeout sur une opération

```c
#define _XOPEN_SOURCE 600
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t timeout_occurred = 0;

void sigalrm_handler(int signum) {
    (void)signum;
    timeout_occurred = 1;
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigalrm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    printf("Opération avec timeout de 5 secondes\n");
    printf("Simulons une opération longue...\n\n");

    alarm(5);  // Timeout de 5 secondes

    int progress = 0;
    while (progress < 100 && !timeout_occurred) {
        printf("\rProgression: %d%%", progress);
        fflush(stdout);

        usleep(100000);  // 100ms
        progress += 1;
    }

    alarm(0);  // Annuler l'alarme si terminé à temps

    printf("\n\n");

    if (timeout_occurred) {
        printf("TIMEOUT! Opération trop longue.\n");
        return 1;
    } else {
        printf("Opération terminée avec succès.\n");
        return 0;
    }
}
```

### Exemple : Alarme périodique

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t alarm_count = 0;

void sigalrm_handler(int signum) {
    (void)signum;
    alarm_count++;
    alarm(1);  // Réarmer pour 1 seconde
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigalrm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGALRM, &sa, NULL);

    printf("Compteur avec alarme chaque seconde\n");
    printf("Ctrl+C pour arrêter\n\n");

    alarm(1);  // Première alarme dans 1 seconde

    while (alarm_count < 10) {
        printf("Alarmes reçues: %d\n", alarm_count);
        sleep(1);
    }

    alarm(0);  // Désactiver l'alarme

    printf("\nTerminé après 10 alarmes\n");

    return 0;
}
```

## SIGPIPE (13) : Écriture dans un pipe fermé

### Description

**`SIGPIPE`** est envoyé quand un processus tente d'écrire dans un pipe ou un socket dont l'autre extrémité est fermée.

**Caractéristiques :**
- Numéro : 13
- Déclenchement : Écriture dans pipe/socket fermé
- Action par défaut : Terminer
- Capturable : Oui

### Problème courant : Serveur réseau

Sans gestion de `SIGPIPE`, un serveur peut se terminer brutalement si un client se déconnecte :

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

int main(void) {
    // IMPORTANT : Ignorer SIGPIPE dans les applications réseau
    signal(SIGPIPE, SIG_IGN);

    printf("Serveur réseau (PID: %d)\n", getpid());
    printf("SIGPIPE ignoré, gestion via errno\n\n");

    // Simuler l'écriture dans un pipe fermé
    int pipefd[2];
    pipe(pipefd);

    close(pipefd[0]);  // Fermer la lecture

    printf("Tentative d'écriture dans un pipe fermé...\n");

    if (write(pipefd[1], "test", 4) == -1) {
        if (errno == EPIPE) {
            printf("EPIPE détecté : pipe fermé (mais pas de SIGPIPE!)\n");
            printf("Le programme continue normalement\n");
        }
    }

    close(pipefd[1]);

    printf("\nServeur toujours actif!\n");

    return 0;
}
```

### Bonne pratique : Applications réseau

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>

void setup_server_signals(void) {
    // Ignorer SIGPIPE - gérer les erreurs via EPIPE
    signal(SIGPIPE, SIG_IGN);

    // Gérer SIGTERM pour arrêt propre
    struct sigaction sa;
    sa.sa_handler = /* votre handler */;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGTERM, &sa, NULL);
}

ssize_t safe_write(int fd, const void *buf, size_t count) {
    ssize_t n = write(fd, buf, count);

    if (n == -1) {
        if (errno == EPIPE) {
            fprintf(stderr, "Client déconnecté\n");
            // Gérer la déconnexion proprement
        } else if (errno == EINTR) {
            // Signal reçu, réessayer
            return safe_write(fd, buf, count);
        }
    }

    return n;
}
```

## SIGSEGV (11) : Segmentation fault

### Description

**`SIGSEGV`** est envoyé quand un programme tente d'accéder à une zone mémoire invalide.

**Caractéristiques :**
- Numéro : 11
- Déclenchement : Violation d'accès mémoire
- Action par défaut : Terminer + core dump
- Capturable : Oui (mais dangereux)

### Causes courantes

1. Déréférencement d'un pointeur NULL
2. Accès hors limites d'un tableau
3. Accès à une mémoire libérée
4. Stack overflow

### Exemple : Déclenchement volontaire

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigsegv_handler(int signum) {
    (void)signum;
    write(STDERR_FILENO, "\nSIGSEGV capturé!\n", 19);
    write(STDERR_FILENO, "Erreur mémoire détectée\n", 27);
    _exit(1);  // Terminer immédiatement
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigsegv_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGSEGV, &sa, NULL);

    printf("Programme avec gestion SIGSEGV\n");
    printf("Déclenchement volontaire d'un segfault...\n\n");

    // Provoquer un SIGSEGV
    int *ptr = NULL;
    *ptr = 42;  // Déréférencement de NULL

    printf("Cette ligne ne sera jamais affichée\n");

    return 0;
}
```

**⚠️ IMPORTANT :** Dans la vraie vie, ne captez pas `SIGSEGV` sauf pour logger avant de terminer. Le programme est dans un état invalide et continuer est dangereux.

### Usage légitime : Logging avant crash

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <execinfo.h>
#include <stdlib.h>

void sigsegv_handler(int signum) {
    (void)signum;
    void *array[10];
    size_t size;

    write(STDERR_FILENO, "\n=== CRASH DÉTECTÉ ===\n", 25);

    // Obtenir la backtrace
    size = backtrace(array, 10);

    write(STDERR_FILENO, "Stack trace:\n", 13);
    backtrace_symbols_fd(array, size, STDERR_FILENO);

    write(STDERR_FILENO, "\n===================\n", 21);

    // Terminer immédiatement
    _exit(1);
}

int main(void) {
    signal(SIGSEGV, sigsegv_handler);

    // Code avec bug...
    int *ptr = NULL;
    *ptr = 42;

    return 0;
}
```

Compiler avec `-rdynamic` pour de meilleures backtraces :
```bash
gcc -rdynamic -o programme programme.c
```

## SIGFPE (8) : Erreur arithmétique

### Description

**`SIGFPE`** est envoyé lors d'erreurs arithmétiques graves (division par zéro, overflow, etc.).

**Caractéristiques :**
- Numéro : 8
- Déclenchement : Erreur mathématique
- Action par défaut : Terminer + core dump
- Capturable : Oui

### Exemple : Division par zéro

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>

static jmp_buf jump_buffer;

void sigfpe_handler(int signum) {
    (void)signum;
    write(STDERR_FILENO, "\nErreur: Division par zéro!\n", 28);
    longjmp(jump_buffer, 1);
}

int safe_divide(int a, int b, int *result) {
    struct sigaction sa, old_sa;

    sa.sa_handler = sigfpe_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGFPE, &sa, &old_sa);

    if (setjmp(jump_buffer) == 0) {
        *result = a / b;
        sigaction(SIGFPE, &old_sa, NULL);
        return 0;  // Succès
    } else {
        sigaction(SIGFPE, &old_sa, NULL);
        return -1;  // Erreur
    }
}

int main(void) {
    int result;

    if (safe_divide(10, 2, &result) == 0) {
        printf("10 / 2 = %d\n", result);
    }

    if (safe_divide(10, 0, &result) == -1) {
        printf("Division par zéro évitée\n");
    }

    printf("Programme continue normalement\n");

    return 0;
}
```

**Note :** En pratique, il vaut mieux vérifier `b != 0` avant de diviser plutôt que de capturer `SIGFPE`.

## Signaux de contrôle de job

### SIGSTOP (19) et SIGCONT (18)

**`SIGSTOP`** stoppe un processus, **`SIGCONT`** le relance.

```bash
# Stopper un processus
kill -STOP PID

# Vérifier qu'il est stoppé
ps aux | grep PID
# État : T (stopped)

# Relancer le processus
kill -CONT PID
```

**`SIGSTOP`** n'est **pas capturable** (comme `SIGKILL`).

### SIGTSTP (20) : Ctrl+Z

**`SIGTSTP`** est généré par **Ctrl+Z** et peut être intercepté (contrairement à `SIGSTOP`).

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigtstp_handler(int signum) {
    (void)signum;
    write(STDOUT_FILENO, "\nCtrl+Z détecté, mais je refuse de m'arrêter!\n", 49);
}

int main(void) {
    signal(SIGTSTP, sigtstp_handler);

    printf("Essayez Ctrl+Z...\n");

    while (1) {
        printf("En cours...\n");
        sleep(2);
    }

    return 0;
}
```

## Tableau récapitulatif

| Signal | Num | Origine | Capturable | Usage principal |
|--------|-----|---------|------------|-----------------|
| `SIGHUP` | 1 | Terminal/Admin | Oui | Rechargement config |
| `SIGINT` | 2 | Ctrl+C | Oui | Interruption utilisateur |
| `SIGQUIT` | 3 | Ctrl+\\ | Oui | Quitter + core dump |
| `SIGFPE` | 8 | Programme | Oui | Erreur arithmétique |
| `SIGKILL` | 9 | Admin | **Non** | Tuer immédiatement |
| `SIGUSR1` | 10 | Utilisateur | Oui | Signal personnalisé 1 |
| `SIGSEGV` | 11 | Programme | Oui* | Violation mémoire |
| `SIGUSR2` | 12 | Utilisateur | Oui | Signal personnalisé 2 |
| `SIGPIPE` | 13 | Système | Oui | Pipe/socket fermé |
| `SIGALRM` | 14 | alarm() | Oui | Timer/timeout |
| `SIGTERM` | 15 | Admin | Oui | Terminaison propre |
| `SIGCHLD` | 17 | Enfant | Oui | Enfant terminé |
| `SIGCONT` | 18 | Admin | Oui | Continuer |
| `SIGSTOP` | 19 | Admin | **Non** | Stopper |
| `SIGTSTP` | 20 | Ctrl+Z | Oui | Stopper (terminal) |

*Capturable mais dangereux - à utiliser uniquement pour logger avant crash

## Résumé et bonnes pratiques

### Terminaison propre

1. **Toujours gérer `SIGTERM`** pour permettre un arrêt propre
2. **Gérer `SIGINT`** pour les programmes interactifs
3. **Ne jamais utiliser `SIGKILL`** sauf en dernier recours

### Signals personnalisés

1. **`SIGUSR1` et `SIGUSR2`** pour communication personnalisée
2. **`SIGHUP`** pour rechargement de configuration (convention)

### Applications réseau

1. **Toujours ignorer `SIGPIPE`** : `signal(SIGPIPE, SIG_IGN);`
2. **Gérer `EPIPE`** lors des écritures

### Gestion des enfants

1. **Installer un handler `SIGCHLD`** pour éviter les zombies
2. **Utiliser `SA_NOCLDWAIT`** si les codes de retour ne vous intéressent pas

### Debugging

1. **`SIGSEGV`, `SIGFPE`, `SIGILL`** : Logger puis terminer
2. **Ne jamais continuer** après ces signaux

### Ordre d'arrêt recommandé

```
SIGTERM → attendre → SIGKILL
(propre)            (force)
```

La connaissance approfondie de ces signaux vous permet de créer des applications robustes qui communiquent correctement avec le système et les autres processus !

⏭️ [Masquage de signaux](/17-processus-et-signaux/07-masquage-signaux.md)
