🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 17.5 Gestion des signaux

## Introduction

Les **signaux** sont un mécanisme de communication asynchrone entre processus sous Linux. Ils permettent au noyau ou à d'autres processus d'**interrompre** un processus pour lui notifier un événement spécifique.

**Analogie :** Un signal est comme une alarme ou une notification urgente. Lorsqu'un processus reçoit un signal, il doit réagir immédiatement, quelle que soit son activité en cours.

### Caractéristiques des signaux

- **Asynchrones** : Peuvent arriver à n'importe quel moment
- **Numérotés** : Chaque signal a un numéro unique (ex: SIGINT = 2)
- **Nommés** : Identifiés par des constantes symboliques (ex: `SIGINT`, `SIGTERM`)
- **Action par défaut** : Chaque signal a un comportement par défaut
- **Personnalisables** : On peut définir un gestionnaire personnalisé

## Les signaux courants

### Tableau des signaux principaux

| Signal | Numéro | Origine | Action par défaut | Description |
|--------|--------|---------|-------------------|-------------|
| `SIGHUP` | 1 | Terminal | Terminer | Déconnexion du terminal |
| `SIGINT` | 2 | Clavier (Ctrl+C) | Terminer | Interruption |
| `SIGQUIT` | 3 | Clavier (Ctrl+\\) | Terminer + core dump | Quitter |
| `SIGILL` | 4 | Programme | Terminer + core dump | Instruction illégale |
| `SIGABRT` | 6 | `abort()` | Terminer + core dump | Abandon |
| `SIGFPE` | 8 | Programme | Terminer + core dump | Erreur mathématique (division par 0) |
| `SIGKILL` | 9 | Externe | Terminer | **Tuer immédiatement** (non capturable) |
| `SIGSEGV` | 11 | Programme | Terminer + core dump | Violation de segmentation |
| `SIGPIPE` | 13 | Système | Terminer | Écriture dans un pipe fermé |
| `SIGALRM` | 14 | `alarm()` | Terminer | Alarme (timer) |
| `SIGTERM` | 15 | Externe | Terminer | Demande de terminaison |
| `SIGUSR1` | 10 | Externe | Terminer | Signal utilisateur 1 |
| `SIGUSR2` | 12 | Externe | Terminer | Signal utilisateur 2 |
| `SIGCHLD` | 17 | Enfant | Ignorer | Enfant terminé ou stoppé |
| `SIGCONT` | 18 | Externe | Continuer | Continuer si stoppé |
| `SIGSTOP` | 19 | Externe | Stopper | **Stopper** (non capturable) |
| `SIGTSTP` | 20 | Clavier (Ctrl+Z) | Stopper | Stopper (terminal) |

### Signaux non capturables

Deux signaux **ne peuvent pas être interceptés ou ignorés** :

- **`SIGKILL` (9)** : Tue le processus immédiatement
- **`SIGSTOP` (19)** : Stoppe le processus immédiatement

Ces signaux garantissent que le système garde toujours le contrôle sur les processus.

## Envoyer des signaux

### Avec la commande `kill`

Malgré son nom, `kill` permet d'envoyer **n'importe quel signal** à un processus :

```bash
# Envoyer SIGTERM (15) - terminaison propre
kill PID  
kill -15 PID  
kill -TERM PID  

# Envoyer SIGKILL (9) - tuer immédiatement
kill -9 PID  
kill -KILL PID  

# Envoyer SIGINT (2) - équivalent à Ctrl+C
kill -2 PID  
kill -INT PID  

# Envoyer un signal personnalisé
kill -USR1 PID  
kill -10 PID  

# Lister tous les signaux disponibles
kill -l
```

### Avec la fonction `kill()` en C

```c
#include <signal.h>
#include <sys/types.h>

int kill(pid_t pid, int sig);
```

**Paramètres :**
- `pid` : PID du processus cible
- `sig` : Numéro du signal à envoyer

**Retour :**
- `0` en cas de succès
- `-1` en cas d'erreur

**Exemple :**

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

