/* ============================================================================
   Section 33.3 : Etude de cas Redis
   Description : Hash table avec rehashing incremental inspire de Redis dict
   Fichier source : 03-etude-cas-redis.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/* ============================================ */
/* Hash Table avec rehashing (Redis dict)       */
/* ============================================ */

/*
 * Redis utilise deux tables de hachage (ht[0] et ht[1]).
 * Quand la table est trop pleine, le rehashing se fait
 * incrementalement : un peu a chaque operation, pas tout d'un coup.
 * Cela evite les latences dues a un gros rehash.
 */

/* Entree dans la table */
typedef struct dict_entry {
    char *key;
    char *value;
    struct dict_entry *next;  /* Chaining pour les collisions */
} dict_entry_t;

/* Une table de hachage */
typedef struct {
    dict_entry_t **table;
    size_t size;       /* Nombre de buckets */
    size_t used;       /* Nombre d'entrees */
    size_t sizemask;   /* size - 1 (pour modulo rapide) */
} dict_ht_t;

/* Le dictionnaire avec deux tables pour le rehashing */
typedef struct {
    dict_ht_t ht[2];        /* ht[0] = table principale, ht[1] = rehash */
    int rehash_idx;          /* -1 si pas de rehash en cours */
    size_t total_ops;        /* Statistique: operations totales */
    size_t rehash_moves;     /* Statistique: entrees deplacees */
} dict_t;

/* ============================================ */
/* Fonction de hachage (djb2 de Dan Bernstein)  */
/* ============================================ */

static uint32_t dict_hash(const char *key)
{
    uint32_t hash = 5381;
    int c;
    while ((c = (unsigned char)*key++) != 0) {
        hash = ((hash << 5) + hash) + (uint32_t)c;  /* hash * 33 + c */
    }
    return hash;
}

/* ============================================ */
/* Operations de base                           */
/* ============================================ */

/* Initialiser une table */
static int ht_init(dict_ht_t *ht, size_t size)
{
    ht->table = calloc(size, sizeof(dict_entry_t *));
    if (!ht->table) return -1;
    ht->size = size;
    ht->used = 0;
    ht->sizemask = size - 1;
    return 0;
}

/* Liberer une table */
static void ht_free(dict_ht_t *ht)
{
    for (size_t i = 0; i < ht->size; i++) {
        dict_entry_t *entry = ht->table[i];
        while (entry) {
            dict_entry_t *next = entry->next;
            free(entry->key);
            free(entry->value);
            free(entry);
            entry = next;
        }
    }
    free(ht->table);
    ht->table = NULL;
    ht->size = 0;
    ht->used = 0;
}

/* Creer un dictionnaire */
static dict_t *dict_create(void)
{
    dict_t *d = calloc(1, sizeof(dict_t));
    if (!d) return NULL;

    if (ht_init(&d->ht[0], 4) < 0) {
        free(d);
        return NULL;
    }
    d->rehash_idx = -1;
    return d;
}

/* Detruire un dictionnaire */
static void dict_destroy(dict_t *d)
{
    if (!d) return;
    ht_free(&d->ht[0]);
    if (d->ht[1].table) ht_free(&d->ht[1]);
    free(d);
}

/* ============================================ */
/* Rehashing incremental                        */
/* ============================================ */

/* Demarrer le rehashing (doubler la taille) */
static int dict_start_rehash(dict_t *d)
{
    if (d->rehash_idx != -1) return 0;  /* Deja en cours */

    size_t new_size = d->ht[0].size * 2;
    printf("    [rehash] Debut: %zu -> %zu buckets\n",
           d->ht[0].size, new_size);

    if (ht_init(&d->ht[1], new_size) < 0) return -1;
    d->rehash_idx = 0;
    return 0;
}

