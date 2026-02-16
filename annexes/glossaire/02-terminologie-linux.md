🔝 Retour au [Sommaire](/SOMMAIRE.md)

# D.2 Terminologie système Linux

## Glossaire des termes essentiels des systèmes Linux/Unix

Ce glossaire rassemble les termes fondamentaux relatifs à Linux et aux systèmes Unix-like que vous rencontrerez lors de la programmation système en C. Chaque définition est accompagnée d'explications accessibles pour faciliter votre compréhension.

---

## A

### **Appel système (System call / syscall)**
Interface entre un programme utilisateur et le noyau Linux. Permet de demander des services du système d'exploitation (lecture fichier, création processus, allocation mémoire, etc.). En C, on utilise souvent des wrappers de la libc (`open()`, `read()`, `write()`) plutôt que les syscalls directs.
```c
// Wrapper libc (recommandé)
int fd = open("/tmp/file.txt", O_RDONLY);

// Syscall direct (rare)
syscall(SYS_open, "/tmp/file.txt", O_RDONLY);
```

### **Architecture x86_64 (AMD64)**
Architecture 64 bits dominante pour les processeurs Intel et AMD. Successeur de x86 (32 bits). La plupart des distributions Linux modernes ciblent cette architecture.

### **ARM**
Famille d'architectures de processeurs RISC largement utilisée dans l'embarqué, les smartphones et désormais les serveurs (ARM64/AArch64). Linux supporte pleinement ARM.

### **Atomic operation**
Opération garantie de s'exécuter complètement sans interruption, même en présence de multithreading. Essentielle pour la synchronisation lock-free.

---

## B

### **Background (Arrière-plan)**
Mode d'exécution d'un processus qui tourne sans monopoliser le terminal. Opposé de **foreground** (premier plan).
```bash
# Lancer un programme en background
./mon_programme &
```

### **Bash (Bourne Again Shell)**
Shell par défaut sur la plupart des distributions Linux. Interprète de commandes permettant l'interaction avec le système.

### **Binary (Binaire)**
Fichier exécutable compilé contenant du code machine. Sous Linux, souvent au format ELF.

### **Block device**
Périphérique accessible par blocs de données (disques durs, SSD, clés USB). Permet l'accès aléatoire. Opposé de **character device**.

### **Boot**
Processus de démarrage du système, depuis le firmware (BIOS/UEFI) jusqu'au chargement du noyau et du système d'initialisation.

### **Bootloader**
Programme chargeant le noyau au démarrage. Exemples : GRUB, systemd-boot.

### **BSD (Berkeley Software Distribution)**
Famille de systèmes Unix dont dérive une partie de l'ADN de Linux (notamment les sockets réseau). FreeBSD, OpenBSD, NetBSD sont des systèmes BSD modernes.

### **Buffer cache**
Cache mémoire du noyau pour accélérer les accès aux périphériques bloc (disques).

### **Build system**
Système automatisant la compilation de projets complexes. Exemples : Make, CMake, Meson, Ninja.

---

## C

### **Cache (mémoire cache)**
Mémoire rapide (L1, L2, L3) entre le CPU et la RAM. Stocke temporairement les données fréquemment accédées pour accélérer l'exécution.

### **cgroups (Control Groups)**
Mécanisme du noyau Linux limitant, comptabilisant et isolant les ressources (CPU, mémoire, I/O) utilisées par des groupes de processus. Base des conteneurs (Docker).

### **Character device**
Périphérique accessible caractère par caractère de manière séquentielle (terminaux, ports série). Opposé de **block device**.

### **Chroot**
Mécanisme changeant la racine du système de fichiers pour un processus. Utilisé pour l'isolation et la sécurité. Base conceptuelle des conteneurs.

### **CLI (Command Line Interface)**
Interface en ligne de commande. Mode d'interaction textuel avec le système via un shell.

### **Clone**
Appel système Linux créant un nouveau processus ou thread avec un contrôle fin sur ce qui est partagé (mémoire, descripteurs, etc.). Plus flexible que `fork()`.

### **Context switch (Changement de contexte)**
Opération où le noyau suspend un processus/thread et en active un autre. Sauvegarde et restauration de l'état CPU. Opération coûteuse.

