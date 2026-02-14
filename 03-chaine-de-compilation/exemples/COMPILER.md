# Chapitre 03 - Exemples compilables

## Fichiers sans exemples compilables

Les fichiers suivants sont purement conceptuels ou ne contiennent que des commandes bash/illustrations :

- **README.md** : Introduction et structure du chapitre (contient un programme demo extrait ci-dessous)
- **01-quatre-etapes.md** : Les quatre etapes de la compilation (programme extrait ci-dessous)
- **02-visualisation-pipeline.md** : Visualisation du pipeline (programme extrait ci-dessous)
- **03-options-gcc.md** : Options de GCC (nombreuses commandes bash, un programme extrait ci-dessous)
- **04-hello-world.md** : Demystification du Hello World (programme extrait, reste conceptuel)
- **05-fichiers-intermediaires.md** : Fichiers intermediaires (programme extrait ci-dessous)
- **06-symboles.md** : Symboles et table des symboles (3 programmes extraits ci-dessous)

---

## Exemples compilables

### 01_demo.c

- **Section** : 3.0 Introduction - Un Premier Apercu
- **Fichier source** : README.md (lignes 260-266)
- **Description** : Programme de demonstration de la chaine de compilation
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 01_demo.c -o 01_demo
```
- **Sortie attendue** :
```
Bienvenue dans la chaîne de compilation !
```

---

### 02_hello_message.c

- **Section** : 3.1 Les quatre etapes de la compilation
- **Fichier source** : 01-quatre-etapes.md (lignes 47-56)
- **Description** : Programme hello avec macro `#define MESSAGE`
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 02_hello_message.c -o 02_hello_message
```
- **Sortie attendue** :
```
Bonjour le monde!
```

---

### 03_hello_pipeline.c

- **Section** : 3.2 Visualisation du pipeline
- **Fichier source** : 02-visualisation-pipeline.md (lignes 50-61)
- **Description** : Programme avec macros AUTEUR/ANNEE pour visualiser le pipeline de compilation
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 03_hello_pipeline.c -o 03_hello_pipeline
```
- **Sortie attendue** :
```
Programme créé par Alice en 2025
```

---

### 04_debug_conditionnel.c

- **Section** : 3.3 Options de GCC
- **Fichier source** : 03-options-gcc.md (lignes 537-548)
- **Description** : Compilation conditionnelle avec `-DDEBUG`
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 04_debug_conditionnel.c -o 04_debug_conditionnel
```
- **Sortie attendue (sans DEBUG)** :
```
Programme principal
```
- **Compilation avec DEBUG** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -DDEBUG 04_debug_conditionnel.c -o 04_debug_conditionnel
```
- **Sortie attendue (avec DEBUG)** :
```
Mode debug activé
Programme principal
```

---

### 05_hello_world.c

- **Section** : 3.4 Demystification du "Hello World"
- **Fichier source** : 04-hello-world.md (lignes 10-16)
- **Description** : Le classique Hello World, analyse en detail dans le chapitre
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 05_hello_world.c -o 05_hello_world
```
- **Sortie attendue** :
```
Hello, World!
```

---

### 06_fichiers_intermediaires.c

- **Section** : 3.5 Fichiers intermediaires
- **Fichier source** : 05-fichiers-intermediaires.md (lignes 63-95)
- **Description** : Programme elabore avec fonction `calculer_somme`, macro `CARRE` et variable globale `compteur`
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 06_fichiers_intermediaires.c -o 06_fichiers_intermediaires
```
- **Sortie attendue** :
```
Somme de 1 à 5 : 15
Carré de 5 : 25
Nombre d'itérations : 5
```

---

### 07_symboles.c

- **Section** : 3.6 Symboles et table des symboles
- **Fichier source** : 06-symboles.md (lignes 37-52)
- **Description** : Programme avec variable globale, fonction et main (3 symboles)
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 07_symboles.c -o 07_symboles
```
- **Sortie attendue** :
```
1
```

---

### 08_types_symboles.c

- **Section** : 3.6 Symboles et table des symboles
- **Fichier source** : 06-symboles.md (lignes 218-238)
- **Description** : Illustration des differents types de symboles (D, B, R, d, t, T, U) pour analyse avec `nm`
- **Compilation** (en fichier objet uniquement, comme dans le .md) :
```bash
gcc -Wall -Wextra -pedantic -std=c17 -c 08_types_symboles.c -o 08_types_symboles.o
nm 08_types_symboles.o
```
- **Note** : Compile sans `-Werror` car les elements `static` sont volontairement inutilises pour illustrer les symboles locaux (d, t). Les warnings sont attendus.
- **Sortie attendue (nm)** :
```
0000000000000000 R constante
0000000000000000 t helper
0000000000000025 T main
000000000000000b T publique
                 U puts
0000000000000000 D var_init
0000000000000000 B var_non_init
0000000000000004 d var_static
```

---

### 09a_main.c + 09b_affichage.c

- **Section** : 3.6 Symboles et table des symboles - Resolution des symboles
- **Fichier source** : 06-symboles.md (lignes 604-623)
- **Description** : Exemple multi-fichiers illustrant la resolution des symboles par le linker
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 09a_main.c 09b_affichage.c -o 09_multifile
```
- **Sortie attendue** :
```
Compteur: 0
```

---

## Resume

| # | Fichier | Section | Standard | Resultat |
|---|---------|---------|----------|----------|
| 1 | 01_demo.c | 3.0 Introduction | C17 | OK |
| 2 | 02_hello_message.c | 3.1 Quatre etapes | C17 | OK |
| 3 | 03_hello_pipeline.c | 3.2 Visualisation pipeline | C17 | OK |
| 4 | 04_debug_conditionnel.c | 3.3 Options GCC | C17 | OK |
| 5 | 05_hello_world.c | 3.4 Hello World | C17 | OK |
| 6 | 06_fichiers_intermediaires.c | 3.5 Fichiers intermediaires | C17 | OK |
| 7 | 07_symboles.c | 3.6 Symboles | C17 | OK |
| 8 | 08_types_symboles.c | 3.6 Symboles (nm) | C17 | OK (compile -c sans -Werror) |
| 9 | 09a_main.c + 09b_affichage.c | 3.6 Resolution symboles | C17 | OK |

**Total : 9 programmes (10 fichiers .c), 0 corrections necessaires dans les .md.**
