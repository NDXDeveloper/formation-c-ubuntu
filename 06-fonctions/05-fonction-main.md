🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 6.5 La fonction main et ses paramètres

## Introduction

La fonction `main` est **spéciale** en C : c'est le **point d'entrée** de votre programme. Lorsque vous exécutez un programme C, le système d'exploitation ne connaît qu'une seule fonction à appeler : `main`.

Contrairement aux autres fonctions que vous écrivez :
- Elle n'a **jamais de prototype** (le système l'appelle automatiquement)
- Elle doit **obligatoirement** s'appeler `main`
- Sa valeur de retour a une signification particulière pour le système d'exploitation
- Elle peut recevoir des **arguments de la ligne de commande**

Comprendre `main` est essentiel pour :
- ✅ Créer des programmes interactifs qui acceptent des paramètres
- ✅ Communiquer avec le système d'exploitation (codes de retour)
- ✅ Construire des outils en ligne de commande (CLI)
- ✅ Intégrer vos programmes dans des scripts shell

---

## Les deux formes de main

### Forme 1 : Sans paramètres

La forme la plus simple, que vous connaissez déjà :

```c
int main(void) {
    // Votre code ici
    return 0;
}
```

**Quand l'utiliser ?** : Quand votre programme n'a pas besoin de paramètres de ligne de commande.

---

### Forme 2 : Avec paramètres (argc, argv)

La forme complète qui accepte les arguments de ligne de commande :

```c
int main(int argc, char *argv[]) {
    // Votre code ici
    return 0;
}
```

Ou de manière équivalente (notation alternative) :

```c
int main(int argc, char **argv) {
    // Identique à la forme ci-dessus
    return 0;
}
```

**Quand l'utiliser ?** : Quand votre programme doit recevoir des paramètres depuis la ligne de commande.

---

## Les paramètres argc et argv

### argc : Argument Count

`argc` est un **entier** qui indique le **nombre d'arguments** passés au programme (incluant le nom du programme lui-même).

```c
int main(int argc, char *argv[]) {
    printf("Nombre d'arguments : %d\n", argc);
    return 0;
}
```

**Exemple d'exécution :**

```bash
$ ./programme
Nombre d'arguments : 1

$ ./programme arg1 arg2 arg3
Nombre d'arguments : 4
```

**Point important** : `argc` est **toujours ≥ 1** car le premier argument est le nom du programme.

---

### argv : Argument Vector

`argv` est un **tableau de chaînes de caractères** (pointeurs) contenant les arguments.

```c
char *argv[]  // Tableau de pointeurs vers char
```

**Structure de argv :**

```
argv[0]  →  Nom du programme (ou chemin)  
argv[1]  →  Premier argument  
argv[2]  →  Deuxième argument  
...
argv[argc-1]  →  Dernier argument  
argv[argc]    →  NULL (sentinel)  
```

**Exemple :**

```c
#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("Nom du programme : %s\n", argv[0]);

    for (int i = 1; i < argc; i++) {
        printf("Argument %d : %s\n", i, argv[i]);
    }

    return 0;
}
```

**Exécution :**

```bash
$ ./programme bonjour le monde
Nom du programme : ./programme  
Argument 1 : bonjour  
Argument 2 : le  
Argument 3 : monde  
```

---

## Visualisation de la mémoire

Quand vous exécutez :

```bash
$ ./programme hello world
```

En mémoire, `argc` et `argv` ressemblent à ceci :

```
argc = 3

argv:
    ┌─────────┐
    │ argv[0] │ ──→ "./programme\0"
    ├─────────┤
    │ argv[1] │ ──→ "hello\0"
    ├─────────┤
    │ argv[2] │ ──→ "world\0"
    ├─────────┤
    │ argv[3] │ ──→ NULL
    └─────────┘
```

**Points clés** :
- Chaque `argv[i]` est un **pointeur** vers une chaîne de caractères
- Les chaînes se terminent par `\0` (null terminator)
- `argv[argc]` est toujours `NULL` (utile pour itérer sans connaître argc)

---

## Exemples pratiques

### Exemple 1 : Afficher tous les arguments

```c
#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("=== Liste des arguments ===\n");

    for (int i = 0; i < argc; i++) {
        printf("argv[%d] = %s\n", i, argv[i]);
    }

    return 0;
}
```

**Exécution :**

```bash
$ ./programme arg1 arg2 "argument avec espaces"
=== Liste des arguments ===
argv[0] = ./programme  
argv[1] = arg1  
argv[2] = arg2  
argv[3] = argument avec espaces  
```

