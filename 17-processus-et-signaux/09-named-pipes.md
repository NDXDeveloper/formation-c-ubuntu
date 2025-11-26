🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 17.9 Named pipes (FIFO)

## Introduction

Un **named pipe** (aussi appelé **FIFO** pour "First In, First Out") est une extension des pipes anonymes qui possède un **nom dans le système de fichiers**. Cette caractéristique simple mais puissante permet à des processus **non apparentés** de communiquer entre eux.

**Analogie :** Un pipe anonyme est comme un tube invisible entre deux processus. Un FIFO est comme une boîte aux lettres publique : n'importe qui connaissant son adresse peut y déposer ou récupérer du courrier.

### Différences avec les pipes anonymes

| Caractéristique | Pipe anonyme | Named pipe (FIFO) |
|-----------------|--------------|-------------------|
| Nom dans le système | Non | Oui (fichier spécial) |
| Processus utilisateurs | Apparentés (parent-enfant) | N'importe lesquels |
| Création | `pipe()` | `mkfifo()` ou commande `mkfifo` |
| Persistance | Disparaît avec les processus | Reste dans le système de fichiers |
| Visibilité | Invisible | Visible avec `ls` |

### Quand utiliser les FIFO ?

Les FIFO sont parfaits pour :
- Communication entre processus **indépendants**
- Communication entre **scripts** shell
- Architecture **client-serveur** simple
- **Logging** centralisé
- **Communication IPC** simple et légère

## Qu'est-ce qu'un FIFO ?

### Un fichier spécial

Un FIFO apparaît comme un fichier dans le système de fichiers, mais c'est un **fichier spécial** :

```bash
$ ls -l mon_fifo
prw-r--r-- 1 user user 0 Nov 26 10:00 mon_fifo
#↑
#p = pipe (FIFO)
```

Le `p` au début indique que c'est un **pipe** (FIFO).

### Caractéristiques

- **Unidirectionnel** : Comme les pipes anonymes
- **Bloquant** : Lecture et écriture bloquent selon les conditions
- **Pas de stockage** : Les données transitent en mémoire, pas sur disque
- **Synchronisation** : L'ouverture bloque jusqu'à ce que les deux côtés soient ouverts
- **Taille nulle** : Le fichier a toujours une taille de 0 octets

## Création d'un FIFO

### Avec la commande `mkfifo`

La façon la plus simple de créer un FIFO :

```bash
# Créer un FIFO
mkfifo mon_fifo

# Vérifier sa création
ls -l mon_fifo
# prw-r--r-- 1 user user 0 Nov 26 10:00 mon_fifo

# Avec des permissions spécifiques
mkfifo -m 666 mon_fifo_public
```

### Avec la fonction `mkfifo()` en C

```c
#include <sys/types.h>
#include <sys/stat.h>

int mkfifo(const char *pathname, mode_t mode);
```

**Paramètres :**
- `pathname` : Chemin du FIFO à créer
- `mode` : Permissions (comme pour `chmod`)

**Retour :**
- `0` en cas de succès
- `-1` en cas d'erreur (ex: fichier existe déjà)

### Exemple : Créer un FIFO en C

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

int main(void) {
    const char *fifo_path = "/tmp/mon_fifo";

    // Créer le FIFO avec permissions rw-rw-rw-
    if (mkfifo(fifo_path, 0666) == -1) {
        if (errno == EEXIST) {
            printf("Le FIFO existe déjà\n");
        } else {
            perror("mkfifo");
            return 1;
        }
    } else {
        printf("FIFO créé avec succès: %s\n", fifo_path);
    }

    return 0;
}
```

**Sortie :**
```
FIFO créé avec succès: /tmp/mon_fifo
```

### Vérifier l'existence avant création

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

int create_fifo_if_needed(const char *path) {
    struct stat st;

    // Vérifier si le fichier existe
    if (stat(path, &st) == 0) {
        // Le fichier existe, vérifier si c'est un FIFO
        if (S_ISFIFO(st.st_mode)) {
            printf("FIFO existe déjà: %s\n", path);
            return 0;
        } else {
            fprintf(stderr, "Erreur: %s existe mais n'est pas un FIFO\n", path);
            return -1;
        }
    }

    // Le fichier n'existe pas, créer le FIFO
    if (mkfifo(path, 0666) == -1) {
        perror("mkfifo");
        return -1;
    }

    printf("FIFO créé: %s\n", path);
    return 0;
}

int main(void) {
    create_fifo_if_needed("/tmp/my_fifo");
    return 0;
}
```

