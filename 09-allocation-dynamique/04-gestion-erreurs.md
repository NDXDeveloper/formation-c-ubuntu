🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 9.4 Gestion des erreurs d'allocation

## Introduction

L'allocation dynamique peut échouer pour diverses raisons : mémoire insuffisante, fragmentation, limites système, etc. Un programme robuste **doit toujours gérer ces échecs** pour éviter les crashes et les comportements imprévisibles.

Dans cette section, vous apprendrez :
- Pourquoi et quand les allocations échouent
- Comment détecter les erreurs d'allocation
- Les stratégies de récupération et de gestion d'erreurs
- Les patterns de code robustes
- Comment tester votre gestion d'erreurs

> **Principe fondamental :** Ne jamais supposer qu'une allocation réussira. Toujours vérifier et gérer les échecs.

---

## Pourquoi les allocations échouent

### Causes principales

| Cause | Description | Fréquence |
|-------|-------------|-----------|
| **Mémoire insuffisante** | Plus de RAM disponible | Courante |
| **Fragmentation** | Pas de bloc continu assez grand | Moyenne |
| **Limites système** | Quotas, ulimit, cgroups | Moyenne |
| **Allocation trop grande** | Demande déraisonnable (ex: 10 TB) | Rare mais possible |
| **Erreur système** | Corruption, bug kernel | Très rare |

### Visualisation de la fragmentation

```
État du Heap fragmenté :

┌─────┬────┬─────┬───┬──────┬────┬─────────┐
│ Bloc│Lib-│ Bloc│Lib│ Bloc │Lib-│  Libre  │
│  A  │re  │  B  │re │  C   │re  │         │
│100KB│20KB│80KB │15 │150KB │25KB│  200KB  │
└─────┴────┴─────┴───┴──────┴────┴─────────┘

Tentative d'allouer 100 KB :
→ Total libre : 20 + 15 + 25 + 200 = 260 KB ✓
→ Plus grand bloc continu : 200 KB ✓
→ Allocation réussit dans le dernier bloc

Tentative d'allouer 250 KB :
→ Total libre : 260 KB ✓
→ Plus grand bloc continu : 200 KB seulement ✗
→ Allocation ÉCHOUE malgré assez de mémoire totale !
```

### Exemple d'échec progressif

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    size_t taille = 1024 * 1024;  // 1 MB
    void* ptr;
    int compteur = 0;

    printf("Allocation de blocs de 1 MB...\n");

    // Allouer jusqu'à l'échec
    while ((ptr = malloc(taille)) != NULL) {
        compteur++;
        printf("Bloc %d alloué\n", compteur);

        // ⚠️ NE PAS LIBÉRER pour forcer l'échec
        // En pratique, on libérerait avec free()
    }

    printf("\nÉchec après %d allocations\n", compteur);
    printf("Mémoire totale allouée : %zu MB\n",
           (size_t)compteur * taille / (1024 * 1024));

    return 0;
}
```

---

## Détecter les échecs d'allocation

### Vérification systématique du retour

**❌ Code dangereux (sans vérification) :**

```c
int* tableau = malloc(100 * sizeof(int));  
tableau[0] = 42;  // ⚠️ CRASH si malloc retourne NULL !  
```

**✅ Code correct (avec vérification) :**

```c
int* tableau = malloc(100 * sizeof(int));

if (tableau == NULL) {
    fprintf(stderr, "Erreur : allocation échouée\n");
    return 1;  // Sortie propre
}

tableau[0] = 42;  // Sécurisé : tableau est valide
```

### Pattern de base

```c
void* ptr = malloc(size);

if (ptr == NULL) {
    // Gérer l'erreur
    return ERROR_CODE;
}

// Utiliser ptr en toute sécurité
```

---

## Stratégies de gestion d'erreurs

### Stratégie 1 : Retour d'erreur simple

**Principe :** Retourner un code d'erreur ou NULL à l'appelant.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fonction qui retourne NULL en cas d'échec
char* dupliquer_chaine(const char* source) {
    if (source == NULL) {
        return NULL;
    }

    size_t longueur = strlen(source) + 1;
    char* copie = malloc(longueur);

    if (copie == NULL) {
        // Échec d'allocation : retourner NULL
        return NULL;
    }

    strcpy(copie, source);
    return copie;
}

int main(void) {
    const char* original = "Hello, World!";
    char* copie = dupliquer_chaine(original);

    if (copie == NULL) {
        fprintf(stderr, "Erreur : impossible de dupliquer la chaîne\n");
        return 1;
    }

    printf("Copie : %s\n", copie);
    free(copie);

    return 0;
}
```

