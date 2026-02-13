🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Module 6 : Programmation Système Linux

> **Formation : Maîtriser C sur Ubuntu - De la Syntaxe au Système**
> Niveau : **Expert** 🔥 • Durée estimée : **50-70 heures de lecture approfondie et expérimentation intensive**

---

## 🎯 Objectifs du module

Bienvenue au premier module de niveau **Expert**. Vous avez maîtrisé le langage C et ses outils (Modules 1-5). Maintenant, vous allez apprendre à **dialoguer directement avec le système d'exploitation**.

Ce module vous transforme d'un développeur d'applications en un **ingénieur système**. Vous n'écrivez plus du code qui *utilise* le système, mais du code qui *est* le système.

À l'issue de ce module, vous serez capable de :

1. ✅ **Manipuler les fichiers** au niveau système (file descriptors, appels système)
2. ✅ **Créer et gérer des processus** (fork, exec, wait, signaux)
3. ✅ **Programmer des applications multithreads** (POSIX threads, synchronisation)
4. ✅ **Implémenter la communication inter-processus** (IPC : pipes, shared memory, sockets)
5. ✅ **Créer des serveurs réseau** performants (TCP/UDP, epoll, non-blocking I/O)
6. ✅ **Comprendre eBPF** : la technologie révolutionnaire pour l'observabilité et la sécurité 🔥

---

## 🚀 Qu'est-ce que la programmation système ?

### La différence fondamentale

**Programmation d'application** (Modules 1-5) :
```c
// Bibliothèque standard C
FILE *f = fopen("data.txt", "r");  
fscanf(f, "%d", &value);  
fclose(f);

// Le système fait la magie
```

**Programmation système** (Module 6) :
```c
// Appel système direct
int fd = open("data.txt", O_RDONLY);  
if (fd == -1) {
    perror("open");
    exit(1);
}
ssize_t n = read(fd, buffer, sizeof(buffer));  
close(fd);

// Vous contrôlez TOUT
```

### La stack complète

```
┌─────────────────────────────────────┐
│   Votre Application (Module 2-4)    │  ← Ce que vous avez appris
├─────────────────────────────────────┤
│   Bibliothèque C (stdio, stdlib)    │  ← Abstraction
├─────────────────────────────────────┤
│   Programmation Système (Module 6)  │  ← VOUS ÊTES ICI
│   (appels système, IPC, threads)    │
├─────────────────────────────────────┤
│   Kernel Linux                      │  ← Interface directe
├─────────────────────────────────────┤
│   Matériel (CPU, RAM, I/O)          │
└─────────────────────────────────────┘
```

**Vous descendez d'un niveau dans la stack.**

### Ce que ça implique

**Plus de contrôle** :
- ✅ Performances maximales
- ✅ Fonctionnalités avancées
- ✅ Interaction directe avec le kernel

**Plus de responsabilité** :
- ⚠️ Erreurs plus difficiles à déboguer
- ⚠️ Bugs peuvent crasher le système entier
- ⚠️ Race conditions et deadlocks
- ⚠️ Comportements dépendants du kernel

**Plus de complexité** :
- 🧠 Concepts abstraits (processus, threads, IPC)
- 🧠 Concurrence et synchronisation
- 🧠 Documentation dense (man pages)

---

## 🧩 Place dans la formation globale

### Vous avez construit les fondations

**Module 1-2** : Langage et syntaxe  
**Module 3** : Gestion mémoire  
**Module 4** : Structures de données  
**Module 5** : Outillage professionnel

**→ Vous savez coder en C de manière professionnelle**

### Maintenant, vous passez au système

**Module 6** vous ouvre les portes de :
- 🔧 **DevOps** : Créer des outils système, agents de monitoring
- 🌐 **Backend** : Serveurs haute performance
- 🛡️ **Sécurité** : Outils de forensics, scanners
- 📊 **Observabilité** : Traçage kernel, métriques système
- 🐳 **Containers** : Comprendre Docker, Kubernetes de l'intérieur
- ⚡ **Performance** : Optimisation bas niveau

### Le gap de compétences

