🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 25.1 Codes de retour vs exceptions simulées

## Introduction

En C, contrairement à des langages modernes comme Python, Java ou C++, il n'existe **pas de mécanisme d'exceptions natif**. Lorsqu'une erreur se produit, le programmeur doit choisir comment la signaler et la gérer. Cette section explore les deux principales approches : les codes de retour (la méthode standard en C) et les exceptions simulées (techniques avancées).

---

## Les codes de retour : L'approche standard en C

### Principe fondamental

La méthode la plus courante en C consiste à **retourner un code d'état** depuis les fonctions pour indiquer le succès ou l'échec de l'opération.

### Convention classique

```c
// Convention : 0 = succès, valeur négative ou non-nulle = erreur
int fonction_qui_peut_echouer(void) {
    if (/* tout va bien */) {
        return 0;  // Succès
    }
    return -1;     // Échec
}
```

### Exemple concret : Ouverture de fichier

```c
#include <stdio.h>
#include <stdlib.h>

int lire_fichier(const char *nom_fichier, char **contenu) {
    FILE *fichier = fopen(nom_fichier, "r");

    if (fichier == NULL) {
        return -1;  // Erreur : impossible d'ouvrir le fichier
    }

    // ... lecture du fichier ...

    fclose(fichier);
    return 0;  // Succès
}

int main(void) {
    char *contenu = NULL;

    if (lire_fichier("config.txt", &contenu) != 0) {
        fprintf(stderr, "Erreur : impossible de lire le fichier\n");
        return EXIT_FAILURE;
    }

    printf("Fichier lu avec succès\n");
    free(contenu);
    return EXIT_SUCCESS;
}
```

### Variantes de codes de retour

#### 1. Booléen (succès/échec)

```c
#include <stdbool.h>

bool initialiser_systeme(void) {
    // ... initialisation ...
    return true;  // ou false en cas d'erreur
}
```

#### 2. Codes d'erreur typés (énumérations)

```c
typedef enum {
    SUCCES = 0,
    ERREUR_MEMOIRE = 1,
    ERREUR_FICHIER = 2,
    ERREUR_RESEAU = 3,
    ERREUR_PERMISSION = 4
} CodeErreur;

CodeErreur ouvrir_connexion(const char *serveur) {
    if (serveur == NULL) {
        return ERREUR_RESEAU;
    }

    // Tentative d'allocation
    void *buffer = malloc(1024);
    if (buffer == NULL) {
        return ERREUR_MEMOIRE;
    }

    // ... reste du code ...
    free(buffer);
    return SUCCES;
}

int main(void) {
    CodeErreur resultat = ouvrir_connexion("example.com");

    switch (resultat) {
        case SUCCES:
            printf("Connexion établie\n");
            break;
        case ERREUR_MEMOIRE:
            fprintf(stderr, "Erreur : mémoire insuffisante\n");
            break;
        case ERREUR_RESEAU:
            fprintf(stderr, "Erreur : problème réseau\n");
            break;
        default:
            fprintf(stderr, "Erreur inconnue\n");
    }

    return (resultat == SUCCES) ? EXIT_SUCCESS : EXIT_FAILURE;
}
```

#### 3. Retour de pointeur NULL

```c
// Convention : NULL = erreur, pointeur valide = succès
char* allouer_buffer(size_t taille) {
    char *buffer = malloc(taille);

    if (buffer == NULL) {
        return NULL;  // Erreur d'allocation
    }

    // Initialisation du buffer
    memset(buffer, 0, taille);
    return buffer;  // Succès
}

int main(void) {
    char *buffer = allouer_buffer(1024);

    if (buffer == NULL) {
        fprintf(stderr, "Erreur : allocation impossible\n");
        return EXIT_FAILURE;
    }

    // Utilisation du buffer
    free(buffer);
    return EXIT_SUCCESS;
}
```

### Avantages des codes de retour

- ✅ **Simplicité** : Facile à comprendre et à implémenter
- ✅ **Explicite** : La gestion d'erreur est visible dans le code
- ✅ **Performance** : Pas de surcharge (overhead) significative
- ✅ **Portable** : Fonctionne partout, sur tous les compilateurs
- ✅ **Contrôle total** : Le programmeur décide à chaque étape

### Inconvénients des codes de retour

- ❌ **Verbosité** : Beaucoup de `if (... != 0)` à écrire
- ❌ **Erreurs ignorées** : Rien n'oblige à vérifier le code de retour
- ❌ **Propagation manuelle** : Il faut remonter l'erreur manuellement
- ❌ **Ambiguïté** : Difficile de retourner à la fois une valeur ET un code d'erreur

### Pattern : Retour via paramètres

