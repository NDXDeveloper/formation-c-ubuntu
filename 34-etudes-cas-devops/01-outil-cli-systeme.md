🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 34.1 Création d'un outil CLI système

## Introduction

Les outils en ligne de commande (CLI - Command Line Interface) sont au cœur de l'écosystème Linux et UNIX. Des commandes que vous utilisez quotidiennement comme `ls`, `grep`, `find`, `top`, `ps` sont toutes écrites en C. Dans cette section, nous allons apprendre à créer nos propres outils CLI système professionnels en C.

### Pourquoi créer des outils CLI en C ?

**Avantages du C pour les outils système :**

1. **Performance** : Exécution rapide, démarrage instantané
2. **Légèreté** : Binaires compacts, faible empreinte mémoire
3. **Portabilité** : Fonctionne sur tous les systèmes UNIX/Linux
4. **Intégration système** : Accès direct aux appels système Linux
5. **Scripting** : Facilement intégrable dans des scripts shell
6. **Tradition** : S'inscrit dans la philosophie UNIX

**Exemples d'outils CLI système :**
- Outils de monitoring (collecte de métriques système)
- Parsers de logs (analyse et extraction de données)
- Utilitaires de fichiers (recherche, conversion, validation)
- Outils réseau (diagnostics, tests de connexion)
- Gestionnaires de processus (supervision, contrôle)

---

## Philosophie UNIX des outils CLI

### Les principes fondamentaux

Les outils UNIX suivent une philosophie bien établie, énoncée par Doug McIlroy :

> **"Make each program do one thing well."**
>
> Faites en sorte que chaque programme fasse une chose, et qu'il le fasse bien.

**Les 3 règles d'or :**

1. **Une seule responsabilité** : Un outil = une fonction claire
2. **Composabilité** : Les outils peuvent être combinés via pipes
3. **Simplicité** : Interface simple, comportement prévisible

### Le modèle de communication UNIX

```bash
# Chaque outil lit sur stdin, écrit sur stdout, erreurs sur stderr
cat fichier.txt | grep "erreur" | wc -l

# Redirection des sorties
./mon_outil < input.txt > output.txt 2> errors.log

# Chaînage de commandes
./outil1 | ./outil2 | ./outil3
```

**Les 3 flux standards :**
- **stdin** (0) : Entrée standard
- **stdout** (1) : Sortie standard
- **stderr** (2) : Sortie d'erreur

---

## Anatomie d'un outil CLI système

### Structure typique

```
mon_outil [OPTIONS] [ARGUMENTS]
```

**Composants essentiels :**

1. **Parsing d'arguments** : Analyser les options et paramètres
2. **Interaction système** : Lire fichiers, exécuter commandes, collecter infos
3. **Traitement des données** : Logique métier de l'outil
4. **Output formaté** : Présenter les résultats de manière claire
5. **Gestion des erreurs** : Gérer les cas d'erreur proprement
6. **Codes de retour** : Indiquer succès (0) ou échec (≠0)

### Exemple minimal : "Hello Tool"

Voici un squelette de base pour un outil CLI :

```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // 1. Parsing des arguments
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <nom>\n", argv[0]);
        return 1;
    }

    const char *nom = argv[1];

    // 2. Traitement
    printf("Bonjour, %s!\n", nom);

    // 3. Code de retour
    return 0;
}
```

**Utilisation :**
```bash
$ ./hello_tool Alice
Bonjour, Alice!

$ echo $?  # Vérifier le code de retour
0

$ ./hello_tool
Usage: ./hello_tool <nom>

$ echo $?
1
```

---

## Les trois piliers d'un outil CLI professionnel

Cette section se décompose en trois parties essentielles qui correspondent aux trois aspects fondamentaux de tout outil CLI système :

### 1. Parsing d'arguments (Section 34.1.1)

**Objectif :** Comprendre ce que l'utilisateur demande

