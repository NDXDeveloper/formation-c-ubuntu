🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 8.4 Manipulation de chaînes

## Introduction

Maintenant que vous maîtrisez les bases des chaînes en C (section 8.3), nous allons explorer des techniques plus avancées de manipulation. Cette section vous montrera comment parser, transformer, découper et construire des chaînes de manière efficace et sécurisée.

**Prérequis :**
- Maîtrise des bases des chaînes (section 8.3)
- Compréhension des pointeurs
- Connaissance des fonctions de `<string.h>`

## Découpage de chaînes (Tokenization)

Le découpage de chaînes consiste à diviser une chaîne en plusieurs sous-chaînes (tokens) selon des délimiteurs.

### strtok() : La fonction de tokenization

`strtok()` permet de découper une chaîne selon des délimiteurs (espaces, virgules, etc.).

```c
#include <stdio.h>
#include <string.h>

int main(void) {
    char phrase[] = "Bonjour,le,monde,en,C";
    char *token;

    // Premier appel : on passe la chaîne
    token = strtok(phrase, ",");

    // Appels suivants : on passe NULL
    while (token != NULL) {
        printf("%s\n", token);
        token = strtok(NULL, ",");
    }

    return 0;
}
```

**Sortie :**
```
Bonjour  
le  
monde  
en  
C  
```

**Comment ça fonctionne ?**
1. Premier appel : `strtok(phrase, ",")` trouve le premier token
2. `strtok()` remplace le délimiteur par `'\0'`
3. Appels suivants avec `NULL` : continue depuis la position précédente
4. Retourne `NULL` quand il n'y a plus de tokens

**⚠️ Important :** `strtok()` **modifie** la chaîne originale !

```c
char phrase[] = "A,B,C";  
printf("Avant : %s\n", phrase);  // "A,B,C"  

char *token = strtok(phrase, ",");  
printf("Après : %s\n", phrase);   // "A" (les virgules sont remplacées par '\0')  
```

### strtok() avec plusieurs délimiteurs

```c
char texte[] = "Nom: Jean; Age: 30; Ville: Paris";  
char *token = strtok(texte, ":;");  

while (token != NULL) {
    printf("[%s]\n", token);
    token = strtok(NULL, ":;");
}
```

**Sortie :**
```
[Nom]
[ Jean]
[ Age]
[ 30]
[ Ville]
[ Paris]
```

### Problèmes avec strtok()

1. **Modifie la chaîne originale**
   ```c
   const char *phrase = "A,B,C";
   strtok(phrase, ",");  // ❌ Erreur : modification d'une zone lecture seule
   ```

2. **Non thread-safe** (utilise un état interne statique)
   ```c
   // Si deux parties du code utilisent strtok() en même temps,
   // elles interfèrent l'une avec l'autre
   ```

3. **Ne peut pas gérer plusieurs tokenizations simultanées**

### Alternative thread-safe : strtok_r()

**Note :** `strtok_r()` est une fonction **POSIX**, pas C standard. Pour l'utiliser avec `-std=c17`, il faut définir `_POSIX_C_SOURCE` avant les includes.

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>

int main(void) {
    char phrase[] = "Bonjour,le,monde";
    char *token;
    char *saveptr;  // Pointeur de sauvegarde

    token = strtok_r(phrase, ",", &saveptr);

    while (token != NULL) {
        printf("%s\n", token);
        token = strtok_r(NULL, ",", &saveptr);
    }

    return 0;
}
```

**Avantage :** `strtok_r()` stocke l'état dans `saveptr`, permettant plusieurs tokenizations simultanées.

### Implémentation personnalisée safe

**Note :** Cet exemple utilise `strdup()`, une fonction **POSIX** (ajoutée au standard C23). Avec `-std=c17`, il faut définir `_POSIX_C_SOURCE`.

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Découpe une chaîne sans la modifier (copie chaque token)
char **split_string(const char *str, const char *delim, int *count) {
    // Copie de travail
    char *copy = strdup(str);
    if (!copy) return NULL;

    // Compte les tokens
    int n = 0;
    char *temp = strdup(str);
    char *token = strtok(temp, delim);
    while (token != NULL) {
        n++;
        token = strtok(NULL, delim);
    }
    free(temp);

    // Alloue le tableau de résultats
    char **result = malloc(n * sizeof(char*));
    if (!result) {
        free(copy);
        return NULL;
    }

    // Remplit le tableau
    int i = 0;
    token = strtok(copy, delim);
    while (token != NULL) {
        result[i] = strdup(token);
        i++;
        token = strtok(NULL, delim);
    }

    free(copy);
    *count = n;
    return result;
}

int main(void) {
    const char *phrase = "un,deux,trois,quatre";
    int count;
    char **tokens = split_string(phrase, ",", &count);

    for (int i = 0; i < count; i++) {
        printf("Token %d: %s\n", i, tokens[i]);
        free(tokens[i]);  // Libérer chaque token
    }
    free(tokens);  // Libérer le tableau

    return 0;
}
```