Pour contourner l'ambiguïté, on peut retourner la valeur via un pointeur et le code d'erreur comme valeur de retour :

```c
int calculer_division(int a, int b, int *resultat) {
    if (b == 0) {
        return -1;  // Erreur : division par zéro
    }

    *resultat = a / b;
    return 0;  // Succès
}

int main(void) {
    int resultat;

    if (calculer_division(10, 0, &resultat) != 0) {
        fprintf(stderr, "Erreur : division par zéro\n");
        return EXIT_FAILURE;
    }

    printf("Résultat : %d\n", resultat);
    return EXIT_SUCCESS;
}
```

---

## Les exceptions simulées : Techniques avancées

Puisque C ne possède pas d'exceptions natives, certaines techniques permettent de les **simuler** pour obtenir un comportement similaire.

### Technique 1 : setjmp/longjmp (Sauts non-locaux)

#### Principe

Les fonctions `setjmp()` et `longjmp()` permettent de **sauter** d'un point du code à un autre, même à travers plusieurs niveaux d'appels de fonctions. Cela ressemble au `try/catch` des autres langages.

```c
#include <setjmp.h>

jmp_buf buffer_saut;  // Enregistre l'environnement de saut

void fonction_critique(void) {
    // Si une erreur se produit, on "saute" vers le point de capture
    longjmp(buffer_saut, 1);  // 1 = code d'erreur
}
```

#### Exemple complet

```c
#include <stdio.h>
#include <setjmp.h>
#include <stdlib.h>

jmp_buf point_capture;

typedef enum {
    AUCUNE_ERREUR = 0,
    ERREUR_MEMOIRE = 1,
    ERREUR_FICHIER = 2
} TypeErreur;

void allouer_donnees(void) {
    void *ptr = malloc(1000000000000);  // Allocation volontairement énorme

    if (ptr == NULL) {
        printf("Erreur détectée, déclenchement de l'exception...\n");
        longjmp(point_capture, ERREUR_MEMOIRE);  // "Lance" l'exception
    }

    free(ptr);
}

void fonction_intermediaire(void) {
    printf("Entrée dans fonction_intermediaire\n");
    allouer_donnees();
    printf("Cette ligne ne sera jamais exécutée\n");
}

int main(void) {
    printf("Début du programme\n");

    // Équivalent du "try" : on enregistre le point de retour
    int code_erreur = setjmp(point_capture);

    if (code_erreur == AUCUNE_ERREUR) {
        // Bloc "try" : code normal
        printf("Exécution du code protégé\n");
        fonction_intermediaire();
        printf("Fin normale du programme\n");
    } else {
        // Bloc "catch" : gestion de l'erreur
        printf("Exception capturée !\n");

        switch (code_erreur) {
            case ERREUR_MEMOIRE:
                fprintf(stderr, "Erreur : allocation mémoire échouée\n");
                break;
            case ERREUR_FICHIER:
                fprintf(stderr, "Erreur : problème de fichier\n");
                break;
            default:
                fprintf(stderr, "Erreur inconnue\n");
        }

        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
```

**Sortie du programme :**
```
Début du programme
Exécution du code protégé
Entrée dans fonction_intermediaire
Erreur détectée, déclenchement de l'exception...
Exception capturée !
Erreur : allocation mémoire échouée
```

#### Fonctionnement de setjmp/longjmp

1. **`setjmp(buffer)`** :
   - Sauvegarde l'état actuel (registres, pile, pointeur d'instruction)
   - Retourne **0** lors de l'appel initial
   - Retourne une **valeur non-nulle** lors d'un retour via `longjmp()`

2. **`longjmp(buffer, valeur)`** :
   - Restaure l'état sauvegardé dans `buffer`
   - Le programme reprend **immédiatement après** le `setjmp()`
   - La valeur passée devient le "code d'erreur"

#### Avantages de setjmp/longjmp

- ✅ **Propagation automatique** : Pas besoin de vérifier chaque fonction
- ✅ **Nettoyage centralisé** : Un seul point de gestion des erreurs
- ✅ **Ressemble aux exceptions** : Familier pour ceux qui connaissent try/catch

#### Inconvénients et dangers ⚠️

- ❌ **Très dangereux** : Peut causer des fuites mémoire et des comportements imprévisibles
- ❌ **Pas de destructeurs automatiques** : Les ressources allouées (malloc, fopen) ne sont PAS libérées automatiquement
- ❌ **Variables locales** : Les variables non-`volatile` peuvent avoir des valeurs indéfinies après longjmp
- ❌ **Difficulté de maintenance** : Le code devient difficile à suivre et déboguer
- ❌ **Non thread-safe** : Problématique dans un contexte multi-thread