Un outil CLI professionnel doit pouvoir accepter diverses options et paramètres :

```bash
# Options courtes et longues
./mon_outil -v --output=fichier.txt --format json input.dat

# Options avec et sans valeurs
./mon_outil -v -o fichier.txt -n 10 fichier1 fichier2

# Aide et version
./mon_outil --help
./mon_outil --version
```

**Ce que vous apprendrez :**
- Utiliser `argc` et `argv`
- Parser les options avec `getopt()` et `getopt_long()`
- Gérer les options obligatoires et optionnelles
- Valider les arguments
- Créer une aide claire

### 2. Interaction avec le système (Section 34.1.2)

**Objectif :** Effectuer des opérations système

Un outil CLI doit interagir avec le système d'exploitation :

```c
// Lire des fichiers
open(), read(), stat()

// Exécuter des commandes
system(), popen(), fork()+exec()

// Obtenir des infos système
uname(), getpid(), /proc

// Manipuler des répertoires
opendir(), readdir(), mkdir()
```

**Ce que vous apprendrez :**
- Gestion des fichiers et répertoires
- Exécution de commandes système
- Lecture d'informations système (/proc, /sys)
- Variables d'environnement
- Gestion des signaux

### 3. Output formaté (Section 34.1.3)

**Objectif :** Présenter les résultats clairement

Un bon outil CLI communique efficacement avec l'utilisateur :

```bash
# Output simple
✓ Opération réussie

# Tableaux formatés
┌────────────┬────────┬──────────┐
│ Nom        │ Taille │ Modifié  │
├────────────┼────────┼──────────┤
│ fichier1   │ 1.2 MB │ 10:30    │
│ fichier2   │ 845 KB │ 09:15    │
└────────────┴────────┴──────────┘

# Barres de progression
[████████████████████████        ] 65%

# Logs avec niveaux
[2025-01-15 14:30] INFO  Démarrage du programme
[2025-01-15 14:30] ERROR Connexion échouée
```

**Ce que vous apprendrez :**
- Formatage avec `printf()` et spécificateurs
- Couleurs et styles (codes ANSI)
- Tableaux et graphiques ASCII
- Barres de progression
- Formats structurés (JSON, CSV)

---

## Conventions et standards

### Conventions de nommage

**Options courtes** : Un seul caractère précédé de `-`
```bash
-h    # Aide
-v    # Verbose
-o    # Output
-n    # Numérique
```

**Options longues** : Mots complets précédés de `--`
```bash
--help
--verbose
--output
--version
```

**Correspondances communes :**
- `-h` / `--help` : Afficher l'aide
- `-v` / `--verbose` : Mode verbeux
- `-V` / `--version` : Version du programme
- `-q` / `--quiet` : Mode silencieux
- `-f` / `--force` : Forcer l'opération
- `-o FILE` / `--output=FILE` : Fichier de sortie

### Codes de retour standards

```c
// Standards POSIX
#define EXIT_SUCCESS 0   // Succès
#define EXIT_FAILURE 1   // Erreur générale

// Codes spécifiques (bash conventions)
// 2   - Erreur de syntaxe
// 126 - Commande non exécutable
// 127 - Commande non trouvée
// 128+n - Terminé par signal n
```

**Utilisation dans les scripts :**
```bash
#!/bin/bash

./mon_outil input.txt
if [ $? -eq 0 ]; then
    echo "Succès"
else
    echo "Échec"
    exit 1
fi
```

### Messages de sortie

**stdout** : Données et résultats normaux
```c
printf("Résultat: %d\n", valeur);
```

**stderr** : Erreurs et avertissements
```c
fprintf(stderr, "Erreur: fichier non trouvé\n");
```

---

## Bonnes pratiques générales

### 1. Interface utilisateur cohérente

