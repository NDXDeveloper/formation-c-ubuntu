🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 33.4 Étude de cas : Nginx

## Introduction

**Nginx : Le serveur web haute performance**

Nginx (prononcé "engine-x"), créé par Igor Sysoev en 2004, est l'un des serveurs web les plus populaires au monde. Il alimente plus de 400 millions de sites web, incluant des géants comme Netflix, Airbnb, et WordPress.com.

Nginx n'est pas qu'un simple serveur web - c'est aussi :
- **Reverse proxy** haute performance
- **Load balancer**
- **Cache HTTP**
- **API Gateway**
- **Serveur de streaming** (RTMP, HLS)

**Pourquoi étudier Nginx ?**
- ✅ Architecture événementielle multi-process élégante
- ✅ Code C modulaire et extensible
- ✅ Performance exceptionnelle (peut gérer 10 000+ connexions simultanées)
- ✅ Gestion avancée des événements (epoll, kqueue)
- ✅ Parser HTTP robuste
- ✅ Système de configuration sophistiqué
- ✅ Projet open-source actif

**Ce que vous apprendrez** :
- L'architecture master/worker multi-process
- Le modèle événementiel avancé
- Le parsing de protocoles (HTTP)
- La gestion de milliers de connexions simultanées
- Les techniques d'optimisation pour les serveurs
- La modularité et l'extensibilité en C

---

## 📊 Vue d'ensemble du projet

### Statistiques (Nginx 1.24+)

```
Taille du projet   : ~150 000 lignes de code C  
Nombre de fichiers : ~400 fichiers .c et .h  
Contributeurs      : 300+ développeurs  
Première version   : 2004  
Langage            : C (99%), Perl (scripts de config)  
Architecture       : Multi-process, event-driven  
Performance        : 10K+ connexions simultanées  
Part de marché     : ~33% des serveurs web actifs  
```

### Philosophie de Nginx

Nginx a été conçu pour résoudre le **problème C10K** (gérer 10 000 connexions simultanées) avec :

1. **Architecture événementielle** : Pas de threads par connexion
2. **Multi-process** : Un master + N workers (généralement = nombre de CPU cores)
3. **Non-bloquant** : Toutes les I/O sont asynchrones
4. **Zéro-copy** : Utilisation de sendfile() pour servir des fichiers
5. **Modularité** : Tout est un module (HTTP, mail, stream)

**Comparaison avec Apache** :

| Aspect | Apache | Nginx |
|--------|--------|-------|
| Modèle | Thread/process par connexion | Event-driven |
| Mémoire | ~4-8 MB par connexion | ~10 KB par connexion |
| C10K | Difficile | Conçu pour ça |
| Configuration | .htaccess distribué | Centralisée |
| Modules | Chargement dynamique | Compilation statique (ou dynamique depuis 1.9.11) |

---

## 📁 Étape 1 : Structure du projet

### Architecture des répertoires

Clonez Nginx pour explorer son code :

```bash
# Nginx officiel (GitHub, dépôt principal depuis 2024)
git clone https://github.com/nginx/nginx.git  
cd nginx  
tree -L 1 -d  
```

```
nginx/
├── auto/               # Scripts de configuration (comme autotools)
├── conf/               # Fichiers de configuration exemples
├── contrib/            # Scripts et outils contributés
├── docs/               # Documentation
├── misc/               # Utilitaires divers
├── src/                # Code source (TOUT est ici)
│   ├── core/           # Cœur de Nginx
│   ├── event/          # Système d'événements
│   ├── http/           # Module HTTP
│   ├── mail/           # Module mail proxy
│   ├── stream/         # Module stream (TCP/UDP proxy)
│   └── os/             # Code spécifique à l'OS
└── objs/               # Fichiers de build (généré)
```

**Observation** : Le code est très organisé par fonctionnalité.

---

### Fichiers clés dans `src/core/`

| Fichier | Rôle | Lignes |
|---------|------|--------|
| `nginx.c` | Point d'entrée, gestion master | ~1500 |
| `ngx_cycle.c` | Cycle de vie du serveur | ~1200 |
| `ngx_connection.c` | Gestion des connexions | ~1200 |
| `ngx_palloc.c` | Pool allocator (mémoire) | ~300 |
| `ngx_array.c` | Tableaux dynamiques | ~200 |
| `ngx_list.c` | Listes | ~100 |
| `ngx_queue.c` | Files à double extrémité | ~100 |
| `ngx_rbtree.c` | Red-Black tree | ~500 |
| `ngx_string.c` | Manipulation de chaînes | ~300 |

### Fichiers clés dans `src/event/`

| Fichier | Rôle | Lignes |
|---------|------|--------|
| `ngx_event.c` | Cœur du système d'événements | ~1200 |
| `ngx_event_accept.c` | Acceptation de connexions | ~500 |
| `modules/ngx_epoll_module.c` | Backend epoll (Linux) | ~900 |
| `modules/ngx_kqueue_module.c` | Backend kqueue (BSD/macOS) | ~800 |
| `ngx_event_timer.c` | Gestion des timers | ~300 |

### Fichiers clés dans `src/http/`

| Fichier | Rôle | Lignes |
|---------|------|--------|
| `ngx_http.c` | Initialisation du module HTTP | ~2000 |
| `ngx_http_core_module.c` | Directives de config HTTP | ~4000 |
| `ngx_http_request.c` | Traitement des requêtes | ~3500 |
| `ngx_http_parse.c` | Parsing HTTP | ~2000 |
| `ngx_http_upstream.c` | Proxy/load balancing | ~6000 |
| `modules/ngx_http_static_module.c` | Servir des fichiers statiques | ~300 |

---

## 🔍 Étape 2 : Point d'entrée et architecture

### Le fichier `src/core/nginx.c`

