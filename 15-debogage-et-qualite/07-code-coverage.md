🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 15.7 Code Coverage (Couverture de Code)

## Introduction

La **couverture de code** (code coverage) est une métrique qui mesure **quelle proportion de votre code source a été exécutée** lors des tests. C'est un indicateur essentiel de la qualité et de la complétude de vos tests, particulièrement important en langage C où les bugs peuvent avoir des conséquences graves.

### Qu'est-ce que la couverture de code ?

La couverture de code répond à une question simple mais fondamentale :

> **"Quelles parties de mon code ont été réellement testées ?"**

Imaginez que votre code soit une maison et vos tests des inspecteurs. La couverture de code vous indique quelles pièces les inspecteurs ont visitées et lesquelles n'ont jamais été ouvertes.

**Exemple simple :**

```c
int calculate(int x) {
    if (x > 0) {
        return x * 2;    // Ligne A
    } else {
        return x / 2;    // Ligne B
    }
}

// Test 1
int result1 = calculate(10);  // Exécute la ligne A

// Test 2
int result2 = calculate(-5);  // Exécute la ligne B
```

Dans cet exemple :
- **Avec Test 1 seul** : Couverture = 50% (ligne A exécutée, ligne B jamais testée)
- **Avec Test 1 + Test 2** : Couverture = 100% (toutes les lignes exécutées)

### Pourquoi la couverture de code est-elle cruciale en C ?

Le langage C est particulièrement sensible aux erreurs qui peuvent ne se manifester que dans certaines conditions :

**Problèmes typiques détectables par la couverture :**

1. **Chemins d'erreur non testés**
```c
FILE *file = fopen("data.txt", "r");  
if (file == NULL) {  
    // ❌ Ce code de gestion d'erreur est-il testé ?
    fprintf(stderr, "Cannot open file\n");
    return -1;
}
// Suite du code...
```

2. **Branches conditionnelles manquantes**
```c
if (x > 0 && y > 0) {
    // ✅ Testé avec x=5, y=10
    process_positive();
} else {
    // ❌ Jamais testé ! Que se passe-t-il si x=0 ou y=-5 ?
    process_negative();
}
```

3. **Code mort (dead code)**
```c
void cleanup() {
    // ❌ Cette fonction n'est jamais appelée
    free_resources();
}
```

4. **Cas limites (edge cases)**
```c
int divide(int a, int b) {
    // ❌ Le cas b=0 est-il testé ?
    if (b == 0) {
        return 0;
    }
    return a / b;
}
```

---

## Types de couverture de code

Il existe plusieurs **niveaux** de couverture, du plus simple au plus exhaustif :

### 1. Couverture de lignes (Line Coverage)

**Définition :** Pourcentage de lignes de code exécutées au moins une fois.

**Exemple :**

```c
int factorial(int n) {
    int result = 1;           // Ligne 1 ✅
    for (int i = 1; i <= n; i++) {  // Ligne 2 ✅
        result *= i;          // Ligne 3 ✅
    }
    return result;            // Ligne 4 ✅
}

// Test
factorial(5);  // Toutes les lignes exécutées → 100%
```

**Métrique :**
```
Couverture = (Lignes exécutées / Lignes exécutables) × 100
           = (4 / 4) × 100 = 100%
```

### 2. Couverture de fonctions (Function Coverage)

**Définition :** Pourcentage de fonctions appelées au moins une fois.

**Exemple :**

```c
void function_a() { /* ... */ }  
void function_b() { /* ... */ }  
void function_c() { /* ... */ }  

// Tests
function_a();  
function_b();  
// function_c() jamais appelée

// Couverture de fonctions = 2/3 = 66.7%
```

### 3. Couverture de branches (Branch Coverage)

**Définition :** Pourcentage de branches conditionnelles testées (tous les chemins if/else).

**Exemple :**

```c
int check_value(int x) {
    if (x > 0) {
        return 1;  // Branche TRUE
    } else {
        return 0;  // Branche FALSE
    }
}

// Test 1
check_value(5);   // Teste seulement la branche TRUE
// Couverture de branches = 50%

// Test 2
check_value(-3);  // Teste la branche FALSE
// Couverture de branches = 100%
```

**Important :** Une couverture de lignes à 100% ne garantit pas une couverture de branches à 100% !

**Exemple trompeur :**

```c
int process(int x, int y) {
    if (x > 0 && y > 0) {  // 4 branches possibles !
        return 1;
    }
    return 0;
}

// Test
process(5, 10);  // Couverture de lignes = 100%
                 // Mais couverture de branches = 25% seulement !
```

