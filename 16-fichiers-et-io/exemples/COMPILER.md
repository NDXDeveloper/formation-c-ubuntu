# Chapitre 16 - Fichiers et I/O : Guide de compilation

## Compilation standard

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o <programme> <source>.c
```

## Exceptions

- **07** : sans `-pedantic` (utilise `_GNU_SOURCE` pour `syscall()`)
- **24** : sans `-pedantic` (utilise `sys/epoll.h`, Linux-spécifique)
- **25, 26** : nécessitent `-lrt` (POSIX AIO)
- **27, 28** : sans `-pedantic` (utilise `_GNU_SOURCE` pour io_uring), nécessitent `-luring`

---

## Liste des exemples

### Section 16.1 : Descripteurs de fichiers (01-descripteurs-fichiers.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 01 | `01_descripteurs_standard.c` | Écriture sur stdout et stderr avec `write()` | `Bonjour depuis stdout` (stdout) + `Ceci est une erreur` (stderr) |
| 02 | `02_creer_ecrire.c` | Création et écriture dans un fichier avec `open`/`write`/`close` | `Fichier ouvert avec succes, descripteur = 3` |
| 03 | `03_lire_fichier.c` | Lecture d'un fichier par blocs avec `read()` | Affiche 3 lignes de texte lues depuis un fichier temporaire |
| 04 | `04_ecrire_fichier.c` | Écriture dans un fichier avec comptage d'octets | `15 octets ecrits` |
| 05 | `05_limite_descripteurs.c` | Affichage des limites de descripteurs de fichiers | Limite souple et dure (ex: 1048576) |

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 01_descripteurs_standard 01_descripteurs_standard.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 02_creer_ecrire 02_creer_ecrire.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 03_lire_fichier 03_lire_fichier.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 04_ecrire_fichier 04_ecrire_fichier.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 05_limite_descripteurs 05_limite_descripteurs.c  
```

### Section 16.2 : Appels système (02-appels-systeme.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 06 | `06_strace_simple.c` | Programme simple pour observer les syscalls avec `strace` | `Bonjour` (tester avec `strace -e write ./06_strace_simple`) |
| 07 | `07_syscall_direct.c` | Appels système directs avec `syscall()` | Thread ID + `Direct syscall` |
| 08 | `08_errno_gestion.c` | Gestion des erreurs avec `errno`, `perror` et `strerror` | Messages d'erreur ENOENT et EBADF |

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 06_strace_simple 06_strace_simple.c  
gcc -Wall -Wextra -Werror -std=c17 -o 07_syscall_direct 07_syscall_direct.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 08_errno_gestion 08_errno_gestion.c  
```

### Section 16.3 : Différence appels système vs bibliothèque (03-systeme-vs-bibliotheque.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 09 | `09_benchmark_io.c` | Benchmark `write()` vs `fprintf()` pour 10000 lignes | Temps comparés (stdio ~10x plus rapide) |
| 10 | `10_copie_fichier.c` | Copie de fichier avec les deux approches | `Copie avec appels systeme... OK` + idem stdio |

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 09_benchmark_io 09_benchmark_io.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 10_copie_fichier 10_copie_fichier.c  
```

### Section 16.4 : Permissions et modes (04-permissions-modes.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 11 | `11_permissions_creation.c` | Création de fichiers avec différentes permissions | Affiche les 3 modes (0600, 0644, 0755) |
| 12 | `12_umask.c` | Manipulation du umask et son effet sur les permissions | Umask actuel, nouveau, permissions effectives 0600 |
| 13 | `13_afficher_permissions.c` | Gestionnaire complet affichant permissions, type et accès | Usage: `./13_afficher_permissions /bin/bash` → `-rwxr-xr-x (0755)` |

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 11_permissions_creation 11_permissions_creation.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 12_umask 12_umask.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 13_afficher_permissions 13_afficher_permissions.c  
```

### Section 16.5 : I/O bufferisé vs non bufferisé (05-io-bufferise.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 14 | `14_benchmark_buffering.c` | Benchmark 4 modes d'écriture (10000 itérations) | Bufferisé ~10x plus rapide, fflush annule le gain |
| 15 | `15_logger_flexible.c` | Logger flexible avec 3 modes de buffering | Affiche les 3 fichiers de log créés |

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 14_benchmark_buffering 14_benchmark_buffering.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 15_logger_flexible 15_logger_flexible.c  
```

