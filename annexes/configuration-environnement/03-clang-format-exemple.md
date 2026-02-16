🔝 Retour au [Sommaire](/SOMMAIRE.md)

# C.3 - .clang-format exemple

## Table des matières
- [Introduction](#introduction)
- [Qu'est-ce que clang-format ?](#quest-ce-que-clang-format-)
- [Pourquoi utiliser clang-format ?](#pourquoi-utiliser-clang-format-)
- [Installation](#installation)
- [Le fichier .clang-format](#le-fichier-clang-format)
- [Styles prédéfinis](#styles-pr%C3%A9d%C3%A9finis)
- [Configuration complète commentée](#configuration-compl%C3%A8te-comment%C3%A9e)
- [Options détaillées par catégorie](#options-d%C3%A9taill%C3%A9es-par-cat%C3%A9gorie)
- [Exemples avant/après](#exemples-avantapr%C3%A8s)
- [Configurations par style](#configurations-par-style)
- [Utilisation de clang-format](#utilisation-de-clang-format)
- [Intégration dans les éditeurs](#int%C3%A9gration-dans-les-%C3%A9diteurs)
- [Intégration dans Git](#int%C3%A9gration-dans-git)
- [Intégration dans le CI/CD](#int%C3%A9gration-dans-le-cicd)
- [Cas d'usage avancés](#cas-dusage-avanc%C3%A9s)
- [Dépannage](#d%C3%A9pannage)

---

## Introduction

Le formatage du code est un aspect crucial de la programmation professionnelle. Un code bien formaté est plus lisible, plus maintenable et facilite la collaboration en équipe. `clang-format` est l'outil standard pour formater automatiquement du code C (et C++) selon des règles prédéfinies.

**Ce que vous allez apprendre :**
- Installer et utiliser clang-format
- Créer un fichier `.clang-format` personnalisé
- Comprendre les options de formatage
- Intégrer clang-format dans votre workflow

---

## Qu'est-ce que clang-format ?

### **Définition**

`clang-format` est un outil de formatage automatique de code développé par le projet LLVM. Il reformate votre code C/C++ selon un style cohérent et personnalisable.

**Analogie simple :** C'est comme la fonction "Embellir" (Beautify) d'un traitement de texte, mais pour du code. Il réorganise automatiquement votre code pour le rendre propre et cohérent.

### **Caractéristiques principales**

- ✅ **Formatage automatique** : Réorganise les espaces, indentations, retours à la ligne
- ✅ **Configurable** : Centaines d'options personnalisables
- ✅ **Styles prédéfinis** : LLVM, Google, Chromium, Mozilla, WebKit, Microsoft, GNU
- ✅ **Reproductible** : Même config = même résultat partout
- ✅ **Rapide** : Formate instantanément des fichiers entiers
- ✅ **Intégrable** : Fonctionne avec tous les éditeurs et CI/CD

### **Ce que clang-format ne fait PAS**

- ❌ Ne corrige pas les bugs
- ❌ Ne modifie pas la logique du code
- ❌ Ne renomme pas les variables
- ❌ Ne réorganise pas les fonctions

**Important :** clang-format ne touche qu'au **formatage visuel**, pas au comportement du code.

---

## Pourquoi utiliser clang-format ?

### **Avantages individuels**

- 🎯 **Gain de temps** : Plus besoin de formater manuellement
- 🎯 **Cohérence** : Style uniforme dans tout le projet
- 🎯 **Lisibilité** : Code plus facile à lire et comprendre
- 🎯 **Focus** : Concentrez-vous sur la logique, pas le formatage

### **Avantages en équipe**

- 👥 **Code reviews plus simples** : Pas de débats sur les espaces
- 👥 **Onboarding facilité** : Les nouveaux suivent automatiquement le style
- 👥 **Réduction des conflits Git** : Moins de diffs liés au formatage
- 👥 **Standards respectés** : Conformité aux conventions du projet

### **Exemple concret**

**Sans clang-format :**
```c
// Développeur A aime cette indentation :
if(x>0){  
printf("positif");  
}

// Développeur B préfère celle-là :
if (x > 0)
{
    printf("positif");
}
```

**Avec clang-format :**
```c
// Tout le monde obtient le même résultat :
if (x > 0) {
    printf("positif");
}
```

---

## Installation

### **Sur Ubuntu/Debian**

```bash
# Installation depuis les dépôts officiels
sudo apt update  
sudo apt install clang-format  

# Vérifier l'installation
clang-format --version
```

**Résultat attendu :**
```
Ubuntu clang-format version 14.0.0-1ubuntu1
```

### **Installer une version spécifique**

```bash
# Pour installer clang-format-15
sudo apt install clang-format-15

# Créer un alias
sudo update-alternatives --install /usr/bin/clang-format clang-format /usr/bin/clang-format-15 100
```

### **Depuis le site officiel (version récente)**

```bash
# Télécharger depuis GitHub releases
wget https://github.com/llvm/llvm-project/releases/download/llvmorg-17.0.6/clang+llvm-17.0.6-x86_64-linux-gnu-ubuntu-22.04.tar.xz

# Extraire
tar -xf clang+llvm-17.0.6-x86_64-linux-gnu-ubuntu-22.04.tar.xz

# Copier clang-format
sudo cp clang+llvm-17.0.6-x86_64-linux-gnu-ubuntu-22.04/bin/clang-format /usr/local/bin/

# Vérifier
clang-format --version
```

### **Vérifier l'installation**

```bash
# Afficher l'emplacement
which clang-format

# Afficher la version
clang-format --version

# Tester le formatage
echo "int main(){return 0;}" | clang-format
```

**Résultat attendu :**
```c
int main() { return 0; }
```

---

## Le fichier .clang-format

### **Qu'est-ce que c'est ?**

Le fichier `.clang-format` est un fichier de configuration qui définit toutes les règles de formatage pour votre projet. Il est placé à la racine du projet et s'applique automatiquement à tous les fichiers C/C++.

**Emplacement :**
```bash
mon_projet/
├── .clang-format          ← Le fichier de config
├── src/
│   ├── main.c
│   └── utils.c
└── include/
    └── utils.h
```

### **Format du fichier**

Le fichier `.clang-format` utilise le format **YAML** (Yet Another Markup Language).

**Structure de base :**
```yaml
---
# Commentaire
CleDeLOption: Valeur  
AutreOption: AutreValeur  
...
```

**Règles YAML importantes :**
- Les espaces comptent (pas de tabulations)
- Les `:` doivent être suivis d'un espace
- Sensible à la casse

### **Comment clang-format trouve le fichier ?**

Quand vous formatez un fichier, clang-format cherche `.clang-format` :

1. Dans le même dossier que le fichier
2. Dans le dossier parent
3. Dans le dossier parent du parent
4. Et ainsi de suite jusqu'à la racine

**Exemple :**
```
Formatage de : /projet/src/module/fichier.c

Recherche de .clang-format dans :
1. /projet/src/module/.clang-format
2. /projet/src/.clang-format
3. /projet/.clang-format          ← Trouvé ici !
4. /.clang-format
```

### **Créer un fichier .clang-format**

**Méthode 1 : Depuis un style prédéfini**

```bash
# Générer un fichier basé sur le style LLVM
clang-format -style=llvm -dump-config > .clang-format

# Ou Google style
clang-format -style=google -dump-config > .clang-format
```

**Méthode 2 : Créer manuellement**

```bash
touch .clang-format  
nano .clang-format  
# Copiez-collez votre configuration
```

**Méthode 3 : Copier un template**

```bash
# Télécharger un template depuis GitHub
wget https://raw.githubusercontent.com/torvalds/linux/master/.clang-format
```

---

## Styles prédéfinis

clang-format propose plusieurs styles de base inspirés de projets célèbres.

### **Comparaison des styles**

| Style | Origine | Indentation | Accolades | Espaces |
|-------|---------|-------------|-----------|---------|
| **LLVM** | Projet LLVM/Clang | 2 espaces | Same line | Compacts |
| **Google** | Google C++ Style Guide | 2 espaces | Same line | Nombreux |
| **Chromium** | Projet Chromium | 2 espaces | Same line | Compacts |
| **Mozilla** | Mozilla Firefox | 2 espaces | Same line | Équilibrés |
| **WebKit** | WebKit (Safari) | 4 espaces | Same line | Équilibrés |
| **Microsoft** | Microsoft | 4 espaces | Same line | Équilibrés |
| **GNU** | Projet GNU | 2 espaces | New line | Très espacés |
| **Linux** | Kernel Linux | 8 tabs | Same line | Minimal |

### **Aperçu de chaque style**

#### **Style LLVM (Recommandé pour débuter)**

```c
int main(int argc, char *argv[]) {
  if (argc > 1) {
    printf("Hello %s\n", argv[1]);
  }
  return 0;
}
```

#### **Style Google**

```c
int main(int argc, char* argv[]) {
  if (argc > 1) {
    printf("Hello %s\n", argv[1]);
  }
  return 0;
}
```

#### **Style GNU**

```c
int  
main (int argc, char *argv[])  
{
  if (argc > 1)
    {
      printf ("Hello %s\n", argv[1]);
    }
  return 0;
}
```

#### **Style Linux (Kernel)**

```c
int main(int argc, char *argv[])
{
        if (argc > 1) {
                printf("Hello %s\n", argv[1]);
        }
        return 0;
}
```

### **Utiliser un style prédéfini**

**Option 1 : Dans le fichier .clang-format**

```yaml
---
BasedOnStyle: LLVM
...
```

**Option 2 : En ligne de commande**

```bash
clang-format -style=llvm fichier.c
```

**Option 3 : Personnaliser à partir d'un style**

```yaml
---
BasedOnStyle: LLVM  
IndentWidth: 4          # Modifier l'indentation à 4  
ColumnLimit: 100        # Modifier la limite à 100 colonnes  
...
```

---

## Configuration complète commentée

Voici un fichier `.clang-format` complet avec toutes les options importantes commentées :

```yaml
---
# ============================================
# Configuration clang-format pour projets C
# Standard recommandé 2025
# ============================================

# Style de base (personnalisable ensuite)
BasedOnStyle: LLVM

# -------------------- LANGUE --------------------

# Langage : C (clang-format utilise Cpp pour C et C++)
Language: Cpp  
Standard: Latest

# -------------------- INDENTATION --------------------

# Nombre d'espaces pour l'indentation
IndentWidth: 4

# Utiliser des espaces (pas des tabulations)
UseTab: Never

# Indentation des étiquettes de case
IndentCaseLabels: false

# Indentation des labels (goto)
IndentGotoLabels: true

# Indentation des directives du préprocesseur
IndentPPDirectives: AfterHash

# Indentation des fonctions imbriquées (GNU style)
IndentExternBlock: NoIndent

# Indentation du contenu des namespaces (C++ seulement, mais présent)
NamespaceIndentation: None

# -------------------- ACCOLADES --------------------

# Position des accolades
BreakBeforeBraces: Linux
# Options disponibles :
#   - Attach (K&R) : if (x) {
#   - Linux : if (x) { ... } else {
#   - Allman : if (x)\n{\n
#   - GNU : if (x)\n  {\n
#   - WebKit : if (x) {
#   - Custom : configuration détaillée

# Configuration personnalisée des accolades (si BreakBeforeBraces: Custom)
# BraceWrapping:
#   AfterFunction: true
#   AfterControlStatement: false
#   AfterStruct: true
#   AfterEnum: true

# -------------------- ESPACES --------------------

# Espaces autour des opérateurs
SpaceBeforeAssignmentOperators: true

# Espaces autour des parenthèses
SpaceBeforeParens: ControlStatements
# Options :
#   - Never : if(x)
#   - ControlStatements : if (x) mais fonction()
#   - Always : if (x) et fonction ()

# Espaces dans les parenthèses vides
SpaceInEmptyParentheses: false

# Espaces dans les parenthèses
SpacesInParentheses: false

# Espaces dans les crochets
SpacesInSquareBrackets: false

# Espaces dans les commentaires
SpacesBeforeTrailingComments: 2

# Espaces après les cast
SpaceAfterCStyleCast: false

# Espaces après le mot-clé (sizeof, typeof, etc.)
SpaceAfterLogicalNot: false

# Espaces avant les accolades
SpaceBeforeRangeBasedForLoopColon: true

# -------------------- ALIGNEMENT --------------------

# Aligner les déclarations consécutives
AlignConsecutiveDeclarations: false

# Aligner les assignations consécutives
AlignConsecutiveAssignments: false

# Aligner les macros
AlignConsecutiveMacros: true

# Aligner les backslashes des macros multilignes
AlignEscapedNewlines: Right

# Aligner les opérandes
AlignOperands: true

# Aligner les commentaires de fin de ligne
AlignTrailingComments: true

# Aligner les paramètres de fonction sur plusieurs lignes
AlignAfterOpenBracket: Align

# -------------------- RETOURS À LA LIGNE --------------------

# Limite de colonnes (longueur maximale d'une ligne)
ColumnLimit: 80

# Permettre les courtes fonctions sur une ligne
AllowShortFunctionsOnASingleLine: Empty
# Options :
#   - None : Jamais
#   - Empty : Seulement si vide
#   - Inline : Fonctions inline
#   - All : Toutes

# Permettre les courts if sur une ligne
AllowShortIfStatementsOnASingleLine: Never

# Permettre les courts blocs sur une ligne
AllowShortBlocksOnASingleLine: Never

# Permettre les courtes boucles sur une ligne
AllowShortLoopsOnASingleLine: false

# Permettre les courts case sur une ligne
AllowShortCaseLabelsOnASingleLine: false

# Casser avant les opérateurs binaires
BreakBeforeBinaryOperators: None

# Casser avant les opérateurs ternaires
BreakBeforeTernaryOperators: true

# Casser après le type de retour
AlwaysBreakAfterReturnType: None
# AlwaysBreakAfterDefinitionReturnType: None

# Casser les chaînes de caractères longues
BreakStringLiterals: true

# -------------------- POINTEURS ET RÉFÉRENCES --------------------

# Position de * et & dans les déclarations
PointerAlignment: Right
# Options :
#   - Left : int* ptr
#   - Right : int *ptr
#   - Middle : int * ptr

# Dériver l'alignement des pointeurs (override PointerAlignment)
DerivePointerAlignment: false

# -------------------- COMMENTAIRES --------------------

# Reformater les commentaires
ReflowComments: true

# -------------------- INCLUDES --------------------

# Trier les includes
SortIncludes: true

# Catégories d'includes (pour le tri)
IncludeCategories:
  - Regex: '^".*"'
    Priority: 1
  - Regex: '^<.*\.h>'
    Priority: 2
  - Regex: '^<.*>'
    Priority: 3

# -------------------- PRÉPROCESSEUR --------------------

# Indenter les directives du préprocesseur
IndentPPDirectives: AfterHash

# -------------------- AUTRES --------------------

# Ajouter une ligne vide à la fin du fichier
InsertTrailingCommas: None

# Pénalité pour casser avant une assignation
PenaltyBreakAssignment: 2

# Pénalité pour casser une chaîne de caractères
PenaltyBreakString: 1000

# Pénalité pour dépasser ColumnLimit
PenaltyExcessCharacter: 1000000

# Pénalité pour retour à la ligne dans un commentaire
PenaltyBreakComment: 300

# Conserver les lignes vides
MaxEmptyLinesToKeep: 1

# Garder les espaces avant les commentaires de bloc
SpacesInContainerLiterals: false

# Compacter les petites fonctions/boucles
KeepEmptyLinesAtTheStartOfBlocks: false

# -------------------- STYLE SPÉCIFIQUE C --------------------

# Ne pas casser après le type de retour des fonctions
AlwaysBreakAfterReturnType: None

# Indentation des wraps de fonction
ContinuationIndentWidth: 4

# Construire les paramètres sur plusieurs lignes
BinPackParameters: false  
BinPackArguments: false  

# Toujours casser après la virgule des paramètres
AllowAllParametersOfDeclarationOnNextLine: false  
AllowAllArgumentsOnNextLine: false  

...
```

---

## Options détaillées par catégorie

### **1. Indentation**

#### **IndentWidth**

```yaml
IndentWidth: 4
```

**Description :** Nombre d'espaces pour chaque niveau d'indentation.

**Exemples :**

```c
// IndentWidth: 2
if (condition) {
  printf("Hello");
}

// IndentWidth: 4
if (condition) {
    printf("Hello");
}

// IndentWidth: 8
if (condition) {
        printf("Hello");
}
```

**Recommandation :**
- **4 espaces** : Standard pour C moderne
- **2 espaces** : Google, LLVM
- **8 espaces** : Linux Kernel (mais avec tabs)

#### **UseTab**

```yaml
UseTab: Never
```

**Options :**
- `Never` : Toujours des espaces (recommandé)
- `Always` : Toujours des tabs
- `ForIndentation` : Tabs pour indentation, espaces pour alignement
- `ForContinuationAndIndentation` : Tabs partout sauf alignement

**Pourquoi "Never" ?**
- Les espaces s'affichent pareil partout
- Les tabs peuvent être 2, 4 ou 8 espaces selon l'éditeur
- Meilleure portabilité

#### **IndentCaseLabels**

```yaml
IndentCaseLabels: false
```

**Effet :**

```c
// IndentCaseLabels: true
switch (x) {
    case 1:
        printf("One");
        break;
    case 2:
        printf("Two");
        break;
}

// IndentCaseLabels: false
switch (x) {  
case 1:  
    printf("One");
    break;
case 2:
    printf("Two");
    break;
}
```

### **2. Accolades**

#### **BreakBeforeBraces**

```yaml
BreakBeforeBraces: Linux
```

**Comparaison des styles :**

```c
// Attach (K&R)
if (x) {
    // ...
} else {
    // ...
}

// Linux
if (x) {
    // ...
} else {
    // ...
}
// Mais pour les fonctions :
void fonction()
{
    // ...
}

// Allman
if (x)
{
    // ...
}
else
{
    // ...
}

// GNU
if (x)
  {
    // ...
  }
```

**Recommandation :** `Linux` ou `Attach` pour la plupart des projets C modernes.

### **3. Espaces**

#### **SpaceBeforeParens**

```yaml
SpaceBeforeParens: ControlStatements
```

**Options :**

```c
// Never
if(condition)  
fonction(arg)  

// ControlStatements (recommandé)
if (condition)  
fonction(arg)  

// Always
if (condition)  
fonction (arg)  
```

#### **PointerAlignment**

```yaml
PointerAlignment: Right
```

**Exemples :**

```c
// Left
int* ptr;  
char* str;  

// Right (recommandé pour C)
int *ptr;  
char *str;  

// Middle
int * ptr;  
char * str;  
```

**Débat classique :**
- **Left** : `int*` est un type à part entière
- **Right** : `*ptr` montre que c'est un pointeur

**Convention C standard :** `Right` (K&R)

### **4. Longueur de ligne**

#### **ColumnLimit**

```yaml
ColumnLimit: 80
```

**Valeurs courantes :**
- **80** : Standard historique (terminaux 80 colonnes)
- **100** : Moderne, bon compromis
- **120** : Larges écrans modernes
- **0** : Pas de limite (déconseillé)

**Exemple :**

```c
// ColumnLimit: 80
void fonction_avec_beaucoup_de_parametres(
    int param1, int param2, int param3,
    int param4);

// ColumnLimit: 120
void fonction_avec_beaucoup_de_parametres(int param1, int param2, int param3, int param4);
```

### **5. Alignement**

#### **AlignConsecutiveAssignments**

```yaml
AlignConsecutiveAssignments: true
```

**Effet :**

```c
// false
int x = 1;  
int longer_name = 2;  
int y = 3;  

// true
int x            = 1;  
int longer_name  = 2;  
int y            = 3;  
```

**Note :** Peut créer des diffs Git importants lors de modifications.

#### **AlignConsecutiveMacros**

```yaml
AlignConsecutiveMacros: true
```

**Effet :**

```c
// true
#define SHORT       10
#define LONGER      20
#define VERY_LONG   30
```

### **6. Retours à la ligne**

#### **AllowShortFunctionsOnASingleLine**

```yaml
AllowShortFunctionsOnASingleLine: Empty
```

**Options :**

```c
// None : jamais
int get_x()
{
    return x;
}

// Empty : seulement si vide
void fonction() {}

// All : même avec du contenu
int get_x() { return x; }
```

---

## Exemples avant/après

### **Exemple 1 : Code désordonné**

**Avant clang-format :**

```c
#include <stdio.h>
#include<stdlib.h>
#include <string.h>

int main(int argc,char*argv[]){  
if(argc<2){  
fprintf(stderr,"Usage: %s <name>\n",argv[0]);  
return 1;}  
char*name=argv[1];  
int len=strlen(name);  
printf("Hello, %s!\n",name);  
printf("Your name has %d characters.\n",len);  
return 0;  
}
```

**Après clang-format (style LLVM, IndentWidth: 4, ColumnLimit: 80) :**

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <name>\n", argv[0]);
        return 1;
    }
    char *name = argv[1];
    int len = strlen(name);
    printf("Hello, %s!\n", name);
    printf("Your name has %d characters.\n", len);
    return 0;
}
```

### **Exemple 2 : Fonction complexe**

**Avant :**

```c
int calculate_something(int a,int b,int c,int d,int e){  
if(a>b&&c<d){return a+c;}else if(a<b&&c>d){return b+d;}else{return e;}}  
```

**Après :**

```c
int calculate_something(int a, int b, int c, int d, int e)
{
    if (a > b && c < d) {
        return a + c;
    } else if (a < b && c > d) {
        return b + d;
    } else {
        return e;
    }
}
```

### **Exemple 3 : Structures et tableaux**

**Avant :**

```c
struct Person{char name[50];int age;float salary;};  
int numbers[]={1,2,3,4,5,6,7,8,9,10};  
```

**Après :**

```c
struct Person {
    char name[50];
    int age;
    float salary;
};

int numbers[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
```

### **Exemple 4 : Macros**

**Avant :**

```c
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define SQUARE(x) ((x)*(x))
```

**Après (avec AlignConsecutiveMacros: true) :**

```c
#define MAX(a, b)    ((a) > (b) ? (a) : (b))
#define MIN(a, b)    ((a) < (b) ? (a) : (b))
#define SQUARE(x)    ((x) * (x))
```

---

## Configurations par style

### **Configuration "Projet Open Source Moderne"**

```yaml
---
BasedOnStyle: LLVM  
IndentWidth: 4  
ColumnLimit: 100  
PointerAlignment: Right  
BreakBeforeBraces: Linux  
AllowShortFunctionsOnASingleLine: Empty  
AlignConsecutiveMacros: true  
SpaceBeforeParens: ControlStatements  
...
```

### **Configuration "Style Google Adapté"**

```yaml
---
BasedOnStyle: Google  
IndentWidth: 2  
ColumnLimit: 80  
PointerAlignment: Left  
AllowShortFunctionsOnASingleLine: Inline  
...
```

### **Configuration "Style Linux Kernel"**

```yaml
---
BasedOnStyle: LLVM  
IndentWidth: 8  
UseTab: Always  
BreakBeforeBraces: Linux  
ColumnLimit: 80  
PointerAlignment: Right  
IndentCaseLabels: false  
AllowShortIfStatementsOnASingleLine: Never  
AllowShortLoopsOnASingleLine: false  
...
```

### **Configuration "Style GNU"**

```yaml
---
BasedOnStyle: GNU  
IndentWidth: 2  
ColumnLimit: 79  
BreakBeforeBraces: GNU  
AlwaysBreakAfterReturnType: All  
SpaceBeforeParens: Always  
...
```

### **Configuration "Embedded/Système"**

```yaml
---
BasedOnStyle: LLVM  
IndentWidth: 4  
UseTab: Never  
ColumnLimit: 120  
PointerAlignment: Right  
BreakBeforeBraces: Attach  
AllowShortFunctionsOnASingleLine: None  
AlignConsecutiveDeclarations: true  
AlignConsecutiveAssignments: true  
...
```

---

## Utilisation de clang-format

### **Ligne de commande**

#### **Formater un fichier (affiche le résultat)**

```bash
clang-format fichier.c
```

#### **Formater et écraser le fichier**

```bash
clang-format -i fichier.c
```

#### **Formater plusieurs fichiers**

```bash
clang-format -i src/*.c include/*.h
```

#### **Formater récursivement tous les fichiers C**

```bash
find . -name "*.c" -o -name "*.h" | xargs clang-format -i
```

#### **Formater avec un style spécifique**

```bash
clang-format -style=google fichier.c
```

#### **Formater avec un fichier de config personnalisé**

```bash
clang-format -style=file:/chemin/vers/.clang-format fichier.c
```

#### **Afficher les modifications sans les appliquer (dry-run)**

```bash
clang-format --dry-run --Werror fichier.c
```

#### **Formater seulement une partie du fichier**

```bash
# Lignes 10 à 20
clang-format -lines=10:20 fichier.c
```

### **Script de formatage complet**

Créez un script `format-all.sh` :

```bash
#!/bin/bash
# Script de formatage de tous les fichiers C/H du projet

# Couleurs pour l'affichage
RED='\033[0;31m'  
GREEN='\033[0;32m'  
NC='\033[0m' # No Color  

# Vérifier que clang-format est installé
if ! command -v clang-format &> /dev/null; then
    echo -e "${RED}❌ clang-format n'est pas installé${NC}"
    echo "Installez-le avec : sudo apt install clang-format"
    exit 1
fi

# Vérifier que .clang-format existe
if [ ! -f ".clang-format" ]; then
    echo -e "${RED}❌ Fichier .clang-format introuvable${NC}"
    exit 1
fi

echo "🔍 Recherche des fichiers C/H..."

# Trouver tous les fichiers .c et .h
files=$(find . -type f \( -name "*.c" -o -name "*.h" \) | grep -v "build/" | grep -v "vendor/")

if [ -z "$files" ]; then
    echo -e "${RED}❌ Aucun fichier trouvé${NC}"
    exit 1
fi

count=0  
for file in $files; do  
    echo "✏️  Formatage : $file"
    clang-format -i "$file"
    ((count++))
done

echo -e "${GREEN}✅ $count fichiers formatés avec succès${NC}"
```

**Rendre le script exécutable et l'utiliser :**

```bash
chmod +x format-all.sh
./format-all.sh
```

---

## Intégration dans les éditeurs

### **Visual Studio Code**

#### **Extension C/C++**

L'extension officielle C/C++ de Microsoft inclut clang-format.

**Configuration dans settings.json :**

```json
{
  "C_Cpp.clang_format_style": "file",
  "C_Cpp.clang_format_fallbackStyle": "LLVM",
  "editor.formatOnSave": true,
  "[c]": {
    "editor.defaultFormatter": "ms-vscode.cpptools"
  }
}
```

**Utilisation :**
- `Shift+Alt+F` : Formater le fichier
- Automatique à la sauvegarde si `formatOnSave: true`

#### **Extension Clang-Format**

Alternative : extension dédiée.

```bash
code --install-extension xaver.clang-format
```

**Configuration :**

```json
{
  "clang-format.executable": "/usr/bin/clang-format",
  "clang-format.style": "file"
}
```

### **Vim/Neovim**

#### **Avec vim-clang-format**

**Installation (vim-plug) :**

```vim
Plug 'rhysd/vim-clang-format'
```

**Configuration dans .vimrc :**

```vim
" Formatage automatique à la sauvegarde
autocmd FileType c,cpp ClangFormatAutoEnable

" Raccourci pour formater (Leader + f)
autocmd FileType c,cpp nnoremap <buffer><Leader>f :<C-u>ClangFormat<CR>

" Style à utiliser
let g:clang_format#style_options = {
    \ "BasedOnStyle": "LLVM",
    \ "IndentWidth": 4,
    \ "ColumnLimit": 80,
    \ }
```

#### **Sans plugin (commande externe)**

```vim
" Formater le fichier avec clang-format
autocmd FileType c,cpp nnoremap <buffer><F8> :!clang-format -i %<CR><CR>
```

### **Emacs**

**Configuration :**

```elisp
(require 'clang-format)

;; Raccourci pour formater
(global-set-key (kbd "C-c f") 'clang-format-region)

;; Formatage automatique à la sauvegarde
(add-hook 'c-mode-hook
  (lambda ()
    (add-hook 'before-save-hook 'clang-format-buffer nil 'local)))
```

### **CLion / IntelliJ**

CLion intègre clang-format nativement.

**Configuration :**
1. `File` → `Settings` → `Editor` → `Code Style` → `C/C++`
2. Cochez `Enable ClangFormat`
3. Sélectionnez `Use .clang-format file`

**Raccourcis :**
- `Ctrl+Alt+L` : Formater le fichier
- `Ctrl+Alt+Shift+L` : Formater la sélection

---

## Intégration dans Git

### **Pre-commit hook**

Un hook Git qui formate automatiquement le code avant chaque commit.

**Créer le hook `.git/hooks/pre-commit` :**

```bash
#!/bin/bash
# Hook Git pre-commit pour formater automatiquement le code

# Récupérer les fichiers C/H modifiés
files=$(git diff --cached --name-only --diff-filter=ACM | grep -E '\.(c|h)$')

if [ -n "$files" ]; then
    echo "🔧 Formatage automatique avec clang-format..."

    for file in $files; do
        clang-format -i "$file"
        git add "$file"
        echo "✅ $file formaté"
    done

    echo "✨ Formatage terminé"
fi

exit 0
```

**Rendre le hook exécutable :**

```bash
chmod +x .git/hooks/pre-commit
```

**Test :**

```bash
# Modifiez un fichier C
echo "int main(){return 0;}" > test.c  
git add test.c  
git commit -m "Test"  

# Le code sera automatiquement formaté avant le commit
```

### **Pre-commit framework (moderne)**

Utilisez le framework `pre-commit` pour une gestion plus avancée.

**Installation :**

```bash
pip install pre-commit
```

**Configuration `.pre-commit-config.yaml` :**

```yaml
repos:
  - repo: https://github.com/pre-commit/mirrors-clang-format
    rev: v17.0.6
    hooks:
      - id: clang-format
        types: [c]
```

**Activation :**

```bash
pre-commit install
```

**Test manuel :**

```bash
pre-commit run --all-files
```

### **Vérifier le formatage sans modifier**

Script pour vérifier si des fichiers ne sont pas formatés :

```bash
#!/bin/bash
# check-format.sh

files=$(find . -name "*.c" -o -name "*.h" | grep -v build)  
status=0  

for file in $files; do
    diff -u <(cat "$file") <(clang-format "$file") > /dev/null
    if [ $? -ne 0 ]; then
        echo "❌ $file n'est pas formaté"
        status=1
    fi
done

if [ $status -eq 0 ]; then
    echo "✅ Tous les fichiers sont correctement formatés"
fi

exit $status
```

**Utilisation :**

```bash
chmod +x check-format.sh
./check-format.sh
```

---

## Intégration dans le CI/CD

### **GitHub Actions**

**Workflow `.github/workflows/format-check.yml` :**

```yaml
name: Format Check

on:
  push:
    branches: [ main, develop ]
  pull_request:
    branches: [ main ]

jobs:
  format-check:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4

    - name: Install clang-format
      run: |
        sudo apt-get update
        sudo apt-get install -y clang-format

    - name: Check formatting
      run: |
        # Trouver tous les fichiers C/H
        files=$(find . -name "*.c" -o -name "*.h")

        # Vérifier chaque fichier
        for file in $files; do
          clang-format --dry-run --Werror "$file"
        done

    - name: Show formatting diff (if failed)
      if: failure()
      run: |
        files=$(find . -name "*.c" -o -name "*.h")
        for file in $files; do
          echo "=== $file ==="
          diff -u "$file" <(clang-format "$file") || true
        done
```

**Alternative avec action dédiée :**

```yaml
name: Format Check

on: [push, pull_request]

jobs:
  format:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v4
    - uses: DoozyX/clang-format-lint-action@v0.16
      with:
        source: '.'
        extensions: 'c,h'
        clangFormatVersion: 16
```

### **GitLab CI**

**Configuration `.gitlab-ci.yml` :**

```yaml
stages:
  - lint

format-check:
  stage: lint
  image: ubuntu:24.04

  before_script:
    - apt-get update && apt-get install -y clang-format git

  script:
    - |
      files=$(find . -name "*.c" -o -name "*.h")
      status=0
      for file in $files; do
        clang-format --dry-run --Werror "$file" || status=1
      done
      exit $status

  only:
    - merge_requests
    - main
```

### **Jenkins**

**Jenkinsfile :**

```groovy
pipeline {
    agent any

    stages {
        stage('Format Check') {
            steps {
                sh '''
                    files=$(find . -name "*.c" -o -name "*.h")
                    for file in $files; do
                        clang-format --dry-run --Werror "$file"
                    done
                '''
            }
        }
    }
}
```

---

## Cas d'usage avancés

### **Désactiver le formatage sur certaines sections**

Utilisez des commentaires spéciaux :

```c
// clang-format off
int matrix[3][3] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9}
};
// clang-format on

// Ce code sera formaté normalement
int x = 1;
```

### **Fichiers de configuration par dossier**

Vous pouvez avoir plusieurs `.clang-format` dans différents dossiers :

```
projet/
├── .clang-format          (Style général : LLVM)
├── src/
│   └── legacy/
│       └── .clang-format  (Style legacy : GNU)
└── external/
    └── .clang-format      (Style externe : Google)
```

### **Formater seulement les lignes modifiées**

Utilisez `git-clang-format` :

```bash
# Installer git-clang-format
sudo apt install clang-format

# Formater seulement les lignes modifiées depuis le dernier commit
git clang-format

# Formater les changements par rapport à main
git clang-format main
```

### **Générer un rapport de formatage**

```bash
#!/bin/bash
# format-report.sh

files=$(find . -name "*.c" -o -name "*.h")  
total=0  
unformatted=0  

for file in $files; do
    ((total++))
    diff -u <(cat "$file") <(clang-format "$file") > /dev/null
    if [ $? -ne 0 ]; then
        ((unformatted++))
        echo "❌ $file"
    fi
done

formatted=$((total - unformatted))  
percentage=$((100 * formatted / total))  

echo ""  
echo "📊 Rapport de formatage"  
echo "Total de fichiers : $total"  
echo "Fichiers formatés : $formatted"  
echo "Fichiers non formatés : $unformatted"  
echo "Pourcentage : $percentage%"  
```

---

## Dépannage

### **Problème 1 : clang-format n'est pas trouvé**

**Erreur :**
```
bash: clang-format: command not found
```

**Solution :**
```bash
# Installer clang-format
sudo apt install clang-format

# Vérifier l'installation
which clang-format  
clang-format --version  
```

### **Problème 2 : Le fichier .clang-format n'est pas détecté**

**Symptômes :** clang-format utilise un style par défaut au lieu de votre config.

**Solutions :**

**a) Vérifier le fichier :**
```bash
# Le fichier existe-t-il ?
ls -la .clang-format

# Le fichier est-il valide ?
clang-format -style=file -dump-config > /dev/null
```

**b) Utiliser un chemin absolu :**
```bash
clang-format -style=file:/chemin/absolu/.clang-format fichier.c
```

**c) Vérifier le répertoire courant :**
```bash
pwd  # Assurez-vous d'être dans le bon dossier
```

### **Problème 3 : Erreur de syntaxe YAML**

**Erreur :**
```
Error reading .clang-format: Invalid argument
```

**Causes communes :**
- Tabulations au lieu d'espaces
- Pas d'espace après les `:`
- Indentation incorrecte

**Solution :**

```yaml
# ❌ Incorrect
IndentWidth:4              # Pas d'espace après :
    UseTab: Never          # Indentation incorrecte

# ✅ Correct
IndentWidth: 4  
UseTab: Never  
```

**Vérifier la syntaxe :**
```bash
clang-format -style=file -dump-config
```

### **Problème 4 : Formatage non désiré**

**Symptômes :** clang-format change des choses que vous ne voulez pas modifier.

**Solutions :**

**a) Désactiver pour une section :**
```c
// clang-format off
// Code non formaté
// clang-format on
```

**b) Ajuster les options :**
```yaml
# Par exemple, garder les courtes fonctions sur une ligne
AllowShortFunctionsOnASingleLine: All
```

**c) Utiliser des pénalités :**
```yaml
# Rendre les coupures de ligne moins agressives
PenaltyBreakString: 1000000  
PenaltyExcessCharacter: 1000000  
```

### **Problème 5 : Versions incompatibles**

**Symptômes :** Différents résultats selon les machines.

**Cause :** Versions différentes de clang-format.

**Solution :**

```bash
# Vérifier la version
clang-format --version

# Spécifier la version dans le projet
echo "# clang-format version: 14.0.0" >> .clang-format

# Utiliser Docker pour garantir la version
docker run --rm -v $(pwd):/code -w /code \
  xianpengshen/clang-tools:14 \
  clang-format -i *.c
```

---

## Récapitulatif et bonnes pratiques

### **Checklist de configuration**

✅ **Installer clang-format**
```bash
sudo apt install clang-format
```

✅ **Créer .clang-format à la racine du projet**
```bash
clang-format -style=llvm -dump-config > .clang-format
# Puis personnaliser
```

✅ **Tester la configuration**
```bash
clang-format test.c
```

✅ **Intégrer dans l'éditeur**
- VS Code : `"editor.formatOnSave": true`
- Vim : Plugin vim-clang-format

✅ **Ajouter un hook Git**
```bash
# Créer .git/hooks/pre-commit
```

✅ **Ajouter dans le CI/CD**
- GitHub Actions / GitLab CI

✅ **Documenter dans le README**
```markdown
## Formatage du code

Ce projet utilise clang-format. Pour formater tout le code :

 ```bash
./format-all.sh
 ```
```

### **Bonnes pratiques**

🎯 **Choisir un style et s'y tenir**
- Évitez de changer fréquemment
- Documentez le choix du style

🎯 **Commiter le fichier .clang-format**
- Partagez la config avec l'équipe
- Versionner les changements

🎯 **Formater régulièrement**
- Pre-commit hook = format automatique
- CI/CD = vérification obligatoire

🎯 **Communiquer en équipe**
- Expliquez pourquoi ce style
- Formez les nouveaux développeurs

🎯 **Ne pas formater le code legacy en une fois**
- Formatez progressivement
- Ou créez un commit "reformat" séparé

### **Ressources supplémentaires**

📚 **Documentation officielle**
- [Clang-Format Style Options](https://clang.llvm.org/docs/ClangFormatStyleOptions.html)

🔧 **Configurateurs en ligne**
- [Clang-Format Configurator](https://zed0.co.uk/clang-format-configurator/)
- [Clang Format Interactive](https://clang-format-configurator.site/)

💡 **Exemples de configurations**
- [Google Style Guide](https://google.github.io/styleguide/cppguide.html)
- [LLVM Coding Standards](https://llvm.org/docs/CodingStandards.html)
- [Linux Kernel Coding Style](https://www.kernel.org/doc/html/latest/process/coding-style.html)

---

## Conclusion

`clang-format` est un outil indispensable pour tout développeur C moderne. Il garantit :

- ✨ **Cohérence** : Code uniforme dans tout le projet
- ✨ **Gain de temps** : Pas de formatage manuel
- ✨ **Collaboration** : Facilite les code reviews
- ✨ **Qualité** : Code plus lisible et maintenable

**Prochaines étapes :**

1. Installez clang-format sur votre machine
2. Créez un fichier `.clang-format` pour votre projet
3. Testez sur quelques fichiers
4. Intégrez dans votre éditeur
5. Ajoutez un pre-commit hook
6. Configurez le CI/CD pour vérifier le formatage

**Astuce finale :** N'ayez pas peur d'expérimenter avec les options ! Vous pouvez toujours revenir en arrière en modifiant `.clang-format`.

---

🚀 **Bon formatage et bon code !**

⏭️ [.devcontainer.json complet](/annexes/configuration-environnement/04-devcontainer-json-complet.md)
