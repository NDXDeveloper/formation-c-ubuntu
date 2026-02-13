🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 8.3 Les chaînes en C

## Introduction

En C, il n'existe pas de type `string` comme dans d'autres langages. Les chaînes de caractères sont simplement des **tableaux de caractères terminés par un caractère nul**. Cette particularité fait des chaînes en C l'un des concepts les plus importants — et parfois déroutants — à maîtriser.

**Ce que vous devez savoir avant cette section :**
- Les tableaux unidimensionnels
- Les pointeurs de base
- Le type `char`

## Qu'est-ce qu'une chaîne en C ?

Une chaîne de caractères (string) en C est un **tableau de caractères qui se termine par le caractère nul** `'\0'`.

```c
char message[] = "Bonjour";
```

**En mémoire, cela donne :**

```
Index :     0    1    2    3    4    5    6    7
          ┌────┬────┬────┬────┬────┬────┬────┬────┐
message : │ B  │ o  │ n  │ j  │ o  │ u  │ r  │ \0 │
          └────┴────┴────┴────┴────┴────┴────┴────┘
```

**Points cruciaux :**
- Le caractère `'\0'` (code ASCII 0) marque la **fin de la chaîne**
- Ce caractère est **automatiquement ajouté** lors de l'initialisation avec des guillemets doubles
- La longueur réelle du tableau est **longueur visible + 1** (pour le `'\0'`)

### Pourquoi le caractère nul ?

Le `'\0'` permet aux fonctions de savoir où s'arrête la chaîne sans avoir besoin de passer la longueur séparément.

```c
// Sans '\0', comment savoir où s'arrête la chaîne ?
char mauvais[] = {'H', 'i'};  // ❌ Pas de '\0' : comportement indéfini

// Avec '\0', tout est clair
char bon[] = {'H', 'i', '\0'};  // ✅ Chaîne valide
```

## Déclaration et initialisation

### Méthode 1 : Initialisation avec une chaîne littérale (recommandée)

```c
char message[] = "Bonjour";
```

- Le compilateur calcule automatiquement la taille (8 caractères : 7 + '\0')
- Le `'\0'` est ajouté automatiquement
- C'est la méthode la plus simple et la plus courante

### Méthode 2 : Spécifier la taille explicitement

```c
char message[8] = "Bonjour";  // Taille exacte  
char message[20] = "Bonjour"; // Taille plus grande (reste rempli de '\0')  
```

**Attention :** Si la taille est trop petite, le `'\0'` ne sera pas inclus !

```c
char message[7] = "Bonjour";  // ❌ Pas de place pour '\0' !
// Comportement indéfini lors de l'utilisation
```

### Méthode 3 : Initialisation caractère par caractère

```c
char message[8] = {'B', 'o', 'n', 'j', 'o', 'u', 'r', '\0'};
```

**Important :** N'oubliez jamais le `'\0'` final !

### Méthode 4 : Déclaration puis affectation

```c
char message[20];
// On ne peut PAS faire : message = "Bonjour";  ❌
// Il faut utiliser strcpy() ou remplir manuellement
```

**Pourquoi ?** Le nom d'un tableau est une constante, on ne peut pas le réassigner.

## Chaînes littérales

Une **chaîne littérale** est du texte entre guillemets doubles dans le code source.

```c
"Ceci est une chaîne littérale"
```

### Caractéristiques des chaînes littérales

1. **Stockage en mémoire lecture seule**
   ```c
   char *ptr = "Bonjour";
   ptr[0] = 'b';  // ❌ Comportement indéfini : modification d'une zone lecture seule
   ```

2. **Persistance pendant toute l'exécution**
   ```c
   char *obtenir_message(void) {
       return "Message constant";  // ✅ OK : chaîne littérale
   }
   ```

3. **Type réel : `const char *`**
   ```c
   const char *ptr = "Bonjour";  // ✅ Bonne pratique : const explicite
   ```

### Différence : tableau vs pointeur vers chaîne littérale

```c
// Cas 1 : Tableau de caractères (modifiable)
char tab[] = "Bonjour";  
tab[0] = 'b';  // ✅ OK : mémoire modifiable  
printf("%s\n", tab);  // "bonjour"  

// Cas 2 : Pointeur vers chaîne littérale (non modifiable)
char *ptr = "Bonjour";  
ptr[0] = 'b';  // ❌ Erreur : modification d'une zone lecture seule  

// Version correcte avec const
const char *ptr = "Bonjour";
// ptr[0] = 'b';  // ❌ Erreur de compilation : on ne peut pas modifier
```