int main(void) {
    pid_t target_pid = 12345;  // PID du processus cible

    printf("Envoi de SIGTERM au processus %d\n", target_pid);

    if (kill(target_pid, SIGTERM) == 0) {
        printf("Signal envoyé avec succès\n");
    } else {
        perror("kill");
        return 1;
    }

    return 0;
}
```

### S'envoyer un signal à soi-même

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(void) {
    printf("Je m'envoie SIGINT...\n");

    // Envoyer SIGINT à soi-même
    kill(getpid(), SIGINT);

    // Ou utiliser raise()
    // raise(SIGINT);

    printf("Cette ligne ne sera probablement jamais affichée\n");

    return 0;
}
```

### La fonction `raise()`

Équivalent simplifié pour s'envoyer un signal :

```c
#include <signal.h>

int raise(int sig);

// Équivalent à:
// kill(getpid(), sig);
```

## Gérer les signaux : La fonction `signal()`

### Prototype

```c
#include <signal.h>

typedef void (*sighandler_t)(int);  
sighandler_t signal(int signum, sighandler_t handler);  
```

**Paramètres :**
- `signum` : Le signal à gérer
- `handler` : Fonction de gestion ou action spéciale

**Actions spéciales :**
- `SIG_DFL` : Comportement par défaut
- `SIG_IGN` : Ignorer le signal

### Premier exemple : Ignorer SIGINT

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(void) {
    printf("Programme démarré (PID: %d)\n", getpid());
    printf("Essayez Ctrl+C... ça ne marchera pas!\n\n");

    // Ignorer SIGINT (Ctrl+C)
    signal(SIGINT, SIG_IGN);

    // Boucle infinie
    for (int i = 1; i <= 10; i++) {
        printf("Itération %d - Ctrl+C ne fonctionne pas\n", i);
        sleep(1);
    }

    printf("\nProgramme terminé normalement\n");

    return 0;
}
```

**Test :**
```bash
$ ./programme
Programme démarré (PID: 12345)  
Essayez Ctrl+C... ça ne marchera pas!  

Itération 1 - Ctrl+C ne fonctionne pas
^C                   # Vous appuyez sur Ctrl+C
Itération 2 - Ctrl+C ne fonctionne pas
^C                   # Encore Ctrl+C
Itération 3 - Ctrl+C ne fonctionne pas
...
```

### Définir un gestionnaire personnalisé

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Fonction appelée lors de la réception du signal
void signal_handler(int signum) {
    printf("\n[HANDLER] Signal %d reçu (SIGINT)!\n", signum);
    printf("[HANDLER] Mais je continue à fonctionner...\n\n");
}

int main(void) {
    printf("Programme démarré (PID: %d)\n", getpid());
    printf("Appuyez sur Ctrl+C pour tester le gestionnaire\n\n");

    // Installer le gestionnaire pour SIGINT
    signal(SIGINT, signal_handler);

    for (int i = 1; i <= 10; i++) {
        printf("Itération %d\n", i);
        sleep(1);
    }

    printf("Programme terminé\n");

    return 0;
}
```

**Sortie :**
```
Programme démarré (PID: 12345)  
Appuyez sur Ctrl+C pour tester le gestionnaire  

Itération 1  
Itération 2  
^C
[HANDLER] Signal 2 reçu (SIGINT)!
[HANDLER] Mais je continue à fonctionner...

Itération 3  
Itération 4  
```

## Gestionnaire de signal : Contraintes importantes

### Fonctions non-réentrantes

⚠️ **ATTENTION** : Un gestionnaire de signal peut interrompre le programme **à n'importe quel moment**, même au milieu d'une fonction.

**Problème :** Certaines fonctions ne sont pas "**async-signal-safe**" et ne doivent pas être appelées dans un handler.

### Fonctions dangereuses dans un handler

❌ **NE PAS utiliser :**
- `printf()`, `fprintf()` (sauf `write()`)
- `malloc()`, `free()`
- `pthread_mutex_lock()`
- La plupart des fonctions de la libc

✅ **Autorisées :**
- `write()` (I/O direct)
- `_exit()` (terminaison immédiate)
- Opérations simples sur variables volatiles

### Liste complète des fonctions async-signal-safe

Consultez `man 7 signal-safety` pour la liste exhaustive. Principales :

```
_exit(), access(), alarm(), chdir(), chmod(), chown(),
close(), dup(), execve(), fork(), getpid(), kill(),  
link(), lseek(), mkdir(), open(), pause(), pipe(),  
read(), rename(), rmdir(), setuid(), signal(), sleep(),  
stat(), time(), umask(), unlink(), wait(), write()  
```

