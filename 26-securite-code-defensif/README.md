🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 26. Sécurité et Code défensif

## Introduction au chapitre

La sécurité en programmation C n'est pas un luxe, c'est une **nécessité absolue**. Le langage C, de par sa puissance et sa proximité avec le matériel, offre aux développeurs une liberté exceptionnelle. Mais comme le dit l'adage :

> **"Un grand pouvoir implique de grandes responsabilités"**

Ce chapitre vous enseignera comment écrire du code C **défensif** et **sécurisé**, capable de résister aux attaques et de protéger les données des utilisateurs.

---

## Pourquoi la sécurité est-elle critique en C ?

### 1. Le C ne vous protège pas

Contrairement à des langages modernes comme Python, Java ou Rust, le C **ne vérifie pas** automatiquement :

```c
int tableau[10];
tableau[100] = 42;  // ❌ Pas d'erreur au compile-time !
                    // ❌ Pas d'erreur au runtime (comportement indéfini)
                    // ✅ En Rust : erreur de compilation
                    // ✅ En Python : exception IndexError
```

**En C, vous êtes le gardien de votre propre sécurité.**

### 2. Les conséquences des bugs de sécurité

#### Impact personnel
- **Perte de données** : Documents, photos, travail
- **Vol d'identité** : Accès aux comptes bancaires, emails
- **Compromission de la vie privée** : Données personnelles exposées

#### Impact professionnel
- **Coûts financiers** : Réparation, amendes, poursuites
- **Perte de réputation** : Confiance des clients détruite
- **Responsabilité légale** : RGPD, PCI-DSS, etc.

#### Impact sociétal
- **Infrastructures critiques** : Hôpitaux, centrales électriques
- **Sécurité nationale** : Systèmes militaires, gouvernementaux
- **Effet domino** : Une faille peut affecter des millions de personnes

### 3. Exemples historiques de vulnérabilités

#### Heartbleed (2014)
**Langage** : C
**Type** : Buffer over-read dans OpenSSL
**Impact** : 500 000+ serveurs compromis, millions de mots de passe exposés
**Cause** : Validation insuffisante de la longueur d'un buffer

```c
// Code vulnérable (simplifié)
void traiter_heartbeat(char *payload, int longueur_declaree) {
    char buffer[64];
    // Bug : pas de vérification si longueur_declaree > taille réelle
    memcpy(buffer, payload, longueur_declaree);  // ❌ Buffer over-read
    send(buffer, longueur_declaree);
}
```

**Coût estimé** : > 500 millions de dollars

#### WannaCry (2017)
**Langage** : Exploite une faille dans du code C (Windows SMB)
**Type** : Buffer overflow
**Impact** : 230 000+ ordinateurs dans 150 pays, hôpitaux paralysés
**Cause** : Buffer overflow non patché

**Coût estimé** : > 4 milliards de dollars

#### Shellshock (2014)
**Langage** : C (Bash)
**Type** : Command injection
**Impact** : Millions de serveurs Linux/Unix vulnérables
**Cause** : Parsing incorrect des variables d'environnement

**Coût estimé** : > 100 millions de dollars

#### Stuxnet (2010)
**Langage** : Exploite des failles C dans Windows
**Type** : Multiple zero-days
**Impact** : Sabotage de centrifugeuses nucléaires iraniennes
**Sophistication** : Considéré comme l'attaque cyber la plus avancée de l'histoire

---

## Le principe du code défensif

### Définition

Le **code défensif** est une approche de programmation où vous :
1. **Anticipez les erreurs** avant qu'elles n'arrivent
2. **Validez toutes les entrées** avant de les utiliser
3. **Vérifiez tous les cas limites** (edge cases)
4. **Ne faites confiance à rien** venant de l'extérieur
5. **Limitez les dégâts** en cas de faille

### Analogie : La sécurité routière

**Code non défensif** (conduite agressive) :
```
Rouler sans ceinture
Ne pas vérifier les angles morts
Ignorer les feux
Conduire en état d'ivresse
→ Accident presque garanti
```

**Code défensif** (conduite défensive) :
```
Boucler sa ceinture
Vérifier les rétroviseurs
Respecter le code de la route
Anticiper les erreurs des autres conducteurs
→ Probabilité d'accident minimisée
```

