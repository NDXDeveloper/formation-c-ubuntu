/* ============================================================================
   Section 24.3 : Garbage collection en C
   Description : Mark and Sweep GC - marquer puis balayer les objets inutilises
   Fichier source : 03-garbage-collection.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Configuration du GC
#define GC_INITIAL_CAPACITY 256
#define GC_GROWTH_FACTOR 2

// En-tete de chaque objet gere
typedef struct GCObject {
    bool marked;              // Marque lors du GC
    size_t size;              // Taille de l'objet
    struct GCObject *next;    // Liste chainee des objets
} GCObject;

// Structure du garbage collector
typedef struct {
    GCObject *first_object;   // Premier objet de la liste
    size_t num_objects;       // Nombre d'objets
    size_t max_objects;       // Seuil pour declencher le GC
    void **roots;             // Tableau des racines
    size_t num_roots;
    size_t capacity_roots;
} GarbageCollector;

// Declaration anticipee (gc_collect est appele dans gc_alloc)
void gc_collect(GarbageCollector *gc);

// Creer un garbage collector
GarbageCollector *gc_create(void) {
    GarbageCollector *gc = malloc(sizeof(GarbageCollector));
    if (!gc) return NULL;

    gc->first_object = NULL;
    gc->num_objects = 0;
    gc->max_objects = GC_INITIAL_CAPACITY;
    gc->num_roots = 0;
    gc->capacity_roots = 10;
    gc->roots = malloc(gc->capacity_roots * sizeof(void *));

    printf("[GC] Cree (seuil: %zu objets)\n", gc->max_objects);

    return gc;
}

// Allouer un objet via le GC
void *gc_alloc(GarbageCollector *gc, size_t size) {
    // Declencher le GC si necessaire
    if (gc->num_objects >= gc->max_objects) {
        gc_collect(gc);
    }

    // Allouer l'objet avec son en-tete
    GCObject *obj = malloc(sizeof(GCObject) + size);
    if (!obj) return NULL;

    obj->marked = false;
    obj->size = size;

    // Ajouter a la liste des objets
    obj->next = gc->first_object;
    gc->first_object = obj;
    gc->num_objects++;

    printf("[GC] Objet alloue (%zu bytes) - Total: %zu\n",
           size, gc->num_objects);

    // Retourner le pointeur apres l'en-tete
    return (void *)(obj + 1);
}

// Ajouter une racine
void gc_add_root(GarbageCollector *gc, void *ptr) {
    if (gc->num_roots >= gc->capacity_roots) {
        gc->capacity_roots *= 2;
        gc->roots = realloc(gc->roots, gc->capacity_roots * sizeof(void *));
    }

    gc->roots[gc->num_roots++] = ptr;
}

// Retirer une racine
void gc_remove_root(GarbageCollector *gc, void *ptr) {
    for (size_t i = 0; i < gc->num_roots; i++) {
        if (gc->roots[i] == ptr) {
            gc->roots[i] = gc->roots[--gc->num_roots];
            return;
        }
    }
}

// Phase MARK : marquer un objet
static void gc_mark_object(void *ptr) {
    if (!ptr) return;

    GCObject *obj = ((GCObject *)ptr) - 1;

    if (obj->marked) return;  // Deja marque

    obj->marked = true;
    printf("  [MARK] Objet marque (%p)\n", ptr);
}

// Phase MARK : marquer depuis les racines
static void gc_mark(GarbageCollector *gc) {
    printf("\n  --- Phase MARK ---\n");

    for (size_t i = 0; i < gc->num_roots; i++) {
        if (gc->roots[i]) {
            gc_mark_object(gc->roots[i]);
        }
    }
}

// Phase SWEEP : liberer les objets non marques
static void gc_sweep(GarbageCollector *gc) {
    printf("  --- Phase SWEEP ---\n");

    GCObject **obj_ptr = &gc->first_object;
    size_t collected = 0;

    while (*obj_ptr) {
        GCObject *obj = *obj_ptr;

        if (!obj->marked) {
            // Objet non marque -> a liberer
            *obj_ptr = obj->next;
            printf("  [SWEEP] Objet collecte (%p, %zu bytes)\n",
                   (void *)(obj + 1), obj->size);
            free(obj);
            gc->num_objects--;
            collected++;
        } else {
            // Objet marque -> reinitialiser le marqueur pour le prochain GC
            obj->marked = false;
            obj_ptr = &obj->next;
        }
    }

    printf("  %zu objet(s) collecte(s), %zu restant(s)\n",
           collected, gc->num_objects);

    // Ajuster le seuil
    gc->max_objects = gc->num_objects * GC_GROWTH_FACTOR;
    if (gc->max_objects < GC_INITIAL_CAPACITY) {
        gc->max_objects = GC_INITIAL_CAPACITY;
    }
}

// Lancer une collection
void gc_collect(GarbageCollector *gc) {
    printf("\n=== GARBAGE COLLECTION ===\n");
    printf("  Objets avant GC: %zu\n", gc->num_objects);

    gc_mark(gc);
    gc_sweep(gc);

    printf("  Nouveau seuil: %zu objets\n", gc->max_objects);
    printf("=========================\n");
}

// Detruire le GC (liberer tout)
void gc_destroy(GarbageCollector *gc) {
    printf("\n[GC] Destruction du GC\n");

    GCObject *obj = gc->first_object;
    size_t destroyed = 0;
    while (obj) {
        GCObject *next = obj->next;
        free(obj);
        destroyed++;
        obj = next;
    }

    printf("[GC] %zu objet(s) restant(s) libere(s)\n", destroyed);

    free(gc->roots);
    free(gc);
}

void exemple_mark_and_sweep(void) {
    printf("=== Exemple Mark and Sweep GC ===\n\n");

    // Creer le GC
    GarbageCollector *gc = gc_create();
    if (!gc) return;

    // Allouer des objets
    char *str1 = gc_alloc(gc, 100);
    strcpy(str1, "String 1");
    gc_add_root(gc, str1);  // str1 est une racine

    char *str2 = gc_alloc(gc, 200);
    strcpy(str2, "String 2");
    gc_add_root(gc, str2);  // str2 est une racine

    char *str3 = gc_alloc(gc, 150);
    strcpy(str3, "String 3 - temporaire");
    // str3 n'est PAS une racine

    char *str4 = gc_alloc(gc, 300);
    strcpy(str4, "String 4");
    gc_add_root(gc, str4);  // str4 est une racine

    printf("\nEtat actuel:\n");
    printf("  str1 (racine): %s\n", str1);
    printf("  str2 (racine): %s\n", str2);
    printf("  str3 (temporaire): %s\n", str3);
    printf("  str4 (racine): %s\n", str4);

    // Declencher manuellement le GC
    gc_collect(gc);
    // str3 sera collecte car ce n'est pas une racine !

    printf("\nApres GC:\n");
    printf("  str1: %s (toujours valide)\n", str1);
    printf("  str2: %s (toujours valide)\n", str2);
    printf("  str3: [INVALIDE - collecte]\n");
    printf("  str4: %s (toujours valide)\n", str4);

    // Retirer une racine
    gc_remove_root(gc, str2);
    printf("\nstr2 n'est plus une racine\n");

    // Nouveau GC
    gc_collect(gc);
    // str2 sera collecte cette fois

    printf("\nApres 2eme GC:\n");
    printf("  str1: %s (toujours valide)\n", str1);
    printf("  str4: %s (toujours valide)\n", str4);

    // Nettoyage
    gc_destroy(gc);
    printf("\n=== Fin de l'exemple ===\n");
}

int main(void) {
    exemple_mark_and_sweep();
    return 0;
}
