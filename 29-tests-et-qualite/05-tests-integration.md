🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 29.5 Tests d'intégration

## Introduction

Vous avez testé vos fonctions individuellement avec des tests unitaires. Mais que se passe-t-il quand ces fonctions interagissent entre elles ? Quand votre code lit un vrai fichier, se connecte à une base de données, ou communique via le réseau ? C'est le rôle des **tests d'intégration** : vérifier que les composants fonctionnent correctement **ensemble**.

---

## Test unitaire vs Test d'intégration

### Différences fondamentales

| Aspect | Test unitaire | Test d'intégration |
|--------|---------------|-------------------|
| **Portée** | Une seule fonction/module | Plusieurs modules ensemble |
| **Dépendances** | Mockées/stubbées | Réelles ou simulées |
| **Vitesse** | Très rapide (< 1ms) | Plus lent (ms à secondes) |
| **Isolation** | Totale | Partielle ou nulle |
| **Complexité** | Simple | Plus complexe |
| **Objectif** | Vérifier la logique | Vérifier les interactions |

### Analogie

- **Test unitaire** = Tester chaque pièce d'une voiture séparément (moteur, freins, roues)
- **Test d'intégration** = Assembler la voiture et vérifier qu'elle roule

### Exemple comparatif

**Code à tester :**

```c
// user_service.c
typedef struct {
    int id;
    char name[50];
} User;

// Fonction 1 : Valider un utilisateur
bool validate_user(User *user) {
    if (!user) return false;
    if (strlen(user->name) == 0) return false;
    if (user->id <= 0) return false;
    return true;
}

// Fonction 2 : Sauvegarder dans un fichier
bool save_user_to_file(User *user, const char *filename) {
    FILE *f = fopen(filename, "a");
    if (!f) return false;

    fprintf(f, "%d,%s\n", user->id, user->name);
    fclose(f);
    return true;
}

// Fonction 3 : Enregistrer un utilisateur (combine les deux)
bool register_user(User *user, const char *filename) {
    if (!validate_user(user)) {
        return false;
    }
    return save_user_to_file(user, filename);
}
```

**Test unitaire (fonction isolée) :**

```c
void test_validate_user_with_valid_data(void) {
    User user = {.id = 1, .name = "Alice"};

    // Test d'une seule fonction
    TEST_ASSERT_TRUE(validate_user(&user));
}

void test_validate_user_with_empty_name(void) {
    User user = {.id = 1, .name = ""};

    TEST_ASSERT_FALSE(validate_user(&user));
}
```

**Test d'intégration (plusieurs modules ensemble) :**

```c
void test_register_user_creates_file_entry(void) {
    User user = {.id = 123, .name = "Bob"};
    const char *test_file = "/tmp/test_users.txt";

    // Nettoyer avant
    remove(test_file);

    // Appeler la fonction qui combine validation + sauvegarde
    bool result = register_user(&user, test_file);

    TEST_ASSERT_TRUE(result);

    // Vérifier que le fichier a été créé avec le bon contenu
    FILE *f = fopen(test_file, "r");
    TEST_ASSERT_NOT_NULL(f);

    char line[100];
    fgets(line, sizeof(line), f);
    TEST_ASSERT_EQUAL_STRING("123,Bob\n", line);

    fclose(f);
    remove(test_file);
}
```

**Différence clé :** Le test d'intégration vérifie que :
1. La validation fonctionne
2. La sauvegarde fonctionne
3. **Les deux fonctionnent ensemble correctement**
4. **Le fichier réel est créé avec le bon format**

---

## Pourquoi les tests d'intégration sont essentiels

### Les bugs que seuls les tests d'intégration détectent

#### 1. Problèmes d'interface entre modules

