🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 5.2 Opérateurs logiques

## Introduction

Les opérateurs logiques sont des outils essentiels en programmation C qui permettent de combiner plusieurs conditions et de prendre des décisions complexes dans votre code. Ils sont particulièrement utiles dans les structures conditionnelles (`if`, `while`, etc.) pour tester plusieurs critères simultanément.

En C, il existe trois opérateurs logiques principaux que nous allons explorer en détail.

---

## Les trois opérateurs logiques

### 1. L'opérateur ET logique : `&&`

L'opérateur `&&` (ET logique) renvoie **vrai** (1) uniquement si **les deux** conditions sont vraies. Si l'une des deux est fausse, le résultat est faux (0).

**Syntaxe :**
```c
condition1 && condition2
```

**Exemple simple :**
```c
#include <stdio.h>

int main() {
    int age = 25;
    int permis = 1;  // 1 signifie "a le permis", 0 signifie "n'a pas le permis"

    // Pour conduire, il faut avoir 18 ans ET avoir le permis
    if (age >= 18 && permis == 1) {
        printf("Vous pouvez conduire.\n");
    } else {
        printf("Vous ne pouvez pas conduire.\n");
    }

    return 0;
}
```

**Table de vérité pour `&&` :**

| Condition 1 | Condition 2 | Résultat |
|------------|------------|----------|
| Vrai       | Vrai       | **Vrai** |
| Vrai       | Faux       | Faux     |
| Faux       | Vrai       | Faux     |
| Faux       | Faux       | Faux     |

**Cas d'usage typiques :**
- Vérifier qu'un nombre est dans un intervalle : `(x >= 10 && x <= 20)`
- Valider plusieurs critères : `(utilisateur_connecte && acces_autorise)`
- Conditions de sécurité : `(vitesse_ok && frein_ok && batterie_ok)`

---

### 2. L'opérateur OU logique : `||`

L'opérateur `||` (OU logique) renvoie **vrai** (1) si **au moins une** des conditions est vraie. Il ne renvoie faux (0) que si **toutes** les conditions sont fausses.

**Syntaxe :**
```c
condition1 || condition2
```

**Exemple simple :**
```c
#include <stdio.h>

int main() {
    char jour = 'S';  // S pour Samedi

    // Le magasin est fermé le dimanche OU les jours fériés
    if (jour == 'D' || jour == 'F') {
        printf("Le magasin est fermé.\n");
    } else {
        printf("Le magasin est ouvert.\n");
    }

    return 0;
}
```

**Table de vérité pour `||` :**

| Condition 1 | Condition 2 | Résultat |
|------------|------------|----------|
| Vrai       | Vrai       | **Vrai** |
| Vrai       | Faux       | **Vrai** |
| Faux       | Vrai       | **Vrai** |
| Faux       | Faux       | Faux     |

**Cas d'usage typiques :**
- Vérifier plusieurs options : `(paiement_carte || paiement_especes)`
- Valider des entrées alternatives : `(touche == 'o' || touche == 'O')`
- Gestion d'erreurs : `(erreur_reseau || erreur_disque)`

---

### 3. L'opérateur NON logique : `!`

L'opérateur `!` (NON logique ou négation) inverse la valeur d'une condition. Si la condition est vraie, `!` la rend fausse, et vice versa.

**Syntaxe :**
```c
!condition
```

**Exemple simple :**
```c
#include <stdio.h>

int main() {
    int pluie = 0;  // 0 signifie "il ne pleut pas"

    // Si il ne pleut PAS
    if (!pluie) {
        printf("Vous pouvez sortir sans parapluie.\n");
    } else {
        printf("N'oubliez pas votre parapluie.\n");
    }

    return 0;
}
```

**Table de vérité pour `!` :**

| Condition | Résultat |
|-----------|----------|
| Vrai      | **Faux** |
| Faux      | **Vrai** |

**Cas d'usage typiques :**
- Inverser une condition : `if (!fichier_ouvert)`
- Vérifier l'absence : `if (!erreur)`
- Simplifier la logique : `if (!termine)` au lieu de `if (termine == 0)`

---

## Combinaison d'opérateurs logiques

Vous pouvez combiner plusieurs opérateurs logiques pour créer des conditions complexes. Il est recommandé d'utiliser des parenthèses pour clarifier la priorité des opérations.

**Exemple complet :**
```c
#include <stdio.h>

int main() {
    int temperature = 22;
    int humidite = 65;
    int pluie = 0;

    // Conditions pour une bonne journée de randonnée
    if ((temperature >= 15 && temperature <= 25) &&
        (humidite < 80) &&
        !pluie) {
        printf("Excellentes conditions pour une randonnée !\n");
    } else {
        printf("Les conditions ne sont pas idéales.\n");
    }

    return 0;
}
```

---

## Priorité des opérateurs

Lorsque vous combinez plusieurs opérateurs logiques, il est important de connaître leur ordre de priorité :

