🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 26.1 Secure C Coding Standards (CERT C)

## Introduction

Les **CERT C Secure Coding Standards** sont un ensemble de règles et de recommandations établies par le CERT (Computer Emergency Response Team) de l'Université Carnegie Mellon. Ces standards visent à éliminer les pratiques de programmation dangereuses qui peuvent conduire à des vulnérabilités de sécurité dans les programmes écrits en C.

### Pourquoi les standards CERT C sont-ils essentiels ?

Le langage C offre une grande liberté au développeur, mais cette liberté s'accompagne de responsabilités. Sans règles strictes, il est facile de créer du code qui :
- Peut être exploité par des attaquants
- Provoque des comportements imprévisibles
- Conduit à des crashs ou des pertes de données
- Compromet la sécurité du système entier

Les standards CERT C ont été créés pour répondre à ces problèmes en fournissant des directives claires et testées.

---

## Qu'est-ce qu'un "Undefined Behavior" ?

Un concept central dans CERT C est celui du **comportement indéfini** (undefined behavior). C'est une situation où le standard du langage C ne spécifie pas ce qui doit se passer. Le résultat peut varier selon :
- Le compilateur utilisé
- La plateforme (processeur, OS)
- Les options de compilation
- Même l'heure d'exécution !

### Exemple simple de comportement indéfini

```c
int main(void) {
    int tableau[5];
    printf("%d\n", tableau[10]);  // ❌ Accès hors limites : undefined behavior
    return 0;
}
```

Ce code peut :
- Afficher une valeur aléatoire
- Crasher le programme
- Sembler fonctionner parfaitement
- Être exploité par un attaquant pour lire de la mémoire sensible

**Le problème** : Vous ne pouvez pas prédire ce qui va se passer.

---

## Les catégories principales de règles CERT C

Les standards CERT C sont organisés en catégories correspondant aux différents aspects du langage. Voici les principales :

### 1. **Préprocesseur (PRE)**

Règles concernant l'utilisation du préprocesseur C (`#define`, `#include`, etc.).

**Exemple de règle : PRE31-C**
*"Évitez les effets de bord dans les arguments de macros"*

```c
// ❌ Mauvais : effet de bord dans une macro
#define CARRE(x) ((x) * (x))

int i = 5;
int resultat = CARRE(i++);  // Danger ! i est incrémenté deux fois
// Expansion : ((i++) * (i++))
```

**Problème** : `i` est incrémenté deux fois, le résultat est imprévisible (comportement indéfini).

**Solution** :
```c
// ✅ Bon : utiliser une fonction inline
static inline int carre(int x) {
    return x * x;
}

int i = 5;
int resultat = carre(i++);  // i est incrémenté une seule fois
```

---

### 2. **Déclarations et Initialisation (DCL)**

Règles sur la manière de déclarer et initialiser les variables.

**Exemple de règle : DCL30-C**
*"Déclarez les objets avec la portée appropriée"*

```c
// ❌ Mauvais : portée globale sans nécessité
int compteur = 0;  // Accessible partout

void incrementer(void) {
    compteur++;
}

void autre_fonction(void) {
    compteur = 999;  // Modification accidentelle possible
}
```

**Solution** :
```c
// ✅ Bon : portée limitée
static int compteur = 0;  // Visible uniquement dans ce fichier

// Ou encore mieux : portée locale
void incrementer(void) {
    static int compteur = 0;  // Variable persistante mais locale à la fonction
    compteur++;
}
```

---

### 3. **Expressions (EXP)**

Règles concernant l'écriture d'expressions sûres.

**Exemple de règle : EXP33-C**
*"Ne pas lire de variables non initialisées"*

```c
// ❌ Mauvais
void traiter_donnees(void) {
    int valeur;  // Non initialisée !

    if (condition_complexe()) {
        valeur = 42;
    }
    // Si condition_complexe() retourne false, valeur n'est pas initialisée

    printf("Valeur : %d\n", valeur);  // ❌ Danger !
}
```

**Solution** :
```c
// ✅ Bon : initialisation systématique
void traiter_donnees(void) {
    int valeur = 0;  // Valeur par défaut sûre

    if (condition_complexe()) {
        valeur = 42;
    }

    printf("Valeur : %d\n", valeur);  // ✅ Sûr
}
```

---

### 4. **Entiers (INT)**

