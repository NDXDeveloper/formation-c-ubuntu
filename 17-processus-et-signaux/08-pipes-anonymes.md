🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 17.8 Pipes anonymes

## Introduction

Un **pipe** (prononcé "païpe", aussi appelé **tube** en français) est un mécanisme de **communication inter-processus** (IPC - Inter-Process Communication) sous Linux. Il permet à deux processus d'échanger des données de manière simple et efficace.

**Analogie :** Un pipe est comme un tuyau unidirectionnel : on écrit d'un côté et on lit de l'autre. L'eau (les données) ne peut circuler que dans un sens.

```
Processus A  →→→  [PIPE]  →→→  Processus B
  (écriture)                    (lecture)
```

### Caractéristiques des pipes anonymes

- **Unidirectionnels** : Les données circulent dans un seul sens
- **Anonymes** : N'ont pas de nom dans le système de fichiers
- **Entre processus apparentés** : Typiquement parent-enfant
- **Buffer limité** : Taille de buffer fixe (généralement 64 Ko sous Linux)
- **Synchronisation automatique** : Si le pipe est vide, la lecture bloque
- **Destruction automatique** : Disparaissent quand tous les descripteurs sont fermés

### Quand utiliser les pipes ?

Les pipes sont parfaits pour :
- Communication parent-enfant
- Chaîner des processus (comme dans le shell : `ls | grep txt`)
- Streaming de données entre processus
- Redirection d'entrées/sorties

## Concept fondamental

### Le pipe en image

```
         ÉCRITURE                LECTURE
            ↓                        ↓
    +-------+-------+        +-------+-------+
    | fd[1] |       |        |       | fd[0] |
    +-------+-------+        +-------+-------+
            ↓                        ↑
            |                        |
            ↓                        ↑
     +-----------------------------------+
     |                                   |
     |        BUFFER DU PIPE             |
     |         (dans le noyau)           |
     |                                   |
     +-----------------------------------+
```

