🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 26.4 Format string vulnerabilities

## Introduction

Les **format string vulnerabilities** (vulnérabilités de chaîne de format) sont parmi les failles de sécurité les plus dangereuses en C. Bien que moins connues que les buffer overflows, elles permettent à un attaquant de :
- **Lire** la mémoire arbitrairement
- **Écrire** dans la mémoire arbitrairement
- **Exécuter** du code malveillant
- **Crasher** le programme

Cette vulnérabilité a été découverte tardivement (fin des années 1990) et continue d'affecter du code legacy.

### Définition simple

Une format string vulnerability se produit lorsque **des données contrôlées par l'utilisateur sont utilisées comme chaîne de format** dans des fonctions comme `printf`, `sprintf`, `fprintf`, etc.

---

## Comment fonctionne printf ?

Avant de comprendre la vulnérabilité, il faut comprendre comment fonctionne `printf`.

### Utilisation normale

```c
int age = 25;  
char nom[] = "Alice";  

printf("Bonjour %s, vous avez %d ans\n", nom, age);
// Affiche : Bonjour Alice, vous avez 25 ans
```

**Fonctionnement** :
1. `printf` lit la chaîne de format (`"Bonjour %s, vous avez %d ans\n"`)
2. Quand elle rencontre `%s`, elle récupère l'argument suivant (pointeur vers "Alice")
3. Quand elle rencontre `%d`, elle récupère l'argument suivant (25)
4. Elle affiche le résultat formaté

### Les spécificateurs de format

| Spécificateur | Type attendu | Description |
|---------------|--------------|-------------|
| `%d` ou `%i` | `int` | Entier signé décimal |
| `%u` | `unsigned int` | Entier non signé décimal |
| `%x` | `unsigned int` | Hexadécimal minuscule |
| `%X` | `unsigned int` | Hexadécimal majuscule |
| `%p` | `void*` | Pointeur (adresse) |
| `%s` | `char*` | Chaîne de caractères |
| `%c` | `int` | Caractère |
| `%f` | `double` | Nombre à virgule flottante |
| `%n` | `int*` | **Nombre de caractères écrits** |

**⚠️ Attention au `%n`** : Ce spécificateur est particulièrement dangereux car il **écrit** en mémoire !

### Comment printf accède aux arguments

Sur la plupart des architectures, `printf` est une fonction variadique qui lit ses arguments depuis **la pile** (stack).

```
Pile mémoire lors de l'appel printf("Hello %s, age: %d", nom, age) :

Adresses hautes
    ↓
+------------------+
| age (25)         |  ← Argument 3
+------------------+
| nom (0x1234...)  |  ← Argument 2 (pointeur)
+------------------+
| "Hello..." ptr   |  ← Argument 1 (format string)
+------------------+
| Adresse retour   |
+------------------+
    ↑
Adresses basses
```

`printf` lit séquentiellement depuis la pile pour récupérer ses arguments.

---

## La vulnérabilité : Qu'est-ce qui peut mal tourner ?

### Exemple vulnérable

```c
// ❌ CODE VULNÉRABLE
#include <stdio.h>

int main(void) {
    char input[100];

    printf("Entrez votre nom : ");
    fgets(input, sizeof(input), stdin);

    // VULNÉRABILITÉ ICI !
    printf(input);  // ← Input utilisé directement comme format string !

    return 0;
}
```

**Que se passe-t-il ?**

Si l'utilisateur entre simplement `"Alice"`, tout va bien :
```
printf("Alice") → Affiche : Alice
```

Mais si l'utilisateur entre `"%x %x %x %x"` :
```
printf("%x %x %x %x")
```

`printf` va chercher 4 arguments sur la pile... qui n'existent pas ! Elle va donc lire **des valeurs arbitraires de la pile** et les afficher en hexadécimal.

### Démonstration

```c
// Programme vulnérable
#include <stdio.h>

int main(void) {
    char input[100];
    fgets(input, sizeof(input), stdin);
    printf(input);  // Vulnérable !
    return 0;
}
```

**Test 1 - Utilisation normale** :
```bash
$ ./programme
Alice  
Alice  
```

**Test 2 - Lecture de la pile** :
```bash
$ ./programme
%x %x %x %x
bffff760 8048480 bffff758 80484a0
```

Les valeurs `bffff760`, `8048480`, etc. sont des **données de la pile** qui ont été exposées !