## Utilisation d'un FIFO

### Ouverture avec `open()`

Un FIFO s'ouvre comme un fichier normal avec `open()` :

```c
#include <fcntl.h>
#include <unistd.h>

// Ouverture en lecture
int fd = open("/tmp/mon_fifo", O_RDONLY);

// Ouverture en écriture
int fd = open("/tmp/mon_fifo", O_WRONLY);
```

### Comportement bloquant de `open()`

⚠️ **IMPORTANT** : Par défaut, `open()` **bloque** jusqu'à ce que l'autre côté soit ouvert :

- `open(fifo, O_RDONLY)` bloque jusqu'à ce qu'un processus ouvre en **écriture**
- `open(fifo, O_WRONLY)` bloque jusqu'à ce qu'un processus ouvre en **lecture**

### Premier exemple : Deux programmes distincts

#### Programme 1 : Écrivain (`writer.c`)

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    const char *fifo_path = "/tmp/mon_fifo";

    printf("[Writer] Ouverture du FIFO en écriture...\n");
    printf("[Writer] (bloque jusqu'à ce qu'un lecteur se connecte)\n");

    int fd = open(fifo_path, O_WRONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("[Writer] Connecté! Envoi de messages...\n");

    const char *messages[] = {
        "Premier message",
        "Deuxième message",
        "Troisième message",
        NULL
    };

    for (int i = 0; messages[i] != NULL; i++) {
        write(fd, messages[i], strlen(messages[i]) + 1);
        printf("[Writer] Envoyé: %s\n", messages[i]);
        sleep(1);
    }

    close(fd);
    printf("[Writer] Terminé\n");

    return 0;
}
```

#### Programme 2 : Lecteur (`reader.c`)

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    const char *fifo_path = "/tmp/mon_fifo";

    printf("[Reader] Ouverture du FIFO en lecture...\n");
    printf("[Reader] (bloque jusqu'à ce qu'un écrivain se connecte)\n");

    int fd = open(fifo_path, O_RDONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("[Reader] Connecté! Réception de messages...\n");

    char buffer[100];
    ssize_t n;

    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        printf("[Reader] Reçu: %s\n", buffer);
    }

    close(fd);
    printf("[Reader] Terminé\n");

    return 0;
}
```

#### Utilisation

```bash
# Terminal 1 : Créer le FIFO
mkfifo /tmp/mon_fifo

# Terminal 2 : Lancer le lecteur (va bloquer)
./reader

# Terminal 3 : Lancer l'écrivain
./writer

# Les deux programmes communiquent maintenant!
```

## Mode non-bloquant

### Flag `O_NONBLOCK`

Pour éviter le blocage lors de l'ouverture :

```c
#include <fcntl.h>

int fd = open("/tmp/mon_fifo", O_WRONLY | O_NONBLOCK);

if (fd == -1) {
    if (errno == ENXIO) {
        printf("Aucun lecteur disponible\n");
    } else {
        perror("open");
    }
}
```

### Exemple : Vérifier la disponibilité

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

