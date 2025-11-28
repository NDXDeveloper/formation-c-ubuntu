🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 28.3 Interfaçage avec Rust (FFI)

## Introduction

### Pourquoi interfacer C et Rust ?

Rust est un langage moderne qui vise à offrir la performance de C/C++ avec des garanties de sécurité mémoire à la compilation. L'interopérabilité avec C est un pilier fondamental de Rust.

**Cas d'usage typiques :**
- 🔄 **Migration progressive** : Remplacer du code C legacy par du Rust moderne
- 📚 **Réutilisation de bibliothèques** : Accéder à l'écosystème C existant (SQLite, OpenSSL, etc.)
- 🚀 **Modernisation** : Ajouter de nouvelles fonctionnalités Rust à un projet C
- 🔌 **Plugins et extensions** : Créer des modules Rust pour des applications C
- ⚡ **Performance critique** : Réécrire des parties sensibles en Rust tout en gardant C

**Exemple concret :** Vous maintenez une application C ancienne et voulez ajouter de nouvelles fonctionnalités. Plutôt que d'ajouter du code C (risques de bugs mémoire), vous écrivez les nouvelles parties en Rust et les intégrez via FFI.

### Rust et C : Une compatibilité par design

Rust a été conçu dès le départ pour une excellente interopérabilité avec C :
- ✅ **ABI compatible** : Rust peut utiliser la même ABI (Application Binary Interface) que C
- ✅ **FFI intégré** : Support natif sans bibliothèque tierce
- ✅ **Zero-cost abstraction** : Pas de surcoût à l'exécution
- ✅ **Outils modernes** : `bindgen` génère automatiquement les bindings
- ✅ **Sécurité** : Le compilateur vous aide à éviter les erreurs

---

## FFI : Foreign Function Interface

### Qu'est-ce que FFI ?

FFI (Foreign Function Interface) est le mécanisme qui permet à Rust d'interagir avec du code écrit dans d'autres langages, principalement C.

**Deux directions possibles :**
1. **Rust appelle C** : Utiliser des bibliothèques C depuis Rust
2. **C appelle Rust** : Exposer des fonctions Rust à du code C

**Le mot clé magique : `extern`**

```rust
// Déclarer des fonctions C que Rust va appeler
extern "C" {
    fn fonction_c(arg: i32) -> i32;
}

// Déclarer des fonctions Rust que C peut appeler
#[no_mangle]
pub extern "C" fn fonction_rust(arg: i32) -> i32 {
    arg * 2
}
```

---

## Appeler du C depuis Rust

### Exemple 1 : Fonction C simple

Commençons par l'exemple le plus basique : appeler une fonction C depuis Rust.

**Fichier C : `mathlib.c`**
```c
int addition(int a, int b) {
    return a + b;
}

int multiplication(int a, int b) {
    return a * b;
}
```

**Compiler en bibliothèque statique :**
```bash
gcc -c mathlib.c -o mathlib.o
ar rcs libmathlib.a mathlib.o
```

**Fichier Rust : `src/main.rs`**
```rust
// Déclarer les fonctions C
extern "C" {
    fn addition(a: i32, b: i32) -> i32;
    fn multiplication(a: i32, b: i32) -> i32;
}

fn main() {
    unsafe {
        let somme = addition(5, 7);
        let produit = multiplication(3, 4);

        println!("5 + 7 = {}", somme);
        println!("3 * 4 = {}", produit);
    }
}
```

**Fichier de build : `build.rs`**
```rust
fn main() {
    // Indiquer où trouver la bibliothèque
    println!("cargo:rustc-link-search=native=.");
    println!("cargo:rustc-link-lib=static=mathlib");
}
```

**Structure du projet :**
```
mon_projet/
├── Cargo.toml
├── build.rs
├── libmathlib.a
└── src/
    └── main.rs
```

**Compilation et exécution :**
```bash
cargo build
cargo run
```

### Anatomie du code FFI

**1. Le bloc `extern "C"`**
```rust
extern "C" {
    fn fonction_c(arg: i32) -> i32;
}
```

- **`extern "C"`** : Utilise la convention d'appel C (ABI C)
- Déclare les signatures des fonctions C
- Comme un fichier header en C

**2. Le bloc `unsafe`**
```rust
unsafe {
    let resultat = addition(5, 7);
}
```

- **Obligatoire** pour appeler des fonctions externes
- Rust ne peut pas vérifier la sécurité du code C
- C'est à vous de garantir que l'appel est sûr

**3. Le fichier `build.rs`**
```rust
println!("cargo:rustc-link-search=native=.");
println!("cargo:rustc-link-lib=static=mathlib");
```

- Script exécuté avant la compilation
- Configure le linker
- `rustc-link-search` : où chercher les bibliothèques
- `rustc-link-lib` : quelle bibliothèque lier

### Exemple 2 : Utiliser la bibliothèque standard C

```rust
// Utiliser des fonctions de libc
extern "C" {
    fn abs(n: i32) -> i32;
    fn sqrt(n: f64) -> f64;
    fn strlen(s: *const u8) -> usize;
}

fn main() {
    unsafe {
        println!("abs(-42) = {}", abs(-42));
        println!("sqrt(16.0) = {}", sqrt(16.0));

        let texte = b"Hello\0";  // Chaîne C (null-terminated)
        println!("strlen = {}", strlen(texte.as_ptr()));
    }
}
```

