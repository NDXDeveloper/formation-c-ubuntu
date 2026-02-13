🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 2.4 Configuration des extensions et outils de productivité

## Introduction

Maintenant que vous avez choisi et installé votre éditeur, il est temps de le transformer en un véritable outil de productivité. Une configuration bien pensée peut vous faire gagner des heures de travail et rendre votre expérience de développement beaucoup plus agréable.

**Analogie :** C'est comme aménager un atelier. Vous avez les outils de base, maintenant il faut les organiser, ajouter un bon éclairage, des raccourcis pratiques, pour travailler efficacement.

Dans cette section, nous nous concentrerons principalement sur **VS Code** (l'outil recommandé pour débuter), mais nous couvrirons aussi les configurations de base pour d'autres éditeurs.

---

## Configuration de VS Code pour le C

### Philosophie de configuration

**Règle d'or :** Commencez simple, ajoutez progressivement.

Ne tombez pas dans le piège de passer des jours à configurer votre éditeur. Commencez avec une configuration minimale fonctionnelle, puis ajoutez des outils au fur et à mesure que vous en ressentez le besoin.

---

## Extensions essentielles pour VS Code

### 1. C/C++ (Microsoft) - INDISPENSABLE

Cette extension est la pierre angulaire du développement C dans VS Code.

**Installation :**
1. `Ctrl+Shift+X` pour ouvrir le panneau des extensions
2. Recherchez "C/C++"
3. Installez l'extension par Microsoft (icône avec le logo MS)

**Fonctionnalités fournies :**
- **IntelliSense** : Auto-complétion intelligente du code
- **Navigation** : Aller à la définition (`F12`), trouver les références
- **Formatage** : Code formaté automatiquement
- **Débogage** : Intégration avec GDB
- **Détection d'erreurs** : Soulignement des erreurs en temps réel

**Configuration de l'extension C/C++ :**

Ouvrez les paramètres (`Ctrl+,`) et recherchez "C_Cpp". Voici les paramètres recommandés :

```json
{
    "C_Cpp.default.cStandard": "c17",
    "C_Cpp.default.compilerPath": "/usr/bin/gcc",
    "C_Cpp.default.intelliSenseMode": "linux-gcc-x64",
    "C_Cpp.errorSquiggles": "enabled",
    "C_Cpp.autocomplete": "default"
}
```

**Comment accéder aux paramètres JSON :**
1. `Ctrl+Shift+P` pour ouvrir la palette de commandes
2. Tapez "Preferences: Open User Settings (JSON)"
3. Ajoutez les lignes ci-dessus dans le fichier

### 2. C/C++ Extension Pack (Microsoft) - RECOMMANDÉ

Un pack qui regroupe plusieurs extensions utiles.

**Contenu du pack :**
- C/C++ Themes : Thèmes de couleurs optimisés pour C
- CMake : Support de CMake
- CMake Tools : Outils d'intégration CMake

**Installation :**
Recherchez "C/C++ Extension Pack" dans les extensions.

### 3. Code Runner - TRÈS UTILE

Permet d'exécuter rapidement votre code avec un simple raccourci.

**Installation :**
Recherchez "Code Runner" dans les extensions (par Jun Han).

**Utilisation :**
- Clic droit sur votre fichier `.c` → "Run Code"
- Ou raccourci : `Ctrl+Alt+N`

**Configuration recommandée :**

```json
{
    "code-runner.runInTerminal": true,
    "code-runner.clearPreviousOutput": true,
    "code-runner.saveFileBeforeRun": true,
    "code-runner.executorMap": {
        "c": "cd $dir && gcc -Wall -Wextra -std=c17 $fileName -o $fileNameWithoutExt && ./$fileNameWithoutExt"
    }
}
```

Cette configuration :
- Exécute le code dans le terminal (pour voir les entrées/sorties)
- Efface la sortie précédente
- Sauvegarde automatiquement avant d'exécuter
- Compile avec les warnings activés (`-Wall -Wextra`)
- Utilise `executorMap` pour définir la commande spécifique au langage C

### 4. Error Lens - POUR VOIR LES ERREURS CLAIREMENT

Affiche les erreurs et warnings directement dans le code, à côté de la ligne concernée.

**Installation :**
Recherchez "Error Lens" (par Alexander).

**Résultat :** Les erreurs apparaissent en rouge directement dans votre code, plus besoin d'aller chercher dans le panneau de problèmes.

### 5. Coloration des paires d'accolades (natif) - UTILE

Colore les paires de parenthèses/accolades pour mieux les identifier.

**Note :** Cette fonctionnalité est intégrée nativement dans VS Code depuis la version 1.60. Il n'est pas nécessaire d'installer d'extension tierce.

**Activation :**
```json
{
    "editor.bracketPairColorization.enabled": true,
    "editor.guides.bracketPairs": "active"
}
```

### 6. indent-rainbow - POUR L'INDENTATION

Colore les niveaux d'indentation pour mieux visualiser la structure du code.

**Installation :**
Recherchez "indent-rainbow" (par oderwat).

### 7. Better C++ Syntax - OPTIONNEL

Améliore la coloration syntaxique pour C et C++.

**Installation :**
Recherchez "Better C++ Syntax" (par Jeff Hykin).

### 8. GitLens - POUR GIT

Améliore l'intégration Git dans VS Code (nous verrons Git plus tard).

**Installation :**
Recherchez "GitLens" (par GitKraken).

### 9. Todo Tree - POUR LES ANNOTATIONS

Met en évidence les commentaires TODO, FIXME, etc. dans votre code.

**Installation :**
Recherchez "Todo Tree" (par Gruntfuggly).

**Exemple :**
```c
// TODO: Optimiser cette fonction
// FIXME: Bug si n < 0
```

Ces commentaires apparaîtront dans un panneau dédié.

### 10. Bookmarks - POUR NAVIGUER

Permet de marquer des lignes importantes et d'y revenir rapidement.

**Installation :**
Recherchez "Bookmarks" (par Alessandro Fragnani).

---

## Configuration des paramètres VS Code

Ouvrez les paramètres utilisateur en JSON (`Ctrl+Shift+P` → "Preferences: Open User Settings (JSON)").

Voici une configuration complète recommandée pour débuter en C :

```json
{
    // ============================================
    // ÉDITEUR - APPARENCE
    // ============================================
    "editor.fontSize": 14,
    "editor.fontFamily": "'Fira Code', 'Cascadia Code', 'JetBrains Mono', monospace",
    "editor.fontLigatures": true,
    "editor.lineNumbers": "on",
    "editor.minimap.enabled": true,
    "editor.rulers": [80, 120],
    "workbench.colorTheme": "Dark+ (default dark)",

    // ============================================
    // ÉDITEUR - COMPORTEMENT
    // ============================================
    "editor.tabSize": 4,
    "editor.insertSpaces": true,
    "editor.detectIndentation": false,
    "editor.wordWrap": "off",
    "editor.formatOnSave": true,
    "editor.formatOnPaste": false,
    "editor.autoClosingBrackets": "always",
    "editor.autoClosingQuotes": "always",
    "editor.suggestSelection": "first",
    "editor.acceptSuggestionOnEnter": "on",

    // ============================================
    // ÉDITEUR - GUIDES VISUELS
    // ============================================
    "editor.bracketPairColorization.enabled": true,
    "editor.guides.bracketPairs": "active",
    "editor.guides.indentation": true,
    "editor.renderWhitespace": "boundary",
    "editor.renderControlCharacters": false,

    // ============================================
    // FICHIERS
    // ============================================
    "files.autoSave": "onFocusChange",
    "files.trimTrailingWhitespace": true,
    "files.insertFinalNewline": true,
    "files.encoding": "utf8",
    "files.eol": "\n",

    // ============================================
    // TERMINAL
    // ============================================
    "terminal.integrated.fontSize": 13,
    "terminal.integrated.fontFamily": "monospace",
    "terminal.integrated.cursorStyle": "line",

    // ============================================
    // C/C++ CONFIGURATION
    // ============================================
    "C_Cpp.default.cStandard": "c17",
    "C_Cpp.default.compilerPath": "/usr/bin/gcc",
    "C_Cpp.default.intelliSenseMode": "linux-gcc-x64",
    "C_Cpp.errorSquiggles": "enabled",
    "C_Cpp.clang_format_fallbackStyle": "{ BasedOnStyle: LLVM, IndentWidth: 4, ColumnLimit: 80 }",

    // ============================================
    // CODE RUNNER
    // ============================================
    "code-runner.runInTerminal": true,
    "code-runner.clearPreviousOutput": true,
    "code-runner.saveFileBeforeRun": true,
    "code-runner.executorMap": {
        "c": "cd $dir && gcc -Wall -Wextra -std=c17 $fileName -o $fileNameWithoutExt && ./$fileNameWithoutExt"
    },

    // ============================================
    // DIVERS
    // ============================================
    "explorer.confirmDelete": false,
    "explorer.confirmDragAndDrop": false,
    "telemetry.telemetryLevel": "off"
}
```

### Explications des paramètres importants

#### Apparence

- **fontSize: 14** : Taille de police confortable (ajustez selon vos préférences)
- **fontFamily** : Polices avec ligatures (rendent le code plus lisible)
- **fontLigatures** : Active les ligatures (== devient ≡, != devient ≠, etc.)
- **rulers: [80, 120]** : Lignes verticales pour limiter la longueur des lignes

#### Comportement

- **tabSize: 4** : 4 espaces par tabulation (standard C)
- **insertSpaces: true** : Utilise des espaces au lieu de tabulations
- **formatOnSave: true** : Formate automatiquement le code quand vous sauvegardez
- **autoClosingBrackets** : Ferme automatiquement les accolades

#### Fichiers

- **autoSave: "onFocusChange"** : Sauvegarde automatiquement quand vous changez de fenêtre
- **trimTrailingWhitespace: true** : Supprime les espaces en fin de ligne
- **insertFinalNewline: true** : Ajoute une ligne vide à la fin du fichier (convention Unix)
- **eol: "\n"** : Utilise le format Unix pour les fins de ligne

---

## Raccourcis clavier essentiels

### Raccourcis VS Code par défaut

| Raccourci | Action |
|-----------|--------|
| `Ctrl+S` | Sauvegarder |
| `Ctrl+Shift+S` | Sauvegarder sous... |
| `Ctrl+N` | Nouveau fichier |
| `Ctrl+O` | Ouvrir un fichier |
| `Ctrl+W` | Fermer l'onglet actuel |
| `Ctrl+Shift+P` | Palette de commandes |
| `Ctrl+P` | Recherche rapide de fichier |
| `Ctrl+,` | Paramètres |
| `Ctrl+B` | Afficher/masquer la barre latérale |
| `Ctrl+` ` | Afficher/masquer le terminal |

### Raccourcis pour l'édition

| Raccourci | Action |
|-----------|--------|
| `Ctrl+X` | Couper la ligne (si rien n'est sélectionné) |
| `Ctrl+C` | Copier la ligne (si rien n'est sélectionné) |
| `Ctrl+V` | Coller |
| `Ctrl+D` | Sélectionner le mot suivant identique |
| `Ctrl+Shift+K` | Supprimer la ligne |
| `Alt+↑` / `Alt+↓` | Déplacer la ligne vers le haut/bas |
| `Shift+Alt+↑` / `Shift+Alt+↓` | Dupliquer la ligne vers le haut/bas |
| `Ctrl+/` | Commenter/décommenter |
| `Ctrl+]` / `Ctrl+[` | Indenter/désindenter |
| `Ctrl+Space` | Déclencher l'auto-complétion |

### Raccourcis pour la navigation

| Raccourci | Action |
|-----------|--------|
| `Ctrl+G` | Aller à la ligne |
| `Ctrl+F` | Rechercher |
| `Ctrl+H` | Rechercher et remplacer |
| `Ctrl+Shift+F` | Rechercher dans tous les fichiers |
| `F12` | Aller à la définition |
| `Alt+F12` | Aperçu de la définition |
| `Shift+F12` | Trouver toutes les références |
| `Ctrl+Tab` | Naviguer entre les onglets |

### Raccourcis pour le débogage

| Raccourci | Action |
|-----------|--------|
| `F5` | Démarrer le débogage |
| `F9` | Placer/enlever un breakpoint |
| `F10` | Step over (passer à la ligne suivante) |
| `F11` | Step into (entrer dans la fonction) |
| `Shift+F11` | Step out (sortir de la fonction) |

### Personnaliser les raccourcis

Pour modifier les raccourcis : `Ctrl+K Ctrl+S` (ou File → Preferences → Keyboard Shortcuts).

---

## Installation de polices de code

Les polices avec ligatures rendent le code plus lisible.

### Fira Code (Recommandée)

```bash
# Télécharger et installer
sudo apt install fonts-firacode
```

Ou manuellement :
1. Téléchargez depuis https://github.com/tonsky/FiraCode
2. Décompressez l'archive
3. Copiez les fichiers `.ttf` dans `~/.fonts/`
4. Actualisez : `fc-cache -f -v`

### JetBrains Mono

```bash
# Télécharger
wget https://download.jetbrains.com/fonts/JetBrainsMono-2.304.zip

# Décompresser
unzip JetBrainsMono-2.304.zip -d ~/.fonts/

# Actualiser
fc-cache -f -v
```

### Cascadia Code

Téléchargez depuis https://github.com/microsoft/cascadia-code/releases

Installez comme Fira Code.

**Après installation :** Redémarrez VS Code et la police apparaîtra dans les paramètres.

---

## Thèmes recommandés

### Thèmes sombres

**Dark+ (default dark)** : Le thème par défaut, excellent pour commencer.

**One Dark Pro** : Populaire et agréable pour les yeux.
```
Ctrl+Shift+X → Rechercher "One Dark Pro"
```

**Dracula Official** : Contraste élevé, très populaire.

**Monokai Pro** : Classique, utilisé par beaucoup de développeurs.

### Thèmes clairs

**Light+ (default light)** : Le thème clair par défaut.

**Solarized Light** : Doux pour les yeux en extérieur ou environnement lumineux.

### Changer de thème

`Ctrl+K Ctrl+T` puis sélectionnez un thème dans la liste.

---

## Snippets (fragments de code)

Les snippets sont des modèles de code réutilisables.

### Utiliser les snippets intégrés

Dans un fichier `.c`, tapez :
- `for` puis `Tab` → Génère une boucle for
- `if` puis `Tab` → Génère une condition if
- `main` puis `Tab` → Génère la fonction main

### Créer vos propres snippets

`Ctrl+Shift+P` → "Preferences: Configure User Snippets" → "c.json"

Exemple de snippet personnalisé :

```json
{
    "Header comment": {
        "prefix": "header",
        "body": [
            "/*",
            " * Fichier: $TM_FILENAME",
            " * Auteur: ${1:Votre nom}",
            " * Date: $CURRENT_DATE/$CURRENT_MONTH/$CURRENT_YEAR",
            " * Description: ${2:Description}",
            " */",
            "",
            "$0"
        ],
        "description": "Ajoute un en-tête de fichier"
    },

    "Function comment": {
        "prefix": "fcom",
        "body": [
            "/**",
            " * @brief ${1:Description courte}",
            " * ",
            " * ${2:Description détaillée}",
            " * ",
            " * @param ${3:param} ${4:Description du paramètre}",
            " * @return ${5:Description du retour}",
            " */",
            "$0"
        ],
        "description": "Commentaire de fonction"
    },

    "Safe malloc": {
        "prefix": "smalloc",
        "body": [
            "${1:type} *${2:ptr} = malloc(sizeof(${1:type}) * ${3:count});",
            "if (${2:ptr} == NULL) {",
            "    fprintf(stderr, \"Erreur d'allocation mémoire\\\\n\");",
            "    exit(EXIT_FAILURE);",
            "}",
            "$0"
        ],
        "description": "Allocation mémoire avec vérification"
    }
}
```

**Utilisation :**
- Tapez `header` puis `Tab` → Insère un en-tête de fichier
- Tapez `fcom` puis `Tab` → Insère un commentaire de fonction
- Tapez `smalloc` puis `Tab` → Insère un malloc sécurisé

---

## Configuration de tasks.json (tâches de build)

Les tâches permettent de compiler rapidement avec un raccourci.

### Créer un fichier tasks.json

1. `Ctrl+Shift+P` → "Tasks: Configure Task"
2. Sélectionnez "Create tasks.json from template"
3. Choisissez "Others"

Remplacez le contenu par :

```json
{
    "version": "2.0.0",
    "tasks": [
        {
            "label": "Compiler C (debug)",
            "type": "shell",
            "command": "gcc",
            "args": [
                "-g",
                "-Wall",
                "-Wextra",
                "-std=c17",
                "${file}",
                "-o",
                "${fileDirname}/${fileBasenameNoExtension}"
            ],
            "group": {
                "kind": "build",
                "isDefault": true
            },
            "problemMatcher": ["$gcc"],
            "detail": "Compile le fichier actuel avec GCC (mode debug)"
        },
        {
            "label": "Compiler C (release)",
            "type": "shell",
            "command": "gcc",
            "args": [
                "-O2",
                "-Wall",
                "-Wextra",
                "-std=c17",
                "${file}",
                "-o",
                "${fileDirname}/${fileBasenameNoExtension}"
            ],
            "problemMatcher": ["$gcc"],
            "detail": "Compile le fichier actuel avec GCC (optimisé)"
        },
        {
            "label": "Nettoyer",
            "type": "shell",
            "command": "rm",
            "args": [
                "-f",
                "${fileDirname}/${fileBasenameNoExtension}"
            ],
            "detail": "Supprime l'exécutable"
        }
    ]
}
```

### Utiliser les tâches

- `Ctrl+Shift+B` : Lance la tâche de build par défaut (debug)
- `Ctrl+Shift+P` → "Tasks: Run Task" : Choisir une tâche spécifique

---

## Configuration du formatage automatique

### Utiliser clang-format

Installez clang-format :

```bash
sudo apt install clang-format
```

### Créer un fichier .clang-format

À la racine de votre projet, créez `.clang-format` :

```yaml
---
BasedOnStyle: LLVM  
IndentWidth: 4  
ColumnLimit: 80  
UseTab: Never  
BreakBeforeBraces: Linux  
AllowShortIfStatementsOnASingleLine: false  
IndentCaseLabels: true  
PointerAlignment: Right  
SpaceBeforeParens: ControlStatements  
```

### Activer le formatage dans VS Code

Dans vos paramètres (`settings.json`) :

```json
{
    "editor.formatOnSave": true,
    "editor.defaultFormatter": "ms-vscode.cpptools",
    "C_Cpp.formatting": "clangFormat"
}
```

Maintenant, quand vous sauvegardez (`Ctrl+S`), votre code sera automatiquement formaté.

---

## Configuration pour d'autres éditeurs

### CLion

CLion est déjà bien configuré out-of-the-box. Quelques ajustements recommandés :

**Paramètres (Ctrl+Alt+S) :**

1. **Editor → Code Style → C/C++** : Ajustez l'indentation (4 espaces recommandé)
2. **Editor → Font** : Choisissez une police avec ligatures (Fira Code, JetBrains Mono)
3. **Keymap** : Choisissez le keymap qui vous convient (Default, VS Code, Emacs, Vim...)
4. **Plugins** : Installez "Rainbow Brackets" pour colorier les paires d'accolades

**Plugins recommandés :**
- Key Promoter X (apprend les raccourcis clavier)
- .ignore (gestion des fichiers .gitignore)

### Vim/Neovim

Configuration de base dans `~/.vimrc` :

```vim
" ===================================
" GÉNÉRAL
" ===================================
set number              " Numéros de ligne  
set relativenumber      " Numéros relatifs  
set ruler               " Position du curseur  
set showcmd             " Affiche les commandes  
set wildmenu            " Menu de complétion  
set mouse=a             " Support souris  

" ===================================
" INDENTATION
" ===================================
set tabstop=4           " Tabulations = 4 espaces  
set shiftwidth=4        " Indentation = 4 espaces  
set expandtab           " Convertir tab en espaces  
set autoindent          " Auto-indentation  
set smartindent         " Indentation intelligente  

" ===================================
" RECHERCHE
" ===================================
set incsearch           " Recherche incrémentale  
set hlsearch            " Surligne les résultats  
set ignorecase          " Ignore la casse  
set smartcase           " Casse intelligente  

" ===================================
" APPARENCE
" ===================================
syntax on               " Coloration syntaxique  
colorscheme desert      " Thème  
set background=dark  
set cursorline          " Surligne la ligne actuelle  

" ===================================
" FICHIERS
" ===================================
set encoding=utf-8  
set fileencoding=utf-8  

" ===================================
" COMPILATION C
" ===================================
" F5 pour compiler et exécuter
autocmd FileType c map <F5> :w<CR>:!gcc -Wall -Wextra -std=c17 % -o %< && ./%< <CR>

" F6 pour compiler seulement
autocmd FileType c map <F6> :w<CR>:!gcc -Wall -Wextra -std=c17 % -o %< <CR>
```

**Plugins recommandés avec vim-plug :**

Installez vim-plug :
```bash
curl -fLo ~/.vim/autoload/plug.vim --create-dirs \
    https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim
```

Ajoutez dans `~/.vimrc` :

```vim
call plug#begin('~/.vim/plugged')

" Auto-complétion
Plug 'ycm-core/YouCompleteMe'

" Explorateur de fichiers
Plug 'preservim/nerdtree'

" Barre de statut
Plug 'vim-airline/vim-airline'

" Git integration
Plug 'tpope/vim-fugitive'

" Commentaires rapides
Plug 'tpope/vim-commentary'

" Pairs d'accolades colorées
Plug 'luochen1990/rainbow'

call plug#end()

" Activer rainbow brackets
let g:rainbow_active = 1
```

Puis installez les plugins : `:PlugInstall`

### Code::Blocks

Configuration minimale :

1. **Settings → Editor** :
   - Tab size: 4
   - Use spaces instead of tabs: Coché

2. **Settings → Compiler** :
   - Compiler flags: Activez `-Wall`, `-Wextra`, `-pedantic`
   - C standard: Sélectionnez `c17` ou `c11`

3. **Settings → Editor → Syntax highlighting** :
   - Choisissez un thème de couleurs

---

## Outils de productivité additionnels

### 1. File templates

Créez des modèles de fichiers pour gagner du temps.

**Pour VS Code**, créez un fichier `template.c` :

```c
/**
 * @file template.c
 * @author Votre Nom
 * @date DATE
 * @brief Description courte
 */

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    // TODO: Implémenter

    return EXIT_SUCCESS;
}
```

Copiez ce template au démarrage de chaque nouveau projet.

### 2. Scripts de compilation

Créez un script `compile.sh` à la racine de vos projets :

```bash
#!/bin/bash