---

## Les dangers des format string vulnerabilities

### 1. Lecture de mémoire (Information Disclosure)

Un attaquant peut lire des données sensibles stockées dans la pile ou ailleurs en mémoire.

```c
#include <stdio.h>

int main(void) {
    char secret[] = "MotDePasse123";
    char input[100];

    printf("Entrez une commande : ");
    fgets(input, sizeof(input), stdin);

    printf(input);  // Vulnérable

    return 0;
}
```

**Attaque** :
```bash
$ ./programme
%s %s %s %s %s %s %s %s
(null) (null) (null) MotDePasse123 ...
```

L'attaquant a récupéré le mot de passe !

### 2. Crash du programme (Denial of Service)

```c
printf(input);
```

**Attaque avec `%s`** :
```bash
$ ./programme
%s %s %s %s %s %s %s %s
Segmentation fault (core dumped)
```

**Pourquoi ?** `%s` attend un pointeur vers une chaîne. Si `printf` lit une valeur invalide de la pile et tente de la déréférencer comme adresse, le programme crashe.

### 3. Écriture en mémoire (Code Execution)

Le spécificateur **`%n`** est particulièrement dangereux car il écrit en mémoire.

#### Comment fonctionne %n

```c
int count;  
printf("Hello%n World\n", &count);  
// count contiendra 5 (nombre de caractères avant %n : "Hello")
```

**`%n` écrit dans la mémoire pointée par l'argument** le nombre de caractères écrits jusqu'à présent.

#### Exploitation

Si un attaquant contrôle la format string ET peut placer une adresse sur la pile, il peut **écrire n'importe où en mémoire** !

**Exemple conceptuel** :
```c
// Programme vulnérable
void fonction_vulnerable(char *input) {
    printf(input);  // Vulnérable
}

// L'attaquant peut crafted une entrée comme :
// "\x10\x20\x30\x40%n"
// Où 0x40302010 est l'adresse cible à modifier
```

Avec des techniques avancées, un attaquant peut :
- Modifier une adresse de retour sur la pile
- Écraser un pointeur de fonction
- Modifier des variables globales critiques
- Exécuter du code arbitraire

---

## Anatomie d'une attaque format string

### Étape 1 : Trouver l'offset

L'attaquant doit d'abord déterminer à quelle position sur la pile se trouvent ses données.

```c
// Programme vulnérable
#include <stdio.h>

int main(void) {
    char input[100];
    fgets(input, sizeof(input), stdin);
    printf(input);
    return 0;
}
```

**Test avec marqueur** :
```bash
$ ./programme
AAAA%x.%x.%x.%x.%x.%x  
AAAA bffff760.8048480.bffff758.41414141.2e78252e.78252e78  
                                ^^^^^^^^
                                "AAAA" en hexa !
```

L'attaquant voit que `AAAA` (0x41414141) apparaît au 4ème argument. Il peut maintenant contrôler une valeur sur la pile.

### Étape 2 : Lire une adresse spécifique

Avec l'accès direct positionnel `%n$` :

```bash
$ ./programme
%4$x
41414141  # Lit directement le 4ème argument
```

### Étape 3 : Écriture avec %n

```bash
$ ./programme
\x10\x20\x30\x40%4$n
# Écrit à l'adresse 0x40302010 le nombre de caractères écrits
```

---

## Fonctions vulnérables

Toutes les fonctions de la famille `printf` sont potentiellement vulnérables :

### Fonctions de sortie

| Fonction | Vulnérable si... |
|----------|------------------|
| `printf(fmt, ...)` | `fmt` contrôlé par l'utilisateur |
| `fprintf(file, fmt, ...)` | `fmt` contrôlé par l'utilisateur |
| `sprintf(buf, fmt, ...)` | `fmt` contrôlé par l'utilisateur |
| `snprintf(buf, size, fmt, ...)` | `fmt` contrôlé par l'utilisateur |
| `vprintf(fmt, args)` | `fmt` contrôlé par l'utilisateur |
| `vsprintf(buf, fmt, args)` | `fmt` contrôlé par l'utilisateur |
| `vsnprintf(buf, size, fmt, args)` | `fmt` contrôlé par l'utilisateur |

### Fonctions de logging