**Pas besoin de `build.rs`** : La libc est déjà liée par défaut.

**Note importante :** En pratique, utilisez plutôt la crate `libc` qui fournit des bindings sûrs :

```rust
use libc::{abs, sqrt, strlen};

fn main() {
    unsafe {
        println!("abs(-42) = {}", abs(-42));
    }
}
```

---

## Appeler du Rust depuis C

### Exemple 1 : Fonction Rust pour C

Créons une fonction Rust que du code C pourra appeler.

**Fichier Rust : `src/lib.rs`**
```rust
// Exposer une fonction pour C
#[no_mangle]
pub extern "C" fn factorielle_rust(n: u32) -> u64 {
    if n <= 1 {
        1
    } else {
        n as u64 * factorielle_rust(n - 1)
    }
}

#[no_mangle]
pub extern "C" fn fibonacci_rust(n: u32) -> u64 {
    match n {
        0 => 0,
        1 => 1,
        _ => fibonacci_rust(n - 1) + fibonacci_rust(n - 2),
    }
}
```

**Attributs essentiels :**

1. **`#[no_mangle]`** :
   - Empêche le "name mangling" du compilateur
   - La fonction garde son nom exact (`factorielle_rust`)
   - **Obligatoire** pour que C puisse trouver la fonction

2. **`pub`** :
   - Rend la fonction publique
   - Nécessaire pour l'export

3. **`extern "C"`** :
   - Convention d'appel C
   - Compatible avec l'ABI C

**Compiler en bibliothèque dynamique :**

**`Cargo.toml`**
```toml
[package]
name = "rustlib"
version = "0.1.0"
edition = "2021"

[lib]
crate-type = ["cdylib"]  # Pour bibliothèque dynamique
# ou
# crate-type = ["staticlib"]  # Pour bibliothèque statique
```

**Compilation :**
```bash
cargo build --release
# Crée : target/release/librustlib.so (Linux)
#    ou : target/release/librustlib.dylib (macOS)
#    ou : target/release/rustlib.dll (Windows)
```

**Fichier header C : `rustlib.h`**
```c
#ifndef RUSTLIB_H
#define RUSTLIB_H

#include <stdint.h>

// Déclarer les fonctions Rust
uint64_t factorielle_rust(uint32_t n);
uint64_t fibonacci_rust(uint32_t n);

#endif
```

**Programme C : `main.c`**
```c
#include <stdio.h>
#include "rustlib.h"

int main() {
    printf("5! = %lu\n", factorielle_rust(5));
    printf("fib(10) = %lu\n", fibonacci_rust(10));

    return 0;
}
```

**Compilation et exécution :**
```bash
# Compiler le programme C
gcc main.c -o main -L./target/release -lrustlib

# Exécuter (Linux)
LD_LIBRARY_PATH=./target/release ./main

# Sortie :
# 5! = 120
# fib(10) = 55
```

---

## Types et conversions

### Correspondance des types primitifs

| Type C | Type Rust | Remarques |
|--------|-----------|-----------|
| `char` | `i8` ou `u8` | Selon signé/non signé |
| `short` | `i16` | |
| `unsigned short` | `u16` | |
| `int` | `i32` | Généralement 32 bits |
| `unsigned int` | `u32` | |
| `long` | `i64` ou `i32` | Dépend de l'architecture |
| `long long` | `i64` | |
| `size_t` | `usize` | Taille de pointeur |
| `ssize_t` | `isize` | |
| `float` | `f32` | |
| `double` | `f64` | |
| `void*` | `*mut c_void` ou `*const c_void` | Pointeur brut |
| `T*` | `*mut T` | Pointeur mutable |
| `const T*` | `*const T` | Pointeur constant |

**Conseil :** Utilisez les types `libc` pour plus de clarté :

```rust
use libc::{c_int, c_uint, c_float, c_double, size_t};

extern "C" {
    fn fonction_c(a: c_int, b: size_t) -> c_float;
}
```

### Types fixes avec `std::os::raw`

Pour une meilleure portabilité, Rust fournit des types explicites :

```rust
use std::os::raw::{c_char, c_int, c_long, c_ulong, c_void};

extern "C" {
    fn fonction(a: c_int, ptr: *mut c_void) -> c_long;
}
```

---

## Gestion des chaînes de caractères

Les chaînes sont un des aspects les plus délicats de l'interopérabilité.

### Rust String → C String

**En C :** Les chaînes sont des `char*` terminées par `\0` (null-terminated).
**En Rust :** Les `String` et `&str` ne sont PAS null-terminated et encodées en UTF-8.

```rust
use std::ffi::CString;
use std::os::raw::c_char;

extern "C" {
    fn afficher_chaine_c(s: *const c_char);
}

fn main() {
    // Créer une CString (null-terminated)
    let texte = CString::new("Bonjour depuis Rust!").unwrap();

    unsafe {
        afficher_chaine_c(texte.as_ptr());
    }

    // texte est automatiquement libéré ici
}
```

**`CString` - Points clés :**

1. **`CString::new()`** :
   - Crée une chaîne null-terminated
   - Retourne `Result` (peut échouer si la chaîne contient des `\0`)
   - Alloue de la mémoire

