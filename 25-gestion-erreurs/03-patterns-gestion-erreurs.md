🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 25.3 Patterns de gestion d'erreurs

## Introduction

En C, la gestion d'erreurs robuste repose sur l'utilisation de **patterns** (modèles ou schémas) éprouvés. Ces patterns sont des techniques de structuration du code qui permettent de gérer les erreurs de manière **cohérente**, **lisible** et **maintenable**. Cette section présente les patterns les plus courants et leurs cas d'usage.

---

## Pattern 1 : Vérification Systématique (Check-and-Return)

### Principe

Le pattern le plus simple et le plus fondamental : **vérifier chaque appel de fonction** et retourner immédiatement en cas d'erreur.

### Structure de base

```c
int fonction(void) {
    int resultat;

    resultat = operation1();
    if (resultat != 0) {
        return resultat;  // Propagation de l'erreur
    }

    resultat = operation2();
    if (resultat != 0) {
        return resultat;
    }

    resultat = operation3();
    if (resultat != 0) {
        return resultat;
    }

    return 0;  // Succès
}
```

### Exemple concret

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fonction utilitaire pour lire une ligne
int lire_ligne(FILE *f, char *buffer, size_t taille) {
    if (fgets(buffer, taille, f) == NULL) {
        if (feof(f)) {
            return 1;  // Fin de fichier
        }
        return -1;  // Erreur de lecture
    }
    return 0;  // Succès
}

// Fonction qui traite un fichier de configuration
int charger_config(const char *fichier) {
    FILE *f = fopen(fichier, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", fichier);
        return -1;
    }

    char ligne[256];

    // Lecture ligne 1
    if (lire_ligne(f, ligne, sizeof(ligne)) != 0) {
        fprintf(stderr, "Erreur : lecture ligne 1\n");
        fclose(f);
        return -1;
    }
    printf("Config 1: %s", ligne);

    // Lecture ligne 2
    if (lire_ligne(f, ligne, sizeof(ligne)) != 0) {
        fprintf(stderr, "Erreur : lecture ligne 2\n");
        fclose(f);
        return -1;
    }
    printf("Config 2: %s", ligne);

    fclose(f);
    return 0;
}

int main(void) {
    if (charger_config("config.txt") != 0) {
        return EXIT_FAILURE;
    }

    printf("Configuration chargée avec succès\n");
    return EXIT_SUCCESS;
}
```

### Avantages

- ✅ **Simple et direct** : Facile à comprendre
- ✅ **Explicite** : Le flux d'erreur est visible
- ✅ **Propagation naturelle** : Les erreurs remontent automatiquement

### Inconvénients

- ❌ **Répétitif** : Beaucoup de code dupliqué (`fclose` répété)
- ❌ **Risque d'oubli** : Facile d'oublier de nettoyer les ressources
- ❌ **Verbeux** : Code long pour de simples vérifications

---

## Pattern 2 : Nettoyage Centralisé avec Goto

### Principe

Utiliser des **labels goto** pour centraliser le nettoyage des ressources. C'est le pattern préféré dans le noyau Linux et de nombreux projets C professionnels.

### Structure de base

```c
int fonction(void) {
    Ressource *r1 = NULL;
    Ressource *r2 = NULL;
    Ressource *r3 = NULL;
    int resultat = -1;  // Par défaut : erreur

    r1 = allouer_ressource1();
    if (r1 == NULL) {
        goto cleanup;
    }

    r2 = allouer_ressource2();
    if (r2 == NULL) {
        goto cleanup_r1;
    }

    r3 = allouer_ressource3();
    if (r3 == NULL) {
        goto cleanup_r2;
    }

    // Traitement réussi
    resultat = 0;

    // Nettoyage en cascade (ordre inverse)
    liberer_ressource3(r3);
cleanup_r2:
    liberer_ressource2(r2);
cleanup_r1:
    liberer_ressource1(r1);
cleanup:
    return resultat;
}
```

### Exemple concret : Traitement de fichier multi-étapes

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int traiter_donnees(const char *fichier_entree, const char *fichier_sortie) {
    FILE *entree = NULL;
    FILE *sortie = NULL;
    char *buffer = NULL;
    int resultat = -1;  // Par défaut : échec

    // Étape 1 : Ouvrir le fichier d'entrée
    entree = fopen(fichier_entree, "r");
    if (entree == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", fichier_entree);
        goto cleanup;
    }

    // Étape 2 : Ouvrir le fichier de sortie
    sortie = fopen(fichier_sortie, "w");
    if (sortie == NULL) {
        fprintf(stderr, "Erreur : impossible de créer %s\n", fichier_sortie);
        goto cleanup_entree;
    }

    // Étape 3 : Allouer le buffer
    buffer = malloc(4096);
    if (buffer == NULL) {
        fprintf(stderr, "Erreur : allocation mémoire échouée\n");
        goto cleanup_sortie;
    }

    // Traitement des données
    size_t octets_lus;
    while ((octets_lus = fread(buffer, 1, 4096, entree)) > 0) {
        if (fwrite(buffer, 1, octets_lus, sortie) != octets_lus) {
            fprintf(stderr, "Erreur : écriture échouée\n");
            goto cleanup_buffer;  // Erreur : on nettoie tout
        }
    }

    // Vérifier si fread a échoué
    if (ferror(entree)) {
        fprintf(stderr, "Erreur : lecture échouée\n");
        goto cleanup_buffer;
    }

    printf("Traitement réussi\n");
    resultat = 0;  // Succès !

    // Nettoyage en cascade (ordre inverse de l'allocation)
cleanup_buffer:
    free(buffer);
cleanup_sortie:
    fclose(sortie);
cleanup_entree:
    fclose(entree);
cleanup:
    return resultat;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage : %s <entree> <sortie>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int code = traiter_donnees(argv[1], argv[2]);
    return (code == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
```