De même en programmation :
```
Code non défensif : Supposer que tout va bien
Code défensif : Vérifier que tout va bien
```

### Les 5 piliers du code défensif

#### 1. Ne jamais faire confiance aux données externes

```c
// ❌ Code non défensif
void traiter_age(char *input) {
    int age = atoi(input);  // Suppose que input est valide
    printf("Vous avez %d ans\n", age);
}

// ✅ Code défensif
void traiter_age(char *input) {
    if (input == NULL) {
        fprintf(stderr, "Erreur : entrée nulle\n");
        return;
    }

    char *endptr;
    errno = 0;
    long age = strtol(input, &endptr, 10);

    if (errno != 0 || endptr == input || *endptr != '\0') {
        fprintf(stderr, "Erreur : âge invalide\n");
        return;
    }

    if (age < 0 || age > 150) {
        fprintf(stderr, "Erreur : âge non réaliste\n");
        return;
    }

    printf("Vous avez %ld ans\n", age);
}
```

#### 2. Vérifier tous les codes de retour

```c
// ❌ Code non défensif
FILE *f = fopen("config.txt", "r");
char buffer[256];
fgets(buffer, sizeof(buffer), f);  // Crash si f == NULL !

// ✅ Code défensif
FILE *f = fopen("config.txt", "r");
if (f == NULL) {
    perror("Impossible d'ouvrir config.txt");
    return -1;
}

char buffer[256];
if (fgets(buffer, sizeof(buffer), f) == NULL) {
    fprintf(stderr, "Erreur de lecture\n");
    fclose(f);
    return -1;
}

fclose(f);
```

#### 3. Limiter la portée et les privilèges

```c
// ❌ Variables globales accessibles partout
int compteur_global = 0;

void fonction1(void) { compteur_global++; }
void fonction2(void) { compteur_global = 0; }  // Modification accidentelle

// ✅ Portée limitée
static int compteur = 0;  // Visible uniquement dans ce fichier

int obtenir_compteur(void) { return compteur; }
void incrementer_compteur(void) { compteur++; }
```

#### 4. Fail securely (échouer de manière sécurisée)

```c
// ❌ Échec non sécurisé
bool verifier_mot_de_passe(const char *mot_de_passe) {
    if (mot_de_passe == NULL) {
        return true;  // ❌ Accès accordé en cas d'erreur !
    }
    return strcmp(mot_de_passe, "secret123") == 0;
}

// ✅ Échec sécurisé
bool verifier_mot_de_passe(const char *mot_de_passe) {
    if (mot_de_passe == NULL) {
        return false;  // ✅ Accès refusé en cas d'erreur
    }
    return strcmp(mot_de_passe, "secret123") == 0;
}
```

#### 5. Défense en profondeur

Ne comptez pas sur une seule protection :

```
Couche 1 : Validation des entrées
    ↓ (si contournée)
Couche 2 : Vérification des limites
    ↓ (si contournée)
Couche 3 : Sanitizers et vérifications runtime
    ↓ (si contournée)
Couche 4 : Permissions minimales
    ↓ (si contournée)
Couche 5 : Monitoring et alertes
```

---

## Les types de vulnérabilités en C

### 1. Vulnérabilités mémoire

#### Buffer Overflow
```c
char buffer[10];
strcpy(buffer, "Une chaîne beaucoup trop longue");  // ❌ Débordement
```

**Conséquences** :
- Corruption de données adjacentes
- Crash du programme
- Exécution de code arbitraire

#### Use-After-Free
```c
int *ptr = malloc(sizeof(int));
free(ptr);
*ptr = 42;  // ❌ Accès à mémoire libérée
```

**Conséquences** :
- Corruption de mémoire
- Comportement imprévisible
- Exploitation possible

#### Double Free
```c
int *ptr = malloc(sizeof(int));
free(ptr);
free(ptr);  // ❌ Double libération
```

**Conséquences** :
- Corruption du heap
- Crash du programme

### 2. Vulnérabilités d'injection

#### Format String Vulnerability
```c
printf(user_input);  // ❌ L'utilisateur contrôle le format
```

