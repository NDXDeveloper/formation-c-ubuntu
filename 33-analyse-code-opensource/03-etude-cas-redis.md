🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 33.3 Étude de cas : Redis

## Introduction

**Redis : Le champion des structures de données en mémoire**

Redis (Remote Dictionary Server), créé par Salvatore Sanfilippo en 2009, est bien plus qu'une simple base de données clé-valeur. C'est un **chef-d'œuvre d'ingénierie** qui démontre comment construire un système ultra-performant avec un code C élégant et des structures de données sophistiquées.

Redis est utilisé par des millions de serveurs dans le monde pour :
- **Cache haute performance** (Facebook, Twitter, GitHub)
- **Sessions utilisateur** (Stack Overflow)
- **Files de messages** (Pub/Sub)
- **Leaderboards** (jeux en ligne)
- **Rate limiting** (APIs)

**Pourquoi étudier Redis ?**
- ✅ Code C exceptionnellement propre et lisible
- ✅ Structures de données avancées implémentées from scratch
- ✅ Architecture single-threaded simple mais performante
- ✅ Commentaires excellents (presque chaque ligne importante)
- ✅ Performance extrême (100 000+ opérations/seconde)
- ✅ Projet actif avec une communauté engagée

**Ce que vous apprendrez** :
- Comment implémenter des structures de données avancées
- L'architecture d'un serveur réseau performant
- Le modèle événementiel (event loop)
- La gestion optimisée de la mémoire
- Les techniques d'optimisation en C

---

## 📊 Vue d'ensemble du projet

### Statistiques (Redis 7.x)

```
Taille du projet   : ~80 000 lignes de code C
Nombre de fichiers : ~200 fichiers .c et .h
Contributeurs      : 600+ développeurs
Première version   : 2009
Langage            : C (99%), Tcl (tests)
Performance        : 100K+ ops/sec sur matériel standard
Latence            : <1ms (percentile 99)
```

### Philosophie de Redis

Redis se distingue par plusieurs principes :

1. **Simplicité** : Pas de dépendances externes (sauf libc)
2. **Single-threaded** : Un seul thread pour toutes les commandes
3. **In-memory** : Tout en RAM pour la vitesse
4. **Structures de données riches** : Au-delà du simple clé-valeur
5. **Persistance optionnelle** : RDB snapshots ou AOF log

---

## 📁 Étape 1 : Structure du projet

### Architecture des répertoires

Clonez Redis pour explorer son code :

```bash
git clone https://github.com/redis/redis.git
cd redis
tree -L 1 -d
```

```
redis/
├── src/                # Code source principal (tout est ici !)
├── deps/               # Dépendances (jemalloc, hiredis, lua)
├── tests/              # Tests unitaires et d'intégration
├── utils/              # Scripts utilitaires
├── runtest             # Script de lancement des tests
└── redis.conf          # Configuration par défaut
```

**Observation** : Contrairement à beaucoup de projets, Redis met **tout le code dans `src/`**. C'est simple et efficace.

---

### Fichiers clés dans `src/`

| Fichier | Rôle | Lignes |
|---------|------|--------|
| `server.c` | Cœur du serveur, initialisation, main loop | ~6000 |
| `networking.c` | Gestion des connexions clients | ~3000 |
| `db.c` | Opérations sur les bases de données | ~2000 |
| `object.c` | Gestion des objets Redis | ~1500 |
| `sds.c` | Simple Dynamic Strings (chaînes) | ~1000 |
| `dict.c` | Hash table | ~1000 |
| `t_string.c` | Commandes String (SET, GET, etc.) | ~1500 |
| `t_list.c` | Commandes List (LPUSH, RPOP, etc.) | ~2000 |
| `t_hash.c` | Commandes Hash (HSET, HGET, etc.) | ~1000 |
| `t_set.c` | Commandes Set (SADD, SMEMBERS, etc.) | ~1000 |
| `t_zset.c` | Commandes Sorted Set (ZADD, ZRANGE, etc.) | ~3000 |
| `ae.c` | Event loop (multiplexage I/O) | ~500 |
| `anet.c` | Abstraction réseau TCP | ~500 |
| `zmalloc.c` | Wrapper d'allocation mémoire | ~300 |

**Point d'entrée** : `server.c` contient la fonction `main()`.

---

## 🔍 Étape 2 : Point d'entrée et flux principal

### Le fichier `server.c`

```c
// server.c (simplifié)
int main(int argc, char **argv) {
    struct timeval tv;

    // 1. Initialisation du serveur
    initServerConfig();

    // 2. Chargement de la configuration
    if (argc >= 2) {
        loadServerConfig(argv[1], NULL);
    }

    // 3. Mode daemon si configuré
    if (server.daemonize) daemonize();

    // 4. Initialisation des structures
    initServer();

    // 5. Chargement des données depuis le disque
    loadDataFromDisk();

    // 6. Boucle principale événementielle
    aeSetBeforeSleepProc(server.el, beforeSleep);
    aeSetAfterSleepProc(server.el, afterSleep);
    aeMain(server.el);  // ← Point crucial : boucle infinie

    // 7. Nettoyage (jamais atteint en mode normal)
    aeDeleteEventLoop(server.el);
    return 0;
}
```

**Flux simplifié** :

```
Démarrage Redis
      ↓
initServerConfig()  ← Configuration par défaut
      ↓
loadServerConfig()  ← Lecture redis.conf
      ↓
initServer()        ← Création event loop, ouverture socket
      ↓
loadDataFromDisk()  ← Chargement RDB/AOF
      ↓
aeMain(event_loop)  ← BOUCLE INFINIE
      ↓
   [accepte client]
   [lit commande]
   [exécute commande]
   [répond au client]
      ↓
   [répète...]
```

---