### **Core**
1. Cœur physique d'un processeur multi-cœur
2. Fichier **core dump** généré lors d'un crash

### **CPU (Central Processing Unit)**
Processeur principal exécutant les instructions des programmes.

### **Cron / Crontab**
Démon Linux permettant de planifier l'exécution périodique de tâches (jobs).

---

## D

### **Daemon (Démon)**
Processus s'exécutant en arrière-plan sans terminal associé, généralement démarré au boot. Exemples : `sshd` (serveur SSH), `httpd` (serveur web), `systemd`.

### **Daemonization**
Processus de transformation d'un programme normal en daemon :
1. Fork et terminer le parent
2. Créer une nouvelle session (`setsid`)
3. Changer de répertoire (`chdir("/")`)
4. Fermer stdin/stdout/stderr
5. Optionnel : écrire un PID file

### **/dev**
Répertoire contenant les fichiers spéciaux représentant les périphériques matériels et virtuels.
- `/dev/null` : puits sans fond
- `/dev/zero` : source infinie de zéros
- `/dev/random` : générateur aléatoire
- `/dev/sda` : premier disque SATA

### **Descripteur de fichier (File descriptor)**
Entier identifiant un fichier ouvert au niveau processus.
- `0` = stdin (entrée standard)
- `1` = stdout (sortie standard)
- `2` = stderr (sortie erreur)

### **Device driver (Pilote de périphérique)**
Module du noyau permettant la communication avec un périphérique matériel.

### **Distribution Linux**
Système d'exploitation complet basé sur le noyau Linux, accompagné d'outils GNU et d'un gestionnaire de paquets. Exemples : Ubuntu, Debian, Fedora, Arch Linux.

### **dmesg**
Commande affichant les messages du buffer du noyau (logs de démarrage, erreurs hardware).

### **Docker**
Plateforme de conteneurisation utilisant les namespaces et cgroups Linux pour isoler des applications.

### **DMA (Direct Memory Access)**
Mécanisme permettant aux périphériques d'accéder directement à la RAM sans passer par le CPU.

---

## E

### **ELF (Executable and Linkable Format)**
Format standard des fichiers binaires sous Linux (exécutables, bibliothèques partagées, fichiers objets).

### **Environment variable (Variable d'environnement)**
Variable globale accessible par tous les processus d'une session. Exemples : `PATH`, `HOME`, `USER`, `SHELL`.
```c
char *home = getenv("HOME");
```

### **epoll**
Mécanisme Linux de surveillance efficace de multiples descripteurs de fichiers (sockets, pipes). Alternative moderne à `select()` et `poll()`, scalable pour des milliers de connexions.

### **/etc**
Répertoire contenant les fichiers de configuration du système.
- `/etc/passwd` : comptes utilisateurs
- `/etc/fstab` : points de montage
- `/etc/hosts` : résolution DNS locale

### **exec() (famille de fonctions)**
Famille d'appels système remplaçant l'image du processus courant par un nouveau programme. `execve()`, `execl()`, `execlp()`, etc.
```c
execl("/bin/ls", "ls", "-la", NULL);
```

### **Exit code (Code de sortie)**
Valeur entière retournée par un processus à son parent à sa terminaison. `0` = succès, autre = erreur. Récupéré via `wait()` ou `$?` en shell.

### **Exit status**
Voir **Exit code**.

---

## F

### **File system (Système de fichiers)**
Structure organisant le stockage des données sur un support. Linux supporte de nombreux systèmes : ext4, Btrfs, XFS, ZFS, etc.

### **FIFO (First In, First Out)**
1. Principe de file d'attente
2. **Named pipe** : mécanisme IPC via un fichier spécial créé avec `mkfifo()`

### **Firmware**
Logiciel de bas niveau contrôlant le matériel (BIOS, UEFI, firmware de périphériques).

### **Fork**
Appel système créant un nouveau processus (processus fils) qui est une copie du processus appelant (processus parent).
```c
pid_t pid = fork();  
if (pid == 0) {  
    // Code du processus fils
} else {
    // Code du processus parent
}
```