### Stratégie 2 : Paramètre de sortie avec code d'erreur

**Principe :** Utiliser un paramètre de sortie et retourner un code d'état.

```c
#include <stdio.h>
#include <stdlib.h>

typedef enum {
    SUCCESS = 0,
    ERROR_NULL_PARAM,
    ERROR_ALLOCATION,
    ERROR_INVALID_SIZE
} ErrorCode;

// Fonction avec code d'erreur explicite
ErrorCode creer_tableau(int** tableau, size_t taille) {
    // Validation des paramètres
    if (tableau == NULL) {
        return ERROR_NULL_PARAM;
    }

    if (taille == 0) {
        return ERROR_INVALID_SIZE;
    }

    // Tentative d'allocation
    *tableau = malloc(taille * sizeof(int));

    if (*tableau == NULL) {
        return ERROR_ALLOCATION;
    }

    return SUCCESS;
}

const char* error_to_string(ErrorCode err) {
    switch (err) {
        case SUCCESS:           return "Succès";
        case ERROR_NULL_PARAM:  return "Paramètre NULL";
        case ERROR_ALLOCATION:  return "Échec d'allocation mémoire";
        case ERROR_INVALID_SIZE: return "Taille invalide";
        default:                return "Erreur inconnue";
    }
}

int main(void) {
    int* mon_tableau = NULL;
    ErrorCode result = creer_tableau(&mon_tableau, 100);

    if (result != SUCCESS) {
        fprintf(stderr, "Erreur : %s\n", error_to_string(result));
        return 1;
    }

    // Utilisation du tableau
    mon_tableau[0] = 42;
    printf("Tableau créé avec succès : %d\n", mon_tableau[0]);

    free(mon_tableau);
    return 0;
}
```

### Stratégie 3 : Nettoyage automatique avec goto

**Principe :** Utiliser `goto` pour un nettoyage centralisé en cas d'erreur multiple.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* nom;
    char* prenom;
    int* notes;
    size_t nb_notes;
} Etudiant;

Etudiant* creer_etudiant(const char* nom, const char* prenom, size_t nb_notes) {
    Etudiant* etudiant = NULL;
    char* nom_copie = NULL;
    char* prenom_copie = NULL;
    int* notes = NULL;

    // Allocation de la structure
    etudiant = malloc(sizeof(Etudiant));
    if (etudiant == NULL) {
        fprintf(stderr, "Erreur : allocation de la structure\n");
        goto error;
    }

    // Allocation du nom
    nom_copie = malloc(strlen(nom) + 1);
    if (nom_copie == NULL) {
        fprintf(stderr, "Erreur : allocation du nom\n");
        goto error;
    }
    strcpy(nom_copie, nom);

    // Allocation du prénom
    prenom_copie = malloc(strlen(prenom) + 1);
    if (prenom_copie == NULL) {
        fprintf(stderr, "Erreur : allocation du prénom\n");
        goto error;
    }
    strcpy(prenom_copie, prenom);

    // Allocation des notes
    notes = malloc(nb_notes * sizeof(int));
    if (notes == NULL) {
        fprintf(stderr, "Erreur : allocation des notes\n");
        goto error;
    }

    // Tout a réussi : initialiser la structure
    etudiant->nom = nom_copie;
    etudiant->prenom = prenom_copie;
    etudiant->notes = notes;
    etudiant->nb_notes = nb_notes;

    return etudiant;

error:
    // Nettoyage en cas d'erreur
    free(notes);
    free(prenom_copie);
    free(nom_copie);
    free(etudiant);
    return NULL;
}

void liberer_etudiant(Etudiant* etudiant) {
    if (etudiant != NULL) {
        free(etudiant->notes);
        free(etudiant->prenom);
        free(etudiant->nom);
        free(etudiant);
    }
}