1. **`!`** (NON) - Priorité la plus élevée
2. **`&&`** (ET)
3. **`||`** (OU) - Priorité la plus basse

**Exemple illustrant la priorité :**
```c
int a = 1, b = 0, c = 1;

// Sans parenthèses : ! est évalué en premier
if (!a || b && c) {
    // Équivalent à : (!a) || (b && c)
    // Résultat : 0 || 0 = 0 (faux)
}

// Avec parenthèses pour plus de clarté
if (!(a || b) && c) {
    // Résultat : !(1) && 1 = 0 && 1 = 0 (faux)
}
```

**💡 Bonne pratique :** Utilisez toujours des parenthèses pour rendre votre code plus lisible, même si elles ne sont pas strictement nécessaires.

---

## Évaluation en court-circuit (Short-circuit evaluation)

C'est un concept important à comprendre : le C optimise l'évaluation des expressions logiques.

### Pour l'opérateur `&&`

Si la première condition est **fausse**, la seconde n'est **jamais évaluée** car le résultat sera forcément faux.

```c
int x = 5;
int y = 10;

// Si x > 10 est faux, y > 5 n'est jamais testé
if (x > 10 && y > 5) {
    printf("Les deux conditions sont vraies.\n");
}
```

### Pour l'opérateur `||`

Si la première condition est **vraie**, la seconde n'est **jamais évaluée** car le résultat sera forcément vrai.

```c
int connecte = 1;
int admin = 0;

// Si connecte == 1 est vrai, admin == 1 n'est jamais testé
if (connecte == 1 || admin == 1) {
    printf("Accès autorisé.\n");
}
```

**Pourquoi c'est important ?**

Cela peut avoir des conséquences si vous avez des effets de bord dans vos conditions :

```c
int compteur = 0;

// Attention : compteur++ pourrait ne jamais s'exécuter
if (x > 10 || compteur++ > 5) {
    // Si x > 10 est vrai, compteur++ n'est jamais exécuté
}
```

**💡 Conseil :** Évitez de placer des incréments ou des appels de fonctions ayant des effets de bord directement dans les conditions logiques.

---

## Valeurs vraies et fausses en C

En C, il n'existe pas de type booléen natif avant C99 (où `<stdbool.h>` a été introduit). Voici comment C interprète les valeurs :

- **Faux** : La valeur `0` (zéro)
- **Vrai** : Toute valeur **différente de zéro** (1, -1, 42, 100, etc.)

**Exemple :**
```c
#include <stdio.h>

int main() {
    int valeur1 = 5;
    int valeur2 = 0;
    int valeur3 = -3;

    if (valeur1) {
        printf("valeur1 est considérée comme vraie\n");  // S'affiche
    }

    if (valeur2) {
        printf("valeur2 est considérée comme vraie\n");  // Ne s'affiche pas
    }

    if (valeur3) {
        printf("valeur3 est considérée comme vraie\n");  // S'affiche (-3 != 0)
    }

    return 0;
}
```

### Utilisation de `<stdbool.h>` (C99 et supérieur)

À partir de C99, vous pouvez utiliser le type `bool` pour plus de clarté :

```c
#include <stdio.h>
#include <stdbool.h>

int main() {
    bool est_majeur = true;
    bool a_permis = false;

    if (est_majeur && !a_permis) {
        printf("Vous êtes majeur mais sans permis.\n");
    }

    return 0;
}
```

---

## Erreurs courantes à éviter

### 1. Confusion entre `=` et `==`

```c
int x = 10;

// ❌ ERREUR : Affectation au lieu de comparaison
if (x = 5) {
    printf("Cette condition est toujours vraie !\n");
}

// ✅ CORRECT : Comparaison
if (x == 5) {
    printf("x est égal à 5\n");
}
```

### 2. Oublier les parenthèses dans les conditions complexes

```c
int age = 25;
int salaire = 30000;

// ❌ Difficile à lire et source d'erreurs
if (age > 18 && age < 65 || salaire > 50000 && salaire < 100000)

// ✅ Beaucoup plus clair avec des parenthèses
if ((age > 18 && age < 65) || (salaire > 50000 && salaire < 100000))
```

### 3. Utiliser `&&` au lieu de `||` (et vice versa)

```c
int jour = 7;

// ❌ ERREUR LOGIQUE : Un jour ne peut pas être 6 ET 7 en même temps
if (jour == 6 && jour == 7) {
    printf("C'est le week-end\n");
}

// ✅ CORRECT : Un jour peut être 6 OU 7
if (jour == 6 || jour == 7) {
    printf("C'est le week-end\n");
}
```

### 4. Double négation inutile

```c
int actif = 1;

// ❌ Compliqué et peu lisible
if (!!actif) {
    printf("Actif\n");
}

// ✅ Simple et clair
if (actif) {
    printf("Actif\n");
}
```

---

## Exemples pratiques récapitulatifs

### Validation d'un âge pour différentes catégories

