🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Module 3 : La Gestion de la Mémoire

> **Formation : Maîtriser C sur Ubuntu - De la Syntaxe au Système**
> Niveau : **Intermédiaire** • Durée estimée : **30-40 heures de lecture et expérimentation approfondie**

---

## 🎯 Objectifs du module

Bienvenue au **module le plus important** de cette formation. La gestion de la mémoire est ce qui distingue le C de la plupart des langages modernes, et c'est à la fois sa plus grande force et son plus grand défi.

Si vous maîtrisez ce module, vous maîtriserez l'essence du C.

À l'issue de ce module, vous comprendrez :

1. ✅ **Les pointeurs** : concept, syntaxe, et arithmétique
2. ✅ **La relation pointeurs-tableaux** et les chaînes de caractères
3. ✅ **Stack vs Heap** : les deux zones mémoire critiques
4. ✅ **L'allocation dynamique** : `malloc`, `calloc`, `realloc`, `free`
5. ✅ **Les bugs mémoire** : fuites, double free, dangling pointers, buffer overflows
6. ✅ **Les outils de détection** : Valgrind, sanitizers

---

## ⚠️ Avertissement important

### Ce module est difficile

**Soyons honnêtes** : les pointeurs sont la raison pour laquelle beaucoup de développeurs abandonnent l'apprentissage du C. Ce n'est pas un mythe, c'est une réalité.

**Pourquoi c'est difficile :**
- 🧠 Les pointeurs demandent une **nouvelle façon de penser**
- 🔍 Les bugs mémoire sont **invisibles** et se manifestent de manière imprévisible
- 💥 Les erreurs mémoire causent des **crashes spectaculaires** (segmentation fault)
- 🐛 Le debugging nécessite des outils spécialisés

**Mais vous POUVEZ y arriver si vous :**
- ✅ Prenez votre temps (ne précipitez pas ce module)
- ✅ Dessinez des schémas mémoire pour chaque concept
- ✅ Testez TOUS les exemples de code
- ✅ Utilisez Valgrind et les sanitizers dès le début
- ✅ Acceptez de ne pas tout comprendre du premier coup

> **Note importante** : Il est **normal** de relire certaines sections plusieurs fois. La compréhension des pointeurs se construit progressivement, par couches successives.

---

## 🧩 Place dans la formation globale

### D'où venez-vous ?
**Module 2** vous a appris les fondamentaux : types, opérateurs, fonctions. Vous savez écrire du code simple qui manipule des variables sur la **pile (stack)**.

Mais vous avez rencontré des limitations :
- Impossible de modifier une variable dans une fonction appelée (passage par valeur)
- Impossible de retourner plusieurs valeurs d'une fonction
- Impossible de créer des structures de données de taille variable
- Pas de contrôle fin sur la durée de vie des données

**Les pointeurs et la gestion mémoire résolvent tous ces problèmes.**

### Où allez-vous ?
**Module 4** (Structures de données) s'appuie entièrement sur ce module. Vous ne pouvez pas créer de listes chaînées, d'arbres, ou de tables de hachage sans maîtriser :
- Les pointeurs
- L'allocation dynamique
- La gestion des ressources

**Module 6** (Programmation système) utilise intensivement les pointeurs pour interagir avec le noyau Linux.

### Ce module est la fondation de tout le reste

```
Module 2: Fondamentaux
         ↓
    [Barrière de difficulté]
         ↓
Module 3: Gestion mémoire ← VOUS ÊTES ICI
         ↓
    [Tous les modules avancés]
```

---

## 📚 Structure du module

Ce module est organisé en **trois chapitres profondément interconnectés** :

### 🎯 [Chapitre 7 : Les Pointeurs - Démystification](07-pointeurs/README.md)
**Contenu** : 7 sections • **Niveau** : Intermédiaire • **Durée** : 12-15 heures

**Vous découvrirez :**
- Le concept d'adresse mémoire et de référence
- La syntaxe des pointeurs (`*`, `&`)
- L'arithmétique des pointeurs
- Les pointeurs `void*` et le casting
- Les pointeurs `NULL` et dangling pointers
- Les pointeurs constants (`const`)
- Les pointeurs restreints (`restrict` - C99)