```c
// Module A : Retourne une chaîne allouée dynamiquement
char *module_a_get_data(void) {
    char *data = malloc(100);
    strcpy(data, "Hello");
    return data;
}

// Module B : Utilise les données
void module_b_process(char *data) {
    printf("%s\n", data);
    // ❌ Oublie de free(data) !
}

// Tests unitaires : Chaque fonction passe
void test_module_a(void) {
    char *data = module_a_get_data();
    TEST_ASSERT_NOT_NULL(data);
    free(data); // Le test libère correctement
}

void test_module_b(void) {
    char *data = "Test";
    module_b_process(data); // Pas de fuite avec une string littérale
}

// Test d'intégration : Détecte la fuite
void test_integration_detects_leak(void) {
    char *data = module_a_get_data();
    module_b_process(data);
    // ❌ Valgrind détectera la fuite ici !
    // Le test d'intégration révèle que module_b ne respecte pas le contrat
}
```

#### 2. Problèmes de format de données

```c
// Module de sérialisation
char *serialize_user(User *user) {
    char *json = malloc(256);
    sprintf(json, "{id:%d,name:\"%s\"}", user->id, user->name);
    return json; // ❌ Format JSON invalide (manque les guillemets autour de "id")
}

// Module de désérialisation
User *deserialize_user(const char *json) {
    // Attend un JSON valide : {"id":123,"name":"Alice"}
    // ...
}

// Tests unitaires : Peuvent passer si on ne vérifie pas le format exact
// Test d'intégration : Échoue car le format ne matche pas
void test_serialize_deserialize_roundtrip(void) {
    User original = {.id = 42, .name = "Test"};

    char *json = serialize_user(&original);
    User *restored = deserialize_user(json);

    // ❌ Échoue car le JSON est invalide
    TEST_ASSERT_NOT_NULL(restored);
}
```

#### 3. Problèmes de timing et de concurrence

```c
// Thread 1 : Écrit dans un fichier
void *writer_thread(void *arg) {
    write_log("Event A");
    write_log("Event B");
    return NULL;
}

// Thread 2 : Lit le fichier
void *reader_thread(void *arg) {
    char *content = read_log();
    // Suppose que les deux lignes sont présentes
    return NULL;
}

// Test unitaire : Chaque fonction fonctionne seule
// Test d'intégration : Révèle une race condition
```

---

## Types de tests d'intégration

### 1. Tests d'intégration de composants (Component Integration)

Tester l'interaction entre 2-3 modules étroitement liés.

**Exemple : Parser + Validator**

```c
// parser.c
typedef struct {
    char key[50];
    char value[100];
} ConfigEntry;

ConfigEntry *parse_config_line(const char *line) {
    ConfigEntry *entry = malloc(sizeof(ConfigEntry));
    sscanf(line, "%[^=]=%s", entry->key, entry->value);
    return entry;
}

// validator.c
bool validate_config_entry(ConfigEntry *entry) {
    if (strlen(entry->key) == 0) return false;
    if (strlen(entry->value) == 0) return false;
    return true;
}

// Test d'intégration
void test_parse_and_validate_config(void) {
    const char *line = "port=8080";

    // Parser
    ConfigEntry *entry = parse_config_line(line);
    TEST_ASSERT_NOT_NULL(entry);

    // Valider
    bool valid = validate_config_entry(entry);
    TEST_ASSERT_TRUE(valid);

    // Vérifier le contenu
    TEST_ASSERT_EQUAL_STRING("port", entry->key);
    TEST_ASSERT_EQUAL_STRING("8080", entry->value);

    free(entry);
}
```

### 2. Tests d'intégration avec fichiers

Tester les opérations réelles sur le système de fichiers.

**Exemple : Configuration complète**

```c
// config_manager.c
typedef struct {
    char db_host[100];
    int db_port;
    char log_file[256];
} AppConfig;

AppConfig *load_config(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) return NULL;

    AppConfig *config = malloc(sizeof(AppConfig));
    char line[256];

    while (fgets(line, sizeof(line), f)) {
        char key[50], value[100];
        sscanf(line, "%[^=]=%s", key, value);

        if (strcmp(key, "db_host") == 0) {
            strcpy(config->db_host, value);
        } else if (strcmp(key, "db_port") == 0) {
            config->db_port = atoi(value);
        } else if (strcmp(key, "log_file") == 0) {
            strcpy(config->log_file, value);
        }
    }

    fclose(f);
    return config;
}

// Test d'intégration avec fichier temporaire
void test_load_config_from_file(void) {
    // Créer un fichier de config temporaire
    const char *test_file = "/tmp/test_config.ini";
    FILE *f = fopen(test_file, "w");
    fprintf(f, "db_host=localhost\n");
    fprintf(f, "db_port=5432\n");
    fprintf(f, "log_file=/var/log/app.log\n");
    fclose(f);

    // Charger la config
    AppConfig *config = load_config(test_file);

    // Vérifications
    TEST_ASSERT_NOT_NULL(config);
    TEST_ASSERT_EQUAL_STRING("localhost", config->db_host);
    TEST_ASSERT_EQUAL_INT(5432, config->db_port);
    TEST_ASSERT_EQUAL_STRING("/var/log/app.log", config->log_file);

    // Nettoyage
    free(config);
    remove(test_file);
}
```

