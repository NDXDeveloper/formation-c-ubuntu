🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 25. Gestion des erreurs robuste

## Introduction au chapitre

La **gestion des erreurs** est l'un des aspects les plus cruciaux, et pourtant souvent négligés, de la programmation en C. Contrairement à des langages modernes qui disposent de mécanismes d'exceptions intégrés (try/catch), le C vous laisse une **liberté totale** — et une **responsabilité totale** — dans la manière de détecter, signaler et gérer les erreurs.

Cette liberté est à la fois une force et un piège :
- ✅ **Force** : Contrôle total, pas de surcharge cachée, performances optimales
- ⚠️ **Piège** : Aucune protection automatique, les erreurs ignorées mènent à des bugs silencieux

Dans ce chapitre, nous allons explorer en profondeur les techniques et patterns qui permettent d'écrire du code C **robuste**, capable de gérer élégamment les situations d'erreur et de se comporter de manière prévisible même face à l'imprévu.

---

## Pourquoi la gestion d'erreurs est-elle si importante ?

### 1. La réalité du monde réel

Dans un programme réel, **tout peut échouer** :

```c
// Allocation mémoire : peut échouer si la mémoire est insuffisante
char *buffer = malloc(1024);  // Et si malloc retourne NULL ?

// Ouverture de fichier : peut échouer si le fichier n'existe pas
FILE *f = fopen("config.txt", "r");  // Et si le fichier est absent ?

// Appel système : peut échouer pour diverses raisons
int fd = socket(AF_INET, SOCK_STREAM, 0);  // Et si le réseau est down ?

// Division : peut produire une erreur
int result = a / b;  // Et si b vaut 0 ?
```

**Ignorer ces possibilités d'échec, c'est programmer un crash futur.**

### 2. Les conséquences d'une mauvaise gestion

#### Exemple : Code sans gestion d'erreur

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void traiter_fichier(const char *nom) {
    // ⚠️ DANGER : Aucune vérification d'erreur
    FILE *f = fopen(nom, "r");

    char buffer[100];
    fgets(buffer, sizeof(buffer), f);  // CRASH si f == NULL

    printf("Lu : %s", buffer);

    fclose(f);
}

int main(void) {
    traiter_fichier("inexistant.txt");  // CRASH !
    return 0;
}
```

**Résultat :**
```
Segmentation fault (core dumped)
```

#### Le même code avec gestion d'erreur

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int traiter_fichier(const char *nom) {
    // ✅ Vérification d'erreur
    FILE *f = fopen(nom, "r");
    if (f == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir '%s'\n", nom);
        return -1;
    }

    char buffer[100];
    if (fgets(buffer, sizeof(buffer), f) == NULL) {
        fprintf(stderr, "Erreur : lecture échouée\n");
        fclose(f);
        return -1;
    }

    printf("Lu : %s", buffer);

    fclose(f);
    return 0;
}

int main(void) {
    if (traiter_fichier("inexistant.txt") != 0) {
        fprintf(stderr, "Échec du traitement\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```

**Résultat :**
```
Erreur : impossible d'ouvrir 'inexistant.txt'
Échec du traitement
```

**Le programme se termine proprement avec un message clair.**

### 3. Types de problèmes causés par une gestion d'erreurs insuffisante

| Problème | Cause | Conséquence |
|----------|-------|-------------|
| **Segmentation fault** | Déréférencement de NULL | Crash immédiat |
| **Corruption de données** | Écriture dans une zone invalide | Bugs silencieux, imprévisibles |
| **Fuite mémoire** | Erreur ignorée, `free()` non appelé | Consommation mémoire croissante |
| **Deadlock** | Mutex non libéré après erreur | Programme bloqué |
| **Comportement indéfini** | État incohérent après erreur | Tout peut arriver |
| **Données incorrectes** | Calcul sur valeur d'erreur | Résultats faux |

---

## Les défis de la gestion d'erreurs en C

### 1. Pas de mécanisme d'exception natif

Contrairement à Python, Java, C++ :

```python
# Python : exceptions intégrées
try:
    f = open("data.txt")
    data = f.read()
except FileNotFoundError:
    print("Fichier introuvable")
finally:
    f.close()
```

En C, **tout est manuel** :

```c
// C : Gestion manuelle à chaque étape
FILE *f = fopen("data.txt", "r");  
if (f == NULL) {  
    fprintf(stderr, "Fichier introuvable\n");
    return -1;
}

// ... lecture ...

fclose(f);  // Ne pas oublier !
```

