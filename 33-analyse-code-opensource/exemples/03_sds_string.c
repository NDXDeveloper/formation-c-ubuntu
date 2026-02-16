/* ============================================================================
   Section 33.3 : Etude de cas Redis
   Description : Simple Dynamic String (SDS) inspire de Redis
   Fichier source : 03-etude-cas-redis.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* ============================================ */
/* Simple Dynamic String (SDS) - Redis          */
/* ============================================ */

/*
 * Redis n'utilise pas les chaines C classiques (char*).
 * Il utilise SDS : un wrapper qui stocke la taille et la capacite
 * dans un header avant la chaine.
 *
 * Avantages :
 * - strlen() en O(1) au lieu de O(n)
 * - Pas de buffer overflow (agrandissement automatique)
 * - Compatible avec les fonctions C classiques (c'est un char*)
 * - Binary-safe (peut contenir des \0)
 */

/* Header SDS stocke juste avant la chaine */
typedef struct {
    size_t len;     /* Longueur utilisee */
    size_t alloc;   /* Capacite totale allouee */
} sds_header_t;

/* Un sds est simplement un char* qui pointe apres le header */
typedef char *sds;

/* ============================================ */
/* Operations SDS                               */
/* ============================================ */

/* Acceder au header depuis le pointeur sds */
static sds_header_t *sds_hdr(const sds s)
{
    return (sds_header_t *)(s - sizeof(sds_header_t));
}

/* Creer un nouveau SDS */
static sds sds_new(const char *init)
{
    size_t initlen = init ? strlen(init) : 0;
    size_t alloc = initlen + 1;  /* +1 pour le \0 */

    /* Allouer header + chaine */
    sds_header_t *hdr = malloc(sizeof(sds_header_t) + alloc);
    if (!hdr) return NULL;

    hdr->len = initlen;
    hdr->alloc = alloc;

    sds s = (char *)(hdr + 1);  /* Pointeur apres le header */

    if (init && initlen > 0) {
        memcpy(s, init, initlen);
    }
    s[initlen] = '\0';

    return s;
}

/* Liberer un SDS */
static void sds_free(sds s)
{
    if (s) {
        free(sds_hdr(s));
    }
}

/* Longueur en O(1) */
static size_t sds_len(const sds s)
{
    return sds_hdr(s)->len;
}

/* Espace disponible */
static size_t sds_avail(const sds s)
{
    sds_header_t *hdr = sds_hdr(s);
    return hdr->alloc - hdr->len - 1;  /* -1 pour le \0 */
}

/* Agrandir si necessaire */
static sds sds_grow(sds s, size_t addlen)
{
    if (sds_avail(s) >= addlen) {
        return s;  /* Assez de place */
    }

    sds_header_t *hdr = sds_hdr(s);
    size_t newlen = hdr->len + addlen;

    /* Strategie Redis : doubler si < 1MB, sinon +1MB */
    size_t newalloc;
    if (newlen < 1024 * 1024) {
        newalloc = newlen * 2 + 1;
    } else {
        newalloc = newlen + 1024 * 1024 + 1;
    }

    sds_header_t *newhdr = realloc(hdr, sizeof(sds_header_t) + newalloc);
    if (!newhdr) return NULL;

    newhdr->alloc = newalloc;
    return (char *)(newhdr + 1);
}

/* Concatener (equivalent de strcat mais safe) */
static sds sds_cat(sds s, const char *t)
{
    size_t tlen = strlen(t);
    s = sds_grow(s, tlen);
    if (!s) return NULL;

    sds_header_t *hdr = sds_hdr(s);
    memcpy(s + hdr->len, t, tlen);
    hdr->len += tlen;
    s[hdr->len] = '\0';

    return s;
}

/* Concatener un autre SDS */
static sds sds_cat_sds(sds s, const sds t)
{
    size_t tlen = sds_len(t);
    s = sds_grow(s, tlen);
    if (!s) return NULL;

    sds_header_t *hdr = sds_hdr(s);
    memcpy(s + hdr->len, t, tlen);
    hdr->len += tlen;
    s[hdr->len] = '\0';

    return s;
}

