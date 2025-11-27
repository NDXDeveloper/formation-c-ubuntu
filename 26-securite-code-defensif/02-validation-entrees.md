🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 26.2 Validation des entrées

## Introduction

La **validation des entrées** est l'un des principes fondamentaux de la sécurité logicielle. Elle repose sur une règle simple mais cruciale :

> **"Ne jamais faire confiance aux données provenant de l'extérieur"**

Toute donnée qui n'a pas été créée directement par votre programme doit être considérée comme potentiellement dangereuse jusqu'à preuve du contraire.

### Pourquoi valider les entrées ?

Sans validation appropriée, votre programme peut :
- **Crasher** de manière inattendue
- **Corrompre** des données importantes
- Être **exploité** par des attaquants pour exécuter du code malveillant
- **Exposer** des informations sensibles
- Causer des **pertes financières** ou des **problèmes de sécurité**

### Principe de base

```
Données externes → VALIDATION → Traitement sécurisé
                         ↓
                   (Rejet si invalide)
```

---

## Qu'est-ce qu'une "entrée" ?

Une entrée est toute donnée qui vient de l'extérieur de votre programme. Voici les sources principales :

### 1. **Entrées utilisateur directes**
```c
// Via stdin
char nom[50];
scanf("%s", nom);  // ⚠️ Entrée utilisateur

// Via arguments de ligne de commande
int main(int argc, char *argv[]) {
    // argv[1], argv[2], etc. sont des entrées utilisateur
}
```

### 2. **Fichiers**
```c
FILE *fichier = fopen("config.txt", "r");
char ligne[256];
fgets(ligne, sizeof(ligne), fichier);  // ⚠️ Contenu du fichier
```

### 3. **Réseau**
```c
char buffer[1024];
recv(socket_fd, buffer, sizeof(buffer), 0);  // ⚠️ Données réseau
```

### 4. **Variables d'environnement**
```c
char *chemin = getenv("PATH");  // ⚠️ Variable d'environnement
```

### 5. **Bases de données et APIs externes**
```c
// Résultats de requêtes SQL, réponses HTTP, etc.
```

**Règle d'or** : Toutes ces sources doivent être validées !

---

## Les types de validation

### 1. Validation de type (Type Checking)

Vérifier que la donnée est du type attendu.

#### Exemple : Conversion chaîne → entier

```c
// ❌ Mauvais : pas de validation
void traiter_age(const char *entree) {
    int age = atoi(entree);  // atoi retourne 0 en cas d'erreur !
    printf("Vous avez %d ans\n", age);
}

// Si l'utilisateur entre "abc", age vaudra 0
// Si l'utilisateur entre "25abc", age vaudra 25 (conversion partielle)
```

```c
// ✅ Bon : validation avec strtol
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

bool lire_age(const char *entree, int *age) {
    char *fin;
    errno = 0;

    long valeur = strtol(entree, &fin, 10);

    // Vérifications multiples
    if (errno == ERANGE) {
        fprintf(stderr, "Erreur : nombre trop grand ou trop petit\n");
        return false;
    }

    if (fin == entree) {
        fprintf(stderr, "Erreur : aucun chiffre trouvé\n");
        return false;
    }

    if (*fin != '\0' && *fin != '\n') {
        fprintf(stderr, "Erreur : caractères invalides après le nombre\n");
        return false;
    }

    if (valeur < INT_MIN || valeur > INT_MAX) {
        fprintf(stderr, "Erreur : valeur hors limites pour un int\n");
        return false;
    }

    *age = (int)valeur;
    return true;
}

// Utilisation
int main(void) {
    char entree[100];
    printf("Entrez votre âge : ");

    if (fgets(entree, sizeof(entree), stdin) == NULL) {
        return 1;
    }

    int age;
    if (lire_age(entree, &age)) {
        printf("Vous avez %d ans\n", age);
    } else {
        printf("Âge invalide\n");
    }

    return 0;
}
```

### 2. Validation de plage (Range Checking)

Vérifier que la valeur se situe dans un intervalle acceptable.