**Note** : Les guillemets permettent de passer un argument contenant des espaces.

---

### Exemple 2 : Programme de salutation

```c
#include <stdio.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage : %s <prénom>\n", argv[0]);
        return 1;  // Code d'erreur
    }

    printf("Bonjour, %s !\n", argv[1]);
    return 0;
}
```

**Exécution :**

```bash
$ ./salutation
Usage : ./salutation <prénom>

$ ./salutation Alice
Bonjour, Alice !
```

---

### Exemple 3 : Calculatrice simple

```c
#include <stdio.h>
#include <stdlib.h>  // Pour atoi()
#include <string.h>  // Pour strcmp()

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage : %s <nombre1> <opération> <nombre2>\n", argv[0]);
        printf("Opérations : +, -, *, /\n");
        return 1;
    }

    int a = atoi(argv[1]);  // Convertit chaîne → entier
    char *op = argv[2];
    int b = atoi(argv[3]);

    int resultat;

    if (strcmp(op, "+") == 0) {
        resultat = a + b;
    } else if (strcmp(op, "-") == 0) {
        resultat = a - b;
    } else if (strcmp(op, "*") == 0) {
        resultat = a * b;
    } else if (strcmp(op, "/") == 0) {
        if (b == 0) {
            printf("Erreur : division par zéro\n");
            return 1;
        }
        resultat = a / b;
    } else {
        printf("Erreur : opération inconnue '%s'\n", op);
        return 1;
    }

    printf("%d %s %d = %d\n", a, op, b, resultat);
    return 0;
}
```

**Exécution :**

```bash
$ ./calculatrice 10 + 5
10 + 5 = 15

$ ./calculatrice 20 / 4
20 / 4 = 5

$ ./calculatrice 7 "*" 6
7 * 6 = 42

$ ./calculatrice 10 / 0
Erreur : division par zéro
```

**Note** : Pour `*`, il faut utiliser des guillemets ou échapper avec `\*` car `*` est un caractère spécial du shell.

---

## La valeur de retour de main

### Signification

La valeur retournée par `main` est transmise au **système d'exploitation** :

- **0** : Succès, tout s'est bien passé
- **Non-zéro** : Échec, une erreur s'est produite

```c
int main(void) {
    // ... code ...

    if (/* erreur */) {
        return 1;  // Ou toute valeur non-nulle
    }

    return 0;  // Succès
}
```

---

### Constantes standard

Le fichier `<stdlib.h>` définit des constantes pour la lisibilité :

```c
#include <stdlib.h>

int main(void) {
    // ... code ...

    if (/* erreur */) {
        return EXIT_FAILURE;  // Généralement 1
    }

    return EXIT_SUCCESS;  // Toujours 0
}
```

**Avantage** : Plus lisible et portable que les valeurs magiques.

---

### Utilisation dans les scripts shell

Le code de retour est accessible dans les scripts shell via la variable `$?` :

```bash
#!/bin/bash

./mon_programme arg1 arg2

if [ $? -eq 0 ]; then
    echo "Programme terminé avec succès"
else
    echo "Le programme a échoué"
fi
```

**Ou de manière plus concise :**

```bash
./mon_programme arg1 arg2 && echo "Succès" || echo "Échec"
```

---

## Conversion des arguments

Les arguments sont **toujours des chaînes de caractères**. Pour les utiliser comme nombres, vous devez les convertir :

### Conversion en entier : atoi()

```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage : %s <nombre>\n", argv[0]);
        return 1;
    }

    int nombre = atoi(argv[1]);  // String to Integer
    printf("Le double de %d est %d\n", nombre, nombre * 2);

    return 0;
}
```

**Limitation de atoi** : Pas de gestion d'erreur (retourne 0 si la conversion échoue).

---

### Conversion sécurisée : strtol()

```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage : %s <nombre>\n", argv[0]);
        return 1;
    }

    char *endptr;
    errno = 0;
    long nombre = strtol(argv[1], &endptr, 10);  // Base 10

    // Vérification d'erreurs
    if (errno != 0) {
        perror("strtol");
        return 1;
    }

    if (endptr == argv[1]) {
        printf("Erreur : aucun chiffre trouvé dans '%s'\n", argv[1]);
        return 1;
    }

    if (*endptr != '\0') {
        printf("Attention : caractères non convertis : '%s'\n", endptr);
    }

    printf("Nombre converti : %ld\n", nombre);
    return 0;
}
```

**Exécution :**

```bash
$ ./programme 123
Nombre converti : 123

$ ./programme 456abc
Attention : caractères non convertis : 'abc'  
Nombre converti : 456  

$ ./programme abc
Erreur : aucun chiffre trouvé dans 'abc'
```