/* Dupliquer */
static sds sds_dup(const sds s)
{
    return sds_new(s);
}

/* Vider sans liberer */
static void sds_clear(sds s)
{
    sds_hdr(s)->len = 0;
    s[0] = '\0';
}

/* Formater (comme sprintf mais dans un SDS) */
static sds sds_catprintf(sds s, const char *fmt, ...)
{
    char buf[256];
    va_list ap;

    /* Utiliser vsnprintf pour formater dans un buffer temporaire */
    va_start(ap, fmt);
    int len = vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    if (len < 0) return s;
    return sds_cat(s, buf);
}

/* ============================================ */
/* Demonstration                                */
/* ============================================ */

static void show_sds_info(const char *label, const sds s)
{
    printf("  %-20s : \"%s\" (len=%zu, alloc=%zu, avail=%zu)\n",
           label, s, sds_len(s), sds_hdr(s)->alloc, sds_avail(s));
}

int main(void)
{
    printf("=== Simple Dynamic String - SDS (Redis) ===\n\n");

    /* Creation */
    printf("--- Creation ---\n");
    sds greeting = sds_new("Hello");
    show_sds_info("sds_new(\"Hello\")", greeting);

    /* Concatenation safe */
    printf("\n--- Concatenation (safe, pas de buffer overflow) ---\n");
    greeting = sds_cat(greeting, ", ");
    show_sds_info("cat(\", \")", greeting);

    greeting = sds_cat(greeting, "World");
    show_sds_info("cat(\"World\")", greeting);

    greeting = sds_cat(greeting, "!");
    show_sds_info("cat(\"!\")", greeting);

    /* strlen O(1) */
    printf("\n--- Longueur en O(1) ---\n");
    printf("  sds_len() = %zu (pas besoin de parcourir la chaine)\n",
           sds_len(greeting));

    /* Comparaison avec strcat (unsafe) */
    printf("\n--- Comparaison avec strcat ---\n");
    printf("  strcat : buffer overflow si buf trop petit\n");
    printf("  sds_cat: agrandissement automatique, toujours safe\n");

    /* Duplication */
    printf("\n--- Duplication ---\n");
    sds copy = sds_dup(greeting);
    show_sds_info("sds_dup(greeting)", copy);
    printf("  Adresses differentes: greeting=%p, copy=%p\n",
           (void *)greeting, (void *)copy);

    /* Clear et reutilisation */
    printf("\n--- Clear et reutilisation ---\n");
    sds_clear(copy);
    show_sds_info("apres sds_clear()", copy);

    copy = sds_cat(copy, "Redis");
    show_sds_info("cat(\"Redis\")", copy);

    /* Concatenation SDS + SDS */
    printf("\n--- Concatenation SDS + SDS ---\n");
    sds full = sds_new("Projet: ");
    full = sds_cat_sds(full, copy);
    show_sds_info("cat_sds(\"Projet: \"+copy)", full);

    /* Printf-like */
    printf("\n--- Printf dans SDS ---\n");
    sds info = sds_new("Infos: ");
    info = sds_catprintf(info, "count=%d, name=%s", 42, "test");
    show_sds_info("catprintf()", info);

    /* Compatible avec les fonctions C */
    printf("\n--- Compatibilite C ---\n");
    printf("  printf(\"%%s\", sds) = \"%s\"\n", greeting);
    printf("  strcmp(sds, \"Hello, World!\") = %d\n",
           strcmp(greeting, "Hello, World!"));

    /* Liberation */
    sds_free(greeting);
    sds_free(copy);
    sds_free(full);
    sds_free(info);

    printf("\n--- Pourquoi Redis utilise SDS ---\n");
    printf("1. strlen() en O(1) au lieu de O(n)\n");
    printf("2. Pas de buffer overflow (agrandissement auto)\n");
    printf("3. Binary-safe (peut contenir des \\0)\n");
    printf("4. Compatible avec les fonctions C standard\n");
    printf("5. Pre-allocation pour reduire les realloc\n");

    return EXIT_SUCCESS;
}