### La structure centrale : `struct redisServer`

Redis a UNE structure globale qui contient tout l'état du serveur.

```c
// server.h (simplifié)
struct redisServer {
    // Configuration
    int port;                       // Port d'écoute (6379 par défaut)
    char *bindaddr[CONFIG_BINDADDR_MAX];
    int dbnum;                      // Nombre de databases (16 par défaut)

    // Bases de données
    redisDb *db;                    // Tableau de databases

    // Event loop
    aeEventLoop *el;                // Event loop principal
    int cronloops;                  // Compteur pour tâches périodiques

    // Clients
    list *clients;                  // Liste de tous les clients
    list *clients_to_close;         // Clients à fermer
    client *current_client;         // Client en cours de traitement

    // Statistiques
    long long stat_numcommands;     // Nombre total de commandes
    long long stat_numconnections;  // Nombre total de connexions

    // Persistance
    int saveparams[...];            // Paramètres de sauvegarde
    time_t lastsave;                // Timestamp dernier save
    int dirty;                      // Nombre de modifications depuis dernier save

    // Limites mémoire
    size_t maxmemory;               // Limite mémoire
    int maxmemory_policy;           // Politique d'éviction

    // Réplication
    char *masterhost;               // Adresse du master
    int masterport;                 // Port du master
    client *master;                 // Client master

    // ... beaucoup d'autres champs
};

extern struct redisServer server;  // Variable globale !
```

**Pattern utilisé** : Variable globale `server` accessible partout. C'est simple et efficace pour un serveur single-threaded.

---

## 🗄️ Étape 3 : Structures de données fondamentales

Redis est célèbre pour ses structures de données. Étudions les principales.

### 3.1 SDS : Simple Dynamic String

**Problème** : Les chaînes C (`char *`) ont des limitations :
- Pas de longueur stockée (besoin de `strlen()`)
- Pas de capacité (reallocation manuelle)
- Pas de sécurité (buffer overflow facile)

**Solution Redis** : SDS (Simple Dynamic String)

```c
// sds.h
typedef char *sds;  // Un sds est juste un char * !

// Mais avec un header caché AVANT le pointeur
struct __attribute__ ((__packed__)) sdshdr8 {
    uint8_t len;        // Longueur actuelle
    uint8_t alloc;      // Capacité allouée (sans compter le header et \0)
    unsigned char flags; // Type de header
    char buf[];         // Données (flexible array member)
};
```

**Le truc génial** : Le header est **avant** le pointeur !

```
Mémoire :
┌─────┬───────┬───────┬─────────────────┬────┐
│ len │ alloc │ flags │   buf (data)    │ \0 │
└─────┴───────┴───────┴─────────────────┴────┘
                       ↑
                       sds pointe ici !
```

**Avantages** :
- ✅ Compatible avec les fonctions C standard (`printf("%s", s)`)
- ✅ Longueur en O(1) : `sdslen(s)` lit juste le header
- ✅ Pas de buffer overflow : `sdscat()` agrandit automatiquement
- ✅ Binary safe : Peut contenir des `\0` au milieu

**API** :

```c
// Création
sds s = sdsnew("hello");           // Crée "hello"
s = sdscat(s, " world");           // Concatène, s = "hello world"
printf("Length: %zu\n", sdslen(s)); // O(1) !

// Modification sécurisée
s = sdscatprintf(s, " %d", 2024);  // Comme sprintf, mais sûr

// Libération
sdsfree(s);
```

**Code d'implémentation** :

```c
// sds.c
sds sdsnewlen(const void *init, size_t initlen) {
    struct sdshdr8 *sh;

    // Allouer header + données + \0
    sh = malloc(sizeof(struct sdshdr8) + initlen + 1);
    if (sh == NULL) return NULL;

    sh->len = initlen;
    sh->alloc = initlen;
    sh->flags = SDS_TYPE_8;

    // Copier les données
    if (init && initlen)
        memcpy(sh->buf, init, initlen);
    sh->buf[initlen] = '\0';

    return (char*)sh->buf;  // Retourne le pointeur vers buf !
}

size_t sdslen(const sds s) {
    // Récupérer le header depuis le pointeur
    struct sdshdr8 *sh = (void*)(s - sizeof(struct sdshdr8));
    return sh->len;
}
```

**À retenir** : SDS est un exemple parfait d'abstraction élégante en C.

---

### 3.2 Dict : Hash Table

Redis implémente sa propre hash table optimisée.

```c
// dict.h
typedef struct dictEntry {
    void *key;
    union {
        void *val;
        uint64_t u64;
        int64_t s64;
        double d;
    } v;
    struct dictEntry *next;  // Chaînage pour collisions
} dictEntry;

typedef struct dictht {
    dictEntry **table;       // Tableau de pointeurs vers dictEntry
    unsigned long size;      // Taille du tableau (toujours 2^n)
    unsigned long sizemask;  // size - 1 (pour modulo rapide)
    unsigned long used;      // Nombre d'entrées
} dictht;

typedef struct dict {
    dictType *type;          // Fonctions pour ce type de dict
    void *privdata;          // Données privées
    dictht ht[2];            // Deux hash tables pour rehashing incrémental
    long rehashidx;          // Index de rehashing (-1 si pas en cours)
    int16_t pauserehash;     // >0 si rehashing en pause
} dict;
```

**Concept clé** : Deux hash tables pour le **rehashing incrémental**.

```
État normal (pas de rehashing) :
ht[0] : [utilisée]
ht[1] : [vide]

Pendant le rehashing :
ht[0] : [partiellement vidée] ← Ancienne table
ht[1] : [partiellement remplie] ← Nouvelle table

Après le rehashing :
ht[0] : [nouvelle table]
ht[1] : [vide]
```

