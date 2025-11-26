🔝 Retour au [Sommaire](/SOMMAIRE.md)

# C.2 - VS Code settings.json et extensions

## Table des matières
- [Introduction](#introduction)
- [Pourquoi VS Code pour C ?](#pourquoi-vs-code-pour-c-)
- [Qu'est-ce que settings.json ?](#quest-ce-que-settingsjson-)
- [Accéder aux paramètres VS Code](#acc%C3%A9der-aux-param%C3%A8tres-vs-code)
- [Configuration settings.json complète](#configuration-settingsjson-compl%C3%A8te)
- [Explications détaillées des paramètres](#explications-d%C3%A9taill%C3%A9es-des-param%C3%A8tres)
- [Extensions essentielles pour C](#extensions-essentielles-pour-c)
- [Installation des extensions](#installation-des-extensions)
- [Configuration des extensions](#configuration-des-extensions)
- [Raccourcis clavier utiles](#raccourcis-clavier-utiles)
- [Intégration avec les outils Linux](#int%C3%A9gration-avec-les-outils-linux)
- [Thèmes et personnalisation](#th%C3%A8mes-et-personnalisation)
- [Dépannage et astuces](#d%C3%A9pannage-et-astuces)

---

## Introduction

Visual Studio Code (VS Code) est l'éditeur de code le plus populaire en 2025, plébiscité par les développeurs pour sa légèreté, sa flexibilité et son écosystème d'extensions. Bien configuré, il devient un IDE complet pour la programmation en C sous Linux.

**Prérequis :**
- VS Code installé sur Ubuntu
- Accès terminal (bash)
- GCC, GDB, Make installés

**Si VS Code n'est pas installé :**
```bash
# Téléchargez le paquet .deb depuis https://code.visualstudio.com/
# Ou installez via snap
sudo snap install --classic code
```

---

## Pourquoi VS Code pour C ?

### **Avantages par rapport aux autres éditeurs**

| Critère | VS Code | Vim | CLion | Gedit |
|---------|---------|-----|-------|-------|
| **Courbe d'apprentissage** | ⭐⭐⭐⭐⭐ Facile | ⭐⭐ Difficile | ⭐⭐⭐⭐ Moyenne | ⭐⭐⭐⭐⭐ Très facile |
| **Extensibilité** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐ |
| **Intégration Git** | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐ |
| **Débogage graphique** | ⭐⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐ |
| **Performance** | ⭐⭐⭐⭐ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐⭐ |
| **Gratuit** | ✅ Oui | ✅ Oui | ❌ Non | ✅ Oui |

### **Forces de VS Code pour C/Linux**

- ✅ **IntelliSense** : Autocomplétion intelligente pour C
- ✅ **Débogage intégré** : Interface graphique pour GDB
- ✅ **Terminal intégré** : Compilez et exécutez sans quitter l'éditeur
- ✅ **Extensions DevOps** : CMake, Makefile, Docker, etc.
- ✅ **Personnalisable** : settings.json permet de tout configurer
- ✅ **Communauté active** : Des milliers d'extensions disponibles

---

## Qu'est-ce que settings.json ?

### **Définition**

`settings.json` est le fichier de configuration principal de VS Code. Il contient tous les paramètres personnalisés de votre éditeur sous forme de paires clé-valeur au format JSON.

**Analogie :** C'est comme le fichier `.vimrc` pour Vim, mais en format JSON (JavaScript Object Notation).

### **Types de paramètres**

VS Code a **trois niveaux de configuration** :

1. **Paramètres par défaut** : Intégrés dans VS Code (non modifiables directement)
2. **Paramètres utilisateur** : S'appliquent à tous vos projets (fichier `settings.json` global)
3. **Paramètres de l'espace de travail** : Spécifiques à un projet (fichier `.vscode/settings.json` dans le dossier du projet)

**Ordre de priorité :**
```
Espace de travail > Utilisateur > Défaut
```

### **Emplacement des fichiers**

**Paramètres utilisateur (global) :**
```bash
~/.config/Code/User/settings.json
```

**Paramètres de l'espace de travail (projet) :**
```bash
/chemin/vers/votre/projet/.vscode/settings.json
```

**Important :** Le dossier `.vscode` est généralement ajouté dans `.gitignore` (configuration personnelle), sauf si vous voulez partager la config avec votre équipe.

---

## Accéder aux paramètres VS Code

### **Méthode 1 : Interface graphique (débutants)**

1. Ouvrez VS Code
2. Appuyez sur `Ctrl+,` (ou `Cmd+,` sur Mac)
3. Une interface graphique apparaît avec tous les paramètres

**Avantage :** Vous pouvez chercher des paramètres par mot-clé et voir les descriptions.

### **Méthode 2 : Ouvrir settings.json directement**

1. Appuyez sur `Ctrl+Shift+P` pour ouvrir la **palette de commandes**
2. Tapez : `Preferences: Open Settings (JSON)`
3. Appuyez sur `Entrée`

**Avantage :** Contrôle total et édition rapide du JSON.

### **Méthode 3 : Raccourci clavier rapide**

```
Ctrl+Shift+P  →  Taper "settings json"  →  Entrée
```

### **Différence User Settings vs Workspace Settings**

Dans la palette de commandes, vous verrez :
- `Preferences: Open User Settings (JSON)` → Configuration globale
- `Preferences: Open Workspace Settings (JSON)` → Configuration du projet en cours

**Conseil :** Utilisez **User Settings** pour votre configuration générale C, et **Workspace Settings** pour les spécificités d'un projet (par exemple, chemin vers un Makefile).

---

## Configuration settings.json complète

Voici une configuration `settings.json` complète, commentée et optimisée pour la programmation en C sous Linux :

```json
{
  // ============================================
  // CONFIGURATION VS CODE POUR C (ÉDITION 2025)
  // ============================================

  // -------------------- APPARENCE ET ÉDITEUR --------------------

  // Thème de couleur
  "workbench.colorTheme": "Dark+ (default dark)",

  // Thème d'icônes de fichiers
  "workbench.iconTheme": "vscode-icons",

  // Police de l'éditeur (recommandée pour le code)
  "editor.fontFamily": "'Fira Code', 'Droid Sans Mono', 'monospace'",

  // Taille de la police
  "editor.fontSize": 14,

  // Activer les ligatures (rend les symboles comme != plus jolis)
  "editor.fontLigatures": true,

  // Hauteur des lignes (espace entre les lignes)
  "editor.lineHeight": 22,

  // Afficher les numéros de ligne
  "editor.lineNumbers": "on",

  // Afficher une règle verticale à 80 caractères
  "editor.rulers": [80, 120],

  // Mettre en surbrillance la ligne du curseur
  "editor.renderLineHighlight": "all",

  // Afficher les espaces et tabs (utile pour le débogage)
  "editor.renderWhitespace": "boundary",

  // Mini-map (carte du code à droite)
  "editor.minimap.enabled": true,
  "editor.minimap.maxColumn": 80,

  // Largeur de la barre latérale
  "workbench.sideBar.location": "left",

  // Zoom de l'interface
  "window.zoomLevel": 0,

  // -------------------- COMPORTEMENT DE L'ÉDITEUR --------------------

  // Indentation automatique
  "editor.autoIndent": "full",

  // Taille de l'indentation (4 espaces pour C)
  "editor.tabSize": 4,

  // Insérer des espaces au lieu de tabs
  "editor.insertSpaces": true,

  // Détecter automatiquement l'indentation du fichier
  "editor.detectIndentation": true,

  // Retour à la ligne automatique
  "editor.wordWrap": "off",

  // Sauvegarde automatique
  "files.autoSave": "afterDelay",
  "files.autoSaveDelay": 1000,

  // Formater automatiquement lors de la sauvegarde
  "editor.formatOnSave": true,

  // Formater automatiquement lors de la frappe
  "editor.formatOnType": false,

  // Formater automatiquement lors du collage
  "editor.formatOnPaste": true,

  // Trimmer les espaces en fin de ligne lors de la sauvegarde
  "files.trimTrailingWhitespace": true,

  // Ajouter une ligne vide à la fin des fichiers
  "files.insertFinalNewline": true,

  // Encodage des fichiers
  "files.encoding": "utf8",

  // Fin de ligne (LF pour Linux)
  "files.eol": "\n",

  // -------------------- INTELLISENSE ET COMPLÉTION --------------------

  // Activer les suggestions
  "editor.quickSuggestions": {
    "other": true,
    "comments": false,
    "strings": false
  },

  // Délai avant l'affichage des suggestions (en ms)
  "editor.quickSuggestionsDelay": 10,

  // Accepter les suggestions avec Tab
  "editor.tabCompletion": "on",

  // Taille de la fenêtre de suggestion
  "editor.suggestFontSize": 13,
  "editor.suggestLineHeight": 20,

  // Afficher des snippets (bouts de code réutilisables)
  "editor.snippetSuggestions": "top",

  // Afficher la documentation lors du survol
  "editor.hover.enabled": true,
  "editor.hover.delay": 300,

  // -------------------- RECHERCHE --------------------

  // Exclure certains dossiers de la recherche
  "search.exclude": {
    "**/node_modules": true,
    "**/build": true,
    "**/dist": true,
    "**/.git": true,
    "**/*.o": true,
    "**/*.a": true,
    "**/*.so": true
  },

  // Utiliser .gitignore pour exclure des fichiers
  "search.useGlobalIgnoreFiles": true,
  "search.useIgnoreFiles": true,

  // -------------------- FICHIERS --------------------

  // Associations de types de fichiers
  "files.associations": {
    "*.h": "c",
    "*.c": "c",
    "Makefile": "makefile",
    "*.mk": "makefile"
  },

  // Fichiers à exclure de l'explorateur
  "files.exclude": {
    "**/.git": true,
    "**/*.o": true,
    "**/*.out": true,
    "**/a.out": true,
    "**/.vscode": false
  },

  // Surveillance des fichiers (pour rechargement auto)
  "files.watcherExclude": {
    "**/.git/objects/**": true,
    "**/build/**": true,
    "**/node_modules/**": true
  },

  // -------------------- TERMINAL --------------------

  // Shell par défaut (bash pour Linux)
  "terminal.integrated.defaultProfile.linux": "bash",

  // Police du terminal
  "terminal.integrated.fontFamily": "monospace",
  "terminal.integrated.fontSize": 13,

  // Taille du curseur
  "terminal.integrated.cursorBlinking": true,
  "terminal.integrated.cursorStyle": "line",

  // Copier la sélection automatiquement
  "terminal.integrated.copyOnSelection": true,

  // Nombre de lignes du scrollback
  "terminal.integrated.scrollback": 10000,

  // -------------------- GIT --------------------

  // Activer Git
  "git.enabled": true,

  // Validation automatique
  "git.autofetch": true,

  // Confirmer avant de synchroniser
  "git.confirmSync": false,

  // Afficher les changements inline dans l'éditeur
  "scm.diffDecorations": "all",

  // -------------------- CONFIGURATION SPÉCIFIQUE C --------------------

  // Paramètres pour les fichiers C
  "[c]": {
    "editor.defaultFormatter": "ms-vscode.cpptools",
    "editor.tabSize": 4,
    "editor.insertSpaces": true,
    "editor.formatOnSave": true,
    "editor.rulers": [80]
  },

  // Configuration C/C++ Extension
  "C_Cpp.default.cStandard": "c11",
  "C_Cpp.default.cppStandard": "c++17",
  "C_Cpp.default.intelliSenseMode": "linux-gcc-x64",
  "C_Cpp.default.compilerPath": "/usr/bin/gcc",
  "C_Cpp.default.includePath": [
    "${workspaceFolder}/**",
    "/usr/include",
    "/usr/local/include"
  ],
  "C_Cpp.clang_format_fallbackStyle": "{ BasedOnStyle: LLVM, IndentWidth: 4, UseTab: Never, BreakBeforeBraces: Linux, AllowShortIfStatementsOnASingleLine: false, IndentCaseLabels: false, ColumnLimit: 80 }",
  "C_Cpp.clang_format_style": "file",
  "C_Cpp.errorSquiggles": "enabled",
  "C_Cpp.intelliSenseEngine": "default",
  "C_Cpp.enhancedColorization": "enabled",
  "C_Cpp.autocompleteAddParentheses": true,

  // Configuration CMake
  "cmake.configureOnOpen": false,
  "cmake.buildDirectory": "${workspaceFolder}/build",
  "cmake.generator": "Unix Makefiles",
  "cmake.configureSettings": {
    "CMAKE_BUILD_TYPE": "Debug",
    "CMAKE_EXPORT_COMPILE_COMMANDS": "ON"
  },

  // -------------------- EXTENSIONS SPÉCIFIQUES --------------------

  // Clang-format (formatage du code C)
  "clang-format.executable": "/usr/bin/clang-format",
  "clang-format.style": "file",

  // Code Runner (exécution rapide)
  "code-runner.runInTerminal": true,
  "code-runner.saveFileBeforeRun": true,
  "code-runner.clearPreviousOutput": true,
  "code-runner.executorMap": {
    "c": "cd $dir && gcc -Wall -Wextra -std=c11 -g $fileName -o $fileNameWithoutExt && $dir$fileNameWithoutExt"
  },

  // Error Lens (affiche les erreurs inline)
  "errorLens.enabled": true,
  "errorLens.enabledDiagnosticLevels": ["error", "warning"],

  // Bracket Pair Colorizer 2 (couleurs pour les accolades)
  "bracket-pair-colorizer-2.colors": [
    "Gold",
    "Orchid",
    "LightSkyBlue"
  ],

  // GitLens (Git avancé)
  "gitlens.currentLine.enabled": true,
  "gitlens.hovers.currentLine.over": "line",

  // TODO Highlight
  "todohighlight.keywords": [
    {
      "text": "TODO:",
      "color": "#ff6b6b",
      "backgroundColor": "transparent",
      "overviewRulerColor": "grey"
    },
    {
      "text": "FIXME:",
      "color": "#ffa502",
      "backgroundColor": "transparent"
    },
    {
      "text": "NOTE:",
      "color": "#26de81"
    }
  ],

  // -------------------- SÉCURITÉ ET PERFORMANCE --------------------

  // Désactiver la télémétrie
  "telemetry.telemetryLevel": "off",

  // Limiter l'utilisation mémoire
  "files.maxMemoryForLargeFilesMB": 4096,

  // Désactiver les animations (pour plus de performance)
  "workbench.enableExperiments": false,
  "editor.smoothScrolling": false,

  // -------------------- AUTRES --------------------

  // Confirmation avant la suppression de fichiers
  "explorer.confirmDelete": true,

  // Confirmation avant le glisser-déposer
  "explorer.confirmDragAndDrop": false,

  // Ouvrir les fichiers dans un nouvel onglet
  "workbench.editor.enablePreview": false,

  // Breadcrumbs (chemin du fichier en haut)
  "breadcrumbs.enabled": true,

  // Suggestions de chemins de fichiers
  "path-intellisense.autoSlashAfterDirectory": true,
  "path-intellisense.extensionOnImport": true
}
```

---

## Explications détaillées des paramètres

### **1. Apparence de l'éditeur**

#### **Police de caractères**

```json
"editor.fontFamily": "'Fira Code', 'Droid Sans Mono', 'monospace'"
```

**Pourquoi Fira Code ?**
- Police conçue pour le code
- Support des ligatures (transforme `!=` en `≠`, `->` en `→`)
- Excellente lisibilité

**Installation de Fira Code :**
```bash
sudo apt install fonts-firacode
```

**Alternatives populaires :**
- JetBrains Mono
- Source Code Pro
- Cascadia Code
- Hack

#### **Règles verticales**

```json
"editor.rulers": [80, 120]
```

**Utilité :** Affiche des lignes verticales pour respecter les conventions :
- 80 caractères : Standard historique (limite des terminaux)
- 120 caractères : Limite moderne acceptable

#### **Whitespace rendering**

```json
"editor.renderWhitespace": "boundary"
```

**Valeurs possibles :**
- `"none"` : N'affiche rien
- `"boundary"` : Affiche les espaces entre les mots (recommandé)
- `"all"` : Affiche tous les espaces et tabs (peut être distrayant)

### **2. Indentation et formatage**

```json
"editor.tabSize": 4,
"editor.insertSpaces": true
```

**Standard C :** 4 espaces (pas de tabulations)

**Pourquoi éviter les tabs ?**
- Les tabs s'affichent différemment selon l'éditeur (2, 4, 8 espaces)
- Les espaces garantissent une indentation cohérente partout

**Formatage automatique :**
```json
"editor.formatOnSave": true
```

**Important :** Nécessite une extension de formatage (voir section Extensions).

### **3. Sauvegarde automatique**

```json
"files.autoSave": "afterDelay",
"files.autoSaveDelay": 1000
```

**Options :**
- `"off"` : Pas de sauvegarde automatique
- `"afterDelay"` : Sauvegarde après 1 seconde d'inactivité (recommandé)
- `"onFocusChange"` : Sauvegarde quand vous changez de fenêtre
- `"onWindowChange"` : Sauvegarde quand vous changez d'application

### **4. Configuration spécifique C**

```json
"[c]": {
  "editor.defaultFormatter": "ms-vscode.cpptools",
  "editor.formatOnSave": true
}
```

**Explication :** Les paramètres entre `"[c]"` ne s'appliquent qu'aux fichiers `.c` et `.h`.

**C/C++ Extension - Paramètres critiques :**

```json
"C_Cpp.default.cStandard": "c11"
```
→ Utilise la norme C11 (recommandé en 2025)

```json
"C_Cpp.default.compilerPath": "/usr/bin/gcc"
```
→ Chemin vers GCC (vérifiez avec `which gcc`)

```json
"C_Cpp.default.includePath": [
  "${workspaceFolder}/**",
  "/usr/include"
]
```
→ Dossiers où chercher les fichiers `.h`

### **5. Terminal intégré**

```json
"terminal.integrated.defaultProfile.linux": "bash"
```

**Alternatives :**
- `"zsh"` : Si vous utilisez Zsh
- `"fish"` : Si vous utilisez Fish shell

**Astuce :** Le terminal intégré est accessible avec `Ctrl+`\` (backtick)

### **6. Configuration Git**

```json
"git.autofetch": true
```

**Effet :** VS Code récupère automatiquement les changements du dépôt distant (équivalent de `git fetch`).

```json
"scm.diffDecorations": "all"
```

**Effet :** Affiche les modifications Git directement dans l'éditeur (lignes ajoutées en vert, supprimées en rouge).

---

## Extensions essentielles pour C

### **Liste des extensions recommandées**

| Extension | Éditeur | Fonction | Priorité |
|-----------|---------|----------|----------|
| **C/C++** | Microsoft | IntelliSense, débogage, navigation | ⭐⭐⭐ Essentiel |
| **CMake Tools** | Microsoft | Support CMake complet | ⭐⭐⭐ Essentiel |
| **Makefile Tools** | Microsoft | Support Makefile | ⭐⭐ Recommandé |
| **clangd** | LLVM | Autocomplétion et analyse | ⭐⭐ Alternatif |
| **Code Runner** | Jun Han | Exécution rapide de code | ⭐⭐ Recommandé |
| **GitLens** | GitKraken | Git avancé | ⭐⭐⭐ Essentiel |
| **Error Lens** | Alexander | Erreurs inline | ⭐⭐⭐ Essentiel |
| **Better Comments** | Aaron Bond | Commentaires colorés | ⭐⭐ Recommandé |
| **Bracket Pair Colorizer 2** | CoenraadS | Coloration des accolades | ⭐⭐ Recommandé |
| **Todo Tree** | Gruntfuggly | Gestion des TODOs | ⭐⭐ Recommandé |
| **vscode-icons** | VSCode Icons Team | Icônes de fichiers | ⭐ Optionnel |
| **Path Intellisense** | Christian Kohler | Autocomplétion de chemins | ⭐⭐ Recommandé |
| **Remote - SSH** | Microsoft | Développement distant | ⭐⭐ Recommandé |
| **Docker** | Microsoft | Support Docker | ⭐ Optionnel |
| **Hex Editor** | Microsoft | Éditeur hexadécimal | ⭐ Optionnel |

---

## Installation des extensions

### **Méthode 1 : Interface graphique**

1. Cliquez sur l'icône **Extensions** dans la barre latérale (ou `Ctrl+Shift+X`)
2. Recherchez le nom de l'extension (ex : "C/C++")
3. Cliquez sur **Install**
4. Redémarrez VS Code si demandé

### **Méthode 2 : Ligne de commande**

```bash
# Extension C/C++ (essentielle)
code --install-extension ms-vscode.cpptools

# Extension CMake Tools
code --install-extension ms-vscode.cmake-tools

# Extension Makefile Tools
code --install-extension ms-vscode.makefile-tools

# GitLens
code --install-extension eamodio.gitlens

# Error Lens
code --install-extension usernamehw.errorlens

# Code Runner
code --install-extension formulahendry.code-runner

# Better Comments
code --install-extension aaron-bond.better-comments

# Bracket Pair Colorizer 2
code --install-extension coenraads.bracket-pair-colorizer-2

# Todo Tree
code --install-extension gruntfuggly.todo-tree

# vscode-icons
code --install-extension vscode-icons-team.vscode-icons

# Path Intellisense
code --install-extension christian-kohler.path-intellisense

# Remote SSH
code --install-extension ms-vscode-remote.remote-ssh
```

**Installer toutes les extensions d'un coup :**

```bash
#!/bin/bash
# Script d'installation des extensions VS Code pour C

extensions=(
  "ms-vscode.cpptools"
  "ms-vscode.cmake-tools"
  "ms-vscode.makefile-tools"
  "eamodio.gitlens"
  "usernamehw.errorlens"
  "formulahendry.code-runner"
  "aaron-bond.better-comments"
  "coenraads.bracket-pair-colorizer-2"
  "gruntfuggly.todo-tree"
  "vscode-icons-team.vscode-icons"
  "christian-kohler.path-intellisense"
  "ms-vscode-remote.remote-ssh"
)

for ext in "${extensions[@]}"; do
  echo "Installation de $ext..."
  code --install-extension "$ext"
done

echo "✅ Toutes les extensions sont installées !"
```

**Sauvegardez ce script dans `install_vscode_extensions.sh` et exécutez :**

```bash
chmod +x install_vscode_extensions.sh
./install_vscode_extensions.sh
```

### **Méthode 3 : Fichier extensions.json (projet partagé)**

Créez un fichier `.vscode/extensions.json` dans votre projet :

```json
{
  "recommendations": [
    "ms-vscode.cpptools",
    "ms-vscode.cmake-tools",
    "ms-vscode.makefile-tools",
    "eamodio.gitlens",
    "usernamehw.errorlens",
    "formulahendry.code-runner",
    "aaron-bond.better-comments",
    "coenraads.bracket-pair-colorizer-2",
    "gruntfuggly.todo-tree",
    "vscode-icons-team.vscode-icons"
  ]
}
```

**Effet :** Quand quelqu'un ouvre ce projet, VS Code lui proposera d'installer ces extensions.

---

## Configuration des extensions

### **1. C/C++ Extension (Microsoft)**

**Fichier de configuration : `c_cpp_properties.json`**

VS Code peut le générer automatiquement :

1. Ouvrez un fichier `.c`
2. Appuyez sur `Ctrl+Shift+P`
3. Tapez : `C/C++: Edit Configurations (JSON)`
4. Un fichier `.vscode/c_cpp_properties.json` est créé

**Contenu type :**

```json
{
  "configurations": [
    {
      "name": "Linux",
      "includePath": [
        "${workspaceFolder}/**",
        "/usr/include",
        "/usr/local/include"
      ],
      "defines": [],
      "compilerPath": "/usr/bin/gcc",
      "cStandard": "c11",
      "cppStandard": "c++17",
      "intelliSenseMode": "linux-gcc-x64",
      "compileCommands": "${workspaceFolder}/build/compile_commands.json"
    }
  ],
  "version": 4
}
```

**Explication des champs :**

- `"includePath"` : Dossiers où chercher les `.h`
- `"compilerPath"` : Chemin vers GCC
- `"cStandard"` : Norme C à utiliser (c11, c17, c23)
- `"compileCommands"` : Fichier généré par CMake (pour IntelliSense précis)

### **2. CMake Tools**

**Configuration dans settings.json :**

```json
"cmake.configureOnOpen": false,
"cmake.buildDirectory": "${workspaceFolder}/build",
"cmake.generator": "Unix Makefiles"
```

**Utilisation :**

1. Ouvrez un projet avec `CMakeLists.txt`
2. La barre de statut affiche des options CMake
3. Cliquez sur **Configure** pour configurer le projet
4. Cliquez sur **Build** pour compiler

**Raccourcis clavier CMake :**

| Raccourci | Action |
|-----------|--------|
| `F7` | Compiler le projet |
| `Shift+F7` | Compiler et exécuter |
| `Ctrl+F5` | Lancer sans débogage |
| `F5` | Lancer avec débogage |

### **3. Code Runner**

**Configuration :**

```json
"code-runner.executorMap": {
  "c": "cd $dir && gcc -Wall -Wextra -std=c11 -g $fileName -o $fileNameWithoutExt && $dir$fileNameWithoutExt"
}
```

**Utilisation :**

1. Ouvrez un fichier `.c`
2. Cliquez sur le bouton **▶ Run** en haut à droite
3. Ou utilisez le raccourci `Ctrl+Alt+N`

**Variables disponibles :**

- `$dir` : Répertoire du fichier
- `$fileName` : Nom du fichier avec extension (`main.c`)
- `$fileNameWithoutExt` : Nom sans extension (`main`)
- `$workspaceFolder` : Racine du projet

### **4. Error Lens**

**Effet :** Affiche les erreurs directement dans l'éditeur, à droite de la ligne.

**Configuration :**

```json
"errorLens.enabled": true,
"errorLens.enabledDiagnosticLevels": ["error", "warning", "info"]
```

**Exemple visuel :**

```c
int main() {
    int x = "hello";  // ❌ Error: incompatible pointer to integer conversion
    return 0;
}
```

### **5. GitLens**

**Fonctionnalités :**

- Voir l'auteur de chaque ligne (blame)
- Historique des commits
- Comparaison de branches
- Stash viewer

**Configuration minimaliste :**

```json
"gitlens.currentLine.enabled": true,
"gitlens.codeLens.enabled": false  // Désactive les annotations dans le code
```

### **6. Better Comments**

**Effet :** Colore les commentaires selon leur type.

**Exemple :**

```c
// TODO: Implémenter la fonction de tri   → Rouge
// FIXME: Bug quand n = 0                  → Orange
// NOTE: Cette fonction est optimisée      → Vert
// ? Est-ce que c'est la bonne approche ?  → Bleu
// ! ATTENTION : Code critique             → Rouge vif
```

### **7. Bracket Pair Colorizer 2**

**Effet :** Colore les accolades/parenthèses de couleurs différentes selon le niveau d'imbrication.

**Exemple :**

```c
int main() {           // → Accolade or
    if (x > 0) {       // → Accolade orchidée
        for (;;) {     // → Accolade bleu ciel
            // ...
        }
    }
}
```

**Note :** VS Code 1.67+ inclut cette fonctionnalité nativement, mais l'extension offre plus d'options.

---

## Raccourcis clavier utiles

### **Raccourcis par défaut VS Code**

| Raccourci | Action |
|-----------|--------|
| `Ctrl+S` | Sauvegarder |
| `Ctrl+Shift+S` | Sauvegarder tout |
| `Ctrl+P` | Recherche rapide de fichiers |
| `Ctrl+Shift+P` | Palette de commandes |
| `Ctrl+F` | Rechercher dans le fichier |
| `Ctrl+H` | Rechercher et remplacer |
| `Ctrl+Shift+F` | Rechercher dans tout le projet |
| `Ctrl+D` | Sélectionner l'occurrence suivante |
| `Ctrl+/` | Commenter/décommenter |
| `Ctrl+Shift+K` | Supprimer la ligne |
| `Alt+↑/↓` | Déplacer la ligne |
| `Shift+Alt+↑/↓` | Dupliquer la ligne |
| `Ctrl+Space` | Déclencher IntelliSense |
| `F12` | Aller à la définition |
| `Shift+F12` | Trouver toutes les références |
| `Ctrl+K Ctrl+C` | Commenter le bloc |
| `Ctrl+K Ctrl+U` | Décommenter le bloc |

### **Raccourcis spécifiques C**

| Raccourci | Action | Extension |
|-----------|--------|-----------|
| `F5` | Démarrer le débogage | C/C++ Extension |
| `Shift+F5` | Arrêter le débogage | C/C++ Extension |
| `F9` | Ajouter/retirer un breakpoint | C/C++ Extension |
| `F10` | Step Over | C/C++ Extension |
| `F11` | Step Into | C/C++ Extension |
| `Shift+F11` | Step Out | C/C++ Extension |
| `Ctrl+Alt+N` | Exécuter le code | Code Runner |
| `Ctrl+Alt+M` | Arrêter l'exécution | Code Runner |

### **Personnaliser les raccourcis**

1. `Ctrl+K Ctrl+S` : Ouvrir les raccourcis clavier
2. Recherchez l'action que vous voulez modifier
3. Double-cliquez et entrez le nouveau raccourci

**Fichier de configuration :** `~/.config/Code/User/keybindings.json`

**Exemple : Compiler avec Ctrl+B**

```json
[
  {
    "key": "ctrl+b",
    "command": "workbench.action.tasks.build",
    "when": "editorTextFocus"
  }
]
```

---

## Intégration avec les outils Linux

### **1. Compiler avec GCC**

**Fichier : `tasks.json`**

Créez `.vscode/tasks.json` :

```json
{
  "version": "2.0.0",
  "tasks": [
    {
      "label": "Compiler C (GCC)",
      "type": "shell",
      "command": "gcc",
      "args": [
        "-Wall",
        "-Wextra",
        "-std=c11",
        "-g",
        "${file}",
        "-o",
        "${fileDirname}/${fileBasenameNoExtension}"
      ],
      "group": {
        "kind": "build",
        "isDefault": true
      },
      "problemMatcher": ["$gcc"],
      "detail": "Compile le fichier C actuel avec GCC"
    }
  ]
}
```

**Utilisation :**

- `Ctrl+Shift+B` : Compile le fichier actif
- Les erreurs de compilation apparaissent dans le panneau **Problems**

### **2. Déboguer avec GDB**

**Fichier : `launch.json`**

Créez `.vscode/launch.json` :

```json
{
  "version": "0.2.0",
  "configurations": [
    {
      "name": "GDB: Debug C",
      "type": "cppdbg",
      "request": "launch",
      "program": "${fileDirname}/${fileBasenameNoExtension}",
      "args": [],
      "stopAtEntry": false,
      "cwd": "${fileDirname}",
      "environment": [],
      "externalConsole": false,
      "MIMode": "gdb",
      "setupCommands": [
        {
          "description": "Activer le pretty-printing pour gdb",
          "text": "-enable-pretty-printing",
          "ignoreFailures": true
        }
      ],
      "preLaunchTask": "Compiler C (GCC)",
      "miDebuggerPath": "/usr/bin/gdb",
      "logging": {
        "engineLogging": false
      }
    }
  ]
}
```

**Utilisation :**

1. Ajoutez un **breakpoint** en cliquant à gauche d'une ligne (point rouge)
2. Appuyez sur `F5` pour lancer le débogage
3. Utilisez les contrôles de débogage dans la barre supérieure

**Interface de débogage :**

- **Variables** : Voir les valeurs des variables locales
- **Watch** : Ajouter des expressions à surveiller
- **Call Stack** : Pile d'appels de fonctions
- **Breakpoints** : Liste de tous les points d'arrêt

### **3. Utiliser Make/CMake**

**Pour Make :**

Créez une tâche dans `tasks.json` :

```json
{
  "label": "Make",
  "type": "shell",
  "command": "make",
  "group": {
    "kind": "build",
    "isDefault": true
  },
  "problemMatcher": ["$gcc"]
}
```

**Pour CMake :**

L'extension CMake Tools gère automatiquement la compilation. Configurez juste `settings.json` :

```json
"cmake.configureOnOpen": true,
"cmake.buildDirectory": "${workspaceFolder}/build"
```

### **4. Linting avec cppcheck**

Installez cppcheck :

```bash
sudo apt install cppcheck
```

Ajoutez une tâche dans `tasks.json` :

```json
{
  "label": "Linter (cppcheck)",
  "type": "shell",
  "command": "cppcheck",
  "args": [
    "--enable=all",
    "--inconclusive",
    "--std=c11",
    "${file}"
  ],
  "problemMatcher": {
    "owner": "cppcheck",
    "fileLocation": ["relative", "${workspaceFolder}"],
    "pattern": {
      "regexp": "^(.*):(\\d+):(\\d+):\\s+(warning|error):\\s+(.*)$",
      "file": 1,
      "line": 2,
      "column": 3,
      "severity": 4,
      "message": 5
    }
  }
}
```

---

## Thèmes et personnalisation

### **Thèmes de couleurs populaires**

**Installer un thème :**

1. `Ctrl+Shift+X` → Rechercher le thème
2. Installer et activer

**Thèmes recommandés pour C :**

| Thème | Style | Particularité |
|-------|-------|---------------|
| **One Dark Pro** | Sombre | Très populaire, inspiré d'Atom |
| **Dracula Official** | Sombre | Palette pastel, reposant |
| **Monokai Pro** | Sombre | Coloré, excellent contraste |
| **Nord** | Sombre/Clair | Palette arctique, élégant |
| **Gruvbox** | Sombre/Clair | Rétro, tons chauds |
| **Material Theme** | Sombre/Clair | Material Design de Google |
| **Solarized** | Sombre/Clair | Couleurs scientifiquement choisies |
| **GitHub Theme** | Clair | Style GitHub, professionnel |

**Changer de thème :**

```
Ctrl+K Ctrl+T  →  Sélectionner le thème
```

### **Icônes de fichiers**

**Extensions d'icônes recommandées :**

- **vscode-icons** : Le plus complet
- **Material Icon Theme** : Style Material Design
- **Seti File Icons** : Minimaliste

**Activer :**

```json
"workbench.iconTheme": "vscode-icons"
```

### **Personnalisation des couleurs**

Vous pouvez modifier les couleurs individuellement dans `settings.json` :

```json
"workbench.colorCustomizations": {
  "editor.background": "#1e1e1e",
  "editor.foreground": "#d4d4d4",
  "editorCursor.foreground": "#ffcc00",
  "editor.lineHighlightBackground": "#2a2a2a",
  "editorLineNumber.foreground": "#858585"
}
```

**Personnaliser la coloration syntaxique C :**

```json
"editor.tokenColorCustomizations": {
  "textMateRules": [
    {
      "scope": "keyword.control.c",
      "settings": {
        "foreground": "#569CD6",
        "fontStyle": "bold"
      }
    },
    {
      "scope": "storage.type.c",
      "settings": {
        "foreground": "#4EC9B0"
      }
    }
  ]
}
```

---

## Dépannage et astuces

### **Problèmes courants**

#### **1. IntelliSense ne fonctionne pas**

**Symptômes :**
- Pas d'autocomplétion
- Les erreurs ne sont pas détectées
- `#include <stdio.h>` souligné en rouge

**Solutions :**

**a) Vérifier que GCC est installé :**
```bash
gcc --version
```

**b) Vérifier le chemin du compilateur :**
```json
"C_Cpp.default.compilerPath": "/usr/bin/gcc"
```

**c) Régénérer la configuration :**
```
Ctrl+Shift+P  →  C/C++: Reset IntelliSense Database
```

**d) Créer compile_commands.json avec CMake :**
```bash
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
```

#### **2. Le débogage ne démarre pas**

**Erreur courante :**
```
Unable to start debugging. Unable to establish a connection to GDB.
```

**Solution :**

**a) Vérifier que GDB est installé :**
```bash
sudo apt install gdb
```

**b) Compiler avec le flag `-g` :**
```bash
gcc -g main.c -o main
```

**c) Vérifier `launch.json` :**
```json
"miDebuggerPath": "/usr/bin/gdb"
```

#### **3. Code Runner ne compile pas**

**Symptôme :** Le bouton ▶ ne fait rien ou affiche une erreur.

**Solution :**

Vérifiez `settings.json` :
```json
"code-runner.runInTerminal": true,
"code-runner.executorMap": {
  "c": "cd $dir && gcc -Wall $fileName -o $fileNameWithoutExt && ./$fileNameWithoutExt"
}
```

#### **4. Les extensions sont lentes**

**Solution :**

**a) Désactiver les extensions inutiles :**
```
Ctrl+Shift+X  →  Cliquez sur l'engrenage de l'extension  →  Disable
```

**b) Augmenter la limite mémoire :**
```json
"files.maxMemoryForLargeFilesMB": 8192
```

**c) Exclure les gros dossiers :**
```json
"files.watcherExclude": {
  "**/build/**": true,
  "**/node_modules/**": true
}
```

#### **5. Formatage incorrect**

**Symptôme :** Le code est mal formaté après `Shift+Alt+F`.

**Solution :**

**a) Installer clang-format :**
```bash
sudo apt install clang-format
```

**b) Créer un fichier `.clang-format` à la racine du projet :**
```yaml
BasedOnStyle: LLVM
IndentWidth: 4
UseTab: Never
BreakBeforeBraces: Linux
AllowShortIfStatementsOnASingleLine: false
ColumnLimit: 80
```

**c) Configurer VS Code pour l'utiliser :**
```json
"C_Cpp.clang_format_style": "file"
```