```
Développeurs C qui maîtrisent le langage : 1,000,000+  
Développeurs C qui maîtrisent la programmation système : 10,000
                                                             ↑
                                                         VOUS ICI
```

**Ces compétences sont rares et extrêmement valorisées.**

---

## 📚 Structure du module

Ce module est organisé en **six chapitres**, chacun couvrant un aspect fondamental de la programmation système Linux :

### 📂 [Chapitre 16 : Gestion des Fichiers et Entrées/Sorties](16-fichiers-et-io/README.md)
**Contenu** : 8 sections • **Niveau** : Expert • **Durée** : 8-10 heures

**Vous découvrirez :**
- File descriptors : la base de tout I/O sous Linux
- Appels système : `open`, `read`, `write`, `close`, `lseek`
- Différence appels système vs bibliothèque standard
- Permissions et modes (chmod, umask)
- I/O bufferisé vs non bufferisé
- Redirection avec `dup`, `dup2`
- I/O multiplexing : `select`, `poll`, `epoll`
- I/O asynchrone (AIO)

**Concepts clés :**
```c
// Tout sous Linux est un fichier
int fd = open("/dev/null", O_WRONLY);  // Device  
int fd = open("data.txt", O_RDONLY);   // Fichier normal  
int fd = socket(AF_INET, SOCK_STREAM, 0); // Socket réseau  
int fd = open("/proc/cpuinfo", O_RDONLY); // Pseudo-fichier

// File descriptor = entier
// 0 = stdin, 1 = stdout, 2 = stderr

// I/O multiplexing avec epoll (moderne)
int epoll_fd = epoll_create1(0);  
struct epoll_event ev = {.events = EPOLLIN, .data.fd = fd};  
epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev);  
epoll_wait(epoll_fd, events, MAX_EVENTS, timeout);
```

**Pourquoi c'est crucial :**
- 🔑 **Fondation de tout** : processus, threads, réseau utilisent les FD
- ⚡ **Performance** : I/O non bloquant + epoll = serveurs haute performance
- 🎯 **Contrôle** : Bypasser les buffers de stdio pour la latence minimale

---

### 🔄 [Chapitre 17 : Processus et Signaux](17-processus-et-signaux/README.md)
**Contenu** : 9 sections • **Niveau** : Expert • **Durée** : 10-12 heures

**Vous maîtriserez :**
- Création de processus : `fork`, `exec`, `vfork`
- PIDs et hiérarchie de processus (arbre)
- Processus orphelins et zombies (et comment les éviter)
- Variables d'environnement (`environ`, `getenv`, `setenv`)
- Signaux : `SIGINT`, `SIGTERM`, `SIGKILL`, `SIGCHLD`...
- Handlers de signaux personnalisés
- Masquage de signaux (sigprocmask)
- Communication inter-processus basique : pipes anonymes et named pipes (FIFO)

**Concepts clés :**
```c
// Fork : créer un processus enfant
pid_t pid = fork();  
if (pid == 0) {
    // Code du processus enfant
    execl("/bin/ls", "ls", "-l", NULL);
} else if (pid > 0) {
    // Code du processus parent
    int status;
    wait(&status);  // Attendre la fin de l'enfant
}

// Signaux : communication asynchrone
void signal_handler(int sig) {
    if (sig == SIGINT) {
        printf("Ctrl+C reçu!\n");
        exit(0);
    }
}
signal(SIGINT, signal_handler);

// Pipe : communication parent-enfant
int pipefd[2];  
pipe(pipefd);  // pipefd[0] = read, pipefd[1] = write
```

**Pourquoi c'est crucial :**
- 🔧 **Outils système** : Tout daemon/service utilise fork
- 🔌 **Shell scripting** : Comprendre comment bash fonctionne
- 🚦 **Gestion de services** : systemd, supervisord, etc.
- 💬 **IPC** : Base de la communication entre programmes

---

### 🧵 [Chapitre 18 : Threads et Concurrence (POSIX)](18-threads-et-concurrence/README.md) ⭐🔥
**Contenu** : 13 sections • **Niveau** : Expert-Avancé • **Durée** : 15-20 heures