Règles pour éviter les problèmes liés aux opérations sur les entiers.

**Exemple de règle : INT30-C**
*"S'assurer que les opérations sur entiers non signés n'enveloppent pas"*

```c
// ❌ Mauvais : débordement d'entier non détecté
void allouer_memoire(size_t nb_elements) {
    size_t taille = nb_elements * sizeof(int);
    int *tableau = malloc(taille);  // Si débordement, alloue moins que nécessaire !

    // Utilisation du tableau...
}

// Si nb_elements est très grand, nb_elements * sizeof(int) peut déborder
// Exemple : sur un système 32 bits, si nb_elements = 1073741824 (2^30)
// et sizeof(int) = 4, alors 1073741824 * 4 = 4294967296 = 0 (débordement !)
```

**Solution** :
```c
// ✅ Bon : vérification du débordement
#include <stdint.h>
#include <limits.h>

void allouer_memoire(size_t nb_elements) {
    // Vérification avant la multiplication
    if (nb_elements > SIZE_MAX / sizeof(int)) {
        fprintf(stderr, "Erreur : trop d'éléments demandés\n");
        return;
    }

    size_t taille = nb_elements * sizeof(int);
    int *tableau = malloc(taille);

    if (tableau == NULL) {
        fprintf(stderr, "Erreur d'allocation\n");
        return;
    }

    // Utilisation sûre du tableau...
    free(tableau);
}
```

---

### 5. **Nombres à virgule flottante (FLP)**

Règles pour la manipulation sûre des `float` et `double`.

**Exemple de règle : FLP30-C**
*"Ne pas utiliser de valeurs flottantes comme compteurs de boucle"*

```c
// ❌ Mauvais : imprécision des flottants
for (float x = 0.0f; x <= 1.0f; x += 0.1f) {
    printf("%f\n", x);
}
// Problème : 0.1 n'est pas représentable exactement en binaire
// La boucle peut ne pas s'exécuter exactement 11 fois comme prévu
```

**Solution** :
```c
// ✅ Bon : utiliser des entiers
for (int i = 0; i <= 10; i++) {
    float x = i * 0.1f;
    printf("%f\n", x);
}
```

---

### 6. **Tableaux (ARR)**

Règles pour éviter les erreurs courantes avec les tableaux.

**Exemple de règle : ARR30-C**
*"Ne pas former ou utiliser de pointeurs hors limites"*

```c
// ❌ Mauvais
int tableau[10];
int *ptr = tableau + 15;  // Pointe au-delà du tableau !

if (condition) {
    *ptr = 42;  // Écriture hors limites = corruption mémoire
}
```

**Solution** :
```c
// ✅ Bon : rester dans les limites
int tableau[10];
size_t index = obtenir_index();

if (index < 10) {  // Vérification des limites
    tableau[index] = 42;
} else {
    fprintf(stderr, "Erreur : index hors limites\n");
}
```

---

### 7. **Chaînes de caractères (STR)**

Règles spécifiques aux chaînes C (qui sont des sources majeures de vulnérabilités).

**Exemple de règle : STR31-C**
*"Garantir que les buffers de stockage pour chaînes ont une taille suffisante"*

```c
// ❌ Mauvais : risque de buffer overflow
void copier_nom(const char *source) {
    char nom[10];
    strcpy(nom, source);  // Danger ! Si source > 9 caractères
    printf("Nom : %s\n", nom);
}

// Si source = "JeanFrancoisDupont", débordement du buffer !
```

**Solution** :
```c
// ✅ Bon : utilisation de fonctions sécurisées
void copier_nom(const char *source) {
    char nom[10];
    strncpy(nom, source, sizeof(nom) - 1);  // Limite la copie
    nom[sizeof(nom) - 1] = '\0';  // Garantit la terminaison null
    printf("Nom : %s\n", nom);
}

// Encore mieux : avec les fonctions C11
#ifdef __STDC_LIB_EXT1__
void copier_nom_securise(const char *source) {
    char nom[10];
    if (strcpy_s(nom, sizeof(nom), source) != 0) {
        fprintf(stderr, "Erreur : chaîne trop longue\n");
        return;
    }
    printf("Nom : %s\n", nom);
}
#endif
```

---

### 8. **Mémoire (MEM)**

Règles pour la gestion sûre de la mémoire dynamique.

