🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 23.1 Macros paramétrées

## Introduction

Les macros paramétrées sont des macros qui acceptent des arguments, similaires aux fonctions, mais qui sont traitées par le préprocesseur avant la compilation. Elles permettent de créer du code réutilisable et générique, tout en offrant potentiellement de meilleures performances que les fonctions traditionnelles.

> **Note importante** : Bien que puissantes, les macros paramétrées doivent être utilisées avec précaution. Dans la plupart des cas, une fonction classique est préférable pour sa sécurité de type et sa facilité de débogage.

## Syntaxe de base

Une macro paramétrée se déclare avec la directive `#define` suivie d'un nom, de paramètres entre parenthèses, et d'un corps de macro :

```c
#define NOM_MACRO(param1, param2) corps_de_la_macro
```

### Premier exemple simple

```c
#include <stdio.h>

// Macro qui calcule le carré d'un nombre
#define CARRE(x) ((x) * (x))

int main(void) {
    int a = 5;
    int resultat = CARRE(a);

    printf("Le carré de %d est %d\n", a, resultat);
    // Affiche : Le carré de 5 est 25

    return 0;
}
```

### Comment ça fonctionne ?

Le préprocesseur remplace textuellement `CARRE(a)` par `((a) * (a))` **avant** la compilation. C'est une substitution de texte pure et simple, pas un appel de fonction.

Si vous écrivez :
```c
int resultat = CARRE(5);
```

Le préprocesseur transforme cela en :
```c
int resultat = ((5) * (5));
```

## Pourquoi mettre des parenthèses ?

C'est une règle **cruciale** : toujours entourer les paramètres et le corps de la macro de parenthèses.

### Exemple du problème sans parenthèses

```c
#include <stdio.h>

// ❌ MAUVAIS : Sans parenthèses
#define MAUVAIS_CARRE(x) x * x

int main(void) {
    int resultat = MAUVAIS_CARRE(2 + 3);
    printf("Résultat : %d\n", resultat);
    // On s'attend à (2+3)² = 25
    // Mais on obtient 11 !

    return 0;
}
```

**Explication du bug** :
Le préprocesseur effectue la substitution :
```c
int resultat = 2 + 3 * 2 + 3;
```

Avec la priorité des opérateurs : `2 + (3 * 2) + 3 = 2 + 6 + 3 = 11` ❌

### Solution correcte

```c
// ✅ BON : Avec parenthèses
#define BON_CARRE(x) ((x) * (x))

int main(void) {
    int resultat = BON_CARRE(2 + 3);
    // Devient : ((2 + 3) * (2 + 3)) = 25 ✓

    return 0;
}
```

## Macros avec plusieurs paramètres

Les macros peuvent accepter plusieurs paramètres, séparés par des virgules.

```c
#include <stdio.h>

// Macro qui retourne le maximum de deux valeurs
#define MAX(a, b) ((a) > (b) ? (a) : (b))

// Macro qui retourne le minimum de deux valeurs
#define MIN(a, b) ((a) < (b) ? (a) : (b))

int main(void) {
    int x = 10, y = 20;

    printf("Max entre %d et %d : %d\n", x, y, MAX(x, y));
    // Affiche : Max entre 10 et 20 : 20

    printf("Min entre %d et %d : %d\n", x, y, MIN(x, y));
    // Affiche : Min entre 10 et 20 : 10

    return 0;
}
```

### Exemple : Macro de conversion

```c
#include <stdio.h>

// Conversion Celsius vers Fahrenheit
#define C_VERS_F(celsius) (((celsius) * 9.0 / 5.0) + 32.0)

// Conversion Fahrenheit vers Celsius
#define F_VERS_C(fahrenheit) (((fahrenheit) - 32.0) * 5.0 / 9.0)

int main(void) {
    double temp_c = 25.0;
    double temp_f = C_VERS_F(temp_c);

    printf("%.1f°C = %.1f°F\n", temp_c, temp_f);
    // Affiche : 25.0°C = 77.0°F

    return 0;
}
```

## Macros multi-lignes

Pour créer des macros qui s'étendent sur plusieurs lignes, on utilise le caractère de continuation `\` à la fin de chaque ligne (sauf la dernière).

```c
#include <stdio.h>

#define AFFICHER_INFOS(nom, age) \
    do { \
        printf("Nom : %s\n", nom); \
        printf("Âge : %d\n", age); \
    } while(0)

int main(void) {
    AFFICHER_INFOS("Alice", 30);
    // Affiche :
    // Nom : Alice
    // Âge : 30

    return 0;
}
```

### L'idiome `do { ... } while(0)`

Vous remarquerez le pattern `do { ... } while(0)`. C'est une technique standard pour créer des macros multi-instructions qui se comportent comme une seule instruction.

**Pourquoi est-ce nécessaire ?**