```c
// nginx.c (simplifié)
int main(int argc, char *const *argv)
{
    ngx_int_t         i;
    ngx_log_t        *log;
    ngx_cycle_t      *cycle, init_cycle;
    ngx_core_conf_t  *ccf;

    // 1. Initialisation de base
    if (ngx_strerror_init() != NGX_OK) {
        return 1;
    }

    // 2. Parsing des arguments
    if (ngx_get_options(argc, argv) != NGX_OK) {
        return 1;
    }

    // 3. Initialisation du cycle
    ngx_memzero(&init_cycle, sizeof(ngx_cycle_t));
    init_cycle.log = log;
    ngx_cycle = &init_cycle;

    // 4. Lecture de la configuration
    cycle = ngx_init_cycle(&init_cycle);
    if (cycle == NULL) {
        return 1;
    }

    // 5. Devenir daemon si configuré
    if (ngx_daemon(cycle->log) != NGX_OK) {
        return 1;
    }

    // 6. Mode single-process (debug) ou multi-process
    if (ngx_process == NGX_PROCESS_SINGLE) {
        ngx_single_process_cycle(cycle);
    } else {
        ngx_master_process_cycle(cycle);  // ← Mode normal
    }

    return 0;
}
```

**Flux de démarrage** :

```
Démarrage Nginx
      ↓
main() dans nginx.c
      ↓
Parsing arguments (-c config, -t test, etc.)
      ↓
ngx_init_cycle()  ← Lecture nginx.conf, init modules
      ↓
Mode daemon
      ↓
ngx_master_process_cycle()
      ↓
  ┌─────────────────────┐
  │  Master Process     │
  │  (gestion, reload)  │
  └─────────────────────┘
           ↓
    Fork N workers
           ↓
  ┌─────────────────┐
  │ Worker 1        │  ← Gère les connexions
  │ (event loop)    │
  └─────────────────┘
  ┌─────────────────┐
  │ Worker 2        │
  │ (event loop)    │
  └─────────────────┘
       ...
  ┌─────────────────┐
  │ Worker N        │
  │ (event loop)    │
  └─────────────────┘
```

---

### Architecture Master/Workers

**Master Process** :
- Lit la configuration
- Crée les workers
- Gère les signaux (reload, shutdown)
- Supervise les workers (redémarre si crash)

**Worker Processes** :
- Acceptent les connexions
- Traitent les requêtes HTTP
- Gèrent l'event loop
- Communiquent via shared memory

```c
// os/unix/ngx_process_cycle.c
void ngx_master_process_cycle(ngx_cycle_t *cycle)
{
    char              *title;
    u_char            *p;
    size_t             size;
    ngx_int_t          i;
    ngx_uint_t         sigio;
    sigset_t           set;
    struct itimerval   itv;
    ngx_uint_t         live;
    ngx_msec_t         delay;
    ngx_core_conf_t   *ccf;

    // Bloquer les signaux
    sigemptyset(&set);
    sigaddset(&set, SIGCHLD);
    sigaddset(&set, SIGALRM);
    sigaddset(&set, SIGIO);
    sigaddset(&set, SIGINT);
    // ...
    if (sigprocmask(SIG_BLOCK, &set, NULL) == -1) {
        ngx_log_error(NGX_LOG_ALERT, cycle->log, ngx_errno,
                      "sigprocmask() failed");
    }

    // Lancer les workers
    ngx_start_worker_processes(cycle, ccf->worker_processes,
                               NGX_PROCESS_RESPAWN);

    // Boucle du master
    for ( ;; ) {
        delay = 500;  // 500ms

        // Attendre des signaux
        ngx_log_debug0(NGX_LOG_DEBUG_EVENT, cycle->log, 0, "sigsuspend");
        sigsuspend(&set);

        // Traiter les signaux reçus
        if (ngx_reap) {
            ngx_reap = 0;
            live = ngx_reap_children(cycle);  // Récupérer les workers morts
        }

        if (ngx_terminate) {
            ngx_signal_worker_processes(cycle, SIGTERM);  // Arrêt
        }

        if (ngx_quit) {
            ngx_signal_worker_processes(cycle, SIGQUIT);  // Graceful shutdown
        }

        if (ngx_reconfigure) {
            ngx_reconfigure = 0;

            if (ngx_new_binary == 0) {
                // Recharger la configuration
                cycle = ngx_init_cycle(cycle);
                ngx_start_worker_processes(cycle, ccf->worker_processes,
                                          NGX_PROCESS_RESPAWN);
            }
        }
    }
}
```

---

### Worker Process

```c
// os/unix/ngx_process_cycle.c
static void ngx_worker_process_cycle(ngx_cycle_t *cycle, void *data)
{
    ngx_int_t worker = (intptr_t) data;

    ngx_process = NGX_PROCESS_WORKER;
    ngx_worker = worker;

    // Initialisation du worker
    ngx_worker_process_init(cycle, worker);

    // Changer le titre du process
    ngx_setproctitle("worker process");

    // Boucle principale du worker
    for ( ;; ) {

        if (ngx_exiting) {
            // Fermeture gracieuse
            if (ngx_event_no_timers_left() == NGX_OK) {
                ngx_worker_process_exit(cycle);
            }
        }

        // Traiter les événements
        ngx_process_events_and_timers(cycle);  // ← Cœur du worker

        // Traiter les signaux
        if (ngx_terminate) {
            ngx_worker_process_exit(cycle);
        }

        if (ngx_quit) {
            ngx_quit = 0;
            if (!ngx_exiting) {
                ngx_close_listening_sockets(cycle);
                ngx_exiting = 1;
            }
        }

        if (ngx_reopen) {
            ngx_reopen = 0;
            ngx_reopen_files(cycle, -1);
        }
    }
}
```

---

## 🗄️ Étape 3 : Structures de données fondamentales

### 3.1 Pool Allocator : `ngx_pool_t`

Nginx utilise des **memory pools** pour une gestion efficace de la mémoire.

**Concept** : Allouer un gros bloc de mémoire, puis distribuer des petits morceaux sans appeler `malloc()` à chaque fois.

