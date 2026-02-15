# Chapitre 22 - Pointeurs avancés : Guide de compilation

## Compilation standard

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 fichier.c -o fichier
```

Les fichiers utilisant `##__VA_ARGS__` (extension GNU) doivent être compilés **sans `-pedantic`** :

```bash
gcc -Wall -Wextra -Werror -std=c17 fichier.c -o fichier
```

---

## Section 22.1 : Pointeurs de fonctions (Callbacks)

### 01_pointeurs_fonctions_base.c
- **Section** : 22.1 - Pointeurs de fonctions
- **Description** : Exemple simple de pointeur de fonction avec addition et soustraction
- **Fichier source** : 01-pointeurs-fonctions.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 01_pointeurs_fonctions_base.c -o 01_pointeurs_fonctions_base`
- **Sortie attendue** :
```
Resultat de l'addition : 8  
Resultat de la soustraction : 2
```

### 02_callbacks_calculatrice.c
- **Section** : 22.1 - Pointeurs de fonctions
- **Description** : Calculatrice générique utilisant des callbacks
- **Fichier source** : 01-pointeurs-fonctions.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 02_callbacks_calculatrice.c -o 02_callbacks_calculatrice`
- **Sortie attendue** :
```
Calcul de 10 et 5...  
Addition : 15  
Calcul de 10 et 5...  
Multiplication : 50  
Calcul de 10 et 5...  
Division : 2
```

### 03_tri_personnalise.c
- **Section** : 22.1 - Pointeurs de fonctions
- **Description** : Tri personnalisé avec qsort et fonctions de comparaison
- **Fichier source** : 01-pointeurs-fonctions.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 03_tri_personnalise.c -o 03_tri_personnalise`
- **Sortie attendue** :
```
Tableau original : 64 34 25 12 22 11 90  
Tri croissant : 11 12 22 25 34 64 90  
Tri decroissant : 90 64 34 25 22 12 11
```

### 04_typedef_operations.c
- **Section** : 22.1 - Pointeurs de fonctions
- **Description** : Utilisation de typedef pour simplifier les pointeurs de fonctions
- **Fichier source** : 01-pointeurs-fonctions.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 04_typedef_operations.c -o 04_typedef_operations`
- **Sortie attendue** :
```
10 + 5 = 15
10 - 5 = 5
```

### 05_systeme_evenements.c
- **Section** : 22.1 - Pointeurs de fonctions
- **Description** : Système simple de gestion d'événements avec callbacks
- **Fichier source** : 01-pointeurs-fonctions.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 05_systeme_evenements.c -o 05_systeme_evenements`
- **Sortie attendue** :
```
Evenement 'info' declenche : [INFO] Application demarree  
Evenement 'warning' declenche : [ATTENTION] Memoire faible  
Evenement 'erreur' declenche : [ERREUR] Fichier introuvable
```

---

## Section 22.2 : Tableaux de pointeurs de fonctions

### 06_tableau_operations.c
- **Section** : 22.2 - Tableaux de pointeurs de fonctions
- **Description** : Tableau de pointeurs de fonctions pour opérations arithmétiques
- **Fichier source** : 02-tableaux-pointeurs-fonctions.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 06_tableau_operations.c -o 06_tableau_operations`
- **Sortie attendue** :
```
Addition : 20 + 5 = 25  
Soustraction : 20 - 5 = 15  
Multiplication : 20 * 5 = 100  
Division : 20 / 5 = 4
```

### 07_typedef_tableau.c
- **Section** : 22.2 - Tableaux de pointeurs de fonctions
- **Description** : Simplification avec typedef pour les tableaux de pointeurs de fonctions
- **Fichier source** : 02-tableaux-pointeurs-fonctions.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 07_typedef_tableau.c -o 07_typedef_tableau`
- **Sortie attendue** :
```
6 * 7 = 42
```

### 08_table_dispatch.c
- **Section** : 22.2 - Tableaux de pointeurs de fonctions
- **Description** : Table de dispatch remplaçant un switch-case
- **Fichier source** : 02-tableaux-pointeurs-fonctions.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 08_table_dispatch.c -o 08_table_dispatch`
- **Sortie attendue** :
```
10 + 5 = 15
10 - 5 = 5
10 * 5 = 50
10 / 5 = 2
```

### 09_fsm_porte.c
- **Section** : 22.2 - Tableaux de pointeurs de fonctions
- **Description** : Machine à états finis (FSM) pour une porte automatique
- **Fichier source** : 02-tableaux-pointeurs-fonctions.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 09_fsm_porte.c -o 09_fsm_porte`
- **Note** : Utilise `sleep()` pour simuler les temps d'ouverture/fermeture
- **Sortie attendue** :
```
=== SIMULATION PORTE AUTOMATIQUE ===