**Concepts clés :**
```c
int x = 42;  
int *ptr = &x;     // ptr contient l'adresse de x
*ptr = 100;        // Modifie x via le pointeur
printf("%p\n", (void *)ptr);  // Affiche l'adresse mémoire

// Arithmétique des pointeurs
int arr[5] = {1, 2, 3, 4, 5};  
int *p = arr;  
p++;  // Avance de sizeof(int) bytes, pas de 1 byte !

// Pointeur constant vs constante pointée
const int *p1;     // Pointeur vers int constant  
int *const p2;     // Pointeur constant vers int  
const int *const p3;  // Les deux
```

**Pourquoi c'est crucial :** Les pointeurs sont le mécanisme fondamental pour :
- Passer des données par référence
- Manipuler des tableaux dynamiques
- Créer des structures de données complexes
- Interagir avec le système d'exploitation
- Optimiser les performances

**Difficulté principale :** Comprendre que `*` et `&` ont des significations différentes selon le contexte.

---

### 📊 [Chapitre 8 : Tableaux et Chaînes de caractères](08-tableaux-et-chaines/README.md)
**Contenu** : 7 sections • **Niveau** : Intermédiaire • **Durée** : 10-12 heures

**Vous apprendrez :**
- La relation profonde entre tableaux et pointeurs
- Les tableaux multidimensionnels
- La représentation des chaînes (strings) en C
- Les fonctions de manipulation de chaînes (`strcpy`, `strlen`, `strcmp`...)
- **Les dangers : buffer overflows et attaques**
- Les fonctions sécurisées (`strncpy`, `strncat`, `snprintf`)
- L'immutabilité des chaînes littérales

**Concepts clés :**
```c
// Équivalence tableau-pointeur (avec nuances)
int arr[5];  
int *ptr = arr;  // arr se dégrade en pointeur vers son premier élément

// Les chaînes sont des tableaux de char terminés par '\0'
char str[] = "Hello";  // {'H', 'e', 'l', 'l', 'o', '\0'}  
const char *s = "World";  // Chaîne littérale (constante !)

// Buffer overflow (BUG CRITIQUE)
char buffer[10];  
strcpy(buffer, "Cette chaîne est trop longue");  // ❌ CRASH ou corruption

// Version sécurisée
strncpy(buffer, "Texte", sizeof(buffer) - 1);  
buffer[sizeof(buffer) - 1] = '\0';  // Assure la terminaison
```

**Pourquoi c'est crucial :** Les buffer overflows sont la **première cause de vulnérabilités** en C (et ont été responsables de milliers de failles de sécurité historiques). Comprendre les chaînes C est essentiel pour écrire du code sécurisé.

**Difficulté principale :** Comprendre que `char *` peut représenter une chaîne littérale (constante) OU un pointeur vers un buffer modifiable.

---

### 🏗️ [Chapitre 9 : Allocation Dynamique](09-allocation-dynamique/README.md)
**Contenu** : 7 sections • **Niveau** : Intermédiaire-Avancé • **Durée** : 10-13 heures

**Vous maîtriserez :**
- La différence fondamentale entre Stack et Heap
- Les fonctions d'allocation : `malloc`, `calloc`, `realloc`, `free`
- La gestion des erreurs d'allocation
- Les fuites mémoire (memory leaks) : causes et prévention
- Le double free et la corruption du tas
- Les stratégies d'allocation personnalisées

**Concepts clés :**
```c
// Allocation sur le tas (heap)
int *arr = malloc(10 * sizeof(int));  
if (arr == NULL) {
    // Gestion d'erreur OBLIGATOIRE
    perror("malloc failed");
    return -1;
}

// Utilisation
for (int i = 0; i < 10; i++) {
    arr[i] = i * 2;
}

// Libération OBLIGATOIRE
free(arr);
arr = NULL;  // Bonne pratique

// calloc : initialise à zéro
int *zeros = calloc(10, sizeof(int));

// realloc : redimensionner un tableau existant
int *data = malloc(10 * sizeof(int));
int *tmp = realloc(data, 20 * sizeof(int));  // Attention : peut changer l'adresse !
if (tmp != NULL) data = tmp;  // Ne jamais écrire : data = realloc(data, ...)
```

