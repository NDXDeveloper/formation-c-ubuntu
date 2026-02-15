🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 26.6 Principe du moindre privilège

## Introduction

Le **Principe du moindre privilège** (Principle of Least Privilege - PoLP) est un concept fondamental de sécurité informatique qui stipule :

> Un programme, un utilisateur ou un processus ne devrait avoir accès qu'aux ressources **strictement nécessaires** pour accomplir sa tâche, et rien de plus.

### Analogie simple

Imaginez une entreprise :
- Un employé de la comptabilité n'a pas besoin d'accéder aux dossiers médicaux
- Un stagiaire n'a pas besoin du code de la porte du coffre-fort
- Un visiteur n'a pas besoin d'un badge donnant accès à tous les étages

De même, un programme :
- Qui lit des fichiers de configuration n'a pas besoin de droits d'écriture
- Qui fait des calculs n'a pas besoin d'accéder au réseau
- Qui affiche des données n'a pas besoin de privilèges d'administrateur

---

## Pourquoi le principe du moindre privilège est-il crucial ?

### 1. Limiter l'impact des vulnérabilités

Même avec du code parfait, des bugs existent. Si une vulnérabilité est exploitée, le principe du moindre privilège **limite les dégâts**.

**Scénario sans PoLP** :
```
Programme vulnérable avec privilèges root
    ↓
Attaquant exploite la faille
    ↓
Attaquant obtient un accès root
    ↓
Système entièrement compromis
```

**Scénario avec PoLP** :
```
Programme vulnérable avec privilèges limités
    ↓
Attaquant exploite la faille
    ↓
Attaquant obtient des privilèges limités
    ↓
Dégâts contenus dans un sandbox
```

### 2. Réduire la surface d'attaque

Plus un programme a de privilèges, plus il y a de façons de l'exploiter.

**Sans PoLP** : Un programme avec accès root peut :
- Lire/écrire n'importe quel fichier
- Modifier la configuration système
- Installer des backdoors
- Écouter le réseau
- Accéder à tous les processus

**Avec PoLP** : Un programme limité peut uniquement :
- Lire ses fichiers de configuration
- Écrire dans son répertoire dédié
- Accéder au réseau si nécessaire

### 3. Faciliter le débogage et l'audit

Avec des privilèges limités, il est plus facile de :
- Tracer les actions du programme
- Comprendre ce qui a mal tourné
- Détecter des comportements anormaux

---

## Niveaux de privilèges sous Linux

### Hiérarchie des utilisateurs

```
root (UID 0) - Administrateur système
    ↓
Utilisateurs système (UID 1-999)
    ↓
Utilisateurs normaux (UID 1000+)
    ↓
Utilisateurs restreints
```

### Commandes pour vérifier les privilèges

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

void afficher_privileges(void) {
    printf("UID réel      : %d\n", getuid());      // UID de l'utilisateur qui a lancé le programme
    printf("UID effectif  : %d\n", geteuid());     // UID avec lequel le programme s'exécute
    printf("GID réel      : %d\n", getgid());      // GID de l'utilisateur
    printf("GID effectif  : %d\n", getegid());     // GID avec lequel le programme s'exécute
}