```c
// ❌ Mauvais : pas de vérification de plage
void definir_volume(int volume) {
    set_volume_hardware(volume);  // Que se passe-t-il si volume = -1000 ?
}
```

```c
// ✅ Bon : validation de plage
bool definir_volume(int volume) {
    // Le volume doit être entre 0 et 100
    if (volume < 0 || volume > 100) {
        fprintf(stderr, "Erreur : volume doit être entre 0 et 100\n");
        return false;
    }

    set_volume_hardware(volume);
    return true;
}
```

#### Validation de plage pour les indices de tableau

```c
// ❌ Mauvais
int tableau[10];
void acceder_element(int index) {
    printf("%d\n", tableau[index]);  // Danger !
}
```

```c
// ✅ Bon
int tableau[10];
bool acceder_element(int index) {
    if (index < 0 || index >= 10) {
        fprintf(stderr, "Erreur : index hors limites\n");
        return false;
    }

    printf("%d\n", tableau[index]);
    return true;
}
```

### 3. Validation de longueur (Length Checking)

Critique pour les chaînes de caractères et les buffers.

```c
// ❌ TRÈS DANGEREUX : buffer overflow
void copier_nom(const char *nom_utilisateur) {
    char buffer[32];
    strcpy(buffer, nom_utilisateur);  // Pas de vérification de longueur !
    printf("Nom : %s\n", buffer);
}

// Si nom_utilisateur fait 100 caractères → débordement de buffer
```

```c
// ✅ Bon : validation de longueur
bool copier_nom(const char *nom_utilisateur) {
    char buffer[32];
    size_t longueur = strlen(nom_utilisateur);

    // Vérification : place pour la chaîne + '\0'
    if (longueur >= sizeof(buffer)) {
        fprintf(stderr, "Erreur : nom trop long (max %zu caractères)\n",
                sizeof(buffer) - 1);
        return false;
    }

    strcpy(buffer, nom_utilisateur);  // Maintenant sûr
    printf("Nom : %s\n", buffer);
    return true;
}
```

```c
// ✅ Encore mieux : utiliser strncpy de manière sûre
bool copier_nom_securise(const char *nom_utilisateur) {
    char buffer[32];

    strncpy(buffer, nom_utilisateur, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0';  // Garantit la terminaison null

    // Vérifier si la chaîne a été tronquée
    if (strlen(nom_utilisateur) >= sizeof(buffer)) {
        fprintf(stderr, "Attention : nom tronqué à %zu caractères\n",
                sizeof(buffer) - 1);
    }

    printf("Nom : %s\n", buffer);
    return true;
}
```

### 4. Validation de format (Format Checking)

Vérifier que la structure de la donnée respecte un format attendu.

#### Exemple : Validation d'une adresse email

```c
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

bool valider_email(const char *email) {
    if (email == NULL || *email == '\0') {
        return false;
    }

    size_t longueur = strlen(email);
    if (longueur < 3 || longueur > 254) {  // RFC 5321
        return false;
    }

    // Rechercher le '@'
    const char *arobase = strchr(email, '@');
    if (arobase == NULL) {
        return false;  // Pas de '@'
    }

    if (arobase == email) {
        return false;  // '@' au début
    }

    if (arobase == email + longueur - 1) {
        return false;  // '@' à la fin
    }

    // Vérifier qu'il n'y a qu'un seul '@'
    if (strchr(arobase + 1, '@') != NULL) {
        return false;  // Plusieurs '@'
    }

    // Vérifier la présence d'un point après le '@'
    const char *point = strchr(arobase + 1, '.');
    if (point == NULL || point == arobase + 1 || point == email + longueur - 1) {
        return false;
    }

    return true;
}

// Utilisation
if (valider_email("utilisateur@example.com")) {
    printf("Email valide\n");
} else {
    printf("Email invalide\n");
}
```

#### Exemple : Validation d'un nom de fichier