```c
// core/ngx_palloc.h
typedef struct ngx_pool_s ngx_pool_t;

struct ngx_pool_s {
    ngx_pool_data_t       d;           // Données du pool
    size_t                max;         // Taille max pour allocation "small"
    ngx_pool_t           *current;     // Pool courant pour allocations
    ngx_chain_t          *chain;
    ngx_pool_large_t     *large;       // Liste des grandes allocations
    ngx_pool_cleanup_t   *cleanup;     // Callbacks de nettoyage
    ngx_log_t            *log;
};

typedef struct {
    u_char               *last;        // Fin des données utilisées
    u_char               *end;         // Fin du bloc
    ngx_pool_t           *next;        // Pool suivant
    ngx_uint_t            failed;      // Nombre d'échecs d'allocation
} ngx_pool_data_t;
```

**Visualisation** :

```
Pool 1:
┌────────────────────────────────────┐
│ [header] [alloc1][alloc2][alloc3]  │ ← last
│                                ... │ ← end
└────────────────────────────────────┘
         ↓ next
Pool 2:
┌────────────────────────────────────┐
│ [header] [alloc4][alloc5]          │
│                          ...       │
└────────────────────────────────────┘
```

**API** :

```c
// Créer un pool
ngx_pool_t *pool = ngx_create_pool(16384, log);  // 16 KB

// Allouer depuis le pool
void *ptr = ngx_palloc(pool, 256);        // Aligné  
void *ptr2 = ngx_pnalloc(pool, 128);      // Non aligné (plus rapide)  
void *ptr3 = ngx_pcalloc(pool, 512);      // Mis à zéro  

// Détruire le pool (libère TOUT d'un coup)
ngx_destroy_pool(pool);
```

**Implémentation** :

```c
// core/ngx_palloc.c
void *ngx_palloc(ngx_pool_t *pool, size_t size)
{
    if (size <= pool->max) {
        // Petite allocation : utiliser le pool courant
        return ngx_palloc_small(pool, size, 1);
    }

    // Grande allocation : malloc direct
    return ngx_palloc_large(pool, size);
}

static ngx_inline void *ngx_palloc_small(ngx_pool_t *pool, size_t size, ngx_uint_t align)
{
    u_char      *m;
    ngx_pool_t  *p;

    p = pool->current;

    do {
        m = p->d.last;

        if (align) {
            m = ngx_align_ptr(m, NGX_ALIGNMENT);  // Alignement
        }

        if ((size_t) (p->d.end - m) >= size) {
            // Il y a de la place dans ce pool
            p->d.last = m + size;
            return m;
        }

        p = p->d.next;  // Essayer le pool suivant

    } while (p);

    // Aucun pool n'a assez de place : allouer un nouveau pool
    return ngx_palloc_block(pool, size);
}
```

**Avantages des pools** :
- ✅ Allocation ultra-rapide (juste un déplacement de pointeur)
- ✅ Pas de fragmentation
- ✅ Libération en masse (destroy détruit tout)
- ✅ Parfait pour les requêtes HTTP (créer pool, traiter requête, destroy)

---

### 3.2 Chaînes : `ngx_chain_t`

Nginx utilise des **chaînes de buffers** pour les I/O.

```c
// core/ngx_buf.h
typedef struct ngx_buf_s  ngx_buf_t;  
typedef struct ngx_chain_s ngx_chain_t;  

struct ngx_buf_s {
    u_char          *pos;      // Position de lecture
    u_char          *last;     // Position d'écriture
    off_t            file_pos; // Position dans le fichier
    off_t            file_last;

    u_char          *start;    // Début du buffer
    u_char          *end;      // Fin du buffer
    ngx_file_t      *file;     // Fichier associé
    ngx_buf_t       *shadow;

    // Flags
    unsigned         temporary:1;
    unsigned         memory:1;
    unsigned         mmap:1;
    unsigned         recycled:1;
    unsigned         in_file:1;
    unsigned         flush:1;
    unsigned         sync:1;
    unsigned         last_buf:1;      // Dernier buffer de la réponse
    unsigned         last_in_chain:1;
    unsigned         last_shadow:1;
    unsigned         temp_file:1;
};

struct ngx_chain_s {
    ngx_buf_t    *buf;
    ngx_chain_t  *next;
};
```

**Utilisation** : Chaîner plusieurs buffers pour représenter une réponse HTTP.

```
Réponse HTTP :
┌──────────────┐     ┌──────────────┐     ┌──────────────┐
│ Chain 1      │ --> │ Chain 2      │ --> │ Chain 3      │ --> NULL
│ buf: headers │     │ buf: body    │     │ buf: footer  │
└──────────────┘     └──────────────┘     └──────────────┘
```

**Exemple** : Servir un fichier avec sendfile

```c
// http/ngx_http_core_module.c (simplifié)
ngx_int_t ngx_http_send_response(ngx_http_request_t *r, ngx_uint_t status,
    ngx_str_t *content_type, ngx_buf_t *b)
{
    ngx_chain_t   out;

    // Préparer les headers
    r->headers_out.status = status;
    r->headers_out.content_type = *content_type;
    r->headers_out.content_length_n = b->last - b->pos;

    // Envoyer les headers
    ngx_http_send_header(r);

    // Envoyer le body
    out.buf = b;
    out.next = NULL;

    return ngx_http_output_filter(r, &out);
}
```

---

### 3.3 Red-Black Tree : `ngx_rbtree_t`

Nginx utilise des **arbres rouge-noir** pour les timers et les caches.

```c
// core/ngx_rbtree.h
typedef struct ngx_rbtree_node_s  ngx_rbtree_node_t;

struct ngx_rbtree_node_s {
    ngx_rbtree_key_t       key;      // Clé (par exemple, timestamp)
    ngx_rbtree_node_t     *left;     // Fils gauche
    ngx_rbtree_node_t     *right;    // Fils droit
    ngx_rbtree_node_t     *parent;   // Parent
    u_char                 color;    // Rouge ou noir
    u_char                 data;     // Données custom
};

typedef struct ngx_rbtree_s  ngx_rbtree_t;

struct ngx_rbtree_s {
    ngx_rbtree_node_t     *root;     // Racine
    ngx_rbtree_node_t     *sentinel; // Sentinelle (nœud vide)
    ngx_rbtree_insert_pt   insert;   // Fonction d'insertion custom
};
```

**Usage** : Gestion des timers (événements temporels)

