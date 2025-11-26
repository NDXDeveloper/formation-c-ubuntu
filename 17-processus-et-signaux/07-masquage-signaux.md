🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 17.7 Masquage de signaux

## Introduction

Le **masquage de signaux** (ou blocage de signaux) est un mécanisme qui permet de **bloquer temporairement** la délivrance de certains signaux à un processus. Les signaux masqués ne disparaissent pas : ils sont mis **en attente** et seront délivrés dès que le masque sera levé.

**Analogie :** C'est comme activer le mode "Ne pas déranger" sur votre téléphone. Les notifications ne sont pas perdues, elles sont simplement mises en attente jusqu'à ce que vous désactiviez ce mode.

### Pourquoi masquer des signaux ?

Le masquage de signaux est essentiel dans plusieurs situations :

1. **Protéger les sections critiques** : Empêcher l'interruption pendant des opérations sensibles
2. **Éviter les conditions de course** : Garantir l'atomicité d'opérations complexes
3. **Gérer plusieurs signaux** : Contrôler l'ordre de traitement
4. **Synchronisation** : Attendre spécifiquement certains signaux

## Concepts fondamentaux

### Les trois états d'un signal

Un signal peut être dans l'un de ces trois états :

```
1. GÉNÉRÉ → 2. EN ATTENTE → 3. DÉLIVRÉ
   (sent)      (pending)      (delivered)
```

**États détaillés :**

1. **Généré** : Le signal est envoyé par le noyau ou un autre processus
2. **En attente** : Le signal est bloqué et attend d'être délivré
3. **Délivré** : Le processus traite le signal (handler ou action par défaut)

### Le masque de signaux

Chaque processus possède un **masque de signaux** (signal mask) qui définit quels signaux sont actuellement bloqués.

```
Masque vide : Tous les signaux peuvent être délivrés
Masque plein : Tous les signaux (sauf SIGKILL/SIGSTOP) sont bloqués
```

### Signaux non masquables

⚠️ **Important** : Les signaux **`SIGKILL`** et **`SIGSTOP`** **ne peuvent jamais être bloqués**.

## Les ensembles de signaux : `sigset_t`

### Le type `sigset_t`

Le type `sigset_t` représente un **ensemble de signaux**. C'est un type opaque (on ne manipule pas directement sa structure interne).

```c
#include <signal.h>

sigset_t mon_ensemble;
```

### Fonctions de manipulation

Cinq fonctions permettent de manipuler les ensembles de signaux :

```c
#include <signal.h>

int sigemptyset(sigset_t *set);           // Vider l'ensemble
int sigfillset(sigset_t *set);            // Remplir avec tous les signaux
int sigaddset(sigset_t *set, int signum); // Ajouter un signal
int sigdelset(sigset_t *set, int signum); // Retirer un signal
int sigismember(const sigset_t *set, int signum); // Tester l'appartenance
```

**Retour :**
- `0` en cas de succès
- `-1` en cas d'erreur
- `sigismember()` : `1` si présent, `0` sinon, `-1` si erreur

### Exemple : Créer un ensemble

```c
#include <stdio.h>
#include <signal.h>

int main(void) {
    sigset_t myset;

    // 1. Initialiser l'ensemble (vide)
    sigemptyset(&myset);

    // 2. Ajouter des signaux
    sigaddset(&myset, SIGINT);
    sigaddset(&myset, SIGTERM);
    sigaddset(&myset, SIGUSR1);

    // 3. Tester l'appartenance
    if (sigismember(&myset, SIGINT)) {
        printf("SIGINT est dans l'ensemble\n");
    }

    if (!sigismember(&myset, SIGQUIT)) {
        printf("SIGQUIT n'est PAS dans l'ensemble\n");
    }

    // 4. Retirer un signal
    sigdelset(&myset, SIGTERM);

    if (!sigismember(&myset, SIGTERM)) {
        printf("SIGTERM a été retiré\n");
    }

    return 0;
}
```

**Sortie :**
```
SIGINT est dans l'ensemble
SIGQUIT n'est PAS dans l'ensemble
SIGTERM a été retiré
```

### Exemple : Ensemble complet

```c
#include <stdio.h>
#include <signal.h>

int main(void) {
    sigset_t fullset, emptyset;

    // Ensemble avec tous les signaux
    sigfillset(&fullset);

    // Ensemble vide
    sigemptyset(&emptyset);

    printf("Ensemble complet contient SIGINT: %s\n",
           sigismember(&fullset, SIGINT) ? "Oui" : "Non");

    printf("Ensemble vide contient SIGINT: %s\n",
           sigismember(&emptyset, SIGINT) ? "Oui" : "Non");

    return 0;
}
```