### **Foreground (Premier plan)**
Mode d'exécution d'un processus attaché au terminal et recevant les entrées clavier. Opposé de **background**.

### **Free Software**
Logiciel garantissant quatre libertés : utiliser, étudier, modifier, redistribuer. Linux et GNU sont des projets free software.

### **fsck (File System Check)**
Outil vérifiant et réparant l'intégrité d'un système de fichiers.

### **FTP (File Transfer Protocol)**
Protocole de transfert de fichiers. Version sécurisée : SFTP (sur SSH).

### **Futex (Fast Userspace Mutex)**
Primitive de synchronisation Linux combinant code utilisateur et noyau pour des mutex performants.

---

## G

### **GID (Group ID)**
Identifiant numérique d'un groupe d'utilisateurs. Utilisé pour les permissions de fichiers.

### **Git**
Système de contrôle de version distribué, créé par Linus Torvalds pour le développement du noyau Linux.

### **glibc (GNU C Library)**
Implémentation GNU de la bibliothèque standard C. Libc par défaut sur la plupart des distributions Linux.

### **GNU**
Projet de système d'exploitation libre lancé par Richard Stallman. Fournit de nombreux outils utilisés avec Linux (gcc, bash, coreutils).

### **GPL (GNU General Public License)**
Licence libre copyleft utilisée par le noyau Linux et de nombreux logiciels GNU.

### **Group**
Ensemble d'utilisateurs partageant des permissions d'accès communes.

### **GRUB (GRand Unified Bootloader)**
Bootloader par défaut de la plupart des distributions Linux.

---

## H

### **Hard link**
Lien créant une entrée de répertoire pointant vers le même inode qu'un fichier existant. Le fichier persiste tant qu'un hard link existe.
```bash
ln fichier_original lien_dur
```

### **Hardware**
Composants physiques de l'ordinateur (processeur, mémoire, disques, périphériques).

### **Heap**
Zone de mémoire pour l'allocation dynamique. Gérée par `malloc()` en C, par `brk()`/`sbrk()` au niveau système.

### **/home**
Répertoire contenant les répertoires personnels des utilisateurs (`/home/alice`, `/home/bob`).

### **Hostname**
Nom identifiant une machine sur un réseau.

### **HTTP (HyperText Transfer Protocol)**
Protocole de communication du web. Version sécurisée : HTTPS.

---

## I

### **I/O (Input/Output)**
Opérations d'entrée/sortie (lecture/écriture fichiers, réseau, périphériques).

### **init**
Premier processus lancé par le noyau (PID 1). Ancêtre de tous les autres processus. Remplacé par **systemd** sur la plupart des distributions modernes.

### **inode**
Structure de données du système de fichiers contenant les métadonnées d'un fichier (permissions, tailles, dates, pointeurs vers les blocs de données). Identifié par un numéro d'inode unique.

### **Interrupt (Interruption)**
Signal matériel ou logiciel suspendant temporairement l'exécution normale du CPU pour traiter un événement urgent (arrivée de données réseau, frappe clavier).

### **IPC (Inter-Process Communication)**
Mécanismes permettant aux processus de communiquer et synchroniser leurs actions :
- Pipes
- Named pipes (FIFO)
- Sockets
- Shared memory
- Message queues
- Sémaphores
- Signaux

### **ISO**
Format d'image disque, souvent utilisé pour les distributions Linux installables.

### **ioctl**
Appel système générique pour contrôler les périphériques et effectuer des opérations non standards.

---

## J

### **Job**
Tâche ou processus géré par le shell. Peut être en foreground ou background.

### **Journal (systemd-journald)**
Système de logging de systemd, remplaçant/complétant syslog.

---

## K

### **Kernel (Noyau)**
Cœur du système d'exploitation Linux. Gère les ressources matérielles, la mémoire, les processus, les I/O. Code privilégié s'exécutant en **kernel space**.

### **Kernel module**
Code chargeable/déchargeable dynamiquement dans le noyau (drivers, fonctionnalités). Extension du noyau sans recompilation.
```bash
lsmod           # Lister les modules chargés  
insmod module.ko  # Charger un module  
rmmod module    # Décharger un module  
```

