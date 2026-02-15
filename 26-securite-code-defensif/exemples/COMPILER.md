# Chapitre 26 - Sécurité et code défensif : Compilation des exemples

## Flags de compilation standard

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17
```

---

## 01_cert_regles.c

- **Section** : 26.1 - Standards de codage sécurisé (CERT C)
- **Description** : Démonstration des règles CERT C principales (PRE31, DCL30, EXP33, INT30, FLP30, STR31, MEM30, FIO30, ENV33, ARR30)
- **Fichier source** : 01-cert-c-standards.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 01_cert_regles.c -o 01_cert_regles
  ```
- **Sortie attendue** : Démonstrations de chaque règle CERT C avec exemples conformes et explications

---

## 02_validation_type.c

- **Section** : 26.2 - Validation des entrées
- **Description** : Validation de type avec strtol (détection nombre invalide, overflow, entrée vide)
- **Fichier source** : 02-validation-entrees.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 02_validation_type.c -o 02_validation_type
  ```
- **Sortie attendue** : Tests de conversion avec strtol : "42" OK, "abc" échoue, "12xyz" échoue, valeur overflow échoue, "" échoue

---

## 03_validation_email.c

- **Section** : 26.2 - Validation des entrées
- **Description** : Validation de format email (présence @, point après @, longueur)
- **Fichier source** : 02-validation-entrees.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 03_validation_email.c -o 03_validation_email
  ```
- **Sortie attendue** : Tests de 11 adresses email, validation format avec détails des vérifications

---

## 04_validation_date.c

- **Section** : 26.2 - Validation des entrées
- **Description** : Validation sémantique de dates avec gestion des années bissextiles
- **Fichier source** : 02-validation-entrees.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 04_validation_date.c -o 04_validation_date
  ```
- **Sortie attendue** : Tests de 9 dates incluant cas limites (2000 divisible par 400, 1900 divisible par 100)

---

## 05_validation_nom_utilisateur.c

- **Section** : 26.2 - Validation des entrées
- **Description** : Validation de nom d'utilisateur avec codes d'erreur enum (longueur, format, plage de caractères)
- **Fichier source** : 02-validation-entrees.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 05_validation_nom_utilisateur.c -o 05_validation_nom_utilisateur
  ```
- **Sortie attendue** : Tests de 10 noms d'utilisateur avec codes de validation détaillés

---

## 06_validation_regex.c

- **Section** : 26.2 - Validation des entrées
- **Description** : Validation avec expressions régulières POSIX (regcomp/regexec/regfree)
- **Fichier source** : 02-validation-entrees.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 06_validation_regex.c -o 06_validation_regex
  ```
- **Sortie attendue** : Tests de pattern email et numéro de téléphone français avec regex POSIX

---

## 07_safe_string/ (multi-fichiers)

- **Section** : 26.3 - Prévention des buffer overflows
- **Description** : Bibliothèque de chaînes sécurisées (safe_strcpy, safe_strcat, safe_sprintf)
- **Fichier source** : 03-prevention-buffer-overflows.md
- **Fichiers** : `safe_string.h`, `safe_string.c`, `main.c`
- **Compilation** :
  ```bash
  cd 07_safe_string/
  gcc -Wall -Wextra -Werror -pedantic -std=c17 safe_string.c main.c -o safe_string_test
  ```
- **Sortie attendue** : Tests des fonctions sécurisées : copies OK, détection de dépassements, concaténation sûre, sprintf sûr

---

## 08_saisie_securisee.c

- **Section** : 26.3 - Prévention des buffer overflows
- **Description** : Saisie sécurisée avec fgets et snprintf (non-interactif avec données simulées)
- **Fichier source** : 03-prevention-buffer-overflows.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 08_saisie_securisee.c -o 08_saisie_securisee
  ```
- **Sortie attendue** : Démonstrations de snprintf avec troncature, messages formatés, fonction lire_ligne

---

## 09_config_securisee.c

- **Section** : 26.3 - Prévention des buffer overflows
- **Description** : Parseur de fichier de configuration sécurisé avec limites sscanf et strncpy
- **Fichier source** : 03-prevention-buffer-overflows.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 09_config_securisee.c -o 09_config_securisee
  ```
- **Sortie attendue** : Création d'un fichier config temporaire, parsing sécurisé, test fichier inexistant

---

## 10_format_string_demo.c

- **Section** : 26.4 - Format string vulnerabilities
- **Description** : Démonstration de la bonne utilisation de printf (toujours printf("%s", data))
- **Fichier source** : 04-format-string-vulnerabilities.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 10_format_string_demo.c -o 10_format_string_demo
  ```
- **Sortie attendue** : Comparaison approche vulnérable vs sécurisée, traitement d'entrées malveillantes

---

## 11_logging_securise.c

- **Section** : 26.4 - Format string vulnerabilities
- **Description** : Système de logging sécurisé avec timestamp et niveaux de gravité
- **Fichier source** : 04-format-string-vulnerabilities.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 11_logging_securise.c -o 11_logging_securise
  ```
- **Sortie attendue** : Messages de log avec timestamp, niveaux INFO/WARNING/ERROR, macros de format constantes

---

## 12_templates_securises.c

- **Section** : 26.4 - Format string vulnerabilities
- **Description** : Système de templates sécurisé avec whitelist de formats autorisés
- **Fichier source** : 04-format-string-vulnerabilities.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 12_templates_securises.c -o 12_templates_securises
  ```