```c
// event/ngx_event_timer.c
ngx_rbtree_t              ngx_event_timer_rbtree;  
static ngx_rbtree_node_t  ngx_event_timer_sentinel;  

void ngx_event_timer_init(ngx_log_t *log)
{
    ngx_rbtree_init(&ngx_event_timer_rbtree, &ngx_event_timer_sentinel,
                    ngx_rbtree_insert_timer_value);
}

void ngx_event_add_timer(ngx_event_t *ev, ngx_msec_t timer)
{
    ngx_msec_t      key;
    ngx_msec_int_t  diff;

    key = ngx_current_msec + timer;  // Timestamp d'expiration

    // Insérer dans l'arbre
    ev->timer.key = key;
    ngx_rbtree_insert(&ngx_event_timer_rbtree, &ev->timer);

    ev->timer_set = 1;
}
```

**Avantage** : Insertion, suppression, et recherche en O(log n).

---

### 3.4 Strings : `ngx_str_t`

Nginx a son propre type de chaîne **sans null terminator**.

```c
// core/ngx_string.h
typedef struct {
    size_t      len;   // Longueur
    u_char     *data;  // Données (PAS forcément null-terminé)
} ngx_str_t;
```

**Important** : `ngx_str_t` n'est **pas** null-terminé par défaut.

```c
ngx_str_t  str = ngx_string("hello");  // Macro pour initialiser

// DANGER : Ne fonctionne pas toujours
printf("%s\n", str.data);  // ❌ Peut crasher !

// Correct : utiliser len
printf("%.*s\n", (int)str.len, str.data);  // ✅
```

**Pourquoi ?** Performance. Pas besoin de `\0` si on connaît la longueur.

---

## 🧩 Étape 4 : L'Event Loop

### Architecture événementielle

Chaque worker Nginx gère des milliers de connexions avec un seul thread grâce à l'event loop.

```c
// event/ngx_event.c
void ngx_process_events_and_timers(ngx_cycle_t *cycle)
{
    ngx_uint_t  flags;
    ngx_msec_t  timer, delta;

    // 1. Calculer le timeout (prochain timer)
    timer = ngx_event_find_timer();

    // 2. Flags pour l'event loop
    flags = NGX_UPDATE_TIME;

    // 3. Traiter les événements I/O (epoll_wait, etc.)
    (void) ngx_process_events(cycle, timer, flags);

    // 4. Traiter les événements expirés (timers)
    ngx_event_expire_timers();

    // 5. Traiter les événements postés (deferred)
    ngx_event_process_posted(cycle, &ngx_posted_accept_events);
    ngx_event_process_posted(cycle, &ngx_posted_events);
}
```

### Backend epoll (Linux)

```c
// event/modules/ngx_epoll_module.c
static ngx_int_t ngx_epoll_process_events(ngx_cycle_t *cycle, ngx_msec_t timer,
    ngx_uint_t flags)
{
    int                events;
    uint32_t           revents;
    ngx_int_t          instance, i;
    ngx_uint_t         level;
    ngx_err_t          err;
    ngx_event_t       *rev, *wev;
    ngx_queue_t       *queue;
    ngx_connection_t  *c;

    // Attendre des événements (bloquant jusqu'à timeout)
    events = epoll_wait(ep, event_list, (int) nevents, timer);

    if (events == -1) {
        err = ngx_errno;
        if (err == EINTR) {
            return NGX_OK;  // Interrompu par un signal
        }
        ngx_log_error(NGX_LOG_ALERT, cycle->log, err, "epoll_wait() failed");
        return NGX_ERROR;
    }

    if (events == 0) {
        if (timer != NGX_TIMER_INFINITE) {
            return NGX_OK;  // Timeout
        }
    }

    // Traiter chaque événement
    for (i = 0; i < events; i++) {
        c = event_list[i].data.ptr;  // Récupérer la connexion

        revents = event_list[i].events;

        // Événement de lecture
        if (revents & (EPOLLIN|EPOLLERR|EPOLLHUP)) {
            rev = c->read;

            if ((revents & EPOLLIN) && rev->active) {
                rev->ready = 1;
                rev->available = 1;

                // Poster l'événement pour traitement
                ngx_post_event(rev, &ngx_posted_events);
            }
        }

        // Événement d'écriture
        if (revents & (EPOLLOUT|EPOLLERR|EPOLLHUP)) {
            wev = c->write;

            if ((revents & EPOLLOUT) && wev->active) {
                wev->ready = 1;
                ngx_post_event(wev, &ngx_posted_events);
            }
        }
    }

    return NGX_OK;
}
```

**Flux** :

```
Worker en attente
      ↓
epoll_wait(timeout)  ← Bloque jusqu'à événement ou timeout
      ↓
[Client envoie requête HTTP]
      ↓
epoll_wait retourne : socket lisible
      ↓
Poster événement de lecture
      ↓
Traiter les événements postés
      ↓
ngx_http_wait_request_handler()
      ↓
Lire la requête HTTP
      ↓
Parser la requête
      ↓
Générer la réponse
      ↓
Envoyer la réponse (non-bloquant)
      ↓
Retour à epoll_wait
```

---

## 🔬 Étape 5 : Traitement d'une requête HTTP

### Flux complet d'une requête

```
Client → [SYN] → Nginx
                    ↓
              accept() dans ngx_event_accept.c
                    ↓
              Créer ngx_connection_t
                    ↓
              Enregistrer événement de lecture
                    ↓
[Client envoie "GET / HTTP/1.1\r\n..."]
                    ↓
              epoll détecte lecture disponible
                    ↓
              ngx_http_wait_request_handler()
                    ↓
              Lire depuis socket → buffer
                    ↓
              ngx_http_parse_request_line()
                    ↓
              Parser méthode, URI, version
                    ↓
              ngx_http_process_request_headers()
                    ↓
              Parser headers (Host:, User-Agent:, etc.)
                    ↓
              ngx_http_process_request()
                    ↓
              Trouver le handler approprié (static, proxy, etc.)
                    ↓
              ngx_http_static_handler() (exemple)
                    ↓
              open() le fichier
                    ↓
              Construire la réponse (headers + body)
                    ↓
              ngx_http_send_header()
                    ↓
              ngx_http_output_filter()
                    ↓
              sendfile() ou write()
                    ↓
              Fermer connexion ou keep-alive
```

