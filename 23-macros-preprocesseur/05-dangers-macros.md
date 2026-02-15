🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 23.5 Dangers et pièges des macros

## Introduction

Les macros sont un outil puissant du préprocesseur C, mais elles comportent de nombreux pièges qui peuvent conduire à des bugs subtils et difficiles à détecter. Contrairement aux fonctions, les macros effectuent une **substitution textuelle** sans vérification de type, ce qui peut causer des comportements inattendus.

Ce chapitre explore les dangers les plus courants des macros et comment les éviter. **Comprendre ces pièges est essentiel** pour décider quand utiliser une macro et quand préférer une fonction.

> **Règle d'or** : En cas de doute, préférez une fonction à une macro. Les macros ne devraient être utilisées que lorsqu'elles apportent un avantage réel (performance critique, généricité, compilation conditionnelle).

---

## 1. Évaluation multiple des arguments

### Le piège le plus dangereux

C'est probablement le piège **le plus fréquent et le plus dangereux** avec les macros. Comme le préprocesseur fait une substitution textuelle, si un argument apparaît plusieurs fois dans la macro, il sera **évalué plusieurs fois**.

### Exemple du problème

```c
#include <stdio.h>

#define CARRE(x) ((x) * (x))

int main(void) {
    int compteur = 5;

    printf("Compteur initial : %d\n", compteur);

    int resultat = CARRE(compteur++);

    printf("Résultat : %d\n", resultat);
    printf("Compteur final : %d\n", compteur);

    return 0;
}
```

**Que pensez-vous qu'il va afficher ?**

On pourrait s'attendre à :
```
Compteur initial : 5  
Résultat : 25  
Compteur final : 6  
```

> ⚠️ **Attention** : L'expression `((compteur++) * (compteur++))` modifie `compteur` deux fois sans point de séquence. C'est un **comportement indéfini** (*undefined behavior*). Le résultat varie selon le compilateur et le niveau d'optimisation.

**Un résultat fréquemment observé** :
```
Compteur initial : 5  
Résultat : 30  
Compteur final : 7
```

**Explication d'un scénario possible** :

Le préprocesseur transforme `CARRE(compteur++)` en :
```c
((compteur++) * (compteur++))
```

Le `compteur` est incrémenté **deux fois** : une fois pour chaque occurrence dans la macro !

Détail d'une évaluation possible :
1. Premier `compteur++` : utilise 5, puis incrémente → compteur = 6
2. Deuxième `compteur++` : utilise 6, puis incrémente → compteur = 7
3. Résultat : 5 × 6 = 30

### Autres exemples d'effets de bord

```c
#include <stdio.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int fonction_avec_effet(void) {
    static int appels = 0;
    printf("Fonction appelée %d fois\n", ++appels);
    return appels;
}

int main(void) {
    printf("Résultat : %d\n", MAX(fonction_avec_effet(), 0));

    return 0;
}
```

**Sortie** :
```
Fonction appelée 1 fois  
Fonction appelée 2 fois  
Résultat : 2  
```

La fonction est appelée **deux fois** ! Une fois pour la comparaison, et une autre pour retourner le résultat.

### Comment éviter ce piège

**✅ Solution 1** : Ne jamais utiliser d'expressions avec effets de bord dans les arguments de macros

```c
// ❌ Mauvais
int resultat = CARRE(compteur++);

// ✅ Bon
int temp = compteur++;  
int resultat = CARRE(temp);  
```

**✅ Solution 2** : Utiliser une fonction à la place

```c
// Fonction inline (C99+)
static inline int carre(int x) {
    return x * x;
}

// Maintenant safe avec des effets de bord
int resultat = carre(compteur++);  // compteur incrémenté une seule fois
```

**✅ Solution 3** : Créer une macro plus complexe avec des variables temporaires (GCC/Clang)

```c
#define CARRE(x) ({ \
    __typeof__(x) _x = (x); \
    _x * _x; \
})

// Maintenant safe
int resultat = CARRE(compteur++);  // compteur incrémenté une seule fois
```