#### Exemple de fuite mémoire avec longjmp

```c
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

jmp_buf point_retour;

void fonction_dangereuse(void) {
    char *buffer = malloc(1024);  // Allocation

    if (buffer == NULL) {
        longjmp(point_retour, 1);  // Saut ! Le buffer n'est JAMAIS libéré !
    }

    // ... utilisation du buffer ...
    free(buffer);  // Cette ligne ne sera jamais exécutée
}

int main(void) {
    if (setjmp(point_retour) == 0) {
        fonction_dangereuse();
    } else {
        fprintf(stderr, "Erreur capturée, mais fuite mémoire !\n");
    }

    return 0;
}
```

**🚨 FUITE MÉMOIRE** : Le `malloc` n'est jamais accompagné de son `free`.

#### Règles d'utilisation sûre de setjmp/longjmp

1. **Nettoyer avant de sauter** : Toujours libérer les ressources avant `longjmp()`
2. **Variables volatiles** : Utiliser `volatile` pour les variables locales qui doivent survivre au saut
3. **Contexte limité** : Utiliser uniquement dans des cas très spécifiques (parsers, interpréteurs)
4. **Documentation claire** : Documenter abondamment le flux de contrôle

```c
volatile int variable_protegee = 42;  // Garantit la cohérence après longjmp
```

---

### Technique 2 : Pattern "Goto pour nettoyage"

Une alternative plus sûre et très courante dans le noyau Linux : utiliser `goto` pour centraliser le nettoyage des ressources en cas d'erreur.

#### Exemple : Nettoyage robuste avec goto

```c
#include <stdio.h>
#include <stdlib.h>

int traiter_fichier(const char *nom) {
    FILE *fichier = NULL;
    char *buffer1 = NULL;
    char *buffer2 = NULL;
    int resultat = -1;  // Par défaut : erreur

    // Étape 1 : Ouvrir le fichier
    fichier = fopen(nom, "r");
    if (fichier == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir le fichier\n");
        goto cleanup;  // Saut vers le nettoyage
    }

    // Étape 2 : Allouer le premier buffer
    buffer1 = malloc(1024);
    if (buffer1 == NULL) {
        fprintf(stderr, "Erreur : allocation buffer1\n");
        goto cleanup_fichier;
    }

    // Étape 3 : Allouer le deuxième buffer
    buffer2 = malloc(2048);
    if (buffer2 == NULL) {
        fprintf(stderr, "Erreur : allocation buffer2\n");
        goto cleanup_buffer1;
    }

    // Traitement réussi
    printf("Traitement du fichier réussi\n");
    resultat = 0;  // Succès

    // Nettoyage en cascade (ordre inverse de l'allocation)
    free(buffer2);
cleanup_buffer1:
    free(buffer1);
cleanup_fichier:
    fclose(fichier);
cleanup:
    return resultat;
}

int main(void) {
    int code = traiter_fichier("data.txt");
    return (code == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
```

#### Avantages du pattern goto

- ✅ **Sûr** : Aucun risque de fuite, toutes les ressources sont libérées
- ✅ **Lisible** : Le chemin de nettoyage est clair et linéaire
- ✅ **Prouvé** : Utilisé massivement dans le noyau Linux
- ✅ **Maintenable** : Facile d'ajouter des étapes de nettoyage

#### Principe : Nettoyage en cascade

```
Allocation A → Allocation B → Allocation C → Traitement
                                                  ↓
                                              Succès/Échec
                                                  ↓
                              Libération C ← Libération B ← Libération A
```

---

### Technique 3 : Macros pour simuler try/catch

Certains projets définissent des macros pour créer une syntaxe proche de `try/catch` :

```c
#include <setjmp.h>

// Définition des macros
#define TRY do { jmp_buf ex_buf__; if (!setjmp(ex_buf__)) {
#define CATCH } else {
#define END_TRY } } while(0)
#define THROW longjmp(ex_buf__, 1)

// Utilisation
int main(void) {
    TRY {
        printf("Code protégé\n");

        if (/* condition erreur */) {
            THROW;  // "Lance" l'exception
        }

        printf("Fin normale\n");
    }
    CATCH {
        fprintf(stderr, "Erreur capturée\n");
    }
    END_TRY;

    return 0;
}
```

**⚠️ Attention** : Cette approche hérite de **tous les dangers** de `setjmp/longjmp` et rend le code encore plus opaque.

---

## Comparaison : Codes de retour vs Exceptions simulées

