🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 35.1 Analyse de core dumps en production

## Introduction

Un **core dump** (ou fichier core) est une photographie complète de l'état de la mémoire d'un programme au moment où il s'est arrêté brutalement. C'est comme une "boîte noire" qui enregistre tout ce qui se passait dans votre application juste avant le crash.

En production, les core dumps sont souvent votre **seul moyen** de comprendre pourquoi une application critique s'est arrêtée de manière inattendue, surtout quand le problème ne se reproduit pas facilement en développement.

### Pourquoi les core dumps sont essentiels en production ?

- **Debugging post-mortem** : Analyser un crash sans pouvoir reproduire le bug
- **Erreurs intermittentes** : Capturer des problèmes qui n'arrivent qu'une fois sur mille
- **Contexte complet** : Accès à toutes les variables, la pile d'appels, et l'état mémoire
- **Production vs développement** : Les bugs en production ont souvent des causes différentes (charge, données réelles, timing)

---

## Configuration pour générer des core dumps

### Vérifier la limite de taille des core dumps

Par défaut sur beaucoup de systèmes, la génération de core dumps est **désactivée** pour économiser de l'espace disque.

```bash
# Vérifier la limite actuelle (0 = désactivé)
ulimit -c
```

### Activer les core dumps

```bash
# Pour la session courante : taille illimitée
ulimit -c unlimited

# Pour rendre permanent (ajouter dans /etc/security/limits.conf)
* soft core unlimited
* hard core unlimited
```

### Configurer l'emplacement et le nom des core dumps

Par défaut, les core dumps sont créés dans le répertoire de travail du programme avec le nom `core` ou `core.PID`. En production, il est recommandé de centraliser ces fichiers.

```bash
# Voir la configuration actuelle
cat /proc/sys/kernel/core_pattern

# Configuration recommandée pour la production (nécessite root)
echo "/var/crash/core.%e.%p.%t" | sudo tee /proc/sys/kernel/core_pattern
```

**Légende des motifs** :
- `%e` : Nom de l'exécutable
- `%p` : PID du processus
- `%t` : Timestamp (secondes depuis epoch)
- `%u` : UID de l'utilisateur
- `%s` : Numéro du signal ayant causé le dump

**Exemple de nom généré** : `/var/crash/core.myapp.12345.1709123456`

### Créer le répertoire de stockage

```bash
sudo mkdir -p /var/crash
sudo chmod 1777 /var/crash  # Accessible à tous, mais chacun ne voit que ses fichiers
```

---

## Anatomie d'un core dump

Un core dump contient :

1. **Le code du programme** (sections .text)
2. **Toutes les données** (sections .data, .bss)
3. **La pile d'exécution** (stack) de chaque thread
4. **Le tas** (heap) avec toutes les allocations dynamiques
5. **Les registres CPU** au moment du crash
6. **Les bibliothèques partagées** mappées en mémoire

### Taille des core dumps

⚠️ **Attention** : Un core dump peut être **très volumineux** (plusieurs Go pour une application avec beaucoup de mémoire allouée).

```bash
# Limiter la taille si nécessaire (en Ko)
ulimit -c 1048576  # Limite à 1 Go
```

---

## Analyse avec GDB : Les bases

### Charger un core dump

```bash
# Syntaxe générale
gdb <exécutable> <fichier_core>

# Exemple
gdb ./myapp /var/crash/core.myapp.12345.1709123456
```

**Important** : Vous devez avoir :
- L'exécutable **exact** qui a généré le core (même version)
- Les symboles de débogage (compilé avec `-g`)

### Premières commandes essentielles

```gdb
# 1. Voir où le crash s'est produit
(gdb) backtrace
# ou
(gdb) bt

# 2. Voir le code source autour du crash
(gdb) list

# 3. Examiner les variables locales
(gdb) info locals

# 4. Examiner les arguments de la fonction
(gdb) info args

# 5. Voir tous les threads
(gdb) info threads

# 6. Changer de frame dans la pile
(gdb) frame 3
(gdb) up      # Monter d'un niveau
(gdb) down    # Descendre d'un niveau
```