Etat : FERMEE  
Etat : FERMEE
  -> Personne detectee, ouverture...
Etat : OUVERTURE EN COURS
  -> Moteur active...
Etat : OUVERTE  
Etat : OUVERTE
  -> Timeout, fermeture...
Etat : FERMETURE EN COURS
  -> Obstacle detecte ! Reouverture...
Etat : OUVERTURE EN COURS
  -> Moteur active...
Etat : OUVERTE  
Etat : OUVERTE

=== FIN DE LA SIMULATION ===
```

### 10_handlers_creux.c
- **Section** : 22.2 - Tableaux de pointeurs de fonctions
- **Description** : Initialisation partielle et tableaux creux de pointeurs de fonctions
- **Fichier source** : 02-tableaux-pointeurs-fonctions.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 10_handlers_creux.c -o 10_handlers_creux`
- **Sortie attendue** :
```
Appel du handler 0 : Handler 0 : code 0  
Handler 1 : non defini  
Appel du handler 2 : Handler 2 : code 200  
Handler 3 : non defini  
Handler 4 : non defini  
Appel du handler 5 : Handler 5 : code 500  
Handler 6 : non defini  
Handler 7 : non defini  
Handler 8 : non defini  
Handler 9 : non defini
```

### 11_operations_2d.c
- **Section** : 22.2 - Tableaux de pointeurs de fonctions
- **Description** : Tableaux multidimensionnels de pointeurs de fonctions
- **Fichier source** : 02-tableaux-pointeurs-fonctions.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 11_operations_2d.c -o 11_operations_2d`
- **Sortie attendue** :
```
Categorie 0, Operation 0 (addition) : 10 + 5 = 15  
Categorie 0, Operation 2 (multiplication) : 10 * 5 = 50  
Categorie 1, Operation 1 (modulo) : 10 % 3 = 1  
Categorie 1, Operation 2 (maximum) : max(10, 5) = 10
```

### 12_passage_tableau.c
- **Section** : 22.2 - Tableaux de pointeurs de fonctions
- **Description** : Passage de tableaux de pointeurs de fonctions en paramètre
- **Fichier source** : 02-tableaux-pointeurs-fonctions.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 12_passage_tableau.c -o 12_passage_tableau`
- **Sortie attendue** :
```
Traitement de 7 et 3 :
  Operation 0 : 10
  Operation 1 : 21
```

---

## Section 22.3 : Pointeurs multi-niveaux

### 13_pointeur_double.c
- **Section** : 22.3 - Pointeurs multi-niveaux
- **Description** : Concept de base du pointeur double (pointeur de pointeur)
- **Fichier source** : 03-pointeurs-multi-niveaux.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 13_pointeur_double.c -o 13_pointeur_double`
- **Sortie attendue** :
```
Valeur directe : 42  
Via ptr : 42  
Via pptr : 42

Adresses :  
Adresse de valeur : 0x... (variable)  
Contenu de ptr : 0x... (pointe vers valeur)  
Adresse de ptr : 0x...  
Contenu de pptr : 0x... (pointe vers ptr)
```

### 14_allouer_pointeur.c
- **Section** : 22.3 - Pointeurs multi-niveaux
- **Description** : Modifier un pointeur dans une fonction via pointeur double
- **Fichier source** : 03-pointeurs-multi-niveaux.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 14_allouer_pointeur.c -o 14_allouer_pointeur`
- **Sortie attendue** :
```
mon_ptr est toujours NULL ! (version incorrecte)  
Allocation reussie ! Valeur : 100
```

### 15_matrice_dynamique.c
- **Section** : 22.3 - Pointeurs multi-niveaux
- **Description** : Allocation et manipulation d'une matrice 2D dynamique
- **Fichier source** : 03-pointeurs-multi-niveaux.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 15_matrice_dynamique.c -o 15_matrice_dynamique`
- **Sortie attendue** :
```
Matrice 3x4 :
  0   1   2   3
  4   5   6   7
  8   9  10  11
```

### 16_matrice_contigue.c
- **Section** : 22.3 - Pointeurs multi-niveaux
- **Description** : Allocation contiguë d'une matrice 2D (cache-friendly)
- **Fichier source** : 03-pointeurs-multi-niveaux.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 16_matrice_contigue.c -o 16_matrice_contigue`
- **Sortie attendue** :
```
Matrice contigue 3x4 :
  0   1   2   3
  4   5   6   7
  8   9  10  11
```