int main(void) {
    afficher_privileges();
    return 0;
}
```

**Compilation et test** :
```bash
gcc privileges.c -o privileges
./privileges
# UID réel      : 1000
# UID effectif  : 1000
# GID réel      : 1000
# GID effectif  : 1000
```

---

## Application du PoLP en programmation C

### 1. Ne jamais exécuter en root sauf nécessité absolue

```c
// ❌ MAUVAIS : Programme qui s'attend à tourner en root
int main(void) {
    if (geteuid() != 0) {
        fprintf(stderr, "Ce programme doit être lancé en root\n");
        return 1;
    }

    // Code avec privilèges root...
    return 0;
}
```

**Problème** : Si une vulnérabilité est exploitée, l'attaquant a un accès root complet.

```c
// ✅ BON : Programme qui fonctionne sans privilèges élevés
int main(void) {
    if (geteuid() == 0) {
        fprintf(stderr, "ATTENTION : Ne pas lancer ce programme en root\n");
        return 1;
    }

    // Code avec privilèges normaux...
    return 0;
}
```

### 2. Abandonner les privilèges dès que possible

Si votre programme a besoin temporairement de privilèges élevés (par exemple, pour ouvrir un port < 1024), abandonnez-les immédiatement après.

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

int main(void) {
    // Supposons que le programme est lancé avec setuid root

    if (geteuid() != 0) {
        fprintf(stderr, "Privilèges insuffisants\n");
        return 1;
    }

    printf("Privilèges root : UID effectif = %d\n", geteuid());

    // === OPÉRATION PRIVILÉGIÉE ===
    // Par exemple : ouvrir un port < 1024, lire un fichier protégé, etc.
    FILE *fichier_protege = fopen("/etc/shadow", "r");
    if (fichier_protege != NULL) {
        printf("Fichier protégé ouvert\n");
        fclose(fichier_protege);
    }

    // === ABANDONNER LES PRIVILÈGES ROOT ===
    uid_t uid_reel = getuid();
    gid_t gid_reel = getgid();

    // Abandonner les privilèges de groupe d'abord
    if (setgid(gid_reel) != 0) {
        perror("Erreur setgid");
        return 1;
    }

    // Puis abandonner les privilèges utilisateur
    if (setuid(uid_reel) != 0) {
        perror("Erreur setuid");
        return 1;
    }

    printf("Privilèges abandonnés : UID effectif = %d\n", geteuid());

    // === SUITE DU PROGRAMME SANS PRIVILÈGES ===
    // Vérifier qu'on ne peut plus reprendre les privilèges
    if (setuid(0) == 0) {
        fprintf(stderr, "ERREUR : Peut encore obtenir root !\n");
        return 1;
    }

    printf("Impossible de reprendre root : OK\n");

    // Le reste du programme s'exécute avec les privilèges de l'utilisateur normal
    // Si une vulnérabilité est exploitée ici, l'attaquant n'a PAS root

    return 0;
}
```

**Compilation et test** :
```bash
gcc -o drop_privileges drop_privileges.c  
sudo chown root:root drop_privileges  
sudo chmod u+s drop_privileges  # Setuid bit  
./drop_privileges
```

### 3. Utiliser des utilisateurs dédiés

Créez des utilisateurs système dédiés pour vos applications :

```bash
# Créer un utilisateur système sans shell
sudo useradd -r -s /bin/false monappli

# Créer des répertoires dédiés
sudo mkdir -p /var/lib/monappli  
sudo mkdir -p /var/log/monappli  

# Définir les permissions
sudo chown -R monappli:monappli /var/lib/monappli  
sudo chown -R monappli:monappli /var/log/monappli  
sudo chmod 750 /var/lib/monappli  
sudo chmod 750 /var/log/monappli  
```

Le programme peut alors être lancé :
```bash
sudo -u monappli ./mon_programme
```

---

## Permissions de fichiers

### Comprendre les permissions Unix

```
-rwxr-xr--
 |||||||||
 ||||||||└─ Autres : lecture
 |||||||└── Autres : pas d'écriture
 ||||||└─── Autres : pas d'exécution
 |||||└──── Groupe : lecture
 ||||└───── Groupe : pas d'écriture
 |||└────── Groupe : exécution
 ||└─────── Propriétaire : lecture
 |└──────── Propriétaire : écriture
 └───────── Propriétaire : exécution
```

### Définir les bonnes permissions

```c
#include <sys/stat.h>
#include <fcntl.h>

// ❌ MAUVAIS : Fichier lisible par tous
int fd = open("secret.txt", O_CREAT | O_WRONLY, 0666);
// Permissions : rw-rw-rw- (tout le monde peut lire)

// ✅ BON : Fichier lisible uniquement par le propriétaire
int fd = open("secret.txt", O_CREAT | O_WRONLY, 0600);
// Permissions : rw------- (seul le propriétaire peut lire/écrire)
```

```c
// ❌ MAUVAIS : Répertoire accessible par tous
mkdir("/tmp/monappli", 0777);  // rwxrwxrwx

// ✅ BON : Répertoire accessible uniquement par le propriétaire
mkdir("/tmp/monappli", 0700);  // rwx------
```

### Vérifier les permissions avant utilisation