### 2. Rien ne vous oblige à vérifier les erreurs

Le compilateur C ne vous force **pas** à vérifier les codes de retour :

```c
malloc(1024);         // Compilateur : OK ! (mais très dangereux)  
fopen("file.txt", "r");  // Compilateur : OK ! (mais peut crasher)  
```

**C'est votre responsabilité de penser à TOUS les cas d'erreur.**

### 3. Propagation manuelle des erreurs

Les erreurs ne remontent **pas automatiquement** :

```c
int fonction_profonde(void) {
    if (erreur) {
        return -1;  // Comment la fonction appelante le saura-t-elle ?
    }
    return 0;
}

int fonction_intermediaire(void) {
    int resultat = fonction_profonde();
    // Il FAUT vérifier resultat ici
    if (resultat != 0) {
        return -1;  // Et propager l'erreur
    }
    return 0;
}

int main(void) {
    int resultat = fonction_intermediaire();
    // Et encore vérifier ici
    if (resultat != 0) {
        fprintf(stderr, "Erreur détectée\n");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
```

**Chaque niveau doit explicitement vérifier et propager l'erreur.**

### 4. Gestion des ressources complexe

En cas d'erreur, il faut **nettoyer toutes les ressources** déjà allouées :

```c
int fonction_complexe(void) {
    FILE *f = NULL;
    char *buffer = NULL;
    int *tableau = NULL;

    f = fopen("data.txt", "r");
    if (f == NULL) {
        goto cleanup;  // Sauter au nettoyage
    }

    buffer = malloc(1024);
    if (buffer == NULL) {
        goto cleanup;  // Ne pas oublier de fermer f !
    }

    tableau = malloc(100 * sizeof(int));
    if (tableau == NULL) {
        goto cleanup;  // Ne pas oublier f et buffer !
    }

    // ... traitement ...

cleanup:
    free(tableau);
    free(buffer);
    if (f != NULL) fclose(f);
    return (tableau != NULL) ? 0 : -1;
}
```

**Oublier de libérer une ressource = fuite mémoire ou descripteur de fichier.**

---

## Les principes d'une gestion d'erreurs robuste

### 1. Détecter systématiquement

**Vérifiez TOUJOURS** le résultat des opérations qui peuvent échouer :

```c
// ❌ DANGEREUX
char *ptr = malloc(1024);  
ptr[0] = 'A';  // Et si malloc a échoué ?  

// ✅ SÛR
char *ptr = malloc(1024);  
if (ptr == NULL) {  
    fprintf(stderr, "Erreur : allocation échouée\n");
    return -1;
}
ptr[0] = 'A';  // Maintenant c'est sûr
```

### 2. Signaler clairement

Les erreurs doivent être **visibles et informatives** :

```c
// ❌ Message vague
fprintf(stderr, "Erreur\n");

// ✅ Message informatif
fprintf(stderr, "Erreur : impossible d'ouvrir le fichier '%s' : %s\n",
        nom_fichier, strerror(errno));
```

### 3. Gérer ou propager

À chaque niveau, **décidez** :
- **Gérer** : L'erreur peut être résolue localement
- **Propager** : L'erreur doit être remontée à l'appelant

```c
int lire_config(const char *fichier) {
    FILE *f = fopen(fichier, "r");
    if (f == NULL) {
        // Option 1 : Gérer (utiliser des valeurs par défaut)
        fprintf(stderr, "Config absente, utilisation des défauts\n");
        initialiser_config_defaut();
        return 0;

        // Option 2 : Propager (l'appelant doit gérer)
        // return -1;
    }

    // ... lecture ...
    fclose(f);
    return 0;
}
```

### 4. Nettoyer les ressources

**En cas d'erreur, libérez toutes les ressources déjà acquises** :

```c
int fonction(void) {
    void *ptr1 = malloc(100);
    if (ptr1 == NULL) return -1;

    void *ptr2 = malloc(200);
    if (ptr2 == NULL) {
        free(ptr1);  // ✅ Ne pas oublier !
        return -1;
    }

    // ... traitement ...

    free(ptr2);
    free(ptr1);
    return 0;
}
```

### 5. Être cohérent

**Utilisez les mêmes conventions** partout dans votre code :