### **Kernel panic**
Erreur fatale du noyau entraînant l'arrêt du système. Équivalent Linux du "Blue Screen of Death" Windows.

### **Kernel space**
Espace d'adressage privilégié où s'exécute le noyau. Accès complet au matériel. Opposé de **user space**.

### **Kill**
Commande/appel système envoyant un signal à un processus. Pas nécessairement pour tuer (SIGKILL), peut être utilisé pour d'autres signaux.
```bash
kill -9 1234      # SIGKILL (force la terminaison)  
kill -TERM 1234   # SIGTERM (demande propre)  
```

---

## L

### **LD_LIBRARY_PATH**
Variable d'environnement spécifiant les répertoires où chercher les bibliothèques partagées dynamiques.

### **ldd**
Commande affichant les bibliothèques dynamiques dont dépend un exécutable.
```bash
ldd /bin/ls
```

### **Library**
Voir **Bibliothèque** dans le glossaire C. Sous Linux : `.a` (statique), `.so` (partagée).

### **Link (Lien)**
Référence à un fichier. Deux types : **hard link** et **symbolic link**.

### **Linker (Éditeur de liens)**
Programme combinant les fichiers objets en un exécutable. Sous Linux : `ld` (souvent invoqué via `gcc`).

### **Linux**
1. Noyau de système d'exploitation créé par Linus Torvalds en 1991
2. Par extension, famille de systèmes d'exploitation utilisant ce noyau

### **Live CD/USB**
Système Linux démarrable depuis un support amovible sans installation sur le disque dur.

### **Localhost**
Adresse de loopback référençant la machine locale : `127.0.0.1` (IPv4) ou `::1` (IPv6).

### **Lock file**
Fichier créé pour signaler qu'une ressource est utilisée et empêcher l'accès concurrent.

### **Log**
Enregistrement d'événements système ou applicatifs. Souvent dans `/var/log/`.

### **Loopback**
Interface réseau virtuelle permettant à la machine de communiquer avec elle-même.

### **LTS (Long Term Support)**
Version d'une distribution (ou logiciel) maintenue sur une longue durée (5-10 ans pour Ubuntu LTS).

---

## M

### **Man page (Page de manuel)**
Documentation système accessible via la commande `man`.
```bash
man 2 open   # Section 2 : appels système  
man 3 printf # Section 3 : fonctions bibliothèque  
man 7 signal # Section 7 : conventions et divers  
```

### **Memory-mapped file (Fichier mappé en mémoire)**
Technique mappant un fichier dans l'espace d'adressage d'un processus via `mmap()`. Permet d'accéder au fichier comme à de la mémoire.

### **Mount (Monter)**
Action de rendre accessible un système de fichiers en l'attachant à un point de l'arborescence.
```bash
mount /dev/sda1 /mnt
```

### **Mount point**
Répertoire où un système de fichiers est monté.

### **Multicast**
Transmission réseau d'un paquet à un groupe de destinataires.

### **Multithreading**
Exécution de plusieurs threads au sein d'un même processus, partageant le même espace mémoire.

### **Mutex (Mutual Exclusion)**
Mécanisme de synchronisation garantissant qu'un seul thread accède à une ressource critique à la fois.

---

## N

### **Namespace**
Mécanisme d'isolation Linux créant des vues séparées des ressources système (PIDs, network, mount points, etc.). Base des conteneurs Docker.

### **Network interface**
Interface réseau physique (eth0, wlan0) ou virtuelle (lo pour loopback).

### **NFS (Network File System)**
Protocole permettant de monter des systèmes de fichiers distants sur le réseau.

### **Nice / Niceness**
Valeur de priorité d'un processus pour le scheduler. De -20 (haute priorité) à 19 (basse priorité).
```bash
nice -n 10 ./mon_programme  # Lance avec basse priorité
```

### **Node**
1. Machine dans un cluster
2. Fichier spécial dans `/dev/` représentant un périphérique

---

## O

### **Open-source**
Modèle de développement où le code source est publiquement accessible et modifiable. Linux est open-source.

### **Orphan process (Processus orphelin)**
Processus dont le parent est terminé. Ré-adopté par `init` (PID 1) ou `systemd`.