**Comparaison Stack vs Heap :**
```
STACK (Pile)                      HEAP (Tas)
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
✅ Rapide                         ✅ Taille flexible
✅ Automatique                    ✅ Durée de vie contrôlée
✅ Pas de fragmentation          ✅ Partage entre fonctions
❌ Taille limitée (~1-8 Mo)      ❌ Plus lent
❌ Durée de vie limitée          ❌ Gestion manuelle
❌ Pas de redimensionnement      ❌ Risque de fragmentation
```

**Pourquoi c'est crucial :** La majorité des bugs en C sont liés à la mémoire :
- Fuites mémoire (votre programme consomme de plus en plus de RAM)
- Use-after-free (accès à de la mémoire libérée)
- Double free (libérer deux fois la même mémoire)
- Buffer overflow (écrire au-delà de la zone allouée)

**Difficulté principale :** Garder en tête qui est responsable de libérer chaque allocation (ownership).

---

## 🔍 Le modèle mental à construire

### Avant ce module : Vision simple
```
int x = 42;  → "x contient 42"
```

### Après ce module : Vision complète
```
┌─────────────────────────┐
│  Stack Frame            │
│  ┌─────────┬─────────┐  │
│  │ Nom: x  │ Valeur  │  │
│  ├─────────┼─────────┤  │
│  │ Adresse │ 0x7fff  │  │
│  │  0x7fff │   42    │  │
│  └─────────┴─────────┘  │
│                         │
│  int *ptr = &x;         │
│  ┌─────────┬─────────┐  │
│  │ Nom:ptr │ Valeur  │  │
│  ├─────────┼─────────┤  │
│  │ Adresse │ 0x7ff8  │  │
│  │  0x7ff8 │ 0x7fff  │  ← ptr pointe vers x
│  └─────────┴─────────┘  │
└─────────────────────────┘
```

Ce niveau de compréhension est nécessaire pour :
- Déboguer les segfaults
- Comprendre les messages de Valgrind
- Écrire du code sans fuites mémoire
- Optimiser l'utilisation de la mémoire

---

## 🎓 Prérequis

### Absolument nécessaire
- ✅ **Module 1 et 2 complétés** et bien compris
- ✅ Maîtrise des **fonctions et de la pile** (Module 2, Chapitre 6)
- ✅ Compréhension des **types et de sizeof**
- ✅ Capacité à **lire des messages d'erreur** de compilation

### Fortement recommandé
- ✅ Familiarité avec **GDB** (les bases suffisent)
- ✅ Installation de **Valgrind** sur votre système
- ✅ Activation des **sanitizers** dans votre compilateur

### Vérification préalable
```bash
# Assurez-vous d'avoir ces outils
gcc --version           # GCC 9+  
valgrind --version      # Valgrind 3.15+  
gdb --version          # GDB 8+

# Testez un programme avec sanitizers
gcc -fsanitize=address -g programme.c -o programme
```

---

## 🗺️ Stratégie d'apprentissage recommandée

### Pour ce module particulier, adoptez cette approche :

#### 📐 1. Dessinez TOUT
Les pointeurs deviennent clairs quand vous les visualisez. Pour chaque exemple :

```
int x = 10;  
int *p = &x;
*p = 20;

DESSINEZ :
┌─────┬─────┐
│  x  │ 10  │ ← Adresse 0x1000
└─────┴─────┘
   ↑
┌──┴──┬───────┐
│  p  │ 0x1000│
└─────┴───────┘
```

#### 🧪 2. Testez méthodiquement
Pour chaque concept, suivez ce workflow :

```bash
# 1. Créez un fichier de test minimal
cat > test_ptr.c << 'EOF'
#include <stdio.h>
int main(void) {
    int x = 42;
    int *p = &x;
    printf("Adresse de x: %p\n", (void*)&x);
    printf("Valeur de p: %p\n", (void*)p);
    printf("Valeur pointée: %d\n", *p);
    return 0;
}
EOF

# 2. Compilez avec tous les warnings et sanitizers
gcc -Wall -Wextra -Werror -fsanitize=address -g test_ptr.c -o test_ptr

# 3. Exécutez
./test_ptr

# 4. Vérifiez avec Valgrind (même si sanitizers sont activés)
valgrind --leak-check=full ./test_ptr
```

#### 🐛 3. Provoquez les bugs intentionnellement
Comprendre les bugs par l'expérience :

