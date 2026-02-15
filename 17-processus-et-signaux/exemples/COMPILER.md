# Chapitre 17 : Processus et Signaux - Guide de compilation

## Compilation standard

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o <nom> <nom>.c
```

## Exceptions de compilation

| Fichier | Commande | Raison |
|---------|----------|--------|
| `17_pids_threads.c` | `gcc -Wall -Wextra -Werror -std=c17 -o 17_pids_threads 17_pids_threads.c` | `_GNU_SOURCE` + `syscall()` incompatible avec `-pedantic` |
| `74_sigsegv_backtrace.c` | `gcc -Wall -Wextra -Werror -pedantic -std=c17 -rdynamic -o 74_sigsegv_backtrace 74_sigsegv_backtrace.c` | `-rdynamic` pour backtrace lisible |

## Programmes interactifs / boucle infinie

Les programmes suivants contiennent des boucles infinites ou des `sleep()` longs. Ils sont conçus pour être testés interactivement (Ctrl+C ou `kill` depuis un autre terminal) :
51, 52, 53, 54, 55, 56, 57, 58, 62, 63, 64, 65, 67, 68, 69, 70, 71, 78, 79, 80, 81, 82, 83, 85, 86, 87, 88, 119

## Programmes bugués (pédagogiques)

| Fichier | Comportement |
|---------|-------------|
| `22_zombie_masse.c` | Crée 1000 zombies - exemple dangereux intentionnel |
| `95_pipe_sans_close.c` | Bloque indéfiniment - illustre l'absence de close(fd[1]) |

## Programmes en paires (nécessitent 2 terminaux)

| Paire | Description |
|-------|-------------|
| `111_fifo_writer.c` + `112_fifo_reader.c` | Écrivain/lecteur FIFO simple |
| `114_fifo_server.c` + `115_fifo_client.c` | Serveur/client FIFO bidirectionnel |
| `116_fifo_queue_writer.c` + `117_fifo_queue_reader.c` | File d'attente FIFO |
| `120_fifo_monitor.c` + `121_fifo_reporter.c` | Monitor/reporter de stats système |
| `122_fifo_worker.c` + `123_fifo_scheduler.c` | Worker/scheduler task queue |

Pour les paires FIFO, lancer d'abord le lecteur/serveur/worker, puis l'écrivain/client/scheduler dans un autre terminal. Créer le FIFO au préalable si nécessaire : `mkfifo /tmp/<nom_fifo>`.

---

## Section 17.1 : Création de processus (01-creation-processus.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 01 | `01_fork_simple.c` | Premier exemple de fork() - parent et enfant | PID parent et enfant, messages des deux processus |
| 02 | `02_fork_variables.c` | Fork avec modification de variables - espaces mémoire séparés | Variable modifiée différemment dans parent et enfant |
| 03 | `03_fork_ordre.c` | Ordre d'exécution non déterministe entre parent et enfant | Messages parent/enfant dans un ordre variable |
| 04 | `04_fork_erreurs.c` | Gestion des erreurs de fork() | Message d'erreur si fork échoue (rare) |
| 05 | `05_fork_multiples.c` | Création de multiples processus enfants | Messages de 3 enfants avec PIDs |
| 06 | `06_fork_travail.c` | Processus de travail - parent et enfant avec tâches différentes | Parent supervise, enfant calcule |
| 07 | `07_infos_processus.c` | Fonctions utiles liées aux processus (getpid, getppid, etc.) | PID, PPID, PGID, UID, EUID |

## Section 17.2 : Les PIDs et hiérarchie (02-pids-hierarchie.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 08 | `08_pids_basique.c` | Obtenir le PID du processus courant et de son parent | PID et PPID |
| 09 | `09_parent_enfant.c` | Relations parent-enfant après fork() | PIDs parent et enfant, PPID de l'enfant |
| 10 | `10_chaine_parente.c` | Remonter la chaîne de parenté jusqu'à systemd via /proc | Chaîne PID → ... → 1 (systemd) |
| 11 | `11_famille_processus.c` | Parent avec plusieurs enfants - visualisation avec pstree | 3 enfants avec sleep(10), utiliser pstree |
| 12 | `12_multi_generations.c` | Hiérarchie multi-générationnelle (4 générations) | 4 niveaux de processus avec PIDs |
| 13 | `13_groupe_processus.c` | Groupe de processus (PGID) | PID, PPID et PGID de parent et enfant |
| 14 | `14_session.c` | Session de processus (SID, PGID, PID) | PID, PGID et SID |
| 15 | `15_lister_enfants.c` | Lister les processus enfants via /proc | Liste des PIDs enfants trouvés dans /proc |
| 16 | `16_reparenting.c` | Reparenting - adoption d'un orphelin par systemd (PID 1) | Orphelin avec PPID passant à 1 |
| 17 | `17_pids_threads.c` | PIDs et threads - TID via syscall (**sans -pedantic**) | PID et TID du thread principal |
| 18 | `18_daemonize.c` | Daemonisation (fork + setsid) | Daemon en arrière-plan, parent se termine |

## Section 17.3 : Processus orphelins et zombies (03-orphelins-zombies.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 19 | `19_orphelin.c` | Comment un processus devient orphelin | PPID change après mort du parent |
| 20 | `20_daemon_orphelin.c` | Daemon créé volontairement comme orphelin | Processus orphelin en boucle infinie |
| 21 | `21_zombie.c` | Création d'un processus zombie (pédagogique) | Zombie visible avec `ps aux`, interactif |
| 22 | `22_zombie_masse.c` | Création de 1000 zombies (**dangereux**) | 1000 processus zombies |
| 23 | `23_wait_basique.c` | Utilisation basique de wait() | Parent attend et récupère le code retour |
| 24 | `24_wait_status.c` | Macros WIFEXITED/WEXITSTATUS | Code de sortie analysé |
| 25 | `25_wait_multiples.c` | Attendre plusieurs enfants | Tous les enfants récupérés |
| 26 | `26_waitpid_specifique.c` | waitpid() pour un enfant spécifique | Attend un enfant précis par PID |
| 27 | `27_waitpid_wnohang.c` | Mode non-bloquant avec WNOHANG | Polling non-bloquant |
| 28 | `28_sigchld_handler.c` | Gestionnaire SIGCHLD asynchrone | Enfants nettoyés automatiquement |
| 29 | `29_sigchld_ignore.c` | Ignorer SIGCHLD (SIG_IGN) | Pas de zombies |
| 30 | `30_diagnostic_zombies.c` | Diagnostic - recherche de processus zombies | Scan de /proc pour zombies |

## Section 17.4 : Variables d'environnement (04-variables-environnement.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 31 | `31_getenv_basique.c` | Lecture basique avec getenv() | Valeur de PATH |
| 32 | `32_getenv_multiples.c` | Lecture de plusieurs variables | HOME, USER, SHELL, PATH |
| 33 | `33_getenv_defaut.c` | Vérification d'existence et valeur par défaut | Valeurs ou "(non defini)" |
| 34 | `34_environ_tableau.c` | Accès direct via extern environ | Liste des premières variables |
| 35 | `35_environ_parser.c` | Parser les variables (séparation nom/valeur) | Nom et valeur séparés |
| 36 | `36_setenv.c` | Définir des variables avec setenv() | Variable créée/modifiée |
| 37 | `37_setenv_overwrite.c` | Paramètre overwrite de setenv() | Comportement 0 vs 1 |
| 38 | `38_unsetenv.c` | Supprimer une variable | Variable supprimée |
| 39 | `39_putenv.c` | putenv() avec malloc | Variable définie via putenv |
| 40 | `40_heritage_fork.c` | Héritage des variables lors d'un fork() | Variables héritées par l'enfant |
| 41 | `41_heritage_exec.c` | Héritage des variables lors d'un exec() | Variables héritées via exec |
| 42 | `42_config_env.c` | Configuration d'application via variables | Config lue depuis l'environnement |
| 43 | `43_execve_env.c` | Environnement personnalisé avec execve() | Programme lancé avec env custom |
| 44 | `44_recherche_path.c` | Recherche d'un exécutable dans le PATH | Chemin trouvé ou non |
| 45 | `45_repertoire_temp.c` | Recherche du répertoire temporaire | TMPDIR ou /tmp |
| 46 | `46_localisation.c` | Variables de localisation | LANG, LC_ALL, etc. |
| 47 | `47_securite_env.c` | Sécurité - validation et nettoyage | Validation des valeurs |
| 48 | `48_debug_env.c` | Mode debug via variable d'environnement | Messages debug si DEBUG=1 |
| 49 | `49_afficher_env.c` | Afficher l'environnement complet avec formatage | Toutes les variables formatées |

## Section 17.5 : Gestion des signaux (05-gestion-signaux.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 50 | `50_kill_signal.c` | Envoyer SIGINT à soi-même avec kill() | Processus tué par SIGINT |
| 51 | `51_ignorer_sigint.c` | Ignorer SIGINT (Ctrl+C inactif) | Ctrl+C sans effet pendant 5s |
| 52 | `52_handler_signal.c` | Gestionnaire personnalisé avec signal() | Message personnalisé sur Ctrl+C |
| 53 | `53_handler_safe.c` | Handler async-signal-safe avec write() | Écriture safe dans le handler |
| 54 | `54_flag_volatile.c` | Pattern volatile sig_atomic_t | Flag mis à 1 par le signal |
| 55 | `55_sigaction_basique.c` | sigaction() avec SA_RESTART | Handler installé proprement |
| 56 | `56_sigaction_siginfo.c` | SA_SIGINFO (si_pid, si_uid) | Info sur l'émetteur du signal |
| 57 | `57_multi_signaux.c` | Gestion SIGINT, SIGTERM, SIGUSR1 | Différents messages par signal |
| 58 | `58_bloquer_sigint.c` | Bloquer SIGINT avec sigprocmask() | SIGINT bloqué puis débloqué |
| 59 | `59_sigchld_signal.c` | SIGCHLD pour nettoyer les enfants | Nettoyage automatique des enfants |
| 60 | `60_nocldwait.c` | SA_NOCLDWAIT pour éviter les zombies | Pas de zombies sans handler |
| 61 | `61_alarm_timeout.c` | Timeout avec alarm()/SIGALRM + longjmp | Timeout après 5 secondes |
| 62 | `62_serveur_robuste.c` | Serveur robuste avec gestion complète des signaux | Serveur multi-signaux interactif |

## Section 17.6 : Signaux courants (06-signaux-courants.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 63 | `63_sigterm_service.c` | SIGTERM pour arrêt propre d'un service | Service avec log dans /tmp |
| 64 | `64_sigint_confirmer.c` | Confirmer avant de quitter (double Ctrl+C) | 2 Ctrl+C pour quitter |
| 65 | `65_script_interruptible.c` | Script interruptible avec barre de progression | Barre de progression interruptible |
| 66 | `66_sigkill_impossible.c` | SIGKILL et SIGSTOP impossibles à intercepter | Échec de signal() pour ces signaux |
| 67 | `67_sigusr_stats.c` | SIGUSR1/SIGUSR2 pour stats et debug | Stats et toggle debug |
| 68 | `68_sighup_reload.c` | SIGHUP pour rechargement de configuration | Reload config sur SIGHUP |
| 69 | `69_sigchld_serveur.c` | SIGCHLD - gestion des enfants type serveur | Nettoyage automatique |
| 70 | `70_sigalrm_timeout.c` | SIGALRM timeout avec progression | Timeout avec affichage |
| 71 | `71_sigalrm_periodique.c` | Alarme périodique (réamorçage) | Alarme toutes les 2 secondes |
| 72 | `72_sigpipe.c` | SIGPIPE - écriture dans pipe fermé | EPIPE détecté |
| 73 | `73_sigsegv_handler.c` | SIGSEGV - capturer une segfault | Message avant crash |
| 74 | `74_sigsegv_backtrace.c` | SIGSEGV avec backtrace (**-rdynamic**) | Backtrace avant crash |
| 75 | `75_sigfpe_division.c` | SIGFPE - division par zéro avec recovery | Recovery via longjmp |

## Section 17.7 : Masquage de signaux (07-masquage-signaux.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 76 | `76_sigset_creer.c` | Créer et manipuler un ensemble sigset_t | Tests d'appartenance |
| 77 | `77_sigset_complet.c` | Ensemble complet vs ensemble vide | sigfillset vs sigemptyset |
| 78 | `78_bloquer_sigint.c` | Bloquer SIGINT temporairement | 3 phases (normal, bloqué, débloqué) |
| 79 | `79_sig_block.c` | Mode SIG_BLOCK - ajouter au masque | 3 signaux bloqués cumulativement |
| 80 | `80_sig_unblock.c` | Mode SIG_UNBLOCK - retirer du masque | Déblocage sélectif |
| 81 | `81_sig_setmask.c` | Mode SIG_SETMASK - remplacer le masque | Remplacement complet |
| 82 | `82_sauver_masque.c` | Sauvegarder et restaurer le masque | Section critique avec restauration |
| 83 | `83_sigpending.c` | Visualiser signaux en attente | Affichage des signaux pending |
| 84 | `84_section_critique.c` | Pattern section critique complet | 3 itérations protégées |
| 85 | `85_handler_autoblock.c` | Auto-blocage pendant le handler | Signal bloqué dans le handler |
| 86 | `86_sa_mask_handler.c` | Bloquer SIGINT pendant handler SIGUSR1 | SIGINT différé pendant SIGUSR1 |
| 87 | `87_sigsuspend.c` | Attendre un signal avec sigsuspend | Réveil sur SIGUSR1 |
| 88 | `88_sigsuspend_multi.c` | Attendre plusieurs signaux | Réveil sur USR1/USR2/INT |
| 89 | `89_transaction.c` | Protéger une transaction | 3 étapes protégées |
| 90 | `90_synchro_parent_enfant.c` | Synchronisation parent-enfant | Parent attend le signal de l'enfant |
| 91 | `91_reentrance.c` | Réentrance sécurisée avec masquage | Counter incrémenté de façon sûre |

## Section 17.8 : Pipes anonymes (08-pipes-anonymes.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 92 | `92_pipe_creer.c` | Créer un pipe et afficher les descripteurs | fd[0]=3, fd[1]=4 |
| 93 | `93_pipe_seul_processus.c` | Communication via pipe dans un seul processus | Message envoyé et reçu |
| 94 | `94_pipe_parent_enfant.c` | Communication parent-enfant via pipe | Message transmis via pipe |
| 95 | `95_pipe_sans_close.c` | Problème sans close fd[1] (**bloque**) | Bloque indéfiniment |
| 96 | `96_pipe_bidirectionnel.c` | Communication bidirectionnelle (2 pipes) | Échange dans les 2 sens |
| 97 | `97_pipe_lecture_bloquante.c` | Lecture bloquante - read attend les données | Lecture après 3 secondes |
| 98 | `98_pipe_ecriture_bloquante.c` | Écriture bloquante - buffer limité | Remplissage jusqu'à ~100 Ko |
| 99 | `99_pipe_nonbloquant.c` | Mode non-bloquant avec O_NONBLOCK | "Pipe vide" (EAGAIN) |
| 100 | `100_sigpipe_pipe.c` | SIGPIPE - écriture sans lecteur | Tué par SIGPIPE (code 141) |
| 101 | `101_sigpipe_ignorer.c` | Ignorer SIGPIPE et détecter EPIPE | "Erreur EPIPE detectee" |
| 102 | `102_dup2_stdout.c` | Rediriger stdout vers un pipe avec dup2 | Printf redirigé vers pipe |
| 103 | `103_pipe_ls_wc.c` | Simuler ls -l \| wc -l | Nombre de lignes |
| 104 | `104_pipe_structure.c` | Transmettre une structure via pipe | Produit envoyé/reçu |
| 105 | `105_pipe_tableau.c` | Transmettre un tableau de structures | 5 points transmis |
| 106 | `106_pipe_robuste.c` | Gestion d'erreurs robuste (EINTR) | Message avec gestion robuste |
| 107 | `107_calculateur_pipe.c` | Calculateur parallèle (carré) | 7^2 = 49 |
| 108 | `108_logger_pipe.c` | Logger centralisé (3 workers) | Messages horodatés des workers |

## Section 17.9 : Named pipes - FIFO (09-named-pipes.md)

| # | Fichier | Description | Sortie attendue |
|---|---------|-------------|-----------------|
| 109 | `109_fifo_creer.c` | Créer un FIFO avec mkfifo | "FIFO cree avec succes" |
| 110 | `110_fifo_verifier.c` | Vérifier l'existence avant création | Crée puis détecte existant |
| 111 | `111_fifo_writer.c` | Écrivain FIFO (paire avec 112) | Envoie 3 messages |
| 112 | `112_fifo_reader.c` | Lecteur FIFO (paire avec 111) | Reçoit les messages |
| 113 | `113_fifo_nonbloquant.c` | Ouverture non-bloquante O_NONBLOCK | "Aucun lecteur connecte" |
| 114 | `114_fifo_server.c` | Serveur FIFO bidirectionnel (paire avec 115) | Reçoit et répond en majuscules |
| 115 | `115_fifo_client.c` | Client FIFO bidirectionnel (paire avec 114) | Envoie et reçoit réponses |
| 116 | `116_fifo_queue_writer.c` | Écrivain file d'attente (paire avec 117) | Envoie un message (argument) |
| 117 | `117_fifo_queue_reader.c` | Lecteur file d'attente (paire avec 116) | Reçoit les messages |
| 118 | `118_fifo_unlink.c` | Supprimer un FIFO avec unlink | Crée puis supprime |
| 119 | `119_fifo_cleanup.c` | Nettoyage automatique avec handler signal | Ctrl+C nettoie le FIFO |
| 120 | `120_fifo_monitor.c` | Monitor système (paire avec 121) | Affiche les stats reçues |
| 121 | `121_fifo_reporter.c` | Reporter système (paire avec 120) | Envoie 5 rapports de stats |
| 122 | `122_fifo_worker.c` | Worker task queue (paire avec 123) | Traite les tâches reçues |
| 123 | `123_fifo_scheduler.c` | Scheduler task queue (paire avec 122) | Envoie 4 tâches |
| 124 | `124_fifo_event_logger.c` | Event logger - journalisation | Log horodaté des événements |
| 125 | `125_fifo_test.c` | Programme de test et diagnostic FIFO | Test existence et ouverture |

---

**Total : 125 programmes** (9 sections)