### Variante : Labels descriptifs

Vous pouvez utiliser des labels plus descriptifs :

```c
int fonction(void) {
    FILE *fichier = NULL;
    char *buffer = NULL;
    int resultat = -1;

    fichier = fopen("data.txt", "r");
    if (fichier == NULL) {
        goto error_no_cleanup;
    }

    buffer = malloc(1024);
    if (buffer == NULL) {
        goto error_close_file;
    }

    // ... traitement ...
    resultat = 0;

    // Nettoyage
    free(buffer);
error_close_file:
    fclose(fichier);
error_no_cleanup:
    return resultat;
}
```

### Avantages

- ✅ **Robuste** : Impossible d'oublier de libérer une ressource
- ✅ **Lisible** : Le chemin de nettoyage est clair
- ✅ **Efficace** : Pas de surcharge (overhead)
- ✅ **Prouvé** : Utilisé dans des millions de lignes de code (Linux kernel)

### Inconvénients

- ❌ **Goto stigmatisé** : Certains développeurs sont réticents (injustement)
- ❌ **Ordre important** : Les labels doivent être dans le bon ordre

---

## Pattern 3 : Guard Clauses (Retours Anticipés)

### Principe

**Valider les préconditions au début** de la fonction et retourner immédiatement en cas d'erreur. Cela évite l'imbrication excessive de `if`.

### ❌ Sans guard clauses (mauvais)

```c
int traiter_utilisateur(const char *nom, int age, const char *email) {
    if (nom != NULL) {
        if (age >= 0 && age < 150) {
            if (email != NULL) {
                // Vrai traitement imbriqué au 3e niveau
                printf("Utilisateur valide : %s\n", nom);
                return 0;
            } else {
                fprintf(stderr, "Erreur : email NULL\n");
                return -1;
            }
        } else {
            fprintf(stderr, "Erreur : âge invalide\n");
            return -1;
        }
    } else {
        fprintf(stderr, "Erreur : nom NULL\n");
        return -1;
    }
}
```

### ✅ Avec guard clauses (bon)

```c
int traiter_utilisateur(const char *nom, int age, const char *email) {
    // Validation des paramètres : retour immédiat si invalide
    if (nom == NULL) {
        fprintf(stderr, "Erreur : nom NULL\n");
        return -1;
    }

    if (age < 0 || age >= 150) {
        fprintf(stderr, "Erreur : âge invalide (%d)\n", age);
        return -1;
    }

    if (email == NULL) {
        fprintf(stderr, "Erreur : email NULL\n");
        return -1;
    }

    // Le code principal n'est plus imbriqué
    printf("Utilisateur valide : %s, %d ans, %s\n", nom, age, email);
    return 0;
}
```