```c
// ❌ Sans do-while
#define MAUVAISE_MACRO(x) \
    printf("Valeur : %d\n", x); \
    printf("Carré : %d\n", (x) * (x))

// Utilisation avec un if
if (condition)
    MAUVAISE_MACRO(5);
else
    autre_chose();

// Après expansion, devient :
if (condition)
    printf("Valeur : %d\n", 5);
    printf("Carré : %d\n", (5) * (5));  // ❌ Hors du if !
else
    autre_chose();
```

**Solution avec do-while :**

```c
// ✅ Avec do-while
#define BONNE_MACRO(x) \
    do { \
        printf("Valeur : %d\n", x); \
        printf("Carré : %d\n", (x) * (x)); \
    } while(0)

// Maintenant ça fonctionne correctement
if (condition)
    BONNE_MACRO(5);  // Traité comme une seule instruction
else
    autre_chose();
```

## Avantages des macros paramétrées

### 1. Pas d'overhead d'appel de fonction

```c
// Fonction classique
int carre_fonction(int x) {
    return x * x;
}

// Macro
#define CARRE_MACRO(x) ((x) * (x))

// La macro est directement remplacée dans le code
// Pas d'appel de fonction, pas de stack frame
```

### 2. Généricité (typage polymorphe)

Les macros fonctionnent avec n'importe quel type compatible :

```c
#include <stdio.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))

int main(void) {
    int i1 = 10, i2 = 20;
    printf("Max int : %d\n", MAX(i1, i2));  // Fonctionne avec int

    double d1 = 3.14, d2 = 2.71;
    printf("Max double : %.2f\n", MAX(d1, d2));  // Fonctionne avec double

    char c1 = 'A', c2 = 'Z';
    printf("Max char : %c\n", MAX(c1, c2));  // Fonctionne avec char

    return 0;
}
```

### 3. Évaluation au moment de la compilation

Avec des constantes, le calcul est fait à la compilation :

```c
#define TAILLE_BUFFER (1024 * 1024)  // 1 Mo
char buffer[TAILLE_BUFFER];  // Taille connue à la compilation
```

## Dangers et pièges des macros

### 1. Évaluation multiple des paramètres

**Le piège le plus courant :**

```c
#define CARRE(x) ((x) * (x))

int compteur = 5;  
int resultat = CARRE(compteur++);
// Devient : ((compteur++) * (compteur++))
// compteur est modifié DEUX fois sans point de séquence !
// C'est un COMPORTEMENT INDÉFINI (undefined behavior)

printf("Résultat : %d, Compteur : %d\n", resultat, compteur);
// Résultat imprévisible ! Le compilateur peut produire n'importe quoi.
```

**Explication** : Le paramètre `compteur++` apparaît deux fois dans la macro, donc il est évalué deux fois.

**Solution** : Ne jamais utiliser d'effets de bord dans les arguments de macros.

```c
// ✅ Bon usage
int compteur = 5;  
int temp = compteur++;  
int resultat = CARRE(temp);  
```

### 2. Problèmes de précédence d'opérateurs

Sans parenthèses appropriées :

```c
#define DOUBLE(x) x + x

int resultat = DOUBLE(3) * 2;
// Devient : 3 + 3 * 2 = 3 + 6 = 9
// Au lieu de : (3 + 3) * 2 = 12

// ✅ Solution
#define DOUBLE(x) ((x) + (x))
```

### 3. Problèmes de type

Les macros n'effectuent aucune vérification de type :

```c
#define DIVISER(a, b) ((a) / (b))

int resultat = DIVISER(10, 0);  // Division par zéro !
// Le compilateur ne détecte pas l'erreur
```

### 4. Messages d'erreur confus

En cas d'erreur, le débogueur et les messages d'erreur pointent vers le code expansé, pas vers la macro originale.

```c
#define CALCUL_COMPLEXE(x) ((x) * (x) + (x) / (x))

int resultat = CALCUL_COMPLEXE(0);
// Erreur de division par zéro, mais le message ne mentionne pas la macro
```

## Bonnes pratiques

### 1. Nommage

Par convention, les macros s'écrivent en **MAJUSCULES** pour les distinguer des fonctions :

```c
#define MAX(a, b) ((a) > (b) ? (a) : (b))  // ✅ Bon
#define max(a, b) ((a) > (b) ? (a) : (b))  // ❌ Peut être confondu avec une fonction
```

### 2. Toujours utiliser des parenthèses

```c
// ✅ Bon
#define MACRO(x, y) (((x) + (y)) * 2)

// ❌ Mauvais
#define MACRO(x, y) x + y * 2
```

### 3. Préférer les fonctions inline (C99+)

Pour les opérations simples, les fonctions `inline` offrent la sécurité des fonctions avec les performances des macros :

```c
// Alternative moderne à une macro
static inline int carre(int x) {
    return x * x;
}

// Avantages :
// - Vérification de type
// - Pas d'évaluation multiple
// - Meilleurs messages d'erreur
// - Peut être débogué normalement
```

### 4. Documenter les macros

```c
/**
 * @brief Calcule le maximum entre deux valeurs
 * @warning Ne pas utiliser d'expressions avec effets de bord
 * @param a Première valeur
 * @param b Deuxième valeur
 * @return La plus grande des deux valeurs
 */
#define MAX(a, b) ((a) > (b) ? (a) : (b))
```