## Recherche et remplacement

### Trouver toutes les occurrences d'un caractère

```c
#include <stdio.h>
#include <string.h>

void trouver_tous(const char *str, char cherche) {
    const char *ptr = str;
    int position = 0;

    while ((ptr = strchr(ptr, cherche)) != NULL) {
        position = ptr - str;
        printf("Trouvé '%c' à la position %d\n", cherche, position);
        ptr++;  // Passer au caractère suivant
    }
}

int main(void) {
    char phrase[] = "Bonjour le monde";
    trouver_tous(phrase, 'o');
    return 0;
}
```

**Sortie :**
```
Trouvé 'o' à la position 1  
Trouvé 'o' à la position 4  
Trouvé 'o' à la position 12  
```

### Trouver toutes les occurrences d'une sous-chaîne

```c
#include <stdio.h>
#include <string.h>

void trouver_toutes_sous_chaines(const char *str, const char *cherche) {
    const char *ptr = str;
    int position;

    while ((ptr = strstr(ptr, cherche)) != NULL) {
        position = ptr - str;
        printf("Trouvé \"%s\" à la position %d\n", cherche, position);
        ptr += strlen(cherche);  // Avancer après la sous-chaîne trouvée
    }
}

int main(void) {
    char texte[] = "Le chat est un chat. Le chat dort.";
    trouver_toutes_sous_chaines(texte, "chat");
    return 0;
}
```

**Sortie :**
```
Trouvé "chat" à la position 3  
Trouvé "chat" à la position 15  
Trouvé "chat" à la position 24  
```

### Remplacer un caractère

```c
#include <stdio.h>

void remplacer_char(char *str, char ancien, char nouveau) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == ancien) {
            str[i] = nouveau;
        }
    }
}

int main(void) {
    char phrase[] = "Bonjour le monde";
    printf("Avant : %s\n", phrase);

    remplacer_char(phrase, 'o', '0');
    printf("Après : %s\n", phrase);  // "B0nj0ur le m0nde"

    return 0;
}
```

### Remplacer une sous-chaîne (version simple)

```c
#include <stdio.h>
#include <string.h>

// Remplace la première occurrence
int remplacer_premiere(char *str, size_t taille,
                       const char *ancien, const char *nouveau) {
    char *pos = strstr(str, ancien);
    if (pos == NULL) {
        return 0;  // Pas trouvé
    }

    size_t len_ancien = strlen(ancien);
    size_t len_nouveau = strlen(nouveau);
    size_t len_apres = strlen(pos + len_ancien);

    // Vérifier l'espace disponible
    if (strlen(str) - len_ancien + len_nouveau >= taille) {
        return -1;  // Pas assez d'espace
    }

    // Déplacer la fin de la chaîne
    memmove(pos + len_nouveau, pos + len_ancien, len_apres + 1);

    // Copier la nouvelle sous-chaîne
    memcpy(pos, nouveau, len_nouveau);

    return 1;  // Succès
}

int main(void) {
    char buffer[100] = "Le chat dort. Le chat joue.";

    printf("Avant : %s\n", buffer);
    remplacer_premiere(buffer, sizeof(buffer), "chat", "chien");
    printf("Après : %s\n", buffer);  // "Le chien dort. Le chat joue."

    return 0;
}
```

### Remplacer toutes les occurrences

