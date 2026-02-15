🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 26.8 Compilation avec hardening flags

## Introduction

Le **hardening** (durcissement) du code consiste à activer des protections au moment de la compilation pour rendre l'exploitation des vulnérabilités beaucoup plus difficile, voire impossible.

### Analogie simple

Imaginez votre programme comme une maison :

**Sans hardening** :
- Portes en bois simple
- Fenêtres sans barreaux
- Pas d'alarme
- Pas de caméras

**Avec hardening** :
- Porte blindée
- Fenêtres avec barreaux
- Système d'alarme
- Caméras de surveillance
- Coffre-fort pour les objets de valeur

Le hardening **ne corrige pas les bugs** dans votre code, mais il rend leur exploitation beaucoup plus difficile.

### Principe de défense en profondeur

```
Code sans bugs (impossible à garantir)
    ↓ (si bug)
Validation des entrées
    ↓ (si contournée)
HARDENING FLAGS ← Rend l'exploitation difficile
    ↓ (si contournée)
Principe du moindre privilège
    ↓ (si contourné)
Monitoring et détection
```

---

## Pourquoi le hardening est crucial ?

### 1. Protection contre les exploits

Même avec du code bien écrit, des bugs peuvent exister. Le hardening :
- Détecte les exploits au runtime
- Arrête le programme avant qu'un attaquant ne prenne le contrôle
- Rend certaines techniques d'exploitation obsolètes

### 2. Conformité et standards

Beaucoup de standards de sécurité (OWASP, CERT, PCI-DSS) **exigent** le hardening.

### 3. Coût minimal

- Configuration : quelques minutes
- Impact sur les performances : généralement < 5%
- Bénéfice : protection contre des exploits critiques

**ROI exceptionnel !**

---

## Les flags de hardening essentiels

### 1. Stack Canaries (-fstack-protector)

#### Qu'est-ce qu'un stack canary ?

Un **canary** (canari) est une valeur sentinelle placée sur la pile entre les variables locales et l'adresse de retour.

**Fonctionnement** :

```
Stack sans canary :
+-------------------+
| Adresse de retour | ← Cible des buffer overflows
+-------------------+
| Variables locales |
| char buffer[10]   | ← Un overflow ici écrase directement l'adresse de retour
+-------------------+

Stack avec canary :
+-------------------+
| Adresse de retour |
+-------------------+
| CANARY (0xDEADBEEF) | ← Valeur aléatoire
+-------------------+
| Variables locales |
| char buffer[10]   | ← Un overflow doit d'abord écraser le canary
+-------------------+
```

Si le canary est modifié, le programme détecte l'attaque et s'arrête :
```
*** stack smashing detected ***: terminated
Aborted (core dumped)
```

#### Options disponibles

```bash
# Aucune protection (à éviter !)
gcc -fno-stack-protector programme.c

# Protection basique (fonctions avec buffers > 8 octets)
gcc -fstack-protector programme.c

# Protection forte (toutes fonctions avec buffers locaux)
gcc -fstack-protector-strong programme.c  # ← RECOMMANDÉ

# Protection maximale (toutes fonctions)
gcc -fstack-protector-all programme.c     # Plus d'overhead
```

#### Exemple de détection

```c
// vulnerable.c
#include <string.h>

void fonction_vulnerable(char *input) {
    char buffer[10];
    strcpy(buffer, input);  // Buffer overflow potentiel
}

int main(int argc, char *argv[]) {
    if (argc > 1) {
        fonction_vulnerable(argv[1]);
    }
    return 0;
}
```

**Sans protection** :
```bash
gcc -fno-stack-protector vulnerable.c -o vulnerable
./vulnerable "AAAAAAAAAAAAAAAAAAAA"
Segmentation fault  # Crash silencieux
```

**Avec protection** :
```bash
gcc -fstack-protector-strong vulnerable.c -o vulnerable
./vulnerable "AAAAAAAAAAAAAAAAAAAA"
*** stack smashing detected ***: terminated
Aborted (core dumped)  # Attaque détectée !
```

---

### 2. FORTIFY_SOURCE (-D_FORTIFY_SOURCE)

#### Qu'est-ce que FORTIFY_SOURCE ?

FORTIFY_SOURCE remplace les appels à des fonctions dangereuses par des versions **vérifiées** au runtime.

