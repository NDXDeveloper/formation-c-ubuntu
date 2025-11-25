# 🔧 Maîtriser C sur Ubuntu : De la Syntaxe au Système

![License](https://img.shields.io/badge/License-CC%20BY%204.0-blue.svg)
![C Standard](https://img.shields.io/badge/C-C11%20%7C%20C17%20%7C%20C23-orange.svg)
![Platform](https://img.shields.io/badge/Platform-Ubuntu%20%7C%20Linux-purple.svg)
![Completion](https://img.shields.io/badge/Modules-9%2F9-green.svg)
![Language](https://img.shields.io/badge/Langue-Français-blue.svg)

**Une formation complète et moderne pour développeurs et DevOps souhaitant maîtriser le C sous Linux.**

---

## 📖 Table des matières

- [À propos](#-à-propos)
- [Contenu](#-contenu-de-la-formation)
- [Installation](#-démarrage-rapide)
- [Utilisation](#-comment-utiliser-cette-formation)
- [Parcours recommandés](#-parcours-recommandés)
- [Licence](#-licence)
- [Contact](#-contact)

---

## 📋 À propos

Cette formation propose une approche complète du langage C dans l'écosystème Linux moderne. Du débutant à l'expert, vous apprendrez non seulement la syntaxe du langage, mais aussi l'outillage DevOps (CMake, CI/CD, sanitizers), la programmation système, et les technologies émergentes comme eBPF.

**✨ Points clés :**
- 📚 **9 modules progressifs** du niveau débutant au niveau expert
- 🛠️ **Outillage moderne** : CMake, DevContainers, Sanitizers, clang-format
- 🔥 **Focus DevOps** : CI/CD avec GitHub Actions, tests automatisés, packaging
- 🐧 **Programmation système Linux** : processus, threads, sockets, IPC
- ⚡ **Technologies avancées** : eBPF, atomics C11, optimisation
- 🇫🇷 **En français** et gratuit (CC BY 4.0)

**Durée estimée :** Variable selon le parcours (6 à 25 semaines) • **Niveau :** Tous niveaux

---

## 📚 Contenu de la formation

> 📑 **Consultez le [SOMMAIRE.md](SOMMAIRE.md) complet pour la table des matières détaillée**

### Les 9 Modules

**Module 1 : L'Écosystème C sous Linux** *(Débutant)*
- Introduction, normes C (C99, C11, C17, C23), configuration Ubuntu
- **⭐ DevContainers** pour environnements reproductibles

**Module 2 : Fondamentaux du Langage** *(Débutant)*
- Types, opérateurs, structures de contrôle, fonctions
- **⭐ clang-format** et bonnes pratiques

**Module 3 : La Gestion de la Mémoire** *(Intermédiaire)*
- Pointeurs, tableaux, chaînes, allocation dynamique
- Stack vs Heap, fuites mémoire

**Module 4 : Structures de Données et Modularité** *(Intermédiaire)*
- struct, union, enum, listes chaînées, arbres
- Organisation du code et compilation modulaire

**Module 5 : Outillage DevOps et Automatisation** *(Avancé)*
- **🔥 CMake en priorité**, GNU Make
- Bibliothèques statiques/dynamiques
- **⭐ Sanitizers, Valgrind, GDB, analyse statique**

**Module 6 : Programmation Système Linux** *(Expert)*
- I/O bas niveau, processus, signaux
- **⭐ Threads POSIX, atomics C11**
- Sockets, IPC avancé
- **🔥 Introduction à eBPF** (observabilité moderne)

**Module 7 : Techniques Avancées et Optimisation** *(Expert)*
- Pointeurs avancés, macros, préprocesseur
- Gestion d'erreurs robuste, sécurité
- Optimisation, profiling, interopérabilité

**Module 8 : C dans un Pipeline CI/CD** *(Bonus DevOps)*
- Tests unitaires, frameworks (Criterion, Unity)
- **🔥 GitHub Actions** pour projets C (template complet)
- Cross-compilation, packaging (.deb, .rpm, Docker)

**Module 9 : Projets de Synthèse** *(Optionnel)*
- Analyse de code open-source (Git, Redis, Nginx)
- Études de cas DevOps (CLI, monitoring, serveur web)

### Annexes

- **Standards C, man pages, livres de référence**
- **Antisèches** : GCC, GDB, CMake, sanitizers
- **Configurations** : .vimrc, VS Code, .devcontainer.json
- **Glossaire** complet

---

## 🚀 Démarrage rapide

### Prérequis

- **Système** : Ubuntu 22.04+ ou toute distribution Linux récente
- **Niveau** : Aucun prérequis pour les modules débutants
- **Temps** : 30 min à 1h par jour recommandé

### Installation de l'environnement

```bash
# Installation de la toolchain C
sudo apt update
sudo apt install build-essential gcc gdb cmake valgrind clang-format

# Vérification
gcc --version
cmake --version
```

### Cloner cette formation

```bash
git clone https://github.com/NDXDeveloper/formation-c-ubuntu.git
cd formation-c-ubuntu
```

### Premier programme

```bash
# Créer un fichier hello.c
echo '#include <stdio.h>
int main(void) {
    printf("Hello, C World!\\n");
    return 0;
}' > hello.c

# Compiler et exécuter
gcc -Wall -Wextra hello.c -o hello
./hello
```

---

## 📁 Structure du projet

```
formation-c-ubuntu/
├── README.md
├── SOMMAIRE.md                          # Table des matières détaillée
├── LICENSE
├── 01-ecosysteme-c-linux/
├── 02-configuration-environnement/
├── 03-chaine-de-compilation/
├── ...
├── 35-debugging-code-complexe/
├── annexes/
│   ├── references-essentielles/
│   ├── antiseches/
│   ├── configuration-environnement/
│   └── glossaire/
├── parcours-recommandes/
└── ressources-complementaires/
```

---

## 🎯 Comment utiliser cette formation

### 🌱 Débutant complet
👉 Commencez par le [Module 1](01-ecosysteme-c-linux/) et suivez l'ordre séquentiel
👉 Prévoyez 18-25 semaines à temps partiel

### 🔧 Développeur Backend → DevOps
👉 Parcourez les modules 1-3, puis concentrez-vous sur les modules 5 (Outillage), 8 (CI/CD), et la section eBPF du module 6
👉 Durée estimée : 8-12 semaines

### ⚙️ SysAdmin → Ingénieur Système
👉 Priorité absolue au module 6 (Programmation système Linux)
👉 Complétez avec les modules 3, 5, et 7
👉 Durée estimée : 12-16 semaines

### 🎮 Embedded → Linux Embarqué
👉 Modules 1-4 complets, puis module 7 (optimisation)
👉 Ajoutez le module 8 pour la cross-compilation
👉 Durée estimée : 10-14 semaines

**💡 Conseil :** Créez un répertoire de pratique et testez chaque exemple de code !

---

## 🗓️ Parcours recommandés

> 📘 **Consultez les [Parcours Détaillés](parcours-recommandes/) pour des recommandations personnalisées**

| Parcours | Modules prioritaires | Durée | Public cible |
|----------|---------------------|-------|--------------|
| 🚀 **Backend→DevOps** | 1-3, 5 🔥, 6 (eBPF), 8 🔥 | 8-12 sem. | Automatisation, observabilité |
| 🐧 **SysAdmin→Système** | 1-3, 6 🔥, 5, 7 | 12-16 sem. | Compréhension bas niveau |
| 🎮 **Embedded→Linux** | 1-4 🔥, 7 🔥, 8 (cross) | 10-14 sem. | Systèmes contraints |
| 🌟 **Débutant→Expert** | 1→9 séquentiel | 18-25 sem. | Maîtrise complète |

**Légende :** 🔥 = Essentiel pour ce parcours

---

## ⚙️ Technologies et outils couverts

### Compilateurs et Build Systems
- **GCC** (GNU Compiler Collection) - options, optimisation
- **CMake** 🔥 - build system moderne (priorité)
- **GNU Make** - comprendre les Makefile legacy

### Debugging et Analyse
- **Sanitizers** 🔥 (ASan, UBSan, TSan, LSan)
- **Valgrind** (Memcheck, Helgrind, Cachegrind)
- **GDB** - debugging interactif et avancé
- **cppcheck, clang-tidy** - analyse statique

### DevOps et CI/CD
- **GitHub Actions** 🔥 - template complet pour projets C
- **DevContainers** - environnements reproductibles
- **Docker** - conteneurisation multi-stage
- **gcov/lcov** - code coverage

### Programmation Système
- **POSIX Threads** (pthread)
- **Sockets** (TCP/UDP)
- **eBPF** 🔥 - observabilité et networking moderne
- **IPC** - pipes, shared memory, mmap

---

## 🎓 Ce que vous saurez faire

Après cette formation, vous serez capable de :

✅ **Écrire du C moderne** avec les standards C11/C17/C23
✅ **Configurer un environnement de développement professionnel** (IDE, DevContainers, CI/CD)
✅ **Déboguer efficacement** avec sanitizers, Valgrind, et GDB
✅ **Créer des builds multiplateformes** avec CMake
✅ **Développer des outils système** (CLI, agents de monitoring)
✅ **Programmer des applications réseau** (serveurs TCP, HTTP)
✅ **Maîtriser le multithreading** (threads POSIX, synchronisation, atomics)
✅ **Utiliser eBPF** pour l'observabilité et le tracing
✅ **Mettre en place un pipeline CI/CD complet** pour vos projets C
✅ **Optimiser les performances** (profiling, cache awareness, SIMD)

---

## 🎯 Philosophie de la formation

Cette formation adopte une approche **pratique et moderne** :

- **Qualité avant quantité** : Accent sur les outils et pratiques 2025 (sanitizers, CMake, CI/CD)
- **DevOps-first** : Intégration continue dès le début, pas seulement à la fin
- **Sécurité par défaut** : `-Wall -Werror`, validation des entrées, code défensif
- **Observabilité** : eBPF et outils modernes de monitoring
- **Humilité** : Le C est un outil puissant, cette formation vous guide avec pragmatisme

---

## 📝 Licence

Ce projet est sous licence **CC BY 4.0** (Creative Commons Attribution 4.0 International).

✅ Libre d'utiliser, modifier, partager (même commercialement) avec attribution.

**Attribution suggérée :**
```
Formation C Ubuntu par Nicolas DEOUX
https://github.com/NDXDeveloper/formation-c-ubuntu
Licence CC BY 4.0
```

Voir le fichier [LICENSE](LICENSE) pour les détails complets.

---

## 👨‍💻 Contact

**Nicolas DEOUX**
- 📧 [NDXDev@gmail.com](mailto:NDXDev@gmail.com)
- 💼 [LinkedIn](https://www.linkedin.com/in/nicolas-deoux-ab295980/)
- 🐙 [GitHub](https://github.com/NDXDeveloper)

---

## 🙏 Remerciements

Un grand merci à la communauté C et Linux, aux mainteneurs de GCC, Valgrind, CMake, et à tous les contributeurs open-source qui rendent ces technologies accessibles.

**Ressources qui ont inspiré cette formation :**
- [The C Programming Language](https://en.wikipedia.org/wiki/The_C_Programming_Language) (K&R)
- [Advanced Programming in the UNIX Environment](https://stevens.netmeister.org/631/) (Stevens)
- [Linux Kernel Documentation](https://www.kernel.org/doc/html/latest/)
- [eBPF.io](https://ebpf.io/)

---

<div align="center">

**🔧 Bon apprentissage avec le C sous Linux ! 🐧**

[![Follow](https://img.shields.io/github/followers/NDXDeveloper?style=social)](https://github.com/NDXDeveloper)

**[⬆ Retour en haut](#-maîtriser-c-sur-ubuntu--de-la-syntaxe-au-système)**

*Dernière mise à jour : Novembre 2025*

</div>
