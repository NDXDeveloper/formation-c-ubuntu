# Chapitre 34 - Etudes de cas DevOps : Compilation des exemples

Compilation standard :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 fichier.c -o fichier
```

## Section 34.1 : Creation d'un outil CLI systeme (01-outil-cli-systeme.md)

| Fichier | Description | Compilation |
|---------|-------------|-------------|
| `01_hello_tool.c` | Exemple minimal "Hello Tool" CLI | standard |
| `02_cli_skeleton.c` | Squelette complet CLI avec getopt_long | standard + `-D_POSIX_C_SOURCE=200809L` |

**Sortie attendue (01):** `Bonjour, <nom>!`
**Sortie attendue (02):** Aide, version, ou traitement de fichiers selon options

## Section 34.1.1 : Parsing d'arguments (01.1-parsing-arguments.md)

| Fichier | Description | Compilation |
|---------|-------------|-------------|
| `03_demo_argv.c` | Demonstration de argc et argv | standard |
| `04_manual_parse.c` | Parsing manuel avec strcmp | standard |
| `05_getopt_simple.c` | Parsing avec getopt() POSIX | standard + `-D_POSIX_C_SOURCE=200809L` |
| `06_getopt_long.c` | Parsing avec getopt_long() | standard + `-D_POSIX_C_SOURCE=200809L` |

**Sortie attendue (03):** Liste des arguments `argv[0]`, `argv[1]`, ...
**Sortie attendue (04):** "Mode verbose activé" avec `-v`
**Sortie attendue (05):** Affiche options parsées avec `-v -o file -n 10`
**Sortie attendue (06):** Affiche configuration avec `--verbose --output file`

## Section 34.1.2 : Interaction avec le systeme (01.2-interaction-systeme.md)

| Fichier | Description | Compilation |
|---------|-------------|-------------|
| `07_file_operations.c` | Lecture/ecriture fichier + access() | standard + `-D_POSIX_C_SOURCE=200809L` |
| `08_copy_syscall.c` | Copie de fichier avec open/read/write | standard + `-D_POSIX_C_SOURCE=200809L` |
| `09_file_info.c` | Informations fichier avec lstat() | standard + `-D_POSIX_C_SOURCE=200809L` |
| `10_list_directory.c` | Lister repertoire avec opendir/readdir | standard + `-D_DEFAULT_SOURCE` |
| `11_popen_capture.c` | Capture de sortie avec popen() | standard + `-D_POSIX_C_SOURCE=200809L` |
| `12_fork_exec.c` | Execution avec fork() + execvp() | standard + `-D_POSIX_C_SOURCE=200809L` |
| `13_environment.c` | Variables d'environnement getenv/setenv | standard + `-D_POSIX_C_SOURCE=200809L` |
| `14_sysinfo_complete.c` | Outil complet d'information systeme | standard |
| `15_proc_info.c` | Lecture de /proc (CPU, memoire, uptime) | standard |

**Note (10):** `DT_REG` et autres constantes `d_type` necessitent `-D_DEFAULT_SOURCE`

**Sortie attendue (07):** Ecriture, verification access(), relecture d'un fichier temporaire
**Sortie attendue (08):** Copie d'un fichier via appels systeme bas niveau
**Sortie attendue (09):** Permissions, type, taille, dates de /etc/passwd et /tmp
**Sortie attendue (10):** Liste des fichiers/dossiers de /etc/apt avec types [f]/[d]
**Sortie attendue (11):** Nombre de processus et uptime via popen
**Sortie attendue (12):** Sortie de `ls -la /tmp` via fork+exec
**Sortie attendue (13):** HOME, USER, PATH + test setenv/unsetenv
**Sortie attendue (14):** Systeme, utilisateur, memoire, disque en un rapport complet
**Sortie attendue (15):** Modele CPU, nombre CPUs, memoire, uptime depuis /proc

## Section 34.1.3 : Output formate (01.3-output-formate.md)

| Fichier | Description | Compilation |
|---------|-------------|-------------|
| `16_printf_formats.c` | Specificateurs de format printf | standard |
| `17_ansi_colors.c` | Couleurs et styles ANSI | standard |
| `18_log_levels.c` | Systeme de log avec niveaux et horodatage | standard |
| `19_table_format.c` | Tableau formate avec bordures ASCII | standard |
| `20_progress_bar.c` | Barre de progression animee | standard + `-D_DEFAULT_SOURCE` |
| `21_format_sizes.c` | Formatage tailles fichiers (Ko/Mo/Go/To) | standard |
| `22_format_duree.c` | Formatage durees lisibles | standard |
| `23_json_csv_output.c` | Generation sorties JSON et CSV | standard |
| `24_bar_chart.c` | Graphique a barres horizontales ASCII | standard |

**Note (20):** `usleep()` necessite `-D_DEFAULT_SOURCE`

**Sortie attendue (16):** Demonstration des formats %d, %x, %f, largeur, precision
**Sortie attendue (17):** Texte en couleurs ANSI (rouge, vert, gras, etc.)
**Sortie attendue (18):** Messages DEBUG/INFO/OK/WARN/ERROR avec horodatage
**Sortie attendue (19):** Tableau aligne avec noms, ages, salaires
**Sortie attendue (20):** Barre de progression animee 0-100%
**Sortie attendue (21):** 512o, 1.00 Ko, 1.00 Mo, 1.00 Go, 1.00 To
**Sortie attendue (22):** 45s, 2m 5s, 1h 1m 5s, 1j 1h 2m 5s, 3j 0h 1m 5s
**Sortie attendue (23):** Objet JSON, tableau JSON, donnees CSV
**Sortie attendue (24):** Barres horizontales CPU/RAM/Disque/Reseau

## Section 34.2.1 : Lecture efficace de gros fichiers (02.1-lecture-gros-fichiers.md)

| Fichier | Description | Compilation |
|---------|-------------|-------------|
| `25_benchmark_read.c` | Benchmark comparatif fread/fgets/mmap | standard + `-D_DEFAULT_SOURCE` |
| `26_mmap_reader.c` | Lecture de fichier avec mmap | standard + `-D_DEFAULT_SOURCE` |

**Note:** `-D_DEFAULT_SOURCE` necessaire pour `madvise()` et `MADV_SEQUENTIAL`

**Sortie attendue (25):** Tableau comparatif des debits (MB/s) - Usage: `./25_benchmark_read <fichier>`
**Sortie attendue (26):** 5 premieres lignes + total lignes - Usage: `./26_mmap_reader <fichier>`

## Section 34.2.2 : Expressions regulieres (02.2-expressions-regulieres.md)

| Fichier | Description | Compilation |
|---------|-------------|-------------|
| `27_regex_basic.c` | Base POSIX regex - extraction de date | standard |
| `28_apache_log_parser.c` | Parser de logs Apache/Nginx avec regex | standard |

**Sortie attendue (27):** Match complet: 2025-01-15, Annee: 2025, Mois: 01, Jour: 15
**Sortie attendue (28):** IP, date, methode, URL, code, taille pour 3 lignes de log

## Section 34.2.3 : Agregation et statistiques (02.3-agregation-statistiques.md)

| Fichier | Description | Compilation |
|---------|-------------|-------------|
| `29_hashmap_counter.c` | Table de hachage pour compter des occurrences | standard + `-D_POSIX_C_SOURCE=200809L` |

**Note:** `strdup()` necessite `-D_POSIX_C_SOURCE=200809L`

**Sortie attendue (29):** IPs et compteurs: 192.168.1.100: 3, 10.0.0.5: 2, 172.16.0.1: 1

## Section 34.3.1 : Collecte de metriques (03.1-collecte-metriques.md)

| Fichier | Description | Compilation |
|---------|-------------|-------------|
| `30_metrics_collector.c` | Collecte CPU et memoire depuis /proc | standard |

**Note:** Mesure CPU prend ~1 seconde (sleep entre deux lectures de /proc/stat)

**Sortie attendue (30):** Utilisation CPU %, memoire totale/utilisee/disponible, swap

## Section 34.3.2 : Export Prometheus (03.2-export-prometheus.md)

| Fichier | Description | Compilation |
|---------|-------------|-------------|
| `31_monitoring_agent/` | Projet multi-fichiers (serveur HTTP + metriques Prometheus) | voir ci-dessous |

**Fichiers:** `metrics.h`, `metrics.c`, `http_server.c`

**Compilation:**
```bash
cd 31_monitoring_agent/
gcc -Wall -Wextra -Werror -pedantic -std=c17 -D_POSIX_C_SOURCE=200809L -c metrics.c
gcc -Wall -Wextra -Werror -pedantic -std=c17 -D_POSIX_C_SOURCE=200809L -c http_server.c
gcc metrics.o http_server.o -o monitoring_agent
```

**Note:** Serveur interactif - ecoute sur port 8080, Ctrl+C pour arreter.
Metriques disponibles sur `http://localhost:8080/metrics`