2. **`.as_ptr()`** :
   - Obtient un `*const c_char`
   - Pointeur valide tant que `CString` existe
   - **Ne pas stocker** ce pointeur au-delà de la durée de vie du `CString`

3. **Libération automatique** :
   - Rust gère automatiquement la mémoire
   - Pas besoin de `free()` manuel

**Exemple complet :**

**C : `strings.c`**
```c
#include <stdio.h>

void afficher_chaine_c(const char *s) {
    printf("C reçoit : %s\n", s);
}
```

**Rust :**
```rust
use std::ffi::CString;
use std::os::raw::c_char;

extern "C" {
    fn afficher_chaine_c(s: *const c_char);
}

fn main() {
    let message = CString::new("Hello from Rust!").unwrap();

    unsafe {
        afficher_chaine_c(message.as_ptr());
    }
}
```

### C String → Rust String

```rust
use std::ffi::CStr;
use std::os::raw::c_char;

extern "C" {
    fn obtenir_message_c() -> *const c_char;
}

fn main() {
    unsafe {
        // Obtenir un pointeur C
        let c_ptr = obtenir_message_c();

        // Convertir en &CStr (slice de chaîne C)
        let c_str = CStr::from_ptr(c_ptr);

        // Convertir en &str (peut échouer si pas UTF-8 valide)
        match c_str.to_str() {
            Ok(rust_str) => println!("Message: {}", rust_str),
            Err(_) => println!("Erreur : pas UTF-8 valide"),
        }

        // Si on veut une String (owned)
        let rust_string = c_str.to_string_lossy();
        println!("String: {}", rust_string);
    }
}
```

**Méthodes de conversion :**

- **`CStr::from_ptr(ptr)`** : Crée un `&CStr` depuis un `*const c_char`
- **`.to_str()`** : Convertit en `&str` (erreur si pas UTF-8)
- **`.to_string_lossy()`** : Convertit en `String` (remplace les caractères invalides)
- **`.to_bytes()`** : Obtient `&[u8]` (bytes bruts)

### Gestion de la propriété

**⚠️ CRITIQUE : Qui possède la mémoire ?**

```rust
// Cas 1 : C possède la mémoire (constante statique)
extern "C" {
    fn get_version() -> *const c_char;  // Retourne "1.0.0"
}

unsafe {
    let version = CStr::from_ptr(get_version());
    // Pas de free : C garde la propriété
}

// Cas 2 : C alloue, Rust doit libérer
extern "C" {
    fn malloc_string() -> *mut c_char;
    fn free(ptr: *mut c_void);
}

unsafe {
    let ptr = malloc_string();
    let c_str = CStr::from_ptr(ptr);
    let rust_str = c_str.to_string_lossy();

    // OBLIGATOIRE : libérer la mémoire C
    free(ptr as *mut c_void);
}

// Cas 3 : Rust alloue, C utilise temporairement
let rust_string = CString::new("temporary").unwrap();
unsafe {
    fonction_c(rust_string.as_ptr());
    // Rust libère automatiquement
}
```

---

## Travailler avec les tableaux

### Passer un slice Rust à C

```rust
use std::os::raw::c_int;

extern "C" {
    fn somme_tableau_c(tableau: *const c_int, taille: usize) -> c_int;
}

fn main() {
    let nombres = vec![10, 20, 30, 40, 50];

    unsafe {
        let somme = somme_tableau_c(nombres.as_ptr() as *const c_int, nombres.len());
        println!("Somme : {}", somme);
    }
}
```

**Fichier C : `arrays.c`**
```c
int somme_tableau_c(const int *tableau, size_t taille) {
    int somme = 0;
    for (size_t i = 0; i < taille; i++) {
        somme += tableau[i];
    }
    return somme;
}
```

**Points d'attention :**
- **`.as_ptr()`** : Obtient un pointeur brut vers les données
- Le slice **ne doit pas être modifié/déplacé** pendant l'utilisation en C
- Passer la longueur explicitement (C ne connaît pas la taille)

### Recevoir un tableau de C

```rust
use std::slice;

extern "C" {
    fn creer_tableau_c(taille: usize) -> *mut i32;
    fn liberer_tableau_c(ptr: *mut i32);
}

fn main() {
    unsafe {
        let taille = 10;
        let ptr = creer_tableau_c(taille);

        // Créer un slice Rust depuis le pointeur C
        let tableau = slice::from_raw_parts(ptr, taille);

        println!("Tableau : {:?}", tableau);

        // Libérer la mémoire C
        liberer_tableau_c(ptr);
    }
}
```

**`slice::from_raw_parts` :**
- Crée un slice depuis un pointeur et une longueur
- **Dangereux** : Aucune vérification de validité
- Le pointeur doit rester valide durant toute la durée de vie du slice

---

## Structures C en Rust

### Définir une structure compatible C

```rust
// Utiliser #[repr(C)] pour garantir la compatibilité C
#[repr(C)]
pub struct Personne {
    pub age: i32,
    pub salaire: f64,
}

#[no_mangle]
pub extern "C" fn creer_personne(age: i32, salaire: f64) -> Personne {
    Personne { age, salaire }
}

#[no_mangle]
pub extern "C" fn augmenter_salaire(p: *mut Personne, pourcentage: f64) {
    unsafe {
        if !p.is_null() {
            (*p).salaire *= 1.0 + (pourcentage / 100.0);
        }
    }
}

#[no_mangle]
pub extern "C" fn afficher_personne(p: *const Personne) {
    unsafe {
        if !p.is_null() {
            println!("Age: {}, Salaire: {:.2}€", (*p).age, (*p).salaire);
        }
    }
}
```

