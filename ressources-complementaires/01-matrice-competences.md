🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Matrice de Compétences par Parcours

## Introduction

Cette matrice vous aide à identifier **quels modules de la formation sont les plus importants** selon votre profil et vos objectifs professionnels. Chaque parcours met l'accent sur des compétences différentes en fonction de votre domaine d'activité.

## Comment lire cette matrice ?

La matrice utilise un système d'étoiles pour indiquer le niveau d'importance de chaque module :

- ⭐ **Optionnel** : Ce module peut être étudié plus tard ou rapidement survolé
- ⭐⭐ **Recommandé** : Ce module apporte des connaissances utiles pour votre parcours
- ⭐⭐⭐ **Essentiel** : Ce module est crucial pour votre profil, à étudier en priorité

---

## 📊 Tableau des Compétences par Parcours

| Module | Backend→DevOps | SysAdmin→Système | Embedded | Débutant Complet |
|--------|:--------------:|:----------------:|:--------:|:----------------:|
| **M1** Écosystème C/Linux | ⭐⭐ | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ |
| **M2** Fondamentaux du Langage | ⭐⭐ | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ |
| **M3** Gestion de la Mémoire | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ |
| **M4** Structures de Données | ⭐⭐ | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ |
| **M5** Outillage DevOps | ⭐⭐⭐ | ⭐⭐ | ⭐⭐ | ⭐⭐⭐ |
| **M6** Programmation Système | ⭐⭐ | ⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐ |
| **M7** Techniques Avancées | ⭐ | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ |
| **M8** CI/CD | ⭐⭐⭐ | ⭐⭐ | ⭐⭐ | ⭐⭐⭐ |
| **M9** Projets de Synthèse | ⭐⭐ | ⭐⭐ | ⭐ | ⭐⭐⭐ |

---

## 📖 Explication Détaillée par Parcours

### 🔧 Parcours Backend → DevOps

**Profil ciblé** : Vous êtes développeur backend (Python, Go, Java...) et souhaitez maîtriser C pour créer des outils système, comprendre l'infrastructure et améliorer l'observabilité.

#### Modules Essentiels (⭐⭐⭐)
- **Module 5 - Outillage DevOps** : CMake, sanitizers, Valgrind sont indispensables pour automatiser les builds et détecter les bugs
- **Module 8 - CI/CD** : GitHub Actions, tests automatisés et packaging sont au cœur de votre métier

#### Modules Recommandés (⭐⭐)
- **Module 1** : Comprendre l'écosystème C/Linux pour travailler efficacement
- **Module 2** : Les bases du langage C pour écrire du code fonctionnel
- **Module 3** : Gestion mémoire pour déboguer et éviter les fuites
- **Module 4** : Organiser le code de manière modulaire et maintenable
- **Module 6** : I/O, processus et sockets pour interagir avec le système
- **Module 9** : Études de cas pour apprendre par l'exemple (CLI tools, monitoring)

#### Modules Optionnels (⭐)
- **Module 7** : Techniques avancées, sauf gestion d'erreurs (Section 25) et sécurité (Section 26)

**Pourquoi cette priorisation ?** Votre objectif est d'automatiser et d'observer, pas de développer des systèmes bas niveau complexes. Vous avez besoin d'outils modernes (CI/CD) plus que d'optimisations micro-architecturales.

---

### 🖥️ Parcours SysAdmin → Ingénieur Système

**Profil ciblé** : Vous administrez des serveurs Linux et voulez comprendre **comment le système fonctionne en profondeur** pour diagnostiquer, optimiser et créer des outils système.

#### Modules Essentiels (⭐⭐⭐)
- **Module 3 - Gestion de la Mémoire** : Comprendre stack/heap est crucial pour analyser les crash et les fuites
- **Module 6 - Programmation Système** : C'est le cœur de votre apprentissage (processus, signaux, threads, sockets, eBPF)

#### Modules Recommandés (⭐⭐)
- **Module 1** : L'écosystème C/Linux pour bien démarrer
- **Module 2** : Fondamentaux nécessaires avant d'attaquer le système
- **Module 4** : Structures de données pour manipuler des informations complexes
- **Module 5 (Sections 13, 15)** : Build systems et debugging (GDB, Valgrind)
- **Module 7** : Gestion d'erreurs, sécurité et performance pour du code production
- **Module 8** : CI/CD pour automatiser le déploiement d'outils système
- **Module 9** : Projets système et debugging avancé

#### Modules Optionnels (⭐)
- Aucun module n'est vraiment optionnel pour ce parcours complet

**Pourquoi cette priorisation ?** Vous devez maîtriser les appels système, les threads, les sockets et eBPF pour diagnostiquer et monitorer efficacement. Le Module 6 est votre priorité absolue.

---

### 🔌 Parcours Embedded → Linux Embarqué

**Profil ciblé** : Vous développez pour des systèmes embarqués (IoT, robotique, automobile) avec des contraintes de mémoire et de performance.

#### Modules Essentiels (⭐⭐⭐)
- **Module 1 - Écosystème** : Comprendre la chaîne de compilation est vital pour la cross-compilation
- **Module 2 - Fondamentaux** : Maîtrise parfaite de la syntaxe car vous n'avez pas de marge d'erreur
- **Module 3 - Mémoire** : Stack/heap, fuites mémoire et allocation sont critiques en embarqué
- **Module 4 - Structures** : Packed structures, bit-fields et unions pour économiser chaque octet
- **Module 7 - Techniques Avancées** : Pointeurs avancés, macros et **optimisation** (Section 27) sont indispensables