### Section 16.6 : dup, dup2 et redirection (06-dup-redirection.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 16 | `16_dup_basic.c` | Duplication de descripteurs avec `dup()` et partage d'offset | `fd original : 3`, `fd duplique : 4`, contenu `ABCDEFGHI` |
| 17 | `17_dup2_redirection.c` | Sauvegarde et restauration de stdout avec `dup`/`dup2` | Messages sur terminal + contenu du fichier redirigé |
| 18 | `18_redirection_stderr.c` | Redirection de stdout et stderr vers le même fichier | Contenu du fichier combined.log (3 messages) |
| 19 | `19_pipe_ls_wc.c` | Implémentation de `ls /tmp \| wc -l` avec pipe et `dup2` | Nombre de fichiers dans /tmp |
| 20 | `20_fork_exec_redirect.c` | Redirection de la sortie d'un processus enfant (`fork`+`exec`) | 5 premières lignes de `ls -l /usr/bin` |

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 16_dup_basic 16_dup_basic.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 17_dup2_redirection 17_dup2_redirection.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 18_redirection_stderr 18_redirection_stderr.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 19_pipe_ls_wc 19_pipe_ls_wc.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 20_fork_exec_redirect 20_fork_exec_redirect.c  
```

### Section 16.7 : I/O multiplexing (07-io-multiplexing.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 21 | `21_select_timeout.c` | Utilisation de `select()` avec timeout sur stdin | Timeout 2s ou affiche le texte tapé (tester : `echo "test" \| ./21_select_timeout`) |
| 22 | `22_poll_timeout.c` | Utilisation de `poll()` avec timeout sur stdin | Timeout 2s ou affiche le texte tapé (tester : `echo "test" \| ./22_poll_timeout`) |
| 23 | `23_serveur_poll.c` | Serveur TCP echo avec `poll()` gérant plusieurs clients | Tester : `echo "hello" \| nc -q0 localhost 8080` |
| 24 | `24_serveur_epoll.c` | Serveur TCP echo avec `epoll()` (Linux spécifique) | Tester : `echo "hello" \| nc -q0 localhost 8081` |

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 21_select_timeout 21_select_timeout.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 22_poll_timeout 22_poll_timeout.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 23_serveur_poll 23_serveur_poll.c  
gcc -Wall -Wextra -Werror -std=c17 -o 24_serveur_epoll 24_serveur_epoll.c  
```

### Section 16.8 : I/O asynchrone (08-io-asynchrone.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 25 | `25_aio_polling.c` | Lecture asynchrone POSIX AIO avec vérification par polling | Affiche octets lus et contenu (nécessite `-lrt`) |
| 26 | `26_aio_signal.c` | Lecture asynchrone POSIX AIO avec notification par signal | Signal reçu + contenu lu (nécessite `-lrt`) |
| 27 | `27_io_uring_basic.c` | Lecture de fichier avec io_uring (API moderne Linux) | Affiche octets lus + contenu (nécessite `-luring`) |
| 28 | `28_io_uring_serveur.c` | Serveur TCP echo avec io_uring (haute performance) | Tester : `echo "hello" \| nc -q0 localhost 8082` (nécessite `-luring`) |

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 25_aio_polling 25_aio_polling.c -lrt  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 26_aio_signal 26_aio_signal.c -lrt  
gcc -Wall -Wextra -Werror -std=c17 -o 27_io_uring_basic 27_io_uring_basic.c -luring  
gcc -Wall -Wextra -Werror -std=c17 -o 28_io_uring_serveur 28_io_uring_serveur.c -luring  
```

## Dépendances

- **liburing-dev** : nécessaire pour les exemples 27 et 28 (`sudo apt install liburing-dev`)
- **nc (netcat)** : utile pour tester les serveurs TCP (`sudo apt install netcat-openbsd`)
- **strace** : utile pour l'exemple 06 (`sudo apt install strace`)

## Compilation de tous les exemples

```bash
cd 16-fichiers-et-io/exemples

# Section 16.1
for f in 01 02 03 04 05; do
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o ${f}_* ${f}_*.c 2>/dev/null
done

# Section 16.2
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 06_strace_simple 06_strace_simple.c  
gcc -Wall -Wextra -Werror -std=c17 -o 07_syscall_direct 07_syscall_direct.c  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 08_errno_gestion 08_errno_gestion.c  

# Sections 16.3-16.5
for f in 09 10 11 12 13 14 15; do
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o ${f}_* ${f}_*.c 2>/dev/null
done

# Section 16.6
for f in 16 17 18 19 20; do
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o ${f}_* ${f}_*.c 2>/dev/null
done

# Section 16.7
for f in 21 22 23; do
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o ${f}_* ${f}_*.c 2>/dev/null
done  
gcc -Wall -Wextra -Werror -std=c17 -o 24_serveur_epoll 24_serveur_epoll.c  

# Section 16.8
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 25_aio_polling 25_aio_polling.c -lrt  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 26_aio_signal 26_aio_signal.c -lrt  
gcc -Wall -Wextra -Werror -std=c17 -o 27_io_uring_basic 27_io_uring_basic.c -luring  
gcc -Wall -Wextra -Werror -std=c17 -o 28_io_uring_serveur 28_io_uring_serveur.c -luring  
```

## Nettoyage

```bash
cd 16-fichiers-et-io/exemples  
rm -f 01_descripteurs_standard 02_creer_ecrire 03_lire_fichier 04_ecrire_fichier  
rm -f 05_limite_descripteurs 06_strace_simple 07_syscall_direct 08_errno_gestion  
rm -f 09_benchmark_io 10_copie_fichier 11_permissions_creation 12_umask  
rm -f 13_afficher_permissions 14_benchmark_buffering 15_logger_flexible  
rm -f 16_dup_basic 17_dup2_redirection 18_redirection_stderr  
rm -f 19_pipe_ls_wc 20_fork_exec_redirect  
rm -f 21_select_timeout 22_poll_timeout 23_serveur_poll 24_serveur_epoll  
rm -f 25_aio_polling 26_aio_signal 27_io_uring_basic 28_io_uring_serveur  
```