**Les 4 branches :**
1. `x > 0` TRUE et `y > 0` TRUE → ✅ Testée
2. `x > 0` TRUE et `y > 0` FALSE → ❌ Non testée
3. `x > 0` FALSE et `y > 0` TRUE → ❌ Non testée
4. `x > 0` FALSE et `y > 0` FALSE → ❌ Non testée

### 4. Couverture de conditions (Condition Coverage)

**Définition :** Chaque condition booléenne évaluée à TRUE et FALSE.

**Exemple :**

```c
if (a > 0 && b > 0) {
    // ...
}
```

Pour 100% de couverture de conditions :
- Tester avec `a > 0` TRUE et FALSE
- Tester avec `b > 0` TRUE et FALSE

### 5. Couverture de chemins (Path Coverage)

**Définition :** Tous les chemins d'exécution possibles testés.

**Exemple :**

```c
void process(int x) {
    if (x > 0) {        // Point de décision 1
        printf("A");
    }
    if (x < 10) {       // Point de décision 2
        printf("B");
    }
}
```

**Chemins possibles :**
1. x > 0 TRUE, x < 10 TRUE → Affiche "AB"
2. x > 0 TRUE, x < 10 FALSE → Affiche "A"
3. x > 0 FALSE, x < 10 TRUE → Affiche "B"
4. x > 0 FALSE, x < 10 FALSE → Affiche rien

**Note :** La couverture de chemins devient rapidement **explosive** (nombreux chemins possibles), donc rarement utilisée à 100%.

---

## Visualisation des différents types de couverture

### Tableau comparatif

| Type de couverture | Difficulté | Utilité | Faux sentiment de sécurité |
|-------------------|------------|---------|---------------------------|
| **Lignes** | ⭐ Facile | ⭐⭐⭐ Moyenne | ⚠️ Élevé |
| **Fonctions** | ⭐ Facile | ⭐⭐ Faible | ⚠️⚠️ Très élevé |
| **Branches** | ⭐⭐ Moyen | ⭐⭐⭐⭐ Élevée | ⚠️ Faible |
| **Conditions** | ⭐⭐⭐ Difficile | ⭐⭐⭐⭐ Élevée | ⚠️ Faible |
| **Chemins** | ⭐⭐⭐⭐⭐ Très difficile | ⭐⭐⭐⭐⭐ Maximale | ❌ Aucun |

### Exemple d'évolution

```c
int evaluate(int a, int b) {
    if (a > 0) {
        if (b > 0) {
            return a + b;
        }
        return a;
    }
    return 0;
}
```

**Avec test : `evaluate(5, 10)`**

| Métrique | Résultat | Explication |
|----------|----------|-------------|
| Couverture lignes | 75% | 3 lignes sur 4 exécutées |
| Couverture fonctions | 100% | La fonction est appelée |
| Couverture branches | 50% | 2 branches sur 4 testées |

---

## Interpréter les métriques de couverture

### Que signifie X% de couverture ?

**Seuils généralement acceptés :**

| Pourcentage | Évaluation | Recommandation |
|-------------|------------|----------------|
| **< 50%** | 🔴 Insuffisant | Tests gravement incomplets |
| **50-70%** | 🟠 Faible | Ajouter des tests significatifs |
| **70-80%** | 🟡 Acceptable | Bon pour du code non critique |
| **80-90%** | 🟢 Bon | Standard professionnel |
| **90-100%** | 🟢 Excellent | Critique pour code mission-critical |

**Attention :** Ces seuils dépendent du contexte !

### Exemple d'interprétation

**Projet A : Bibliothèque de cryptographie**
- Couverture : 95%
- ✅ **Excellent** : Code critique nécessite tests exhaustifs

**Projet B : Prototype/POC**
- Couverture : 60%
- ✅ **Acceptable** : Priorité à la rapidité de développement

**Projet C : Application de production**
- Couverture : 50%
- ❌ **Insuffisant** : Risques de bugs en production

---

## La couverture n'est pas une fin en soi

### Le piège du 100%

**Mythe :** "100% de couverture = code sans bugs"

**Réalité :** La couverture mesure uniquement **ce qui est exécuté**, pas **ce qui est correctement testé**.

### Exemple de mauvais test avec 100% de couverture

