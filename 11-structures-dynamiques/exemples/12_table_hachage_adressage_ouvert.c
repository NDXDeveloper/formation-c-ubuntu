/* ============================================================================
   Section 11.4 : Tables de hachage - Adressage ouvert (sondage lineaire)
   Description : Table de hachage avec sondage lineaire et tombstones
                 pour la suppression
   Fichier source : 04-tables-hachage.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L  /* Pour strdup() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TABLE_SIZE 10

typedef struct HashEntry {
    char* key;
    int value;
    bool is_occupied;
    bool is_deleted;   /* Tombstone : marqueur de suppression */
} HashEntry;

typedef struct HashTableOpen {
    HashEntry entries[TABLE_SIZE];
    size_t size;
} HashTableOpen;

/* Fonction de hachage djb2 */
unsigned int hash(const char* key) {
    unsigned long hash_value = 5381;
    int c;
    while ((c = *key++)) {
        hash_value = ((hash_value << 5) + hash_value) + (unsigned long)c;
    }
    return (unsigned int)(hash_value % TABLE_SIZE);
}

void init_hash_table_open(HashTableOpen* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->entries[i].key = NULL;
        table->entries[i].is_occupied = false;
        table->entries[i].is_deleted = false;
    }
    table->size = 0;
}

bool hash_table_open_insert(HashTableOpen* table, const char* key, int value) {
    if (table->size >= TABLE_SIZE) {
        fprintf(stderr, "Erreur : table pleine\n");
        return false;
    }

    unsigned int index = hash(key);
    unsigned int original_index = index;

    /* Sondage lineaire */
    while (table->entries[index].is_occupied &&
           !table->entries[index].is_deleted &&
           strcmp(table->entries[index].key, key) != 0) {

        index = (index + 1) % TABLE_SIZE;

        if (index == original_index) {
            fprintf(stderr, "Erreur : table pleine (boucle complete)\n");
            return false;
        }
    }

    /* Mise a jour d'une cle existante */
    if (table->entries[index].is_occupied &&
        !table->entries[index].is_deleted &&
        strcmp(table->entries[index].key, key) == 0) {
        table->entries[index].value = value;
        return true;
    }

    /* Insertion dans une case libre ou supprimee */
    if (table->entries[index].key != NULL) {
        free(table->entries[index].key);
    }

    table->entries[index].key = strdup(key);
    table->entries[index].value = value;
    table->entries[index].is_occupied = true;
    table->entries[index].is_deleted = false;
    table->size++;
    return true;
}

bool hash_table_open_search(HashTableOpen* table, const char* key, int* value_out) {
    unsigned int index = hash(key);
    unsigned int original_index = index;

    while (table->entries[index].is_occupied ||
           table->entries[index].is_deleted) {

        if (table->entries[index].is_occupied &&
            !table->entries[index].is_deleted &&
            strcmp(table->entries[index].key, key) == 0) {
            if (value_out != NULL) {
                *value_out = table->entries[index].value;
            }
            return true;
        }

        index = (index + 1) % TABLE_SIZE;

        if (index == original_index) {
            break;
        }
    }

    return false;
}

bool hash_table_open_delete(HashTableOpen* table, const char* key) {
    unsigned int index = hash(key);
    unsigned int original_index = index;

    while (table->entries[index].is_occupied ||
           table->entries[index].is_deleted) {

        if (table->entries[index].is_occupied &&
            !table->entries[index].is_deleted &&
            strcmp(table->entries[index].key, key) == 0) {
            table->entries[index].is_deleted = true;
            table->size--;
            return true;
        }

        index = (index + 1) % TABLE_SIZE;

        if (index == original_index) {
            break;
        }
    }

    return false;
}

void hash_table_open_print(HashTableOpen* table) {
    printf("=== Table de hachage (adressage ouvert) ===\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        printf("  [%d] ", i);
        if (table->entries[i].is_deleted) {
            printf("[SUPPRIME (tombstone)]\n");
        } else if (table->entries[i].is_occupied) {
            printf("\"%s\" -> %d\n", table->entries[i].key,
                   table->entries[i].value);
        } else {
            printf("[vide]\n");
        }
    }
    printf("Taille : %zu / %d\n", table->size, TABLE_SIZE);
}

void hash_table_open_free(HashTableOpen* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        if (table->entries[i].key != NULL) {
            free(table->entries[i].key);
            table->entries[i].key = NULL;
        }
        table->entries[i].is_occupied = false;
        table->entries[i].is_deleted = false;
    }
    table->size = 0;
}

int main(void) {
    HashTableOpen table;
    init_hash_table_open(&table);

    /* Insertions */
    printf("=== Insertions ===\n");
    hash_table_open_insert(&table, "Alice", 123456);
    hash_table_open_insert(&table, "Bob", 789012);
    hash_table_open_insert(&table, "Charlie", 345678);
    hash_table_open_insert(&table, "Zoe", 901234);
    hash_table_open_print(&table);

    /* Recherche */
    printf("\n=== Recherche ===\n");
    int value;
    if (hash_table_open_search(&table, "Charlie", &value)) {
        printf("Charlie trouve : %d\n", value);
    }
    if (!hash_table_open_search(&table, "David", &value)) {
        printf("David non trouve\n");
    }

    /* Suppression avec tombstone */
    printf("\n=== Suppression de Bob (tombstone) ===\n");
    hash_table_open_delete(&table, "Bob");
    hash_table_open_print(&table);

    /* Verification : recherche apres suppression */
    printf("\n=== Recherche apres suppression ===\n");
    if (!hash_table_open_search(&table, "Bob", &value)) {
        printf("Bob non trouve (supprime)\n");
    }
    if (hash_table_open_search(&table, "Charlie", &value)) {
        printf("Charlie toujours accessible : %d\n", value);
    }

    hash_table_open_free(&table);
    return 0;
}
