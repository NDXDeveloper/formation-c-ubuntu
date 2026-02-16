/* ============================================================================
   Section 34.2.3 : Agregation et statistiques
   Description : Table de hachage pour compter des occurrences (IPs, URLs)
   Fichier source : 02.3-agregation-statistiques.md
   ============================================================================ */
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 10000

typedef struct Entry {
    char *cle;
    long long valeur;
    struct Entry *suivant;
} Entry;

typedef struct {
    Entry *buckets[HASH_SIZE];
} HashMap;

unsigned long hash_str(const char *str) {
    unsigned long h = 5381;
    int c;

    while ((c = *str++)) {
        h = ((h << 5) + h) + (unsigned long)c;
    }

    return h % HASH_SIZE;
}

HashMap *creer_hashmap(void) {
    HashMap *map = calloc(1, sizeof(HashMap));
    return map;
}

void incrementer(HashMap *map, const char *cle) {
    unsigned long index = hash_str(cle);
    Entry *entry = map->buckets[index];

    while (entry != NULL) {
        if (strcmp(entry->cle, cle) == 0) {
            entry->valeur++;
            return;
        }
        entry = entry->suivant;
    }

    Entry *nouvelle = malloc(sizeof(Entry));
    nouvelle->cle = strdup(cle);
    nouvelle->valeur = 1;
    nouvelle->suivant = map->buckets[index];
    map->buckets[index] = nouvelle;
}

void parcourir_hashmap(HashMap *map, void (*callback)(const char*, long long)) {
    for (int i = 0; i < HASH_SIZE; i++) {
        Entry *entry = map->buckets[i];
        while (entry != NULL) {
            callback(entry->cle, entry->valeur);
            entry = entry->suivant;
        }
    }
}

void liberer_hashmap(HashMap *map) {
    for (int i = 0; i < HASH_SIZE; i++) {
        Entry *entry = map->buckets[i];
        while (entry != NULL) {
            Entry *suivant = entry->suivant;
            free(entry->cle);
            free(entry);
            entry = suivant;
        }
    }
    free(map);
}

void afficher_entry(const char *cle, long long valeur) {
    printf("  %s: %lld\n", cle, valeur);
}

int main(void) {
    HashMap *ips = creer_hashmap();

    // Compter les IPs
    incrementer(ips, "192.168.1.100");
    incrementer(ips, "10.0.0.5");
    incrementer(ips, "192.168.1.100");
    incrementer(ips, "172.16.0.1");
    incrementer(ips, "192.168.1.100");
    incrementer(ips, "10.0.0.5");

    printf("IPs et leurs compteurs:\n");
    parcourir_hashmap(ips, afficher_entry);

    liberer_hashmap(ips);
    return 0;
}