### Exemple avec ressources

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int sauvegarder_donnees(const char *fichier, const void *data, size_t taille) {
    // Guard clause 1 : Validation des paramètres
    if (fichier == NULL || data == NULL || taille == 0) {
        fprintf(stderr, "Erreur : paramètres invalides\n");
        return -1;
    }

    // Guard clause 2 : Ouverture du fichier
    FILE *f = fopen(fichier, "wb");
    if (f == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir %s\n", fichier);
        return -1;
    }

    // Guard clause 3 : Écriture
    if (fwrite(data, 1, taille, f) != taille) {
        fprintf(stderr, "Erreur : écriture incomplète\n");
        fclose(f);
        return -1;
    }

    // Succès
    fclose(f);
    return 0;
}

int main(void) {
    char donnees[] = "Hello, World!";

    if (sauvegarder_donnees("output.txt", donnees, strlen(donnees)) != 0) {
        return EXIT_FAILURE;
    }

    printf("Données sauvegardées\n");
    return EXIT_SUCCESS;
}
```

### Avantages

- ✅ **Lisibilité** : Élimine l'imbrication profonde
- ✅ **Validation claire** : Les préconditions sont au début
- ✅ **Maintenance facile** : Ajouter une validation est simple

### Inconvénients

- ❌ **Multiples points de sortie** : Peut compliquer le débogage
- ❌ **Nécessite discipline** : Il faut nettoyer avant chaque return

---

## Pattern 4 : Structure de Retour avec État

### Principe

Au lieu de retourner un simple code d'erreur, **retourner une structure** contenant à la fois le résultat et les informations d'erreur.

### Structure de base

```c
typedef struct {
    int code;           // 0 = succès, autre = erreur
    int erreur_ligne;   // Ligne de l'erreur (si applicable)
    char message[256];  // Message d'erreur détaillé
} Resultat;

// Fonction qui retourne un Resultat
Resultat effectuer_operation(void);
```

### Exemple : Parser de configuration

```c
#include <stdio.h>
#include <string.h>

typedef enum {
    SUCCES = 0,
    ERREUR_SYNTAXE = 1,
    ERREUR_VALEUR = 2,
    ERREUR_FICHIER = 3
} CodeErreur;

typedef struct {
    CodeErreur code;
    int ligne;
    char message[256];
    int valeur;  // Résultat si succès
} ResultatParsing;

ResultatParsing parser_entier(const char *texte, int ligne) {
    ResultatParsing resultat = {0};
    resultat.ligne = ligne;

    if (texte == NULL || texte[0] == '\0') {
        resultat.code = ERREUR_SYNTAXE;
        snprintf(resultat.message, sizeof(resultat.message),
                 "Texte vide à la ligne %d", ligne);
        return resultat;
    }

    char *fin;
    long valeur = strtol(texte, &fin, 10);

    if (*fin != '\0') {
        resultat.code = ERREUR_SYNTAXE;
        snprintf(resultat.message, sizeof(resultat.message),
                 "Caractère invalide '%c' à la ligne %d", *fin, ligne);
        return resultat;
    }

    if (valeur < 0 || valeur > 100) {
        resultat.code = ERREUR_VALEUR;
        snprintf(resultat.message, sizeof(resultat.message),
                 "Valeur %ld hors limites [0-100] à la ligne %d", valeur, ligne);
        return resultat;
    }

    // Succès
    resultat.code = SUCCES;
    resultat.valeur = (int)valeur;
    snprintf(resultat.message, sizeof(resultat.message), "OK");

    return resultat;
}