## Section 34.4.1 : Architecture event-driven (04.1-architecture-event-driven.md)

| Fichier | Description | Compilation |
|---------|-------------|-------------|
| `32_epoll_server.c` | Serveur HTTP event-driven avec epoll | sans `-pedantic` + `-D_GNU_SOURCE` |

**Note:** epoll est une extension Linux (pas POSIX), necessite `-D_GNU_SOURCE`, sans `-pedantic`

**Sortie attendue (32):** Serveur interactif sur port 8080, Ctrl+C pour arreter

## Section 34.4.2 : HTTP Parsing (04.2-http-parsing.md)

| Fichier | Description | Compilation |
|---------|-------------|-------------|
| `33_http_parser.c` | Parsing de requetes HTTP (request line + headers) | standard |

**Sortie attendue (33):** Methode, URI, version et headers parses pour GET et POST

---

## Sections theoriques (pas d'exemples)

- `02-parser-logs.md` : Introduction theorique au parsing de logs
- `03-agent-monitoring.md` : Introduction theorique au monitoring
- `03.3-daemonisation.md` : Daemonisation (code non testable en standalone)
- `04-serveur-web.md` : Introduction theorique au serveur web
- `04.3-static-file-serving.md` : Serveur de fichiers statiques (serveur interactif complet)

## Resume

- **33 programmes** (30 standalone + 3 fichiers multi-projet)
- **1 projet multi-fichiers** (31_monitoring_agent/)
- **0 correction** dans les fichiers .md