### 3. Tests d'intégration avec base de données

Utiliser une base de données de test réelle (souvent SQLite pour la simplicité).

**Exemple : Couche d'accès aux données**

```c
#include <sqlite3.h>

// database.c
typedef struct {
    sqlite3 *db;
} Database;

Database *db_connect(const char *filename) {
    Database *database = malloc(sizeof(Database));

    int rc = sqlite3_open(filename, &database->db);
    if (rc != SQLITE_OK) {
        free(database);
        return NULL;
    }

    return database;
}

bool db_create_user_table(Database *database) {
    const char *sql =
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY,"
        "name TEXT NOT NULL"
        ");";

    char *err_msg = NULL;
    int rc = sqlite3_exec(database->db, sql, NULL, NULL, &err_msg);

    if (rc != SQLITE_OK) {
        sqlite3_free(err_msg);
        return false;
    }

    return true;
}

bool db_insert_user(Database *database, int id, const char *name) {
    char sql[256];
    sprintf(sql, "INSERT INTO users (id, name) VALUES (%d, '%s');", id, name);

    char *err_msg = NULL;
    int rc = sqlite3_exec(database->db, sql, NULL, NULL, &err_msg);

    if (rc != SQLITE_OK) {
        sqlite3_free(err_msg);
        return false;
    }

    return true;
}

int db_count_users(Database *database) {
    sqlite3_stmt *stmt;
    const char *sql = "SELECT COUNT(*) FROM users;";

    if (sqlite3_prepare_v2(database->db, sql, -1, &stmt, NULL) != SQLITE_OK) {
        return -1;
    }

    int count = 0;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        count = sqlite3_column_int(stmt, 0);
    }

    sqlite3_finalize(stmt);
    return count;
}

void db_close(Database *database) {
    if (database) {
        sqlite3_close(database->db);
        free(database);
    }
}

// Test d'intégration complet
void test_database_operations(void) {
    const char *test_db = ":memory:"; // Base en mémoire (rapide)

    // Connexion
    Database *db = db_connect(test_db);
    TEST_ASSERT_NOT_NULL(db);

    // Créer la table
    bool created = db_create_user_table(db);
    TEST_ASSERT_TRUE(created);

    // Insérer des utilisateurs
    TEST_ASSERT_TRUE(db_insert_user(db, 1, "Alice"));
    TEST_ASSERT_TRUE(db_insert_user(db, 2, "Bob"));

    // Vérifier le nombre
    int count = db_count_users(db);
    TEST_ASSERT_EQUAL_INT(2, count);

    // Nettoyage
    db_close(db);
}
```

**Conseil :** Utilisez `:memory:` pour SQLite dans les tests (ultra-rapide, pas de fichier à nettoyer).

### 4. Tests d'intégration réseau

Tester les communications client/serveur réelles.

**Exemple : Client HTTP simple**