### Exemple d'analyse simple

Supposons ce code qui crashe :

```c
#include <stdio.h>
#include <stdlib.h>

void process_data(int *array, int size) {
    for (int i = 0; i <= size; i++) {  // Bug : <= au lieu de <
        array[i] = i * 2;
    }
}

int main() {
    int *data = malloc(10 * sizeof(int));
    process_data(data, 10);
    free(data);
    return 0;
}
```

**Compilation avec symboles** :
```bash
gcc -g -o myapp myapp.c
ulimit -c unlimited
./myapp
# Segmentation fault (core dumped)
```

**Analyse du core dump** :
```gdb
$ gdb ./myapp core
(gdb) bt
#0  0x0000555555555189 in process_data (array=0x555555559260, size=10) at myapp.c:6
#1  0x00005555555551b5 in main () at myapp.c:12

(gdb) frame 0
(gdb) list
1   #include <stdio.h>
2   #include <stdlib.h>
3
4   void process_data(int *array, int size) {
5       for (int i = 0; i <= size; i++) {
6           array[i] = i * 2;      ← CRASH ICI
7       }
8   }

(gdb) print i
$1 = 10

(gdb) print size
$2 = 10

(gdb) print array
$3 = (int *) 0x555555559260
```

**Diagnostic** : On accède à `array[10]` alors que le tableau ne va que de 0 à 9 (buffer overflow).

---

## Techniques d'investigation avancées

### 1. Examiner la mémoire brute

```gdb
# Examiner 16 mots (4 octets) à partir d'une adresse
(gdb) x/16xw 0x555555559260

# Examiner en tant que chaîne de caractères
(gdb) x/s 0x7fffffffe000

# Formats :
# x = hexadécimal, d = décimal, s = string
# b = byte, h = halfword (2 bytes), w = word (4 bytes), g = giant (8 bytes)
```

### 2. Analyse des pointeurs NULL ou invalides

```gdb
# Si le crash vient d'un pointeur NULL
(gdb) bt
#0  0x0000555555555180 in process_user (user=0x0) at app.c:45

(gdb) frame 0
(gdb) print user
$1 = (struct User *) 0x0

# Remonter pour trouver d'où vient le NULL
(gdb) up
(gdb) print user_list
```

### 3. Identifier les corruptions de mémoire

```gdb
# Vérifier les structures critiques
(gdb) print *mystruct
$1 = {
  magic_number = 0,         ← Devrait être 0xDEADBEEF
  data = 0x4141414141414141  ← Suspect (AAAAAAAA)
}

# Examiner le heap autour de l'objet
(gdb) x/100xw mystruct
```

### 4. Analyse multi-thread

```gdb
# Lister tous les threads
(gdb) info threads
  Id   Target Id         Frame
* 1    Thread 0x7f... (LWP 12345) in __pthread_cond_wait
  2    Thread 0x7f... (LWP 12346) in read
  3    Thread 0x7f... (LWP 12347) in malloc

# Basculer sur un thread spécifique
(gdb) thread 3
(gdb) bt

# Afficher la backtrace de TOUS les threads
(gdb) thread apply all bt
```

### 5. Examiner les variables globales

```gdb
# Variables globales et statiques
(gdb) print global_counter
(gdb) print 'fichier.c'::static_var
```

---

## Cas d'usage typiques en production

### Scénario 1 : Segmentation Fault aléatoire

**Symptômes** : Application crashe une fois par jour, impossible à reproduire.

**Démarche** :
1. Activer les core dumps avec timestamp
2. Attendre le prochain crash
3. Analyser le core avec GDB :
   ```gdb
   (gdb) bt full     # Backtrace complète avec toutes les variables
   (gdb) info registers
   ```
4. Chercher des patterns : pointeurs NULL, indices hors limites, corruption

### Scénario 2 : Deadlock

**Symptômes** : Application figée, mais ne crashe pas (pas de core dump naturel).