```c
// ❌ Dangereux
void log_message(const char *message) {
    fprintf(log_file, message);  // Vulnérable !
}

// ❌ Dangereux aussi
void debug_print(char *data) {
    printf(data);  // Vulnérable !
}
```

### Fonctions syslog

```c
#include <syslog.h>

// ❌ Dangereux
void log_error(char *error_msg) {
    syslog(LOG_ERR, error_msg);  // Vulnérable !
}
```

---

## Prévention : Les bonnes pratiques

### Règle d'or

> **Jamais, au grand jamais, passer des données contrôlées par l'utilisateur comme format string !**

### 1. Toujours utiliser un format fixe

```c
// ❌ DANGEREUX
printf(user_input);  
fprintf(file, user_input);  
syslog(LOG_INFO, user_input);  
```

```c
// ✅ SÛR
printf("%s", user_input);  
fprintf(file, "%s", user_input);  
syslog(LOG_INFO, "%s", user_input);  
```

**Explication** : En utilisant `"%s"` comme format, vous dites explicitement à `printf` que l'argument est une chaîne à afficher. Même si `user_input` contient des `%x` ou `%n`, ils seront affichés littéralement, pas interprétés.

### 2. Validation et assainissement

Si vous devez vraiment construire dynamiquement une format string (rare), validez-la :

```c
bool est_format_sur(const char *format) {
    // Vérifier qu'il n'y a pas de spécificateurs dangereux
    if (strchr(format, '%') != NULL) {
        return false;  // Rejeter toute chaîne contenant '%'
    }
    return true;
}

void afficher_message(const char *message) {
    if (!est_format_sur(message)) {
        fprintf(stderr, "Erreur : format invalide\n");
        return;
    }
    printf(message);
}
```

### 3. Utiliser des constantes pour les formats

```c
// ✅ Définir les formats comme constantes
#define FMT_USER_INFO "Utilisateur: %s, ID: %d\n"
#define FMT_ERROR "Erreur: %s\n"

void afficher_info(const char *nom, int id) {
    printf(FMT_USER_INFO, nom, id);  // Format fixe, sûr
}

void afficher_erreur(const char *msg) {
    fprintf(stderr, FMT_ERROR, msg);  // Format fixe, sûr
}
```

### 4. Wrappers sécurisés

```c
// Wrapper sécurisé pour printf
void safe_printf(const char *message) {
    printf("%s", message);  // Toujours utiliser %s
}

// Wrapper sécurisé pour fprintf
void safe_fprintf(FILE *stream, const char *message) {
    fprintf(stream, "%s", message);
}

// Wrapper sécurisé pour syslog
void safe_syslog(int priority, const char *message) {
    syslog(priority, "%s", message);
}
```

---

## Exemples de code vulnérable et corrigé

### Exemple 1 : Programme de logging simple

```c
// ❌ VERSION VULNÉRABLE
#include <stdio.h>
#include <time.h>

void log_event(const char *event) {
    time_t now = time(NULL);
    printf(ctime(&now));  // OK
    printf(event);        // VULNÉRABLE !
    printf("\n");
}

int main(void) {
    char user_event[200];

    printf("Entrez un événement à logger : ");
    fgets(user_event, sizeof(user_event), stdin);

    log_event(user_event);

    return 0;
}
```

**Test d'exploitation** :
```bash
$ ./log
Entrez un événement à logger : %x %x %x %x  
Wed Nov 27 10:30:45 2024  
bffff760 8048480 bffff758 80484a0  
```

```c
// ✅ VERSION SÉCURISÉE
#include <stdio.h>
#include <time.h>

void log_event(const char *event) {
    time_t now = time(NULL);
    printf("%s", ctime(&now));  // OK
    printf("%s", event);        // SÉCURISÉ !
    printf("\n");
}

int main(void) {
    char user_event[200];

    printf("Entrez un événement à logger : ");
    fgets(user_event, sizeof(user_event), stdin);

    log_event(user_event);

    return 0;
}
```

### Exemple 2 : Fonction d'erreur personnalisée

```c
// ❌ VERSION VULNÉRABLE
#include <stdio.h>
#include <stdlib.h>

void erreur(const char *message) {
    fprintf(stderr, "ERREUR: ");
    fprintf(stderr, message);      // VULNÉRABLE !
    fprintf(stderr, "\n");
    exit(1);
}

int main(void) {
    char input[100];

    printf("Entrez un nombre : ");
    fgets(input, sizeof(input), stdin);

    int nombre = atoi(input);
    if (nombre <= 0) {
        erreur(input);  // Passe l'entrée utilisateur directement
    }

    return 0;
}
```