```c
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// http_client.c
char *http_get(const char *host, int port, const char *path) {
    // Créer socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) return NULL;

    // Configuration serveur
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, host, &server_addr.sin_addr);

    // Connexion
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(sockfd);
        return NULL;
    }

    // Envoyer requête
    char request[512];
    sprintf(request, "GET %s HTTP/1.1\r\nHost: %s\r\n\r\n", path, host);
    send(sockfd, request, strlen(request), 0);

    // Recevoir réponse
    char *response = malloc(4096);
    int bytes = recv(sockfd, response, 4095, 0);
    response[bytes] = '\0';

    close(sockfd);
    return response;
}

// Test d'intégration avec un vrai serveur
void test_http_get_integration(void) {
    // Nécessite un serveur de test sur localhost:8080
    char *response = http_get("127.0.0.1", 8080, "/test");

    TEST_ASSERT_NOT_NULL(response);
    TEST_ASSERT_TRUE(strstr(response, "HTTP/1.1 200 OK") != NULL);

    free(response);
}
```

**Note :** Pour les tests réseau, vous pouvez :
- Lancer un serveur de test dans le même processus
- Utiliser un serveur HTTP léger (comme `python3 -m http.server`)
- Utiliser des outils comme `nc` (netcat) pour simuler des réponses

---

## Organisation des tests d'intégration

### Structure de projet recommandée

```
project/
├── src/
│   ├── parser.c
│   ├── validator.c
│   ├── database.c
│   └── http_client.c
├── tests/
│   ├── unit/              # Tests unitaires
│   │   ├── test_parser.c
│   │   ├── test_validator.c
│   │   └── test_database.c
│   └── integration/       # Tests d'intégration
│       ├── test_config_loading.c
│       ├── test_database_operations.c
│       └── test_http_workflow.c
└── test_data/             # Données de test
    ├── sample_config.ini
    └── test_database.db
```

### Séparation dans CMake

```cmake
# Tests unitaires (rapides)
add_executable(unit_tests
    tests/unit/test_parser.c
    tests/unit/test_validator.c
    src/parser.c
    src/validator.c
)

add_test(NAME UnitTests COMMAND unit_tests)

# Tests d'intégration (plus lents)
add_executable(integration_tests
    tests/integration/test_config_loading.c
    tests/integration/test_database_operations.c
    src/parser.c
    src/validator.c
    src/database.c
)

target_link_libraries(integration_tests sqlite3)  
add_test(NAME IntegrationTests COMMAND integration_tests)  

# Exécuter séparément
# make && ctest -R UnitTests           # Tests unitaires seulement
# make && ctest -R IntegrationTests    # Tests d'intégration seulement
```

---

## Fixtures et données de test

### Qu'est-ce qu'une fixture ?

Une **fixture** est l'état initial nécessaire pour exécuter un test (fichiers, base de données, etc.).

### Créer des fixtures réutilisables

```c
// test_fixtures.c
#include <stdio.h>
#include <stdlib.h>

// Fixture : Créer un fichier de configuration
const char *create_test_config_file(void) {
    static char filename[256];
    sprintf(filename, "/tmp/test_config_%d.ini", rand());

    FILE *f = fopen(filename, "w");
    fprintf(f, "server=localhost\n");
    fprintf(f, "port=5432\n");
    fprintf(f, "debug=true\n");
    fclose(f);

    return filename;
}

// Fixture : Créer une base de données de test
Database *create_test_database(void) {
    Database *db = db_connect(":memory:");
    db_create_user_table(db);

    // Insérer des données de test
    db_insert_user(db, 1, "Alice");
    db_insert_user(db, 2, "Bob");
    db_insert_user(db, 3, "Charlie");

    return db;
}

// Fixture : Nettoyer un fichier
void cleanup_test_file(const char *filename) {
    remove(filename);
}

// Utilisation dans les tests
void test_with_fixtures(void) {
    // Setup : Créer la fixture
    const char *config_file = create_test_config_file();
    Database *db = create_test_database();

    // Test
    AppConfig *config = load_config(config_file);
    TEST_ASSERT_NOT_NULL(config);

    int count = db_count_users(db);
    TEST_ASSERT_EQUAL_INT(3, count);

    // Teardown : Nettoyer
    free(config);
    db_close(db);
    cleanup_test_file(config_file);
}
```

### Fixtures globales (setup/teardown de suite)

Avec **Unity** :