int main(void) {
    const char *fifo_path = "/tmp/mon_fifo";

    printf("Tentative d'ouverture non-bloquante...\n");

    int fd = open(fifo_path, O_WRONLY | O_NONBLOCK);

    if (fd == -1) {
        if (errno == ENXIO) {
            printf("Aucun lecteur connecté\n");
            printf("Attente d'un lecteur...\n");

            // Retenter en mode bloquant
            fd = open(fifo_path, O_WRONLY);

            if (fd != -1) {
                printf("Lecteur connecté!\n");
            }
        } else {
            perror("open");
            return 1;
        }
    } else {
        printf("Ouvert immédiatement (lecteur présent)\n");
    }

    if (fd != -1) {
        write(fd, "Message", 8);
        close(fd);
    }

    return 0;
}
```

## Communication bidirectionnelle

Comme pour les pipes anonymes, il faut **deux FIFO** pour une communication bidirectionnelle :

### Exemple : Client-Serveur

#### Serveur (`server.c`)

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#define FIFO_REQUEST  "/tmp/fifo_request"
#define FIFO_RESPONSE "/tmp/fifo_response"

int main(void) {
    // Créer les FIFO
    mkfifo(FIFO_REQUEST, 0666);
    mkfifo(FIFO_RESPONSE, 0666);

    printf("[Serveur] Démarrage...\n");

    // Ouvrir les FIFO
    int fd_req = open(FIFO_REQUEST, O_RDONLY);
    int fd_resp = open(FIFO_RESPONSE, O_WRONLY);

    printf("[Serveur] Prêt à recevoir des requêtes\n");

    char buffer[100];
    ssize_t n;

    while ((n = read(fd_req, buffer, sizeof(buffer))) > 0) {
        printf("[Serveur] Requête reçue: %s\n", buffer);

        // Traiter la requête (exemple: convertir en majuscules)
        for (int i = 0; buffer[i]; i++) {
            if (buffer[i] >= 'a' && buffer[i] <= 'z') {
                buffer[i] = buffer[i] - 'a' + 'A';
            }
        }

        // Envoyer la réponse
        char response[100];
        snprintf(response, sizeof(response), "Réponse: %s", buffer);
        write(fd_resp, response, strlen(response) + 1);

        printf("[Serveur] Réponse envoyée\n");
    }

    close(fd_req);
    close(fd_resp);

    // Nettoyer
    unlink(FIFO_REQUEST);
    unlink(FIFO_RESPONSE);

    printf("[Serveur] Arrêt\n");

    return 0;
}
```

#### Client (`client.c`)

```c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define FIFO_REQUEST  "/tmp/fifo_request"
#define FIFO_RESPONSE "/tmp/fifo_response"

int main(void) {
    printf("[Client] Connexion au serveur...\n");

    // Ouvrir les FIFO
    int fd_req = open(FIFO_REQUEST, O_WRONLY);
    int fd_resp = open(FIFO_RESPONSE, O_RDONLY);

    if (fd_req == -1 || fd_resp == -1) {
        perror("open");
        return 1;
    }

    printf("[Client] Connecté!\n");

    // Envoyer des requêtes
    const char *requests[] = {
        "hello",
        "world",
        "test",
        NULL
    };

    for (int i = 0; requests[i] != NULL; i++) {
        // Envoyer la requête
        printf("[Client] Envoi: %s\n", requests[i]);
        write(fd_req, requests[i], strlen(requests[i]) + 1);

        // Recevoir la réponse
        char buffer[100];
        read(fd_resp, buffer, sizeof(buffer));
        printf("[Client] Reçu: %s\n\n", buffer);
    }

    close(fd_req);
    close(fd_resp);

    printf("[Client] Déconnecté\n");

    return 0;
}
```

**Utilisation :**
```bash
# Terminal 1
./server

# Terminal 2
./client
```

## Gestion de plusieurs clients

### Problème : Un seul client à la fois

Par défaut, un FIFO ne peut gérer qu'un écrivain et un lecteur à la fois.

### Solution 1 : File d'attente

Les écrivains sont automatiquement mis en file d'attente :

```c
// writer1.c, writer2.c, writer3.c (identiques)
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <message>\n", argv[0]);
        return 1;
    }

    const char *fifo = "/tmp/queue_fifo";

    printf("[%d] Ouverture du FIFO...\n", getpid());
    int fd = open(fifo, O_WRONLY);

    printf("[%d] Envoi: %s\n", getpid(), argv[1]);
    write(fd, argv[1], strlen(argv[1]) + 1);

    close(fd);

    return 0;
}
```