**Solution** : Forcer la génération d'un core dump sur un processus vivant :

```bash
# Trouver le PID
ps aux | grep myapp

# Générer un core dump sans tuer le processus (Linux >= 3.18)
sudo gcore -o /var/crash/myapp 12345

# Analyser
gdb ./myapp /var/crash/myapp.12345
(gdb) info threads
(gdb) thread apply all bt
```

Chercher des threads bloqués sur des mutex :
```gdb
(gdb) thread 2
(gdb) bt
#0  __pthread_mutex_lock
#1  acquire_lock at myapp.c:123
```

### Scénario 3 : Corruption de heap

**Symptômes** : Crash dans `malloc()` ou `free()` avec messages cryptiques.

**Démarche** :
```gdb
(gdb) bt
#0  malloc.c:2374
#1  free (ptr=0x123456) at malloc.c:2957
#2  cleanup at myapp.c:456

# Le crash est dans malloc, mais la cause est ailleurs
# Chercher des double-free, buffer overflows, ou use-after-free

(gdb) frame 2
(gdb) print ptr
(gdb) x/16xw ptr   # Examiner la mémoire
```

**Astuce** : Recompiler avec AddressSanitizer pour le prochain crash :
```bash
gcc -g -fsanitize=address -o myapp myapp.c
```

---

## Bonnes pratiques pour la production

### 1. Compilation avec symboles séparés

En production, vous ne voulez pas inclure les symboles de debug dans le binaire (taille).

**Solution** : Fichiers de symboles séparés (.debug)

```bash
# Compiler avec debug
gcc -g -o myapp myapp.c

# Séparer les symboles
objcopy --only-keep-debug myapp myapp.debug
strip --strip-debug --strip-unneeded myapp

# Créer un lien entre le binaire et les symboles
objcopy --add-gnu-debuglink=myapp.debug myapp
```

**Chargement dans GDB** :
```gdb
(gdb) symbol-file myapp.debug
```

### 2. Rotation des core dumps

Les core dumps prennent de la place. Automatisez leur nettoyage :

```bash
# Crontab pour supprimer les cores de plus de 7 jours
0 2 * * * find /var/crash -name "core.*" -mtime +7 -delete
```

### 3. Logging avant crash

Ajoutez des signaux handlers pour logger avant de crasher :

```c
#include <signal.h>
#include <execinfo.h>

void signal_handler(int sig) {
    void *array[10];
    size_t size = backtrace(array, 10);

    fprintf(stderr, "Error: signal %d:\n", sig);
    backtrace_symbols_fd(array, size, STDERR_FILENO);

    // Rétablir le handler par défaut et re-raise
    signal(sig, SIG_DFL);
    raise(sig);
}

int main() {
    signal(SIGSEGV, signal_handler);
    signal(SIGABRT, signal_handler);
    // ... votre code
}
```

### 4. Automatisation de l'analyse

Script pour analyser automatiquement les nouveaux core dumps :

```bash
#!/bin/bash
# auto_analyze_core.sh

CORE_FILE=$1
EXECUTABLE=$2

if [ ! -f "$CORE_FILE" ]; then
    echo "Core file not found: $CORE_FILE"
    exit 1
fi

echo "=== Analyzing $CORE_FILE ==="
gdb -batch -ex "bt full" -ex "info registers" -ex "thread apply all bt" \
    "$EXECUTABLE" "$CORE_FILE" > "${CORE_FILE}.analysis.txt"

echo "Analysis saved to ${CORE_FILE}.analysis.txt"
```

### 5. Intégration avec systemd

Pour les services systemd, configurer la gestion des core dumps :

```ini
# /etc/systemd/system/myapp.service
[Service]
LimitCORE=infinity
```

Vérifier :
```bash
systemctl show myapp.service | grep LimitCORE
```

---

## Outils complémentaires

### `coredumpctl` (systemd)

Sur les systèmes avec systemd, les core dumps sont gérés par `systemd-coredump` :

