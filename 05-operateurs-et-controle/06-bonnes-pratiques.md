🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 5.6 Bonnes pratiques de formatage

## Introduction

Le **formatage du code** n'est pas qu'une question d'esthétique. Un code bien formaté est :
- **Plus facile à lire** et à comprendre
- **Plus maintenable** dans le temps
- **Moins sujet aux erreurs** de logique
- **Plus professionnel** et collaboratif

Dans cette section, nous allons explorer les conventions de formatage les plus courantes en C, ainsi que les raisons pour lesquelles elles sont importantes. Rappelez-vous : **le code est lu beaucoup plus souvent qu'il n'est écrit**.

---

## Pourquoi le formatage est important ?

### Code mal formaté vs bien formaté

**❌ Exemple de code mal formaté :**
```c
#include <stdio.h>
int main(){int x=10;int y=20;int z=x+y;
if(z>25){printf("Grand\n");}else{printf("Petit\n");}
for(int i=0;i<5;i++){printf("%d ",i);}return 0;}
```

**✅ Même code bien formaté :**
```c
#include <stdio.h>

int main() {
    int x = 10;
    int y = 20;
    int z = x + y;

    if (z > 25) {
        printf("Grand\n");
    } else {
        printf("Petit\n");
    }

    for (int i = 0; i < 5; i++) {
        printf("%d ", i);
    }

    return 0;
}
```

La différence est évidente : le second est beaucoup plus facile à lire et à comprendre !

---

## L'indentation

### Principe de base

L'**indentation** consiste à décaler le code vers la droite pour montrer la structure hiérarchique du programme. Chaque niveau de profondeur est indenté d'un cran supplémentaire.

**Convention la plus courante en C : 4 espaces par niveau**

```c
#include <stdio.h>

int main() {                           // Niveau 0
    int x = 10;                        // Niveau 1

    if (x > 5) {                       // Niveau 1
        printf("x est grand\n");       // Niveau 2

        if (x > 8) {                   // Niveau 2
            printf("x est très grand\n");  // Niveau 3
        }
    }

    return 0;                          // Niveau 1
}
```

### Espaces vs Tabulations

Il existe deux écoles :
- **Espaces** : Toujours affichés de la même façon, quel que soit l'éditeur
- **Tabulations** : Peuvent être configurées différemment selon l'éditeur

**💡 Recommandation :**
- Pour les projets personnels : choisissez ce qui vous convient
- Pour les projets d'équipe : suivez la convention établie
- Dans le doute : **4 espaces** est le standard le plus répandu en C

**⚠️ Important :** Ne mélangez jamais espaces et tabulations dans un même fichier !

### Configuration de l'éditeur

La plupart des éditeurs modernes peuvent être configurés pour insérer des espaces quand vous appuyez sur Tab :

**VS Code (settings.json) :**
```json
{
    "editor.insertSpaces": true,
    "editor.tabSize": 4
}
```

**Vim (.vimrc) :**
```vim
set expandtab
set tabstop=4
set shiftwidth=4
```

---

## Les accolades

### Styles courants

Il existe plusieurs styles de placement des accolades. Les deux plus courants en C sont :

**1. Style K&R (Kernighan & Ritchie) - Le plus courant en C**

```c
int main() {
    if (condition) {
        // Code
    } else {
        // Code
    }

    while (condition) {
        // Code
    }
}
```

**Caractéristiques :**
- Accolade ouvrante sur la même ligne que la déclaration de fonction
- Accolade ouvrante sur la même ligne pour `if`, `while`, `for`
- `else` sur la même ligne que l'accolade fermante du `if`

**2. Style Allman - Populaire dans certains projets**

```c
int main()
{
    if (condition)
    {
        // Code
    }
    else
    {
        // Code
    }

    while (condition)
    {
        // Code
    }
}
```

**Caractéristiques :**
- Toutes les accolades sur leur propre ligne
- Plus vertical, prend plus d'espace

**💡 Recommandation :** Le style K&R est le plus utilisé en C et dans le noyau Linux. C'est un bon choix par défaut.

### Toujours utiliser des accolades

Même pour les blocs d'une seule ligne, utilisez toujours des accolades pour éviter les erreurs :