```c
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

bool verifier_permissions_fichier(const char *chemin) {
    struct stat st;

    if (stat(chemin, &st) != 0) {
        perror("Erreur stat");
        return false;
    }

    // Vérifier que le fichier appartient à l'utilisateur
    if (st.st_uid != getuid()) {
        fprintf(stderr, "Le fichier n'appartient pas à l'utilisateur courant\n");
        return false;
    }

    // Vérifier que seul le propriétaire peut lire/écrire
    mode_t mode = st.st_mode & 0777;
    if (mode != 0600 && mode != 0400) {
        fprintf(stderr, "Permissions du fichier trop permissives : %o\n", mode);
        return false;
    }

    return true;
}

int main(void) {
    const char *fichier_sensible = "credentials.txt";

    if (!verifier_permissions_fichier(fichier_sensible)) {
        fprintf(stderr, "Fichier sensible non sécurisé\n");
        return 1;
    }

    // Utilisation du fichier...

    return 0;
}
```

---

## Capabilities Linux

Les **capabilities** permettent de diviser les privilèges root en unités plus petites.

### Problème avec setuid root

```
Programme setuid root
    ↓
Possède TOUS les privilèges root
    ↓
Alors qu'il n'a besoin que d'un seul privilège
```

### Solution avec capabilities

Au lieu de donner tous les privilèges root, donnez seulement la capability nécessaire.

**Exemple** : Programme qui a besoin de binder un port < 1024

```c
// serveur.c
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main(void) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return 1;
    }

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(80);  // Port 80 (< 1024, nécessite privilèges)
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        fprintf(stderr, "Vous n'avez pas les privilèges nécessaires\n");
        close(sock);
        return 1;
    }

    printf("Port 80 bindé avec succès\n");

    // Suite du serveur...

    close(sock);
    return 0;
}
```

**Compilation et attribution de capability** :
```bash
gcc serveur.c -o serveur

# Au lieu de : sudo chmod u+s serveur (setuid root)
# Utiliser :
sudo setcap cap_net_bind_service=+ep serveur

# Vérifier
getcap serveur
# serveur = cap_net_bind_service+ep

# Lancer sans sudo
./serveur
# Port 80 bindé avec succès
```

**Avantages** :
- Le programme peut binder le port 80
- Mais il n'a PAS les autres privilèges root
- Si compromis, l'attaquant ne peut pas faire grand-chose

### Capabilities courantes

| Capability | Description | Usage |
|------------|-------------|-------|
| `CAP_NET_BIND_SERVICE` | Binder ports < 1024 | Serveurs web, DNS |
| `CAP_NET_RAW` | Utiliser sockets RAW | ping, traceroute |
| `CAP_CHOWN` | Changer propriétaire fichiers | Gestionnaires de fichiers |
| `CAP_DAC_OVERRIDE` | Ignorer permissions fichiers | Backup tools |
| `CAP_SETUID` / `CAP_SETGID` | Changer UID/GID | Daemons |
| `CAP_SYS_ADMIN` | Administration système | Outils système |
| `CAP_SYS_PTRACE` | Tracer d'autres processus | Debuggers |

### Vérifier les capabilities dans le code

```c
#include <stdio.h>
#include <sys/capability.h>
#include <stdlib.h>

void afficher_capabilities(void) {
    cap_t caps = cap_get_proc();
    if (caps == NULL) {
        perror("cap_get_proc");
        return;
    }

    char *texte = cap_to_text(caps, NULL);
    if (texte != NULL) {
        printf("Capabilities : %s\n", texte);
        cap_free(texte);
    }

    cap_free(caps);
}

int main(void) {
    afficher_capabilities();
    return 0;
}
```

**Compilation** :
```bash
gcc -o check_caps check_caps.c -lcap
```

---

## Isolation et sandboxing

### 1. chroot - Emprisonnement du système de fichiers

`chroot` change la racine du système de fichiers pour un processus.

