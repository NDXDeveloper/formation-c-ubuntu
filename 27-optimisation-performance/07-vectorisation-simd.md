🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 27.7 Vectorisation et SIMD

## Introduction

**SIMD** (Single Instruction Multiple Data) est une technique qui permet au processeur d'effectuer **la même opération sur plusieurs données en parallèle** avec une seule instruction. C'est l'un des moyens les plus efficaces d'accélérer du code de calcul intensif.

La **vectorisation** est le processus de transformation du code pour utiliser ces instructions SIMD.

### Pourquoi SIMD est-il important ?

Sur un processeur moderne, les instructions SIMD peuvent traiter :
- **4 entiers de 32 bits** simultanément (128 bits avec SSE)
- **8 entiers de 32 bits** simultanément (256 bits avec AVX)
- **16 entiers de 32 bits** simultanément (512 bits avec AVX-512)

**Résultat :** Un gain de performance potentiel de **4x, 8x ou même 16x** sur du code bien vectorisé !

### Analogie : La caisse du supermarché

**Sans SIMD (scalaire) :**
```
Caissier unique qui scanne les articles un par un
├── Article 1 : bip
├── Article 2 : bip
├── Article 3 : bip
└── Article 4 : bip
Temps total : 4 secondes
```

**Avec SIMD (vectoriel) :**
```
4 caissiers en parallèle qui scannent simultanément
├── Caissier 1 : Article 1 bip ┐
├── Caissier 2 : Article 2 bip ├─ En même temps !
├── Caissier 3 : Article 3 bip │
└── Caissier 4 : Article 4 bip ┘
Temps total : 1 seconde (4x plus rapide)
```

Même principe : une instruction SIMD traite 4 (ou 8, 16) données en même temps qu'une instruction normale en traite une.

---

## Architectures SIMD

### x86-64 (Intel/AMD)

| Extension | Année | Taille registres | Capacité |
|-----------|-------|------------------|----------|
| **SSE** | 1999 | 128 bits | 4 floats ou 2 doubles |
| **SSE2** | 2001 | 128 bits | 4 int32, 8 int16, 16 int8 |
| **SSE3** | 2004 | 128 bits | Améliorations |
| **SSSE3** | 2006 | 128 bits | Plus d'instructions |
| **SSE4** | 2006-2008 | 128 bits | Encore plus |
| **AVX** | 2011 | 256 bits | 8 floats, 4 doubles |
| **AVX2** | 2013 | 256 bits | Opérations entières |
| **AVX-512** | 2016+ | 512 bits | 16 floats, 8 doubles |

### ARM (smartphones, Raspberry Pi, serveurs ARM)

| Extension | Taille registres | Capacité |
|-----------|------------------|----------|
| **NEON** | 128 bits | 4 floats, 4 int32 |
| **SVE** | Variable (128-2048 bits) | Vectorisation adaptative |

### Vérifier les capacités de votre CPU

```bash
# Sur Linux x86-64
lscpu | grep Flags

# Ou plus détaillé
cat /proc/cpuinfo | grep flags | head -1

# Résultat typique :
# flags : ... sse sse2 sse3 ssse3 sse4_1 sse4_2 avx avx2 ...
```