## La fonction `sigprocmask()` : Modifier le masque

### Prototype

```c
#include <signal.h>

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset);
```

**Paramètres :**

- **`how`** : Mode de modification du masque
  - `SIG_BLOCK` : Ajouter les signaux de `set` au masque actuel
  - `SIG_UNBLOCK` : Retirer les signaux de `set` du masque
  - `SIG_SETMASK` : Remplacer complètement le masque par `set`

- **`set`** : Ensemble de signaux à utiliser (peut être `NULL`)
- **`oldset`** : Reçoit l'ancien masque (peut être `NULL`)

**Retour :**
- `0` en cas de succès
- `-1` en cas d'erreur

### Exemple basique : Bloquer SIGINT

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(void) {
    sigset_t newmask, oldmask;

    printf("PID: %d\n", getpid());

    // Préparer le masque avec SIGINT
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    printf("Phase 1: SIGINT normal (3 secondes)\n");
    printf("Essayez Ctrl+C maintenant!\n");
    sleep(3);

    // Bloquer SIGINT
    printf("\nPhase 2: SIGINT BLOQUÉ (5 secondes)\n");
    printf("Ctrl+C ne fonctionnera pas temporairement!\n");
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);
    sleep(5);

    // Débloquer SIGINT
    printf("\nPhase 3: SIGINT débloqué\n");
    printf("Les signaux en attente seront délivrés maintenant...\n");
    sigprocmask(SIG_UNBLOCK, &newmask, NULL);

    sleep(2);
    printf("\nProgramme terminé normalement\n");

    return 0;
}
```

**Test :**
```
Phase 1: SIGINT normal (3 secondes)
Essayez Ctrl+C maintenant!
^C                              # Le programme se termine

Ou si vous attendez:

Phase 2: SIGINT BLOQUÉ (5 secondes)
Ctrl+C ne fonctionnera pas temporairement!
^C^C^C                         # Aucun effet visible

Phase 3: SIGINT débloqué
Les signaux en attente seront délivrés maintenant...
                               # Le programme se termine immédiatement
```

### Les trois modes de `sigprocmask()`

#### Mode 1 : `SIG_BLOCK` - Ajouter au masque

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(void) {
    sigset_t mask;

    // Bloquer SIGINT et SIGTERM
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);

    printf("Blocage de SIGINT et SIGTERM...\n");
    sigprocmask(SIG_BLOCK, &mask, NULL);

    // Maintenant, bloquer aussi SIGUSR1 (en plus des autres)
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);

    printf("Ajout de SIGUSR1 au masque...\n");
    sigprocmask(SIG_BLOCK, &mask, NULL);

    // À ce stade, SIGINT, SIGTERM ET SIGUSR1 sont bloqués

    printf("3 signaux bloqués: SIGINT, SIGTERM, SIGUSR1\n");
    sleep(5);

    return 0;
}
```

#### Mode 2 : `SIG_UNBLOCK` - Retirer du masque

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(void) {
    sigset_t mask;

    // Bloquer plusieurs signaux
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTERM);
    sigaddset(&mask, SIGUSR1);

    printf("Blocage de SIGINT, SIGTERM et SIGUSR1...\n");
    sigprocmask(SIG_BLOCK, &mask, NULL);

    sleep(3);

    // Débloquer uniquement SIGINT
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);

    printf("Déblocage de SIGINT (SIGTERM et SIGUSR1 restent bloqués)\n");
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    sleep(3);

    return 0;
}
```

#### Mode 3 : `SIG_SETMASK` - Remplacer complètement

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

int main(void) {
    sigset_t mask1, mask2;

    // Premier masque : SIGINT
    sigemptyset(&mask1);
    sigaddset(&mask1, SIGINT);

    printf("Masque 1: SIGINT bloqué\n");
    sigprocmask(SIG_SETMASK, &mask1, NULL);
    sleep(2);

    // Deuxième masque : SIGTERM (remplace SIGINT)
    sigemptyset(&mask2);
    sigaddset(&mask2, SIGTERM);

    printf("Masque 2: SIGTERM bloqué (SIGINT débloqué)\n");
    sigprocmask(SIG_SETMASK, &mask2, NULL);
    sleep(2);

    // Vider le masque (débloquer tout)
    sigemptyset(&mask1);
    printf("Masque vide: tous les signaux débloqués\n");
    sigprocmask(SIG_SETMASK, &mask1, NULL);

    sleep(2);

    return 0;
}
```