**Exemple d'exploitation** :
```bash
$ ./programme "%x %x %x %x"
bffff760 8048480 bffff758 80484a0  # Fuite de mémoire !
```

#### Command Injection
```c
char cmd[256];
sprintf(cmd, "cat %s", filename);
system(cmd);  // ❌ Si filename = "file.txt; rm -rf /"
```

**Conséquences** : Exécution de commandes arbitraires

### 3. Vulnérabilités arithmétiques

#### Integer Overflow
```c
size_t taille = nb_elements * sizeof(int);  // Peut overflow !
void *buffer = malloc(taille);  // Alloue trop peu de mémoire
```

**Conséquences** :
- Allocation insuffisante
- Buffer overflow secondaire

#### Integer Underflow
```c
unsigned int compteur = 0;
compteur--;  // Devient UINT_MAX (4 294 967 295)
```

### 4. Vulnérabilités de logique

#### Race Conditions
```c
if (access(filename, R_OK) == 0) {
    // ⚠️ Fenêtre de temps exploitable
    FILE *f = fopen(filename, "r");  // Le fichier peut avoir changé
}
```

#### TOCTOU (Time-Of-Check-Time-Of-Use)
```c
if (fichier_existe("config.txt")) {
    // Entre ces deux lignes, un attaquant peut créer un lien symbolique
    copier_fichier("config.txt", "/etc/passwd");  // ❌ Escalade de privilèges
}
```

---

## Vue d'ensemble du chapitre

Ce chapitre est organisé en 8 sections progressives qui couvrent tous les aspects de la sécurité en C :

### Section 26.1 : Secure C Coding Standards (CERT C)
- 📚 **Objectif** : Apprendre les standards de l'industrie
- 🎯 **Vous apprendrez** :
    - Les règles CERT C
    - Les patterns à éviter
    - Les bonnes pratiques recommandées

### Section 26.2 : Validation des entrées
- 🔍 **Objectif** : Ne jamais faire confiance aux données externes
- 🎯 **Vous apprendrez** :
    - Types de validation (type, plage, longueur, format)
    - Techniques de validation
    - Listes blanches vs listes noires

### Section 26.3 : Prévention des buffer overflows
- 🛡️ **Objectif** : Éliminer la vulnérabilité #1
- 🎯 **Vous apprendrez** :
    - Comment se produisent les buffer overflows
    - Fonctions dangereuses à éviter
    - Alternatives sécurisées
    - Protection au niveau du compilateur

### Section 26.4 : Format string vulnerabilities
- ⚠️ **Objectif** : Comprendre et prévenir les attaques par format string
- 🎯 **Vous apprendrez** :
    - Fonctionnement de printf
    - Exploitation de format strings
    - Protection simple : toujours `printf("%s", data)`

### Section 26.5 : Integer overflow
- 🔢 **Objectif** : Gérer les débordements arithmétiques
- 🎯 **Vous apprendrez** :
    - Comment se produisent les overflows
    - Vérification avant opération
    - Builtins GCC pour détection

### Section 26.6 : Principe du moindre privilège
- 🔐 **Objectif** : Limiter les dégâts en cas de compromission
- 🎯 **Vous apprendrez** :
    - Abandonner les privilèges root
    - Capabilities Linux
    - Isolation et sandboxing

### Section 26.7 : Static analysis et fuzzing
- 🔬 **Objectif** : Détecter les bugs avant la production
- 🎯 **Vous apprendrez** :
    - Analyse statique (cppcheck, clang-tidy)
    - Fuzzing (AFL, libFuzzer)
    - Intégration dans CI/CD

### Section 26.8 : Compilation avec hardening flags
- 🛡️ **Objectif** : Activer les protections du compilateur
- 🎯 **Vous apprendrez** :
    - Stack canaries
    - FORTIFY_SOURCE
    - PIE/ASLR
    - RELRO, NX bit

---

## Prérequis pour ce chapitre

### Connaissances techniques

Avant d'aborder ce chapitre, vous devriez maîtriser :

✅ **Module 2 : Fondamentaux du langage**
- Types de données
- Pointeurs de base
- Fonctions