**Règle de bonne pratique :** Utilisez toujours `const char *` pour les chaînes littérales.

## Caractère vs Chaîne

**Attention à ne pas confondre !**

```c
char caractere = 'A';      // Guillemets simples : un seul caractère  
char chaine[] = "A";       // Guillemets doubles : chaîne (2 octets : 'A' et '\0')  
```

**En mémoire :**

```c
// caractere :
// 1 octet : 65 (code ASCII de 'A')

// chaine :
// 2 octets : 65, 0
//           'A' '\0'
```

**Conséquence :**

```c
sizeof('A')   // 4 (car 'A' est promu en int en C)  
sizeof("A")   // 2 (caractère + '\0')  

char c = 'A';      // OK  
char s[] = "A";    // OK  
char c2 = "A";     // ❌ Erreur de type  
```

## Affichage et lecture de chaînes

### Affichage avec printf

```c
char nom[] = "Alice";

printf("%s\n", nom);           // Alice  
printf("Bonjour %s\n", nom);   // Bonjour Alice  
```

**Le format `%s` :**
- Affiche tous les caractères jusqu'au `'\0'`
- Attend un pointeur vers char (ou un tableau de char)

### Lecture avec scanf (⚠️ dangereuse)

```c
char nom[50];  
scanf("%s", nom);  // ⚠️ Ne lit qu'un mot (s'arrête à l'espace)  
```

**Problèmes avec scanf :**
1. Ne lit qu'un seul mot (s'arrête au premier espace)
2. Aucune vérification de taille : risque de buffer overflow
3. Ne consomme pas le retour à la ligne

**Exemple :**
```c
char nom[50];  
printf("Votre nom : ");  
scanf("%s", nom);  // Si l'utilisateur entre "Jean Dupont", ne lit que "Jean"  
```

### Lecture sécurisée avec fgets (recommandée)

```c
char nom[50];  
printf("Votre nom : ");  
fgets(nom, sizeof(nom), stdin);  

// Problème : fgets garde le '\n'
// Solution : le retirer manuellement
nom[strcspn(nom, "\n")] = '\0';
```

**Avantages de fgets :**
- Limite la lecture à la taille du buffer
- Lit une ligne complète (avec espaces)
- Plus sûr contre les buffer overflows

## Longueur d'une chaîne

### strlen() : Longueur de la chaîne

```c
#include <string.h>

char message[] = "Bonjour";  
size_t longueur = strlen(message);  // 7 (ne compte pas '\0')  
```

**Important :** `strlen()` compte les caractères **jusqu'au '\0'**, mais ne compte pas le `'\0'` lui-même.

### sizeof() : Taille du tableau

```c
char message[] = "Bonjour";

printf("strlen(message) = %zu\n", strlen(message));  // 7  
printf("sizeof(message) = %zu\n", sizeof(message));  // 8  
```

**Différence cruciale :**
- `strlen()` : longueur de la chaîne (nombre de caractères visibles)
- `sizeof()` : taille totale du tableau en octets (inclut le '\0')

```c
char message[50] = "Bonjour";

strlen(message)   // 7 : nombre de caractères avant '\0'  
sizeof(message)   // 50 : taille totale du tableau  
```

## Passage de chaînes aux fonctions

Comme pour tous les tableaux, passer une chaîne à une fonction revient à passer un pointeur.

```c
void afficher(char *str) {
    printf("%s\n", str);
}

// Équivalent :
void afficher(char str[]) {
    printf("%s\n", str);
}

int main(void) {
    char message[] = "Bonjour";
    afficher(message);  // Passe l'adresse du premier caractère
    return 0;
}
```

### Fonction qui compte les voyelles

```c
#include <stdio.h>
#include <ctype.h>

int compter_voyelles(const char *str) {
    int compte = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        char c = tolower(str[i]);
        if (c == 'a' || c == 'e' || c == 'i' || c == 'o' || c == 'u' || c == 'y') {
            compte++;
        }
    }

    return compte;
}

int main(void) {
    char phrase[] = "Bonjour le monde";
    printf("Voyelles : %d\n", compter_voyelles(phrase));  // 6
    return 0;
}
```

