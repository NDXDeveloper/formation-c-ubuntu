🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 25.2 La variable errno

## Introduction

La variable `errno` est un mécanisme **fondamental** en C pour obtenir des informations détaillées sur les erreurs qui se produisent lors de l'exécution d'appels système ou de fonctions de la bibliothèque standard. C'est l'équivalent d'un "code d'erreur global" qui permet de savoir **pourquoi** une opération a échoué.

---

## Qu'est-ce que errno ?

### Définition

`errno` est une **variable globale** (ou plutôt une macro qui pointe vers une variable) définie dans `<errno.h>`. Elle contient un code numérique représentant la dernière erreur survenue.

```c
#include <errno.h>

// errno est une variable entière
// Chaque valeur correspond à un type d'erreur spécifique
```

### Principe de fonctionnement

1. Une fonction de la bibliothèque standard ou un appel système **échoue**
2. La fonction définit `errno` avec un **code d'erreur spécifique**
3. La fonction retourne une **valeur d'échec** (généralement -1, NULL, ou EOF)
4. Le programmeur peut alors **consulter errno** pour connaître la cause de l'échec

```c
FILE *fichier = fopen("inexistant.txt", "r");

if (fichier == NULL) {
    // fopen a échoué, errno contient maintenant le code d'erreur
    printf("Erreur numéro : %d\n", errno);
}
```

---

## Exemple introductif

```c
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(void) {
    FILE *fichier = fopen("fichier_inexistant.txt", "r");

    if (fichier == NULL) {
        printf("Erreur lors de l'ouverture du fichier\n");
        printf("Code errno : %d\n", errno);
        printf("Signification : %s\n", strerror(errno));
    } else {
        printf("Fichier ouvert avec succès\n");
        fclose(fichier);
    }

    return 0;
}
```

**Sortie possible :**
```
Erreur lors de l'ouverture du fichier  
Code errno : 2  
Signification : No such file or directory  
```

---

## Les codes d'erreur courants

Les codes d'erreur sont définis par des **constantes symboliques** dans `<errno.h>`. Voici les plus fréquemment rencontrés :

### Tableau des erreurs essentielles

| Constante | Valeur typique | Signification | Contexte courant |
|-----------|----------------|---------------|------------------|
| `EACCES` | 13 | Permission refusée | Fichiers, sockets |
| `ENOENT` | 2 | Fichier/répertoire inexistant | fopen, open |
| `ENOMEM` | 12 | Mémoire insuffisante | malloc, mmap |
| `EINVAL` | 22 | Argument invalide | Paramètres incorrects |
| `EBADF` | 9 | Descripteur de fichier invalide | read, write, close |
| `EEXIST` | 17 | Fichier existe déjà | Création avec O_EXCL |
| `EISDIR` | 21 | C'est un répertoire | Tentative d'ouvrir un dossier comme fichier |
| `ENOTDIR` | 20 | N'est pas un répertoire | Chemin attendu comme dossier |
| `EMFILE` | 24 | Trop de fichiers ouverts | Limite processus atteinte |
| `ENOSPC` | 28 | Plus d'espace disque | Écriture sur disque plein |
| `EPIPE` | 32 | Pipe cassé | Écriture dans un pipe fermé |
| `EAGAIN` | 11 | Ressource temporairement indisponible | I/O non-bloquant |
| `EINTR` | 4 | Appel système interrompu | Interruption par signal |
| `EWOULDBLOCK` | 11 (souvent = EAGAIN) | Opération bloquerait | I/O non-bloquant |

### Exemple avec plusieurs codes d'erreur

```c
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int main(void) {
    int fd = open("/etc/shadow", O_RDONLY);  // Fichier protégé

    if (fd == -1) {
        switch (errno) {
            case EACCES:
                fprintf(stderr, "Erreur : Permission refusée\n");
                break;
            case ENOENT:
                fprintf(stderr, "Erreur : Fichier inexistant\n");
                break;
            case EISDIR:
                fprintf(stderr, "Erreur : C'est un répertoire\n");
                break;
            default:
                fprintf(stderr, "Erreur inconnue : %s\n", strerror(errno));
        }
        return 1;
    }

    printf("Fichier ouvert avec succès\n");
    close(fd);
    return 0;
}
```

**Sortie attendue (sans droits root) :**
```
Erreur : Permission refusée
```

---

## Fonctions pour exploiter errno

### 1. strerror() : Obtenir un message lisible

La fonction `strerror()` convertit un code errno en **chaîne de caractères descriptive** :