```c
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>

int main(void) {
    // Nécessite privilèges root
    if (geteuid() != 0) {
        fprintf(stderr, "Ce programme nécessite root\n");
        return 1;
    }

    // Créer un environnement chrooté
    const char *jail = "/var/jail/monappli";

    if (chroot(jail) != 0) {
        perror("chroot");
        return 1;
    }

    // Changer vers le nouveau répertoire racine
    if (chdir("/") != 0) {
        perror("chdir");
        return 1;
    }

    printf("Chrooté dans %s\n", jail);

    // Abandonner root maintenant que le chroot est fait
    uid_t uid_reel = getuid();
    gid_t gid_reel = getgid();

    if (setgid(gid_reel) != 0 || setuid(uid_reel) != 0) {
        perror("Impossible d'abandonner les privilèges");
        return 1;
    }

    // Le processus ne peut maintenant voir que /var/jail/monappli comme /
    // Il ne peut pas accéder au reste du système de fichiers

    return 0;
}
```

### 2. Namespaces - Isolation complète

Les namespaces Linux isolent différentes ressources système.

```c
#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int fonction_enfant(void *arg) {
    (void)arg;
    printf("Dans le namespace isolé\n");
    printf("PID : %d\n", getpid());  // Devrait être 1 dans le nouveau namespace

    // Le processus est isolé

    return 0;
}

int main(void) {
    // Créer un nouveau processus avec namespaces isolés
    const int STACK_SIZE = 1024 * 1024;
    char *stack = malloc(STACK_SIZE);
    if (stack == NULL) {
        perror("malloc");
        return 1;
    }

    // Flags pour isolation
    int flags = CLONE_NEWPID |   // Namespace PID isolé
                CLONE_NEWNET |   // Namespace réseau isolé
                CLONE_NEWNS |    // Namespace montage isolé
                CLONE_NEWUTS;    // Namespace hostname isolé

    pid_t pid = clone(fonction_enfant, stack + STACK_SIZE, flags | SIGCHLD, NULL);

    if (pid == -1) {
        perror("clone");
        fprintf(stderr, "Nécessite CAP_SYS_ADMIN ou root\n");
        free(stack);
        return 1;
    }

    printf("Processus parent : PID enfant = %d\n", pid);

    // Attendre la fin du processus enfant
    waitpid(pid, NULL, 0);

    free(stack);
    return 0;
}
```

### 3. seccomp - Filtrage des appels système

`seccomp` permet de restreindre les appels système qu'un processus peut faire.

```c
#include <stdio.h>
#include <unistd.h>
#include <sys/prctl.h>
#include <linux/seccomp.h>
#include <linux/filter.h>
#include <linux/audit.h>
#include <sys/syscall.h>

void activer_seccomp_strict(void) {
    // Mode strict : seuls read, write, exit et sigreturn sont autorisés
    if (prctl(PR_SET_SECCOMP, SECCOMP_MODE_STRICT) != 0) {
        perror("prctl");
        _exit(1);
    }

    printf("Mode seccomp strict activé\n");
}

int main(void) {
    printf("Avant seccomp : tout fonctionne\n");

    // Activer seccomp
    activer_seccomp_strict();

    // Ces opérations fonctionnent (autorisées)
    printf("Après seccomp : écriture OK\n");

    // Cette opération causera un SIGKILL (non autorisée)
    // getpid();  // Décommenter pour tester

    printf("Fin du programme\n");
    return 0;
}
```

**Note** : En production, on utilise souvent `seccomp-bpf` avec des filtres plus sophistiqués.

---

## Exemples pratiques

### Exemple 1 : Serveur web minimaliste sécurisé