```c
// Fixtures globales
static Database *global_test_db = NULL;

void suiteSetUp(void) {
    // Appelé une fois avant tous les tests de la suite
    global_test_db = create_test_database();
}

void suiteTearDown(void) {
    // Appelé une fois après tous les tests
    db_close(global_test_db);
}

void setUp(void) {
    // Appelé avant chaque test
    // Réinitialiser l'état si nécessaire
}

void tearDown(void) {
    // Appelé après chaque test
}
```

---

## Tests d'intégration avec processus externes

### Lancer un serveur de test

Certains tests nécessitent un serveur HTTP, un daemon, etc.

**Exemple : Tester un client avec un serveur de test**

```c
#include <sys/wait.h>
#include <unistd.h>

// Démarrer un serveur HTTP simple en arrière-plan
pid_t start_test_server(void) {
    pid_t pid = fork();

    if (pid == 0) {
        // Processus enfant : Démarrer le serveur
        execlp("python3", "python3", "-m", "http.server", "8080", NULL);
        exit(1); // Si exec échoue
    }

    // Processus parent : Attendre que le serveur soit prêt
    sleep(1);
    return pid;
}

void stop_test_server(pid_t pid) {
    kill(pid, SIGTERM);
    waitpid(pid, NULL, 0);
}

void test_with_real_server(void) {
    // Setup : Démarrer le serveur
    pid_t server_pid = start_test_server();

    // Test : Faire une requête HTTP
    char *response = http_get("127.0.0.1", 8080, "/");
    TEST_ASSERT_NOT_NULL(response);
    free(response);

    // Teardown : Arrêter le serveur
    stop_test_server(server_pid);
}
```

### Utiliser des containers Docker pour l'isolation

Pour des tests d'intégration complexes, Docker permet d'avoir des environnements propres.

**Script de test avec Docker :**

```bash
#!/bin/bash
# run_integration_tests.sh

# Démarrer une base PostgreSQL de test
docker run -d --name test_postgres \
    -e POSTGRES_PASSWORD=test \
    -p 5433:5432 \
    postgres:15

# Attendre que la DB soit prête
sleep 3

# Exécuter les tests
./integration_tests

# Nettoyer
docker stop test_postgres  
docker rm test_postgres  
```

---

## Tests d'intégration multi-threads

### Tester la concurrence réelle

```c
#include <pthread.h>

// Code à tester : Compteur thread-safe
typedef struct {
    int count;
    pthread_mutex_t mutex;
} Counter;

Counter *counter_create(void) {
    Counter *c = malloc(sizeof(Counter));
    c->count = 0;
    pthread_mutex_init(&c->mutex, NULL);
    return c;
}

void counter_increment(Counter *c) {
    pthread_mutex_lock(&c->mutex);
    c->count++;
    pthread_mutex_unlock(&c->mutex);
}

int counter_get(Counter *c) {
    pthread_mutex_lock(&c->mutex);
    int value = c->count;
    pthread_mutex_unlock(&c->mutex);
    return value;
}

void counter_destroy(Counter *c) {
    pthread_mutex_destroy(&c->mutex);
    free(c);
}

// Test d'intégration : Plusieurs threads incrémentent
typedef struct {
    Counter *counter;
    int iterations;
} ThreadArg;

void *increment_thread(void *arg) {
    ThreadArg *targ = (ThreadArg *)arg;

    for (int i = 0; i < targ->iterations; i++) {
        counter_increment(targ->counter);
    }

    return NULL;
}

void test_counter_thread_safety(void) {
    Counter *counter = counter_create();

    // Créer 10 threads qui incrémentent 1000 fois chacun
    const int NUM_THREADS = 10;
    const int ITERATIONS = 1000;

    pthread_t threads[NUM_THREADS];
    ThreadArg arg = {.counter = counter, .iterations = ITERATIONS};

    // Lancer les threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, increment_thread, &arg);
    }

    // Attendre la fin
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Vérifier le résultat
    int final_count = counter_get(counter);
    TEST_ASSERT_EQUAL_INT(NUM_THREADS * ITERATIONS, final_count);

    counter_destroy(counter);
}
```

**Ce que ce test vérifie :**
- Pas de race condition
- Les mutex fonctionnent correctement
- Le compteur final est correct (10 × 1000 = 10000)

---

## Techniques avancées

