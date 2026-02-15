# Chapitre 19 - Memoire partagee et IPC : Guide de compilation

## Flags de compilation standard

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17
```

Notes :
- Les programmes POSIX shared memory/semaphores necessitent `-lrt` et/ou `-lpthread`
- Les programmes utilisant `usleep()` necessitent `_DEFAULT_SOURCE`
- Les programmes utilisant `clock_gettime()` necessitent `_POSIX_C_SOURCE 200809L`

---

## Section 19.1 : Shared Memory (Memoire Partagee)

### 01_shared_data.h + 01_producer.c + 01_consumer.c
- **Section** : 19.1 - Shared Memory POSIX
- **Description** : Paire producteur-consommateur avec POSIX shared memory (shm_open/mmap)
- **Fichier source** : 01-shared-memory.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 01_producer.c -o 01_producer -lrt
  gcc -Wall -Wextra -Werror -pedantic -std=c17 01_consumer.c -o 01_consumer -lrt
  ```
- **Execution** : Lancer `./01_producer` dans un terminal, puis `./01_consumer` dans un autre
- **Sortie attendue producer** :
  ```
  Producteur : Ecriture des donnees...
  Producteur : counter = 42
  Producteur : message = Hello from producer!
  Appuyez sur Entree pour terminer...
  ```
- **Sortie attendue consumer** :
  ```
  Consommateur : Lecture des donnees...
  Consommateur : counter = 42
  Consommateur : message = Hello from producer!
  Consommateur : nouveau counter = 52
  ```

---

## Section 19.2 : Semaphores System V

### 02_semaphore_demo.c
- **Section** : 19.2 - Semaphores System V
- **Description** : Mutex simple avec semaphore System V, fork parent/fils avec exclusion mutuelle
- **Fichier source** : 02-semaphores-system-v.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 02_semaphore_demo.c -o 02_semaphore_demo
  ```
- **Sortie attendue** (le parent attend que le fils finisse sa section critique) :
  ```
  Semaphore cree (ID: XXXXX)
  [FILS] Tentative d'acces a la section critique...
  [FILS] Entree en section critique
  [FILS] Travail... 1/5
  [FILS] Travail... 2/5
  [FILS] Travail... 3/5
  [FILS] Travail... 4/5
  [FILS] Travail... 5/5
  [FILS] Sortie de la section critique
  [PARENT] Tentative d'acces a la section critique...
  [PARENT] Entree en section critique
  [PARENT] Travail... 1/3
  [PARENT] Travail... 2/3
  [PARENT] Travail... 3/3
  [PARENT] Sortie de la section critique
  Suppression du semaphore
  ```
- **Duree** : ~9 secondes (sleep dans les boucles)

### 03_producer_consumer_sysv.c
- **Section** : 19.2 - Semaphores System V
- **Description** : Producteur-consommateur complet avec 3 semaphores (empty/full/mutex) et shm System V
- **Fichier source** : 02-semaphores-system-v.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 03_producer_consumer_sysv.c -o 03_producer_consumer_sysv
  ```
- **Sortie attendue** :
  ```
  [PROD] Produit : 0 a l'index 0
  [PROD] Produit : 10 a l'index 1
  ...
  [CONS] Consomme : 0 de l'index 0
  [CONS] Consomme : 10 de l'index 1
  ...
  Termine.
  ```
- **Note** : `_DEFAULT_SOURCE` pour `usleep()`, sem_flg=0 (pas SEM_UNDO pour eviter deadlock)

---

## Section 19.3 : Message Queues System V