**LE CHAPITRE LE PLUS IMPORTANT ET LE PLUS DIFFICILE DU MODULE**

**Vous apprendrez :**
- Introduction à pthread (POSIX threads)
- Création et terminaison de threads
- Passage de paramètres aux threads
- Threads détachés vs joinable
- **Race conditions** : le bug le plus dangereux
- **Mutex** : exclusion mutuelle et sections critiques
- **Deadlocks** : détection et prévention
- Variables de condition (condition variables)
- Sémaphores POSIX
- **Atomics C11** : concurrence lock-free (stdatomic.h) 🔥
- Read-write locks (rwlock)
- Thread-local storage (TLS)
- Barrières de threads

**Concepts clés :**
```c
// Créer un thread
pthread_t thread;  
pthread_create(&thread, NULL, thread_function, arg);  
pthread_join(thread, NULL);

// Mutex : protéger une section critique
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;  
pthread_mutex_lock(&lock);
// Section critique (un seul thread à la fois)
shared_counter++;  
pthread_mutex_unlock(&lock);

// Atomics C11 : sans lock
#include <stdatomic.h>
atomic_int counter = 0;  
atomic_fetch_add(&counter, 1);  // Thread-safe, sans mutex !

// Condition variable : attendre un événement
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;  
pthread_mutex_lock(&lock);  
while (!condition) {
    pthread_cond_wait(&cond, &lock);  // Attend et libère le lock
}
pthread_mutex_unlock(&lock);
```

**Pourquoi c'est crucial :**
- 🚀 **Performance** : Exploiter les CPUs multi-cœurs
- 🌐 **Serveurs modernes** : Tous utilisent threads ou async I/O
- 🐛 **Bugs les plus difficiles** : Race conditions, deadlocks, heisenbugs
- 🔬 **Compétence rare** : Peu de développeurs maîtrisent vraiment

**Difficulté :**
La concurrence est **contre-intuitive**. Un programme peut fonctionner 999 fois et crasher la 1000ème. Les bugs sont **non-déterministes** et **difficiles à reproduire**.

**Outils essentiels :**
- ThreadSanitizer (TSan) : Détecte les race conditions
- Helgrind (Valgrind) : Détecte les problèmes de synchronisation
- GDB avec threads : Déboguer des programmes multithreads

---

### 🔗 [Chapitre 19 : Mémoire partagée et IPC avancé](19-memoire-partagee-ipc/README.md)
**Contenu** : 5 sections • **Niveau** : Expert • **Durée** : 8-10 heures

**Vous maîtriserez :**
- Shared memory (POSIX et System V)
- Sémaphores System V (vs POSIX)
- Message queues System V
- Comparaison POSIX IPC vs System V IPC
- Memory-mapped files (`mmap`, `munmap`)

**Concepts clés :**
```c
// Shared memory POSIX
int shm_fd = shm_open("/myshm", O_CREAT | O_RDWR, 0666);  
ftruncate(shm_fd, SIZE);  
void *ptr = mmap(NULL, SIZE, PROT_READ | PROT_WRITE,
                 MAP_SHARED, shm_fd, 0);
// Plusieurs processus partagent cette mémoire

// Memory-mapped file (rapide !)
int fd = open("bigfile.dat", O_RDONLY);  
void *map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);
// Accès direct comme un tableau, kernel gère le cache
```

**Pourquoi c'est crucial :**
- ⚡ **Performance extrême** : Shared memory est l'IPC le plus rapide
- 📊 **Big data** : mmap pour traiter des fichiers > RAM
- 🔄 **IPC complexe** : Communication entre processus non reliés
- 🐳 **Containers** : Comprendre comment Docker partage des ressources

---

### 🌐 [Chapitre 20 : Réseau (Sockets)](20-reseau-sockets/README.md)
**Contenu** : 10 sections • **Niveau** : Expert • **Durée** : 12-15 heures

**Vous créerez :**
- Serveurs et clients TCP/UDP
- Architecture client/serveur
- Sockets non-bloquants
- Serveurs concurrents (multi-clients)
- Serveurs haute performance avec epoll
- Mini-serveur HTTP fonctionnel