### **Astuces de productivité**

#### **1. Multi-curseurs**

**Ajouter des curseurs :**
- `Ctrl+Alt+↑/↓` : Ajouter un curseur au-dessus/en-dessous
- `Ctrl+D` : Sélectionner l'occurrence suivante du mot
- `Ctrl+Shift+L` : Sélectionner toutes les occurrences

**Exemple :**
```c
int a = 0;
int b = 0;
int c = 0;
// Sélectionnez "int" et appuyez sur Ctrl+Shift+L
// Vous pouvez maintenant modifier les 3 lignes en même temps
```

#### **2. Snippets personnalisés**

**Créer un snippet :**

1. `Ctrl+Shift+P` → `Preferences: Configure User Snippets`
2. Choisissez `c.json`
3. Ajoutez vos snippets :

```json
{
  "Main function": {
    "prefix": "main",
    "body": [
      "#include <stdio.h>",
      "",
      "int main(int argc, char *argv[]) {",
      "    $0",
      "    return 0;",
      "}"
    ],
    "description": "Template fonction main"
  },
  "For loop": {
    "prefix": "fori",
    "body": [
      "for (int ${1:i} = 0; ${1:i} < ${2:n}; ${1:i}++) {",
      "    $0",
      "}"
    ]
  }
}
```