```c
#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *remplacer_toutes(const char *str, const char *ancien, const char *nouveau) {
    // Compter les occurrences
    int count = 0;
    const char *tmp = str;
    while ((tmp = strstr(tmp, ancien)) != NULL) {
        count++;
        tmp += strlen(ancien);
    }

    if (count == 0) {
        return strdup(str);  // Aucun remplacement
    }

    // Calculer la nouvelle taille
    size_t len_ancien = strlen(ancien);
    size_t len_nouveau = strlen(nouveau);
    size_t nouvelle_taille = strlen(str) + count * (len_nouveau - len_ancien) + 1;

    // Allouer le nouveau buffer
    char *resultat = malloc(nouvelle_taille);
    if (!resultat) return NULL;

    // Construire la nouvelle chaîne
    char *dest = resultat;
    const char *src = str;

    while (*src) {
        const char *pos = strstr(src, ancien);

        if (pos == NULL) {
            // Copier le reste
            strcpy(dest, src);
            break;
        }

        // Copier jusqu'à l'occurrence
        size_t len = pos - src;
        memcpy(dest, src, len);
        dest += len;

        // Copier le remplacement
        memcpy(dest, nouveau, len_nouveau);
        dest += len_nouveau;

        // Avancer dans la source
        src = pos + len_ancien;
    }

    return resultat;
}

int main(void) {
    const char *texte = "Le chat dort. Le chat joue. Le chat mange.";
    char *nouveau = remplacer_toutes(texte, "chat", "chien");

    printf("Original : %s\n", texte);
    printf("Modifié  : %s\n", nouveau);
    // "Le chien dort. Le chien joue. Le chien mange."

    free(nouveau);
    return 0;
}
```

## Extraction de sous-chaînes

### Extraire une sous-chaîne (substring)

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *substring(const char *str, int debut, int longueur) {
    int len_str = strlen(str);

    // Vérifications
    if (debut < 0 || debut >= len_str || longueur <= 0) {
        return NULL;
    }

    // Ajuster la longueur si nécessaire
    if (debut + longueur > len_str) {
        longueur = len_str - debut;
    }

    // Allouer et copier
    char *result = malloc(longueur + 1);
    if (!result) return NULL;

    strncpy(result, str + debut, longueur);
    result[longueur] = '\0';

    return result;
}

int main(void) {
    const char *texte = "Bonjour le monde";

    char *sub1 = substring(texte, 0, 7);   // "Bonjour"
    char *sub2 = substring(texte, 8, 2);   // "le"
    char *sub3 = substring(texte, 11, 5);  // "monde"

    printf("%s\n", sub1);
    printf("%s\n", sub2);
    printf("%s\n", sub3);

    free(sub1);
    free(sub2);
    free(sub3);

    return 0;
}
```

### Extraire entre deux délimiteurs

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *extraire_entre(const char *str, const char *debut, const char *fin) {
    const char *pos_debut = strstr(str, debut);
    if (!pos_debut) return NULL;

    pos_debut += strlen(debut);  // Passer le délimiteur de début

    const char *pos_fin = strstr(pos_debut, fin);
    if (!pos_fin) return NULL;

    size_t longueur = pos_fin - pos_debut;
    char *result = malloc(longueur + 1);
    if (!result) return NULL;

    strncpy(result, pos_debut, longueur);
    result[longueur] = '\0';

    return result;
}

int main(void) {
    const char *html = "<title>Mon Site Web</title>";
    char *titre = extraire_entre(html, "<title>", "</title>");

    if (titre) {
        printf("Titre : %s\n", titre);  // "Mon Site Web"
        free(titre);
    }

    return 0;
}
```

## Nettoyage et formatage

### Supprimer les espaces en début et fin (trim)

```c
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void trim(char *str) {
    if (!str || *str == '\0') return;

    // Trouver le premier non-espace
    char *debut = str;
    while (isspace((unsigned char)*debut)) {
        debut++;
    }

    // Tout est espace
    if (*debut == '\0') {
        *str = '\0';
        return;
    }

    // Trouver le dernier non-espace
    char *fin = str + strlen(str) - 1;
    while (fin > debut && isspace((unsigned char)*fin)) {
        fin--;
    }

    // Déplacer et terminer
    size_t longueur = fin - debut + 1;
    memmove(str, debut, longueur);
    str[longueur] = '\0';
}

int main(void) {
    char texte[] = "   Bonjour le monde   ";
    printf("Avant : '%s'\n", texte);
    trim(texte);
    printf("Après : '%s'\n", texte);  // "Bonjour le monde"

    return 0;
}
```

### Supprimer les espaces multiples

```c
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void compresser_espaces(char *str) {
    char *src = str;
    char *dest = str;
    int espace_precedent = 0;

    while (*src) {
        if (isspace((unsigned char)*src)) {
            if (!espace_precedent) {
                *dest++ = ' ';
                espace_precedent = 1;
            }
        } else {
            *dest++ = *src;
            espace_precedent = 0;
        }
        src++;
    }
    *dest = '\0';
}

int main(void) {
    char texte[] = "Bonjour    le     monde";
    printf("Avant : '%s'\n", texte);
    compresser_espaces(texte);
    printf("Après : '%s'\n", texte);  // "Bonjour le monde"

    return 0;
}
```

