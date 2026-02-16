# Chapitre 29 - Tests et qualite : Compilation des exemples

Unity est telecharge dans `/tmp/claude/ch29/unity_fw/src/` (unity.c, unity.h, unity_internals.h).  
Les frameworks Check, cmocka, CUnit et Criterion sont installes via apt.

## Fichiers partages

- `math_utils.h` / `math_utils.c` : fonctions add() et multiply() utilisees par les exemples 01-05

## Exemples

### 01_unity_test.c
- **Section** : 29.2 - Tests unitaires : Frameworks
- **Description** : Test avec le framework Unity (leger, embarque)
- **Fichier source** : 02-frameworks-tests-unitaires.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 01_unity_test \
      01_unity_test.c math_utils.c \
      /tmp/claude/ch29/unity_fw/src/unity.c \
      -I/tmp/claude/ch29/unity_fw/src -I.
  ```
- **Sortie attendue** : `3 Tests 0 Failures 0 Ignored OK`

### 02_check_test.c
- **Section** : 29.2 - Tests unitaires : Frameworks
- **Description** : Test avec le framework Check (fork, robuste)
- **Fichier source** : 02-frameworks-tests-unitaires.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 02_check_test \
      02_check_test.c math_utils.c \
      -lcheck -lm -lsubunit -lpthread
  ```
- **Sortie attendue** : `Running suite(s): Math` / `100%: Checks: 2, Failures: 0, Errors: 0`

### 03_cmocka_test.c
- **Section** : 29.2 - Tests unitaires : Frameworks
- **Description** : Test avec le framework cmocka (mocking integre)
- **Fichier source** : 02-frameworks-tests-unitaires.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 03_cmocka_test \
      03_cmocka_test.c math_utils.c -lcmocka
  ```
- **Sortie attendue** : `[  PASSED  ] 2 test(s).`

### 04_cunit_test.c
- **Section** : 29.2 - Tests unitaires : Frameworks
- **Description** : Test avec le framework CUnit (academique)
- **Fichier source** : 02-frameworks-tests-unitaires.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 04_cunit_test \
      04_cunit_test.c math_utils.c -lcunit
  ```
- **Sortie attendue** : `Run Summary: ... tests 2 ... Ran 2 ... Passed 2 ... Failed 0`

### 05_criterion_test.c
- **Section** : 29.2 - Tests unitaires : Frameworks
- **Description** : Test avec le framework Criterion (moderne, parametrique)
- **Fichier source** : 02-frameworks-tests-unitaires.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 05_criterion_test \
      05_criterion_test.c math_utils.c -lcriterion
  ```
- **Sortie attendue** : 5 tests (2 simples + 3 parametriques), tous passes

### 06_assertions_unity.c
- **Section** : 29.3 - Ecriture de tests et assertions
- **Description** : Demonstration des assertions Unity (entiers, flottants, chaines, tableaux, memoire, pointeurs, booleens)
- **Fichier source** : 03-ecriture-tests.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -DUNITY_INCLUDE_DOUBLE \
      -o 06_assertions_unity 06_assertions_unity.c \
      /tmp/claude/ch29/unity_fw/src/unity.c \
      -I/tmp/claude/ch29/unity_fw/src -I.
  ```
- **Note** : `-DUNITY_INCLUDE_DOUBLE` requis pour TEST_ASSERT_DOUBLE_WITHIN
- **Sortie attendue** : `11 Tests 0 Failures 0 Ignored OK`

### 07_edge_cases.c
- **Section** : 29.3 - Ecriture de tests et assertions
- **Description** : Tests de cas limites (NULL, zero, debordements, tableaux vides)
- **Fichier source** : 03-ecriture-tests.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 07_edge_cases \
      07_edge_cases.c /tmp/claude/ch29/unity_fw/src/unity.c \
      -I/tmp/claude/ch29/unity_fw/src -I.
  ```
- **Sortie attendue** : `5 Tests 0 Failures 0 Ignored OK`

### 08_structures_tests.c
- **Section** : 29.3 - Ecriture de tests et assertions
- **Description** : Tests de structures (Employee, liste chainee, codes d'erreur)
- **Fichier source** : 03-ecriture-tests.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 08_structures_tests \
      08_structures_tests.c /tmp/claude/ch29/unity_fw/src/unity.c \
      -I/tmp/claude/ch29/unity_fw/src -I.
  ```
- **Sortie attendue** : `5 Tests 0 Failures 0 Ignored OK`

