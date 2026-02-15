🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 22.5 Fonctions variadiques

## Introduction

Les **fonctions variadiques** (ou "variadic functions" en anglais) sont des fonctions qui peuvent accepter un **nombre variable d'arguments**. Vous utilisez déjà ces fonctions tous les jours sans le savoir : `printf()`, `scanf()`, `fprintf()` sont toutes des fonctions variadiques !

### Qu'est-ce qu'une fonction variadique ?

Une fonction variadique est une fonction dont le nombre de paramètres n'est pas fixé à l'avance. Elle peut recevoir 1, 2, 5, 10 arguments ou plus, selon les besoins.

**Analogie** : Imaginez une machine à café qui peut préparer votre boisson avec un nombre variable d'ingrédients :
- Café simple : `preparer("café")`
- Café au lait : `preparer("café", "lait")`
- Cappuccino : `preparer("café", "lait", "mousse", "cacao")`

La machine s'adapte au nombre d'ingrédients que vous lui donnez. C'est exactement ce que font les fonctions variadiques.

### Exemples que vous connaissez déjà

```c
// printf peut prendre un nombre variable d'arguments
printf("Bonjour");                           // 1 argument  
printf("x = %d", 10);                        // 2 arguments  
printf("x = %d, y = %d", 10, 20);           // 3 arguments  
printf("%s a %d ans et vit à %s", "Paul", 25, "Paris");  // 4 arguments  
```

**Pourquoi sont-elles utiles ?**
- Flexibilité : une seule fonction peut gérer différents cas
- Simplicité : pas besoin de créer plusieurs versions de la même fonction
- Pratique : comme `printf()` qui évite d'avoir `printf1()`, `printf2()`, `printf3()`, etc.

---

## Le problème qu'elles résolvent

### Sans fonctions variadiques

Imaginons que nous voulons créer une fonction pour calculer la somme de nombres. Sans fonctions variadiques :

```c
int somme2(int a, int b) {
    return a + b;
}

int somme3(int a, int b, int c) {
    return a + b + c;
}

int somme4(int a, int b, int c, int d) {
    return a + b + c + d;
}

// ... et ainsi de suite pour 5, 6, 7 nombres...
```

**Problèmes** :
- ⚠️ Code répétitif
- ⚠️ Impossible de prévoir tous les cas
- ⚠️ Maintenance difficile
- ⚠️ API peu pratique pour l'utilisateur

### Avec fonctions variadiques

```c
#include <stdarg.h>

int somme(int count, ...) {
    // Une seule fonction pour tous les cas !
    // Détails plus loin...
}

// Utilisation
int s1 = somme(2, 10, 20);              // Somme de 2 nombres  
int s2 = somme(3, 10, 20, 30);          // Somme de 3 nombres  
int s3 = somme(5, 1, 2, 3, 4, 5);       // Somme de 5 nombres  
```

**Avantages** :
- ✅ Une seule fonction
- ✅ Flexible et extensible
- ✅ Code maintenable
- ✅ API élégante

---

## Syntaxe de base

### Les composants essentiels

Pour créer une fonction variadique, vous avez besoin de :

1. **Le header** : `#include <stdarg.h>`
2. **La notation** : `...` (trois points) dans les paramètres
3. **Le type** : `va_list` pour stocker les arguments
4. **Les macros** : `va_start()`, `va_arg()`, `va_end()`

### Anatomie d'une fonction variadique

```c
#include <stdarg.h>

type_retour nom_fonction(type param_fixe, ...) {
    va_list args;                    // 1. Déclaration de la liste d'arguments
    va_start(args, param_fixe);      // 2. Initialisation

    // 3. Récupération des arguments avec va_arg()
    type arg1 = va_arg(args, type);
    type arg2 = va_arg(args, type);

    va_end(args);                    // 4. Nettoyage

    return resultat;
}
```

**Points clés** :
- Il faut **au moins un paramètre fixe** avant les `...`
- Les trois points `...` doivent être **en dernier**
- `va_start()` prend comme second paramètre le **dernier paramètre fixe**

---

## Premier exemple simple

Créons une fonction qui calcule la somme d'un nombre variable d'entiers :

