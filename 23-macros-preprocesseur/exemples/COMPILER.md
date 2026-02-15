# Chapitre 23 - Macros et preprocesseur : Guide de compilation

Compilation par defaut :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 fichier.c -o fichier
```

Les exceptions sont indiquees pour chaque fichier concerne.

---

## Section 23.1 : Macros parametrees (01-macros-parametrees.md)

### 01_carre.c
- **Description** : Macro simple qui calcule le carre d'un nombre
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 01_carre.c -o 01_carre`
- **Sortie attendue** : Calcul du carre de 5, 3+2, et avec variable

### 02_mauvais_carre.c
- **Description** : Bug classique - macro sans parentheses (resultat incorrect)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 02_mauvais_carre.c -o 02_mauvais_carre`
- **Sortie attendue** : Demonstration du bug de precedence (CARRE(3+2) != 25)

### 03_max_min.c
- **Description** : Macros MAX et MIN avec plusieurs parametres
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 03_max_min.c -o 03_max_min`
- **Sortie attendue** : Calculs de max/min avec entiers et flottants

### 04_conversion.c
- **Description** : Macros de conversion Celsius/Fahrenheit
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 04_conversion.c -o 04_conversion`
- **Sortie attendue** : Conversions de temperatures

### 05_multilignes.c
- **Description** : Macro multi-lignes avec idiome do-while(0)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 05_multilignes.c -o 05_multilignes`
- **Sortie attendue** : Echange de variables et affichage des valeurs

### 06_genericite.c
- **Description** : Genericite des macros - fonctionnent avec differents types
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 06_genericite.c -o 06_genericite`
- **Sortie attendue** : MAX/MIN appliques sur int, double, char

### 07_exemple_complet.c
- **Description** : Exemple complet combinant constantes, macros et debug
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 07_exemple_complet.c -o 07_exemple_complet`
- **Sortie attendue** : Surface, clamping, conversions, debug conditionnel

### 08_debug_check.c
- **Description** : Macros de debug et de verification de pointeur NULL
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17 08_debug_check.c -o 08_debug_check`
- **Note** : Sans `-pedantic` (utilise `##__VA_ARGS__`, extension GNU)
- **Sortie attendue** : Messages de debug avec fichier/ligne, verification de pointeurs

---

## Section 23.2 : Stringification et concatenation (02-stringification-concatenation.md)

### 09_stringifier.c
- **Description** : Operateur # - transformation d'arguments en chaines
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 09_stringifier.c -o 09_stringifier`
- **Sortie attendue** : Expressions transformees en chaines de caracteres

### 10_print_var.c
- **Description** : Debug printing - affichage de variables avec leur nom
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 10_print_var.c -o 10_print_var`
- **Sortie attendue** : Variables affichees avec leur nom (ex: "x = 42")

### 11_debug_avance.c
- **Description** : Macro de debug avancee avec __FILE__ et __LINE__
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 11_debug_avance.c -o 11_debug_avance`
- **Sortie attendue** : Messages de debug avec localisation fichier:ligne

### 12_print_types.c
- **Description** : Stringification avec differents types (int, float, string)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 12_print_types.c -o 12_print_types`
- **Sortie attendue** : Affichage de valeurs avec leurs expressions et types

### 13_double_expansion.c
- **Description** : Double expansion - piege et solution pour stringify de macros
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 13_double_expansion.c -o 13_double_expansion`
- **Sortie attendue** : Comparaison stringify direct vs double expansion de macros

### 14_concat_simple.c
- **Description** : Operateur ## - concatenation de tokens (token pasting)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 14_concat_simple.c -o 14_concat_simple`
- **Sortie attendue** : Variables creees par concatenation de tokens