**Fonctions protégées** :
- `memcpy`, `memset`, `memmove`
- `strcpy`, `strcat`, `sprintf`
- `gets`, `scanf`
- etc.

#### Niveaux de protection

```bash
# Niveau 1 : Vérifications au compile-time uniquement
gcc -D_FORTIFY_SOURCE=1 -O2 programme.c

# Niveau 2 : Vérifications au compile-time ET runtime (recommandé)
gcc -D_FORTIFY_SOURCE=2 -O2 programme.c

# Niveau 3 : Vérifications plus strictes (GCC ≥ 12)
gcc -D_FORTIFY_SOURCE=3 -O2 programme.c
```

**Important** : Nécessite l'optimisation (`-O1`, `-O2`, ou `-O3`) !

#### Exemple de détection

```c
// fortify_test.c
#include <string.h>

int main(void) {
    char dest[10];
    char source[50] = "Ceci est une chaîne beaucoup trop longue";

    strcpy(dest, source);  // Buffer overflow !

    return 0;
}
```

**Sans FORTIFY** :
```bash
gcc fortify_test.c -o test
./test
# Peut crasher ou sembler fonctionner (comportement indéfini)
```

**Avec FORTIFY** :
```bash
gcc -D_FORTIFY_SOURCE=2 -O2 fortify_test.c -o test
./test
*** buffer overflow detected ***: terminated
Aborted (core dumped)
```

---

### 3. Position Independent Executable (PIE / -fPIE -pie)

#### Qu'est-ce que PIE ?

PIE permet à l'exécutable d'être chargé à une **adresse mémoire aléatoire** à chaque exécution.

**Sans PIE** (adresses fixes) :
```
Exécution 1 : Programme chargé à 0x08048000  
Exécution 2 : Programme chargé à 0x08048000  ← Toujours pareil !  
Exécution 3 : Programme chargé à 0x08048000  

Un attaquant peut prédire les adresses mémoire
```

**Avec PIE** (ASLR activé) :
```
Exécution 1 : Programme chargé à 0x55555555000  
Exécution 2 : Programme chargé à 0x7fff8821000  ← Aléatoire !  
Exécution 3 : Programme chargé à 0x7f1234ab000  

L'attaquant ne peut pas deviner les adresses
```

#### Compilation

```bash
# Créer un PIE
gcc -fPIE -pie programme.c -o programme

# Vérifier
file programme
# Output: programme: ELF 64-bit LSB pie executable, ...
#                                   ^^^
```

#### Pourquoi c'est important ?

De nombreuses exploitations nécessitent de connaître des adresses mémoire précises :
- Adresse d'une fonction système
- Adresse d'un shellcode
- Adresse d'un ROP gadget

Avec PIE + ASLR, ces adresses changent à chaque exécution, rendant l'exploitation beaucoup plus difficile.

---

### 4. RELRO (Relocation Read-Only)

#### Qu'est-ce que RELRO ?

RELRO protège la **Global Offset Table (GOT)** et d'autres sections critiques contre l'écriture.

**GOT** : Table contenant les adresses des fonctions de bibliothèques partagées.

**Attaque classique** : Écraser une entrée de la GOT pour rediriger l'exécution.

#### Niveaux de protection

```bash
# Partial RELRO (par défaut sur systèmes modernes)
gcc -Wl,-z,relro programme.c

# Full RELRO (recommandé)
gcc -Wl,-z,relro,-z,now programme.c
```

**Différence** :
- **Partial RELRO** : GOT en lecture seule après initialisation
- **Full RELRO** : GOT complètement en lecture seule (toutes résolutions au chargement)

#### Impact

**Full RELRO** :
- ✅ Meilleure sécurité (GOT non modifiable)
- ⚠️ Temps de démarrage légèrement plus long (résolution de tous les symboles)
- ⚠️ Utilisation mémoire légèrement plus élevée

Pour la plupart des programmes, l'impact est négligeable.

---

### 5. NX Bit / DEP (-z noexecstack)

#### Qu'est-ce que le NX bit ?

Le **NX bit** (No-eXecute) marque certaines zones mémoire (pile, tas) comme **non-exécutables**.

**Sans NX** :
```
Un attaquant injecte du code malveillant dans un buffer
    ↓
Redirige l'exécution vers ce buffer
    ↓
Le code malveillant s'exécute
```