⚠️ **Note** : Cette dernière approche utilise des extensions GNU non standard.

---

## 2. Problèmes de précédence d'opérateurs

### Le danger des parenthèses manquantes

Sans parenthèses appropriées, les opérateurs peuvent s'évaluer dans un ordre inattendu.

### Exemple du problème

```c
#include <stdio.h>

// ❌ Macro dangereuse sans parenthèses
#define MAUVAIS_CARRE(x) x * x
#define MAUVAIS_DOUBLE(x) x + x

int main(void) {
    // Test 1
    int resultat1 = MAUVAIS_CARRE(3 + 2);
    printf("MAUVAIS_CARRE(3 + 2) = %d\n", resultat1);
    // On s'attend à (3+2)² = 25
    // On obtient : 3 + 2 * 3 + 2 = 3 + 6 + 2 = 11 ❌

    // Test 2
    int resultat2 = MAUVAIS_DOUBLE(5) * 3;
    printf("MAUVAIS_DOUBLE(5) * 3 = %d\n", resultat2);
    // On s'attend à (5+5)*3 = 30
    // On obtient : 5 + 5 * 3 = 5 + 15 = 20 ❌

    return 0;
}
```

**Explication des erreurs** :

**Cas 1** : `MAUVAIS_CARRE(3 + 2)`
```c
// Expansion : 3 + 2 * 3 + 2
// Évaluation avec priorité : 3 + (2 * 3) + 2 = 11
```

**Cas 2** : `MAUVAIS_DOUBLE(5) * 3`
```c
// Expansion : 5 + 5 * 3
// Évaluation avec priorité : 5 + (5 * 3) = 20
```

### Solution : Toujours utiliser des parenthèses

```c
#include <stdio.h>

// ✅ Macros correctes avec parenthèses
#define BON_CARRE(x) ((x) * (x))
#define BON_DOUBLE(x) ((x) + (x))

int main(void) {
    int resultat1 = BON_CARRE(3 + 2);
    printf("BON_CARRE(3 + 2) = %d\n", resultat1);
    // Expansion : ((3 + 2) * (3 + 2)) = 25 ✓

    int resultat2 = BON_DOUBLE(5) * 3;
    printf("BON_DOUBLE(5) * 3 = %d\n", resultat2);
    // Expansion : ((5) + (5)) * 3 = 30 ✓

    return 0;
}
```

### Règle des parenthèses

**Règle absolue** : Toujours mettre des parenthèses autour :
1. De **chaque occurrence** du paramètre : `(x)`
2. De **toute l'expression** de la macro : `(...)`

```c
// ❌ Dangereux
#define CALCUL(a, b) a + b * 2

// ⚠️ Insuffisant (manque les parenthèses autour des paramètres)
#define CALCUL(a, b) (a + b * 2)

// ✅ Correct
#define CALCUL(a, b) ((a) + (b) * 2)
```

---

## 3. Macros et instructions de contrôle

### Le problème du if sans accolades

```c
#include <stdio.h>

// ❌ Macro problématique
#define AFFICHER_DEBUG(msg) \
    printf("DEBUG: %s\n", msg); \
    printf("Fichier: %s\n", __FILE__)

int main(void) {
    int erreur = 1;

    if (erreur)
        AFFICHER_DEBUG("Une erreur est survenue");
    else
        printf("Tout va bien\n");

    return 0;
}
```

**Erreur de compilation !**

```
error: 'else' without a previous 'if'
```

**Explication** :

Après expansion, le code devient :
```c
if (erreur)
    printf("DEBUG: %s\n", "Une erreur est survenue");
    printf("Fichier: %s\n", "main.c");  // ← Plus dans le if !
else  // ← 'else' sans 'if' correspondant
    printf("Tout va bien\n");
```

Le deuxième `printf` n'est plus dans le `if`, donc le `else` n'a plus de `if` correspondant.