### 17_tableau_chaines.c
- **Section** : 22.3 - Pointeurs multi-niveaux
- **Description** : Création et manipulation d'un tableau dynamique de chaînes
- **Fichier source** : 03-pointeurs-multi-niveaux.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 17_tableau_chaines.c -o 17_tableau_chaines`
- **Sortie attendue** :
```
Langages de programmation :
0: C
1: Python
2: Rust
```

### 18_pointeur_triple.c
- **Section** : 22.3 - Pointeurs multi-niveaux
- **Description** : Pointeur triple et tableau 3D dynamique (cube)
- **Fichier source** : 03-pointeurs-multi-niveaux.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 18_pointeur_triple.c -o 18_pointeur_triple`
- **Sortie attendue** :
```
Valeur via p1 : 42  
Valeur via p2 : 42  
Valeur via p3 : 42  
Nouvelle valeur : 100

--- Cube 3D dynamique ---
Couche 0 :
    0   1   2   3
    4   5   6   7
    8   9  10  11
Couche 1 :
   12  13  14  15
   16  17  18  19
   20  21  22  23
```

### 19_tri_chaines.c
- **Section** : 22.3 - Pointeurs multi-niveaux
- **Description** : Tri de chaînes de caractères par échange de pointeurs
- **Fichier source** : 03-pointeurs-multi-niveaux.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 19_tri_chaines.c -o 19_tri_chaines`
- **Sortie attendue** :
```
Avant tri :
  banane
  pomme
  orange
  fraise
  kiwi

Apres tri :
  banane
  fraise
  kiwi
  orange
  pomme
```

---

## Section 22.4 : Pointeurs opaques

### 20_personne/ (multi-fichiers)
- **Section** : 22.4 - Pointeurs opaques
- **Description** : Type opaque Personne avec encapsulation
- **Fichier source** : 04-pointeurs-opaques.md
- **Fichiers** : `personne.h`, `personne.c`, `main.c`
- **Compilation** :
```bash
cd 20_personne/  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -c personne.c -o personne.o  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -c main.c -o main.o  
gcc personne.o main.o -o personne_demo
```
- **Sortie attendue** :
```
[LOG] Salaire modifie pour Jean Dupont
Age : 30  
Salaire : 45000.00  
Personne : Jean Dupont, 30 ans
```

### 21_stack/ (multi-fichiers)
- **Section** : 22.4 - Pointeurs opaques
- **Description** : Pile (stack) opaque avec API publique
- **Fichier source** : 04-pointeurs-opaques.md
- **Fichiers** : `stack.h`, `stack.c`, `main.c`
- **Compilation** :
```bash
cd 21_stack/  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -c stack.c -o stack.o  
gcc -Wall -Wextra -Werror -pedantic -std=c17 -c main.c -o main.o  
gcc stack.o main.o -o stack_demo
```
- **Sortie attendue** :
```
Taille : 3  
Sommet : 30  
Depile : 30  
Depile : 20  
Depile : 10
```

---

## Section 22.5 : Fonctions variadiques

### 22_somme_variadique.c
- **Section** : 22.5 - Fonctions variadiques
- **Description** : Fonction somme avec nombre variable d'arguments
- **Fichier source** : 05-fonctions-variadiques.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 22_somme_variadique.c -o 22_somme_variadique`
- **Sortie attendue** :
```
Somme de 2 nombres : 30  
Somme de 4 nombres : 10  
Somme de 6 nombres : 105
```

### 23_log_message.c
- **Section** : 22.5 - Fonctions variadiques
- **Description** : Fonction de logging utilisant vprintf
- **Fichier source** : 05-fonctions-variadiques.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 23_log_message.c -o 23_log_message`
- **Sortie attendue** :
```
[INFO] Application demarree
[WARNING] Memoire faible : 85%
[ERROR] Fichier config.txt introuvable a la ligne 42
```

### 24_max_variadique.c
- **Section** : 22.5 - Fonctions variadiques
- **Description** : Trouver le maximum parmi un nombre variable d'entiers
- **Fichier source** : 05-fonctions-variadiques.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 24_max_variadique.c -o 24_max_variadique`
- **Sortie attendue** :
```
Max(3, 7, 2) = 7  
Max(5, 15, 8, 23, 12, 19) = 23
```