**`#[repr(C)]` - ESSENTIEL :**
- Force le layout mémoire à être compatible avec C
- Sans ça, Rust peut réorganiser les champs
- **Toujours utiliser** pour les structures FFI

**Utilisation depuis C :**

**Header `rustlib.h` :**
```c
#ifndef RUSTLIB_H
#define RUSTLIB_H

#include <stdint.h>

typedef struct {
    int32_t age;
    double salaire;
} Personne;

Personne creer_personne(int32_t age, double salaire);
void augmenter_salaire(Personne *p, double pourcentage);
void afficher_personne(const Personne *p);

#endif
```

**Programme C :**
```c
#include <stdio.h>
#include "rustlib.h"

int main() {
    Personne alice = creer_personne(30, 45000.0);

    afficher_personne(&alice);
    augmenter_salaire(&alice, 10.0);
    afficher_personne(&alice);

    return 0;
}
```

### Structures avec chaînes

```rust
use std::os::raw::c_char;
use std::ffi::CString;
use std::ptr;

#[repr(C)]
pub struct Utilisateur {
    pub nom: *mut c_char,
    pub age: i32,
}

#[no_mangle]
pub extern "C" fn creer_utilisateur(nom: *const c_char, age: i32) -> *mut Utilisateur {
    unsafe {
        if nom.is_null() {
            return ptr::null_mut();
        }

        // Dupliquer la chaîne C
        let nom_copy = libc::strdup(nom);

        let utilisateur = Box::new(Utilisateur {
            nom: nom_copy,
            age,
        });

        Box::into_raw(utilisateur)
    }
}

#[no_mangle]
pub extern "C" fn liberer_utilisateur(u: *mut Utilisateur) {
    unsafe {
        if !u.is_null() {
            let utilisateur = Box::from_raw(u);
            libc::free(utilisateur.nom as *mut libc::c_void);
            // Le Box sera automatiquement libéré
        }
    }
}
```

**Pattern `Box` :**
- **`Box::new()`** : Alloue sur le heap
- **`Box::into_raw()`** : Convertit en pointeur brut (transfère la propriété à C)
- **`Box::from_raw()`** : Reprend la propriété depuis C (pour libérer)

---

## Gestion des erreurs

### Codes d'erreur C

C utilise typiquement des codes de retour pour les erreurs.

```rust
use std::os::raw::c_int;

const SUCCESS: c_int = 0;
const ERROR_INVALID_INPUT: c_int = -1;
const ERROR_NOT_FOUND: c_int = -2;

#[no_mangle]
pub extern "C" fn diviser_entiers(a: c_int, b: c_int, resultat: *mut c_int) -> c_int {
    if resultat.is_null() {
        return ERROR_INVALID_INPUT;
    }

    if b == 0 {
        return ERROR_INVALID_INPUT;
    }

    unsafe {
        *resultat = a / b;
    }

    SUCCESS
}
```

**Utilisation depuis C :**
```c
int resultat;
int code = diviser_entiers(10, 2, &resultat);

if (code == 0) {
    printf("Résultat : %d\n", resultat);
} else {
    fprintf(stderr, "Erreur : %d\n", code);
}
```

### Pattern Result pour FFI

Wrappez les fonctions FFI en fonctions Rust idiomatiques :

```rust
pub enum MathError {
    DivisionParZero,
    Overflow,
}

// Fonction interne FFI (unsafe)
#[no_mangle]
pub extern "C" fn diviser_c(a: i32, b: i32, resultat: *mut i32) -> i32 {
    // ... implémentation
}

// Wrapper Rust safe
pub fn diviser(a: i32, b: i32) -> Result<i32, MathError> {
    let mut resultat = 0;
    let code = unsafe { diviser_c(a, b, &mut resultat) };

    match code {
        0 => Ok(resultat),
        -1 => Err(MathError::DivisionParZero),
        _ => Err(MathError::Overflow),
    }
}
```

---

## bindgen : Génération automatique de bindings

### Qu'est-ce que bindgen ?

`bindgen` est un outil qui génère automatiquement les bindings Rust depuis des headers C. C'est la méthode recommandée pour interfacer avec de grandes bibliothèques C.

**Installation :**
```bash
cargo install bindgen-cli
```

### Exemple simple

**Header C : `mathlib.h`**
```c
#ifndef MATHLIB_H
#define MATHLIB_H

int addition(int a, int b);
int multiplication(int a, int b);

typedef struct {
    int x;
    int y;
} Point;

Point creer_point(int x, int y);
int distance_carree(Point p1, Point p2);

#endif
```

**Générer les bindings :**
```bash
bindgen mathlib.h -o bindings.rs
```

**Fichier généré `bindings.rs` :**
```rust
/* automatically generated by rust-bindgen */

extern "C" {
    pub fn addition(a: ::std::os::raw::c_int, b: ::std::os::raw::c_int)
        -> ::std::os::raw::c_int;

    pub fn multiplication(a: ::std::os::raw::c_int, b: ::std::os::raw::c_int)
        -> ::std::os::raw::c_int;
}

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct Point {
    pub x: ::std::os::raw::c_int,
    pub y: ::std::os::raw::c_int,
}

extern "C" {
    pub fn creer_point(x: ::std::os::raw::c_int, y: ::std::os::raw::c_int)
        -> Point;

    pub fn distance_carree(p1: Point, p2: Point)
        -> ::std::os::raw::c_int;
}
```

