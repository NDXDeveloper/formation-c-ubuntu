🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 16.6 dup, dup2 et redirection

## Introduction

Les fonctions `dup()` et `dup2()` permettent de **dupliquer des descripteurs de fichiers** et sont essentielles pour la **redirection** des entrées/sorties (stdin, stdout, stderr). Ces mécanismes sont au cœur du fonctionnement du shell et de nombreux programmes système.

> **💡 Analogie simple :** Imaginez que vous avez un tuyau d'arrosage connecté à un robinet (le descripteur de fichier). `dup()` crée un **deuxième robinet** qui contrôle le **même tuyau**. Ouvrir ou fermer l'un des robinets n'affecte pas l'autre, mais l'eau coule toujours par le même tuyau.

## Rappel : Table des descripteurs de fichiers

Chaque processus possède une **table de descripteurs de fichiers** :

```
┌─────────────────────────────────┐
│  Table des descripteurs         │
├─────┬───────────────────────────┤
│ fd  │  Pointeur vers fichier    │
├─────┼───────────────────────────┤
│  0  │  → stdin (clavier)        │
│  1  │  → stdout (terminal)      │
│  2  │  → stderr (terminal)      │
│  3  │  → /tmp/data.txt          │
│  4  │  → /var/log/app.log       │
│ ... │  ...                      │
└─────┴───────────────────────────┘
```

**Principe :** Chaque fd pointe vers une structure dans le noyau qui représente le fichier ouvert.

## Duplication de descripteurs : dup()

### Qu'est-ce que dup() ?

La fonction `dup()` crée une **copie** d'un descripteur de fichier. Les deux descripteurs pointent vers le **même fichier ouvert**.

```c
#include <unistd.h>

int dup(int oldfd);
```

**Paramètres :**
- `oldfd` : Le descripteur à dupliquer

**Valeur de retour :**
- Un nouveau descripteur de fichier (le plus petit disponible) en cas de succès
- `-1` en cas d'erreur

### Comportement de dup()

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    int fd = open("fichier.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    printf("fd original : %d\n", fd);  // Typiquement 3

    // Dupliquer le descripteur
    int fd_copy = dup(fd);
    if (fd_copy == -1) {
        perror("dup");
        close(fd);
        return 1;
    }

    printf("fd dupliqué : %d\n", fd_copy);  // Typiquement 4

    // Les deux pointent vers le même fichier
    write(fd, "Hello ", 6);
    write(fd_copy, "World\n", 6);  // Écrit dans le même fichier !

    close(fd);
    close(fd_copy);

    return 0;
}
```

**Résultat dans fichier.txt :**
```
Hello World
```

### Visualisation de dup()

**Avant dup() :**
```
┌─────────────────┐
│ Table des fd    │
├─────┬───────────┤
│  3  │  → file.txt
└─────┴───────────┘
```

**Après `fd_copy = dup(fd)` :**
```
┌─────────────────┐
│ Table des fd    │
├─────┬───────────┤
│  3  │  → ┐      │
│  4  │  → ├─→ file.txt (même fichier)
└─────┴───────────┘
```

**Points importants :**
- Les deux descripteurs partagent le **même offset de fichier** (position de lecture/écriture)
- Fermer l'un n'affecte pas l'autre
- Le nouveau fd obtient le **plus petit numéro disponible**

### Exemple : Partage de l'offset

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    int fd = open("test.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    int fd_copy = dup(fd);

    write(fd, "ABC", 3);        // Écrit "ABC", offset → 3
    write(fd_copy, "DEF", 3);   // Écrit "DEF" après, offset → 6
    write(fd, "GHI", 3);        // Écrit "GHI" après, offset → 9

    close(fd);
    close(fd_copy);

    return 0;
}
```

**Contenu de test.txt :**
```
ABCDEFGHI
```

**Explication :** Les deux descripteurs partagent le même offset, donc les écritures sont séquentielles.

## Duplication avec choix du fd : dup2()

### Qu'est-ce que dup2() ?

La fonction `dup2()` duplique un descripteur vers un **numéro spécifique** de votre choix, fermant d'abord ce numéro s'il était déjà utilisé.