**Pourquoi deux tables ?** Pour éviter de bloquer Redis pendant le rehashing d'une énorme table. Redis déplace les entrées **progressivement** lors des accès.

**Algorithme de rehashing incrémental** :

```c
// dict.c (simplifié)
int dictRehash(dict *d, int n) {
    // Déplacer n entrées de ht[0] vers ht[1]
    for (int empty_visits = n * 10; n-- && d->ht[0].used != 0; ) {
        dictEntry *de, *nextde;

        // Trouver une entrée non vide dans ht[0]
        while (d->ht[0].table[d->rehashidx] == NULL) {
            d->rehashidx++;
            if (--empty_visits == 0) return 1;  // Limite de buckets vides
        }

        de = d->ht[0].table[d->rehashidx];
        // Déplacer toutes les entrées de ce bucket
        while (de) {
            nextde = de->next;

            // Calculer le nouvel index dans ht[1]
            unsigned long h = dictHashKey(d, de->key) & d->ht[1].sizemask;
            de->next = d->ht[1].table[h];
            d->ht[1].table[h] = de;

            d->ht[0].used--;
            d->ht[1].used++;
            de = nextde;
        }
        d->ht[0].table[d->rehashidx] = NULL;
        d->rehashidx++;
    }

    // Rehashing terminé ?
    if (d->ht[0].used == 0) {
        free(d->ht[0].table);
        d->ht[0] = d->ht[1];
        _dictReset(&d->ht[1]);
        d->rehashidx = -1;
        return 0;
    }
    return 1;
}
```

**Utilisation** : Chaque opération sur le dict fait progresser le rehashing de quelques entrées.

```c
dictEntry *dictFind(dict *d, const void *key) {
    if (dictIsRehashing(d)) _dictRehashStep(d);  // Avancer d'1 étape

    // Chercher dans les deux tables
    for (int table = 0; table <= 1; table++) {
        unsigned long h = dictHashKey(d, key) & d->ht[table].sizemask;
        dictEntry *he = d->ht[table].table[h];

        while (he) {
            if (dictCompareKeys(d, key, he->key))
                return he;
            he = he->next;
        }

        if (!dictIsRehashing(d)) break;  // Pas besoin de chercher dans ht[1]
    }
    return NULL;
}
```

---

### 3.3 Skiplist : Structure pour Sorted Sets

Les Sorted Sets de Redis utilisent une **skiplist** pour des insertions/recherches en O(log n).

**Qu'est-ce qu'une skiplist ?** Une liste chaînée avec des "raccourcis" sur plusieurs niveaux.

```
Niveau 3:  [1] ---------------------------------> [9]
Niveau 2:  [1] -------> [4] -----------------> [9]
Niveau 1:  [1] -> [2] -> [4] -> [5] -> [7] -> [9]
```

**Implémentation Redis** :

```c
// server.h
#define ZSKIPLIST_MAXLEVEL 32  // Hauteur max
#define ZSKIPLIST_P 0.25       // Probabilité pour chaque niveau

typedef struct zskiplistNode {
    sds ele;                    // Élément (membre du sorted set)
    double score;               // Score pour le tri
    struct zskiplistNode *backward;  // Pointeur vers le nœud précédent
    struct zskiplistLevel {
        struct zskiplistNode *forward;  // Pointeur vers le prochain nœud
        unsigned long span;             // Distance jusqu'au prochain
    } level[];                  // Tableau flexible de niveaux
} zskiplistNode;

typedef struct zskiplist {
    struct zskiplistNode *header, *tail;
    unsigned long length;       // Nombre d'éléments
    int level;                  // Niveau max actuel
} zskiplist;
```

**Création d'un nœud avec hauteur aléatoire** :

```c
// t_zset.c
int zslRandomLevel(void) {
    int level = 1;
    while ((random() & 0xFFFF) < (ZSKIPLIST_P * 0xFFFF))
        level += 1;
    return (level < ZSKIPLIST_MAXLEVEL) ? level : ZSKIPLIST_MAXLEVEL;
}

zskiplistNode *zslCreateNode(int level, double score, sds ele) {
    zskiplistNode *zn = zmalloc(sizeof(*zn) + level * sizeof(struct zskiplistLevel));
    zn->score = score;
    zn->ele = ele;
    return zn;
}
```

**Recherche dans la skiplist** :

```c
// t_zset.c (simplifié)
zskiplistNode *zslSearch(zskiplist *zsl, double score, sds ele) {
    zskiplistNode *x = zsl->header;

    // Descendre les niveaux du plus haut au plus bas
    for (int i = zsl->level - 1; i >= 0; i--) {
        while (x->level[i].forward &&
               (x->level[i].forward->score < score ||
                (x->level[i].forward->score == score &&
                 sdscmp(x->level[i].forward->ele, ele) < 0)))
        {
            x = x->level[i].forward;  // Avancer sur ce niveau
        }
    }

    x = x->level[0].forward;  // Descendre au niveau 0
    if (x && x->score == score && sdscmp(x->ele, ele) == 0)
        return x;  // Trouvé !
    return NULL;
}
```

**Complexité** : O(log n) en moyenne pour recherche, insertion, suppression.

---

### 3.4 L'objet Redis : `robj`

Tous les types Redis (String, List, Hash, Set, Sorted Set) sont encapsulés dans `robj`.

```c
// server.h
typedef struct redisObject {
    unsigned type:4;        // Type: OBJ_STRING, OBJ_LIST, etc.
    unsigned encoding:4;    // Encodage: OBJ_ENCODING_RAW, OBJ_ENCODING_INT, etc.
    unsigned lru:24;        // LRU time (pour éviction) ou LFU data
    int refcount;           // Compteur de références
    void *ptr;              // Pointeur vers les données réelles
} robj;
```