### 09_mock_function_ptr.c
- **Section** : 29.4 - Mocking et stubbing
- **Description** : Mocking via pointeurs de fonctions (logger stub et mock)
- **Fichier source** : 04-mocking-stubbing.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 09_mock_function_ptr \
      09_mock_function_ptr.c /tmp/claude/ch29/unity_fw/src/unity.c \
      -I/tmp/claude/ch29/unity_fw/src -I.
  ```
- **Sortie attendue** : `3 Tests 0 Failures 0 Ignored OK`

### 10_notification_service/ (multi-fichiers)
- **Section** : 29.4 - Mocking et stubbing
- **Description** : Service de notification avec injection de dependances, stubs et mocks
- **Fichier source** : 04-mocking-stubbing.md
- **Fichiers** : `notification_service.h`, `notification_service.c`, `test_notification_service.c`
- **Compilation** :
  ```bash
  cd 10_notification_service/
  gcc -Wall -Wextra -Werror -std=c17 -o test_notification_service \
      test_notification_service.c notification_service.c \
      /tmp/claude/ch29/unity_fw/src/unity.c \
      -I/tmp/claude/ch29/unity_fw/src -I.
  ```
- **Sortie attendue** : `4 Tests 0 Failures 0 Ignored OK`

### 11_integration_file.c
- **Section** : 29.5 - Tests d'integration
- **Description** : Test d'integration avec fichier (validation + sauvegarde utilisateur)
- **Fichier source** : 05-tests-integration.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 11_integration_file \
      11_integration_file.c /tmp/claude/ch29/unity_fw/src/unity.c \
      -I/tmp/claude/ch29/unity_fw/src -I.
  ```
- **Sortie attendue** : `7 Tests 0 Failures 0 Ignored OK`
- **Note** : Cree et supprime des fichiers temporaires dans /tmp/

### 12_integration_threads.c
- **Section** : 29.5 - Tests d'integration
- **Description** : Test d'integration multi-threads (compteur thread-safe avec mutex)
- **Fichier source** : 05-tests-integration.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 12_integration_threads \
      12_integration_threads.c /tmp/claude/ch29/unity_fw/src/unity.c \
      -I/tmp/claude/ch29/unity_fw/src -I. -pthread
  ```
- **Note** : `-pthread` requis
- **Sortie attendue** : `3 Tests 0 Failures 0 Ignored OK`

### 13_coverage_demo/ (multi-fichiers)
- **Section** : 29.6 - Mesure de couverture
- **Description** : Demonstration de gcov avec une calculatrice (couverture 100%)
- **Fichier source** : 06-mesure-couverture.md
- **Fichiers** : `calculator.h`, `calculator.c`, `test_calculator.c`
- **Compilation et mesure** :
  ```bash
  cd 13_coverage_demo/
  gcc -Wall -Wextra -Werror -std=c17 --coverage \
      -o test_calculator test_calculator.c calculator.c \
      /tmp/claude/ch29/unity_fw/src/unity.c \
      -I/tmp/claude/ch29/unity_fw/src -I.
  ./test_calculator
  gcov test_calculator-calculator.gcda
  ```
- **Sortie attendue** : `5 Tests 0 Failures 0 Ignored OK` + `Lines executed:100.00% of 11`
- **Note** : `Error: Division by zero` sur stderr est normal (test divide_by_zero)
- **Nettoyage** : `rm -f test_calculator *.gcno *.gcda *.gcov`

### 14_tdd_email_validation.c
- **Section** : 29.7 - Test-Driven Development (TDD)
- **Description** : Validation d'email developpee par TDD (Red-Green-Refactor)
- **Fichier source** : 07-tdd.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 14_tdd_email_validation \
      14_tdd_email_validation.c /tmp/claude/ch29/unity_fw/src/unity.c \
      -I/tmp/claude/ch29/unity_fw/src -I.
  ```
- **Sortie attendue** : `9 Tests 0 Failures 0 Ignored OK`

### 15_tdd_circular_buffer.c
- **Section** : 29.7 - Test-Driven Development (TDD)
- **Description** : Buffer circulaire developpe par TDD (session complete)
- **Fichier source** : 07-tdd.md
- **Compilation** :
  ```bash
  gcc -Wall -Wextra -Werror -std=c17 -o 15_tdd_circular_buffer \
      15_tdd_circular_buffer.c /tmp/claude/ch29/unity_fw/src/unity.c \
      -I/tmp/claude/ch29/unity_fw/src -I.
  ```
- **Sortie attendue** : `6 Tests 0 Failures 0 Ignored OK`