# Script de compilation rapide

# Couleurs
RED='\033[0;31m'  
GREEN='\033[0;32m'  
NC='\033[0m' # No Color  

# Paramètres
SOURCE="$1"  
OUTPUT="${SOURCE%.c}"  
FLAGS="-Wall -Wextra -std=c17 -g"  

if [ -z "$SOURCE" ]; then
    echo "Usage: ./compile.sh fichier.c"
    exit 1
fi

echo "Compilation de $SOURCE..."  
gcc $FLAGS "$SOURCE" -o "$OUTPUT"  

if [ $? -eq 0 ]; then
    echo -e "${GREEN}✓ Compilation réussie${NC}"
    echo "Exécutable: ./$OUTPUT"
else
    echo -e "${RED}✗ Erreur de compilation${NC}"
    exit 1
fi
```

Rendez-le exécutable :
```bash
chmod +x compile.sh
```

Utilisation :
```bash
./compile.sh monprogramme.c
```

### 3. Alias Bash utiles

Ajoutez dans `~/.bashrc` :

```bash
# Alias pour la compilation C
alias gcc-debug='gcc -Wall -Wextra -g -std=c17'  
alias gcc-release='gcc -Wall -Wextra -O2 -std=c17'  
alias gcc-strict='gcc -Wall -Wextra -Werror -pedantic -std=c17'  

