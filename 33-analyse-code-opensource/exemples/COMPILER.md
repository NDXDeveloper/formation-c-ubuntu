# Chapitre 33 - Analyse de code open-source : Compilation des exemples

Note : Les .md de ce chapitre contiennent uniquement des fragments illustratifs  
de Git, Redis et Nginx non compilables standalone. Les exemples ci-dessous sont  
des implementations simplifiees des patterns identifies dans ces projets.

## Exemples

### 01_goto_cleanup.c
- **Section** : 33.1 - Pattern Goto Cleanup
- **Description** : Pattern goto cleanup (Git / Linux Kernel) pour liberation propre des ressources
- **Fichier source** : README.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 01_goto_cleanup 01_goto_cleanup.c
  ```
- **Sortie attendue** : 4 tests (cas nominal, fichier inexistant, sans goto, avantages)

### 02_dispatch_table.c
- **Section** : 33.2 - Etude de cas Git
- **Description** : Pattern dispatch table pour router les commandes vers handlers
- **Fichier source** : 02-etude-cas-git.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 02_dispatch_table 02_dispatch_table.c
  ```
- **Sortie attendue** : mini-git avec commandes version/status/add/commit/log/help + commande inconnue

### 03_sds_string.c
- **Section** : 33.3 - Etude de cas Redis
- **Description** : Simple Dynamic String (SDS) inspire de Redis
- **Fichier source** : 03-etude-cas-redis.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 03_sds_string 03_sds_string.c
  ```
- **Sortie attendue** : Demo creation, concatenation, duplication, clear, printf, compatibilite C

### 04_memory_pool.c
- **Section** : 33.4 - Etude de cas Nginx
- **Description** : Memory pool inspire de Nginx (ngx_pool_t)
- **Fichier source** : 04-etude-cas-nginx.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 04_memory_pool 04_memory_pool.c
  ```
- **Sortie attendue** : Simulation requete HTTP avec allocations pool, statistiques, destruction

### 05_event_loop.c
- **Section** : 33.3/33.4 - Event Loop (Redis/Nginx)
- **Description** : Boucle evenementielle simplifiee avec poll()
- **Fichier source** : 03-etude-cas-redis.md, 04-etude-cas-nginx.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -D_POSIX_C_SOURCE=199309L \
      -o 05_event_loop 05_event_loop.c
  ```
- **Sortie attendue** : 4 commandes traitees via event loop (SET/GET/SET/DEL)

### 06_hash_table.c
- **Section** : 33.3 - Etude de cas Redis
- **Description** : Hash table avec rehashing incremental inspire de Redis dict
- **Fichier source** : 03-etude-cas-redis.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -D_POSIX_C_SOURCE=200809L \
      -o 06_hash_table 06_hash_table.c
  ```
- **Sortie attendue** : 10 insertions avec rehash automatique, recherches, mise a jour, suppression

### 07_object_pool.c
- **Section** : 33.2 - Object Pooling (Git/Redis)
- **Description** : Pattern object pool inspire de Git (cache) et Redis (shared int)
- **Fichier source** : README.md, 02-etude-cas-git.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -D_POSIX_C_SOURCE=200809L \
      -o 07_object_pool 07_object_pool.c
  ```
- **Sortie attendue** : Pool entiers partages (refcount) + object cache avec lazy loading

### 08_coding_styles.c
- **Section** : 33.5.2 - Comprendre le coding style
- **Description** : Comparaison des coding styles C (K&R, Allman, GNU, Google)
- **Fichier source** : 05.2-coding-style.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 08_coding_styles 08_coding_styles.c
  ```
- **Sortie attendue** : 4 styles comptent 'i' dans "Hello, World!..." = 3 chacun, test NULL = -1

## Notes
- **05** necessite `-D_POSIX_C_SOURCE=199309L` pour `clock_gettime()`
- **06/07** necessitent `-D_POSIX_C_SOURCE=200809L` pour `strdup()`
- **03** utilise `stdarg.h` (va_list) pour la fonction catprintf