int main(void) {
    const char *tests[] = {"42", "150", "abc", ""};

    for (int i = 0; i < 4; i++) {
        ResultatParsing r = parser_entier(tests[i], i + 1);

        if (r.code == SUCCES) {
            printf("✓ Ligne %d : valeur = %d\n", r.ligne, r.valeur);
        } else {
            fprintf(stderr, "✗ Erreur : %s\n", r.message);
        }
    }

    return 0;
}
```

**Sortie :**
```
✓ Ligne 1 : valeur = 42
✗ Erreur : Valeur 150 hors limites [0-100] à la ligne 2
✗ Erreur : Caractère invalide 'a' à la ligne 3
✗ Erreur : Texte vide à la ligne 4
```

### Variante : Retour via pointeur

Certaines API préfèrent retourner le code d'erreur directement et la valeur via un pointeur :

```c
CodeErreur parser_entier(const char *texte, int *resultat, char *msg, size_t msg_len) {
    if (texte == NULL) {
        snprintf(msg, msg_len, "Texte NULL");
        return ERREUR_SYNTAXE;
    }

    // ... parsing ...

    *resultat = valeur_parsee;
    snprintf(msg, msg_len, "OK");
    return SUCCES;
}

// Utilisation
int valeur;
char message[256];
CodeErreur code = parser_entier("42", &valeur, message, sizeof(message));

if (code != SUCCES) {
    fprintf(stderr, "Erreur : %s\n", message);
}
```

### Avantages

- ✅ **Informations riches** : Code, message, contexte
- ✅ **Debugging facilité** : Toutes les infos en un seul endroit
- ✅ **Traçabilité** : Facile de logger les erreurs

### Inconvénients

- ❌ **Surcharge mémoire** : Structures plus grosses
- ❌ **Syntaxe plus lourde** : Plus de code à écrire

---

## Pattern 5 : Wrapper Functions (Fonctions Enveloppes)

### Principe

Créer des fonctions qui **encapsulent** les appels système/bibliothèque et gèrent les erreurs de manière uniforme.

### Exemple : Wrappers pour allocation mémoire

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Wrapper qui termine le programme si malloc échoue
void* xmalloc(size_t taille) {
    void *ptr = malloc(taille);

    if (ptr == NULL) {
        fprintf(stderr, "Erreur fatale : allocation de %zu octets échouée\n", taille);
        exit(EXIT_FAILURE);
    }

    return ptr;
}

// Wrapper qui initialise à zéro
void* xcalloc(size_t nb, size_t taille) {
    void *ptr = calloc(nb, taille);

    if (ptr == NULL) {
        fprintf(stderr, "Erreur fatale : calloc(%zu, %zu) échoué\n", nb, taille);
        exit(EXIT_FAILURE);
    }

    return ptr;
}

// Wrapper pour realloc
void* xrealloc(void *ptr, size_t nouvelle_taille) {
    void *nouveau = realloc(ptr, nouvelle_taille);

    if (nouveau == NULL && nouvelle_taille > 0) {
        fprintf(stderr, "Erreur fatale : realloc(%zu) échoué\n", nouvelle_taille);
        exit(EXIT_FAILURE);
    }

    return nouveau;
}

int main(void) {
    // Plus besoin de vérifier : xmalloc termine le programme en cas d'échec
    char *buffer = xmalloc(1024);
    strcpy(buffer, "Hello, World!");

    printf("%s\n", buffer);

    // Redimensionnement sûr
    buffer = xrealloc(buffer, 2048);

    free(buffer);
    return EXIT_SUCCESS;
}
```

### Exemple : Wrapper pour ouverture de fichier

```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

FILE* xfopen(const char *chemin, const char *mode) {
    FILE *f = fopen(chemin, mode);

    if (f == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir '%s' en mode '%s': %s\n",
                chemin, mode, strerror(errno));
        exit(EXIT_FAILURE);
    }

    return f;
}

void xfclose(FILE *f) {
    if (f != NULL && fclose(f) != 0) {
        fprintf(stderr, "Avertissement : fclose a échoué\n");
    }
}

int main(void) {
    FILE *f = xfopen("data.txt", "w");

    fprintf(f, "Hello, World!\n");

    xfclose(f);

    printf("Fichier écrit avec succès\n");
    return EXIT_SUCCESS;
}
```

### Avantages

- ✅ **Simplifie le code appelant** : Pas de vérifications répétées
- ✅ **Gestion cohérente** : Toutes les erreurs traitées de la même façon
- ✅ **Facilite le refactoring** : Modifier la gestion d'erreur en un seul endroit