### Exemple incorrect

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// ❌ DANGEREUX : printf() dans un handler
void bad_handler(int signum) {
    (void)signum;
    printf("Signal reçu!\n");  // printf() n'est PAS async-signal-safe!
    // Peut causer un deadlock ou corruption de données
}

int main(void) {
    signal(SIGINT, bad_handler);

    while (1) {
        printf("En cours...\n");
        sleep(1);
    }

    return 0;
}
```

### Exemple correct

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

// ✅ BON : utilisation de write()
void safe_handler(int signum) {
    (void)signum;
    const char msg[] = "\n[HANDLER] Signal reçu!\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

int main(void) {
    signal(SIGINT, safe_handler);

    while (1) {
        write(STDOUT_FILENO, "En cours...\n", 12);
        sleep(1);
    }

    return 0;
}
```

## Pattern courant : Variable flag

Une approche sûre consiste à **positionner un flag** dans le handler et le vérifier dans la boucle principale :

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

// Variable globale volatile
volatile sig_atomic_t interrupted = 0;

void sigint_handler(int signum) {
    (void)signum;  // Paramètre non utilisé
    interrupted = 1;
}

int main(void) {
    printf("Programme démarré (PID: %d)\n", getpid());
    printf("Appuyez sur Ctrl+C pour arrêter proprement\n\n");

    signal(SIGINT, sigint_handler);

    int count = 0;
    while (!interrupted) {
        printf("Itération %d\n", ++count);
        sleep(1);
    }

    printf("\n[MAIN] Interruption détectée\n");
    printf("[MAIN] Nettoyage en cours...\n");

    // Faire le nettoyage nécessaire
    sleep(1);

    printf("[MAIN] Terminaison propre\n");

    return 0;
}
```

**Points clés :**
- `volatile` : Indique que la variable peut changer de manière imprévisible
- `sig_atomic_t` : Type garanti atomique pour les opérations de lecture/écriture
- Le handler ne fait que positionner le flag
- La logique principale est dans `main()`

## La fonction `sigaction()` : Recommandée

`sigaction()` est plus robuste et portable que `signal()`. Elle offre un meilleur contrôle.

### Prototype

```c
#include <signal.h>

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact);
```

**Structure `sigaction` :**

```c
struct sigaction {
    void (*sa_handler)(int);              // Fonction handler
    void (*sa_sigaction)(int, siginfo_t *, void *);  // Handler étendu
    sigset_t sa_mask;                     // Signaux à bloquer pendant le handler
    int sa_flags;                         // Options
    void (*sa_restorer)(void);            // Obsolète
};
```

### Exemple basique avec `sigaction()`

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sigint_handler(int signum) {
    (void)signum;
    const char msg[] = "\n[HANDLER] SIGINT reçu\n";
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);
}

int main(void) {
    struct sigaction sa;

    // Initialiser la structure
    memset(&sa, 0, sizeof(sa));

    // Définir le handler
    sa.sa_handler = sigint_handler;

    // Pas de signaux bloqués supplémentaires pendant le handler
    sigemptyset(&sa.sa_mask);

    // Options : redémarrer automatiquement les appels système interrompus
    sa.sa_flags = SA_RESTART;

    // Installer le handler
    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("Handler installé pour SIGINT\n");
    printf("Appuyez sur Ctrl+C\n\n");

    while (1) {
        printf("En cours...\n");
        sleep(2);
    }

    return 0;
}
```

### Flags importants de `sa_flags`

| Flag | Description |
|------|-------------|
| `SA_RESTART` | Redémarre automatiquement les appels système interrompus |
| `SA_NOCLDSTOP` | Ne pas recevoir `SIGCHLD` quand un enfant est stoppé |
| `SA_NOCLDWAIT` | Ne pas créer de zombies pour les enfants |
| `SA_NODEFER` | Ne pas bloquer le signal pendant son traitement |
| `SA_RESETHAND` | Réinitialiser le handler à `SIG_DFL` après réception |
| `SA_SIGINFO` | Utiliser `sa_sigaction` au lieu de `sa_handler` |

### Handler étendu avec `SA_SIGINFO`

