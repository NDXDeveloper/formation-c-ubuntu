# Chapitre 01 - Exemples compilables

## Fichiers sans exemples compilables

Les fichiers suivants sont purement conceptuels et ne contiennent aucun programme compilable :

- **README.md** : Introduction et structure du chapitre
- **01-pourquoi-le-c-en-2025.md** : Argumentation sur la pertinence du C
- **02-relation-c-unix-linux.md** : Histoire C/UNIX/Linux (extraits illustratifs non compilables)
- **03.3-c11-base-production.md** : Argumentation pour C11 (extraits illustratifs)
- **04-c-vs-cpp-vs-rust.md** : Comparaison de langages (extraits multi-langages)

---

## Exemples compilables

### 01_detect_version.c

- **Section** : 1.3 Normes du langage
- **Fichier source** : 03-normes-du-langage.md (lignes 461-489)
- **Description** : Detection de la version du standard C utilisee via les macros `__STDC__` et `__STDC_VERSION__`
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 01_detect_version.c -o 01_detect_version
```
- **Sortie attendue** (avec `-std=c17`) :
```
Compilateur conforme au standard C
Version: C17
```
- **Note** : La sortie varie selon le flag `-std=` utilise (c99 → "C99", c11 → "C11", etc.)

---

### 02_test_std.c

- **Section** : 1.3.2 Verification du support compilateur
- **Fichier source** : 03.2-verification-support-compilateur.md (lignes 127-149)
- **Description** : Programme de test du standard C detecte (variante plus concise)
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 02_test_std.c -o 02_test_std
```
- **Sortie attendue** (avec `-std=c17`) :
```
Standard C détecté : C17
```
- **Note** : Tester avec differents flags : `-std=c99`, `-std=c11`, `-std=c17`

---

### 03_test_atomics.c

- **Section** : 1.3.2 Verification du support compilateur
- **Fichier source** : 03.2-verification-support-compilateur.md (lignes 293-301)
- **Description** : Test du support des types atomiques (C11) avec `<stdatomic.h>`
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 03_test_atomics.c -o 03_test_atomics
```
- **Sortie attendue** :
```
Compteur atomique : 5
```

---

### 04_test_generic.c

- **Section** : 1.3.2 Verification du support compilateur
- **Fichier source** : 03.2-verification-support-compilateur.md (lignes 321-336)
- **Description** : Test du support de `_Generic` (C11) pour la selection de type
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 04_test_generic.c -o 04_test_generic
```
- **Sortie attendue** :
```
Type de 42 : int
Type de 3.14f : float
Type de "Hello" : string
```

---

### 05_test_threads.c

- **Section** : 1.3.2 Verification du support compilateur
- **Fichier source** : 03.2-verification-support-compilateur.md (lignes 262-276)
- **Description** : Test du support des threads natifs C11 avec `<threads.h>`
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 05_test_threads.c -o 05_test_threads -pthread
```
- **Sortie attendue** :
```
Thread C11 fonctionne !
```
- **Note** : Le flag `-pthread` est necessaire. Si `<threads.h>` n'est pas disponible sur votre systeme, utilisez `<pthread.h>` (POSIX threads) a la place.

---

### 06_compiler_info.c

- **Section** : 1.3.2 Verification du support compilateur
- **Fichier source** : 03.2-verification-support-compilateur.md (lignes 457-472)
- **Description** : Affichage des informations du compilateur via les macros predefinies
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 06_compiler_info.c -o 06_compiler_info
```
- **Sortie attendue** (varie selon le systeme) :
```
Compilé avec GCC X.Y.Z
Version C : 201710
```

---

### 07_test_c23.c

- **Section** : 1.3.1 Focus sur C23
- **Fichier source** : 03.1-focus-c23.md (lignes 442-464)
- **Description** : Exemple de programme C23 utilisant attributs `[[nodiscard]]`, constantes binaires, `typeof` et `nullptr`
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c2x 07_test_c23.c -o 07_test_c23
```
- **Sortie attendue** :
```
Result: 20
Pointer is null: yes
```
- **Note** : Necessite GCC 13+ avec `-std=c2x` ou GCC 14+ avec `-std=c23`. Le .md indique `-std=c23` mais GCC 13 utilise `-std=c2x`.

---

### 08_test_c23_features.c

- **Section** : 1.3.2 Verification du support compilateur
- **Fichier source** : 03.2-verification-support-compilateur.md (lignes 358-376)
- **Description** : Test des fonctionnalites C23 (attributs, constantes binaires, nullptr)
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c2x 08_test_c23_features.c -o 08_test_c23_features
```
- **Sortie attendue** :
```
Flags : 10
Pointer is null : oui
```
- **Note** : Necessite GCC 13+ avec `-std=c2x` ou GCC 14+ avec `-std=c23`.

---

## Resume

| # | Fichier | Section | Standard | Resultat |
|---|---------|---------|----------|----------|
| 1 | 01_detect_version.c | 1.3 Normes du langage | C17 | OK |
| 2 | 02_test_std.c | 1.3.2 Support compilateur | C17 | OK |
| 3 | 03_test_atomics.c | 1.3.2 Support compilateur | C17 | OK |
| 4 | 04_test_generic.c | 1.3.2 Support compilateur | C17 | OK |
| 5 | 05_test_threads.c | 1.3.2 Support compilateur | C17 | OK |
| 6 | 06_compiler_info.c | 1.3.2 Support compilateur | C17 | OK |
| 7 | 07_test_c23.c | 1.3.1 Focus C23 | C23 | OK |
| 8 | 08_test_c23_features.c | 1.3.2 Support compilateur | C23 | OK |

**Total : 8 programmes, 0 corrections necessaires.**
