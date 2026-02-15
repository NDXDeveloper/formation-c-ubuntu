🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 17.4 Variables d'environnement

## Introduction

Les **variables d'environnement** sont des paires clé-valeur qui stockent des informations de configuration utilisées par le système d'exploitation et les programmes. Elles constituent un mécanisme essentiel pour :

- Configurer le comportement des programmes
- Partager des informations entre processus
- Personnaliser l'environnement utilisateur
- Stocker des chemins système importants

Sous Linux, chaque processus possède son propre **environnement**, hérité de son processus parent lors de sa création.

## Qu'est-ce qu'une variable d'environnement ?

### Structure

Une variable d'environnement est une chaîne de caractères au format :

```
NOM=VALEUR
```

**Exemples :**
```bash
PATH=/usr/bin:/bin:/usr/local/bin  
HOME=/home/utilisateur  
USER=utilisateur  
LANG=fr_FR.UTF-8  
```

### Conventions de nommage

Par convention :
- Les noms sont en **MAJUSCULES**
- Les mots sont séparés par des **underscores** (`_`)
- Exemples : `PATH`, `HOME`, `LD_LIBRARY_PATH`

**Note :** Ce ne sont que des conventions. Techniquement, les noms peuvent être en minuscules, mais c'est rare.

## Visualiser les variables d'environnement

### Depuis le shell

```bash
# Afficher toutes les variables d'environnement
env

# Ou
printenv

# Afficher une variable spécifique
echo $PATH  
echo $HOME  

# Afficher avec printenv
printenv PATH  
printenv HOME  
```

### Variables courantes

| Variable | Description | Exemple |
|----------|-------------|---------|
| `PATH` | Chemins de recherche des exécutables | `/usr/bin:/bin:/usr/local/bin` |
| `HOME` | Répertoire personnel de l'utilisateur | `/home/utilisateur` |
| `USER` | Nom de l'utilisateur | `utilisateur` |
| `SHELL` | Shell par défaut | `/bin/bash` |
| `PWD` | Répertoire de travail courant | `/home/utilisateur/projets` |
| `LANG` | Langue et encodage | `fr_FR.UTF-8` |
| `EDITOR` | Éditeur de texte par défaut | `vim` ou `nano` |
| `TERM` | Type de terminal | `xterm-256color` |
| `LD_LIBRARY_PATH` | Chemins des bibliothèques partagées | `/usr/local/lib` |

## Accéder aux variables d'environnement en C

### La fonction `getenv()`

La fonction la plus simple pour lire une variable d'environnement :

```c
#include <stdlib.h>

char *getenv(const char *name);
```

**Retour :**
- Pointeur vers la valeur de la variable (si elle existe)
- `NULL` si la variable n'existe pas

**Important :** Le pointeur retourné pointe vers une zone mémoire interne. Ne le modifiez pas et ne le libérez pas avec `free()`.

### Exemple basique

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    char *path = getenv("PATH");

    if (path != NULL) {
        printf("PATH = %s\n", path);
    } else {
        printf("PATH n'est pas défini\n");
    }

    return 0;
}
```

**Sortie typique :**
```
PATH = /usr/local/bin:/usr/bin:/bin:/usr/local/games:/usr/games
```

### Exemple avec plusieurs variables

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    const char *variables[] = {
        "HOME", "USER", "SHELL", "PWD", "LANG", NULL
    };

    printf("Variables d'environnement:\n");
    printf("===========================\n\n");

    for (int i = 0; variables[i] != NULL; i++) {
        char *value = getenv(variables[i]);

        if (value != NULL) {
            printf("%-10s = %s\n", variables[i], value);
        } else {
            printf("%-10s = (non définie)\n", variables[i]);
        }
    }

    return 0;
}
```

**Sortie exemple :**
```
Variables d'environnement:
===========================

HOME       = /home/pierre  
USER       = pierre  
SHELL      = /bin/bash  
PWD        = /home/pierre/projets  
LANG       = fr_FR.UTF-8  
```

### Vérification de l'existence

Toujours vérifier que `getenv()` ne retourne pas `NULL` :

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    char *editor = getenv("EDITOR");

    if (editor == NULL || strlen(editor) == 0) {
        printf("EDITOR non défini, utilisation de 'vi' par défaut\n");
        editor = "vi";
    }

    printf("Éditeur configuré: %s\n", editor);

    return 0;
}
```

## Le tableau `environ`

### Accès direct à l'environnement complet

Le tableau global `environ` contient **toutes** les variables d'environnement sous forme de chaînes `"NOM=VALEUR"`.

```c
#include <stdio.h>

