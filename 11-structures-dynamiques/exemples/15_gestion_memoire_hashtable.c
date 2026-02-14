/* ============================================================================
   Section 11.7 : Gestion memoire des structures dynamiques
   Description : Table de hachage avec liberation correcte de toutes les
                 allocations (cles + noeuds + buckets)
   Fichier source : 07-gestion-memoire.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L  /* Pour strdup() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 10

typedef struct HashNode {
    char* key;
    int value;
    struct HashNode* next;
} HashNode;

typedef struct HashTable {
    HashNode* buckets[TABLE_SIZE];
} HashTable;

unsigned int hash(const char* key) {
    unsigned long h = 5381;
    int c;
    while ((c = *key++)) {
        h = ((h << 5) + h) + (unsigned long)c;
    }
    return (unsigned int)(h % TABLE_SIZE);
}

void init_hash_table(HashTable* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
}

void insert(HashTable* table, const char* key, int value) {
    unsigned int index = hash(key);

    HashNode* node = malloc(sizeof(HashNode));
    if (node == NULL) {
        return;
    }

    node->key = strdup(key);
    if (node->key == NULL) {
        free(node);
        return;
    }

    node->value = value;
    node->next = table->buckets[index];
    table->buckets[index] = node;
}

void free_hash_table(HashTable* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = table->buckets[i];
        while (current != NULL) {
            HashNode* next = current->next;
            free(current->key);   /* Liberer la cle */
            free(current);        /* Liberer le noeud */
            current = next;
        }
        table->buckets[i] = NULL;
    }
}

void print_table(HashTable* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = table->buckets[i];
        if (current != NULL) {
            printf("  Bucket %d: ", i);
            while (current != NULL) {
                printf("[\"%s\"=%d] ", current->key, current->value);
                current = current->next;
            }
            printf("\n");
        }
    }
}

int main(void) {
    HashTable table;
    init_hash_table(&table);

    printf("Insertions...\n");
    insert(&table, "Alice", 123);
    insert(&table, "Bob", 456);
    insert(&table, "Charlie", 789);
    insert(&table, "David", 101);
    insert(&table, "Eve", 202);

    printf("\nContenu de la table :\n");
    print_table(&table);

    printf("\nLiberation de la table...\n");
    free_hash_table(&table);
    printf("Memoire liberee correctement.\n");
    printf("(Verifier avec : valgrind --leak-check=full ./15_gestion_memoire_hashtable)\n");

    return 0;
}