### **OOM (Out Of Memory)**
Situation où le système manque de mémoire. Le **OOM Killer** du noyau peut alors tuer des processus pour libérer de la RAM.

---

## P

### **Package manager (Gestionnaire de paquets)**
Outil d'installation, mise à jour et suppression de logiciels. Exemples :
- APT (Debian, Ubuntu)
- DNF/YUM (Fedora, RHEL)
- Pacman (Arch Linux)

### **Page (Page mémoire)**
Unité de gestion de la mémoire virtuelle. Taille typique : 4 KB sur x86_64.

### **Pagination**
Mécanisme de mémoire virtuelle divisant la mémoire en pages. Permet le **swap** et la protection mémoire.

### **Parent process**
Processus ayant créé un ou plusieurs processus fils via `fork()`.

### **Partition**
Division logique d'un disque dur. Chaque partition peut avoir son propre système de fichiers.

### **PATH**
Variable d'environnement contenant la liste des répertoires où chercher les exécutables.
```bash
echo $PATH
# /usr/local/bin:/usr/bin:/bin
```

### **Permissions**
Droits d'accès aux fichiers et répertoires : lecture (r), écriture (w), exécution (x), pour le propriétaire, le groupe, et les autres.
```bash
ls -l fichier.txt
-rw-r--r-- 1 user group 1024 Dec 1 10:00 fichier.txt
```

### **PID (Process ID)**
Identifiant numérique unique d'un processus. Le premier processus (init/systemd) a le PID 1.

### **Pipe (Tube)**
Mécanisme IPC permettant la communication unidirectionnelle entre processus. Opérateur shell : `|`
```bash
cat fichier.txt | grep "erreur" | wc -l
```
En C : `pipe()` crée un pipe anonyme.

### **PID file**
Fichier contenant le PID d'un daemon, souvent dans `/var/run/`.

### **Poll**
Appel système surveillant plusieurs descripteurs de fichiers pour détecter une activité I/O.

### **Port**
Numéro identifiant un point de terminaison de communication réseau (0-65535). Ports connus : 80 (HTTP), 443 (HTTPS), 22 (SSH).

### **POSIX (Portable Operating System Interface)**
Famille de standards définissant l'API des systèmes Unix-like. Facilite la portabilité du code.

### **PPID (Parent Process ID)**
PID du processus parent.

### **Priority**
Importance relative d'un processus pour le scheduler. Voir **Nice**.

### **/proc**
Système de fichiers virtuel exposant des informations sur les processus et le noyau. Exemple : `/proc/cpuinfo`, `/proc/meminfo`, `/proc/[PID]/`.

### **Process (Processus)**
Instance d'un programme en cours d'exécution. Possède son propre espace mémoire, PID, descripteurs de fichiers.

### **Process group**
Ensemble de processus partageant un PGID (Process Group ID). Permet d'envoyer des signaux à un groupe.

### **Process table**
Structure du noyau maintenant les informations sur tous les processus actifs.

### **Protocol**
Ensemble de règles définissant la communication. Exemples : TCP, UDP, IP, HTTP, SSH.

### **Pseudo-terminal (PTY)**
Émulation logicielle d'un terminal. Utilisé par les émulateurs de terminaux (xterm, gnome-terminal) et SSH.

---

## R

### **Race condition**
Voir glossaire C. En système : situation où plusieurs processus/threads accèdent concurremment à une ressource partagée sans synchronisation appropriée.

### **RAID (Redundant Array of Independent Disks)**
Technique combinant plusieurs disques pour améliorer la performance et/ou la fiabilité.

### **RAM (Random Access Memory)**
Mémoire vive volatile du système.

### **Reboot**
Redémarrage du système.

### **Redirection**
Changement de la source ou destination des flux standard (stdin, stdout, stderr).
```bash
programme > sortie.txt      # Redirige stdout vers fichier  
programme 2> erreurs.txt    # Redirige stderr vers fichier  
programme < entree.txt      # Redirige stdin depuis fichier  
```

### **Relative path (Chemin relatif)**
Chemin par rapport au répertoire courant. Exemple : `../fichier.txt`