```c
// ✅ VERSION SÉCURISÉE
#include <stdio.h>
#include <stdlib.h>

void erreur(const char *message) {
    fprintf(stderr, "ERREUR: %s\n", message);  // SÉCURISÉ avec %s
    exit(1);
}

int main(void) {
    char input[100];

    printf("Entrez un nombre : ");
    fgets(input, sizeof(input), stdin);

    int nombre = atoi(input);
    if (nombre <= 0) {
        erreur("nombre invalide");  // Ou : erreur(input) maintenant sûr
    }

    return 0;
}
```

### Exemple 3 : Serveur web minimaliste

```c
// ❌ VERSION VULNÉRABLE
#include <stdio.h>
#include <string.h>

void handle_request(const char *uri) {
    char response[1024];

    // Construction de la réponse
    sprintf(response, "HTTP/1.1 404 Not Found\n");
    sprintf(response + strlen(response), "Page not found: ");
    sprintf(response + strlen(response), uri);  // VULNÉRABLE !

    printf("%s", response);
}

int main(void) {
    char uri[256];

    printf("URI : ");
    fgets(uri, sizeof(uri), stdin);

    handle_request(uri);
    return 0;
}
```

```c
// ✅ VERSION SÉCURISÉE
#include <stdio.h>
#include <string.h>

void handle_request(const char *uri) {
    char response[1024];

    // Construction sécurisée avec snprintf
    snprintf(response, sizeof(response),
             "HTTP/1.1 404 Not Found\nPage not found: %s",
             uri);  // SÉCURISÉ avec %s

    printf("%s", response);
}

int main(void) {
    char uri[256];

    printf("URI : ");
    fgets(uri, sizeof(uri), stdin);

    handle_request(uri);
    return 0;
}
```

### Exemple 4 : Système de templates

```c
// ❌ VERSION VULNÉRABLE
#include <stdio.h>
#include <string.h>

void generer_email(const char *template, const char *nom) {
    char email[500];

    // Template fourni par l'utilisateur
    sprintf(email, template, nom);  // VULNÉRABLE si template contrôlé !
    printf("%s", email);
}

int main(void) {
    char template[200];

    printf("Template d'email : ");
    fgets(template, sizeof(template), stdin);

    generer_email(template, "Alice");
    return 0;
}
```

```c
// ✅ VERSION SÉCURISÉE
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

// Liste blanche de templates autorisés
typedef struct {
    const char *nom;
    const char *format;
} Template;

const Template TEMPLATES[] = {
    {"bienvenue", "Bienvenue %s!\n"},
    {"reminder", "Rappel pour %s\n"},
    {"confirmation", "Confirmation pour %s\n"}
};

const size_t NB_TEMPLATES = sizeof(TEMPLATES) / sizeof(TEMPLATES[0]);

bool generer_email(const char *nom_template, const char *nom) {
    // Recherche dans la liste blanche
    for (size_t i = 0; i < NB_TEMPLATES; i++) {
        if (strcmp(nom_template, TEMPLATES[i].nom) == 0) {
            // Template sûr trouvé
            printf(TEMPLATES[i].format, nom);
            return true;
        }
    }

    fprintf(stderr, "Erreur : template '%s' non autorisé\n", nom_template);
    return false;
}

int main(void) {
    char template_name[50];

    printf("Nom du template (bienvenue/reminder/confirmation) : ");
    fgets(template_name, sizeof(template_name), stdin);

    // Retirer le '\n'
    size_t len = strlen(template_name);
    if (len > 0 && template_name[len-1] == '\n') {
        template_name[len-1] = '\0';
    }

    generer_email(template_name, "Alice");
    return 0;
}
```

---

## Détection des format string vulnerabilities

### 1. Analyse de code manuelle

Recherchez ces patterns dans votre code :

```bash
# Recherche de patterns suspects
grep -n "printf(" *.c  
grep -n "fprintf(" *.c  
grep -n "sprintf(" *.c  
grep -n "snprintf(" *.c  
grep -n "syslog(" *.c  
```