// Déclaration externe du tableau environ
extern char **environ;

int main(void) {
    printf("Toutes les variables d'environnement:\n");
    printf("=====================================\n\n");

    for (int i = 0; environ[i] != NULL; i++) {
        printf("%s\n", environ[i]);
    }

    return 0;
}
```

**Sortie partielle :**
```
Toutes les variables d'environnement:
=====================================

HOME=/home/pierre  
USER=pierre  
PATH=/usr/bin:/bin:/usr/local/bin  
SHELL=/bin/bash  
LANG=fr_FR.UTF-8  
PWD=/home/pierre  
...
```

### Accès via `main()`

Une alternative à `environ` : passer l'environnement comme troisième paramètre de `main()` :

```c
#include <stdio.h>

int main(int argc, char *argv[], char *envp[]) {
    (void)argc;
    (void)argv;
    printf("Variables d'environnement (via envp):\n\n");

    for (int i = 0; envp[i] != NULL; i++) {
        printf("%s\n", envp[i]);
    }

    return 0;
}
```

**Note :** `envp` et `environ` pointent vers les mêmes données, mais `environ` est plus portable et recommandé.

### Parser une variable d'environnement

```c
#include <stdio.h>
#include <string.h>

extern char **environ;

void print_variable_details(const char *env_string) {
    // Copier la chaîne pour ne pas modifier l'original
    char buffer[256];
    strncpy(buffer, env_string, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';

    // Trouver le '='
    char *equals = strchr(buffer, '=');

    if (equals != NULL) {
        *equals = '\0';  // Séparer nom et valeur
        char *name = buffer;
        char *value = equals + 1;

        printf("Nom: %-15s Valeur: %s\n", name, value);
    }
}

int main(void) {
    printf("Analyse des variables d'environnement:\n\n");

    for (int i = 0; environ[i] != NULL && i < 10; i++) {
        print_variable_details(environ[i]);
    }

    return 0;
}
```

## Modifier les variables d'environnement

### La fonction `setenv()`

```c
#include <stdlib.h>

int setenv(const char *name, const char *value, int overwrite);
```

**Paramètres :**
- `name` : Nom de la variable
- `value` : Nouvelle valeur
- `overwrite` :
  - `0` : Ne pas modifier si la variable existe déjà
  - `1` : Écraser la valeur existante

**Retour :**
- `0` en cas de succès
- `-1` en cas d'erreur

### Exemple d'utilisation

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // Afficher la valeur initiale
    char *editor = getenv("EDITOR");
    printf("EDITOR initial: %s\n", editor ? editor : "(non défini)");

    // Définir une nouvelle valeur
    if (setenv("EDITOR", "vim", 1) == 0) {
        printf("EDITOR modifié avec succès\n");
    } else {
        perror("setenv");
        return 1;
    }

    // Vérifier la modification
    editor = getenv("EDITOR");
    printf("EDITOR après modification: %s\n", editor);

    return 0;
}
```

**Sortie :**
```
EDITOR initial: nano  
EDITOR modifié avec succès  
EDITOR après modification: vim  
```

### Créer une nouvelle variable

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // Créer une variable personnalisée
    if (setenv("MA_VARIABLE", "ma_valeur", 1) == 0) {
        printf("Variable créée\n");
    }

    // La lire
    char *value = getenv("MA_VARIABLE");
    printf("MA_VARIABLE = %s\n", value);

    return 0;
}
```

### Le paramètre `overwrite`

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // Définir une variable
    setenv("TEST", "valeur1", 1);
    printf("TEST = %s\n", getenv("TEST"));

    // Tenter de la modifier avec overwrite = 0
    setenv("TEST", "valeur2", 0);  // N'écrase PAS
    printf("TEST après setenv(..., 0) = %s\n", getenv("TEST"));

    // Modifier avec overwrite = 1
    setenv("TEST", "valeur3", 1);  // Écrase
    printf("TEST après setenv(..., 1) = %s\n", getenv("TEST"));

    return 0;
}
```

**Sortie :**
```
TEST = valeur1  
TEST après setenv(..., 0) = valeur1  
TEST après setenv(..., 1) = valeur3  
```

### La fonction `unsetenv()`

Pour supprimer une variable d'environnement :

```c
#include <stdlib.h>

int unsetenv(const char *name);
```