### Supprimer tous les espaces

```c
void supprimer_espaces(char *str) {
    char *src = str;
    char *dest = str;

    while (*src) {
        if (!isspace((unsigned char)*src)) {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';
}
```

### Supprimer les caractères non alphanumériques

```c
#include <stdio.h>
#include <ctype.h>

void garder_alnum(char *str) {
    char *src = str;
    char *dest = str;

    while (*src) {
        if (isalnum((unsigned char)*src)) {
            *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';
}

int main(void) {
    char texte[] = "Bonjour@123#monde!";
    garder_alnum(texte);
    printf("%s\n", texte);  // "Bonjour123monde"
    return 0;
}
```

## Conversion de casse

### Tout en majuscules

```c
#include <ctype.h>

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}
```

### Tout en minuscules

```c
void to_lowercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower((unsigned char)str[i]);
    }
}
```

### Première lettre en majuscule (capitalize)

```c
#include <stdio.h>
#include <ctype.h>

void capitalize(char *str) {
    int nouveau_mot = 1;

    for (int i = 0; str[i]; i++) {
        if (isspace((unsigned char)str[i])) {
            nouveau_mot = 1;
        } else if (nouveau_mot) {
            str[i] = toupper((unsigned char)str[i]);
            nouveau_mot = 0;
        } else {
            str[i] = tolower((unsigned char)str[i]);
        }
    }
}

int main(void) {
    char texte[] = "bonjour le monde en c";
    capitalize(texte);
    printf("%s\n", texte);  // "Bonjour Le Monde En C"
    return 0;
}
```

### Alterner majuscules/minuscules

```c
void alternate_case(char *str) {
    int upper = 1;

    for (int i = 0; str[i]; i++) {
        if (isalpha((unsigned char)str[i])) {
            str[i] = upper ? toupper((unsigned char)str[i])
                          : tolower((unsigned char)str[i]);
            upper = !upper;
        }
    }
}
```

## Construction de chaînes

### Concaténation multiple sécurisée

```c
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

// Concatène plusieurs chaînes dans un buffer
int concat_safe(char *dest, size_t dest_size, int n, ...) {
    va_list args;
    va_start(args, n);

    dest[0] = '\0';
    size_t total = 0;

    for (int i = 0; i < n; i++) {
        const char *str = va_arg(args, const char*);
        size_t len = strlen(str);

        if (total + len >= dest_size) {
            va_end(args);
            return -1;  // Débordement
        }

        strcat(dest, str);
        total += len;
    }

    va_end(args);
    return 0;
}

int main(void) {
    char buffer[100];

    if (concat_safe(buffer, sizeof(buffer), 5,
                    "Bonjour", " ", "le", " ", "monde") == 0) {
        printf("%s\n", buffer);  // "Bonjour le monde"
    }

    return 0;
}
```

### Construction avec snprintf

```c
#include <stdio.h>

int main(void) {
    char buffer[100];
    const char *nom = "Alice";
    int age = 25;
    double taille = 1.65;

    snprintf(buffer, sizeof(buffer),
             "Nom: %s, Age: %d, Taille: %.2f m",
             nom, age, taille);

    printf("%s\n", buffer);
    // "Nom: Alice, Age: 25, Taille: 1.65 m"

    return 0;
}
```

### Joindre un tableau de chaînes

```c
#include <stdio.h>
#include <string.h>

void join_strings(char *dest, size_t dest_size,
                  const char **strings, int count,
                  const char *separator) {
    dest[0] = '\0';

    for (int i = 0; i < count; i++) {
        if (i > 0) {
            strncat(dest, separator, dest_size - strlen(dest) - 1);
        }
        strncat(dest, strings[i], dest_size - strlen(dest) - 1);
    }
}

int main(void) {
    const char *mots[] = {"un", "deux", "trois", "quatre"};
    char resultat[100];

    join_strings(resultat, sizeof(resultat), mots, 4, ", ");
    printf("%s\n", resultat);  // "un, deux, trois, quatre"

    return 0;
}
```

## Parsing de données structurées

### Parser une ligne CSV

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    char nom[50];
    int age;
    char ville[50];
} Personne;

