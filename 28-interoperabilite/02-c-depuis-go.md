🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 28.2 Appeler du C depuis Go

## Introduction

### Pourquoi appeler du C depuis Go ?

Go est un langage moderne, performant et conçu pour la simplicité. Cependant, il existe des situations où l'interopérabilité avec C devient nécessaire ou avantageuse.

**Cas d'usage typiques :**
- 🔧 **Réutilisation de code existant** : Intégrer des bibliothèques C matures et éprouvées
- 📚 **Accès à des APIs système** : Utiliser des bibliothèques bas niveau non disponibles en Go pur
- ⚡ **Performance critique** : Dans certains cas spécifiques (calculs SIMD, assembleur optimisé)
- 🌐 **Interopérabilité legacy** : Moderniser progressivement une base de code C
- 🔌 **Drivers et matériel** : Interfacer avec du matériel via des bibliothèques C

**Exemple concret :** Vous développez un service Go qui doit interfacer avec une bibliothèque de traitement vidéo (comme FFmpeg) ou une base de données embarquée (comme SQLite) dont les bindings officiels sont en C.

### Go et C : Une relation privilégiée

Contrairement à d'autres langages, Go a été conçu dès le départ avec l'interopérabilité C en tête :
- ✅ **cgo** est inclus dans la toolchain Go standard
- ✅ Syntaxe relativement simple pour appeler du C
- ✅ Gestion automatique de nombreux aspects (conversions de types, etc.)
- ✅ Performance excellente (pas de couche d'abstraction lourde)

**Attention :** Bien que puissant, cgo a des implications importantes sur la compilation et le déploiement que nous aborderons.

---

## cgo : L'outil d'interopérabilité

### Qu'est-ce que cgo ?

`cgo` est un outil intégré à Go qui permet d'appeler du code C depuis Go et vice-versa. Il fonctionne en :
1. Analysant les commentaires spéciaux dans le code Go
2. Générant du code "glue" (connecteur) automatiquement
3. Compilant le tout ensemble

**Activation de cgo :**
```go
import "C"  // Cette import active cgo
```

Cette simple ligne d'import déclenche toute la machinerie cgo !

---

## Premier exemple : Hello from C

Commençons par l'exemple le plus simple possible.

### Code Go avec C inline

**Fichier `hello.go` :**
```go
package main

/*
#include <stdio.h>

void hello_from_c() {
    printf("Hello from C!\n");
}
*/
import "C"

func main() {
    C.hello_from_c()
}
```

**Compilation et exécution :**
```bash
go run hello.go
# Affiche : Hello from C!
```

### Anatomie du code

**1. Le bloc de commentaire spécial :**
```go
/*
#include <stdio.h>

void hello_from_c() {
    printf("Hello from C!\n");
}
*/
```

- **Doit être juste avant** `import "C"` (sans ligne vide)
- Contient du code C pur (headers, fonctions, définitions)
- Appelé le **"préambule C"** ou **"preamble"**

**2. L'import magique :**
```go
import "C"
```

- Active cgo
- Crée un pseudo-package `C` contenant toutes les définitions C
- Pas de guillemets autour du `C` !

**3. L'appel de fonction C :**
```go
C.hello_from_c()
```

- Préfixe `C.` pour accéder aux symboles C
- La fonction C devient accessible comme une fonction Go

---

## Passer des arguments et retourner des valeurs

### Exemple 1 : Fonction simple avec retour

**Fichier `math_ops.go` :**
```go
package main

/*
int addition(int a, int b) {
    return a + b;
}

int multiplication(int a, int b) {
    return a * b;
}
*/
import "C"  
import "fmt"  

func main() {
    // Appeler les fonctions C
    somme := C.addition(5, 7)
    produit := C.multiplication(3, 4)

    fmt.Printf("5 + 7 = %d\n", somme)
    fmt.Printf("3 * 4 = %d\n", produit)
}
```

**Sortie :**
```
5 + 7 = 12
3 * 4 = 12
```

**Points clés :**
- Les types C (`int`) sont automatiquement convertis en types Go (`C.int`)
- Le retour est également un type C qu'on peut utiliser directement
- `%d` fonctionne car `C.int` est compatible avec le formatage Go

### Exemple 2 : Fonction avec plusieurs types

**Fichier `calculs.go` :**
```go
package main

/*
#cgo LDFLAGS: -lm
#include <math.h>

double puissance(double base, int exposant) {
    return pow(base, (double)exposant);
}

float moyenne(float a, float b) {
    return (a + b) / 2.0f;
}
*/
import "C"  
import "fmt"

func main() {
    // Types float et double
    resultat1 := C.puissance(2.5, 3)
    fmt.Printf("2.5^3 = %f\n", resultat1)

    resultat2 := C.moyenne(10.5, 20.3)
    fmt.Printf("Moyenne de 10.5 et 20.3 = %f\n", resultat2)
}
```

**Remarques :**
- `C.double` correspond à `float64` en Go
- `C.float` correspond à `float32` en Go
- La bibliothèque math C (`<math.h>`) est directement utilisable

---

## Conversions de types

### Types de base : Correspondance C ↔ Go

| Type C | Type cgo | Type Go équivalent | Remarques |
|--------|----------|-------------------|-----------|
| `char` | `C.char` | `byte` / `int8` | Caractère 8 bits |
| `signed char` | `C.schar` | `int8` | Entier signé 8 bits |
| `unsigned char` | `C.uchar` | `uint8` | Entier non signé 8 bits |
| `short` | `C.short` | `int16` | Entier court |
| `unsigned short` | `C.ushort` | `uint16` | |
| `int` | `C.int` | `int32` (généralement) | Attention : taille dépend de l'architecture |
| `unsigned int` | `C.uint` | `uint32` | |
| `long` | `C.long` | `int32` ou `int64` | Dépend de l'architecture |
| `long long` | `C.longlong` | `int64` | |
| `float` | `C.float` | `float32` | |
| `double` | `C.double` | `float64` | |
| `size_t` | `C.size_t` | `uint` | Taille mémoire |

### Conversions explicites

Parfois, vous devez convertir entre types Go et types C :

```go
package main

/*
#include <stdint.h>

int32_t traiter_entier(int32_t valeur) {
    return valeur * 2;
}
*/
import "C"  
import "fmt"  

func main() {
    // Variable Go
    var nombre int32 = 42

    // Conversion Go → C
    resultat := C.traiter_entier(C.int32_t(nombre))

    // Conversion C → Go
    nombreGo := int32(resultat)

    fmt.Printf("Résultat : %d\n", nombreGo)
}
```

**Bonnes pratiques :**
- Utilisez les types `stdint.h` pour plus de clarté (`int32_t`, `uint64_t`, etc.)
- Convertissez explicitement quand il y a ambiguïté
- Attention aux différences de taille entre architectures (32 vs 64 bits)

---

## Travailler avec les chaînes de caractères

Les chaînes sont un des aspects les plus délicats de l'interopérabilité C/Go car elles sont gérées très différemment.

### Go String → C String

```go
package main

/*
#include <stdio.h>
#include <string.h>

void afficher_chaine(const char *chaine) {
    printf("C reçoit : %s\n", chaine);
    printf("Longueur : %zu\n", strlen(chaine));
}
*/
import "C"  
import "unsafe"

func main() {
    // String Go
    message := "Bonjour depuis Go!"

    // Conversion en C string
    cMessage := C.CString(message)

    // IMPORTANT : Libérer la mémoire allouée par CString
    defer C.free(unsafe.Pointer(cMessage))

    // Appeler la fonction C
    C.afficher_chaine(cMessage)
}
```

**Points critiques :**

1. **`C.CString()`** :
   - Convertit une `string` Go en `*C.char`
   - **Alloue de la mémoire** avec `malloc`
   - La chaîne est copiée (pas de référence partagée)

2. **`defer C.free()`** :
   - **OBLIGATOIRE** pour éviter les fuites mémoire
   - Utilise `unsafe.Pointer` pour le casting
   - `defer` assure la libération même en cas d'erreur

3. **Import `unsafe`** :
   - Nécessaire pour `unsafe.Pointer`
   - À ajouter : `import "unsafe"`

### C String → Go String

```go
package main

/*
#include <stdlib.h>
#include <string.h>

char* obtenir_message() {
    char *message = (char*)malloc(50);
    strcpy(message, "Message depuis C");
    return message;
}

const char* obtenir_message_const() {
    return "Message constant depuis C";
}
*/
import "C"  
import (  
    "fmt"
    "unsafe"
)

func main() {
    // Cas 1 : C string allouée dynamiquement
    cMessage := C.obtenir_message()
    goMessage := C.GoString(cMessage)
    C.free(unsafe.Pointer(cMessage))  // Libérer la mémoire C
    fmt.Println("Message 1 :", goMessage)

    // Cas 2 : C string constante (littérale)
    cMessageConst := C.obtenir_message_const()
    goMessageConst := C.GoString(cMessageConst)
    // Pas de free() ici : c'est une constante statique
    fmt.Println("Message 2 :", goMessageConst)
}
```

**Fonctions de conversion :**

- **`C.GoString(*C.char)`** : Convertit en `string` Go (copie)
- **`C.GoStringN(*C.char, C.int)`** : Idem avec longueur explicite
- **`C.GoBytes(unsafe.Pointer, C.int)`** : Convertit en `[]byte`

**Règle de gestion mémoire :**
- Si C alloue avec `malloc` → Go doit `free`
- Si C retourne une constante statique → Pas de `free`
- Si Go alloue avec `CString` → Go doit `free`

---

## Travailler avec les tableaux

### Passer un tableau Go à C

```go
package main

/*
#include <stddef.h>

int somme_tableau(int *tableau, size_t taille) {
    int somme = 0;
    for (size_t i = 0; i < taille; i++) {
        somme += tableau[i];
    }
    return somme;
}

void doubler_valeurs(int *tableau, size_t taille) {
    for (size_t i = 0; i < taille; i++) {
        tableau[i] *= 2;
    }
}
*/
import "C"  
import (  
    "fmt"
    "unsafe"
)

func main() {
    // Slice Go
    nombres := []int32{10, 20, 30, 40, 50}

    // Obtenir un pointeur vers le premier élément
    pointeur := (*C.int)(unsafe.Pointer(&nombres[0]))
    taille := C.size_t(len(nombres))

    // Calcul de la somme
    somme := C.somme_tableau(pointeur, taille)
    fmt.Printf("Somme : %d\n", somme)

    // Modification en place
    C.doubler_valeurs(pointeur, taille)
    fmt.Printf("Après doublement : %v\n", nombres)
}
```

**Sortie :**
```
Somme : 150  
Après doublement : [20 40 60 80 100]  
```

**Explication :**

1. **`&nombres[0]`** : Adresse du premier élément du slice
2. **`unsafe.Pointer()`** : Conversion en pointeur générique
3. **`(*C.int)()`** : Cast vers le type C attendu
4. Les modifications C sont visibles en Go car même mémoire

**⚠️ Attention :**
- Le slice Go ne doit pas être réalloué pendant l'utilisation en C
- Utiliser des types de taille fixe (`int32`, pas `int`) pour la portabilité

### Retourner un tableau de C vers Go

```go
package main

/*
#include <stdlib.h>

int* creer_tableau(int taille) {
    int *tableau = (int*)malloc(taille * sizeof(int));
    for (int i = 0; i < taille; i++) {
        tableau[i] = i * i;  // Carrés : 0, 1, 4, 9, 16...
    }
    return tableau;
}
*/
import "C"  
import (  
    "fmt"
    "unsafe"
)

func main() {
    taille := 10

    // Créer le tableau en C
    cTableau := C.creer_tableau(C.int(taille))
    defer C.free(unsafe.Pointer(cTableau))

    // Convertir en slice Go
    // Attention : technique avancée avec unsafe
    longueur := taille
    goTableau := (*[1 << 30]C.int)(unsafe.Pointer(cTableau))[:longueur:longueur]

    // Utilisation
    fmt.Println("Carrés :", goTableau)
}
```

**Technique de conversion détaillée :**

```go
// 1. Cast en tableau C de taille maximale
(*[1 << 30]C.int)(unsafe.Pointer(cTableau))

// 2. Slice avec la vraie longueur
[:longueur:longueur]
//   ^         ^
//   |         capacité
//   longueur
```

**⚠️ Avertissement :** Cette technique utilise `unsafe` de manière avancée. À utiliser avec précaution !

---

## Structures C en Go

### Définir et utiliser une structure

```go
package main

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char nom[50];
    int age;
    double salaire;
} Personne;

void initialiser_personne(Personne *p, const char *nom, int age, double salaire) {
    strncpy(p->nom, nom, 49);
    p->nom[49] = '\0';
    p->age = age;
    p->salaire = salaire;
}

void afficher_personne(const Personne *p) {
    printf("Nom: %s, Age: %d, Salaire: %.2f€\n", p->nom, p->age, p->salaire);
}

void augmenter_salaire(Personne *p, double pourcentage) {
    p->salaire *= (1.0 + pourcentage / 100.0);
}
*/
import "C"  
import (  
    "fmt"
    "unsafe"
)

func main() {
    // Allocation d'une structure C
    var personne C.Personne

    // Initialisation via fonction C
    nom := C.CString("Alice Dupont")
    defer C.free(unsafe.Pointer(nom))

    C.initialiser_personne(&personne, nom, 30, 45000.0)

    // Affichage
    C.afficher_personne(&personne)

    // Modification
    C.augmenter_salaire(&personne, 10.0)

    // Accès aux champs depuis Go
    fmt.Printf("Nouveau salaire (lu depuis Go) : %.2f€\n", float64(personne.salaire))
}
```

**Sortie :**
```
Nom: Alice Dupont, Age: 30, Salaire: 45000.00€  
Nouveau salaire (lu depuis Go) : 49500.00€  
```

**Accès aux champs :**

```go
// Lecture
age := personne.age  
salaire := float64(personne.salaire)  

// Écriture
personne.age = 31  
personne.salaire = C.double(50000.0)  

// Chaînes fixes : tableau de caractères
// Accès avec C.GoString
nomGo := C.GoString(&personne.nom[0])
```

### Structures avec pointeurs

```go
package main

/*
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *nom;      // Pointeur vers chaîne allouée dynamiquement
    int id;
} Element;

Element* creer_element(int id, const char *nom) {
    Element *e = (Element*)malloc(sizeof(Element));
    e->id = id;
    e->nom = strdup(nom);  // Duplique la chaîne
    return e;
}

void liberer_element(Element *e) {
    if (e) {
        free(e->nom);
        free(e);
    }
}
*/
import "C"  
import (  
    "fmt"
    "unsafe"
)

func main() {
    nom := C.CString("Element-A")
    defer C.free(unsafe.Pointer(nom))

    // Créer un élément
    element := C.creer_element(42, nom)
    defer C.liberer_element(element)

    // Accéder aux champs
    fmt.Printf("ID: %d\n", element.id)
    fmt.Printf("Nom: %s\n", C.GoString(element.nom))
}
```

**Gestion mémoire avec structures :**
- Structures allouées par C → libérées par C
- Champs pointeurs → attention aux fuites
- Toujours fournir une fonction de nettoyage C

---

## Utiliser des bibliothèques C externes

Jusqu'ici nous avons écrit du C inline. Utilisons maintenant une vraie bibliothèque.

### Exemple avec une bibliothèque compilée séparément

**1. Créer la bibliothèque C**

**Fichier `mathlib.h` :**
```c
#ifndef MATHLIB_H
#define MATHLIB_H

int factorielle(int n);  
double racine_cubique(double x);  

#endif
```

**Fichier `mathlib.c` :**
```c
#include "mathlib.h"
#include <math.h>

int factorielle(int n) {
    if (n <= 1) return 1;
    return n * factorielle(n - 1);
}

double racine_cubique(double x) {
    return cbrt(x);  // Fonction de <math.h>
}
```

**Compilation :**
```bash
# Créer une bibliothèque statique
gcc -c mathlib.c -o mathlib.o  
ar rcs libmathlib.a mathlib.o  

# Ou une bibliothèque dynamique
gcc -shared -o libmathlib.so -fPIC mathlib.c -lm
```

**2. Utiliser depuis Go**

**Fichier `main.go` :**
```go
package main

/*
#cgo CFLAGS: -I.
#cgo LDFLAGS: -L. -lmathlib -lm

#include "mathlib.h"
*/
import "C"  
import "fmt"  

func main() {
    // Utiliser la bibliothèque
    fact := C.factorielle(5)
    fmt.Printf("5! = %d\n", fact)

    racine := C.racine_cubique(27.0)
    fmt.Printf("∛27 = %f\n", racine)
}
```

**Explication des directives cgo :**

```go
#cgo CFLAGS: -I.
```
- **CFLAGS** : Options pour le compilateur C
- **-I.** : Chercher les headers dans le répertoire courant

```go
#cgo LDFLAGS: -L. -lmathlib -lm
```
- **LDFLAGS** : Options pour l'éditeur de liens (linker)
- **-L.** : Chercher les bibliothèques dans le répertoire courant
- **-lmathlib** : Lier avec libmathlib
- **-lm** : Lier avec libm (bibliothèque mathématique standard)

**3. Compilation et exécution**

```bash
# Avec bibliothèque statique
go build -o main main.go
./main

# Avec bibliothèque dynamique (spécifier LD_LIBRARY_PATH)
LD_LIBRARY_PATH=. go run main.go
```

### Directives cgo avancées

```go
/*
#cgo CFLAGS: -I/usr/local/include -Wall
#cgo LDFLAGS: -L/usr/local/lib -lsqlite3

// Directives conditionnelles selon l'OS
#cgo linux LDFLAGS: -ldl
#cgo darwin LDFLAGS: -framework CoreFoundation

// Selon l'architecture
#cgo amd64 CFLAGS: -DARCH_64
#cgo 386 CFLAGS: -DARCH_32
*/
```

**Tags de compilation :**
- `linux`, `darwin`, `windows` : Système d'exploitation
- `amd64`, `386`, `arm`, `arm64` : Architecture
- Permet d'adapter la compilation selon la plateforme

---

## Callbacks : Appeler Go depuis C

Parfois, C a besoin d'appeler une fonction Go (callback). C'est possible mais nécessite une technique spéciale.

### Exemple : Fonction de tri avec comparateur personnalisé

```go
package main

/*
#include <stdlib.h>

// Typedef pour le type de fonction de comparaison
typedef int (*comparateur_t)(void*, void*);

// Fonction C qui trie avec un comparateur
void tri_avec_comparateur(int *tableau, int taille, comparateur_t cmp);

void tri_avec_comparateur(int *tableau, int taille, comparateur_t cmp) {
    // Tri à bulles simple (pas efficace, mais pédagogique)
    for (int i = 0; i < taille - 1; i++) {
        for (int j = 0; j < taille - i - 1; j++) {
            if (cmp(&tableau[j], &tableau[j+1]) > 0) {
                int temp = tableau[j];
                tableau[j] = tableau[j+1];
                tableau[j+1] = temp;
            }
        }
    }
}
*/
import "C"  
import (  
    "fmt"
    "unsafe"
)

// Comparateur Go qui sera appelé depuis C
//export comparerEntiers
func comparerEntiers(a, b unsafe.Pointer) C.int {
    valA := *(*C.int)(a)
    valB := *(*C.int)(b)

    if valA < valB {
        return -1
    } else if valA > valB {
        return 1
    }
    return 0
}

func main() {
    nombres := []C.int{64, 34, 25, 12, 22, 11, 90}

    fmt.Println("Avant tri :", nombres)

    // Appeler la fonction C avec notre callback Go
    C.tri_avec_comparateur(
        &nombres[0],
        C.int(len(nombres)),
        C.comparateur_t(C.comparerEntiers),
    )

    fmt.Println("Après tri :", nombres)
}
```

**Points clés des callbacks :**

1. **`//export nomFonction`** :
   - Directive spéciale pour exposer une fonction Go à C
   - Doit être **avant** la définition de la fonction
   - Pas d'espace entre `//` et `export`

2. **Signature de la fonction Go** :
   - Doit correspondre au typedef C
   - Utilise `unsafe.Pointer` pour les pointeurs génériques
   - Retourne un type C (`C.int`, etc.)

3. **Limitations** :
   - Pas de closures (la fonction doit être au niveau package)
   - Performance : chaque appel de C → Go a un coût
   - Complexité accrue

**⚠️ Conseil :** Limitez l'utilisation des callbacks Go→C. Privilégiez les callbacks C→C quand possible.

---

## Gestion des erreurs

### Codes d'erreur C

La plupart des fonctions C retournent des codes d'erreur. Voici comment les gérer proprement en Go.

```go
package main

/*
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

int ouvrir_fichier_simulation(const char *nom) {
    if (strcmp(nom, "invalide.txt") == 0) {
        errno = ENOENT;  // File not found
        return -1;
    }
    return 0;  // Succès
}
*/
import "C"  
import (
    "fmt"
    "unsafe"
)

func ouvrirFichier(nom string) error {
    cNom := C.CString(nom)
    defer C.free(unsafe.Pointer(cNom))

    resultat, err := C.ouvrir_fichier_simulation(cNom)

    if resultat == -1 {
        // Récupérer errno via le retour à deux valeurs de cgo
        return fmt.Errorf("erreur lors de l'ouverture : %v", err)
    }

    return nil
}

func main() {
    // Cas d'erreur
    if err := ouvrirFichier("invalide.txt"); err != nil {
        fmt.Println("Erreur :", err)
    }

    // Cas de succès
    if err := ouvrirFichier("valide.txt"); err != nil {
        fmt.Println("Erreur :", err)
    } else {
        fmt.Println("Fichier ouvert avec succès")
    }
}
```

**Gestion idiomatique Go :**

```go
func fonctionQuiAppelleC() (resultat int, err error) {
    cParam := C.CString("param")
    defer C.free(unsafe.Pointer(cParam))

    ret := C.fonction_c(cParam)

    if ret < 0 {
        return 0, fmt.Errorf("erreur C : code %d", ret)
    }

    return int(ret), nil
}
```

**Wrapping de bibliothèques C :**

Créez une couche Go qui transforme les codes d'erreur C en `error` Go :

```go
type MaBibliotheque struct {
    // état interne
}

func (b *MaBibliotheque) Fonction() error {
    ret := C.fonction_c_unsafe()
    if ret != C.SUCCESS {
        return ErreurDeFonction
    }
    return nil
}

var (
    ErreurDeFonction = errors.New("la fonction a échoué")
    ErreurAutre      = errors.New("autre erreur")
)
```

---

## Build et compilation

### Compilation simple

```bash
# Compilation normale
go build

# Exécution directe
go run main.go
```

### Cross-compilation avec cgo

**⚠️ Problème majeur :** cgo désactive la cross-compilation facile de Go !

```bash
# Sans cgo : facile
GOOS=linux GOARCH=amd64 go build    # ✅ Fonctionne

# Avec cgo : complexe
GOOS=linux GOARCH=amd64 go build    # ❌ Échec sans cross-compiler C
```

**Solution : Utiliser un cross-compiler C**

```bash
# Installer un cross-compiler (exemple pour ARM)
sudo apt-get install gcc-arm-linux-gnueabihf

# Compiler pour ARM
CGO_ENABLED=1 \  
GOOS=linux \  
GOARCH=arm \  
CC=arm-linux-gnueabihf-gcc \  
go build  
```

### Build tags pour gérer cgo

```go
// +build cgo

package main

// Code utilisant cgo
```

```go
// +build !cgo

package main

// Implémentation alternative sans cgo
```

**Utilisation :**
```bash
# Avec cgo
go build -tags cgo

# Sans cgo
CGO_ENABLED=0 go build
```

### Variables d'environnement importantes

| Variable | Description | Exemple |
|----------|-------------|---------|
| `CGO_ENABLED` | Active/désactive cgo | `CGO_ENABLED=0` |
| `CC` | Compilateur C à utiliser | `CC=clang` |
| `CXX` | Compilateur C++ | `CXX=clang++` |
| `CGO_CFLAGS` | Flags pour le compilateur C | `CGO_CFLAGS="-O3"` |
| `CGO_LDFLAGS` | Flags pour le linker | `CGO_LDFLAGS="-lm"` |

---

## Performance et optimisation

### Le coût de cgo

**Fait important :** Chaque appel de Go vers C (et vice-versa) a un coût !

**Benchmark illustratif :**
```go
package main

import "testing"

/*
int simple_addition_c(int a, int b) {
    return a + b;
}
*/
import "C"

func additionGo(a, b int) int {
    return a + b
}

func BenchmarkAdditionGo(b *testing.B) {
    for i := 0; i < b.N; i++ {
        _ = additionGo(5, 7)
    }
}

func BenchmarkAdditionC(b *testing.B) {
    for i := 0; i < b.N; i++ {
        _ = C.simple_addition_c(5, 7)
    }
}
```

**Résultats typiques :**
```
BenchmarkAdditionGo-8    1000000000    0.3 ns/op  
BenchmarkAdditionC-8      30000000     40 ns/op  
```

**Conclusion :** L'appel cgo est ~100x plus lent qu'un appel Go natif !

### Quand utiliser cgo pour la performance ?

**✅ Utilisez cgo quand :**
- Les appels sont **peu fréquents**
- Le travail en C est **conséquent** (traitement de gros volumes)
- Vous bénéficiez d'optimisations SIMD/assembleur spécifiques
- Vous utilisez une bibliothèque C hautement optimisée

**❌ N'utilisez PAS cgo si :**
- Vous faites des millions d'appels de petites fonctions
- Le code Go équivalent est déjà performant
- Vous pouvez faire du batch (traiter beaucoup de données en un seul appel)

**Optimisation : Batching**

```go
// ❌ Lent : millions d'appels cgo
for i := 0; i < 1_000_000; i++ {
    resultat := C.traiter_un_element(data[i])
    // ...
}

// ✅ Rapide : un seul appel cgo
resultat := C.traiter_tableau(&data[0], len(data))
```

---

## Bonnes pratiques

### 1. Minimiser la surface d'interaction

**Créez une couche d'abstraction Go :**

```go
// Package mathc encapsule les appels à libmath.c
package mathc

/*
#include "mathlib.h"
*/
import "C"

// Factorielle calcule n!
func Factorielle(n int) int {
    return int(C.factorielle(C.int(n)))
}

// Les utilisateurs de ce package n'ont jamais à toucher à cgo !
```

**Avantages :**
- Interface Go propre et idiomatique
- Facilite les tests (mocking possible)
- Isole la complexité cgo

### 2. Gestion rigoureuse de la mémoire

**Checklist :**
- ✅ Tout `C.CString()` a un `C.free()` correspondant
- ✅ Utiliser `defer` pour garantir la libération
- ✅ Documenter qui est responsable de libérer quoi
- ✅ Tests avec `valgrind` ou sanitizers

**Pattern de wrapper sûr :**

```go
type Ressource struct {
    ptr *C.ressource_t
}

func NouvelleRessource() (*Ressource, error) {
    ptr := C.creer_ressource()
    if ptr == nil {
        return nil, errors.New("échec création ressource")
    }

    r := &Ressource{ptr: ptr}

    // Finalizer pour libération automatique (backup)
    runtime.SetFinalizer(r, func(r *Ressource) {
        r.Close()
    })

    return r, nil
}

func (r *Ressource) Close() error {
    if r.ptr != nil {
        C.liberer_ressource(r.ptr)
        r.ptr = nil
    }
    return nil
}
```

### 3. Documentation

Documentez l'usage de cgo dans votre package :

```go
// Package native fournit des bindings pour libawesome.
//
// Ce package utilise cgo et nécessite :
//   - libawesome-dev installé
//   - Un compilateur C (gcc ou clang)
//
// Pour désactiver cgo et utiliser une implémentation pure Go :
//   CGO_ENABLED=0 go build
package native
```

### 4. Tests

**Testez avec et sans cgo :**

```bash
# Avec cgo
go test

# Sans cgo (implémentation fallback)
CGO_ENABLED=0 go test
```

**Tests de fuite mémoire :**

```bash
# Avec Valgrind
go test -c  
valgrind --leak-check=full ./package.test  

# Avec Address Sanitizer
CGO_CFLAGS="-fsanitize=address" go test
```

### 5. Build reproductibles

**Utilisez des modules Go :**

```bash
go mod init monprojet  
go mod tidy  
```

**Vendoring des dépendances C :**

```
monprojet/
├── go.mod
├── main.go
└── vendor/
    └── libawesome/
        ├── awesome.h
        ├── awesome.c
        └── build.sh
```

---

## Pièges courants et solutions

### Piège 1 : Oublier de free

```go
// ❌ FUITE MÉMOIRE
func mauvais() {
    s := C.CString("hello")
    C.fonction(s)
    // Oups, pas de free !
}

// ✅ CORRECT
func bon() {
    s := C.CString("hello")
    defer C.free(unsafe.Pointer(s))
    C.fonction(s)
}
```

### Piège 2 : Passer des slices qui peuvent être réalloués

```go
// ❌ DANGEREUX
func dangereux() {
    slice := make([]int32, 10)
    ptr := (*C.int)(unsafe.Pointer(&slice[0]))

    // Si le slice est réalloué ailleurs, ptr devient invalide !
    slice = append(slice, 999)  // Réallocation possible

    C.utiliser_pointeur(ptr)  // CRASH potentiel
}

// ✅ SÛR
func sur() {
    slice := make([]int32, 10)
    ptr := (*C.int)(unsafe.Pointer(&slice[0]))

    C.utiliser_pointeur(ptr)

    // Réallocations seulement APRÈS l'usage en C
    slice = append(slice, 999)
}
```

### Piège 3 : Retenir des pointeurs C dans des goroutines

```go
// ❌ DANGEREUX
func dangereux() {
    cString := C.CString("hello")

    go func() {
        time.Sleep(time.Second)
        C.fonction(cString)  // cString peut avoir été free !
        C.free(unsafe.Pointer(cString))
    }()
}

// ✅ CORRECT
func correct() {
    cString := C.CString("hello")

    go func(s *C.char) {
        defer C.free(unsafe.Pointer(s))
        time.Sleep(time.Second)
        C.fonction(s)
    }(cString)
}
```

### Piège 4 : Types de taille variable

```go
// ❌ RISQUE : int peut être 32 ou 64 bits
func risque() {
    var n int = 42
    C.fonction(C.int(n))  // int Go vs int C : tailles différentes ?
}

// ✅ EXPLICITE
func explicite() {
    var n int32 = 42
    C.fonction(C.int32_t(n))  // Taille garantie
}
```

### Piège 5 : Import "C" mal placé

```go
// ❌ NE COMPILE PAS
import "C"

/*
#include <stdio.h>
*/

// ✅ CORRECT
/*
#include <stdio.h>
*/
import "C"
```

**Règle :** Le commentaire C doit être **immédiatement** avant `import "C"`.

---

## Comparaison avec d'autres approches

### cgo vs Pure Go

| Aspect | cgo | Pure Go |
|--------|-----|---------|
| **Performance** | Variable (overhead appels) | Excellente |
| **Portabilité** | Nécessite compilateur C | Parfaite |
| **Cross-compilation** | Complexe | Triviale |
| **Taille binaire** | Plus grosse | Optimale |
| **Maintenabilité** | Plus complexe | Simple |
| **Écosystème** | Accès bibliothèques C | Bibliothèques Go |

**Recommandation :** Privilégiez Go pur quand possible. Utilisez cgo uniquement quand nécessaire.

### cgo vs Bindings Go purs

De nombreuses bibliothèques C ont des **réimplémentations en Go pur** :

| Bibliothèque C | Alternative Go pure |
|----------------|---------------------|
| SQLite (cgo) | modernc.org/sqlite |
| libgit2 (cgo) | go-git |
| libpcap (cgo) | gopacket (partiel) |

**Avantages des bindings purs Go :**
- ✅ Pas de dépendance C
- ✅ Cross-compilation facile
- ✅ Déploiement simplifié
- ❌ Parfois moins de fonctionnalités
- ❌ Peut être légèrement moins performant

---

## Cas d'usage réel : Wrapper SQLite

Exemple concret : créer un wrapper minimal pour SQLite.

```go
package main

/*
#cgo LDFLAGS: -lsqlite3
#include <sqlite3.h>
#include <stdlib.h>
*/
import "C"  
import (  
    "fmt"
    "unsafe"
)

type DB struct {
    conn *C.sqlite3
}

// Ouvrir une base de données
func Ouvrir(nomFichier string) (*DB, error) {
    cNom := C.CString(nomFichier)
    defer C.free(unsafe.Pointer(cNom))

    var db *C.sqlite3
    resultat := C.sqlite3_open(cNom, &db)

    if resultat != C.SQLITE_OK {
        return nil, fmt.Errorf("erreur ouverture : %s",
            C.GoString(C.sqlite3_errmsg(db)))
    }

    return &DB{conn: db}, nil
}

// Fermer la base de données
func (db *DB) Fermer() error {
    if db.conn != nil {
        C.sqlite3_close(db.conn)
        db.conn = nil
    }
    return nil
}

// Exécuter une requête simple
func (db *DB) Exec(sql string) error {
    cSQL := C.CString(sql)
    defer C.free(unsafe.Pointer(cSQL))

    var errMsg *C.char
    resultat := C.sqlite3_exec(db.conn, cSQL, nil, nil, &errMsg)

    if resultat != C.SQLITE_OK {
        defer C.sqlite3_free(unsafe.Pointer(errMsg))
        return fmt.Errorf("erreur SQL : %s", C.GoString(errMsg))
    }

    return nil
}

func main() {
    // Utilisation
    db, err := Ouvrir(":memory:")
    if err != nil {
        panic(err)
    }
    defer db.Fermer()

    // Créer une table
    err = db.Exec(`
        CREATE TABLE users (
            id INTEGER PRIMARY KEY,
            nom TEXT
        )
    `)
    if err != nil {
        panic(err)
    }

    // Insérer des données
    err = db.Exec("INSERT INTO users (nom) VALUES ('Alice')")
    if err != nil {
        panic(err)
    }

    fmt.Println("Base de données SQLite utilisée avec succès !")
}
```

**Points techniques :**
- Utilise la bibliothèque système SQLite (`-lsqlite3`)
- Wrapper orienté objet Go (`type DB struct`)
- Gestion d'erreurs idiomatique Go
- Libération automatique avec `defer`

---

## Outils et debugging

### Vérifier l'utilisation de cgo

```bash
# Voir si un binaire utilise cgo
go version -m monprogramme

# Sortie contient : build -compiler=gc (pas de cgo) ou
# build -compiler=gccgo (cgo utilisé)
```

### Debugging avec GDB

```bash
# Compiler avec symboles de debug
go build -gcflags="all=-N -l"

# Lancer sous GDB
gdb ./monprogramme

# Dans GDB
(gdb) break main.main
(gdb) run
(gdb) step
```

### Inspection des symboles

```bash
# Voir les symboles C dans le binaire
nm monprogramme | grep " C "

# Voir les dépendances dynamiques
ldd monprogramme
```

### Variables d'environnement de debug

```bash
# Afficher les commandes cgo exécutées
CGO_CFLAGS="-v" go build

# Garder les fichiers temporaires
go build -work -x
```

---

## Limites et considérations

### Limitations de cgo

1. **Cross-compilation difficile**
   - Nécessite un cross-compiler C
   - Configuration complexe

2. **Taille du binaire**
   - Binaires plus gros (CGO runtime)
   - Dépendances dynamiques possibles

3. **Performance des appels**
   - Overhead de 40-100ns par appel
   - Pas adapté aux appels fréquents

4. **Garbage Collector**
   - Le GC Go ne gère pas la mémoire C
   - Attention aux références croisées

5. **Callbacks limités**
   - Pas de closures
   - Performance impactée

### Alternatives à considérer

**1. Pure Go**
- Réimplémenter en Go si possible
- Rechercher des bibliothèques Go équivalentes

**2. Appels système directs**
- Package `syscall` pour les APIs kernel
- Package `unix` pour des APIs POSIX

**3. Communication inter-processus**
- Lancer un processus C séparé
- Communiquer via pipes, sockets ou gRPC

**4. Plugins Go**
- Package `plugin` (Linux/macOS)
- Charger du code Go dynamiquement

---

## Récapitulatif

### Points clés à retenir

1. **cgo permet d'appeler du C depuis Go facilement**
   - Syntaxe relativement simple
   - Intégré à la toolchain Go standard
   - Puissant mais avec des compromis

2. **Syntaxe essentielle**
   ```go
   /*
   #include <header.h>
   void fonction_c() { ... }
   */
   import "C"

   func main() {
       C.fonction_c()
   }
   ```

3. **Gestion mémoire critique**
   - `C.CString()` → toujours `C.free()`
   - Utiliser `defer` systématiquement
   - Attention à la durée de vie des objets

4. **Performance**
   - Overhead par appel : ~40-100ns
   - Batching recommandé
   - Mesurer avant d'optimiser

5. **Build et déploiement**
   - Cross-compilation complexe
   - Binaires plus gros
   - Dépendances C à gérer

6. **Quand utiliser cgo**
   - ✅ Bibliothèques C matures sans équivalent Go
   - ✅ APIs système bas niveau
   - ✅ Performance critique (calculs lourds)
   - ❌ Appels fréquents de petites fonctions
   - ❌ Quand une alternative Go pure existe

### Checklist avant d'utiliser cgo

- [ ] Existe-t-il une bibliothèque Go pure équivalente ?
- [ ] Le gain de performance justifie-t-il la complexité ?
- [ ] La cross-compilation est-elle un problème ?
- [ ] L'équipe est-elle à l'aise avec C ?
- [ ] La gestion mémoire est-elle bien comprise ?
- [ ] Les tests (y compris sanitizers) sont-ils en place ?

### Le mot de la fin

cgo est un outil puissant qui ouvre les portes de l'écosystème C à Go. Cependant, comme tout outil puissant, il doit être utilisé avec discernement. La philosophie Go privilégie la simplicité et la maintenabilité : n'utilisez cgo que lorsque les bénéfices dépassent clairement les coûts.

**Bon développement ! 🚀**

---


⏭️ [Interfaçage avec Rust (FFI)](/28-interoperabilite/03-interfacage-rust.md)