# Alias pour Valgrind
alias valgrind-check='valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes'

# Alias pour le nettoyage (limité au répertoire courant, non récursif)
alias clean-exe='find . -maxdepth 1 -type f -executable -not -name "*.sh" -delete'
```

Rechargez :
```bash
source ~/.bashrc
```

---

## Organisation de votre workspace

### Structure de dossiers recommandée

```
~/projets-c/
├── exercices/
│   ├── ex01/
│   ├── ex02/
│   └── ...
├── projets/
│   ├── calculatrice/
│   ├── gestionnaire-fichiers/
│   └── ...
├── templates/
│   ├── template.c
│   ├── template-header.h
│   └── Makefile.template
└── outils/
    ├── compile.sh
    └── valgrind-check.sh
```

### Workspace VS Code

Créez un workspace pour vos projets C :

1. File → Save Workspace As...
2. Nommez-le `projets-c.code-workspace`

Exemple de contenu :

```json
{
    "folders": [
        {
            "path": "exercices"
        },
        {
            "path": "projets"
        }
    ],
    "settings": {
        "C_Cpp.default.cStandard": "c17",
        "editor.formatOnSave": true
    }
}
```

---

## Checklist de configuration finale

Avant de commencer à coder sérieusement, vérifiez que vous avez :

- ✅ **VS Code installé** avec l'extension C/C++
- ✅ **Code Runner** installé et configuré
- ✅ **Paramètres settings.json** personnalisés
- ✅ **Police avec ligatures** installée (Fira Code ou autre)
- ✅ **Thème** choisi et agréable pour vos yeux
- ✅ **Raccourcis clavier** essentiels mémorisés
- ✅ **Terminal intégré** fonctionnel
- ✅ **Formatage automatique** activé
- ✅ **Structure de dossiers** créée
- ✅ **Template de fichier** créé

---

## Dépannage

### L'auto-complétion ne fonctionne pas

**Solution 1 :** Vérifiez que l'extension C/C++ est bien installée et activée.

**Solution 2 :** Rechargez VS Code (`Ctrl+Shift+P` → "Developer: Reload Window").

**Solution 3 :** Créez un fichier `c_cpp_properties.json` :

`Ctrl+Shift+P` → "C/C++: Edit Configurations (JSON)"

```json
{
    "configurations": [
        {
            "name": "Linux",
            "includePath": [
                "${workspaceFolder}/**",
                "/usr/include/**"
            ],
            "defines": [],
            "compilerPath": "/usr/bin/gcc",
            "cStandard": "c17",
            "intelliSenseMode": "linux-gcc-x64"
        }
    ],
    "version": 4
}
```

### Le formatage automatique ne marche pas

Vérifiez que clang-format est installé :
```bash
which clang-format
```

Si non installé :
```bash
sudo apt install clang-format
```

### Code Runner n'exécute pas le programme

Vérifiez la configuration dans `settings.json` et assurez-vous que `"code-runner.runInTerminal"` est `true`.

---

## Récapitulatif

À ce stade, vous avez :

- ✅ Installé et configuré les extensions essentielles de VS Code
- ✅ Personnalisé l'apparence et le comportement de votre éditeur
- ✅ Créé des snippets pour gagner du temps
- ✅ Configuré le formatage automatique
- ✅ Appris les raccourcis clavier essentiels
- ✅ Mis en place une structure de projet
- ✅ Configuré des tâches de build

Votre environnement de développement est maintenant **optimisé pour la productivité**.

**Prochaine étape :** Dans la section 2.5, nous découvrirons les DevContainers, une technologie moderne pour créer des environnements de développement reproductibles.

---

## Points clés à retenir

1. **Commencer simple** : Ne surchargez pas votre éditeur au début
2. **Extensions essentielles** : C/C++ (Microsoft) et Code Runner minimum
3. **Formatage automatique** : Activez `formatOnSave` avec clang-format
4. **Raccourcis** : Apprenez-les progressivement, ils font gagner énormément de temps
5. **Snippets** : Créez vos propres modèles de code fréquemment utilisé
6. **Organisation** : Une structure de dossiers claire dès le début
7. **Personnalisation progressive** : Ajoutez des outils au fur et à mesure des besoins
8. **Sauvegardez votre config** : Exportez vos settings.json pour les réutiliser

**Conseil final :** Prenez le temps de vous familiariser avec votre environnement. Un éditeur bien configuré devient un véritable allié dans votre apprentissage du C.

---


⏭️ [DevContainers : Environnement reproductible moderne](/02-configuration-environnement/05-devcontainers.md)