**Exemple :**

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // Créer une variable
    setenv("MA_VARIABLE", "test", 1);
    printf("Avant unsetenv: %s\n", getenv("MA_VARIABLE"));

    // La supprimer
    unsetenv("MA_VARIABLE");

    char *value = getenv("MA_VARIABLE");
    if (value == NULL) {
        printf("Après unsetenv: variable supprimée\n");
    }

    return 0;
}
```

### La fonction `putenv()` (moins recommandée)

```c
#include <stdlib.h>

int putenv(char *string);
```

**Attention :** `putenv()` prend une chaîne `"NOM=VALEUR"` et **ne copie pas** la chaîne. Le pointeur devient part de l'environnement. Évitez d'utiliser des chaînes automatiques (sur la pile).

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    // ❌ DANGEREUX : chaîne sur la pile
    // char buffer[64];
    // sprintf(buffer, "MA_VAR=test");
    // putenv(buffer);  // ERREUR: buffer disparaît!

    // ✅ BON : allocation dynamique ou chaîne statique
    char *env_string = malloc(64);
    sprintf(env_string, "MA_VAR=test");
    putenv(env_string);

    printf("MA_VAR = %s\n", getenv("MA_VAR"));

    // Note: ne pas free(env_string) car putenv() l'utilise!

    return 0;
}
```

**Recommandation :** Utilisez `setenv()` plutôt que `putenv()` pour éviter ces problèmes.

## Héritage des variables d'environnement

### Lors d'un `fork()`

Quand un processus crée un enfant avec `fork()`, l'enfant **hérite** de toutes les variables d'environnement du parent :

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(void) {
    // Parent définit une variable
    setenv("MA_VARIABLE", "valeur_parent", 1);

    printf("[Parent] MA_VARIABLE = %s\n", getenv("MA_VARIABLE"));

    pid_t pid = fork();

    if (pid == 0) {
        // ENFANT
        printf("[Enfant] MA_VARIABLE héritée = %s\n",
               getenv("MA_VARIABLE"));

        // Enfant modifie la variable
        setenv("MA_VARIABLE", "valeur_enfant", 1);
        printf("[Enfant] MA_VARIABLE modifiée = %s\n",
               getenv("MA_VARIABLE"));

        return 0;
    }
    else {
        // PARENT
        wait(NULL);

        // La modification de l'enfant n'affecte PAS le parent
        printf("[Parent] MA_VARIABLE après fork = %s\n",
               getenv("MA_VARIABLE"));
    }

    return 0;
}
```

**Sortie :**
```
[Parent] MA_VARIABLE = valeur_parent
[Enfant] MA_VARIABLE héritée = valeur_parent
[Enfant] MA_VARIABLE modifiée = valeur_enfant
[Parent] MA_VARIABLE après fork = valeur_parent
```

### Observation importante

Les modifications de l'environnement dans un processus **n'affectent pas** les autres processus, même pas le parent ou les frères :

- ✅ Héritage lors de `fork()` : L'enfant reçoit une **copie**
- ❌ Pas de synchronisation : Les modifications sont locales

### Lors d'un `exec()`

Quand un processus fait un `exec()`, il peut :
1. **Hériter** de l'environnement actuel
2. **Spécifier** un nouvel environnement

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    setenv("MA_VARIABLE", "test", 1);

    printf("Avant exec, MA_VARIABLE = %s\n", getenv("MA_VARIABLE"));

    // exec() avec héritage de l'environnement
    execl("/usr/bin/printenv", "printenv", "MA_VARIABLE", NULL);

    // Si exec réussit, on n'arrive jamais ici
    perror("execl");
    return 1;
}
```

## Variables d'environnement personnalisées

### Passer des informations à un programme

Les variables d'environnement sont couramment utilisées pour configurer les programmes :

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(void) {
    // Lire le niveau de verbosité depuis une variable
    char *verbosity = getenv("APP_VERBOSITY");
    int level = 1;  // Valeur par défaut

    if (verbosity != NULL) {
        level = atoi(verbosity);
    }

    printf("Niveau de verbosité: %d\n", level);

    if (level >= 2) {
        printf("[DEBUG] Mode verbeux activé\n");
    }

    // Lire un fichier de configuration
    char *config_file = getenv("APP_CONFIG");

    if (config_file != NULL) {
        printf("Utilisation du fichier de config: %s\n", config_file);
    } else {
        printf("Utilisation du fichier par défaut: /etc/app.conf\n");
    }

    return 0;
}
```

**Utilisation :**
```bash
# Mode par défaut
./mon_programme