**Types Redis** :

| Type | Valeur | Description |
|------|--------|-------------|
| `OBJ_STRING` | 0 | Chaîne de caractères ou entier |
| `OBJ_LIST` | 1 | Liste doublement chaînée |
| `OBJ_SET` | 2 | Ensemble non ordonné |
| `OBJ_ZSET` | 3 | Ensemble trié (sorted set) |
| `OBJ_HASH` | 4 | Table de hachage |
| `OBJ_STREAM` | 5 | Stream (depuis Redis 5.0) |

**Encodages multiples** : Un même type peut avoir plusieurs encodages pour optimiser la mémoire.

Exemple pour `OBJ_STRING` :
- `OBJ_ENCODING_RAW` : sds (chaîne longue)
- `OBJ_ENCODING_INT` : Entier stocké directement dans `ptr`
- `OBJ_ENCODING_EMBSTR` : Chaîne courte (<44 bytes) encodée directement

```c
// object.c
robj *createStringObject(const char *ptr, size_t len) {
    if (len <= OBJ_ENCODING_EMBSTR_SIZE_LIMIT)
        return createEmbeddedStringObject(ptr, len);  // Optimisé
    else
        return createRawStringObject(ptr, len);       // sds normal
}
```

**Reference counting** : Redis utilise le comptage de références pour partager des objets.

```c
// object.c
void incrRefCount(robj *o) {
    if (o->refcount != OBJ_SHARED_REFCOUNT)
        o->refcount++;
}

void decrRefCount(robj *o) {
    if (o->refcount == 1) {
        // Libérer l'objet
        switch(o->type) {
            case OBJ_STRING: freeStringObject(o); break;
            case OBJ_LIST: freeListObject(o); break;
            // ...
        }
    } else {
        o->refcount--;
    }
}
```

---

## 🧩 Étape 4 : L'Event Loop (ae.c)

Redis est single-threaded mais peut gérer des milliers de clients grâce à l'**event loop**.

### Structure de l'event loop

```c
// ae.h
typedef struct aeEventLoop {
    int maxfd;                  // Descriptor le plus élevé
    int setsize;                // Taille max du tracked set
    long long timeEventNextId;
    aeFileEvent *events;        // Tableau d'événements fichier
    aeFiredEvent *fired;        // Événements déclenchés
    aeTimeEvent *timeEventHead; // Liste d'événements temporels
    int stop;                   // Flag pour arrêter la boucle
    void *apidata;              // Données spécifiques à l'API (epoll, kqueue, etc.)
    aeBeforeSleepProc *beforesleep;
    aeBeforeSleepProc *aftersleep;
} aeEventLoop;

typedef struct aeFileEvent {
    int mask;                   // AE_READABLE, AE_WRITABLE
    aeFileProc *rfileProc;      // Callback pour lecture
    aeFileProc *wfileProc;      // Callback pour écriture
    void *clientData;
} aeFileEvent;
```

### Boucle principale

```c
// ae.c (simplifié)
void aeMain(aeEventLoop *eventLoop) {
    eventLoop->stop = 0;
    while (!eventLoop->stop) {
        // Callback avant le sleep
        if (eventLoop->beforesleep != NULL)
            eventLoop->beforesleep(eventLoop);

        // Attendre des événements (epoll_wait, select, etc.)
        aeProcessEvents(eventLoop, AE_ALL_EVENTS|AE_CALL_AFTER_SLEEP);
    }
}

int aeProcessEvents(aeEventLoop *eventLoop, int flags) {
    int processed = 0, numevents;

    // Calculer le timeout basé sur les événements temporels
    struct timeval tv, *tvp = NULL;
    if (flags & AE_TIME_EVENTS && !(flags & AE_DONT_WAIT)) {
        tvp = &tv;
        aeSearchNearestTimer(eventLoop, tvp);  // Prochain timer
    }

    // Attendre des événements I/O (epoll_wait ou équivalent)
    numevents = aeApiPoll(eventLoop, tvp);

    // Traiter les événements I/O déclenchés
    for (int j = 0; j < numevents; j++) {
        aeFileEvent *fe = &eventLoop->events[eventLoop->fired[j].fd];
        int fd = eventLoop->fired[j].fd;
        int mask = eventLoop->fired[j].mask;

        // Callback de lecture
        if (fe->mask & mask & AE_READABLE) {
            fe->rfileProc(eventLoop, fd, fe->clientData, mask);
            processed++;
        }

        // Callback d'écriture
        if (fe->mask & mask & AE_WRITABLE) {
            fe->wfileProc(eventLoop, fd, fe->clientData, mask);
            processed++;
        }
    }

    // Traiter les événements temporels
    if (flags & AE_TIME_EVENTS)
        processed += processTimeEvents(eventLoop);

    return processed;
}
```

### Multiplexage I/O multi-plateforme

Redis supporte plusieurs backends :

```c
// ae.c
#ifdef HAVE_EVPORT
#include "ae_evport.c"      // Solaris
#else
#ifdef HAVE_EPOLL
#include "ae_epoll.c"       // Linux (le plus rapide)
#else
#ifdef HAVE_KQUEUE
#include "ae_kqueue.c"      // BSD, macOS
#else
#include "ae_select.c"      // Fallback POSIX
#endif
#endif
#endif
```

**Exemple avec epoll** :