### Solution : L'idiome do-while(0)

```c
#include <stdio.h>

// ✅ Macro correcte avec do-while(0)
#define AFFICHER_DEBUG(msg) \
    do { \
        printf("DEBUG: %s\n", msg); \
        printf("Fichier: %s\n", __FILE__); \
    } while(0)

int main(void) {
    int erreur = 1;

    if (erreur)
        AFFICHER_DEBUG("Une erreur est survenue");
    else
        printf("Tout va bien\n");

    return 0;
}
```

**Maintenant ça fonctionne !**

Le `do-while(0)` permet de :
1. Englober plusieurs instructions en une seule
2. Nécessiter un point-virgule après l'appel
3. Se comporter exactement comme une instruction unique

### Pourquoi do-while(0) et pas juste des accolades ?

```c
// ❌ Avec juste des accolades
#define MACRO(x) { printf("%d\n", x); }

// Utilisation
if (condition)
    MACRO(5);  // { printf("%d\n", 5); };
else
    autre_chose();
```

Le point-virgule après `MACRO(5)` crée une instruction vide qui casse le `else` !

```c
// ✅ Avec do-while(0)
#define MACRO(x) do { printf("%d\n", x); } while(0)

// Utilisation
if (condition)
    MACRO(5);  // do { printf("%d\n", 5); } while(0);
else
    autre_chose();  // ✓ Fonctionne correctement
```

---

## 4. Problèmes de type

### Les macros n'ont pas de type

Contrairement aux fonctions, les macros ne font aucune vérification de type.

```c
#include <stdio.h>

#define DIVISER(a, b) ((a) / (b))

int main(void) {
    // Compilation réussit, mais erreur à l'exécution !
    int resultat = DIVISER(10, 0);  // Division par zéro !
    printf("Résultat : %d\n", resultat);

    return 0;
}
```

**Avec une fonction** :
```c
int diviser(int a, int b) {
    if (b == 0) {
        fprintf(stderr, "Erreur : division par zéro\n");
        return 0;
    }
    return a / b;
}
```

La fonction peut effectuer des vérifications, pas la macro !

### Conversion de types implicite dangereuse

```c
#include <stdio.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main(void) {
    int i = -1;
    unsigned int u = 1;

    // Comportement inattendu !
    printf("MAX(-1, 1u) = %u\n", MAX(i, u));
    // Affiche un grand nombre, pas 1 !

    return 0;
}
```

**Explication** : `-1` est converti en `unsigned int` pour la comparaison, devenant un très grand nombre positif.

Avec une fonction typée :
```c
int max_int(int a, int b) {
    return (a > b) ? a : b;
}

// Le type est clair et cohérent
```

---

## 5. Problèmes de portée et d'espace de noms

### Collision de noms

```c
#define MAX 100

void fonction(void) {
    int max = 50;  // Variable locale

    // Oups ! MAX est remplacé par 100 partout
    if (max < MAX) {
        printf("OK\n");
    }
}
```

Les macros **n'ont pas de portée** ! Elles remplacent du texte partout jusqu'à ce qu'elles soient `#undef`.

### Collision avec des identificateurs

```c
#include <stdio.h>

#define BEGIN {
#define END }

int main(void) BEGIN
    printf("Hello\n");
END  // ← Confusion ! Ressemble à du Pascal, pas du C
```

### Masquage involontaire

```c
#include <stdio.h>
#include <stdlib.h>

// Une bibliothèque définit
#define malloc(size) mon_malloc(size)

// Plus tard dans votre code
void* ma_fonction(void) {
    // Vous pensiez appeler malloc() standard
    // Mais c'est mon_malloc() qui est appelé !
    return malloc(100);
}
```

---

## 6. Débogage difficile

### Les macros sont invisibles pour le débogueur

