🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 29.1 Philosophie du testing en C

## Introduction

Le testing en C présente des défis uniques comparé aux langages modernes. L'absence de système de tests intégré, la gestion manuelle de la mémoire, et la nature bas niveau du langage rendent les tests à la fois plus difficiles et plus essentiels. Cette section explore pourquoi et comment adopter une approche de testing efficace pour vos projets C.

---

## Pourquoi tester en C ?

### Les spécificités du C qui rendent les tests cruciaux

Le C est un langage puissant mais sans filet de sécurité. Contrairement aux langages modernes avec garbage collection, vérification des limites de tableaux, ou système de types strict, le C vous donne un contrôle total... et donc une responsabilité totale.

**Problèmes typiques du C détectables par les tests :**

- **Fuites mémoire** : Oublier un `free()` après un `malloc()`
- **Buffer overflows** : Écrire au-delà des limites d'un tableau
- **Use-after-free** : Utiliser un pointeur après avoir libéré sa mémoire
- **Pointeurs invalides** : Déréférencer `NULL` ou un pointeur non initialisé
- **Race conditions** : Problèmes de concurrence dans du code multi-threaded
- **Débordements d'entiers** : Dépasser la capacité d'un type numérique

Sans tests, ces bugs peuvent rester silencieux pendant des mois, voire des années, jusqu'à causer un crash en production.

### Le coût de l'absence de tests

Un bug en production dans du code C peut avoir des conséquences graves :

- **Systèmes embarqués** : Redémarrage d'un dispositif médical ou automobile
- **Serveurs** : Corruption de données ou crash d'un service critique
- **Outils système** : Perte de données utilisateur ou comportement imprévisible

Le coût de correction d'un bug augmente exponentiellement avec le temps :

```
Bug trouvé pendant le codage       →  5 minutes de fix  
Bug trouvé en phase de test        →  30 minutes à 1 heure  
Bug trouvé en production           →  Plusieurs heures + impact client  
Bug causant une faille de sécurité →  Jours de travail + dégâts réputationnels  
```

---

## Les principes fondamentaux du testing en C

### 1. Tester tôt, tester souvent

En C, attendre la fin du développement pour tester est une recette pour le désastre. Chaque fonction, chaque module devrait être testé au fur et à mesure de son écriture.

**Approche recommandée :**

- Écrivez une fonction
- Écrivez son test immédiatement
- Vérifiez qu'elle passe le test
- Passez à la fonction suivante

Cette approche itérative permet de détecter les problèmes quand le contexte est encore frais dans votre esprit.

### 2. Les tests comme documentation vivante

Un bon test en C ne se contente pas de vérifier le comportement, il **documente** également comment utiliser le code.

```c
// Ce test documente l'utilisation de la fonction
void test_string_copy_normal_case(void) {
    char dest[20];
    const char *src = "Hello";

    // Cas d'usage standard
    string_copy(dest, src, sizeof(dest));

    // Comportement attendu
    assert(strcmp(dest, "Hello") == 0);
}
```

Un développeur qui lit ce test comprend immédiatement :
- Les paramètres attendus
- L'ordre des arguments
- Le comportement normal de la fonction

### 3. Tests isolés et reproductibles

Chaque test doit être **indépendant** des autres. Un test ne doit pas dépendre de l'état laissé par un test précédent.

**Mauvaise pratique :**

```c
static int global_counter = 0;

void test_increment(void) {
    increment(&global_counter);
    assert(global_counter == 1); // Échoue si le test est exécuté deux fois
}
```

**Bonne pratique :**

```c
void test_increment(void) {
    int counter = 0; // État local, isolé
    increment(&counter);
    assert(counter == 1); // Toujours vrai, peu importe l'ordre d'exécution
}
```

### 4. Tester les cas limites (edge cases)

En C, les cas limites sont souvent la source principale de bugs. Ne testez pas seulement le "happy path" (le cas normal), testez aussi :

- **Valeurs nulles** : Que se passe-t-il si on passe `NULL` ?
- **Tableaux vides** : Comportement avec une longueur de 0
- **Débordements** : Que se passe-t-il avec `INT_MAX + 1` ?
- **Ressources épuisées** : Que faire si `malloc()` échoue ?

**Exemple :**

```c
void test_array_sum_edge_cases(void) {
    // Cas limite 1 : tableau NULL
    assert(array_sum(NULL, 5) == 0);

    // Cas limite 2 : longueur 0
    int arr[] = {1, 2, 3};
    assert(array_sum(arr, 0) == 0);

    // Cas limite 3 : valeurs négatives
    int neg[] = {-5, -10};
    assert(array_sum(neg, 2) == -15);
}
```

---

## Les niveaux de testing en C

### Tests unitaires

Les **tests unitaires** vérifient le comportement d'une fonction ou d'un module isolé. C'est le niveau le plus bas et le plus important.

**Caractéristiques :**
- Rapides à exécuter (< 1 ms par test)
- Testent une seule fonction ou concept
- N'utilisent pas d'I/O (pas de fichiers, réseau, base de données)
- Faciles à déboguer en cas d'échec