---

### Parsing de la requête HTTP

```c
// http/ngx_http_parse.c
ngx_int_t ngx_http_parse_request_line(ngx_http_request_t *r, ngx_buf_t *b)
{
    u_char  c, ch, *p, *m;
    enum {
        sw_start = 0,
        sw_method,
        sw_spaces_before_uri,
        sw_schema,
        sw_schema_slash,
        sw_schema_slash_slash,
        sw_host_start,
        sw_host,
        sw_host_end,
        sw_host_ip_literal,
        sw_port,
        sw_after_slash_in_uri,
        sw_check_uri,
        sw_uri,
        sw_http_09,
        sw_http_H,
        sw_http_HT,
        sw_http_HTT,
        sw_http_HTTP,
        sw_first_major_digit,
        sw_major_digit,
        sw_first_minor_digit,
        sw_minor_digit,
        sw_spaces_after_digit,
        sw_almost_done
    } state;

    state = r->state;

    for (p = b->pos; p < b->last; p++) {
        ch = *p;

        switch (state) {

        case sw_start:
            r->request_start = p;

            if (ch == CR || ch == LF) {
                break;  // Ignorer lignes vides initiales
            }

            if ((ch < 'A' || ch > 'Z') && ch != '_') {
                return NGX_HTTP_PARSE_INVALID_METHOD;
            }

            state = sw_method;
            break;

        case sw_method:
            if (ch == ' ') {
                r->method_end = p - 1;
                m = r->request_start;

                // Identifier la méthode
                switch (p - m) {
                case 3:
                    if (ngx_str3_cmp(m, 'G', 'E', 'T', ' ')) {
                        r->method = NGX_HTTP_GET;
                        break;
                    }
                    if (ngx_str3_cmp(m, 'P', 'U', 'T', ' ')) {
                        r->method = NGX_HTTP_PUT;
                        break;
                    }
                    break;

                case 4:
                    if (ngx_str4_cmp(m, 'P', 'O', 'S', 'T')) {
                        r->method = NGX_HTTP_POST;
                        break;
                    }
                    if (ngx_str4_cmp(m, 'H', 'E', 'A', 'D')) {
                        r->method = NGX_HTTP_HEAD;
                        break;
                    }
                    break;

                // ... autres méthodes
                }

                state = sw_spaces_before_uri;
                break;
            }

            if ((ch < 'A' || ch > 'Z') && ch != '_') {
                return NGX_HTTP_PARSE_INVALID_METHOD;
            }

            break;

        case sw_spaces_before_uri:
            if (ch == '/') {
                r->uri_start = p;
                state = sw_after_slash_in_uri;
                break;
            }

            switch (ch) {
            case ' ':
                break;
            case 'h':
                r->schema_start = p;
                state = sw_schema;
                break;
            default:
                return NGX_HTTP_PARSE_INVALID_REQUEST;
            }
            break;

        // ... beaucoup d'autres états pour parser l'URI, la version HTTP, etc.

        case sw_almost_done:
            r->request_end = p - 1;
            switch (ch) {
            case LF:
                goto done;
            default:
                return NGX_HTTP_PARSE_INVALID_REQUEST;
            }
        }
    }

    // Pas encore terminé : besoin de plus de données
    b->pos = p;
    r->state = state;
    return NGX_AGAIN;

done:
    b->pos = p + 1;
    r->state = sw_start;
    return NGX_OK;
}
```

**État-machine** : Le parser est une énorme machine à états qui traite caractère par caractère.

**Avantages** :
- ✅ Pas de backtracking
- ✅ Gère les requêtes partielles (retourne `NGX_AGAIN`)
- ✅ Très rapide
- ✅ Robuste face aux entrées malformées

---

### Handler de fichiers statiques

```c
// http/modules/ngx_http_static_module.c
static ngx_int_t ngx_http_static_handler(ngx_http_request_t *r)
{
    u_char                    *last, *location;
    size_t                     root, len;
    ngx_str_t                  path;
    ngx_int_t                  rc;
    ngx_uint_t                 level;
    ngx_log_t                 *log;
    ngx_buf_t                 *b;
    ngx_chain_t                out;
    ngx_open_file_info_t       of;
    ngx_http_core_loc_conf_t  *clcf;

    // Seules GET et HEAD sont supportées
    if (!(r->method & (NGX_HTTP_GET|NGX_HTTP_HEAD))) {
        return NGX_HTTP_NOT_ALLOWED;
    }

    // Construire le chemin du fichier
    if (ngx_http_map_uri_to_path(r, &path, &root, 0) == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    log = r->connection->log;

    // Ouvrir le fichier
    ngx_memzero(&of, sizeof(ngx_open_file_info_t));

    of.read_ahead = clcf->read_ahead;
    of.directio = clcf->directio;
    of.valid = clcf->open_file_cache_valid;
    of.min_uses = clcf->open_file_cache_min_uses;
    of.errors = clcf->open_file_cache_errors;
    of.events = clcf->open_file_cache_events;

    if (ngx_http_set_disable_symlinks(r, clcf, &path, &of) != NGX_OK) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    if (ngx_open_cached_file(clcf->open_file_cache, &path, &of, r->pool)
        != NGX_OK)
    {
        // Fichier non trouvé ou erreur
        switch (of.err) {
        case 0:
            return NGX_HTTP_INTERNAL_SERVER_ERROR;
        case NGX_ENOENT:
        case NGX_ENOTDIR:
        case NGX_ENAMETOOLONG:
            level = NGX_LOG_ERR;
            rc = NGX_HTTP_NOT_FOUND;
            break;
        case NGX_EACCES:
            level = NGX_LOG_ERR;
            rc = NGX_HTTP_FORBIDDEN;
            break;
        default:
            level = NGX_LOG_CRIT;
            rc = NGX_HTTP_INTERNAL_SERVER_ERROR;
            break;
        }

        ngx_log_error(level, log, of.err, "%s \"%s\" failed", of.failed, path.data);
        return rc;
    }

    // Si c'est un répertoire → redirection 301 avec '/' final
    if (of.is_dir) {
        ngx_http_clear_location(r);
        r->headers_out.location = ngx_list_push(&r->headers_out.headers);
        // Construire l'URL avec '/' final...
        return NGX_HTTP_MOVED_PERMANENTLY;
    }

    // Configurer les headers de réponse
    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = of.size;
    r->headers_out.last_modified_time = of.mtime;

    // Envoyer les headers
    rc = ngx_http_send_header(r);
    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        return rc;
    }

    // Allouer un buffer pour le body
    b = ngx_calloc_buf(r->pool);
    if (b == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    b->file = ngx_pcalloc(r->pool, sizeof(ngx_file_t));
    if (b->file == NULL) {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    // Configurer le buffer pour sendfile
    b->file_pos = 0;
    b->file_last = of.size;
    b->in_file = b->file_last ? 1 : 0;
    b->last_buf = (r == r->main) ? 1 : 0;
    b->last_in_chain = 1;

    b->file->fd = of.fd;
    b->file->name = path;
    b->file->log = log;
    b->file->directio = of.is_directio;

    // Envoyer le fichier
    out.buf = b;
    out.next = NULL;

    return ngx_http_output_filter(r, &out);
}
```