Un pipe est créé avec **deux descripteurs de fichiers** :
- **`fd[0]`** : Lecture (extrémité de sortie)
- **`fd[1]`** : Écriture (extrémité d'entrée)

### Règle d'or

> **Ce qui est écrit dans `fd[1]` peut être lu depuis `fd[0]`**

## La fonction `pipe()`

### Prototype

```c
#include <unistd.h>

int pipe(int pipefd[2]);
```

**Paramètre :**
- `pipefd` : Tableau de 2 entiers qui recevra les descripteurs

**Retour :**
- `0` en cas de succès
- `-1` en cas d'erreur

**Après l'appel :**
- `pipefd[0]` : Descripteur pour la lecture
- `pipefd[1]` : Descripteur pour l'écriture

### Premier exemple : Créer un pipe

```c
#include <stdio.h>
#include <unistd.h>

int main(void) {
    int pipefd[2];

    // Créer le pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    printf("Pipe créé avec succès!\n");
    printf("Descripteur de lecture:  fd[0] = %d\n", pipefd[0]);
    printf("Descripteur d'écriture:  fd[1] = %d\n", pipefd[1]);

    // Fermer les descripteurs
    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}
```

**Sortie typique :**
```
Pipe créé avec succès!  
Descripteur de lecture:  fd[0] = 3  
Descripteur d'écriture:  fd[1] = 4  
```

## Communication dans un seul processus

Bien que peu utile en pratique, on peut écrire et lire dans le même processus pour comprendre le mécanisme :

```c
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int pipefd[2];
    char buffer[100];
    const char *message = "Hello via pipe!";

    // Créer le pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    printf("Envoi du message: \"%s\"\n", message);

    // Écrire dans le pipe
    write(pipefd[1], message, strlen(message) + 1);

    // Lire depuis le pipe
    read(pipefd[0], buffer, sizeof(buffer));

    printf("Message reçu: \"%s\"\n", buffer);

    // Fermer les descripteurs
    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}
```

**Sortie :**
```
Envoi du message: "Hello via pipe!"  
Message reçu: "Hello via pipe!"  
```

## Communication parent-enfant : Pattern de base

Le vrai intérêt des pipes est la communication entre processus. Voici le pattern standard :

### Pattern : Parent écrit, Enfant lit

```
1. Créer le pipe
2. fork()
3. Parent : fermer fd[0], écrire dans fd[1]
4. Enfant : fermer fd[1], lire depuis fd[0]
```

### Exemple complet

```c
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];
    pid_t pid;

    // 1. Créer le pipe AVANT le fork
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    // 2. Créer le processus enfant
    pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // === PROCESSUS ENFANT ===

        // Fermer l'extrémité d'écriture (on ne l'utilise pas)
        close(pipefd[1]);

        printf("[Enfant] En attente de données...\n");

        // Lire depuis le pipe
        char buffer[100];
        ssize_t n = read(pipefd[0], buffer, sizeof(buffer));

        if (n > 0) {
            printf("[Enfant] Message reçu: \"%s\"\n", buffer);
        }

        // Fermer l'extrémité de lecture
        close(pipefd[0]);

        return 0;
    }
    else {
        // === PROCESSUS PARENT ===

        // Fermer l'extrémité de lecture (on ne l'utilise pas)
        close(pipefd[0]);

        // Envoyer un message
        const char *message = "Bonjour de la part du parent!";

        printf("[Parent] Envoi du message: \"%s\"\n", message);

        write(pipefd[1], message, strlen(message) + 1);

        // Fermer l'extrémité d'écriture
        close(pipefd[1]);

        // Attendre l'enfant
        wait(NULL);

        printf("[Parent] Communication terminée\n");
    }

    return 0;
}
```

**Sortie :**
```
[Parent] Envoi du message: "Bonjour de la part du parent!"
[Enfant] En attente de données...
[Enfant] Message reçu: "Bonjour de la part du parent!"
[Parent] Communication terminée
```

### Diagramme du flux de données

```
APRÈS fork():

Parent                         Enfant  
fd[0] ─┐                   ┌─ fd[0]  
       │                   │
fd[1] ─┼─→ [PIPE] ─────────┼─→ LECTURE
       │                   │
     ÉCRITURE        close(fd[1])
```

## Importance de fermer les descripteurs

### Pourquoi fermer les descripteurs non utilisés ?

1. **Détection de fin de données (EOF)** : Un processus lecteur reçoit EOF seulement quand **tous** les descripteurs d'écriture sont fermés
2. **Libération de ressources** : Évite de gaspiller les descripteurs de fichiers
3. **Éviter les blocages** : Empêche les situations de deadlock

### Exemple : Problème sans fermeture

```c
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0) {
        // ENFANT
        // ❌ ERREUR: Ne ferme pas fd[1]
        // close(pipefd[1]);  <- manquant!

        char buffer[100];

        printf("[Enfant] Lecture...\n");

        // Cette lecture ne terminera JAMAIS (pas de EOF)
        // car fd[1] est toujours ouvert dans l'enfant
        while (read(pipefd[0], buffer, sizeof(buffer)) > 0) {
            printf("[Enfant] Données reçues\n");
        }

        printf("[Enfant] EOF reçu\n");  // N'arrive jamais!

        return 0;
    }
    else {
        // PARENT
        close(pipefd[0]);

        write(pipefd[1], "test", 5);
        close(pipefd[1]);

        wait(NULL);
    }

    return 0;
}
```

### Solution correcte

```c
if (fork() == 0) {
    // ENFANT
    close(pipefd[1]);  // ✅ IMPORTANT: Fermer fd[1]

    char buffer[100];
    ssize_t n;

    while ((n = read(pipefd[0], buffer, sizeof(buffer))) > 0) {
        printf("[Enfant] Données reçues: %zd octets\n", n);
    }

    printf("[Enfant] EOF reçu\n");  // Arrive correctement!
    close(pipefd[0]);

    return 0;
}
```

## Communication bidirectionnelle : Deux pipes

Pour communiquer dans les deux sens, il faut **deux pipes** :

```
Parent → Enfant : pipe1  
Enfant → Parent : pipe2  
```

### Exemple : Échange bidirectionnel

```c
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main(void) {
    int pipe_parent_to_child[2];  // Parent → Enfant
    int pipe_child_to_parent[2];  // Enfant → Parent

    // Créer les deux pipes
    if (pipe(pipe_parent_to_child) == -1 || pipe(pipe_child_to_parent) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == 0) {
        // === ENFANT ===

        // Fermer les extrémités non utilisées
        close(pipe_parent_to_child[1]);  // On n'écrit pas dans pipe1
        close(pipe_child_to_parent[0]);  // On ne lit pas depuis pipe2

        // Recevoir du parent
        char buffer[100];
        read(pipe_parent_to_child[0], buffer, sizeof(buffer));
        printf("[Enfant] Reçu du parent: \"%s\"\n", buffer);

        // Envoyer au parent
        const char *response = "Message de l'enfant";
        write(pipe_child_to_parent[1], response, strlen(response) + 1);
        printf("[Enfant] Envoyé au parent: \"%s\"\n", response);

        // Fermer les descripteurs
        close(pipe_parent_to_child[0]);
        close(pipe_child_to_parent[1]);

        return 0;
    }
    else {
        // === PARENT ===

        // Fermer les extrémités non utilisées
        close(pipe_parent_to_child[0]);  // On ne lit pas depuis pipe1
        close(pipe_child_to_parent[1]);  // On n'écrit pas dans pipe2

        // Envoyer à l'enfant
        const char *message = "Message du parent";
        write(pipe_parent_to_child[1], message, strlen(message) + 1);
        printf("[Parent] Envoyé à l'enfant: \"%s\"\n", message);

        // Recevoir de l'enfant
        char buffer[100];
        read(pipe_child_to_parent[0], buffer, sizeof(buffer));
        printf("[Parent] Reçu de l'enfant: \"%s\"\n", buffer);

        // Fermer les descripteurs
        close(pipe_parent_to_child[1]);
        close(pipe_child_to_parent[0]);

        wait(NULL);
    }

    return 0;
}
```

**Sortie :**
```
[Parent] Envoyé à l'enfant: "Message du parent"
[Enfant] Reçu du parent: "Message du parent"
[Enfant] Envoyé au parent: "Message de l'enfant"
[Parent] Reçu de l'enfant: "Message de l'enfant"
```

## Comportement bloquant

### Lecture bloquante

Si le pipe est **vide**, `read()` **bloque** jusqu'à ce que des données soient disponibles (ou EOF).

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0) {
        // ENFANT
        close(pipefd[1]);

        printf("[Enfant] Lecture (va bloquer 3 secondes)...\n");

        char buffer[100];
        read(pipefd[0], buffer, sizeof(buffer));  // BLOQUE

        printf("[Enfant] Données reçues: \"%s\"\n", buffer);
        close(pipefd[0]);

        return 0;
    }
    else {
        // PARENT
        close(pipefd[0]);

        printf("[Parent] Attente de 3 secondes avant d'écrire...\n");
        sleep(3);

        printf("[Parent] Écriture dans le pipe\n");
        write(pipefd[1], "Données!", 9);

        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}