```c
// reader.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    const char *fifo = "/tmp/queue_fifo";

    int fd = open(fifo, O_RDONLY);

    printf("[Reader] Réception des messages...\n");

    char buffer[100];
    ssize_t n;

    while ((n = read(fd, buffer, sizeof(buffer))) > 0) {
        printf("[Reader] Message: %s\n", buffer);
    }

    close(fd);

    return 0;
}
```

**Test :**
```bash
mkfifo /tmp/queue_fifo

# Terminal 1
./reader

# Terminal 2, 3, 4
./writer "Message 1"
./writer "Message 2"
./writer "Message 3"
```

### Solution 2 : FIFO par client

Chaque client utilise son propre FIFO de réponse :

```c
// Structure de requête
typedef struct {
    pid_t client_pid;
    char message[100];
} Request;

// Serveur
char fifo_response[100];
snprintf(fifo_response, sizeof(fifo_response),
         "/tmp/fifo_response_%d", request.client_pid);

// Client crée son FIFO de réponse
char my_fifo[100];
snprintf(my_fifo, sizeof(my_fifo), "/tmp/fifo_response_%d", getpid());
mkfifo(my_fifo, 0666);
```

## Scripts shell avec FIFO

### Exemple 1 : Logger simple

```bash
#!/bin/bash
# logger.sh

FIFO="/tmp/log_fifo"

# Créer le FIFO
mkfifo "$FIFO" 2>/dev/null

# Lire et logger
while true; do
    if read line < "$FIFO"; then
        echo "[$(date '+%Y-%m-%d %H:%M:%S')] $line" >> app.log
        echo "[LOG] $line"
    fi
done
```

```bash
#!/bin/bash
# send_log.sh

FIFO="/tmp/log_fifo"

# Envoyer un message de log
echo "Application started" > "$FIFO"
echo "Processing data..." > "$FIFO"
echo "Application finished" > "$FIFO"
```

### Exemple 2 : Pipeline shell

```bash
#!/bin/bash

FIFO="/tmp/data_fifo"
mkfifo "$FIFO"

# Producteur en arrière-plan
(
    for i in {1..10}; do
        echo "Ligne $i"
        sleep 1
    done
) > "$FIFO" &

# Consommateur
grep "5" < "$FIFO"

# Nettoyer
rm "$FIFO"
```

## Permissions et sécurité

### Définir les permissions

```c
#include <sys/stat.h>

// Lecture/Écriture pour propriétaire uniquement
mkfifo("/tmp/private_fifo", 0600);

// Lecture/Écriture pour tout le monde
mkfifo("/tmp/public_fifo", 0666);

// Lecture seule pour groupe
mkfifo("/tmp/readonly_fifo", 0640);
```

### Vérifier les permissions

```bash
# Voir les permissions
ls -l /tmp/mon_fifo

# Modifier les permissions
chmod 660 /tmp/mon_fifo
```

### Considérations de sécurité

⚠️ **Attention aux FIFO dans `/tmp`** :

```c
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>

// Créer un FIFO sécurisé dans le répertoire de l'utilisateur
int create_secure_fifo(char *path, size_t size) {
    const char *home = getenv("HOME");

    if (home == NULL) {
        fprintf(stderr, "HOME non défini\n");
        return -1;
    }

    // Créer dans ~/.fifos/
    snprintf(path, size, "%s/.fifos/myapp_fifo", home);

    // Créer le répertoire si nécessaire
    char dir[PATH_MAX];
    snprintf(dir, sizeof(dir), "%s/.fifos", home);
    mkdir(dir, 0700);

    // Créer le FIFO avec permissions restrictives
    if (mkfifo(path, 0600) == -1) {
        perror("mkfifo");
        return -1;
    }

    printf("FIFO sécurisé créé: %s\n", path);
    return 0;
}
```