### 15_declare_var.c
- **Description** : Generation de noms de variables avec ##
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 15_declare_var.c -o 15_declare_var`
- **Sortie attendue** : Declarations de variables avec prefixes generes

### 16_getter.c
- **Description** : Generation automatique de fonctions getter avec ##
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 16_getter.c -o 16_getter`
- **Sortie attendue** : Appels de getters generes automatiquement

### 17_property.c
- **Description** : Generation de getters et setters complets avec ##
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 17_property.c -o 17_property`
- **Sortie attendue** : Get/set sur des proprietes generees

### 18_make_version.c
- **Description** : Concatenation avec nombres pour generer des identificateurs
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 18_make_version.c -o 18_make_version`
- **Sortie attendue** : Numero de version compose par concatenation

### 19_double_expansion_concat.c
- **Description** : Double expansion pour forcer l'expansion avant concatenation
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 19_double_expansion_concat.c -o 19_double_expansion_concat`
- **Sortie attendue** : Comparaison concat direct vs double expansion

### 20_test_unitaire.c
- **Description** : Combinaison de # et ## pour macro de tests unitaires
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 20_test_unitaire.c -o 20_test_unitaire`
- **Sortie attendue** : Execution de tests avec resultats PASS/FAIL

### 21_define_struct.c
- **Description** : Generation de structures et accesseurs avec # et ##
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 21_define_struct.c -o 21_define_struct`
- **Sortie attendue** : Structures generees avec affichage des champs

### 22_log_niveaux.c
- **Description** : Logging avec niveaux utilisant la stringification
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 22_log_niveaux.c -o 22_log_niveaux`
- **Sortie attendue** : Messages de log avec niveau stringify

### 23_converter.c
- **Description** : Generation de fonctions de conversion avec # et ##
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 23_converter.c -o 23_converter`
- **Sortie attendue** : Conversions generees entre types

### 24_config_setter.c
- **Description** : Interface de configuration avec setters generes par macros
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 24_config_setter.c -o 24_config_setter`
- **Sortie attendue** : Configuration modifiee via setters generes

### 25_color_enum.c
- **Description** : X-macros pour associer noms de chaines aux valeurs d'enum
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 25_color_enum.c -o 25_color_enum`
- **Sortie attendue** : Couleurs affichees avec enum et noms de chaines

### 26_framework_tests.c
- **Description** : Mini-framework de tests combinant # et ## (exemple complet)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 26_framework_tests.c -o 26_framework_tests`
- **Sortie attendue** : Suite de tests avec compteur de succes/echecs

---

## Section 23.3 : Compilation conditionnelle (03-compilation-conditionnelle.md)

### 27_ifdef_debug.c
- **Description** : Directive #ifdef - inclusion de code si macro definie
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 27_ifdef_debug.c -o 27_ifdef_debug`
- **Sortie attendue** : Message affiche seulement si DEBUG defini

### 28_ifndef_version.c
- **Description** : Directive #ifndef - valeur par defaut si macro non definie
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 28_ifndef_version.c -o 28_ifndef_version`
- **Sortie attendue** : Version par defaut si non fournie par -D

### 29_ifdef_else.c
- **Description** : Directive #ifdef/#else pour branche alternative
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 29_ifdef_else.c -o 29_ifdef_else`
- **Sortie attendue** : Branche debug ou release selon macro definie

### 30_if_version.c
- **Description** : Directive #if - evaluation d'expressions constantes
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 30_if_version.c -o 30_if_version`
- **Sortie attendue** : Comportement different selon valeur de VERSION

### 31_elif_log.c
- **Description** : Directive #elif - chaine de conditions (niveaux de log)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 31_elif_log.c -o 31_elif_log`
- **Sortie attendue** : Niveau de log actif selon configuration

### 32_defined.c
- **Description** : Operateur defined() - test d'existence de macros
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 32_defined.c -o 32_defined`
- **Sortie attendue** : Test de combinaisons de macros definies

### 33_detection_os.c
- **Description** : Detection du systeme d'exploitation via macros predefinies
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 33_detection_os.c -o 33_detection_os`
- **Sortie attendue** : "Systeme: Linux" (sur Ubuntu)