```c
#include <stdio.h>

#define CALCUL_COMPLEXE(x) ((x) * (x) + (x) * 2 + 1)

int main(void) {
    int valeur = 5;
    int resultat = CALCUL_COMPLEXE(valeur);

    printf("Résultat : %d\n", resultat);

    return 0;
}
```

**En débogage avec GDB** :
- Vous ne pouvez pas mettre un breakpoint **dans** la macro
- Vous ne voyez que le code expansé
- Les messages d'erreur pointent vers le code expansé, pas la macro

### Messages d'erreur confus

```c
#define MACRO_COMPLEXE(x, y) \
    ((x) + (y)) * ((x) - (y)) / ((x) * (y))

int main(void) {
    int a = 5, b = 0;
    int resultat = MACRO_COMPLEXE(a, b);  // Division par zéro !

    return 0;
}
```

**Message d'erreur** (à l'exécution) :
```
Floating point exception (division by zero)
  at line 7
```

Le message ne mentionne pas que l'erreur vient de `MACRO_COMPLEXE` ! Il faut deviner.

### Solution pour le débogage

```c
// Pour le débogage, définir la macro comme une fonction
#ifdef DEBUG
    int calcul_complexe(int x) {
        return x * x + x * 2 + 1;
    }
    #define CALCUL_COMPLEXE(x) calcul_complexe(x)
#else
    #define CALCUL_COMPLEXE(x) ((x) * (x) + (x) * 2 + 1)
#endif
```

Maintenant vous pouvez déboguer la fonction en mode DEBUG !

---

## 7. Expansion récursive et problèmes de précédence

### Macros qui s'appellent elles-mêmes

```c
// ❌ La récursion ne fonctionne pas comme attendu
#define FACT(n) ((n) <= 1 ? 1 : (n) * FACT((n) - 1))

int main(void) {
    // Le préprocesseur détecte la récursion et arrête l'expansion
    int resultat = FACT(5);  // Ne calcule PAS 5!

    return 0;
}
```

Le préprocesseur C **ne supporte pas la récursion** dans les macros ! Il détecte que `FACT` s'appelle elle-même et arrête l'expansion.

### Macros imbriquées complexes

```c
#define A(x) B(x)
#define B(x) C(x)
#define C(x) (x) + 1

// Difficile de suivre et déboguer
int resultat = A(5);
```

Plus vous imbriquez les macros, plus c'est difficile à comprendre et maintenir.

---

## 8. Taille du code (Code Bloat)

### Les macros dupliquent le code

```c
#define CALCUL_COMPLEXE(x) \
    ((x) * (x) * (x) + (x) * (x) + (x) + 1)

int main(void) {
    int a = CALCUL_COMPLEXE(1);
    int b = CALCUL_COMPLEXE(2);
    int c = CALCUL_COMPLEXE(3);
    int d = CALCUL_COMPLEXE(4);
    int e = CALCUL_COMPLEXE(5);

    return 0;
}
```

**Après expansion**, le code est **dupliqué 5 fois** :
```c
int main(void) {
    int a = ((1) * (1) * (1) + (1) * (1) + (1) + 1);
    int b = ((2) * (2) * (2) + (2) * (2) + (2) + 1);
    int c = ((3) * (3) * (3) + (3) * (3) + (3) + 1);
    int d = ((4) * (4) * (4) + (4) * (4) + (4) + 1);
    int e = ((5) * (5) * (5) + (5) * (5) + (5) + 1);

    return 0;
}
```

**Avec une fonction** :
```c
int calcul_complexe(int x) {
    return x * x * x + x * x + x + 1;
}

// Le code de la fonction n'existe qu'UNE fois en mémoire
```

### Impact sur la taille de l'exécutable

Pour une macro complexe utilisée 1000 fois, vous obtenez 1000 copies du code !
- **Augmentation de la taille du binaire**
- **Pollution du cache d'instructions**
- **Compilation plus lente**

---

## 9. Macros et opérateurs

### Macros et l'opérateur sizeof