int main(void) {
    Etudiant* etudiant = creer_etudiant("Dupont", "Marie", 5);

    if (etudiant == NULL) {
        fprintf(stderr, "Impossible de créer l'étudiant\n");
        return 1;
    }

    printf("Étudiant créé : %s %s\n", etudiant->prenom, etudiant->nom);

    liberer_etudiant(etudiant);
    return 0;
}
```

**Avantages du goto pour le nettoyage :**
- ✅ Un seul point de sortie
- ✅ Pas de duplication du code de nettoyage
- ✅ Facile à maintenir
- ✅ Approuvé par le Linux kernel coding style

### Stratégie 4 : Wrapper avec exit()

**Principe :** Pour les applications simples, quitter immédiatement en cas d'erreur.

```c
#include <stdio.h>
#include <stdlib.h>

void* malloc_ou_exit(size_t taille, const char* message) {
    void* ptr = malloc(taille);

    if (ptr == NULL) {
        fprintf(stderr, "FATAL : %s\n", message);
        fprintf(stderr, "Impossible d'allouer %zu octets\n", taille);
        exit(EXIT_FAILURE);
    }

    return ptr;
}

int main(void) {
    // Pas besoin de vérifier : le programme se termine en cas d'échec
    int* tableau = malloc_ou_exit(100 * sizeof(int),
                                   "allocation du tableau principal");

    tableau[0] = 42;
    printf("Tableau créé : %d\n", tableau[0]);

    free(tableau);
    return 0;
}
```

⚠️ **Attention :** Cette approche ne convient QUE aux applications simples. Pour les bibliothèques ou applications complexes, préférez retourner des codes d'erreur.

---

## Gestion des erreurs avec errno

### Utilisation de errno

La variable globale `errno` peut fournir plus d'informations sur l'échec.

```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int main(void) {
    // Tentative d'allocation énorme (probablement impossible)
    size_t taille_enorme = (size_t)1024 * 1024 * 1024 * 1024;  // 1 TB

    errno = 0;  // Réinitialiser errno avant l'appel
    void* ptr = malloc(taille_enorme);

    if (ptr == NULL) {
        fprintf(stderr, "Échec de malloc(%zu)\n", taille_enorme);

        if (errno != 0) {
            fprintf(stderr, "errno = %d : %s\n", errno, strerror(errno));
        } else {
            fprintf(stderr, "malloc a retourné NULL sans définir errno\n");
        }

        return 1;
    }

    free(ptr);
    return 0;
}
```

**Note :** POSIX ne garantit PAS que `malloc()` définit `errno`, mais certaines implémentations le font (généralement `ENOMEM`).

---

## Techniques de récupération

### Technique 1 : Réessayer avec une taille réduite

```c
#include <stdio.h>
#include <stdlib.h>

void* malloc_flexible(size_t taille_souhaitee, size_t taille_min, size_t* taille_allouee) {
    size_t taille = taille_souhaitee;
    void* ptr = NULL;

    // Essayer des tailles décroissantes
    while (taille >= taille_min) {
        ptr = malloc(taille);

        if (ptr != NULL) {
            *taille_allouee = taille;
            printf("Allocation réussie : %zu octets\n", taille);
            return ptr;
        }

        // Réduire la taille de 50%
        taille = taille / 2;
        printf("Échec, réessai avec %zu octets...\n", taille);
    }

    // Échec même avec la taille minimale
    *taille_allouee = 0;
    return NULL;
}

int main(void) {
    size_t taille_allouee;

    // Essayer d'allouer 100 MB, minimum 10 MB
    void* buffer = malloc_flexible(100 * 1024 * 1024,  // 100 MB
                                    10 * 1024 * 1024,   // 10 MB minimum
                                    &taille_allouee);

    if (buffer == NULL) {
        fprintf(stderr, "Impossible d'allouer même le minimum\n");
        return 1;
    }

    printf("Utilisation de %zu MB\n", taille_allouee / (1024 * 1024));

    free(buffer);
    return 0;
}
```

### Technique 2 : Pool de mémoire pré-alloué

```c
#include <stdio.h>
#include <stdlib.h>

#define POOL_SIZE (10 * 1024 * 1024)  // 10 MB

typedef struct {
    unsigned char* pool;
    size_t taille_totale;
    size_t position;
} MemoryPool;

// Initialiser le pool
MemoryPool* init_pool(size_t taille) {
    MemoryPool* pool = malloc(sizeof(MemoryPool));
    if (pool == NULL) return NULL;

    pool->pool = malloc(taille);
    if (pool->pool == NULL) {
        free(pool);
        return NULL;
    }

    pool->taille_totale = taille;
    pool->position = 0;

    printf("Pool de %zu octets créé\n", taille);
    return pool;
}