```c
#include <stdio.h>

int main() {
    int age = 16;

    // Enfant : 0 à 12 ans
    if (age >= 0 && age <= 12) {
        printf("Catégorie : Enfant\n");
    }
    // Adolescent : 13 à 17 ans
    else if (age >= 13 && age <= 17) {
        printf("Catégorie : Adolescent\n");
    }
    // Adulte : 18 ans et plus
    else if (age >= 18) {
        printf("Catégorie : Adulte\n");
    }
    else {
        printf("Âge invalide\n");
    }

    return 0;
}
```

### Validation d'un formulaire

```c
#include <stdio.h>
#include <string.h>

int main() {
    char nom[50] = "Dupont";
    char email[100] = "dupont@example.com";
    int age = 25;
    int accepte_conditions = 1;

    // Le formulaire est valide si TOUS les critères sont respectés
    if (strlen(nom) > 0 &&              // Le nom n'est pas vide
        strlen(email) > 5 &&            // L'email a au moins 6 caractères
        age >= 18 &&                    // L'utilisateur est majeur
        accepte_conditions) {           // Les conditions sont acceptées

        printf("✅ Formulaire valide !\n");
    } else {
        printf("❌ Formulaire invalide.\n");

        // Affichage des erreurs spécifiques
        if (strlen(nom) == 0) {
            printf("   - Le nom est requis\n");
        }
        if (strlen(email) <= 5) {
            printf("   - L'email semble invalide\n");
        }
        if (age < 18) {
            printf("   - Vous devez être majeur\n");
        }
        if (!accepte_conditions) {
            printf("   - Vous devez accepter les conditions\n");
        }
    }

    return 0;
}
```

### Système de connexion simple

```c
#include <stdio.h>
#include <string.h>

int main() {
    char utilisateur[50] = "admin";
    char mot_de_passe[50] = "secret123";
    int compte_actif = 1;
    int tentatives = 2;

    char saisie_user[50] = "admin";
    char saisie_mdp[50] = "secret123";

    // Connexion réussie si :
    // - Les identifiants sont corrects ET
    // - Le compte est actif ET
    // - Il reste des tentatives
    if (strcmp(utilisateur, saisie_user) == 0 &&
        strcmp(mot_de_passe, saisie_mdp) == 0 &&
        compte_actif &&
        tentatives > 0) {

        printf("✅ Connexion réussie !\n");
    } else {
        printf("❌ Connexion échouée.\n");

        if (!compte_actif) {
            printf("   Raison : Compte désactivé\n");
        } else if (tentatives <= 0) {
            printf("   Raison : Trop de tentatives\n");
        } else {
            printf("   Raison : Identifiants incorrects\n");
        }
    }

    return 0;
}
```

---

## Bonnes pratiques

1. **Clarté avant tout** : Utilisez des parenthèses pour rendre vos conditions explicites, même si elles ne sont pas obligatoires.

2. **Nommage explicite** : Préférez des variables booléennes avec des noms clairs :
   ```c
   int est_connecte = 1;
   int a_les_droits = 0;
   ```

3. **Évitez les conditions trop complexes** : Si une condition devient trop longue, divisez-la :
   ```c
   // ❌ Difficile à lire
   if (a && b && c && d || e && f && !g) { ... }

   // ✅ Plus lisible
   int condition1 = a && b && c && d;
   int condition2 = e && f && !g;
   if (condition1 || condition2) { ... }
   ```

4. **Utilisez `stdbool.h`** (si C99+) : Cela rend le code plus lisible et moderne.

5. **Commentez les conditions complexes** : Expliquez la logique métier derrière vos conditions.

---

## Résumé

| Opérateur | Symbole | Description | Résultat vrai si... |
|-----------|---------|-------------|---------------------|
| ET logique | `&&` | Les deux conditions doivent être vraies | Condition1 **ET** Condition2 sont vraies |
| OU logique | `||` | Au moins une condition doit être vraie | Condition1 **OU** Condition2 est vraie |
| NON logique | `!` | Inverse la valeur de la condition | Condition est fausse |

**Points clés à retenir :**

- Les opérateurs logiques permettent de combiner plusieurs conditions
- Le C utilise l'évaluation en court-circuit pour optimiser les performances
- En C, `0` est faux et toute autre valeur est vraie
- Utilisez des parenthèses pour améliorer la lisibilité
- Attention à ne pas confondre `=` (affectation) et `==` (comparaison)

---

## Pour aller plus loin

Dans les prochaines sections, vous découvrirez :
- **5.3 Opérateurs bit-à-bit** : Manipulation des bits individuels
- **5.4 Conditions et optimisation** : Techniques pour des conditions plus efficaces
- **5.5 Boucles et contrôle de flux** : Utilisation des opérateurs logiques dans les boucles

Les opérateurs logiques sont fondamentaux et seront utilisés dans presque tous vos programmes C. Prenez le temps de bien les maîtriser !

⏭️ [Opérateurs bit-à-bit](/05-operateurs-et-controle/03-operateurs-bitwise.md)