**Interprétation :**
- `sse` à `sse4_2` : Votre CPU supporte SSE (quasi-universel aujourd'hui)
- `avx` : Support AVX (processeurs depuis 2011)
- `avx2` : Support AVX2 (processeurs depuis 2013)
- `avx512f` : Support AVX-512 (processeurs haut de gamme récents)

---

## Comment fonctionne SIMD ?

### Code scalaire (sans SIMD)

```c
// Additionner deux tableaux
float a[4] = {1.0, 2.0, 3.0, 4.0};
float b[4] = {5.0, 6.0, 7.0, 8.0};
float c[4];

// Version scalaire : 4 opérations distinctes
c[0] = a[0] + b[0];  // Instruction 1
c[1] = a[1] + b[1];  // Instruction 2
c[2] = a[2] + b[2];  // Instruction 3
c[3] = a[3] + b[3];  // Instruction 4
```

**Nombre d'instructions CPU :** 4

### Code vectoriel (avec SIMD)

```c
// Version SIMD conceptuelle
__m128 va = _mm_load_ps(a);      // Charger 4 floats dans un registre SIMD
__m128 vb = _mm_load_ps(b);      // Charger 4 floats dans un registre SIMD
__m128 vc = _mm_add_ps(va, vb);  // UNE SEULE instruction : additionner 4 floats !
_mm_store_ps(c, vc);             // Stocker le résultat
```

**Nombre d'instructions CPU :** 1 (plus load/store)

**Représentation visuelle :**

```
Registre SIMD (128 bits) :
┌────────┬────────┬────────┬────────┐
│  1.0   │  2.0   │  3.0   │  4.0   │  a
└────────┴────────┴────────┴────────┘
┌────────┬────────┬────────┬────────┐
│  5.0   │  6.0   │  7.0   │  8.0   │  b
└────────┴────────┴────────┴────────┘
         ADD (une seule instruction)
         ↓
┌────────┬────────┬────────┬────────┐
│  6.0   │  8.0   │ 10.0   │ 12.0   │  c
└────────┴────────┴────────┴────────┘
```

---

## Vectorisation automatique par le compilateur

### Comment activer la vectorisation automatique

GCC peut automatiquement transformer certaines boucles en code SIMD avec les bons flags :

```bash
# Activer la vectorisation (incluse dans -O3)
gcc -O3 programme.c -o programme

# Ou explicitement
gcc -O2 -ftree-vectorize programme.c -o programme

# Optimiser pour le CPU local (active les instructions disponibles)
gcc -O3 -march=native programme.c -o programme

# Voir les optimisations de vectorisation appliquées
gcc -O3 -ftree-vectorize -fopt-info-vec-optimized programme.c
```

### Exemple : Addition de tableaux

```c
// add_arrays.c
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define SIZE 10000000  // 10 millions

void add_arrays(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

int main() {
    float *a = malloc(SIZE * sizeof(float));
    float *b = malloc(SIZE * sizeof(float));
    float *c = malloc(SIZE * sizeof(float));

    // Initialiser
    for (int i = 0; i < SIZE; i++) {
        a[i] = (float)i;
        b[i] = (float)i * 2.0f;
    }

    clock_t start = clock();
    add_arrays(a, b, c, SIZE);
    clock_t end = clock();

    printf("Temps: %.3f ms\n",
           (double)(end - start) * 1000.0 / CLOCKS_PER_SEC);

    free(a); free(b); free(c);
    return 0;
}
```

**Compilation et test :**

```bash
# Sans vectorisation
gcc -O2 add_arrays.c -o add_O2
./add_O2
# Résultat : ~45 ms

# Avec vectorisation automatique
gcc -O3 -march=native add_arrays.c -o add_O3
./add_O3
# Résultat : ~12 ms  ← 3.7x plus rapide !

# Vérifier la vectorisation
gcc -O3 -march=native -fopt-info-vec-optimized add_arrays.c
# Sortie : add_arrays.c:8:5: optimized: loop vectorized using 256 bit vectors
```

**Explication :** Le compilateur a détecté que la boucle est vectorisable et a généré du code AVX (256 bits = 8 floats en parallèle).

### Voir le code assembleur généré

```bash
gcc -O3 -march=native -S add_arrays.c -o add_arrays.s
cat add_arrays.s | grep -A 10 "add_arrays:"
```

Vous devriez voir des instructions comme `vaddps` (AVX) ou `addps` (SSE) au lieu de `addss` (scalaire).

---

## Conditions pour la vectorisation automatique

Le compilateur peut vectoriser une boucle si :

### ✅ Conditions favorables

1. **Boucle simple** : Un seul compteur, pas de `break` ou `continue` complexes
2. **Pas de dépendances de données** : `a[i]` ne dépend pas de `a[i-1]`
3. **Accès mémoire contigus** : `a[i]`, `a[i+1]`, `a[i+2]`... (pas `a[random]`)
4. **Nombre d'itérations connu** ou facilement calculable
5. **Alignement mémoire** : Données alignées sur 16/32 octets (optimal)
6. **Opérations simples** : Addition, multiplication, comparaison, etc.

### ❌ Obstacles à la vectorisation

```c
// ❌ Dépendance de données
for (int i = 1; i < n; i++) {
    a[i] = a[i] + a[i-1];  // a[i] dépend de a[i-1], pas vectorisable
}

// ❌ Accès non contigus
for (int i = 0; i < n; i++) {
    c[i] = a[i] + b[i * 2];  // Stride = 2 pour b, moins efficace
}

// ❌ Branchements complexes
for (int i = 0; i < n; i++) {
    if (condition_complexe(a[i])) {
        c[i] = calcul1(a[i]);
    } else {
        c[i] = calcul2(a[i]);
    }
}

// ❌ Appels de fonction
for (int i = 0; i < n; i++) {
    c[i] = fonction_externe(a[i]);  // Difficile à vectoriser
}
```

### Comment aider le compilateur

#### a) Utiliser `restrict`

```c
// Indiquer au compilateur que les pointeurs ne se chevauchent pas
void add_arrays(float *restrict a, float *restrict b,
                float *restrict c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}
```

#### b) Aligner les données

```c
// Aligner sur 32 bytes (pour AVX)
float *a = aligned_alloc(32, SIZE * sizeof(float));

// Ou avec GCC
float a[SIZE] __attribute__((aligned(32)));
```

#### c) Annotations pour le compilateur

```c
// Indiquer explicitement qu'une boucle est vectorisable (GCC/Clang)
#pragma GCC ivdep  // Ignore Vector Dependencies
for (int i = 0; i < n; i++) {
    c[i] = a[i] + b[i];
}

// OpenMP SIMD
#pragma omp simd
for (int i = 0; i < n; i++) {
    c[i] = a[i] + b[i];
}
```

---

## Vectorisation manuelle avec intrinsics

Pour un contrôle total, on peut écrire du code SIMD directement avec les **intrinsics** (fonctions built-in du compilateur).

### Introduction aux intrinsics SSE/AVX

Les intrinsics sont des fonctions C qui correspondent **directement** à des instructions assembleur SIMD.

**Convention de nommage :**
```
_mm_<operation>_<type>

Exemples :
_mm_add_ps   → Addition (add) de floats (ps = packed single precision)
_mm_mul_pd   → Multiplication (mul) de doubles (pd = packed double precision)
_mm_load_si128 → Chargement (load) d'entiers (si128 = signed integers 128 bits)
```

**Types de données :**
```c
__m128   : 4 floats (128 bits)
__m128d  : 2 doubles (128 bits)
__m128i  : Entiers (128 bits)

__m256   : 8 floats (256 bits, AVX)
__m256d  : 4 doubles (256 bits, AVX)
__m256i  : Entiers (256 bits, AVX)
```

### Exemple 1 : Addition de floats (SSE)

```c
// add_sse.c
#include <stdio.h>
#include <xmmintrin.h>  // SSE
#include <time.h>

#define SIZE 10000000

// Version scalaire
void add_scalar(float *a, float *b, float *c, int n) {
    for (int i = 0; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

// Version SSE (4 floats à la fois)
void add_sse(float *a, float *b, float *c, int n) {
    int i;

    // Traiter 4 éléments à la fois
    for (i = 0; i <= n - 4; i += 4) {
        __m128 va = _mm_load_ps(&a[i]);   // Charger 4 floats de a
        __m128 vb = _mm_load_ps(&b[i]);   // Charger 4 floats de b
        __m128 vc = _mm_add_ps(va, vb);   // Additionner
        _mm_store_ps(&c[i], vc);          // Stocker le résultat
    }

    // Traiter les éléments restants (si n n'est pas multiple de 4)
    for (; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

int main() {
    // Allouer avec alignement pour SSE (16 bytes)
    float *a = aligned_alloc(16, SIZE * sizeof(float));
    float *b = aligned_alloc(16, SIZE * sizeof(float));
    float *c = aligned_alloc(16, SIZE * sizeof(float));

    for (int i = 0; i < SIZE; i++) {
        a[i] = (float)i;
        b[i] = (float)i * 2.0f;
    }

    clock_t start, end;

    // Test scalaire
    start = clock();
    add_scalar(a, b, c, SIZE);
    end = clock();
    printf("Scalaire: %.3f ms\n",
           (double)(end - start) * 1000.0 / CLOCKS_PER_SEC);

    // Test SSE
    start = clock();
    add_sse(a, b, c, SIZE);
    end = clock();
    printf("SSE:      %.3f ms\n",
           (double)(end - start) * 1000.0 / CLOCKS_PER_SEC);

    free(a); free(b); free(c);
    return 0;
}
```

**Compilation :**

```bash
gcc -O2 -msse add_sse.c -o add_sse
./add_sse
```

**Résultats typiques :**
```
Scalaire: 45.2 ms
SSE:      12.8 ms  ← 3.5x plus rapide
```

### Exemple 2 : Addition de floats (AVX)

```c
// add_avx.c
#include <stdio.h>
#include <immintrin.h>  // AVX
#include <time.h>

#define SIZE 10000000

void add_avx(float *a, float *b, float *c, int n) {
    int i;

    // Traiter 8 éléments à la fois avec AVX
    for (i = 0; i <= n - 8; i += 8) {
        __m256 va = _mm256_load_ps(&a[i]);   // Charger 8 floats
        __m256 vb = _mm256_load_ps(&b[i]);
        __m256 vc = _mm256_add_ps(va, vb);   // Additionner 8 floats en une fois
        _mm256_store_ps(&c[i], vc);
    }

    // Reste
    for (; i < n; i++) {
        c[i] = a[i] + b[i];
    }
}

int main() {
    float *a = aligned_alloc(32, SIZE * sizeof(float));  // Alignement 32 bytes pour AVX
    float *b = aligned_alloc(32, SIZE * sizeof(float));
    float *c = aligned_alloc(32, SIZE * sizeof(float));

    for (int i = 0; i < SIZE; i++) {
        a[i] = (float)i;
        b[i] = (float)i * 2.0f;
    }

    clock_t start = clock();
    add_avx(a, b, c, SIZE);
    clock_t end = clock();

    printf("AVX: %.3f ms\n",
           (double)(end - start) * 1000.0 / CLOCKS_PER_SEC);

    free(a); free(b); free(c);
    return 0;
}
```

**Compilation :**

```bash
gcc -O2 -mavx add_avx.c -o add_avx
./add_avx
```

**Résultats :**
```
AVX: 7.2 ms  ← 6.3x plus rapide que le code scalaire !
```

---

## Intrinsics courants

### Opérations arithmétiques (SSE)

```c
// Addition
__m128 _mm_add_ps(__m128 a, __m128 b);        // 4 floats
__m128d _mm_add_pd(__m128d a, __m128d b);     // 2 doubles

// Soustraction
__m128 _mm_sub_ps(__m128 a, __m128 b);

// Multiplication
__m128 _mm_mul_ps(__m128 a, __m128 b);

// Division
__m128 _mm_div_ps(__m128 a, __m128 b);

// Racine carrée
__m128 _mm_sqrt_ps(__m128 a);

// Minimum/Maximum
__m128 _mm_min_ps(__m128 a, __m128 b);
__m128 _mm_max_ps(__m128 a, __m128 b);
```

### Opérations de chargement/stockage

```c
// Chargement aligné (IMPORTANT : adresse doit être alignée sur 16 bytes)
__m128 _mm_load_ps(const float *p);

// Chargement non aligné (plus lent)
__m128 _mm_loadu_ps(const float *p);

// Stockage aligné
void _mm_store_ps(float *p, __m128 a);

// Stockage non aligné
void _mm_storeu_ps(float *p, __m128 a);

// Définir des valeurs
__m128 _mm_set_ps(float e3, float e2, float e1, float e0);  // Ordre inversé !
__m128 _mm_set1_ps(float a);  // Répéter 'a' dans les 4 positions
__m128 _mm_setzero_ps();      // Mettre à zéro
```

### Opérations logiques et comparaisons

```c
// ET logique
__m128 _mm_and_ps(__m128 a, __m128 b);

// OU logique
__m128 _mm_or_ps(__m128 a, __m128 b);

// Comparaisons (retourne un masque)
__m128 _mm_cmpeq_ps(__m128 a, __m128 b);   // a == b
__m128 _mm_cmplt_ps(__m128 a, __m128 b);   // a < b
__m128 _mm_cmpgt_ps(__m128 a, __m128 b);   // a > b
```

---

## Cas d'étude : Produit scalaire

Le **produit scalaire** (dot product) de deux vecteurs est une opération fréquente en calcul scientifique et graphisme.

```
dot(a, b) = a[0]*b[0] + a[1]*b[1] + ... + a[n-1]*b[n-1]
```

### Version scalaire

```c
float dot_product_scalar(float *a, float *b, int n) {
    float sum = 0.0f;
    for (int i = 0; i < n; i++) {
        sum += a[i] * b[i];
    }
    return sum;
}
```

**Complexité :** O(n), mais séquentiel.

### Version SSE

```c
#include <xmmintrin.h>

float dot_product_sse(float *a, float *b, int n) {
    __m128 vsum = _mm_setzero_ps();  // Accumulateur vectoriel (4 floats)
    int i;

    // Traiter 4 éléments à la fois
    for (i = 0; i <= n - 4; i += 4) {
        __m128 va = _mm_load_ps(&a[i]);
        __m128 vb = _mm_load_ps(&b[i]);
        __m128 vmul = _mm_mul_ps(va, vb);     // Multiplier 4 paires
        vsum = _mm_add_ps(vsum, vmul);        // Accumuler
    }

    // Réduire le vecteur à un scalaire (horizontal add)
    // vsum contient [s0, s1, s2, s3]
    // On veut sum = s0 + s1 + s2 + s3

    // Méthode 1 : Extraire et additionner
    float result[4];
    _mm_store_ps(result, vsum);
    float sum = result[0] + result[1] + result[2] + result[3];

    // Traiter les éléments restants
    for (; i < n; i++) {
        sum += a[i] * b[i];
    }

    return sum;
}
```

**Amélioration avec SSE3 (horizontal add) :**

```c
#include <pmmintrin.h>  // SSE3

float dot_product_sse3(float *a, float *b, int n) {
    __m128 vsum = _mm_setzero_ps();
    int i;

    for (i = 0; i <= n - 4; i += 4) {
        __m128 va = _mm_load_ps(&a[i]);
        __m128 vb = _mm_load_ps(&b[i]);
        __m128 vmul = _mm_mul_ps(va, vb);
        vsum = _mm_add_ps(vsum, vmul);
    }

    // Horizontal add (SSE3)
    vsum = _mm_hadd_ps(vsum, vsum);  // [s0+s1, s2+s3, s0+s1, s2+s3]
    vsum = _mm_hadd_ps(vsum, vsum);  // [s0+s1+s2+s3, ...]

    float sum;
    _mm_store_ss(&sum, vsum);  // Extraire le premier élément

    // Reste
    for (; i < n; i++) {
        sum += a[i] * b[i];
    }

    return sum;
}
```

### Benchmark

```c
#define SIZE 10000000

int main() {
    float *a = aligned_alloc(16, SIZE * sizeof(float));
    float *b = aligned_alloc(16, SIZE * sizeof(float));

    for (int i = 0; i < SIZE; i++) {
        a[i] = (float)i;
        b[i] = 1.0f;
    }

    clock_t start, end;
    float result;

    // Scalaire
    start = clock();
    result = dot_product_scalar(a, b, SIZE);
    end = clock();
    printf("Scalaire: %.3f ms (résultat: %.2f)\n",
           (double)(end - start) * 1000.0 / CLOCKS_PER_SEC, result);

    // SSE
    start = clock();
    result = dot_product_sse(a, b, SIZE);
    end = clock();
    printf("SSE:      %.3f ms (résultat: %.2f)\n",
           (double)(end - start) * 1000.0 / CLOCKS_PER_SEC, result);

    free(a); free(b);
    return 0;
}
```

**Résultats typiques :**
```
Scalaire: 52.3 ms (résultat: 49999995000000.00)
SSE:      14.2 ms (résultat: 49999995000000.00)  ← 3.7x plus rapide
```

---

## Exemple avancé : Filtre d'image (convolution)

Les filtres d'image (flou, détection de contours) utilisent massivement SIMD.

### Flou gaussien simple (moyenne des pixels voisins)

**Version scalaire :**

```c
void blur_scalar(unsigned char *src, unsigned char *dst,
                 int width, int height) {
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sum = 0;
            // Moyenne 3x3
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    sum += src[(y + dy) * width + (x + dx)];
                }
            }
            dst[y * width + x] = sum / 9;
        }
    }
}
```

**Version SSE (traiter 16 pixels à la fois) :**

```c
#include <emmintrin.h>  // SSE2

void blur_sse(unsigned char *src, unsigned char *dst,
              int width, int height) {
    for (int y = 1; y < height - 1; y++) {
        int x;
        for (x = 1; x <= width - 17; x += 16) {  // 16 pixels à la fois
            __m128i sum = _mm_setzero_si128();

            // Accumuler les 9 positions
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    // Charger 16 bytes (16 pixels)
                    __m128i pixels = _mm_loadu_si128(
                        (__m128i*)&src[(y + dy) * width + (x + dx)]
                    );

                    // Convertir en 16-bit pour éviter l'overflow
                    __m128i pixels_lo = _mm_unpacklo_epi8(pixels, _mm_setzero_si128());
                    __m128i pixels_hi = _mm_unpackhi_epi8(pixels, _mm_setzero_si128());

                    // Accumuler
                    sum = _mm_add_epi16(sum, pixels_lo);
                    sum = _mm_add_epi16(sum, pixels_hi);
                }
            }

            // Diviser par 9 (approximation : shift de 3 bits = division par 8)
            sum = _mm_srli_epi16(sum, 3);  // >> 3

            // Reconvertir en 8-bit et stocker
            sum = _mm_packus_epi16(sum, _mm_setzero_si128());
            _mm_storel_epi64((__m128i*)&dst[y * width + x], sum);
        }

        // Traiter les pixels restants
        for (; x < width - 1; x++) {
            // Version scalaire
        }
    }
}
```

**Gain typique :** 8-12x plus rapide pour une image 1920x1080.

---

## Pièges et limitations

### 1. Alignement mémoire

**Problème :** Les instructions `_mm_load_ps` nécessitent un alignement sur 16 bytes (SSE) ou 32 bytes (AVX).

```c
float a[4] = {1.0, 2.0, 3.0, 4.0};  // Peut ne PAS être aligné !
__m128 va = _mm_load_ps(a);         // PEUT PLANTER (segfault) si pas aligné
```

**Solutions :**

```c
// 1. Utiliser aligned_alloc
float *a = aligned_alloc(16, 4 * sizeof(float));

// 2. Attribut de compilation
float a[4] __attribute__((aligned(16))) = {1.0, 2.0, 3.0, 4.0};

// 3. Utiliser loadu (non aligné, mais plus lent)
__m128 va = _mm_loadu_ps(a);  // Fonctionne toujours, légèrement plus lent
```

### 2. Boucles non divisibles

Si le nombre d'éléments n'est pas un multiple de la taille du vecteur, il faut traiter les éléments restants :

```c
// Mauvais : oublie les derniers éléments
for (int i = 0; i < n; i += 4) {  // Si n=10, les éléments 8 et 9 sont ignorés !
    // ...
}

// Correct
int i;
for (i = 0; i <= n - 4; i += 4) {
    // Traitement vectoriel
}
for (; i < n; i++) {
    // Traitement scalaire des éléments restants
}
```

### 3. Portabilité

Le code SIMD est **spécifique à l'architecture** :
- Code SSE/AVX → Seulement x86-64
- Code NEON → Seulement ARM

**Solutions :**

```c
// Compilation conditionnelle
#ifdef __SSE__
    // Code SSE
#else
    // Code scalaire de fallback
#endif

// Ou détection à l'exécution
if (cpu_supports_avx2()) {
    add_avx2(a, b, c, n);
} else if (cpu_supports_sse()) {
    add_sse(a, b, c, n);
} else {
    add_scalar(a, b, c, n);
}
```

### 4. Complexité du code

Le code SIMD est **beaucoup plus verbeux et difficile à maintenir** que le code scalaire.

**Recommandation :** Utilisez la vectorisation automatique (avec `-O3 -march=native`) en priorité. N'écrivez du code SIMD manuel que si :
- Le gain est significatif (>2x)
- C'est un goulot d'étranglement prouvé par profiling
- La vectorisation automatique ne fonctionne pas

---

## Vérifier la vectorisation

### Avec les flags de compilation

```bash
# Voir les boucles vectorisées
gcc -O3 -ftree-vectorize -fopt-info-vec-optimized test.c

# Voir pourquoi certaines boucles ne sont PAS vectorisées
gcc -O3 -ftree-vectorize -fopt-info-vec-missed test.c

# Tout voir
gcc -O3 -ftree-vectorize -fopt-info-vec-all test.c
```

**Exemple de sortie :**

```
test.c:10:5: optimized: loop vectorized using 256 bit vectors
test.c:25:5: missed: couldn't vectorize loop
test.c:25:5: missed: not vectorized: data ref analysis failed
```

### Avec objdump (code assembleur)

```bash
gcc -O3 -march=native test.c -o test
objdump -d test | grep -E "addps|vaddps|addpd|vaddpd"
```

**Instructions à chercher :**
- `addps`, `mulps`, `subps` → SSE
- `vaddps`, `vmulps` → AVX
- `vaddpd`, `vmulpd` → AVX (doubles)

### Avec perf

```bash
# Compter les instructions SIMD exécutées
perf stat -e fp_arith_inst_retired.128b_packed_double \
          -e fp_arith_inst_retired.256b_packed_double ./test

# Intel seulement, événements spécifiques au CPU
```

---

## Bibliothèques utilisant SIMD

Plutôt que d'écrire du code SIMD manuel, vous pouvez utiliser des bibliothèques optimisées :

### Bibliothèques populaires

| Bibliothèque | Domaine | Notes |
|--------------|---------|-------|
| **Intel MKL** | Algèbre linéaire | Propriétaire, très optimisé |
| **OpenBLAS** | Algèbre linéaire | Open-source, excellent |
| **FFTW** | Transformées de Fourier | Optimisations SIMD |
| **Eigen** | Algèbre linéaire (C++) | Header-only, excellent |
| **libsimdpp** | Abstraction SIMD | Multi-plateforme |
| **Highway** | Abstraction SIMD | Moderne, de Google |
| **libjpeg-turbo** | JPEG | Utilise SIMD en interne |
| **libvpx** | Vidéo VP8/VP9 | Optimisé SIMD |

**Exemple avec OpenBLAS :**

```c
#include <cblas.h>

// Produit matrice-vecteur (ultra-optimisé avec SIMD)
cblas_sgemv(CblasRowMajor, CblasNoTrans,
            m, n, 1.0, A, n, x, 1, 0.0, y, 1);
```

OpenBLAS utilisera automatiquement AVX2 ou AVX-512 si disponible.

---

## Bonnes pratiques

### ✅ À faire

1. **Commencer par la vectorisation automatique** (`-O3 -march=native`)
2. **Profiler avant d'optimiser** (identifier les vrais goulots)
3. **Aligner les données** (16 ou 32 bytes)
4. **Utiliser `restrict`** pour éviter l'aliasing
5. **Tester sur plusieurs CPUs** (SSE, AVX, AVX2)
6. **Fournir un fallback scalaire** pour la portabilité
7. **Utiliser des bibliothèques existantes** quand possible
8. **Documenter le code SIMD** (c'est complexe !)

### ❌ À éviter

1. **Vectoriser du code non critique** (perte de temps)
2. **Oublier les éléments restants** (bugs subtils)
3. **Ignorer l'alignement** (segfault ou performance dégradée)
4. **Vectoriser des boucles avec dépendances** (résultats incorrects)
5. **Écrire du SIMD sans benchmark** (gain non vérifié)
6. **Sacrifier la lisibilité** sans gain significatif

---

## Checklist : Ai-je besoin de SIMD ?

Avant de vectoriser manuellement :

- ✅ Ai-je **profileé** et identifié un goulot CPU ?
- ✅ Ce goulot est-il dans **une boucle de calcul** ?
- ✅ Ai-je essayé `-O3 -march=native` ? Quel gain ?
- ✅ La vectorisation automatique échoue-t-elle ?
- ✅ Le gain potentiel justifie-t-il la complexité ?
- ✅ Puis-je maintenir le code SIMD dans le temps ?

Si vous répondez **OUI** à toutes ces questions, alors SIMD manuel peut valoir le coup.

Sinon, restez sur la vectorisation automatique !

---

## Ressources pour aller plus loin

### Documentation officielle

- **Intel Intrinsics Guide** : https://www.intel.com/content/www/us/en/docs/intrinsics-guide/
  (Référence complète de toutes les intrinsics, avec exemples)
- **GCC Vector Extensions** : https://gcc.gnu.org/onlinedocs/gcc/Vector-Extensions.html
- **ARM NEON** : https://developer.arm.com/architectures/instruction-sets/simd-isas/neon

### Livres et cours

- **"Computer Organization and Design"** — Patterson & Hennessy (chapitre sur SIMD)
- **"Optimizing software in C++"** — Agner Fog (gratuit, couvre SIMD en détail)
- **Intel Optimization Manual** — Guide officiel d'optimisation

### Outils

```bash
# Compiler Explorer (voir le code assembleur en ligne)
https://godbolt.org/

# Intel SDE (simuler AVX-512 sans le hardware)
https://www.intel.com/content/www/us/en/developer/articles/tool/software-development-emulator.html
```

---

## Résumé

SIMD est un outil puissant pour accélérer le code de calcul :

1. ✅ **Gain potentiel énorme** : 4x à 16x sur du code bien vectorisé
2. ✅ **Vectorisation automatique** : GCC fait un excellent travail avec `-O3 -march=native`
3. ✅ **Intrinsics manuels** : Pour contrôle total, mais complexe
4. ✅ **Cas d'usage idéaux** : Calculs sur tableaux, traitement d'images, algèbre linéaire
5. ✅ **Pièges** : Alignement, portabilité, complexité

**Hiérarchie des optimisations (rappel) :**

```
1. Bon algorithme (O(n) vs O(n²))          → 100-1000x
2. Structure de données adaptée            → 10-100x
3. SIMD vectorisation                      → 4-16x        ← Nous sommes ici
4. Cache awareness                         → 2-10x
5. Flags de compilation                    → 1.5-3x
```

**Citation :**

> "The best SIMD code is the code you didn't have to write yourself" — Anonyme

**Règle d'or :** Laissez le compilateur vectoriser automatiquement (`-O3 -march=native`). N'écrivez du SIMD manuel qu'en dernier recours, après avoir profileé et confirmé le gain !

---

*Prochaine section : 27.8 Link-Time Optimization (LTO)*

⏭️ [Link-Time Optimization (LTO)](/27-optimisation-performance/08-lto.md)