```c
// Test 1 : Dangling pointer
int *p;
{
    int x = 42;
    p = &x;
}  // x n'existe plus
// printf("%d\n", *p);  // ❌ Comportement indéfini

// Test 2 : Fuite mémoire
void leak_test() {
    int *p = malloc(100 * sizeof(int));
    // Oubli de free(p)
}

// Test 3 : Double free
int *p = malloc(sizeof(int));  
free(p);  
free(p);  // ❌ Corruption du tas
```

Observez les messages de Valgrind et des sanitizers pour chaque bug.

#### 📖 4. Lecture progressive
**Ne lisez pas tout le module d'un coup.** Suivez ce rythme :

1. **Jour 1-3** : Chapitre 7, sections 7.1-7.3 (bases des pointeurs)
2. **Jour 4-5** : Chapitre 7, sections 7.4-7.7 (pointeurs avancés)
3. **Pause** : Revisitez ce qui n'est pas clair
4. **Jour 6-8** : Chapitre 8 (tableaux et chaînes)
5. **Pause** : Testez vos connaissances
6. **Jour 9-11** : Chapitre 9 (allocation dynamique)
7. **Jour 12-14** : Révision complète et expérimentation

---

## 🛠️ Outils essentiels pour ce module

### 1. Valgrind - Votre meilleur ami
```bash
# Installation
sudo apt install valgrind

# Utilisation de base
valgrind --leak-check=full \
         --show-leak-kinds=all \
         --track-origins=yes \
         --verbose \
         ./votre_programme

# Exemple de sortie
==12345== HEAP SUMMARY:
==12345==     in use at exit: 40 bytes in 1 blocks
==12345==   total heap usage: 1 allocs, 0 frees, 40 bytes allocated
==12345== 40 bytes in 1 blocks are definitely lost
```

### 2. AddressSanitizer (ASan)
```bash
# Compilation avec ASan
gcc -fsanitize=address -fno-omit-frame-pointer -g programme.c -o programme

# ASan détecte automatiquement :
# - Buffer overflows
# - Use-after-free
# - Use-after-return
# - Double free
# - Memory leaks (avec ASAN_OPTIONS=detect_leaks=1)
```

### 3. GDB pour déboguer les pointeurs
```bash
# Lancer GDB
gdb ./programme

# Commandes utiles
(gdb) print ptr           # Affiche l'adresse contenue dans ptr
(gdb) print *ptr          # Affiche la valeur pointée
(gdb) print &variable     # Affiche l'adresse de variable
(gdb) x/10xw ptr          # Examine 10 mots en hexadécimal à partir de ptr
```

---

## ⚠️ Les 7 erreurs fatales à éviter

### 🚨 Erreur 1 : Déréférencer un pointeur non initialisé
```c
int *p;          // ❌ p contient une adresse aléatoire
*p = 42;         // ❌ CRASH ou corruption silencieuse

// ✅ Solution
int *p = NULL;   // Initialiser à NULL  
if (p != NULL) {
    *p = 42;
}
```

### 🚨 Erreur 2 : Retourner l'adresse d'une variable locale
```c
int* get_value() {
    int x = 42;
    return &x;   // ❌ x est détruit à la sortie de la fonction
}

// ✅ Solution : allouer sur le tas
int* get_value() {
    int *p = malloc(sizeof(int));
    *p = 42;
    return p;  // L'appelant doit faire free()
}
```

### 🚨 Erreur 3 : Oublier de libérer la mémoire
```c
void process_data() {
    int *data = malloc(1000 * sizeof(int));
    // ... utilisation ...
    // ❌ Oubli de free(data) → fuite mémoire
}

// ✅ Solution : toujours free ce que vous malloc
void process_data() {
    int *data = malloc(1000 * sizeof(int));
    if (data == NULL) return;
    // ... utilisation ...
    free(data);
}
```

### 🚨 Erreur 4 : Utiliser après libération (use-after-free)
```c
int *p = malloc(sizeof(int));
*p = 42;
free(p);  
printf("%d\n", *p);  // ❌ Comportement indéfini

// ✅ Solution
free(p);  
p = NULL;  // Empêche l'accès accidentel
```

### 🚨 Erreur 5 : Buffer overflow
```c
char buffer[10];  
strcpy(buffer, "Chaîne trop longue");  // ❌ Écrit au-delà du buffer

// ✅ Solution
strncpy(buffer, "Texte", sizeof(buffer) - 1);  
buffer[sizeof(buffer) - 1] = '\0';
```

