/* ============================================================================
   Section 33.2 : Object Pooling (Git/Redis)
   Description : Pattern object pool inspire de Git (cache) et Redis (shared int)
   Fichier source : README.md, 02-etude-cas-git.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* ============================================ */
/* Object Pool (Git / Redis)                    */
/* ============================================ */

/*
 * Pattern utilise dans Git et Redis pour reutiliser des objets
 * au lieu de les recreer a chaque fois.
 *
 * Git : cache d'objets (commits, trees, blobs) deja parses.
 *       lookup_commit() retourne l'objet existant si deja charge.
 *
 * Redis : shared.integers[0..9999] pre-alloues au demarrage.
 *         Toutes les cles qui valent un entier 0-9999 partagent
 *         le meme objet en memoire.
 *
 * Avantages :
 * - Economie de memoire (un seul objet partage)
 * - Rapidite (pas d'allocation/liberation)
 * - Coherence (une seule instance)
 */

/* ============================================ */
/* Exemple 1 : Pool d'entiers (Redis-like)      */
/* ============================================ */

#define SHARED_INT_MAX 256

typedef struct {
    int value;
    int refcount;
} shared_int_t;

typedef struct {
    shared_int_t pool[SHARED_INT_MAX];
    int hits;
    int misses;
} int_pool_t;

static int_pool_t *int_pool_create(void)
{
    int_pool_t *pool = malloc(sizeof(int_pool_t));
    if (!pool) return NULL;

    for (int i = 0; i < SHARED_INT_MAX; i++) {
        pool->pool[i].value = i;
        pool->pool[i].refcount = 0;
    }
    pool->hits = 0;
    pool->misses = 0;
    return pool;
}

/* Obtenir un entier partage (ne pas free!) */
static shared_int_t *int_pool_get(int_pool_t *pool, int value)
{
    if (value >= 0 && value < SHARED_INT_MAX) {
        pool->pool[value].refcount++;
        pool->hits++;
        return &pool->pool[value];
    }
    pool->misses++;
    return NULL;  /* Hors pool, faudrait allouer */
}

static void int_pool_release(int_pool_t *pool, int value)
{
    if (value >= 0 && value < SHARED_INT_MAX) {
        if (pool->pool[value].refcount > 0) {
            pool->pool[value].refcount--;
        }
    }
}

static void int_pool_destroy(int_pool_t *pool)
{
    free(pool);
}

/* ============================================ */
/* Exemple 2 : Object cache (Git-like)          */
/* ============================================ */

/* Simuler les types d'objets Git */
typedef enum {
    OBJ_BLOB,
    OBJ_TREE,
    OBJ_COMMIT,
    OBJ_TAG
} obj_type_t;

static const char *obj_type_name(obj_type_t type)
{
    switch (type) {
    case OBJ_BLOB:   return "blob";
    case OBJ_TREE:   return "tree";
    case OBJ_COMMIT: return "commit";
    case OBJ_TAG:    return "tag";
    }
    return "unknown";
}

/* Un objet dans le cache */
typedef struct obj {
    char hash[41];          /* SHA-1 hex (40 chars + \0) */
    obj_type_t type;
    bool parsed;            /* Lazy loading : parse seulement si necessaire */
    char *data;             /* Donnees (chargees a la demande) */
    struct obj *next;       /* Chaining dans la hash table */
} obj_t;

/* Cache d'objets */
#define OBJ_CACHE_SIZE 64

typedef struct {
    obj_t *buckets[OBJ_CACHE_SIZE];
    int total;
    int cache_hits;
    int cache_misses;
} obj_cache_t;

static obj_cache_t *obj_cache_create(void)
{
    obj_cache_t *cache = calloc(1, sizeof(obj_cache_t));
    return cache;
}

static unsigned int hash_string(const char *s)
{
    unsigned int h = 0;
    while (*s) {
        h = h * 31 + (unsigned char)*s++;
    }
    return h % OBJ_CACHE_SIZE;
}

/* Lookup : retourne l'objet existant ou en cree un nouveau */
static obj_t *obj_cache_lookup(obj_cache_t *cache, const char *hash,
                               obj_type_t type)
{
    unsigned int idx = hash_string(hash);

    /* Chercher dans le cache */
    obj_t *obj = cache->buckets[idx];
    while (obj) {
        if (strcmp(obj->hash, hash) == 0) {
            cache->cache_hits++;
            return obj;  /* Retourne l'objet existant */
        }
        obj = obj->next;
    }

    /* Pas trouve : creer un nouvel objet (non parse) */
    cache->cache_misses++;
    obj = calloc(1, sizeof(obj_t));
    if (!obj) return NULL;

    strncpy(obj->hash, hash, 40);
    obj->hash[40] = '\0';
    obj->type = type;
    obj->parsed = false;
    obj->data = NULL;

    /* Inserer dans le cache */
    obj->next = cache->buckets[idx];
    cache->buckets[idx] = obj;
    cache->total++;

    return obj;
}

