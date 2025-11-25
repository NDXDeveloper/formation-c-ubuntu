🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 1.3 Normes du langage : ANSI C, C99, C11, C17, C23

## Introduction

Le langage C existe depuis plus de 50 ans, mais il n'est pas resté figé. Au fil des décennies, il a évolué à travers différentes **normes** (ou **standards**) qui définissent précisément ce qui constitue le langage C et comment il doit se comporter.

Comprendre ces différentes versions est essentiel pour :
- Savoir quelle syntaxe et quelles fonctionnalités vous pouvez utiliser
- Comprendre le code existant que vous lirez
- Choisir le bon standard pour vos projets
- Éviter les incompatibilités et les surprises

Dans cette section, nous allons explorer l'évolution du C à travers ses normes principales, de la première standardisation en 1989 jusqu'à la toute récente norme C23.

> 💡 **Rassurez-vous** : Vous n'avez pas besoin de connaître tous les détails de chaque version. L'important est de comprendre les différences majeures et de savoir quelle version utiliser.

---

## 📖 Qu'est-ce qu'une norme (standard) ?

### Définition

Une **norme du langage C** est un document officiel publié par l'ISO (International Organization for Standardization) qui définit :

- **La syntaxe** : Comment écrire du code C valide
- **La sémantique** : Ce que chaque instruction doit faire
- **La bibliothèque standard** : Quelles fonctions sont disponibles
- **Le comportement** : Comment le code doit s'exécuter

### Pourquoi standardiser ?

**Sans norme**, chaque compilateur pourrait interpréter le C différemment :
```c
// Ce code pourrait faire des choses différentes selon le compilateur !
int x = 5 / 2;  // 2 ou 2.5 ? Arrondi ou troncature ?
```

**Avec une norme**, tous les compilateurs conformes produisent le même résultat :
```c
int x = 5 / 2;  // Toujours 2 (division entière définie par la norme)
```

### Les avantages de la standardisation

- ✅ **Portabilité** : Votre code fonctionne sur différents compilateurs et plateformes
- ✅ **Prévisibilité** : Vous savez exactement ce que votre code fait
- ✅ **Compatibilité** : Les bibliothèques et outils fonctionnent ensemble
- ✅ **Pérennité** : Votre code reste valide pendant des décennies

---

## 🕰️ Historique des normes C

### Chronologie complète

```
1972 ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━ 2025
 │                                                                │
 │ C créé (K&R C)
 │
 └─── 1978: "The C Programming Language" (Kernighan & Ritchie)
       │
       └─── 1989: ANSI C (C89) ─── Première standardisation
             │
             └─── 1990: ISO C (C90) ─── Version internationale
                   │
                   └─── 1999: C99 ─── Évolution majeure
                         │
                         └─── 2011: C11 ─── Threads & atomiques
                               │
                               └─── 2017: C17 ─── Corrections
                                     │
                                     └─── 2023: C23 ─── Moderne

```

### Tableau récapitulatif

| Année | Nom officiel | Nom courant | Type | Importance |
|-------|--------------|-------------|------|------------|
| 1978 | - | K&R C | Livre | 🟡 Historique |
| 1989 | ANSI X3.159-1989 | **C89** | Norme ANSI | 🟢 Legacy |
| 1990 | ISO/IEC 9899:1990 | **C90** | Norme ISO | 🟢 Legacy |
| 1999 | ISO/IEC 9899:1999 | **C99** | Révision majeure | 🔵 Largement utilisé |
| 2011 | ISO/IEC 9899:2011 | **C11** | Révision majeure | 🟣 **Standard actuel** ⭐ |
| 2017 | ISO/IEC 9899:2018 | **C17** (ou C18) | Corrections | 🟣 **Standard actuel** ⭐ |
| 2023 | ISO/IEC 9899:2023 | **C23** | Révision majeure | 🆕 Nouvelle norme |

> 🎯 **À retenir** : En 2025, **C11/C17** est la version la plus utilisée en production, bien que C23 soit désormais disponible.