```c
#include <unistd.h>

int dup2(int oldfd, int newfd);
```

**Paramètres :**
- `oldfd` : Le descripteur à dupliquer
- `newfd` : Le numéro de descripteur souhaité

**Valeur de retour :**
- `newfd` en cas de succès
- `-1` en cas d'erreur

### Comportement de dup2()

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    printf("fd original : %d\n", fd);  // 3

    // Dupliquer fd vers le descripteur 10
    int result = dup2(fd, 10);
    if (result == -1) {
        perror("dup2");
        return 1;
    }

    printf("Nouveau fd : %d\n", result);  // 10

    // Maintenant fd 3 et fd 10 pointent vers output.txt
    write(fd, "Via fd 3\n", 9);
    write(10, "Via fd 10\n", 10);

    close(fd);
    close(10);

    return 0;
}
```

### Différences entre dup() et dup2()

| Aspect | dup(oldfd) | dup2(oldfd, newfd) |
|--------|------------|---------------------|
| **Choix du fd** | ❌ Non (plus petit disponible) | ✅ Oui (vous choisissez) |
| **Fermeture automatique** | N/A | ✅ Ferme `newfd` s'il était ouvert |
| **Atomicité** | N/A | ✅ Opération atomique |
| **Usage typique** | Sauvegarde temporaire | Redirection stdin/stdout/stderr |

### Cas spécial : dup2() avec oldfd == newfd

```c
int result = dup2(fd, fd);  // oldfd == newfd
```

**Comportement :**
- Ne fait rien si `oldfd` est valide
- Retourne `newfd` (== oldfd)
- Ne ferme pas le descripteur
- Utile pour vérifier qu'un fd est valide

## Redirection des entrées/sorties standards

### Concept de redirection

La **redirection** consiste à faire pointer stdin (0), stdout (1) ou stderr (2) vers un fichier ou un autre descripteur.

**Dans le shell :**
```bash
./programme > output.txt        # Redirige stdout vers output.txt
./programme < input.txt         # Redirige stdin depuis input.txt
./programme 2> errors.txt       # Redirige stderr vers errors.txt
./programme > out.txt 2>&1      # Redirige stdout et stderr vers out.txt
```

**En C avec dup2() :**
```c
// Rediriger stdout vers un fichier
int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);  
dup2(fd, STDOUT_FILENO);  // stdout (1) pointe maintenant vers output.txt  
close(fd);                // On peut fermer fd, le fd 1 reste valide  

printf("Ceci va dans output.txt\n");
```

### Exemple complet : Redirection de stdout

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    printf("Message 1 : sur le terminal\n");

    // Ouvrir un fichier
    int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    // Rediriger stdout vers le fichier
    if (dup2(fd, STDOUT_FILENO) == -1) {
        perror("dup2");
        close(fd);
        return 1;
    }

    close(fd);  // On n'a plus besoin de fd, stdout pointe toujours vers le fichier

    printf("Message 2 : dans le fichier\n");
    printf("Message 3 : aussi dans le fichier\n");

    return 0;
}
```

**Résultat :**
- "Message 1" s'affiche sur le terminal
- "Message 2" et "Message 3" sont écrits dans output.txt

**Explication :**
```
Avant dup2(fd, 1) :
┌─────────────────┐
│  0  → stdin     │
│  1  → stdout    │ ← terminal
│  2  → stderr    │
│  3  → output.txt│ ← fd
└─────────────────┘

Après dup2(fd, 1) :
┌─────────────────┐
│  0  → stdin     │
│  1  → output.txt│ ← redirigé !
│  2  → stderr    │
│  3  → fermé     │
└─────────────────┘
```

### Sauvegarder et restaurer stdout

Pour temporairement rediriger stdout puis revenir au comportement normal :

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    // 1. Sauvegarder stdout
    int stdout_backup = dup(STDOUT_FILENO);
    if (stdout_backup == -1) {
        perror("dup");
        return 1;
    }

    printf("Message sur le terminal\n");

    // 2. Rediriger stdout vers un fichier
    int fd = open("log.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(fd, STDOUT_FILENO);
    close(fd);

    printf("Message dans log.txt\n");
    printf("Autre message dans log.txt\n");

    // 3. Restaurer stdout original
    dup2(stdout_backup, STDOUT_FILENO);
    close(stdout_backup);

    printf("De retour sur le terminal\n");

    return 0;
}
```

**Sortie :**
```
Terminal:  
Message sur le terminal  
De retour sur le terminal  