### 04_sender.c + 04_receiver.c
- **Section** : 19.3 - Message Queues System V
- **Description** : Communication simple par message queue, envoi et reception de 5 messages avec types alternes
- **Fichier source** : 03-message-queues.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 04_sender.c -o 04_sender
  gcc -Wall -Wextra -Werror -pedantic -std=c17 04_receiver.c -o 04_receiver
  ```
- **Execution** : `./04_sender` puis `./04_receiver`
- **Sortie attendue sender** :
  ```
  Queue de messages creee (ID: XXXXX)
  Envoi : type=1, texte="Message numero 1"
  Envoi : type=2, texte="Message numero 2"
  Envoi : type=1, texte="Message numero 3"
  Envoi : type=2, texte="Message numero 4"
  Envoi : type=1, texte="Message numero 5"
  Tous les messages ont ete envoyes.
  ```
- **Sortie attendue receiver** :
  ```
  Connexion a la queue (ID: XXXXX)
  Recu : type=1, texte="Message numero 1"
  Recu : type=2, texte="Message numero 2"
  Recu : type=1, texte="Message numero 3"
  Recu : type=2, texte="Message numero 4"
  Recu : type=1, texte="Message numero 5"
  Tous les messages ont ete recus.
  ```

### 05_client_priority.c + 05_server_priority.c
- **Section** : 19.3 - Message Queues System V
- **Description** : Serveur multi-clients avec systeme de priorites (URGENT/NORMAL/LOW)
- **Fichier source** : 03-message-queues.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 05_client_priority.c -o 05_client_priority
  gcc -Wall -Wextra -Werror -pedantic -std=c17 05_server_priority.c -o 05_server_priority
  ```
- **Execution** : `./05_client_priority` puis `./05_server_priority`
- **Sortie attendue** (les urgents traites en premier, independamment de l'ordre d'envoi) :
  ```
  Serveur demarre (ID queue: XXXXX)
  En attente de messages...

  [URGENT] ALERTE SECURITE !
  [URGENT] Crash du serveur DB !
  [NORMAL] Traiter la commande #123
  [LOW]    Tache de nettoyage
  [LOW]    Archiver les logs

  Queue supprimee.
  ```
- **Note** : `_DEFAULT_SOURCE` pour `usleep()`, serveur modifie pour traiter 5 messages puis quitter

### 06_app.c + 06_logger.c
- **Section** : 19.3 - Message Queues System V
- **Description** : Systeme de logging distribue avec filtrage par niveau (ERROR/WARN/INFO/DEBUG)
- **Fichier source** : 03-message-queues.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 06_app.c -o 06_app
  gcc -Wall -Wextra -Werror -pedantic -std=c17 06_logger.c -o 06_logger
  ```
- **Execution** : `./06_app` puis `./06_logger 3` (niveau min INFO=3)
- **Sortie attendue logger** (les DEBUG sont filtres) :
  ```
  Logger demarre (niveau min: INFO )
  Listening...

  [date] [ERROR] [PID XXXXX] Echec de connexion a la base de donnees
  [date] [WARN ] [PID XXXXX] Configuration manquante, utilisation des defauts
  [date] [INFO ] [PID XXXXX] Application demarree
  [date] [INFO ] [PID XXXXX] Tentative de reconnexion...
  [date] [INFO ] [PID XXXXX] Connexion etablie

  Queue supprimee. 5 messages traites.
  ```
- **Note** : `_POSIX_C_SOURCE 200809L` pour `ctime_r()`, logger modifie en mode non-bloquant

---

## Section 19.4 : POSIX IPC vs System V IPC

### 07_sysv_mutex.c
- **Section** : 19.4 - POSIX IPC vs System V IPC
- **Description** : Mutex simple avec semaphore System V (comparaison)
- **Fichier source** : 04-posix-vs-system-v.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 07_sysv_mutex.c -o 07_sysv_mutex
  ```
- **Sortie attendue** :
  ```
  Section critique (System V)
  ```

### 08_posix_mutex.c
- **Section** : 19.4 - POSIX IPC vs System V IPC
- **Description** : Mutex simple avec semaphore POSIX (comparaison)
- **Fichier source** : 04-posix-vs-system-v.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 08_posix_mutex.c -o 08_posix_mutex -lpthread
  ```
- **Sortie attendue** :
  ```
  Section critique (POSIX)
  ```

### 09_sysv_shared_counter.c
- **Section** : 19.4 - POSIX IPC vs System V IPC
- **Description** : Compteur partage avec System V shared memory
- **Fichier source** : 04-posix-vs-system-v.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 09_sysv_shared_counter.c -o 09_sysv_shared_counter
  ```
- **Sortie attendue** :
  ```
  Compteur (System V) : 42
  ```