### 34_chemins_fichiers.c
- **Description** : Chemins de fichiers multi-plateforme
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 34_chemins_fichiers.c -o 34_chemins_fichiers`
- **Sortie attendue** : Chemins avec separateurs adaptes a l'OS

### 35_detection_compilateur.c
- **Description** : Detection du compilateur (GCC, Clang, MSVC)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 35_detection_compilateur.c -o 35_detection_compilateur`
- **Sortie attendue** : "Compilateur: GCC version X.Y.Z"

### 36_debug_conditionnel.c
- **Description** : Mode debug active/desactive avec macros variadiques
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17 36_debug_conditionnel.c -o 36_debug_conditionnel`
- **Note** : Sans `-pedantic` (utilise `##__VA_ARGS__`, extension GNU)
- **Sortie attendue** : Messages de debug conditionnels avec fichier/ligne

### 37_niveaux_log.c
- **Description** : Systeme de logging avec niveaux conditionnels
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17 37_niveaux_log.c -o 37_niveaux_log`
- **Note** : Sans `-pedantic` (utilise `##__VA_ARGS__`, extension GNU)
- **Sortie attendue** : Messages de log filtres par niveau

### 38_detection_arch.c
- **Description** : Detection de l'architecture processeur
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 38_detection_arch.c -o 38_detection_arch`
- **Sortie attendue** : "Architecture: x86_64" (sur machine 64 bits)

### 39_version_standard.c
- **Description** : Detection de la version du standard C
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 39_version_standard.c -o 39_version_standard`
- **Sortie attendue** : Version du standard C detectee (C17)

### 40_atomics.c
- **Description** : Compatibilite C11+ avec detection des atomics
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 40_atomics.c -o 40_atomics`
- **Sortie attendue** : Information sur le support des atomics

### 41_production_dev.c
- **Description** : Code de production vs developpement avec assertions
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 41_production_dev.c -o 41_production_dev`
- **Sortie attendue** : Comportement different selon mode production/dev

### 42_feature_flags.c
- **Description** : Feature flags - activation/desactivation de fonctionnalites
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 42_feature_flags.c -o 42_feature_flags`
- **Sortie attendue** : Liste des fonctionnalites activees/desactivees

### 43_macros_predefinies.c
- **Description** : Macros predefinies standards (__FILE__, __LINE__, etc.)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 43_macros_predefinies.c -o 43_macros_predefinies`
- **Sortie attendue** : Valeurs des macros predefinies du preprocesseur

### 44_assert_msg.c
- **Description** : Macro d'assertion personnalisee avec message
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 44_assert_msg.c -o 44_assert_msg`
- **Sortie attendue** : Assertions reussies et message sur echec

### 45_defines_cli.c
- **Description** : Definitions de macros depuis la ligne de commande (-D)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 45_defines_cli.c -o 45_defines_cli`
- **Note** : Peut aussi etre compile avec `-DVERSION=2 -DDEBUG` pour tester les defines CLI
- **Sortie attendue** : Valeurs des macros definies en ligne de commande

### 46_config_build/ (multi-fichiers)
- **Description** : Programme principal utilisant la configuration de build
- **Fichiers** : `config.h`, `main.c`
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 46_config_build/main.c -o 46_config_build/main`
- **Sortie attendue** : Configuration de build affichee depuis header

---

## Section 23.4 : Macros cross-platform (04-macros-cross-platform.md)

### 47_detection_os_detaillee.c
- **Description** : Detection detaillee du systeme d'exploitation
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 47_detection_os_detaillee.c -o 47_detection_os_detaillee`
- **Sortie attendue** : OS detecte avec details (Linux, version kernel)

