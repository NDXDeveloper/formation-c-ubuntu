🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 5.1 Opérateurs arithmétiques

## Introduction

Les opérateurs arithmétiques en C permettent d'effectuer des calculs mathématiques de base. Ils sont similaires à ceux que vous connaissez en mathématiques, mais avec quelques particularités propres à la programmation.

## Les cinq opérateurs arithmétiques de base

### 1. Addition (`+`)

L'opérateur d'addition additionne deux valeurs.

```c
int a = 10;  
int b = 5;  
int somme = a + b;  // somme vaut 15  
```

### 2. Soustraction (`-`)

L'opérateur de soustraction soustrait la valeur de droite de celle de gauche.

```c
int a = 10;  
int b = 5;  
int difference = a - b;  // difference vaut 5  
```

### 3. Multiplication (`*`)

L'opérateur de multiplication multiplie deux valeurs.

```c
int a = 10;  
int b = 5;  
int produit = a * b;  // produit vaut 50  
```

### 4. Division (`/`)

L'opérateur de division divise la valeur de gauche par celle de droite.

```c
int a = 10;  
int b = 5;  
int quotient = a / b;  // quotient vaut 2  
```

**⚠️ Attention importante avec la division :**

Lorsque vous divisez deux entiers (`int`), le résultat est **toujours un entier**. La partie décimale est **tronquée** (supprimée), pas arrondie.

```c
int a = 7;  
int b = 2;  
int resultat = a / b;  // resultat vaut 3, PAS 3.5  
```

Pour obtenir un résultat décimal, au moins une des valeurs doit être un nombre à virgule flottante (`float` ou `double`) :

```c
float a = 7.0;  
int b = 2;  
float resultat = a / b;  // resultat vaut 3.5  

// Ou avec un cast :
int x = 7;  
int y = 2;  
float resultat2 = (float)x / y;  // resultat2 vaut 3.5  
```

### 5. Modulo (`%`)

L'opérateur modulo retourne le **reste** de la division entière.

```c
int a = 10;  
int b = 3;  
int reste = a % b;  // reste vaut 1 (car 10 = 3*3 + 1)  
```

**Utilisation courante du modulo :**
- Vérifier si un nombre est pair : `if (n % 2 == 0)`
- Vérifier si un nombre est divisible par un autre : `if (n % 5 == 0)`
- Faire boucler une valeur dans un intervalle

⚠️ **Important :** Le modulo ne fonctionne qu'avec des entiers. Vous ne pouvez pas l'utiliser avec `float` ou `double`.

## Priorité des opérateurs