```c
// ae_epoll.c
static int aeApiPoll(aeEventLoop *eventLoop, struct timeval *tvp) {
    aeApiState *state = eventLoop->apidata;
    int retval, numevents = 0;

    // Attendre des événements (bloquant)
    retval = epoll_wait(state->epfd, state->events, eventLoop->setsize,
                        tvp ? (tvp->tv_sec*1000 + tvp->tv_usec/1000) : -1);

    if (retval > 0) {
        numevents = retval;
        for (int j = 0; j < numevents; j++) {
            struct epoll_event *e = state->events + j;
            int mask = 0;

            if (e->events & EPOLLIN) mask |= AE_READABLE;
            if (e->events & EPOLLOUT) mask |= AE_WRITABLE;
            if (e->events & EPOLLERR) mask |= AE_WRITABLE;
            if (e->events & EPOLLHUP) mask |= AE_WRITABLE;

            eventLoop->fired[j].fd = e->data.fd;
            eventLoop->fired[j].mask = mask;
        }
    }
    return numevents;
}
```

---

## 🔬 Étape 5 : Analyse d'une commande complète

Analysons `SET` et `GET` pour comprendre le flux complet.

### Traitement d'une commande

**Flux général** :

```
Client envoie: "SET mykey hello\r\n"
       ↓
[Event loop détecte données lisibles sur le socket]
       ↓
readQueryFromClient()  (networking.c)
       ↓
processInputBuffer()   (parsing du protocole RESP)
       ↓
processCommand()       (server.c)
       ↓
Recherche "SET" dans la command table
       ↓
setCommand()           (t_string.c)
       ↓
setGenericCommand()    (logique SET)
       ↓
dbAdd(db, key, val)    (ajoute dans la base)
       ↓
addReply(c, shared.ok) (envoie "+OK\r\n" au client)
```

### La table de commandes

```c
// server.c
struct redisCommand redisCommandTable[] = {
    {"get", getCommand, 2,
     "read-only fast @string",
     0, NULL, 1, 1, 1, 0, 0, 0},

    {"set", setCommand, -3,
     "write use-memory @string",
     0, NULL, 1, 1, 1, 0, 0, 0},

    {"lpush", lpushCommand, -3,
     "write use-memory fast @list",
     0, NULL, 1, 1, 1, 0, 0, 0},

    // ... 200+ commandes
};
```

**Champs importants** :
- `name` : Nom de la commande
- `proc` : Fonction à appeler
- `arity` : Nombre d'arguments (-3 = au moins 3)
- `flags` : Métadonnées (read-only, write, etc.)

### Implémentation de SET

```c
// t_string.c
void setCommand(client *c) {
    int flags = OBJ_SET_NO_FLAGS;

    // Parser les options (EX, PX, NX, XX, etc.)
    for (int j = 3; j < c->argc; j++) {
        char *a = c->argv[j]->ptr;
        if (!strcasecmp(a, "nx")) {
            flags |= OBJ_SET_NX;
        } else if (!strcasecmp(a, "xx")) {
            flags |= OBJ_SET_XX;
        }
        // ... autres options
    }

    // Encoder la valeur (int ou string)
    c->argv[2] = tryObjectEncoding(c->argv[2]);

    // Appeler la fonction générique
    setGenericCommand(c, flags, c->argv[1], c->argv[2], expire, unit, NULL, NULL);
}

void setGenericCommand(client *c, int flags, robj *key, robj *val,
                       robj *expire, int unit, robj *ok_reply, robj *abort_reply)
{
    long long milliseconds = 0;

    // Vérifier si la clé existe déjà (pour NX/XX)
    if ((flags & OBJ_SET_NX && lookupKeyWrite(c->db, key) != NULL) ||
        (flags & OBJ_SET_XX && lookupKeyWrite(c->db, key) == NULL))
    {
        addReply(c, abort_reply ? abort_reply : shared.null[c->resp]);
        return;
    }

    // Ajouter/remplacer dans la base
    genericSetKey(c, c->db, key, val, flags & OBJ_SET_KEEPTTL, 1);
    server.dirty++;

    // Définir l'expiration si nécessaire
    if (expire) {
        setExpire(c, c->db, key, milliseconds);
    }

    // Notifier les watchers
    notifyKeyspaceEvent(NOTIFY_STRING, "set", key, c->db->id);

    // Répondre au client
    if (expire) {
        notifyKeyspaceEvent(NOTIFY_GENERIC, "expire", key, c->db->id);
    }
    addReply(c, ok_reply ? ok_reply : shared.ok);
}
```

### Implémentation de GET

```c
// t_string.c
void getCommand(client *c) {
    getGenericCommand(c);
}

int getGenericCommand(client *c) {
    robj *o;

    // Chercher la clé dans la base
    if ((o = lookupKeyReadOrReply(c, c->argv[1], shared.null[c->resp])) == NULL)
        return C_OK;  // Clé n'existe pas → réponse NULL

    // Vérifier que c'est bien une string
    if (checkType(c, o, OBJ_STRING)) return C_ERR;

    // Envoyer la valeur au client
    addReplyBulk(c, o);
    return C_OK;
}

robj *lookupKeyReadOrReply(client *c, robj *key, robj *reply) {
    robj *o = lookupKeyRead(c->db, key);
    if (!o) addReplyOrErrorObject(c, reply);
    return o;
}

robj *lookupKeyRead(redisDb *db, robj *key) {
    return lookupKeyReadWithFlags(db, key, LOOKUP_NONE);
}

robj *lookupKeyReadWithFlags(redisDb *db, robj *key, int flags) {
    robj *val;

    // Vérifier si la clé a expiré
    if (expireIfNeeded(db, key) == 1) {
        // Clé expirée et supprimée
        return NULL;
    }

    // Rechercher dans le dictionnaire
    val = lookupKey(db, key, flags);

    // Mettre à jour les statistiques LRU/LFU
    if (val) {
        if (!hasActiveChildProcess() && !(flags & LOOKUP_NOTOUCH)) {
            if (server.maxmemory_policy & MAXMEMORY_FLAG_LFU) {
                updateLFU(val);
            } else {
                val->lru = LRU_CLOCK();
            }
        }
    } else {
        server.stat_keyspace_misses++;
    }

    return val;
}
```