### 25_concatener.c
- **Section** : 22.5 - Fonctions variadiques
- **Description** : Concaténation de plusieurs chaînes avec fonction variadique
- **Fichier source** : 05-fonctions-variadiques.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 25_concatener.c -o 25_concatener`
- **Sortie attendue** :
```
Bonjour le monde !  
C est genial
```

### 26_mini_printf.c
- **Section** : 22.5 - Fonctions variadiques
- **Description** : Implémentation simplifiée de printf
- **Fichier source** : 05-fonctions-variadiques.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 26_mini_printf.c -o 26_mini_printf`
- **Sortie attendue** :
```
Nombre : 42, Texte : Hello, Char : A  
Pourcentage : 100%  
Melange : Jean a 30 ans et mesure 175cm
```

---

## Section 22.6 : va_list et macros variadiques

### 27_va_copy.c
- **Section** : 22.6 - va_list et macros variadiques
- **Description** : Utilisation de va_copy pour parcourir les arguments deux fois
- **Fichier source** : 06-va-list.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 27_va_copy.c -o 27_va_copy`
- **Sortie attendue** :
```
Premier parcours : 10 20 30 40  
Deuxieme parcours : 10 20 30 40
```

### 28_passer_va_list.c
- **Section** : 22.6 - va_list et macros variadiques
- **Description** : Passer va_list à des fonctions auxiliaires
- **Fichier source** : 06-va-list.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 28_passer_va_list.c -o 28_passer_va_list`
- **Sortie attendue** :
```
Somme : 60  
Somme doublee : 120
```

### 29_log_timestamp.c
- **Section** : 22.6 - va_list et macros variadiques
- **Description** : Wrapper de printf avec timestamp automatique
- **Fichier source** : 06-va-list.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 29_log_timestamp.c -o 29_log_timestamp`
- **Sortie attendue** (timestamp variable) :
```
[HH:MM:SS] Application demarree
[HH:MM:SS] Utilisateur admin connecte
[HH:MM:SS] Traitement de 42 fichiers
```

### 30_formater_message.c
- **Section** : 22.6 - va_list et macros variadiques
- **Description** : Formatage dans une chaîne avec vsnprintf
- **Fichier source** : 06-va-list.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 30_formater_message.c -o 30_formater_message`
- **Sortie attendue** :
```
Erreur 404 : Not Found  
Utilisateur: Alice, ID: 1001
```

### 31_macros_variadiques.c ⚠️ sans -pedantic
- **Section** : 22.6 - va_list et macros variadiques
- **Description** : Macros variadiques de base avec `__VA_ARGS__` et `##__VA_ARGS__`
- **Fichier source** : 06-va-list.md
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17 31_macros_variadiques.c -o 31_macros_variadiques`
- **Note** : Sans `-pedantic` (extension GNU `##__VA_ARGS__`)
- **Sortie attendue** :
```
--- Test AFFICHER ---
Bonjour  
x = 42  
x = 10, y = 20

--- Test LOG ---
Bonjour  
x = 42

--- Test PRINT_IF ---
Mode debug active  
Valeur : 42
```

### 32_debug_print.c ⚠️ sans -pedantic
- **Section** : 22.6 - va_list et macros variadiques
- **Description** : Macro de debug conditionnelle avec `__FILE__` et `__LINE__`
- **Fichier source** : 06-va-list.md
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17 32_debug_print.c -o 32_debug_print`
- **Note** : Sans `-pedantic` (extension GNU `##__VA_ARGS__`). Sortie sur stderr.
- **Sortie attendue** (stderr) :
```
[DEBUG] 32_debug_print.c:16: Application demarree
[DEBUG] 32_debug_print.c:17: Valeur de x : 42
[DEBUG] 32_debug_print.c:18: Calcul : 10 + 20 = 30
```

### 33_logging_niveaux.c ⚠️ sans -pedantic
- **Section** : 22.6 - va_list et macros variadiques
- **Description** : Système de logging par niveaux avec macros variadiques
- **Fichier source** : 06-va-list.md
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17 33_logging_niveaux.c -o 33_logging_niveaux`
- **Note** : Sans `-pedantic` (extension GNU `##__VA_ARGS__`)
- **Sortie attendue** (timestamp variable) :
```
[HH:MM:SS] [ INFO] Application demarree
[HH:MM:SS] [ WARN] Memoire faible : 85% utilise
[HH:MM:SS] [ERROR] Echec de connexion a 192.168.1.1:8080
```