# Mode verbeux
APP_VERBOSITY=2 ./mon_programme

# Avec config personnalisée
APP_CONFIG=/home/user/my_config.conf ./mon_programme
```

### Convention de nommage pour applications

Il est courant de préfixer les variables avec le nom de l'application :

```
MYAPP_DEBUG=1  
MYAPP_PORT=8080  
MYAPP_DATABASE_URL=postgresql://localhost/mydb  
MYAPP_LOG_LEVEL=info  
```

## Modifier l'environnement avant un `exec()`

### Avec `execle()` ou `execve()`

Ces variantes d'`exec()` permettent de spécifier explicitement l'environnement :

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    // Créer un nouvel environnement
    char *newenv[] = {
        "PATH=/usr/bin:/bin",
        "HOME=/tmp",
        "USER=testuser",
        "MA_VARIABLE=ma_valeur",
        NULL  // Terminateur
    };

    printf("Exécution de /usr/bin/env avec environnement personnalisé:\n\n");

    // Remplacer le processus avec un nouvel environnement
    execve("/usr/bin/env", (char *[]){"/usr/bin/env", NULL}, newenv);

    // Si on arrive ici, exec a échoué
    perror("execve");
    return 1;
}
```

### Construire un environnement dynamiquement

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

extern char **environ;

int main(void) {
    // Compter les variables actuelles
    int count = 0;
    while (environ[count] != NULL) {
        count++;
    }

    // Allouer un nouveau tableau (existant + 2 nouvelles + NULL)
    char **newenv = malloc((count + 3) * sizeof(char*));

    // Copier l'environnement existant
    for (int i = 0; i < count; i++) {
        newenv[i] = environ[i];
    }

    // Ajouter de nouvelles variables
    newenv[count] = "NOUVELLE_VAR1=valeur1";
    newenv[count + 1] = "NOUVELLE_VAR2=valeur2";
    newenv[count + 2] = NULL;

    printf("Exécution avec environnement étendu:\n\n");

    execve("/usr/bin/printenv",
           (char *[]){"/usr/bin/printenv", "NOUVELLE_VAR1", NULL},
           newenv);

    perror("execve");
    free(newenv);
    return 1;
}
```

## Cas d'usage pratiques

### 1. Configuration d'application

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *host;
    int port;
    int debug;
} Config;

Config load_config_from_env(void) {
    Config config;

    // Hôte
    char *host = getenv("SERVER_HOST");
    config.host = (host != NULL) ? host : "localhost";

    // Port
    char *port_str = getenv("SERVER_PORT");
    config.port = (port_str != NULL) ? atoi(port_str) : 8080;

    // Debug
    char *debug = getenv("DEBUG");
    config.debug = (debug != NULL && strcmp(debug, "1") == 0);

    return config;
}

int main(void) {
    Config config = load_config_from_env();

    printf("Configuration de l'application:\n");
    printf("  Hôte:  %s\n", config.host);
    printf("  Port:  %d\n", config.port);
    printf("  Debug: %s\n", config.debug ? "activé" : "désactivé");

    return 0;
}
```

**Utilisation :**
```bash
SERVER_HOST=0.0.0.0 SERVER_PORT=9000 DEBUG=1 ./mon_serveur
```

### 2. Recherche dans PATH

Trouver un exécutable dans les répertoires du `PATH` :

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char* find_in_path(const char *program) {
    char *path = getenv("PATH");
    if (path == NULL) {
        return NULL;
    }

    // Copier PATH pour ne pas le modifier
    char *path_copy = strdup(path);
    char *dir = strtok(path_copy, ":");
    static char full_path[1024];

    while (dir != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, program);

        // Vérifier si le fichier existe et est exécutable
        if (access(full_path, X_OK) == 0) {
            free(path_copy);
            return full_path;
        }

        dir = strtok(NULL, ":");
    }

    free(path_copy);
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <programme>\n", argv[0]);
        return 1;
    }

    char *path = find_in_path(argv[1]);

    if (path != NULL) {
        printf("'%s' trouvé: %s\n", argv[1], path);
    } else {
        printf("'%s' non trouvé dans PATH\n", argv[1]);
    }

    return 0;
}
```

**Utilisation :**
```bash
./find_in_path ls
# Sortie: 'ls' trouvé: /usr/bin/ls
```

### 3. Répertoire temporaire

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char* get_temp_dir(void) {
    char *tmpdir = getenv("TMPDIR");
    if (tmpdir == NULL || strlen(tmpdir) == 0) {
        tmpdir = getenv("TEMP");
    }
    if (tmpdir == NULL || strlen(tmpdir) == 0) {
        tmpdir = getenv("TMP");
    }
    if (tmpdir == NULL || strlen(tmpdir) == 0) {
        tmpdir = "/tmp";  // Valeur par défaut
    }
    return tmpdir;
}

int main(void) {
    char *tmpdir = get_temp_dir();
    printf("Répertoire temporaire: %s\n", tmpdir);

    // Construire un chemin vers un fichier temporaire
    char temp_file[1024];
    snprintf(temp_file, sizeof(temp_file), "%s/mon_fichier_temp.txt", tmpdir);
    printf("Fichier temporaire: %s\n", temp_file);

    return 0;
}
```

