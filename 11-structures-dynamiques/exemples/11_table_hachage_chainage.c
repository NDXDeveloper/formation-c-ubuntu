/* ============================================================================
   Section 11.4 : Tables de hachage - Chainage
   Description : Table de hachage avec gestion des collisions par chainage
                 (listes chainees dans chaque bucket)
   Fichier source : 04-tables-hachage.md
   ============================================================================ */

#define _POSIX_C_SOURCE 200809L  /* Pour strdup() */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TABLE_SIZE 10

/* Noeud pour la liste chainee */
typedef struct HashNode {
    char* key;
    int value;
    struct HashNode* next;
} HashNode;

/* Table de hachage */
typedef struct HashTable {
    HashNode* buckets[TABLE_SIZE];
    size_t size;
} HashTable;

void init_hash_table(HashTable* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->buckets[i] = NULL;
    }
    table->size = 0;
}

/* Fonction de hachage djb2 */
unsigned int hash(const char* key) {
    unsigned long hash_value = 5381;
    int c;
    while ((c = *key++)) {
        hash_value = ((hash_value << 5) + hash_value) + (unsigned long)c;
    }
    return (unsigned int)(hash_value % TABLE_SIZE);
}

void hash_table_insert(HashTable* table, const char* key, int value) {
    unsigned int index = hash(key);

    /* Verifier si la cle existe deja (mise a jour) */
    HashNode* current = table->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            current->value = value;
            return;
        }
        current = current->next;
    }

    /* Cle non trouvee, creer un nouveau noeud */
    HashNode* new_node = (HashNode*)malloc(sizeof(HashNode));
    if (new_node == NULL) {
        fprintf(stderr, "Erreur : allocation memoire\n");
        exit(EXIT_FAILURE);
    }

    new_node->key = strdup(key);
    new_node->value = value;

    /* Insertion au debut de la liste */
    new_node->next = table->buckets[index];
    table->buckets[index] = new_node;
    table->size++;
}

bool hash_table_search(HashTable* table, const char* key, int* value_out) {
    unsigned int index = hash(key);

    HashNode* current = table->buckets[index];
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (value_out != NULL) {
                *value_out = current->value;
            }
            return true;
        }
        current = current->next;
    }

    return false;
}

bool hash_table_delete(HashTable* table, const char* key) {
    unsigned int index = hash(key);

    HashNode* current = table->buckets[index];
    HashNode* previous = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (previous == NULL) {
                table->buckets[index] = current->next;
            } else {
                previous->next = current->next;
            }

            free(current->key);
            free(current);
            table->size--;
            return true;
        }

        previous = current;
        current = current->next;
    }

    return false;
}

void hash_table_print(HashTable* table) {
    printf("=== Table de hachage ===\n");
    for (int i = 0; i < TABLE_SIZE; i++) {
        printf("Bucket %d: ", i);

        HashNode* current = table->buckets[i];
        if (current == NULL) {
            printf("[vide]\n");
        } else {
            while (current != NULL) {
                printf("[\"%s\" -> %d]", current->key, current->value);
                if (current->next != NULL) {
                    printf(" -> ");
                }
                current = current->next;
            }
            printf("\n");
        }
    }
    printf("Taille totale : %zu elements\n", table->size);
}

void hash_table_free(HashTable* table) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        HashNode* current = table->buckets[i];
        while (current != NULL) {
            HashNode* next = current->next;
            free(current->key);
            free(current);
            current = next;
        }
        table->buckets[i] = NULL;
    }
    table->size = 0;
}

int main(void) {
    HashTable table;
    init_hash_table(&table);

    /* Insertion */
    printf("=== Insertions ===\n");
    hash_table_insert(&table, "Alice", 123456);
    hash_table_insert(&table, "Bob", 789012);
    hash_table_insert(&table, "Charlie", 345678);
    hash_table_insert(&table, "Zoe", 901234);

    hash_table_print(&table);

    /* Recherche */
    printf("\n=== Recherche ===\n");
    int value;
    if (hash_table_search(&table, "Alice", &value)) {
        printf("Alice trouvee : %d\n", value);
    }

    if (!hash_table_search(&table, "David", &value)) {
        printf("David non trouve\n");
    }

    /* Mise a jour */
    printf("\n=== Mise a jour ===\n");
    hash_table_insert(&table, "Alice", 999999);
    hash_table_search(&table, "Alice", &value);
    printf("Alice apres mise a jour : %d\n", value);

    /* Suppression */
    printf("\n=== Suppression de Bob ===\n");
    hash_table_delete(&table, "Bob");
    hash_table_print(&table);

    /* Liberation */
    hash_table_free(&table);

    return 0;
}