```

### Écriture bloquante

Si le **buffer du pipe est plein**, `write()` **bloque** jusqu'à ce qu'il y ait de la place.

```c
#include <stdio.h>
#include <unistd.h>

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    printf("Remplissage du pipe jusqu'à ce qu'il soit plein...\n");

    char data[1024];
    size_t total = 0;

    // Écrire jusqu'au blocage (ou erreur)
    while (1) {
        ssize_t n = write(pipefd[1], data, sizeof(data));

        if (n == -1) {
            perror("write");
            break;
        }

        total += n;
        printf("Écrit: %zu Ko\n", total / 1024);

        // Arrêter après ~100 Ko pour l'exemple
        if (total > 100 * 1024) {
            break;
        }
    }

    printf("Total écrit: %zu Ko\n", total / 1024);

    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}
```

### Mode non-bloquant (avancé)

On peut rendre un pipe non-bloquant avec `fcntl()` :

```c
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    // Rendre la lecture non-bloquante
    int flags = fcntl(pipefd[0], F_GETFL);
    fcntl(pipefd[0], F_SETFL, flags | O_NONBLOCK);

    char buffer[100];
    ssize_t n = read(pipefd[0], buffer, sizeof(buffer));

    if (n == -1 && errno == EAGAIN) {
        printf("Pipe vide (lecture non-bloquante)\n");
    }

    close(pipefd[0]);
    close(pipefd[1]);

    return 0;
}
```

## Signal SIGPIPE

### Le problème

Si on écrit dans un pipe dont **tous les lecteurs sont fermés**, le processus reçoit **`SIGPIPE`** et se termine par défaut.

```c
#include <stdio.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    // Fermer l'extrémité de lecture
    close(pipefd[0]);

    printf("Tentative d'écriture dans un pipe sans lecteur...\n");

    // Cette écriture génère SIGPIPE
    if (write(pipefd[1], "test", 4) == -1) {
        perror("write");
        printf("Cette ligne ne s'affichera probablement pas\n");
    }

    close(pipefd[1]);

    return 0;
}
```

**Sans gestion :** Le programme se termine avec le message `Broken pipe`.

### Solution : Ignorer SIGPIPE

```c
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    // Ignorer SIGPIPE
    signal(SIGPIPE, SIG_IGN);

    close(pipefd[0]);

    printf("Tentative d'écriture dans un pipe sans lecteur...\n");

    if (write(pipefd[1], "test", 4) == -1) {
        if (errno == EPIPE) {
            printf("Erreur EPIPE détectée (pas de lecteur)\n");
            printf("Le programme continue normalement\n");
        }
    }

    close(pipefd[1]);

    return 0;
}
```

## Redirection avec `dup2()`

### Concept

`dup2()` permet de **rediriger** un descripteur vers un autre. C'est la base de la redirection shell.

```c
#include <unistd.h>