**Avec NX** :
```
Un attaquant injecte du code malveillant dans un buffer
    ↓
Tente de rediriger l'exécution vers ce buffer
    ↓
SEGFAULT : Zone mémoire non-exécutable !
```

#### Activation

```bash
# Pile non-exécutable (par défaut sur systèmes modernes)
gcc -z noexecstack programme.c

# Vérifier
readelf -l programme | grep GNU_STACK
# GNU_STACK      0x000000 0x0000000000000000 ... RW  0x10
#                                                 ^^
#                                                 Pas de 'E' (Execute)
```

**Note** : Sur les systèmes modernes (Linux, macOS), le NX bit est activé par défaut.

---

### 6. Optimisation et warnings

#### Optimisation

L'optimisation peut révéler des bugs et améliorer la sécurité.

```bash
# Niveau 2 recommandé (bon compromis)
gcc -O2 programme.c

# Niveau 3 pour performance maximale
gcc -O3 programme.c
```

**Attention** : L'optimisation peut masquer certains bugs pendant le développement. Utilisez `-O0` ou `-Og` pour le débogage.

#### Warnings comme erreurs

```bash
gcc -Wall -Wextra -Werror -Wformat-security programme.c
```

**Flags importants** :

| Flag | Description |
|------|-------------|
| `-Wall` | Active la plupart des warnings |
| `-Wextra` | Warnings supplémentaires |
| `-Werror` | Traite les warnings comme des erreurs |
| `-Wformat=2` | Vérifications strictes de format |
| `-Wformat-security` | Détecte les format string vulnerabilities |
| `-Werror=format-security` | Traite les format strings comme erreurs fatales |

---

## Configuration recommandée par niveau

### Niveau 1 : Minimum vital (Débutant)

```bash
gcc -fstack-protector-strong \
    -D_FORTIFY_SOURCE=2 -O2 \
    -Wall -Wextra \
    programme.c -o programme
```

**Ce que ça fait** :
- ✅ Stack canaries sur fonctions critiques
- ✅ Vérification des fonctions dangereuses
- ✅ Warnings activés

### Niveau 2 : Production standard (Recommandé)

```bash
gcc -fstack-protector-strong \
    -D_FORTIFY_SOURCE=2 -O2 \
    -fPIE -pie \
    -Wl,-z,relro,-z,now \
    -Wall -Wextra -Werror \
    -Wformat=2 -Wformat-security \
    programme.c -o programme
```

**Ce que ça fait** :
- ✅ Tout du niveau 1
- ✅ PIE activé (ASLR)
- ✅ Full RELRO (GOT en lecture seule)
- ✅ Warnings traités comme erreurs

### Niveau 3 : Paranoïaque (Sécurité maximale)

```bash
gcc -fstack-protector-all \
    -D_FORTIFY_SOURCE=3 -O2 \
    -fPIE -pie \
    -Wl,-z,relro,-z,now \
    -Wl,-z,noexecstack \
    -Wall -Wextra -Werror \
    -Wformat=2 -Wformat-security \
    -Wconversion -Wsign-conversion \
    -Wstrict-overflow=5 \
    -fstack-clash-protection \
    -fcf-protection=full \
    programme.c -o programme
```

**Ce que ça fait** :
- ✅ Tout du niveau 2
- ✅ Stack canaries partout
- ✅ FORTIFY niveau 3
- ✅ Protection contre stack clash
- ✅ Control Flow Guard (Intel CET)
- ✅ Warnings additionnels

**Note** : Niveau 3 peut avoir un impact sur les performances (5-10%).

---

## Flags avancés

### 1. Stack Clash Protection (-fstack-clash-protection)

Protège contre les attaques qui tentent de faire sauter la guard page de la pile.

```bash
gcc -fstack-clash-protection programme.c
```

### 2. Control Flow Integrity (-fcf-protection)

Sur processeurs Intel récents (≥ 11ème génération), protège contre les attaques ROP.

```bash
# Protection complète (shadow stack + IBT)
gcc -fcf-protection=full programme.c

# Shadow stack uniquement
gcc -fcf-protection=return programme.c

# IBT uniquement
gcc -fcf-protection=branch programme.c
```

### 3. SafeStack (-fsanitize=safe-stack)

Sépare les variables dangereuses (buffers) des métadonnées critiques (adresses de retour).

```bash
clang -fsanitize=safe-stack programme.c
```

**Note** : Disponible uniquement avec Clang.