```c
#include <stdio.h>
#include <stdarg.h>

/**
 * Calcule la somme d'un nombre variable d'entiers
 * @param count Nombre d'arguments à additionner
 * @param ... Les entiers à additionner
 * @return La somme
 */
int somme(int count, ...) {
    va_list args;           // Liste pour parcourir les arguments
    va_start(args, count);  // Initialisation (count est le dernier param fixe)

    int total = 0;

    // Parcours de tous les arguments
    for (int i = 0; i < count; i++) {
        int valeur = va_arg(args, int);  // Récupère le prochain int
        total += valeur;
    }

    va_end(args);  // Nettoyage obligatoire

    return total;
}

int main(void) {
    printf("Somme de 2 nombres : %d\n", somme(2, 10, 20));
    printf("Somme de 4 nombres : %d\n", somme(4, 1, 2, 3, 4));
    printf("Somme de 6 nombres : %d\n", somme(6, 5, 10, 15, 20, 25, 30));

    return 0;
}
```

**Sortie** :
```
Somme de 2 nombres : 30  
Somme de 4 nombres : 10  
Somme de 6 nombres : 105  
```

**Explication ligne par ligne** :
1. `va_list args;` : Crée une variable qui va parcourir les arguments
2. `va_start(args, count);` : Initialise `args` pour commencer après `count`
3. `va_arg(args, int);` : Récupère le prochain argument comme un `int`
4. `va_end(args);` : Libère les ressources (obligatoire)

---

## Les macros en détail

### va_list

```c
va_list args;
```

**Rôle** : Type opaque qui va contenir l'état du parcours des arguments.

**Analogie** : C'est comme un curseur dans un livre qui se souvient de où vous en êtes dans votre lecture.

### va_start()

```c
va_start(args, dernier_param_fixe);
```

**Rôle** : Initialise la liste d'arguments. Positionne le "curseur" juste après le dernier paramètre fixe.

**Paramètres** :
- `args` : La variable `va_list` à initialiser
- `dernier_param_fixe` : Le nom du dernier paramètre fixe (avant les `...`)

**Important** : Doit être appelé avant toute utilisation de `va_arg()`.

### va_arg()

```c
type valeur = va_arg(args, type);
```

**Rôle** : Récupère le prochain argument et avance le curseur.

**Paramètres** :
- `args` : La liste d'arguments
- `type` : Le type de l'argument à récupérer (pas une variable, mais le nom du type lui-même)

**Attention** :
- Vous devez connaître le type exact de l'argument !
- Le type est passé SANS parenthèses : `int`, pas `(int)`
- Appeler `va_arg()` avance automatiquement au prochain argument

### va_end()

```c
va_end(args);
```

**Rôle** : Nettoie et libère les ressources associées à `va_list`.

