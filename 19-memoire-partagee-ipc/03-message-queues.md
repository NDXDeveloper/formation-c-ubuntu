🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 19.3 Message Queues System V

## Introduction

Les **files de messages** (message queues) sont un mécanisme de communication inter-processus (IPC) qui permet d'échanger des données structurées entre processus de manière **asynchrone**. Contrairement aux pipes qui transmettent un flux brut d'octets, les message queues envoient des messages discrets et typés.

### Analogie du monde réel

Imaginez une **boîte aux lettres** :
- Plusieurs personnes peuvent **déposer** des lettres (messages)
- Une ou plusieurs personnes peuvent **récupérer** les lettres
- Chaque lettre a une **étiquette de catégorie** (type de message)
- Vous pouvez choisir de lire **uniquement certaines catégories** de lettres
- Les lettres restent dans la boîte jusqu'à ce qu'on les lise
- **Pas besoin d'être présent en même temps** : asynchrone !

C'est exactement le fonctionnement d'une message queue.

### Pourquoi les message queues ?

**Avantages :**
- ✅ **Communication asynchrone** : L'émetteur n'attend pas le récepteur
- ✅ **Messages typés** : Chaque message a un type, permettant la sélection
- ✅ **Ordre préservé** : FIFO par défaut (premier entré, premier sorti)
- ✅ **Buffer automatique** : Le noyau stocke les messages
- ✅ **Multi-producteurs/consommateurs** : Plusieurs processus peuvent envoyer/recevoir

**Cas d'usage typiques :**
- Systèmes de tâches distribuées (workers)
- Communication client-serveur asynchrone
- Systèmes de logging centralisés
- File d'attente de jobs/commandes
- Coordination de processus indépendants

---

## Concepts fondamentaux

### Architecture des Message Queues

```
┌──────────────┐       ┌──────────────┐       ┌──────────────┐
│ Processus A  │       │ Processus B  │       │ Processus C  │
│ (Producteur) │       │ (Producteur) │       │(Consommateur)│
└──────┬───────┘       └──────┬───────┘       └──────▲───────┘
       │ msgsnd()             │ msgsnd()             │ msgrcv()
       │                      │                      │
       └──────────────┬───────┴──────────────────────┘
                      │
              ┌───────▼────────────────────┐
              │   Message Queue (noyau)    │
              │                            │
              │  [Msg type=1, data="A"]    │  ← FIFO
              │  [Msg type=1, data="B"]    │
              │  [Msg type=2, data="X"]    │
              │  [Msg type=3, data="Y"]    │
              └────────────────────────────┘
```