### 48_chemins_portables.c
- **Description** : Separateur de chemin portable Windows/Unix
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 48_chemins_portables.c -o 48_chemins_portables`
- **Sortie attendue** : Chemins construits avec separateur '/' (Unix)

### 49_couleurs_terminal.c
- **Description** : Couleurs ANSI dans le terminal (portable)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 49_couleurs_terminal.c -o 49_couleurs_terminal`
- **Sortie attendue** : Texte colore avec sequences ANSI

### 50_types_portables.c
- **Description** : Types de donnees portables avec stdint.h
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 50_types_portables.c -o 50_types_portables`
- **Sortie attendue** : Tailles et limites des types portables

### 51_likely_unlikely.c
- **Description** : Extensions GCC/Clang - LIKELY/UNLIKELY pour optimisation
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 51_likely_unlikely.c -o 51_likely_unlikely`
- **Sortie attendue** : Branchements avec hints de prediction

### 52_arch_64bits.c
- **Description** : Detection 32 vs 64 bits
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 52_arch_64bits.c -o 52_arch_64bits`
- **Sortie attendue** : "Architecture 64 bits" avec taille des pointeurs

### 53_endianness.c
- **Description** : Detection et conversion d'endianness (boutisme)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 53_endianness.c -o 53_endianness`
- **Sortie attendue** : Detection little/big endian, conversions octets

### 54_platform/ (multi-fichiers)
- **Description** : Utilisation du header platform.h centralise
- **Fichiers** : `platform.h`, `main.c`
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 54_platform/main.c -o 54_platform/main`
- **Sortie attendue** : Informations plateforme depuis header centralise

---

## Section 23.5 : Dangers et pieges des macros (05-dangers-macros.md)

### 55_evaluation_multiple.c
- **Description** : BUG INTENTIONNEL - evaluation multiple avec effets de bord
- **Compilation** : `gcc -Wall -Wextra -std=c17 55_evaluation_multiple.c -o 55_evaluation_multiple`
- **Note** : Sans `-Werror` (comportement indefini intentionnel pour demonstration)
- **Sortie attendue** : Demonstration du bug d'evaluation multiple dans macros

### 56_effets_bord_max.c
- **Description** : BUG INTENTIONNEL - MAX appelle la fonction deux fois
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 56_effets_bord_max.c -o 56_effets_bord_max`
- **Sortie attendue** : Compteur incremente 2 fois au lieu de 1 (bug pedagogique)

### 57_precedence.c
- **Description** : BUG INTENTIONNEL - problemes de precedence sans parentheses
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 57_precedence.c -o 57_precedence`
- **Sortie attendue** : Resultats incorrects dus aux precedences sans parentheses

### 58_do_while.c
- **Description** : Idiome do-while(0) pour macros multi-instructions
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 58_do_while.c -o 58_do_while`
- **Sortie attendue** : Macros multi-instructions fonctionnant dans if/else

### 59_types_danger.c
- **Description** : BUG INTENTIONNEL - comparaison signed/unsigned avec MAX
- **Compilation** : `gcc -Wall -Wextra -std=c17 59_types_danger.c -o 59_types_danger`
- **Note** : Sans `-Werror` (warning sign-compare intentionnel pour demonstration)
- **Sortie attendue** : Resultat surprenant de comparaison signed/unsigned

### 60_generic.c
- **Description** : Alternative C11 _Generic pour genericite typee
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 60_generic.c -o 60_generic`
- **Sortie attendue** : Fonctions generiques selectionnees par type a la compilation

### 61_swap_refactoring.c
- **Description** : Refactoring macro vers fonction - swap generique
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 61_swap_refactoring.c -o 61_swap_refactoring`
- **Sortie attendue** : Echange de variables via macro puis via fonction inline

---

## Section 23.6 : Macros predefinies utiles (06-macros-predefinies.md)