---

## 🎓 Étape 6 : Patterns et techniques avancées

### 6.1 Shared Memory

Les workers partagent certaines données via shared memory.

```c
// core/ngx_shmem.h
typedef struct {
    u_char      *addr;     // Adresse du segment
    size_t       size;     // Taille
    ngx_str_t    name;     // Nom
    ngx_log_t   *log;
    ngx_uint_t   exists;   // Déjà existant
} ngx_shm_t;

// Créer un segment de mémoire partagée
ngx_int_t ngx_shm_alloc(ngx_shm_t *shm)
{
#if (NGX_HAVE_MAP_ANON)
    shm->addr = (u_char *) mmap(NULL, shm->size,
                                PROT_READ|PROT_WRITE,
                                MAP_ANON|MAP_SHARED, -1, 0);
    if (shm->addr == MAP_FAILED) {
        ngx_log_error(NGX_LOG_ALERT, shm->log, ngx_errno,
                      "mmap(MAP_ANON|MAP_SHARED, %uz) failed", shm->size);
        return NGX_ERROR;
    }
    return NGX_OK;
#endif
}
```

**Utilisation** : Cache partagé entre workers, statistiques, zones de rate limiting.

---

### 6.2 Zero-Copy avec sendfile

Nginx utilise `sendfile()` pour servir des fichiers sans copier en user-space.

```c
// os/unix/ngx_linux_sendfile_chain.c
ssize_t ngx_linux_sendfile(ngx_connection_t *c, ngx_buf_t *file, size_t size)
{
    off_t      offset;
    ssize_t    n;
    ngx_err_t  err;

    offset = file->file_pos;

    // Envoi direct depuis le fichier vers le socket
    n = sendfile(c->fd, file->file->fd, &offset, size);

    if (n == -1) {
        err = ngx_errno;

        if (err == NGX_EAGAIN || err == NGX_EINTR) {
            return NGX_AGAIN;  // Retry
        }

        ngx_log_error(NGX_LOG_CRIT, c->log, err, "sendfile() failed");
        return NGX_ERROR;
    }

    return n;
}
```

**Avantage** : Pas de copie user-space → kernel → socket. Direct kernel → socket.

```
Méthode classique :  
read(file) → buffer user-space → write(socket)  
     ↑              ↑                   ↑
  2 copies    buffer inutile      2 syscalls

Avec sendfile :  
sendfile(file → socket)  
     ↑
  0 copie, 1 syscall
```

---

### 6.3 Configuration modulaire

Nginx a un système de configuration sophistiqué basé sur des directives.

```c
// http/ngx_http_core_module.c
static ngx_command_t  ngx_http_core_commands[] = {

    { ngx_string("listen"),
      NGX_HTTP_SRV_CONF|NGX_CONF_1MORE,
      ngx_http_core_listen,
      NGX_HTTP_SRV_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("server_name"),
      NGX_HTTP_SRV_CONF|NGX_CONF_1MORE,
      ngx_http_core_server_name,
      NGX_HTTP_SRV_CONF_OFFSET,
      0,
      NULL },

    { ngx_string("root"),
      NGX_HTTP_MAIN_CONF|NGX_HTTP_SRV_CONF|NGX_HTTP_LOC_CONF|NGX_HTTP_LIF_CONF
                        |NGX_CONF_TAKE1,
      ngx_http_core_root,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },

    // ... 100+ directives

    ngx_null_command
};
```

**Parsing de la configuration** : Lecture de `nginx.conf` et appel des callbacks appropriés.

---

### 6.4 Request Phases

Nginx traite les requêtes HTTP en plusieurs phases.

```c
// http/ngx_http_core_module.h
typedef enum {
    NGX_HTTP_POST_READ_PHASE = 0,       // Après lecture de la requête
    NGX_HTTP_SERVER_REWRITE_PHASE,      // Rewrite au niveau serveur
    NGX_HTTP_FIND_CONFIG_PHASE,         // Trouver la location
    NGX_HTTP_REWRITE_PHASE,             // Rewrite au niveau location
    NGX_HTTP_POST_REWRITE_PHASE,        // Après rewrite
    NGX_HTTP_PREACCESS_PHASE,           // Avant contrôle d'accès
    NGX_HTTP_ACCESS_PHASE,              // Contrôle d'accès
    NGX_HTTP_POST_ACCESS_PHASE,         // Après contrôle d'accès
    NGX_HTTP_PRECONTENT_PHASE,          // Avant génération du contenu
    NGX_HTTP_CONTENT_PHASE,             // Génération du contenu
    NGX_HTTP_LOG_PHASE                  // Logging
} ngx_http_phases;
```

**Chaque module** peut s'enregistrer dans une ou plusieurs phases.