---

## 🎓 Étape 6 : Patterns et techniques avancées

### 6.1 Shared Objects (Flyweight pattern)

Redis préalloue des objets fréquemment utilisés.

```c
// server.c
struct sharedObjectsStruct shared;

void createSharedObjects(void) {
    shared.ok = createObject(OBJ_STRING, sdsnew("+OK\r\n"));
    shared.err = createObject(OBJ_STRING, sdsnew("-ERR\r\n"));
    shared.emptybulk = createObject(OBJ_STRING, sdsnew("$0\r\n\r\n"));
    shared.czero = createObject(OBJ_STRING, sdsnew(":0\r\n"));
    shared.cone = createObject(OBJ_STRING, sdsnew(":1\r\n"));

    // Petits entiers (-1 à 10000)
    for (int j = 0; j < OBJ_SHARED_INTEGERS; j++) {
        shared.integers[j] = createObject(OBJ_STRING, (void*)(long)j);
        shared.integers[j]->encoding = OBJ_ENCODING_INT;
        shared.integers[j]->refcount = OBJ_SHARED_REFCOUNT;
    }

    // ... autres objets partagés
}
```

**Avantage** : Économie mémoire massive. `OK` est référencé des millions de fois mais n'existe qu'une fois en mémoire.

---

### 6.2 Object Sharing et COW (Copy-on-Write)

Redis peut partager des objets entre clés.

```c
// db.c
void setKey(client *c, redisDb *db, robj *key, robj *val, int flags) {
    // Essayer de partager l'objet si c'est un entier
    if (val->encoding == OBJ_ENCODING_RAW &&
        sdsEncodedObject(val) &&
        val->refcount == 1)
    {
        val = tryObjectSharing(val);  // Partager si possible
    }

    // Ajouter dans la base
    dbAdd(db, key, val);
}
```

---

### 6.3 Allocation mémoire instrumentée

Redis wrappe toutes les allocations pour tracker la mémoire.

```c
// zmalloc.c
void *zmalloc(size_t size) {
    void *ptr = malloc(size + PREFIX_SIZE);

    if (!ptr) zmalloc_oom_handler(size);

    // Stocker la taille allouée avant le pointeur retourné
    *((size_t*)ptr) = size;
    update_zmalloc_stat_alloc(size);

    return (char*)ptr + PREFIX_SIZE;
}

void zfree(void *ptr) {
    if (ptr == NULL) return;

    // Récupérer la taille
    void *realptr = (char*)ptr - PREFIX_SIZE;
    size_t oldsize = *((size_t*)realptr);

    update_zmalloc_stat_free(oldsize);
    free(realptr);
}

size_t zmalloc_used_memory(void) {
    return atomicGet(used_memory);  // Mémoire totale utilisée
}
```

**Utilité** : Redis connaît exactement sa consommation mémoire en temps réel pour implémenter `maxmemory`.

---

### 6.4 Éviction LRU/LFU

Quand `maxmemory` est atteint, Redis évince des clés selon la politique configurée.

```c
// evict.c
int performEvictions(void) {
    if (server.maxmemory == 0) return C_OK;  // Pas de limite

    size_t mem_used = zmalloc_used_memory();
    if (mem_used <= server.maxmemory) return C_OK;  // OK

    // Boucle d'éviction
    while (mem_used > server.maxmemory) {
        int policy = server.maxmemory_policy;
        robj *keyobj;
        long long delta;

        if (policy == MAXMEMORY_ALLKEYS_LRU ||
            policy == MAXMEMORY_VOLATILE_LRU)
        {
            // Trouver la clé LRU (Least Recently Used)
            struct evictionPoolEntry *pool = EvictionPoolLRU;
            keyobj = findLRUKey(pool);
        }
        else if (policy == MAXMEMORY_ALLKEYS_LFU ||
                 policy == MAXMEMORY_VOLATILE_LFU)
        {
            // Trouver la clé LFU (Least Frequently Used)
            keyobj = findLFUKey();
        }
        // ... autres politiques (RANDOM, TTL)

        if (keyobj) {
            delta = (long long) zmalloc_used_memory();
            dbDelete(server.db + selected_db, keyobj);  // Supprimer
            delta -= (long long) zmalloc_used_memory();
            mem_used -= delta;
        }
    }

    return C_OK;
}
```

**LRU approximatif** : Redis ne maintient pas une liste LRU complète (trop coûteux). Il échantillonne des clés aléatoirement et évince la plus vieille.

---

### 6.5 Persistance : RDB et AOF

**RDB (Redis Database)** : Snapshot binaire complet de la base.

```c
// rdb.c (simplifié)
int rdbSave(char *filename) {
    FILE *fp = fopen(filename, "w");

    // En-tête
    rdbWriteRaw(fp, "REDIS", 5);
    rdbSaveType(fp, RDB_VERSION);

    // Pour chaque base de données
    for (int j = 0; j < server.dbnum; j++) {
        redisDb *db = server.db + j;
        dict *d = db->dict;

        if (dictSize(d) == 0) continue;

        // Écrire le sélecteur de DB
        rdbSaveType(fp, RDB_OPCODE_SELECTDB);
        rdbSaveLen(fp, j);

        // Sauver toutes les clés
        dictIterator *di = dictGetIterator(d);
        dictEntry *de;
        while ((de = dictNext(di)) != NULL) {
            sds key = dictGetKey(de);
            robj *val = dictGetVal(de);

            // Sauver la clé et la valeur
            rdbSaveKeyValuePair(fp, key, val, expiretime);
        }
        dictReleaseIterator(di);
    }

    // EOF marker
    rdbSaveType(fp, RDB_OPCODE_EOF);
    fclose(fp);
    return C_OK;
}
```