### 4. Shadow Call Stack (-fsanitize=shadow-call-stack)

Maintient une pile parallèle pour les adresses de retour.

```bash
clang -fsanitize=shadow-call-stack programme.c
```

**Note** : Nécessite support matériel (ARM64, RISC-V).

---

## Makefile avec hardening

### Makefile simple

```makefile
CC = gcc  
CFLAGS = -std=c11 -Wall -Wextra -Werror  

# Flags de hardening
HARDENING = -fstack-protector-strong \
            -D_FORTIFY_SOURCE=2 \
            -fPIE \
            -Wformat=2 \
            -Wformat-security \
            -Werror=format-security

LDFLAGS = -pie \
          -Wl,-z,relro,-z,now \
          -Wl,-z,noexecstack

# Configuration selon le mode
ifeq ($(DEBUG),1)
    CFLAGS += -g -O0 -DDEBUG
else
    CFLAGS += -O2 $(HARDENING)
endif

SOURCES = main.c utils.c  
OBJECTS = $(SOURCES:.c=.o)  
TARGET = monprogramme  

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJECTS) $(TARGET)

# Build de débogage
debug:
	$(MAKE) DEBUG=1

# Build de production
release:
	$(MAKE) DEBUG=0

.PHONY: all clean debug release
```

**Utilisation** :
```bash
# Build production avec hardening
make release

# Build debug sans hardening
make debug
```

### Makefile avancé avec vérification

```makefile
CC = gcc  
CFLAGS = -std=c11 -Wall -Wextra -Werror  

# Hardening flags
HARDENING = -fstack-protector-strong \
            -D_FORTIFY_SOURCE=2 \
            -fPIE \
            -Wformat=2 \
            -Wformat-security \
            -Werror=format-security \
            -fstack-clash-protection

LDFLAGS = -pie \
          -Wl,-z,relro,-z,now \
          -Wl,-z,noexecstack

# Vérifier si Control Flow Guard est supporté
CFG_SUPPORT := $(shell $(CC) -fcf-protection=full -x c -c /dev/null -o /dev/null 2>/dev/null && echo yes)  
ifeq ($(CFG_SUPPORT),yes)  
    HARDENING += -fcf-protection=full
endif

ifeq ($(DEBUG),1)
    CFLAGS += -g -O0 -DDEBUG
else
    CFLAGS += -O2 $(HARDENING)
endif

SOURCES = main.c utils.c  
OBJECTS = $(SOURCES:.c=.o)  
TARGET = monprogramme  

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	@echo "Build terminé avec hardening"
	@$(MAKE) verify-hardening

%.o: %.c
	$(CC) $(CFLAGS) -c $<

# Vérifier le hardening du binaire
verify-hardening:
	@echo "=== Vérification du hardening ==="
	@if command -v checksec >/dev/null 2>&1; then \
		checksec --file=$(TARGET); \
	else \
		echo "checksec non installé, vérification manuelle :"; \
		readelf -l $(TARGET) | grep GNU_STACK || echo "NX: ?"; \
		readelf -l $(TARGET) | grep GNU_RELRO || echo "RELRO: ?"; \
		file $(TARGET) | grep -q "pie executable" && echo "PIE: Oui" || echo "PIE: Non"; \
	fi

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean verify-hardening
```

---

## Vérification du hardening

### 1. checksec (Outil recommandé)

**Installation** :
```bash
# Ubuntu/Debian
sudo apt install checksec

# Ou via pip
pip install checksec.py

# Ou le script shell
wget https://github.com/slimm609/checksec.sh/raw/master/checksec  
chmod +x checksec  
```

**Utilisation** :
```bash
checksec --file=./monprogramme
```

**Exemple de sortie** :
```
RELRO           STACK CANARY      NX            PIE             FORTIFY  
Full RELRO      Canary found      NX enabled    PIE enabled     Yes  
```

### 2. Vérification manuelle avec readelf

```bash
# Vérifier NX (pile non-exécutable)
readelf -l programme | grep GNU_STACK
# Doit contenir RW (pas RWE)

# Vérifier RELRO
readelf -l programme | grep GNU_RELRO
# Doit être présent

# Vérifier si c'est un PIE
readelf -h programme | grep Type
# Doit afficher "DYN (Shared object file)"

# Ou plus simple :
file programme
# Doit contenir "pie executable"
```

### 3. Vérification avec hardening-check

