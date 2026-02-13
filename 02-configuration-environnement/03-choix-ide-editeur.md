🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 2.3 Choix de l'IDE/Éditeur

## Introduction

Maintenant que votre toolchain est installée et que vous maîtrisez les versions de GCC, il est temps de choisir votre outil de travail principal : l'éditeur ou l'IDE dans lequel vous allez écrire votre code C.

### Éditeur vs IDE : Quelle est la différence ?

Avant de plonger dans les options, clarifions ces deux termes :

#### Éditeur de texte

Un **éditeur de texte** est comme un bloc-notes amélioré. Il vous permet d'écrire du code avec des fonctionnalités comme :
- Coloration syntaxique (les mots-clés apparaissent en couleur)
- Numérotation des lignes
- Auto-complétion basique
- Recherche et remplacement

**Exemples :** VS Code, Sublime Text, Vim, Nano, Gedit

**Analogie :** C'est comme un traitement de texte pour écrivain, mais conçu pour les programmeurs.

#### IDE (Integrated Development Environment)

Un **IDE** (Environnement de Développement Intégré) est un outil tout-en-un qui intègre :
- Un éditeur de code
- Un compilateur intégré (vous cliquez sur "Build" au lieu de taper des commandes)
- Un débogueur visuel (points d'arrêt, inspection de variables)
- Un gestionnaire de projets
- Parfois des outils de profiling, de tests, etc.

**Exemples :** CLion, Code::Blocks, Eclipse CDT

**Analogie :** C'est comme un atelier complet avec tous les outils déjà installés et prêts à l'emploi.

### Quel est le meilleur choix ?

**Il n'y a pas de "meilleur" outil universel.** Cela dépend de :
- Votre niveau d'expérience
- Vos préférences personnelles
- Les ressources de votre machine
- Le type de projets que vous développez

**Pour les débutants**, nous recommandons généralement **VS Code** car il offre un excellent équilibre entre simplicité et puissance.

---

## Option 1 : Visual Studio Code (Recommandé pour débuter)

### Qu'est-ce que VS Code ?

**Visual Studio Code** (VS Code) est un éditeur de code gratuit et open-source développé par Microsoft. C'est actuellement l'un des éditeurs les plus populaires au monde.

### Pourquoi VS Code est idéal pour débuter en C ?

- ✅ **Gratuit et open-source**
- ✅ **Installation facile**
- ✅ **Extensions puissantes** pour le C/C++
- ✅ **Interface intuitive**
- ✅ **Débogueur intégré** (avec configuration)
- ✅ **Terminal intégré** (vous pouvez compiler sans quitter l'éditeur)
- ✅ **Git intégré** (gestion de versions)
- ✅ **Communauté immense** (beaucoup de tutoriels et d'aide)
- ✅ **Léger** (consomme peu de ressources)

### Installation de VS Code sur Ubuntu

#### Méthode 1 : Snap (recommandée)

```bash
sudo snap install code --classic
```

Cette méthode installe VS Code et le maintient automatiquement à jour.

#### Méthode 2 : Via les dépôts Microsoft

```bash
# Télécharger et installer la clé GPG
wget -qO- https://packages.microsoft.com/keys/microsoft.asc | gpg --dearmor > packages.microsoft.gpg  
sudo install -D -o root -g root -m 644 packages.microsoft.gpg /etc/apt/keyrings/packages.microsoft.gpg  

# Ajouter le dépôt
sudo sh -c 'echo "deb [arch=amd64,arm64,armhf signed-by=/etc/apt/keyrings/packages.microsoft.gpg] https://packages.microsoft.com/repos/code stable main" > /etc/apt/sources.list.d/vscode.list'

# Installer
sudo apt update  
sudo apt install code  
```

#### Méthode 3 : Package .deb

Téléchargez le fichier `.deb` depuis le site officiel : https://code.visualstudio.com/

```bash
sudo dpkg -i code_*.deb  
sudo apt install -f  # Résout les dépendances  
```

### Lancement de VS Code

```bash
code
```

Ou depuis le menu d'applications Ubuntu : cherchez "Visual Studio Code".

### Extensions essentielles pour C

VS Code sans extensions est un simple éditeur de texte. Voici les extensions indispensables pour programmer en C :

#### 1. C/C++ (Microsoft)

**La plus importante !** Elle fournit :
- IntelliSense (auto-complétion intelligente)
- Débogage
- Navigation dans le code
- Formatage

**Installation :**
1. Ouvrez VS Code
2. Cliquez sur l'icône Extensions (carré sur la barre latérale gauche) ou `Ctrl+Shift+X`
3. Recherchez "C/C++"
4. Cliquez sur "Install" pour l'extension développée par Microsoft

#### 2. C/C++ Extension Pack (Microsoft)

Un pack qui regroupe plusieurs extensions utiles :
- C/C++ Themes
- CMake
- CMake Tools

**Installation :** Même procédure, recherchez "C/C++ Extension Pack".

#### 3. Code Runner (Optionnel)

Permet d'exécuter rapidement votre code avec un simple clic.

**Installation :** Recherchez "Code Runner" dans les extensions.

### Configuration de base pour C

#### Créer un dossier de travail

```bash
mkdir ~/projets-c  
cd ~/projets-c  
code .  
```

Cette commande ouvre VS Code dans votre dossier de projets.

#### Créer un fichier C

Dans VS Code :
1. `Ctrl+N` pour un nouveau fichier
2. `Ctrl+S` pour sauvegarder
3. Nommez-le `hello.c`

VS Code détectera automatiquement qu'il s'agit de code C et activera la coloration syntaxique.

#### Compiler depuis le terminal intégré

Ouvrez le terminal intégré avec `Ctrl+` ` (backtick) ou via le menu View → Terminal.

```bash
gcc hello.c -o hello
./hello
```

### Avantages de VS Code

- **Léger et rapide** : Se lance en quelques secondes
- **Personnalisable** : Des milliers d'extensions
- **Multi-plateforme** : Fonctionne sur Linux, Windows, macOS
- **Gratuit** : Aucun coût
- **Mise à jour régulière** : Nouvelles fonctionnalités fréquentes

### Inconvénients de VS Code

- **Configuration initiale** : Nécessite l'installation d'extensions
- **Pas un "vrai" IDE** : Moins intégré qu'un IDE complet comme CLion
- **Peut être distrayant** : Trop d'extensions peuvent ralentir l'éditeur

---

## Option 2 : CLion (IDE professionnel)

### Qu'est-ce que CLion ?

**CLion** est un IDE complet développé par JetBrains, spécialement conçu pour C et C++. C'est un outil professionnel de très haute qualité.

### Pourquoi choisir CLion ?

- ✅ **IDE complet** : Tout est intégré (build, debug, refactoring)
- ✅ **IntelliSense exceptionnel** : La meilleure auto-complétion du marché
- ✅ **Débogueur puissant** : Très visuel et facile à utiliser
- ✅ **Support natif de CMake**
- ✅ **Refactoring automatisé** : Renommer des variables, extraire des fonctions, etc.
- ✅ **Analyse de code en temps réel** : Détecte les erreurs avant la compilation
- ✅ **Intégration Git avancée**

### Installation de CLion

#### Via Snap

```bash
sudo snap install clion --classic
```

#### Via JetBrains Toolbox (Recommandé)

JetBrains Toolbox est un gestionnaire qui facilite l'installation et la mise à jour de tous les IDE JetBrains.

1. Téléchargez Toolbox : https://www.jetbrains.com/toolbox-app/
2. Extrayez l'archive
3. Exécutez le fichier `jetbrains-toolbox`
4. Installez CLion depuis l'interface Toolbox

### Licence et coût

⚠️ **CLion est payant** : Environ 89€/an pour un individu.

**Mais** : Plusieurs options gratuites existent :
- **Étudiants** : Licence gratuite avec une adresse email universitaire
- **Projets open-source** : Licence gratuite
- **Essai gratuit** : 30 jours

Pour vérifier votre éligibilité : https://www.jetbrains.com/community/education/

### Premier lancement

Au premier lancement, CLion vous guide pour :
1. Détecter votre toolchain (GCC, GDB, CMake)
2. Configurer le compilateur
3. Créer votre premier projet

### Avantages de CLion

- **Expérience intégrée** : Tout fonctionne out-of-the-box
- **Productivité maximale** : Génération de code, refactoring intelligent
- **Qualité professionnelle** : Utilisé par de nombreuses entreprises
- **Documentation intégrée** : Hover sur une fonction affiche sa documentation

### Inconvénients de CLion

- **Payant** : Coût non négligeable (sauf étudiants/open-source)
- **Lourd** : Nécessite au moins 8 GB de RAM
- **Courbe d'apprentissage** : Beaucoup de fonctionnalités peuvent dérouter au début
- **Orienté CMake** : Moins adapté si vous utilisez Make simple

---

## Option 3 : Vim/Neovim (Pour les aventureux)

### Qu'est-ce que Vim ?

**Vim** est un éditeur de texte en ligne de commande, extrêmement puissant mais avec une courbe d'apprentissage raide. **Neovim** est une version modernisée de Vim.

### Pourquoi Vim est populaire chez les développeurs système ?

- ✅ **Léger** : Consomme très peu de ressources
- ✅ **Rapide** : Édition ultra-rapide une fois maîtrisé
- ✅ **Disponible partout** : Pré-installé sur presque tous les systèmes Linux
- ✅ **Édition modale** : Une philosophie unique et efficace
- ✅ **Personnalisable** : Configuration via fichiers texte
- ✅ **Pas de souris nécessaire** : Tout au clavier

### Installation

Vim est généralement déjà installé sur Ubuntu. Sinon :

```bash
# Vim
sudo apt install vim

# Neovim (version moderne)
sudo apt install neovim
```

### Lancement

```bash
vim hello.c
# ou
nvim hello.c
```

### Les modes de Vim

Vim fonctionne avec des **modes** :
- **Mode Normal** : Pour naviguer et exécuter des commandes (touche `Esc`)
- **Mode Insertion** : Pour écrire du texte (touche `i`)
- **Mode Visuel** : Pour sélectionner du texte (touche `v`)
- **Mode Commande** : Pour sauvegarder, quitter, etc. (touche `:`)

### Commandes de base

```
i          → Passer en mode insertion  
Esc        → Retour au mode normal  
:w         → Sauvegarder
:q         → Quitter
:wq        → Sauvegarder et quitter
:q!        → Quitter sans sauvegarder
```

### Configuration pour C

Créez un fichier `~/.vimrc` (ou `~/.config/nvim/init.vim` pour Neovim) :

```vim
" Numérotation des lignes
set number

" Coloration syntaxique
syntax on

" Indentation automatique
set autoindent  
set smartindent  

" Tabulations = 4 espaces
set tabstop=4  
set shiftwidth=4  
set expandtab  

" Afficher les espaces invisibles
set list  
set listchars=tab:→\ ,trail:·  

" Recherche intelligente
set ignorecase  
set smartcase  

" Thème
colorscheme desert
```

### Plugins pour C

Avec un gestionnaire de plugins comme **vim-plug**, vous pouvez ajouter :
- **YouCompleteMe** : Auto-complétion
- **NERDTree** : Explorateur de fichiers
- **ALE** : Analyse de code en temps réel

### Avantages de Vim

- **Efficacité ultime** : Une fois maîtrisé, l'édition est extrêmement rapide
- **Légerté** : Fonctionne même sur des machines anciennes
- **SSH-friendly** : Idéal pour éditer des fichiers à distance
- **Philosophie unique** : Une vraie compétence transférable

### Inconvénients de Vim

- **Courbe d'apprentissage très raide** : Peut prendre des semaines à maîtriser
- **Configuration complexe** : Nécessite du temps pour le configurer
- **Pas pour les débutants** : Non recommandé si vous apprenez le C en même temps
- **Débogage** : Moins intégré qu'un IDE (nécessite GDB en ligne de commande)

---

## Option 4 : Code::Blocks (IDE léger et simple)

### Qu'est-ce que Code::Blocks ?

**Code::Blocks** est un IDE open-source, léger et spécialement conçu pour C et C++.

### Pourquoi choisir Code::Blocks ?

- ✅ **Gratuit et open-source**
- ✅ **Léger** : Fonctionne sur des machines modestes
- ✅ **Simple** : Interface claire et intuitive
- ✅ **IDE complet** : Build, debug, gestion de projets
- ✅ **Pas besoin d'extensions** : Tout est intégré

### Installation

```bash
sudo apt install codeblocks
```

### Lancement

```bash
codeblocks
```

Ou depuis le menu d'applications Ubuntu.

### Première utilisation

1. **Créer un nouveau projet** : File → New → Project → Console Application
2. **Choisir C** (pas C++)
3. **Sélectionner GCC** comme compilateur
4. **Écrire votre code**
5. **Cliquer sur Build** (icône d'engrenage) pour compiler
6. **Cliquer sur Run** (icône play) pour exécuter

### Avantages de Code::Blocks

- **Tout intégré** : Pas besoin de configuration complexe
- **Débogueur visuel** : Points d'arrêt, inspection de variables
- **Gestion de projets** : Organisation facile de plusieurs fichiers
- **Léger** : Plus léger que CLion

### Inconvénients de Code::Blocks

- **Interface datée** : Look un peu "vieillot"
- **Moins de fonctionnalités** : Pas de refactoring avancé
- **Communauté plus petite** : Moins de plugins et tutoriels
- **Développement ralenti** : Mises à jour moins fréquentes

---

## Option 5 : Geany (Éditeur ultra-léger)

### Qu'est-ce que Geany ?

**Geany** est un éditeur de texte léger avec quelques fonctionnalités d'IDE.

### Installation

```bash
sudo apt install geany
```

### Pourquoi choisir Geany ?

- ✅ **Extrêmement léger** : Démarre instantanément
- ✅ **Simple** : Pas de complexité inutile
- ✅ **Compilation intégrée** : Boutons pour Build/Execute
- ✅ **Pas de configuration** : Fonctionne immédiatement

### Avantages de Geany

- **Idéal pour les petits scripts**
- **Parfait pour machines anciennes**
- **Interface minimaliste**

### Inconvénients de Geany

- **Pas de vrai débogueur intégré**
- **Auto-complétion basique**
- **Pas adapté aux gros projets**

---

## Option 6 : Autres alternatives

### Eclipse CDT

IDE complet basé sur Eclipse, orienté C/C++.

**Avantages :** Gratuit, puissant, beaucoup de plugins  
**Inconvénients :** Lourd, interface complexe, lent au démarrage  

```bash
sudo snap install eclipse --classic
```

### Sublime Text

Éditeur rapide et élégant, payant mais avec période d'essai illimitée.

**Avantages :** Très rapide, interface moderne  
**Inconvénients :** Payant (80$), pas un vrai IDE  

```bash
sudo snap install sublime-text --classic
```

### Kate (KDE)

Éditeur par défaut de KDE, très complet.

```bash
sudo apt install kate
```

---

## Comparaison rapide

| Outil | Type | Gratuit | Difficulté | Ressources | Recommandé pour |
|-------|------|---------|------------|------------|-----------------|
| **VS Code** | Éditeur+ | ✅ | 🟢 Facile | Léger | Débutants, tous usages |
| **CLion** | IDE | ❌ (€) | 🟡 Moyen | Moyen/Lourd | Projets complexes |
| **Vim/Neovim** | Éditeur | ✅ | 🔴 Difficile | Ultra-léger | SSH, experts |
| **Code::Blocks** | IDE | ✅ | 🟢 Facile | Léger | Débutants, IDE simple |
| **Geany** | Éditeur | ✅ | 🟢 Très facile | Ultra-léger | Scripts simples |
| **Eclipse CDT** | IDE | ✅ | 🔴 Difficile | Lourd | Gros projets d'entreprise |

---

## Comment choisir ?

### Vous débutez complètement en programmation ?

→ **VS Code** ou **Code::Blocks**

Ces deux outils sont suffisamment simples pour ne pas vous submerger, tout en offrant les fonctionnalités essentielles.

### Vous avez déjà programmé dans un autre langage ?

→ **VS Code** (si vous aimez les éditeurs configurables)  
→ **CLion** (si vous préférez un IDE tout intégré et que vous êtes étudiant)

### Vous développez sur des serveurs distants (SSH) ?

→ **Vim/Neovim**

Apprendre Vim est un investissement qui paie sur le long terme si vous travaillez souvent à distance.

### Votre machine est ancienne (4 GB RAM ou moins) ?

→ **Geany** ou **Vim**

Ces outils consomment très peu de ressources.

### Vous voulez devenir DevOps ou SysAdmin ?

→ **VS Code** + apprendre **Vim** progressivement

VS Code pour votre développement local, Vim pour l'édition rapide sur les serveurs.

---

## Notre recommandation pour ce tutoriel

**Pour suivre ce tutoriel, nous recommandons VS Code.**

Pourquoi ?
- C'est gratuit et accessible
- La configuration est simple
- Il offre un bon équilibre entre simplicité et puissance
- Il est utilisé par une grande partie de la communauté
- Les exemples dans ce tutoriel peuvent facilement être adaptés à VS Code

**Mais** : Si vous préférez un autre outil, ce n'est pas un problème ! Tout ce qui est enseigné dans ce cours fonctionne indépendamment de votre éditeur.

---

## Installation recommandée : VS Code pour débutants

Voici la configuration minimale pour bien démarrer avec VS Code :

### Étape 1 : Installer VS Code

```bash
sudo snap install code --classic
```

### Étape 2 : Installer l'extension C/C++

1. Lancez VS Code : `code`
2. `Ctrl+Shift+X` pour ouvrir les extensions
3. Recherchez "C/C++" (par Microsoft)
4. Cliquez sur "Install"

### Étape 3 : Créer un dossier de travail

```bash
mkdir ~/apprendre-c  
cd ~/apprendre-c  
code .  
```

### Étape 4 : Créer votre premier fichier

Dans VS Code :
1. `Ctrl+N` → Nouveau fichier
2. `Ctrl+S` → Sauvegarder sous `hello.c`
3. Écrivez :

```c
#include <stdio.h>

int main(void) {
    printf("Mon environnement est prêt !\n");
    return 0;
}
```

### Étape 5 : Compiler et exécuter

Ouvrez le terminal intégré (`Ctrl+` `) :

```bash
gcc hello.c -o hello
./hello
```

Si vous voyez "Mon environnement est prêt !", vous êtes opérationnel !

---

## Configuration avancée VS Code (optionnel pour l'instant)

Nous verrons dans la section 2.4 comment configurer :
- Les paramètres de formatage automatique
- Les raccourcis clavier personnalisés
- L'intégration Git
- Les thèmes et couleurs
- Les tâches de build automatiques

Pour l'instant, l'installation de base suffit amplement.

---

## Récapitulatif

À ce stade, vous avez :

- ✅ Compris la différence entre éditeur et IDE
- ✅ Découvert les principales options disponibles sur Ubuntu
- ✅ Comparé leurs avantages et inconvénients
- ✅ Choisi (ou commencé à choisir) votre outil de développement
- ✅ Installé votre éditeur/IDE
- ✅ Testé la compilation d'un premier programme

**Prochaine étape :** Dans la section 2.4, nous allons configurer en détail votre environnement de développement pour maximiser votre productivité.

---

## Points clés à retenir

1. **Pas de "meilleur" outil** : Le meilleur outil est celui avec lequel vous êtes à l'aise
2. **VS Code = bon compromis** : Léger, gratuit, puissant, facile pour débuter
3. **CLion = pro** : Si vous êtes étudiant ou voulez un vrai IDE
4. **Vim = efficacité** : Pour les serveurs distants et l'efficacité maximale (mais courbe d'apprentissage)
5. **L'outil ne fait pas le programmeur** : Ce qui compte, c'est votre maîtrise du langage C
6. **Vous pouvez changer** : Rien ne vous empêche de commencer avec VS Code et passer à Vim plus tard

**Conseil :** Ne passez pas des heures à configurer votre éditeur au début. Installez la configuration minimale et concentrez-vous sur l'apprentissage du C. Vous affinerez votre configuration au fil du temps.

---


⏭️ [Configuration des extensions et outils de productivité](/02-configuration-environnement/04-extensions-et-outils.md)