```c
// serveur_securise.c
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pwd.h>
#include <grp.h>

#define PORT 8080
#define USER "www-data"

bool abandonner_privileges(void) {
    // Récupérer les infos de l'utilisateur cible
    struct passwd *pw = getpwnam(USER);
    if (pw == NULL) {
        fprintf(stderr, "Utilisateur %s introuvable\n", USER);
        return false;
    }

    // Abandonner les privilèges de groupe d'abord
    if (setgid(pw->pw_gid) != 0) {
        perror("setgid");
        return false;
    }

    // Initialiser les groupes supplémentaires
    if (initgroups(USER, pw->pw_gid) != 0) {
        perror("initgroups");
        return false;
    }

    // Abandonner les privilèges utilisateur
    if (setuid(pw->pw_uid) != 0) {
        perror("setuid");
        return false;
    }

    // Vérifier qu'on ne peut plus reprendre root
    if (setuid(0) == 0) {
        fprintf(stderr, "ERREUR : Peut encore obtenir root\n");
        return false;
    }

    printf("Privilèges abandonnés : maintenant %s (UID %d)\n", USER, getuid());
    return true;
}

int creer_serveur(int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return -1;
    }

    // Réutiliser l'adresse
    int opt = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(sock);
        return -1;
    }

    if (listen(sock, 10) < 0) {
        perror("listen");
        close(sock);
        return -1;
    }

    printf("Serveur en écoute sur le port %d\n", port);
    return sock;
}

int main(void) {
    // Vérifier qu'on est root au départ
    if (geteuid() != 0) {
        fprintf(stderr, "Ce programme doit être lancé en root\n");
        return 1;
    }

    // 1. Créer le socket (nécessite root si port < 1024)
    int server_sock = creer_serveur(PORT);
    if (server_sock < 0) {
        return 1;
    }

    // 2. Abandonner les privilèges root AVANT de traiter les requêtes
    if (!abandonner_privileges()) {
        close(server_sock);
        return 1;
    }

    // 3. Boucle principale du serveur (sans privilèges)
    printf("Serveur prêt (sans privilèges root)\n");

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_sock = accept(server_sock,
                                (struct sockaddr*)&client_addr,
                                &client_len);

        if (client_sock < 0) {
            perror("accept");
            continue;
        }

        // Traiter la requête (code simplifié)
        const char *response = "HTTP/1.1 200 OK\r\n"
                              "Content-Type: text/plain\r\n"
                              "\r\n"
                              "Hello from secure server!\n";

        send(client_sock, response, strlen(response), 0);
        close(client_sock);
    }

    close(server_sock);
    return 0;
}
```

**Lancement** :
```bash
gcc serveur_securise.c -o serveur_securise  
sudo ./serveur_securise  
```

### Exemple 2 : Programme de traitement de fichiers

```c
// traiter_fichier.c
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>

bool verifier_securite_chemin(const char *chemin) {
    // Résoudre le chemin canonique
    char chemin_reel[PATH_MAX];
    if (realpath(chemin, chemin_reel) == NULL) {
        perror("realpath");
        return false;
    }

    // Vérifier que le chemin est dans un répertoire autorisé
    const char *repertoires_autorises[] = {
        "/home/user/documents",
        "/tmp/monappli"
    };

    bool autorise = false;
    for (size_t i = 0; i < sizeof(repertoires_autorises) / sizeof(char*); i++) {
        if (strncmp(chemin_reel, repertoires_autorises[i],
                   strlen(repertoires_autorises[i])) == 0) {
            autorise = true;
            break;
        }
    }

    if (!autorise) {
        fprintf(stderr, "Accès refusé : fichier hors des répertoires autorisés\n");
        return false;
    }

    // Vérifier les permissions du fichier
    struct stat st;
    if (stat(chemin_reel, &st) != 0) {
        perror("stat");
        return false;
    }

    // Vérifier que le fichier appartient à l'utilisateur
    if (st.st_uid != getuid()) {
        fprintf(stderr, "Accès refusé : fichier n'appartient pas à l'utilisateur\n");
        return false;
    }

    return true;
}

bool traiter_fichier(const char *chemin) {
    // Vérifications de sécurité
    if (!verifier_securite_chemin(chemin)) {
        return false;
    }

    // Ouvrir le fichier en lecture seule (pas d'écriture)
    int fd = open(chemin, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return false;
    }

    // Traitement du fichier...
    char buffer[4096];
    ssize_t bytes_lus;

    while ((bytes_lus = read(fd, buffer, sizeof(buffer))) > 0) {
        // Traiter les données (exemple : compter les lignes)
    }

    close(fd);
    return true;
}

int main(int argc, char *argv[]) {
    // Vérifier qu'on ne tourne PAS en root
    if (geteuid() == 0) {
        fprintf(stderr, "ERREUR : Ne pas lancer ce programme en root\n");
        return 1;
    }

    if (argc != 2) {
        fprintf(stderr, "Usage : %s <fichier>\n", argv[0]);
        return 1;
    }

    if (!traiter_fichier(argv[1])) {
        fprintf(stderr, "Échec du traitement\n");
        return 1;
    }

    printf("Traitement réussi\n");
    return 0;
}
```