### Sauvegarder et restaurer le masque

Bonne pratique : sauvegarder l'ancien masque et le restaurer ensuite.

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void critical_section(void) {
    sigset_t newmask, oldmask;

    // Préparer un masque bloquant SIGINT
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    // Bloquer et sauvegarder l'ancien masque
    printf("Entrée en section critique (SIGINT bloqué)\n");
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    // Code critique ici
    printf("Opération sensible en cours...\n");
    sleep(3);

    // Restaurer l'ancien masque
    printf("Sortie de section critique (masque restauré)\n");
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

int main(void) {
    printf("Programme démarré (PID: %d)\n\n", getpid());

    printf("Avant section critique - Ctrl+C fonctionne\n");
    sleep(2);

    critical_section();

    printf("\nAprès section critique - Ctrl+C fonctionne à nouveau\n");
    sleep(2);

    return 0;
}
```

## Vérifier les signaux en attente : `sigpending()`

### La fonction `sigpending()`

```c
#include <signal.h>

int sigpending(sigset_t *set);
```

Permet de savoir quels signaux sont actuellement **en attente** (bloqués mais générés).

### Exemple complet : Visualiser les signaux en attente

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void print_pending_signals(void) {
    sigset_t pending;

    if (sigpending(&pending) == -1) {
        perror("sigpending");
        return;
    }

    printf("Signaux en attente: ");

    int found = 0;

    if (sigismember(&pending, SIGINT)) {
        printf("SIGINT ");
        found = 1;
    }
    if (sigismember(&pending, SIGTERM)) {
        printf("SIGTERM ");
        found = 1;
    }
    if (sigismember(&pending, SIGUSR1)) {
        printf("SIGUSR1 ");
        found = 1;
    }
    if (sigismember(&pending, SIGUSR2)) {
        printf("SIGUSR2 ");
        found = 1;
    }

    if (!found) {
        printf("(aucun)");
    }

    printf("\n");
}

int main(void) {
    sigset_t mask;

    printf("PID: %d\n\n", getpid());

    // Bloquer SIGINT et SIGUSR1
    sigemptyset(&mask);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGUSR1);

    printf("Blocage de SIGINT et SIGUSR1\n");
    sigprocmask(SIG_BLOCK, &mask, NULL);

    printf("\nPendant 10 secondes, envoyez des signaux:\n");
    printf("  kill -INT %d\n", getpid());
    printf("  kill -USR1 %d\n", getpid());
    printf("  (ou appuyez sur Ctrl+C)\n\n");

    for (int i = 1; i <= 10; i++) {
        printf("Seconde %d/10 - ", i);
        print_pending_signals();
        sleep(1);
    }

    printf("\nDéblocage des signaux...\n");
    sigprocmask(SIG_UNBLOCK, &mask, NULL);

    printf("Signaux délivrés!\n");

    sleep(1);

    return 0;
}
```

**Test :**
```bash
# Terminal 1
$ ./programme
PID: 12345

# Terminal 2
$ kill -INT 12345
$ kill -USR1 12345

# Terminal 1 (suite)
Seconde 1/10 - Signaux en attente: (aucun)
Seconde 2/10 - Signaux en attente: SIGINT
Seconde 3/10 - Signaux en attente: SIGINT SIGUSR1
...
Déblocage des signaux...
# Le programme se termine (SIGINT délivré)
```

## Section critique : Pattern complet

### Protéger une section critique

Voici le pattern standard pour protéger une section critique :

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

typedef struct {
    int compteur;
    char buffer[256];
} SharedData;

SharedData data = {0};

void update_data(void) {
    sigset_t newmask, oldmask;

    // Préparer le masque (bloquer tous les signaux sauf SIGKILL/SIGSTOP)
    sigfillset(&newmask);

    // Bloquer les signaux et sauvegarder l'ancien masque
    sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    printf("[Section critique] Début\n");

    // === SECTION CRITIQUE ===
    // Aucun signal ne peut interrompre cette partie

    data.compteur++;
    snprintf(data.buffer, sizeof(data.buffer),
             "Donnée %d", data.compteur);

    printf("[Section critique] Modification de data.compteur = %d\n",
           data.compteur);

    sleep(2);  // Simuler une opération longue

    // === FIN SECTION CRITIQUE ===

    printf("[Section critique] Fin\n");

    // Restaurer le masque de signaux
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

int main(void) {
    printf("Programme démarré (PID: %d)\n", getpid());
    printf("Les signaux seront bloqués pendant les sections critiques\n\n");

    for (int i = 0; i < 3; i++) {
        printf("=== Itération %d ===\n", i + 1);
        update_data();
        sleep(1);
    }

    printf("\nProgramme terminé\n");

    return 0;
}
```

### Alternative : Bloquer sélectivement

Si vous n'avez pas besoin de bloquer **tous** les signaux :

```c
void update_data_selective(void) {
    sigset_t newmask, oldmask;

    // Ne bloquer que certains signaux
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);
    sigaddset(&newmask, SIGTERM);
    sigaddset(&newmask, SIGUSR1);

    sigprocmask(SIG_BLOCK, &newmask, &oldmask);

    // Section critique

    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}
```

## Masquage et handlers : Comportement combiné

### Signaux bloqués pendant l'exécution d'un handler

Quand un handler s'exécute, le signal qui l'a déclenché est **automatiquement bloqué** (sauf si vous utilisez `SA_NODEFER`).

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigint_handler(int signum) {
    (void)signum;

    printf("\n[Handler] Début\n");

    // Le signal SIGINT est automatiquement bloqué ici
    // Si vous appuyez sur Ctrl+C, il sera mis en attente

    printf("[Handler] Travail pendant 3 secondes...\n");
    printf("[Handler] Appuyez sur Ctrl+C, il sera mis en attente\n");

    sleep(3);

    printf("[Handler] Fin\n");

    // À la sortie du handler, SIGINT sera débloqué
    // et le signal en attente sera délivré
}

int main(void) {
    struct sigaction sa;

    sa.sa_handler = sigint_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;  // Pas de SA_NODEFER

    sigaction(SIGINT, &sa, NULL);

    printf("Appuyez sur Ctrl+C\n");

    while (1) {
        sleep(1);
    }

    return 0;
}
```

### Bloquer d'autres signaux pendant un handler

Le champ `sa_mask` de `struct sigaction` permet de bloquer des signaux supplémentaires pendant l'exécution du handler :

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void sigusr1_handler(int signum) {
    (void)signum;
    printf("\n[SIGUSR1 Handler] Début (3 secondes)\n");
    printf("[SIGUSR1 Handler] SIGINT est bloqué pendant ce handler\n");
    sleep(3);
    printf("[SIGUSR1 Handler] Fin\n\n");
}

void sigint_handler(int signum) {
    (void)signum;
    printf("\n[SIGINT Handler] Reçu!\n\n");
}

int main(void) {
    struct sigaction sa_usr1, sa_int;

    // Handler pour SIGINT
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = SA_RESTART;
    sigaction(SIGINT, &sa_int, NULL);

    // Handler pour SIGUSR1
    sa_usr1.sa_handler = sigusr1_handler;
    sigemptyset(&sa_usr1.sa_mask);

    // Bloquer SIGINT pendant l'exécution du handler SIGUSR1
    sigaddset(&sa_usr1.sa_mask, SIGINT);

    sa_usr1.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa_usr1, NULL);

    printf("PID: %d\n\n", getpid());
    printf("Tests:\n");
    printf("1. kill -USR1 %d  (puis appuyez rapidement sur Ctrl+C)\n", getpid());
    printf("2. Ctrl+C sera bloqué pendant le handler SIGUSR1\n\n");

    while (1) {
        sleep(1);
    }

    return 0;
}
```

## Attendre des signaux : `sigsuspend()`

### Le problème de la condition de course

Considérez ce code problématique :

```c
// ❌ MAUVAIS : Condition de course
sigset_t mask;
sigemptyset(&mask);
sigaddset(&mask, SIGUSR1);