int dup2(int oldfd, int newfd);
```

**Effet :** Après `dup2(oldfd, newfd)`, `newfd` devient une copie de `oldfd`.

### Exemple : Rediriger stdout vers un pipe

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0) {
        // ENFANT : va écrire dans stdout, mais stdout → pipe

        close(pipefd[0]);  // Ne lit pas

        // Rediriger stdout vers le pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);  // Fermer l'original (on a la copie)

        // Maintenant, printf() écrit dans le pipe!
        printf("Ce message va dans le pipe!\n");
        printf("Ligne 2 dans le pipe!\n");

        return 0;
    }
    else {
        // PARENT : lit depuis le pipe

        close(pipefd[1]);  // N'écrit pas

        char buffer[200];
        ssize_t n;

        printf("[Parent] Lecture depuis le pipe:\n");

        while ((n = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[n] = '\0';
            printf("%s", buffer);
        }

        close(pipefd[0]);
        wait(NULL);
    }

    return 0;
}
```

**Sortie :**
```
[Parent] Lecture depuis le pipe:
Ce message va dans le pipe!  
Ligne 2 dans le pipe!  
```

## Simuler une commande shell : `ls | wc`

### Exemple complet

Voici comment reproduire `ls | wc` en C :

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    // Premier enfant : ls
    pid_t pid1 = fork();

    if (pid1 == 0) {
        // ENFANT 1 : ls

        // Rediriger stdout vers le pipe
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Exécuter ls
        execlp("ls", "ls", "-l", NULL);

        perror("execlp ls");
        return 1;
    }

    // Deuxième enfant : wc
    pid_t pid2 = fork();

    if (pid2 == 0) {
        // ENFANT 2 : wc

        // Rediriger stdin depuis le pipe
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        // Exécuter wc
        execlp("wc", "wc", "-l", NULL);

        perror("execlp wc");
        return 1;
    }

    // PARENT

    // Fermer les deux extrémités dans le parent
    close(pipefd[0]);
    close(pipefd[1]);

    // Attendre les deux enfants
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
```

**Ce programme équivaut à :**
```bash
ls -l | wc -l
```

## Envoi de données structurées

### Exemple : Transmettre une structure

```c
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

typedef struct {
    int id;
    char name[50];
    float price;
} Product;

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0) {
        // ENFANT : Lecteur
        close(pipefd[1]);

        Product received;
        read(pipefd[0], &received, sizeof(Product));

        printf("[Enfant] Produit reçu:\n");
        printf("  ID:    %d\n", received.id);
        printf("  Nom:   %s\n", received.name);
        printf("  Prix:  %.2f €\n", received.price);

        close(pipefd[0]);
        return 0;
    }
    else {
        // PARENT : Émetteur
        close(pipefd[0]);

        Product product = {
            .id = 42,
            .name = "Laptop",
            .price = 799.99
        };

        printf("[Parent] Envoi du produit:\n");
        printf("  ID:    %d\n", product.id);
        printf("  Nom:   %s\n", product.name);
        printf("  Prix:  %.2f €\n", product.price);

        write(pipefd[1], &product, sizeof(Product));

        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}
```

### Tableau de structures

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

typedef struct {
    int x;
    int y;
} Point;

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0) {
        // ENFANT
        close(pipefd[1]);

        Point points[5];
        read(pipefd[0], points, sizeof(points));

        printf("[Enfant] Points reçus:\n");
        for (int i = 0; i < 5; i++) {
            printf("  Point %d: (%d, %d)\n", i, points[i].x, points[i].y);
        }

        close(pipefd[0]);
        return 0;
    }
    else {
        // PARENT
        close(pipefd[0]);

        Point points[5] = {
            {0, 0}, {1, 2}, {3, 4}, {5, 6}, {7, 8}
        };

        printf("[Parent] Envoi de 5 points\n");
        write(pipefd[1], points, sizeof(points));

        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}
```