### 4. Variables de localisation

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

int main(void) {
    // Lire la locale depuis l'environnement
    char *lang = getenv("LANG");
    char *lc_all = getenv("LC_ALL");

    printf("Configuration de localisation:\n");
    printf("  LANG:   %s\n", lang ? lang : "(non défini)");
    printf("  LC_ALL: %s\n", lc_all ? lc_all : "(non défini)");

    // Configurer la locale pour le programme
    if (setlocale(LC_ALL, "") == NULL) {
        fprintf(stderr, "Erreur lors de la configuration de la locale\n");
    }

    // Afficher des nombres avec formatage local
    printf("\nExemple de formatage:\n");
    printf("  Nombre: %d\n", 1234567);

    return 0;
}
```

## Variables d'environnement et sécurité

### Précautions importantes

Les variables d'environnement peuvent poser des **risques de sécurité** :

#### 1. Ne jamais faire confiance aveuglément

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

// ❌ DANGEREUX
void bad_example(void) {
    char *path = getenv("FILE_PATH");
    char command[256];
    sprintf(command, "cat %s", path);  // Injection de commande!
    system(command);
}

// ✅ BON
void good_example(void) {
    char *path = getenv("FILE_PATH");

    if (path == NULL) {
        fprintf(stderr, "FILE_PATH non défini\n");
        return;
    }

    // Valider le chemin
    if (strlen(path) > PATH_MAX || strchr(path, ';') != NULL) {
        fprintf(stderr, "Chemin invalide\n");
        return;
    }

    // Utiliser une API sûre
    FILE *fp = fopen(path, "r");
    if (fp != NULL) {
        // Traiter le fichier...
        fclose(fp);
    }
}
```

#### 2. Nettoyer l'environnement pour les programmes privilégiés

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

void clean_environment(void) {
    // Supprimer les variables potentiellement dangereuses
    unsetenv("LD_PRELOAD");
    unsetenv("LD_LIBRARY_PATH");
    unsetenv("IFS");

    // Définir un PATH sûr
    setenv("PATH", "/usr/bin:/bin", 1);
}

int main(void) {
    // Pour un programme setuid ou privilégié
    if (getuid() != geteuid()) {
        clean_environment();
    }

    // Suite du programme...

    return 0;
}
```

#### 3. Variables sensibles

Certaines variables peuvent altérer le comportement du système :

- `LD_PRELOAD` : Charge des bibliothèques avant les autres
- `LD_LIBRARY_PATH` : Chemins de recherche des bibliothèques
- `IFS` : Séparateur de champs (shells)
- `PATH` : Peut être modifié pour exécuter un programme malveillant

### Bonnes pratiques

1. **Valider** toutes les entrées provenant de l'environnement
2. **Nettoyer** l'environnement pour les programmes privilégiés
3. **Limiter** la taille des valeurs acceptées
4. **Vérifier** les caractères spéciaux
5. **Ne jamais** passer directement des variables à `system()` ou `exec()`

## Déboguer avec les variables d'environnement

### Activer le mode debug

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int is_debug_enabled(void) {
    char *debug = getenv("DEBUG");
    return (debug != NULL &&
            (strcmp(debug, "1") == 0 ||
             strcmp(debug, "true") == 0 ||
             strcmp(debug, "yes") == 0));
}

void debug_log(const char *message) {
    if (is_debug_enabled()) {
        fprintf(stderr, "[DEBUG] %s\n", message);
    }
}

int main(void) {
    debug_log("Démarrage de l'application");

    // Code principal
    printf("Application en cours d'exécution...\n");

    debug_log("Traitement terminé");

    return 0;
}
```