---

### Conversion en flottant : strtod()

```c
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage : %s <nombre_flottant>\n", argv[0]);
        return 1;
    }

    char *endptr;
    double nombre = strtod(argv[1], &endptr);

    if (endptr == argv[1]) {
        printf("Erreur : conversion impossible\n");
        return 1;
    }

    printf("Nombre : %.2f\n", nombre);
    printf("Carré : %.2f\n", nombre * nombre);

    return 0;
}
```

**Exécution :**

```bash
$ ./programme 3.14
Nombre : 3.14  
Carré : 9.86  
```

---

## Parsing d'options : Techniques de base

### Options simples (flags)

```c
#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    int verbose = 0;
    int debug = 0;

    // Parser les options
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = 1;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            debug = 1;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            printf("Usage : %s [-v] [-d] [-h]\n", argv[0]);
            printf("  -v, --verbose  Mode verbeux\n");
            printf("  -d, --debug    Mode debug\n");
            printf("  -h, --help     Afficher cette aide\n");
            return 0;
        } else {
            printf("Option inconnue : %s\n", argv[i]);
            return 1;
        }
    }

    printf("Mode verbose : %s\n", verbose ? "activé" : "désactivé");
    printf("Mode debug : %s\n", debug ? "activé" : "désactivé");

    if (verbose) {
        printf("[VERBOSE] Démarrage du programme...\n");
    }

    if (debug) {
        printf("[DEBUG] argc=%d\n", argc);
    }

    return 0;
}
```

**Exécution :**

```bash
$ ./programme -v -d
Mode verbose : activé  
Mode debug : activé  
[VERBOSE] Démarrage du programme...
[DEBUG] argc=3
```

---

### Options avec valeurs

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
    char *output_file = NULL;
    int count = 10;  // Valeur par défaut

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0 || strcmp(argv[i], "--output") == 0) {
            if (i + 1 < argc) {
                output_file = argv[++i];  // Argument suivant
            } else {
                printf("Erreur : -o requiert un argument\n");
                return 1;
            }
        } else if (strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--count") == 0) {
            if (i + 1 < argc) {
                count = atoi(argv[++i]);
            } else {
                printf("Erreur : -n requiert un argument\n");
                return 1;
            }
        } else {
            printf("Option inconnue : %s\n", argv[i]);
            return 1;
        }
    }

    printf("Fichier de sortie : %s\n", output_file ? output_file : "(non spécifié)");
    printf("Nombre d'itérations : %d\n", count);

    return 0;
}
```

**Exécution :**

```bash
$ ./programme -o resultat.txt -n 5
Fichier de sortie : resultat.txt  
Nombre d'itérations : 5  

$ ./programme -n 20
Fichier de sortie : (non spécifié)  
Nombre d'itérations : 20  
```

---

## Fonction getopt() (POSIX)

Pour les programmes plus complexes, la bibliothèque POSIX fournit `getopt()` :

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // Pour getopt()

int main(int argc, char *argv[]) {
    int verbose = 0;
    char *output_file = NULL;
    int count = 10;
    int opt;

    // "o:" signifie que -o attend un argument
    // "n:" signifie que -n attend un argument
    // "v" est un flag sans argument
    while ((opt = getopt(argc, argv, "vo:n:h")) != -1) {
        switch (opt) {
            case 'v':
                verbose = 1;
                break;
            case 'o':
                output_file = optarg;  // optarg contient l'argument de -o
                break;
            case 'n':
                count = atoi(optarg);
                break;
            case 'h':
                printf("Usage : %s [-v] [-o fichier] [-n nombre] [-h]\n", argv[0]);
                return 0;
            case '?':  // Option inconnue
                return 1;
        }
    }

    printf("Verbose : %s\n", verbose ? "oui" : "non");
    printf("Fichier : %s\n", output_file ? output_file : "(aucun)");
    printf("Count : %d\n", count);

    // Arguments non-options restants
    printf("Arguments restants : ");
    for (int i = optind; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    printf("\n");

    return 0;
}
```

**Exécution :**

```bash
$ ./programme -v -o output.txt -n 15 fichier1 fichier2
Verbose : oui  
Fichier : output.txt  
Count : 15  
Arguments restants : fichier1 fichier2  
```

---

## Cas particuliers et subtilités

### argv[0] : Nom du programme

`argv[0]` contient **le nom utilisé pour lancer le programme**, pas nécessairement son nom réel :