### 10_posix_shared_counter.c
- **Section** : 19.4 - POSIX IPC vs System V IPC
- **Description** : Compteur partage avec POSIX shared memory
- **Fichier source** : 04-posix-vs-system-v.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 10_posix_shared_counter.c -o 10_posix_shared_counter -lrt
  ```
- **Sortie attendue** :
  ```
  Compteur (POSIX) : 42
  ```

### 11_producer_consumer_sysv.c
- **Section** : 19.4 - POSIX IPC vs System V IPC
- **Description** : Producteur-consommateur complet version System V (code complete depuis extrait du .md)
- **Fichier source** : 04-posix-vs-system-v.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 11_producer_consumer_sysv.c -o 11_producer_consumer_sysv
  ```
- **Sortie attendue** :
  ```
  [SysV PROD] 10 -> index 0
  [SysV PROD] 20 -> index 1
  ...
  [SysV CONS] 10 <- index 0
  [SysV CONS] 20 <- index 1
  ...
  Termine (System V).
  ```
- **Note** : `_DEFAULT_SOURCE` pour `usleep()`, code complete a partir de l'extrait incomplet du .md

### 12_producer_consumer_posix.c
- **Section** : 19.4 - POSIX IPC vs System V IPC
- **Description** : Producteur-consommateur complet version POSIX (code complete depuis extrait du .md)
- **Fichier source** : 04-posix-vs-system-v.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 12_producer_consumer_posix.c -o 12_producer_consumer_posix -lrt -lpthread
  ```
- **Sortie attendue** :
  ```
  [POSIX PROD] 10 -> index 0
  [POSIX PROD] 20 -> index 1
  ...
  [POSIX CONS] 10 <- index 0
  [POSIX CONS] 20 <- index 1
  ...
  Termine (POSIX).
  ```
- **Note** : `_DEFAULT_SOURCE` pour `usleep()`, code complete a partir de l'extrait incomplet du .md

---

## Section 19.5 : Memory-mapped Files (mmap)

### 13_read_file_mmap.c
- **Section** : 19.5 - Memory-mapped Files (mmap)
- **Description** : Lire un fichier avec mmap et l'afficher
- **Fichier source** : 05-mmap.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 13_read_file_mmap.c -o 13_read_file_mmap
  ```
- **Execution** : `echo "Hello from mmap!" > test.txt && ./13_read_file_mmap test.txt`
- **Sortie attendue** :
  ```
  Taille du fichier : 17 octets

  Contenu du fichier :
  ---
  Hello from mmap!
  ---
  Premier caractere : 'H'
  Dernier caractere : 0x0a
  Mapping libere avec succes
  ```

### 14_modify_file_mmap.c
- **Section** : 19.5 - Memory-mapped Files (mmap)
- **Description** : Modifier un fichier via mmap (mise en majuscules avec MAP_SHARED)
- **Fichier source** : 05-mmap.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 14_modify_file_mmap.c -o 14_modify_file_mmap
  ```
- **Execution** : `echo "hello world" > test.txt && ./14_modify_file_mmap test.txt`
- **Sortie attendue** :
  ```
  Contenu original : hello world

  Contenu modifie : HELLO WORLD

  Fichier modifie avec succes
  ```
- Le fichier contient maintenant `HELLO WORLD`

### 15_shared_example.c
- **Section** : 19.5 - Memory-mapped Files (mmap)
- **Description** : MAP_SHARED - les modifications sont persistees dans le fichier
- **Fichier source** : 05-mmap.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 15_shared_example.c -o 15_shared_example
  ```
- **Sortie attendue** :
  ```
  Avant : AAAA
  Apres : BBAA
  Fichier : BBAA
  ```

### 16_private_example.c
- **Section** : 19.5 - Memory-mapped Files (mmap)
- **Description** : MAP_PRIVATE - copy-on-write, le fichier reste inchange
- **Fichier source** : 05-mmap.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 16_private_example.c -o 16_private_example
  ```
- **Sortie attendue** :
  ```
  Avant : AAAA
  Apres mapping : BBAA
  Fichier : AAAA
  ```

### 17_anonymous_mmap.c
- **Section** : 19.5 - Memory-mapped Files (mmap)
- **Description** : Memoire anonyme MAP_ANONYMOUS (allocation sans fichier)
- **Fichier source** : 05-mmap.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 17_anonymous_mmap.c -o 17_anonymous_mmap
  ```
- **Sortie attendue** :
  ```
  data[0] = 42
  data[1] = 100
  ```