### **Repository (Dépôt)**
Serveur contenant des paquets logiciels pour une distribution Linux.

### **RHEL (Red Hat Enterprise Linux)**
Distribution Linux commerciale pour entreprises. Clone open-source : CentOS, Rocky Linux.

### **Root**
1. Utilisateur superadministrateur (UID 0), avec tous les privilèges
2. Répertoire racine du système de fichiers : `/`

### **Root directory**
Répertoire `/`, sommet de l'arborescence des fichiers.

### **Runlevel**
Niveau d'exécution définissant les services actifs (concept SysVinit, remplacé par les **targets** systemd).

---

## S

### **Scheduler (Ordonnanceur)**
Composant du noyau décidant quel processus/thread s'exécute sur quel CPU et quand. Implémente des algorithmes de scheduling (EEVDF depuis Linux 6.6, CFS avant).

### **Section critique**
Voir glossaire C. Portion de code accédant à des ressources partagées.

### **Segmentation fault**
Voir glossaire C. Erreur d'accès mémoire invalide.

### **Semaphore**
Mécanisme de synchronisation contrôlant l'accès à une ressource via un compteur.

### **Service**
Programme s'exécutant en arrière-plan, généralement démarré au boot. Géré par systemd via des **unit files**.

### **Session**
Groupe de process groups. Un terminal peut avoir une session active.

### **setuid / setgid**
Bits de permission spéciaux permettant à un exécutable de s'exécuter avec les privilèges de son propriétaire (setuid) ou groupe (setgid), plutôt que ceux de l'utilisateur lançant le programme.

### **Shared library**
Bibliothèque dynamique (`.so`) chargée en mémoire et partagée entre plusieurs processus.

### **Shared memory**
Zone de mémoire accessible par plusieurs processus. IPC performant mais nécessitant synchronisation.

### **Shell**
Interprète de commandes permettant l'interaction avec le système. Exemples : bash, zsh, fish, sh.

### **Shell script**
Programme écrit dans le langage d'un shell, automatisant des séquences de commandes.

### **SIGHUP, SIGINT, SIGKILL, SIGTERM, SIGUSR1, SIGUSR2...**
Voir **Signal**. Exemples de signaux standards.

### **Signal**
Notification asynchrone envoyée à un processus pour indiquer un événement :
- `SIGINT` (2) : Interruption (Ctrl+C)
- `SIGTERM` (15) : Demande de terminaison propre
- `SIGKILL` (9) : Terminaison forcée (non capturable)
- `SIGSEGV` (11) : Segmentation fault
- `SIGCHLD` (17) : Fils terminé

### **SIMD (Single Instruction, Multiple Data)**
Instructions CPU traitant plusieurs données simultanément (vectorisation). Extensions : SSE, AVX sur x86.

### **Socket**
Point de terminaison pour la communication réseau ou IPC. Types : TCP, UDP, Unix domain sockets.

### **SONAME**
Nom de version d'une bibliothèque partagée utilisé pour la compatibilité binaire.

### **Standard streams (Flux standard)**
Trois canaux de communication par défaut :
- **stdin** (0) : entrée standard
- **stdout** (1) : sortie standard
- **stderr** (2) : sortie erreur

### **stat / fstat / lstat**
Appels système récupérant les métadonnées (inode) d'un fichier.

### **Sticky bit**
Bit de permission sur un répertoire : seul le propriétaire d'un fichier peut le supprimer (même si le répertoire est world-writable). Exemple : `/tmp`.

### **strace**
Outil traçant les appels système effectués par un processus. Indispensable pour le débogage système.
```bash
strace ./mon_programme  
strace -p 1234  # Attacher à un processus existant  
```

### **sudo (Superuser Do)**
Commande permettant d'exécuter une commande avec les privilèges root.
```bash
sudo apt update
```

### **Swap**
Espace disque utilisé comme extension de la RAM. Permet d'éviter les OOM mais très lent.

### **Symbolic link (Symlink / Lien symbolique)**
Fichier spécial contenant le chemin vers un autre fichier. Peut traverser les systèmes de fichiers.
```bash
ln -s /chemin/vers/fichier lien_symbolique
```