## Suppression d'un FIFO

### Avec la commande `rm`

```bash
rm /tmp/mon_fifo
```

### Avec la fonction `unlink()`

```c
#include <unistd.h>

int unlink(const char *pathname);
```

**Exemple :**

```c
#include <stdio.h>
#include <unistd.h>

int main(void) {
    const char *fifo_path = "/tmp/mon_fifo";

    if (unlink(fifo_path) == 0) {
        printf("FIFO supprimé: %s\n", fifo_path);
    } else {
        perror("unlink");
        return 1;
    }

    return 0;
}
```

### Pattern : Nettoyage automatique

```c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

static const char *fifo_path = "/tmp/my_app_fifo";

void cleanup(int sig) {
    printf("\nNettoyage...\n");
    unlink(fifo_path);
    exit(0);
}

int main(void) {
    // Installer les handlers de nettoyage
    signal(SIGINT, cleanup);
    signal(SIGTERM, cleanup);

    // Créer le FIFO
    mkfifo(fifo_path, 0666);

    printf("FIFO créé. Appuyez sur Ctrl+C pour quitter.\n");

    // Programme principal...
    while (1) {
        sleep(1);
    }

    return 0;
}
```

## Cas d'usage pratiques

### 1. System Monitor

```c
// monitor.c - Affiche les statistiques système
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define FIFO_PATH "/tmp/monitor_fifo"

typedef struct {
    float cpu_usage;
    long memory_used;
    int process_count;
} SystemStats;

int main(void) {
    mkfifo(FIFO_PATH, 0666);

    printf("Monitor démarré. En attente de statistiques...\n");

    int fd = open(FIFO_PATH, O_RDONLY);

    SystemStats stats;

    while (read(fd, &stats, sizeof(SystemStats)) > 0) {
        printf("=== Statistiques Système ===\n");
        printf("CPU:       %.1f%%\n", stats.cpu_usage);
        printf("Mémoire:   %ld MB\n", stats.memory_used);
        printf("Processus: %d\n", stats.process_count);
        printf("============================\n\n");
    }

    close(fd);
    unlink(FIFO_PATH);

    return 0;
}
```

```c
// reporter.c - Envoie les statistiques
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

#define FIFO_PATH "/tmp/monitor_fifo"

typedef struct {
    float cpu_usage;
    long memory_used;
    int process_count;
} SystemStats;

int main(void) {
    int fd = open(FIFO_PATH, O_WRONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Simuler des statistiques
    for (int i = 0; i < 5; i++) {
        SystemStats stats = {
            .cpu_usage = 45.5 + i * 2,
            .memory_used = 4096 + i * 100,
            .process_count = 150 + i
        };

        write(fd, &stats, sizeof(SystemStats));
        sleep(2);
    }

    close(fd);

    return 0;
}
```

### 2. Task Queue

```c
// worker.c - Traite les tâches
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define TASK_FIFO "/tmp/task_queue"

typedef struct {
    int task_id;
    char command[100];
} Task;

int main(void) {
    mkfifo(TASK_FIFO, 0666);

    printf("[Worker] Prêt à traiter des tâches\n");

    int fd = open(TASK_FIFO, O_RDONLY);

    Task task;

    while (read(fd, &task, sizeof(Task)) > 0) {
        printf("[Worker] Traitement tâche #%d: %s\n",
               task.task_id, task.command);

        // Simuler le traitement
        sleep(2);

        printf("[Worker] Tâche #%d terminée\n", task.task_id);
    }

    close(fd);
    unlink(TASK_FIFO);

    return 0;
}
```