### Inconvénients

- ❌ **Perte de contrôle** : Le comportement est décidé par le wrapper
- ❌ **Peut cacher des problèmes** : Erreurs fatales vs récupérables

---

## Pattern 6 : Context Object (Objet de Contexte)

### Principe

Passer une **structure de contexte** entre les fonctions pour accumuler les informations d'erreur.

### Structure de base

```c
typedef struct {
    int code_erreur;
    int nb_erreurs;
    int nb_avertissements;
    char dernier_message[256];
} Contexte;
```

### Exemple complet : Validation de formulaire

```c
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    int nb_erreurs;
    int nb_avertissements;
    char messages[10][256];  // Historique des erreurs
    int nb_messages;
} ContexteValidation;

void init_contexte(ContexteValidation *ctx) {
    memset(ctx, 0, sizeof(*ctx));
}

void ajouter_erreur(ContexteValidation *ctx, const char *message) {
    ctx->nb_erreurs++;

    if (ctx->nb_messages < 10) {
        snprintf(ctx->messages[ctx->nb_messages], 256, "[ERREUR] %s", message);
        ctx->nb_messages++;
    }
}

void ajouter_avertissement(ContexteValidation *ctx, const char *message) {
    ctx->nb_avertissements++;

    if (ctx->nb_messages < 10) {
        snprintf(ctx->messages[ctx->nb_messages], 256, "[WARN] %s", message);
        ctx->nb_messages++;
    }
}

bool valider_nom(const char *nom, ContexteValidation *ctx) {
    if (nom == NULL || nom[0] == '\0') {
        ajouter_erreur(ctx, "Le nom est vide");
        return false;
    }

    if (strlen(nom) < 2) {
        ajouter_avertissement(ctx, "Le nom est très court");
    }

    return true;
}

bool valider_age(int age, ContexteValidation *ctx) {
    if (age < 0) {
        ajouter_erreur(ctx, "L'âge est négatif");
        return false;
    }

    if (age > 150) {
        ajouter_erreur(ctx, "L'âge est irréaliste");
        return false;
    }

    if (age < 18) {
        ajouter_avertissement(ctx, "L'utilisateur est mineur");
    }

    return true;
}

bool valider_email(const char *email, ContexteValidation *ctx) {
    if (email == NULL || email[0] == '\0') {
        ajouter_erreur(ctx, "L'email est vide");
        return false;
    }

    if (strchr(email, '@') == NULL) {
        ajouter_erreur(ctx, "L'email ne contient pas de @");
        return false;
    }

    return true;
}

bool valider_utilisateur(const char *nom, int age, const char *email,
                         ContexteValidation *ctx) {
    bool valide = true;

    valide &= valider_nom(nom, ctx);
    valide &= valider_age(age, ctx);
    valide &= valider_email(email, ctx);

    return valide;
}

void afficher_rapport(const ContexteValidation *ctx) {
    printf("\n=== Rapport de validation ===\n");
    printf("Erreurs : %d\n", ctx->nb_erreurs);
    printf("Avertissements : %d\n", ctx->nb_avertissements);

    printf("\nDétails :\n");
    for (int i = 0; i < ctx->nb_messages; i++) {
        printf("  %s\n", ctx->messages[i]);
    }
}

int main(void) {
    ContexteValidation ctx;
    init_contexte(&ctx);

    // Test 1 : Utilisateur invalide
    printf("Test 1 : Utilisateur invalide\n");
    bool ok = valider_utilisateur("J", 200, "invalide", &ctx);
    afficher_rapport(&ctx);
    printf("Résultat : %s\n", ok ? "VALIDE" : "INVALIDE");

    // Test 2 : Utilisateur valide avec avertissement
    printf("\n\nTest 2 : Utilisateur mineur\n");
    init_contexte(&ctx);  // Réinitialisation
    ok = valider_utilisateur("Alice", 16, "alice@example.com", &ctx);
    afficher_rapport(&ctx);
    printf("Résultat : %s\n", ok ? "VALIDE" : "INVALIDE");

    return 0;
}
```