Comme en mathématiques, les opérateurs ont une **priorité** (ordre d'exécution) :

1. **Multiplication, Division, Modulo** (`*`, `/`, `%`) - **priorité haute**
2. **Addition, Soustraction** (`+`, `-`) - **priorité basse**

```c
int resultat = 2 + 3 * 4;  // resultat vaut 14, pas 20
// Car 3 * 4 est calculé en premier (= 12), puis 2 + 12
```

Pour modifier l'ordre d'évaluation, utilisez des **parenthèses** :

```c
int resultat = (2 + 3) * 4;  // resultat vaut 20
// Car 2 + 3 est calculé en premier (= 5), puis 5 * 4
```

## Division par zéro

**Attention danger ⚠️** : La division par zéro est une **erreur fatale** en C.

```c
int a = 10;  
int b = 0;  
int resultat = a / b;  // ❌ Erreur d'exécution : programme crash !  
```

Votre programme plantera avec une erreur du type "Floating point exception" (même pour les entiers). **Vérifiez toujours** que le diviseur n'est pas zéro avant de diviser :

```c
int a = 10;  
int b = 0;  

if (b != 0) {
    int resultat = a / b;
    printf("Résultat : %d\n", resultat);
} else {
    printf("Erreur : division par zéro impossible\n");
}
```

## Mélange de types (types mixtes)

Lorsque vous mélangez différents types numériques dans une opération, C effectue une **conversion automatique** vers le type le plus "large" :

```c
int a = 5;  
float b = 2.0;  
float resultat = a + b;  // a est converti en float, resultat vaut 7.0  
```

**Hiérarchie de conversion (du plus petit au plus grand) :**
1. `int`
2. `long`
3. `float`
4. `double`

```c
int x = 5;  
double y = 2.5;  
double resultat = x * y;  // x est converti en double, resultat vaut 12.5  
```

## L'opérateur unaire moins (`-`)

Le signe moins peut aussi être utilisé pour inverser le signe d'une valeur :

```c
int a = 10;  
int b = -a;  // b vaut -10  

int c = -5;  
int d = -c;  // d vaut 5  
```

## Exemples pratiques complets

### Exemple 1 : Calcul de moyenne

```c
#include <stdio.h>

int main(void) {
    int note1 = 15;
    int note2 = 12;
    int note3 = 18;

    float moyenne = (note1 + note2 + note3) / 3.0;  // Division par 3.0 (float)

    printf("Moyenne : %.2f\n", moyenne);  // Affiche : Moyenne : 15.00

    return 0;
}
```

### Exemple 2 : Conversion de température (Celsius vers Fahrenheit)

```c
#include <stdio.h>

int main(void) {
    float celsius = 25.0;
    float fahrenheit = (celsius * 9.0 / 5.0) + 32.0;

    printf("%.1f°C = %.1f°F\n", celsius, fahrenheit);
    // Affiche : 25.0°C = 77.0°F

    return 0;
}
```

### Exemple 3 : Vérifier si un nombre est pair

```c
#include <stdio.h>

int main(void) {
    int nombre = 42;

    if (nombre % 2 == 0) {
        printf("%d est pair\n", nombre);
    } else {
        printf("%d est impair\n", nombre);
    }

    return 0;
}
```

### Exemple 4 : Calculer le reste d'une division

```c
#include <stdio.h>

int main(void) {
    int dividende = 17;
    int diviseur = 5;

    int quotient = dividende / diviseur;
    int reste = dividende % diviseur;

    printf("%d ÷ %d = %d reste %d\n", dividende, diviseur, quotient, reste);
    // Affiche : 17 ÷ 5 = 3 reste 2

    return 0;
}
```

## Pièges courants à éviter

### Piège 1 : Division entière non désirée

```c
int a = 5;  
int b = 2;  
float resultat = a / b;  // ❌ resultat vaut 2.0, pas 2.5 !  
// La division entière est faite AVANT l'affectation au float

// ✅ Solution :
float resultat = (float)a / b;  // Maintenant resultat vaut 2.5
```

### Piège 2 : Débordement (overflow)

```c
int a = 2000000000;  // Proche de la limite d'un int (environ 2.1 milliards)  
int b = 2;  
int resultat = a * b;  // ❌ Débordement ! Résultat imprévisible  

// ✅ Solution : utiliser un type plus grand
long long resultat = (long long)a * b;  // Pas de débordement
```

### Piège 3 : Précision des flottants

```c
float a = 0.1;  
float b = 0.2;  
float somme = a + b;  // somme peut ne pas être exactement 0.3  
// À cause de la représentation binaire des nombres flottants

// Pour comparer des flottants, utilisez une marge d'erreur :
if (somme > 0.29 && somme < 0.31) {
    printf("Approximativement égal à 0.3\n");
}
```

## Points clés à retenir

✅ **Les 5 opérateurs** : `+`, `-`, `*`, `/`, `%`

✅ **Division entière** : Quand on divise deux `int`, le résultat est un `int` (partie décimale supprimée)

✅ **Modulo** : Donne le reste de la division, ne fonctionne qu'avec les entiers

✅ **Division par zéro** : Provoque un crash, toujours vérifier le diviseur

✅ **Priorité** : `*`, `/`, `%` avant `+`, `-`. Utilisez des parenthèses pour clarifier

✅ **Conversion automatique** : Les types sont convertis vers le plus "large" dans les opérations mixtes

---

**Prochaine étape** : Découvrez les opérateurs logiques (5.2) pour combiner des conditions !

⏭️ [Opérateurs logiques](/05-operateurs-et-controle/02-operateurs-logiques.md)