**Concepts clés :**
```c
// Serveur TCP basique
int server_fd = socket(AF_INET, SOCK_STREAM, 0);  
struct sockaddr_in addr = {
    .sin_family = AF_INET,
    .sin_port = htons(8080),
    .sin_addr.s_addr = INADDR_ANY
};
bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));  
listen(server_fd, 10);

while (1) {
    int client_fd = accept(server_fd, NULL, NULL);
    // Traiter la connexion client
    close(client_fd);
}

// Serveur haute performance (epoll)
int epoll_fd = epoll_create1(0);
// Ajouter server_fd à epoll
while (1) {
    int n = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    for (int i = 0; i < n; i++) {
        // Traiter chaque événement sans bloquer
    }
}
```

**Pourquoi c'est crucial :**
- 🌐 **Internet** : Tout est sockets (HTTP, SSH, DNS...)
- 🚀 **Backend** : API REST, microservices, websockets
- 📊 **Performance** : epoll peut gérer 10,000+ connexions simultanées
- 🔧 **DevOps** : Agents, collecteurs, proxies

**Architecture moderne :**
```
Serveur traditionnel (threads)  →  1 thread par client
                                   Limite : ~1000 clients

Serveur moderne (epoll)        →  1 thread, event loop
                                   Limite : ~100,000 clients
```

---

### 🔥 [Chapitre 21 : Introduction à eBPF](21-introduction-ebpf/README.md) ⭐🔥
**Contenu** : 6 sections • **Niveau** : Expert-Visionnaire • **Durée** : 10-15 heures

**LA TECHNOLOGIE QUI RÉVOLUTIONNE LINUX**

**Vous découvrirez :**
- Qu'est-ce qu'eBPF et pourquoi c'est révolutionnaire
- Architecture eBPF (programs, maps, verifier, JIT)
- Use cases DevOps : tracing, monitoring, networking, sécurité
- Toolchain : libbpf, bpftool
- Création de votre premier programme eBPF
- Écosystème : Cilium, Pixie, Falco

**Concepts clés :**
```c
// eBPF program (kernel space)
SEC("tracepoint/syscalls/sys_enter_execve")  
int trace_execve(struct trace_event_raw_sys_enter *ctx) {
    char comm[16];
    bpf_get_current_comm(&comm, sizeof(comm));
    bpf_printk("Executing: %s\n", comm);
    return 0;
}

// User space : charger et attacher
struct bpf_object *obj = bpf_object__open_file("prog.o", NULL);  
bpf_object__load(obj);  
bpf_program__attach(bpf_object__find_program_by_name(obj, "trace_execve"));
```

**Pourquoi c'est révolutionnaire :**
- 🔍 **Observabilité** : Tracer le kernel SANS modifier le kernel
- 🚀 **Performance** : JIT compilation, exécution dans le kernel
- 🛡️ **Sécurité** : Filtering réseau, détection d'intrusions
- 🌐 **Networking** : Cilium (CNI Kubernetes), load balancing
- 📊 **Monitoring** : Pixie, bpftrace, BCC tools

**Impact industrie :**
- Google, Facebook, Netflix utilisent eBPF massivement
- Cilium remplace iptables dans Kubernetes
- Falco (sécurité) détecte les anomalies en temps réel
- bpftrace remplace dtrace/strace

**eBPF est l'avenir de la programmation système Linux.**

---

## 🎓 Prérequis

### Absolument nécessaire
- ✅ **Modules 1-5 complétés** : Maîtrise totale du C et des outils
- ✅ **Compréhension profonde des pointeurs**
- ✅ **Gestion mémoire sans failles**
- ✅ **Maîtrise de GDB, Valgrind, sanitizers**
- ✅ **Lecture fluide des man pages**

### Fortement recommandé
- ✅ **Expérience avec Linux** : Utilisation quotidienne, admin système
- ✅ **Notions de systèmes d'exploitation** (cours universitaire ou équivalent)
- ✅ **Compréhension de TCP/IP** (basique)
- ✅ **Notions de concurrence** (threads, mutex, même dans un autre langage)