**Exemple de règle : MEM30-C**
*"Ne pas accéder à de la mémoire libérée"*

```c
// ❌ Mauvais : use-after-free
int *ptr = malloc(sizeof(int) * 10);
*ptr = 42;
free(ptr);

printf("%d\n", *ptr);  // ❌ Accès à mémoire libérée !
ptr[5] = 100;          // ❌ Encore plus dangereux
```

**Solution** :
```c
// ✅ Bon : mise à NULL après free
int *ptr = malloc(sizeof(int) * 10);
if (ptr == NULL) {
    return;  // Gestion d'erreur
}

*ptr = 42;
free(ptr);
ptr = NULL;  // Évite les accès accidentels

// Si on essaie d'accéder :
if (ptr != NULL) {
    printf("%d\n", *ptr);  // Ne s'exécute pas
}
```

---

### 9. **Entrées/Sorties (FIO)**

Règles pour les opérations de fichiers et I/O sécurisées.

**Exemple de règle : FIO30-C**
*"Exclure les entrées utilisateur des chaînes de format"*

```c
// ❌ TRÈS DANGEREUX : Format string vulnerability
void afficher_message(const char *entree_utilisateur) {
    printf(entree_utilisateur);  // ❌ VULNÉRABILITÉ CRITIQUE !
}

// Un attaquant peut fournir : "%s%s%s%s%s" ou "%n" pour crasher ou exploiter
```

**Solution** :
```c
// ✅ Bon : format fixe
void afficher_message(const char *entree_utilisateur) {
    printf("%s", entree_utilisateur);  // Format contrôlé
}

// Ou encore mieux : avec limite de longueur
void afficher_message_limite(const char *entree_utilisateur) {
    printf("%.100s", entree_utilisateur);  // Maximum 100 caractères
}
```

---

### 10. **Environnement (ENV)**

Règles concernant l'interaction avec l'environnement d'exécution.

**Exemple de règle : ENV33-C**
*"Ne pas appeler system() avec des données non validées"*

```c
// ❌ EXTRÊMEMENT DANGEREUX : Injection de commande
void ouvrir_fichier(const char *nom_fichier) {
    char commande[256];
    sprintf(commande, "cat %s", nom_fichier);
    system(commande);  // ❌ VULNÉRABILITÉ D'INJECTION
}

// Un attaquant peut fournir : "fichier.txt; rm -rf /"
// La commande devient : "cat fichier.txt; rm -rf /"
```

**Solution** :
```c
// ✅ Bon : éviter system() et valider les entrées
void ouvrir_fichier(const char *nom_fichier) {
    // Validation stricte du nom de fichier
    for (size_t i = 0; nom_fichier[i] != '\0'; i++) {
        if (!isalnum(nom_fichier[i]) && nom_fichier[i] != '.' && nom_fichier[i] != '_') {
            fprintf(stderr, "Erreur : nom de fichier invalide\n");
            return;
        }
    }

    // Utilisation d'API sûres
    FILE *fichier = fopen(nom_fichier, "r");
    if (fichier == NULL) {
        perror("Erreur d'ouverture");
        return;
    }

    // Lecture et affichage du contenu
    char ligne[256];
    while (fgets(ligne, sizeof(ligne), fichier) != NULL) {
        printf("%s", ligne);
    }

    fclose(fichier);
}
```

---

## Niveaux de sévérité CERT C

Chaque règle CERT C est classée selon trois critères :

### 1. **Sévérité** (Severity)
L'impact si la règle est violée :
- **Haute** : Peut conduire à l'exécution de code arbitraire
- **Moyenne** : Peut conduire à un déni de service
- **Basse** : Peut conduire à un comportement incorrect

### 2. **Probabilité** (Likelihood)
La probabilité qu'une violation soit exploitée :
- **Haute** : Facilement exploitable
- **Moyenne** : Exploitable dans certaines conditions
- **Basse** : Difficile à exploiter

### 3. **Coût de remédiation** (Remediation Cost)
L'effort nécessaire pour corriger le problème :
- **Haut** : Nécessite une refonte majeure
- **Moyen** : Modifications significatives
- **Bas** : Correction simple et rapide

### Exemple de priorité

**Règle STR31-C** (Buffer overflow dans les chaînes) :
- Sévérité : **HAUTE** (exécution de code arbitraire possible)
- Probabilité : **HAUTE** (vulnérabilité commune)
- Coût : **BAS** (utiliser `strncpy` au lieu de `strcpy`)

