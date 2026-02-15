# Chapitre 25 - Gestion des erreurs robuste : Compilation des exemples

## Commande de compilation standard

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o <programme> <fichier.c>
```

Exceptions :
- `30_static_assert_packed.c` : sans `-pedantic` (`__attribute__((packed))`)

---

## Section 25.1 : Codes de retour vs exceptions simulées

| # | Fichier | Description | Source | Sortie attendue |
|---|---------|-------------|--------|-----------------|
| 01 | `01_codes_retour_enum.c` | Codes d'erreur avec énumérations | 01-codes-retour-vs-exceptions.md | `Connexion etablie` |
| 02 | `02_codes_retour_null.c` | Retour de pointeur NULL pour signaler une erreur | 01-codes-retour-vs-exceptions.md | `Buffer alloue avec succes (1024 octets)` |
| 03 | `03_codes_retour_param.c` | Retour de valeur via paramètre et code d'erreur | 01-codes-retour-vs-exceptions.md | `Erreur : division par zero` puis `10 / 3 = 3` |
| 04 | `04_setjmp_longjmp.c` | Exemple complet de setjmp/longjmp (exceptions simulées) | 01-codes-retour-vs-exceptions.md | Affiche le flux try/catch, termine avec `Erreur : allocation memoire echouee` (exit 1) |
| 05 | `05_setjmp_fuite.c` | Fuite mémoire avec longjmp (**bug intentionnel pédagogique**) | 01-codes-retour-vs-exceptions.md | `Erreur capturee, mais fuite memoire !` |
| 06 | `06_goto_cleanup.c` | Pattern goto pour nettoyage centralisé des ressources | 01-codes-retour-vs-exceptions.md | Test avec fichier inexistant (code -1) puis fichier existant (code 0) |
| 07 | `07_macros_try_catch.c` | Macros pour simuler try/catch avec setjmp/longjmp | 01-codes-retour-vs-exceptions.md | Test 1 : exécution normale, Test 2 : erreur capturée |

---

## Section 25.2 : La variable errno

| # | Fichier | Description | Source | Sortie attendue |
|---|---------|-------------|--------|-----------------|
| 08 | `08_errno_intro.c` | Introduction à errno avec fopen et strerror | 02-errno.md | `Code errno : 2` / `Signification : No such file or directory` |
| 09 | `09_errno_codes.c` | Codes errno avec switch et open sur /etc/shadow | 02-errno.md | `Erreur : Permission refusee` (exit 1) |
| 10 | `10_strerror_demo.c` | Démonstration de strerror() | 02-errno.md | ENOENT, EACCES, ENOMEM avec messages en anglais |
| 11 | `11_perror_demo.c` | Démonstration de perror() | 02-errno.md | `Erreur fopen: No such file or directory` (stderr) |
| 12 | `12_strerror_vs_perror.c` | Comparaison entre strerror() et perror() | 02-errno.md | Deux lignes similaires `Erreur: No such file or directory` |
| 13 | `13_errno_non_reinit.c` | errno n'est PAS réinitialisé après un succès | 02-errno.md | Montre errno=2 persistant après un fopen réussi |
| 14 | `14_errno_sauvegarde.c` | Sauvegarder errno avant d'appeler d'autres fonctions | 02-errno.md | Log + code erreur sauvegardé correctement |
| 15 | `15_errno_pattern.c` | Pattern recommandé pour gestion d'erreurs avec errno | 02-errno.md | Test fichier inexistant (échec) puis fichier existant (succès) |
| 16 | `16_copier_fichier.c` | Gestionnaire d'erreurs robuste - copie de fichier avec goto | 02-errno.md | Copie réussie, fichier inexistant, permission refusée |

---

## Section 25.3 : Patterns de gestion d'erreurs

| # | Fichier | Description | Source | Sortie attendue |
|---|---------|-------------|--------|-----------------|
| 17 | `17_check_return.c` | Pattern Check-and-Return (vérification systématique) | 03-patterns-gestion-erreurs.md | Chargement config réussi puis échec fichier inexistant |
| 18 | `18_goto_cleanup_multi.c` | Goto cleanup multi-étapes (traitement fichier) | 03-patterns-gestion-erreurs.md | `Traitement reussi` / `Code retour : 0` |
| 19 | `19_guard_clauses.c` | Guard Clauses (retours anticipés) avec validation | 03-patterns-gestion-erreurs.md | Sauvegarde OK, puis échecs attendus (NULL, taille 0) |
| 20 | `20_structure_retour.c` | Structure de retour avec état (parser de configuration) | 03-patterns-gestion-erreurs.md | `valeur = 42`, erreurs pour 150/abc/vide |
| 21 | `21_wrapper_malloc.c` | Wrapper Functions pour allocation mémoire sécurisée | 03-patterns-gestion-erreurs.md | `Hello, World!` + redimensionnement + calloc |
| 22 | `22_wrapper_fichier.c` | Wrapper Functions pour ouverture/fermeture de fichier | 03-patterns-gestion-erreurs.md | Écriture puis relecture du fichier |
| 23 | `23_context_validation.c` | Context Object - validation de formulaire | 03-patterns-gestion-erreurs.md | Test 1 : INVALIDE (2 erreurs, 1 warn), Test 2 : VALIDE (0 erreur, 1 warn) |
| 24 | `24_error_callback.c` | Error Callback - fonctions de rappel personnalisées | 03-patterns-gestion-erreurs.md | 4 tests avec différents callbacks (défaut, simple, fichier, statistiques) |

---

## Section 25.4 : Assertions

| # | Fichier | Description | Source | Sortie attendue |
|---|---------|-------------|--------|-----------------|
| 25 | `25_assert_intro.c` | Introduction à assert() - **crash intentionnel** | 04-assertions.md | `Age valide : 25` puis assertion failure + Aborted |
| 26 | `26_assert_ndebug.c` | Désactivation des assertions avec NDEBUG | 04-assertions.md | `valeur = -10` (pas de crash, assertion désactivée) |
| 27 | `27_assert_msg.c` | Assertions avec messages personnalisés - **crash intentionnel** | 04-assertions.md | Assertion failure avec message `La valeur doit etre positive` |
| 28 | `28_assert_macro_custom.c` | Macro personnalisée ASSERT_MSG - **crash intentionnel** | 04-assertions.md | Message détaillé (fichier, ligne, fonction) puis Aborted |
| 29 | `29_static_assert_types.c` | static_assert pour vérifier la taille des types | 04-assertions.md | `sizeof(int) = 4`, `sizeof(void*) = 8` |
| 30 | `30_static_assert_packed.c` | static_assert pour structure packed (sans -pedantic) | 04-assertions.md | `sizeof(Paquet) = 5 octets` |
| 31 | `31_static_assert_constantes.c` | static_assert pour constantes et énumérations | 04-assertions.md | Vérification des constantes et priorités |
| 32 | `32_assert_liste_chainee.c` | Assertions dans une liste chaînée | 04-assertions.md | `Nombre d'elements : 3` / `Liste : 30 20 10` |
| 33 | `33_assert_tests.c` | Assertions et tests unitaires complémentaires | 04-assertions.md | 3 tests passés avec succès |
| 34 | `34_assert_buffer.c` | Gestionnaire de mémoire avec assertions et static_assert | 04-assertions.md | Buffer créé, ajouts, `Contenu : Hello, World!` |