```bash
$ ./programme
# argv[0] = "./programme"

$ /chemin/complet/vers/programme
# argv[0] = "/chemin/complet/vers/programme"

$ ln -s programme mon_lien
$ ./mon_lien
# argv[0] = "./mon_lien"
```

**Utilisation courante** : Afficher le nom du programme dans les messages d'erreur.

```c
printf("Usage : %s <arguments>\n", argv[0]);
```

---

### Arguments vides et espaces

Les arguments peuvent contenir des espaces ou être vides :

```bash
$ ./programme "argument avec espaces" "" troisieme
```

```
argc = 4  
argv[0] = "./programme"  
argv[1] = "argument avec espaces"  
argv[2] = ""  
argv[3] = "troisieme"  
```

---

### Arguments avec caractères spéciaux

Le shell interprète certains caractères (*, ?, $, etc.). Pour les passer littéralement :

```bash
$ ./programme '*'          # Guillemets simples
$ ./programme "*"          # Guillemets doubles
$ ./programme \*           # Échappement
$ ./programme '$HOME'      # Bloque l'expansion
$ ./programme "$HOME"      # Permet l'expansion : /home/user
```

---

## Exemple complet : Outil de traitement de fichiers

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void afficher_aide(const char *prog_name) {
    printf("Usage : %s [OPTIONS] <fichier>\n", prog_name);
    printf("Options :\n");
    printf("  -c        Compter les lignes\n");
    printf("  -u        Convertir en majuscules\n");
    printf("  -o FILE   Fichier de sortie (défaut : stdout)\n");
    printf("  -h        Afficher cette aide\n");
}

int main(int argc, char *argv[]) {
    int compter = 0;
    int majuscules = 0;
    char *output_file = NULL;
    int opt;

    // Parser les options
    while ((opt = getopt(argc, argv, "cuo:h")) != -1) {
        switch (opt) {
            case 'c':
                compter = 1;
                break;
            case 'u':
                majuscules = 1;
                break;
            case 'o':
                output_file = optarg;
                break;
            case 'h':
                afficher_aide(argv[0]);
                return EXIT_SUCCESS;
            default:
                afficher_aide(argv[0]);
                return EXIT_FAILURE;
        }
    }

    // Vérifier qu'il reste un argument (le fichier d'entrée)
    if (optind >= argc) {
        printf("Erreur : fichier d'entrée manquant\n");
        afficher_aide(argv[0]);
        return EXIT_FAILURE;
    }

    const char *input_file = argv[optind];

    // Ouvrir le fichier d'entrée
    FILE *input = fopen(input_file, "r");
    if (input == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    // Ouvrir le fichier de sortie (ou utiliser stdout)
    FILE *output = stdout;
    if (output_file != NULL) {
        output = fopen(output_file, "w");
        if (output == NULL) {
            perror("fopen");
            fclose(input);
            return EXIT_FAILURE;
        }
    }

    // Traitement
    char ligne[1024];
    int nb_lignes = 0;

    while (fgets(ligne, sizeof(ligne), input) != NULL) {
        nb_lignes++;

        if (majuscules) {
            for (int i = 0; ligne[i]; i++) {
                if (ligne[i] >= 'a' && ligne[i] <= 'z') {
                    ligne[i] = ligne[i] - 'a' + 'A';
                }
            }
        }

        if (!compter) {
            fprintf(output, "%s", ligne);
        }
    }

    if (compter) {
        fprintf(output, "Nombre de lignes : %d\n", nb_lignes);
    }

    // Nettoyage
    fclose(input);
    if (output != stdout) {
        fclose(output);
    }

    return EXIT_SUCCESS;
}
```

**Utilisation :**

```bash
$ ./outil -c fichier.txt
Nombre de lignes : 42

$ ./outil -u fichier.txt -o resultat.txt
# Convertit en majuscules et sauvegarde dans resultat.txt

$ ./outil -c -u fichier.txt
# Impossible : -c et -u sont incompatibles (à améliorer)
```

---

## Bonnes pratiques

### 1. Toujours vérifier argc

```c
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage : %s <argument>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Utiliser argv[1] en toute sécurité
}
```

---

### 2. Fournir une aide (--help)

```c
if (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
    printf("Usage : %s [OPTIONS]\n", argv[0]);
    printf("Description du programme...\n");
    return EXIT_SUCCESS;
}
```

---

### 3. Valider les conversions

```c
// ❌ Mauvais
int nombre = atoi(argv[1]);  // Pas de vérification d'erreur