Le flag `SA_SIGINFO` permet d'obtenir plus d'informations sur le signal :

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void sigint_handler_extended(int signum, siginfo_t *info, void *context) {
    (void)context;  // Non utilisé

    char buffer[256];
    int len;

    len = snprintf(buffer, sizeof(buffer),
                   "\n[HANDLER] Signal %d reçu\n"
                   "[HANDLER] Envoyé par PID: %d\n"
                   "[HANDLER] UID: %d\n\n",
                   signum, info->si_pid, info->si_uid);

    write(STDOUT_FILENO, buffer, len);
}

int main(void) {
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));

    // Utiliser le handler étendu
    sa.sa_sigaction = sigint_handler_extended;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_SIGINFO | SA_RESTART;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("PID: %d\n", getpid());
    printf("Envoyez-moi un signal avec: kill -INT %d\n\n", getpid());

    while (1) {
        sleep(1);
    }

    return 0;
}
```

## Gérer plusieurs signaux

### Exemple complet : SIGINT, SIGTERM, SIGUSR1

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

volatile sig_atomic_t keep_running = 1;  
volatile sig_atomic_t usr1_received = 0;  

void signal_handler(int signum) {
    char msg[64];
    int len;

    switch (signum) {
        case SIGINT:
            len = snprintf(msg, sizeof(msg),
                          "\n[HANDLER] SIGINT (Ctrl+C) reçu\n");
            write(STDOUT_FILENO, msg, len);
            keep_running = 0;
            break;

        case SIGTERM:
            len = snprintf(msg, sizeof(msg),
                          "\n[HANDLER] SIGTERM reçu\n");
            write(STDOUT_FILENO, msg, len);
            keep_running = 0;
            break;

        case SIGUSR1:
            len = snprintf(msg, sizeof(msg),
                          "\n[HANDLER] SIGUSR1 reçu\n");
            write(STDOUT_FILENO, msg, len);
            usr1_received++;
            break;
    }
}

int main(void) {
    struct sigaction sa;

    // Configuration commune
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    // Installer les handlers
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    printf("Programme démarré (PID: %d)\n", getpid());
    printf("Signaux gérés:\n");
    printf("  - Ctrl+C ou SIGINT  : Arrêt propre\n");
    printf("  - SIGTERM           : Arrêt propre\n");
    printf("  - SIGUSR1           : Incrémente un compteur\n");
    printf("\nTestez avec: kill -USR1 %d\n\n", getpid());

    int count = 0;
    while (keep_running) {
        printf("Itération %d (SIGUSR1 reçu %d fois)\n",
               ++count, usr1_received);
        sleep(2);
    }

    printf("\n[MAIN] Arrêt demandé, nettoyage...\n");
    printf("[MAIN] Total SIGUSR1 reçus: %d\n", usr1_received);
    printf("[MAIN] Terminaison propre\n");

    return 0;
}
```

**Test :**
```bash
# Terminal 1
$ ./programme
Programme démarré (PID: 12345)
...
Itération 1 (SIGUSR1 reçu 0 fois)  
Itération 2 (SIGUSR1 reçu 0 fois)  

# Terminal 2
$ kill -USR1 12345
$ kill -USR1 12345
$ kill -INT 12345

# Terminal 1 (suite)
[HANDLER] SIGUSR1 reçu
Itération 3 (SIGUSR1 reçu 1 fois)
[HANDLER] SIGUSR1 reçu
Itération 4 (SIGUSR1 reçu 2 fois)
[HANDLER] SIGINT (Ctrl+C) reçu
[MAIN] Arrêt demandé, nettoyage...
[MAIN] Total SIGUSR1 reçus: 2
[MAIN] Terminaison propre
```

## Masquer et bloquer des signaux

### Concept

Il est possible de **bloquer temporairement** la délivrance de certains signaux. Les signaux bloqués sont mis en attente et délivrés quand ils sont débloqués.

### Fonctions de manipulation des ensembles de signaux

```c
#include <signal.h>

int sigemptyset(sigset_t *set);           // Vider l'ensemble  
int sigfillset(sigset_t *set);            // Remplir avec tous les signaux  
int sigaddset(sigset_t *set, int signum); // Ajouter un signal  
int sigdelset(sigset_t *set, int signum); // Retirer un signal  
int sigismember(const sigset_t *set, int signum); // Tester l'appartenance  
```