✅ **Module 3 : Gestion de la mémoire**
- Stack vs Heap
- malloc/free
- Pointeurs avancés

✅ **Module 4 : Structures de données**
- Tableaux
- Chaînes de caractères
- Structures

✅ **Module 5 : Outillage DevOps** (recommandé)
- Compilation avec GCC
- Makefile
- Debugging avec GDB

### État d'esprit

Plus important que les connaissances techniques, vous devez adopter un **état d'esprit de sécurité** :

🧠 **Pensez comme un attaquant**
- Où pourrait-on injecter des données malveillantes ?
- Que se passe-t-il dans les cas limites ?
- Quelles hypothèses ai-je faites qui pourraient être fausses ?

🔍 **Soyez paranoïaque (dans le bon sens)**
- Validez tout
- Vérifiez tous les codes de retour
- Ne faites confiance à rien

⚖️ **Équilibrez sécurité et praticité**
- La sécurité parfaite n'existe pas
- Identifiez les risques les plus critiques
- Appliquez les protections proportionnées au risque

---

## Comment aborder ce chapitre

### Approche recommandée

#### 1. Lecture active (ne pas juste lire)

```
❌ Lire passivement
    "Ah oui, les buffer overflows, je connais"

✅ Expérimenter activement
    "Créons volontairement un buffer overflow pour voir ce qui se passe"
    "Testons les protections du compilateur"
```

#### 2. Pratiquer sur du vrai code

Pour chaque section :
1. **Lire** les concepts
2. **Reproduire** les exemples vulnérables
3. **Exploiter** (en local !) pour comprendre l'impact
4. **Corriger** avec les techniques apprises
5. **Vérifier** avec les outils (sanitizers, analyseurs statiques)

#### 3. Créer une checklist personnelle

Au fur et à mesure, construisez votre propre checklist de sécurité :

```markdown
## Ma Checklist de Sécurité C

### Avant d'écrire du code
- [ ] Ai-je identifié toutes les entrées externes ?
- [ ] Ai-je pensé aux cas limites ?

### Pendant l'écriture
- [ ] Pas d'utilisation de gets(), strcpy(), sprintf()
- [ ] Validation de toutes les entrées
- [ ] Vérification de tous les malloc()

### Avant de commit
- [ ] Compilation sans warnings (-Wall -Wextra -Werror)
- [ ] cppcheck passé
- [ ] Tests avec Valgrind

### Avant la release
- [ ] Analyse statique complète
- [ ] Fuzzing effectué
- [ ] Hardening flags activés
```

#### 4. Apprendre de vos erreurs

Gardez un **journal de vos bugs** :

```
Date : 2025-01-15
Bug : Buffer overflow dans parse_config()
Cause : Utilisation de strcpy() sans vérification
Impact : Crash du programme
Leçon : Toujours utiliser strncpy() avec terminaison null
Fix : Remplacé par validation + strncpy()
```

---

## Mindset du développeur sécurisé

### Les 10 commandements du code sécurisé

1. **Tu ne feras confiance à aucune entrée externe**
   - Tout ce qui vient de l'utilisateur est suspect

2. **Tu valideras avant de traiter**
   - Type, longueur, format, plage, sémantique

3. **Tu vérifieras tous les codes de retour**
   - malloc(), fopen(), scanf(), etc.

4. **Tu ne supposeras rien**
   - Les pointeurs peuvent être NULL
   - Les tableaux peuvent être vides
   - Les fichiers peuvent ne pas exister

5. **Tu utiliseras les bonnes fonctions**
   - strncpy() au lieu de strcpy()
   - snprintf() au lieu de sprintf()
   - fgets() au lieu de gets()

6. **Tu compileras avec tous les warnings**
   - -Wall -Wextra -Werror dès le jour 1

7. **Tu testeras les cas limites**
   - Chaîne vide, NULL, 0, INT_MAX, etc.

8. **Tu documenteras tes hypothèses**
   - Quelles sont les entrées valides ?
   - Quelles sont les préconditions ?

9. **Tu utiliseras les outils**
   - Sanitizers, Valgrind, analyseurs statiques

10. **Tu apprendras continuellement**
    - Nouvelles vulnérabilités, nouveaux outils