**Sortie :**
```
Test 1 : Utilisateur invalide

=== Rapport de validation ===
Erreurs : 2
Avertissements : 1

Détails :
  [WARN] Le nom est très court
  [ERREUR] L'âge est irréaliste
  [ERREUR] L'email ne contient pas de @
Résultat : INVALIDE


Test 2 : Utilisateur mineur

=== Rapport de validation ===
Erreurs : 0
Avertissements : 1

Détails :
  [WARN] L'utilisateur est mineur
Résultat : VALIDE
```

### Avantages

- ✅ **Accumulation d'erreurs** : Plusieurs erreurs détectées en une passe
- ✅ **Contexte riche** : Historique complet des problèmes
- ✅ **Flexibilité** : Facilite le reporting et le logging

### Inconvénients

- ❌ **Complexité** : Plus de code à écrire et maintenir
- ❌ **Overhead mémoire** : Structure de contexte à passer partout

---

## Pattern 7 : Error Callback (Fonction de Rappel d'Erreur)

### Principe

Permettre au code appelant de **définir un handler** personnalisé pour les erreurs.

### Structure de base

```c
// Type de fonction callback
typedef void (*ErrorHandler)(int code, const char *message);

// Variable globale pour le handler
static ErrorHandler error_handler = NULL;

// Enregistrer un handler
void set_error_handler(ErrorHandler handler) {
    error_handler = handler;
}

// Déclencher une erreur
void report_error(int code, const char *message) {
    if (error_handler != NULL) {
        error_handler(code, message);
    } else {
        // Comportement par défaut
        fprintf(stderr, "Erreur %d: %s\n", code, message);
    }
}
```

### Exemple complet

```c
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

// Type de callback d'erreur
typedef void (*ErrorCallback)(int code, const char *message, void *user_data);

// Gestionnaire d'erreurs global
static struct {
    ErrorCallback callback;
    void *user_data;
} error_system = {NULL, NULL};

// Enregistrer un callback
void set_error_callback(ErrorCallback callback, void *user_data) {
    error_system.callback = callback;
    error_system.user_data = user_data;
}

// Signaler une erreur
void signal_error(int code, const char *format, ...) {
    char message[512];
    va_list args;

    va_start(args, format);
    vsnprintf(message, sizeof(message), format, args);
    va_end(args);

    if (error_system.callback != NULL) {
        error_system.callback(code, message, error_system.user_data);
    } else {
        // Comportement par défaut
        fprintf(stderr, "Erreur %d: %s\n", code, message);
    }
}

// Callback 1 : Affichage simple
void error_handler_simple(int code, const char *message, void *user_data) {
    (void)user_data;  // Non utilisé
    printf("[ERREUR %d] %s\n", code, message);
}

// Callback 2 : Logging dans un fichier
void error_handler_file(int code, const char *message, void *user_data) {
    FILE *log = (FILE *)user_data;

    if (log != NULL) {
        time_t now = time(NULL);
        char timestamp[64];
        strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S",
                 localtime(&now));

        fprintf(log, "[%s] Erreur %d: %s\n", timestamp, code, message);
        fflush(log);
    }
}

// Callback 3 : Compteur d'erreurs
typedef struct {
    int nb_erreurs;
    int nb_critiques;
} StatistiquesErreurs;

void error_handler_stats(int code, const char *message, void *user_data) {
    StatistiquesErreurs *stats = (StatistiquesErreurs *)user_data;

    stats->nb_erreurs++;
    if (code >= 100) {
        stats->nb_critiques++;
    }

    fprintf(stderr, "[%d erreurs, %d critiques] Code %d: %s\n",
            stats->nb_erreurs, stats->nb_critiques, code, message);
}

// Fonction qui peut générer des erreurs
int traiter_donnees(int valeur) {
    if (valeur < 0) {
        signal_error(10, "Valeur négative: %d", valeur);
        return -1;
    }

    if (valeur > 100) {
        signal_error(150, "Valeur trop grande: %d", valeur);
        return -1;
    }

    return 0;
}

int main(void) {
    // Test 1 : Sans callback (défaut)
    printf("=== Test 1 : Comportement par défaut ===\n");
    traiter_donnees(-5);
    traiter_donnees(150);

    // Test 2 : Callback simple
    printf("\n=== Test 2 : Callback simple ===\n");
    set_error_callback(error_handler_simple, NULL);
    traiter_donnees(-10);

    // Test 3 : Callback avec fichier
    printf("\n=== Test 3 : Logging dans fichier ===\n");
    FILE *log = fopen("errors.log", "w");
    set_error_callback(error_handler_file, log);
    traiter_donnees(200);
    fclose(log);
    printf("Erreur loggée dans errors.log\n");

    // Test 4 : Callback avec statistiques
    printf("\n=== Test 4 : Statistiques d'erreurs ===\n");
    StatistiquesErreurs stats = {0, 0};
    set_error_callback(error_handler_stats, &stats);
    traiter_donnees(-1);
    traiter_donnees(50);   // OK
    traiter_donnees(-2);
    traiter_donnees(150);
    printf("Total : %d erreurs, %d critiques\n", stats.nb_erreurs, stats.nb_critiques);

    return 0;
}
```

