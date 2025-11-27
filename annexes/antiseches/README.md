🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Annexe B : Antisèches (Cheat Sheets)

## Introduction

Les **cheat sheets** (ou antisèches) sont des aide-mémoires condensés qui regroupent les commandes, options et syntaxes les plus fréquemment utilisées lors du développement en C sous Linux. Cette annexe constitue une référence rapide pour gagner en productivité au quotidien.

### 🎯 Objectif de cette annexe

Contrairement aux modules de formation qui privilégient l'apprentissage progressif et la compréhension en profondeur, ces antisèches sont conçues pour :

- **Accès rapide** : Retrouver instantanément une commande ou une syntaxe
- **Référence en production** : Disposer d'un guide lors du débogage ou de l'optimisation
- **Support lors des tâches répétitives** : Éviter de chercher dans la documentation complète
- **Aide-mémoire pour les certifications** : Réviser les éléments essentiels avant un examen ou un entretien

### 📋 Organisation des cheat sheets

Cette annexe est structurée en **6 grandes catégories** couvrant l'ensemble du cycle de développement en C :

1. **B.1 Options GCC** - Compilation et flags essentiels
2. **B.2 Commandes GDB** - Débogage interactif
3. **B.3 Appels système Linux** - Interface kernel/userspace
4. **B.4 POSIX Threads API** - Programmation concurrente
5. **B.5 CMake commandes principales** - Build system moderne
6. **B.6 Flags de sanitizers et Valgrind** - Détection d'erreurs et analyse mémoire

Chaque cheat sheet suit une **structure standardisée** :
- **Commandes de base** : Les 20% de commandes utilisées 80% du temps
- **Options avancées** : Pour les cas d'usage spécifiques
- **Exemples pratiques** : Code ou commandes prêtes à l'emploi
- **Pièges courants** : Erreurs fréquentes et comment les éviter
- **Liens vers la documentation** : Pour approfondir un sujet

### 💡 Comment utiliser ces cheat sheets ?

#### **Pendant l'apprentissage**
- Imprimez ou gardez ces cheat sheets à portée de main lors des exercices
- Utilisez-les pour valider la syntaxe avant de consulter la documentation complète
- Annotez-les avec vos propres notes et exemples

#### **En production / travail quotidien**
- Configurez des **alias shell** pour les commandes les plus fréquentes
- Intégrez ces commandes dans vos **scripts d'automatisation**
- Partagez-les avec votre équipe pour standardiser les pratiques

#### **Pendant le débogage**
- Section GDB : Naviguez rapidement dans la pile d'exécution
- Section Valgrind : Identifiez le type d'erreur mémoire
- Section GCC : Activez les bons flags de diagnostic

#### **Pour les revues de code**
- Vérifiez que les flags de compilation recommandés sont présents
- Assurez-vous que les tests incluent les sanitizers appropriés
- Validez que CMake suit les bonnes pratiques modernes

### 🔧 Personnalisation recommandée

Ces cheat sheets sont des **bases solides** que vous devriez adapter à votre contexte :

- **Ajoutez vos propres commandes** : Chaque projet a ses spécificités
- **Créez des snippets IDE** : Transformez ces commandes en raccourcis VS Code/Vim
- **Versionnez vos modifications** : Maintenez vos cheat sheets dans Git avec votre équipe
- **Automatisez** : Convertissez les commandes répétitives en Makefile targets ou scripts

### 📚 Formats disponibles

Pour faciliter l'accès, ces cheat sheets sont disponibles en plusieurs formats :

- **Markdown** (version de référence) : Pour consultation en ligne et intégration documentation
- **PDF imprimable** : Format A4/Letter optimisé pour impression recto-verso
- **HTML interactif** : Avec recherche et copie rapide des commandes
- **Carte de référence rapide** (A5 recto-verso) : Contient l'essentiel pour chaque catégorie

### ⚠️ Avertissements et limitations

#### **Ces cheat sheets ne remplacent pas :**
- La **documentation officielle** (`man` pages, docs GCC/CMake)
- La **compréhension des concepts** : Connaître la syntaxe ≠ comprendre le fonctionnement
- Les **bonnes pratiques de sécurité** : Certaines options peuvent introduire des vulnérabilités

#### **Versions des outils**
Les cheat sheets sont basées sur les versions suivantes (Ubuntu 24.04 LTS) :
- **GCC** : 13.x et ultérieur
- **GDB** : 15.x
- **CMake** : 3.28+
- **Valgrind** : 3.22+
- **Clang/LLVM** : 18.x (pour sanitizers)

Certaines options peuvent différer selon votre environnement. Vérifiez toujours avec `--version` et `--help`.

#### **Notation utilisée**
- `<arg>` : Argument obligatoire à remplacer
- `[option]` : Argument optionnel
- `...` : Peut être répété
- `|` : Alternative (choisir une seule option)
- `#` : Commentaire explicatif

### 🚀 Prochaines étapes

Vous êtes maintenant prêt à explorer les antisèches détaillées. Chaque section qui suit est **autonome** et peut être consultée indépendamment selon vos besoins du moment.

**Recommandation** : Commencez par parcourir rapidement l'ensemble des cheat sheets pour avoir une vue d'ensemble, puis revenez en profondeur sur celles qui correspondent à votre tâche actuelle.

---

> 💡 **Astuce ** : Créez un alias shell pointant vers ces cheat sheets :
> ```bash
> alias cheat='vim ~/docs/c-formation-cheatsheets/'
> alias cheat-gcc='bat ~/docs/c-formation-cheatsheets/gcc.md'
> ```

---

- B.1 [Options GCC](/annexes/antiseches/01-options-gcc.md)
- B.2 [Commandes GDB](/annexes/antiseches/02-commandes-gdb.md)
- B.3 [Appels système Linux](/annexes/antiseches/03-appels-systeme-linux.md)
- B.4 [POSIX threads API](/annexes/antiseches/04-posix-threads-api.md)
- B.5 [CMake commandes principales](/annexes/antiseches/05-cmake-commandes.md)
- B.6 [Flags de sanitizers et Valgrind](/annexes/antiseches/06-flags-sanitizers-valgrind.md)

⏭️ [Options GCC](/annexes/antiseches/01-options-gcc.md)