---

## 📋 Les grandes étapes de l'évolution du C

### Phase 1 : K&R C (1972-1989) - Le C original

**Contexte** : Dennis Ritchie crée le C pour écrire UNIX. Le langage est décrit dans le livre *"The C Programming Language"* de Kernighan et Ritchie (d'où "K&R C").

**Caractéristiques** :
- Pas de norme officielle
- Chaque implémentation avait ses variations
- Syntaxe simple mais peu de sécurité
- Pas de prototypes de fonctions stricts

**Exemple K&R C** (syntaxe ancienne) :
```c
/* Déclaration de fonction à l'ancienne */
int add(a, b)
int a, b;  /* Types déclarés séparément */
{
    return a + b;
}
```

**État en 2025** : Obsolète, mais vous pouvez encore trouver du vieux code écrit ainsi.

---

### Phase 2 : ANSI C / C89 / C90 (1989-1999) - La première standardisation

**Contexte** : Le C devient populaire. Pour éviter la fragmentation, l'ANSI (American National Standards Institute) crée le premier standard officiel en 1989, adopté par l'ISO en 1990.

**Innovation majeure** : Normalisation du langage et de la bibliothèque standard.

**Nouveautés principales** :
- ✅ **Prototypes de fonctions** : Vérification des types à la compilation
- ✅ **Qualificateurs** : `const`, `volatile`
- ✅ **Bibliothèque standard** : `<stdio.h>`, `<stdlib.h>`, `<string.h>`, etc.
- ✅ **Préprocesseur standardisé** : `#define`, `#include`, `#ifdef`
- ✅ **Types** : `void`, `enum`, structures

**Exemple C89** (syntaxe moderne) :
```c
#include <stdio.h>

/* Prototype avec types vérifiés */
int add(int a, int b);

int add(int a, int b) {
    return a + b;
}

int main(void) {
    printf("5 + 3 = %d\n", add(5, 3));
    return 0;
}
```

**État en 2025** : Encore utilisé dans certains systèmes embarqués ultra-contraints et code legacy très ancien.

---

### Phase 3 : C99 (1999-2011) - La modernisation

**Contexte** : 10 ans après C89, le C évolue pour rester compétitif face à C++ et Java. C99 apporte de nombreuses améliorations.

**Nouveautés majeures** :

#### 1. Types améliorés
```c
#include <stdint.h>
#include <stdbool.h>

int64_t big_number;     // Entier 64 bits garanti
bool is_valid = true;   // Type booléen natif
```

#### 2. Déclarations flexibles
```c
// Variables déclarées n'importe où (pas seulement en début de bloc)
int x = 10;
printf("%d\n", x);
int y = 20;  // OK en C99 !
```

#### 3. Boucles for avec déclaration
```c
for (int i = 0; i < 10; i++) {  // i déclaré dans le for
    printf("%d ", i);
}
```

#### 4. Tableaux de longueur variable (VLA)
```c
int n = 10;
int array[n];  // Taille définie à l'exécution !
```

#### 5. Commentaires //
```c
// Commentaires sur une ligne (comme en C++)
int x = 5;  // Au lieu de /* ... */
```

#### 6. Initialisation désignée
```c
struct Point {
    int x, y, z;
};

struct Point p = {.x = 10, .z = 30};  // y est implicitement 0
```

#### 7. Types complexes
```c
#include <complex.h>
double complex z = 1.0 + 2.0*I;
```

#### 8. Fonctions inline
```c
inline int max(int a, int b) {
    return a > b ? a : b;
}
```

**État en 2025** : Très largement utilisé, excellent choix pour la compatibilité maximale.

---

### Phase 4 : C11 (2011-2017) - La concurrence moderne

**Contexte** : L'informatique devient multi-cœurs. C11 intègre le support natif du multithreading et de la programmation concurrente.

**Nouveautés majeures** :

#### 1. Threads natifs 🔥
```c
#include <threads.h>

int thread_function(void *arg) {
    printf("Thread en cours\n");
    return 0;
}

int main(void) {
    thrd_t thread;
    thrd_create(&thread, thread_function, NULL);
    thrd_join(thread, NULL);
    return 0;
}
```

#### 2. Types atomiques 🔥
```c
#include <stdatomic.h>

atomic_int counter = ATOMIC_VAR_INIT(0);

void increment() {
    atomic_fetch_add(&counter, 1);  // Thread-safe !
}
```

#### 3. Sélection générique (`_Generic`)
```c
#define abs(x) _Generic((x), \
    int: abs_int, \
    float: fabsf, \
    double: fabs \
)(x)
```

#### 4. Assertions statiques
```c
static_assert(sizeof(int) == 4, "int doit faire 4 octets");
```

#### 5. Structures anonymes
```c
struct {
    int x, y;
} point = {10, 20};
```

#### 6. Alignement mémoire
```c
#include <stdalign.h>
alignas(16) float vector[4];  // Aligné sur 16 octets
```

#### 7. Unicode amélioré
```c
char16_t utf16_string[] = u"Hello 世界";
char32_t utf32_string[] = U"Hello 世界";
```

**État en 2025** : **Standard recommandé en production** ⭐. Support universel, mature et stable.

---

### Phase 5 : C17 (2017-2023) - Les corrections

**Contexte** : C17 (officiellement publié en 2018, d'où son nom alternatif C18) n'est pas une nouvelle version majeure. C'est une **version de maintenance** qui corrige des bugs et ambiguïtés de C11.

**Changements** :
- ❌ Aucune nouvelle fonctionnalité
- ✅ Corrections de bugs dans la spécification
- ✅ Clarifications de comportements ambigus
- ✅ Amélioration de la cohérence du standard

**En pratique** : C17 = C11 corrigé. Si votre compilateur supporte C17, utilisez-le plutôt que C11.

```bash
gcc -std=c17 program.c  # Identique à C11 en pratique, mais "officiel"
```

**État en 2025** : Équivalent à C11 pour les développeurs. Version stable recommandée.

---

### Phase 6 : C23 (2023-présent) - La modernisation

**Contexte** : Après 12 ans sans évolution majeure, C23 apporte de nouvelles fonctionnalités pour moderniser le langage tout en restant compatible.

**Nouveautés principales** (aperçu) :
- Attributs standardisés `[[deprecated]]`, `[[nodiscard]]`
- Mot-clé `nullptr` (comme en C++)
- `typeof` standardisé
- Constantes binaires `0b1010`
- Directive `#embed` pour inclure des fichiers binaires
- Améliorations des `constexpr`
- Et plus de 50 autres changements...

**État en 2025** : **Support partiel** par les compilateurs. Adoption progressive. Pas encore recommandé pour la production.

> 📌 **Note** : La section 1.3.1 détaille C23 en profondeur. Nous y reviendrons.

---

## 🔍 Comparaison rapide des normes

### Tableau des fonctionnalités

| Fonctionnalité | C89 | C99 | C11 | C17 | C23 |
|----------------|-----|-----|-----|-----|-----|
| **Commentaires //** | ❌ | ✅ | ✅ | ✅ | ✅ |
| **Variables déclarées partout** | ❌ | ✅ | ✅ | ✅ | ✅ |
| **`for (int i=0; ...)`** | ❌ | ✅ | ✅ | ✅ | ✅ |
| **`<stdbool.h>` (bool)** | ❌ | ✅ | ✅ | ✅ | Natif |
| **`<stdint.h>` (int64_t)** | ❌ | ✅ | ✅ | ✅ | ✅ |
| **VLA (tableaux longueur variable)** | ❌ | ✅ | Optionnel | Optionnel | Optionnel |
| **Initialisation désignée** | ❌ | ✅ | ✅ | ✅ | ✅ |
| **`inline`** | ❌ | ✅ | ✅ | ✅ | ✅ |
| **`_Generic`** | ❌ | ❌ | ✅ | ✅ | ✅ |
| **`static_assert`** | ❌ | ❌ | ✅ | ✅ | ✅ |
| **Threads natifs** | ❌ | ❌ | ✅ | ✅ | ✅ |
| **Atomiques (`<stdatomic.h>`)** | ❌ | ❌ | ✅ | ✅ | ✅ |
| **Alignement (`alignas`)** | ❌ | ❌ | ✅ | ✅ | ✅ |
| **`nullptr`** | ❌ | ❌ | ❌ | ❌ | ✅ |
| **`typeof`** | Extension | Extension | Extension | Extension | ✅ |
| **Binaires `0b...`** | ❌ | ❌ | ❌ | ❌ | ✅ |
| **Attributs `[[...]]`** | ❌ | ❌ | ❌ | ❌ | ✅ |
| **`#embed`** | ❌ | ❌ | ❌ | ❌ | ✅ |

### Compatibilité entre versions

```
C89/C90  →  Presque tout compatible avec C99
    ↓
   C99   →  Presque tout compatible avec C11
    ↓
   C11   →  100% compatible avec C17 (juste corrections)
    ↓
   C17   →  Très compatible avec C23 (quelques breaking changes mineurs)
```

**Principe général** : Les normes C sont **rétrocompatibles** à ~95%. Un code C89 compile généralement en C99, C11, C17 et C23.

---

## 🎯 Quelle norme choisir ?

### Recommandations par contexte

| Situation | Norme recommandée | Raison |
|-----------|-------------------|--------|
| **Apprentissage (2025)** | C11 ou C17 | Standard actuel, bien documenté |
| **Nouveau projet pro** | C11 ou C17 | Support universel, stable |
| **Projet existant** | Version actuelle | Ne changez pas sans raison |
| **Embarqué contraint** | C99 ou C11 | Selon toolchain disponible |
| **Compatibilité maximale** | C99 | Fonctionne partout |
| **Systèmes critiques** | C11 | Maturité, certification |
| **Expérimentation** | C23 | Découvrir les nouveautés |

### Notre recommandation pour ce tutoriel

**Nous utiliserons principalement C11 (avec C17)** pour les raisons suivantes :

1. ✅ **Standard actuel** : Ce que vous utiliserez en production
2. ✅ **Support universel** : Fonctionne sur tous les compilateurs modernes
3. ✅ **Fonctionnalités modernes** : Threads, atomiques, génériques
4. ✅ **Documentation abondante** : Livres, tutoriels, forums
5. ✅ **Compatibilité** : Rétrocompatible avec C99, compatible vers C23

**Mentions de C23** : Nous signalerons les nouveautés C23 quand pertinent, mais ne les utiliserons pas systématiquement.

---

## 🔧 Spécifier la norme avec GCC

### Options de compilation

Pour spécifier explicitement quelle norme utiliser :

```bash
# C89/C90
gcc -std=c90 program.c -o program

# C99
gcc -std=c99 program.c -o program

# C11
gcc -std=c11 program.c -o program

# C17
gcc -std=c17 program.c -o program

# C23 (si supporté)
gcc -std=c23 program.c -o program
```

### Versions avec extensions GNU

GCC supporte aussi des versions "GNU" qui ajoutent des extensions :

```bash
# C11 + extensions GNU
gcc -std=gnu11 program.c -o program

# C11 strict (sans extensions)
gcc -std=c11 program.c -o program
```

**Différence** :
- `-std=c11` : C11 strict, conforme à la norme ISO
- `-std=gnu11` : C11 + extensions GNU (plus flexible)

### Mode strict avec `-pedantic`

Pour respecter strictement la norme (portable) :

```bash
gcc -std=c11 -pedantic -Wall -Wextra program.c -o program
```

L'option `-pedantic` émet des warnings si vous utilisez des extensions non standard.

---

## 📊 Macro pour détecter la version C

Dans votre code, vous pouvez détecter quelle version de C est utilisée :

```c
#include <stdio.h>

int main(void) {
    #if defined(__STDC__)
        printf("Compilateur conforme au standard C\n");

        #ifdef __STDC_VERSION__
            #if __STDC_VERSION__ == 199409L
                printf("Version: C94 (amendement)\n");
            #elif __STDC_VERSION__ == 199901L
                printf("Version: C99\n");
            #elif __STDC_VERSION__ == 201112L
                printf("Version: C11\n");
            #elif __STDC_VERSION__ == 201710L
                printf("Version: C17\n");
            #elif __STDC_VERSION__ == 202311L
                printf("Version: C23\n");
            #else
                printf("Version: %ld\n", __STDC_VERSION__);
            #endif
        #else
            printf("Version: C89/C90 (pas de __STDC_VERSION__)\n");
        #endif
    #else
        printf("Compilateur non conforme au standard C\n");
    #endif

    return 0;
}
```

**Compilation et test** :
```bash
gcc -std=c11 detect_version.c -o detect && ./detect
# Sortie: Version: C11

gcc -std=c99 detect_version.c -o detect && ./detect
# Sortie: Version: C99
```

---

## 🎓 Ce que vous devez retenir

### Les points essentiels

1. **Le C a évolué** à travers plusieurs normes officielles depuis 1989
2. **Cinq versions majeures** : C89/C90, C99, C11, C17, C23
3. **C11/C17 est le standard actuel** (2025) - mature et universel
4. **Chaque norme ajoute des fonctionnalités** tout en restant rétrocompatible
5. **C99 apporte la modernité basique** (bool, commentaires //, déclarations flexibles)
6. **C11 apporte la concurrence** (threads, atomiques)
7. **C23 est la nouvelle norme** (support partiel en 2025)

### Chronologie simplifiée

```
C89 (1989) → Première standardisation
    ↓
C99 (1999) → Modernisation (10 ans après)
    ↓
C11 (2011) → Concurrence native (12 ans après)
    ↓
C17 (2017) → Corrections (6 ans après)
    ↓
C23 (2023) → Nouveautés modernes (6 ans après)
```

### Quelle version apprendre ?

**Pour débuter en 2025 : C11 ou C17** ⭐
- Support universel
- Fonctionnalités modernes
- Documentation abondante
- Ce qu'on utilise en production

### Commande standard recommandée

```bash
gcc -std=c11 -Wall -Wextra -pedantic program.c -o program
```

Ou pour être encore plus récent :
```bash
gcc -std=c17 -Wall -Wextra -pedantic program.c -o program
```

---

## 📖 Sections suivantes

Maintenant que vous avez une vue d'ensemble des normes C, nous allons approfondir :

- **1.3.1** : Focus sur C23 - Nouveautés et état de l'adoption
- **1.3.2** : Vérification du support compilateur
- **1.3.3** : Pourquoi C11 reste la base recommandée en production

Ces sections détailleront les aspects pratiques du choix et de l'utilisation des différentes normes.

---

## 📚 Ressources complémentaires

### Documents officiels (gratuits)

- [Draft C99 (N1256)](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1256.pdf)
- [Draft C11 (N1570)](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n1570.pdf)
- [Draft C17 (N2310)](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n2310.pdf)
- [Draft C23 (N3096)](http://www.open-std.org/jtc1/sc22/wg14/www/docs/n3096.pdf)

### Livres recommandés

- *Modern C* (Jens Gustedt) - Focus sur C11/C17
- *C Programming: A Modern Approach* (K.N. King) - Édition mise à jour C11
- *Effective C* (Robert Seacord) - Best practices C11

### Ressources en ligne

- [cppreference.com/w/c](https://en.cppreference.com/w/c) - Référence complète
- [GCC C Standards Support](https://gcc.gnu.org/c99status.html)
- [Clang C Standards Support](https://clang.llvm.org/c_status.html)

---

**Vous avez maintenant une vision claire de l'évolution du langage C et savez vous repérer parmi les différentes normes !** 🚀

⏭️ [Focus sur C23 : Nouveautés et état de l'adoption](/01-ecosysteme-c-linux/03.1-focus-c23.md)