```c
bool valider_nom_fichier(const char *nom) {
    if (nom == NULL || *nom == '\0') {
        return false;
    }

    size_t longueur = strlen(nom);

    // Longueur raisonnable (dépend du système)
    if (longueur > 255) {
        fprintf(stderr, "Erreur : nom de fichier trop long\n");
        return false;
    }

    // Vérifier les caractères dangereux
    const char *caracteres_interdits = "/<>:\"|?*\\";

    for (size_t i = 0; i < longueur; i++) {
        char c = nom[i];

        // Caractères de contrôle
        if (iscntrl((unsigned char)c)) {
            fprintf(stderr, "Erreur : caractère de contrôle interdit\n");
            return false;
        }

        // Caractères spéciaux dangereux
        if (strchr(caracteres_interdits, c) != NULL) {
            fprintf(stderr, "Erreur : caractère '%c' interdit\n", c);
            return false;
        }
    }

    // Noms réservés sur Windows
    const char *noms_reserves[] = {
        "CON", "PRN", "AUX", "NUL",
        "COM1", "COM2", "COM3", "COM4",
        "LPT1", "LPT2", "LPT3"
    };

    for (size_t i = 0; i < sizeof(noms_reserves) / sizeof(noms_reserves[0]); i++) {
        if (strcasecmp(nom, noms_reserves[i]) == 0) {
            fprintf(stderr, "Erreur : nom réservé '%s'\n", noms_reserves[i]);
            return false;
        }
    }

    return true;
}
```

### 5. Validation sémantique (Semantic Checking)

Vérifier que la donnée a un sens dans le contexte.

```c
// Structure pour une date
typedef struct {
    int jour;
    int mois;
    int annee;
} Date;

bool valider_date(const Date *date) {
    if (date == NULL) {
        return false;
    }

    // Validation de l'année
    if (date->annee < 1900 || date->annee > 2100) {
        fprintf(stderr, "Erreur : année invalide\n");
        return false;
    }

    // Validation du mois
    if (date->mois < 1 || date->mois > 12) {
        fprintf(stderr, "Erreur : mois invalide\n");
        return false;
    }

    // Jours par mois
    int jours_par_mois[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Année bissextile
    bool bissextile = (date->annee % 4 == 0 && date->annee % 100 != 0) ||
                      (date->annee % 400 == 0);

    if (bissextile && date->mois == 2) {
        jours_par_mois[1] = 29;
    }

    // Validation du jour
    if (date->jour < 1 || date->jour > jours_par_mois[date->mois - 1]) {
        fprintf(stderr, "Erreur : jour invalide pour ce mois\n");
        return false;
    }

    return true;
}

// Utilisation
Date ma_date = {29, 2, 2024};  // 29 février 2024 (année bissextile)
if (valider_date(&ma_date)) {
    printf("Date valide\n");
} else {
    printf("Date invalide\n");
}
```

---

## Validation des entrées utilisateur avec scanf

`scanf` est particulièrement dangereux sans précautions.

### Problèmes de scanf

```c
// ❌ TRÈS DANGEREUX
char nom[20];
printf("Entrez votre nom : ");
scanf("%s", nom);  // Buffer overflow si > 19 caractères !
```

### Solutions sécurisées

```c
// ✅ Solution 1 : Limiter la longueur
char nom[20];
printf("Entrez votre nom : ");
scanf("%19s", nom);  // Limite à 19 caractères (+ '\0')

// Vider le buffer si nécessaire
int c;
while ((c = getchar()) != '\n' && c != EOF);
```

```c
// ✅ Solution 2 (préférable) : Utiliser fgets
char nom[20];
printf("Entrez votre nom : ");

if (fgets(nom, sizeof(nom), stdin) != NULL) {
    // Retirer le '\n' final si présent
    size_t longueur = strlen(nom);
    if (longueur > 0 && nom[longueur - 1] == '\n') {
        nom[longueur - 1] = '\0';
    }

    printf("Bonjour %s\n", nom);
}
```

### Validation des nombres avec scanf

```c
// ❌ Mauvais : pas de vérification d'erreur
int age;
printf("Entrez votre âge : ");
scanf("%d", &age);  // Que se passe-t-il si l'utilisateur tape "abc" ?
```