### 34_logging_avance.c ⚠️ sans -pedantic
- **Section** : 22.6 - va_list et macros variadiques
- **Description** : Système de logging avancé combinant macros et fonctions variadiques
- **Fichier source** : 06-va-list.md
- **Compilation** : `gcc -Wall -Wextra -Werror -std=c17 34_logging_avance.c -o 34_logging_avance`
- **Note** : Sans `-pedantic` (extension GNU `##__VA_ARGS__`)
- **Sortie attendue** (timestamp variable) :
```
[HH:MM:SS] [ INFO] 34_logging_avance.c:75 (main): Application demarree
[HH:MM:SS] [ INFO] 34_logging_avance.c:68 (traiter_fichier): Traitement de document.txt en cours
[HH:MM:SS] [ WARN] 34_logging_avance.c:69 (traiter_fichier): Fichier volumineux : document.txt
[HH:MM:SS] [ERROR] 34_logging_avance.c:64 (traiter_fichier): Nom de fichier NULL !
[HH:MM:SS] [ INFO] 34_logging_avance.c:80 (main): Application terminee
```

---

## Résumé

| # | Fichier | Section | Description | Flags spéciaux |
|---|---------|---------|-------------|----------------|
| 01 | 01_pointeurs_fonctions_base.c | 22.1 | Pointeur de fonction base | |
| 02 | 02_callbacks_calculatrice.c | 22.1 | Callbacks calculatrice | |
| 03 | 03_tri_personnalise.c | 22.1 | Tri qsort callbacks | |
| 04 | 04_typedef_operations.c | 22.1 | Typedef pointeurs fonctions | |
| 05 | 05_systeme_evenements.c | 22.1 | Système d'événements | |
| 06 | 06_tableau_operations.c | 22.2 | Tableau de pointeurs fonctions | |
| 07 | 07_typedef_tableau.c | 22.2 | Typedef tableau fonctions | |
| 08 | 08_table_dispatch.c | 22.2 | Table de dispatch | |
| 09 | 09_fsm_porte.c | 22.2 | Machine à états (FSM) | sleep() |
| 10 | 10_handlers_creux.c | 22.2 | Tableaux creux | |
| 11 | 11_operations_2d.c | 22.2 | Tableau 2D fonctions | |
| 12 | 12_passage_tableau.c | 22.2 | Passage tableau fonctions | |
| 13 | 13_pointeur_double.c | 22.3 | Pointeur double | |
| 14 | 14_allouer_pointeur.c | 22.3 | Modifier pointeur via ** | |
| 15 | 15_matrice_dynamique.c | 22.3 | Matrice 2D dynamique | |
| 16 | 16_matrice_contigue.c | 22.3 | Matrice contiguë | |
| 17 | 17_tableau_chaines.c | 22.3 | Tableau de chaînes | |
| 18 | 18_pointeur_triple.c | 22.3 | Pointeur triple + cube 3D | |
| 19 | 19_tri_chaines.c | 22.3 | Tri de chaînes | |
| 20 | 20_personne/ | 22.4 | Type opaque Personne | multi-fichiers |
| 21 | 21_stack/ | 22.4 | Pile (stack) opaque | multi-fichiers |
| 22 | 22_somme_variadique.c | 22.5 | Somme variadique | |
| 23 | 23_log_message.c | 22.5 | Logging vprintf | |
| 24 | 24_max_variadique.c | 22.5 | Maximum variadique | |
| 25 | 25_concatener.c | 22.5 | Concaténation variadique | |
| 26 | 26_mini_printf.c | 22.5 | Mini printf | |
| 27 | 27_va_copy.c | 22.6 | va_copy double parcours | |
| 28 | 28_passer_va_list.c | 22.6 | Passer va_list | |
| 29 | 29_log_timestamp.c | 22.6 | Log avec timestamp | |
| 30 | 30_formater_message.c | 22.6 | Formatage vsnprintf | |
| 31 | 31_macros_variadiques.c | 22.6 | Macros ##__VA_ARGS__ | sans -pedantic |
| 32 | 32_debug_print.c | 22.6 | Debug __FILE__/__LINE__ | sans -pedantic |
| 33 | 33_logging_niveaux.c | 22.6 | Logging par niveaux | sans -pedantic |
| 34 | 34_logging_avance.c | 22.6 | Logging avancé complet | sans -pedantic |

**Total** : 34 programmes (32 fichiers .c standalone + 2 projets multi-fichiers = 40 fichiers)

**Programmes sans `-pedantic`** : 31, 32, 33, 34 (extension GNU `##__VA_ARGS__`)

**Projets multi-fichiers** : 20_personne/ (3 fichiers), 21_stack/ (3 fichiers)