### Citations inspirantes

> "Security is not a product, but a process."
> — Bruce Schneier, expert en sécurité

> "The only truly secure system is one that is powered off, cast in a block of concrete and sealed in a lead-lined room with armed guards."
> — Gene Spafford, professeur en sécurité informatique

> "If you think technology can solve your security problems, then you don't understand the problems and you don't understand the technology."
> — Bruce Schneier

---

## Outils et ressources

### Outils que vous utiliserez dans ce chapitre

| Outil | Utilité | Installation |
|-------|---------|--------------|
| **GCC/Clang** | Compilation avec warnings | Pré-installé |
| **Valgrind** | Détection de fuites mémoire | `apt install valgrind` |
| **AddressSanitizer** | Détection d'erreurs mémoire | Flag `-fsanitize=address` |
| **cppcheck** | Analyse statique | `apt install cppcheck` |
| **clang-tidy** | Linting avancé | `apt install clang-tidy` |
| **AFL** | Fuzzing | `apt install afl++` |
| **checksec** | Vérification hardening | `apt install checksec` |

### Ressources externes

#### Documentation officielle
- **CERT C** : https://wiki.sei.cmu.edu/confluence/display/c
- **CWE** : https://cwe.mitre.org/ (Common Weakness Enumeration)
- **OWASP** : https://owasp.org/ (Sécurité web)

#### Livres recommandés
- "The Art of Software Security Assessment" (Dowd, McDonald, Schuh)
- "Secure Coding in C and C++" (Seacord)
- "Hacking: The Art of Exploitation" (Erickson)

#### Cours en ligne
- Stanford CS253: Web Security
- MIT 6.858: Computer Systems Security
- Coursera: Software Security

#### Challenges et CTF
- **picoCTF** : https://picoctf.org/ (débutants)
- **pwnable.kr** : https://pwnable.kr/ (binaire)
- **OverTheWire** : https://overthewire.org/ (Linux)
- **HackTheBox** : https://www.hackthebox.com/ (avancé)

---

## Structure du code dans ce chapitre

### Convention de notation

Tout au long de ce chapitre, nous utiliserons ces symboles :

```c
// ❌ Code vulnérable ou incorrect
char buffer[10];
strcpy(buffer, user_input);

// ✅ Code sécurisé et correct
char buffer[10];
strncpy(buffer, user_input, sizeof(buffer) - 1);
buffer[sizeof(buffer) - 1] = '\0';

// ⚠️ Code ambigu ou partiellement sécurisé
char buffer[10];
strncpy(buffer, user_input, 10);  // Oubli de la terminaison null

// 💡 Conseil ou astuce
// Conseil : Toujours initialiser vos variables
int x = 0;  // 💡 Évite les valeurs indéterminées
```

### Exemples progressifs

Chaque concept sera présenté en trois étapes :

1. **Version vulnérable** (pour comprendre le problème)
2. **Version partiellement corrigée** (approche naïve)
3. **Version robuste** (solution professionnelle)

**Exemple** :

```c
// 1. Version vulnérable
void copier(char *dest, char *src) {
    strcpy(dest, src);  // ❌ Buffer overflow possible
}

// 2. Version partiellement corrigée
void copier(char *dest, char *src, size_t max) {
    strncpy(dest, src, max);  // ⚠️ Oubli de '\0'
}

// 3. Version robuste
bool copier(char *dest, size_t dest_size, const char *src) {
    if (dest == NULL || src == NULL || dest_size == 0) {
        return false;
    }

    size_t src_len = strlen(src);
    if (src_len >= dest_size) {
        return false;  // Source trop grande
    }

    strncpy(dest, src, dest_size - 1);
    dest[dest_size - 1] = '\0';
    return true;
}
```

---

## Exercice de mise en condition

### Avant de commencer, testez votre vulnérabilité

Voici un petit programme. **Sans l'exécuter**, pouvez-vous identifier les vulnérabilités ?

```c
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void traiter_commande(char *cmd) {
    char buffer[64];
    sprintf(buffer, "echo %s", cmd);
    system(buffer);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s <commande>\n", argv[0]);
        return 1;
    }

    int taille = atoi(argv[1]);
    char *data = malloc(taille);

    printf("Entrez votre nom : ");
    gets(data);

    traiter_commande(argv[2]);

    free(data);
    return 0;
}
```