```c
// Convention choisie : 0 = succès, -1 = erreur
int fonction1(void) {
    if (erreur) return -1;
    return 0;
}

int fonction2(void) {
    if (erreur) return -1;  // ✅ Même convention
    return 0;
}

// ❌ Ne pas mélanger les conventions
int fonction3(void) {
    if (erreur) return 1;  // Incohérent !
    return 0;
}
```

---

## Ce que vous allez apprendre dans ce chapitre

Ce chapitre est divisé en six sections complémentaires qui couvrent tous les aspects de la gestion d'erreurs en C :

### 25.1 Codes de retour vs exceptions simulées
- Les différentes façons de signaler une erreur en C
- Avantages et inconvénients des codes de retour
- Techniques pour simuler des exceptions (setjmp/longjmp)
- Quand utiliser chaque approche

### 25.2 La variable errno
- Comprendre le mécanisme errno
- Les codes d'erreur standards
- Utilisation de strerror() et perror()
- Pièges courants et thread-safety

### 25.3 Patterns de gestion d'erreurs
- Check-and-Return : vérification systématique
- Goto Cleanup : nettoyage centralisé (pattern du kernel Linux)
- Guard Clauses : retours anticipés
- Structure de Retour : informations d'erreur riches
- Wrapper Functions : encapsulation de la gestion d'erreurs
- Context Object : accumulation d'erreurs
- Error Callback : personnalisation du traitement

### 25.4 Assertions
- Différence entre assertions et gestion d'erreurs
- Utilisation de assert() pour détecter les bugs
- static_assert() pour les vérifications à la compilation
- Quand et comment désactiver les assertions (NDEBUG)

### 25.5 Design by contract
- Préconditions : obligations de l'appelant
- Postconditions : obligations de la fonction
- Invariants : propriétés toujours vraies
- Implémentation du DbC en C
- Documentation exécutable

### 25.6 Logging
- Différence entre logging et printf()
- Niveaux de log (DEBUG, INFO, WARNING, ERROR, FATAL)
- Implémentation d'un système de logging
- Logging dans des fichiers
- Thread-safety et bonnes pratiques

---

## L'état d'esprit à adopter

### Pensez "défensif"

Programmez en **anticipant** que tout peut mal tourner :

```c
int traiter_utilisateur(const char *nom, int age) {
    // Validation défensive
    if (nom == NULL) {
        fprintf(stderr, "Erreur : nom NULL\n");
        return -1;
    }

    if (age < 0 || age > 150) {
        fprintf(stderr, "Erreur : âge invalide (%d)\n", age);
        return -1;
    }

    // Maintenant on peut travailler en sécurité
    printf("Utilisateur : %s, %d ans\n", nom, age);
    return 0;
}
```

### "Fail-fast" vs "Fail-safe"

**Fail-fast** : Échouer rapidement et bruyamment
```c
if (condition_critique_invalide) {
    fprintf(stderr, "ERREUR FATALE : état incohérent détecté\n");
    abort();  // Arrêt immédiat
}
```

**Fail-safe** : Continuer avec une dégradation gracieuse
```c
if (fichier_config_absent) {
    fprintf(stderr, "Avertissement : config absente, utilisation des défauts\n");
    utiliser_config_defaut();  // Continuer quand même
}
```

Choisissez la stratégie selon le **contexte** :
- Erreurs critiques (corruption de données) → Fail-fast
- Erreurs récupérables (config manquante) → Fail-safe

### Documentation et contrat

**Documentez** clairement le comportement d'erreur de vos fonctions :

```c
/**
 * Ouvre et lit un fichier de configuration.
 *
 * @param nom_fichier Chemin du fichier à lire
 * @param config Pointeur vers la structure à remplir
 *
 * @return 0 en cas de succès
 *         -1 si le fichier n'existe pas
 *         -2 si le fichier est mal formaté
 *         -3 si l'allocation mémoire échoue
 *
 * @note En cas d'erreur, config n'est pas modifié
 * @note errno est défini en cas d'erreur I/O
 */
int lire_config(const char *nom_fichier, Config *config);
```

---

## Exemple récapitulatif : Avant/Après

### Version 1 : Sans gestion d'erreurs (code fragile)