/* Faire un pas de rehashing (deplacer 1 bucket) */
static void dict_rehash_step(dict_t *d)
{
    if (d->rehash_idx == -1) return;

    /* Trouver le prochain bucket non-vide */
    while (d->rehash_idx < (int)d->ht[0].size &&
           d->ht[0].table[d->rehash_idx] == NULL) {
        d->rehash_idx++;
    }

    if (d->rehash_idx >= (int)d->ht[0].size) {
        /* Rehash termine */
        printf("    [rehash] Termine! %zu entrees deplacees\n",
               d->rehash_moves);
        ht_free(&d->ht[0]);
        d->ht[0] = d->ht[1];
        memset(&d->ht[1], 0, sizeof(dict_ht_t));
        d->rehash_idx = -1;
        d->rehash_moves = 0;
        return;
    }

    /* Deplacer toutes les entrees du bucket */
    dict_entry_t *entry = d->ht[0].table[d->rehash_idx];
    while (entry) {
        dict_entry_t *next = entry->next;

        /* Recalculer le hash pour la nouvelle table */
        uint32_t h = dict_hash(entry->key) & d->ht[1].sizemask;
        entry->next = d->ht[1].table[h];
        d->ht[1].table[h] = entry;
        d->ht[1].used++;
        d->ht[0].used--;
        d->rehash_moves++;

        entry = next;
    }
    d->ht[0].table[d->rehash_idx] = NULL;
    d->rehash_idx++;
}

/* ============================================ */
/* Operations dictionnaire                      */
/* ============================================ */

/* Verifier si rehash necessaire */
static void dict_check_rehash(dict_t *d)
{
    if (d->rehash_idx != -1) return;

    /* Ratio d'utilisation > 100% : demarrer le rehash */
    if (d->ht[0].used >= d->ht[0].size) {
        dict_start_rehash(d);
    }
}

/* Ajouter ou mettre a jour */
static int dict_set(dict_t *d, const char *key, const char *value)
{
    d->total_ops++;

    /* Rehash incremental a chaque operation */
    if (d->rehash_idx != -1) {
        dict_rehash_step(d);
    }

    /* Chercher dans quelle table inserer */
    for (int t = 0; t <= 1; t++) {
        if (t == 1 && d->rehash_idx == -1) break;

        dict_ht_t *ht = &d->ht[t];
        if (!ht->table) continue;

        uint32_t h = dict_hash(key) & ht->sizemask;
        dict_entry_t *entry = ht->table[h];

        /* Chercher si la cle existe deja */
        while (entry) {
            if (strcmp(entry->key, key) == 0) {
                free(entry->value);
                entry->value = strdup(value);
                return 0;
            }
            entry = entry->next;
        }
    }

    /* Inserer dans ht[1] si rehash en cours, sinon ht[0] */
    int idx = (d->rehash_idx != -1) ? 1 : 0;
    dict_ht_t *ht = &d->ht[idx];

    uint32_t h = dict_hash(key) & ht->sizemask;
    dict_entry_t *new_entry = malloc(sizeof(dict_entry_t));
    if (!new_entry) return -1;

    new_entry->key = strdup(key);
    new_entry->value = strdup(value);
    new_entry->next = ht->table[h];
    ht->table[h] = new_entry;
    ht->used++;

    /* Verifier si rehash necessaire */
    dict_check_rehash(d);

    return 0;
}

/* Rechercher */
static const char *dict_get(dict_t *d, const char *key)
{
    d->total_ops++;

    /* Rehash incremental */
    if (d->rehash_idx != -1) {
        dict_rehash_step(d);
    }

    /* Chercher dans les deux tables */
    for (int t = 0; t <= 1; t++) {
        if (t == 1 && d->rehash_idx == -1) break;

        dict_ht_t *ht = &d->ht[t];
        if (!ht->table || ht->size == 0) continue;

        uint32_t h = dict_hash(key) & ht->sizemask;
        dict_entry_t *entry = ht->table[h];

        while (entry) {
            if (strcmp(entry->key, key) == 0) {
                return entry->value;
            }
            entry = entry->next;
        }
    }

    return NULL;
}

