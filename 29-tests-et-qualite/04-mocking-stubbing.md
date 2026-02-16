🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 29.4 Mocking et stubbing

## Introduction

Imaginez que vous devez tester une fonction qui envoie un email, écrit dans un fichier, ou interroge une base de données. Comment tester cela sans réellement envoyer l'email, créer le fichier, ou se connecter à la base ? C'est là qu'interviennent le **mocking** et le **stubbing** : des techniques pour simuler des dépendances externes dans vos tests.

---

## Qu'est-ce qu'une dépendance externe ?

### Le problème

Une **dépendance externe** est tout ce dont votre code a besoin mais qui est hors de son contrôle direct :

- **Système de fichiers** : Lire/écrire des fichiers
- **Réseau** : Requêtes HTTP, sockets
- **Base de données** : Connexions SQL
- **Horloge système** : `time()`, `gettimeofday()`
- **Générateur aléatoire** : `rand()`
- **Autres modules** : Fonctions d'une bibliothèque tierce

**Exemple problématique :**

```c
#include <stdio.h>
#include <time.h>

typedef struct {
    char username[50];
    time_t last_login;
} User;

void update_last_login(User *user) {
    user->last_login = time(NULL); // Dépendance : horloge système

    FILE *f = fopen("users.log", "a"); // Dépendance : système de fichiers
    if (f) {
        fprintf(f, "User %s logged in at %ld\n", user->username, user->last_login);
        fclose(f);
    }
}
```

**Pourquoi c'est difficile à tester ?**

1. **Non déterministe** : `time(NULL)` change à chaque exécution
2. **Effets de bord** : Crée/modifie un fichier réel
3. **Dépendances du système** : Nécessite des permissions d'écriture
4. **Lenteur** : Les I/O disque sont lentes
5. **Isolation impossible** : Les tests interfèrent entre eux

---

## Stub vs Mock : Quelle différence ?

Bien que souvent utilisés de manière interchangeable, les termes ont des significations précises :

### Stub (Bouchon)

Un **stub** est une implémentation simplifiée qui retourne des valeurs prédéfinies.

**Objectif :** Fournir des données contrôlées pour tester la logique.

```c
// Vrai code production
int get_user_age_from_db(const char *username) {
    // Connexion DB, requête SQL, etc.
    // ...
    return age;
}

// Stub pour les tests
int get_user_age_from_db_stub(const char *username) {
    if (strcmp(username, "alice") == 0) return 25;
    if (strcmp(username, "bob") == 0) return 30;
    return -1; // Utilisateur inconnu
}
```

**Caractéristiques :**
- ✅ Retourne des valeurs prédéfinies
- ✅ Pas de logique complexe
- ✅ Pas de vérification d'appel
- ✅ Focus sur les **données de sortie**

### Mock (Simulacre)

Un **mock** est un objet qui vérifie **comment** il est utilisé.