| Critère | Codes de retour | setjmp/longjmp | Goto cleanup |
|---------|-----------------|----------------|--------------|
| **Sécurité** | ⭐⭐⭐⭐⭐ Très sûr | ⭐⭐ Dangereux | ⭐⭐⭐⭐⭐ Très sûr |
| **Lisibilité** | ⭐⭐⭐ Moyen | ⭐⭐ Complexe | ⭐⭐⭐⭐ Bon |
| **Performance** | ⭐⭐⭐⭐⭐ Excellent | ⭐⭐⭐ Bon | ⭐⭐⭐⭐⭐ Excellent |
| **Maintenance** | ⭐⭐⭐⭐ Facile | ⭐⭐ Difficile | ⭐⭐⭐⭐ Facile |
| **Propagation erreur** | ⭐⭐ Manuelle | ⭐⭐⭐⭐⭐ Automatique | ⭐⭐⭐ Semi-auto |
| **Thread-safety** | ⭐⭐⭐⭐⭐ Oui | ⭐ Non | ⭐⭐⭐⭐⭐ Oui |

---

## Recommandations pratiques

### Pour les débutants et projets standards

**Utilisez des codes de retour** avec la convention suivante :

```c
// ✅ Approche recommandée
int fonction(void) {
    if (/* erreur */) {
        return -1;  // ou une enum
    }
    return 0;  // Succès
}

// Vérification systématique
if (fonction() != 0) {
    // Gestion de l'erreur
}
```

### Pour des projets complexes avec beaucoup de nettoyage

**Utilisez le pattern goto** pour centraliser le nettoyage :

```c
int fonction_complexe(void) {
    // ... allocations multiples ...

    // En cas d'erreur
    goto cleanup_etape_N;

cleanup_etape_N:
    // Libérer ressource N
cleanup_etape_N_moins_1:
    // Libérer ressource N-1
    // ...
cleanup:
    return code_erreur;
}
```

### Cas limités pour setjmp/longjmp

Réservez `setjmp/longjmp` à des cas **très spécifiques** :
- Interpréteurs de langage
- Parsers complexes
- Gestion de signaux (avec `sigsetjmp/siglongjmp`)

**⚠️ JAMAIS dans du code métier standard !**

---

## Exemple récapitulatif : Les 3 approches

### Approche 1 : Codes de retour (recommandé)

```c
int ouvrir_base_donnees(const char *chemin) {
    if (chemin == NULL) {
        return -1;
    }

    // Tentative d'ouverture
    if (/* échec */) {
        return -2;
    }

    return 0;  // Succès
}

int main(void) {
    int code = ouvrir_base_donnees("/var/db");

    if (code != 0) {
        fprintf(stderr, "Erreur code : %d\n", code);
        return EXIT_FAILURE;
    }

    printf("Base de données ouverte\n");
    return EXIT_SUCCESS;
}
```

### Approche 2 : Goto cleanup (recommandé pour nettoyage complexe)

```c
int ouvrir_base_donnees(const char *chemin) {
    FILE *fichier = NULL;
    void *buffer = NULL;
    int resultat = -1;

    if (chemin == NULL) {
        goto cleanup;
    }

    fichier = fopen(chemin, "r");
    if (fichier == NULL) {
        goto cleanup;
    }

    buffer = malloc(1024);
    if (buffer == NULL) {
        goto cleanup_fichier;
    }

    // Traitement réussi
    resultat = 0;

    free(buffer);
cleanup_fichier:
    if (fichier) fclose(fichier);
cleanup:
    return resultat;
}
```

### Approche 3 : setjmp/longjmp (déconseillé sauf cas particuliers)

```c
jmp_buf erreur_globale;

void ouvrir_base_donnees(const char *chemin) {
    if (chemin == NULL) {
        longjmp(erreur_globale, 1);  // ⚠️ Dangereux !
    }

    // ... traitement ...
}

int main(void) {
    if (setjmp(erreur_globale) == 0) {
        ouvrir_base_donnees("/var/db");
        printf("Succès\n");
    } else {
        fprintf(stderr, "Erreur capturée\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
```

---

## Conclusion

En C, la gestion d'erreurs robuste repose principalement sur **deux techniques sûres** :

1. **Codes de retour** : Simple, clair, performant → **Usage quotidien**
2. **Goto cleanup** : Nettoyage robuste des ressources → **Projets complexes**

Les exceptions simulées via `setjmp/longjmp` existent mais sont **dangereuses** et doivent être réservées à des cas très spécifiques, toujours avec une extrême prudence.

### Règle d'or 🌟

> **En C, gérer les erreurs explicitement avec des codes de retour est un signe de code professionnel et maintenable.**

Le mantra à retenir :
```c
if (fonction() != 0) {
    // Toujours vérifier les codes de retour !
}
```

---

**Prochaine section** : 25.2 La variable errno

⏭️ [La variable errno](/25-gestion-erreurs/02-errno.md)
