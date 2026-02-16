/* ============================================================================
   Section 33.4 : Etude de cas Nginx
   Description : Memory pool inspire de Nginx (ngx_pool_t)
   Fichier source : 04-etude-cas-nginx.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ============================================ */
/* Memory Pool (Nginx)                          */
/* ============================================ */

/*
 * Nginx utilise des memory pools pour eviter les appels
 * frequents a malloc/free. Le principe :
 * - Allouer un gros bloc une seule fois (malloc)
 * - Distribuer des morceaux de ce bloc (bump pointer)
 * - Tout liberer en une seule fois (destroy pool)
 *
 * Avantages :
 * - Allocation en O(1) : juste avancer un pointeur
 * - Pas de fragmentation
 * - Liberation en O(1) : free le bloc entier
 * - Pas de fuites memoire possibles
 */

/* Un bloc de memoire dans la pool */
typedef struct pool_block {
    struct pool_block *next;    /* Bloc suivant (si pool agrandie) */
    uint8_t *data;              /* Debut des donnees */
    uint8_t *current;           /* Position courante (bump pointer) */
    uint8_t *end;               /* Fin du bloc */
} pool_block_t;

/* La pool elle-meme */
typedef struct {
    pool_block_t *head;         /* Premier bloc */
    pool_block_t *current;      /* Bloc courant pour les allocations */
    size_t block_size;          /* Taille de chaque bloc */
    size_t total_alloc;         /* Total alloue (statistique) */
    int num_blocks;             /* Nombre de blocs (statistique) */
} pool_t;

/* ============================================ */
/* Operations sur la pool                       */
/* ============================================ */

/* Creer un nouveau bloc */
static pool_block_t *pool_block_new(size_t size)
{
    pool_block_t *block = malloc(sizeof(pool_block_t) + size);
    if (!block) return NULL;

    block->next = NULL;
    block->data = (uint8_t *)(block + 1);
    block->current = block->data;
    block->end = block->data + size;

    return block;
}

/* Creer une pool */
static pool_t *pool_create(size_t block_size)
{
    pool_t *pool = malloc(sizeof(pool_t));
    if (!pool) return NULL;

    pool_block_t *block = pool_block_new(block_size);
    if (!block) {
        free(pool);
        return NULL;
    }

    pool->head = block;
    pool->current = block;
    pool->block_size = block_size;
    pool->total_alloc = 0;
    pool->num_blocks = 1;

    return pool;
}

/* Allouer depuis la pool (ultra-rapide : bump pointer) */
static void *pool_alloc(pool_t *pool, size_t size)
{
    /* Aligner sur 8 bytes */
    size = (size + 7) & ~(size_t)7;

    pool_block_t *block = pool->current;

    /* Verifier si le bloc courant a assez de place */
    if (block->current + size <= block->end) {
        void *ptr = block->current;
        block->current += size;
        pool->total_alloc += size;
        return ptr;
    }

    /* Pas assez de place : allouer un nouveau bloc */
    size_t new_size = pool->block_size;
    if (size > new_size) {
        new_size = size;  /* Allocation plus grande que le bloc */
    }

    pool_block_t *new_block = pool_block_new(new_size);
    if (!new_block) return NULL;

    /* Chainer le nouveau bloc */
    block->next = new_block;
    pool->current = new_block;
    pool->num_blocks++;

    void *ptr = new_block->current;
    new_block->current += size;
    pool->total_alloc += size;

    return ptr;
}

/* Allouer et initialiser a zero */
static void *pool_calloc(pool_t *pool, size_t count, size_t size)
{
    size_t total = count * size;
    void *ptr = pool_alloc(pool, total);
    if (ptr) {
        memset(ptr, 0, total);
    }
    return ptr;
}

/* Copier une chaine dans la pool */
static char *pool_strdup(pool_t *pool, const char *str)
{
    size_t len = strlen(str) + 1;
    char *copy = pool_alloc(pool, len);
    if (copy) {
        memcpy(copy, str, len);
    }
    return copy;
}

/* Detruire la pool (libere TOUT en une seule fois) */
static void pool_destroy(pool_t *pool)
{
    if (!pool) return;

    pool_block_t *block = pool->head;
    while (block) {
        pool_block_t *next = block->next;
        free(block);
        block = next;
    }
    free(pool);
}