```c
// ✅ BON : Aide claire et complète
void afficher_aide(const char *nom_prog) {
    printf("Usage: %s [OPTIONS] FILE...\n", nom_prog);
    printf("\nOptions:\n");
    printf("  -h, --help     Afficher cette aide\n");
    printf("  -v, --verbose  Mode verbose\n");
    printf("  -o, --output   Fichier de sortie\n");
    printf("\nExemples:\n");
    printf("  %s fichier.txt\n", nom_prog);
    printf("  %s -v -o result.txt input.dat\n", nom_prog);
}

// ❌ MAUVAIS : Aide minimale
void afficher_aide() {
    printf("Usage: prog [options] file\n");
}
```

### 2. Gestion des erreurs robuste

```c
// ✅ BON : Messages d'erreur explicites
FILE *fp = fopen(chemin, "r");
if (fp == NULL) {
    fprintf(stderr, "Erreur: impossible d'ouvrir '%s': %s\n",
            chemin, strerror(errno));
    return -1;
}

// ❌ MAUVAIS : Message vague
if (fp == NULL) {
    printf("Erreur\n");
    return -1;
}
```

### 3. Validation des entrées

```c
// ✅ BON : Validation complète
int port = atoi(port_str);
if (port < 1 || port > 65535) {
    fprintf(stderr, "Erreur: port invalide (doit être entre 1 et 65535)\n");
    return 1;
}

// ❌ MAUVAIS : Pas de validation
int port = atoi(port_str);
// Que se passe-t-il si port_str = "abc" ?
```

### 4. Comportement prévisible

```c
// ✅ BON : Comportement UNIX standard
// - Accepter input depuis stdin si pas de fichier
// - Utiliser stdout par défaut
// - Permettre '-' pour stdin/stdout

if (argc < 2 || strcmp(argv[1], "-") == 0) {
    // Lire depuis stdin
    input = stdin;
} else {
    // Lire depuis fichier
    input = fopen(argv[1], "r");
}
```

### 5. Documentation intégrée

```c
// Toujours fournir --help et --version
if (strcmp(argv[1], "--help") == 0) {
    afficher_aide(argv[0]);
    return 0;
}

if (strcmp(argv[1], "--version") == 0) {
    printf("%s version %s\n", PROGRAM_NAME, VERSION);
    return 0;
}
```

---

## Exemple complet : Squelette d'outil CLI

Voici un template de base que vous pouvez utiliser comme point de départ :

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#define VERSION "1.0.0"

// Variables globales pour les options
static int verbose = 0;
static char *output_file = NULL;

void afficher_version(const char *nom_prog) {
    printf("%s version %s\n", nom_prog, VERSION);
}

void afficher_aide(const char *nom_prog) {
    printf("Usage: %s [OPTIONS] FILE...\n\n", nom_prog);
    printf("Description:\n");
    printf("  Votre description ici.\n\n");
    printf("Options:\n");
    printf("  -h, --help              Afficher cette aide\n");
    printf("  -V, --version           Afficher la version\n");
    printf("  -v, --verbose           Mode verbose\n");
    printf("  -o, --output FILE       Fichier de sortie (défaut: stdout)\n");
    printf("\nExemples:\n");
    printf("  %s fichier.txt\n", nom_prog);
    printf("  %s -v -o resultat.txt input.dat\n", nom_prog);
}

int traiter_fichier(const char *chemin) {
    if (verbose) {
        fprintf(stderr, "Traitement de %s...\n", chemin);
    }

    // Votre logique ici
    FILE *fp = fopen(chemin, "r");
    if (fp == NULL) {
        fprintf(stderr, "Erreur: impossible d'ouvrir '%s'\n", chemin);
        return -1;
    }

    // Traitement...

    fclose(fp);
    return 0;
}