```c
// ✅ Bon : vérification complète
int age;
printf("Entrez votre âge : ");

int resultat = scanf("%d", &age);

if (resultat != 1) {
    fprintf(stderr, "Erreur : entrée invalide\n");

    // Vider le buffer
    int c;
    while ((c = getchar()) != '\n' && c != EOF);

    return false;
}

// Validation de plage
if (age < 0 || age > 150) {
    fprintf(stderr, "Erreur : âge non réaliste\n");
    return false;
}

printf("Vous avez %d ans\n", age);
```

---

## Validation des arguments de ligne de commande

Les arguments passés au programme doivent toujours être validés.

```c
// ❌ Mauvais : utilisation directe
int main(int argc, char *argv[]) {
    int port = atoi(argv[1]);  // Crash si argc < 2 !
    demarrer_serveur(port);
    return 0;
}
```

```c
// ✅ Bon : validation complète
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    // Vérifier le nombre d'arguments
    if (argc != 2) {
        fprintf(stderr, "Usage : %s <port>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Convertir et valider le port
    char *fin;
    errno = 0;
    long port_long = strtol(argv[1], &fin, 10);

    if (errno != 0 || *fin != '\0' || fin == argv[1]) {
        fprintf(stderr, "Erreur : '%s' n'est pas un nombre valide\n", argv[1]);
        return EXIT_FAILURE;
    }

    // Validation de plage (ports valides : 1-65535)
    if (port_long < 1 || port_long > 65535) {
        fprintf(stderr, "Erreur : port doit être entre 1 et 65535\n");
        return EXIT_FAILURE;
    }

    int port = (int)port_long;
    printf("Démarrage du serveur sur le port %d\n", port);
    demarrer_serveur(port);

    return EXIT_SUCCESS;
}
```

---

## Validation des données de fichiers

Ne jamais supposer qu'un fichier contient des données valides.

### Exemple : Lecture d'un fichier de configuration

```c
// ❌ Mauvais
void lire_config(const char *fichier) {
    FILE *f = fopen(fichier, "r");
    int valeur1, valeur2;
    fscanf(f, "%d %d", &valeur1, &valeur2);  // Pas de vérification !
    fclose(f);
}
```

```c
// ✅ Bon
#include <stdio.h>
#include <stdbool.h>

typedef struct {
    int valeur1;
    int valeur2;
} Config;

bool lire_config(const char *fichier, Config *config) {
    if (fichier == NULL || config == NULL) {
        return false;
    }

    FILE *f = fopen(fichier, "r");
    if (f == NULL) {
        perror("Erreur d'ouverture du fichier");
        return false;
    }

    char ligne[256];
    int lignes_lues = 0;

    while (fgets(ligne, sizeof(ligne), f) != NULL && lignes_lues < 2) {
        // Ignorer les lignes vides et les commentaires
        if (ligne[0] == '\n' || ligne[0] == '#') {
            continue;
        }

        int valeur;
        char *fin;
        errno = 0;
        valeur = (int)strtol(ligne, &fin, 10);

        if (errno != 0 || (fin == ligne)) {
            fprintf(stderr, "Erreur : valeur invalide à la ligne %d\n", lignes_lues + 1);
            fclose(f);
            return false;
        }

        // Validation de plage
        if (valeur < 0 || valeur > 1000) {
            fprintf(stderr, "Erreur : valeur hors limites (0-1000)\n");
            fclose(f);
            return false;
        }

        if (lignes_lues == 0) {
            config->valeur1 = valeur;
        } else {
            config->valeur2 = valeur;
        }

        lignes_lues++;
    }

    fclose(f);

    if (lignes_lues != 2) {
        fprintf(stderr, "Erreur : fichier incomplet\n");
        return false;
    }

    return true;
}

// Utilisation
int main(void) {
    Config config;

    if (lire_config("config.txt", &config)) {
        printf("Configuration chargée : %d, %d\n", config.valeur1, config.valeur2);
    } else {
        fprintf(stderr, "Échec du chargement de la configuration\n");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
```

---

## Validation des pointeurs

Toujours vérifier qu'un pointeur n'est pas `NULL` avant de le déréférencer.

```c
// ❌ Mauvais
void traiter_donnees(int *tableau) {
    printf("%d\n", tableau[0]);  // Crash si tableau == NULL !
}
```