// Allouer depuis le pool (simplifié, pas de free individuel)
void* pool_alloc(MemoryPool* pool, size_t taille) {
    if (pool == NULL) return NULL;

    // Alignement sur 8 octets
    taille = (taille + 7) & ~7;

    if (pool->position + taille > pool->taille_totale) {
        fprintf(stderr, "Pool plein (demande %zu, reste %zu)\n",
                taille, pool->taille_totale - pool->position);
        return NULL;
    }

    void* ptr = pool->pool + pool->position;
    pool->position += taille;

    return ptr;
}

// Libérer le pool entier
void destroy_pool(MemoryPool* pool) {
    if (pool != NULL) {
        free(pool->pool);
        free(pool);
    }
}

int main(void) {
    // Créer un pool au démarrage
    MemoryPool* pool = init_pool(POOL_SIZE);

    if (pool == NULL) {
        fprintf(stderr, "Impossible de créer le pool\n");
        return 1;
    }

    // Allouer depuis le pool
    int* tableau1 = pool_alloc(pool, 100 * sizeof(int));
    char* buffer = pool_alloc(pool, 1024);
    double* tableau2 = pool_alloc(pool, 50 * sizeof(double));

    if (tableau1 && buffer && tableau2) {
        printf("Allocations réussies depuis le pool\n");

        // Utilisation...
        tableau1[0] = 42;
        buffer[0] = 'A';
        tableau2[0] = 3.14;
    }

    // Libération globale
    destroy_pool(pool);

    return 0;
}
```

### Technique 3 : Mode dégradé

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char* buffer;
    size_t taille;
    int mode_degrade;
} ProcesseurTexte;

ProcesseurTexte* creer_processeur(size_t taille_optimale) {
    ProcesseurTexte* proc = malloc(sizeof(ProcesseurTexte));
    if (proc == NULL) return NULL;

    // Essayer d'allouer la taille optimale
    proc->buffer = malloc(taille_optimale);

    if (proc->buffer != NULL) {
        proc->taille = taille_optimale;
        proc->mode_degrade = 0;
        printf("Mode normal : buffer de %zu octets\n", taille_optimale);
    } else {
        // Basculer en mode dégradé avec buffer plus petit
        size_t taille_min = 1024;  // 1 KB minimum
        proc->buffer = malloc(taille_min);

        if (proc->buffer == NULL) {
            free(proc);
            return NULL;
        }

        proc->taille = taille_min;
        proc->mode_degrade = 1;
        printf("⚠️ Mode dégradé : buffer de %zu octets seulement\n", taille_min);
    }

    return proc;
}

void traiter_texte(ProcesseurTexte* proc, const char* texte) {
    if (proc == NULL || texte == NULL) return;

    size_t longueur = strlen(texte);

    if (longueur >= proc->taille) {
        if (proc->mode_degrade) {
            printf("⚠️ Texte tronqué en mode dégradé\n");
        }
        longueur = proc->taille - 1;
    }

    strncpy(proc->buffer, texte, longueur);
    proc->buffer[longueur] = '\0';

    printf("Texte traité : %s\n", proc->buffer);
}

void liberer_processeur(ProcesseurTexte* proc) {
    if (proc != NULL) {
        free(proc->buffer);
        free(proc);
    }
}

int main(void) {
    ProcesseurTexte* proc = creer_processeur(10 * 1024 * 1024);  // 10 MB

    if (proc == NULL) {
        fprintf(stderr, "Impossible de créer le processeur\n");
        return 1;
    }

    traiter_texte(proc, "Hello, World!");

    liberer_processeur(proc);
    return 0;
}
```

---

## Messages d'erreur informatifs

### Bonnes pratiques pour les messages

**❌ Message peu utile :**
```c
if (ptr == NULL) {
    printf("Erreur\n");
    return 1;
}
```

**✅ Message informatif :**
```c
if (ptr == NULL) {
    fprintf(stderr, "Erreur ligne %d dans %s : ", __LINE__, __FILE__);
    fprintf(stderr, "échec d'allocation de %zu octets\n", taille);
    perror("malloc");
    return 1;
}
```

### Macro pour logs d'erreur