sigprocmask(SIG_BLOCK, &mask, NULL);

// PROBLÈME: Si SIGUSR1 arrive ICI, on le manque!

sigprocmask(SIG_UNBLOCK, &mask, NULL);
pause();  // Peut ne jamais se réveiller
```

### La solution : `sigsuspend()`

```c
#include <signal.h>

int sigsuspend(const sigset_t *mask);
```

`sigsuspend()` effectue **atomiquement** :
1. Remplace le masque de signaux par `mask`
2. Suspend le processus jusqu'à réception d'un signal
3. Restaure le masque original

### Exemple : Attendre un signal spécifique

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t sigusr1_received = 0;

void sigusr1_handler(int signum) {
    (void)signum;
    sigusr1_received = 1;
    write(STDOUT_FILENO, "[Handler] SIGUSR1 reçu\n", 24);
}

int main(void) {
    sigset_t blockmask, waitmask;
    struct sigaction sa;

    // Installer le handler
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGUSR1, &sa, NULL);

    printf("PID: %d\n", getpid());

    // Bloquer SIGUSR1
    sigemptyset(&blockmask);
    sigaddset(&blockmask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &blockmask, NULL);

    printf("SIGUSR1 bloqué. Envoyez-le avec: kill -USR1 %d\n", getpid());

    // Préparer le masque pour sigsuspend (tous les signaux sauf SIGUSR1)
    sigfillset(&waitmask);
    sigdelset(&waitmask, SIGUSR1);

    printf("Attente de SIGUSR1...\n");

    // Attendre SIGUSR1 de manière atomique
    sigsuspend(&waitmask);

    printf("Réveil après SIGUSR1!\n");
    printf("sigusr1_received = %d\n", sigusr1_received);

    return 0;
}
```