**Utilisation :**
```rust
mod bindings;
use bindings::*;

fn main() {
    unsafe {
        let p1 = creer_point(3, 4);
        let p2 = creer_point(6, 8);

        let dist = distance_carree(p1, p2);
        println!("Distance au carré : {}", dist);
    }
}
```

### Intégration avec `build.rs`

Pour automatiser la génération lors du build :

**`build.rs`**
```rust
extern crate bindgen;

use std::env;
use std::path::PathBuf;

fn main() {
    // Indiquer à Cargo de relancer si le header change
    println!("cargo:rerun-if-changed=mathlib.h");

    // Configuration du linker
    println!("cargo:rustc-link-search=native=.");
    println!("cargo:rustc-link-lib=static=mathlib");

    // Générer les bindings
    let bindings = bindgen::Builder::default()
        .header("mathlib.h")
        .parse_callbacks(Box::new(bindgen::CargoCallbacks))
        .generate()
        .expect("Unable to generate bindings");

    // Écrire les bindings dans OUT_DIR
    let out_path = PathBuf::from(env::var("OUT_DIR").unwrap());
    bindings
        .write_to_file(out_path.join("bindings.rs"))
        .expect("Couldn't write bindings!");
}
```

**`Cargo.toml`**
```toml
[build-dependencies]
bindgen = "0.69"
```

**Utilisation dans le code :**
```rust
// Inclure les bindings générés
include!(concat!(env!("OUT_DIR"), "/bindings.rs"));

fn main() {
    unsafe {
        println!("5 + 7 = {}", addition(5, 7));
    }
}
```

---

## cbindgen : Générer des headers C depuis Rust

### Qu'est-ce que cbindgen ?

`cbindgen` fait l'inverse de `bindgen` : il génère des headers C depuis du code Rust.

**Installation :**
```bash
cargo install cbindgen
```

### Exemple

**Code Rust : `src/lib.rs`**
```rust
#[repr(C)]
pub struct Rectangle {
    pub largeur: f64,
    pub hauteur: f64,
}

#[no_mangle]
pub extern "C" fn creer_rectangle(largeur: f64, hauteur: f64) -> Rectangle {
    Rectangle { largeur, hauteur }
}

#[no_mangle]
pub extern "C" fn aire_rectangle(r: *const Rectangle) -> f64 {
    unsafe {
        if r.is_null() {
            return 0.0;
        }
        (*r).largeur * (*r).hauteur
    }
}
```

**Générer le header :**
```bash
cbindgen --lang c --output rectangle.h
```

**Header généré `rectangle.h` :**
```c
#ifndef RECTANGLE_H
#define RECTANGLE_H

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct Rectangle {
    double largeur;
    double hauteur;
} Rectangle;

Rectangle creer_rectangle(double largeur, double hauteur);

double aire_rectangle(const Rectangle *r);

#endif /* RECTANGLE_H */
```

**Configuration avec `cbindgen.toml`**
```toml
language = "C"

[export]
include = ["Rectangle"]

[export.rename]
"Rectangle" = "rectangle_t"
```

---

## Exemples pratiques

### Exemple 1 : Parser JSON en Rust pour C

**Rust : `src/lib.rs`**
```rust
use serde_json::{Value, Error};
use std::ffi::{CString, CStr};
use std::os::raw::c_char;
use std::ptr;

#[no_mangle]
pub extern "C" fn json_get_field(
    json_str: *const c_char,
    field: *const c_char
) -> *mut c_char {
    unsafe {
        if json_str.is_null() || field.is_null() {
            return ptr::null_mut();
        }

        // Convertir les chaînes C en Rust
        let json_rust = match CStr::from_ptr(json_str).to_str() {
            Ok(s) => s,
            Err(_) => return ptr::null_mut(),
        };

        let field_rust = match CStr::from_ptr(field).to_str() {
            Ok(s) => s,
            Err(_) => return ptr::null_mut(),
        };

        // Parser le JSON
        let v: Value = match serde_json::from_str(json_rust) {
            Ok(v) => v,
            Err(_) => return ptr::null_mut(),
        };

        // Extraire le champ
        let value = match v.get(field_rust) {
            Some(val) => val.to_string(),
            None => return ptr::null_mut(),
        };

        // Retourner en tant que CString
        match CString::new(value) {
            Ok(cstr) => cstr.into_raw(),
            Err(_) => ptr::null_mut(),
        }
    }
}

#[no_mangle]
pub extern "C" fn json_free_string(s: *mut c_char) {
    unsafe {
        if !s.is_null() {
            let _ = CString::from_raw(s);
            // Automatiquement libéré
        }
    }
}
```

**Utilisation depuis C :**
```c
#include <stdio.h>
#include "rustlib.h"

int main() {
    const char *json = "{\"nom\": \"Alice\", \"age\": 30}";

    char *nom = json_get_field(json, "nom");
    if (nom) {
        printf("Nom : %s\n", nom);
        json_free_string(nom);
    }

    char *age = json_get_field(json, "age");
    if (age) {
        printf("Age : %s\n", age);
        json_free_string(age);
    }

    return 0;
}
```