```bash
# Installation
sudo apt install devscripts

# Vérification
hardening-check programme
```

**Exemple de sortie** :
```
programme:
 Position Independent Executable: yes
 Stack protected: yes
 Fortify Source functions: yes
 Read-only relocations: yes
 Immediate binding: yes
```

---

## CMake avec hardening

### CMakeLists.txt avec hardening

```cmake
cmake_minimum_required(VERSION 3.10)  
project(MonProjet C)  

set(CMAKE_C_STANDARD 11)  
set(CMAKE_C_STANDARD_REQUIRED ON)  

# Sources
set(SOURCES
    src/main.c
    src/utils.c
)

# Créer l'exécutable
add_executable(monprogramme ${SOURCES})

# Flags de compilation standard
target_compile_options(monprogramme PRIVATE
    -Wall
    -Wextra
    -Werror
)

# Flags de hardening (seulement en Release)
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(monprogramme PRIVATE
        -fstack-protector-strong
        -fPIE
        -Wformat=2
        -Wformat-security
        -Werror=format-security
    )

    target_compile_definitions(monprogramme PRIVATE
        _FORTIFY_SOURCE=2
    )

    target_link_options(monprogramme PRIVATE
        -pie
        -Wl,-z,relro,-z,now
        -Wl,-z,noexecstack
    )

    # Optimisation
    target_compile_options(monprogramme PRIVATE -O2)
else()
    # Mode debug
    target_compile_options(monprogramme PRIVATE -g -O0)
endif()

# Fonction pour vérifier le hardening
add_custom_command(TARGET monprogramme POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Vérification du hardening..."
    COMMAND bash -c "command -v checksec >/dev/null && checksec --file=$<TARGET_FILE:monprogramme> || echo 'checksec non disponible'"
)
```

**Build** :
```bash
# Release avec hardening
cmake -DCMAKE_BUILD_TYPE=Release -B build  
cmake --build build  

# Debug sans hardening
cmake -DCMAKE_BUILD_TYPE=Debug -B build-debug  
cmake --build build-debug  
```

---

## Impact sur les performances

### Mesure de l'overhead

| Protection | Overhead typique | Impact |
|------------|------------------|--------|
| Stack canaries | < 1% | Négligeable |
| FORTIFY_SOURCE | < 1% | Négligeable |
| PIE | < 1% | Négligeable |
| Full RELRO | < 1% startup | Temps de démarrage |
| NX bit | 0% | Aucun |
| CFI/CET | 1-5% | Faible |
| SafeStack | 5-10% | Modéré |

**Overhead total** avec configuration standard (Niveau 2) : **< 3%**

### Benchmark simple

```c
// benchmark.c
#include <stdio.h>
#include <time.h>

#define ITERATIONS 100000000

void fonction_test(int *array, int size) {
    for (int i = 0; i < size; i++) {
        array[i] = i * 2;
    }
}

int main(void) {
    int array[1000];

    clock_t start = clock();

    for (int i = 0; i < ITERATIONS; i++) {
        fonction_test(array, 1000);
    }

    clock_t end = clock();
    double temps = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Temps d'exécution : %.2f secondes\n", temps);

    return 0;
}
```

**Test** :
```bash
# Sans hardening
gcc -O2 benchmark.c -o bench_normal  
time ./bench_normal  

# Avec hardening
gcc -O2 -fstack-protector-strong -D_FORTIFY_SOURCE=2 -fPIE -pie \
    -Wl,-z,relro,-z,now benchmark.c -o bench_hardened
time ./bench_hardened

# Comparer les résultats
```

**Résultat typique** : Différence < 2%

---

## Cas particuliers et limitations

### 1. Bibliothèques partagées

Pour les bibliothèques `.so`, utilisez `-fPIC` au lieu de `-fPIE` :

```bash
# Compilation d'une bibliothèque
gcc -fPIC -shared -o libmylib.so mylib.c \
    -fstack-protector-strong \
    -D_FORTIFY_SOURCE=2 -O2 \
    -Wl,-z,relro,-z,now
```

### 2. Code assembleur

Si votre code contient de l'assembleur, certaines protections peuvent causer des problèmes :

```bash
# Désactiver stack canaries pour un fichier spécifique
gcc -fno-stack-protector asm_file.c -o asm_file.o
```

### 3. Performance critique

Pour du code où chaque cycle CPU compte :