## Gestion des erreurs robuste

### Vérifier les retours

```c
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>

ssize_t safe_read(int fd, void *buf, size_t count) {
    ssize_t n;

    while ((n = read(fd, buf, count)) == -1) {
        if (errno == EINTR) {
            // Interrompu par un signal, réessayer
            continue;
        }
        // Autre erreur
        return -1;
    }

    return n;
}

ssize_t safe_write(int fd, const void *buf, size_t count) {
    size_t written = 0;

    while (written < count) {
        ssize_t n = write(fd, (const char*)buf + written, count - written);

        if (n == -1) {
            if (errno == EINTR) {
                continue;
            }
            return -1;
        }

        written += n;
    }

    return written;
}

int main(void) {
    int pipefd[2];

    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    if (pid == 0) {
        // ENFANT
        close(pipefd[1]);

        char buffer[100];
        ssize_t n = safe_read(pipefd[0], buffer, sizeof(buffer) - 1);

        if (n == -1) {
            perror("read");
            close(pipefd[0]);
            return 1;
        }

        buffer[n] = '\0';
        printf("[Enfant] Reçu: %s\n", buffer);

        close(pipefd[0]);
        return 0;
    }
    else {
        // PARENT
        close(pipefd[0]);

        const char *msg = "Message avec gestion d'erreurs robuste";

        if (safe_write(pipefd[1], msg, strlen(msg)) == -1) {
            perror("write");
            close(pipefd[1]);
            return 1;
        }

        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}
```

## Cas d'usage pratiques

### 1. Calculateur parallèle

Parent distribue le travail, enfants calculent et renvoient les résultats.

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
    int pipe_to_child[2], pipe_from_child[2];

    pipe(pipe_to_child);
    pipe(pipe_from_child);

    if (fork() == 0) {
        // ENFANT : Calcule le carré
        close(pipe_to_child[1]);
        close(pipe_from_child[0]);

        int number;
        read(pipe_to_child[0], &number, sizeof(int));

        int result = number * number;

        write(pipe_from_child[1], &result, sizeof(int));

        close(pipe_to_child[0]);
        close(pipe_from_child[1]);

        return 0;
    }
    else {
        // PARENT
        close(pipe_to_child[0]);
        close(pipe_from_child[1]);

        int number = 7;
        printf("[Parent] Calcul du carré de %d\n", number);

        write(pipe_to_child[1], &number, sizeof(int));

        int result;
        read(pipe_from_child[0], &result, sizeof(int));

        printf("[Parent] Résultat: %d² = %d\n", number, result);

        close(pipe_to_child[1]);
        close(pipe_from_child[0]);

        wait(NULL);
    }

    return 0;
}
```

### 2. Logger centralisé

Plusieurs processus envoient leurs logs à un processus centralisé.

```c
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>

void worker(int id, int log_pipe) {
    char log_msg[100];

    for (int i = 0; i < 3; i++) {
        snprintf(log_msg, sizeof(log_msg),
                "[Worker %d] Message %d", id, i + 1);

        write(log_pipe, log_msg, strlen(log_msg) + 1);

        sleep(1);
    }
}

int main(void) {
    int log_pipe[2];
    pipe(log_pipe);

    // Créer 3 workers
    for (int i = 0; i < 3; i++) {
        if (fork() == 0) {
            close(log_pipe[0]);
            worker(i + 1, log_pipe[1]);
            close(log_pipe[1]);
            return 0;
        }
    }

    // Logger principal
    close(log_pipe[1]);

    FILE *logfile = fopen("application.log", "w");

    char buffer[100];
    ssize_t n;

    printf("Logger actif, réception des messages...\n\n");

    while ((n = read(log_pipe[0], buffer, sizeof(buffer))) > 0) {
        time_t now = time(NULL);
        char *timestr = ctime(&now);
        timestr[strlen(timestr) - 1] = '\0';  // Retirer '\n'

        fprintf(logfile, "[%s] %s\n", timestr, buffer);
        printf("[%s] %s\n", timestr, buffer);

        fflush(logfile);
    }

    fclose(logfile);
    close(log_pipe[0]);

    // Attendre tous les enfants
    while (wait(NULL) > 0);

    printf("\nLogs écrits dans application.log\n");

    return 0;
}
```

## Erreurs courantes

### 1. Oublier de fermer les descripteurs

```c
// ❌ MAUVAIS
if (fork() == 0) {
    // L'enfant garde fd[1] ouvert
    read(pipefd[0], buffer, size);  // Ne recevra jamais EOF!
}