```c
#include <stdio.h>

#define TAILLE_INT sizeof(int)
#define TABLEAU int tab[10]

int main(void) {
    // ✓ Fonctionne
    printf("Taille : %zu\n", TAILLE_INT);

    // ❌ Problème avec sizeof sur macro de déclaration
    // TABLEAU; // int tab[10];
    // printf("Taille : %zu\n", sizeof(TABLEAU)); // Erreur !

    return 0;
}
```

### Macros et l'opérateur d'adresse &

```c
#define VARIABLE x

int main(void) {
    int x = 10;

    // Fonctionne mais confusion
    int* ptr = &VARIABLE;  // Devient : int* ptr = &x;

    return 0;
}
```

---

## 10. Problèmes spécifiques aux chaînes

### Concaténation de chaînes littérales

```c
#include <stdio.h>

#define MESSAGE "Hello"

int main(void) {
    // ❌ Ne fonctionne pas comme attendu
    printf(MESSAGE " World\n");
    // Fonctionne car les chaînes littérales sont concaténées

    // ❌ Mais ceci ne fonctionne pas
    const char* msg = MESSAGE;
    // printf(msg " World\n");  // Erreur de compilation !

    return 0;
}
```

### Guillemets dans les macros

```c
#include <stdio.h>

#define PRINT(x) printf("x = %d\n", x)

int main(void) {
    int valeur = 42;
    PRINT(valeur);
    // Affiche : x = 42
    // Pas "valeur = 42" comme on pourrait l'espérer !

    return 0;
}
```

Il faut utiliser l'opérateur `#` pour la stringification :
```c
#define PRINT(x) printf(#x " = %d\n", x)
```

---

## 11. Quand NE PAS utiliser de macros

### ❌ N'utilisez PAS de macros pour :

**1. Fonctions simples sans raison de performance**
```c
// ❌ Mauvais
#define add(a, b) ((a) + (b))

// ✅ Meilleur
int add(int a, int b) {
    return a + b;
}

// ✅ Encore mieux (C99+)
static inline int add(int a, int b) {
    return a + b;
}
```

**2. Constantes qui pourraient être des enum**
```c
// ❌ Mauvais
#define LUNDI 1
#define MARDI 2
#define MERCREDI 3

// ✅ Meilleur
enum Jour {
    LUNDI = 1,
    MARDI = 2,
    MERCREDI = 3
};
```

**3. Types qui pourraient être des typedef**
```c
// ❌ Mauvais
#define ENTIER int
#define POINTEUR_ENTIER int*

// ✅ Meilleur
typedef int entier_t;  
typedef int* pointeur_entier_t;  
```

**4. Code complexe difficile à lire**
```c
// ❌ Très mauvais - illisible
#define MEGA_MACRO(a, b, c) \
    do { \
        if ((a) > (b)) { \
            for (int i = 0; i < (c); i++) { \
                printf("%d ", i); \
            } \
        } else { \
            while ((b)-- > 0) { \
                printf("."); \
            } \
        } \
    } while(0)

// ✅ Meilleur - fonction claire
void afficher_selon_condition(int a, int b, int c) {
    if (a > b) {
        for (int i = 0; i < c; i++) {
            printf("%d ", i);
        }
    } else {
        while (b-- > 0) {
            printf(".");
        }
    }
}
```

---

## 12. Alternatives aux macros

### Alternative 1 : Fonctions inline (C99+)

```c
// ✅ Préférez ceci
static inline int carre(int x) {
    return x * x;
}

// Au lieu de
#define CARRE(x) ((x) * (x))
```

**Avantages des fonctions inline** :
- ✅ Vérification de type
- ✅ Pas d'évaluation multiple
- ✅ Peut être déboguée
- ✅ Performances similaires aux macros
- ✅ Portée limitée (static)

### Alternative 2 : Constantes avec const

```c
// ✅ Préférez ceci
const int BUFFER_SIZE = 1024;

// Au lieu de
#define BUFFER_SIZE 1024
```