### Avantages

- ✅ **Flexibilité maximale** : Le comportement est personnalisable
- ✅ **Séparation des responsabilités** : Le code métier ne gère pas l'affichage
- ✅ **Testabilité** : Facile de capturer les erreurs en test

### Inconvénients

- ❌ **Complexité** : Architecture plus sophistiquée
- ❌ **État global** : Peut causer des problèmes en multi-thread

---

## Comparaison des Patterns

| Pattern | Simplicité | Robustesse | Flexibilité | Performance | Usage recommandé |
|---------|:----------:|:----------:|:-----------:|:-----------:|------------------|
| **Check-and-Return** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐⭐ | Code simple, scripts |
| **Goto Cleanup** | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | Code système, kernel |
| **Guard Clauses** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | Fonctions avec validation |
| **Structure de Retour** | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | Parsers, APIs complexes |
| **Wrapper Functions** | ⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐ | Utilitaires, scripts |
| **Context Object** | ⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | Validation, compilateurs |
| **Error Callback** | ⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | Bibliothèques, frameworks |

---

## Bonnes Pratiques Générales

### 1. Cohérence avant tout

**Choisissez UN pattern** et utilisez-le partout dans votre projet. Mélanger les patterns rend le code difficile à maintenir.

```c
// ❌ Incohérent
int fonction1(void) {
    if (erreur) return -1;  // Pattern 1
}

int fonction2(void) {
    if (erreur) goto cleanup;  // Pattern 2
    // ...
cleanup:
    return code;
}

// ✅ Cohérent
int fonction1(void) {
    if (erreur) goto cleanup;
cleanup:
    return code;
}

int fonction2(void) {
    if (erreur) goto cleanup;
cleanup:
    return code;
}
```

### 2. Documenter le contrat d'erreur

Documentez clairement comment chaque fonction gère les erreurs :

```c
/**
 * Ouvre un fichier de configuration.
 *
 * @param chemin Chemin du fichier
 * @return 0 en cas de succès, -1 en cas d'erreur
 *
 * En cas d'erreur, errno est défini et un message est affiché sur stderr.
 * Cette fonction ne modifie pas les variables globales autres qu'errno.
 */
int ouvrir_config(const char *chemin);
```

### 3. Ne jamais ignorer les erreurs

```c
// ❌ Dangereux
malloc(1024);  // Résultat ignoré

// ✅ Correct
void *ptr = malloc(1024);
if (ptr == NULL) {
    // Gérer l'erreur
}
```

### 4. Nettoyer même en cas d'erreur

```c
int fonction(void) {
    char *buffer = malloc(1024);

    if (traitement() != 0) {
        free(buffer);  // ✅ Nettoyage avant de partir
        return -1;
    }

    free(buffer);
    return 0;
}
```

### 5. Propagation intelligente

Ne capturez une erreur que si vous pouvez **faire quelque chose d'utile**. Sinon, propagez-la :

```c
int fonction_bas_niveau(void) {
    if (erreur) {
        return -1;  // Propagation
    }
    return 0;
}

int fonction_haut_niveau(void) {
    if (fonction_bas_niveau() != 0) {
        // Ici on peut ajouter du contexte
        fprintf(stderr, "Erreur dans le traitement principal\n");
        return -1;
    }
    return 0;
}
```