```c
// http/ngx_http.c
void ngx_http_core_run_phases(ngx_http_request_t *r)
{
    ngx_int_t                   rc;
    ngx_http_phase_handler_t   *ph;
    ngx_http_core_main_conf_t  *cmcf;

    cmcf = ngx_http_get_module_main_conf(r, ngx_http_core_module);

    ph = cmcf->phase_engine.handlers;

    while (ph[r->phase_handler].checker) {

        rc = ph[r->phase_handler].checker(r, &ph[r->phase_handler]);

        if (rc == NGX_OK) {
            return;  // Requête terminée
        }
    }
}
```

---

### 6.5 Upstream et Load Balancing

Nginx peut proxyer des requêtes vers des backends.

```c
// http/ngx_http_upstream.h
struct ngx_http_upstream_s {
    ngx_http_upstream_handler_pt     read_event_handler;
    ngx_http_upstream_handler_pt     write_event_handler;

    ngx_peer_connection_t            peer;  // Connexion au backend

    ngx_buf_t                        buffer;  // Buffer pour la réponse

    ngx_http_upstream_conf_t        *conf;
    ngx_http_upstream_srv_conf_t    *upstream;

    // ... beaucoup d'autres champs
};
```

**Load balancing** : Nginx supporte plusieurs algorithmes (round-robin, least-conn, ip-hash).

---

## 🎯 Étape 7 : Ce que nous apprenons de Nginx

### 7.1 Architecture multi-process

**Leçon** : Master/workers est idéal pour les serveurs.

**Avantages** :
- ✅ Isolation : Un worker qui crash n'affecte pas les autres
- ✅ Reload gracieux : Nouveaux workers avec nouvelle config, anciens terminent
- ✅ Utilisation multi-core : Un worker par CPU
- ✅ Pas de locks entre workers (chaque worker a ses propres connexions)

---

### 7.2 Memory Pools

**Leçon** : Pour des workloads avec cycle de vie clair (requête HTTP), les pools sont parfaits.

**Avantages** :
- ✅ Allocation ultra-rapide
- ✅ Pas de fragmentation
- ✅ Libération en masse
- ✅ Pas de fuites possibles (tout détruit à la fin)

**À appliquer** : Serveurs HTTP, traitements par batch, systèmes avec phases distinctes.

---

### 7.3 Event-driven non-bloquant

**Leçon** : Un seul thread peut gérer des milliers de connexions.

**Pattern** :
1. Enregistrer les I/O dans epoll/kqueue
2. Attendre des événements
3. Traiter les événements (callbacks)
4. Retour à 2

**Quand utiliser** : Serveurs I/O-bound (web, proxy, cache).

---

### 7.4 State machines pour parsing

**Leçon** : Les parsers en C sont souvent des machines à états.

**Avantages** :
- ✅ Pas de récursion (pas de stack overflow)
- ✅ Gère les données partielles
- ✅ Performance prévisible
- ✅ Facile à déboguer

---

### 7.5 Zero-copy avec sendfile

**Leçon** : Évitez les copies mémoire inutiles.

**Techniques** :
- `sendfile()` pour fichiers
- `splice()` pour pipes
- Direct DMA quand possible

---

### 7.6 Modularité

**Leçon** : Une bonne architecture de modules permet l'extensibilité.

Nginx a des modules pour :
- HTTP, Mail, Stream
- Load balancing, caching, SSL
- Compression, rewrite, auth
- Logging, monitoring, etc.

**À appliquer** : Définissez des interfaces claires, permettez le chargement dynamique.

---

## 🔧 Étape 8 : Outils pour explorer Nginx

### 8.1 Compiler Nginx

```bash
cd nginx

# Configuration
./auto/configure \
    --prefix=/usr/local/nginx \
    --with-debug \
    --with-cc-opt="-g -O0"  # Symboles de débogage

# Compilation
make

# Installation
sudo make install
```

### 8.2 Déboguer avec GDB

```bash
# Lancer en mode single-process (daemon off + master_process off)
gdb --args ./objs/nginx -c conf/nginx.conf -g "daemon off; master_process off;"

(gdb) break ngx_http_process_request
(gdb) run

# Faire une requête
curl http://localhost/

# Retour dans GDB
(gdb) backtrace
(gdb) print r->uri
(gdb) continue
```

### 8.3 Logs de debug

```nginx
# nginx.conf
error_log logs/error.log debug;
```

Nginx génère des logs très détaillés en mode debug.

### 8.4 Analyse de performance

```bash
# Strace pour voir les syscalls
sudo strace -p $(pgrep -f "nginx: worker")

# Perf pour profiling
sudo perf record -p $(pgrep -f "nginx: worker") -g  
sudo perf report  
```

---

## 📚 Lectures recommandées

### Documentation officielle

- **Site officiel** : https://nginx.org/
- **Documentation** : https://nginx.org/en/docs/
- **Architecture interne** : https://www.aosabook.org/en/nginx.html (excellent article)
- **Guide du développeur** : https://nginx.org/en/docs/dev/development_guide.html

### Code source

**Ordre de lecture recommandé** :

1. `src/core/nginx.c` - Point d'entrée
2. `src/core/ngx_palloc.c` - Memory pools
3. `src/core/ngx_cycle.c` - Cycle de vie
4. `src/event/ngx_event.c` - Système d'événements
5. `src/event/modules/ngx_epoll_module.c` - Backend epoll
6. `src/http/ngx_http.c` - Module HTTP
7. `src/http/ngx_http_request.c` - Traitement des requêtes
8. `src/http/ngx_http_parse.c` - Parsing HTTP

### Livres

- **"Nginx HTTP Server"** - Clément Nedelcu
- **"Mastering Nginx"** - Dimitri Aivaliotis

### Articles

- **"Inside NGINX: How We Designed for Performance & Scale"** (nginx.com)
- **"The Architecture of Open Source Applications - Nginx"** (AOSA Book)

---

## 💡 Exercice mental : Concevoir une fonctionnalité

**Scénario** : Vous devez ajouter un module qui compte le nombre de requêtes par IP et bloque les IPs qui dépassent 100 req/min.

**Questions** :
1. Dans quelle phase HTTP vous enregistreriez-vous ?
2. Comment stockeriez-vous les compteurs (structure de données) ?
3. Comment partageriez-vous les données entre workers ?
4. Comment géreriez-vous le reset des compteurs chaque minute ?

