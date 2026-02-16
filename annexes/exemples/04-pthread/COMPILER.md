# Annexe B.4 - POSIX Threads API : Guide de compilation

## Compilation par défaut

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -pthread -o programme fichier.c
```

**Note** : Tous les programmes nécessitent `-pthread` pour la liaison avec la bibliothèque POSIX Threads.

---

## Liste des exemples

### test_create.c
- **Section** : B.4 - pthread_create
- **Description** : Création d'un thread avec passage de paramètre (int)
- **Fichier source** : 04-posix-threads-api.md (lignes 50-70)
- **Sortie attendue** : `Thread 1 en cours`

### test_mutex.c
- **Section** : B.4 - Mutex
- **Description** : Protection d'un compteur partagé avec mutex (2 threads × 1000 incréments)
- **Fichier source** : 04-posix-threads-api.md (lignes 252-280)
- **Sortie attendue** : `Compteur final : 2000`

### test_prodcons.c
- **Section** : B.4 - Variables de condition
- **Description** : Pattern producteur-consommateur avec mutex et condition variable
- **Fichier source** : 04-posix-threads-api.md (lignes 370-394)
- **Sortie attendue** : `Données reçues : 42`

### test_sem.c
- **Section** : B.4 - Sémaphores POSIX
- **Description** : Sémaphore limitant à 2 threads simultanés parmi 5
- **Fichier source** : 04-posix-threads-api.md (lignes 474-505)
- **Sortie attendue** : 5 messages `Thread N travaille` (par groupes de 2)

### test_rwlock.c
- **Section** : B.4 - Read-Write Locks
- **Description** : Verrou lecture-écriture avec lecteurs multiples et écrivain exclusif
- **Fichier source** : 04-posix-threads-api.md (lignes 567-583)
- **Compilation** : Nécessite `_POSIX_C_SOURCE 200809L` (défini dans le fichier)
- **Sortie attendue** : `Écriture : 1` puis `Lecture : 1` (×2)

### test_tls.c
- **Section** : B.4 - Thread-Local Storage
- **Description** : Variables __thread avec copie propre à chaque thread
- **Fichier source** : 04-posix-threads-api.md (lignes 596-602)
- **Sortie attendue** : Deux threads affichent chacun leur valeur distincte (10 et 20)