```c
// scheduler.c - Ajoute des tâches
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define TASK_FIFO "/tmp/task_queue"

typedef struct {
    int task_id;
    char command[100];
} Task;

int main(void) {
    int fd = open(TASK_FIFO, O_WRONLY);

    if (fd == -1) {
        perror("open");
        return 1;
    }

    const char *commands[] = {
        "backup_database",
        "generate_report",
        "send_emails",
        "cleanup_logs",
        NULL
    };

    for (int i = 0; commands[i] != NULL; i++) {
        Task task = {
            .task_id = i + 1
        };
        strncpy(task.command, commands[i], sizeof(task.command) - 1);

        printf("[Scheduler] Ajout tâche #%d: %s\n",
               task.task_id, task.command);

        write(fd, &task, sizeof(Task));
    }

    close(fd);

    return 0;
}
```

### 3. Event Logger

```c
// event_logger.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>

#define EVENT_FIFO "/tmp/event_log"

typedef struct {
    time_t timestamp;
    int severity;  // 0=INFO, 1=WARNING, 2=ERROR
    char message[200];
} Event;

int main(void) {
    mkfifo(EVENT_FIFO, 0666);

    FILE *logfile = fopen("events.log", "a");

    printf("Event Logger démarré\n");

    int fd = open(EVENT_FIFO, O_RDONLY);

    Event event;
    const char *severity_str[] = {"INFO", "WARNING", "ERROR"};

    while (read(fd, &event, sizeof(Event)) > 0) {
        char timestr[26];
        ctime_r(&event.timestamp, timestr);
        timestr[24] = '\0';  // Retirer '\n'

        printf("[%s] [%s] %s\n",
               timestr,
               severity_str[event.severity],
               event.message);

        fprintf(logfile, "[%s] [%s] %s\n",
                timestr,
                severity_str[event.severity],
                event.message);

        fflush(logfile);
    }

    fclose(logfile);
    close(fd);
    unlink(EVENT_FIFO);

    return 0;
}
```

## Debugging et diagnostic

### Vérifier l'état d'un FIFO

```bash
# Lister les FIFO
ls -lp /tmp | grep "^p"

# Voir qui a ouvert un FIFO
lsof /tmp/mon_fifo

# Tester manuellement
echo "test" > /tmp/mon_fifo  # Écriture
cat /tmp/mon_fifo           # Lecture
```

### Programme de test

```c
// test_fifo.c
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

void test_fifo(const char *path) {
    printf("Test du FIFO: %s\n", path);

    struct stat st;

    // Vérifier l'existence
    if (stat(path, &st) == -1) {
        printf("  Erreur: FIFO n'existe pas\n");
        return;
    }

    // Vérifier que c'est un FIFO
    if (!S_ISFIFO(st.st_mode)) {
        printf("  Erreur: N'est pas un FIFO\n");
        return;
    }

    printf("  ✓ FIFO existe\n");
    printf("  Permissions: %o\n", st.st_mode & 0777);

    // Tester l'ouverture non-bloquante
    int fd = open(path, O_WRONLY | O_NONBLOCK);

    if (fd == -1) {
        if (errno == ENXIO) {
            printf("  ⚠ Aucun lecteur connecté\n");
        } else {
            perror("  Erreur open");
        }
    } else {
        printf("  ✓ Ouverture réussie\n");
        close(fd);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <fifo_path>\n", argv[0]);
        return 1;
    }

    test_fifo(argv[1]);

    return 0;
}
```

## Erreurs courantes

### 1. Oublier de créer le FIFO

```c
// ❌ MAUVAIS
int fd = open("/tmp/mon_fifo", O_RDONLY);  // Échoue si n'existe pas

// ✅ BON
mkfifo("/tmp/mon_fifo", 0666);
int fd = open("/tmp/mon_fifo", O_RDONLY);
```

### 2. Blocage lors de l'ouverture

```c
// ❌ Peut bloquer indéfiniment
int fd = open("/tmp/mon_fifo", O_WRONLY);

// ✅ BON : Utiliser O_NONBLOCK pour tester
int fd = open("/tmp/mon_fifo", O_WRONLY | O_NONBLOCK);
if (fd == -1 && errno == ENXIO) {
    printf("Aucun lecteur\n");
}
```

### 3. Ne pas supprimer le FIFO