**Réponse suggérée** :

1. **Phase** : `NGX_HTTP_PREACCESS_PHASE` (avant le contrôle d'accès)

2. **Structure de données** : Hash table (dict) `IP → compteur`
```c
typedef struct {
    ngx_str_t    ip;        // Adresse IP
    ngx_uint_t   count;     // Nombre de requêtes
    time_t       window;    // Timestamp du début de la fenêtre
} rate_limit_entry_t;
```

3. **Partage entre workers** : Shared memory segment avec spinlock
```c
typedef struct {
    ngx_shmtx_t   mutex;
    ngx_rbtree_t  rbtree;  // Red-black tree pour les IPs
    // ...
} rate_limit_shm_t;
```

4. **Reset des compteurs** : Timer qui s'exécute chaque minute
```c
static void rate_limit_timer_handler(ngx_event_t *ev) {
    // Parcourir l'arbre et reset les compteurs expirés
    time_t now = ngx_time();
    // ...

    // Ré-enregistrer le timer
    ngx_add_timer(ev, 60000);  // 60 secondes
}
```

---

## 🎯 Récapitulatif : Leçons de Nginx

| Principe | Implémentation Nginx | À retenir |
|----------|---------------------|-----------|
| **Multi-process** | Master + N workers | Isolation et utilisation multi-core |
| **Event-driven** | epoll/kqueue + callbacks | Gérer 10K+ connexions avec 1 thread |
| **Memory pools** | ngx_pool_t | Allocation rapide et libération en masse |
| **Zero-copy** | sendfile() | Éviter les copies inutiles |
| **State machines** | Parser HTTP | Robuste et performant |
| **Modularité** | Système de phases | Extensibilité |
| **Shared memory** | mmap() + spinlocks | Partage de données entre processus |
| **Non-bloquant** | Toutes les I/O | Pas d'attente active |

---

## 🔗 Liens avec les autres sections

- **Section 33.1** : Méthodologie appliquée
- **Section 33.2** : Git a aussi une architecture modulaire
- **Section 33.3** : Redis utilise aussi event loop (mais single-process)
- **Module 6** : Programmation système (fork, epoll, sendfile)
- **Module 7** : Optimisations (zero-copy, memory pools)
- **Module 8** : CI/CD (compilation, tests)

---

## 🚀 Pour aller plus loin

### Contribuer à Nginx

1. **Clonez le dépôt** : `git clone https://github.com/nginx/nginx.git`
2. **Lisez le guide** : https://nginx.org/en/docs/dev/development_guide.html
3. **Mailing list** : nginx-devel@nginx.org

### Créer votre propre module

Nginx permet de créer des modules externes :

```c
// ngx_http_mymodule_module.c
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

static char *ngx_http_mymodule(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_command_t ngx_http_mymodule_commands[] = {
    { ngx_string("mymodule"),
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      ngx_http_mymodule,
      0,
      0,
      NULL },
    ngx_null_command
};

static ngx_http_module_t ngx_http_mymodule_module_ctx = {
    NULL,  /* preconfiguration */
    NULL,  /* postconfiguration */
    // ...
};

ngx_module_t ngx_http_mymodule_module = {
    NGX_MODULE_V1,
    &ngx_http_mymodule_module_ctx,
    ngx_http_mymodule_commands,
    NGX_HTTP_MODULE,
    NULL,  /* init master */
    NULL,  /* init module */
    NULL,  /* init process */
    NULL,  /* init thread */
    NULL,  /* exit thread */
    NULL,  /* exit process */
    NULL,  /* exit master */
    NGX_MODULE_V1_PADDING
};

static ngx_int_t ngx_http_mymodule_handler(ngx_http_request_t *r)
{
    ngx_buf_t    *b;
    ngx_chain_t   out;

    r->headers_out.content_type.len = sizeof("text/plain") - 1;
    r->headers_out.content_type.data = (u_char *) "text/plain";

    b = ngx_pcalloc(r->pool, sizeof(ngx_buf_t));

    out.buf = b;
    out.next = NULL;

    u_char *msg = (u_char *) "Hello from my module!\n";
    b->pos = msg;
    b->last = msg + ngx_strlen(msg);
    b->memory = 1;
    b->last_buf = 1;

    r->headers_out.status = NGX_HTTP_OK;
    r->headers_out.content_length_n = ngx_strlen(msg);

    ngx_http_send_header(r);

    return ngx_http_output_filter(r, &out);
}
```

---

## 📝 Points clés à retenir

- ✅ Nginx démontre qu'une architecture **multi-process event-driven** peut gérer énormément de charge
- ✅ Les **memory pools** sont parfaits pour les workloads avec cycle de vie clair
- ✅ Le **modèle master/workers** offre isolation et graceful reload
- ✅ Les **state machines** sont idéales pour parser des protocoles
- ✅ Le **zero-copy** (sendfile) élimine les copies inutiles
- ✅ La **modularité** permet d'étendre facilement les fonctionnalités
- ✅ L'architecture **non-bloquante** évite les threads par connexion

**Comparaison finale** :

| Projet | Architecture | Forces |
|--------|-------------|--------|
| **Git** | Single-process, multi-commandes | Gestion de versions, structures de données complexes |
| **Redis** | Single-process, event-driven | Performance in-memory, structures de données riches |
| **Nginx** | Multi-process, event-driven | Scalabilité, C10K, performance réseau |

Chaque projet a choisi l'architecture la plus adaptée à son cas d'usage !

---

**💡 Conseil final** : Nginx est un **chef-d'œuvre d'ingénierie**. Pour vraiment comprendre, compilez-le avec `-g -O0`, lancez-le en mode single-process avec GDB, posez un breakpoint dans `ngx_http_process_request()`, et faites une requête avec curl. Suivez le flux pas à pas. C'est en **voyant le code s'exécuter** que vous comprendrez vraiment l'architecture événementielle ! 🚀

⏭️ [Contribution à des projets C open-source](/33-analyse-code-opensource/05-contribution-opensource.md)