### Nice to have
- ☑️ Connaissance de Python/Go (pour comprendre les comparaisons)
- ☑️ Expérience DevOps (Docker, Kubernetes)
- ☑️ Notions de sécurité

### Prérequis système
```bash
# Kernel Linux moderne (5.0+) pour eBPF
uname -r  # Devrait afficher >= 5.0

# Outils essentiels
sudo apt install -y \
    strace ltrace \           # Tracer les appels système
    netcat-openbsd socat \    # Test réseau
    bpftool linux-tools-generic  # eBPF

# Man pages complètes
sudo apt install manpages-dev manpages-posix-dev
```

---

## ⚠️ Avertissement : Ce module est exigeant

### Soyez préparé mentalement

**Ce que ce module N'EST PAS :**
- ❌ Un tutoriel "facile"
- ❌ Du code qui fonctionne du premier coup
- ❌ Des concepts intuitifs

**Ce que ce module EST :**
- ✅ Dense et technique
- ✅ Nécessite de la persévérance
- ✅ Demande de la réflexion profonde
- ✅ Récompensant pour ceux qui persistent

### Les défis spécifiques

**1. Bugs non-déterministes**
Un programme multithread peut crasher 1 fois sur 1000. Difficile à déboguer.

**2. Documentation dense**
Les man pages font 20+ pages. Exemple : `man 2 select` fait 30 pages.

**3. Concepts abstraits**
Qu'est-ce qu'un processus ? Un thread ? Un file descriptor ? Ces concepts prennent du temps à internaliser.

**4. Comportements dépendants du système**
Le même code peut se comporter différemment selon :
- La version du kernel
- La configuration système
- Le scheduler
- Le CPU

**5. Responsabilité accrue**
Vos erreurs peuvent :
- Crasher des processus
- Créer des fuites de ressources système
- Ouvrir des failles de sécurité
- Causer des deadlocks système

---

## 🗺️ Stratégie d'apprentissage recommandée

### Approche progressive

#### 1. **Lisez les man pages**
Elles sont votre Bible. Pour chaque appel système :
```bash
man 2 open   # Section 2 = appels système  
man 3 fopen  # Section 3 = bibliothèque C  
man 7 signal # Section 7 = overview
```

#### 2. **Testez avec strace**
Voir ce qui se passe réellement :
```bash
strace ./programme 2>&1 | grep -E 'open|read|write'
# Voir tous les appels système
```

#### 3. **Commencez simple**
Ne créez pas un serveur multithread avec epoll comme premier projet. Commencez par :
- Lire un fichier avec `open`/`read`
- Créer un processus avec `fork`
- Un thread qui incrémente un compteur
- Un pipe entre parent et enfant
- Un serveur TCP echo simple

#### 4. **Utilisez les outils spécialisés**
```bash
# Thread debugging
gcc -fsanitize=thread programme.c  
valgrind --tool=helgrind ./programme

# Network debugging
netstat -tulpn  
ss -tulpn  
tcpdump -i lo port 8080

# eBPF
bpftool prog list  
bpftool map list
```

#### 5. **Lisez du code existant**
- Redis (networking, event loop)
- Nginx (processus, threads, epoll)
- systemd (processus, IPC)

---

## 🛠️ Environnement de développement pour ce module

### Configuration recommandée

```bash
# Créer un workspace dédié
mkdir -p ~/system-programming/{file-io,process,threads,ipc,network,ebpf}

# Installer les outils spécialisés
sudo apt install -y \
    strace ltrace \
    gdb gdbserver \
    valgrind \
    netcat-openbsd socat telnet \
    tcpdump wireshark-cli \
    bpftool bpfcc-tools linux-headers-$(uname -r)

# Man pages complètes
sudo apt install -y manpages-dev manpages-posix-dev

# Template CMake pour programmation système
cat > CMakeLists.txt << 'EOF'  
cmake_minimum_required(VERSION 3.15)  
project(SystemProg C)  
set(CMAKE_C_STANDARD 11)

# Flags système
add_compile_options(-Wall -Wextra -Werror -D_GNU_SOURCE)  
add_compile_options(-fsanitize=thread)  # Pour threads  
add_link_options(-fsanitize=thread)

# Bibliothèques système
find_package(Threads REQUIRED)  
target_link_libraries(myapp Threads::Threads)  
EOF
```