### Pattern : Attendre plusieurs signaux

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t signal_received = 0;
volatile sig_atomic_t which_signal = 0;

void signal_handler(int signum) {
    signal_received = 1;
    which_signal = signum;
}

int main(void) {
    sigset_t blockmask, waitmask;
    struct sigaction sa;

    // Installer le handler pour plusieurs signaux
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;

    sigaction(SIGUSR1, &sa, NULL);
    sigaction(SIGUSR2, &sa, NULL);
    sigaction(SIGINT, &sa, NULL);

    // Bloquer ces signaux
    sigemptyset(&blockmask);
    sigaddset(&blockmask, SIGUSR1);
    sigaddset(&blockmask, SIGUSR2);
    sigaddset(&blockmask, SIGINT);
    sigprocmask(SIG_BLOCK, &blockmask, NULL);

    printf("PID: %d\n", getpid());
    printf("Attente de SIGUSR1, SIGUSR2 ou SIGINT...\n");

    // Masque pour sigsuspend : tout sauf les signaux attendus
    sigfillset(&waitmask);
    sigdelset(&waitmask, SIGUSR1);
    sigdelset(&waitmask, SIGUSR2);
    sigdelset(&waitmask, SIGINT);

    sigsuspend(&waitmask);

    // Identifier quel signal a été reçu
    if (signal_received) {
        switch (which_signal) {
            case SIGUSR1:
                printf("SIGUSR1 reçu\n");
                break;
            case SIGUSR2:
                printf("SIGUSR2 reçu\n");
                break;
            case SIGINT:
                printf("SIGINT reçu\n");
                break;
        }
    }

    return 0;
}
```

## Cas d'usage pratiques

### 1. Protéger une transaction

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

void execute_transaction(void) {
    sigset_t mask, oldmask;

    printf("Début de la transaction\n");

    // Bloquer tous les signaux pendant la transaction
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    // Transaction atomique
    printf("  Étape 1: Ouverture\n");
    sleep(1);
    printf("  Étape 2: Modification\n");
    sleep(1);
    printf("  Étape 3: Validation\n");
    sleep(1);

    // Restaurer les signaux
    sigprocmask(SIG_SETMASK, &oldmask, NULL);

    printf("Transaction terminée\n");
}

int main(void) {
    printf("PID: %d\n", getpid());
    printf("Les signaux seront bloqués pendant les transactions\n\n");

    execute_transaction();

    printf("\nProgramme terminé\n");

    return 0;
}
```

### 2. Fenêtre de synchronisation

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t ready = 0;

void sigusr1_handler(int signum) {
    (void)signum;
    ready = 1;
}