**Objectif :** Vérifier les interactions (nombre d'appels, paramètres reçus).

```c
// Mock qui enregistre les appels
static int log_write_count = 0;  
static char last_log_message[256];  

void log_write_mock(const char *message) {
    log_write_count++;
    strncpy(last_log_message, message, sizeof(last_log_message) - 1);
}

void test_function_logs_correctly(void) {
    log_write_count = 0; // Réinitialiser

    my_function(); // Appelle log_write_mock en interne

    // Vérifier l'interaction
    TEST_ASSERT_EQUAL_INT(1, log_write_count); // Appelé 1 fois ?
    TEST_ASSERT_EQUAL_STRING("Expected message", last_log_message);
}
```

**Caractéristiques :**
- ✅ Enregistre les appels
- ✅ Vérifie les paramètres reçus
- ✅ Vérifie le nombre d'appels
- ✅ Focus sur le **comportement**

### Comparaison

| Aspect | Stub | Mock |
|--------|------|------|
| **Objectif** | Fournir des données | Vérifier les interactions |
| **Vérifie** | Rien (passif) | Appels, paramètres (actif) |
| **Complexité** | Simple | Plus complexe |
| **Usage** | Données d'entrée | Effets de bord |

**Analogie :**
- **Stub** = Un acteur de doublure qui dit ses répliques (données)
- **Mock** = Un acteur qui enregistre toutes les interactions pour analyse

---

## Pourquoi le mocking en C est-il difficile ?

### Les limitations du C

Contrairement aux langages orientés objet (Java, Python, JavaScript), le C n'a pas de :

1. **Réflexion** : Impossible d'inspecter/modifier le code à l'exécution
2. **Polymorphisme natif** : Pas d'interfaces ou de classes virtuelles
3. **Injection de dépendances automatique** : Pas de framework DI
4. **Proxies dynamiques** : Impossible de créer des objets mock à la volée

**Conséquence :** Le mocking en C nécessite plus de travail manuel et de discipline architecturale.

### Solutions disponibles

1. **Pointeurs de fonctions** (manuel, flexible)
2. **Wrapping avec l'éditeur de liens** (linker tricks)
3. **Préprocesseur** (macros conditionnelles)
4. **Frameworks de mocking** (cmocka, fff)

---

## Technique 1 : Pointeurs de fonctions

### Principe

Remplacer les appels directs par des pointeurs de fonctions qu'on peut substituer dans les tests.

### Exemple simple : Logger

**Code de production :**

```c
// logger.h
#ifndef LOGGER_H
#define LOGGER_H

// Pointeur de fonction pour le logger
typedef void (*LogFunction)(const char *message);

// Logger par défaut (production)
void log_to_file(const char *message);

// Pointeur global (peut être remplacé dans les tests)
extern LogFunction current_logger;

// Fonction utilitaire
void log_message(const char *message);

#endif
```

**Implémentation :**

```c
// logger.c
#include "logger.h"
#include <stdio.h>

// Implémentation production
void log_to_file(const char *message) {
    FILE *f = fopen("app.log", "a");
    if (f) {
        fprintf(f, "%s\n", message);
        fclose(f);
    }
}

// Pointeur initialisé au logger de production
LogFunction current_logger = log_to_file;

// Fonction wrapper
void log_message(const char *message) {
    if (current_logger) {
        current_logger(message);
    }
}
```

**Utilisation dans le code :**

```c
// business_logic.c
#include "logger.h"

void process_order(int order_id) {
    log_message("Processing order");

    // Logique métier...

    log_message("Order completed");
}
```

**Tests avec stub :**

```c
// test_business_logic.c
#include "unity.h"
#include "logger.h"

// Stub : Ne fait rien
void log_stub(const char *message) {
    // Silencieux : n'écrit rien
}

void setUp(void) {
    // Remplacer le logger par le stub
    current_logger = log_stub;
}

void tearDown(void) {
    // Restaurer le logger original
    current_logger = log_to_file;
}

void test_process_order(void) {
    // Le test s'exécute sans créer de fichier log
    process_order(12345);

    // Vérifier la logique métier
    // ...
}
```

**Tests avec mock :**

```c
// Mock : Enregistre les appels
static int log_call_count = 0;  
static char logged_messages[10][256];  

void log_mock(const char *message) {
    if (log_call_count < 10) {
        strncpy(logged_messages[log_call_count], message, 255);
        log_call_count++;
    }
}

void test_process_order_logs_correctly(void) {
    log_call_count = 0;
    current_logger = log_mock;

    process_order(12345);

    // Vérifier les interactions
    TEST_ASSERT_EQUAL_INT(2, log_call_count);
    TEST_ASSERT_EQUAL_STRING("Processing order", logged_messages[0]);
    TEST_ASSERT_EQUAL_STRING("Order completed", logged_messages[1]);
}
```

### Avantages et inconvénients

**Avantages :**
- ✅ Portable (C standard)
- ✅ Explicite et compréhensible
- ✅ Contrôle total

**Inconvénients :**
- ❌ Nécessite de modifier le code (ajouter les pointeurs)
- ❌ Overhead minimal à l'exécution (appel indirect)
- ❌ Verbose pour beaucoup de dépendances

---

## Technique 2 : Injection de dépendances explicite

### Principe

Passer les dépendances en paramètres au lieu de les utiliser globalement.

### Exemple : Service Email

**❌ Mauvais (dépendance cachée) :**

```c
void send_welcome_email(const char *user_email) {
    // Dépendance cachée : fonction smtp_send
    smtp_send(user_email, "Welcome!", "Thank you for joining");
}
```

**✅ Bon (injection de dépendance) :**

```c
// Interface : pointeur de fonction
typedef int (*EmailSender)(const char *to, const char *subject, const char *body);

void send_welcome_email(const char *user_email, EmailSender sender) {
    if (sender) {
        sender(user_email, "Welcome!", "Thank you for joining");
    }
}
```

**Production :**

```c
int smtp_send_real(const char *to, const char *subject, const char *body) {
    // Vraie connexion SMTP
    return 0; // Success
}

// Appel en production
send_welcome_email("user@example.com", smtp_send_real);
```

**Tests :**

```c
// Stub
int email_sender_stub(const char *to, const char *subject, const char *body) {
    return 0; // Toujours succès
}

void test_send_welcome_email(void) {
    send_welcome_email("test@example.com", email_sender_stub);
    // Pas d'email réel envoyé
}

// Mock
static int email_sent_count = 0;  
static char last_recipient[100];  

int email_sender_mock(const char *to, const char *subject, const char *body) {
    email_sent_count++;
    strncpy(last_recipient, to, sizeof(last_recipient) - 1);
    return 0;
}

void test_email_is_sent_to_correct_recipient(void) {
    email_sent_count = 0;

    send_welcome_email("alice@example.com", email_sender_mock);

    TEST_ASSERT_EQUAL_INT(1, email_sent_count);
    TEST_ASSERT_EQUAL_STRING("alice@example.com", last_recipient);
}
```

### Structure avec contexte

Pour éviter de passer trop de paramètres, on peut utiliser une structure de contexte :

```c
typedef struct {
    EmailSender send_email;
    LogFunction log;
    DatabaseConnection *db;
} AppContext;

void process_user_registration(User *user, AppContext *ctx) {
    // Utiliser les dépendances du contexte
    ctx->log("Registering user");
    ctx->send_email(user->email, "Welcome", "...");
    // ...
}
```

**Tests :**

```c
void test_user_registration(void) {
    AppContext test_context = {
        .send_email = email_sender_stub,
        .log = log_stub,
        .db = &mock_db
    };

    User user = {.email = "test@example.com"};
    process_user_registration(&user, &test_context);
}
```

---

## Technique 3 : Wrapping avec le linker

### Principe

Utiliser l'option `--wrap` du linker GNU pour intercepter les appels de fonctions.

### Comment ça marche ?

Quand on compile avec `-Wl,--wrap=fonction`, le linker :
1. Redirige tous les appels à `fonction` vers `__wrap_fonction`
2. Rend accessible l'originale via `__real_fonction`

### Exemple : Wrapper time()

**Code de production :**

```c
// user.c
#include <time.h>

void update_user_timestamp(User *user) {
    user->last_seen = time(NULL); // Appel à la vraie fonction
}
```

**Test avec wrapping :**

```c
// test_user.c
#include "unity.h"
#include <time.h>

// Notre fausse implémentation
static time_t fake_time_value = 1234567890;

time_t __wrap_time(time_t *t) {
    if (t) *t = fake_time_value;
    return fake_time_value;
}

void test_update_timestamp(void) {
    User user = {0};
    fake_time_value = 1000000;

    update_user_timestamp(&user);

    // Le timestamp est prévisible
    TEST_ASSERT_EQUAL_INT64(1000000, user.last_seen);
}
```

**Compilation :**

```bash
gcc -o test_runner \
    test_user.c user.c \
    -Wl,--wrap=time \
    unity/unity.c
```

### Appeler la vraie fonction depuis le wrapper

```c
// Déclarer l'accès à la vraie fonction
extern time_t __real_time(time_t *t);

time_t __wrap_time(time_t *t) {
    // En mode test, retourner une valeur fixe
    if (in_test_mode) {
        return fake_time_value;
    }

    // Sinon, appeler la vraie fonction
    return __real_time(t);
}
```

### Wrapper plusieurs fonctions

```c
// Wrapper fopen
FILE *__wrap_fopen(const char *path, const char *mode) {
    // Retourner un fichier en mémoire ou NULL selon le test
    return fake_file;
}

// Wrapper malloc
void *__wrap_malloc(size_t size) {
    // Simuler une allocation échouée
    if (simulate_malloc_failure) {
        return NULL;
    }
    return __real_malloc(size);
}
```

**Compilation :**

```bash
gcc -o test_runner test.c code.c \
    -Wl,--wrap=fopen \
    -Wl,--wrap=malloc \
    -Wl,--wrap=time
```

### Avantages et inconvénients

**Avantages :**
- ✅ Pas besoin de modifier le code de production
- ✅ Fonctionne avec des fonctions de bibliothèque standard
- ✅ Transparent pour le code testé

**Inconvénients :**
- ❌ Spécifique à GNU (ld)
- ❌ Ne fonctionne pas toujours (fonctions inline, macros)
- ❌ Peut être difficile à déboguer

---

## Technique 4 : Macros conditionnelles

### Principe

Utiliser le préprocesseur pour remplacer les fonctions en mode test.

### Exemple : Remplacer rand()

**Code de production :**

```c
// random_utils.h
#ifndef RANDOM_UTILS_H
#define RANDOM_UTILS_H

#ifdef TESTING
    // En mode test, utiliser notre stub
    extern int test_random_value;
    #define GET_RANDOM() test_random_value
#else
    // En production, utiliser rand()
    #include <stdlib.h>
    #define GET_RANDOM() rand()
#endif

int get_random_number(int min, int max);

#endif
```

**Implémentation :**

```c
// random_utils.c
#include "random_utils.h"

int get_random_number(int min, int max) {
    int range = max - min + 1;
    return min + (GET_RANDOM() % range);
}
```

**Tests :**

```c
// test_random_utils.c
#define TESTING  // Activer le mode test
#include "random_utils.h"
#include "unity.h"

int test_random_value = 0; // Définir la variable

void test_random_number_returns_value_in_range(void) {
    // Contrôler la valeur "aléatoire"
    test_random_value = 5;

    int result = get_random_number(1, 10);

    // Résultat prévisible
    TEST_ASSERT_EQUAL_INT(6, result); // 1 + (5 % 10)
}
```

**Compilation :**

```bash
# Production
gcc -o app main.c random_utils.c

# Tests
gcc -DTESTING -o test_runner test_random_utils.c random_utils.c unity/unity.c
```

### Avantages et inconvénients

**Avantages :**
- ✅ Simple à mettre en place
- ✅ Pas de overhead en production
- ✅ Portable

**Inconvénients :**
- ❌ Pollue le code avec des `#ifdef`
- ❌ Nécessite une recompilation
- ❌ Difficile à maintenir pour beaucoup de dépendances

---

## Technique 5 : Frameworks de mocking

### cmocka : Mocking intégré

**cmocka** fournit des fonctions pour créer des mocks facilement.

#### Exemple : Mock d'une fonction de base de données

**Code de production :**

```c
// database.h
typedef struct {
    int id;
    char name[50];
} User;

User *db_get_user(int user_id);
```

**Tests avec cmocka :**

```c
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

// Mock de db_get_user
User *__wrap_db_get_user(int user_id) {
    check_expected(user_id); // Vérifier que le bon ID est passé
    return (User *)mock();    // Retourner une valeur mockée
}

void test_get_user_profile(void **state) {
    (void)state;

    // Configuration du mock
    User mock_user = {.id = 123, .name = "Alice"};

    // Attendre que db_get_user soit appelé avec 123
    expect_value(__wrap_db_get_user, user_id, 123);
    will_return(__wrap_db_get_user, &mock_user);

    // Appeler la fonction qui utilise db_get_user
    User *result = get_user_profile(123);

    // Vérifications
    assert_non_null(result);
    assert_int_equal(123, result->id);
    assert_string_equal("Alice", result->name);
}
```

**Compilation :**

```bash
gcc -o test_runner test.c \
    -Wl,--wrap=db_get_user \
    -lcmocka
```

### fff (Fake Function Framework)

**fff** est un framework header-only pour créer des fakes rapidement.

**Installation :**

```c
// Télécharger fff.h
#include "fff.h"
DEFINE_FFF_GLOBALS;
```

**Exemple :**

```c
// Déclarer une fonction fake
FAKE_VOID_FUNC(log_message, const char *);  
FAKE_VALUE_FUNC(int, send_email, const char *, const char *);  

void test_business_logic(void) {
    RESET_FAKE(log_message);
    RESET_FAKE(send_email);

    // Configurer le retour
    send_email_fake.return_val = 0; // Success

    // Exécuter le code testé
    my_function();

    // Vérifier les appels
    assert_int_equal(1, log_message_fake.call_count);
    assert_int_equal(1, send_email_fake.call_count);
    assert_string_equal("user@example.com", send_email_fake.arg0_val);
}
```

---

## Patterns de mocking courants

### 1. Simuler un échec d'allocation

```c
static int malloc_should_fail = 0;  
static int malloc_call_count = 0;  

void *__wrap_malloc(size_t size) {
    malloc_call_count++;

    if (malloc_should_fail) {
        return NULL;
    }

    return __real_malloc(size);
}

void test_handles_malloc_failure(void) {
    malloc_should_fail = 1;

    char *buffer = create_buffer(1024);

    // Vérifier que la fonction gère l'échec
    TEST_ASSERT_NULL(buffer);
}
```

### 2. Simuler une erreur réseau

```c
typedef enum {
    NETWORK_SUCCESS,
    NETWORK_TIMEOUT,
    NETWORK_CONNECTION_REFUSED
} NetworkStatus;

static NetworkStatus simulated_network_status = NETWORK_SUCCESS;

int __wrap_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
    switch (simulated_network_status) {
        case NETWORK_TIMEOUT:
            errno = ETIMEDOUT;
            return -1;
        case NETWORK_CONNECTION_REFUSED:
            errno = ECONNREFUSED;
            return -1;
        default:
            return 0;
    }
}

void test_handles_connection_timeout(void) {
    simulated_network_status = NETWORK_TIMEOUT;

    int result = connect_to_server("example.com", 80);

    TEST_ASSERT_EQUAL_INT(ERROR_TIMEOUT, result);
}
```

### 3. Vérifier l'ordre des appels

```c
typedef enum {
    CALL_OPEN,
    CALL_WRITE,
    CALL_CLOSE
} CallType;

static CallType call_sequence[10];  
static int call_index = 0;  

FILE *__wrap_fopen(const char *path, const char *mode) {
    call_sequence[call_index++] = CALL_OPEN;
    return (FILE *)0x1234; // Adresse factice
}

int __wrap_fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) {
    call_sequence[call_index++] = CALL_WRITE;
    return nmemb;
}

int __wrap_fclose(FILE *stream) {
    call_sequence[call_index++] = CALL_CLOSE;
    return 0;
}

void test_file_operations_in_correct_order(void) {
    call_index = 0;

    write_to_file("test.txt", "data");

    TEST_ASSERT_EQUAL_INT(CALL_OPEN, call_sequence[0]);
    TEST_ASSERT_EQUAL_INT(CALL_WRITE, call_sequence[1]);
    TEST_ASSERT_EQUAL_INT(CALL_CLOSE, call_sequence[2]);
}
```

### 4. Mock avec état

```c
typedef struct {
    int balance;
    int transaction_count;
} MockBankAccount;

static MockBankAccount mock_account;

void reset_mock_account(void) {
    mock_account.balance = 1000;
    mock_account.transaction_count = 0;
}

int mock_withdraw(int amount) {
    mock_account.transaction_count++;

    if (mock_account.balance >= amount) {
        mock_account.balance -= amount;
        return 0; // Success
    }

    return -1; // Insufficient funds
}

void test_withdrawal_updates_balance(void) {
    reset_mock_account();

    int result = process_withdrawal(100, mock_withdraw);

    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_INT(900, mock_account.balance);
    TEST_ASSERT_EQUAL_INT(1, mock_account.transaction_count);
}
```

---

## Bonnes pratiques

### 1. Isoler les dépendances dès la conception

**Mauvais design (difficile à tester) :**

```c
void save_user(User *user) {
    // Dépendances directes
    FILE *f = fopen("users.dat", "ab");
    fwrite(user, sizeof(User), 1, f);
    fclose(f);

    log_to_syslog("User saved");
}
```

**Bon design (testable) :**

```c
typedef struct {
    int (*save)(User *user);
    void (*log)(const char *message);
} UserRepository;

void save_user(User *user, UserRepository *repo) {
    repo->save(user);
    repo->log("User saved");
}
```

### 2. Garder les mocks simples

Un mock complexe est un code à maintenir. Privilégiez la simplicité.

```c
// ✅ Bon : Simple et clair
int mock_add(int a, int b) {
    return a + b;
}

// ❌ Mauvais : Trop complexe pour un mock
int complex_mock_with_state_machine(int a, int b) {
    static int state = 0;
    switch (state++) {
        case 0: return a + b;
        case 1: return a * b;
        default: return 0;
    }
}
```

### 3. Réinitialiser l'état entre les tests

```c
void setUp(void) {
    // Réinitialiser tous les mocks
    call_count = 0;
    malloc_should_fail = 0;
    memset(logged_messages, 0, sizeof(logged_messages));
}
```

### 4. Documenter les mocks

```c
/**
 * Mock de la fonction send_email.
 *
 * Comportement :
 * - Retourne toujours 0 (succès)
 * - Enregistre le destinataire dans last_recipient
 * - Incrémente email_sent_count
 */
int mock_send_email(const char *to, const char *subject, const char *body) {
    strncpy(last_recipient, to, sizeof(last_recipient) - 1);
    email_sent_count++;
    return 0;
}
```

### 5. Éviter de mocker ce qui n'est pas nécessaire

Ne mocker que les dépendances qui **empêchent le test** :

```c
// ❌ Pas besoin de mocker des fonctions pures
// int mock_strlen(const char *s) { return 5; }

// ✅ Mocker seulement ce qui a des effets de bord
int mock_fwrite(...) { /* ... */ }
```

---

## Quand NE PAS utiliser de mocks

### 1. Fonctions simples et pures

Si une fonction n'a pas de dépendance externe, testez-la directement.

```c
// Pas besoin de mock
int add(int a, int b) {
    return a + b;
}

void test_add(void) {
    TEST_ASSERT_EQUAL_INT(5, add(2, 3));
}
```

### 2. Tests d'intégration

Les tests d'intégration doivent tester les vraies interactions.

```c
// ❌ Test d'intégration avec mocks = Inutile
void test_database_integration_with_mocks(void) {
    mock_db_connect();
    mock_db_query();
    // Ne teste rien de réel
}

// ✅ Vrai test d'intégration
void test_database_integration_real(void) {
    DB *db = db_connect("test_db");
    User *user = db_query(db, "SELECT * FROM users WHERE id=1");
    TEST_ASSERT_NOT_NULL(user);
    db_disconnect(db);
}
```

### 3. Code critique qui doit être testé avec les vraies dépendances

Pour du code de sécurité ou critique, préférez des tests avec de vraies dépendances dans un environnement contrôlé.

---

## Exemple complet : Service d'envoi de notifications

### Code de production

```c
// notification_service.h
#ifndef NOTIFICATION_SERVICE_H
#define NOTIFICATION_SERVICE_H

#include <stdbool.h>

// Dépendances abstraites
typedef struct {
    bool (*send_sms)(const char *phone, const char *message);
    bool (*send_email)(const char *email, const char *subject, const char *body);
    void (*log)(const char *message);
} NotificationDeps;

// Service
typedef struct {
    NotificationDeps deps;
} NotificationService;

NotificationService *notification_service_create(NotificationDeps deps);  
void notification_service_destroy(NotificationService *service);  
bool notify_user(NotificationService *service, const char *user_id, const char *message);  

#endif
```

**Implémentation :**

```c
// notification_service.c
#include "notification_service.h"
#include <stdlib.h>
#include <string.h>

NotificationService *notification_service_create(NotificationDeps deps) {
    NotificationService *service = malloc(sizeof(NotificationService));
    if (service) {
        service->deps = deps;
    }
    return service;
}

void notification_service_destroy(NotificationService *service) {
    free(service);
}

bool notify_user(NotificationService *service, const char *user_id, const char *message) {
    if (!service || !user_id || !message) return false;

    service->deps.log("Sending notification");

    // Essayer SMS en premier
    char phone[20];
    sprintf(phone, "+1234567%s", user_id);
    if (service->deps.send_sms(phone, message)) {
        service->deps.log("SMS sent successfully");
        return true;
    }

    // Fallback sur email
    char email[100];
    sprintf(email, "user%s@example.com", user_id);
    if (service->deps.send_email(email, "Notification", message)) {
        service->deps.log("Email sent successfully");
        return true;
    }

    service->deps.log("All notification methods failed");
    return false;
}
```

### Tests avec stubs et mocks

```c
// test_notification_service.c
#include "unity.h"
#include "notification_service.h"

// Stubs et mocks
static int sms_sent_count = 0;  
static int email_sent_count = 0;  
static char logged_messages[10][256];  
static int log_count = 0;  

bool stub_send_sms_success(const char *phone, const char *message) {
    sms_sent_count++;
    return true;
}

bool stub_send_sms_failure(const char *phone, const char *message) {
    sms_sent_count++;
    return false;
}

bool stub_send_email_success(const char *email, const char *subject, const char *body) {
    email_sent_count++;
    return true;
}

void mock_log(const char *message) {
    if (log_count < 10) {
        strncpy(logged_messages[log_count], message, 255);
        log_count++;
    }
}

void setUp(void) {
    sms_sent_count = 0;
    email_sent_count = 0;
    log_count = 0;
    memset(logged_messages, 0, sizeof(logged_messages));
}

void test_notify_user_sends_sms_when_available(void) {
    NotificationDeps deps = {
        .send_sms = stub_send_sms_success,
        .send_email = stub_send_email_success,
        .log = mock_log
    };

    NotificationService *service = notification_service_create(deps);

    bool result = notify_user(service, "123", "Hello");

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(1, sms_sent_count);
    TEST_ASSERT_EQUAL_INT(0, email_sent_count); // Email pas utilisé
    TEST_ASSERT_EQUAL_INT(2, log_count);

    notification_service_destroy(service);
}

void test_notify_user_fallback_to_email(void) {
    NotificationDeps deps = {
        .send_sms = stub_send_sms_failure, // SMS échoue
        .send_email = stub_send_email_success,
        .log = mock_log
    };

    NotificationService *service = notification_service_create(deps);

    bool result = notify_user(service, "456", "Hello");

    TEST_ASSERT_TRUE(result);
    TEST_ASSERT_EQUAL_INT(1, sms_sent_count);
    TEST_ASSERT_EQUAL_INT(1, email_sent_count); // Fallback
    TEST_ASSERT_EQUAL_STRING("Email sent successfully", logged_messages[1]);

    notification_service_destroy(service);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_notify_user_sends_sms_when_available);
    RUN_TEST(test_notify_user_fallback_to_email);
    return UNITY_END();
}
```

---

## Conclusion

Le mocking et le stubbing en C nécessitent plus de travail manuel que dans les langages modernes, mais avec les bonnes techniques, ils deviennent gérables et même élégants.

### Récapitulatif des techniques

| Technique | Complexité | Portabilité | Cas d'usage |
|-----------|------------|-------------|-------------|
| **Pointeurs de fonctions** | Moyenne | ✅ Parfaite | Dépendances connues à l'avance |
| **Injection de dépendances** | Faible | ✅ Parfaite | Architecture propre |
| **Linker wrapping** | Faible | ⚠️ GNU uniquement | Fonctions système, quick & dirty |
| **Macros conditionnelles** | Faible | ✅ Parfaite | Peu de dépendances |
| **Frameworks (cmocka, fff)** | Moyenne | ✅ Bonne | Projets avec beaucoup de tests |

### Recommandations

1. **Pour débuter** : Injection de dépendances + pointeurs de fonctions
2. **Pour tests rapides** : Linker wrapping
3. **Pour projets sérieux** : cmocka ou fff
4. **Pour l'embarqué** : Pointeurs de fonctions (zéro overhead)

### Points clés à retenir

- ✅ **Isolez vos dépendances** dès la conception
- ✅ **Un mock = une responsabilité** (ne pas trop complexifier)
- ✅ **Réinitialisez l'état** entre les tests
- ✅ **Documentez vos mocks** pour faciliter la maintenance
- ✅ **Ne mocker que le nécessaire** (fonctions avec effets de bord)

Dans la section suivante (**29.5 Tests d'intégration**), nous verrons comment tester les interactions entre plusieurs modules avec de vraies dépendances dans des environnements contrôlés.

---

> **💡 Conseil final :** Le mocking est un outil, pas une fin en soi. Un test qui utilise trop de mocks peut devenir un test de mocks plutôt qu'un test de code réel. Trouvez le bon équilibre entre isolation et réalisme.

⏭️ [Tests d'intégration](/29-tests-et-qualite/05-tests-integration.md)