- **Sortie attendue** : Application de templates bienvenue/rappel/confirmation, rejet de templates inconnus, vérification de sûreté des formats

---

## 13_overflow_demo.c

- **Section** : 26.5 - Integer overflow
- **Description** : Démonstrations de dépassements d'entiers (unsigned wraparound, underflow, limites de types)
- **Fichier source** : 05-integer-overflow.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 13_overflow_demo.c -o 13_overflow_demo
  ```
- **Sortie attendue** : Unsigned char 255+1=0, unsigned 0-1=UINT_MAX, limites INT_MIN/INT_MAX, vérification SIZE_MAX/sizeof pour malloc

---

## 14_overflow_builtins.c

- **Section** : 26.5 - Integer overflow
- **Description** : Utilisation des builtins GCC pour détecter les overflows (__builtin_add/sub/mul_overflow)
- **Fichier source** : 05-integer-overflow.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 14_overflow_builtins.c -o 14_overflow_builtins
  ```
- **Sortie attendue** : Détection d'overflow sur INT_MAX+1, INT_MIN-1, 100000*30000, et multiplication size_t pour malloc

---

## 15_prix_securise.c

- **Section** : 26.5 - Integer overflow
- **Description** : Calcul de prix sécurisé avec int64_t et vérification d'overflow avant chaque opération
- **Fichier source** : 05-integer-overflow.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 15_prix_securise.c -o 15_prix_securise
  ```
- **Sortie attendue** : Panier normal OK, prix négatifs rejetés, valeurs énormes rejetées (overflow), pointeur NULL rejeté

---

## 16_privileges.c

- **Section** : 26.6 - Principe du moindre privilège
- **Description** : Affichage des privilèges du processus et vérification de permissions fichiers
- **Fichier source** : 06-moindre-privilege.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -pedantic -std=c17 16_privileges.c -o 16_privileges
  ```
- **Sortie attendue** : UID/GID réel et effectif, vérification non-root, test permissions 0600 (OK) et 0644 (trop permissives)

---

## 17_seccomp_strict.c

- **Section** : 26.6 - Principe du moindre privilège
- **Description** : Filtrage d'appels système avec seccomp mode strict (seuls read, write, exit, sigreturn autorisés)
- **Fichier source** : 06-moindre-privilege.md
- **Compilation** (sans `-pedantic` : `_DEFAULT_SOURCE`, `syscall`) :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 17_seccomp_strict.c -o 17_seccomp_strict
  ```
- **Sortie attendue** : Messages avant seccomp (printf), activation seccomp strict, messages après (write), sortie via syscall(SYS_exit)

---

## 18_canary_demo.c

- **Section** : 26.8 - Compilation avec hardening flags
- **Description** : Démonstration de la protection stack canary - **Bug intentionnel** : buffer overflow pour déclencher la détection
- **Fichier source** : 08-hardening-flags.md
- **Compilation** (sans `-Werror` : bug intentionnel) :
  ```bash
  gcc -Wall -Wextra -pedantic -std=c17 -fstack-protector-strong 18_canary_demo.c -o 18_canary_demo
  ```
- **Sortie attendue** : Test 1 (entrée courte) OK, Test 2 (entrée longue) → **"*** stack smashing detected ***"** + crash (code 134)

---

## 19_fortify_demo.c

- **Section** : 26.8 - Compilation avec hardening flags
- **Description** : Démonstration de FORTIFY_SOURCE - **Bug intentionnel** : strcpy overflow pour déclencher la détection
- **Fichier source** : 08-hardening-flags.md
- **Compilation** (sans `-Werror` : bug intentionnel, `-O2` requis pour FORTIFY_SOURCE) :
  ```bash
  gcc -Wall -Wextra -pedantic -std=c17 -D_FORTIFY_SOURCE=2 -O2 19_fortify_demo.c -o 19_fortify_demo
  ```
- **Sortie attendue** : Tailles affichées, tentative de strcpy → **"*** buffer overflow detected ***"** + crash (code 134)

---

## 20_hardening_complet.c

- **Section** : 26.8 - Compilation avec hardening flags
- **Description** : Programme compilé avec hardening complet (PIE, RELRO, canary) et vérification des protections actives
- **Fichier source** : 08-hardening-flags.md
- **Compilation** (sans `-pedantic` : conversion function pointer → void*, flags hardening complets) :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -fstack-protector-strong -D_FORTIFY_SOURCE=2 -O2 -fPIE -pie -Wl,-z,relro,-z,now -Wl,-z,noexecstack 20_hardening_complet.c -o 20_hardening_complet
  ```
- **Sortie attendue** : Informations hardening, traitements sécurisés, adresses ASLR (changent à chaque exécution), instructions checksec

---

## Notes

- **Section 26.7** (Analyse statique et fuzzing) : section théorique, pas d'exemples compilables
- Les programmes **18** et **19** contiennent des **bugs intentionnels** (pédagogiques) et crashent volontairement
- Le programme **17** utilise `syscall(SYS_exit)` pour quitter proprement en mode seccomp strict
- Le programme **20** nécessite `-O2` pour que `FORTIFY_SOURCE` soit actif
