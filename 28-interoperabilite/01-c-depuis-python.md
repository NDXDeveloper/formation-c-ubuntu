🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 28.1 Appeler du C depuis Python

## Introduction

### Pourquoi appeler du C depuis Python ?

Python est un langage extrêmement populaire, apprécié pour sa simplicité et sa productivité. Cependant, son exécution interprétée peut être plus lente que du code compilé. C'est là que C entre en jeu !

**Cas d'usage typiques :**
- 🚀 **Performance critique** : Accélérer les calculs intensifs (traitement d'images, algorithmes mathématiques)
- 🔧 **Réutilisation de code existant** : Intégrer des bibliothèques C bien établies
- ⚙️ **Accès bas niveau** : Interagir avec le matériel ou le système d'exploitation
- 📚 **Legacy code** : Moderniser une application C en l'exposant via une API Python

**Exemple concret :** Vous développez un outil d'analyse de logs en Python, mais le parsing de millions de lignes est trop lent. Vous pouvez écrire la partie critique en C et l'appeler depuis Python pour gagner un facteur 10-100x en vitesse !

---

## Les trois principales méthodes

Python offre plusieurs façons d'interfacer avec du code C. Voici les trois approches les plus courantes :

| Méthode | Difficulté | Flexibilité | Performance | Cas d'usage |
|---------|-----------|-------------|-------------|-------------|
| **ctypes** | ⭐ Facile | Moyenne | Excellente | Bibliothèques existantes |
| **cffi** | ⭐⭐ Moyenne | Élevée | Excellente | Projets modernes |
| **Extension C** | ⭐⭐⭐ Avancée | Maximale | Maximale | Intégration profonde |

Nous allons explorer chacune de ces méthodes en détail.

---

## Méthode 1 : ctypes (La plus simple)

### Qu'est-ce que ctypes ?

`ctypes` est un module intégré à Python qui permet de charger des bibliothèques dynamiques (`.so` sur Linux, `.dll` sur Windows) et d'appeler leurs fonctions directement.

**Avantages :**
- ✅ Aucune compilation nécessaire
- ✅ Disponible par défaut avec Python
- ✅ Idéal pour les débutants
- ✅ Parfait pour utiliser des bibliothèques existantes

**Inconvénients :**
- ❌ Syntaxe un peu verbeuse
- ❌ Pas de vérification de types à la compilation

### Exemple 1 : Fonction simple

Créons une fonction C qui additionne deux nombres entiers.

**Fichier `addition.c` :**
```c
// Fonction simple qui additionne deux entiers
int addition(int a, int b) {
    return a + b;
}
```

**Compilation en bibliothèque partagée :**
```bash
# Sur Linux/Ubuntu
gcc -shared -o libaddition.so -fPIC addition.c

# -shared : créer une bibliothèque partagée
# -fPIC : Position Independent Code (requis pour .so)
# -o : nom du fichier de sortie
```

**Utilisation depuis Python avec ctypes :**
```python
import ctypes

# Charger la bibliothèque
lib = ctypes.CDLL('./libaddition.so')

# Spécifier les types des arguments et du retour (optionnel mais recommandé)
lib.addition.argtypes = [ctypes.c_int, ctypes.c_int]  
lib.addition.restype = ctypes.c_int  

# Appeler la fonction
resultat = lib.addition(5, 7)  
print(f"5 + 7 = {resultat}")  # Affiche : 5 + 7 = 12  
```

**Explication étape par étape :**

1. **`ctypes.CDLL()`** : Charge la bibliothèque dynamique
   - Le chemin peut être relatif (`./lib.so`) ou absolu
   - Sur Linux, préfixez avec `lib` et suffixez avec `.so`

2. **`argtypes`** : Définit les types des paramètres
   - `ctypes.c_int` correspond à `int` en C
   - Important pour éviter les erreurs de passage de paramètres

3. **`restype`** : Définit le type de retour
   - Par défaut, ctypes suppose que c'est un `int`
   - Spécifiez explicitement pour d'autres types

### Exemple 2 : Travailler avec des tableaux

Passons maintenant un tableau (array) à une fonction C.

**Fichier `somme_tableau.c` :**
```c
#include <stddef.h>  // Pour size_t

// Calcule la somme des éléments d'un tableau
int somme_tableau(int *tableau, size_t taille) {
    int somme = 0;
    for (size_t i = 0; i < taille; i++) {
        somme += tableau[i];
    }
    return somme;
}
```

**Compilation :**
```bash
gcc -shared -o libsomme.so -fPIC somme_tableau.c
```

**Utilisation depuis Python :**
```python
import ctypes

lib = ctypes.CDLL('./libsomme.so')

# Définir les types
lib.somme_tableau.argtypes = [
    ctypes.POINTER(ctypes.c_int),  # Pointeur vers int
    ctypes.c_size_t                 # size_t
]
lib.somme_tableau.restype = ctypes.c_int

# Créer un tableau Python
nombres = [10, 20, 30, 40, 50]

# Convertir en tableau C
tableau_c = (ctypes.c_int * len(nombres))(*nombres)

# Appeler la fonction
resultat = lib.somme_tableau(tableau_c, len(nombres))  
print(f"Somme : {resultat}")  # Affiche : Somme : 150  
```

**Points clés :**

- **`ctypes.POINTER(ctypes.c_int)`** : Représente un pointeur `int*` en C
- **`(ctypes.c_int * len(nombres))`** : Crée un type "tableau de N entiers"
- **`(*nombres)`** : Initialise le tableau avec les valeurs de la liste Python

### Exemple 3 : Chaînes de caractères

Les chaînes nécessitent une attention particulière car elles sont gérées différemment en C et Python.

**Fichier `strings.c` :**
```c
#include <string.h>
#include <stdio.h>

// Affiche une chaîne en majuscules (modifie sur place)
void mettre_en_majuscules(char *chaine) {
    for (int i = 0; chaine[i] != '\0'; i++) {
        if (chaine[i] >= 'a' && chaine[i] <= 'z') {
            chaine[i] = chaine[i] - 32;  // Conversion ASCII
        }
    }
}

// Retourne la longueur d'une chaîne
int longueur_chaine(const char *chaine) {
    return strlen(chaine);
}
```

**Compilation :**
```bash
gcc -shared -o libstrings.so -fPIC strings.c
```

**Utilisation depuis Python :**
```python
import ctypes

lib = ctypes.CDLL('./libstrings.so')

# Configuration pour longueur_chaine
lib.longueur_chaine.argtypes = [ctypes.c_char_p]  
lib.longueur_chaine.restype = ctypes.c_int  

# Configuration pour mettre_en_majuscules
lib.mettre_en_majuscules.argtypes = [ctypes.c_char_p]  
lib.mettre_en_majuscules.restype = None  # void en C  

# Exemple 1 : Calculer la longueur
texte = b"Bonjour Python"  # Notez le 'b' pour bytes  
longueur = lib.longueur_chaine(texte)  
print(f"Longueur : {longueur}")  # Affiche : Longueur : 14  

# Exemple 2 : Modifier une chaîne (attention : doit être mutable)
texte_mutable = ctypes.create_string_buffer(b"hello world")  
lib.mettre_en_majuscules(texte_mutable)  
print(f"Majuscules : {texte_mutable.value.decode()}")  # HELLO WORLD  
```

**Points d'attention :**

- **`b"texte"`** : Les chaînes doivent être des bytes en Python 3
- **`ctypes.c_char_p`** : Représente un `char*` en C
- **`create_string_buffer()`** : Crée un buffer mutable (nécessaire pour modification)
- **`.value.decode()`** : Convertit le buffer C en string Python

### Correspondance des types ctypes ↔ C

| Type C | Type ctypes | Remarques |
|--------|-------------|-----------|
| `int` | `c_int` | Entier signé (32 bits généralement) |
| `unsigned int` | `c_uint` | Entier non signé |
| `long` | `c_long` | Entier long |
| `float` | `c_float` | Flottant simple précision |
| `double` | `c_double` | Flottant double précision |
| `char` | `c_char` | Caractère (1 byte) |
| `char*` | `c_char_p` | Chaîne de caractères |
| `void*` | `c_void_p` | Pointeur générique |
| `size_t` | `c_size_t` | Taille (non signé) |
| `int*` | `POINTER(c_int)` | Pointeur vers int |

---

## Méthode 2 : CFFI (C Foreign Function Interface)

### Qu'est-ce que CFFI ?

CFFI est une bibliothèque tierce (à installer via pip) qui offre une approche plus moderne et pythonique pour interfacer avec C.

**Installation :**
```bash
pip install cffi
```

**Avantages par rapport à ctypes :**
- ✅ Syntaxe plus claire et naturelle
- ✅ Meilleure gestion des erreurs
- ✅ Mode ABI (comme ctypes) et mode API (avec compilation)
- ✅ Populaire dans la communauté Python moderne

### Mode ABI : Utilisation de bibliothèques existantes

Le mode ABI est similaire à ctypes : on charge une bibliothèque déjà compilée.

**Exemple avec notre fonction addition :**

```python
from cffi import FFI

ffi = FFI()

# Déclarer la signature de la fonction C
ffi.cdef("""
    int addition(int a, int b);
""")

# Charger la bibliothèque
lib = ffi.dlopen('./libaddition.so')

# Appeler la fonction (syntaxe plus naturelle !)
resultat = lib.addition(5, 7)  
print(f"5 + 7 = {resultat}")  
```

**Comparaison avec ctypes :**
- Plus lisible : on déclare directement la signature C
- Pas besoin de spécifier `argtypes` et `restype` séparément
- Détection d'erreurs améliorée

### Mode API : Compilation inline

Le mode API permet de compiler du code C directement depuis Python. C'est très pratique pour des petits bouts de code.

**Exemple complet :**

```python
from cffi import FFI

ffi = FFI()

# Déclarer les fonctions C que l'on va utiliser
ffi.cdef("""
    int multiplication(int a, int b);
    double puissance(double base, int exposant);
""")

# Définir le code C inline
ffi.set_source("_math_operations", """
    int multiplication(int a, int b) {
        return a * b;
    }

    double puissance(double base, int exposant) {
        double resultat = 1.0;
        for (int i = 0; i < exposant; i++) {
            resultat *= base;
        }
        return resultat;
    }
""")

# Compiler (crée un module Python)
ffi.compile(verbose=True)

# Utiliser le module compilé
from _math_operations import lib

print(f"3 * 4 = {lib.multiplication(3, 4)}")  
print(f"2.5^3 = {lib.puissance(2.5, 3)}")  
```

**Workflow :**
1. **`cdef()`** : Déclare les signatures (headers)
2. **`set_source()`** : Fournit l'implémentation C
3. **`compile()`** : Compile et crée un module Python
4. **`from _module import lib`** : Utilise le code compilé

### Exemple avancé : Structure C

Travaillons avec une structure C, ce qui est très courant en programmation système.

**Code Python avec CFFI :**

```python
from cffi import FFI

ffi = FFI()

# Déclarer la structure et les fonctions
ffi.cdef("""
    typedef struct {
        char nom[50];
        int age;
        double salaire;
    } Personne;

    void afficher_personne(Personne *p);
    double augmenter_salaire(Personne *p, double pourcentage);
""")

# Code C
ffi.set_source("_personne", """
    #include <stdio.h>

    typedef struct {
        char nom[50];
        int age;
        double salaire;
    } Personne;

    void afficher_personne(Personne *p) {
        printf("Nom: %s, Age: %d, Salaire: %.2f€\\n",
               p->nom, p->age, p->salaire);
    }

    double augmenter_salaire(Personne *p, double pourcentage) {
        p->salaire *= (1.0 + pourcentage / 100.0);
        return p->salaire;
    }
""")

ffi.compile(verbose=True)

# Utilisation
from _personne import ffi, lib

# Créer une structure depuis Python
personne = ffi.new("Personne *")  
personne.nom = b"Alice Dupont"  
personne.age = 30  
personne.salaire = 45000.0  

# Appeler les fonctions C
lib.afficher_personne(personne)  
nouveau_salaire = lib.augmenter_salaire(personne, 10.0)  
print(f"Nouveau salaire après augmentation : {nouveau_salaire:.2f}€")  
lib.afficher_personne(personne)  
```

**Points clés :**
- **`ffi.new("Type *")`** : Alloue une structure en mémoire C
- Les champs sont accessibles naturellement : `personne.age = 30`
- Les chaînes doivent toujours être des bytes : `b"texte"`

---

## Méthode 3 : Extensions C Python (Python C API)

### Introduction

L'API C de Python permet de créer des modules d'extension natifs. C'est la méthode la plus puissante mais aussi la plus complexe.

**Quand l'utiliser ?**
- Intégration très étroite avec Python (types personnalisés, gestion d'erreurs)
- Performance maximale
- Besoin d'accéder aux internals de Python

**Note pour débutants :** Cette méthode est plus avancée. Si ctypes ou CFFI répondent à vos besoins, privilégiez-les !

### Structure d'une extension minimale

**Fichier `module_exemple.c` :**

```c
#define PY_SSIZE_T_CLEAN
#include <Python.h>

// Fonction C qui sera exposée à Python
static PyObject* addition_py(PyObject* self, PyObject* args) {
    (void)self;
    int a, b;

    // Parser les arguments Python
    if (!PyArg_ParseTuple(args, "ii", &a, &b)) {
        return NULL;  // Erreur de parsing
    }

    // Calcul
    int resultat = a + b;

    // Convertir le résultat en objet Python
    return PyLong_FromLong(resultat);
}

// Table des méthodes du module
static PyMethodDef ModuleMethods[] = {
    {"addition", addition_py, METH_VARARGS, "Additionne deux entiers"},
    {NULL, NULL, 0, NULL}  // Sentinelle
};

// Définition du module
static struct PyModuleDef moduleexemple = {
    PyModuleDef_HEAD_INIT,
    "module_exemple",      // Nom du module
    "Module d'exemple",    // Documentation
    -1,
    ModuleMethods,
    NULL, NULL, NULL, NULL
};

// Fonction d'initialisation
PyMODINIT_FUNC PyInit_module_exemple(void) {
    return PyModule_Create(&moduleexemple);
}
```

### Compilation de l'extension

**Méthode moderne avec `setup.py` :**

```python
from setuptools import setup, Extension

module = Extension(
    'module_exemple',
    sources=['module_exemple.c']
)

setup(
    name='module_exemple',
    version='1.0',
    description='Module C pour Python',
    ext_modules=[module]
)
```

**Installation :**
```bash
python setup.py build  
python setup.py install --user  
```

**Utilisation :**
```python
import module_exemple

resultat = module_exemple.addition(10, 20)  
print(f"10 + 20 = {resultat}")  
```

### Anatomie d'une fonction d'extension

Décomposons la fonction `addition_py` :

```c
static PyObject* addition_py(PyObject* self, PyObject* args)
```

- **`PyObject*`** : Tous les objets Python sont représentés par ce type
- **`self`** : Référence au module (ignoré pour les fonctions simples)
- **`args`** : Tuple contenant les arguments passés depuis Python

```c
if (!PyArg_ParseTuple(args, "ii", &a, &b)) {
    return NULL;
}
```

- **`PyArg_ParseTuple()`** : Extrait les arguments Python
- **`"ii"`** : Format string (deux entiers)
- Retourne `NULL` en cas d'erreur (Python lèvera une exception)

```c
return PyLong_FromLong(resultat);
```

- **`PyLong_FromLong()`** : Convertit un `long` C en objet Python `int`
- Chaque type Python a ses fonctions de conversion

### Gestion des erreurs

Lever une exception depuis C :

```c
static PyObject* diviser_py(PyObject* self, PyObject* args) {
    int a, b;

    if (!PyArg_ParseTuple(args, "ii", &a, &b)) {
        return NULL;
    }

    if (b == 0) {
        PyErr_SetString(PyExc_ZeroDivisionError, "Division par zéro !");
        return NULL;
    }

    return PyLong_FromLong(a / b);
}
```

Depuis Python :
```python
try:
    module_exemple.diviser(10, 0)
except ZeroDivisionError as e:
    print(f"Erreur : {e}")
```

---

## Comparaison des trois méthodes

### Tableau récapitulatif

| Critère | ctypes | CFFI | Extension C |
|---------|--------|------|-------------|
| **Installation** | Inclus avec Python | `pip install cffi` | Inclus (nécessite compilateur) |
| **Courbe d'apprentissage** | Facile | Moyenne | Difficile |
| **Performance** | Excellente | Excellente | Maximale |
| **Portabilité** | Bonne | Très bonne | Bonne (recompilation) |
| **Flexibilité** | Limitée | Élevée | Maximale |
| **Gestion erreurs** | Basique | Bonne | Totale |
| **Recommandé pour** | Débutants, bibliothèques existantes | Projets modernes | Intégration profonde |

### Guide de choix

**Utilisez ctypes si :**
- ✅ Vous débutez avec l'interopérabilité C/Python
- ✅ Vous voulez simplement utiliser une bibliothèque existante
- ✅ Vous ne voulez pas gérer de compilation
- ✅ Vos besoins sont simples (appels de fonctions basiques)

**Utilisez CFFI si :**
- ✅ Vous avez un projet Python moderne
- ✅ Vous voulez écrire du code C directement dans Python
- ✅ Vous avez besoin de structures complexes
- ✅ Vous appréciez une syntaxe claire et pythonique

**Utilisez l'API C Python si :**
- ✅ Vous créez une bibliothèque Python qui doit être très performante
- ✅ Vous avez besoin d'intégration profonde avec Python
- ✅ Vous créez de nouveaux types Python en C
- ✅ Vous maîtrisez déjà C et Python

---

## Bonnes pratiques

### 1. Gestion de la mémoire

**⚠️ Attention critique :** Quand vous passez des données entre Python et C, la gestion de la mémoire devient votre responsabilité !

```python
# ❌ DANGEREUX avec ctypes
def mauvaise_fonction():
    tableau = (ctypes.c_int * 10)(*range(10))
    lib.traiter_tableau(tableau, 10)
    # Le tableau est détruit ici, mais si C garde le pointeur...
    # => SEGMENTATION FAULT potentiel

# ✅ BON
def bonne_fonction():
    tableau = (ctypes.c_int * 10)(*range(10))
    lib.traiter_tableau(tableau, 10)
    # Traitement terminé avant la fin de la fonction
    return resultat
```

**Règle d'or :** Si C garde une référence à de la mémoire Python, assurez-vous que l'objet Python reste vivant !

### 2. Vérification des types

Toujours spécifier explicitement les types avec ctypes :

```python
# ❌ Risqué
lib.ma_fonction(valeur)

# ✅ Sûr
lib.ma_fonction.argtypes = [ctypes.c_int]  
lib.ma_fonction.restype = ctypes.c_int  
lib.ma_fonction(valeur)  
```

### 3. Gestion des erreurs

N'oubliez jamais de vérifier les codes d'erreur C :

```c
// Dans votre code C
int operation_risquee(int valeur) {
    if (valeur < 0) {
        return -1;  // Code d'erreur
    }
    return valeur * 2;
}
```

```python
# En Python
resultat = lib.operation_risquee(-5)  
if resultat == -1:  
    print("Erreur : valeur négative non supportée")
```

### 4. Documentation

Documentez vos interfaces C/Python :

```python
def calculer_moyenne(tableau):
    """
    Calcule la moyenne d'un tableau d'entiers en utilisant du code C.

    Args:
        tableau (list[int]): Liste d'entiers

    Returns:
        float: La moyenne des valeurs

    Raises:
        ValueError: Si le tableau est vide

    Note:
        Cette fonction utilise libmath.so compilée depuis moyenne.c
    """
    if not tableau:
        raise ValueError("Tableau vide")

    tableau_c = (ctypes.c_int * len(tableau))(*tableau)
    return lib.moyenne(tableau_c, len(tableau))
```

### 5. Tests

Testez systématiquement vos interfaces :

```python
import unittest

class TestCFunctions(unittest.TestCase):
    def test_addition(self):
        self.assertEqual(lib.addition(2, 3), 5)
        self.assertEqual(lib.addition(-5, 10), 5)
        self.assertEqual(lib.addition(0, 0), 0)

    def test_division_par_zero(self):
        with self.assertRaises(ZeroDivisionError):
            lib.diviser(10, 0)
```

---

## Pièges courants et solutions

### Piège 1 : Encodage des chaînes

**Problème :**
```python
# ❌ Python 3 : str n'est pas bytes
lib.ma_fonction("Bonjour")  # TypeError !
```

**Solution :**
```python
# ✅ Encoder explicitement
lib.ma_fonction(b"Bonjour")  # bytes
# ou
lib.ma_fonction("Bonjour".encode('utf-8'))
```

### Piège 2 : Durée de vie des objets

**Problème :**
```python
# ❌ Le buffer peut être libéré trop tôt
def obtenir_buffer():
    return ctypes.create_string_buffer(b"temporaire")

buffer = obtenir_buffer()  
lib.utiliser_buffer(buffer)  # Peut crasher !  
```

**Solution :**
```python
# ✅ Garder une référence
class GestionnaireChaineC:
    def __init__(self, texte):
        self.buffer = ctypes.create_string_buffer(texte.encode())

    def utiliser(self):
        lib.utiliser_buffer(self.buffer)
```

### Piège 3 : Modification de constantes

**Problème :**
```python
# ❌ Tenter de modifier une chaîne littérale
texte = b"constante"  
lib.modifier_chaine(texte)  # Peut crasher sur certains systèmes !  
```

**Solution :**
```python
# ✅ Utiliser un buffer mutable
texte = ctypes.create_string_buffer(b"modifiable")  
lib.modifier_chaine(texte)  
```

### Piège 4 : Oubli du GIL (Global Interpreter Lock)

Pour du code C long qui n'accède pas à Python :

```c
// Dans une extension C
static PyObject* calcul_long(PyObject* self, PyObject* args) {
    // Libérer le GIL pour permettre d'autres threads Python
    Py_BEGIN_ALLOW_THREADS

    // Code C long qui n'utilise pas l'API Python
    for (int i = 0; i < 1000000000; i++) {
        // calculs...
    }

    Py_END_ALLOW_THREADS

    Py_RETURN_NONE;
}
```

---

## Cas d'usage réel : Accélération d'un traitement d'images

Imaginons que vous traitiez des images en Python et que ce soit trop lent.

### Version Python pure (lente)

```python
def inverser_couleurs_python(image):
    """Inverse les couleurs d'une image (255 - valeur)"""
    hauteur, largeur = image.shape
    resultat = image.copy()

    for i in range(hauteur):
        for j in range(largeur):
            resultat[i, j] = 255 - image[i, j]

    return resultat

# Sur une image 1920x1080 : ~2 secondes
```

### Version C (rapide)

**Fichier `image_ops.c` :**
```c
#include <stddef.h>

void inverser_couleurs_c(unsigned char *image, size_t taille) {
    for (size_t i = 0; i < taille; i++) {
        image[i] = 255 - image[i];
    }
}
```

**Compilation :**
```bash
gcc -shared -O3 -o libimageops.so -fPIC image_ops.c
```

**Utilisation avec ctypes :**
```python
import ctypes  
import numpy as np  

lib = ctypes.CDLL('./libimageops.so')  
lib.inverser_couleurs_c.argtypes = [  
    ctypes.POINTER(ctypes.c_ubyte),
    ctypes.c_size_t
]

def inverser_couleurs_c(image):
    """Version accélérée avec C"""
    resultat = image.copy()

    # Obtenir un pointeur vers les données NumPy
    hauteur, largeur = resultat.shape
    taille = hauteur * largeur
    pointeur = resultat.ctypes.data_as(ctypes.POINTER(ctypes.c_ubyte))

    # Appeler la fonction C
    lib.inverser_couleurs_c(pointeur, taille)

    return resultat

# Sur une image 1920x1080 : ~0.02 secondes (100x plus rapide !)
```

**Gain :** De 2 secondes à 0.02 seconde, soit un speedup de **100x** !

---

## Outils et ressources

### Debugging

**1. Vérifier le chargement des bibliothèques :**
```bash
# Voir les dépendances d'un .so
ldd libvotre_bibliotheque.so

# Vérifier les symboles exportés
nm -D libvotre_bibliotheque.so
```

**2. Debugger avec GDB :**
```bash
# Lancer Python sous GDB
gdb python
(gdb) run votre_script.py
```

**3. Valgrind pour les fuites mémoire :**
```bash
valgrind --leak-check=full python script_qui_appelle_c.py
```

### Bibliothèques tierces populaires

Plusieurs bibliothèques Python utilisent massivement du code C :

- **NumPy** : Calculs numériques (C et Fortran)
- **Pandas** : Manipulation de données (C et Cython)
- **Pillow** : Traitement d'images (C)
- **Cryptography** : Cryptographie (C via CFFI)
- **lxml** : Parsing XML (C via libxml2)

Vous pouvez vous inspirer de leur code source !

### Documentation officielle

- [Python C API](https://docs.python.org/3/c-api/index.html)
- [ctypes documentation](https://docs.python.org/3/library/ctypes.html)
- [CFFI documentation](https://cffi.readthedocs.io/)

---

## Récapitulatif

### Points clés à retenir

1. **Trois méthodes principales :**
   - **ctypes** : Simple, sans compilation, idéal pour débuter
   - **CFFI** : Moderne, syntaxe claire, bon compromis
   - **Extension C** : Maximale performance et intégration

2. **Quand utiliser C depuis Python :**
   - Boucles intensives sur de grandes données
   - Calculs mathématiques complexes
   - Accès à des bibliothèques système
   - Legacy code à réutiliser

3. **Précautions essentielles :**
   - ⚠️ Gestion mémoire rigoureuse
   - ⚠️ Toujours spécifier les types
   - ⚠️ Tester exhaustivement
   - ⚠️ Documenter les interfaces

4. **Performance :**
   - Les gains peuvent être spectaculaires (10x-100x)
   - Mais attention au surcoût de l'interface Python ↔ C
   - Profitez-en pour de grosses opérations, pas des appels fréquents de petites fonctions

### Le mot de la fin

L'interopérabilité C/Python est un outil puissant dans la boîte à outils du développeur moderne. Elle permet de combiner :
- La **rapidité de développement** de Python
- La **performance et le contrôle** du C

Commencez par ctypes pour des besoins simples, explorez CFFI pour des projets modernes, et n'hésitez pas à plonger dans l'API C pour une intégration maximale quand c'est nécessaire.

**Bon coding ! 🚀**

---


⏭️ [Appeler du C depuis Go](/28-interoperabilite/02-c-depuis-go.md)