### Exemple 3 : Daemon sécurisé

```c
// daemon_securise.c
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <syslog.h>
#include <pwd.h>
#include <grp.h>

#define DAEMON_USER "daemon"
#define PID_FILE "/var/run/mondaemon.pid"
#define WORK_DIR "/var/lib/mondaemon"

bool devenir_daemon(void) {
    // Forker et laisser le parent mourir
    pid_t pid = fork();
    if (pid < 0) {
        return false;
    }
    if (pid > 0) {
        exit(0);  // Parent
    }

    // Créer une nouvelle session
    if (setsid() < 0) {
        return false;
    }

    // Forker à nouveau pour éviter de ré-acquérir un terminal
    pid = fork();
    if (pid < 0) {
        return false;
    }
    if (pid > 0) {
        exit(0);
    }

    // Changer vers un répertoire sûr
    if (chdir(WORK_DIR) < 0) {
        return false;
    }

    // Définir umask pour les permissions de fichiers
    umask(027);  // rwxr-x--- par défaut

    // Fermer les descripteurs de fichiers standards
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // Rediriger vers /dev/null
    open("/dev/null", O_RDONLY);  // stdin
    open("/dev/null", O_WRONLY);  // stdout
    open("/dev/null", O_WRONLY);  // stderr

    return true;
}

bool ecrire_pid_file(void) {
    int fd = open(PID_FILE, O_WRONLY | O_CREAT | O_EXCL, 0644);
    if (fd < 0) {
        return false;
    }

    char pid_str[32];
    snprintf(pid_str, sizeof(pid_str), "%d\n", getpid());
    write(fd, pid_str, strlen(pid_str));
    close(fd);

    return true;
}

bool abandonner_privileges_daemon(void) {
    struct passwd *pw = getpwnam(DAEMON_USER);
    if (pw == NULL) {
        return false;
    }

    if (setgid(pw->pw_gid) != 0) return false;
    if (initgroups(DAEMON_USER, pw->pw_gid) != 0) return false;
    if (setuid(pw->pw_uid) != 0) return false;

    return true;
}

int main(void) {
    // Ouvrir syslog
    openlog("mondaemon", LOG_PID, LOG_DAEMON);

    // Vérifier qu'on est root
    if (geteuid() != 0) {
        syslog(LOG_ERR, "Doit être lancé en root");
        return 1;
    }

    // Devenir daemon
    if (!devenir_daemon()) {
        syslog(LOG_ERR, "Impossible de devenir daemon");
        return 1;
    }

    syslog(LOG_INFO, "Daemon démarré");

    // Écrire le PID file (nécessite root)
    if (!ecrire_pid_file()) {
        syslog(LOG_ERR, "Impossible d'écrire le PID file");
        return 1;
    }

    // Abandonner les privilèges root
    if (!abandonner_privileges_daemon()) {
        syslog(LOG_ERR, "Impossible d'abandonner les privilèges");
        return 1;
    }

    syslog(LOG_INFO, "Privilèges abandonnés, maintenant UID %d", getuid());

    // Boucle principale du daemon (sans privilèges)
    while (1) {
        // Travail du daemon...
        sleep(60);
    }

    // Nettoyage
    unlink(PID_FILE);
    closelog();
    return 0;
}
```

---

## Checklist du principe du moindre privilège

### ✅ Conception

- [ ] Le programme fonctionne-t-il sans root ?
- [ ] Quels privilèges sont réellement nécessaires ?
- [ ] Peut-on utiliser des capabilities au lieu de setuid root ?
- [ ] Y a-t-il un utilisateur système dédié ?

### ✅ Code

- [ ] Les privilèges sont abandonnés dès que possible
- [ ] Impossible de reprendre les privilèges après les avoir abandonnés
- [ ] Vérification : `setuid(0)` échoue après avoir abandonné root
- [ ] Les fichiers sont créés avec les bonnes permissions (0600, 0700)
- [ ] Les chemins de fichiers sont validés et canonicalisés
- [ ] Pas d'accès root si non nécessaire