int main(int argc, char *argv[]) {
    // Options longues
    static struct option long_options[] = {
        {"help",    no_argument,       0, 'h'},
        {"version", no_argument,       0, 'V'},
        {"verbose", no_argument,       0, 'v'},
        {"output",  required_argument, 0, 'o'},
        {0, 0, 0, 0}
    };

    // Parsing des options
    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "hVvo:",
                              long_options, &option_index)) != -1) {
        switch (opt) {
            case 'h':
                afficher_aide(argv[0]);
                return 0;
            case 'V':
                afficher_version(argv[0]);
                return 0;
            case 'v':
                verbose = 1;
                break;
            case 'o':
                output_file = optarg;
                break;
            case '?':
                fprintf(stderr, "Essayez '%s --help' pour plus d'informations.\n",
                        argv[0]);
                return 1;
            default:
                return 1;
        }
    }

    // Vérifier qu'il y a au moins un fichier
    if (optind >= argc) {
        fprintf(stderr, "Erreur: aucun fichier spécifié\n");
        fprintf(stderr, "Essayez '%s --help' pour plus d'informations.\n", argv[0]);
        return 1;
    }

    // Traiter chaque fichier
    int erreurs = 0;
    for (int i = optind; i < argc; i++) {
        if (traiter_fichier(argv[i]) != 0) {
            erreurs++;
        }
    }

    // Retourner le code approprié
    return erreurs > 0 ? 1 : 0;
}
```

**Compilation et test :**
```bash
# Compiler
gcc -Wall -Wextra -o mon_outil mon_outil.c

# Tester l'aide
./mon_outil --help

# Tester la version
./mon_outil --version

# Utilisation normale
./mon_outil fichier.txt

# Avec options
./mon_outil -v -o resultat.txt fichier1.txt fichier2.txt
```

---

## Outils et bibliothèques utiles

### Bibliothèques de parsing d'arguments

| Bibliothèque | Complexité | Fonctionnalités |
|--------------|------------|-----------------|
| `getopt()` | Faible | Options courtes basiques |
| `getopt_long()` | Moyenne | Options courtes et longues |
| `argp` (GNU) | Élevée | Parsing avancé, documentation auto |
| `popt` | Moyenne | Alternative à getopt |

### Bibliothèques d'output

| Bibliothèque | Usage |
|--------------|-------|
| `ncurses` | Interfaces TUI (Text User Interface) |
| `readline` | Édition de ligne de commande |
| `json-c` | Génération/parsing JSON |
| `libyaml` | Génération/parsing YAML |

### Outils de développement

```bash
# Vérification statique
cppcheck mon_outil.c
clang-tidy mon_outil.c

# Détection de fuites mémoire
valgrind --leak-check=full ./mon_outil

# Sanitizers
gcc -fsanitize=address -g mon_outil.c -o mon_outil
```

---

## Intégration dans l'écosystème

### Installation système

```bash
# Installation locale
sudo cp mon_outil /usr/local/bin/
sudo chmod +x /usr/local/bin/mon_outil

# Vérification
which mon_outil
mon_outil --version
```

### Complétion bash

Créer un fichier de complétion pour bash :

```bash
# /etc/bash_completion.d/mon_outil
_mon_outil() {
    local cur prev opts
    cur="${COMP_WORDS[COMP_CWORD]}"
    prev="${COMP_WORDS[COMP_CWORD-1]}"
    opts="-h --help -v --verbose -o --output -V --version"

    case "${prev}" in
        -o|--output)
            COMPREPLY=( $(compgen -f -- ${cur}) )
            return 0
            ;;
        *)
            ;;
    esac

    COMPREPLY=( $(compgen -W "${opts}" -- ${cur}) )
    return 0
}

complete -F _mon_outil mon_outil
```

### Page de manuel

Structure d'une page man basique :

```
MON_OUTIL(1)              User Commands              MON_OUTIL(1)

NAME
       mon_outil - description courte

SYNOPSIS
       mon_outil [OPTIONS] FILE...

DESCRIPTION
       Description détaillée de l'outil.

OPTIONS
       -h, --help
              Afficher l'aide et quitter

       -v, --verbose
              Mode verbeux

EXAMPLES
       mon_outil fichier.txt
              Exemple d'utilisation basique

