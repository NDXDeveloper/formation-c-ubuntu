🔝 Retour au [Sommaire](/SOMMAIRE.md)

# C.1 - .vimrc optimisé pour C

## Table des matières
- [Introduction](#introduction)
- [Qu'est-ce que .vimrc ?](#quest-ce-que-vimrc-)
- [Où se trouve le fichier .vimrc ?](#o%C3%B9-se-trouve-le-fichier-vimrc-)
- [Structure de base](#structure-de-base)
- [Configuration complète pour C](#configuration-compl%C3%A8te-pour-c)
- [Explications détaillées des options](#explications-d%C3%A9taill%C3%A9es-des-options)
- [Plugins recommandés](#plugins-recommand%C3%A9s)
- [Comment appliquer la configuration](#comment-appliquer-la-configuration)
- [Vérification et dépannage](#v%C3%A9rification-et-d%C3%A9pannage)

---

## Introduction

Vim est un éditeur de texte puissant et omniprésent dans les systèmes Linux. Bien configuré, il devient un environnement de développement complet pour la programmation en C. Ce guide vous aidera à créer une configuration `.vimrc` optimale, même si vous débutez avec Vim.

**Pourquoi configurer Vim pour C ?**
- ✅ Coloration syntaxique adaptée au langage C
- ✅ Indentation automatique intelligente
- ✅ Navigation rapide dans le code
- ✅ Intégration avec les outils de compilation (GCC, Make, CMake)
- ✅ Détection automatique des erreurs
- ✅ Productivité accrue

---

## Qu'est-ce que .vimrc ?

Le fichier `.vimrc` (Vim Runtime Configuration) est le fichier de configuration personnel de Vim. Il contient des commandes et des paramètres qui sont exécutés automatiquement à chaque démarrage de Vim.

**Analogie simple :** C'est comme le fichier de préférences de votre navigateur web, mais pour Vim.

**Points clés :**
- C'est un fichier texte simple
- Les lignes commençant par `"` sont des commentaires
- Chaque ligne contient généralement une commande ou un réglage
- Les modifications prennent effet au prochain lancement de Vim (ou avec `:source ~/.vimrc`)

---

## Où se trouve le fichier .vimrc ?

Le fichier `.vimrc` se trouve dans votre répertoire personnel (home directory) :

```bash
~/.vimrc
```

**Chemin complet :**
```bash
/home/votre_nom_utilisateur/.vimrc
```

**Important :** Le point (`.`) devant le nom signifie que c'est un fichier caché. Pour le voir avec `ls`, utilisez :

```bash
ls -la ~ | grep vimrc
```

**Si le fichier n'existe pas :** Pas de panique ! Vous pouvez le créer :

```bash
touch ~/.vimrc
```

Ou directement l'ouvrir avec Vim (il sera créé automatiquement) :

```bash
vim ~/.vimrc
```

---

## Structure de base

Un fichier `.vimrc` typique est organisé en sections logiques. Voici une structure recommandée :

```vim
" ============================================
" Configuration Vim pour la programmation C
" ============================================

" Section 1 : Paramètres généraux
" Section 2 : Interface utilisateur
" Section 3 : Indentation et formatage
" Section 4 : Recherche et navigation
" Section 5 : Spécificités pour le C
" Section 6 : Raccourcis clavier
" Section 7 : Plugins (optionnel)
```

---

## Configuration complète pour C

Voici une configuration `.vimrc` complète et commentée, optimisée pour la programmation en C et accessible aux débutants :

```vim
" ============================================
" Configuration Vim pour la programmation C
" Édition 2025 - Débutants friendly
" ============================================

" -------------------- SECTION 1 : PARAMÈTRES GÉNÉRAUX --------------------

" Désactive la compatibilité avec vi (active les fonctionnalités avancées de Vim)
set nocompatible

" Active la détection automatique du type de fichier
filetype on  
filetype plugin on  
filetype indent on  

" Active la coloration syntaxique
syntax enable  
syntax on  

" Encodage UTF-8 par défaut
set encoding=utf-8  
set fileencoding=utf-8  

" Historique des commandes (nombre de commandes mémorisées)
set history=1000

" Sauvegarde automatique avant certaines actions
set autowrite  
set autoread  

" Désactive les fichiers de sauvegarde (.swp, ~)
set nobackup  
set noswapfile  
set nowritebackup  

" -------------------- SECTION 2 : INTERFACE UTILISATEUR --------------------

" Affiche les numéros de ligne
set number

" Affiche les numéros de ligne relatifs (pratique pour les mouvements)
set relativenumber

" Affiche la position du curseur (ligne, colonne)
set ruler

" Affiche la commande en cours de frappe
set showcmd

" Affiche le mode actuel (INSERT, VISUAL, etc.)
set showmode

" Hauteur de la ligne de commande
set cmdheight=2

" Met en surbrillance la ligne du curseur
set cursorline

" Active le mode souris (utile pour les débutants)
set mouse=a

" Améliore l'affichage du menu d'autocomplétion
set wildmenu  
set wildmode=longest:full,full  

" Ignore certains fichiers dans l'autocomplétion
set wildignore=*.o,*.obj,*.exe,*.dll,*.so,*.a

" Toujours afficher la ligne de statut
set laststatus=2

" Format de la ligne de statut (affiche fichier, ligne, colonne, pourcentage)
set statusline=%F%m%r%h%w\ [FORMAT=%{&ff}]\ [TYPE=%Y]\ [POS=%l,%v][%p%%]\ %{strftime(\"%d/%m/%y\ -\ %H:%M\")}

" Active le repliage de code (folding)
set foldenable  
set foldmethod=syntax  
set foldlevelstart=10  

" Longueur maximale d'affichage pour les lignes longues
set textwidth=80

" Affiche une colonne de référence à 80 caractères
set colorcolumn=80

" -------------------- SECTION 3 : INDENTATION ET FORMATAGE --------------------

" Active l'indentation automatique
set autoindent  
set smartindent  
set cindent  

" Utilise des espaces au lieu des tabulations
set expandtab

" Nombre d'espaces pour une tabulation
set tabstop=4

" Nombre d'espaces pour l'indentation automatique
set shiftwidth=4

" Nombre d'espaces insérés/supprimés avec Tab/Backspace
set softtabstop=4

" Arrondit l'indentation au multiple de shiftwidth
set shiftround

" Paramètres d'indentation C spécifiques
set cinoptions=:0,l1,g0,t0,(0,W4

" Permet le retour à la ligne visuel pour les longues lignes
set wrap  
set linebreak  

" -------------------- SECTION 4 : RECHERCHE ET NAVIGATION --------------------

" Recherche incrémentale (recherche pendant la frappe)
set incsearch

" Met en surbrillance les résultats de recherche
set hlsearch

" Ignore la casse dans les recherches
set ignorecase

" Mais respecte la casse si la recherche contient une majuscule
set smartcase

" Déplacement entre les lignes avec les flèches
set whichwrap+=<,>,h,l,[,]

" Saute à la parenthèse/accolade correspondante lors de la frappe
set showmatch  
set matchtime=2  

" Active le backspace amélioré
set backspace=indent,eol,start

" -------------------- SECTION 5 : SPÉCIFICITÉS POUR LE C --------------------

" Configuration spécifique aux fichiers C
autocmd FileType c,cpp setlocal commentstring=//\ %s  
autocmd FileType c,cpp setlocal tabstop=4 shiftwidth=4 expandtab  

" Active l'autocomplétion pour C
set omnifunc=syntaxcomplete#Complete

" Affiche les types de fichiers .h comme des fichiers C (et non C++)
augroup project
  autocmd!
  autocmd BufRead,BufNewFile *.h,*.c set filetype=c
augroup END

" Compilation rapide avec F5 (compile avec gcc)
autocmd FileType c map <F5> :w<CR>:!gcc -Wall -Wextra -std=c17 % -o %< && ./%<<CR>

" Compilation avec debug (-g) avec F6
autocmd FileType c map <F6> :w<CR>:!gcc -Wall -Wextra -g -std=c17 % -o %< && gdb ./%<<CR>

" Formatage automatique du code C avec F8 (nécessite clang-format)
autocmd FileType c map <F8> :!clang-format -i %<CR><CR>

" Exécution de make avec F9
autocmd FileType c map <F9> :w<CR>:make<CR>

" -------------------- SECTION 6 : RACCOURCIS CLAVIER --------------------

" Leader key (touche préfixe pour les raccourcis personnalisés)
let mapleader = ","

" Sauvegarde rapide avec ,w
nnoremap <leader>w :w<CR>

" Quitter avec ,q
nnoremap <leader>q :q<CR>

" Sauvegarde et quitter avec ,x
nnoremap <leader>x :wq<CR>

" Désactive la surbrillance de recherche avec ,h
nnoremap <leader>h :nohlsearch<CR>

" Navigation entre les buffers
nnoremap <C-n> :bnext<CR>  
nnoremap <C-p> :bprev<CR>  

" Redimensionnement des fenêtres
nnoremap <C-Up> :resize +2<CR>  
nnoremap <C-Down> :resize -2<CR>  
nnoremap <C-Left> :vertical resize -2<CR>  
nnoremap <C-Right> :vertical resize +2<CR>  

" Déplacement de lignes en mode visual
vnoremap J :m '>+1<CR>gv=gv  
vnoremap K :m '<-2<CR>gv=gv  

" Indentation en mode visual (garde la sélection)
vnoremap < <gv  
vnoremap > >gv  

" -------------------- SECTION 7 : COLORATION (OPTIONNEL) --------------------

" Schéma de couleurs (décommentez celui que vous préférez)
" colorscheme desert
" colorscheme slate
" colorscheme molokai
" colorscheme gruvbox

" Active les couleurs 256 pour de meilleurs thèmes
set t_Co=256

" Fond sombre (dark) ou clair (light)
set background=dark

" -------------------- SECTION 8 : PARAMÈTRES AVANCÉS (OPTIONNEL) --------------------

" Affiche les caractères invisibles (espaces, tabs, etc.)
" set list
" set listchars=tab:→\ ,space:·,nbsp:␣,trail:•,eol:¶,precedes:«,extends:»

" Active le spell checking pour les commentaires (en anglais)
" autocmd FileType c,cpp setlocal spell spelllang=en_us

" Complétion automatique des parenthèses/accolades
inoremap ( ()<Left>  
inoremap [ []<Left>  
inoremap { {}<Left>  
inoremap ' ''<Left>  
inoremap " ""<Left>  

" Mais uniquement si pas déjà fermé
inoremap <expr> ) getline('.')[col('.')-1] == ')' ? "\<Right>" : ')'  
inoremap <expr> ] getline('.')[col('.')-1] == ']' ? "\<Right>" : ']'  
inoremap <expr> } getline('.')[col('.')-1] == '}' ? "\<Right>" : '}'  

" -------------------- FIN DE LA CONFIGURATION --------------------
```

---

## Explications détaillées des options

### **Paramètres généraux**

| Option | Description | Exemple |
|--------|-------------|---------|
| `set nocompatible` | Désactive le mode compatible Vi (active les fonctionnalités modernes de Vim) | Obligatoire pour profiter de Vim |
| `filetype on` | Active la détection automatique du type de fichier | Vim reconnaît automatiquement les fichiers `.c` |
| `syntax enable` | Active la coloration syntaxique | Les mots-clés C sont colorés |
| `set encoding=utf-8` | Utilise l'encodage UTF-8 | Support des caractères accentués |

### **Interface utilisateur**

| Option | Description | Recommandation |
|--------|-------------|----------------|
| `set number` | Affiche les numéros de ligne | ✅ Essentiel |
| `set relativenumber` | Numéros relatifs à la position du curseur | ⚠️ Optionnel (utile pour les mouvements Vim) |
| `set cursorline` | Met en surbrillance la ligne courante | ✅ Aide à la lisibilité |
| `set mouse=a` | Active la souris | ✅ Utile pour les débutants |
| `set colorcolumn=80` | Marque visuelle à 80 colonnes | ✅ Respect des conventions C |

### **Indentation**

| Option | Description | Valeur pour C |
|--------|-------------|---------------|
| `set expandtab` | Convertit les tabs en espaces | ✅ Recommandé |
| `set tabstop=4` | Largeur d'affichage d'une tabulation | 4 espaces (standard) |
| `set shiftwidth=4` | Largeur de l'indentation automatique | 4 espaces |
| `set cindent` | Indentation automatique pour C | ✅ Obligatoire |
| `set cinoptions=...` | Options d'indentation fines pour C | Personnalisable |

### **Recherche**

| Option | Description | Exemple |
|--------|-------------|---------|
| `set incsearch` | Recherche pendant la frappe | Trouve "printf" en tapant "prin" |
| `set hlsearch` | Surligne les résultats de recherche | Tous les "int" sont surlignés |
| `set ignorecase` | Ignore la casse | `/function` trouve "Function" |
| `set smartcase` | Respecte la casse si majuscule | `/Function` ne trouve que "Function" |

### **Raccourcis clavier utiles**

**Avec la touche Leader (`,`)** :

| Raccourci | Action | Commande équivalente |
|-----------|--------|---------------------|
| `,w` | Sauvegarder | `:w<CR>` |
| `,q` | Quitter | `:q<CR>` |
| `,x` | Sauvegarder et quitter | `:wq<CR>` |
| `,h` | Désactive la surbrillance de recherche | `:nohlsearch<CR>` |

**Touches de fonction** :

| Touche | Action | Détails |
|--------|--------|---------|
| `F5` | Compiler et exécuter | `gcc -Wall -Wextra -std=c17 fichier.c` |
| `F6` | Compiler avec debug | Lance GDB après compilation |
| `F8` | Formater le code | Utilise `clang-format` |
| `F9` | Lancer Make | Exécute le Makefile |

### **Options spécifiques C**

```vim
" Forcer l'utilisation de // pour les commentaires
autocmd FileType c,cpp setlocal commentstring=//\ %s

" Traiter les .h comme des fichiers C (pas C++)
autocmd BufRead,BufNewFile *.h,*.c set filetype=c

" Autocomplétion pour C
set omnifunc=syntaxcomplete#Complete
```

**Explication :** Ces options garantissent que Vim reconnaît correctement les fichiers C et applique les bons paramètres d'indentation et de formatage.

---

## Plugins recommandés

Pour aller plus loin, vous pouvez installer des plugins Vim. Voici les plus utiles pour C :

### **Gestionnaire de plugins : vim-plug**

Installez d'abord `vim-plug` :

```bash
curl -fLo ~/.vim/autoload/plug.vim --create-dirs \
    https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim
```

Ajoutez ensuite cette section dans votre `.vimrc` :

```vim
" -------------------- SECTION PLUGINS --------------------

call plug#begin('~/.vim/plugged')

" Autocomplétion intelligente
Plug 'ycm-core/YouCompleteMe'

" Explorateur de fichiers
Plug 'preservim/nerdtree'

" Barre de statut améliorée
Plug 'vim-airline/vim-airline'  
Plug 'vim-airline/vim-airline-themes'  

" Intégration Git
Plug 'tpope/vim-fugitive'

" Recherche de fichiers rapide
Plug 'junegunn/fzf', { 'do': { -> fzf#install() } }  
Plug 'junegunn/fzf.vim'  

" Commentaires automatiques
Plug 'tpope/vim-commentary'

" Support CMake
Plug 'cdelledonne/vim-cmake'

" Linting et analyse statique
Plug 'dense-analysis/ale'

" Thème de couleurs moderne
Plug 'morhetz/gruvbox'

call plug#end()
```

**Installation des plugins :**

1. Ouvrez Vim : `vim`
2. Tapez : `:PlugInstall`
3. Attendez que les plugins se téléchargent
4. Quittez et relancez Vim

### **Plugins essentiels expliqués**

**NERDTree** : Arbre de fichiers latéral (comme dans VSCode)
```vim
" Ouvrir NERDTree avec F2
nnoremap <F2> :NERDTreeToggle<CR>
```

**YouCompleteMe** : Autocomplétion intelligente (suggestions en temps réel)
- Nécessite Python et compilation
- Alternatives plus légères : `coc.nvim`, `vim-lsp`

**ALE** : Analyse statique en temps réel
```vim
" Configuration ALE pour C
let g:ale_linters = {'c': ['gcc', 'cppcheck']}  
let g:ale_fixers = {'c': ['clang-format']}  
```

---

## Comment appliquer la configuration

### **Méthode 1 : Copier-coller**

1. Ouvrez le fichier `.vimrc` :
   ```bash
   vim ~/.vimrc
   ```

2. Passez en mode insertion avec la touche `i`

3. Collez la configuration (clic droit → Coller ou `Ctrl+Shift+V`)

4. Sauvegardez et quittez :
   - Appuyez sur `Echap` (ESC)
   - Tapez `:wq` puis `Entrée`

### **Méthode 2 : Télécharger un fichier prêt**

Si vous avez un fichier `.vimrc` déjà préparé :

```bash
# Sauvegarde de l'ancien fichier (si existant)
mv ~/.vimrc ~/.vimrc.backup

# Copie du nouveau fichier
cp /chemin/vers/nouveau/.vimrc ~/
```

### **Méthode 3 : Créer avec echo**

```bash
cat > ~/.vimrc << 'EOF'
" Votre configuration ici
EOF
```

### **Recharger la configuration sans redémarrer Vim**

Si Vim est déjà ouvert :

```vim
:source ~/.vimrc
```

Ou utilisez le raccourci (à ajouter dans votre `.vimrc`) :

```vim
" Recharger .vimrc avec ,r
nnoremap <leader>r :source ~/.vimrc<CR>
```

---

## Vérification et dépannage

### **Vérifier que la configuration est chargée**

1. Ouvrez Vim :
   ```bash
   vim
   ```

2. Vérifiez une option spécifique :
   ```vim
   :set number?
   ```
   Devrait afficher : `number` (si activé)

3. Vérifiez l'encodage :
   ```vim
   :set encoding?
   ```
   Devrait afficher : `encoding=utf-8`

### **Problèmes courants**

#### **Erreur : "Unknown option"**

**Cause :** Une option n'existe pas dans votre version de Vim.

**Solution :**
```bash
# Vérifiez la version de Vim
vim --version | head -n 1

# Mettez à jour Vim si nécessaire
sudo apt update && sudo apt install vim
```

#### **Les couleurs ne s'affichent pas correctement**

**Solution :**
```vim
" Forcez les couleurs 256
set t_Co=256  
set termguicolors  
```

#### **Les raccourcis F5/F6 ne fonctionnent pas**

**Cause :** Votre terminal capture ces touches.

**Solution :** Utilisez des raccourcis alternatifs :
```vim
" Compilation avec ,c
nnoremap <leader>c :w<CR>:!gcc -Wall -Wextra -std=c17 % -o %<<CR>

" Exécution avec ,e
nnoremap <leader>e :!./%<<CR>
```

#### **Vim est lent au démarrage**

**Cause :** Trop de plugins ou syntaxe trop complexe.

**Solution :**
```bash
# Profiler le démarrage de Vim
vim --startuptime vim_startup.log

# Examinez le fichier généré
cat vim_startup.log | tail -20
```

#### **Les accolades ne se ferment pas automatiquement**

**Cause :** Conflit avec un plugin ou terminal.

**Solution :** Testez manuellement :
```vim
:inoremap { {}<Left>
```

---

## Ressources complémentaires

### **Documentation officielle**

- Manuel Vim : `:help` dans Vim
- Options : `:help options`
- Filetype C : `:help ft-c-syntax`

### **Tutoriels interactifs**

```bash
# Lancez le tutoriel Vim intégré (30 min)
vimtutor
```

### **Commandes Vim essentielles à connaître**

| Mode | Commande | Action |
|------|----------|--------|
| Normal | `i` | Passer en mode insertion |
| Normal | `v` | Mode visuel (sélection) |
| Normal | `yy` | Copier la ligne |
| Normal | `dd` | Couper la ligne |
| Normal | `p` | Coller |
| Normal | `u` | Annuler (undo) |
| Normal | `Ctrl+r` | Refaire (redo) |
| Normal | `/motif` | Rechercher "motif" |
| Normal | `n` | Résultat suivant |
| Insertion | `Echap` | Retour au mode normal |
| Commande | `:w` | Sauvegarder |
| Commande | `:q` | Quitter |
| Commande | `:wq` | Sauvegarder et quitter |
| Commande | `:q!` | Quitter sans sauvegarder |

---

## Récapitulatif

**Ce qu'il faut retenir :**

1. ✅ Le fichier `.vimrc` se trouve dans `~/.vimrc`
2. ✅ Les lignes commençant par `"` sont des commentaires
3. ✅ Les paramètres essentiels pour C : `cindent`, `expandtab`, `syntax on`
4. ✅ Rechargez avec `:source ~/.vimrc` après modification
5. ✅ Les raccourcis F5-F9 facilitent la compilation
6. ✅ Les plugins sont optionnels mais puissants

**Prochaines étapes :**

- Pratiquez avec `vimtutor` (30 minutes)
- Ouvrez un fichier `.c` et testez la coloration syntaxique
- Essayez les raccourcis `F5` (compilation) et `,w` (sauvegarde)
- Explorez les plugins avec `vim-plug`

---

**🎯 Avec cette configuration, Vim devient un IDE C fonctionnel, léger et personnalisable !**

---

## Notes finales

**Philosophie Vim :**
> "Vim n'est pas un éditeur pour taper du code rapidement, c'est un éditeur pour éditer du code efficacement."

Au début, Vim peut sembler déroutant. Mais après quelques semaines de pratique, vous ne pourrez plus vous en passer. Les commandes deviennent des réflexes, et votre productivité explose.

**Conseil d'or pour les débutants :**
- ✅ Commencez avec les bases (`i`, `Echap`, `:wq`)
- ✅ Ajoutez progressivement des raccourcis
- ✅ Ne personnalisez que ce que vous comprenez
- ✅ Utilisez `:help` pour apprendre

Bon courage et bon code en C ! 🚀

⏭️ [VS Code settings.json et extensions](/annexes/configuration-environnement/02-vscode-settings.md)