**Exemple de cas d'usage :**
- Tester une fonction de tri
- Vérifier une fonction de calcul mathématique
- Valider un parser de ligne de commande

### Tests d'intégration

Les **tests d'intégration** vérifient que plusieurs modules fonctionnent correctement ensemble.

**Caractéristiques :**
- Plus lents que les tests unitaires
- Testent l'interaction entre composants
- Peuvent utiliser des ressources externes (fichiers temporaires)
- Plus complexes à mettre en place

**Exemple de cas d'usage :**
- Tester qu'un module de logging écrit correctement dans un fichier
- Vérifier qu'un client TCP communique avec un serveur
- Valider qu'un parser lit correctement un fichier de configuration

### Tests système (End-to-End)

Les **tests système** vérifient le comportement complet d'une application.

**Caractéristiques :**
- Très lents (plusieurs secondes)
- Testent l'application comme le ferait un utilisateur
- Nécessitent un environnement complet
- Difficiles à automatiser

**Exemple de cas d'usage :**
- Lancer un programme CLI et vérifier sa sortie
- Tester un daemon du démarrage à l'arrêt
- Vérifier qu'un serveur répond correctement aux requêtes HTTP

---

## Stratégies de testing pour du code C

### La pyramide des tests

Une bonne suite de tests suit généralement la "pyramide des tests" :

```
          /\
         /  \        Tests E2E (peu nombreux, lents)
        /────\
       /      \      Tests d'intégration (modérés)
      /────────\
     /          \    Tests unitaires (nombreux, rapides)
    /────────────\
```

**Règle générale :**
- 70% de tests unitaires
- 20% de tests d'intégration
- 10% de tests système

Cette répartition offre le meilleur rapport **confiance / temps d'exécution**.

### Test-Driven Development (TDD) en C

Le TDD est une approche où l'on écrit les tests **avant** le code de production.

**Cycle TDD :**

1. **Red** : Écrire un test qui échoue
2. **Green** : Écrire le minimum de code pour faire passer le test
3. **Refactor** : Améliorer le code sans casser les tests

**Avantages en C :**
- Force à penser à l'API avant l'implémentation
- Garantit que chaque ligne de code est testée
- Réduit les bugs de conception (fonctions trop complexes, couplage fort)

**Inconvénients :**
- Nécessite de la discipline
- Peut sembler lent au début
- Difficile pour du code bas niveau (drivers, inline assembly)

### Tester du code qui utilise malloc/free

Tester du code avec allocation dynamique nécessite de vérifier que :
1. La mémoire est correctement allouée
2. La mémoire est correctement libérée
3. Le code gère les échecs d'allocation

**Approche recommandée :**

```c
void test_create_list(void) {
    // Test normal
    List *list = list_create();
    assert(list != NULL);
    assert(list->size == 0);

    // Nettoyage obligatoire
    list_destroy(list);

    // Note : Valgrind vérifiera automatiquement les fuites
}
```

**Utilisation de Valgrind :**

Valgrind est votre meilleur ami pour détecter les fuites mémoire dans les tests :

```bash
valgrind --leak-check=full ./test_suite
```

Un bon test en C doit passer **sans aucune fuite** détectée par Valgrind.

### Tester les fonctions avec effets de bord

Certaines fonctions modifient l'état global ou ont des effets de bord (I/O, variables globales). Elles sont plus difficiles à tester.

**Techniques pour tester du code avec effets de bord :**

1. **Injection de dépendances** : Passer les dépendances en paramètre

```c
// Au lieu de :
void log_message(const char *msg) {
    FILE *f = fopen("/var/log/app.log", "a");
    fprintf(f, "%s\n", msg);
    fclose(f);
}

// Préférer :
void log_message(FILE *file, const char *msg) {
    fprintf(file, "%s\n", msg);
}

// Test facile :
void test_log_message(void) {
    FILE *tmp = tmpfile(); // Fichier temporaire
    log_message(tmp, "Test");

    rewind(tmp);
    char buffer[100];
    fgets(buffer, sizeof(buffer), tmp);
    assert(strcmp(buffer, "Test\n") == 0);

    fclose(tmp);
}
```

2. **Abstraction des ressources** : Créer une interface pour les ressources externes

```c
typedef struct {
    void (*write)(const char *msg);
    void (*close)(void);
} Logger;

// Production : écrit dans un fichier
// Tests : écrit dans un buffer en mémoire
```

---

## Les défis spécifiques du testing en C

### 1. Absence de frameworks natifs

Contrairement à Python (pytest), JavaScript (Jest), ou Java (JUnit), le C n'a pas de framework de test standardisé.

**Conséquence :** Vous devez choisir et configurer un framework vous-même (Unity, Check, CUnit, cmocka, etc.), ou écrire vos propres macros d'assertion.

### 2. Gestion manuelle de la mémoire

Chaque test qui alloue de la mémoire doit la libérer. Un oubli crée une fuite que Valgrind détectera.

**Bonne pratique :** Utilisez un pattern "setup/teardown" pour initialiser et nettoyer l'état :