log.txt:  
Message dans log.txt  
Autre message dans log.txt  
```

### Redirection de stdin

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    // Créer un fichier d'entrée
    int fd = open("input.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    write(fd, "Alice\n25\n", 9);
    close(fd);

    // Rediriger stdin vers le fichier
    fd = open("input.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        return 1;
    }

    dup2(fd, STDIN_FILENO);
    close(fd);

    // Maintenant scanf() lit depuis input.txt
    char nom[50];
    int age;

    scanf("%s", nom);
    scanf("%d", &age);

    printf("Nom: %s, Age: %d\n", nom, age);
    // Affiche : Nom: Alice, Age: 25

    return 0;
}
```

### Redirection de stderr

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    // Rediriger stderr vers un fichier de log d'erreurs
    int fd = open("errors.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(fd, STDERR_FILENO);
    close(fd);

    // Les messages d'erreur vont dans errors.log
    fprintf(stderr, "Erreur 1: Fichier introuvable\n");
    fprintf(stderr, "Erreur 2: Permission refusée\n");

    // Les messages normaux vont toujours sur stdout
    printf("Programme terminé\n");

    return 0;
}
```

### Rediriger stdout et stderr vers le même fichier

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(void) {
    int fd = open("combined.log", O_WRONLY | O_CREAT | O_TRUNC, 0644);

    // Rediriger stdout
    dup2(fd, STDOUT_FILENO);

    // Rediriger stderr vers stdout (qui pointe vers le fichier)
    dup2(STDOUT_FILENO, STDERR_FILENO);

    close(fd);

    // Maintenant stdout et stderr vont dans le même fichier
    printf("Message normal\n");
    fprintf(stderr, "Message d'erreur\n");
    printf("Autre message\n");

    return 0;
}
```

**Contenu de combined.log :**
```
Message normal  
Message d'erreur  
Autre message  
```

## Utilisation avec fork() et exec()

### Redirection avant exec()

Un cas d'usage classique : rediriger les sorties d'un programme enfant.

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(void) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        // Processus enfant

        // Rediriger stdout vers un fichier
        int fd = open("child_output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open");
            return 1;
        }

        dup2(fd, STDOUT_FILENO);
        close(fd);

        // Exécuter ls (sa sortie ira dans child_output.txt)
        execlp("ls", "ls", "-l", NULL);

        // Si on arrive ici, exec a échoué
        perror("execlp");
        return 1;
    } else {
        // Processus parent
        wait(NULL);
        printf("Parent : l'enfant a terminé\n");
        printf("Résultat dans child_output.txt\n");
    }

    return 0;
}
```

**Résultat :**
- La sortie de `ls -l` est écrite dans child_output.txt
- Le parent affiche ses messages sur le terminal

### Implémentation simplifiée de la redirection shell

Voici comment le shell implémente `commande > fichier` :

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

void execute_with_redirect(const char *cmd, const char *output_file) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        // Enfant : rediriger et exécuter
        int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1) {
            perror("open");
            return;
        }

        dup2(fd, STDOUT_FILENO);
        close(fd);

        execlp(cmd, cmd, NULL);
        perror("execlp");
        return;
    } else {
        // Parent : attendre
        wait(NULL);
    }
}

int main(void) {
    // Équivalent à : echo > message.txt
    execute_with_redirect("echo", "message.txt");

    printf("Commande exécutée\n");

    return 0;
}
```

## Pipes et redirection

Les pipes utilisent `dup2()` pour connecter stdout d'un processus à stdin d'un autre.

### Exemple : Implémenter `ls | wc -l`