```c
int divide(int a, int b) {
    if (b == 0) {
        return 0;  // Gestion d'erreur
    }
    return a / b;
}

// ❌ Mauvais test qui donne 100% de couverture
void bad_test() {
    divide(10, 2);   // Teste ligne 5
    divide(10, 0);   // Teste lignes 2-3
    // Pas d'assertions ! On n'a pas vérifié les résultats
}

// ✅ Bon test avec assertions
void good_test() {
    assert(divide(10, 2) == 5);      // Vérifie le résultat
    assert(divide(10, 0) == 0);      // Vérifie gestion erreur
    assert(divide(-10, 2) == -5);    // Teste nombres négatifs
}
```

Le mauvais test atteint 100% de couverture mais ne **vérifie rien** !

### Qualité vs Quantité

**Couverture élevée + Tests de mauvaise qualité = Fausse sécurité**

**Exemple :**

```c
int max(int a, int b) {
    return (a > b) ? a : b;
}

// ❌ Test qui couvre 100% mais teste mal
void weak_test() {
    max(5, 3);   // Pas d'assertion, juste exécution
    max(2, 8);   // Pas de vérification
}

// ✅ Test qui couvre 100% ET vérifie correctement
void strong_test() {
    assert(max(5, 3) == 5);
    assert(max(2, 8) == 8);
    assert(max(7, 7) == 7);  // Cas égalité
}
```

---

## Avantages de la mesure de couverture

### 1. Identifier le code non testé

La couverture révèle **immédiatement** les zones dangereuses :

```
Fichier: auth.c  
Couverture: 45%  

Lignes non couvertes:
- 23-28: Gestion de l'authentification échouée
- 45-52: Code de timeout
- 78-85: Gestion des permissions
```

→ Ces zones critiques doivent être testées !

### 2. Guider l'écriture de nouveaux tests

La couverture indique **où** ajouter des tests :

```c
int process_user(User *user) {
    if (user == NULL) {
        return -1;  // ❌ Jamais testé (couverture 0%)
    }

    if (!user->is_active) {
        return 0;   // ❌ Jamais testé (couverture 0%)
    }

    // Suite du code...  ✅ Testé (couverture 100%)
}
```

Action : Ajouter des tests pour les cas NULL et utilisateur inactif.

### 3. Détecter le code mort

Code jamais exécuté = potentiellement obsolète :

```c
void legacy_function() {
    // Couverture: 0%
    // Peut probablement être supprimé
}
```

### 4. Mesurer l'amélioration dans le temps

Suivi de l'évolution :

```
Janvier 2024:  55%  
Février 2024:  62%  
Mars 2024:     70%  
Avril 2024:    78%  ← Progrès constant  
```

### 5. Critère objectif de qualité

Standard d'équipe :

```
Politique: Toute PR doit maintenir la couverture ≥ 80%
```

---

## Limitations et pièges

### 1. Couverture élevée ≠ Tests de qualité

**Exemple :**

```c
int calculate(int x) {
    return x * 2 + 5;  // Bug : devrait être x * 2 - 5
}

// Test qui donne 100% de couverture mais ne détecte pas le bug
void test() {
    int result = calculate(10);
    // Pas d'assertion, juste exécution !
}
```

### 2. Ne mesure pas la qualité des assertions

```c
// ❌ Couverture 100%, test inutile
void bad_test() {
    int result = complex_calculation();
    assert(result != -999999);  // Assertion trop faible
}

// ✅ Couverture 100%, test utile
void good_test() {
    int result = complex_calculation();
    assert(result == 42);  // Assertion précise
}
```

### 3. Couverture coûteuse à atteindre

**Loi de Pareto :**
- 80% de couverture → 20% de l'effort
- 95% de couverture → 80% de l'effort

**Point d'équilibre :** 80-90% pour la plupart des projets

### 4. Code difficile à tester

Certains codes sont difficiles à couvrir :
- Gestion d'erreurs rares (out of memory)
- Code dépendant du matériel
- Interactions système complexes

**Solution :** Utiliser des mocks et stubs.

### 5. Focalisation excessive sur les métriques

**Danger :** Optimiser la couverture plutôt que la qualité réelle

```c
// ❌ Tests écrits juste pour la couverture
void useless_test() {
    function_a();  // Juste pour dire "exécuté"
    function_b();  // Sans vérifier quoi que ce soit
}
```

---

## Couverture et autres pratiques de qualité

### Couverture dans l'écosystème qualité

La couverture de code est **un élément** d'une stratégie globale :