### GDB pour debugging système

```bash
# .gdbinit pour programmation système
cat > ~/.gdbinit << 'EOF'  
set follow-fork-mode child   # Suivre le processus enfant  
set detach-on-fork off        # Ne pas détacher le parent  
set print thread-events on    # Afficher les événements threads  
set scheduler-locking on      # Verrouiller sur le thread actuel

# Breakpoints utiles
define bp_syscalls
    catch syscall open
    catch syscall close
    catch syscall fork
    catch syscall clone
end  
EOF
```

---

## 🏆 Critères de maîtrise

### ✅ Niveau 1 : Fichiers et processus
- [ ] Utiliser open/read/write/close sans bugs
- [ ] Créer des processus avec fork/exec
- [ ] Gérer proprement les signaux
- [ ] Utiliser les pipes pour IPC basique
- [ ] Comprendre les file descriptors

### ✅ Niveau 2 : Concurrence
- [ ] Créer et synchroniser des threads
- [ ] Utiliser mutex sans deadlocks
- [ ] Détecter et corriger les race conditions avec TSan
- [ ] Utiliser les condition variables correctement
- [ ] Comprendre les atomics C11

### ✅ Niveau 3 : IPC et réseau
- [ ] Utiliser shared memory entre processus
- [ ] Créer un serveur TCP fonctionnel
- [ ] Utiliser epoll pour de multiples connexions
- [ ] Implémenter un protocole réseau simple
- [ ] Gérer les erreurs réseau proprement

### ✅ Niveau 4 : Expert
- [ ] Créer un serveur concurrent performant
- [ ] Déboguer des deadlocks et race conditions complexes
- [ ] Utiliser mmap pour des fichiers volumineux
- [ ] Écrire un programme eBPF fonctionnel
- [ ] Comprendre l'architecture complète système

---

## 📚 Ressources complémentaires essentielles

### Livres de référence (OBLIGATOIRES)
- **The Linux Programming Interface** (Michael Kerrisk) - LA Bible (1500 pages)
- **Advanced Programming in the UNIX Environment** (Stevens & Rago) - Classique
- **Unix Network Programming Vol. 1** (Stevens) - Networking
- **Linux System Programming** (Robert Love) - Moderne et accessible

### Man pages et documentation
```bash
# Installer TOUTES les man pages
sudo apt install manpages-dev manpages-posix-dev

# Lectures essentielles
man 2 intro    # Introduction aux appels système  
man 3 intro    # Introduction aux fonctions bibliothèque  
man 7 signal   # Overview des signaux  
man 7 pthreads # Overview POSIX threads  
man 7 socket   # Overview sockets
```