### Exemple 2 : Traitement parallèle avec Rayon

**Rust utilise Rayon pour le parallélisme :**

```rust
use rayon::prelude::*;

#[repr(C)]
pub struct ResultatParallele {
    pub somme: i64,
    pub max: i32,
}

#[no_mangle]
pub extern "C" fn traiter_parallele(
    data: *const i32,
    taille: usize
) -> ResultatParallele {
    unsafe {
        if data.is_null() || taille == 0 {
            return ResultatParallele { somme: 0, max: 0 };
        }

        // Créer un slice depuis le pointeur C
        let slice = std::slice::from_raw_parts(data, taille);

        // Traitement parallèle avec Rayon
        let somme: i64 = slice.par_iter().map(|&x| x as i64).sum();
        let max: i32 = *slice.par_iter().max().unwrap_or(&0);

        ResultatParallele { somme, max }
    }
}
```

**C peut bénéficier du parallélisme Rust :**
```c
int data[1000000];
// ... remplir data ...

ResultatParallele res = traiter_parallele(data, 1000000);
printf("Somme: %ld, Max: %d\n", res.somme, res.max);
```

---

## Bonnes pratiques

### 1. Toujours utiliser `#[repr(C)]`

```rust
// ❌ DANGEREUX : layout non défini
pub struct Donnees {
    pub x: i32,
    pub y: i64,
}

// ✅ CORRECT : layout compatible C
#[repr(C)]
pub struct Donnees {
    pub x: i32,
    pub y: i64,
}
```

### 2. Vérifier les pointeurs null

```rust
#[no_mangle]
pub extern "C" fn traiter_donnees(ptr: *const Data) -> i32 {
    unsafe {
        // ✅ Toujours vérifier
        if ptr.is_null() {
            return -1;  // Code d'erreur
        }

        // Utiliser le pointeur
        (*ptr).value
    }
}
```

### 3. Documenter la gestion mémoire

```rust
/// Crée une nouvelle structure Personne.
///
/// # Gestion mémoire
/// La structure retournée doit être libérée avec `liberer_personne()`.
///
/// # Sécurité
/// Le paramètre `nom` doit être une chaîne C valide et null-terminated.
#[no_mangle]
pub extern "C" fn creer_personne(nom: *const c_char) -> *mut Personne {
    // ...
}

/// Libère une structure Personne.
///
/// # Sécurité
/// - `p` doit avoir été créé par `creer_personne()`
/// - Après l'appel, `p` ne doit plus être utilisé
#[no_mangle]
pub extern "C" fn liberer_personne(p: *mut Personne) {
    // ...
}
```

### 4. Isoler le code unsafe

```rust
// ❌ Trop de code unsafe
#[no_mangle]
pub extern "C" fn fonction(ptr: *const i32) -> i32 {
    unsafe {
        if ptr.is_null() { return 0; }
        let val = *ptr;
        let resultat = val * 2;
        resultat + 10
    }
}

// ✅ Minimiser unsafe
#[no_mangle]
pub extern "C" fn fonction(ptr: *const i32) -> i32 {
    let val = unsafe {
        if ptr.is_null() { return 0; }
        *ptr
    };

    // Code safe
    let resultat = val * 2;
    resultat + 10
}
```

### 5. Fournir des wrappers safe

```rust
// API FFI (unsafe)
#[no_mangle]
pub extern "C" fn operation_unsafe(ptr: *mut Data) -> i32 {
    // ...
}

// Wrapper safe pour utilisation Rust
pub fn operation_safe(data: &mut Data) -> Result<i32, Error> {
    let resultat = unsafe { operation_unsafe(data as *mut Data) };
    if resultat < 0 {
        Err(Error::OperationFailed)
    } else {
        Ok(resultat)
    }
}
```

### 6. Tests de l'API FFI

```rust
#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_creation_personne() {
        use std::ffi::CString;

        let nom = CString::new("Test").unwrap();
        let personne = unsafe { creer_personne(nom.as_ptr()) };

        assert!(!personne.is_null());

        unsafe {
            liberer_personne(personne);
        }
    }

    #[test]
    fn test_pointeur_null() {
        use std::ptr;

        let resultat = unsafe { traiter_donnees(ptr::null()) };
        assert_eq!(resultat, -1);
    }
}
```

---

## Outils et debugging

### Vérification de compatibilité ABI

```bash
# Vérifier le layout d'une structure
cargo rustc -- -Z print-type-sizes
```

### Inspection des symboles

```bash
# Voir les symboles exportés (Linux)
nm -D target/release/librustlib.so | grep " T "

# Voir les symboles (macOS)
nm -gU target/release/librustlib.dylib

# Vérifier le mangling
objdump -t target/release/librustlib.so | grep fonction
```

### Debugging avec GDB/LLDB

```bash
# Compiler avec symboles de debug
cargo build

# Debugger
gdb target/debug/programme
# ou
lldb target/debug/programme
```

### Sanitizers

Rust supporte les sanitizers comme C :

```bash
# Address Sanitizer
RUSTFLAGS="-Z sanitizer=address" cargo build --target x86_64-unknown-linux-gnu

# Memory Sanitizer
RUSTFLAGS="-Z sanitizer=memory" cargo build --target x86_64-unknown-linux-gnu

# Thread Sanitizer
RUSTFLAGS="-Z sanitizer=thread" cargo build --target x86_64-unknown-linux-gnu
```