Pour chaque occurrence, vérifiez si le premier argument (format) est :
- Une constante littérale → ✅ OK
- Une variable/paramètre → ⚠️ Vérifier l'origine
- Une entrée utilisateur → ❌ VULNÉRABLE

### 2. Outils d'analyse statique

#### GCC avec warnings

```bash
gcc -Wall -Wextra -Wformat -Wformat-security mon_programme.c -o mon_programme
```

**Exemple de warning** :
```
warning: format not a string literal and no format arguments [-Wformat-security]
     printf(user_input);
     ^~~~~~
```

#### Clang avec flags de sécurité

```bash
clang -Wformat -Wformat-security -Werror=format-security mon_programme.c -o mon_programme
```

#### Flawfinder

```bash
flawfinder mon_programme.c
```

Exemple de sortie :
```
mon_programme.c:15: [4] (format) printf:
  If format strings can be influenced by an attacker, they can be exploited
  (CWE-134). Use a constant format string.
```

#### cppcheck

```bash
cppcheck --enable=all mon_programme.c
```

#### clang-tidy

```bash
clang-tidy mon_programme.c -checks='cert-*,clang-analyzer-*' --
```

### 3. Tests dynamiques

#### Fuzzing avec AFL

```bash
# Compiler avec AFL
afl-gcc mon_programme.c -o mon_programme

# Créer des cas de test
mkdir test_cases  
echo "Hello" > test_cases/normal.txt  
echo "%x %x %x" > test_cases/format.txt  

# Lancer le fuzzer
afl-fuzz -i test_cases -o findings ./mon_programme
```

#### Test manuel avec patterns

Testez votre programme avec ces entrées :

```
%x
%x %x %x %x
%s %s %s %s
%p %p %p %p
%n
%100000d
AAAA%x.%x.%x.%x
%08x.%08x.%08x.%08x
%1$x
%10$s
```

Si votre programme :
- Affiche des valeurs hexadécimales étranges → Vulnérable à la lecture
- Crashe avec `%s` → Vulnérable
- Accepte `%n` sans erreur → Très vulnérable

---

## Compilation avec protections

### FORTIFY_SOURCE

Active des vérifications au runtime pour détecter les format string attacks.

```bash
gcc -D_FORTIFY_SOURCE=2 -O2 mon_programme.c -o mon_programme
```

**Effet** : Si une format string provient d'une source non constante, le programme peut être arrêté.

Exemple :
```
*** %n in writable segment detected ***
Aborted (core dumped)
```

### Flags de compilation recommandés

```bash
gcc -Wall -Wextra -Werror \
    -Wformat -Wformat-security -Werror=format-security \
    -D_FORTIFY_SOURCE=2 -O2 \
    -fstack-protector-all \
    mon_programme.c -o mon_programme
```

---

## Cas particuliers

### 1. Localisation (i18n)

```c
// ❌ Dangereux avec gettext
#include <libintl.h>

void afficher_message(const char *key) {
    char *translated = gettext(key);
    printf(translated);  // Vulnérable si la traduction contient %
}
```

```c
// ✅ Sécurisé
void afficher_message(const char *key) {
    char *translated = gettext(key);
    printf("%s", translated);  // Sûr
}
```

### 2. Logging avec niveaux

```c
// ❌ Vulnérable
void log_with_level(int level, const char *message) {
    const char *prefixes[] = {"INFO: ", "WARN: ", "ERROR: "};
    printf(prefixes[level]);
    printf(message);  // Vulnérable !
}
```

```c
// ✅ Sécurisé
void log_with_level(int level, const char *message) {
    const char *prefixes[] = {"INFO: ", "WARN: ", "ERROR: "};
    printf("%s%s", prefixes[level], message);  // Sûr
}
```

### 3. Wrappers variadiques personnalisés

```c
// ✅ Wrapper variadique sécurisé
#include <stdarg.h>

void mon_printf(const char *format, ...) {
    // Format est contrôlé par le code, pas l'utilisateur
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}

// Utilisation
mon_printf("Nom: %s, Age: %d\n", nom, age);  // OK
```

**Important** : N'acceptez JAMAIS que le paramètre `format` vienne d'une entrée utilisateur !

---

## Historique et exemples réels

### Vulnérabilités célèbres

1. **Wu-ftpd 2.6.0** (2000)
   - Format string dans le logging
   - Permettait l'exécution de code à distance