### Tutoriels en ligne
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [eBPF Documentation](https://ebpf.io/)
- [Linux Kernel Documentation](https://www.kernel.org/doc/html/latest/)

### Code source à étudier
- [Redis](https://github.com/redis/redis) - Event loop, networking
- [Nginx](https://github.com/nginx/nginx) - Architecture multiprocessus
- [BCC](https://github.com/iovisor/bcc) - Outils eBPF

---

## 💡 Conseils pour réussir ce module

### 1. **Patience et persévérance**
Vous ALLEZ rencontrer des segfaults, des deadlocks, des race conditions. C'est normal.

### 2. **Lisez la documentation complète**
Ne vous contentez pas d'exemples Stack Overflow. Lisez TOUTE la man page.

### 3. **Testez sur des cas simples d'abord**
Pas de serveur HTTP multithread comme premier projet threads.

### 4. **Utilisez les sanitizers systématiquement**
```bash
# TOUJOURS pour les threads
gcc -fsanitize=thread

# TOUJOURS pour la mémoire
gcc -fsanitize=address
```

### 5. **Rejoignez des communautés**
- [r/C_Programming](https://reddit.com/r/C_Programming)
- [Stack Overflow](https://stackoverflow.com/questions/tagged/posix)
- IRC: #kernelnewbies sur OFTC

### 6. **Acceptez de ne pas tout comprendre immédiatement**
La programmation système est cumulative. Vous comprendrez mieux les threads après avoir fait du réseau, et vice versa.

---

## 🗓️ Planning de lecture suggéré

### Approche intensive (temps plein)
| Semaine | Contenu | Focus |
|---------|---------|-------|
| **Semaine 1** | Chapitre 16 : File I/O | Appels système, file descriptors |
| **Semaine 2** | Chapitre 17 : Processus | fork, exec, signaux |
| **Semaine 3-4** | Chapitre 18 : Threads (18.1-18.6) | Création, mutex, race conditions |
| **Semaine 5** | Chapitre 18 : Threads (18.7-18.13) | Deadlocks, atomics, avancé |
| **Semaine 6** | Chapitre 19 : IPC | Shared memory, mmap |
| **Semaine 7-8** | Chapitre 20 : Réseau | Sockets, serveurs |
| **Semaine 9-10** | Chapitre 21 : eBPF | Introduction et premiers programmes |

### Approche à temps partiel (15h/semaine)
| Semaine | Contenu |
|---------|---------|
| **Semaines 1-2** | Chapitre 16 |
| **Semaines 3-4** | Chapitre 17 |
| **Semaines 5-9** | Chapitre 18 (le plus long) |
| **Semaines 10-12** | Chapitre 19 |
| **Semaines 13-16** | Chapitre 20 |
| **Semaines 17-20** | Chapitre 21 |

---

## 📑 Prochaines sections

**Entrez dans le monde de la programmation système :**

- **[Chapitre 16 : Gestion des Fichiers et Entrées/Sorties →](16-fichiers-et-io/README.md)**
- [Chapitre 17 : Processus et Signaux](17-processus-et-signaux/README.md)
- [Chapitre 18 : Threads et Concurrence (POSIX)](18-threads-et-concurrence/README.md) ⭐🔥
- [Chapitre 19 : Mémoire partagée et IPC avancé](19-memoire-partagee-ipc/README.md)
- [Chapitre 20 : Réseau (Sockets)](20-reseau-sockets/README.md)
- [Chapitre 21 : Introduction à eBPF](21-introduction-ebpf/README.md) ⭐🔥

---

## 🎯 Mot de conclusion

Vous vous apprêtez à franchir la ligne qui sépare les développeurs d'applications des ingénieurs système.

**Ce module est difficile.** Mais c'est aussi :
- 🏆 Le plus gratifiant
- 🚀 Le plus impactant pour votre carrière
- 🧠 Le plus formateur intellectuellement
- 💰 Le plus valorisé financièrement

Les compétences de ce module sont utilisées par :
- 📊 **Google** : Infrastructures, monitoring (eBPF)
- 🌐 **Netflix** : Serveurs haute performance
- 🐳 **Docker/Kubernetes** : Containers, networking (eBPF/Cilium)
- 🔒 **Cloudflare** : Sécurité, DDoS protection
- 🔥 **Facebook** : Backend, observabilité (eBPF)

Vous entrez dans le club des ingénieurs qui comprennent vraiment comment Linux fonctionne.

---

**Courage, patience, et que le kernel soit avec vous ! 🔥**

---

## 📝 Note finale

Cette formation est un guide de référence complet. Pour ce module expert :

- **Lisez** chaque man page mentionnée en entier
- **Testez** chaque appel système dans un programme minimal
- **Déboguez** avec strace, GDB, et les sanitizers
- **Expérimentez** : provoquez des erreurs pour les comprendre
- **Consultez** les ressources (TLPI est indispensable)

**Les Modules 7-8** s'appuieront sur ces fondations système pour des applications avancées et du DevOps moderne.

**Le Module 9** vous permettra d'appliquer toutes ces connaissances dans des projets réels : agents de monitoring, parsers de logs, serveurs web.

Prenez votre temps. Ce module peut prendre 2-3 mois à temps partiel. C'est normal et souhaitable.

⏭️ [Chapitre 16 : Gestion des Fichiers et Entrées/Sorties →](16-fichiers-et-io/README.md)