// ✅ Bon
char *endptr;  
long nombre = strtol(argv[1], &endptr, 10);  
if (*endptr != '\0') {  
    printf("Erreur : '%s' n'est pas un nombre valide\n", argv[1]);
    return EXIT_FAILURE;
}
```

---

### 4. Utiliser des constantes pour les codes de retour

```c
return EXIT_SUCCESS;  // Plutôt que : return 0;  
return EXIT_FAILURE;  // Plutôt que : return 1;  
```

---

### 5. Messages d'erreur informatifs

```c
// ❌ Mauvais
printf("Erreur\n");

// ✅ Bon
printf("Erreur : nombre d'arguments invalide\n");  
printf("Usage : %s <fichier>\n", argv[0]);  
```

---

## Erreurs courantes à éviter

### Erreur 1 : Accès hors limites

```c
// ❌ Dangereux : pas de vérification de argc
int main(int argc, char *argv[]) {
    printf("Premier argument : %s\n", argv[1]);  // Crash si argc < 2
    return 0;
}

// ✅ Correct
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Argument manquant\n");
        return 1;
    }
    printf("Premier argument : %s\n", argv[1]);
    return 0;
}
```

---

### Erreur 2 : Modifier argv

```c
// ⚠️ À éviter : modifier les chaînes pointées par argv
int main(int argc, char *argv[]) {
    argv[1][0] = 'X';  // Techniquement autorisé, mais déconseillé
    return 0;
}
```

La norme C garantit que les chaînes de `argv` sont **modifiables**, mais les modifier directement est une mauvaise pratique (cela peut perturber l'affichage du processus dans `ps`, `/proc`, etc.). Copiez-les si vous devez les modifier :

```c
char buffer[256];  
strncpy(buffer, argv[1], sizeof(buffer) - 1);  
buffer[sizeof(buffer) - 1] = '\0';  
// Maintenant, modifier buffer est sûr
```

---

### Erreur 3 : Oublier le nom du programme

```c
// ❌ Oublie que argv[0] est le nom du programme
for (int i = 0; i < argc; i++) {
    process_file(argv[i]);  // Essaie de traiter "./programme" !
}

// ✅ Commence à argv[1]
for (int i = 1; i < argc; i++) {
    process_file(argv[i]);
}
```

---

## Récapitulatif

### Tableau de référence

| Élément | Description | Exemple |
|---------|-------------|---------|
| `int main(void)` | Forme sans paramètres | Programme sans arguments CLI |
| `int main(int argc, char *argv[])` | Forme avec paramètres | Programme avec arguments CLI |
| `argc` | Nombre d'arguments (≥ 1) | `argc = 3` pour `./prog a b` |
| `argv[0]` | Nom du programme | `"./programme"` |
| `argv[1]` à `argv[argc-1]` | Arguments utilisateur | Les vrais paramètres |
| `argv[argc]` | Toujours `NULL` | Sentinel pour itération |
| `return 0` ou `EXIT_SUCCESS` | Succès | Code retour au système |
| `return 1` ou `EXIT_FAILURE` | Échec | Code d'erreur au système |

---

### Points clés à retenir

1. ✅ `main` est le **point d'entrée** du programme (appelée par le système)
2. ✅ `argc` compte le **nombre total d'arguments** (incluant le nom du programme)
3. ✅ `argv[0]` est le **nom du programme**, `argv[1]` est le premier vrai argument
4. ✅ Les arguments sont **toujours des chaînes** (conversion nécessaire pour nombres)
5. ✅ **Toujours vérifier argc** avant d'accéder à `argv[i]`
6. ✅ La valeur de retour de `main` signale **succès (0) ou échec (non-zéro)** au système
7. ✅ `argv[argc]` est toujours `NULL` (utile pour itération)
8. ✅ Utilisez `getopt()` pour parser des options complexes

---

## Pour aller plus loin

Vous maîtrisez maintenant les fonctions en C ! Les concepts abordés dans le chapitre 6 sont fondamentaux :

- **6.1** : Prototypes et déclarations → Organisation du code
- **6.2** : Stack frames → Compréhension de la mémoire
- **6.3** : Passage par valeur → Sémantique des paramètres
- **6.4** : Codes d'erreur → Robustesse
- **6.5** : main et argv → Interaction avec l'environnement

Dans les prochains modules, vous explorerez :
- **Module 3** : La gestion avancée de la mémoire (pointeurs, heap)
- **Module 4** : Les structures de données complexes
- **Module 5** : L'outillage DevOps (build systems, débogage)

Vous êtes prêt à créer des programmes C robustes et professionnels ! 🚀

⏭️ [Les Pointeurs : Démystification](/07-pointeurs/README.md)