```bash
# Mesurer d'abord, optimiser ensuite
gcc -O3 -fstack-protector-strong programme.c
# Tester les performances

# Si vraiment nécessaire, désactiver sélectivement
# Mais documenter pourquoi !
```

### 4. Compatibilité

Certaines protections nécessitent :
- Versions récentes du compilateur (GCC ≥ 8, Clang ≥ 7)
- Support du noyau Linux (≥ 4.14 pour certaines features)
- Support matériel (Intel CET, ARM PAC)

---

## Débogage avec hardening

### Problème : Les sanitizers désactivent certaines protections

```bash
# Avec ASan, les stack canaries sont remplacés
gcc -fsanitize=address programme.c
# Stack canaries non actifs !
```

**Solution** : Tester en deux phases

```bash
# Phase 1 : Développement avec sanitizers
gcc -fsanitize=address,undefined -g programme.c

# Phase 2 : Test final avec hardening complet
gcc -fstack-protector-strong -D_FORTIFY_SOURCE=2 -O2 \
    -fPIE -pie -Wl,-z,relro,-z,now programme.c
```

### Débogage avec GDB

```bash
# Les protections peuvent compliquer le débogage
gcc -g -O0 -fstack-protector-strong programme.c

# GDB fonctionne normalement
gdb ./programme
(gdb) break main
(gdb) run
```

---

## Checklist de hardening

### ✅ Compilation

- [ ] `-fstack-protector-strong` activé
- [ ] `-D_FORTIFY_SOURCE=2` avec `-O2` ou plus
- [ ] `-fPIE -pie` pour l'exécutable
- [ ] `-Wl,-z,relro,-z,now` pour Full RELRO
- [ ] `-Wl,-z,noexecstack` pour NX
- [ ] `-Wall -Wextra -Werror` pour les warnings
- [ ] `-Wformat=2 -Wformat-security` pour format strings

### ✅ Vérification

- [ ] `checksec` exécuté sur le binaire
- [ ] PIE activé (ASLR)
- [ ] Stack canaries présents
- [ ] RELRO complet
- [ ] NX activé
- [ ] FORTIFY détecté

### ✅ Documentation

- [ ] Flags documentés dans le README
- [ ] Justification des désactivations (si applicable)
- [ ] Instructions de build claires

### ✅ CI/CD

- [ ] Build avec hardening dans le pipeline
- [ ] Vérification automatique du hardening
- [ ] Tests de non-régression des performances

---

## Scripts utiles

### Script de vérification complet

```bash
#!/bin/bash
# check_hardening.sh

BINARY=$1

if [ -z "$BINARY" ]; then
    echo "Usage: $0 <binary>"
    exit 1
fi

if [ ! -f "$BINARY" ]; then
    echo "Erreur : $BINARY introuvable"
    exit 1
fi

echo "=== Vérification du hardening pour $BINARY ==="  
echo  

# Vérifier avec checksec si disponible
if command -v checksec >/dev/null 2>&1; then
    checksec --file="$BINARY"
    exit 0
fi

# Sinon, vérification manuelle
echo "PIE:"  
file "$BINARY" | grep -q "pie executable" && echo "  ✓ Activé" || echo "  ✗ Désactivé"  

echo "NX:"  
readelf -l "$BINARY" | grep "GNU_STACK" | grep -q " RW " && echo "  ✓ Activé (pas d'exécution)" || echo "  ✗ Désactivé"  

echo "Stack Canary:"  
readelf -s "$BINARY" | grep -q "__stack_chk_fail" && echo "  ✓ Détecté" || echo "  ✗ Non détecté"  

echo "RELRO:"  
readelf -l "$BINARY" | grep -q "GNU_RELRO" && echo "  ✓ Activé" || echo "  ✗ Désactivé"  

echo "BIND_NOW (Full RELRO):"  
readelf -d "$BINARY" | grep -q "BIND_NOW" && echo "  ✓ Activé" || echo "  ✗ Désactivé"  

echo "FORTIFY:"  
readelf -s "$BINARY" | grep -q "__.*_chk" && echo "  ✓ Détecté" || echo "  ✗ Non détecté"  
```

**Utilisation** :
```bash
chmod +x check_hardening.sh
./check_hardening.sh monprogramme
```

---

## Erreurs courantes et solutions

### Erreur 1 : FORTIFY sans optimisation

