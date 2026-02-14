/* ============================================================================
   Section 9.4 : Gestion des erreurs d'allocation
   Description : Tests de robustesse avec simulation d'echecs d'allocation
   Fichier source : 04-gestion-erreurs.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static bool test_mode = false;
static int allocation_counter = 0;
static int fail_at_allocation = -1;

void set_test_mode(bool enable, int fail_at) {
    test_mode = enable;
    allocation_counter = 0;
    fail_at_allocation = fail_at;
}

void* test_malloc(size_t size) {
    if (test_mode) {
        allocation_counter++;

        if (fail_at_allocation > 0 && allocation_counter == fail_at_allocation) {
            printf("[TEST] Simulation d'echec a l'allocation #%d\n",
                   allocation_counter);
            return NULL;
        }
    }

    return malloc(size);
}

typedef struct {
    int* data;
    char* name;
    double* values;
} ComplexStruct;

ComplexStruct* create_complex(const char* name, int count) {
    ComplexStruct* obj = test_malloc(sizeof(ComplexStruct));
    if (obj == NULL) {
        fprintf(stderr, "Echec allocation structure\n");
        return NULL;
    }

    obj->data = test_malloc(count * sizeof(int));
    if (obj->data == NULL) {
        fprintf(stderr, "Echec allocation data\n");
        free(obj);
        return NULL;
    }

    obj->name = test_malloc(strlen(name) + 1);
    if (obj->name == NULL) {
        fprintf(stderr, "Echec allocation name\n");
        free(obj->data);
        free(obj);
        return NULL;
    }
    strcpy(obj->name, name);

    obj->values = test_malloc(count * sizeof(double));
    if (obj->values == NULL) {
        fprintf(stderr, "Echec allocation values\n");
        free(obj->name);
        free(obj->data);
        free(obj);
        return NULL;
    }

    printf("Structure creee avec succes\n");
    return obj;
}

void destroy_complex(ComplexStruct* obj) {
    if (obj != NULL) {
        free(obj->values);
        free(obj->name);
        free(obj->data);
        free(obj);
    }
}

int main(void) {
    printf("=== Test 1 : Tout reussit ===\n");
    set_test_mode(true, -1);
    ComplexStruct* obj1 = create_complex("test", 10);
    if (obj1) destroy_complex(obj1);

    printf("\n=== Test 2 : Echec a la 2e allocation ===\n");
    set_test_mode(true, 2);
    ComplexStruct* obj2 = create_complex("test", 10);
    if (obj2) destroy_complex(obj2);

    printf("\n=== Test 3 : Echec a la 4e allocation ===\n");
    set_test_mode(true, 4);
    ComplexStruct* obj3 = create_complex("test", 10);
    if (obj3) destroy_complex(obj3);

    return 0;
}