```c
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

int main(void) {
    int pipefd[2];

    // Créer un pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return 1;
    }

    // Premier fork : ls
    pid_t pid1 = fork();
    if (pid1 == 0) {
        // Fermer lecture (on n'en a pas besoin)
        close(pipefd[0]);

        // Rediriger stdout vers l'écriture du pipe
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Exécuter ls
        execlp("ls", "ls", NULL);
        perror("execlp ls");
        return 1;
    }

    // Deuxième fork : wc -l
    pid_t pid2 = fork();
    if (pid2 == 0) {
        // Fermer écriture (on n'en a pas besoin)
        close(pipefd[1]);

        // Rediriger stdin vers la lecture du pipe
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        // Exécuter wc -l
        execlp("wc", "wc", "-l", NULL);
        perror("execlp wc");
        return 1;
    }

    // Parent : fermer les deux extrémités du pipe
    close(pipefd[0]);
    close(pipefd[1]);

    // Attendre les deux enfants
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
```

**Explication :**
```
ls → [stdout redirigé] → [pipe écriture] → [pipe lecture] → [stdin redirigé] → wc
```

## Cas d'usage avancés

### 1. Logger avec timestamp automatique

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(void) {
    // Créer un pipe
    int pipefd[2];
    pipe(pipefd);

    pid_t pid = fork();

    if (pid == 0) {
        // Enfant : ajouter des timestamps
        close(pipefd[1]);  // Fermer écriture

        char buffer[256];
        ssize_t n;

        while ((n = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0) {
            buffer[n] = '\0';

            time_t now = time(NULL);
            char *timestr = ctime(&now);
            timestr[strlen(timestr) - 1] = '\0';  // Enlever le \n
            printf("[%s] %s", timestr, buffer);
            fflush(stdout);
        }

        close(pipefd[0]);
        return 0;
    } else {
        // Parent : rediriger stdout vers le pipe
        close(pipefd[0]);  // Fermer lecture
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        // Maintenant tout ce qui est imprimé a un timestamp
        printf("Démarrage du programme\n");
        sleep(1);
        printf("Traitement en cours\n");
        sleep(1);
        printf("Terminé\n");

        return 0;
    }
}
```

### 2. Capturer stdout dans une variable

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

char* capture_output(void (*func)(void)) {
    static char buffer[4096];
    int pipefd[2];

    pipe(pipefd);

    int stdout_backup = dup(STDOUT_FILENO);
    dup2(pipefd[1], STDOUT_FILENO);
    close(pipefd[1]);

    // Exécuter la fonction
    func();

    // Restaurer stdout
    fflush(stdout);
    dup2(stdout_backup, STDOUT_FILENO);
    close(stdout_backup);

    // Lire la sortie capturée
    ssize_t n = read(pipefd[0], buffer, sizeof(buffer) - 1);
    buffer[n] = '\0';
    close(pipefd[0]);

    return buffer;
}

void ma_fonction(void) {
    printf("Sortie capturée\n");
    printf("Ligne 2\n");
}

int main(void) {
    char *output = capture_output(ma_fonction);

    printf("Sortie normale du main\n");
    printf("Sortie capturée était : [%s]\n", output);

    return 0;
}
```

### 3. Redirection bidirectionnelle

```c
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    // Créer deux pipes : parent→enfant et enfant→parent
    int pipe_to_child[2];
    int pipe_from_child[2];

    pipe(pipe_to_child);
    pipe(pipe_from_child);

    pid_t pid = fork();

    if (pid == 0) {
        // Enfant
        close(pipe_to_child[1]);    // Fermer écriture vers enfant
        close(pipe_from_child[0]);  // Fermer lecture depuis enfant

        // Rediriger stdin et stdout
        dup2(pipe_to_child[0], STDIN_FILENO);
        dup2(pipe_from_child[1], STDOUT_FILENO);

        close(pipe_to_child[0]);
        close(pipe_from_child[1]);

        // Transformer les données
        char buffer[256];
        while (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            // Convertir en majuscules
            for (int i = 0; buffer[i]; i++) {
                if (buffer[i] >= 'a' && buffer[i] <= 'z') {
                    buffer[i] -= 32;
                }
            }
            printf("%s", buffer);
            fflush(stdout);
        }

        return 0;
    } else {
        // Parent
        close(pipe_to_child[0]);    // Fermer lecture vers enfant
        close(pipe_from_child[1]);  // Fermer écriture depuis enfant

        // Envoyer des données à l'enfant
        const char *messages[] = {
            "hello world\n",
            "test message\n",
            "last line\n"
        };

        for (int i = 0; i < 3; i++) {
            write(pipe_to_child[1], messages[i], strlen(messages[i]));

            // Lire la réponse
            char buffer[256];
            ssize_t n = read(pipe_from_child[0], buffer, sizeof(buffer) - 1);
            buffer[n] = '\0';
            printf("Reçu : %s", buffer);
        }

        close(pipe_to_child[1]);
        close(pipe_from_child[0]);

        return 0;
    }
}
```

## Bonnes pratiques

### 1. Toujours vérifier les valeurs de retour

```c
// ✅ BON
int fd_copy = dup(fd);  
if (fd_copy == -1) {  
    perror("dup");
    return 1;
}

// ❌ MAUVAIS
int fd_copy = dup(fd);  
write(fd_copy, data, size);  // fd_copy pourrait être -1 !  
```

### 2. Fermer les descripteurs inutilisés

```c
int fd = open("file.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);  
dup2(fd, STDOUT_FILENO);  
close(fd);  // ✅ Plus besoin de fd, fermer pour libérer une ressource  
```

### 3. Sauvegarder avant de rediriger si vous devez restaurer

```c
int stdout_backup = dup(STDOUT_FILENO);

// ... redirection ...

// Restaurer
dup2(stdout_backup, STDOUT_FILENO);  
close(stdout_backup);  
```

### 4. Utiliser dup2() plutôt que close() + dup()

```c
// ❌ MAUVAIS (race condition possible)
close(STDOUT_FILENO);  
dup(fd);  // Espère obtenir 1, mais pas garanti !  

// ✅ BON (atomique)
dup2(fd, STDOUT_FILENO);
```

### 5. Attention au buffering avec les redirections

```c
printf("Message dans le buffer");
// Si on redirige maintenant, le buffer peut être perdu

// Solution : flusher avant redirection
printf("Message dans le buffer");  
fflush(stdout);  
dup2(fd, STDOUT_FILENO);  
```

## Pièges courants

### 1. Oublier de fermer les descripteurs dans les processus enfants

```c
// ❌ PROBLÉMATIQUE
int pipefd[2];  
pipe(pipefd);  

if (fork() == 0) {
    // Enfant : devrait fermer pipefd[0] s'il n'écrit que
    write(pipefd[1], data, size);
    // Oubli de close(pipefd[0]) !
    // Le lecteur ne recevra jamais EOF
}
```

**Solution :** Fermer les extrémités non utilisées :
```c
if (fork() == 0) {
    close(pipefd[0]);  // ✅ Fermer lecture
    write(pipefd[1], data, size);
    close(pipefd[1]);
}
```

### 2. Perdre l'accès au terminal

```c
// ❌ DANGEREUX
dup2(fd, STDOUT_FILENO);
// Si vous ne sauvegardez pas stdout, vous ne pouvez plus afficher sur terminal !
```

### 3. Confondre les extrémités du pipe

```c
int pipefd[2];  
pipe(pipefd);  

// pipefd[0] = lecture (read end)
// pipefd[1] = écriture (write end)

// ❌ ERREUR COURANTE
write(pipefd[0], data, size);  // Impossible ! C'est la lecture  
read(pipefd[1], buf, size);    // Impossible ! C'est l'écriture  
```

### 4. Ne pas restaurer les descripteurs standards

```c
void fonction_qui_redirige(void) {
    int fd = open("temp.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    dup2(fd, STDOUT_FILENO);
    close(fd);

    printf("Dans le fichier\n");

    // Oubli de restaurer stdout !
}

int main(void) {
    fonction_qui_redirige();
    printf("Où va ce message ?\n");  // Va dans temp.txt, pas sur terminal !
    return 0;
}
```

## Exemple complet : Mini-shell avec redirection

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

void execute_command(char *cmd, char *output_file) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return;
    }

    if (pid == 0) {
        // Enfant
        if (output_file != NULL) {
            int fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) {
                perror("open");
                exit(1);
            }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        // Parser la commande (simplifié)
        char *argv[64];
        int argc = 0;

        char *token = strtok(cmd, " \t\n");
        while (token != NULL && argc < 63) {
            argv[argc++] = token;
            token = strtok(NULL, " \t\n");
        }
        argv[argc] = NULL;

        if (argc > 0) {
            execvp(argv[0], argv);
            perror("execvp");
        }
        exit(1);
    } else {
        // Parent
        wait(NULL);
    }
}