int main(void) {
    sigset_t mask, oldmask;
    struct sigaction sa;

    // Installer le handler
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    // Bloquer SIGUSR1
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    printf("Parent PID: %d\n", getpid());

    pid_t pid = fork();

    if (pid == 0) {
        // ENFANT
        printf("[Enfant] Préparation...\n");
        sleep(2);

        printf("[Enfant] Prêt! Envoi de SIGUSR1 au parent\n");
        kill(getppid(), SIGUSR1);

        printf("[Enfant] Travail terminé\n");
        return 0;
    }
    else {
        // PARENT
        printf("[Parent] Attente du signal de l'enfant...\n");

        // Attendre SIGUSR1 atomiquement
        while (!ready) {
            sigsuspend(&oldmask);
        }

        printf("[Parent] Signal reçu, enfant prêt!\n");
        printf("[Parent] Continuation du travail\n");

        wait(NULL);
    }

    return 0;
}
```

### 3. Réentrance sécurisée

```c
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t counter = 0;

void increment_counter(void) {
    sigset_t mask, oldmask;

    // Bloquer les signaux pendant la modification
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    // Opération critique
    counter++;
    printf("Counter = %d\n", counter);

    // Restaurer
    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}

void signal_handler(int signum) {
    (void)signum;
    // Peut appeler increment_counter en toute sécurité
    increment_counter();
}

int main(void) {
    signal(SIGUSR1, signal_handler);

    printf("PID: %d\n", getpid());

    for (int i = 0; i < 5; i++) {
        increment_counter();
        sleep(1);
    }

    return 0;
}
```

## Erreurs courantes

### 1. Oublier de restaurer le masque

```c
// ❌ MAUVAIS
void bad_critical_section(void) {
    sigset_t mask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    // Section critique

    // ERREUR: Le masque n'est jamais restauré!
}

// ✅ BON
void good_critical_section(void) {
    sigset_t mask, oldmask;
    sigfillset(&mask);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);

    // Section critique

    sigprocmask(SIG_SETMASK, &oldmask, NULL);
}
```

### 2. Ne pas initialiser l'ensemble

```c
// ❌ MAUVAIS
sigset_t mask;
sigaddset(&mask, SIGINT);  // Comportement indéfini!

// ✅ BON
sigset_t mask;
sigemptyset(&mask);
sigaddset(&mask, SIGINT);
```

### 3. Confusion entre bloquer et ignorer

```c
// Bloquer ≠ Ignorer

// Bloquer: le signal est mis en attente
sigprocmask(SIG_BLOCK, &mask, NULL);

// Ignorer: le signal est perdu
signal(SIGINT, SIG_IGN);
```

### 4. Race condition avec pause()

```c
// ❌ MAUVAIS: Race condition
sigprocmask(SIG_UNBLOCK, &mask, NULL);
pause();  // Le signal peut arriver avant pause()!

// ✅ BON: Utiliser sigsuspend()
sigsuspend(&waitmask);
```

## Résumé

### Fonctions principales

| Fonction | Usage |
|----------|-------|
| `sigemptyset(set)` | Vider un ensemble |
| `sigfillset(set)` | Remplir un ensemble |
| `sigaddset(set, sig)` | Ajouter un signal |
| `sigdelset(set, sig)` | Retirer un signal |
| `sigismember(set, sig)` | Tester l'appartenance |
| `sigprocmask(how, set, oldset)` | Modifier le masque |
| `sigpending(set)` | Voir les signaux en attente |
| `sigsuspend(mask)` | Attendre atomiquement |

### Modes de `sigprocmask()`

| Mode | Effet |
|------|-------|
| `SIG_BLOCK` | Ajouter au masque actuel |
| `SIG_UNBLOCK` | Retirer du masque |
| `SIG_SETMASK` | Remplacer complètement |

### Pattern de section critique

```c
sigset_t mask, oldmask;

// Préparer le masque
sigfillset(&mask);  // ou sigemptyset() + sigaddset()

// Bloquer
sigprocmask(SIG_BLOCK, &mask, &oldmask);

// === Section critique ===

// Restaurer
sigprocmask(SIG_SETMASK, &oldmask, NULL);
```

### Concepts clés

- ✅ Le masquage **retarde** la délivrance des signaux
- ✅ Les signaux bloqués sont mis **en attente**
- ✅ Plusieurs instances d'un même signal → **un seul** en attente
- ✅ `SIGKILL` et `SIGSTOP` sont **non masquables**
- ✅ Toujours **sauvegarder et restaurer** le masque
- ✅ Utiliser `sigsuspend()` pour éviter les race conditions

Le masquage de signaux est un outil puissant pour créer des sections critiques sûres et gérer la synchronisation entre processus. C'est un mécanisme fondamental pour la programmation système robuste sous Linux !

⏭️ [Pipes anonymes](/17-processus-et-signaux/08-pipes-anonymes.md)