```c
// ❌ DANGEREUX : Sans accolades
if (condition)
    printf("Ligne 1\n");
    printf("Ligne 2\n");  // Cette ligne est TOUJOURS exécutée !

// ✅ SÛR : Avec accolades
if (condition) {
    printf("Ligne 1\n");
    printf("Ligne 2\n");  // Cette ligne est dans le if
}
```

---

## Les espaces

### Espaces autour des opérateurs

Mettez des espaces autour des opérateurs pour améliorer la lisibilité :

```c
// ❌ COMPACT : Difficile à lire
int result=a+b*c-d/e;
if(x>5&&y<10||z==0){

// ✅ LISIBLE : Espaces autour des opérateurs
int result = a + b * c - d / e;
if (x > 5 && y < 10 || z == 0) {
```

### Espaces après les virgules

```c
// ❌ COMPACT
fonction(arg1,arg2,arg3);
int tableau[]={1,2,3,4,5};

// ✅ LISIBLE
fonction(arg1, arg2, arg3);
int tableau[] = {1, 2, 3, 4, 5};
```

### Pas d'espace après les noms de fonction

```c
// ❌ INCORRECT
printf ("Hello");
strlen (chaine);

// ✅ CORRECT
printf("Hello");
strlen(chaine);
```

### Espaces dans les structures de contrôle

```c
// ❌ INCONSISTANT
if(condition){
while (autre_condition){
for(int i=0;i<10;i++){

// ✅ CONSISTANT
if (condition) {
while (autre_condition) {
for (int i = 0; i < 10; i++) {
```

**Règle :** Un espace après `if`, `while`, `for`, `switch`, mais pas après les noms de fonctions.

---

## Les lignes vides

### Séparer les blocs logiques

Utilisez des lignes vides pour séparer les sections logiques de votre code :

```c
#include <stdio.h>

int main() {
    // Déclarations de variables
    int x = 10;
    int y = 20;
    int resultat;

    // Calcul
    resultat = x + y;

    // Affichage
    printf("Résultat : %d\n", resultat);

    return 0;
}
```

**💡 Conseil :** Les lignes vides sont comme les paragraphes dans un texte : elles aident à organiser les idées.

### Ne pas abuser des lignes vides

```c
// ❌ TROP D'ESPACES


int main() {


    int x = 10;


    printf("%d\n", x);


    return 0;


}


// ✅ ÉQUILIBRÉ

int main() {
    int x = 10;

    printf("%d\n", x);

    return 0;
}
```

---

## Longueur des lignes

### Limite recommandée : 80 caractères

Historiquement, 80 caractères était la largeur standard des terminaux. Aujourd'hui, beaucoup utilisent 100 ou 120 caractères.

**💡 Recommandation :** 80-100 caractères maximum par ligne.