```bash
$ gcc -D_FORTIFY_SOURCE=2 programme.c
# Pas d'erreur, mais FORTIFY n'est PAS actif !
```

**Solution** :
```bash
gcc -D_FORTIFY_SOURCE=2 -O2 programme.c
```

### Erreur 2 : PIE pour bibliothèques

```bash
$ gcc -fPIE -shared -o libmylib.so mylib.c
# Erreur : -fPIE incompatible avec -shared
```

**Solution** :
```bash
gcc -fPIC -shared -o libmylib.so mylib.c
```

### Erreur 3 : Linking avec flags manquants

```bash
$ gcc -fPIE main.c -o main  # Oubli de -pie au linking
# Le binaire n'est pas PIE !
```

**Solution** :
```bash
gcc -fPIE main.c -pie -o main
```

### Erreur 4 : RELRO sans optimization

```bash
$ gcc -Wl,-z,relro,-z,now main.c
# Peut causer des erreurs avec certaines libs
```

**Solution** : Vérifier la compatibilité avec les bibliothèques utilisées.

---

## Standards et conformité

### 1. OWASP

L'OWASP recommande :
- ✅ Stack canaries
- ✅ ASLR/PIE
- ✅ RELRO
- ✅ NX bit

### 2. CIS Benchmarks

Centre for Internet Security recommande :
- ✅ Compiler avec `-fPIE -pie`
- ✅ Full RELRO
- ✅ Stack protector

### 3. CERT C

CERT C Secure Coding Standard requiert :
- ✅ Validation des entrées (non hardening)
- ✅ Compilation avec warnings
- ✅ Stack protection recommandée

---

## Ressources et références

### Documentation officielle

- **GCC Instrumentation Options** : https://gcc.gnu.org/onlinedocs/gcc/Instrumentation-Options.html
- **GCC Security Options** : https://gcc.gnu.org/wiki/Hardening
- **checksec** : https://github.com/slimm609/checksec.sh

### Guides de hardening

- Debian : https://wiki.debian.org/Hardening
- Red Hat : https://developers.redhat.com/articles/2022/06/02/use-compiler-flags-stack-protection-gcc-and-clang
- Ubuntu : https://wiki.ubuntu.com/Security/Features

### Outils

- **checksec.sh** : Vérification de hardening
- **pwntools** : Framework pour tester les protections
- **ROPgadget** : Tester les protections contre ROP

---

## Conclusion

### Récapitulatif

Le hardening est **essentiel** pour tout code en production :

- ✅ **Facile à mettre en place** : Quelques flags de compilation
- ✅ **Impact minimal** : < 3% de performance
- ✅ **Protection maximale** : Rend l'exploitation très difficile
- ✅ **Standard de l'industrie** : Requis par les certifications

### Configuration minimale recommandée

```bash
CFLAGS = -fstack-protector-strong \
         -D_FORTIFY_SOURCE=2 -O2 \
         -fPIE \
         -Wall -Wextra -Werror \
         -Wformat=2 -Wformat-security

LDFLAGS = -pie \
          -Wl,-z,relro,-z,now \
          -Wl,-z,noexecstack
```

### Les 3 règles d'or

1. **Toujours compiler avec hardening en production**
2. **Vérifier le hardening avec checksec**
3. **Documenter les désactivations si nécessaire**

### Message final

> Le hardening ne remplace pas du bon code, mais il transforme une vulnérabilité critique en un exploit complexe, voire impossible.

**Protection en couches** :
```
Code sécurisé (votre travail)
    +
Hardening (flags de compilation)
    +
Moindre privilège (configuration)
    +
Monitoring (détection)
    =
Sécurité en profondeur
```

### Prochaine étape

Vous avez maintenant couvert tous les aspects de la sécurité en C :
1. Standards CERT C
2. Validation des entrées
3. Prévention des buffer overflows
4. Format string vulnerabilities
5. Integer overflows
6. Principe du moindre privilège
7. Analyse statique et fuzzing
8. **Hardening flags** ← Vous êtes ici

**Le code sécurisé** = Bonne conception + Bonnes pratiques + Hardening + Tests + Monitoring

---

**🛡️ Conseil final** : Créez un template de Makefile/CMake avec hardening et utilisez-le pour tous vos nouveaux projets. Le hardening doit être la **norme**, pas l'exception !

⏭️ [Optimisation et Performance](/27-optimisation-performance/README.md)