AUTHOR
       Votre Nom <email@example.com>

MON_OUTIL 1.0             January 2025               MON_OUTIL(1)
```

---

## Checklist de développement

Avant de considérer votre outil CLI comme "terminé", vérifiez :

### Fonctionnalités de base
- [ ] Parsing d'arguments robuste
- [ ] Option `--help` claire et complète
- [ ] Option `--version` fonctionnelle
- [ ] Gestion des erreurs avec messages explicites
- [ ] Codes de retour appropriés (0 = succès, ≠0 = erreur)

### Qualité du code
- [ ] Compilation sans warnings (`-Wall -Wextra`)
- [ ] Pas de fuites mémoire (Valgrind)
- [ ] Tests avec sanitizers (ASan, UBSan)
- [ ] Validation des entrées utilisateur
- [ ] Gestion propre des ressources (fichiers, mémoire)

### Expérience utilisateur
- [ ] Messages d'erreur clairs et utiles
- [ ] Output formaté et lisible
- [ ] Support stdin/stdout selon conventions UNIX
- [ ] Comportement prévisible
- [ ] Documentation (README, exemples)

### Intégration
- [ ] Respecte les conventions POSIX
- [ ] Compatible avec pipes et redirections
- [ ] Détecte si stdout est un terminal
- [ ] Peut être utilisé dans des scripts
- [ ] Installation facile

---

## Ressources et références

### Documentation essentielle

```bash
# Pages de manuel
man 3 getopt
man 3 printf
man 2 open
man 2 stat
man 7 signal

# Standards
man 7 posix
```

### Projets open-source à étudier

**Outils simples (bons pour débuter) :**
- `wc` : Compteur de mots/lignes
- `cat` : Concaténation de fichiers
- `head`/`tail` : Affichage de début/fin de fichier

**Outils avancés (pour inspiration) :**
- `grep` : Recherche de patterns
- `find` : Recherche de fichiers
- `htop` : Moniteur de processus

### Livres recommandés

- *The Linux Programming Interface* - Michael Kerrisk
- *Advanced Programming in the UNIX Environment* - W. Richard Stevens
- *The Art of UNIX Programming* - Eric S. Raymond

---

## Structure des sections suivantes

Maintenant que vous comprenez les fondamentaux, nous allons approfondir chaque aspect :

### 📖 Section 34.1.1 : Parsing d'arguments
- Utilisation de `argc` et `argv`
- `getopt()` pour options courtes
- `getopt_long()` pour options longues
- Validation et gestion d'erreurs
- Patterns courants

### 📖 Section 34.1.2 : Interaction avec le système
- Gestion des fichiers (open, read, write, stat)
- Manipulation de répertoires (opendir, readdir)
- Exécution de commandes (system, popen, fork+exec)
- Informations système (/proc, uname, environnement)
- Gestion des signaux

### 📖 Section 34.1.3 : Output formaté
- Formatage avec printf
- Couleurs et styles (codes ANSI)
- Tableaux et graphiques ASCII
- Barres de progression et spinners
- Formats structurés (JSON, CSV, YAML)

---

## Conclusion

La création d'outils CLI système en C est une compétence fondamentale pour tout développeur Linux. Ces outils sont :

- **Omniprésents** : Utilisés quotidiennement par millions d'utilisateurs
- **Puissants** : Accès direct au système d'exploitation
- **Efficaces** : Performance maximale, empreinte minimale
- **Composables** : S'intègrent dans des workflows complexes

En maîtrisant les trois aspects (parsing, interaction système, output), vous serez capable de créer des outils professionnels qui s'intègrent naturellement dans l'écosystème Linux et qui respectent la philosophie UNIX.

**Prêt à commencer ?** Passons au parsing d'arguments dans la section 34.1.1 !

⏭️ [Parsing d'arguments](/34-etudes-cas-devops/01.1-parsing-arguments.md)