**Important** :
- Toujours appeler `va_end()` avant de quitter la fonction
- Obligatoire même si rien ne semble se passer (comportement défini par l'implémentation)
- Après `va_end()`, `args` n'est plus utilisable (sauf si vous refaites `va_start()`)

---

## Exemples progressifs

### Exemple 1 : Fonction avec types différents

```c
#include <stdio.h>
#include <stdarg.h>

void afficher_info(const char *nom, int nb_infos, ...) {
    printf("Informations sur %s :\n", nom);

    va_list args;
    va_start(args, nb_infos);

    for (int i = 0; i < nb_infos; i++) {
        char *cle = va_arg(args, char*);    // Récupère une chaîne
        int valeur = va_arg(args, int);     // Récupère un entier

        printf("  %s : %d\n", cle, valeur);
    }

    va_end(args);
}

int main(void) {
    afficher_info("Jean", 3,
                  "age", 30,
                  "taille", 175,
                  "poids", 70);

    return 0;
}
```

**Sortie** :
```
Informations sur Jean :
  age : 30
  taille : 175
  poids : 70
```

### Exemple 2 : Fonction de formatage simple

```c
#include <stdio.h>
#include <stdarg.h>

void log_message(const char *niveau, const char *format, ...) {
    printf("[%s] ", niveau);

    va_list args;
    va_start(args, format);

    vprintf(format, args);  // vprintf accepte une va_list !

    va_end(args);
    printf("\n");
}

int main(void) {
    log_message("INFO", "Application démarrée");
    log_message("WARNING", "Mémoire faible : %d%%", 85);
    log_message("ERROR", "Fichier %s introuvable à la ligne %d", "config.txt", 42);

    return 0;
}
```

**Sortie** :
```
[INFO] Application démarrée
[WARNING] Mémoire faible : 85%
[ERROR] Fichier config.txt introuvable à la ligne 42
```

**Note** : `vprintf()` est une version de `printf()` qui accepte une `va_list` au lieu d'arguments variables.

### Exemple 3 : Trouver le maximum

```c
#include <stdio.h>
#include <stdarg.h>
#include <limits.h>

int max(int count, ...) {
    if (count <= 0) {
        return INT_MIN;  // Valeur par défaut pour liste vide
    }

    va_list args;
    va_start(args, count);

    int maximum = va_arg(args, int);  // Premier élément

    for (int i = 1; i < count; i++) {
        int valeur = va_arg(args, int);
        if (valeur > maximum) {
            maximum = valeur;
        }
    }

    va_end(args);
    return maximum;
}

int main(void) {
    printf("Max(3, 7, 2) = %d\n", max(3, 3, 7, 2));
    printf("Max(5, 15, 8, 23, 12, 19) = %d\n", max(5, 15, 8, 23, 12, 19));

    return 0;
}
```

**Sortie** :
```
Max(3, 7, 2) = 7  
Max(5, 15, 8, 23, 12, 19) = 23  
```

---

## Fonctions de la famille printf/scanf

Ces fonctions variadiques ont des variantes qui utilisent `va_list` :

| Fonction normale | Version avec va_list | Usage |
|-----------------|---------------------|--------|
| `printf()` | `vprintf()` | Sortie standard |
| `fprintf()` | `vfprintf()` | Sortie vers fichier |
| `sprintf()` | `vsprintf()` | Sortie vers chaîne |
| `snprintf()` | `vsnprintf()` | Sortie vers chaîne (sûre) |
| `scanf()` | `vscanf()` | Lecture depuis stdin |
| `fscanf()` | `vfscanf()` | Lecture depuis fichier |
| `sscanf()` | `vsscanf()` | Lecture depuis chaîne |

**Exemple : Wrapper de printf**

```c
#include <stdio.h>
#include <stdarg.h>

void mon_printf(const char *format, ...) {
    printf("[CUSTOM] ");

    va_list args;
    va_start(args, format);
    vprintf(format, args);  // Délègue à vprintf
    va_end(args);
}

int main(void) {
    mon_printf("x = %d, y = %d\n", 10, 20);
    return 0;
}
```

**Sortie** :
```
[CUSTOM] x = 10, y = 20
```

---

## Exemple avancé : Fonction générique de construction de chaîne

```c
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/**
 * Concatène plusieurs chaînes en une seule
 * @param dest Buffer de destination
 * @param size Taille du buffer
 * @param count Nombre de chaînes à concaténer
 * @param ... Les chaînes à concaténer
 * @return Longueur totale de la chaîne résultante
 */
size_t concatener(char *dest, size_t size, int count, ...) {
    if (dest == NULL || size == 0) {
        return 0;
    }

    dest[0] = '\0';  // Initialiser à chaîne vide
    size_t total = 0;

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        const char *str = va_arg(args, const char*);

        if (str != NULL) {
            size_t len = strlen(str);
            if (total + len < size) {
                strcat(dest, str);
                total += len;
            } else {
                // Plus de place
                break;
            }
        }
    }

    va_end(args);
    return total;
}

int main(void) {
    char buffer[100];

    concatener(buffer, sizeof(buffer), 4,
               "Bonjour ", "le ", "monde ", "!");
    printf("%s\n", buffer);

    concatener(buffer, sizeof(buffer), 3,
               "C", " est ", "génial");
    printf("%s\n", buffer);

    return 0;
}
```

**Sortie** :
```
Bonjour le monde !  
C est génial  
```

---

## Déterminer le nombre d'arguments

Il y a plusieurs approches pour savoir combien d'arguments ont été passés :

### Approche 1 : Paramètre de comptage explicite

```c
int somme(int count, ...) {
    // count indique le nombre d'arguments
}
```

**Avantages** : Simple, clair, sûr  
**Inconvénient** : L'utilisateur doit compter manuellement  

### Approche 2 : Valeur sentinelle

```c
#include <stdio.h>
#include <stdarg.h>

int somme_sentinel(int premier, ...) {
    int total = premier;

    va_list args;
    va_start(args, premier);

    int valeur;
    while ((valeur = va_arg(args, int)) != -1) {  // -1 = fin
        total += valeur;
    }

    va_end(args);
    return total;
}

int main(void) {
    // Doit se terminer par -1
    printf("Somme : %d\n", somme_sentinel(10, 20, 30, -1));
    return 0;
}
```

**Avantages** : Pas besoin de compter  
**Inconvénients** : La valeur sentinelle ne peut pas être une valeur valide, risque d'oubli  

### Approche 3 : Format string (comme printf)

```c
void print_all(const char *format, ...) {
    va_list args;
    va_start(args, format);

    for (const char *p = format; *p != '\0'; p++) {
        if (*p == 'i') {
            int val = va_arg(args, int);
            printf("%d ", val);
        } else if (*p == 's') {
            char *str = va_arg(args, char*);
            printf("%s ", str);
        }
    }

    va_end(args);
}

// Utilisation
print_all("isi", 42, "hello", 100);  // Format indique les types
```

**Avantages** : Flexible, permet différents types  
**Inconvénient** : Plus complexe  

---

## Pièges et limitations

### 1. Pas de vérification de type

```c
int somme(int count, ...) {
    va_list args;
    va_start(args, count);

    int total = 0;
    for (int i = 0; i < count; i++) {
        total += va_arg(args, int);  // Suppose que c'est un int !
    }

    va_end(args);
    return total;
}

// ❌ DANGEREUX : passer un double au lieu d'un int
int s = somme(2, 10, 3.14);  // Comportement indéfini !
```

**Le problème** : Le compilateur ne peut **pas** vérifier que vous passez le bon type. C'est à vous de faire attention !

### 2. Promotions de types

Certains types sont automatiquement promus :

```c
// ❌ ERREUR : va_arg(args, char) ne marche pas
char c = va_arg(args, char);  // char est promu en int

// ✅ CORRECT
char c = (char)va_arg(args, int);

// Même chose pour short, float
short s = (short)va_arg(args, int);  
float f = (float)va_arg(args, double);  
```

**Règle** :
- `char` et `short` → promus en `int`
- `float` → promu en `double`

### 3. Nombre d'arguments incorrect

```c
void fonction(int count, ...) {
    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        int val = va_arg(args, int);
    }

    va_end(args);
}

// ❌ DANGEREUX : on dit 3 mais on en passe seulement 2
fonction(3, 10, 20);  // Comportement indéfini !
```

**Le problème** : Lire au-delà des arguments passés donne un comportement indéfini.

### 4. Oublier va_end()

```c
void mauvaise_fonction(int count, ...) {
    va_list args;
    va_start(args, count);

    // ... traitement ...

    // ❌ OUBLI : pas de va_end() !
}
```

**Conséquence** : Fuite de ressources potentielle, comportement imprévisible.

### 5. Réutiliser va_list sans réinitialisation

```c
void fonction(int count, ...) {
    va_list args;
    va_start(args, count);

    int premier = va_arg(args, int);

    // ❌ ERREUR : essayer de "rembobiner"
    premier = va_arg(args, int);  // Ne retourne pas au début !

    va_end(args);
}
```

**Solution** : Utiliser `va_copy()` si vous devez parcourir plusieurs fois.

---

## va_copy() : Copier une va_list

Si vous devez parcourir les arguments plusieurs fois :

```c
#include <stdio.h>
#include <stdarg.h>

void afficher_deux_fois(int count, ...) {
    va_list args1, args2;
    va_start(args1, count);
    va_copy(args2, args1);  // Copie de args1 vers args2

    printf("Première passe : ");
    for (int i = 0; i < count; i++) {
        printf("%d ", va_arg(args1, int));
    }
    printf("\n");

    printf("Deuxième passe : ");
    for (int i = 0; i < count; i++) {
        printf("%d ", va_arg(args2, int));
    }
    printf("\n");

    va_end(args1);
    va_end(args2);  // Il faut aussi nettoyer la copie !
}

int main(void) {
    afficher_deux_fois(3, 10, 20, 30);
    return 0;
}
```

**Sortie** :
```
Première passe : 10 20 30  
Deuxième passe : 10 20 30  
```

**Important** : Appeler `va_end()` sur chaque `va_list`, y compris les copies.

---

## Bonnes pratiques

### ✅ À FAIRE

1. **Toujours avoir au moins un paramètre fixe**
```c
// ✅ CORRECT : premier paramètre fixe
void fonction(int count, ...);

// ❌ IMPOSSIBLE en C (possible en C++ avec templates)
void fonction(...);  // Erreur de compilation
```

2. **Documenter le format attendu**
```c
/**
 * Calcule la somme de nombres entiers
 * @param count Nombre d'entiers à additionner
 * @param ... Suite d'entiers (count fois)
 * @return La somme totale
 */
int somme(int count, ...);
```

3. **Vérifier les paramètres**
```c
int somme(int count, ...) {
    if (count <= 0) {
        return 0;  // Cas limite
    }
    // ...
}
```

4. **Utiliser des fonctions v* quand possible**
```c
void mon_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);  // Délègue à vprintf
    va_end(args);
}
```

5. **Toujours appeler va_end()**
```c
void fonction(int count, ...) {
    va_list args;
    va_start(args, count);

    // ... même si erreur ...

    va_end(args);  // Obligatoire
}
```

### ❌ À ÉVITER

1. **Oublier le paramètre de comptage**
```c
// ⚠️ Comment savoir quand s'arrêter ?
int somme(...);  // Impossible en C de toute façon
```

2. **Passer des types non promus**
```c
// ❌ ERREUR
char c = va_arg(args, char);    // char → int  
float f = va_arg(args, float);  // float → double  

// ✅ CORRECT
char c = (char)va_arg(args, int);  
float f = (float)va_arg(args, double);  
```

3. **Supposer un ordre d'évaluation**
```c
// ⚠️ Comportement non défini dans certains cas
fonction(va_arg(args, int), va_arg(args, int));
```

4. **Utiliser après va_end()**
```c
va_list args;  
va_start(args, count);  
va_end(args);  
int x = va_arg(args, int);  // ❌ Comportement indéfini  
```

---

## Alternatives aux fonctions variadiques

### 1. Tableaux

Au lieu de :
```c
int somme(int count, ...);
```

Utiliser :
```c
int somme(int tableau[], int taille);

// Utilisation
int nombres[] = {1, 2, 3, 4, 5};  
int s = somme(nombres, 5);  
```

**Avantages** : Plus sûr, vérification de types, peut passer le tableau  
**Inconvénient** : Moins flexible pour l'utilisateur  

### 2. Structures

```c
typedef struct {
    int count;
    int *values;
} IntList;

int somme_liste(IntList *liste);
```

### 3. Surcharge en C++ (si disponible)

```cpp
// C++ permet la surcharge
int somme(int a, int b);  
int somme(int a, int b, int c);  
int somme(int a, int b, int c, int d);  
```

---

## Cas d'usage recommandés

### ✅ Utiliser les fonctions variadiques pour :

1. **Fonctions de logging/debugging**
```c
void log_debug(const char *format, ...);  
void log_error(const char *format, ...);  
```

2. **Fonctions de formatage**
```c
char* format_string(const char *format, ...);
```

3. **Fonctions mathématiques simples**
```c
int min(int count, ...);  
int max(int count, ...);  
double moyenne(int count, ...);  
```

4. **Construction de structures**
```c
Widget* create_widget(const char *type, ...);
```

### ❌ Éviter pour :

1. **Code critique en performance**
   - Overhead d'appel et de parcours
   - Préférer des fonctions spécialisées

2. **APIs complexes avec beaucoup de paramètres**
   - Préférer des structures de configuration
   - Plus lisible et maintenable

3. **Quand un tableau suffit**
   - Surtout si les arguments sont du même type

---

## Implémentation de printf simplifié

Pour mieux comprendre, voici une implémentation très simplifiée de `printf` :

```c
#include <stdio.h>
#include <stdarg.h>

void mini_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);

    for (const char *p = format; *p != '\0'; p++) {
        if (*p == '%') {
            p++;  // Passer le '%'
            switch (*p) {
                case 'd': {
                    int i = va_arg(args, int);
                    printf("%d", i);
                    break;
                }
                case 's': {
                    char *s = va_arg(args, char*);
                    printf("%s", s);
                    break;
                }
                case 'c': {
                    int c = va_arg(args, int);  // char promu en int
                    putchar(c);
                    break;
                }
                case '%': {
                    putchar('%');
                    break;
                }
                default:
                    putchar('%');
                    putchar(*p);
            }
        } else {
            putchar(*p);
        }
    }

    va_end(args);
}

int main(void) {
    mini_printf("Nombre : %d, Texte : %s, Char : %c\n", 42, "Hello", 'A');
    return 0;
}
```

**Sortie** :
```
Nombre : 42, Texte : Hello, Char : A
```

---

## Sécurité et validation

Les fonctions variadiques peuvent être dangereuses si mal utilisées :

```c
#include <stdio.h>
#include <stdarg.h>

void fonction_securisee(int count, ...) {
    if (count < 0 || count > 100) {
        fprintf(stderr, "Erreur : count invalide\n");
        return;
    }

    va_list args;
    va_start(args, count);

    for (int i = 0; i < count; i++) {
        int val = va_arg(args, int);

        // Validation de chaque valeur
        if (val < 0) {
            fprintf(stderr, "Avertissement : valeur négative ignorée\n");
            continue;
        }

        // Traitement...
    }

    va_end(args);
}
```

---

## Comparaison avec d'autres langages

| Langage | Syntaxe | Fonctionnement |
|---------|---------|----------------|
| **C** | `void func(int n, ...)` | Macros `va_list`, pas de type safety |
| **C++** | Templates variadiques | Type-safe à la compilation |
| **Python** | `def func(*args)` | Tuple d'arguments, dynamique |
| **JavaScript** | `function func(...args)` | Array d'arguments, dynamique |
| **Go** | `func foo(args ...int)` | Slice typé, type-safe |
| **Java** | `void func(int... args)` | Array variadique, type-safe |

**C est le moins safe** car :
- Pas de vérification de types
- Nombre d'arguments non vérifié
- Responsabilité totale du programmeur

---

## Debugging de fonctions variadiques

### Affichage de diagnostic

```c
#include <stdio.h>
#include <stdarg.h>

void debug_args(const char *types, ...) {
    va_list args;
    va_start(args, types);

    printf("Arguments debug :\n");
    for (const char *p = types; *p != '\0'; p++) {
        printf("  Type '%c' : ", *p);

        switch (*p) {
            case 'i':
                printf("%d\n", va_arg(args, int));
                break;
            case 'd':
                printf("%f\n", va_arg(args, double));
                break;
            case 's':
                printf("%s\n", va_arg(args, char*));
                break;
        }
    }

    va_end(args);
}

int main(void) {
    debug_args("ids", 42, 3.14, "Hello");
    return 0;
}
```

### Avec GDB

```bash
(gdb) break fonction
(gdb) run
(gdb) print args
# Difficile de débugger va_list directement
# Mieux vaut ajouter des printf de debug
```

---

## Résumé

Les **fonctions variadiques** permettent de créer des fonctions acceptant un nombre variable d'arguments.

**Composants essentiels** :
```c
#include <stdarg.h>

void fonction(int fixe, ...) {
    va_list args;
    va_start(args, fixe);
    type val = va_arg(args, type);
    va_end(args);
}
```

**Avantages** :
- ✅ Flexibilité : une fonction pour plusieurs cas
- ✅ API élégante : comme `printf()`
- ✅ Moins de code répétitif

**Inconvénients** :
- ⚠️ Pas de vérification de type
- ⚠️ Risque d'erreurs à l'exécution
- ⚠️ Promotions de types à gérer
- ⚠️ Debugging difficile

**Quand utiliser** :
- Logging et formatage
- Fonctions mathématiques simples
- Wrappers de bibliothèques
- APIs de construction

**Quand éviter** :
- Code critique (sécurité/performance)
- Alternatives plus simples disponibles
- Trop de paramètres complexes

**Règles d'or** :
1. Toujours au moins un paramètre fixe
2. Toujours appeler `va_end()`
3. Documenter le format attendu
4. Valider les entrées
5. Utiliser `va_copy()` pour plusieurs parcours

Les fonctions variadiques sont puissantes mais demandent de la rigueur. Utilisez-les avec précaution et préférez des alternatives plus sûres quand c'est possible !

---


⏭️ [va_list et macros variadiques](/22-pointeurs-avances/06-va-list.md)