### Miri : Détecteur d'undefined behavior

Miri est un interpréteur Rust qui détecte les undefined behaviors :

```bash
# Installer Miri
rustup component add miri

# Exécuter les tests avec Miri
cargo miri test
```

---

## Gestion du build

### Structure de projet recommandée

```
mon_projet/
├── Cargo.toml
├── build.rs              # Script de build
├── cbindgen.toml         # Config cbindgen
├── src/
│   ├── lib.rs           # Bibliothèque Rust
│   └── bindings.rs      # Bindings (si générés manuellement)
├── include/
│   └── rustlib.h        # Header C (généré par cbindgen)
├── c_src/
│   ├── main.c           # Programme C de test
│   └── mathlib.c        # Code C à interfacer
└── examples/
    └── utilisation.rs   # Exemples Rust
```

### Cargo.toml pour FFI

```toml
[package]
name = "rustlib"
version = "0.1.0"
edition = "2021"

[lib]
crate-type = ["cdylib", "staticlib"]  # Bibliothèques dynamique et statique

[dependencies]
libc = "0.2"

[build-dependencies]
cbindgen = "0.26"

[profile.release]
lto = true              # Link-Time Optimization
opt-level = 3           # Optimisation maximale
```

### Makefile pour compilation combinée

```makefile
.PHONY: all clean rust c

all: rust c

rust:
	cargo build --release
	cbindgen --output include/rustlib.h

c: rust
	gcc -o main c_src/main.c \
		-Iinclude \
		-Ltarget/release \
		-lrustlib \
		-lpthread -ldl -lm

run: all
	LD_LIBRARY_PATH=target/release ./main

clean:
	cargo clean
	rm -f main include/rustlib.h
```

---

## Performance et optimisation

### Comparaison C vs Rust

**Benchmark typique (traitement de tableau) :**

| Implémentation | Temps | Remarques |
|----------------|-------|-----------|
| C pur (gcc -O3) | 100ms | Baseline |
| Rust (--release) | 98ms | Équivalent |
| Rust → C via FFI | 102ms | Overhead minimal |
| C → Rust via FFI | 101ms | Overhead minimal |

**Conclusion :** L'overhead FFI est **négligeable** pour des opérations conséquentes.

### Quand l'overhead FFI est problématique

```rust
// ❌ LENT : millions d'appels FFI
for i in 0..1_000_000 {
    unsafe {
        petite_fonction_c(i);  // ~40ns d'overhead × 1M = 40ms perdus
    }
}

// ✅ RAPIDE : batching
let data: Vec<i32> = (0..1_000_000).collect();
unsafe {
    traiter_tableau_c(data.as_ptr(), data.len());
}
```

### Link-Time Optimization (LTO)

LTO permet d'optimiser à travers la frontière Rust/C :

**Cargo.toml :**
```toml
[profile.release]
lto = "fat"  # LTO complet (plus lent à compiler, binaire plus rapide)
```

### Optimisations spécifiques

```rust
// Inlining agressif pour fonctions FFI
#[inline(always)]
pub fn wrapper_safe(data: &Data) -> i32 {
    unsafe { fonction_ffi_c(data as *const Data) }
}

// Pas de panic dans le code FFI
#[no_mangle]
pub extern "C" fn fonction() {
    // Éviter panic! qui peut crasher l'appelant C
    // Utiliser des codes d'erreur à la place
}
```

---

## Pièges courants et solutions

### Piège 1 : Oublier `#[repr(C)]`

```rust
// ❌ Layout Rust (non déterministe)
pub struct Data {
    a: i32,
    b: i64,
}

// ✅ Layout C (déterministe)
#[repr(C)]
pub struct Data {
    a: i32,
    b: i64,
}
```

### Piège 2 : Oublier `#[no_mangle]`

```rust
// ❌ Nom manglé : _ZN7rustlib8fonction17h...
pub extern "C" fn fonction() {}

// ✅ Nom préservé : fonction
#[no_mangle]
pub extern "C" fn fonction() {}
```

### Piège 3 : Panic à travers FFI

```rust
// ❌ DANGEREUX : panic peut crasher C
#[no_mangle]
pub extern "C" fn fonction(x: i32) -> i32 {
    let v = vec![1, 2, 3];
    v[x as usize]  // panic! si x >= 3
}

// ✅ SAFE : gestion d'erreur explicite
#[no_mangle]
pub extern "C" fn fonction(x: i32, resultat: *mut i32) -> i32 {
    if resultat.is_null() {
        return -1;
    }

    let v = vec![1, 2, 3];
    if x < 0 || x >= v.len() as i32 {
        return -2;  // Erreur : index invalide
    }

    unsafe {
        *resultat = v[x as usize];
    }

    0  // Succès
}
```

### Piège 4 : Durée de vie des CString

```rust
// ❌ DANGEREUX : CString détruit trop tôt
unsafe {
    let ptr = CString::new("hello").unwrap().as_ptr();
    fonction_c(ptr);  // ptr invalide !
}

// ✅ CORRECT : CString vit assez longtemps
let cstring = CString::new("hello").unwrap();
unsafe {
    fonction_c(cstring.as_ptr());
}
```