```c
#include <string.h>

char *strerror(int errnum);
```

**Exemple :**

```c
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(void) {
    printf("ENOENT : %s\n", strerror(ENOENT));
    printf("EACCES : %s\n", strerror(EACCES));
    printf("ENOMEM : %s\n", strerror(ENOMEM));

    return 0;
}
```

**Sortie :**
```
ENOENT : No such file or directory  
EACCES : Permission denied  
ENOMEM : Cannot allocate memory  
```

### 2. perror() : Afficher l'erreur directement

La fonction `perror()` affiche automatiquement un **message d'erreur sur stderr** en utilisant errno :

```c
#include <stdio.h>

void perror(const char *s);
```

**Comportement :**
- Affiche `s` suivi de `: ` puis du message d'erreur correspondant à errno
- Écrit sur **stderr** (flux d'erreur standard)

**Exemple :**

```c
#include <stdio.h>
#include <errno.h>

int main(void) {
    FILE *fichier = fopen("inexistant.txt", "r");

    if (fichier == NULL) {
        perror("Erreur fopen");  // Affiche "Erreur fopen: No such file or directory"
        return 1;
    }

    fclose(fichier);
    return 0;
}
```

**Sortie sur stderr :**
```
Erreur fopen: No such file or directory
```

### Comparaison strerror vs perror

```c
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(void) {
    FILE *fichier = fopen("test.txt", "r");

    if (fichier == NULL) {
        // Méthode 1 : strerror (manuel)
        fprintf(stderr, "Erreur : %s\n", strerror(errno));

        // Méthode 2 : perror (automatique)
        perror("Erreur");
    }

    return 0;
}
```

**Sortie identique :**
```
Erreur : No such file or directory  
Erreur: No such file or directory  
```

**💡 Conseil :** Utilisez `perror()` pour un code plus concis, `strerror()` si vous avez besoin de formatter le message autrement.

---

## Utilisation correcte de errno

### Règle fondamentale : Vérifier APRÈS l'échec

**⚠️ IMPORTANT** : `errno` n'est défini que si une fonction **échoue**. Ne consultez jamais errno sans avoir vérifié le code de retour de la fonction !

#### ❌ Mauvaise pratique

```c
FILE *fichier = fopen("data.txt", "r");

// ERREUR : On ne sait pas si fopen a échoué !
if (errno != 0) {
    printf("Erreur : %s\n", strerror(errno));
}
```

#### ✅ Bonne pratique

```c
FILE *fichier = fopen("data.txt", "r");

if (fichier == NULL) {  // D'abord vérifier l'échec
    // errno est maintenant fiable
    printf("Erreur : %s\n", strerror(errno));
}
```

### Règle 2 : errno n'est PAS réinitialisé automatiquement

Les fonctions qui **réussissent** ne remettent généralement **pas errno à zéro**. Il garde donc sa valeur précédente.

```c
#include <stdio.h>
#include <errno.h>
#include <string.h>

int main(void) {
    // Première opération : échec
    FILE *f1 = fopen("inexistant.txt", "r");
    if (f1 == NULL) {
        printf("Erreur 1 : %s\n", strerror(errno));  // errno = ENOENT
    }

    // Deuxième opération : succès
    FILE *f2 = fopen("/etc/hosts", "r");
    if (f2 != NULL) {
        printf("Fichier ouvert avec succès\n");
        // errno contient TOUJOURS la valeur ENOENT de l'échec précédent !
        printf("errno vaut : %d (%s)\n", errno, strerror(errno));
        fclose(f2);
    }

    return 0;
}
```

**Sortie :**
```
Erreur 1 : No such file or directory  
Fichier ouvert avec succès  
errno vaut : 2 (No such file or directory)  
```

**💡 Leçon** : Ne consultez errno que **immédiatement après** avoir détecté un échec.

### Règle 3 : Sauvegarder errno si nécessaire

Si vous devez appeler d'autres fonctions avant de traiter l'erreur, **sauvegardez errno** :

```c
#include <stdio.h>
#include <errno.h>
#include <string.h>

void log_erreur(const char *message) {
    // Cette fonction pourrait modifier errno
    fprintf(stderr, "[LOG] %s\n", message);
}

int main(void) {
    FILE *fichier = fopen("inexistant.txt", "r");

    if (fichier == NULL) {
        int erreur_sauvegardee = errno;  // ✅ Sauvegarde

        log_erreur("Tentative d'ouverture échouée");

        // Utilisation de la valeur sauvegardée
        fprintf(stderr, "Code erreur : %s\n", strerror(erreur_sauvegardee));
    }

    return 0;
}
```

---

## Gestion d'erreurs robuste avec errno

### Pattern recommandé

Voici le pattern complet pour gérer proprement les erreurs avec errno :

```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int ouvrir_fichier(const char *chemin, FILE **fichier_out) {
    *fichier_out = fopen(chemin, "r");

    if (*fichier_out == NULL) {
        // Sauvegarde immédiate de errno
        int erreur = errno;

        // Message d'erreur personnalisé selon le code
        switch (erreur) {
            case ENOENT:
                fprintf(stderr, "Le fichier '%s' n'existe pas\n", chemin);
                break;
            case EACCES:
                fprintf(stderr, "Permission refusée pour '%s'\n", chemin);
                break;
            default:
                fprintf(stderr, "Impossible d'ouvrir '%s': %s\n",
                        chemin, strerror(erreur));
        }

        return -1;  // Échec
    }

    return 0;  // Succès
}

int main(void) {
    FILE *fichier;

    if (ouvrir_fichier("data.txt", &fichier) != 0) {
        return EXIT_FAILURE;
    }

    printf("Fichier ouvert avec succès\n");

    // Utilisation du fichier...

    fclose(fichier);
    return EXIT_SUCCESS;
}
```

### Exemple avec appels système (open/read)

```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
    int fd = open("data.bin", O_RDONLY);

    if (fd == -1) {
        perror("Erreur open");
        return EXIT_FAILURE;
    }

    char buffer[100];
    ssize_t octets_lus = read(fd, buffer, sizeof(buffer));

    if (octets_lus == -1) {
        int erreur = errno;  // Sauvegarde
        close(fd);            // Nettoyage

        fprintf(stderr, "Erreur read: %s\n", strerror(erreur));
        return EXIT_FAILURE;
    }

    printf("Lu %zd octets\n", octets_lus);

    close(fd);
    return EXIT_SUCCESS;
}
```

---

## errno et les threads (Thread-safety)

### Le problème historique

Dans les anciennes implémentations, `errno` était une simple variable globale. Dans un programme **multi-thread**, cela posait un problème majeur :

```c
// Thread 1
FILE *f1 = fopen("file1.txt", "r");  // Échoue, errno = 2
// <-- Changement de contexte -->

// Thread 2
FILE *f2 = fopen("file2.txt", "r");  // Échoue, errno = 13

// Thread 1 reprend
if (f1 == NULL) {
    printf("Erreur : %d\n", errno);  // 13 au lieu de 2 ! ❌
}
```

### Solution moderne : Thread-Local Storage (TLS)

Sur les systèmes modernes (POSIX.1-2001 et ultérieurs), `errno` est défini comme une **macro** qui accède à une variable **locale au thread** :

```c
// Implémentation simplifiée moderne
#define errno (*__errno_location())

// Chaque thread a son propre errno
```

**✅ Résultat** : Chaque thread possède sa **propre copie** de errno, éliminant les conflits.

### Vérifier le support TLS

```c
#include <stdio.h>
#include <errno.h>

int main(void) {
    #ifdef _REENTRANT
        printf("errno est thread-safe sur ce système\n");
    #else
        printf("Attention : errno pourrait ne pas être thread-safe\n");
    #endif

    return 0;
}
```

**💡 Sur Linux/Ubuntu moderne** : errno est **toujours thread-safe**, pas d'inquiétude.

---

## Pièges courants et comment les éviter

### Piège 1 : Vérifier errno sans échec

```c
// ❌ FAUX
if (errno == ENOENT) {
    printf("Fichier inexistant\n");
}

// ✅ CORRECT
FILE *f = fopen("data.txt", "r");  
if (f == NULL && errno == ENOENT) {  
    printf("Fichier inexistant\n");
}
```

### Piège 2 : Ne pas sauvegarder errno

```c
// ❌ FAUX
if (fopen("test.txt", "r") == NULL) {
    fprintf(stderr, "Erreur : ");  // fprintf peut modifier errno !
    perror("fopen");                // errno potentiellement modifié
}

// ✅ CORRECT
if (fopen("test.txt", "r") == NULL) {
    int err = errno;
    fprintf(stderr, "Erreur : ");
    fprintf(stderr, "%s\n", strerror(err));
}
```

### Piège 3 : Réutiliser errno après un succès

```c
// ❌ FAUX
FILE *f1 = fopen("inexistant.txt", "r");  // errno = ENOENT  
FILE *f2 = fopen("/etc/hosts", "r");      // Succès, errno inchangé  

if (f2 != NULL) {
    // errno contient TOUJOURS ENOENT !
    printf("errno = %d\n", errno);  // Valeur obsolète
}

// ✅ CORRECT
FILE *f1 = fopen("inexistant.txt", "r");  
if (f1 == NULL) {  
    printf("Erreur : %s\n", strerror(errno));
}

FILE *f2 = fopen("/etc/hosts", "r");  
if (f2 == NULL) {  // Vérifier l'échec PUIS consulter errno  
    printf("Erreur : %s\n", strerror(errno));
} else {
    printf("Succès\n");  // Ne pas consulter errno
}
```

### Piège 4 : Confusion entre codes de retour et errno

Certaines fonctions ne définissent **pas** errno :

```c
#include <stdlib.h>

// malloc retourne NULL en cas d'échec mais ne définit PAS toujours errno
void *ptr = malloc(1000000000000);

if (ptr == NULL) {
    // ❌ FAUX : errno n'est pas garanti d'être défini
    printf("Erreur malloc : %s\n", strerror(errno));

    // ✅ CORRECT : Message générique
    fprintf(stderr, "Erreur : allocation échouée\n");
}
```

**💡 Conseil** : Consultez toujours la **documentation** (man page) pour savoir si une fonction utilise errno.

---

## Fonctions qui utilisent errno

### Appels système (man section 2)

Presque **tous** les appels système définissent errno en cas d'erreur :

```c
open(), read(), write(), close()  
fork(), exec(), wait()  
socket(), bind(), connect(), accept()  
pipe(), mmap(), munmap()  
```

### Fonctions de la bibliothèque standard (man section 3)

De nombreuses fonctions de la **libc** définissent errno :

```c
fopen(), fread(), fwrite(), fclose()  
malloc(), calloc(), realloc()  // Parfois seulement  
strtol(), strtod()              // Pour détecter les overflows  
```

### Vérifier dans la documentation

```bash
# Voir si une fonction utilise errno
man 3 fopen

# Section RETURN VALUE indique :
# "Upon successful completion, fopen() returns a FILE pointer.
#  Otherwise, NULL is returned and errno is set to indicate the error."
```

---

## Exemple complet : Gestionnaire d'erreurs robuste

Voici un exemple complet combinant toutes les bonnes pratiques :

```c
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

// Fonction utilitaire pour afficher les erreurs
void afficher_erreur(const char *fonction, const char *fichier) {
    int erreur = errno;  // Sauvegarde immédiate

    fprintf(stderr, "Erreur dans %s pour '%s': ", fonction, fichier);

    switch (erreur) {
        case ENOENT:
            fprintf(stderr, "Fichier inexistant\n");
            break;
        case EACCES:
            fprintf(stderr, "Permission refusée\n");
            break;
        case EISDIR:
            fprintf(stderr, "C'est un répertoire\n");
            break;
        case ENOMEM:
            fprintf(stderr, "Mémoire insuffisante\n");
            break;
        default:
            fprintf(stderr, "%s (code %d)\n", strerror(erreur), erreur);
    }
}

int copier_fichier(const char *source, const char *destination) {
    int fd_src = -1, fd_dest = -1;
    int resultat = -1;
    char buffer[4096];
    ssize_t octets_lus, octets_ecrits;

    // Ouverture du fichier source
    fd_src = open(source, O_RDONLY);
    if (fd_src == -1) {
        afficher_erreur("open (source)", source);
        goto cleanup;
    }

    // Création du fichier destination
    fd_dest = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_dest == -1) {
        afficher_erreur("open (destination)", destination);
        goto cleanup;
    }

    // Copie des données
    while ((octets_lus = read(fd_src, buffer, sizeof(buffer))) > 0) {
        octets_ecrits = write(fd_dest, buffer, (size_t)octets_lus);

        if (octets_ecrits == -1) {
            afficher_erreur("write", destination);
            goto cleanup;
        }

        if (octets_ecrits != octets_lus) {
            fprintf(stderr, "Erreur : écriture partielle\n");
            goto cleanup;
        }
    }

    // Vérifier si la lecture s'est terminée par une erreur
    if (octets_lus == -1) {
        afficher_erreur("read", source);
        goto cleanup;
    }

    printf("Copie réussie : %s -> %s\n", source, destination);
    resultat = 0;  // Succès

cleanup:
    if (fd_src != -1) close(fd_src);
    if (fd_dest != -1) close(fd_dest);

    return resultat;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage : %s <source> <destination>\n", argv[0]);
        return EXIT_FAILURE;
    }

    int code = copier_fichier(argv[1], argv[2]);

    return (code == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
```

**Utilisation :**

```bash
# Copie réussie
$ ./copie /etc/hosts hosts_copie.txt
Copie réussie : /etc/hosts -> hosts_copie.txt

# Fichier source inexistant
$ ./copie inexistant.txt dest.txt
Erreur dans open (source) pour 'inexistant.txt': Fichier inexistant

# Permission refusée
$ ./copie /etc/shadow dest.txt
Erreur dans open (source) pour '/etc/shadow': Permission refusée
```

---

## Alternatives et compléments à errno

### 1. Fonctions avec codes d'erreur dédiés

Certaines fonctions modernes (POSIX threads) retournent directement le code d'erreur au lieu d'utiliser errno :

```c
#include <pthread.h>

pthread_mutex_t mutex;

int code = pthread_mutex_init(&mutex, NULL);

if (code != 0) {
    // code contient directement le code d'erreur (pas errno)
    fprintf(stderr, "Erreur pthread : %s\n", strerror(code));
}
```

### 2. getaddrinfo et gai_strerror

Les fonctions réseau comme `getaddrinfo()` ont leur propre système d'erreurs :

```c
#include <netdb.h>

struct addrinfo *result;  
int code = getaddrinfo("example.com", "80", NULL, &result);  

if (code != 0) {
    // N'utilisez PAS errno, mais gai_strerror
    fprintf(stderr, "Erreur : %s\n", gai_strerror(code));
}
```

---

## Résumé des bonnes pratiques

### ✅ À FAIRE

1. **Toujours vérifier le code de retour** avant de consulter errno
2. **Sauvegarder errno** si vous appelez d'autres fonctions ensuite
3. **Utiliser perror()** ou **strerror()** pour des messages lisibles
4. **Consulter la man page** pour savoir si une fonction utilise errno
5. **Gérer les erreurs immédiatement** après leur détection

### ❌ À ÉVITER

1. ❌ Consulter errno sans avoir vérifié l'échec de la fonction
2. ❌ Supposer que errno vaut 0 en cas de succès
3. ❌ Ignorer errno après un échec
4. ❌ Modifier errno dans vos propres fonctions (sauf cas très spécifiques)
5. ❌ Utiliser errno pour des fonctions qui ne le définissent pas

---

## Checklist pour gérer errno

Avant de quitter une fonction après une erreur :

```c
int ma_fonction(void) {
    FILE *f = fopen("data.txt", "r");

    if (f == NULL) {
        // ✅ 1. Sauvegarder errno immédiatement
        int erreur = errno;

        // ✅ 2. Nettoyer les ressources déjà allouées
        // (ici aucune)

        // ✅ 3. Logger l'erreur de manière informative
        fprintf(stderr, "Impossible d'ouvrir data.txt: %s\n",
                strerror(erreur));

        // ✅ 4. Retourner un code d'erreur
        return -1;
    }

    // ... code normal ...

    fclose(f);
    return 0;
}
```

---

## Conclusion

La variable `errno` est un mécanisme **essentiel** mais **délicat** pour la gestion d'erreurs en C. Retenez ces points clés :

### 🔑 Points essentiels

1. **errno n'est fiable qu'après un échec** vérifié par le code de retour
2. **Sauvegardez errno** si vous devez appeler d'autres fonctions
3. **Utilisez strerror() ou perror()** pour des messages lisibles
4. **Consultez la documentation** pour savoir si une fonction utilise errno
5. **Sur Linux moderne, errno est thread-safe** grâce au TLS

### 🌟 Règle d'or

> **Vérifiez d'abord le code de retour, consultez ensuite errno.**

```c
if (fonction() == VALEUR_ECHEC) {
    // Maintenant seulement, errno est fiable
    fprintf(stderr, "Erreur : %s\n", strerror(errno));
}
```

La maîtrise de errno est indispensable pour écrire du **code C robuste** capable de gérer élégamment les erreurs système et bibliothèque.

---

**Prochaine section** : 25.3 Patterns de gestion d'erreurs

⏭️ [Patterns de gestion d'erreurs](/25-gestion-erreurs/03-patterns-gestion-erreurs.md)