```c
#include <stdio.h>
#include <stdlib.h>

#define LOG_ERROR(fmt, ...) \
    fprintf(stderr, "[ERREUR] %s:%d dans %s() : " fmt "\n", \
            __FILE__, __LINE__, __func__, __VA_ARGS__)

void* allouer_buffer(size_t taille) {
    void* ptr = malloc(taille);

    if (ptr == NULL) {
        LOG_ERROR("échec d'allocation de %zu octets", taille);
        return NULL;
    }

    return ptr;
}

int main(void) {
    void* buffer = allouer_buffer(100);

    if (buffer == NULL) {
        LOG_ERROR("%s", "impossible de créer le buffer principal");
        return 1;
    }

    free(buffer);
    return 0;
}
```

**Sortie typique :**
```
[ERREUR] programme.c:42 dans allouer_buffer() : échec d'allocation de 100 octets
[ERREUR] programme.c:50 dans main() : impossible de créer le buffer principal
```

---

## Assertions et validation

### Utilisation de assert()

```c
#include <assert.h>
#include <stdlib.h>

void traiter_donnees(int* donnees, size_t taille) {
    // Vérifier les préconditions
    assert(donnees != NULL);  // Ne doit jamais être NULL
    assert(taille > 0);       // Taille doit être positive

    // Traitement...
    for (size_t i = 0; i < taille; i++) {
        donnees[i] *= 2;
    }
}

int main(void) {
    int* tableau = malloc(10 * sizeof(int));

    if (tableau == NULL) {
        // Gérer l'erreur normalement
        return 1;
    }

    for (int i = 0; i < 10; i++) {
        tableau[i] = i;
    }

    traiter_donnees(tableau, 10);  // OK
    // traiter_donnees(NULL, 10);  // ⚠️ Assertion échoue en debug

    free(tableau);
    return 0;
}
```

**Note :** `assert()` est désactivé si `NDEBUG` est défini (mode release).

### Validation des paramètres

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

bool valider_parametres(const void* ptr, size_t taille, const char** message) {
    if (ptr != NULL) {
        *message = "Le pointeur doit être NULL pour une nouvelle allocation";
        return false;
    }

    if (taille == 0) {
        *message = "La taille ne peut pas être zéro";
        return false;
    }

    if (taille > 1024 * 1024 * 1024) {  // 1 GB
        *message = "Taille demandée trop grande (> 1 GB)";
        return false;
    }

    return true;
}

void* allouer_securise(size_t taille) {
    const char* erreur = NULL;

    if (!valider_parametres(NULL, taille, &erreur)) {
        fprintf(stderr, "Validation échouée : %s\n", erreur);
        return NULL;
    }

    void* ptr = malloc(taille);

    if (ptr == NULL) {
        fprintf(stderr, "Allocation de %zu octets échouée\n", taille);
        return NULL;
    }

    return ptr;
}

int main(void) {
    void* buffer = allouer_securise(0);  // Échec de validation

    if (buffer == NULL) {
        fprintf(stderr, "Impossible d'allouer le buffer\n");
    }

    return 0;
}
```

---

## Tests de robustesse

### Simuler des échecs d'allocation

Pour tester la gestion d'erreurs, on peut créer un wrapper qui simule des échecs :

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Variables globales pour les tests
static bool test_mode = false;  
static int allocation_counter = 0;  
static int fail_at_allocation = -1;  // -1 = ne jamais échouer  

void set_test_mode(bool enable, int fail_at) {
    test_mode = enable;
    allocation_counter = 0;
    fail_at_allocation = fail_at;
}

void* test_malloc(size_t size) {
    if (test_mode) {
        allocation_counter++;

        if (fail_at_allocation > 0 && allocation_counter == fail_at_allocation) {
            printf("[TEST] Simulation d'échec à l'allocation #%d\n",
                   allocation_counter);
            return NULL;
        }
    }

    return malloc(size);
}

// Code à tester
typedef struct {
    int* data;
    char* name;
    double* values;
} ComplexStruct;

ComplexStruct* create_complex(const char* name, int count) {
    ComplexStruct* obj = test_malloc(sizeof(ComplexStruct));
    if (obj == NULL) {
        fprintf(stderr, "Échec allocation structure\n");
        return NULL;
    }

    obj->data = test_malloc(count * sizeof(int));
    if (obj->data == NULL) {
        fprintf(stderr, "Échec allocation data\n");
        free(obj);
        return NULL;
    }

    obj->name = test_malloc(strlen(name) + 1);
    if (obj->name == NULL) {
        fprintf(stderr, "Échec allocation name\n");
        free(obj->data);
        free(obj);
        return NULL;
    }
    strcpy(obj->name, name);

    obj->values = test_malloc(count * sizeof(double));
    if (obj->values == NULL) {
        fprintf(stderr, "Échec allocation values\n");
        free(obj->name);
        free(obj->data);
        free(obj);
        return NULL;
    }

    printf("Structure créée avec succès\n");
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
    printf("=== Test 1 : Tout réussit ===\n");
    set_test_mode(true, -1);  // Pas d'échec
    ComplexStruct* obj1 = create_complex("test", 10);
    if (obj1) destroy_complex(obj1);

    printf("\n=== Test 2 : Échec à la 2e allocation ===\n");
    set_test_mode(true, 2);  // Échouer à l'allocation #2
    ComplexStruct* obj2 = create_complex("test", 10);
    if (obj2) destroy_complex(obj2);

    printf("\n=== Test 3 : Échec à la 4e allocation ===\n");
    set_test_mode(true, 4);  // Échouer à l'allocation #4
    ComplexStruct* obj3 = create_complex("test", 10);
    if (obj3) destroy_complex(obj3);

    return 0;
}
```