**Note :** Le `const` indique que la fonction ne modifie pas la chaîne.

## Parcourir une chaîne

### Méthode 1 : Avec index et condition sur '\0'

```c
char message[] = "Bonjour";

for (int i = 0; message[i] != '\0'; i++) {
    printf("%c ", message[i]);
}
// Sortie : B o n j o u r
```

### Méthode 2 : Avec strlen()

```c
char message[] = "Bonjour";  
size_t longueur = strlen(message);  

for (size_t i = 0; i < longueur; i++) {
    printf("%c ", message[i]);
}
```

**Attention :** Calculer `strlen()` dans la condition de la boucle est inefficace !

```c
// ❌ Mauvais : strlen() appelé à chaque itération
for (int i = 0; i < strlen(message); i++) {
    // ...
}

// ✅ Bon : strlen() calculé une seule fois
size_t len = strlen(message);  
for (int i = 0; i < len; i++) {  
    // ...
}
```

### Méthode 3 : Avec pointeur

```c
char message[] = "Bonjour";  
char *ptr = message;  

while (*ptr != '\0') {
    printf("%c ", *ptr);
    ptr++;
}
```

## Tableaux de chaînes

Un **tableau de chaînes** est un tableau bidimensionnel de caractères.

```c
char jours[7][10] = {
    "Lundi",
    "Mardi",
    "Mercredi",
    "Jeudi",
    "Vendredi",
    "Samedi",
    "Dimanche"
};
```

**En mémoire :** Chaque chaîne occupe 10 octets (gaspillage pour "Lundi", "Mardi", etc.)

```
jours[0] : "Lundi\0????"      (10 octets)  
jours[1] : "Mardi\0????"      (10 octets)  
jours[2] : "Mercredi\0"       (10 octets)  
...
```

### Alternative : Tableau de pointeurs

```c
const char *jours[] = {
    "Lundi",
    "Mardi",
    "Mercredi",
    "Jeudi",
    "Vendredi",
    "Samedi",
    "Dimanche"
};
```

**Avantages :**
- Pas de gaspillage mémoire
- Chaque chaîne a sa propre longueur
- Les chaînes sont stockées dans la zone lecture seule

**Accès :**

```c
printf("%s\n", jours[0]);     // "Lundi"  
printf("%c\n", jours[0][1]);  // 'u'  
```

## Copie de chaînes

### ❌ On ne peut pas faire ceci :

```c
char destination[20];  
destination = "Bonjour";  // ❌ Erreur : affectation invalide  
```

### ✅ Utiliser strcpy()

```c
#include <string.h>

char source[] = "Bonjour";  
char destination[20];  

strcpy(destination, source);  
printf("%s\n", destination);  // "Bonjour"  
```

**Danger :** `strcpy()` ne vérifie pas la taille du tableau de destination !

```c
char petit[5];  
strcpy(petit, "Bonjour");  // ❌ Buffer overflow !  
```

### ✅ Version sécurisée : strncpy()

```c
char destination[20];  
strncpy(destination, "Bonjour", sizeof(destination) - 1);  
destination[sizeof(destination) - 1] = '\0';  // Garantir la terminaison  
```

**Problème de strncpy :** Ne garantit pas toujours la terminaison par '\0' !

### ✅ Version moderne (C11) : strcpy_s() ou strlcpy()

```c
// strcpy_s (Windows, C11 Annex K)
strcpy_s(destination, sizeof(destination), source);

// strlcpy (BSD, OpenBSD)
strlcpy(destination, source, sizeof(destination));
```

## Concaténation de chaînes

### strcat() : Ajouter à la fin

```c
#include <string.h>

char destination[50] = "Bonjour";  
char source[] = " le monde";  

strcat(destination, source);  
printf("%s\n", destination);  // "Bonjour le monde"  
```

**Conditions :**
- `destination` doit avoir assez d'espace
- `destination` doit être une chaîne valide (terminée par '\0')

### strncat() : Version limitée

```c
char destination[50] = "Bonjour";  
strncat(destination, " le monde", sizeof(destination) - strlen(destination) - 1);  
```

## Comparaison de chaînes

### ❌ On ne peut pas utiliser ==

```c
char str1[] = "Hello";  
char str2[] = "Hello";  

if (str1 == str2) {  // ❌ Compare les ADRESSES, pas le contenu !
    printf("Égales\n");
}
```