### 1. Tests de bout en bout (End-to-End)

Tester l'application complète comme le ferait un utilisateur.

**Exemple : Programme CLI**

```c
// Application : un outil de ligne de commande
int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <command>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "add") == 0) {
        return cmd_add(argc - 2, argv + 2);
    } else if (strcmp(argv[1], "list") == 0) {
        return cmd_list();
    }

    return 1;
}

// Test E2E : Exécuter le programme réel
void test_cli_add_and_list(void) {
    // Nettoyer l'état
    remove("tasks.db");

    // Exécuter : myapp add "Task 1"
    int result = system("./myapp add \"Task 1\"");
    TEST_ASSERT_EQUAL_INT(0, WEXITSTATUS(result));

    // Exécuter : myapp add "Task 2"
    result = system("./myapp add \"Task 2\"");
    TEST_ASSERT_EQUAL_INT(0, WEXITSTATUS(result));

    // Exécuter : myapp list > output.txt
    result = system("./myapp list > /tmp/output.txt");
    TEST_ASSERT_EQUAL_INT(0, WEXITSTATUS(result));

    // Vérifier la sortie
    FILE *f = fopen("/tmp/output.txt", "r");
    char line[256];
    fgets(line, sizeof(line), f);
    TEST_ASSERT_TRUE(strstr(line, "Task 1") != NULL);
    fgets(line, sizeof(line), f);
    TEST_ASSERT_TRUE(strstr(line, "Task 2") != NULL);
    fclose(f);

    // Nettoyage
    remove("tasks.db");
    remove("/tmp/output.txt");
}
```

### 2. Tests de performance d'intégration

Mesurer les performances des interactions réelles.

```c
#include <time.h>

void test_database_bulk_insert_performance(void) {
    Database *db = db_connect(":memory:");
    db_create_user_table(db);

    const int NUM_INSERTS = 10000;

    // Mesurer le temps
    clock_t start = clock();

    for (int i = 0; i < NUM_INSERTS; i++) {
        char name[50];
        sprintf(name, "User%d", i);
        db_insert_user(db, i, name);
    }

    clock_t end = clock();
    double elapsed = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Inserted %d users in %.3f seconds\n", NUM_INSERTS, elapsed);

    // Vérifier qu'on reste sous un seuil
    TEST_ASSERT_TRUE(elapsed < 1.0); // Moins de 1 seconde

    db_close(db);
}
```

### 3. Tests avec injection d'erreurs

Simuler des erreurs pour tester la robustesse.

```c
// Simuler un disque plein
void test_handles_disk_full_error(void) {
    // Créer un petit fichier système (100KB)
    system("dd if=/dev/zero of=/tmp/small_disk.img bs=1K count=100");
    system("mkfs.ext4 /tmp/small_disk.img");
    system("mkdir -p /tmp/test_mount");
    system("mount -o loop /tmp/small_disk.img /tmp/test_mount");

    // Tenter d'écrire beaucoup de données
    bool result = write_large_file("/tmp/test_mount/big.dat", 1000000);

    // Doit échouer gracieusement
    TEST_ASSERT_FALSE(result);

    // Vérifier qu'on a bien géré l'erreur (pas de crash)
    // ...

    // Nettoyage
    system("umount /tmp/test_mount");
    remove("/tmp/small_disk.img");
}
```

---

## Bonnes pratiques

### 1. Isoler les tests d'intégration

Chaque test doit être indépendant et ne pas affecter les autres.

```c
// ❌ MAUVAIS : Tests partagent l'état
static Database *shared_db = NULL;

void test_insert_user(void) {
    db_insert_user(shared_db, 1, "Alice");
    TEST_ASSERT_EQUAL_INT(1, db_count_users(shared_db));
}

void test_delete_user(void) {
    // ❌ Dépend de test_insert_user
    db_delete_user(shared_db, 1);
    TEST_ASSERT_EQUAL_INT(0, db_count_users(shared_db));
}

// ✅ BON : Chaque test a son propre état
void test_insert_user_isolated(void) {
    Database *db = db_connect(":memory:");
    db_create_user_table(db);

    db_insert_user(db, 1, "Alice");
    TEST_ASSERT_EQUAL_INT(1, db_count_users(db));

    db_close(db);
}

void test_delete_user_isolated(void) {
    Database *db = db_connect(":memory:");
    db_create_user_table(db);
    db_insert_user(db, 1, "Alice");

    db_delete_user(db, 1);
    TEST_ASSERT_EQUAL_INT(0, db_count_users(db));

    db_close(db);
}
```