```
┌──────────────────────────────────────────────┐
│         STRATÉGIE QUALITÉ COMPLÈTE           │
├──────────────────────────────────────────────┤
│                                              │
│  1. Analyse statique (cppcheck, clang-tidy)  │
│     ↓                                        │
│  2. Compilation stricte (-Wall -Werror)      │
│     ↓                                        │
│  3. COUVERTURE DE CODE ← Nous sommes ici     │
│     ↓                                        │
│  4. Tests unitaires (avec assertions)        │
│     ↓                                        │
│  5. Sanitizers (ASan, UBSan)                 │
│     ↓                                        │
│  6. Valgrind (fuites mémoire)                │
│     ↓                                        │
│  7. Tests d'intégration                      │
│     ↓                                        │
│  8. Code review                              │
│                                              │
└──────────────────────────────────────────────┘
```

### Complémentarité des approches

| Technique | Détecte | Ne détecte pas |
|-----------|---------|----------------|
| **Couverture** | Code non testé | Qualité des tests |
| **Tests unitaires** | Bugs fonctionnels | Code non exécuté |
| **Sanitizers** | Erreurs mémoire | Logique métier |
| **Analyse statique** | Patterns dangereux | Bugs runtime |

**Conclusion :** Utilisez toutes ces techniques ensemble !

---

## Quand mesurer la couverture ?

### 1. Pendant le développement

**TDD (Test-Driven Development) :**
```
1. Écrire le test (couverture = 0%)
2. Implémenter le code minimal (couverture augmente)
3. Vérifier la couverture (objectif : 100% de la nouvelle fonction)
4. Refactorer
```

### 2. Avant chaque commit

**Pre-commit hook :**
```bash
# Vérifier que la couverture ne régresse pas
if [ $COVERAGE -lt $THRESHOLD ]; then
    echo "Couverture trop faible, commit refusé"
    exit 1
fi
```

### 3. Dans le CI/CD

**Automatisation :**
- Chaque push → mesure de couverture
- Blocage si < seuil défini
- Rapport publié automatiquement

### 4. Lors des code reviews

**Critère de validation :**
```
PR acceptée SI:
- Fonctionnalité implémentée
- Tests ajoutés
- Couverture ≥ 80%
- Review approuvée
```

---

## Outils de couverture pour C

### Vue d'ensemble des outils

| Outil | Type | Complexité | Sortie | Recommandé pour |
|-------|------|------------|--------|-----------------|
| **gcov** | CLI | ⭐ Simple | Texte | Développement local |
| **lcov** | CLI + GUI | ⭐⭐ Moyen | HTML | Rapports visuels |
| **gcovr** | CLI | ⭐⭐ Moyen | HTML/XML | CI/CD |
| **Codecov** | Cloud | ⭐ Simple | Web | Open-source |
| **Coveralls** | Cloud | ⭐ Simple | Web | Open-source |

### Workflow typique

```
1. Instrumentation (gcc -fprofile-arcs -ftest-coverage)
   ↓
2. Exécution (./programme)
   ↓
3. Collecte (gcov fichier.c)
   ↓
4. Visualisation (lcov + genhtml)
   ↓
5. Publication (Codecov)
```

---

## Stratégies de couverture

### Approche pragmatique

**Ne visez pas systématiquement 100% !**

**Priorisation :**

1. **Critique (90-100%)** : Fonctions de sécurité, gestion mémoire, logique métier essentielle
2. **Important (80-90%)** : Fonctions principales, API publiques
3. **Standard (70-80%)** : Code utilitaire, helpers
4. **Bas (50-70%)** : Code de logging, debug

**Exemple :**

```c
// CRITIQUE - Viser 100%
int authenticate_user(const char *username, const char *password) {
    // Sécurité : tous les chemins doivent être testés
}

// IMPORTANT - Viser 85%
int calculate_balance(Account *account) {
    // Logique métier : bien tester
}

// STANDARD - Viser 75%
const char* get_user_name(User *user) {
    // Simple getter
}

// BAS - Viser 60%
void log_debug(const char *message) {
    // Logging, moins critique
}
```

### Stratégie d'amélioration progressive

Pour un projet existant avec faible couverture :

**Phase 1 (Mois 1-2) : Baseline et code critique**
- Mesurer la couverture actuelle
- Identifier le code critique
- Atteindre 80% sur le code critique

**Phase 2 (Mois 3-4) : Nouveaux développements**
- Tout nouveau code : 80% minimum
- Pas de régression sur l'existant

**Phase 3 (Mois 5-6) : Code existant**
- Améliorer progressivement l'ancien code
- Objectif : +5% de couverture globale par mois

**Phase 4 (Mois 7+) : Maintien**
- Maintenir la couverture ≥ 80%
- Améliorer les zones sensibles

---

## Bonnes pratiques

### 1. Définir des seuils réalistes