int parser_csv_ligne(const char *ligne, Personne *p) {
    char copie[200];
    strncpy(copie, ligne, sizeof(copie) - 1);
    copie[sizeof(copie) - 1] = '\0';

    char *token = strtok(copie, ",");
    if (!token) return 0;
    strncpy(p->nom, token, sizeof(p->nom) - 1);

    token = strtok(NULL, ",");
    if (!token) return 0;
    p->age = atoi(token);

    token = strtok(NULL, ",");
    if (!token) return 0;
    strncpy(p->ville, token, sizeof(p->ville) - 1);

    return 1;
}

int main(void) {
    const char *ligne = "Alice,25,Paris";
    Personne p;

    if (parser_csv_ligne(ligne, &p)) {
        printf("Nom: %s, Age: %d, Ville: %s\n",
               p.nom, p.age, p.ville);
    }

    return 0;
}
```

### Parser une paire clé=valeur

```c
#include <stdio.h>
#include <string.h>

typedef struct {
    char cle[50];
    char valeur[100];
} PaireClefValeur;

int parser_paire(const char *ligne, PaireClefValeur *paire) {
    const char *egal = strchr(ligne, '=');
    if (!egal) return 0;

    // Extraire la clé
    size_t len_cle = egal - ligne;
    if (len_cle >= sizeof(paire->cle)) return 0;

    strncpy(paire->cle, ligne, len_cle);
    paire->cle[len_cle] = '\0';

    // Extraire la valeur
    strncpy(paire->valeur, egal + 1, sizeof(paire->valeur) - 1);
    paire->valeur[sizeof(paire->valeur) - 1] = '\0';

    return 1;
}

int main(void) {
    const char *config = "timeout=30";
    PaireClefValeur paire;

    if (parser_paire(config, &paire)) {
        printf("Clé: %s, Valeur: %s\n", paire.cle, paire.valeur);
    }

    return 0;
}
```

### Parser une URL simple

```c
#include <stdio.h>
#include <string.h>

typedef struct {
    char protocole[10];
    char domaine[100];
    char chemin[200];
} URL;

int parser_url(const char *url, URL *parsed) {
    // Trouver le protocole
    const char *protocole_fin = strstr(url, "://");
    if (!protocole_fin) return 0;

    size_t len_protocole = protocole_fin - url;
    strncpy(parsed->protocole, url, len_protocole);
    parsed->protocole[len_protocole] = '\0';

    // Passer "://"
    const char *reste = protocole_fin + 3;

    // Trouver le domaine
    const char *chemin_debut = strchr(reste, '/');
    if (chemin_debut) {
        size_t len_domaine = chemin_debut - reste;
        strncpy(parsed->domaine, reste, len_domaine);
        parsed->domaine[len_domaine] = '\0';

        strncpy(parsed->chemin, chemin_debut, sizeof(parsed->chemin) - 1);
    } else {
        strncpy(parsed->domaine, reste, sizeof(parsed->domaine) - 1);
        parsed->chemin[0] = '\0';
    }

    return 1;
}

int main(void) {
    const char *url = "https://www.example.com/page/index.html";
    URL parsed;

    if (parser_url(url, &parsed)) {
        printf("Protocole: %s\n", parsed.protocole);
        printf("Domaine: %s\n", parsed.domaine);
        printf("Chemin: %s\n", parsed.chemin);
    }

    return 0;
}
```

## Validation de chaînes

### Vérifier si une chaîne est numérique

```c
#include <stdio.h>
#include <ctype.h>

int est_numerique(const char *str) {
    if (!str || *str == '\0') return 0;

    // Gérer le signe optionnel
    if (*str == '+' || *str == '-') {
        str++;
    }

    if (*str == '\0') return 0;  // Juste un signe

    while (*str) {
        if (!isdigit((unsigned char)*str)) {
            return 0;
        }
        str++;
    }

    return 1;
}

int main(void) {
    printf("%d\n", est_numerique("123"));    // 1
    printf("%d\n", est_numerique("-456"));   // 1
    printf("%d\n", est_numerique("12.34"));  // 0
    printf("%d\n", est_numerique("abc"));    // 0
    return 0;
}
```

### Vérifier un format d'email basique

```c
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int est_email_valide(const char *email) {
    // Doit contenir exactement un @
    const char *arobase = strchr(email, '@');
    if (!arobase || arobase != strrchr(email, '@')) {
        return 0;
    }

    // Au moins un caractère avant @
    if (arobase == email) {
        return 0;
    }

    // Au moins un point après @
    const char *point = strchr(arobase, '.');
    if (!point || point == arobase + 1) {
        return 0;
    }

    // Au moins un caractère après le dernier point
    if (strlen(point) < 2) {
        return 0;
    }

    return 1;
}