**Caractéristiques importantes :**
1. Les messages sont stockés dans le **noyau** (pas dans l'espace utilisateur)
2. Chaque message a un **type** (entier > 0)
3. Les processus peuvent filtrer par type lors de la réception
4. La queue a une **taille maximale** (limite système)

### Différence avec les autres IPC

| Mécanisme | Mode | Données | Stockage |
|-----------|------|---------|----------|
| **Pipes** | Flux continu | Octets bruts | Buffer noyau limité |
| **Message Queues** | Messages discrets | Structures typées | Buffer noyau (files) |
| **Shared Memory** | Accès direct | Région mémoire | RAM partagée |
| **Sockets** | Flux/Datagram | Octets/paquets | Buffers réseau |

---

## L'API System V Message Queues

Comme les sémaphores System V, les message queues utilisent des clés IPC pour l'identification.

### Les 4 fonctions principales

1. **`msgget()`** - Créer ou obtenir une queue
2. **`msgsnd()`** - Envoyer un message
3. **`msgrcv()`** - Recevoir un message
4. **`msgctl()`** - Contrôler et supprimer une queue

---

### 1. `msgget()` - Créer ou obtenir une queue

```c
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

int msgget(key_t key, int msgflg);
```

**Paramètres :**
- `key` : Clé IPC (générée avec `ftok()` ou `IPC_PRIVATE`)
- `msgflg` : Flags de création
  - `IPC_CREAT` : Créer si n'existe pas
  - `IPC_EXCL` : Erreur si existe déjà (avec `IPC_CREAT`)
  - `0666` : Permissions (lecture/écriture)

**Retour :** Identifiant de la queue (msqid), ou -1 en cas d'erreur

**Exemple :**
```c
key_t key = ftok("/tmp/myqueue", 'Q');
int msqid = msgget(key, IPC_CREAT | 0666);
if (msqid == -1) {
    perror("msgget");
    exit(1);
}
printf("Queue créée : ID = %d\n", msqid);
```

---

### 2. `msgsnd()` - Envoyer un message

```c
int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
```

**Paramètres :**
- `msqid` : Identifiant de la queue (retourné par `msgget()`)
- `msgp` : Pointeur vers la structure du message
- `msgsz` : Taille des données du message (sans le type)
- `msgflg` : Flags
  - `0` : Bloquant (attend si queue pleine)
  - `IPC_NOWAIT` : Non-bloquant (retourne erreur si pleine)

**Structure du message :**
```c
struct msgbuf {
    long mtype;       // Type du message (> 0)
    char mtext[1];    // Données (taille variable)
};
```

⚠️ **Important** : `mtype` doit être **strictement positif** (> 0).

**Exemple :**
```c
struct message {
    long mtype;
    char text[100];
};

struct message msg;
msg.mtype = 1;  // Type du message
strcpy(msg.text, "Hello from sender!");

// Envoyer (taille = sizeof(msg.text), pas sizeof(msg) !)
if (msgsnd(msqid, &msg, sizeof(msg.text), 0) == -1) {
    perror("msgsnd");
    exit(1);
}
```

**Piège fréquent** : La taille passée à `msgsnd()` doit être la taille des **données uniquement**, pas de la structure complète (exclure `mtype`).

---

### 3. `msgrcv()` - Recevoir un message

```c
ssize_t msgrcv(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg);
```

**Paramètres :**
- `msqid` : Identifiant de la queue
- `msgp` : Pointeur vers la structure de réception
- `msgsz` : Taille maximale des données à recevoir
- `msgtyp` : **Filtre sur le type** (voir tableau ci-dessous)
- `msgflg` : Flags
  - `0` : Bloquant (attend un message)
  - `IPC_NOWAIT` : Non-bloquant (retourne erreur si vide)
  - `MSG_NOERROR` : Tronque si message trop grand (au lieu d'erreur)

**Filtrage par type (`msgtyp`) :**

| Valeur de `msgtyp` | Comportement |
|--------------------|--------------|
| `0` | Reçoit le **premier message** (peu importe le type) |
| `> 0` (ex: `5`) | Reçoit le premier message de **type = 5** |
| `< 0` (ex: `-3`) | Reçoit le premier message de **type ≤ 3** (priorité aux plus petits) |

**Retour :** Nombre d'octets reçus, ou -1 en cas d'erreur

**Exemple :**
```c
struct message {
    long mtype;
    char text[100];
};

struct message msg;

// Recevoir n'importe quel message
ssize_t bytes = msgrcv(msqid, &msg, sizeof(msg.text), 0, 0);
if (bytes == -1) {
    perror("msgrcv");
    exit(1);
}

printf("Type : %ld\n", msg.mtype);
printf("Message : %s\n", msg.text);
```

---

### 4. `msgctl()` - Contrôle de la queue

```c
int msgctl(int msqid, int cmd, struct msqid_ds *buf);
```

**Commandes principales :**

| Commande | Description |
|----------|-------------|
| `IPC_STAT` | Obtenir les informations de la queue |
| `IPC_SET` | Modifier les paramètres |
| `IPC_RMID` | **Supprimer la queue** |

**Structure d'information :**
```c
struct msqid_ds {
    // Informations utiles :
    unsigned long msg_qnum;    // Nombre de messages actuels
    unsigned long msg_qbytes;  // Taille max de la queue (octets)
    pid_t msg_lspid;           // PID du dernier msgsnd()
    pid_t msg_lrpid;           // PID du dernier msgrcv()
    time_t msg_stime;          // Heure du dernier msgsnd()
    time_t msg_rtime;          // Heure du dernier msgrcv()
};
```

**Exemples :**

```c
// Obtenir les infos
struct msqid_ds info;
msgctl(msqid, IPC_STAT, &info);
printf("Nombre de messages : %lu\n", info.msg_qnum);
printf("Taille max : %lu octets\n", info.msg_qbytes);

// Supprimer la queue
msgctl(msqid, IPC_RMID, NULL);
```

---

## Exemple complet : Communication simple

### Programme d'envoi (sender)

```c
// sender.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 100

// Structure du message
struct message {
    long mtype;           // Type (obligatoire, > 0)
    char text[MSG_SIZE];  // Contenu
};

int main() {
    key_t key;
    int msqid;
    struct message msg;

    // 1. Générer une clé unique
    key = ftok("/tmp", 'M');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    // 2. Créer ou obtenir la queue
    msqid = msgget(key, IPC_CREAT | 0666);
    if (msqid == -1) {
        perror("msgget");
        exit(1);
    }

    printf("Queue de messages créée (ID: %d)\n", msqid);

    // 3. Envoyer plusieurs messages
    for (int i = 1; i <= 5; i++) {
        msg.mtype = i % 2 + 1;  // Types alternés : 1, 2, 1, 2, 1
        snprintf(msg.text, MSG_SIZE, "Message numéro %d", i);

        printf("Envoi : type=%ld, texte=\"%s\"\n", msg.mtype, msg.text);

        // Envoyer (taille = sizeof données, PAS sizeof structure)
        if (msgsnd(msqid, &msg, sizeof(msg.text), 0) == -1) {
            perror("msgsnd");
            exit(1);
        }
    }

    printf("Tous les messages ont été envoyés.\n");

    return 0;
}
```

### Programme de réception (receiver)

```c
// receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 100

struct message {
    long mtype;
    char text[MSG_SIZE];
};

int main() {
    key_t key;
    int msqid;
    struct message msg;

    // 1. Obtenir la clé (même que sender)
    key = ftok("/tmp", 'M');
    if (key == -1) {
        perror("ftok");
        exit(1);
    }

    // 2. Obtenir la queue existante
    msqid = msgget(key, 0666);
    if (msqid == -1) {
        perror("msgget (la queue existe-t-elle ?)");
        exit(1);
    }

    printf("Connexion à la queue (ID: %d)\n", msqid);

    // 3. Recevoir tous les messages
    while (1) {
        // Recevoir n'importe quel type (msgtyp = 0)
        ssize_t bytes = msgrcv(msqid, &msg, sizeof(msg.text), 0, IPC_NOWAIT);

        if (bytes == -1) {
            // Plus de messages (queue vide)
            break;
        }

        printf("Reçu : type=%ld, texte=\"%s\"\n", msg.mtype, msg.text);
    }

    printf("Tous les messages ont été reçus.\n");

    // 4. Supprimer la queue
    if (msgctl(msqid, IPC_RMID, NULL) == -1) {
        perror("msgctl IPC_RMID");
    }

    return 0;
}
```

### Compilation et exécution

```bash
# Compiler
gcc sender.c -o sender
gcc receiver.c -o receiver

# Terminal 1 : Envoyer
./sender

# Terminal 2 : Recevoir (peut être fait immédiatement après ou plus tard)
./receiver
```

**Sortie du sender :**
```
Queue de messages créée (ID: 32768)
Envoi : type=1, texte="Message numéro 1"
Envoi : type=2, texte="Message numéro 2"
Envoi : type=1, texte="Message numéro 3"
Envoi : type=2, texte="Message numéro 4"
Envoi : type=1, texte="Message numéro 5"
Tous les messages ont été envoyés.
```

**Sortie du receiver :**
```
Connexion à la queue (ID: 32768)
Reçu : type=1, texte="Message numéro 1"
Reçu : type=2, texte="Message numéro 2"
Reçu : type=1, texte="Message numéro 3"
Reçu : type=2, texte="Message numéro 4"
Reçu : type=1, texte="Message numéro 5"
Tous les messages ont été reçus.
```

---

## Filtrage par type de message

Le paramètre `msgtyp` de `msgrcv()` permet de **sélectionner** les messages :

### Exemple : Serveur multi-clients avec priorités

```c
// server_priority.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define MSG_SIZE 100

// Types de messages
#define MSG_URGENT   1  // Priorité haute
#define MSG_NORMAL   2  // Priorité normale
#define MSG_LOW      3  // Priorité basse

struct message {
    long mtype;
    char text[MSG_SIZE];
};

int main() {
    key_t key = ftok("/tmp", 'P');
    int msqid = msgget(key, IPC_CREAT | 0666);
    struct message msg;

    printf("Serveur démarré (ID queue: %d)\n", msqid);
    printf("En attente de messages...\n\n");

    while (1) {
        // Recevoir les messages URGENTS en priorité (type <= 1)
        ssize_t bytes = msgrcv(msqid, &msg, sizeof(msg.text), -1, IPC_NOWAIT);

        if (bytes != -1) {
            printf("[URGENT] %s\n", msg.text);
            continue;
        }

        // Puis les messages NORMAUX (type = 2)
        bytes = msgrcv(msqid, &msg, sizeof(msg.text), MSG_NORMAL, IPC_NOWAIT);

        if (bytes != -1) {
            printf("[NORMAL] %s\n", msg.text);
            continue;
        }

        // Enfin les messages LOW (type = 3)
        bytes = msgrcv(msqid, &msg, sizeof(msg.text), MSG_LOW, IPC_NOWAIT);

        if (bytes != -1) {
            printf("[LOW]    %s\n", msg.text);
            continue;
        }

        // Queue vide, attendre un peu
        usleep(100000);  // 100ms
    }

    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}
```

### Client envoyant des messages de différentes priorités

```c
// client_priority.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 100
#define MSG_URGENT 1
#define MSG_NORMAL 2
#define MSG_LOW 3

struct message {
    long mtype;
    char text[MSG_SIZE];
};

void send_message(int msqid, long type, const char *text) {
    struct message msg;
    msg.mtype = type;
    strncpy(msg.text, text, MSG_SIZE);
    msgsnd(msqid, &msg, sizeof(msg.text), 0);
}

int main() {
    key_t key = ftok("/tmp", 'P');
    int msqid = msgget(key, 0666);

    // Envoyer dans un ordre mixte
    send_message(msqid, MSG_LOW, "Tâche de nettoyage");
    send_message(msqid, MSG_NORMAL, "Traiter la commande #123");
    send_message(msqid, MSG_URGENT, "ALERTE SÉCURITÉ !");
    send_message(msqid, MSG_LOW, "Archiver les logs");
    send_message(msqid, MSG_URGENT, "Crash du serveur DB !");

    printf("Messages envoyés avec différentes priorités\n");

    return 0;
}
```

**Résultat** : Le serveur traite d'abord tous les messages URGENT, puis NORMAL, puis LOW, indépendamment de leur ordre d'arrivée.

---

## Patterns courants

### 1. File de tâches (Task Queue)

```c
// worker.c - Consommateur de tâches
struct task_msg {
    long mtype;      // Type = 1 pour toutes les tâches
    int task_id;
    char command[256];
};

while (1) {
    struct task_msg task;

    // Attendre une tâche (bloquant)
    msgrcv(msqid, &task, sizeof(task) - sizeof(long), 1, 0);

    printf("Worker : Exécution tâche #%d : %s\n", task.task_id, task.command);

    // Simuler le traitement
    sleep(2);

    printf("Worker : Tâche #%d terminée\n", task.task_id);
}
```

### 2. Request-Response (Client-Serveur)

```c
// Structure pour requête
struct request {
    long mtype;      // Type = PID du client (pour réponse)
    int request_id;
    char query[256];
};

// Client
request.mtype = getpid();  // Utilise son PID comme type
msgsnd(msqid, &request, sizeof(request) - sizeof(long), 0);

// Attendre la réponse avec son PID
msgrcv(msqid, &response, sizeof(response) - sizeof(long), getpid(), 0);

// Serveur
// Recevoir toute requête
msgrcv(msqid, &req, sizeof(req) - sizeof(long), 0, 0);

// Traiter...

// Répondre avec le PID du client comme type
response.mtype = req.mtype;
msgsnd(msqid, &response, sizeof(response) - sizeof(long), 0);
```

### 3. Broadcast (Diffusion)

```c
// Tous les workers écoutent le même type
#define BROADCAST_TYPE 999

// Émetteur
struct broadcast_msg msg;
msg.mtype = BROADCAST_TYPE;
strcpy(msg.command, "SHUTDOWN");
msgsnd(msqid, &msg, sizeof(msg.text), 0);

// Récepteurs (plusieurs processus)
msgrcv(msqid, &msg, sizeof(msg.text), BROADCAST_TYPE, 0);
if (strcmp(msg.command, "SHUTDOWN") == 0) {
    printf("Arrêt demandé\n");
    exit(0);
}
```

---

## Inspection des message queues

### Commandes système

```bash
# Lister toutes les queues
ipcs -q

# Sortie exemple :
# ------ Message Queues --------
# key        msqid      owner      perms      used-bytes   messages
# 0x4d00054e 32768      user       666        500          5

# Détails d'une queue spécifique
ipcs -q -i 32768

# Supprimer une queue
ipcrm -q 32768
```

### Informations détaillées

```c
// Obtenir les statistiques
struct msqid_ds info;
msgctl(msqid, IPC_STAT, &info);

printf("Nombre de messages : %lu\n", info.msg_qnum);
printf("Taille totale utilisée : %lu octets\n", info.msg_cbytes);
printf("Taille max de la queue : %lu octets\n", info.msg_qbytes);
printf("Dernier envoi : PID %d\n", info.msg_lspid);
printf("Dernière réception : PID %d\n", info.msg_lrpid);
```

---

## Gestion des erreurs

### Erreurs courantes

```c
#include <errno.h>

// Envoi avec gestion d'erreurs complète
if (msgsnd(msqid, &msg, sizeof(msg.text), 0) == -1) {
    if (errno == EAGAIN) {
        fprintf(stderr, "Queue pleine (avec IPC_NOWAIT)\n");
    } else if (errno == EIDRM) {
        fprintf(stderr, "Queue supprimée par un autre processus\n");
    } else if (errno == EINVAL) {
        fprintf(stderr, "msqid invalide ou mtype <= 0\n");
    } else if (errno == EACCES) {
        fprintf(stderr, "Permissions insuffisantes\n");
    } else {
        perror("msgsnd");
    }
    exit(1);
}

// Réception avec gestion d'erreurs
if (msgrcv(msqid, &msg, sizeof(msg.text), 0, 0) == -1) {
    if (errno == E2BIG) {
        fprintf(stderr, "Message trop grand (utilisez MSG_NOERROR)\n");
    } else if (errno == ENOMSG) {
        fprintf(stderr, "Pas de message du type demandé (avec IPC_NOWAIT)\n");
    } else if (errno == EIDRM) {
        fprintf(stderr, "Queue supprimée pendant l'attente\n");
    } else {
        perror("msgrcv");
    }
}
```

### Message trop grand

```c
// Si le message peut être plus grand que le buffer
ssize_t bytes = msgrcv(msqid, &msg, SMALL_SIZE, 0, MSG_NOERROR);
// MSG_NOERROR : tronque le message au lieu de retourner E2BIG
```

---

## Limites système

Les message queues ont des limites imposées par le système :

```bash
# Voir les limites
cat /proc/sys/kernel/msgmax  # Taille max d'un message (8192 octets par défaut)
cat /proc/sys/kernel/msgmnb  # Taille max d'une queue (16384 octets par défaut)
cat /proc/sys/kernel/msgmni  # Nombre max de queues (32000 par défaut)
```

**Dépasser les limites :**
```c
// Si msgsnd() retourne EAGAIN avec IPC_NOWAIT
// → La queue est pleine (msg_cbytes >= msg_qbytes)

// Solutions :
// 1. Attendre (mode bloquant)
msgsnd(msqid, &msg, size, 0);  // Sans IPC_NOWAIT

// 2. Augmenter la taille de la queue (root requis)
struct msqid_ds info;
msgctl(msqid, IPC_STAT, &info);
info.msg_qbytes = 32768;  // Doubler la taille
msgctl(msqid, IPC_SET, &info);
```

---

## Comparaison avec autres IPC

### Message Queues vs Pipes

| Caractéristique | Message Queues | Pipes |
|----------------|----------------|-------|
| **Structure** | Messages discrets | Flux d'octets |
| **Types** | ✅ Messages typés | ❌ Non |
| **Filtrage** | ✅ Par type | ❌ Non |
| **Asynchrone** | ✅ Oui | ⚠️ Limité (buffer) |
| **Ordre** | ✅ FIFO par type | ✅ FIFO |
| **Complexité** | 🟡 Moyenne | 🟢 Faible |
| **Persistance** | ✅ Jusqu'à IPC_RMID | ❌ Disparaît avec le processus |

### Message Queues vs Shared Memory

| Caractéristique | Message Queues | Shared Memory |
|----------------|----------------|---------------|
| **Performance** | 🟡 Moyenne (copies) | ⚡ Très rapide |
| **Synchronisation** | ✅ Intégrée | ❌ Manuelle (sémaphores) |
| **Complexité** | 🟡 Moyenne | 🔴 Élevée |
| **Taille données** | ⚠️ Limitée (8KB par message) | ✅ Illimitée |
| **Use case** | Petits messages | Gros volumes |

### Quand utiliser les Message Queues ?

✅ **Utiliser quand :**
- Communication asynchrone nécessaire
- Messages de petite à moyenne taille (<8KB)
- Besoin de typage/filtrage de messages
- Plusieurs producteurs/consommateurs
- Simplicité préférée sur performance

❌ **Éviter quand :**
- Performance critique (→ Shared Memory)
- Très gros volumes de données (→ Shared Memory + sémaphores)
- Communication réseau (→ Sockets)
- Besoin de simplicité maximale (→ Pipes)

---

## Exemple avancé : Système de logging distribué

```c
// logger.c - Serveur de logging centralisé
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define LOG_ERROR   1
#define LOG_WARNING 2
#define LOG_INFO    3
#define LOG_DEBUG   4

struct log_msg {
    long mtype;        // Niveau de log
    pid_t sender_pid;
    time_t timestamp;
    char message[200];
};

const char* log_level_str(long level) {
    switch (level) {
        case LOG_ERROR: return "ERROR";
        case LOG_WARNING: return "WARN";
        case LOG_INFO: return "INFO";
        case LOG_DEBUG: return "DEBUG";
        default: return "UNKNOWN";
    }
}

int main(int argc, char *argv[]) {
    key_t key = ftok("/tmp", 'L');
    int msqid = msgget(key, IPC_CREAT | 0666);
    struct log_msg log;

    // Récupérer le niveau minimum à afficher (argument)
    long min_level = (argc > 1) ? atoi(argv[1]) : LOG_INFO;

    printf("Logger démarré (niveau min: %s)\n", log_level_str(min_level));
    printf("Listening...\n\n");

    while (1) {
        // Recevoir les messages de niveau >= min_level
        // msgtyp négatif = prend le premier message de type <= abs(msgtyp)
        ssize_t bytes = msgrcv(msqid, &log, sizeof(log) - sizeof(long),
                               -min_level, 0);

        if (bytes == -1) {
            perror("msgrcv");
            break;
        }

        // Formater et afficher
        char time_str[26];
        ctime_r(&log.timestamp, time_str);
        time_str[24] = '\0';  // Retirer le \n

        printf("[%s] [%s] [PID %d] %s\n",
               time_str,
               log_level_str(log.mtype),
               log.sender_pid,
               log.message);

        fflush(stdout);
    }

    msgctl(msqid, IPC_RMID, NULL);
    return 0;
}
```

### Application qui envoie des logs

```c
// app.c - Application qui envoie des logs
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define LOG_ERROR   1
#define LOG_WARNING 2
#define LOG_INFO    3
#define LOG_DEBUG   4

struct log_msg {
    long mtype;
    pid_t sender_pid;
    time_t timestamp;
    char message[200];
};

void send_log(int msqid, long level, const char *message) {
    struct log_msg log;
    log.mtype = level;
    log.sender_pid = getpid();
    log.timestamp = time(NULL);
    strncpy(log.message, message, sizeof(log.message) - 1);

    msgsnd(msqid, &log, sizeof(log) - sizeof(long), 0);
}

int main() {
    key_t key = ftok("/tmp", 'L');
    int msqid = msgget(key, 0666);

    printf("Application démarrée (PID %d)\n", getpid());

    // Simuler différents événements
    send_log(msqid, LOG_INFO, "Application démarrée");
    sleep(1);

    send_log(msqid, LOG_DEBUG, "Initialisation des modules");
    sleep(1);

    send_log(msqid, LOG_WARNING, "Configuration manquante, utilisation des défauts");
    sleep(1);

    send_log(msqid, LOG_ERROR, "Échec de connexion à la base de données");
    sleep(1);

    send_log(msqid, LOG_INFO, "Tentative de reconnexion...");
    sleep(2);

    send_log(msqid, LOG_INFO, "Connexion établie");
    sleep(1);

    send_log(msqid, LOG_DEBUG, "Traitement de 100 requêtes");

    printf("Logs envoyés\n");

    return 0;
}
```

### Utilisation

```bash
# Compiler
gcc logger.c -o logger
gcc app.c -o app

# Terminal 1 : Démarrer le logger (affiche INFO et plus critiques)
./logger 3

# Terminal 2 : Lancer l'application
./app

# Terminal 3 : Lancer une autre application
./app
```

**Sortie du logger :**
```
Logger démarré (niveau min: INFO)
Listening...

[Thu Nov 27 10:30:15 2025] [INFO] [PID 12345] Application démarrée
[Thu Nov 27 10:30:16 2025] [WARN] [PID 12345] Configuration manquante, utilisation des défauts
[Thu Nov 27 10:30:17 2025] [ERROR] [PID 12345] Échec de connexion à la base de données
[Thu Nov 27 10:30:18 2025] [INFO] [PID 12345] Tentative de reconnexion...
[Thu Nov 27 10:30:20 2025] [INFO] [PID 12345] Connexion établie
[Thu Nov 27 10:30:25 2025] [INFO] [PID 67890] Application démarrée
...
```

Les messages DEBUG ne s'affichent pas car le niveau minimum est INFO (3).

---

## Pièges et problèmes courants

### 1. Oublier la taille correcte

```c
struct message {
    long mtype;
    char text[100];
};

struct message msg;

// ❌ FAUX : inclut mtype dans la taille
msgsnd(msqid, &msg, sizeof(msg), 0);

// ✅ CORRECT : exclut mtype
msgsnd(msqid, &msg, sizeof(msg.text), 0);
// OU
msgsnd(msqid, &msg, sizeof(msg) - sizeof(long), 0);
```

### 2. Type de message = 0 ou négatif

```c
// ❌ INTERDIT : mtype doit être > 0
msg.mtype = 0;   // Erreur EINVAL
msg.mtype = -1;  // Erreur EINVAL

// ✅ CORRECT
msg.mtype = 1;   // Type valide
```

### 3. Ne pas supprimer les queues

Les queues persistent après la fin des processus :

```c
// ✅ Toujours nettoyer
msgctl(msqid, IPC_RMID, NULL);
```

Ou manuellement :
```bash
ipcrm -q $(ipcs -q | grep $USER | awk '{print $2}')
```

### 4. Dépasser la taille max

```c
// Si le message dépasse MSGMAX (8192 octets généralement)
// msgsnd() retourne EINVAL

// Vérifier avant :
if (data_size > MSGMAX) {
    fprintf(stderr, "Message trop grand : %zu > %d\n", data_size, MSGMAX);
    // Fragmenter le message ou utiliser shared memory
}
```

### 5. Deadlock avec filtrage

```c
// Processus A : attend type 1
msgrcv(msqid, &msg, size, 1, 0);

// Processus B : attend type 2
msgrcv(msqid, &msg, size, 2, 0);

// Si seul le type 3 arrive → DEADLOCK
// Solution : utiliser IPC_NOWAIT et timeout
```

---

## Bonnes pratiques

### ✅ À faire

1. **Toujours exclure `mtype`** de la taille dans `msgsnd()`/`msgrcv()`
2. **Vérifier que `mtype > 0`** avant d'envoyer
3. **Supprimer les queues** avec `IPC_RMID` quand terminé
4. **Gérer les erreurs** de chaque appel système
5. **Utiliser `ftok()`** pour générer des clés uniques
6. **Définir des constantes** pour les types de messages
7. **Documenter** le protocole de communication

### ❌ À éviter

1. **Ne pas** utiliser `sizeof(struct)` directement pour la taille
2. **Ne pas** oublier que les queues persistent
3. **Ne pas** utiliser `mtype = 0` ou négatif pour l'envoi
4. **Ne pas** ignorer les limites système (MSGMAX)
5. **Ne pas** mélanger plusieurs protocoles dans la même queue sans structure claire

---

## Alternatives modernes

### POSIX Message Queues

Une alternative plus moderne et portable :

```c
#include <mqueue.h>

// API plus simple
mqd_t mq = mq_open("/my_queue", O_CREAT | O_RDWR, 0666, NULL);
mq_send(mq, buffer, size, priority);
mq_receive(mq, buffer, size, &priority);
mq_close(mq);
mq_unlink("/my_queue");
```

**Avantages POSIX** :
- API plus propre et moderne
- Support des priorités intégré
- Notifications asynchrones
- Nommage avec paths (`/queue_name`)

**System V reste pertinent pour** :
- Code legacy
- Systèmes sans support POSIX complet
- Besoin de filtrage par type spécifique

---

## Debugging

### Script de surveillance

```bash
#!/bin/bash
# watch_queue.sh - Surveiller une queue en temps réel

MSQID=$1

while true; do
    clear
    echo "=== Queue $MSQID ==="
    ipcs -q -i $MSQID
    sleep 2
done
```

### Nettoyage complet

```bash
# Supprimer toutes les queues de l'utilisateur
for q in $(ipcs -q | grep $USER | awk '{print $2}'); do
    ipcrm -q $q
done
```

---

## Résumé

Les **message queues System V** permettent une communication asynchrone typée entre processus :

### Points clés

1. ✅ **API en 4 fonctions** : `msgget()`, `msgsnd()`, `msgrcv()`, `msgctl()`
2. ✅ **Messages typés** : Chaque message a un `mtype > 0`
3. ✅ **Filtrage puissant** : Sélection par type avec `msgrcv()`
4. ✅ **Asynchrone** : Émetteur et récepteur indépendants
5. ✅ **FIFO** : Ordre préservé par type
6. ⚠️ **Taille limitée** : ~8KB par message (MSGMAX)
7. ⚠️ **Persistance** : Ne disparaît pas automatiquement

### Cas d'usage idéaux

- ✅ Files de tâches distribuées
- ✅ Systèmes de logging centralisés
- ✅ Communication client-serveur asynchrone
- ✅ Coordination de workers
- ❌ Gros volumes de données (→ Shared Memory)
- ❌ Performance extrême (→ Shared Memory)

### Commandes essentielles

```bash
ipcs -q           # Lister les queues
ipcs -q -i <id>   # Détails d'une queue
ipcrm -q <id>     # Supprimer une queue
```

---

## Références

- Man pages : `man msgget`, `man msgsnd`, `man msgrcv`, `man msgctl`
- Stevens & Rago, *Advanced Programming in the UNIX Environment*, Chapitre 15
- POSIX.1-2001, POSIX.1-2008
- Linux Programmer's Manual

---

**Prochaine section** : 19.4 POSIX IPC vs System V IPC

⏭️ [POSIX IPC vs System V IPC](/19-memoire-partagee-ipc/04-posix-vs-system-v.md)