### 18_shared_memory_fork.c
- **Section** : 19.5 - Memory-mapped Files (mmap)
- **Description** : Memoire partagee entre processus parent/fils avec MAP_SHARED + MAP_ANONYMOUS
- **Fichier source** : 05-mmap.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 18_shared_memory_fork.c -o 18_shared_memory_fork
  ```
- **Sortie attendue** (l'ordre peut varier - race condition) :
  ```
  [FILS] Compteur : 1
  [PARENT] Compteur : 2
  [FILS] Compteur : 3
  ...

  Valeur finale : 10
  ```
- **Note** : `_DEFAULT_SOURCE` pour `usleep()`, race condition intentionnelle (demonstration pedagogique)

### 19_mprotect_example.c
- **Section** : 19.5 - Memory-mapped Files (mmap)
- **Description** : Protection memoire avec mprotect() - provoque un SEGFAULT intentionnel
- **Fichier source** : 05-mmap.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 19_mprotect_example.c -o 19_mprotect_example
  ```
- **Sortie attendue** (termine avec exit code 1) :
  ```
  Donnees : Configuration importante
  Zone maintenant en lecture seule
  SEGFAULT attrape ! Tentative d'ecriture interdite.
  ```
- **Note** : Bug intentionnel (ecriture en zone read-only), le SEGFAULT est capture par un handler

### 20_benchmark.c
- **Section** : 19.5 - Memory-mapped Files (mmap)
- **Description** : Benchmark comparant read() vs mmap() sur un fichier de 10 MB
- **Fichier source** : 05-mmap.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 20_benchmark.c -o 20_benchmark
  ```
- **Sortie attendue** (les temps varient selon la machine) :
  ```
  Creation du fichier de test (10 MB)...
  Benchmarking...

  read() : X.XXX secondes (sum=0)
  mmap() : X.XXX secondes (sum=0)
  ```
- **Note** : `_POSIX_C_SOURCE 200809L` pour `clock_gettime()`, cree et supprime un fichier temporaire `testfile.bin`

---

## Resume

| # | Fichier(s) | Section | Description | Flags speciaux |
|---|-----------|---------|-------------|----------------|
| 01 | 01_shared_data.h + 01_producer.c + 01_consumer.c | 19.1 | Producteur-consommateur POSIX shm | `-lrt` |
| 02 | 02_semaphore_demo.c | 19.2 | Mutex System V avec fork | |
| 03 | 03_producer_consumer_sysv.c | 19.2 | Prod-cons System V complet | |
| 04 | 04_sender.c + 04_receiver.c | 19.3 | Message queue simple | |
| 05 | 05_client_priority.c + 05_server_priority.c | 19.3 | Message queue avec priorites | |
| 06 | 06_app.c + 06_logger.c | 19.3 | Logging distribue | |
| 07 | 07_sysv_mutex.c | 19.4 | Mutex System V (comparaison) | |
| 08 | 08_posix_mutex.c | 19.4 | Mutex POSIX (comparaison) | `-lpthread` |
| 09 | 09_sysv_shared_counter.c | 19.4 | Compteur System V shm | |
| 10 | 10_posix_shared_counter.c | 19.4 | Compteur POSIX shm | `-lrt` |
| 11 | 11_producer_consumer_sysv.c | 19.4 | Prod-cons System V (complete) | |
| 12 | 12_producer_consumer_posix.c | 19.4 | Prod-cons POSIX (complete) | `-lrt -lpthread` |
| 13 | 13_read_file_mmap.c | 19.5 | Lire fichier avec mmap | |
| 14 | 14_modify_file_mmap.c | 19.5 | Modifier fichier avec mmap | |
| 15 | 15_shared_example.c | 19.5 | MAP_SHARED demo | |
| 16 | 16_private_example.c | 19.5 | MAP_PRIVATE demo | |
| 17 | 17_anonymous_mmap.c | 19.5 | Memoire anonyme | |
| 18 | 18_shared_memory_fork.c | 19.5 | Memoire partagee fork | |
| 19 | 19_mprotect_example.c | 19.5 | mprotect() + SEGFAULT | Bug intentionnel |
| 20 | 20_benchmark.c | 19.5 | Benchmark read vs mmap | |

**Total** : 20 programmes / 22 fichiers, 0 correction dans les .md