int main(void) {
    printf("%d\n", est_email_valide("user@example.com"));    // 1
    printf("%d\n", est_email_valide("invalid.email"));       // 0
    printf("%d\n", est_email_valide("@example.com"));        // 0
    printf("%d\n", est_email_valide("user@.com"));           // 0
    return 0;
}
```

### Vérifier une date au format YYYY-MM-DD

```c
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int est_date_valide(const char *date) {
    // Vérifier la longueur
    if (strlen(date) != 10) return 0;

    // Vérifier le format: XXXX-XX-XX
    for (int i = 0; i < 10; i++) {
        if (i == 4 || i == 7) {
            if (date[i] != '-') return 0;
        } else {
            if (!isdigit((unsigned char)date[i])) return 0;
        }
    }

    // Extraire année, mois, jour
    int annee, mois, jour;
    sscanf(date, "%4d-%2d-%2d", &annee, &mois, &jour);

    // Vérifier les plages
    if (annee < 1900 || annee > 2100) return 0;
    if (mois < 1 || mois > 12) return 0;
    if (jour < 1 || jour > 31) return 0;

    // Vérification basique des jours par mois
    int jours_par_mois[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    // Année bissextile
    if ((annee % 4 == 0 && annee % 100 != 0) || (annee % 400 == 0)) {
        jours_par_mois[1] = 29;
    }

    if (jour > jours_par_mois[mois - 1]) return 0;

    return 1;
}

int main(void) {
    printf("%d\n", est_date_valide("2024-12-25"));  // 1
    printf("%d\n", est_date_valide("2024-13-01"));  // 0 (mois invalide)
    printf("%d\n", est_date_valide("2024-02-30"));  // 0 (jour invalide)
    printf("%d\n", est_date_valide("24-12-25"));    // 0 (format incorrect)
    return 0;
}
```

## Comparaison avancée

### Comparaison insensible à la casse

```c
#include <stdio.h>
#include <ctype.h>

int strcmp_case_insensitive(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        char c1 = tolower((unsigned char)*s1);
        char c2 = tolower((unsigned char)*s2);

        if (c1 != c2) {
            return c1 - c2;
        }

        s1++;
        s2++;
    }

    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

int main(void) {
    printf("%d\n", strcmp_case_insensitive("Hello", "hello"));    // 0
    printf("%d\n", strcmp_case_insensitive("WORLD", "world"));    // 0
    printf("%d\n", strcmp_case_insensitive("abc", "ABC"));        // 0
    return 0;
}
```

### Comparaison avec wildcards simples

```c
#include <stdio.h>

// Supporte * (n'importe quelle séquence) et ? (un caractère)
int match_wildcard(const char *pattern, const char *str) {
    if (*pattern == '\0' && *str == '\0') {
        return 1;  // Correspondance complète
    }

    if (*pattern == '*') {
        // * peut correspondre à 0 ou plusieurs caractères
        return match_wildcard(pattern + 1, str) ||
               (*str != '\0' && match_wildcard(pattern, str + 1));
    }

    if (*pattern == '?' && *str != '\0') {
        // ? correspond à exactement un caractère
        return match_wildcard(pattern + 1, str + 1);
    }

    if (*pattern == *str) {
        return match_wildcard(pattern + 1, str + 1);
    }

    return 0;
}

int main(void) {
    printf("%d\n", match_wildcard("h*o", "hello"));        // 1
    printf("%d\n", match_wildcard("h?llo", "hello"));      // 1
    printf("%d\n", match_wildcard("h?llo", "hallo"));      // 1
    printf("%d\n", match_wildcard("*.txt", "file.txt"));   // 1
    printf("%d\n", match_wildcard("*.txt", "file.doc"));   // 0
    return 0;
}
```

## Utilitaires avancés

### Inverser une chaîne

```c
void reverse_string(char *str) {
    int len = strlen(str);

    for (int i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }
}
```

### Rotation d'une chaîne

```c
#include <stdio.h>
#include <string.h>

void rotate_left(char *str, int n) {
    int len = strlen(str);
    if (len == 0) return;

    n = n % len;  // Gérer n > len

    // Inverser les n premiers caractères
    for (int i = 0; i < n / 2; i++) {
        char temp = str[i];
        str[i] = str[n - 1 - i];
        str[n - 1 - i] = temp;
    }

    // Inverser le reste
    for (int i = 0; i < (len - n) / 2; i++) {
        char temp = str[n + i];
        str[n + i] = str[len - 1 - i];
        str[len - 1 - i] = temp;
    }

    // Inverser toute la chaîne
    reverse_string(str);
}

int main(void) {
    char mot[] = "abcdef";
    printf("Avant : %s\n", mot);

    rotate_left(mot, 2);
    printf("Après rotation de 2: %s\n", mot);  // "cdefab"

    return 0;
}
```

### Compter les occurrences d'un caractère

```c
int count_char(const char *str, char c) {
    int count = 0;
    while (*str) {
        if (*str == c) {
            count++;
        }
        str++;
    }
    return count;
}
```

### Compter les occurrences d'une sous-chaîne

```c
#include <string.h>

int count_substring(const char *str, const char *sub) {
    int count = 0;
    const char *ptr = str;
    size_t len = strlen(sub);

    while ((ptr = strstr(ptr, sub)) != NULL) {
        count++;
        ptr += len;
    }

    return count;
}
```

## Exemple complet : Analyseur de logs

```c
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

typedef struct {
    char timestamp[20];
    char niveau[10];
    char message[200];
} LogEntry;

// Parse une ligne de log au format: [YYYY-MM-DD HH:MM:SS] NIVEAU: message
int parser_log(const char *ligne, LogEntry *entry) {
    // Vérifier le format de base
    if (ligne[0] != '[') return 0;

    // Extraire le timestamp
    const char *fermeture = strchr(ligne, ']');
    if (!fermeture) return 0;

    size_t len_timestamp = fermeture - ligne - 1;
    if (len_timestamp >= sizeof(entry->timestamp)) return 0;

    strncpy(entry->timestamp, ligne + 1, len_timestamp);
    entry->timestamp[len_timestamp] = '\0';

    // Passer les espaces
    const char *ptr = fermeture + 1;
    while (*ptr && isspace((unsigned char)*ptr)) {
        ptr++;
    }

    // Extraire le niveau
    const char *deux_points = strchr(ptr, ':');
    if (!deux_points) return 0;

    size_t len_niveau = deux_points - ptr;
    if (len_niveau >= sizeof(entry->niveau)) return 0;

    strncpy(entry->niveau, ptr, len_niveau);
    entry->niveau[len_niveau] = '\0';

    // Extraire le message
    ptr = deux_points + 1;
    while (*ptr && isspace((unsigned char)*ptr)) {
        ptr++;
    }

    strncpy(entry->message, ptr, sizeof(entry->message) - 1);
    entry->message[sizeof(entry->message) - 1] = '\0';

    return 1;
}

void analyser_logs(const char *logs[], int nb_lignes) {
    int erreurs = 0, warnings = 0, infos = 0;

    printf("=== ANALYSE DES LOGS ===\n\n");

    for (int i = 0; i < nb_lignes; i++) {
        LogEntry entry;

        if (parser_log(logs[i], &entry)) {
            printf("Ligne %d:\n", i + 1);
            printf("  Timestamp: %s\n", entry.timestamp);
            printf("  Niveau: %s\n", entry.niveau);
            printf("  Message: %s\n\n", entry.message);

            if (strcmp(entry.niveau, "ERROR") == 0) {
                erreurs++;
            } else if (strcmp(entry.niveau, "WARNING") == 0) {
                warnings++;
            } else if (strcmp(entry.niveau, "INFO") == 0) {
                infos++;
            }
        }
    }

    printf("=== STATISTIQUES ===\n");
    printf("Erreurs: %d\n", erreurs);
    printf("Warnings: %d\n", warnings);
    printf("Infos: %d\n", infos);
}

int main(void) {
    const char *logs[] = {
        "[2024-01-15 10:30:45] INFO: Démarrage de l'application",
        "[2024-01-15 10:31:12] WARNING: Mémoire disponible faible",
        "[2024-01-15 10:32:03] ERROR: Connexion à la base de données échouée",
        "[2024-01-15 10:33:21] INFO: Nouvelle tentative de connexion",
        "[2024-01-15 10:33:25] INFO: Connexion réussie"
    };

    analyser_logs(logs, 5);

    return 0;
}
```

## Bonnes pratiques

### ✅ 1. Toujours vérifier les pointeurs NULL

```c
char *result = strstr(texte, "cherche");  
if (result != NULL) {  // Toujours vérifier !  
    // Utiliser result
}
```

### ✅ 2. Utiliser const pour les paramètres en lecture seule

```c
int compter_mots(const char *texte) {
    // const garantit qu'on ne modifie pas texte
}
```

### ✅ 3. Vérifier les limites de buffer

```c
char buffer[50];  
strncpy(buffer, source, sizeof(buffer) - 1);  
buffer[sizeof(buffer) - 1] = '\0';  // Garantir la terminaison  
```

### ✅ 4. Libérer la mémoire allouée dynamiquement

```c
char *copie = strdup(original);
// ... utilisation ...
free(copie);  // Ne pas oublier !
```

### ✅ 5. Préférer snprintf à sprintf

```c
// ❌ Dangereux
sprintf(buffer, "%s %d", str, num);

// ✅ Sécurisé
snprintf(buffer, sizeof(buffer), "%s %d", str, num);
```

### ✅ 6. Gérer les erreurs de parsing

```c
int resultat = parser_donnees(ligne, &data);  
if (resultat != 0) {  
    fprintf(stderr, "Erreur de parsing ligne %d\n", num_ligne);
    // Gérer l'erreur
}
```

## Pièges à éviter

### ❌ 1. Utiliser strtok() sur des chaînes constantes

```c
const char *texte = "a,b,c";  
strtok(texte, ",");  // ❌ Modifie une zone lecture seule  
```

### ❌ 2. Oublier que strtok() modifie la chaîne

```c
char original[] = "a,b,c";  
printf("Avant: %s\n", original);  
strtok(original, ",");  
printf("Après: %s\n", original);  // Affiche juste "a"  
```

### ❌ 3. Ne pas vérifier les débordements

```c
char petit[5];  
strcpy(petit, "Longue chaîne");  // ❌ Buffer overflow !  
```

### ❌ 4. Utiliser strlen() dans une condition de boucle

```c
// ❌ Inefficace : strlen() appelé à chaque itération
for (int i = 0; i < strlen(str); i++) { }

// ✅ Efficace
size_t len = strlen(str);  
for (int i = 0; i < len; i++) { }  
```

### ❌ 5. Oublier le '\0' lors de la copie manuelle

```c
char dest[10];  
for (int i = 0; i < 5; i++) {  
    dest[i] = src[i];
}
// ❌ Manque dest[5] = '\0';
```

## Résumé des points clés

1. **Tokenization avec strtok()**
   - Modifie la chaîne originale
   - Non thread-safe (utiliser `strtok_r()`)
   - Attention aux chaînes constantes

2. **Recherche et remplacement**
   - `strchr()` et `strstr()` pour trouver
   - Gérer les remplacements avec précaution (taille du buffer)

3. **Extraction de sous-chaînes**
   - Allouer suffisamment de mémoire
   - Toujours terminer par '\0'

4. **Nettoyage et formatage**
   - `trim()`, suppression d'espaces, conversion de casse
   - Utile pour normaliser les entrées utilisateur

5. **Construction de chaînes**
   - Préférer `snprintf()` à `sprintf()`
   - Vérifier les débordements

6. **Parsing**
   - Valider le format avant de parser
   - Gérer les erreurs gracieusement

7. **Validation**
   - Vérifier la structure des données
   - Ne pas faire confiance aux entrées utilisateur

8. **Toujours vérifier**
   - Les pointeurs NULL
   - Les limites de buffer
   - Les valeurs de retour

## Pour aller plus loin

Dans la section suivante, nous verrons :
- **Section 8.5** : Dangers des chaînes : Buffer Overflows et sécurité
- **Section 8.6** : Fonctions sécurisées et alternatives modernes
- **Section 8.7** : Strings littérales et immutabilité en détail

La manipulation de chaînes est une compétence fondamentale en C. Ces techniques sont utilisées quotidiennement dans le parsing de fichiers de configuration, l'analyse de logs, le traitement de données textuelles, et bien plus encore !

---

**Note :** La manipulation de chaînes en C demande de la rigueur et de l'attention aux détails. Testez toujours vos fonctions avec des cas limites (chaînes vides, très longues, avec caractères spéciaux) pour vous assurer qu'elles sont robustes.

⏭️ [Dangers des chaînes : Buffer Overflows](/08-tableaux-et-chaines/05-dangers-buffer-overflows.md)