### ✅ Déploiement

- [ ] Programme lancé avec un utilisateur dédié
- [ ] Répertoires de travail avec permissions restreintes
- [ ] Fichiers de configuration en lecture seule
- [ ] Fichiers sensibles (logs, données) accessibles uniquement par l'application
- [ ] SELinux ou AppArmor configuré si disponible

### ✅ Tests

- [ ] Tester que le programme fonctionne sans root
- [ ] Tester avec un utilisateur non privilégié
- [ ] Vérifier qu'on ne peut pas reprendre les privilèges
- [ ] Tester l'accès aux fichiers avec différents utilisateurs

---

## Techniques avancées

### 1. Pledge (OpenBSD)

OpenBSD a introduit `pledge()` qui permet de déclarer les opérations autorisées.

```c
#include <unistd.h>

int main(void) {
    // Déclarer qu'on a besoin uniquement de stdio
    if (pledge("stdio", NULL) != 0) {
        perror("pledge");
        return 1;
    }

    // Le programme peut maintenant uniquement :
    // - lire/écrire sur les fd existants
    // - pas de network, pas de fichiers, pas de fork

    printf("Hello from pledged program\n");

    // Toute tentative d'ouvrir un fichier ou un socket sera tuée

    return 0;
}
```

**Note** : `pledge` n'est disponible que sur OpenBSD, mais illustre une approche intéressante.

### 2. Landlock (Linux ≥ 5.13)

Landlock est un module de sécurité Linux qui permet de restreindre l'accès au système de fichiers.

```c
#include <linux/landlock.h>
#include <sys/syscall.h>
#include <unistd.h>

// Exemple simplifié (nécessite Linux ≥ 5.13)
int restreindre_acces_fichiers(void) {
    // Créer un ruleset Landlock
    struct landlock_ruleset_attr ruleset_attr = {
        .handled_access_fs = LANDLOCK_ACCESS_FS_READ_FILE |
                            LANDLOCK_ACCESS_FS_WRITE_FILE
    };

    int ruleset_fd = syscall(SYS_landlock_create_ruleset,
                            &ruleset_attr, sizeof(ruleset_attr), 0);
    if (ruleset_fd < 0) {
        return -1;
    }

    // Ajouter des règles (par exemple, autoriser /home/user/documents)
    // ...

    // Appliquer le ruleset
    if (syscall(SYS_landlock_restrict_self, ruleset_fd, 0) < 0) {
        close(ruleset_fd);
        return -1;
    }

    close(ruleset_fd);
    return 0;
}
```

### 3. SELinux / AppArmor

Utiliser les systèmes de sécurité obligatoire du système :

**SELinux** (Red Hat, Fedora, CentOS) :
```bash
# Vérifier le contexte SELinux
ls -Z /usr/bin/monprogramme

# Définir un contexte personnalisé
chcon -t mon_domaine_t /usr/bin/monprogramme
```

**AppArmor** (Ubuntu, Debian) :
```bash
# Créer un profil AppArmor
sudo aa-genprof /usr/bin/monprogramme

# Activer le profil
sudo aa-enforce /usr/bin/monprogramme
```

---

## Erreurs courantes à éviter

### 1. Abandonner les privilèges dans le mauvais ordre

```c
// ❌ MAUVAIS : abandonner UID avant GID
setuid(user_uid);  
setgid(user_gid);  // Peut échouer car déjà plus de privilèges  
```

```c
// ✅ BON : GID d'abord, puis UID
setgid(user_gid);  // OK car on a encore root  
setuid(user_uid);  // Abandonne complètement root  
```

### 2. Oublier de vérifier qu'on ne peut plus reprendre root

```c
// ❌ Incomplet
setuid(user_uid);
// Pas de vérification

// ✅ Complet
setuid(user_uid);  
if (setuid(0) == 0) {  
    fprintf(stderr, "ERREUR : Peut encore obtenir root !\n");
    abort();
}
```

### 3. Permissions de fichiers trop permissives

```c
// ❌ Trop permissif
open("secret.txt", O_CREAT, 0666);  // Lisible par tous

// ✅ Restrictif
open("secret.txt", O_CREAT, 0600);  // Seul le propriétaire
```