```c
// ✅ Bon
bool traiter_donnees(int *tableau, size_t taille) {
    if (tableau == NULL) {
        fprintf(stderr, "Erreur : pointeur NULL\n");
        return false;
    }

    if (taille == 0) {
        fprintf(stderr, "Erreur : taille nulle\n");
        return false;
    }

    for (size_t i = 0; i < taille; i++) {
        printf("%d ", tableau[i]);
    }
    printf("\n");

    return true;
}
```

---

## Listes blanches vs listes noires

### Liste noire (Blacklist)
Rejeter ce qui est explicitement interdit.

```c
// ❌ Approche par liste noire (dangereuse)
bool caractere_autorise(char c) {
    // Interdire seulement quelques caractères
    if (c == '<' || c == '>' || c == ';') {
        return false;
    }
    return true;  // Tout le reste est accepté
}
// Problème : on oublie facilement des caractères dangereux
```

### Liste blanche (Whitelist)
N'accepter que ce qui est explicitement autorisé.

```c
// ✅ Approche par liste blanche (sûre)
bool caractere_autorise(char c) {
    // N'autoriser que les caractères sûrs
    return (c >= 'a' && c <= 'z') ||
           (c >= 'A' && c <= 'Z') ||
           (c >= '0' && c <= '9') ||
           (c == '_' || c == '-' || c == '.');
}
// Tout ce qui n'est pas explicitement autorisé est rejeté
```

**Principe** : Liste blanche > Liste noire en sécurité !

---

## Pattern : Validation en couches

Pour les systèmes complexes, validez à plusieurs niveaux.

```c
// Couche 1 : Validation syntaxique
bool valider_syntaxe_email(const char *email) {
    // Vérifications basiques de format
    return (email != NULL && strlen(email) > 3 && strchr(email, '@') != NULL);
}

// Couche 2 : Validation structurelle
bool valider_structure_email(const char *email) {
    // Vérifications plus approfondies (RFC)
    // ...
    return true;
}

// Couche 3 : Validation sémantique
bool valider_email_complet(const char *email) {
    if (!valider_syntaxe_email(email)) {
        return false;
    }

    if (!valider_structure_email(email)) {
        return false;
    }

    // Vérifications métier (domaine autorisé, etc.)
    const char *domaines_autorises[] = {"example.com", "test.org"};
    const char *arobase = strchr(email, '@');

    for (size_t i = 0; i < sizeof(domaines_autorises) / sizeof(domaines_autorises[0]); i++) {
        if (strcmp(arobase + 1, domaines_autorises[i]) == 0) {
            return true;
        }
    }

    return false;
}
```

---

## Gestion des erreurs de validation

### Pattern : Code de retour

```c
typedef enum {
    VALIDATION_OK = 0,
    VALIDATION_NULL_POINTER,
    VALIDATION_LONGUEUR_INVALIDE,
    VALIDATION_FORMAT_INVALIDE,
    VALIDATION_PLAGE_INVALIDE
} CodeValidation;

CodeValidation valider_nom_utilisateur(const char *nom) {
    if (nom == NULL) {
        return VALIDATION_NULL_POINTER;
    }

    size_t longueur = strlen(nom);
    if (longueur < 3 || longueur > 20) {
        return VALIDATION_LONGUEUR_INVALIDE;
    }

    for (size_t i = 0; i < longueur; i++) {
        if (!isalnum((unsigned char)nom[i]) && nom[i] != '_') {
            return VALIDATION_FORMAT_INVALIDE;
        }
    }

    return VALIDATION_OK;
}

// Utilisation
const char *messages[] = {
    "Validation réussie",
    "Erreur : pointeur NULL",
    "Erreur : longueur invalide (3-20 caractères)",
    "Erreur : caractères invalides (a-z, A-Z, 0-9, _ uniquement)",
    "Erreur : valeur hors limites"
};

CodeValidation resultat = valider_nom_utilisateur("User_123");
if (resultat != VALIDATION_OK) {
    fprintf(stderr, "%s\n", messages[resultat]);
}
```

---