### **Syscall**
Voir **Appel système**.

### **/sys**
Système de fichiers virtuel exposant des informations sur les périphériques, drivers et fonctionnalités du noyau.

### **syslog**
Système de logging standardisé Unix. Souvent remplacé/complété par `journald` sous systemd.

### **systemd**
Système d'initialisation et gestionnaire de services moderne, remplaçant SysVinit sur la plupart des distributions récentes.

### **systemctl**
Commande contrôlant systemd et les services.
```bash
systemctl start nginx  
systemctl enable nginx  
systemctl status nginx  
```

---

## T

### **tar (Tape Archive)**
Outil d'archivage regroupant plusieurs fichiers. Souvent combiné avec gzip/bzip2 pour la compression.
```bash
tar -czf archive.tar.gz dossier/  
tar -xzf archive.tar.gz  
```

### **TCP (Transmission Control Protocol)**
Protocole de transport fiable, orienté connexion, avec garantie d'ordre et de livraison.

### **Terminal / TTY (TeleTYpewriter)**
Interface textuelle de communication avec le système. Peut être :
- Terminal physique
- Pseudo-terminal (PTY) pour émulateurs

### **Thread**
Fil d'exécution au sein d'un processus. Partage la mémoire avec les autres threads du processus.

### **Throughput (Débit)**
Quantité de données traitées par unité de temps.

### **Time slice (Quantum de temps)**
Durée pendant laquelle un processus peut s'exécuter avant que le scheduler ne le préempte.

### **Top / htop**
Commandes affichant en temps réel les processus et l'utilisation des ressources.

### **Trap**
Mécanisme permettant à un shell script de capturer des signaux.

---

## U

### **Ubuntu**
Distribution Linux populaire basée sur Debian, maintenue par Canonical.

### **UDP (User Datagram Protocol)**
Protocole de transport non fiable, sans connexion, à faible latence. Utilisé pour streaming, DNS, etc.

### **UID (User ID)**
Identifiant numérique d'un utilisateur. Root a l'UID 0.

### **umask**
Masque définissant les permissions par défaut des nouveaux fichiers.
```bash
umask 022  # Fichiers créés avec 644, répertoires avec 755
```

### **Umount (Démonter)**
Action de détacher un système de fichiers du point de montage.

### **Unix**
Famille de systèmes d'exploitation dont Linux s'inspire. POSIX standardise l'API Unix.

### **Unix domain socket**
Socket permettant l'IPC entre processus sur la même machine. Plus performant que TCP loopback.

### **Uptime**
Durée depuis le dernier démarrage du système.

### **User**
Compte permettant d'identifier et contrôler l'accès aux ressources. Chaque user a un UID unique.

### **User space**
Espace d'adressage non privilégié où s'exécutent les applications utilisateur. Opposé de **kernel space**.

### **Userland**
Ensemble des programmes utilisateur (par opposition au noyau).

### **/usr**
Répertoire contenant les programmes et bibliothèques utilisateur (`/usr/bin`, `/usr/lib`, `/usr/local`).

---

## V

### **/var**
Répertoire contenant les données variables : logs (`/var/log`), caches, bases de données, files d'attente.

### **Virtual memory (Mémoire virtuelle)**
Abstraction permettant à chaque processus d'avoir son propre espace d'adressage, isolé des autres. Gérée par le MMU (Memory Management Unit).

### **VFS (Virtual File System)**
Couche d'abstraction du noyau permettant de supporter différents systèmes de fichiers via une interface uniforme.

### **Virtual machine (Machine virtuelle)**
Émulation logicielle d'un ordinateur complet. Exemples : VirtualBox, KVM, QEMU.

### **VPN (Virtual Private Network)**
Réseau privé sécurisé sur un réseau public (Internet).

---

## W

### **Wait / waitpid**
Appels système permettant à un processus parent d'attendre la terminaison d'un fils et de récupérer son code de sortie.
```c
pid_t pid = fork();  
if (pid == 0) {  
    exit(42);
} else {
    int status;
    waitpid(pid, &status, 0);
    // Récupération du code de sortie
}
```

### **Wall clock time**
Temps réel écoulé (par opposition au CPU time).