int main(void) {
    char line[256];

    printf("Mini-shell (tapez 'exit' pour quitter)\n");

    while (1) {
        printf("$ ");
        fflush(stdout);

        if (fgets(line, sizeof(line), stdin) == NULL) {
            break;
        }

        // Enlever le \n
        line[strcspn(line, "\n")] = 0;

        if (strcmp(line, "exit") == 0) {
            break;
        }

        // Chercher une redirection
        char *redirect = strchr(line, '>');
        char *output_file = NULL;

        if (redirect != NULL) {
            *redirect = '\0';
            output_file = redirect + 1;

            // Ignorer les espaces
            while (*output_file == ' ' || *output_file == '\t') {
                output_file++;
            }
        }

        execute_command(line, output_file);
    }

    printf("Au revoir!\n");
    return 0;
}
```

**Utilisation :**
```
$ ./mini_shell
Mini-shell (tapez 'exit' pour quitter)
$ ls
file1.txt  file2.txt  mini_shell.c
$ ls > list.txt
$ cat list.txt
file1.txt  
file2.txt  
mini_shell.c  
list.txt  
$ exit
Au revoir!
```

## Fonctions associées

### dup3() (Linux spécifique)

Extension de `dup2()` avec des flags supplémentaires :

```c
#include <unistd.h>