### 🚨 Erreur 6 : Arithmétique de pointeurs incorrecte
```c
int arr[5];  
int *p = arr;  
p = p + 5;  // p pointe juste après le tableau
*p = 42;    // ❌ Écrit en dehors du tableau

// ✅ Solution : vérifier les bornes
if (p < arr + 5) {
    *p = 42;
}
```

### 🚨 Erreur 7 : Confusion entre allocation stack et heap
```c
void get_string(char *dest) {
    char temp[100] = "Hello";
    dest = temp;  // ❌ dest reçoit l'adresse d'une variable locale
}  // temp est détruit ici

// ✅ Solution : copier les données ou allouer sur le tas
void get_string(char *dest, size_t size) {
    strncpy(dest, "Hello", size - 1);
    dest[size - 1] = '\0';
}
```

---

## 🏆 Critères de maîtrise

### ✅ Niveau 1 : Compréhension des pointeurs
- [ ] Expliquer la différence entre `&`, `*` et la valeur
- [ ] Dessiner un schéma mémoire pour n'importe quel code avec pointeurs
- [ ] Prédire le résultat d'opérations sur pointeurs
- [ ] Comprendre l'arithmétique des pointeurs
- [ ] Utiliser `const` correctement avec les pointeurs

### ✅ Niveau 2 : Tableaux et chaînes
- [ ] Expliquer la relation entre tableaux et pointeurs
- [ ] Manipuler des chaînes de caractères sans buffer overflow
- [ ] Utiliser les fonctions sécurisées (`strncpy`, `snprintf`)
- [ ] Comprendre l'immutabilité des chaînes littérales
- [ ] Gérer les tableaux multidimensionnels

### ✅ Niveau 3 : Allocation dynamique
- [ ] Allouer et libérer de la mémoire correctement
- [ ] Gérer les erreurs d'allocation
- [ ] Expliquer Stack vs Heap avec précision
- [ ] Utiliser `realloc` sans bugs
- [ ] Détecter et corriger les fuites mémoire

### ✅ Niveau 4 : Debugging et qualité
- [ ] Utiliser Valgrind pour détecter tous types d'erreurs mémoire
- [ ] Compiler et tester avec ASan systématiquement
- [ ] Déboguer des segfaults avec GDB
- [ ] Écrire du code sans warnings (-Wall -Wextra -Werror)
- [ ] Appliquer les bonnes pratiques de sécurité

---

## 📊 Indicateurs de progression

Vous savez que vous progressez quand :

### Semaine 1-2 (Pointeurs de base)
- ✅ Vous ne confondez plus `*p` et `&p`
- ✅ Vous comprenez les messages "segmentation fault"
- ✅ Vous pouvez expliquer un pointeur à quelqu'un d'autre
- ✅ Vous dessinez naturellement des schémas mémoire

### Semaine 3-4 (Tableaux et chaînes)
- ✅ Vous comprenez pourquoi `arr == &arr[0]`
- ✅ Vous évitez instinctivement `strcpy` au profit de `strncpy`
- ✅ Vous reconnaissez les buffer overflows dans du code
- ✅ Valgrind ne signale plus d'erreurs dans vos programmes

### Semaine 5-6 (Allocation dynamique)
- ✅ Vous vérifiez systématiquement les retours de `malloc`
- ✅ Vous utilisez `free()` automatiquement
- ✅ Vous comprenez les rapports de Valgrind
- ✅ Vous choisissez consciemment entre stack et heap

---

## 📚 Ressources complémentaires critiques

### Livres essentiels pour ce module
- **Pointers on C** (Kenneth Reek) - Le livre de référence sur les pointeurs
- **Understanding and Using C Pointers** (Richard Reese) - Moderne et détaillé
- **The C Programming Language** (K&R) - Chapitre 5 (Pointeurs et tableaux)