**Utilisation :**
- Tapez `main` puis `Tab` → Template complet
- Tapez `fori` puis `Tab` → Boucle for

#### **3. Recherche et remplacement avec regex**

**Exemple : Renommer toutes les variables `temp` en `tmp` :**

1. `Ctrl+H`
2. Activez le mode regex (icône `.*`)
3. Recherche : `\btemp\b`
4. Remplacement : `tmp`
5. `Ctrl+Alt+Enter` : Remplacer tout

#### **4. Refactoring rapide**

**Renommer un symbole :**
1. Placez le curseur sur une fonction/variable
2. Appuyez sur `F2`
3. Tapez le nouveau nom
4. `Entrée` → Toutes les occurrences sont renommées

#### **5. Navigation rapide**

| Action | Raccourci |
|--------|-----------|
| Aller à la définition | `F12` |
| Voir la définition (peek) | `Alt+F12` |
| Aller à la ligne | `Ctrl+G` |
| Aller au symbole | `Ctrl+Shift+O` |
| Rechercher un symbole dans le projet | `Ctrl+T` |
| Retour arrière | `Alt+←` |
| Retour avant | `Alt+→` |

### **Commandes utiles dans la palette**

Quelques commandes à connaître :

- `Developer: Reload Window` : Redémarre VS Code (utile après installation d'extension)
- `Developer: Toggle Developer Tools` : Console de développement (pour debug VS Code lui-même)
- `Preferences: Open Settings (JSON)` : Ouvre settings.json
- `Tasks: Run Task` : Lance une tâche définie
- `Terminal: Clear` : Efface le terminal

---

## Résumé et checklist

### **Configuration minimale recommandée**

✅ **Extensions essentielles installées :**
- [ ] C/C++ (Microsoft)
- [ ] CMake Tools (Microsoft)
- [ ] GitLens
- [ ] Error Lens
- [ ] Code Runner

✅ **Fichiers de configuration créés :**
- [ ] `settings.json` (global ou workspace)
- [ ] `.vscode/c_cpp_properties.json`
- [ ] `.vscode/tasks.json`
- [ ] `.vscode/launch.json`

✅ **Outils système installés :**
- [ ] GCC (`gcc --version`)
- [ ] GDB (`gdb --version`)
- [ ] Make (`make --version`)
- [ ] CMake (`cmake --version`)
- [ ] clang-format (`clang-format --version`)

### **Workflow typique**

1. **Créer un projet C** :
   ```bash
   mkdir mon_projet_c && cd mon_projet_c
   code .
   ```

2. **Créer un fichier C** :
   ```bash
   touch main.c
   ```

3. **Écrire du code** avec autocomplétion (IntelliSense)

4. **Compiler** :
   - Avec Code Runner : `Ctrl+Alt+N`
   - Avec tâche : `Ctrl+Shift+B`
   - Avec CMake : Cliquez sur **Build** dans la barre de statut

5. **Déboguer** :
   - Ajoutez un breakpoint (clic à gauche du numéro de ligne)
   - `F5` pour lancer le débogage

6. **Versionner** :
   - `Ctrl+Shift+G` : Ouvrir le panneau Git
   - Écrivez un message de commit
   - Cliquez sur ✓ pour commiter

---

## Ressources complémentaires

### **Documentation officielle**

- [VS Code pour C/C++](https://code.visualstudio.com/docs/languages/cpp)
- [C/C++ Extension](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cpptools)
- [CMake Tools](https://marketplace.visualstudio.com/items?itemName=ms-vscode.cmake-tools)

### **Tutoriels vidéo**

- [Configurer VS Code pour C](https://www.youtube.com/results?search_query=vscode+c+setup+linux)
- [Déboguer du C avec VS Code](https://www.youtube.com/results?search_query=vscode+gdb+debug)

### **Communautés**

- [r/vscode](https://www.reddit.com/r/vscode/) : Subreddit VS Code
- [Stack Overflow - vscode tag](https://stackoverflow.com/questions/tagged/visual-studio-code)
- [VS Code GitHub](https://github.com/microsoft/vscode/issues) : Bugs et feature requests

---

## Conclusion

Avec cette configuration complète, VS Code devient un **IDE C professionnel** offrant :

- ✅ Autocomplétion intelligente
- ✅ Débogage graphique avec GDB
- ✅ Intégration Git complète
- ✅ Support CMake/Make
- ✅ Analyse statique en temps réel
- ✅ Formatage automatique
- ✅ Navigation de code avancée

**Prochaines étapes :**

1. Testez la configuration avec un petit projet C
2. Explorez les extensions supplémentaires selon vos besoins
3. Personnalisez les raccourcis clavier et les thèmes
4. Partagez votre configuration avec votre équipe (fichiers `.vscode`)

**Conseil final :** N'hésitez pas à expérimenter avec les paramètres. VS Code est extrêmement flexible, et chacun trouve sa configuration idéale avec le temps.

---

🚀 **Bon code en C avec VS Code !**

⏭️ [.clang-format exemple](/annexes/configuration-environnement/03-clang-format-exemple.md)