**AOF (Append-Only File)** : Log de toutes les commandes d'écriture.

```c
// aof.c
void feedAppendOnlyFile(int dictid, robj **argv, int argc) {
    sds buf = sdsempty();

    // Sélection de la DB si nécessaire
    if (server.aof_selected_db != dictid) {
        buf = sdscatprintf(buf, "*2\r\n$6\r\nSELECT\r\n$%d\r\n%d\r\n",
                          (int)sdslen(server.db[dictid].id),
                          dictid);
        server.aof_selected_db = dictid;
    }

    // Écrire la commande au format RESP
    buf = catAppendOnlyGenericCommand(buf, argc, argv);

    // Ajouter au buffer AOF
    server.aof_buf = sdscatlen(server.aof_buf, buf, sdslen(buf));
    sdsfree(buf);
}
```

---

## 🎯 Étape 7 : Ce que nous apprenons de Redis

### 7.1 Structures de données custom

**Leçon** : N'utilisez pas toujours les structures standard. Redis implémente :
- SDS au lieu de `char *`
- Dict personnalisé avec rehashing incrémental
- Skiplist pour les sorted sets
- Ziplist/listpack pour l'optimisation mémoire

**À appliquer** : Identifiez vos besoins spécifiques et créez des structures adaptées.

---

### 7.2 Single-threaded avec event loop

**Leçon** : Un seul thread peut gérer des milliers de clients avec un event loop bien fait.

**Avantages** :
- ✅ Pas de locks/mutex
- ✅ Pas de race conditions
- ✅ Code plus simple
- ✅ Performance prévisible

**Quand utiliser** : Workloads I/O-bound où le CPU n'est pas le goulot.

---

### 7.3 Optimisations mémoire agressives

Redis optimise chaque byte :
- Objets partagés (flyweight)
- Encodages multiples (int vs string)
- Structures compactes (ziplist pour petites listes)
- Comptage de références

**À appliquer** : Pour des systèmes en mémoire, chaque optimisation compte.

---

### 7.4 Code lisible et commenté

Redis a un code **exceptionnellement lisible** :

```c
// dict.c
/* This is the initial size of every hash table */
#define DICT_HT_INITIAL_SIZE     4

/* Our hash table capability is a power of two */
static unsigned long _dictNextPower(unsigned long size) {
    unsigned long i = DICT_HT_INITIAL_SIZE;

    if (size >= LONG_MAX) return LONG_MAX + 1LU;
    while (1) {
        if (i >= size) return i;
        i *= 2;
    }
}
```

**Pourquoi ?** Parce que Salvatore Sanfilippo (antirez) croit que **le code doit être lisible par les humains**.

---

### 7.5 Tests exhaustifs

Redis a des milliers de tests dans `tests/`.

```bash
cd redis/tests
./test-redis.tcl
```

Tests unitaires ET tests d'intégration :
- Tests de chaque commande
- Tests de edge cases
- Tests de charge
- Tests de réplication
- Tests de clustering

---

### 7.6 Profiling et performance

Redis inclut des outils de profiling intégrés.

```bash
# Dans redis-cli
INFO stats
INFO memory

# Profiling des commandes lentes
CONFIG SET slowlog-log-slower-than 10000
SLOWLOG GET 10

# Latency monitoring
LATENCY DOCTOR
```

**Code** :

```c
// server.c
void call(client *c, int flags) {
    long long start = ustime();

    // Exécuter la commande
    c->cmd->proc(c);

    // Mesurer la latence
    long long duration = ustime() - start;

    // Logger si trop lent
    if (duration > server.slowlog_log_slower_than)
        slowlogPushEntryIfNeeded(c, c->argv, c->argc, duration);
}
```

---

## 🔧 Étape 8 : Outils pour explorer Redis

### 8.1 Compiler Redis

```bash
cd redis
make

# Avec symboles de débogage
make noopt  # Équivalent de CFLAGS="-g -O0"

# Lancer Redis
src/redis-server

# Client
src/redis-cli
```

### 8.2 Déboguer avec GDB

```bash
gdb src/redis-server
(gdb) break setCommand
(gdb) run

# Dans un autre terminal
redis-cli SET mykey hello

# Retour dans GDB
(gdb) backtrace
(gdb) print c->argv[1]->ptr
(gdb) continue
```

### 8.3 Analyser la mémoire

```bash
# Dans redis-cli
MEMORY USAGE mykey
MEMORY DOCTOR
MEMORY STATS
```

### 8.4 Lire le code source

**Ordre recommandé** :

1. `server.c` - main() et initialisation
2. `sds.c` - Comprendre SDS
3. `dict.c` - Hash table
4. `object.c` - Objets Redis
5. `networking.c` - Gestion réseau
6. `ae.c` - Event loop
7. `t_string.c` - Commandes String
8. `db.c` - Opérations sur la base

---

## 📚 Lectures approfondies

### Code source commenté

Redis a **d'excellents commentaires** dans le code. Lisez :

```c
// dict.c
/* -------------------------- private prototypes ---------------------------- */

static int _dictExpandIfNeeded(dict *ht);
static unsigned long _dictNextPower(unsigned long size);
static long _dictKeyIndex(dict *ht, const void *key, uint64_t hash, dictEntry **existing);
static int _dictInit(dict *ht, dictType *type, void *privDataPtr);

/* -------------------------- hash functions -------------------------------- */

/* Generic hash function (djb2 by Dan Bernstein).
 * This is an algorithm known to be fast and effective.
 * A different version is djb2a where the + is replaced by ^. */
uint64_t dictGenHashFunction(const void *key, int len) {
    /* 'M', start with ... */
    uint32_t hash = 5381;

    for (int i = 0; i < len; i++)
        hash = ((hash << 5) + hash) + key[i]; /* hash * 33 + c */

    return hash;
}
```