**Avantages** :
- ✅ A un type
- ✅ Peut être débogué
- ✅ Respecte les règles de portée

> ⚠️ **Attention en C** : Contrairement au C++, `const int` en C n'est **pas** une constante de compilation. On ne peut pas l'utiliser comme taille de tableau statique (`static int arr[BUFFER_SIZE];`), dans un `case`, ni comme largeur de champ de bits. Pour ces cas, `#define` ou `enum` restent nécessaires.

### Alternative 3 : Enum pour les constantes liées

```c
// ✅ Préférez ceci
enum Erreur {
    ERR_SUCCES = 0,
    ERR_MEMOIRE = 1,
    ERR_FICHIER = 2,
    ERR_RESEAU = 3
};

// Au lieu de
#define ERR_SUCCES 0
#define ERR_MEMOIRE 1
#define ERR_FICHIER 2
#define ERR_RESEAU 3
```

### Alternative 4 : _Generic (C11)

Pour la généricité typée :

```c
#include <stdio.h>

// Fonctions spécialisées
int max_int(int a, int b) { return (a > b) ? a : b; }  
double max_double(double a, double b) { return (a > b) ? a : b; }  

// Macro générique avec vérification de type (C11)
#define max(a, b) _Generic((a), \
    int: max_int, \
    double: max_double \
)(a, b)

int main(void) {
    printf("%d\n", max(5, 3));        // Appelle max_int
    printf("%.2f\n", max(5.5, 3.2));  // Appelle max_double

    return 0;
}
```

---

## 13. Checklist de sécurité pour les macros

Avant d'utiliser une macro, posez-vous ces questions :

### ✅ Checklist

- [ ] **Est-ce qu'une fonction (inline) ferait l'affaire ?**
- [ ] **Ai-je mis des parenthèses autour de CHAQUE paramètre ?**
- [ ] **Ai-je mis des parenthèses autour de TOUTE l'expression ?**
- [ ] **Pour les macros multi-instructions, ai-je utilisé do-while(0) ?**
- [ ] **Les arguments peuvent-ils avoir des effets de bord ?**
- [ ] **La macro est-elle documentée avec ses limitations ?**
- [ ] **Le nom est-il en MAJUSCULES pour bien la distinguer ?**
- [ ] **Ai-je testé avec des expressions complexes en argument ?**
- [ ] **Ai-je vérifié le code expansé (gcc -E) ?**
- [ ] **Est-ce que le code reste lisible après utilisation ?**

---

## 14. Bonnes pratiques de défense

### 1. Toujours utiliser le préprocesseur pour vérifier

```bash
# Voir le code après expansion des macros
gcc -E fichier.c

# Ou sauvegarder dans un fichier
gcc -E fichier.c > fichier_expanded.c
```

### 2. Documenter les macros dangereuses

```c
/**
 * @brief Retourne le maximum entre deux valeurs
 * @warning Ne pas utiliser avec des expressions ayant des effets de bord
 * @warning Arguments évalués potentiellement deux fois
 *
 * Exemple DANGEREUX:
 *   MAX(x++, y++)  // le plus grand sera incrémenté deux fois !
 *
 * Exemple SÛR:
 *   MAX(5, 10)
 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
```

### 3. Préfixer les macros pour éviter les collisions

```c
// Au lieu de
#define MAX(a, b) ...
#define MIN(a, b) ...

// Utilisez
#define MONPROJET_MAX(a, b) ...
#define MONPROJET_MIN(a, b) ...
```

### 4. Limiter la portée avec #undef

```c
// Définir une macro locale
#define TEMP_MACRO(x) ((x) * 2)

// Utiliser
int resultat = TEMP_MACRO(5);

// Nettoyer
#undef TEMP_MACRO

// Maintenant TEMP_MACRO n'existe plus
```

### 5. Créer des wrappers safe