### Modifier le masque de signaux

```c
#include <signal.h>

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
```

**Paramètre `how` :**
- `SIG_BLOCK` : Ajouter les signaux au masque actuel
- `SIG_UNBLOCK` : Retirer les signaux du masque
- `SIG_SETMASK` : Remplacer le masque complet

### Exemple : Bloquer SIGINT temporairement

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int signum) {
    (void)signum;
    write(STDOUT_FILENO, "\n[HANDLER] SIGINT reçu!\n", 25);
}

int main(void) {
    struct sigaction sa;
    sigset_t mask, oldmask;

    // Installer le handler
    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa, NULL);

    printf("PID: %d\n\n", getpid());

    // Préparer un masque avec SIGINT
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    printf("Phase 1: SIGINT normal (2 secondes)\n");
    printf("Essayez Ctrl+C maintenant!\n");
    sleep(2);

    // Bloquer SIGINT
    printf("\nPhase 2: SIGINT BLOQUÉ (5 secondes)\n");
    printf("Ctrl+C est maintenant bloqué!\n");
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    sleep(5);

    // Débloquer SIGINT
    printf("\nPhase 3: SIGINT débloqué\n");
    printf("Les signaux en attente vont être délivrés...\n");
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    sleep(2);
    printf("\nProgramme terminé\n");

    return 0;
}
```

**Comportement :**
- Phase 1 : Ctrl+C déclenche immédiatement le handler
- Phase 2 : Ctrl+C est mis en attente (aucun effet visible)
- Phase 3 : Le(s) SIGINT en attente sont délivrés immédiatement

## Signaux et processus enfants : `SIGCHLD`

### Le signal `SIGCHLD`

Quand un processus enfant se termine, le noyau envoie `SIGCHLD` au parent. C'est le mécanisme pour éviter les zombies de manière asynchrone.

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

    // Attendre tous les enfants terminés
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        char msg[128];
        int len;

        if (WIFEXITED(status)) {
            len = snprintf(msg, sizeof(msg),
                          "[HANDLER] Enfant %d terminé (code: %d)\n",
                          pid, WEXITSTATUS(status));
        } else {
            len = snprintf(msg, sizeof(msg),
                          "[HANDLER] Enfant %d terminé anormalement\n", pid);
        }

        write(STDOUT_FILENO, msg, len);
    }
}

int main(void) {
    struct sigaction sa;

    // Installer le handler pour SIGCHLD
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;

    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    printf("[Parent] PID: %d\n", getpid());
    printf("[Parent] Création de 3 enfants...\n\n");

    // Créer 3 processus enfants
    for (int i = 1; i <= 3; i++) {
        pid_t pid = fork();

        if (pid == 0) {
            // ENFANT
            printf("[Enfant %d] PID: %d, je dors %d secondes\n",
                   i, getpid(), i);
            sleep(i);
            printf("[Enfant %d] Terminé\n", i);
            exit(i);
        }

        printf("[Parent] Enfant %d créé (PID: %d)\n", i, pid);
    }

    printf("\n[Parent] Tous les enfants créés, j'attends...\n\n");

    // Parent fait autre chose
    for (int i = 0; i < 5; i++) {
        printf("[Parent] Travail en cours... (%d/5)\n", i+1);
        sleep(1);
    }

    printf("\n[Parent] Terminé\n");

    return 0;
}
```

### Éviter complètement les zombies avec `SA_NOCLDWAIT`

Si vous ne vous souciez pas du code de retour des enfants :

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

int main(void) {
    struct sigaction sa;

    // Configuration pour ne pas créer de zombies
    sa.sa_handler = SIG_DFL;  // ou SIG_IGN
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_NOCLDWAIT;  // Pas de zombies!

    sigaction(SIGCHLD, &sa, NULL);

    printf("[Parent] PID: %d\n", getpid());

    for (int i = 0; i < 5; i++) {
        if (fork() == 0) {
            printf("[Enfant %d] PID: %d\n", i, getpid());
            sleep(1);
            exit(0);
        }
    }

    printf("[Parent] Tous les enfants créés\n");
    printf("[Parent] Pas de zombies grâce à SA_NOCLDWAIT!\n");

    sleep(3);

    return 0;
}
```

## Temporisateur avec `alarm()` et `SIGALRM`

### La fonction `alarm()`

```c
#include <unistd.h>

