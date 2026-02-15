/* ============================================================================
   Section 24.3 : Garbage collection en C
   Description : Reference counting - comptage de references automatique
   Fichier source : 03-garbage-collection.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

// Structure de base avec compteur de references
typedef struct {
    int ref_count;      // Compteur de references
    size_t size;        // Taille de l'allocation
    char data[];        // Donnees flexibles (C99)
} RefCountedObject;

// Allouer un objet avec reference counting
void *rc_alloc(size_t size) {
    RefCountedObject *obj = malloc(sizeof(RefCountedObject) + size);
    if (!obj) {
        return NULL;
    }

    obj->ref_count = 1;  // Initialement 1 reference
    obj->size = size;

    printf("  [NEW] Objet alloue (%zu bytes), ref_count=1\n", size);

    return obj->data;  // Retourner le pointeur vers les donnees
}

// Obtenir l'objet depuis un pointeur de donnees
static RefCountedObject *rc_get_object(void *ptr) {
    if (!ptr) return NULL;

    // Le pointeur pointe vers data, reculer pour obtenir l'objet
    return (RefCountedObject *)((char *)ptr - offsetof(RefCountedObject, data));
}

// Incrementer le compteur (nouvelle reference)
void *rc_retain(void *ptr) {
    if (!ptr) return NULL;

    RefCountedObject *obj = rc_get_object(ptr);
    obj->ref_count++;

    printf("  [+] ref_count++ = %d\n", obj->ref_count);

    return ptr;
}

// Decrementer le compteur (liberer une reference)
void rc_release(void *ptr) {
    if (!ptr) return;

    RefCountedObject *obj = rc_get_object(ptr);
    obj->ref_count--;

    printf("  [-] ref_count-- = %d\n", obj->ref_count);

    // Si plus aucune reference, liberer
    if (obj->ref_count == 0) {
        printf("  [FREE] Objet libere (ref_count=0)\n");
        free(obj);
    }
}

// Obtenir le compteur actuel
int rc_get_count(void *ptr) {
    if (!ptr) return 0;
    RefCountedObject *obj = rc_get_object(ptr);
    return obj->ref_count;
}

void exemple_reference_counting(void) {
    printf("=== Exemple Reference Counting ===\n\n");

    // Allouer un buffer
    char *buffer = rc_alloc(100);
    if (!buffer) return;

    strcpy(buffer, "Hello, Reference Counting!");
    printf("Buffer: %s\n", buffer);
    printf("References: %d\n\n", rc_get_count(buffer));

    // Creer une nouvelle reference
    char *buffer2 = rc_retain(buffer);
    printf("buffer2 pointe vers le meme objet\n");
    printf("References: %d\n\n", rc_get_count(buffer));

    // Creer une troisieme reference
    char *buffer3 = rc_retain(buffer);
    printf("buffer3 pointe vers le meme objet\n");
    printf("References: %d\n\n", rc_get_count(buffer));

    // Liberer buffer (decremente)
    printf("Liberation de buffer...\n");
    rc_release(buffer);
    printf("References restantes: %d\n\n", rc_get_count(buffer2));

    // Liberer buffer2 (decremente)
    printf("Liberation de buffer2...\n");
    rc_release(buffer2);
    printf("References restantes: %d\n\n", rc_get_count(buffer3));

    // Liberer buffer3 (decremente a 0 -> liberation automatique)
    printf("Liberation de buffer3...\n");
    rc_release(buffer3);

    printf("\n=== Fin de l'exemple ===\n");
}

int main(void) {
    exemple_reference_counting();
    return 0;
}