```c
// ✅ Toujours nettoyer
void cleanup(void) {
    unlink("/tmp/mon_fifo");
}

atexit(cleanup);
```

### 4. Permissions incorrectes

```c
// ❌ Trop restrictif
mkfifo("/tmp/shared_fifo", 0600);  // Seulement le propriétaire

// ✅ Pour partage entre utilisateurs
mkfifo("/tmp/shared_fifo", 0666);
```

### 5. Oublier de gérer SIGPIPE

```c
// ✅ Comme pour les pipes anonymes
signal(SIGPIPE, SIG_IGN);
```

## Comparaison détaillée

### Pipes vs FIFO

| Aspect | Pipe anonyme | FIFO (Named pipe) |
|--------|--------------|-------------------|
| **Création** | `pipe()` | `mkfifo()` ou commande |
| **Nom** | Aucun | Fichier dans système |
| **Visibilité** | Invisible | `ls` montre le fichier |
| **Processus** | Apparentés | N'importe lesquels |
| **Persistance** | Temporaire | Jusqu'à suppression |
| **Utilisation shell** | Automatique (`|`) | Manuelle |
| **Complexité** | Simple | Moyenne |

### Quand utiliser quoi ?

**Utilisez des pipes anonymes quand :**
- Communication parent-enfant
- Pipelines shell (`cmd1 | cmd2`)
- Processus lancés ensemble

**Utilisez des FIFO quand :**
- Processus indépendants
- Communication entre scripts
- Architecture client-serveur simple
- Besoin de persistance

## Alternatives aux FIFO

| IPC | Avantages | Inconvénients |
|-----|-----------|---------------|
| **Pipes anonymes** | Simple, rapide | Processus apparentés |
| **FIFO** | Processus indépendants | Unidirectionnel |
| **Sockets Unix** | Bidirectionnel, API riche | Plus complexe |
| **Shared memory** | Très rapide | Besoin de synchronisation |
| **Message queues** | Messages structurés | Plus lourd |
| **Sockets TCP/IP** | Réseau | Overhead réseau |

## Résumé

### Concepts clés

- Les FIFO sont des **pipes avec un nom** dans le système de fichiers
- Permettent la communication entre **processus indépendants**
- Comportement identique aux pipes anonymes une fois ouverts
- `open()` **bloque** par défaut jusqu'à connexion des deux côtés
- Doivent être **explicitement supprimés** avec `unlink()`

### Fonctions principales

| Fonction | Usage |
|----------|-------|
| `mkfifo(path, mode)` | Créer un FIFO |
| `open(path, flags)` | Ouvrir un FIFO |
| `read(fd, buf, n)` | Lire depuis le FIFO |
| `write(fd, buf, n)` | Écrire dans le FIFO |
| `close(fd)` | Fermer le descripteur |
| `unlink(path)` | Supprimer le FIFO |

### Pattern typique

```c
// Programme 1: Lecteur
mkfifo("/tmp/my_fifo", 0666);
int fd = open("/tmp/my_fifo", O_RDONLY);
read(fd, buffer, size);
close(fd);

// Programme 2: Écrivain
int fd = open("/tmp/my_fifo", O_WRONLY);
write(fd, data, size);
close(fd);

// Nettoyage
unlink("/tmp/my_fifo");
```

### Bonnes pratiques

1. ✅ Toujours vérifier si le FIFO existe avant de le créer
2. ✅ Utiliser `O_NONBLOCK` pour tester la disponibilité
3. ✅ Définir des permissions appropriées
4. ✅ Nettoyer le FIFO à la fin (`unlink()`)
5. ✅ Gérer `SIGPIPE` comme pour les pipes
6. ✅ Placer les FIFO dans des répertoires sécurisés
7. ✅ Documenter le protocole de communication

Les FIFO sont un mécanisme IPC simple et efficace, parfait pour la communication entre processus indépendants sur la même machine !

⏭️ [Threads et Concurrence (POSIX)](/18-threads-et-concurrence/README.md)