```c
#include <stdio.h>
#include <stdlib.h>

void traiter_donnees(const char *fichier_entree, const char *fichier_sortie) {
    FILE *entree = fopen(fichier_entree, "r");
    FILE *sortie = fopen(fichier_sortie, "w");

    char *buffer = malloc(1024);

    size_t n = fread(buffer, 1, 1024, entree);
    fwrite(buffer, 1, n, sortie);

    free(buffer);
    fclose(sortie);
    fclose(entree);
}

int main(void) {
    traiter_donnees("input.txt", "output.txt");
    printf("Traitement terminé\n");
    return 0;
}
```

**Problèmes :**
- ❌ Crash si `fopen` échoue (déréférence de NULL)
- ❌ Fuite mémoire si `malloc` échoue
- ❌ Pas de vérification de `fread`/`fwrite`
- ❌ Impossible de savoir si ça a réussi

### Version 2 : Avec gestion d'erreurs robuste

```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int traiter_donnees(const char *fichier_entree, const char *fichier_sortie) {
    FILE *entree = NULL;
    FILE *sortie = NULL;
    char *buffer = NULL;
    int resultat = -1;  // Par défaut : échec

    // Vérification des paramètres
    if (fichier_entree == NULL || fichier_sortie == NULL) {
        fprintf(stderr, "Erreur : paramètres NULL\n");
        goto cleanup;
    }

    // Ouverture fichier d'entrée
    entree = fopen(fichier_entree, "r");
    if (entree == NULL) {
        fprintf(stderr, "Erreur : impossible d'ouvrir '%s' : %s\n",
                fichier_entree, strerror(errno));
        goto cleanup;
    }

    // Ouverture fichier de sortie
    sortie = fopen(fichier_sortie, "w");
    if (sortie == NULL) {
        fprintf(stderr, "Erreur : impossible de créer '%s' : %s\n",
                fichier_sortie, strerror(errno));
        goto cleanup_entree;
    }

    // Allocation buffer
    buffer = malloc(1024);
    if (buffer == NULL) {
        fprintf(stderr, "Erreur : allocation mémoire échouée\n");
        goto cleanup_sortie;
    }

    // Lecture
    size_t n = fread(buffer, 1, 1024, entree);
    if (ferror(entree)) {
        fprintf(stderr, "Erreur : lecture de '%s' échouée\n", fichier_entree);
        goto cleanup_buffer;
    }

    // Écriture
    if (fwrite(buffer, 1, n, sortie) != n) {
        fprintf(stderr, "Erreur : écriture dans '%s' échouée\n", fichier_sortie);
        goto cleanup_buffer;
    }

    // Succès !
    printf("Traitement réussi : %zu octets copiés\n", n);
    resultat = 0;

    // Nettoyage en cascade
cleanup_buffer:
    free(buffer);
cleanup_sortie:
    fclose(sortie);
cleanup_entree:
    fclose(entree);
cleanup:
    return resultat;
}

int main(void) {
    int code = traiter_donnees("input.txt", "output.txt");

    if (code == 0) {
        printf("Programme terminé avec succès\n");
        return EXIT_SUCCESS;
    } else {
        fprintf(stderr, "Programme terminé avec des erreurs\n");
        return EXIT_FAILURE;
    }
}
```

**Avantages :**
- ✅ Toutes les erreurs sont détectées et signalées
- ✅ Aucune fuite mémoire, même en cas d'erreur
- ✅ Messages d'erreur clairs et informatifs
- ✅ Code de retour indique le succès/échec
- ✅ Nettoyage automatique des ressources

---

## Prêt à commencer ?

La gestion d'erreurs robuste peut sembler fastidieuse au début, mais elle devient rapidement une **seconde nature**. Les avantages sont immenses :

- 🛡️ **Stabilité** : Votre programme ne crashe plus de manière imprévisible
- 🔍 **Débogage** : Les erreurs sont détectées tôt avec des messages clairs
- 📊 **Fiabilité** : Comportement prévisible dans toutes les situations
- 🏆 **Professionnalisme** : Code de qualité production

Les sections suivantes vous fourniront tous les outils et techniques nécessaires pour écrire du code C **robuste** et **maintenable**. Chaque pattern et technique présentée a été éprouvée dans des millions de lignes de code en production.

**N'oubliez jamais** : En C, la gestion d'erreurs n'est pas optionnelle — c'est la différence entre un programme qui fonctionne et un programme qui crashe.

---

**Commençons maintenant par explorer les différentes façons de signaler et gérer les erreurs en C →**

⏭️ [Codes de retour vs exceptions simulées](/25-gestion-erreurs/01-codes-retour-vs-exceptions.md)