**Pourquoi limiter la longueur ?**
- Facilite la lecture (l'œil ne doit pas balayer trop loin)
- Permet d'afficher plusieurs fichiers côte à côte
- Plus facile à afficher sur différents appareils

### Casser les lignes longues

```c
// ❌ TROP LONG
int resultat = fonction_avec_nom_tres_long(premier_argument, deuxieme_argument, troisieme_argument, quatrieme_argument);

// ✅ CASSÉ SUR PLUSIEURS LIGNES
int resultat = fonction_avec_nom_tres_long(premier_argument,
                                            deuxieme_argument,
                                            troisieme_argument,
                                            quatrieme_argument);

// ✅ ALTERNATIVE
int resultat = fonction_avec_nom_tres_long(
    premier_argument,
    deuxieme_argument,
    troisieme_argument,
    quatrieme_argument
);
```

### Casser les conditions longues

```c
// ❌ TROP LONG
if (condition_tres_longue_numero_1 && condition_tres_longue_numero_2 && condition_tres_longue_numero_3) {

// ✅ CASSÉ LOGIQUEMENT
if (condition_tres_longue_numero_1 &&
    condition_tres_longue_numero_2 &&
    condition_tres_longue_numero_3) {

// ✅ ALTERNATIVE AVEC EXTRACTION
bool condition1 = condition_tres_longue_numero_1;
bool condition2 = condition_tres_longue_numero_2;
bool condition3 = condition_tres_longue_numero_3;

if (condition1 && condition2 && condition3) {
```

---

## Conventions de nommage

### Variables

**Convention snake_case (la plus courante en C) :**

```c
// ✅ BON
int nombre_utilisateurs;
float temperature_moyenne;
char nom_fichier[100];

// ❌ À ÉVITER (camelCase, plus courant en Java/JavaScript)
int nombreUtilisateurs;
float temperatureMoyenne;
```

**Noms descriptifs :**

```c
// ❌ PEU CLAIR
int x;
int n;
float t;

// ✅ CLAIR
int nombre_etudiants;
int compteur_erreurs;
float temperature_celsius;
```

**Éviter les noms d'une seule lettre (sauf pour les compteurs de boucle) :**

```c
// ✅ ACCEPTABLE pour les boucles simples
for (int i = 0; i < 10; i++) {
    for (int j = 0; j < 5; j++) {
        // ...
    }
}

// ✅ MEILLEUR pour les boucles complexes
for (int ligne = 0; ligne < nb_lignes; ligne++) {
    for (int colonne = 0; colonne < nb_colonnes; colonne++) {
        // ...
    }
}
```

### Constantes

**Convention : MAJUSCULES avec underscores :**

```c
// ✅ BON
#define PI 3.14159
#define MAX_BUFFER_SIZE 1024
#define NOMBRE_ETUDIANTS 50

const int TAILLE_TABLEAU = 100;
```

### Fonctions

**Convention : snake_case, verbes d'action :**

```c
// ✅ BON
int calculer_moyenne(int *tableau, int taille);
void afficher_menu();
bool est_valide(int valeur);
char* obtenir_nom_utilisateur();

// ❌ PEU CLAIR
int calc(int *t, int n);
void menu();
```

### Types personnalisés (structures, enums)

**Convention : snake_case avec suffixe _t :**

```c
// ✅ BON
typedef struct {
    char nom[50];
    int age;
} personne_t;

typedef enum {
    ROUGE,
    VERT,
    BLEU
} couleur_t;
```

### Préfixes pour la portée

```c
// Variables globales (à éviter généralement)
int g_compteur_global;

// Variables statiques
static int s_compteur_module;

// Pointeurs (optionnel)
int *p_tableau;
```

---

## Les commentaires

### Quand commenter

**✅ Commentez :**
- L'intention et le "pourquoi", pas le "quoi"
- Les algorithmes complexes
- Les comportements non évidents
- Les limitations connues
- Les TODOs et FIXMEs

**❌ N'en faites pas trop :**
- Le code évident
- Répéter ce que le code fait déjà clairement

### Exemples de bons commentaires

```c
// ✅ BON : Explique le "pourquoi"
// On utilise un buffer de 1024 octets car c'est la taille de bloc
// optimale pour ce système de fichiers
char buffer[1024];

// ✅ BON : Explique une subtilité
// Attention : i commence à 1 car l'index 0 est réservé pour l'en-tête
for (int i = 1; i < taille; i++) {

// ✅ BON : Marque un problème connu
// TODO: Optimiser cette boucle O(n²) en O(n log n)
for (int i = 0; i < n; i++) {
    for (int j = 0; j < n; j++) {
```

### Exemples de commentaires inutiles

```c
// ❌ MAUVAIS : Répète le code
// Incrémenter i
i++;

// ❌ MAUVAIS : État évident
// Boucle de 0 à 9
for (int i = 0; i < 10; i++) {

// ❌ MAUVAIS : Commentaire obsolète
// Cette fonction utilise un tri rapide
void trier(int *tableau, int taille) {
    // En réalité, code utilise un tri à bulles
}
```

### Style des commentaires

**Commentaires sur une ligne :**
```c
// Ceci est un commentaire sur une ligne
int x = 10;  // Commentaire en fin de ligne
```

**Commentaires multi-lignes :**
```c
/*
 * Ceci est un commentaire
 * sur plusieurs lignes
 */

/* Alternative simple */
```

**Documentation de fonction (style Doxygen) :**
```c
/**
 * @brief Calcule la moyenne d'un tableau d'entiers
 *
 * @param tableau Pointeur vers le tableau d'entiers
 * @param taille Nombre d'éléments dans le tableau
 * @return La moyenne des valeurs, ou 0.0 si le tableau est vide
 */
float calculer_moyenne(int *tableau, int taille) {
    if (taille == 0) {
        return 0.0;
    }

    int somme = 0;
    for (int i = 0; i < taille; i++) {
        somme += tableau[i];
    }

    return (float)somme / taille;
}
```

---

## Organisation du fichier source

### Structure typique d'un fichier .c

```c
/* 1. Commentaire d'en-tête (optionnel) */
/*
 * Nom du fichier : calculatrice.c
 * Description : Fonctions de calcul mathématique
 * Auteur : Votre Nom
 * Date : 2025-01-15
 */

/* 2. Inclusion des headers système */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* 3. Inclusion des headers locaux */
#include "calculatrice.h"
#include "utilitaires.h"

/* 4. Définitions de constantes */
#define PI 3.14159
#define MAX_TAILLE 100

/* 5. Définitions de types */
typedef struct {
    int x;
    int y;
} point_t;

/* 6. Déclarations de variables globales (à minimiser) */
static int compteur_global = 0;

/* 7. Prototypes de fonctions statiques (locales au fichier) */
static void fonction_helper(void);
static int calcul_interne(int a, int b);

/* 8. Définitions des fonctions */

/**
 * @brief Fonction principale
 */
int main(void) {
    printf("Programme démarré\n");

    fonction_helper();

    return 0;
}

/**
 * @brief Fonction helper locale
 */
static void fonction_helper(void) {
    // Implémentation
}

/**
 * @brief Calcul interne
 */
static int calcul_interne(int a, int b) {
    return a + b;
}
```

### Structure typique d'un fichier .h

```c
/* 1. Garde d'inclusion */
#ifndef CALCULATRICE_H
#define CALCULATRICE_H

/* 2. Inclusion des dépendances */
#include <stdbool.h>

/* 3. Définitions de constantes */
#define VERSION "1.0.0"

/* 4. Définitions de types */
typedef struct {
    float x;
    float y;
} coordonnee_t;

/* 5. Prototypes de fonctions publiques */
int additionner(int a, int b);
float calculer_moyenne(int *tableau, int taille);
bool valider_entree(int valeur);

/* 6. Fin de la garde d'inclusion */
#endif /* CALCULATRICE_H */
```

---

## Exemples complets : Avant/Après

### Exemple 1 : Fonction de validation

**❌ Avant (mal formaté) :**
```c
#include <stdio.h>
#include <stdbool.h>
bool valider(int age,char*nom){if(age<0||age>120)return false;
if(nom==NULL)return false;int i=0;while(nom[i]!='\0'){i++;}
if(i==0||i>50)return false;return true;}
int main(){int a=25;char n[]="Jean";if(valider(a,n))printf("OK\n");
else printf("Erreur\n");return 0;}
```

**✅ Après (bien formaté) :**
```c
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#define AGE_MIN 0
#define AGE_MAX 120
#define LONGUEUR_NOM_MAX 50

/**
 * @brief Valide les données d'une personne
 *
 * @param age Âge de la personne
 * @param nom Nom de la personne
 * @return true si valide, false sinon
 */
bool valider_personne(int age, char *nom) {
    // Vérifier l'âge
    if (age < AGE_MIN || age > AGE_MAX) {
        return false;
    }

    // Vérifier que le nom existe
    if (nom == NULL) {
        return false;
    }

    // Vérifier la longueur du nom
    size_t longueur = strlen(nom);
    if (longueur == 0 || longueur > LONGUEUR_NOM_MAX) {
        return false;
    }

    return true;
}

int main(void) {
    int age = 25;
    char nom[] = "Jean";

    if (valider_personne(age, nom)) {
        printf("Données valides\n");
    } else {
        printf("Données invalides\n");
    }

    return 0;
}
```

### Exemple 2 : Tri à bulles

**❌ Avant (mal formaté) :**
```c
void tri(int*t,int n){for(int i=0;i<n-1;i++){for(int j=0;j<n-i-1;j++)
{if(t[j]>t[j+1]){int tmp=t[j];t[j]=t[j+1];t[j+1]=tmp;}}}}
```

**✅ Après (bien formaté) :**
```c
/**
 * @brief Trie un tableau d'entiers en ordre croissant
 *        Utilise l'algorithme du tri à bulles (O(n²))
 *
 * @param tableau Pointeur vers le tableau à trier
 * @param taille Nombre d'éléments dans le tableau
 */
void tri_a_bulles(int *tableau, int taille) {
    // Pour chaque passage
    for (int i = 0; i < taille - 1; i++) {
        // Comparer les éléments adjacents
        for (int j = 0; j < taille - i - 1; j++) {
            // Échanger si dans le mauvais ordre
            if (tableau[j] > tableau[j + 1]) {
                int temporaire = tableau[j];
                tableau[j] = tableau[j + 1];
                tableau[j + 1] = temporaire;
            }
        }
    }
}
```

---

## Styles de formatage populaires

### 1. Style Linux Kernel (K&R variant)

```c
int fonction(int param)
{
        int local = 0;  // Tabulation de 8 espaces

        if (condition) {
                // Code
        } else {
                // Code
        }

        return local;
}
```

**Caractéristiques :**
- Tabulations de 8 espaces
- Accolades sur la même ligne pour tout sauf les fonctions
- Très répandu dans les projets système Linux

### 2. Style GNU

```c
int
fonction (int param)
{
  int local = 0;  // 2 espaces

  if (condition)
    {
      // Code
    }
  else
    {
      // Code
    }

  return local;
}
```

**Caractéristiques :**
- Type de retour sur sa propre ligne
- 2 espaces d'indentation
- Espaces avant les parenthèses

### 3. Style K&R classique (recommandé pour débuter)

```c
int fonction(int param) {
    int local = 0;  // 4 espaces

    if (condition) {
        // Code
    } else {
        // Code
    }

    return local;
}
```

**Caractéristiques :**
- 4 espaces d'indentation
- Accolades sur la même ligne
- Simple et propre

**💡 Recommandation :** Choisissez un style et soyez **cohérent** dans tout votre projet.

---

## Configuration automatique

### Fichier .editorconfig

Créez un fichier `.editorconfig` à la racine de votre projet pour assurer la cohérence :

```ini
# Configuration pour tous les fichiers
[*]
charset = utf-8
end_of_line = lf
insert_final_newline = true
trim_trailing_whitespace = true

# Fichiers C
[*.{c,h}]
indent_style = space
indent_size = 4
max_line_length = 100
```

### Exemple de configuration VS Code

```json
{
    "editor.insertSpaces": true,
    "editor.tabSize": 4,
    "editor.rulers": [80, 100],
    "editor.trimAutoWhitespace": true,
    "files.insertFinalNewline": true,
    "files.trimTrailingWhitespace": true,

    "[c]": {
        "editor.defaultFormatter": "xaver.clang-format",
        "editor.formatOnSave": true
    }
}
```

---

## Checklist des bonnes pratiques

### ✅ Structure générale
- [ ] Indentation cohérente (4 espaces recommandé)
- [ ] Un seul style d'accolades dans tout le projet
- [ ] Lignes de maximum 80-100 caractères
- [ ] Lignes vides pour séparer les blocs logiques
- [ ] Pas de lignes vides excessives

### ✅ Espacement
- [ ] Espaces autour des opérateurs (`a + b` et non `a+b`)
- [ ] Espaces après les virgules (`func(a, b)` et non `func(a,b)`)
- [ ] Espace après les mots-clés (`if (condition)` et non `if(condition)`)
- [ ] Pas d'espace après les noms de fonctions (`printf()` et non `printf ()`)

### ✅ Nommage
- [ ] Variables : snake_case descriptif
- [ ] Constantes : MAJUSCULES_AVEC_UNDERSCORES
- [ ] Fonctions : snake_case avec verbes d'action
- [ ] Pas de noms d'une lettre (sauf compteurs simples)

### ✅ Commentaires
- [ ] Commentaires pour expliquer le "pourquoi", pas le "quoi"
- [ ] Documentation des fonctions complexes
- [ ] Pas de commentaires obsolètes
- [ ] TODOs marqués clairement

### ✅ Organisation
- [ ] Headers système avant headers locaux
- [ ] Constantes avant les fonctions
- [ ] Prototypes des fonctions statiques
- [ ] Gardes d'inclusion dans les .h

---

## Erreurs de formatage courantes

### 1. Mélange d'espaces et de tabulations

```c
// ❌ MAUVAIS : Mélange espaces et tabs (invisible mais problématique)
int main() {
    int x = 10;  // 4 espaces
	int y = 20;  // 1 tabulation
    return 0;    // 4 espaces
}
```

**Solution :** Configurez votre éditeur pour utiliser soit uniquement des espaces, soit uniquement des tabs.

### 2. Espaces en fin de ligne

```c
// ❌ MAUVAIS : Espaces invisibles après le code
int x = 10;

return 0;
```

**Solution :** Configurez votre éditeur pour supprimer automatiquement les espaces en fin de ligne.

### 3. Pas de ligne vide en fin de fichier

```c
// ❌ MAUVAIS : Le fichier se termine par }
    return 0;
}
```

```c
// ✅ BON : Une ligne vide à la fin
    return 0;
}
↵
```

**Pourquoi ?** C'est une convention POSIX et certains compilateurs émettent des warnings.

### 4. Incohérence dans le style

```c
// ❌ MAUVAIS : Mélange de styles
int fonction1(int x)
{
    if (x > 0) {  // Style mixte !
        return x;
    }
}

int fonction2(int y) {
    if(y>0){  // Pas d'espaces
        return y;
    }
}
```

**Solution :** Choisissez UN style et tenez-vous-y.

---

## Outils pour maintenir un bon formatage

### 1. Formateurs automatiques

- **clang-format** : Le plus populaire pour C/C++
- **indent** : Outil GNU classique
- **astyle** : Artistic Style

**Exemple avec clang-format :**
```bash
# Formater un fichier
clang-format -i mon_fichier.c

# Formater tous les fichiers .c et .h
find . -name "*.c" -o -name "*.h" | xargs clang-format -i
```

### 2. Linters

- **clang-tidy** : Analyse statique avancée
- **cppcheck** : Détection d'erreurs
- **splint** : Vérification de style

### 3. Hooks Git

Créez un hook pre-commit pour vérifier le formatage avant chaque commit :

```bash
#!/bin/bash
# .git/hooks/pre-commit

# Vérifier le formatage avec clang-format
for file in $(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|h)$')
do
    clang-format --dry-run --Werror "$file"
    if [ $? -ne 0 ]; then
        echo "Erreur de formatage dans $file"
        echo "Exécutez: clang-format -i $file"
        exit 1
    fi
done
```

---

## Résumé

### Principes fondamentaux

1. **Cohérence** : Le plus important. Choisissez un style et tenez-vous-y.
2. **Lisibilité** : Le code est lu plus souvent qu'écrit.
3. **Simplicité** : Ne compliquez pas inutilement.

### Points clés

| Aspect | Recommandation |
|--------|---------------|
| **Indentation** | 4 espaces |
| **Accolades** | Style K&R (sur la même ligne) |
| **Longueur de ligne** | 80-100 caractères max |
| **Nommage variables** | snake_case |
| **Nommage constantes** | MAJUSCULES |
| **Espaces** | Autour des opérateurs et après virgules |
| **Commentaires** | Expliquer le "pourquoi", pas le "quoi" |

### Règle d'or

> **"Tout code devrait sembler avoir été écrit par une seule personne, peu importe le nombre de contributeurs."**
>
> — Code Style Guide

---

## Pour aller plus loin

Dans les prochaines sections, vous découvrirez :
- **5.7 Formatage automatique avec clang-format** : Configuration et utilisation pratique
- **Chapitre 12** : Organisation du code et compilation modulaire
- **Chapitre 32** : Documentation et maintenance

Un code bien formaté est un code professionnel. Prenez l'habitude dès maintenant d'écrire du code propre, et cela deviendra naturel. Vos collègues (et votre futur vous) vous remercieront !

⏭️ [Formatage automatique avec clang-format](/05-operateurs-et-controle/07-clang-format.md)