### ✅ Utiliser strcmp()

```c
#include <string.h>

char str1[] = "Hello";  
char str2[] = "Hello";  
char str3[] = "World";  

if (strcmp(str1, str2) == 0) {
    printf("str1 et str2 sont identiques\n");  // ✅ Affiche
}

if (strcmp(str1, str3) != 0) {
    printf("str1 et str3 sont différentes\n");  // ✅ Affiche
}
```

**Valeur de retour de strcmp() :**
- `0` : les chaînes sont identiques
- `< 0` : première chaîne est "plus petite" (ordre lexicographique)
- `> 0` : première chaîne est "plus grande"

```c
strcmp("abc", "abc")  // 0  
strcmp("abc", "abd")  // < 0  
strcmp("abd", "abc")  // > 0  
```

### strncmp() : Comparaison limitée

```c
if (strncmp("Hello", "Help", 3) == 0) {
    printf("Les 3 premiers caractères sont identiques\n");  // ✅ Affiche
}
```

## Recherche dans une chaîne

### strchr() : Trouver un caractère

```c
#include <string.h>

char phrase[] = "Bonjour le monde";  
char *position = strchr(phrase, 'o');  

if (position != NULL) {
    printf("Trouvé à l'index : %td\n", position - phrase);  // 1
    printf("Reste : %s\n", position);  // "onjour le monde"
}
```

### strstr() : Trouver une sous-chaîne

```c
char phrase[] = "Bonjour le monde";  
char *position = strstr(phrase, "monde");

if (position != NULL) {
    printf("Trouvé à l'index : %td\n", position - phrase);  // 11
    printf("Reste : %s\n", position);  // "monde"
}
```

## Conversion de chaînes

### De chaîne vers nombre

```c
#include <stdlib.h>

// Conversion en entier
char nombre_str[] = "42";  
int nombre = atoi(nombre_str);  
printf("%d\n", nombre);  // 42  

// Version plus robuste
char *fin;  
long valeur = strtol("123abc", &fin, 10);  
printf("Valeur : %ld\n", valeur);  // 123  
printf("Reste : %s\n", fin);        // "abc"  

// Conversion en flottant
double pi = atof("3.14159");  
printf("%.5f\n", pi);  // 3.14159  
```

### De nombre vers chaîne

```c
#include <stdio.h>

char buffer[50];  
int nombre = 42;  

sprintf(buffer, "%d", nombre);  
printf("Chaîne : %s\n", buffer);  // "42"  

// Version plus sûre (C99)
snprintf(buffer, sizeof(buffer), "%d", nombre);
```

## Chaînes de format avec printf et scanf

### Spécificateurs de format pour chaînes

```c
char nom[] = "Alice";

printf("%s\n", nom);         // Alice  
printf("%10s\n", nom);       // "     Alice" (justifié à droite, largeur 10)  
printf("%-10s\n", nom);      // "Alice     " (justifié à gauche, largeur 10)  
printf("%.3s\n", nom);       // "Ali" (3 premiers caractères)  
printf("%10.3s\n", nom);     // "       Ali" (largeur 10, 3 caractères)  
```

### Lecture limitée avec scanf

```c
char nom[20];  
scanf("%19s", nom);  // Lit au maximum 19 caractères (+ '\0')  
```

## Manipulation caractère par caractère

### Fonctions de <ctype.h>

```c
#include <ctype.h>

char c = 'a';

isalpha(c)   // Vrai si lettre  
isdigit(c)   // Vrai si chiffre  
isalnum(c)   // Vrai si lettre ou chiffre  
isspace(c)   // Vrai si espace, tab, newline...  
isupper(c)   // Vrai si majuscule  
islower(c)   // Vrai si minuscule  

toupper(c)   // Convertit en majuscule  
tolower(c)   // Convertit en minuscule  
```

### Exemple : Convertir en majuscules

```c
#include <stdio.h>
#include <ctype.h>

void to_uppercase(char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper(str[i]);
    }
}

int main(void) {
    char message[] = "Bonjour le Monde";
    to_uppercase(message);
    printf("%s\n", message);  // "BONJOUR LE MONDE"
    return 0;
}
```

## Dangers et pièges courants

### ❌ Erreur 1 : Oublier le '\0'