### Utilisation de Valgrind pour tester

```bash
# Compiler avec symboles de debug
gcc -g -o test_erreurs test_erreurs.c

# Tester avec Valgrind
valgrind --leak-check=full --show-leak-kinds=all ./test_erreurs

# Vérifier qu'il n'y a pas de fuites même en cas d'erreur
```

---

## Limites système et ulimit

### Vérifier les limites

```c
#include <stdio.h>
#include <sys/resource.h>

void afficher_limites(void) {
    struct rlimit limit;

    // Limite de taille de la Stack
    if (getrlimit(RLIMIT_STACK, &limit) == 0) {
        printf("Stack : ");
        if (limit.rlim_cur == RLIM_INFINITY) {
            printf("illimitée\n");
        } else {
            printf("%ld octets (%ld MB)\n",
                   (long)limit.rlim_cur,
                   (long)limit.rlim_cur / (1024 * 1024));
        }
    }

    // Limite de taille des données (Heap)
    if (getrlimit(RLIMIT_DATA, &limit) == 0) {
        printf("Heap : ");
        if (limit.rlim_cur == RLIM_INFINITY) {
            printf("illimitée\n");
        } else {
            printf("%ld octets (%ld MB)\n",
                   (long)limit.rlim_cur,
                   (long)limit.rlim_cur / (1024 * 1024));
        }
    }

    // Limite d'adresse mémoire virtuelle
    if (getrlimit(RLIMIT_AS, &limit) == 0) {
        printf("Mémoire virtuelle : ");
        if (limit.rlim_cur == RLIM_INFINITY) {
            printf("illimitée\n");
        } else {
            printf("%ld octets (%ld MB)\n",
                   (long)limit.rlim_cur,
                   (long)limit.rlim_cur / (1024 * 1024));
        }
    }
}

int main(void) {
    afficher_limites();
    return 0;
}
```

### Modifier les limites (ulimit)

```bash
# Afficher toutes les limites
ulimit -a

# Limiter la mémoire virtuelle à 512 MB pour tester
ulimit -v 524288  # en KB
./mon_programme

# Limiter la Stack à 4 MB
ulimit -s 4096
./mon_programme
```

---

## Checklist de gestion d'erreurs robuste

### ✅ Liste de vérification

**Avant l'allocation :**
- [ ] Valider les paramètres (taille > 0, pointeurs NULL, etc.)
- [ ] Vérifier si l'allocation est vraiment nécessaire
- [ ] Estimer si la taille demandée est raisonnable

**Après l'allocation :**
- [ ] Vérifier systématiquement si le retour est NULL
- [ ] Afficher un message d'erreur informatif
- [ ] Libérer les ressources déjà allouées (éviter les fuites)
- [ ] Retourner un code d'erreur ou NULL à l'appelant

**Pendant l'utilisation :**
- [ ] Ne jamais supposer qu'un pointeur est valide
- [ ] Valider les pointeurs avant utilisation
- [ ] Gérer les cas limites (tableaux vides, etc.)

**À la libération :**
- [ ] Libérer dans l'ordre inverse de l'allocation
- [ ] Mettre les pointeurs à NULL après free()
- [ ] Vérifier avec Valgrind qu'il n'y a pas de fuites

