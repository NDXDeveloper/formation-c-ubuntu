🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Module 2 : Fondamentaux du Langage

> **Formation : Maîtriser C sur Ubuntu - De la Syntaxe au Système**
> Niveau : **Débutant** • Durée estimée : **20-25 heures de lecture et expérimentation**

---

## 🎯 Objectifs du module

Maintenant que vous comprenez l'écosystème C (Module 1), il est temps d'apprendre le **langage lui-même**. Ce module couvre la syntaxe fondamentale du C et les mécanismes de base qui font tourner chaque programme C, quelle que soit sa complexité.

À l'issue de ce module, vous maîtriserez :

1. ✅ **Les types de données** et leur représentation en mémoire
2. ✅ **Les opérateurs** (arithmétiques, logiques, bit-à-bit)
3. ✅ **Les structures de contrôle** (conditions, boucles)
4. ✅ **Les fonctions** et la pile d'appels (stack)
5. ✅ **Le formatage automatique** du code avec clang-format
6. ✅ **Les bonnes pratiques** de style et lisibilité

---

## 🧩 Place dans la formation globale

### D'où venez-vous ?
**Module 1** vous a appris à configurer votre environnement et à comprendre la chaîne de compilation. Vous savez maintenant *comment* transformer du code source en exécutable, mais vous n'avez pas encore appris *quoi* écrire.

### Où allez-vous ?
**Module 3** abordera la gestion de la mémoire (pointeurs, allocation dynamique), qui s'appuie sur les fondamentaux de ce module. Vous devez maîtriser les bases avant d'attaquer les concepts plus complexes.

### Pourquoi ce module est crucial
**Sans ces fondamentaux, vous ne pourrez pas :**
- Comprendre les messages d'erreur du compilateur
- Écrire du code qui fait ce que vous voulez
- Lire et maintenir du code C existant
- Progresser vers les modules avancés

---

## 📚 Structure du module

Ce module est organisé en **trois chapitres progressifs** qui construisent les uns sur les autres :

### 🔢 [Chapitre 4 : Types de données et Représentation mémoire](04-types-et-memoire/README.md)
**Contenu** : 7 sections • **Niveau** : Fondamental

**Vous découvrirez :**
- Les types primitifs (`int`, `char`, `float`, `double`) et leur taille exacte
- La différence entre `signed` et `unsigned`
- Les constantes et littéraux
- Les conversions de types (implicites et explicites)
- L'endianness et son impact sur la portabilité
- Les nouveautés de C23 concernant les types

**Concepts clés :**
```c
// Taille des types selon l'architecture
sizeof(int)      // 4 bytes sur la plupart des systèmes
sizeof(long)     // 4 ou 8 bytes selon 32-bit vs 64-bit
sizeof(pointer)  // Dépend de l'architecture

// Signed vs Unsigned
int a = -1;           // Peut être négatif
unsigned int b = -1;  // Devient un très grand nombre positif !

// Conversions implicites dangereuses
float x = 3.14f;
int y = x;  // Perte de la partie décimale
```

**Pourquoi c'est important :** La représentation mémoire des données est au cœur de nombreux bugs en C. Comprendre comment les types sont stockés vous évite les overflow, les pertes de précision, et les comportements indéfinis.

---

### ⚙️ [Chapitre 5 : Opérateurs et Structures de contrôle](05-operateurs-et-controle/README.md)
**Contenu** : 7 sections + sous-section clang-format • **Niveau** : Fondamental

**Vous apprendrez :**
- Les opérateurs arithmétiques et leurs priorités
- Les opérateurs logiques (`&&`, `||`, `!`)
- Les opérateurs bit-à-bit (`&`, `|`, `^`, `~`, `<<`, `>>`)
- Les conditions (`if`, `else`, `switch`)
- Les boucles (`for`, `while`, `do-while`)
- Les bonnes pratiques de formatage
- **L'automatisation du formatage avec clang-format** ⭐