unsigned int alarm(unsigned int seconds);
```

Déclenche `SIGALRM` après `seconds` secondes.

### Exemple : Timeout sur une opération

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <setjmp.h>

static jmp_buf jump_buffer;  
static volatile sig_atomic_t timeout_occurred = 0;  

void alarm_handler(int signum) {
    (void)signum;
    timeout_occurred = 1;
    longjmp(jump_buffer, 1);
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = alarm_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, NULL);

    printf("Vous avez 5 secondes pour appuyer sur Entrée!\n");

    // Définir un timeout de 5 secondes
    alarm(5);

    if (setjmp(jump_buffer) == 0) {
        // Première exécution
        char buffer[100];
        fgets(buffer, sizeof(buffer), stdin);

        // Annuler l'alarme si l'utilisateur a répondu à temps
        alarm(0);

        printf("Bravo! Vous avez été assez rapide!\n");
    } else {
        // Retour depuis longjmp (timeout)
        printf("\n\nTemps écoulé! Trop lent!\n");
    }

    return 0;
}
```

## Exemple complet : Serveur robuste

Voici un exemple de squelette de serveur gérant proprement les signaux :

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

volatile sig_atomic_t shutdown_requested = 0;  
volatile sig_atomic_t reload_config = 0;  

void signal_handler(int signum) {
    char msg[64];
    int len;

    switch (signum) {
        case SIGINT:
        case SIGTERM:
            len = snprintf(msg, sizeof(msg),
                          "\n[Signal] Arrêt demandé (signal %d)\n", signum);
            write(STDOUT_FILENO, msg, len);
            shutdown_requested = 1;
            break;

        case SIGHUP:
            len = snprintf(msg, sizeof(msg),
                          "\n[Signal] Rechargement config (signal %d)\n", signum);
            write(STDOUT_FILENO, msg, len);
            reload_config = 1;
            break;

        case SIGUSR1:
            len = snprintf(msg, sizeof(msg),
                          "\n[Signal] SIGUSR1 - Affichage des stats (signal %d)\n", signum);
            write(STDOUT_FILENO, msg, len);
            // Afficher des statistiques
            break;
    }
}

void sigchld_handler(int signum) {
    (void)signum;
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        char msg[64];
        int len = snprintf(msg, sizeof(msg),
                          "[Signal] Worker %d terminé\n", pid);
        write(STDOUT_FILENO, msg, len);
    }
}

void setup_signals(void) {
    struct sigaction sa;

    // Signaux d'arrêt
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
    sigaction(SIGUSR1, &sa, NULL);

    // SIGCHLD
    sa.sa_handler = sigchld_handler;
    sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
    sigaction(SIGCHLD, &sa, NULL);

    // Ignorer SIGPIPE
    signal(SIGPIPE, SIG_IGN);
}

void load_config(void) {
    printf("[Config] Chargement de la configuration...\n");
    sleep(1);
    printf("[Config] Configuration chargée\n");
}

int main(void) {
    printf("=== Serveur Robuste ===\n");
    printf("PID: %d\n\n", getpid());

    setup_signals();
    load_config();

    printf("Commandes:\n");
    printf("  kill -TERM %d  : Arrêt propre\n", getpid());
    printf("  kill -HUP %d   : Recharger config\n", getpid());
    printf("  kill -USR1 %d  : Afficher stats\n", getpid());
    printf("\nServeur démarré...\n\n");

    int request_count = 0;

    while (!shutdown_requested) {
        // Vérifier si rechargement demandé
        if (reload_config) {
            reload_config = 0;
            load_config();
        }

        // Simuler le traitement de requêtes
        printf("[Serveur] Traitement requête #%d\n", ++request_count);
        sleep(2);
    }

    printf("\n[Serveur] Arrêt en cours...\n");
    printf("[Serveur] Nettoyage des ressources...\n");
    sleep(1);
    printf("[Serveur] Total requêtes traitées: %d\n", request_count);
    printf("[Serveur] Arrêt propre terminé\n");

    return 0;
}
```

## Bonnes pratiques

### 1. Utilisez `sigaction()` plutôt que `signal()`

`sigaction()` est plus portable et offre un meilleur contrôle.

### 2. Gardez les handlers simples

Positionnez un flag et traitez la logique dans le programme principal.

### 3. Utilisez `volatile sig_atomic_t`

Pour les variables partagées entre le handler et le programme principal.

### 4. N'utilisez que des fonctions async-signal-safe

Dans les handlers, utilisez uniquement `write()` et autres fonctions sûres.

### 5. Bloquez les signaux pendant les sections critiques

Si nécessaire, utilisez `sigprocmask()` pour protéger les sections critiques.

### 6. Gérez `SIGCHLD` pour éviter les zombies

Installez un handler ou utilisez `SA_NOCLDWAIT`.

### 7. Ignorez `SIGPIPE`

Pour les applications réseau, ignorez `SIGPIPE` et gérez `EPIPE` avec `errno`.

```c
signal(SIGPIPE, SIG_IGN);
```

### 8. Testez la restauration des handlers

Certains systèmes réinitialisent le handler après chaque signal. Avec `sigaction()`, cela n'arrive pas si vous utilisez les bons flags.

## Erreurs courantes

### 1. Oublier `volatile sig_atomic_t`

```c
// ❌ MAUVAIS
int flag = 0;