### 5. Utiliser des macros pour les valeurs constantes

C'est l'usage le plus sûr :

```c
#define TAILLE_MAX_NOM 256
#define VERSION_MAJEURE 2
#define VERSION_MINEURE 5
#define PI 3.14159265359
```

## Cas d'usage appropriés

### 1. Constantes manifestes

```c
#define BUFFER_SIZE 1024
#define MAX_CONNECTIONS 100
#define TIMEOUT_MS 5000
```

### 2. Macros de débogage

```c
#ifdef DEBUG
    #define DEBUG_PRINT(fmt, ...) \
        fprintf(stderr, "[DEBUG] %s:%d: " fmt "\n", \
                __FILE__, __LINE__, ##__VA_ARGS__)
#else
    #define DEBUG_PRINT(fmt, ...) do {} while(0)
#endif

// Utilisation
DEBUG_PRINT("Valeur de x: %d", x);
```

### 3. Macros de gestion d'erreurs

```c
#define CHECK_NULL(ptr) \
    do { \
        if ((ptr) == NULL) { \
            fprintf(stderr, "Erreur : pointeur NULL à %s:%d\n", \
                    __FILE__, __LINE__); \
            return -1; \
        } \
    } while(0)

// Utilisation
int* data = malloc(sizeof(int) * 100);  
CHECK_NULL(data);  
```

### 4. Abstraction de code répétitif

```c
#define DECLARER_GETTER_SETTER(type, nom) \
    type get_##nom(void) { return nom; } \
    void set_##nom(type valeur) { nom = valeur; }

// Génère automatiquement les getters/setters
static int age;  
DECLARER_GETTER_SETTER(int, age)  

// Équivalent à :
// int get_age(void) { return age; }
// void set_age(int valeur) { age = valeur; }
```

## Comparaison : Macro vs Fonction

| Critère | Macro | Fonction |
|---------|-------|----------|
| Performance | Pas d'overhead d'appel | Overhead d'appel (léger) |
| Vérification de type | ❌ Aucune | ✅ Stricte |
| Débogage | ❌ Difficile | ✅ Facile |
| Taille du code | Peut augmenter | Code unique |
| Généricité | ✅ Tous types | ❌ Type fixe (sauf C11+ _Generic) |
| Évaluation multiple | ⚠️ Risque | ✅ Pas de risque |
| Sécurité | ⚠️ Attention requise | ✅ Sûre |

## Exemple complet commenté

```c
#include <stdio.h>

// Constantes
#define TAILLE_MAX 100
#define VERSION "1.0.0"

// Macro simple avec parenthèses
#define CARRE(x) ((x) * (x))

// Macro multi-paramètres
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

// Macro multi-ligne avec do-while
#define AFFICHER_SEPARATEUR() \
    do { \
        printf("========================\n"); \
    } while(0)

// Macro de débogage conditionnelle
#ifdef DEBUG
    #define LOG(msg) printf("[LOG] %s\n", msg)
#else
    #define LOG(msg) do {} while(0)
#endif

int main(void) {
    printf("Programme version %s\n", VERSION);
    AFFICHER_SEPARATEUR();

    // Utilisation de CARRE
    int nombre = 7;
    printf("Le carré de %d est %d\n", nombre, CARRE(nombre));

    // Utilisation de MAX/MIN
    int a = 15, b = 23;
    printf("Max(%d, %d) = %d\n", a, b, MAX(a, b));
    printf("Min(%d, %d) = %d\n", a, b, MIN(a, b));

    // Les macros fonctionnent avec différents types
    double x = 3.14, y = 2.71;
    printf("Max(%.2f, %.2f) = %.2f\n", x, y, MAX(x, y));

    AFFICHER_SEPARATEUR();
    LOG("Fin du programme");

    return 0;
}
```

## Résumé

Les macros paramétrées sont un outil puissant du préprocesseur C qui permet :

✅ **Avantages** :
- Code générique fonctionnant avec plusieurs types
- Aucun overhead d'appel de fonction
- Utiles pour les constantes et le code conditionnel

⚠️ **Inconvénients** :
- Pas de vérification de type
- Risque d'évaluation multiple des arguments
- Messages d'erreur difficiles à interpréter
- Expansion du code (code bloat)

**Recommandation générale** : Utilisez les macros avec parcimonie. Préférez les fonctions (surtout `inline` depuis C99) pour la plupart des cas. Réservez les macros pour :
- Les constantes manifestes
- Le code de débogage conditionnel
- Les cas où la généricité est vraiment nécessaire
- L'abstraction de code répétitif

**Règle d'or** : Toujours entourer les paramètres et le corps de la macro de parenthèses, et utiliser `do { ... } while(0)` pour les macros multi-instructions.

---

**Prochaine section** : 23.2 Stringification et concaténation (manipulations avancées du préprocesseur)

⏭️ [Stringification et concaténation](/23-macros-preprocesseur/02-stringification-concatenation.md)