```c
char message[7] = {'B', 'o', 'n', 'j', 'o', 'u', 'r'};  // ❌ Pas de '\0'  
printf("%s\n", message);  // Comportement indéfini : continue après le tableau  
```

### ❌ Erreur 2 : Buffer overflow

```c
char petit[5];  
strcpy(petit, "Bonjour");  // ❌ Déborde du tableau (7 + '\0' = 8 octets)  
```

**Conséquence :** Corruption de mémoire, plantage, faille de sécurité.

### ❌ Erreur 3 : Modifier une chaîne littérale

```c
char *ptr = "Bonjour";  
ptr[0] = 'b';  // ❌ Comportement indéfini : modification de zone lecture seule  
```

**Solution :** Utilisez `const` ou copiez dans un tableau.

```c
const char *ptr = "Bonjour";  // ✅ const empêche la modification

// Ou :
char tableau[] = "Bonjour";   // ✅ Copie modifiable  
tableau[0] = 'b';              // ✅ OK  
```

### ❌ Erreur 4 : Retourner une chaîne locale

```c
char *creer_message(void) {
    char message[] = "Bonjour";
    return message;  // ❌ DANGEREUX : message est détruit après le retour
}
```

**Solutions valides :**

```c
// Solution 1 : Retourner une chaîne littérale
const char *creer_message(void) {
    return "Bonjour";  // ✅ OK : chaîne littérale statique
}

// Solution 2 : Allouer dynamiquement (malloc)
char *creer_message(void) {
    char *ptr = malloc(20);
    strcpy(ptr, "Bonjour");
    return ptr;  // ✅ OK : mémoire sur le tas
    // ⚠️ N'oubliez pas de free() plus tard !
}

// Solution 3 : Utiliser un buffer statique
char *creer_message(void) {
    static char buffer[20] = "Bonjour";
    return buffer;  // ✅ OK : statique
    // ⚠️ Attention : partagé entre tous les appels
}
```

### ❌ Erreur 5 : Comparer avec == au lieu de strcmp()

```c
char str1[] = "Hello";  
char str2[] = "Hello";  

if (str1 == str2) {  // ❌ Compare les adresses !
    // Ne sera jamais vrai
}

if (strcmp(str1, str2) == 0) {  // ✅ Compare le contenu
    // Ceci fonctionne
}
```

## Bonnes pratiques

### ✅ 1. Toujours utiliser const pour les chaînes en lecture seule

```c
void afficher(const char *message) {
    printf("%s\n", message);
}
```

### ✅ 2. Vérifier les retours de fonctions

```c
char *position = strchr(phrase, 'x');  
if (position != NULL) {  
    // Traitement
} else {
    printf("Caractère non trouvé\n");
}
```

### ✅ 3. Utiliser sizeof() pour les limites

```c
char buffer[50];  
snprintf(buffer, sizeof(buffer), "Valeur : %d", nombre);  
fgets(buffer, sizeof(buffer), stdin);  
```

### ✅ 4. Préférer les fonctions sécurisées

```c
// Au lieu de strcpy
strncpy(dest, src, sizeof(dest) - 1);  
dest[sizeof(dest) - 1] = '\0';  

// Au lieu de strcat
strncat(dest, src, sizeof(dest) - strlen(dest) - 1);

// Au lieu de sprintf
snprintf(buffer, sizeof(buffer), "%s", source);
```

### ✅ 5. Documenter les contraintes de taille

```c
/**
 * Copie un nom d'utilisateur
 * @param dest Buffer de destination (doit avoir au moins 50 octets)
 * @param src  Nom source
 */
void copier_nom(char *dest, const char *src) {
    strncpy(dest, src, 49);
    dest[49] = '\0';
}
```

## Exemple complet : Manipulation de chaînes