```bash
# Lister les core dumps
coredumpctl list

# Analyser le dernier core dump
coredumpctl debug

# Extraire un core dump spécifique
coredumpctl dump <PID> > core.dump
```

### Analyse avec LLDB (alternative à GDB)

```bash
# Charger un core dump
lldb ./myapp -c core.12345

# Commandes similaires
(lldb) bt
(lldb) frame variable
(lldb) thread list
```

---

## Limitations et pièges à éviter

### 1. Core dump incomplet

Si le programme a beaucoup de mémoire mappée, le core peut être tronqué.

**Solution** : Configurer `/proc/sys/kernel/core_pipe_limit`

### 2. Binaire et core dump désynchronisés

Si vous avez redéployé l'application entre le crash et l'analyse, le binaire ne correspondra pas.

**Solution** : Versionner et archiver chaque build avec ses symboles

### 3. ASLR (Address Space Layout Randomization)

Les adresses changent à chaque exécution, compliquant l'analyse.

**Pour le debugging** (pas en production !) :
```bash
echo 0 | sudo tee /proc/sys/kernel/randomize_va_space
```

### 4. Informations sensibles

Les core dumps peuvent contenir des mots de passe, tokens, clés API en mémoire.

**Solutions** :
- Effacer les données sensibles de la mémoire après usage
- Restreindre l'accès aux core dumps : `chmod 600 /var/crash/core.*`
- Utiliser `core_pattern` avec filtrage : `|/usr/local/bin/filter_core`

---

## Checklist d'analyse d'un core dump en production

1. ✅ **Collecter les informations de contexte**
   - Date et heure du crash
   - Version exacte de l'application
   - Charge système au moment du crash
   - Logs applicatifs avant le crash

2. ✅ **Vérifier les prérequis**
   - Binaire correspondant au core dump
   - Symboles de débogage disponibles
   - Bibliothèques partagées identiques

3. ✅ **Analyse initiale avec GDB**
   ```
   bt full
   info threads
   info registers
   ```

4. ✅ **Identifier le point de crash**
   - Frame 0 de la backtrace
   - Code source correspondant
   - Valeurs des variables locales

5. ✅ **Remonter la chaîne d'appels**
   - Examiner chaque frame
   - Chercher l'origine du problème

6. ✅ **Hypothèses et validation**
   - Formuler des hypothèses (NULL pointer, overflow, ...)
   - Vérifier avec `print`, `x/`, `whatis`

7. ✅ **Documentation et correction**
   - Documenter la cause racine
   - Créer un ticket avec la backtrace
   - Ajouter un test de non-régression

---

## Ressources pour aller plus loin

### Documentation officielle
- `man core` : Format des core dumps
- `man gdb` : Manuel complet de GDB
- [GDB Documentation](https://sourceware.org/gdb/documentation/)

### Outils avancés
- **Crash Utility** : Analyse de core dumps kernel
- **RetroScope** : Time-travel debugging
- **rr (Mozilla)** : Enregistrement et replay de l'exécution

### Lectures recommandées
- *"Advanced Linux Programming"* par Mark Mitchell et al.
- *"The Art of Debugging with GDB"* par Norman Matloff

---

## Résumé

L'analyse de core dumps en production est une compétence essentielle pour tout ingénieur système. Les points clés à retenir :

- **Préparez-vous** : Configurez la génération de core dumps **avant** le crash
- **Symboles de débogage** : Toujours compiler avec `-g` et archiver les binaires
- **GDB est votre ami** : Maîtrisez `bt`, `frame`, `print`, `x/`
- **Méthodologie** : Analyse systématique du point de crash vers la cause racine
- **Sécurité** : Protégez les core dumps, ils contiennent des données sensibles
- **Automatisation** : Scripts d'analyse et rotation automatique

Un core dump bien analysé peut transformer un bug mystérieux en production en une correction triviale. C'est votre **boîte noire** pour comprendre ce qui s'est réellement passé quand tout a mal tourné.

---


⏭️ [Résolution de memory leaks](/35-debugging-code-complexe/02-resolution-memory-leaks.md)
