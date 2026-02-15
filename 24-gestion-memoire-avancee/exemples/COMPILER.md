# Chapitre 24 - Gestion memoire avancee : Compilation des exemples

## Flags standards

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 fichier.c -o fichier
```

Les exemples utilisant `__attribute__((cleanup))` (extension GNU) sont compiles
**sans `-pedantic`** pour eviter les warnings sur les extensions GCC.

---

## 01_cleanup_basic.c

- **Section** : 24.1.1 - Extension GNU
- **Description** : Liberation automatique de memoire avec `__attribute__((cleanup))`
- **Fichier source** : 01.1-extension-gnu.md
- **Compilation** : sans `-pedantic` (extension GNU `__attribute__((cleanup))`)

```bash
gcc -Wall -Wextra -Werror -std=c17 01_cleanup_basic.c -o 01_cleanup_basic
```

- **Sortie attendue** :

```
=== Debut de la fonction ===
Contenu : Hello, cleanup!
=== Fin de la fonction ===
Nettoyage automatique : liberation de 0x...
```

---

## cleanup_patterns.h

- **Section** : 24.1.3 - Patterns cleanup
- **Description** : Bibliotheque complete de patterns cleanup reutilisables (header)
- **Fichier source** : 01.3-patterns.md
- **Compilation** : fichier d'en-tete, pas de compilation directe

---

## 02_cleanup_patterns_demo.c

- **Section** : 24.1.3 - Patterns cleanup
- **Description** : Traitement de fichier utilisateurs avec cleanup_patterns.h
- **Fichier source** : 01.3-patterns.md
- **Compilation** : sans `-pedantic` (extension GNU `__attribute__((cleanup))` + `STEAL` macro avec statement expression)

```bash
gcc -Wall -Wextra -Werror -std=c17 02_cleanup_patterns_demo.c -o 02_cleanup_patterns_demo
```

- **Sortie attendue** :

```
=== 5 utilisateurs trouves ===
1. Alice
2. Bob
3. Charlie
4. Diane
5. Eve

Traitement reussi !
```

---

## 03_cleanup_ordre.c

- **Section** : 24.1.4 - Limitations et portabilite
- **Description** : Demonstration de l'ordre de cleanup (LIFO)
- **Fichier source** : 01.4-limitations-portabilite.md
- **Compilation** : sans `-pedantic` (extension GNU `__attribute__((cleanup))`)

```bash
gcc -Wall -Wextra -Werror -std=c17 03_cleanup_ordre.c -o 03_cleanup_ordre
```

- **Sortie attendue** :

```
1. Debut
2. a declare
3. b declare
4. c declare
5. Fin du scope
[cleanup c] Liberation de c
[cleanup b] Liberation de b
[cleanup a] Liberation de a
```

---

## 04_cleanup_portable.c

- **Section** : 24.1.4 - Limitations et portabilite
- **Description** : Compilation conditionnelle pour cleanup portable
- **Fichier source** : 01.4-limitations-portabilite.md
- **Compilation** : sans `-pedantic` (utilise `__attribute__((cleanup))` quand disponible)

```bash
gcc -Wall -Wextra -Werror -std=c17 04_cleanup_portable.c -o 04_cleanup_portable
```

- **Sortie attendue** :

```
=== Features disponibles ===
  cleanup automatique : Oui (GCC/Clang)

--- Avec cleanup (si supporte) ---
Buffer: Donnees traitees avec succes  
Fichier ouvert: oui

--- Pattern goto (toujours portable) ---
Buffer: Pattern goto cleanup - portable partout  
Resultat: 0
```

---

## 05_arena_allocator.c

- **Section** : 24.2 - Custom allocators
- **Description** : Arena allocator - allocation lineaire avec liberation groupee
- **Fichier source** : 02-custom-allocators.md
- **Compilation** : flags standards

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 05_arena_allocator.c -o 05_arena_allocator
```

- **Sortie attendue** :

```
str1: Hello from arena!  
str2: Arena allocation is fast!  
Premiers nombres: 0, 1, 4  
Memoire utilisee: 500 / 10240 bytes

Apres reset: 0 bytes utilises  
new_str: Reutilisation de l'arena

--- Test alignement ---
Double aligne a 0x...: 3.14  
Aligne sur 8 bytes: oui

Arena detruite
```

---

## 06_pool_allocator.c

- **Section** : 24.2 - Custom allocators
- **Description** : Pool allocator - objets de taille fixe avec free individuel
- **Fichier source** : 02-custom-allocators.md
- **Compilation** : flags standards

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 06_pool_allocator.c -o 06_pool_allocator
```

- **Sortie attendue** :

```
Pool cree avec succes
=== Pool Stats ===
Block size: 40 bytes  
Total blocks: 100  
Allocated: 0  
Free: 100  
Memory usage: 4000 bytes

Entites creees:
- Entity 1 (ID: 1, Value: 10.5)
- Entity 2 (ID: 2, Value: 20.3)
- Entity 3 (ID: 3, Value: 15.7)