```c
#include <stdio.h>
#include <string.h>
#include <ctype.h>

// Compte les mots dans une chaîne
int compter_mots(const char *str) {
    int compte = 0;
    int dans_mot = 0;

    for (int i = 0; str[i] != '\0'; i++) {
        if (isspace(str[i])) {
            dans_mot = 0;
        } else if (dans_mot == 0) {
            dans_mot = 1;
            compte++;
        }
    }

    return compte;
}

// Inverse une chaîne
void inverser(char *str) {
    int longueur = strlen(str);

    for (int i = 0; i < longueur / 2; i++) {
        char temp = str[i];
        str[i] = str[longueur - 1 - i];
        str[longueur - 1 - i] = temp;
    }
}

// Vérifie si une chaîne est un palindrome
int est_palindrome(const char *str) {
    int gauche = 0;
    int droite = strlen(str) - 1;

    while (gauche < droite) {
        // Ignorer les non-lettres
        while (gauche < droite && !isalpha(str[gauche])) {
            gauche++;
        }
        while (gauche < droite && !isalpha(str[droite])) {
            droite--;
        }

        // Comparer (insensible à la casse)
        if (tolower(str[gauche]) != tolower(str[droite])) {
            return 0;
        }

        gauche++;
        droite--;
    }

    return 1;
}

// Retire les espaces en début et fin
void trim(char *str) {
    // Trouver le premier non-espace
    int debut = 0;
    while (isspace(str[debut])) {
        debut++;
    }

    // Trouver le dernier non-espace
    int fin = strlen(str) - 1;
    while (fin >= debut && isspace(str[fin])) {
        fin--;
    }

    // Déplacer et terminer
    int i;
    for (i = 0; debut <= fin; debut++, i++) {
        str[i] = str[debut];
    }
    str[i] = '\0';
}

int main(void) {
    // Test comptage de mots
    char phrase[] = "Bonjour le monde en C";
    printf("Nombre de mots : %d\n", compter_mots(phrase));  // 5

    // Test inversion
    char mot[] = "Bonjour";
    printf("Avant : %s\n", mot);
    inverser(mot);
    printf("Après : %s\n", mot);  // "ruojnoB"

    // Test palindrome
    char pal1[] = "kayak";
    char pal2[] = "A man a plan a canal Panama";
    printf("\"%s\" palindrome ? %s\n", pal1,
           est_palindrome(pal1) ? "Oui" : "Non");  // Oui
    printf("\"%s\" palindrome ? %s\n", pal2,
           est_palindrome(pal2) ? "Oui" : "Non");  // Oui

    // Test trim
    char espaces[] = "   texte   ";
    printf("Avant trim : '%s'\n", espaces);
    trim(espaces);
    printf("Après trim : '%s'\n", espaces);  // "texte"

    return 0;
}
```

## Résumé des points clés

1. **Une chaîne = tableau de char terminé par '\0'**
   - Le `'\0'` est essentiel et automatiquement ajouté avec `"..."`

2. **Différence char vs string**
   - `'A'` : un caractère
   - `"A"` : une chaîne (2 octets : 'A' et '\0')

3. **Chaînes littérales en lecture seule**
   - Utilisez `const char *` pour les pointeurs vers littérales
   - Utilisez un tableau pour copier et modifier

4. **strlen() vs sizeof()**
   - `strlen()` : nombre de caractères avant '\0'
   - `sizeof()` : taille totale du tableau

5. **Ne jamais utiliser strcpy() sans vérification**
   - Préférez `strncpy()` ou `snprintf()`
   - Vérifiez toujours la taille du buffer

6. **strcmp() pour comparer, pas ==**
   - `==` compare les adresses
   - `strcmp()` compare le contenu

7. **Fonctions de <string.h> essentielles**
   - `strlen`, `strcpy`, `strncpy`, `strcat`, `strncat`
   - `strcmp`, `strncmp`, `strchr`, `strstr`

8. **Dangers majeurs**
   - Buffer overflow
   - Chaînes non terminées
   - Modification de littérales
   - Retour de chaînes locales

## Pour aller plus loin

Dans les sections suivantes, nous verrons :
- **Section 8.4** : Manipulation avancée de chaînes
- **Section 8.5** : Dangers des chaînes et buffer overflows
- **Section 8.6** : Fonctions sécurisées
- **Chapitre 9** : Allocation dynamique de chaînes

La maîtrise des chaînes en C est **absolument essentielle**. Elles sont au cœur de nombreux programmes et sources fréquentes de bugs et de vulnérabilités. Prenez le temps de bien comprendre ces concepts !

---

**Note :** Les chaînes en C peuvent être frustrantes au début par rapport à d'autres langages, mais comprendre leur fonctionnement bas niveau vous donnera une excellente compréhension de la gestion mémoire et vous aidera à écrire du code plus sûr et plus efficace.

⏭️ [Manipulation de chaînes](/08-tableaux-et-chaines/04-manipulation-chaines.md)