### 62_func.c
- **Description** : __func__ - nom de la fonction courante (C99)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 62_func.c -o 62_func`
- **Sortie attendue** : Noms des fonctions affichees dynamiquement

### 63_counter.c
- **Description** : __COUNTER__ - compteur auto-incrementant (extension GCC/Clang)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 63_counter.c -o 63_counter`
- **Sortie attendue** : Identifiants uniques generes par __COUNTER__

### 64_logging_complet.c
- **Description** : Systeme de logging complet avec timestamps et niveaux
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17 64_logging_complet.c -o 64_logging_complet`
- **Note** : Sans `-pedantic` (utilise `##__VA_ARGS__`, extension GNU)
- **Sortie attendue** : Messages de log avec timestamp, fichier, ligne, niveau

### 65_gestion_erreurs.c
- **Description** : Macros de gestion d'erreurs avec contexte complet
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 65_gestion_erreurs.c -o 65_gestion_erreurs`
- **Sortie attendue** : Gestion d'erreurs avec messages contextuels (fichier/ligne/fonction)

### 66_time_it.c
- **Description** : Macro de profilage - mesure du temps d'execution
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 66_time_it.c -o 66_time_it`
- **Sortie attendue** : Temps d'execution mesure pour blocs de code

### 67_build_info.c
- **Description** : Affichage complet des informations de build
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 67_build_info.c -o 67_build_info`
- **Sortie attendue** : Date/heure de compilation, fichier, compilateur, standard C

### 68_features.c
- **Description** : Detection des fonctionnalites C11 disponibles
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 68_features.c -o 68_features`
- **Sortie attendue** : Liste des fonctionnalites C11 detectees (atomics, threads, etc.)

### 69_version_system.c
- **Description** : Systeme de version automatique avec macros
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 69_version_system.c -o 69_version_system`
- **Sortie attendue** : Version du programme composee de majeure.mineure.patch

---

## Section 23.7 : X-Macros (07-x-macros.md)

### 70_fruits_xmacro.c
- **Description** : Principe de base des X-Macros avec liste de fruits
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 70_fruits_xmacro.c -o 70_fruits_xmacro`
- **Sortie attendue** :
```
Il y a 4 fruits  
Fruit 0: POMME  
Fruit 1: BANANE  
Fruit 2: ORANGE  
Fruit 3: FRAISE
```

### 71_error_codes.c
- **Description** : Codes d'erreur avec messages via X-Macros
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 71_error_codes.c -o 71_error_codes`
- **Sortie attendue** :
```
=== Codes d'erreur ===
  0 : SUCCESS         -> Operation reussie
  1 : FILE_NOT_FOUND  -> Fichier introuvable
  ...
Test: get_error_message(ERR_FILE_NOT_FOUND) = Fichier introuvable  
Test: get_error_message(99) = Erreur inconnue
```

### 72_machine_etats.c
- **Description** : Machine a etats avec X-Macros
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 72_machine_etats.c -o 72_machine_etats`
- **Sortie attendue** :
```
=== Machine a etats ===
Etat actuel: Connexion en cours  
Etat valide: Oui  
Transition: CONNECTING -> CONNECTED
...
```

### 73_configuration.c
- **Description** : Configuration avec valeurs par defaut via X-Macros
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 73_configuration.c -o 73_configuration`
- **Sortie attendue** :
```
=== Configuration ===
  MAX_CONNECTIONS  = 100
  TIMEOUT_MS       = 5000
  ...
Modification: MAX_CONNECTIONS = 200
```

### 74_commandes.c
- **Description** : Table de dispatch pour commandes avec X-Macros
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 74_commandes.c -o 74_commandes`
- **Sortie attendue** :
```
=== Commandes disponibles ===
  help    : Afficher l'aide
  open    : Ouvrir un fichier
  ...
Execution de 'help': ...  
Execution de 'save': ...
```