---

### Documentation officielle

- **Site officiel** : https://redis.io/
- **Documentation** : https://redis.io/docs/
- **Protocole RESP** : https://redis.io/docs/reference/protocol-spec/
- **Persistance** : https://redis.io/docs/management/persistence/

### Articles du créateur

Antirez (Salvatore Sanfilippo) a écrit d'excellents articles :
- "Redis Manifesto" : Philosophie de design
- "A few things about Redis development" : Processus de développement
- Blog : http://antirez.com/

### Livres recommandés

- **"Redis in Action"** - Josiah Carlson
- **"The Little Redis Book"** - Karl Seguin (gratuit)

---

## 💡 Exercice mental : Concevoir une fonctionnalité

**Scénario** : Vous devez ajouter une commande `GETMULTIPLE key1 key2 key3` qui retourne les valeurs de plusieurs clés.

**Questions** :
1. Dans quel fichier ajouteriez-vous `getmultipleCommand()` ?
2. Comment parseriez-vous les arguments ?
3. Comment optimiseriez-vous pour éviter plusieurs lookups ?
4. Comment géreriez-vous les clés qui n'existent pas ?

**Réponse suggérée** :

1. Fichier `t_string.c` (commandes String)

2. Parser les arguments :
```c
void getmultipleCommand(client *c) {
    if (c->argc < 2) {
        addReplyError(c, "wrong number of arguments");
        return;
    }

    addReplyArrayLen(c, c->argc - 1);  // Nombre de réponses

    for (int i = 1; i < c->argc; i++) {
        robj *o = lookupKeyRead(c->db, c->argv[i]);
        if (o == NULL) {
            addReplyNull(c);
        } else {
            if (o->type != OBJ_STRING) {
                addReplyError(c, "WRONGTYPE");
                return;
            }
            addReplyBulk(c, o);
        }
    }
}
```

3. L'optimisation est déjà là : un seul appel réseau, plusieurs lookups locaux.

4. Retourner `NULL` pour les clés inexistantes (déjà fait ci-dessus).

---

## 🎯 Récapitulatif : Leçons de Redis

| Principe | Implémentation Redis | À retenir |
|----------|---------------------|-----------|
| **Structures custom** | SDS, dict, skiplist | Créez des types adaptés à vos besoins |
| **Single-threaded** | Event loop (ae.c) | Simple et performant pour I/O-bound |
| **Optimisation mémoire** | Shared objects, encodings | Chaque byte compte en mémoire |
| **Rehashing incrémental** | Deux hash tables | Évitez les pauses longues |
| **Abstraction réseau** | anet.c | Isolation du code plateforme |
| **Instrumentation** | zmalloc | Tracker toutes les allocations |
| **Code lisible** | Commentaires partout | Le code est pour les humains |
| **Tests exhaustifs** | tests/ (Tcl) | Testez chaque edge case |

---

## 🔗 Liens avec les autres sections

- **Section 33.1** : Méthodologie appliquée ici
- **Section 33.2** : Git utilise aussi object pooling
- **Section 33.4** : Nginx a une architecture événementielle similaire
- **Module 3** : Gestion mémoire (malloc, free, leaks)
- **Module 4** : Structures de données (listes, hash tables)
- **Module 6** : Event loop, sockets, I/O multiplexing
- **Module 7** : Optimisations et techniques avancées

---

## 🚀 Pour aller plus loin

### Contribuer à Redis

1. **Clonez le dépôt** : `git clone https://github.com/redis/redis.git`
2. **Compilez** : `make`
3. **Testez** : `make test`
4. **Lisez CONTRIBUTING.md**
5. **Trouvez un issue** : https://github.com/redis/redis/issues

### Projets similaires à étudier

- **KeyDB** : Fork multi-threaded de Redis
- **Dragonfly** : Alternative en C++ avec architecture moderne
- **Memcached** : Plus simple, bon pour apprendre les bases

### Implémenter votre propre Redis

**Projet d'apprentissage** : Créez un "Mini-Redis" avec :
- Serveur TCP basique
- Event loop simple (select)
- Commandes GET/SET
- Dict simple
- Persistance RDB basique

C'est l'un des meilleurs projets pour apprendre le C système !

---

## 📝 Points clés à retenir

- ✅ Redis démontre qu'un design **simple** (single-threaded) peut être **extrêmement performant**
- ✅ Les **structures de données custom** (SDS, dict, skiplist) sont au cœur du succès de Redis
- ✅ L'**event loop** permet de gérer des milliers de clients avec un seul thread
- ✅ Le **rehashing incrémental** évite les pauses qui bloqueraient le serveur
- ✅ Les **optimisations mémoire** (shared objects, encodings multiples) sont critiques
- ✅ Le **code lisible** avec de bons commentaires facilite grandement la maintenance
- ✅ L'**instrumentation** (zmalloc, slowlog) permet de debugger et optimiser en production

**Prochaine étape** : Étudier Nginx (33.4) pour voir une architecture événementielle encore plus poussée avec du multi-processing, ou revenir à Git (33.2) pour comparer les approches.

---

**💡 Conseil final** : Redis est parfait pour apprendre parce que le code est **intentionnellement lisible**. Clonez le dépôt, compilez-le, ajoutez des `printf()` dans `setCommand()` et `getCommand()`, et observez le flux en temps réel. Ensuite, essayez d'implémenter votre propre commande simple !

Le meilleur moyen d'apprendre est de **modifier le code et de voir ce qui casse**. Redis est assez robuste pour résister à vos expérimentations. 🚀

⏭️ [Étude de cas : Nginx](/33-analyse-code-opensource/04-etude-cas-nginx.md)