/* Supprimer */
static int dict_delete(dict_t *d, const char *key)
{
    d->total_ops++;

    if (d->rehash_idx != -1) {
        dict_rehash_step(d);
    }

    for (int t = 0; t <= 1; t++) {
        if (t == 1 && d->rehash_idx == -1) break;

        dict_ht_t *ht = &d->ht[t];
        if (!ht->table || ht->size == 0) continue;

        uint32_t h = dict_hash(key) & ht->sizemask;
        dict_entry_t *prev = NULL;
        dict_entry_t *entry = ht->table[h];

        while (entry) {
            if (strcmp(entry->key, key) == 0) {
                if (prev) {
                    prev->next = entry->next;
                } else {
                    ht->table[h] = entry->next;
                }
                free(entry->key);
                free(entry->value);
                free(entry);
                ht->used--;
                return 0;
            }
            prev = entry;
            entry = entry->next;
        }
    }

    return -1;  /* Cle non trouvee */
}

/* Taille totale */
static size_t dict_size(const dict_t *d)
{
    return d->ht[0].used + d->ht[1].used;
}

/* ============================================ */
/* Demonstration                                */
/* ============================================ */

int main(void)
{
    printf("=== Hash Table avec Rehashing Incremental (Redis) ===\n\n");

    dict_t *d = dict_create();
    if (!d) {
        fprintf(stderr, "Erreur creation dictionnaire\n");
        return EXIT_FAILURE;
    }

    /* Inserer des elements (declenchera le rehashing) */
    printf("--- Insertion de donnees ---\n");
    const char *keys[] = {
        "name", "city", "lang", "os", "editor",
        "shell", "db", "web", "vcs", "debug"
    };
    const char *values[] = {
        "Redis", "Paris", "C", "Linux", "vim",
        "bash", "redis", "nginx", "git", "gdb"
    };
    int n = (int)(sizeof(keys) / sizeof(keys[0]));

    for (int i = 0; i < n; i++) {
        printf("  SET %s = %s (taille: %zu)\n", keys[i], values[i],
               dict_size(d) + 1);
        dict_set(d, keys[i], values[i]);
    }

    /* Rechercher */
    printf("\n--- Recherche ---\n");
    for (int i = 0; i < n; i++) {
        const char *val = dict_get(d, keys[i]);
        printf("  GET %s = %s\n", keys[i], val ? val : "(null)");
    }

    /* Cle inexistante */
    const char *missing = dict_get(d, "nonexistent");
    printf("  GET nonexistent = %s\n", missing ? missing : "(null)");

    /* Mise a jour */
    printf("\n--- Mise a jour ---\n");
    dict_set(d, "city", "Lyon");
    printf("  SET city = Lyon\n");
    printf("  GET city = %s\n", dict_get(d, "city"));

    /* Suppression */
    printf("\n--- Suppression ---\n");
    dict_delete(d, "editor");
    printf("  DEL editor\n");
    printf("  GET editor = %s\n",
           dict_get(d, "editor") ? dict_get(d, "editor") : "(null)");
    printf("  Taille: %zu\n", dict_size(d));

    /* Statistiques */
    printf("\n--- Statistiques ---\n");
    printf("  Entrees: %zu\n", dict_size(d));
    printf("  Buckets ht[0]: %zu (used: %zu)\n",
           d->ht[0].size, d->ht[0].used);
    if (d->ht[1].table) {
        printf("  Buckets ht[1]: %zu (used: %zu) [rehash en cours]\n",
               d->ht[1].size, d->ht[1].used);
    }
    printf("  Operations totales: %zu\n", d->total_ops);
    printf("  Rehash en cours: %s\n",
           d->rehash_idx != -1 ? "oui" : "non");

    /* Nettoyage */
    dict_destroy(d);

    printf("\n--- Pourquoi le rehashing incremental ---\n");
    printf("1. Pas de latence soudaine lors du redimensionnement\n");
    printf("2. Chaque operation GET/SET fait un peu de rehash\n");
    printf("3. Deux tables coexistent pendant le rehash\n");
    printf("4. Les lookups cherchent dans les deux tables\n");
    printf("5. Redis reste rapide meme avec des millions de cles\n");

    return EXIT_SUCCESS;
}