**Concepts clés :**
```c
// Opérateurs bit-à-bit pour manipulation bas niveau
unsigned int flags = 0;
flags |= (1 << 3);   // Set bit 3
flags &= ~(1 << 3);  // Clear bit 3
if (flags & (1 << 3)) { /* Test bit 3 */ }

// Priorité des opérateurs
int x = 5 + 3 * 2;  // 11, pas 16 !

// Switch avec fall-through
switch (value) {
    case 1:
    case 2:  // Fall-through intentionnel
        do_something();
        break;
    default:
        do_default();
}
```

**Focus sur clang-format :**
Cette section couvre un outil moderne essentiel pour maintenir un code propre et cohérent. Vous apprendrez à :
- Installer et configurer clang-format
- Choisir un style de formatage (Google, LLVM, Mozilla, etc.)
- Intégrer clang-format dans VS Code, Vim, CLion
- Automatiser le formatage avec Git hooks
- Forcer le formatage dans le CI/CD

**Pourquoi c'est important :** Les opérateurs bit-à-bit sont omniprésents en programmation système (manipulation de registres, flags, protocoles réseau). Les structures de contrôle sont la base de toute logique. Et le formatage automatique élimine les débats stériles sur le style.

---

### 🔧 [Chapitre 6 : Les Fonctions](06-fonctions/README.md)
**Contenu** : 5 sections • **Niveau** : Fondamental

**Vous maîtriserez :**
- La déclaration vs la définition de fonctions
- Les prototypes et leur rôle dans la compilation
- Le mécanisme de la pile (stack frames)
- Le passage par valeur
- Les valeurs de retour et codes d'erreur
- La fonction `main` et ses paramètres (`argc`, `argv`)

**Concepts clés :**
```c
// Prototype (déclaration)
int calculate(int a, int b);

// Définition
int calculate(int a, int b) {
    return a * 2 + b;
}

// La pile d'appels
void func1() {
    int x = 10;  // Sur la pile
    func2(x);    // Nouveau frame sur la pile
}  // Frame de func1 détruit, x n'existe plus

// Fonction main
int main(int argc, char *argv[]) {
    // argc = nombre d'arguments
    // argv[0] = nom du programme
    // argv[1], argv[2]... = arguments
    return 0;  // Code de retour
}
```