// ✅ BON
volatile sig_atomic_t flag = 0;
```

### 2. Utiliser `printf()` dans un handler

```c
// ❌ MAUVAIS
void handler(int sig) {
    printf("Signal reçu\n");  // Peut causer un deadlock!
}

// ✅ BON
void handler(int sig) {
    write(STDOUT_FILENO, "Signal reçu\n", 13);
}
```

### 3. Ne pas gérer les appels système interrompus

Certains appels système retournent `-1` avec `errno == EINTR` quand interrompus par un signal. Utilisez `SA_RESTART` ou gérez manuellement :

```c
ssize_t n;  
do {  
    n = read(fd, buffer, size);
} while (n == -1 && errno == EINTR);
```

### 4. Oublier de bloquer les signaux dans les sections critiques

Si vous manipulez des structures de données, bloquez temporairement les signaux.

## Débogage

### Tracer les signaux avec `strace`

```bash
# Voir tous les signaux reçus par un processus
strace -e trace=signal ./programme

# Tracer un processus en cours
strace -e trace=signal -p PID
```

### Lister les handlers actifs

```bash
# Voir les handlers de signaux d'un processus
cat /proc/PID/status | grep Sig

# Ou de manière plus lisible
grep Sig /proc/PID/status
```

## Résumé

### Concepts clés

- Les **signaux** sont des notifications asynchrones envoyées aux processus
- Chaque signal a un **comportement par défaut** (terminer, ignorer, stopper)
- On peut **intercepter** la plupart des signaux avec un handler personnalisé
- **`SIGKILL`** et **`SIGSTOP`** ne peuvent pas être interceptés

### Fonctions principales

| Fonction | Usage |
|----------|-------|
| `signal(sig, handler)` | Installer un handler (simple mais limité) |
| `sigaction(sig, &sa, NULL)` | Installer un handler (recommandé) |
| `kill(pid, sig)` | Envoyer un signal à un processus |
| `raise(sig)` | S'envoyer un signal |
| `alarm(seconds)` | Déclencher `SIGALRM` après un délai |
| `sigprocmask(...)` | Bloquer/débloquer des signaux |

### Règles de sécurité dans les handlers

1. ✅ Utiliser **uniquement** des fonctions async-signal-safe
2. ✅ Privilégier les **flags** `volatile sig_atomic_t`
3. ✅ Garder les handlers **courts et simples**
4. ❌ **Jamais** de `printf()`, `malloc()`, ou fonctions complexes

### Signaux courants

- **`SIGINT`** (Ctrl+C) : Interruption utilisateur
- **`SIGTERM`** : Demande d'arrêt propre
- **`SIGKILL`** : Tuer immédiatement (non capturable)
- **`SIGUSR1/2`** : Signaux utilisateur personnalisables
- **`SIGCHLD`** : Enfant terminé
- **`SIGALRM`** : Alarme/timer

La maîtrise des signaux est essentielle pour créer des applications robustes qui réagissent correctement aux événements système et aux demandes d'arrêt !

⏭️ [Signaux courants](/17-processus-et-signaux/06-signaux-courants.md)