### **Watchdog**
Mécanisme surveillant un système/processus et le redémarrant en cas de problème.

### **Working directory (Répertoire de travail)**
Répertoire courant d'un processus. Modifiable avec `chdir()`.

---

## X

### **X11 / X Window System**
Système de fenêtrage pour Unix/Linux. Permet les interfaces graphiques. Progressivement remplacé par **Wayland**.

---

## Z

### **Zombie process (Processus zombie)**
Processus terminé dont le parent n'a pas encore récupéré le code de sortie via `wait()`. État `Z` dans `ps`. Occupe une entrée dans la table des processus mais aucune autre ressource.
```bash
ps aux | grep Z
```

### **Zone mémoire**
Région de mémoire avec des caractéristiques spécifiques (kernel space, user space, stack, heap, text segment, etc.).

---

## 📚 Sections du manuel (man pages)

Les pages de manuel sont organisées en sections :

| Section | Contenu |
|---------|---------|
| **1** | Commandes utilisateur (ls, cat, grep) |
| **2** | Appels système (open, read, fork) |
| **3** | Fonctions bibliothèque (printf, malloc, pthread_create) |
| **4** | Fichiers spéciaux et périphériques (/dev) |
| **5** | Formats de fichiers (passwd, fstab) |
| **7** | Divers (conventions, protocoles, signal) |
| **8** | Commandes d'administration système (mount, iptables) |

```bash
man 2 open    # Appel système open()  
man 3 printf  # Fonction bibliothèque printf()  
```

---

## 🔍 Commandes essentielles à connaître

### Informations système
```bash
uname -a        # Version du noyau et architecture  
lscpu           # Informations CPU  
free -h         # Utilisation mémoire  
df -h           # Espace disque  
lsblk           # Liste des périphériques bloc  
dmesg           # Messages du noyau  
```

### Gestion des processus
```bash
ps aux          # Liste tous les processus  
ps -ef          # Format alternatif  
top / htop      # Monitoring en temps réel  
pgrep nom       # Trouver PID par nom  
kill -SIGNAL PID  
killall nom     # Tuer par nom  
```

### Gestion des fichiers
```bash
ls -la          # Liste détaillée avec fichiers cachés  
stat fichier    # Métadonnées détaillées  
file fichier    # Type de fichier  
lsof            # Fichiers ouverts  
du -sh dossier  # Taille d'un dossier  
```

### Réseau
```bash
ip addr         # Adresses IP  
ip route        # Table de routage  
ss -tulpn       # Sockets en écoute  
netstat -tulpn  # Alternatif à ss (ancien)  
ping host       # Test de connectivité  
traceroute host # Traçage de route  
```

### Système
```bash
systemctl status service  # État d'un service  
journalctl -u service     # Logs d'un service  
lsmod                     # Modules noyau chargés  
strace commande           # Tracer appels système  
```

---

## 💡 Conventions typographiques

- **Gras** : Terme principal défini
- `Code inline` : Commandes, fichiers, code C
- *Italique* : Emphase, termes étrangers

---

## 🚀 Conseils d'utilisation

Ce glossaire est conçu comme une référence rapide pour :

1. **Comprendre les messages d'erreur système**
2. **Lire la documentation Linux (man pages)**
3. **Préparer des entretiens techniques**
4. **Développer des programmes système en C**

**Astuce** : Les termes en gras dans les définitions renvoient souvent à d'autres entrées du glossaire.

**Complément** : Pour la terminologie spécifique au langage C, consultez **D.1 Terminologie C**.

---

## 📖 Ressources pour approfondir

- **Man pages** : `man man` pour apprendre à naviguer
- **Linux Documentation Project** : [tldp.org](https://tldp.org)
- **Arch Wiki** : Documentation détaillée (utile même pour autres distributions)
- **Kernel.org** : Documentation officielle du noyau Linux
- **POSIX standards** : Standards ouverts des systèmes Unix-like

---

*Ce glossaire est maintenu à jour avec les systèmes Linux modernes (kernel 5.x/6.x, systemd, distributions 2024-2025).*

⏭️ [Acronymes DevOps](/annexes/glossaire/03-acronymes-devops.md)