// ✅ BON
if (fork() == 0) {
    close(pipefd[1]);  // Fermer l'extrémité non utilisée
    read(pipefd[0], buffer, size);
}
```

### 2. Créer le pipe après le fork

```c
// ❌ MAUVAIS
fork();  
pipe(pipefd);  // Parent et enfant ont des pipes différents!  

// ✅ BON
pipe(pipefd);  // Créer AVANT le fork  
fork();  
```

### 3. Inversion lecture/écriture

```c
// ❌ MAUVAIS
write(pipefd[0], ...);  // fd[0] est pour la LECTURE!  
read(pipefd[1], ...);   // fd[1] est pour l'ÉCRITURE!  

// ✅ BON
write(pipefd[1], ...);  // Écriture  
read(pipefd[0], ...);   // Lecture  
```

### 4. Ne pas vérifier les erreurs

```c
// ❌ MAUVAIS
pipe(pipefd);  
write(pipefd[1], data, size);  

// ✅ BON
if (pipe(pipefd) == -1) {
    perror("pipe");
    return 1;
}

if (write(pipefd[1], data, size) == -1) {
    perror("write");
    return 1;
}
```

### 5. Deadlock avec communication bidirectionnelle

```c
// ❌ MAUVAIS (peut causer un deadlock)
// Parent écrit, puis lit
write(pipe1[1], data, size);  // Peut bloquer si buffer plein  
read(pipe2[0], buffer, size); // Attend l'enfant qui attend aussi  

// ✅ BON : Utiliser deux processus ou non-bloquant
```

## Limites et alternatives

### Limites des pipes anonymes

- **Unidirectionnels** : Un seul sens de communication
- **Processus apparentés** : Uniquement entre parent-enfant
- **Pas de nom** : Impossible de se reconnecter
- **Buffer limité** : Environ 64 Ko

### Alternatives

| Besoin | Alternative |
|--------|-------------|
| Communication bidirectionnelle | Deux pipes ou sockets |
| Processus non apparentés | Named pipes (FIFO) ou sockets |
| Buffer plus grand | Fichiers temporaires |
| Communication réseau | Sockets TCP/UDP |
| Mémoire partagée | Shared memory (shm) |

## Résumé

### Concepts clés

- Les pipes permettent la **communication unidirectionnelle** entre processus
- Un pipe a **deux descripteurs** : `fd[0]` (lecture) et `fd[1]` (écriture)
- Le pipe doit être créé **avant** le `fork()`
- **Toujours fermer** les descripteurs non utilisés
- La lecture bloque si le pipe est vide
- L'écriture bloque si le buffer est plein

### Pattern standard

```c
int pipefd[2];  
pipe(pipefd);  

if (fork() == 0) {
    // ENFANT (lecteur)
    close(pipefd[1]);
    read(pipefd[0], buffer, size);
    close(pipefd[0]);
} else {
    // PARENT (écrivain)
    close(pipefd[0]);
    write(pipefd[1], data, size);
    close(pipefd[1]);
    wait(NULL);
}
```

### Fonctions principales

| Fonction | Usage |
|----------|-------|
| `pipe(fd)` | Créer un pipe |
| `read(fd[0], buf, n)` | Lire depuis le pipe |
| `write(fd[1], buf, n)` | Écrire dans le pipe |
| `close(fd)` | Fermer un descripteur |
| `dup2(old, new)` | Rediriger un descripteur |

### Bonnes pratiques

1. ✅ Créer le pipe **avant** `fork()`
2. ✅ Fermer les descripteurs **non utilisés**
3. ✅ Vérifier les **valeurs de retour**
4. ✅ Gérer `SIGPIPE` pour éviter les crashes
5. ✅ Utiliser des fonctions robustes avec gestion de `EINTR`

Les pipes sont un mécanisme fondamental de la communication inter-processus sous Linux, utilisés quotidiennement dans les shells et les applications système !

⏭️ [Named pipes (FIFO)](/17-processus-et-signaux/09-named-pipes.md)