### 2. Utiliser des répertoires temporaires

```c
#include <stdlib.h>

char *create_temp_directory(void) {
    char template[] = "/tmp/test_XXXXXX";
    char *temp_dir = mkdtemp(template);
    return strdup(temp_dir);
}

void test_with_temp_dir(void) {
    char *temp_dir = create_temp_directory();

    char file_path[512];
    sprintf(file_path, "%s/test.txt", temp_dir);

    // Utiliser le fichier
    write_file(file_path, "Test data");

    // Nettoyage automatique
    char cleanup_cmd[512];
    sprintf(cleanup_cmd, "rm -rf %s", temp_dir);
    system(cleanup_cmd);
    free(temp_dir);
}
```

### 3. Timeout pour les tests lents

Les tests d'intégration peuvent bloquer (réseau, deadlock, etc.).

Avec **Check** :

```c
// Timeout de 5 secondes
tcase_set_timeout(tc_integration, 5);
```

Avec un timer manuel :

```c
#include <signal.h>

static void timeout_handler(int sig) {
    TEST_FAIL_MESSAGE("Test timeout");
}

void test_with_timeout(void) {
    // Configurer un timeout de 3 secondes
    signal(SIGALRM, timeout_handler);
    alarm(3);

    // Code de test
    // ...

    // Annuler le timeout
    alarm(0);
}
```

### 4. Logs détaillés pour le débogage

```c
void test_complex_workflow(void) {
    printf("[TEST] Starting test_complex_workflow\n");

    printf("[TEST] Step 1: Connecting to database\n");
    Database *db = db_connect(":memory:");
    TEST_ASSERT_NOT_NULL(db);

    printf("[TEST] Step 2: Creating tables\n");
    bool created = db_create_tables(db);
    TEST_ASSERT_TRUE(created);

    printf("[TEST] Step 3: Inserting data\n");
    // ...

    printf("[TEST] Test completed successfully\n");
}
```

### 5. Utiliser des variables d'environnement

Configurer les tests via l'environnement.

```c
const char *get_test_db_path(void) {
    const char *path = getenv("TEST_DB_PATH");
    return path ? path : ":memory:";
}

void test_database_with_env_config(void) {
    Database *db = db_connect(get_test_db_path());
    // ...
}
```

**Utilisation :**

```bash
# Base en mémoire (rapide)
./integration_tests

# Base sur disque (pour debugging)
TEST_DB_PATH=/tmp/test.db ./integration_tests
```

---

## Intégration dans le CI/CD

### GitHub Actions : Tests d'intégration avec services

```yaml
name: Integration Tests

on: [push, pull_request]

jobs:
  integration:
    runs-on: ubuntu-latest

    services:
      postgres:
        image: postgres:15
        env:
          POSTGRES_PASSWORD: test
        options: >-
          --health-cmd pg_isready
          --health-interval 10s
          --health-timeout 5s
          --health-retries 5
        ports:
          - 5432:5432

    steps:
      - uses: actions/checkout@v4

      - name: Install dependencies
        run: |
          sudo apt-get update
          sudo apt-get install -y libsqlite3-dev libpq-dev

      - name: Build
        run: |
          mkdir build && cd build
          cmake ..
          make

      - name: Run integration tests
        env:
          TEST_DB_HOST: localhost
          TEST_DB_PORT: 5432
          TEST_DB_PASSWORD: test
        run: |
          cd build
          ./integration_tests
```

### Séparer tests rapides et lents

```yaml
jobs:
  unit-tests:
    runs-on: ubuntu-latest
    steps:
      - run: ./unit_tests  # Rapides, toujours exécutés

  integration-tests:
    runs-on: ubuntu-latest
    # Seulement sur main ou PR
    if: github.ref == 'refs/heads/main' || github.event_name == 'pull_request'
    steps:
      - run: ./integration_tests  # Plus lents, exécution conditionnelle
```