```c
// Macro originale (dangereuse)
#define UNSAFE_MAX(a, b) ((a) > (b) ? (a) : (b))

// Wrapper avec variables temporaires (GCC/Clang)
#define SAFE_MAX(a, b) ({ \
    __typeof__(a) _a = (a); \
    __typeof__(b) _b = (b); \
    _a > _b ? _a : _b; \
})

int main(void) {
    int x = 15;

    // Dangereux
    int r1 = UNSAFE_MAX(x++, 10);  // x incrémenté deux fois (15 > 10)

    // Safe
    int r2 = SAFE_MAX(x++, 10);  // x incrémenté une seule fois

    return 0;
}
```

---

## 15. Exemple de refactoring macro → fonction

### Avant (avec macro problématique)

```c
#include <stdio.h>

#define SWAP(a, b) \
    do { \
        int temp = a; \
        a = b; \
        b = temp; \
    } while(0)

int main(void) {
    int x = 5, y = 10;

    SWAP(x, y);

    printf("x = %d, y = %d\n", x, y);

    return 0;
}
```

**Problèmes** :
- ❌ Pas de vérification de type
- ❌ Variable `temp` peut entrer en conflit
- ❌ Difficile à déboguer
- ❌ Ne fonctionne qu'avec int

### Après (avec fonction)

```c
#include <stdio.h>
#include <string.h>

// Fonction générique avec void*
void swap_generic(void* a, void* b, size_t taille) {
    unsigned char temp[taille];  // VLA (C99)
    memcpy(temp, a, taille);
    memcpy(a, b, taille);
    memcpy(b, temp, taille);
}

// Macro wrapper type-safe
#define SWAP(a, b) swap_generic(&(a), &(b), sizeof(a))

int main(void) {
    int x = 5, y = 10;
    SWAP(x, y);
    printf("int: x = %d, y = %d\n", x, y);

    double d1 = 3.14, d2 = 2.71;
    SWAP(d1, d2);
    printf("double: d1 = %.2f, d2 = %.2f\n", d1, d2);

    return 0;
}
```

**Avantages** :
- ✅ Fonctionne avec n'importe quel type
- ✅ Type-safe
- ✅ Peut être déboguée
- ✅ Réutilisable

---

## Résumé des dangers

### Dangers majeurs

| Danger | Description | Solution |
|--------|-------------|----------|
| **Évaluation multiple** | Arguments évalués plusieurs fois | Éviter les effets de bord / Utiliser fonction |
| **Précédence** | Ordre d'évaluation incorrect | TOUJOURS mettre des parenthèses |
| **Instructions multiples** | Problèmes avec if/else | Utiliser do-while(0) |
| **Pas de type** | Aucune vérification | Utiliser fonctions inline |
| **Débogage** | Impossible à déboguer | Préférer les fonctions |
| **Code bloat** | Duplication du code | Fonctions pour code complexe |
| **Portée** | Pollution globale | Limiter avec #undef |

### Recommandations générales

🎯 **Ordre de préférence** :

1. **Fonction normale** (sécurité maximale)
2. **Fonction inline** (sécurité + performance)
3. **Constante (const/enum)** (pour les valeurs)
4. **Macro simple** (seulement si nécessaire)
5. **Macro complexe** (dernier recours, bien documentée)

### La règle d'or

> **"Si vous hésitez entre une macro et une fonction, choisissez la fonction."**

Les macros sont utiles, mais elles sont **dangereuses par nature**. Utilisez-les uniquement quand :
- Les fonctions inline ne suffisent pas
- Vous avez besoin de compilation conditionnelle
- Vous avez besoin de manipuler des tokens (#, ##)
- La généricité est critique et _Generic n'est pas disponible

Dans tous les autres cas, préférez les fonctions !

---

**Prochaine section** : 23.6 Macros prédéfinies utiles (macros standards du préprocesseur)

⏭️ [Macros prédéfinies utiles](/23-macros-preprocesseur/06-macros-predefinies.md)