## Outils et bibliothèques pour la validation

### 1. Expressions régulières (POSIX)

```c
#include <regex.h>
#include <stdbool.h>

bool valider_regex(const char *chaine, const char *pattern) {
    regex_t regex;
    int resultat;

    // Compiler l'expression régulière
    resultat = regcomp(&regex, pattern, REG_EXTENDED);
    if (resultat != 0) {
        fprintf(stderr, "Erreur : regex invalide\n");
        return false;
    }

    // Tester la correspondance
    resultat = regexec(&regex, chaine, 0, NULL, 0);

    // Libérer la regex
    regfree(&regex);

    return (resultat == 0);
}

// Utilisation
if (valider_regex("user@example.com", "^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}$")) {
    printf("Email valide\n");
}
```

### 2. Bibliothèques de validation

Pour des projets plus complexes, considérez :
- **libcurl** : Validation d'URL
- **GLib** : Fonctions de validation intégrées
- **ICU** : Validation de texte Unicode

---

## Checklist de validation des entrées

Avant de traiter toute entrée, posez-vous ces questions :

- ✅ **Type** : La donnée est-elle du type attendu ?
- ✅ **Longueur** : La taille est-elle acceptable ?
- ✅ **Plage** : La valeur est-elle dans l'intervalle valide ?
- ✅ **Format** : La structure respecte-t-elle le format attendu ?
- ✅ **Caractères** : N'y a-t-il que des caractères autorisés ?
- ✅ **Sémantique** : La valeur a-t-elle du sens dans le contexte ?
- ✅ **Null** : Les pointeurs ne sont-ils pas NULL ?
- ✅ **Limites** : Les buffers sont-ils assez grands ?

---

## Erreurs courantes à éviter

### 1. Valider après utilisation

```c
// ❌ Mauvais
void traiter(char *data) {
    printf("%s\n", data);  // Utilisé d'abord

    if (data == NULL) {    // Validé après !
        return;
    }
}
```

```c
// ✅ Bon
void traiter(char *data) {
    if (data == NULL) {    // Validé d'abord
        return;
    }

    printf("%s\n", data);  // Puis utilisé
}
```

### 2. Validation incomplète

```c
// ❌ Mauvais
void set_age(int age) {
    if (age > 0) {  // Oubli de la limite supérieure
        mon_age = age;
    }
}
// Un utilisateur peut entrer 999999
```

```c
// ✅ Bon
void set_age(int age) {
    if (age > 0 && age <= 150) {  // Plage complète
        mon_age = age;
    }
}
```

### 3. Faire confiance aux données internes

```c
// ❌ Même les données de fichiers de config doivent être validées !
Config config = charger_config("config.txt");
// Ne pas supposer que config est valide
```

---

## Résumé : Principes de validation

### 🎯 Règles fondamentales

1. **Tout valider** : Toute entrée externe doit être validée
2. **Valider tôt** : Avant d'utiliser la donnée
3. **Liste blanche** : Autoriser explicitement plutôt qu'interdire
4. **Défense en profondeur** : Plusieurs couches de validation
5. **Fail securely** : En cas d'erreur, refuser l'accès
6. **Messages clairs** : Informer l'utilisateur sans révéler de détails internes

### 💡 Conseils pratiques

- **Utilisez `strtol`** au lieu de `atoi`
- **Utilisez `fgets`** au lieu de `scanf("%s")`
- **Vérifiez les codes de retour** de toutes les fonctions
- **Testez avec des entrées malveillantes** (fuzzing)
- **Documentez vos validations** dans le code

### 🔒 Sécurité

> "Ne jamais faire confiance aux données externes, même si elles viennent d'une source supposée sûre"

La validation des entrées est votre première ligne de défense contre :
- Les bugs
- Les crashs
- Les vulnérabilités de sécurité
- Les attaques malveillantes

---

**🚀 Prochaine étape** : Maintenant que vous savez valider les entrées, nous verrons comment prévenir spécifiquement les buffer overflows dans la section suivante.

⏭️ [Prévention des buffer overflows](/26-securite-code-defensif/03-prevention-buffer-overflows.md)