---

## Pattern récapitulatif : Fonction robuste complète

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

typedef enum {
    OK = 0,
    ERR_PARAM_NULL,
    ERR_PARAM_INVALID,
    ERR_ALLOCATION
} Status;

/**
 * Crée une copie d'une chaîne avec gestion d'erreurs robuste
 *
 * @param source Chaîne source (ne doit pas être NULL)
 * @param dest Pointeur vers la destination (sera alloué)
 * @return Code de statut
 */
Status dupliquer_chaine_robuste(const char* source, char** dest) {
    // 1. Validation des paramètres
    if (source == NULL) {
        fprintf(stderr, "Erreur : source est NULL\n");
        return ERR_PARAM_NULL;
    }

    if (dest == NULL) {
        fprintf(stderr, "Erreur : pointeur de destination est NULL\n");
        return ERR_PARAM_NULL;
    }

    size_t longueur = strlen(source);

    // Vérification de longueur raisonnable (< 1 MB)
    if (longueur > 1024 * 1024) {
        fprintf(stderr, "Erreur : chaîne trop longue (%zu octets)\n", longueur);
        return ERR_PARAM_INVALID;
    }

    // 2. Allocation
    *dest = malloc(longueur + 1);

    // 3. Vérification de l'allocation
    if (*dest == NULL) {
        fprintf(stderr, "Erreur : allocation de %zu octets échouée\n",
                longueur + 1);

        if (errno != 0) {
            fprintf(stderr, "  errno : %s\n", strerror(errno));
        }

        return ERR_ALLOCATION;
    }

    // 4. Copie des données
    strcpy(*dest, source);

    // 5. Log de succès (optionnel en production)
    #ifdef DEBUG
    printf("Duplication réussie : %zu octets alloués\n", longueur + 1);
    #endif

    return OK;
}

int main(void) {
    char* copie = NULL;
    Status status = dupliquer_chaine_robuste("Hello, World!", &copie);

    if (status != OK) {
        fprintf(stderr, "Échec de la duplication (code %d)\n", status);
        return 1;
    }

    printf("Copie : %s\n", copie);

    free(copie);
    copie = NULL;

    return 0;
}
```

---

## Récapitulatif

### Points clés à retenir

1. **Toujours vérifier le retour** de malloc/calloc/realloc
2. **Gérer les erreurs explicitement** avec codes d'erreur ou messages
3. **Nettoyer les ressources** en cas d'échec partiel (goto cleanup)
4. **Tester la gestion d'erreurs** en simulant des échecs
5. **Messages informatifs** : fichier, ligne, fonction, taille demandée
6. **Ne jamais supposer** qu'une allocation réussira

### Stratégies selon le contexte

| Contexte | Stratégie recommandée |
|----------|----------------------|
| **Bibliothèque** | Retourner NULL ou code d'erreur |
| **Application critique** | Logging + récupération gracieuse |
| **Application simple** | Wrapper avec exit() acceptable |
| **Allocations multiples** | Pattern goto cleanup |
| **Tests** | Simuler des échecs avec wrappers |

### Erreurs à éviter

- ❌ Ne pas vérifier le retour de malloc
- ❌ Perdre des références en cas d'échec (fuites)
- ❌ Messages d'erreur génériques ("Erreur")
- ❌ Ne pas tester les chemins d'erreur
- ❌ Supposer que errno est toujours défini

---

## Pour aller plus loin

### Sections recommandées

- **Section 9.5** : Fuites mémoire : causes et prévention
- **Section 9.6** : Double free et corruption de tas
- **Module 5, Section 15.1** : Sanitizers (AddressSanitizer pour détecter les erreurs)
- **Module 5, Section 15.4** : Valgrind pour détecter les fuites

### Outils de détection

```bash
# AddressSanitizer : détecte les erreurs d'allocation
gcc -fsanitize=address -g programme.c -o programme
./programme

# Valgrind : détecte fuites et erreurs
valgrind --leak-check=full ./programme

# Limiter la mémoire pour tester
ulimit -v 102400  # 100 MB
./programme
```

### Ressources

- CERT C Coding Standard : MEM règles
- "The Practice of Programming" (Kernighan & Pike)
- Linux kernel coding style (utilisation de goto)

---


⏭️ [Fuites mémoire : causes et prévention](/09-allocation-dynamique/05-fuites-memoire.md)