### 75_serialiseur.c
- **Description** : Generation de serialiseur avec X-Macros
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 75_serialiseur.c -o 75_serialiseur`
- **Sortie attendue** :
```
Person {
  id: 12345
  name: Alice Dupont
  age: 30
  salary: 45000.50
}
```

### 76_tests_unitaires.c
- **Description** : Tests unitaires automatiques avec X-Macros
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 76_tests_unitaires.c -o 76_tests_unitaires`
- **Sortie attendue** :
```
Execution de 4 tests...

> test_addition
> test_soustraction
> test_multiplication
> test_division

============================
Resultats:
  Reussis: 8
  Echoues: 0
============================
```

### 77_registres.c
- **Description** : Registres materiels avec X-Macros
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 77_registres.c -o 77_registres`
- **Sortie attendue** :
```
=== MAP DES REGISTRES ===
Nom            Addr     Perm   Description
-----------    ------   ----   -------------------
REG_STATUS     0x0000   RW     Registre de statut  
REG_CONTROL    0x0004   RW     Registre de controle
...
=========================

Registre: REG_STATUS
  Adresse:     0x0000
  Permissions: RW
  Description: Registre de statut
```

### 78_menu.c
- **Description** : Mini-framework de menu avec X-Macros
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 78_menu.c -o 78_menu`
- **Note** : Programme interactif (attend une entree clavier). Tester avec : `printf '1\n0\n' | ./78_menu`
- **Sortie attendue** :
```
Application de demonstration - X-Macros

=== MENU PRINCIPAL ===
 1. Nouveau fichier
 2. Ouvrir
 3. Enregistrer
 4. Enregistrer sous
 5. Imprimer
 0. Quitter
======================

Choisissez une option:
-> Creer un nouveau document
  Execution de: Nouveau fichier
...
```

---

## Resume

| Plage | Section | Fichier source | Nb exemples |
|-------|---------|----------------|-------------|
| 01-08 | 23.1 Macros parametrees | 01-macros-parametrees.md | 8 |
| 09-26 | 23.2 Stringification et concatenation | 02-stringification-concatenation.md | 18 |
| 27-46 | 23.3 Compilation conditionnelle | 03-compilation-conditionnelle.md | 20 |
| 47-54 | 23.4 Macros cross-platform | 04-macros-cross-platform.md | 8 |
| 55-61 | 23.5 Dangers et pieges des macros | 05-dangers-macros.md | 7 |
| 62-69 | 23.6 Macros predefinies utiles | 06-macros-predefinies.md | 8 |
| 70-78 | 23.7 X-Macros | 07-x-macros.md | 9 |
| **Total** | | | **78 programmes (80 fichiers)** |

### Exceptions de compilation

| Fichier | Exception | Raison |
|---------|-----------|--------|
| 08_debug_check.c | Sans `-pedantic` | `##__VA_ARGS__` (extension GNU) |
| 36_debug_conditionnel.c | Sans `-pedantic` | `##__VA_ARGS__` (extension GNU) |
| 37_niveaux_log.c | Sans `-pedantic` | `##__VA_ARGS__` (extension GNU) |
| 64_logging_complet.c | Sans `-pedantic` | `##__VA_ARGS__` (extension GNU) |
| 55_evaluation_multiple.c | Sans `-Werror` | Bug intentionnel (UB pedagogique) |
| 59_types_danger.c | Sans `-Werror` | Bug intentionnel (sign-compare pedagogique) |

### Projets multi-fichiers

| Dossier | Fichiers | Compilation |
|---------|----------|-------------|
| 46_config_build/ | config.h, main.c | `gcc -Wall -Wextra -Werror -pedantic -std=c17 46_config_build/main.c -o 46_config_build/main` |
| 54_platform/ | platform.h, main.c | `gcc -Wall -Wextra -Werror -pedantic -std=c17 54_platform/main.c -o 54_platform/main` |

### Programme interactif

| Fichier | Test |
|---------|------|
| 78_menu.c | `printf '1\n0\n' \| ./78_menu` |