#### Modules Recommandés (⭐⭐)
- **Module 5 (Section 13)** : CMake pour gérer la cross-compilation
- **Module 6 (Sections 16-17)** : I/O bas niveau et gestion des processus
- **Module 8 (Section 31)** : Cross-compilation et déploiement ARM
- **Module 5 (Section 15)** : Debugging et sanitizers pour traquer les bugs

#### Modules Optionnels (⭐)
- **Module 9** : Projets de synthèse (moins prioritaire que la pratique directe sur votre hardware)

**Pourquoi cette priorisation ?** En embarqué, chaque octet et chaque cycle CPU comptent. Vous devez maîtriser la mémoire, l'optimisation et la cross-compilation avant tout.

---

### 🎓 Parcours Débutant Complet → Ingénieur C Full-Stack

**Profil ciblé** : Vous débutez en C et souhaitez acquérir une **maîtrise complète** du langage, du code basique au déploiement en production.

#### Modules Essentiels (⭐⭐⭐)
**Tous les modules sont essentiels**, car l'objectif est d'acquérir une vision globale et professionnelle du développement C moderne.

- **Module 1** : Bases indispensables pour démarrer
- **Module 2** : Fondamentaux du langage (types, opérateurs, fonctions)
- **Module 3** : Gestion mémoire (pointeurs, allocation dynamique)
- **Module 4** : Structures de données et organisation du code
- **Module 5** : Outils DevOps modernes (CMake, sanitizers, Valgrind)
- **Module 6** : Programmation système Linux
- **Module 7** : Techniques avancées et optimisation
- **Module 8** : CI/CD et bonnes pratiques professionnelles
- **Module 9** : Projets réels et contribution open-source

#### Modules Recommandés (⭐⭐)
- Aucun, car vous suivez le parcours complet

#### Modules Optionnels (⭐)
- Aucun dans ce parcours

**Pourquoi cette priorisation ?** En tant que débutant visant la maîtrise complète, vous devez construire une base solide (Modules 1-4), puis progressivement monter en compétence vers les outils DevOps (Module 5, 8), la programmation système (Module 6), et enfin les techniques avancées (Module 7, 9).

---

## 🧭 Comment utiliser cette matrice ?

### Étape 1 : Identifiez votre profil
Choisissez le parcours qui correspond le mieux à votre situation actuelle et vos objectifs :
- Vous venez du développement web/backend ? → **Backend → DevOps**
- Vous êtes administrateur système ? → **SysAdmin → Système**
- Vous faites de l'embarqué ? → **Embedded**
- Vous débutez complètement en C ? → **Débutant Complet**

### Étape 2 : Priorisez vos modules
- Commencez par les modules **⭐⭐⭐ Essentiels** de votre parcours
- Complétez avec les modules **⭐⭐ Recommandés**
- Les modules **⭐ Optionnels** peuvent être étudiés plus tard selon vos besoins

### Étape 3 : Adaptez selon votre contexte
Cette matrice est un guide, pas une règle absolue. Vous pouvez :
- Combiner plusieurs parcours si votre profil est hybride
- Approfondir certaines sections spécifiques même si elles sont marquées optionnelles
- Ajuster l'ordre d'apprentissage selon vos projets

---

## 💡 Exemple Concret d'Utilisation

**Profil** : Développeur Python travaillant dans une équipe DevOps

**Parcours choisi** : Backend → DevOps

**Plan d'action** :
1. ✅ **Semaine 1-2** : Module 1 (comprendre l'écosystème C/Linux)
2. ✅ **Semaine 3-4** : Module 2 (apprendre la syntaxe de base)
3. ✅ **Semaine 5-6** : Module 3 (maîtriser la gestion mémoire pour déboguer)
4. ✅ **Semaine 7-8** : Module 5 (CMake, sanitizers → outils DevOps essentiels)
5. ✅ **Semaine 9-10** : Module 8 (CI/CD avec GitHub Actions)
6. ✅ **Semaine 11-12** : Module 6 (I/O, processus, sockets, eBPF pour observabilité)
7. ✅ **Semaine 13** : Module 9 (projet pratique : CLI tool ou agent monitoring)

**Résultat** : En 3 mois, vous maîtrisez C pour vos besoins DevOps sans perdre de temps sur des sujets moins pertinents pour votre métier.

---

## 📈 Évolution et Progression

### Après avoir terminé votre parcours initial

Une fois votre parcours de base terminé, vous pouvez :

1. **Approfondir les modules optionnels** de votre parcours
2. **Explorer d'autres parcours** pour élargir vos compétences
   - Exemple : DevOps → ajouter programmation système (Module 6 complet)
   - Exemple : Embedded → ajouter CI/CD moderne (Module 8)
3. **Contribuer à l'open-source** (Module 9, Section 33.5)
4. **Mentorer d'autres développeurs** en partageant vos apprentissages

---

## 🎯 Résumé

Cette matrice de compétences est votre **boussole** pour naviguer efficacement dans cette formation complète. Elle vous permet de :

- ✅ **Gagner du temps** en ciblant les modules les plus pertinents
- ✅ **Progresser efficacement** en suivant un chemin adapté à votre profil
- ✅ **Atteindre vos objectifs professionnels** sans vous disperser
- ✅ **Adapter votre apprentissage** selon vos besoins réels

**N'oubliez pas** : La meilleure façon d'apprendre C est de **coder régulièrement** (même 30 minutes par jour), de **tester tous les concepts**, et de **casser du code pour comprendre** comment ça fonctionne réellement.

---

**Bonne formation ! 🚀**

⏭️ [Critères d'évaluation et certification](/ressources-complementaires/02-evaluation-certification.md)