/* Parse un objet (lazy loading - seulement quand necessaire) */
static void obj_parse(obj_t *obj)
{
    if (obj->parsed) return;  /* Deja parse */

    /* Simuler le chargement depuis le disque */
    char buf[128];
    snprintf(buf, sizeof(buf), "Donnees de %s %.7s...",
             obj_type_name(obj->type), obj->hash);
    obj->data = strdup(buf);
    obj->parsed = true;
}

static void obj_cache_destroy(obj_cache_t *cache)
{
    for (int i = 0; i < OBJ_CACHE_SIZE; i++) {
        obj_t *obj = cache->buckets[i];
        while (obj) {
            obj_t *next = obj->next;
            free(obj->data);
            free(obj);
            obj = next;
        }
    }
    free(cache);
}

/* ============================================ */
/* Demonstration                                */
/* ============================================ */

int main(void)
{
    printf("=== Object Pool / Cache (Git & Redis) ===\n\n");

    /* --- Pool d'entiers (Redis) --- */
    printf("--- Pool d'entiers partages (Redis) ---\n");
    printf("Redis pre-alloue les entiers 0-9999 au demarrage.\n");
    printf("Simulation avec 0-%d:\n\n", SHARED_INT_MAX - 1);

    int_pool_t *ipool = int_pool_create();

    /* Simuler plusieurs cles ayant la meme valeur entiere */
    printf("  SET user:age 25 -> ");
    shared_int_t *a1 = int_pool_get(ipool, 25);
    printf("objet partage @%p (refcount=%d)\n", (void *)a1, a1->refcount);

    printf("  SET item:qty 25 -> ");
    shared_int_t *a2 = int_pool_get(ipool, 25);
    printf("objet partage @%p (refcount=%d)\n", (void *)a2, a2->refcount);

    printf("  Meme objet? %s (economie memoire!)\n\n",
           a1 == a2 ? "OUI" : "NON");

    printf("  SET score 42 -> ");
    shared_int_t *b = int_pool_get(ipool, 42);
    printf("objet @%p (refcount=%d)\n", (void *)b, b->refcount);

    printf("  SET big 1000 -> ");
    shared_int_t *c = int_pool_get(ipool, 1000);
    printf("%s (hors pool, allocation classique)\n\n",
           c ? "objet partage" : "NULL");

    int_pool_release(ipool, 25);
    int_pool_release(ipool, 25);
    int_pool_release(ipool, 42);

    printf("  Stats pool: hits=%d, misses=%d\n",
           ipool->hits, ipool->misses);
    int_pool_destroy(ipool);

    /* --- Object cache (Git) --- */
    printf("\n--- Object Cache avec Lazy Loading (Git) ---\n");
    printf("Git ne charge les objets qu'a la demande.\n");
    printf("lookup_commit() retourne le cache si deja charge.\n\n");

    obj_cache_t *cache = obj_cache_create();

    /* Simuler des lookups Git */
    const char *hashes[] = {
        "abcd1234abcd1234abcd1234abcd1234abcd1234",
        "ef567890ef567890ef567890ef567890ef567890",
        "abcd1234abcd1234abcd1234abcd1234abcd1234",  /* Doublon! */
        "1111aaaa2222bbbb3333cccc4444dddd5555eeee",
    };
    obj_type_t types[] = { OBJ_COMMIT, OBJ_TREE, OBJ_COMMIT, OBJ_BLOB };

    for (int i = 0; i < 4; i++) {
        printf("  lookup_object(%.7s..., %s) -> ",
               hashes[i], obj_type_name(types[i]));
        obj_t *obj = obj_cache_lookup(cache, hashes[i], types[i]);
        printf("@%p (parsed=%s)\n",
               (void *)obj, obj->parsed ? "oui" : "non");
    }

    /* Lazy loading : parser seulement quand necessaire */
    printf("\n  Lazy loading:\n");
    obj_t *commit = obj_cache_lookup(cache, hashes[0], OBJ_COMMIT);
    printf("  Avant parse: data=%s\n",
           commit->data ? commit->data : "(null)");

    obj_parse(commit);
    printf("  Apres parse: data=\"%s\"\n", commit->data);

    obj_parse(commit);  /* Deuxieme appel: no-op */
    printf("  Re-parse: data=\"%s\" (pas de double chargement)\n",
           commit->data);

    printf("\n  Stats cache: total=%d, hits=%d, misses=%d\n",
           cache->total, cache->cache_hits, cache->cache_misses);

    obj_cache_destroy(cache);

    /* Resume */
    printf("\n--- Avantages de l'Object Pooling ---\n");
    printf("1. Economie memoire : objets partages entre utilisateurs\n");
    printf("2. Rapidite : pas d'allocation/liberation repetee\n");
    printf("3. Lazy loading : charger seulement quand necessaire\n");
    printf("4. Cache : eviter de recharger les memes donnees\n");
    printf("5. Pattern utilise dans Git, Redis, JVM, Python...\n");

    return EXIT_SUCCESS;
}