### Piège 5 : Threads et FFI

```rust
// ❌ Problème potentiel
#[no_mangle]
pub extern "C" fn lancer_thread() {
    std::thread::spawn(|| {
        // Code qui peut panic
    });
    // Thread non joint : peut causer des problèmes
}

// ✅ Gestion propre des threads
#[no_mangle]
pub extern "C" fn lancer_thread() -> bool {
    std::panic::catch_unwind(|| {
        std::thread::spawn(|| {
            // Travail
        }).join().unwrap();
    }).is_ok()
}
```

---

## Cas d'usage réel : Bibliothèque de compression

Créons une bibliothèque Rust de compression accessible depuis C.

**Rust : `src/lib.rs`**
```rust
use flate2::Compression;
use flate2::write::{GzEncoder, GzDecoder};
use std::io::Write;
use std::os::raw::{c_uchar, c_int};
use std::ptr;
use std::slice;

#[no_mangle]
pub extern "C" fn compresser(
    input: *const c_uchar,
    input_len: usize,
    output: *mut c_uchar,
    output_len: *mut usize
) -> c_int {
    unsafe {
        if input.is_null() || output.is_null() || output_len.is_null() {
            return -1;
        }

        let input_slice = slice::from_raw_parts(input, input_len);
        let mut encoder = GzEncoder::new(Vec::new(), Compression::default());

        if encoder.write_all(input_slice).is_err() {
            return -2;
        }

        let compressed = match encoder.finish() {
            Ok(data) => data,
            Err(_) => return -3,
        };

        if compressed.len() > *output_len {
            *output_len = compressed.len();
            return -4;  // Buffer trop petit
        }

        ptr::copy_nonoverlapping(
            compressed.as_ptr(),
            output,
            compressed.len()
        );

        *output_len = compressed.len();
        0  // Succès
    }
}
```

**Header C : `compress.h`**
```c
#ifndef COMPRESS_H
#define COMPRESS_H

#include <stddef.h>
#include <stdint.h>

int compresser(
    const uint8_t *input,
    size_t input_len,
    uint8_t *output,
    size_t *output_len
);

#endif
```

**Utilisation C :**
```c
#include <stdio.h>
#include <stdlib.h>
#include "compress.h"

int main() {
    const char *texte = "Hello World! " * 100;  // Texte répétitif
    size_t input_len = strlen(texte);

    uint8_t *output = malloc(input_len);
    size_t output_len = input_len;

    int result = compresser(
        (uint8_t*)texte,
        input_len,
        output,
        &output_len
    );

    if (result == 0) {
        printf("Compressé : %zu -> %zu bytes (%.1f%%)\n",
               input_len, output_len,
               100.0 * output_len / input_len);
    } else {
        fprintf(stderr, "Erreur : %d\n", result);
    }

    free(output);
    return 0;
}
```

---

## Récapitulatif

### Points clés à retenir

1. **FFI en Rust est simple et sûr**
   - `extern "C"` pour la compatibilité ABI
   - `#[no_mangle]` pour préserver les noms
   - `#[repr(C)]` pour les structures
   - `unsafe` pour marquer le code non vérifié

2. **Deux directions possibles**
   - Rust → C : Utiliser des bibliothèques C
   - C → Rust : Exposer des fonctions Rust à C

3. **Gestion mémoire critique**
   - Clarifier qui alloue et qui libère
   - Utiliser `CString` pour les chaînes
   - `Box::into_raw()` et `Box::from_raw()` pour transférer la propriété

4. **Outils puissants**
   - `bindgen` : Génère du Rust depuis des headers C
   - `cbindgen` : Génère des headers C depuis du Rust
   - Automatisation via `build.rs`

5. **Performance**
   - Overhead FFI négligeable pour opérations conséquentes
   - LTO pour optimisations cross-language
   - Éviter les appels FFI fréquents de petites fonctions

6. **Sécurité**
   - Toujours vérifier les pointeurs null
   - Éviter les panics dans le code FFI
   - Documenter les invariants de sécurité
   - Tests exhaustifs

### Checklist avant d'utiliser FFI

- [ ] Les structures utilisent `#[repr(C)]`
- [ ] Les fonctions exportées utilisent `#[no_mangle]`
- [ ] Les fonctions exportées utilisent `extern "C"`
- [ ] Les pointeurs null sont vérifiés
- [ ] La gestion mémoire est documentée
- [ ] Les panics sont évités ou catchés
- [ ] Le code unsafe est minimisé et isolé
- [ ] Les tests incluent des cas d'erreur
- [ ] Un header C est généré (cbindgen)
- [ ] La documentation explique l'API FFI

### Le mot de la fin

Rust offre une des meilleures expériences d'interopérabilité avec C. La combinaison de la sécurité de Rust et de l'écosystème C mature ouvre des possibilités extraordinaires :
- **Migrer progressivement** du C vers Rust
- **Moderniser** des applications legacy
- **Combiner** le meilleur des deux mondes

Rust ne cherche pas à remplacer C du jour au lendemain, mais à coexister harmonieusement avec lui. FFI est le pont qui rend cette coexistence non seulement possible, mais agréable.

**Bon coding avec Rust et C ! 🦀⚡**

---


⏭️ [ABI et compatibilité](/28-interoperabilite/04-abi-compatibilite.md)