/* Statistiques */
static void pool_stats(const pool_t *pool)
{
    printf("  Blocs: %d, Taille bloc: %zu, Total alloue: %zu bytes\n",
           pool->num_blocks, pool->block_size, pool->total_alloc);

    size_t total_capacity = 0;
    pool_block_t *block = pool->head;
    int i = 0;
    while (block) {
        size_t cap = (size_t)(block->end - block->data);
        size_t used = (size_t)(block->current - block->data);
        printf("    Bloc %d: %zu/%zu bytes utilises (%.0f%%)\n",
               i++, used, cap, cap > 0 ? 100.0 * (double)used / (double)cap : 0.0);
        total_capacity += cap;
        block = block->next;
    }
    printf("  Capacite totale: %zu bytes, Utilisation: %.0f%%\n",
           total_capacity,
           total_capacity > 0 ? 100.0 * (double)pool->total_alloc / (double)total_capacity : 0.0);
}

/* ============================================ */
/* Demonstration                                */
/* ============================================ */

/* Simuler une requete HTTP (comme Nginx) */
typedef struct {
    char *method;
    char *uri;
    char *host;
    int status;
} http_request_t;

int main(void)
{
    printf("=== Memory Pool (Nginx) ===\n\n");

    /* Creer une pool de 1024 bytes */
    pool_t *pool = pool_create(1024);
    if (!pool) {
        fprintf(stderr, "Erreur: creation pool echouee\n");
        return EXIT_FAILURE;
    }

    /* Simuler le traitement d'une requete HTTP */
    printf("--- Simulation requete HTTP ---\n");
    printf("(Nginx alloue tout dans une pool par requete)\n\n");

    http_request_t *req = pool_calloc(pool, 1, sizeof(http_request_t));
    req->method = pool_strdup(pool, "GET");
    req->uri = pool_strdup(pool, "/index.html");
    req->host = pool_strdup(pool, "www.example.com");
    req->status = 200;

    printf("  Requete: %s %s\n", req->method, req->uri);
    printf("  Host: %s\n", req->host);
    printf("  Status: %d\n", req->status);

    /* Allouer des headers supplementaires */
    printf("\n--- Allocation de headers ---\n");
    const char *headers[] = {
        "Content-Type: text/html",
        "Content-Length: 1234",
        "Connection: keep-alive",
        "Cache-Control: max-age=3600",
        "X-Request-Id: abc123def456",
    };
    int num_headers = (int)(sizeof(headers) / sizeof(headers[0]));

    char **header_copies = pool_alloc(pool, (size_t)num_headers * sizeof(char *));
    for (int i = 0; i < num_headers; i++) {
        header_copies[i] = pool_strdup(pool, headers[i]);
        printf("  Header: %s\n", header_copies[i]);
    }

    /* Statistiques */
    printf("\n--- Statistiques pool ---\n");
    pool_stats(pool);

    /* Comparer avec malloc/free */
    printf("\n--- Comparaison malloc vs pool ---\n");
    printf("  malloc: 1 appel systeme par allocation\n");
    printf("  pool  : 1 seul malloc initial, ensuite bump pointer\n\n");
    printf("  Allocations effectuees:\n");
    printf("    - 1 http_request_t (%zu bytes)\n", sizeof(http_request_t));
    printf("    - 3 chaines (method, uri, host)\n");
    printf("    - 1 tableau de pointeurs (%zu bytes)\n",
           (size_t)num_headers * sizeof(char *));
    printf("    - %d chaines (headers)\n", num_headers);
    printf("    Total: %d allocations, 0 appels a free() individuels\n",
           3 + 1 + 1 + num_headers);

    /* Destruction : tout libere en une seule fois */
    printf("\n--- Destruction ---\n");
    printf("  pool_destroy() : libere TOUT en une seule operation\n");
    printf("  (Nginx fait ca a la fin de chaque requete HTTP)\n");
    pool_destroy(pool);

    printf("\n--- Avantages du memory pool ---\n");
    printf("1. Allocation en O(1) : avancer un pointeur\n");
    printf("2. Pas de fragmentation memoire\n");
    printf("3. Liberation instantanee : free un seul bloc\n");
    printf("4. Impossible d'avoir des fuites memoire\n");
    printf("5. Cache-friendly (allocations contigues)\n");

    return EXIT_SUCCESS;
}