```c
void setup(void) {
    // Initialiser les ressources
}

void teardown(void) {
    // Libérer les ressources
}

void test_something(void) {
    setup();
    // ... test ...
    teardown();
}
```

### 3. Pas de réflexion ni d'introspection

Vous ne pouvez pas inspecter automatiquement les types ou les structures comme en Python. Cela rend certains types de tests (comme la sérialisation automatique) plus complexes.

### 4. Pointeurs et adresses mémoire

Comparer des pointeurs dans les tests peut être trompeur :

```c
void test_string_literal(void) {
    const char *s1 = "hello";
    const char *s2 = "hello";

    // ATTENTION : peut être vrai ou faux selon l'optimisation du compilateur
    assert(s1 == s2); // Comparaison d'adresses

    // Toujours vrai : comparaison de contenu
    assert(strcmp(s1, s2) == 0);
}
```

---

## Philosophie "Fail Fast, Fail Loud"

En C, il vaut mieux qu'un programme crashe immédiatement plutôt qu'il continue avec des données corrompues.

### Utilisation d'assertions en production

Contrairement aux langages où les assertions sont désactivées en production, en C système critique, elles sont souvent **activées**.

**Exemple :**

```c
void process_packet(const uint8_t *data, size_t len) {
    assert(data != NULL);
    assert(len > 0 && len <= MAX_PACKET_SIZE);

    // Traitement...
}
```

Si une assertion échoue, le programme crashe avec un core dump, ce qui permet de diagnostiquer le problème.

### Tests comme première ligne de défense

Les tests ne remplacent pas les assertions, ils les complètent :

- **Assertions** : Vérifient les invariants en production
- **Tests** : Vérifient le comportement attendu en développement

---

## Couverture de code (Code Coverage)

La **couverture de code** mesure le pourcentage de lignes de code exécutées par les tests.

### Interpréter la couverture

**80% de couverture ne signifie PAS que votre code est fiable à 80%.**

Exemple de code avec 100% de couverture mais non testé :

```c
int divide(int a, int b) {
    return a / b; // 100% couvert par un test
}

void test_divide(void) {
    assert(divide(10, 2) == 5); // Test qui passe
}
```

Problème : Le test ne vérifie pas le cas `b == 0`, qui causera un crash.

### Objectif raisonnable

- **Code critique** : Viser 90-100%
- **Code utilitaire** : 70-80% acceptable
- **Code expérimental** : > 50%

**Important :** Une couverture élevée est un indicateur, pas une garantie. La **qualité des tests** compte plus que leur nombre.

---

## Les anti-patterns à éviter

### 1. Tests qui ne testent rien

```c
void test_useless(void) {
    my_function();
    // Pas d'assertion → le test passe toujours
}
```

**Solution :** Toujours avoir au moins une assertion.

### 2. Tests trop larges

```c
void test_everything(void) {
    // Teste 10 fonctions différentes
    // Si ça échoue, impossible de savoir laquelle
}
```

**Solution :** Un test = un concept.

### 3. Tests dépendants de l'ordre d'exécution

```c
static int counter = 0;

void test_1(void) { counter++; }  
void test_2(void) { assert(counter == 1); } // Échoue si test_2 est lancé seul  
```

**Solution :** Chaque test doit être indépendant.

### 4. Ignorer les warnings du compilateur

```c
void test_something(void) {
    int *ptr;
    *ptr = 5; // Warning: uninitialized variable
    assert(*ptr == 5);
}
```

**Solution :** Compiler les tests avec `-Wall -Werror`.

---

## Conclusion : Une question de rigueur

Tester du code C demande plus de rigueur que dans d'autres langages, mais c'est précisément pour cela que c'est indispensable. Un bug en C peut avoir des conséquences catastrophiques, et les tests sont votre première ligne de défense.

### Récapitulatif des principes clés

1. **Testez tôt, testez souvent** : Ne remettez pas les tests à plus tard
2. **Isolez vos tests** : Chaque test doit être indépendant
3. **Testez les cas limites** : NULL, 0, débordements, erreurs d'allocation
4. **Utilisez Valgrind** : Zéro fuite doit être la norme
5. **Préférez la quantité ET la qualité** : Beaucoup de bons tests valent mieux que quelques tests parfaits

### Prochaines étapes

Dans les sections suivantes, nous verrons :
- **29.2** : Les frameworks de tests unitaires concrets (Unity, Check, cmocka)
- **29.3** : L'écriture de tests efficaces et les bonnes pratiques
- **29.4** : Le mocking et le stubbing pour isoler les dépendances
- **29.5** : Les tests d'intégration et leur automatisation
- **29.6** : La mesure de couverture de code
- **29.7** : Le Test-Driven Development (TDD) en pratique

---

> **💡 Conseil final :** Ne voyez pas les tests comme une perte de temps, mais comme un **investissement**. Le temps passé à écrire des tests est toujours inférieur au temps passé à déboguer en production.

⏭️ [Tests unitaires : frameworks](/29-tests-et-qualite/02-frameworks-tests-unitaires.md)