Apres allocations:
=== Pool Stats ===
...
Allocated: 3

Apres liberation de e2:
=== Pool Stats ===
...
Allocated: 2

Nouvelle entite: Entity 4 (ID: 4, Value: 99.9)

Toutes les entites liberees:
=== Pool Stats ===
...
Allocated: 0
```

---

## 07_stack_allocator.c

- **Section** : 24.2 - Custom allocators
- **Description** : Stack allocator - allocation LIFO avec marks/restore
- **Fichier source** : 02-custom-allocators.md
- **Compilation** : flags standards

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 07_stack_allocator.c -o 07_stack_allocator
```

- **Sortie attendue** :

```
Stack cree: 1048576 bytes disponibles  
Alloue temp1: 1000 bytes utilises  
Alloue temp2: 3000 bytes utilises  
Alloue numbers: 3400 bytes utilises

Donnees:  
temp1: Donnees temporaires 1  
temp2: Donnees temporaires 2  
numbers[50]: 50

Restauration a after_temp1...  
Bytes utilises apres restauration: 1000  
temp1 toujours valide: Donnees temporaires 1

Restauration complete...  
Bytes utilises: 0 (retour au debut)

Stack detruit
```

---

## 08_arena_parser.c

- **Section** : 24.2 - Custom allocators
- **Description** : Parseur JSON simple utilisant un arena allocator
- **Fichier source** : 02-custom-allocators.md
- **Compilation** : flags standards

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 08_arena_parser.c -o 08_arena_parser
```

- **Sortie attendue** :

```
Parsing JSON: {"name": "Alice", "age": 30, "city": "Paris"}

Token 1: {  
Token 2: STRING: "name"  
Token 3: STRING: "Alice"  
Token 4: STRING: "age"  
Token 5: NUMBER: 30  
Token 6: STRING: "city"  
Token 7: STRING: "Paris"  
Token 8: }

8 tokens parses
Memoire utilisee: 245 bytes  
Parseur detruit (tout libere d'un coup)
```

---

## 09_reference_counting.c

- **Section** : 24.3 - Garbage collection en C
- **Description** : Reference counting - comptage de references automatique
- **Fichier source** : 03-garbage-collection.md
- **Compilation** : flags standards

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 09_reference_counting.c -o 09_reference_counting
```

- **Sortie attendue** :

```
=== Exemple Reference Counting ===

  [NEW] Objet alloue (100 bytes), ref_count=1
Buffer: Hello, Reference Counting!  
References: 1

  [+] ref_count++ = 2
buffer2 pointe vers le meme objet  
References: 2

  [+] ref_count++ = 3
buffer3 pointe vers le meme objet  
References: 3

Liberation de buffer...
  [-] ref_count-- = 2
References restantes: 2

Liberation de buffer2...
  [-] ref_count-- = 1
References restantes: 1

Liberation de buffer3...
  [-] ref_count-- = 0
  [FREE] Objet libere (ref_count=0)

=== Fin de l'exemple ===
```

---

## 10_mark_and_sweep.c

- **Section** : 24.3 - Garbage collection en C
- **Description** : Mark and Sweep GC - marquer puis balayer les objets inutilises
- **Fichier source** : 03-garbage-collection.md
- **Compilation** : flags standards

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 10_mark_and_sweep.c -o 10_mark_and_sweep
```

- **Sortie attendue** :

```
=== Exemple Mark and Sweep GC ===

[GC] Cree (seuil: 256 objets)
[GC] Objet alloue (100 bytes) - Total: 1
[GC] Objet alloue (200 bytes) - Total: 2
[GC] Objet alloue (150 bytes) - Total: 3
[GC] Objet alloue (300 bytes) - Total: 4

Etat actuel:
  str1 (racine): String 1
  str2 (racine): String 2
  str3 (temporaire): String 3 - temporaire
  str4 (racine): String 4

=== GARBAGE COLLECTION ===
  Objets avant GC: 4
  --- Phase MARK ---
  [MARK] Objet marque (0x...)
  [MARK] Objet marque (0x...)
  [MARK] Objet marque (0x...)
  --- Phase SWEEP ---
  [SWEEP] Objet collecte (0x..., 150 bytes)
  1 objet(s) collecte(s), 3 restant(s)
  ...

str2 n'est plus une racine

=== GARBAGE COLLECTION ===
  ...
  [SWEEP] Objet collecte (0x..., 200 bytes)
  1 objet(s) collecte(s), 2 restant(s)
  ...

[GC] Destruction du GC
[GC] 2 objet(s) restant(s) libere(s)

=== Fin de l'exemple ===
```

---

## Notes

- **Boehm GC** (section 03-garbage-collection.md) : exemple non inclus, necessite `libgc-dev`
  (`sudo apt-get install libgc-dev`) et compilation avec `-lgc`
- **Sections theoriques** sans exemples compilables : 01-raii-cleanup.md (introduction),
  01.2-systemd-glib.md (patterns systemd/GLib, necessite bibliotheques externes)
- Les adresses memoire (`0x...`) varient a chaque execution