### Visualisations interactives
- [Pointer Visualizer](http://pythontutor.com/c.html) - Voir l'exécution pas à pas
- [Memory Visualizer](https://www.cs.usfca.edu/~galles/visualization/) - Structures de données
- [C Pointer Tutorial](https://www.youtube.com/watch?v=h-HBipu_1P0) - Vidéo pédagogique

### Documentation technique
- [Valgrind Manual](https://valgrind.org/docs/manual/manual.html)
- [ASan Documentation](https://github.com/google/sanitizers/wiki/AddressSanitizer)
- [GDB Tutorial](https://www.sourceware.org/gdb/current/onlinedocs/gdb/)

---

## 💡 Conseils de survie pour ce module

### Pour ne pas abandonner

#### 1. **Acceptez la confusion initiale**
Les pointeurs sont contre-intuitifs au début. C'est NORMAL. Tout le monde passe par là.

#### 2. **Pratiquez quotidiennement**
Même 30 minutes par jour > 4 heures le week-end. Votre cerveau a besoin de temps pour assimiler.

#### 3. **Utilisez les outils dès le début**
Ne codez JAMAIS sans Valgrind ou ASan. Ces outils sont vos filets de sécurité.

#### 4. **Rejoignez une communauté**
- [r/C_Programming](https://reddit.com/r/C_Programming)
- Discord "The Programmer's Hangout"
- Stack Overflow avec le tag `[c]`

#### 5. **Gardez un journal**
Notez les concepts qui vous bloquent, puis revisitez-les après quelques jours. Souvent, ils deviennent clairs avec du recul.

### Quand demander de l'aide

Cherchez de l'aide si :
- ❓ Vous êtes bloqué plus de 2 heures sur un concept
- ❓ Valgrind signale des erreurs que vous ne comprenez pas
- ❓ Vos programmes crashent de manière inexpliquée
- ❓ Vous ne voyez pas la différence entre deux approches

Ne restez pas seul face aux difficultés.

---

## 🗓️ Planning de lecture réaliste

### Approche intensive (temps plein)
| Semaine | Contenu | Focus |
|---------|---------|-------|
| **Semaine 1** | Chapitre 7 (7.1-7.4) | Pointeurs de base, arithmétique |
| **Semaine 2** | Chapitre 7 (7.5-7.7) + révision | Pointeurs avancés |
| **Semaine 3** | Chapitre 8 (8.1-8.4) | Tableaux, chaînes |
| **Semaine 4** | Chapitre 8 (8.5-8.7) | Sécurité, buffer overflows |
| **Semaine 5** | Chapitre 9 (9.1-9.4) | Stack/Heap, malloc/free |
| **Semaine 6** | Chapitre 9 (9.5-9.7) + révision | Bugs mémoire, debugging |

### Approche à temps partiel (10h/semaine)
| Semaine | Contenu |
|---------|---------|
| **Semaines 1-3** | Chapitre 7 complet |
| **Semaines 4-6** | Chapitre 8 complet |
| **Semaines 7-9** | Chapitre 9 complet |
| **Semaine 10** | Révision et consolidation |

---

## 📑 Prochaines sections

**Prêt à relever le défi ? Commencez ici :**

- **[Chapitre 7 : Les Pointeurs - Démystification →](07-pointeurs/README.md)**
- [Chapitre 8 : Tableaux et Chaînes de caractères](08-tableaux-et-chaines/README.md)
- [Chapitre 9 : Allocation Dynamique](09-allocation-dynamique/README.md)

---

## 🎯 Mot de conclusion

Ce module va vous transformer. Après l'avoir complété, vous ne verrez plus jamais la mémoire de la même façon. Vous comprendrez ce qui se passe "sous le capot" dans n'importe quel programme.

**C'est difficile, mais ça en vaut la peine.**

Les pointeurs ne sont pas magiques, ils sont juste des adresses mémoire. La gestion mémoire n'est pas mystérieuse, c'est juste de la discipline et de bons outils.

Prenez votre temps, soyez patient avec vous-même, et n'abandonnez pas. Des milliers de développeurs avant vous ont réussi à maîtriser ces concepts. Vous le pouvez aussi.

---

**Courage, et que la mémoire soit avec vous ! 🚀**

---

## 📝 Note finale

Cette formation est un guide de référence complet. Pour ce module en particulier :

- **Lisez attentivement** chaque explication
- **Testez systématiquement** les exemples avec Valgrind/ASan
- **Dessinez des schémas** mémoire pour visualiser
- **Expérimentez** en modifiant le code
- **Revisitez** les sections difficiles après quelques jours

Le **Module 9** (Projets de synthèse) vous donnera l'opportunité d'appliquer toutes ces connaissances dans des contextes réalistes.

⏭️ [Chapitre 7 : Les Pointeurs - Démystification →](07-pointeurs/README.md)