### Compilation spéciale section 25.4

```bash
# 30 : sans -pedantic (__attribute__((packed)))
gcc -Wall -Wextra -Werror -std=c17 -o 30_static_assert_packed 30_static_assert_packed.c

# 25, 27, 28 : crashent intentionnellement (assertions)
# Compiler normalement, l'exécution provoque un abort()
```

---

## Section 25.5 : Design by contract

| # | Fichier | Description | Source | Sortie attendue |
|---|---------|-------------|--------|-----------------|
| 35 | `35_dbc_division.c` | Division entière avec contrat (pré/postconditions) | 05-design-by-contract.md | `17 / 5 = 3 reste 2` |
| 36 | `36_dbc_factorielle.c` | Macros de contrat PRECONDITION/POSTCONDITION | 05-design-by-contract.md | Factorielles de 0! à 12! |
| 37 | `37_dbc_pile.c` | Pile (Stack) avec contrat rigoureux DbC | 05-design-by-contract.md | Empilage 10/20/30, sommet=30, dépilage 30/20/10 |
| 38 | `38_dbc_file.c` | File d'attente (Queue) circulaire avec DbC | 05-design-by-contract.md | Enfilage/défilage avec vérification des invariants |

---

## Section 25.6 : Logging

| # | Fichier | Description | Source | Sortie attendue |
|---|---------|-------------|--------|-----------------|
| 39 | `39_logging_simple.c` | Système de logging minimaliste avec niveaux | 06-logging.md | 5 messages (TRACE filtré), avec timestamp |
| 40 | `40_logging_fichier.c` | Logging dans un fichier avec contexte (fichier:ligne) | 06-logging.md | Contenu du fichier log avec 8 messages |
| 41 | `41_logging_syslog.c` | Utilisation de syslog (POSIX) | 06-logging.md | `Messages envoyes au syslog` |
| 42 | `42_logging_serveur.c` | Serveur simulé avec logging complet | 06-logging.md | 23 messages, 2 erreurs dans les statistiques |

---

## Notes

- **42 programmes** au total (42 fichiers .c)
- **3 programmes crashent intentionnellement** (25, 27, 28) : assertions pédagogiques
- **1 programme avec bug intentionnel** (05) : fuite mémoire avec longjmp
- **1 programme sans -pedantic** (30) : `__attribute__((packed))`
- Les programmes 40 et 42 créent des fichiers temporaires dans `/tmp/` et les nettoient
- Le programme 41 (syslog) envoie des messages au journal système

## Script de compilation complet

```bash
#!/bin/bash
FLAGS="-Wall -Wextra -Werror -pedantic -std=c17"
FLAGS_NO_PEDANTIC="-Wall -Wextra -Werror -std=c17"

for f in *.c; do
    prog="${f%.c}"
    case "$f" in
        30_static_assert_packed.c)
            gcc $FLAGS_NO_PEDANTIC -o "$prog" "$f"
            ;;
        *)
            gcc $FLAGS -o "$prog" "$f"
            ;;
    esac
    echo "Compilé : $prog"
done
```