---

## Recommandations par Type de Projet

### Petit script / Programme simple
→ **Check-and-Return** + **Guard Clauses**

### Application système / Driver
→ **Goto Cleanup** (comme le kernel Linux)

### Bibliothèque / API publique
→ **Structure de Retour** + **Error Callback** (optionnel)

### Compilateur / Parser / Validateur
→ **Context Object** pour accumuler les erreurs

### Outil CLI / Utilitaire
→ **Wrapper Functions** pour simplifier

---

## Exemple Récapitulatif : Tous les patterns combinés

Voici un exemple qui combine intelligemment plusieurs patterns :

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Pattern 4 : Structure de retour
typedef enum {
    OK = 0,
    ERR_MEMOIRE = 1,
    ERR_FICHIER = 2,
    ERR_FORMAT = 3
} CodeErreur;

typedef struct {
    CodeErreur code;
    char message[256];
} Resultat;

// Pattern 7 : Callback d'erreur
typedef void (*LogCallback)(const char *message);
static LogCallback log_callback = NULL;

void set_log_callback(LogCallback cb) {
    log_callback = cb;
}

void log_message(const char *message) {
    if (log_callback != NULL) {
        log_callback(message);
    }
}

// Pattern 5 : Wrapper
void* safe_malloc(size_t taille, Resultat *res) {
    void *ptr = malloc(taille);
    if (ptr == NULL) {
        res->code = ERR_MEMOIRE;
        snprintf(res->message, sizeof(res->message),
                 "Allocation de %zu octets échouée", taille);
        log_message(res->message);
    }
    return ptr;
}

// Pattern 2 : Goto cleanup + Pattern 3 : Guard clauses
Resultat traiter_fichier(const char *chemin) {
    Resultat res = {OK, ""};
    FILE *f = NULL;
    char *buffer = NULL;

    // Guard clause
    if (chemin == NULL) {
        res.code = ERR_FICHIER;
        snprintf(res.message, sizeof(res.message), "Chemin NULL");
        goto cleanup;
    }

    f = fopen(chemin, "r");
    if (f == NULL) {
        res.code = ERR_FICHIER;
        snprintf(res.message, sizeof(res.message),
                 "Impossible d'ouvrir %s", chemin);
        goto cleanup;
    }

    buffer = safe_malloc(1024, &res);
    if (buffer == NULL) {
        goto cleanup_file;
    }

    // Traitement...
    snprintf(res.message, sizeof(res.message), "Traitement réussi");
    log_message(res.message);

    free(buffer);
cleanup_file:
    fclose(f);
cleanup:
    return res;
}

// Callback de logging simple
void simple_logger(const char *message) {
    printf("[LOG] %s\n", message);
}

int main(void) {
    set_log_callback(simple_logger);

    Resultat r = traiter_fichier("data.txt");

    if (r.code != OK) {
        fprintf(stderr, "Erreur : %s\n", r.message);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
```

---

## Conclusion

La gestion d'erreurs robuste en C repose sur des **patterns éprouvés** :

### 🎯 Choisissez selon le contexte

- **Code simple** : Check-and-Return + Guard Clauses
- **Code système** : Goto Cleanup
- **Bibliothèques** : Structure de Retour + Callbacks
- **Validation complexe** : Context Object

### 🌟 Principes universels

1. **Cohérence** : Un seul pattern par projet
2. **Explicite** : Les erreurs doivent être visibles
3. **Robuste** : Toujours nettoyer les ressources
4. **Documenté** : Expliquer le contrat d'erreur
5. **Testé** : Vérifier tous les chemins d'erreur

### 💡 Règle d'or

> **Un bon code C gère les erreurs de manière prévisible, cohérente et maintenable.**

Maîtriser ces patterns vous permettra d'écrire du code C **professionnel** et **fiable**, capable de gérer élégamment les situations d'erreur.

---

**Prochaine section** : 25.4 Assertions

⏭️ [Assertions](/25-gestion-erreurs/04-assertions.md)