### 4. Faire confiance aux variables d'environnement

```c
// ❌ Dangereux avec setuid
char *path = getenv("PATH");
// PATH peut être contrôlé par l'attaquant

// ✅ Définir explicitement
setenv("PATH", "/usr/bin:/bin", 1);
```

### 5. Setuid sur des scripts shell

```bash
#!/bin/bash
# ❌ DANGEREUX : setuid sur un script shell
```

**Problème** : Les scripts shell sont sujets à de nombreuses attaques (injection, race conditions).

**Solution** : Écrire un wrapper en C qui appelle le script ou réimplémenter en C.

---

## Résumé : Les règles d'or du moindre privilège

### 🎯 Principes fondamentaux

1. **Minimiser les privilèges dès la conception**
   - Ne demander que les permissions nécessaires
   - Pas de root si possible

2. **Abandonner les privilèges dès que possible**
   - Faire les opérations privilégiées en premier
   - Abandonner immédiatement après

3. **Vérifier l'abandon des privilèges**
   - S'assurer qu'on ne peut plus reprendre root
   - Tester avec `setuid(0)`

4. **Utiliser des mécanismes modernes**
   - Capabilities au lieu de setuid root
   - Namespaces pour l'isolation
   - seccomp pour filtrer les syscalls

5. **Sécuriser le système de fichiers**
   - Permissions restrictives (0600, 0700)
   - Utilisateurs dédiés
   - Validation des chemins

### 📋 Ordre correct d'abandon des privilèges

```
1. Opérations nécessitant root
   ↓
2. setgid(gid_cible)
   ↓
3. initgroups(user, gid_cible)
   ↓
4. setuid(uid_cible)
   ↓
5. Vérifier : setuid(0) échoue
   ↓
6. Continuer sans privilèges
```

### 💡 Conseils pratiques

**Pour un serveur** :
- Créer un utilisateur dédié (`www-data`, `nginx`, etc.)
- Binder le port en root, puis abandonner les privilèges
- Ou utiliser capabilities (`CAP_NET_BIND_SERVICE`)

**Pour un daemon** :
- Utiliser systemd avec `User=` et `Group=` (recommandé)
- Ou abandonner les privilèges après la phase d'initialisation
- Écrire les fichiers importants (PID, logs) avant d'abandonner root

**Pour un outil de traitement** :
- Ne jamais nécessiter root
- Valider tous les chemins de fichiers
- Permissions restrictives sur les fichiers créés

### 🔒 Défense en profondeur

Le principe du moindre privilège est une **couche** de sécurité :

```
Couche 1 : Code sans vulnérabilités
    ↓ (Si échec)
Couche 2 : Validation des entrées
    ↓ (Si échec)
Couche 3 : Moindre privilège ← Limite les dégâts
    ↓ (Si échec)
Couche 4 : Isolation (chroot, namespaces)
    ↓ (Si échec)
Couche 5 : Monitoring et détection
```

### ⚠️ Ce que le moindre privilège ne fait PAS

- Ne corrige pas les bugs dans votre code
- Ne remplace pas la validation des entrées
- Ne protège pas contre toutes les attaques

**Mais** : Il limite considérablement l'impact d'une exploitation réussie.

---

## Conclusion

Le **principe du moindre privilège** est un pilier de la sécurité :

- ✅ **Simple à comprendre** : Donner uniquement ce qui est nécessaire
- ✅ **Efficace** : Limite l'impact des failles
- ✅ **Pratique** : Mécanismes intégrés à Linux (setuid, capabilities, namespaces)
- ✅ **Défensif** : Protège même quand le code a des bugs

### Message clé

> Un programme qui n'a pas besoin de root ne doit JAMAIS tourner en root.
>
> Un programme qui a temporairement besoin de root doit l'abandonner dès que possible.

En appliquant rigoureusement le principe du moindre privilège, vous transformez une vulnérabilité potentiellement catastrophique en un incident mineur et contrôlable.

**Prochaine étape** : La section suivante sur l'analyse statique et le fuzzing vous montrera comment détecter les vulnérabilités avant qu'elles ne soient exploitées.

⏭️ [Static analysis et fuzzing](/26-securite-code-defensif/07-static-analysis-fuzzing.md)