```yaml
# Exemple de politique
coverage_policy:
  minimum_global: 80%
  minimum_new_code: 85%
  critical_modules: 95%
```

### 2. Exclure le code non pertinent

```c
// LCOV_EXCL_START
void debug_dump() {
    // Code de debug, pas besoin de tester
}
// LCOV_EXCL_STOP
```

### 3. Coupler couverture et qualité

```c
// ✅ Bon test
void test_divide() {
    assert(divide(10, 2) == 5);        // Cas normal
    assert(divide(10, 0) == 0);        // Cas erreur
    assert(divide(-10, 2) == -5);      // Cas négatif
    assert(divide(1, 3) == 0);         // Cas troncature
}
```

### 4. Automatiser la mesure

```makefile
# Makefile
coverage:
    gcc -fprofile-arcs -ftest-coverage ...
    ./run_tests
    lcov --capture ...
    genhtml ...
```

### 5. Communiquer les résultats

**Badge dans le README :**

```markdown
![Coverage](https://img.shields.io/badge/coverage-85%25-brightgreen)
```

### 6. Ne pas tricher

```c
// ❌ Mauvais : test juste pour la couverture
void fake_test() {
    unused_function();  // Pas d'assertion
}

// ✅ Bon : vraie vérification
void real_test() {
    int result = unused_function();
    assert(result == expected);
}
```

---

## Métriques complémentaires

La couverture seule ne suffit pas. Combinez avec :

### 1. Taux de détection de défauts

Nombre de bugs trouvés par les tests / Total de bugs.

### 2. Qualité des assertions

```c
// Faible qualité
assert(result != NULL);

// Haute qualité
assert(result->value == 42);  
assert(strcmp(result->name, "test") == 0);  
```

### 3. Mutation testing

Introduire des bugs volontaires et vérifier que les tests les détectent.

### 4. Complexité du code

Combiner couverture + complexité cyclomatique :
- Haute couverture + Faible complexité = ✅ Excellent
- Haute couverture + Haute complexité = ⚠️ Tester plus
- Faible couverture + Haute complexité = 🔴 Urgent !

---

## Résumé

La couverture de code est un outil **puissant mais non suffisant** pour garantir la qualité :

### Ce que la couverture fait

- ✅ **Identifie le code non testé**
- ✅ **Guide l'écriture de nouveaux tests**
- ✅ **Détecte le code mort**
- ✅ **Fournit une métrique objective**
- ✅ **Suit l'évolution de la qualité**

### Ce que la couverture ne fait pas

- ❌ **Ne garantit pas l'absence de bugs**
- ❌ **Ne mesure pas la qualité des tests**
- ❌ **Ne remplace pas les autres vérifications**
- ❌ **Ne teste pas la logique métier**
- ❌ **N'assure pas la robustesse**

### Les principes essentiels

1. **Viser 80-90%** pour la plupart des projets
2. **Combiner avec tests de qualité** (assertions fortes)
3. **Prioriser le code critique** (sécurité, mémoire)
4. **Automatiser la mesure** (CI/CD)
5. **Ne pas tricher** (tests réels, pas juste exécution)
6. **Utiliser avec d'autres outils** (sanitizers, Valgrind, analyse statique)

### Citation à retenir

> **"La couverture de code vous dit ce qui n'est pas testé, pas ce qui est bien testé."**

---

## Prochaines étapes

Dans les sections suivantes, nous allons découvrir les outils pratiques pour mesurer et visualiser la couverture en C :

- **15.7.1 gcov** : L'outil de base fourni avec GCC pour mesurer la couverture
- **15.7.2 lcov** : Interface graphique pour transformer les données gcov en rapports HTML
- **15.7.3 Intégration dans le CI** : Automatiser la mesure dans votre pipeline de développement

**💡 Conseil de démarrage :** Commencez par mesurer la couverture actuelle de votre projet avec gcov, sans chercher à l'améliorer immédiatement. Comprendre où vous en êtes est la première étape vers l'amélioration !

---

**🎯 Objectif :** À la fin de ce chapitre, vous saurez :
1. Mesurer précisément la couverture de votre code C
2. Générer des rapports visuels professionnels
3. Intégrer la couverture dans votre workflow quotidien
4. Fixer des objectifs réalistes et les atteindre
5. Utiliser la couverture comme outil d'amélioration continue

La couverture de code n'est pas une destination, c'est un **voyage continu** vers un code plus robuste et mieux testé ! 🚀

⏭️ [gcov](/15-debogage-et-qualite/07.1-gcov.md)