→ **Priorité maximale** : À corriger immédiatement !

---

## Comment appliquer CERT C dans vos projets

### 1. **Analyseurs statiques**

Utilisez des outils qui détectent automatiquement les violations CERT C :

```bash
# Installation de cppcheck (supporte CERT C)
sudo apt install cppcheck

# Analyse avec règles CERT
cppcheck --enable=all --cert=all --suppress=missingIncludeSystem mon_code.c
```

```bash
# clang-tidy avec vérifications de sécurité
clang-tidy mon_code.c -checks='cert-*,clang-analyzer-*' -- -std=c11
```

### 2. **Options de compilation strictes**

Activez tous les warnings et traitez-les comme des erreurs :

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c11 \
    -Wformat=2 -Wformat-security \
    -Wstrict-overflow=5 -Warray-bounds \
    mon_code.c -o mon_programme
```

### 3. **Code reviews**

- Créez une checklist basée sur CERT C
- Vérifiez particulièrement les zones à risque :
  - Manipulation de chaînes
  - Allocation dynamique
  - Calculs arithmétiques
  - Entrées utilisateur

### 4. **Tests et fuzzing**

Testez votre code avec des entrées malveillantes :

```bash
# Fuzzing avec AFL (American Fuzzy Lop)
afl-gcc mon_programme.c -o mon_programme
afl-fuzz -i entrees_test -o resultats ./mon_programme @@
```

---

## Les erreurs les plus courantes à éviter

### Top 5 des violations CERT C en production

1. **Buffer overflow** (STR31-C, ARR30-C)
   - Toujours vérifier les limites
   - Utiliser les variantes sécurisées (`strncpy`, `snprintf`)

2. **Use-after-free** (MEM30-C)
   - Mettre les pointeurs à `NULL` après `free()`
   - Utiliser des outils comme Valgrind

3. **Integer overflow** (INT30-C)
   - Vérifier les débordements avant les opérations
   - Utiliser les types appropriés (`size_t`, `ssize_t`)

4. **Format string vulnerabilities** (FIO30-C)
   - Ne jamais passer de données utilisateur comme format
   - Toujours utiliser `printf("%s", data)` et non `printf(data)`

5. **Uninitialized variables** (EXP33-C)
   - Initialiser toutes les variables à la déclaration
   - Activer les warnings du compilateur

---

## Ressources officielles

### Documentation CERT C

- **Site officiel** : [wiki.sei.cmu.edu/confluence/display/c](https://wiki.sei.cmu.edu/confluence/display/c)
- **Guide PDF** : "SEI CERT C Coding Standard"
- **Liste complète** : Plus de 100 règles détaillées avec exemples

### Standards connexes

- **ISO/IEC TS 17961** : Annexe C sécurisé (basé sur CERT C)
- **MISRA C** : Standard pour l'embarqué critique (automobile, aéronautique)
- **CWE** (Common Weakness Enumeration) : Classification des faiblesses logicielles

---

## Conclusion

Les **CERT C Secure Coding Standards** ne sont pas de simples recommandations théoriques : ce sont des règles éprouvées qui :

- ✅ **Préviennent** les vulnérabilités de sécurité
- ✅ **Améliorent** la fiabilité du code
- ✅ **Réduisent** les coûts de maintenance
- ✅ **Protègent** les utilisateurs et les systèmes

### Message clé pour les débutants

Vous n'avez pas besoin de mémoriser toutes les règles CERT C dès maintenant. Commencez par :

1. **Toujours initialiser vos variables**
2. **Vérifier les limites des tableaux**
3. **Utiliser les fonctions sécurisées** (`strncpy`, `snprintf`)
4. **Activer tous les warnings du compilateur**
5. **Tester avec Valgrind et les sanitizers**

Au fil du temps, ces bonnes pratiques deviendront naturelles, et votre code sera plus sûr et plus robuste.

---

**💡 Conseil** : Gardez un signet vers le wiki CERT C et consultez-le régulièrement. Chaque règle est expliquée avec des exemples concrets de code vulnérable et sécurisé.

**🔐 Sécurité = Discipline + Outils + Connaissances**

⏭️ [Validation des entrées](/26-securite-code-defensif/02-validation-entrees.md)