2. **Linux rpc.statd** (2000)
   - Vulnérabilité dans le logging syslog
   - Escalade de privilèges locale

3. **ProFTPD** (2006)
   - Format string dans les messages d'erreur
   - Exploitation à distance

4. **Samba** (2012)
   - Format string dans la gestion des ACL
   - CVE-2012-2111

### Impact

Les format string vulnerabilities ont permis :
- Des compromissions de serveurs web
- Des escalades de privilèges
- Des prises de contrôle à distance
- Des dénis de service

---

## Checklist de prévention

### ✅ Code Review

Lors de la revue de code, vérifiez :

- [ ] Aucun appel à `printf(user_data)`
- [ ] Tous les `printf/fprintf/sprintf` ont un format constant ou `"%s"`
- [ ] Pas de format string construite dynamiquement avec entrées utilisateur
- [ ] Les fonctions de logging utilisent des formats fixes
- [ ] Les messages d'erreur utilisent `"%s"` pour afficher les données utilisateur

### ✅ Compilation

- [ ] Flag `-Wformat-security` activé
- [ ] Flag `-Werror=format-security` activé (traiter en erreur)
- [ ] `_FORTIFY_SOURCE=2` activé avec `-O2`

### ✅ Tests

- [ ] Tests manuels avec `%x`, `%s`, `%n`
- [ ] Fuzzing avec entrées malveillantes
- [ ] Analyse statique effectuée (cppcheck, clang-tidy)
- [ ] Revue de toutes les fonctions de logging

---

## Résumé : Les règles d'or

### 🎯 Principe fondamental

> **Jamais utiliser de données externes comme format string**

### 📋 Règles à suivre

1. **Toujours utiliser un format fixe**
   ```c
   printf("%s", user_input);  // ✅ Toujours comme ça
   printf(user_input);        // ❌ Jamais comme ça
   ```

2. **Les formats sont du CODE, pas des DONNÉES**
   - Les format strings doivent être des constantes littérales
   - Ne jamais les construire à partir d'entrées utilisateur

3. **Privilégier les constantes**
   ```c
   #define FMT_LOG "Log: %s\n"
   printf(FMT_LOG, message);
   ```

4. **Compiler avec les bons flags**
   ```bash
   -Wformat-security -Werror=format-security -D_FORTIFY_SOURCE=2
   ```

5. **Tester systématiquement**
   - Essayez `%x %x %x` dans toutes les entrées
   - Utilisez des outils d'analyse statique

### 🔒 Hiérarchie de sécurité

```
Niveau 1 (Minimal) :
└─ Toujours utiliser printf("%s", data) au lieu de printf(data)

Niveau 2 (Recommandé) :
└─ Formats constants uniquement
└─ Flags de compilation activés

Niveau 3 (Paranoïaque) :
└─ Analyse statique systématique
└─ Wrappers sécurisés pour toutes les fonctions printf
└─ Tests de fuzzing réguliers
```

### 💡 Message clé

Si vous retenez une seule chose de cette section :

> **printf(variable) est TOUJOURS DANGEREUX**
>
> **printf("%s", variable) est TOUJOURS SÛR**

Cette simple règle élimine 99% des format string vulnerabilities !

---

## Différences avec les buffer overflows

| Aspect | Buffer Overflow | Format String |
|--------|-----------------|---------------|
| **Cause** | Écriture au-delà des limites | Format contrôlé par l'utilisateur |
| **Impact** | Corruption mémoire adjacente | Lecture/écriture arbitraire |
| **Détection** | Plus facile (crashs fréquents) | Plus subtile |
| **Exploitation** | Contrôle du flux d'exécution | Contrôle fin de la mémoire |
| **Prévention** | Vérifier les longueurs | Formats constants |

**Les deux peuvent mener à l'exécution de code arbitraire !**

---

**🔐 Conclusion** : Les format string vulnerabilities sont moins connues que les buffer overflows mais tout aussi dangereuses. La bonne nouvelle ? Elles sont **extrêmement faciles à prévenir** : utilisez simplement `printf("%s", data)` au lieu de `printf(data)` et activez les bons flags de compilation !

**Prochaine étape** : La section suivante couvre les integer overflows, une autre classe de vulnérabilités critiques.

⏭️ [Integer overflow](/26-securite-code-defensif/05-integer-overflow.md)
