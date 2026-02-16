# Annexe B.3 - Appels système Linux : Guide de compilation

## Compilation par défaut

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o programme fichier.c
```

---

## Liste des exemples

### test_open.c
- **Section** : B.3 - Fichiers (open/close)
- **Description** : Ouverture, écriture et lecture d'un fichier avec open/write/read
- **Fichier source** : 03-appels-systeme-linux.md
- **Sortie attendue** : `Lu : Hello, syscalls!`

### test_dup2.c
- **Section** : B.3 - Fichiers (dup2)
- **Description** : Redirection de stdout vers un fichier avec dup2
- **Fichier source** : 03-appels-systeme-linux.md
- **Sortie attendue** : Crée output.txt contenant le texte redirigé

### test_pipe.c
- **Section** : B.3 - Pipes
- **Description** : Communication parent-enfant via pipe (version originale sans sys/wait.h)
- **Fichier source** : 03-appels-systeme-linux.md

### test_pipe2.c
- **Section** : B.3 - Pipes
- **Description** : Communication parent-enfant via pipe (version corrigée avec sys/wait.h)
- **Fichier source** : 03-appels-systeme-linux.md
- **Sortie attendue** : `Parent envoie: ...` puis `Enfant reçoit: ...`

### test_fork.c
- **Section** : B.3 - Processus (fork)
- **Description** : Création de processus avec fork
- **Fichier source** : 03-appels-systeme-linux.md
- **Sortie attendue** : Messages du parent et de l'enfant avec leurs PIDs

### test_execl.c
- **Section** : B.3 - Processus (exec)
- **Description** : Remplacement de processus avec execl (lance /bin/ls)
- **Fichier source** : 03-appels-systeme-linux.md
- **Sortie attendue** : Listing du répertoire courant

### test_forkexec.c
- **Section** : B.3 - Processus (fork+exec)
- **Description** : Pattern fork+exec classique
- **Fichier source** : 03-appels-systeme-linux.md
- **Sortie attendue** : L'enfant exécute ls, le parent attend

### test_waitpid.c
- **Section** : B.3 - Processus (waitpid)
- **Description** : Attente d'un processus enfant avec waitpid et récupération du code de sortie
- **Fichier source** : 03-appels-systeme-linux.md
- **Sortie attendue** : `Enfant terminé avec code: 42`

### test_stat.c
- **Section** : B.3 - Système de fichiers (stat)
- **Description** : Récupération des métadonnées d'un fichier avec stat
- **Fichier source** : 03-appels-systeme-linux.md
- **Sortie attendue** : Taille, permissions et dates du fichier

### test_mmap.c
- **Section** : B.3 - Mémoire (mmap anonyme)
- **Description** : Allocation de mémoire partagée avec mmap anonyme
- **Fichier source** : 03-appels-systeme-linux.md
- **Sortie attendue** : `data[0] = 42`

### test_mmap_file.c
- **Section** : B.3 - Mémoire (mmap fichier)
- **Description** : Mapping d'un fichier en mémoire avec mmap
- **Fichier source** : 03-appels-systeme-linux.md
- **Sortie attendue** : Premiers octets du fichier mappé

### test_clock.c
- **Section** : B.3 - Temps (clock_gettime)
- **Description** : Mesure de temps avec clock_gettime
- **Fichier source** : 03-appels-systeme-linux.md
- **Sortie attendue** : Durée d'exécution en nanosecondes

### test_readdir.c
- **Section** : B.3 - Répertoires (opendir/readdir)
- **Description** : Lecture du contenu d'un répertoire
- **Fichier source** : 03-appels-systeme-linux.md
- **Sortie attendue** : Liste des fichiers du répertoire courant

### test_alarm.c
- **Section** : B.3 - Signaux (alarm/signal)
- **Description** : Utilisation d'alarm et signal pour un timer
- **Fichier source** : 03-appels-systeme-linux.md
- **Sortie attendue** : `Temps écoulé !` après 2 secondes

### test_capture.c
- **Section** : B.3 - Signaux (capture)
- **Description** : Capture de SIGINT avec handler personnalisé
- **Fichier source** : 03-appels-systeme-linux.md
- **Note** : Programme interactif (compilation seule)

### test_copyfile.c
- **Section** : B.3 - Copie de fichier (sendfile)
- **Description** : Copie de fichier avec sendfile (zero-copy)
- **Fichier source** : 03-appels-systeme-linux.md
- **Sortie attendue** : `copy_file: OK`

### test_daemon.c
- **Section** : B.3 - Démon (fork/setsid)
- **Description** : Création d'un processus démon
- **Fichier source** : 03-appels-systeme-linux.md
- **Note** : Compilation seule (ne pas exécuter - crée un processus démon)

### test_tcpserver.c
- **Section** : B.3 - Réseau (socket TCP)
- **Description** : Serveur TCP simple (socket/bind/listen/accept)
- **Fichier source** : 03-appels-systeme-linux.md
- **Note** : Compilation seule (serveur bloquant en attente de connexion)