<details>
<summary>Cliquez pour voir les vulnérabilités (après avoir cherché !)</summary>

**Vulnérabilités identifiées** :

1. **`gets(data)`** : Buffer overflow garanti (gets est obsolète et dangereux)
2. **`sprintf(buffer, "echo %s", cmd)`** : Buffer overflow si cmd > 59 caractères
3. **`system(buffer)`** : Command injection (ex: cmd = "test; rm -rf /")
4. **`atoi(argv[1])`** : Pas de validation (peut être négatif ou énorme)
5. **`malloc(taille)`** : Pas de vérification si malloc échoue
6. **Pas de vérification** que argc >= 3 avant d'accéder à argv[2]
7. **`free(data)`** : Si malloc a échoué, free(NULL) (OK mais data jamais utilisé)

**Score** :
- 0-2 vulnérabilités trouvées : Lisez attentivement ce chapitre !
- 3-4 vulnérabilités : Bon œil, continuez à progresser
- 5-6 vulnérabilités : Excellent, ce chapitre affinera vos connaissances
- 7 vulnérabilités : Vous êtes prêt à devenir un expert en sécurité !

</details>

---

## Message aux débutants

### Ne vous découragez pas !

La sécurité peut sembler intimidante au début. C'est normal. Voici quelques conseils :

#### Commencez petit
Vous n'avez pas besoin de tout maîtriser immédiatement. Commencez par :
1. Compiler avec `-Wall -Wextra -Werror`
2. Utiliser `strncpy()` au lieu de `strcpy()`
3. Vérifier les retours de `malloc()`

Puis progressez graduellement.

#### Chaque bug est une leçon
Quand vous trouvez un bug de sécurité dans votre code :
- ✅ C'est une **opportunité d'apprentissage**
- ❌ Ce n'est PAS un échec personnel

Les meilleurs développeurs trouvent encore des bugs. La différence est qu'ils apprennent à les éviter.

#### La sécurité est un voyage, pas une destination
Vous ne serez jamais "fini" d'apprendre la sécurité. C'est normal :
- Nouvelles vulnérabilités découvertes régulièrement
- Nouveaux outils développés
- Nouvelles techniques d'attaque

Adoptez un **mindset d'apprentissage continu**.

---

## Prêt à commencer ?

Vous avez maintenant une vue d'ensemble de ce qui vous attend. Ce chapitre vous transformera en un développeur C conscient de la sécurité, capable de :

- ✅ Identifier les vulnérabilités dans du code existant
- ✅ Écrire du code défensif dès le départ
- ✅ Utiliser les outils pour détecter les bugs
- ✅ Protéger vos programmes contre les attaques
- ✅ Comprendre les standards de l'industrie

### Citation finale

> "The difference between a good programmer and a great programmer is not how many languages they know, but how well they write secure code."

**Bonne chance dans votre voyage vers la maîtrise de la sécurité en C !** 🛡️

---

## Plan du chapitre

Voici le parcours que nous allons suivre :

1. **[26.1] CERT C Coding Standards** → Les règles de l'industrie
2. **[26.2] Validation des entrées** → Ne jamais faire confiance
3. **[26.3] Buffer overflows** → La vulnérabilité classique
4. **[26.4] Format strings** → L'attaque subtile
5. **[26.5] Integer overflows** → Les maths peuvent mentir
6. **[26.6] Moindre privilège** → Limiter les dégâts
7. **[26.7] Analyse & Fuzzing** → Détecter avant production
8. **[26.8] Hardening flags** → Protection du compilateur

**Temps estimé** : 15-20 heures pour une maîtrise complète

**Prérequis** : Modules 1-4 terminés

**Prochaine section** : 26.1 Secure C Coding Standards (CERT C) →

---

*"Un programme qui ne crash pas n'est pas nécessairement sécurisé, mais un programme sécurisé doit être robuste."*

⏭️ [Secure C Coding Standards (CERT C)](/26-securite-code-defensif/01-cert-c-standards.md)