int dup3(int oldfd, int newfd, int flags);
```

**Flags possibles :**
- `O_CLOEXEC` : Le nouveau fd sera fermé automatiquement lors d'un `exec()`

```c
int fd = open("file.txt", O_RDONLY);  
int newfd = dup3(fd, 10, O_CLOEXEC);  
// newfd (10) sera fermé si on appelle exec()
```

## Tableau récapitulatif

| Fonction | Usage | Choix du nouveau fd | Ferme ancien si existe |
|----------|-------|---------------------|------------------------|
| `dup(oldfd)` | Duplication simple | ❌ (plus petit disponible) | N/A |
| `dup2(oldfd, newfd)` | Duplication avec choix | ✅ | ✅ |
| `dup3(oldfd, newfd, flags)` | Avec flags (Linux) | ✅ | ✅ |

## Points clés à retenir

✅ **dup()** duplique un fd vers le plus petit numéro disponible

✅ **dup2()** duplique un fd vers un numéro spécifique (atomique)

✅ **Descripteurs dupliqués** partagent le même fichier et le même offset

✅ **Redirection** = utiliser dup2() pour faire pointer stdin/stdout/stderr ailleurs

✅ **Sauvegarder avant de rediriger** si vous devez restaurer plus tard

✅ **Fermer les descripteurs** inutilisés (surtout avec pipes et fork)

✅ **dup2() est atomique** : préférez-le à close() + dup()

✅ **Flusher les buffers** avant redirection pour éviter les pertes

✅ **Utilisé par le shell** pour `>`, `<`, `2>`, `|`

✅ **Essentiel pour pipes** entre processus

---

## Prochaines étapes

Maintenant que vous maîtrisez dup/dup2 et les redirections, explorez :
- **Section 16.7** : I/O multiplexing (select, poll, epoll)
- **Section 17.8** : Pipes anonymes (plus de détails)
- **Section 17.9** : Named pipes (FIFO)
- **Module 6** : Programmation système complète

Les redirections via `dup()` et `dup2()` sont au cœur du modèle Unix "tout est fichier". Maîtriser ces fonctions vous permet de créer des pipelines complexes et de contrôler finement les entrées/sorties de vos programmes !

⏭️ [I/O multiplexing](/16-fichiers-et-io/07-io-multiplexing.md)