**Pourquoi c'est important :** Les fonctions sont l'unité de base de la modularité en C. Comprendre la pile est essentiel pour déboguer les segfaults et comprendre les limitations du passage par valeur (ce qui motive l'introduction des pointeurs au Module 3).

---

## 🎓 Prérequis

### Connaissances requises
- ✅ **Module 1 complété** : Environnement configuré et chaîne de compilation comprise
- ✅ **Notions algorithmiques** : Concept de variable, boucle, condition (dans n'importe quel langage)
- ✅ **Système numérique** : Binaire, hexadécimal (niveau basique)

### Pas nécessaire à ce stade
- ❌ Pointeurs (Module 3)
- ❌ Allocation dynamique (Module 3)
- ❌ Structures de données (Module 4)
- ❌ Programmation système (Module 6)

---

## 🗺️ Parcours de lecture recommandé

Cette formation est un **guide de référence complet**. Voici comment l'utiliser efficacement :

### 📖 Lecture séquentielle (recommandée pour débutants)
1. **Lisez chaque section dans l'ordre**
2. **Compilez et testez** les exemples de code fournis
3. **Modifiez les exemples** pour comprendre ce qui se passe
4. **Consultez la documentation GCC** quand un comportement vous surprend

### 🔍 Consultation ciblée (pour développeurs expérimentés)
- Utilisez la table des matières pour aller directement aux concepts qui vous manquent
- Lisez les sections marquées ⭐ en priorité (concepts modernes)
- Consultez les sections sur les "pièges" pour éviter les erreurs classiques

### 🧪 Approche expérimentale
Pour chaque concept :
```bash
# 1. Créez un fichier de test
nano test_types.c

# 2. Écrivez un petit programme illustrant le concept
# 3. Compilez avec warnings
gcc -Wall -Wextra -Werror test_types.c -o test

# 4. Exécutez et observez
./test

# 5. Modifiez et recompilez pour explorer
```

---

## 🛠️ Outils et ressources nécessaires

### Environnement de développement
```bash
# Vérifiez que votre environnement est prêt
gcc --version      # GCC 9.0+
clang-format --version  # Pour le chapitre 5

# Créez votre répertoire de travail
mkdir -p ~/c-learning/module-02
cd ~/c-learning/module-02
```

### Documentation de référence
- **man pages** : `man 3 printf`, `man 3 strcmp`, etc.
- **GCC warnings** : `gcc -Q --help=warning`
- **C standard** : [open-std.org](https://www.open-std.org/jtc1/sc22/wg14/)

### Outils d'exploration
```bash
# Voir la représentation binaire
xxd -b fichier

# Inspecter les symboles
nm programme

# Voir le code assembleur généré
gcc -S programme.c
cat programme.s
```

---

## 🎯 Compétences développées

| Compétence | Description | Applications |
|------------|-------------|--------------|
| **Typage fort** | Comprendre les types et conversions | Éviter les bugs subtils, écrire du code portable |
| **Opérations bas niveau** | Manipulation bit-à-bit | Drivers, protocoles réseau, optimisation |
| **Logique de programme** | Structures de contrôle | Tout algorithme |
| **Modularité** | Fonctions et organisation | Code maintenable et testable |
| **Style de code** | Formatage automatisé | Collaboration en équipe, revues de code |
| **Debugging mental** | Comprendre la pile | Résoudre les segfaults et stack overflows |

---

## 📖 Concepts fondamentaux abordés

### Le modèle de mémoire C
```
┌─────────────────────────────┐
│    Code (text segment)      │  ← Instructions du programme
├─────────────────────────────┤
│    Data (données globales)  │  ← Variables globales initialisées
├─────────────────────────────┤
│    BSS (données non init.)  │  ← Variables globales non initialisées
├─────────────────────────────┤
│    Heap (tas)               │  ← Allocation dynamique (Module 3)
│           ↓                 │
│           ...               │
│           ↑                 │
│    Stack (pile)             │  ← Variables locales, appels de fonctions
├─────────────────────────────┤
│    Kernel space             │  ← Système d'exploitation
└─────────────────────────────┘
```

Dans ce module, nous nous concentrons principalement sur la **pile (stack)** et les variables locales.

---

## ⚠️ Pièges courants et comment les éviter

### 🚨 Piège 1 : Confondre `=` et `==`
```c
int x = 5;
if (x = 10) {  // ❌ Assignment, pas comparaison !
    // Ce bloc s'exécute toujours
}

if (x == 10) {  // ✅ Comparaison correcte
    // ...
}
```

**Solution** : Compilez avec `-Wall -Wextra` pour détecter ce genre d'erreur.

---

### 🚨 Piège 2 : Overflow d'entiers
```c
int a = 2000000000;
int b = 2000000000;
int c = a + b;  // ❌ Overflow ! Comportement indéfini

// ✅ Solution : utiliser long ou détecter l'overflow
long c = (long)a + (long)b;
```

---

### 🚨 Piège 3 : Conversions implicites dangereuses
```c
unsigned int a = 10;
int b = -1;
if (a > b) {  // ❌ Faux ! b est converti en unsigned
    // Cette condition est FAUSSE car -1 devient un très grand nombre unsigned
}

// ✅ Solution : Comparer des types compatibles
if ((int)a > b) { /* ... */ }
```

---

### 🚨 Piège 4 : Switch sans break
```c
switch (x) {
    case 1:
        do_something();
        // ❌ Oubli de break, le code continue dans case 2 !
    case 2:
        do_something_else();
        break;
}

// ✅ Toujours mettre break (ou commenter explicitement le fall-through)
```

---

### 🚨 Piège 5 : Modifications de variables dans les conditions
```c
while (x++ < 10) {  // ❌ Difficile à lire et déboguer
    // ...
}

// ✅ Plus clair
while (x < 10) {
    // ...
    x++;
}
```

---

## 🏆 Critères de maîtrise

Vous avez assimilé ce module si vous pouvez :

### ✅ Niveau 1 : Compréhension des types
- [ ] Expliquer la différence entre `int`, `long`, `short`
- [ ] Calculer la taille de n'importe quel type avec `sizeof`
- [ ] Prédire le résultat de conversions de types
- [ ] Identifier les risques d'overflow
- [ ] Comprendre l'impact de l'endianness

### ✅ Niveau 2 : Maîtrise des opérateurs
- [ ] Utiliser les opérateurs bit-à-bit pour manipuler des flags
- [ ] Comprendre les priorités des opérateurs
- [ ] Écrire des conditions complexes correctes
- [ ] Optimiser les boucles
- [ ] Configurer et utiliser clang-format

### ✅ Niveau 3 : Fonctions et pile
- [ ] Écrire des prototypes corrects
- [ ] Comprendre comment la pile fonctionne
- [ ] Expliquer pourquoi le passage par valeur limite les modifications
- [ ] Utiliser correctement les codes de retour
- [ ] Parser les arguments de `main` (argc/argv)

### ✅ Niveau 4 : Autonomie
- [ ] Lire et comprendre du code C simple
- [ ] Déboguer les erreurs de compilation liées aux types
- [ ] Écrire du code propre et formaté automatiquement
- [ ] Consulter efficacement les man pages
- [ ] Anticiper les comportements indéfinis

---

## 🗓️ Planning de lecture suggéré

### Approche intensive (temps plein)
| Jour | Contenu | Focus |
|------|---------|-------|
| **Jour 1** | Chapitre 4 : Types (sections 4.1-4.3) | Types de base, signed/unsigned |
| **Jour 2** | Chapitre 4 : Types (sections 4.4-4.6) | Conversions, endianness, C23 |
| **Jour 3** | Chapitre 5 : Opérateurs (sections 5.1-5.3) | Arithmétiques, logiques, bitwise |
| **Jour 4** | Chapitre 5 : Contrôle (sections 5.4-5.6) | Conditions, boucles, bonnes pratiques |
| **Jour 5** | Chapitre 5 : clang-format (section 5.7) | Formatage automatique |
| **Jour 6** | Chapitre 6 : Fonctions (sections 6.1-6.3) | Déclaration, pile, passage par valeur |
| **Jour 7** | Chapitre 6 : Fonctions (sections 6.4-6.5) | Retour, main, consolidation |

### Approche à temps partiel (5h/semaine)
| Semaine | Contenu |
|---------|---------|
| **Semaine 1-2** | Chapitre 4 complet |
| **Semaine 3-4** | Chapitre 5 (sans clang-format) |
| **Semaine 5** | Chapitre 5 (clang-format) |
| **Semaine 6-7** | Chapitre 6 complet |

---

## 💡 Conseils de lecture et d'apprentissage

### Pour maximiser votre compréhension

#### 1. **Testez tous les exemples**
Ne vous contentez pas de lire le code. Compilez-le, exécutez-le, modifiez-le.

```bash
# Workflow recommandé pour chaque concept
gcc -Wall -Wextra exemple.c -o exemple
./exemple
# Puis modifiez exemple.c et recompilez
```

#### 2. **Provoquez des erreurs intentionnellement**
C'est en cassant que vous apprenez.

```c
// Testez volontairement les limites
int x = 2147483647;  // INT_MAX
x = x + 1;  // Que se passe-t-il ?
printf("%d\n", x);
```

#### 3. **Utilisez les warnings du compilateur**
Ils sont vos meilleurs professeurs.

```bash
# Compilez TOUJOURS avec ces flags
gcc -Wall -Wextra -Werror -pedantic programme.c
```

#### 4. **Consultez les man pages**
```bash
man 3 printf   # Documentation de printf
man limits.h   # Limites des types
man stdint.h   # Types à taille fixe
```

#### 5. **Gardez des notes**
Maintenez un fichier de référence personnel avec les concepts qui vous posent problème.

---

## 📚 Ressources complémentaires

### Livres de référence
- **The C Programming Language** (K&R) - Chapitres 2-4
- **C Programming: A Modern Approach** (K.N. King) - Chapitres 3-9
- **Modern C** (Jens Gustedt) - Niveaux 1-2

### Documentation en ligne
- [cppreference.com](https://en.cppreference.com/w/c) - Référence complète
- [GCC Warning Options](https://gcc.gnu.org/onlinedocs/gcc/Warning-Options.html)
- [SEI CERT C Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c/SEI+CERT+C+Coding+Standard)

### Outils interactifs
- [Compiler Explorer](https://godbolt.org/) - Voir le code assembleur généré
- [C Tutor](http://pythontutor.com/c.html) - Visualiser l'exécution pas à pas
- [OnlineGDB](https://www.onlinegdb.com/online_c_compiler) - Compilateur en ligne

---

## 🔗 Liens avec les autres modules

### Ce module prépare directement
- **Module 3** : Gestion de la mémoire (pointeurs, allocation dynamique)
  - Les fonctions avec passage par valeur montrent les limites
  - La pile est le fondement pour comprendre les pointeurs

- **Module 4** : Structures de données
  - Les types de base sont combinés en structures
  - Les opérateurs bit-à-bit sont utilisés pour l'optimisation

- **Module 6** : Programmation système
  - Les flags et masques bit-à-bit sont omniprésents
  - Les codes de retour suivent les conventions système

---

## 🎯 Après ce module

### Vous serez capable de
- ✅ Écrire des programmes C simples mais fonctionnels
- ✅ Comprendre la majorité du code C que vous lirez
- ✅ Déboguer les erreurs de types et de logique
- ✅ Formater automatiquement votre code
- ✅ Utiliser les outils de base (GCC, clang-format)

### Vous NE serez PAS encore capable de
- ❌ Manipuler des pointeurs (Module 3)
- ❌ Gérer la mémoire dynamiquement (Module 3)
- ❌ Créer des structures de données complexes (Module 4)
- ❌ Faire de la programmation système (Module 6)
- ❌ Optimiser finement les performances (Module 7)

**C'est normal !** Chaque module construit sur le précédent. Les pointeurs (Module 3) sont le prochain grand défi.

---

## 📑 Prochaines sections

**Commencez votre exploration des fondamentaux :**

- **[Chapitre 4 : Types de données et Représentation mémoire →](04-types-et-memoire/README.md)**
- [Chapitre 5 : Opérateurs et Structures de contrôle](05-operateurs-et-controle/README.md)
- [Chapitre 6 : Les Fonctions](06-fonctions/README.md)

---

## 📝 Note sur la nature de cette formation

Cette formation est un **guide de référence complet**, pas un cours avec exercices guidés.

**Comment l'utiliser :**
- Lisez les explications détaillées de chaque concept
- Testez les exemples de code fournis
- Expérimentez en modifiant les exemples
- Consultez les sections comme une documentation
- Créez vos propres mini-projets pour appliquer les concepts

**Module 9** (Projets de synthèse) propose des études de cas complètes où vous appliquerez tous ces fondamentaux dans des contextes réels.

---

**Bonne lecture, et que votre compréhension du C s'approfondisse à chaque section ! 🚀**


⏭️ [Chapitre 4 : Types de données et Représentation mémoire →](04-types-et-memoire/README.md)