**Utilisation :**
```bash
# Sans debug
./mon_programme
# Sortie: Application en cours d'exécution...

# Avec debug
DEBUG=1 ./mon_programme
# Sortie:
# [DEBUG] Démarrage de l'application
# Application en cours d'exécution...
# [DEBUG] Traitement terminé
```

## Afficher l'environnement complet

### Programme utilitaire

```c
#include <stdio.h>
#include <string.h>

extern char **environ;

void print_environment(void) {
    int count = 0;

    printf("Variables d'environnement:\n");
    printf("==========================\n\n");

    for (int i = 0; environ[i] != NULL; i++) {
        // Séparer nom et valeur pour un affichage formaté
        char *eq = strchr(environ[i], '=');
        if (eq != NULL) {
            int name_len = eq - environ[i];
            printf("%-20.*s = %s\n", name_len, environ[i], eq + 1);
            count++;
        }
    }

    printf("\nTotal: %d variables\n", count);
}

int main(void) {
    print_environment();
    return 0;
}
```

## Différences avec les variables shell

### Variables d'environnement vs Variables shell

Dans le shell :

```bash
# Variable shell (locale au shell)
MA_VARIABLE="test"

# Variable d'environnement (exportée)
export MA_VARIABLE="test"
```

**Important :** Seules les variables **exportées** sont visibles par les programmes lancés depuis le shell.

### Exemple

```bash
# Terminal 1
$ MA_VAR="valeur"        # Variable shell
$ export MA_VAR_EXP="valeur_exportee"  # Variable d'environnement
$ ./mon_programme

# mon_programme.c
int main(void) {
    printf("MA_VAR: %s\n", getenv("MA_VAR"));           // NULL
    printf("MA_VAR_EXP: %s\n", getenv("MA_VAR_EXP"));  // "valeur_exportee"
    return 0;
}
```

## Limites et considérations

### Taille maximale

Les variables d'environnement ont des limites :

- **Taille d'une variable** : Généralement 128 Ko sur Linux
- **Nombre de variables** : Limité par la mémoire disponible
- **Taille totale** : Environ 32 Ko sur certains systèmes

### Performance

L'accès à `getenv()` est rapide, mais pour des accès très fréquents, vous pouvez cacher la valeur :

```c
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    // Cache la valeur au démarrage
    static char *cached_path = NULL;

    if (cached_path == NULL) {
        cached_path = getenv("PATH");
        if (cached_path == NULL) {
            cached_path = "/usr/bin:/bin";
        }
    }

    // Utiliser cached_path au lieu d'appeler getenv() à chaque fois
    printf("PATH: %s\n", cached_path);

    return 0;
}
```

## Résumé

### Fonctions principales

| Fonction | Usage | Retour |
|----------|-------|--------|
| `getenv(name)` | Lire une variable | Pointeur vers valeur ou `NULL` |
| `setenv(name, value, overwrite)` | Définir/modifier | `0` succès, `-1` erreur |
| `unsetenv(name)` | Supprimer | `0` succès, `-1` erreur |
| `putenv(string)` | Définir (moins sûr) | `0` succès, `-1` erreur |

### Points clés

- ✅ Les variables d'environnement permettent de **configurer** les programmes
- ✅ Elles sont **héritées** lors de `fork()` (copie)
- ✅ Elles peuvent être **modifiées** avec `setenv()` (local au processus)
- ✅ Toujours **valider** les valeurs provenant de l'environnement
- ✅ Le tableau `environ` donne accès à toutes les variables
- ⚠️ Les modifications ne sont **pas partagées** entre processus
- ⚠️ Attention aux **risques de sécurité** (injection, privilèges)

### Bonnes pratiques

1. **Toujours vérifier** que `getenv()` ne retourne pas `NULL`
2. **Préférer `setenv()`** à `putenv()`
3. **Valider** les valeurs avant utilisation
4. **Documenter** les variables utilisées par votre programme
5. **Préfixer** vos variables avec le nom de l'application
6. **Fournir des valeurs par défaut** raisonnables
7. **Ne jamais stocker** de données sensibles (mots de passe) dans l'environnement

Les variables d'environnement sont un mécanisme fondamental pour la configuration des programmes sous Linux et constituent un outil essentiel dans la boîte à outils du programmeur système !

⏭️ [Gestion des signaux](/17-processus-et-signaux/05-gestion-signaux.md)