---

## Debugging des tests d'intégration

### Problème : Test qui échoue de manière intermittente

**Cause fréquente :** Race condition, timing

**Solution :**

```c
// Au lieu de sleep(1) fixe
void wait_for_file(const char *path, int max_seconds) {
    for (int i = 0; i < max_seconds * 10; i++) {
        if (access(path, F_OK) == 0) {
            return; // Fichier existe
        }
        usleep(100000); // 100ms
    }
    TEST_FAIL_MESSAGE("Timeout waiting for file");
}

void test_async_file_creation(void) {
    start_async_writer();
    wait_for_file("/tmp/output.txt", 5); // Max 5 secondes
    // ...
}
```

### Problème : Nettoyer l'état entre tests

**Solution : Utiliser un wrapper setUp/tearDown robuste**

```c
static char *test_data_dir = NULL;

void setUp(void) {
    // Créer un répertoire unique pour ce test
    char template[] = "/tmp/test_XXXXXX";
    test_data_dir = strdup(mkdtemp(template));
}

void tearDown(void) {
    // Nettoyer tout le répertoire
    if (test_data_dir) {
        char cmd[512];
        sprintf(cmd, "rm -rf %s", test_data_dir);
        system(cmd);
        free(test_data_dir);
        test_data_dir = NULL;
    }
}
```

---

## Récapitulatif : Quand utiliser quel type de test ?

| Scénario | Type de test | Exemple |
|----------|--------------|---------|
| Tester une fonction mathématique | Unitaire | `test_add(2, 3) == 5` |
| Tester un parser de JSON | Unitaire | `parse_json("{}")` |
| Tester que le parser + validator marchent ensemble | Intégration (composants) | Parser → Validator |
| Tester la lecture d'un vrai fichier | Intégration (I/O) | Créer fichier → Lire → Vérifier |
| Tester une requête SQL réelle | Intégration (DB) | INSERT → SELECT → Vérifier |
| Tester un client HTTP avec un serveur | Intégration (réseau) | Démarrer serveur → GET → Vérifier réponse |
| Tester le programme complet | End-to-end | `./app add "Task" && ./app list` |
| Tester sous forte charge | Performance | Insérer 100k lignes en < 5s |

---

## Conclusion

Les tests d'intégration sont le complément indispensable des tests unitaires. Ils révèlent les bugs qui ne peuvent être détectés qu'en faisant interagir plusieurs composants réellement.

### Points clés à retenir

1. **Tests unitaires** = Logique isolée (rapides, nombreux)
2. **Tests d'intégration** = Interactions réelles (plus lents, sélectifs)
3. **Isoler chaque test** : État propre, pas de dépendances entre tests
4. **Utiliser des fixtures** : Données de test réutilisables
5. **Nettoyer systématiquement** : Fichiers temporaires, bases de données
6. **Timeouts** : Éviter les tests bloqués
7. **CI/CD** : Automatiser les tests d'intégration avec services (Docker, GitHub Actions)

### Pyramide des tests revisitée

```
        /\
       /  \       E2E Tests (5%)
      /────\      - Très lents
     /      \     - Application complète
    /────────\
   /          \   Integration Tests (15%)
  /────────────\  - Moyennement rapides
 /              \ - Plusieurs modules
/────────────────\
                  Unit Tests (80%)
                  - Très rapides
                  - Fonctions isolées
```

**Objectif :** 80% de tests unitaires rapides, 15% d'intégration, 5% end-to-end.

Dans la section suivante (**29.6 Mesure de couverture**), nous verrons comment mesurer l'efficacité de vos tests avec des outils de code coverage.

---

> **💡 Conseil final :** Ne sacrifiez pas les tests unitaires pour les tests d'intégration. Les deux sont complémentaires : les tests unitaires donnent confiance dans la logique, les tests d'intégration donnent confiance dans l'assemblage.

⏭️ [Mesure de couverture](/29-tests-et-qualite/06-mesure-couverture.md)
