🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 20.10 Création d'un Mini-Serveur HTTP

## Introduction

Maintenant que nous maîtrisons les sockets TCP, la gestion d'erreurs, et les serveurs concurrents, nous allons mettre tout cela en pratique en créant un **mini-serveur HTTP**.

HTTP (HyperText Transfer Protocol) est le protocole qui fait fonctionner le Web. Chaque fois que vous naviguez sur Internet, votre navigateur communique avec des serveurs web via HTTP.

Dans cette section, nous allons :
- Comprendre les bases du protocole HTTP
- Parser des requêtes HTTP
- Générer des réponses HTTP
- Servir des fichiers statiques (HTML, CSS, images)
- Créer un serveur HTTP fonctionnel en C

**Objectif :** Créer un serveur web minimal mais fonctionnel, comme une version ultra-simplifiée de Nginx ou Apache.

---

## Le Protocole HTTP : Les Bases

### Qu'est-ce que HTTP ?

HTTP est un protocole **texte** basé sur **requête/réponse**.

**Analogie :** C'est comme une conversation standardisée au restaurant :
- **Client (vous) :** "Je voudrais un café, s'il vous plaît" (requête)
- **Serveur (serveur) :** "Voici votre café" (réponse)

### Architecture HTTP

```
Client (Navigateur)          Serveur Web
       |                           |
       |--- Requête HTTP --------->|
       |                           |
       |<-- Réponse HTTP ----------|
       |                           |
```

### Caractéristiques d'HTTP

✅ **Sans état (stateless)** : Chaque requête est indépendante

✅ **Texte** : Les en-têtes sont en texte lisible (pas binaire)

✅ **TCP** : HTTP utilise TCP (port 80 pour HTTP, 443 pour HTTPS)

✅ **Requête/Réponse** : Communication en deux temps

---

## Anatomie d'une Requête HTTP

### Format Général

```
MÉTHODE /chemin HTTP/1.1  
En-Tête-1: Valeur1  
En-Tête-2: Valeur2  

Corps (optionnel)
```

### Exemple Concret

```http
GET /index.html HTTP/1.1  
Host: www.example.com  
User-Agent: Mozilla/5.0  
Accept: text/html  
Connection: close  

```

**Décomposition :**

1. **Ligne de requête :** `GET /index.html HTTP/1.1`
   - `GET` : Méthode (action demandée)
   - `/index.html` : Chemin de la ressource
   - `HTTP/1.1` : Version du protocole

2. **En-têtes :** Métadonnées sur la requête
   - `Host` : Nom de domaine du serveur
   - `User-Agent` : Navigateur utilisé
   - `Accept` : Types de contenu acceptés
   - `Connection` : Gestion de la connexion

3. **Ligne vide :** Sépare les en-têtes du corps (`\r\n\r\n`)

4. **Corps :** Données (absent pour GET, présent pour POST)

---

## Méthodes HTTP Principales

| Méthode | Description | A un corps ? |
|---------|-------------|--------------|
| **GET** | Récupérer une ressource | Non |
| **POST** | Envoyer des données au serveur | Oui |
| **HEAD** | Comme GET mais sans le corps de réponse | Non |
| **PUT** | Créer/remplacer une ressource | Oui |
| **DELETE** | Supprimer une ressource | Non |
| **OPTIONS** | Lister les méthodes supportées | Non |

**Notre mini-serveur** implémentera principalement **GET** et **HEAD**.

---

## Anatomie d'une Réponse HTTP

### Format Général

```
HTTP/1.1 CODE Message  
En-Tête-1: Valeur1  
En-Tête-2: Valeur2  

Corps
```

### Exemple Concret

```http
HTTP/1.1 200 OK  
Content-Type: text/html  
Content-Length: 137  
Connection: close  

<!DOCTYPE html>
<html>
<head><title>Hello</title></head>
<body><h1>Hello, World!</h1></body>
</html>
```

**Décomposition :**

1. **Ligne de statut :** `HTTP/1.1 200 OK`
   - `HTTP/1.1` : Version du protocole
   - `200` : Code de statut
   - `OK` : Message descriptif

2. **En-têtes :** Métadonnées sur la réponse
   - `Content-Type` : Type MIME du contenu
   - `Content-Length` : Taille du corps en octets
   - `Connection` : `close` = fermer après la réponse

3. **Ligne vide :** `\r\n\r\n`

4. **Corps :** Le contenu HTML

---

## Codes de Statut HTTP

| Code | Signification | Usage |
|------|---------------|-------|
| **200** | OK | Succès, tout va bien |
| **301** | Moved Permanently | Redirection permanente |
| **302** | Found | Redirection temporaire |
| **304** | Not Modified | Cache valide |
| **400** | Bad Request | Requête malformée |
| **403** | Forbidden | Accès refusé |
| **404** | Not Found | Ressource introuvable |
| **500** | Internal Server Error | Erreur serveur |
| **503** | Service Unavailable | Serveur surchargé |

---

## Types MIME Courants

Le `Content-Type` indique le type de contenu :

| Extension | Type MIME | Description |
|-----------|-----------|-------------|
| `.html` | `text/html` | Page HTML |
| `.css` | `text/css` | Feuille de style |
| `.js` | `text/javascript` | Script JavaScript |
| `.txt` | `text/plain` | Texte brut |
| `.json` | `application/json` | Données JSON |
| `.png` | `image/png` | Image PNG |
| `.jpg` | `image/jpeg` | Image JPEG |
| `.gif` | `image/gif` | Image GIF |
| `.pdf` | `application/pdf` | Document PDF |

---

## Parser une Requête HTTP

### Étapes du Parsing

1. Lire la ligne de requête
2. Extraire la méthode, le chemin, et la version
3. Lire les en-têtes ligne par ligne
4. Détecter la fin des en-têtes (`\r\n\r\n`)

### Structure de Données

```c
#define MAX_PATH 256
#define MAX_HEADERS 32

typedef struct {
    char method[16];        // GET, POST, etc.
    char path[MAX_PATH];    // /index.html
    char version[16];       // HTTP/1.1

    // En-têtes
    int num_headers;
    struct {
        char name[64];
        char value[256];
    } headers[MAX_HEADERS];
} http_request_t;
```

---

### Fonction de Parsing

```c
#include <string.h>
#include <ctype.h>

int parse_request_line(const char *line, http_request_t *req) {
    // Format: "GET /path HTTP/1.1"
    int n = sscanf(line, "%15s %255s %15s",
                   req->method, req->path, req->version);

    if (n != 3) {
        return -1;  // Requête malformée
    }

    return 0;
}

int parse_header(const char *line, http_request_t *req) {
    if (req->num_headers >= MAX_HEADERS) {
        return -1;  // Trop d'en-têtes
    }

    // Trouver le ':'
    char *colon = strchr(line, ':');
    if (!colon) {
        return -1;  // En-tête malformé
    }

    // Extraire le nom (avant ':')
    size_t name_len = colon - line;
    if (name_len >= 64) name_len = 63;
    strncpy(req->headers[req->num_headers].name, line, name_len);
    req->headers[req->num_headers].name[name_len] = '\0';

    // Extraire la valeur (après ': ')
    const char *value = colon + 1;
    while (*value == ' ' || *value == '\t') value++;  // Ignorer espaces

    strncpy(req->headers[req->num_headers].value, value, 255);
    req->headers[req->num_headers].value[255] = '\0';

    req->num_headers++;
    return 0;
}

int parse_http_request(const char *buffer, http_request_t *req) {
    memset(req, 0, sizeof(http_request_t));

    const char *line_start = buffer;
    const char *line_end;
    int is_first_line = 1;

    while ((line_end = strstr(line_start, "\r\n")) != NULL) {
        size_t line_len = line_end - line_start;

        if (line_len == 0) {
            // Ligne vide = fin des en-têtes
            break;
        }

        // Copier la ligne dans un buffer temporaire
        char line[512];
        if (line_len >= sizeof(line)) line_len = sizeof(line) - 1;
        strncpy(line, line_start, line_len);
        line[line_len] = '\0';

        if (is_first_line) {
            // Parser la ligne de requête
            if (parse_request_line(line, req) < 0) {
                return -1;
            }
            is_first_line = 0;
        } else {
            // Parser un en-tête
            parse_header(line, req);
        }

        // Passer à la ligne suivante
        line_start = line_end + 2;  // +2 pour '\r\n'
    }

    return 0;
}
```

---

## Trouver le Type MIME

```c
const char* get_mime_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";

    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) {
        return "text/html";
    } else if (strcmp(ext, ".css") == 0) {
        return "text/css";
    } else if (strcmp(ext, ".js") == 0) {
        return "text/javascript";
    } else if (strcmp(ext, ".json") == 0) {
        return "application/json";
    } else if (strcmp(ext, ".png") == 0) {
        return "image/png";
    } else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
        return "image/jpeg";
    } else if (strcmp(ext, ".gif") == 0) {
        return "image/gif";
    } else if (strcmp(ext, ".txt") == 0) {
        return "text/plain";
    } else {
        return "application/octet-stream";
    }
}
```

---

## Générer une Réponse HTTP

### Fonction pour Envoyer une Réponse

```c
int send_http_response(int client_fd, int status_code, const char *status_msg,
                       const char *content_type, const char *body, size_t body_len) {
    char header[1024];
    int header_len;

    // Construire l'en-tête HTTP
    header_len = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        status_code, status_msg,
        content_type,
        body_len
    );

    // Envoyer l'en-tête
    if (send(client_fd, header, header_len, 0) < 0) {
        perror("send header");
        return -1;
    }

    // Envoyer le corps (si présent)
    if (body && body_len > 0) {
        if (send(client_fd, body, body_len, 0) < 0) {
            perror("send body");
            return -1;
        }
    }

    return 0;
}
```

---

### Réponses Pré-Définies

```c
void send_404(int client_fd) {
    const char *body =
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head><title>404 Not Found</title></head>\n"
        "<body>\n"
        "<h1>404 Not Found</h1>\n"
        "<p>The requested resource was not found on this server.</p>\n"
        "</body>\n"
        "</html>\n";

    send_http_response(client_fd, 404, "Not Found", "text/html",
                      body, strlen(body));
}

void send_400(int client_fd) {
    const char *body =
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head><title>400 Bad Request</title></head>\n"
        "<body>\n"
        "<h1>400 Bad Request</h1>\n"
        "<p>Your browser sent a request that this server could not understand.</p>\n"
        "</body>\n"
        "</html>\n";

    send_http_response(client_fd, 400, "Bad Request", "text/html",
                      body, strlen(body));
}

void send_500(int client_fd) {
    const char *body =
        "<!DOCTYPE html>\n"
        "<html>\n"
        "<head><title>500 Internal Server Error</title></head>\n"
        "<body>\n"
        "<h1>500 Internal Server Error</h1>\n"
        "<p>The server encountered an internal error.</p>\n"
        "</body>\n"
        "</html>\n";

    send_http_response(client_fd, 500, "Internal Server Error", "text/html",
                      body, strlen(body));
}
```

---

## Servir des Fichiers Statiques

### Fonction pour Lire et Envoyer un Fichier

```c
#include <sys/stat.h>
#include <fcntl.h>

void serve_file(int client_fd, const char *filepath) {
    // Ouvrir le fichier
    int file_fd = open(filepath, O_RDONLY);
    if (file_fd < 0) {
        send_404(client_fd);
        return;
    }

    // Obtenir la taille du fichier
    struct stat st;
    if (fstat(file_fd, &st) < 0) {
        close(file_fd);
        send_500(client_fd);
        return;
    }

    size_t file_size = st.st_size;

    // Allouer un buffer pour le contenu
    char *content = malloc(file_size);
    if (!content) {
        close(file_fd);
        send_500(client_fd);
        return;
    }

    // Lire le fichier
    ssize_t bytes_read = read(file_fd, content, file_size);
    close(file_fd);

    if (bytes_read < 0 || (size_t)bytes_read != file_size) {
        free(content);
        send_500(client_fd);
        return;
    }

    // Déterminer le type MIME
    const char *mime_type = get_mime_type(filepath);

    // Envoyer la réponse
    send_http_response(client_fd, 200, "OK", mime_type, content, file_size);

    free(content);
}
```

---

## Sécurité : Validation du Chemin

### Problème : Directory Traversal

Un attaquant pourrait demander :
```
GET /../../../etc/passwd HTTP/1.1
```

Pour accéder à des fichiers sensibles !

### Solution : Validation et Normalisation

```c
#include <limits.h>
#include <stdlib.h>

int is_safe_path(const char *path, const char *base_dir) {
    char resolved_path[PATH_MAX];
    char resolved_base[PATH_MAX];

    // Résoudre les chemins absolus (supprime .., ., etc.)
    if (realpath(path, resolved_path) == NULL) {
        return 0;  // Chemin invalide
    }

    if (realpath(base_dir, resolved_base) == NULL) {
        return 0;
    }

    // Vérifier que le chemin résolu est dans base_dir
    size_t base_len = strlen(resolved_base);
    if (strncmp(resolved_path, resolved_base, base_len) != 0) {
        return 0;  // Tentative de sortir du répertoire autorisé
    }

    return 1;  // Chemin sûr
}
```

---

## Exemple Complet : Mini-Serveur HTTP

Voici un serveur HTTP complet et fonctionnel.

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 8080
#define BUFFER_SIZE 4096
#define WEB_ROOT "./www"

// Structures
typedef struct {
    char method[16];
    char path[256];
    char version[16];
} http_request_t;

// Ignorer SIGPIPE
void setup_signals() {
    signal(SIGPIPE, SIG_IGN);
}

// Parser la ligne de requête
int parse_request_line(const char *line, http_request_t *req) {
    int n = sscanf(line, "%15s %255s %15s",
                   req->method, req->path, req->version);
    return (n == 3) ? 0 : -1;
}

// Obtenir le type MIME
const char* get_mime_type(const char *path) {
    const char *ext = strrchr(path, '.');
    if (!ext) return "application/octet-stream";

    if (strcmp(ext, ".html") == 0) return "text/html";
    if (strcmp(ext, ".css") == 0) return "text/css";
    if (strcmp(ext, ".js") == 0) return "text/javascript";
    if (strcmp(ext, ".json") == 0) return "application/json";
    if (strcmp(ext, ".png") == 0) return "image/png";
    if (strcmp(ext, ".jpg") == 0) return "image/jpeg";
    if (strcmp(ext, ".gif") == 0) return "image/gif";
    if (strcmp(ext, ".txt") == 0) return "text/plain";

    return "application/octet-stream";
}

// Envoyer une réponse HTTP
void send_response(int fd, int code, const char *msg, const char *type,
                   const char *body, size_t len) {
    char header[1024];
    int hlen = snprintf(header, sizeof(header),
        "HTTP/1.1 %d %s\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        code, msg, type, len
    );

    send(fd, header, hlen, 0);
    if (body && len > 0) {
        send(fd, body, len, 0);
    }
}

// Envoyer une erreur 404
void send_404(int fd) {
    const char *body =
        "<!DOCTYPE html><html><head><title>404</title></head>"
        "<body><h1>404 Not Found</h1></body></html>";
    send_response(fd, 404, "Not Found", "text/html", body, strlen(body));
}

// Envoyer une erreur 400
void send_400(int fd) {
    const char *body =
        "<!DOCTYPE html><html><head><title>400</title></head>"
        "<body><h1>400 Bad Request</h1></body></html>";
    send_response(fd, 400, "Bad Request", "text/html", body, strlen(body));
}

// Servir un fichier (head_only=1 pour HEAD : en-têtes sans corps)
void serve_file(int client_fd, const char *req_path, int head_only) {
    char filepath[512];

    // Construire le chemin complet
    snprintf(filepath, sizeof(filepath), "%s%s", WEB_ROOT, req_path);

    // Si c'est un répertoire, ajouter /index.html
    struct stat st;
    if (stat(filepath, &st) == 0 && S_ISDIR(st.st_mode)) {
        strncat(filepath, "/index.html", sizeof(filepath) - strlen(filepath) - 1);
    }

    printf("Serving: %s\n", filepath);

    // Ouvrir le fichier
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        send_404(client_fd);
        return;
    }

    // Obtenir la taille
    if (fstat(fd, &st) < 0) {
        close(fd);
        send_404(client_fd);
        return;
    }

    size_t size = st.st_size;

    // Lire le contenu
    char *content = malloc(size);
    if (!content) {
        close(fd);
        send_response(client_fd, 500, "Internal Server Error",
                     "text/plain", "Out of memory", 13);
        return;
    }

    if (read(fd, content, size) != (ssize_t)size) {
        close(fd);
        free(content);
        send_response(client_fd, 500, "Internal Server Error",
                     "text/plain", "Read error", 10);
        return;
    }

    close(fd);

    // Envoyer la réponse
    const char *mime = get_mime_type(filepath);
    if (head_only) {
        // HEAD : en-têtes avec Content-Length, mais pas de corps
        send_response(client_fd, 200, "OK", mime, NULL, size);
    } else {
        send_response(client_fd, 200, "OK", mime, content, size);
    }

    free(content);
}

// Gérer un client
void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    // Lire la requête
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        close(client_fd);
        return;
    }

    buffer[bytes] = '\0';

    // Parser la première ligne
    http_request_t req;
    char *line_end = strstr(buffer, "\r\n");
    if (!line_end) {
        send_400(client_fd);
        close(client_fd);
        return;
    }

    *line_end = '\0';
    if (parse_request_line(buffer, &req) < 0) {
        send_400(client_fd);
        close(client_fd);
        return;
    }

    printf("%s %s %s\n", req.method, req.path, req.version);

    // Supporter GET et HEAD
    if (strcmp(req.method, "GET") != 0 && strcmp(req.method, "HEAD") != 0) {
        send_response(client_fd, 405, "Method Not Allowed",
                     "text/plain", "Method not allowed", 18);
        close(client_fd);
        return;
    }

    // HEAD : mêmes en-têtes que GET, mais sans le corps
    int head_only = (strcmp(req.method, "HEAD") == 0);

    // Servir le fichier
    serve_file(client_fd, req.path, head_only);

    close(client_fd);
}

int main() {
    int server_fd;
    struct sockaddr_in addr;

    setup_signals();

    // Créer le répertoire www s'il n'existe pas
    mkdir(WEB_ROOT, 0755);

    // Créer un fichier index.html par défaut
    char index_path[256];
    snprintf(index_path, sizeof(index_path), "%s/index.html", WEB_ROOT);
    if (access(index_path, F_OK) != 0) {
        FILE *f = fopen(index_path, "w");
        if (f) {
            fprintf(f,
                "<!DOCTYPE html>\n"
                "<html>\n"
                "<head>\n"
                "  <title>Mini HTTP Server</title>\n"
                "  <style>\n"
                "    body { font-family: Arial; margin: 50px; }\n"
                "    h1 { color: #333; }\n"
                "  </style>\n"
                "</head>\n"
                "<body>\n"
                "  <h1>Welcome to Mini HTTP Server</h1>\n"
                "  <p>This is a simple HTTP server written in C.</p>\n"
                "  <p>Place your files in the <code>www/</code> directory.</p>\n"
                "</body>\n"
                "</html>\n"
            );
            fclose(f);
            printf("Created default index.html\n");
        }
    }

    // Créer socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // SO_REUSEADDR
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // Bind
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Listen
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Mini HTTP Server running on http://localhost:%d\n", PORT);
    printf("Serving files from: %s\n", WEB_ROOT);
    printf("Press Ctrl+C to stop\n\n");

    // Boucle principale
    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        int client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_fd < 0) {
            perror("accept");
            continue;
        }

        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, sizeof(client_ip));
        printf("[%s:%d] Connection\n", client_ip, ntohs(client_addr.sin_port));

        handle_client(client_fd);
    }

    close(server_fd);
    return 0;
}
```

**Compilation :**
```bash
gcc -o http_server http_server.c -Wall -Wextra
```

**Utilisation :**
```bash
./http_server
```

**Tester :**
Ouvrez votre navigateur et allez sur `http://localhost:8080`

---

## Amélioration : Serveur Concurrent

Pour gérer plusieurs clients simultanément, utilisons `fork()` :

```c
// Gestionnaire SIGCHLD pour éviter les zombies
void sigchld_handler(int sig) {
    (void)sig;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int main() {
    // ... (code d'initialisation) ...

    // Installer le gestionnaire
    struct sigaction sa;
    sa.sa_handler = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    sigaction(SIGCHLD, &sa, NULL);

    // Boucle principale
    while (1) {
        int client_fd = accept(server_fd, ...);
        if (client_fd < 0) continue;

        pid_t pid = fork();

        if (pid < 0) {
            perror("fork");
            close(client_fd);
            continue;
        }

        if (pid == 0) {
            // Processus fils
            close(server_fd);  // Le fils n'en a pas besoin
            handle_client(client_fd);
            exit(0);
        } else {
            // Processus parent
            close(client_fd);  // Le parent n'en a pas besoin
        }
    }
}
```

---

## Amélioration : Logging

### Format de Log Apache

```c
void log_request(const char *client_ip, const char *method,
                 const char *path, int status_code, size_t bytes_sent) {
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);
    char time_str[64];

    strftime(time_str, sizeof(time_str), "%d/%b/%Y:%H:%M:%S %z", tm_info);

    printf("%s - - [%s] \"%s %s HTTP/1.1\" %d %zu\n",
           client_ip, time_str, method, path, status_code, bytes_sent);
}
```

**Exemple de sortie :**
```
127.0.0.1 - - [27/11/2025:14:30:45 +0000] "GET /index.html HTTP/1.1" 200 1234
127.0.0.1 - - [27/11/2025:14:30:46 +0000] "GET /style.css HTTP/1.1" 200 567
127.0.0.1 - - [27/11/2025:14:30:47 +0000] "GET /logo.png HTTP/1.1" 200 8912
```

---

## Amélioration : Keep-Alive

HTTP/1.1 supporte le **keep-alive** : réutiliser la même connexion pour plusieurs requêtes.

```c
void handle_client_keepalive(int client_fd) {
    char buffer[BUFFER_SIZE];

    while (1) {
        // Lire une requête
        ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes <= 0) break;

        buffer[bytes] = '\0';

        // Parser et traiter
        http_request_t req;
        // ... parsing ...

        // Chercher l'en-tête Connection
        int keep_alive = 1;
        // ... vérifier si "Connection: close" ...

        // Servir le fichier
        serve_file(client_fd, req.path);

        if (!keep_alive) {
            break;  // Le client veut fermer
        }

        // Continuer à écouter sur cette connexion
    }

    close(client_fd);
}
```

---

## Amélioration : Compression gzip

Pour les navigateurs qui supportent la compression :

> **Note :** La fonction `compress()` de zlib produit du format **zlib** (RFC 1950), pas du **gzip** (RFC 1952). Pour produire du vrai gzip compatible avec `Content-Encoding: gzip`, il faut utiliser `deflateInit2()` avec `MAX_WBITS + 16`.

```c
#include <zlib.h>

// Compression au format gzip (pas zlib !)
char* gzip_compress(const char *data, size_t len, size_t *compressed_len) {
    uLongf bound = compressBound(len);
    char *compressed = malloc(bound);
    if (!compressed) return NULL;

    z_stream strm = {0};
    // MAX_WBITS + 16 = format gzip
    if (deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
                     MAX_WBITS + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
        free(compressed);
        return NULL;
    }

    strm.next_in = (Bytef*)data;
    strm.avail_in = len;
    strm.next_out = (Bytef*)compressed;
    strm.avail_out = bound;

    if (deflate(&strm, Z_FINISH) != Z_STREAM_END) {
        deflateEnd(&strm);
        free(compressed);
        return NULL;
    }

    *compressed_len = strm.total_out;
    deflateEnd(&strm);
    return compressed;
}

void serve_file_compressed(int client_fd, const char *filepath,
                           int accept_gzip) {
    // ... lire le fichier ...

    if (accept_gzip && size > 1000) {  // Compresser si > 1KB
        size_t compressed_size;
        char *compressed = gzip_compress(content, size, &compressed_size);

        if (compressed) {
            // Envoyer avec Content-Encoding: gzip
            char header[1024];
            snprintf(header, sizeof(header),
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: %s\r\n"
                "Content-Length: %zu\r\n"
                "Content-Encoding: gzip\r\n"
                "\r\n",
                mime_type, compressed_size
            );

            send(client_fd, header, strlen(header), 0);
            send(client_fd, compressed, compressed_size, 0);

            free(compressed);
            free(content);
            return;
        }
    }

    // Envoyer non compressé
    // ...
}
```

---

## Amélioration : Range Requests

Support du téléchargement partiel (utile pour les gros fichiers, vidéos) :

```c
// Parser l'en-tête Range: bytes=0-1023
typedef struct {
    size_t start;
    size_t end;
    int valid;
} range_t;

range_t parse_range(const char *range_header, size_t file_size) {
    range_t range = {0, 0, 0};

    if (strncmp(range_header, "bytes=", 6) != 0) {
        return range;
    }

    const char *range_spec = range_header + 6;

    if (sscanf(range_spec, "%zu-%zu", &range.start, &range.end) == 2) {
        if (range.start <= range.end && range.end < file_size) {
            range.valid = 1;
        }
    } else if (sscanf(range_spec, "%zu-", &range.start) == 1) {
        if (range.start < file_size) {
            range.end = file_size - 1;
            range.valid = 1;
        }
    }

    return range;
}

void serve_file_with_range(int client_fd, const char *filepath, range_t range) {
    // ... ouvrir fichier ...

    if (range.valid) {
        // Réponse 206 Partial Content
        lseek(fd, range.start, SEEK_SET);
        size_t length = range.end - range.start + 1;

        char header[1024];
        snprintf(header, sizeof(header),
            "HTTP/1.1 206 Partial Content\r\n"
            "Content-Type: %s\r\n"
            "Content-Length: %zu\r\n"
            "Content-Range: bytes %zu-%zu/%zu\r\n"
            "\r\n",
            mime_type, length, range.start, range.end, file_size
        );

        send(client_fd, header, strlen(header), 0);

        // Envoyer seulement la partie demandée
        char buffer[8192];
        size_t remaining = length;
        while (remaining > 0) {
            size_t to_read = (remaining < sizeof(buffer)) ? remaining : sizeof(buffer);
            ssize_t n = read(fd, buffer, to_read);
            if (n <= 0) break;

            send(client_fd, buffer, n, 0);
            remaining -= n;
        }
    } else {
        // Réponse 200 normale
        // ...
    }
}
```

---

## Test du Serveur

### Structure de Test

```
projet/
├── http_server.c
└── www/
    ├── index.html
    ├── style.css
    ├── script.js
    └── images/
        └── logo.png
```

### Fichier HTML de Test

**www/index.html :**
```html
<!DOCTYPE html>
<html lang="fr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Mini HTTP Server</title>
    <link rel="stylesheet" href="/style.css">
</head>
<body>
    <header>
        <h1>Bienvenue sur Mini HTTP Server</h1>
    </header>

    <main>
        <section>
            <h2>À propos</h2>
            <p>Ce serveur HTTP est écrit en C et supporte :</p>
            <ul>
                <li>Méthodes GET et HEAD</li>
                <li>Fichiers statiques (HTML, CSS, JS, images)</li>
                <li>Détection automatique des types MIME</li>
                <li>Codes de statut HTTP</li>
            </ul>
        </section>

        <section>
            <h2>Test</h2>
            <button onclick="testServer()">Tester le serveur</button>
            <div id="result"></div>
        </section>
    </main>

    <script src="/script.js"></script>
</body>
</html>
```

**www/style.css :**
```css
body {
    font-family: Arial, sans-serif;
    max-width: 800px;
    margin: 50px auto;
    padding: 20px;
    background-color: #f5f5f5;
}

header {
    background-color: #333;
    color: white;
    padding: 20px;
    border-radius: 5px;
}

main {
    background-color: white;
    padding: 20px;
    margin-top: 20px;
    border-radius: 5px;
    box-shadow: 0 2px 5px rgba(0,0,0,0.1);
}

button {
    background-color: #4CAF50;
    color: white;
    padding: 10px 20px;
    border: none;
    border-radius: 4px;
    cursor: pointer;
}

button:hover {
    background-color: #45a049;
}
```

**www/script.js :**
```javascript
function testServer() {
    const result = document.getElementById('result');
    result.innerHTML = '<p>Test en cours...</p>';

    fetch('/index.html')
        .then(response => {
            if (response.ok) {
                result.innerHTML = '<p style="color: green;">✓ Serveur fonctionne !</p>';
            } else {
                result.innerHTML = '<p style="color: red;">✗ Erreur : ' + response.status + '</p>';
            }
        })
        .catch(error => {
            result.innerHTML = '<p style="color: red;">✗ Erreur : ' + error + '</p>';
        });
}
```

### Tests avec curl

```bash
# Test GET simple
curl http://localhost:8080/

# Test avec en-têtes
curl -i http://localhost:8080/index.html

# Test 404
curl http://localhost:8080/nonexistent.html

# Test HEAD
curl -I http://localhost:8080/

# Test de performance (100 requêtes)
ab -n 100 -c 10 http://localhost:8080/
```

---

## Benchmarking

### Avec Apache Bench

```bash
# Installer Apache Bench
sudo apt install apache2-utils

# Test : 10 000 requêtes, 100 concurrentes
ab -n 10000 -c 100 http://localhost:8080/
```

**Exemple de sortie :**
```
Requests per second:    2500.00 [#/sec] (mean)  
Time per request:       40.000 [ms] (mean)  
Time per request:       0.400 [ms] (mean, across all concurrent requests)  
```

---

### Avec wrk

```bash
# Installer wrk
git clone https://github.com/wg/wrk.git  
cd wrk && make  

# Test : 30 secondes, 12 threads, 400 connexions
./wrk -t12 -c400 -d30s http://localhost:8080/
```

---

## Comparaison avec Nginx

| Métrique | Mini-Serveur | Nginx |
|----------|--------------|-------|
| **Req/sec** | ~2 500 | ~25 000 |
| **Latence (p50)** | ~40 ms | ~4 ms |
| **Mémoire** | ~5 MB | ~10 MB |
| **Concurrence max** | ~100 | ~10 000+ |

**Note :** Notre mini-serveur est 10x plus lent, mais c'est normal ! Nginx est optimisé depuis 20 ans.

---

## En Résumé

Nous avons créé un **serveur HTTP fonctionnel** en C qui :

✅ Parse les requêtes HTTP

✅ Sert des fichiers statiques

✅ Détecte automatiquement les types MIME

✅ Gère les erreurs (404, 400, 500)

✅ Supporte plusieurs clients (avec fork)

✅ Implémente les bases de HTTP/1.1

**Ce que vous avez appris :**
- Le protocole HTTP (requêtes, réponses, codes de statut)
- Parsing de texte en C
- Lecture et envoi de fichiers
- Sécurité (validation des chemins)
- Architecture client/serveur en pratique

**Améliorations possibles :**
- Keep-alive pour réutiliser les connexions
- Compression gzip pour réduire la bande passante
- Range requests pour les gros fichiers
- HTTPS avec OpenSSL
- Cache pour les fichiers fréquents
- Virtual hosts (plusieurs sites sur un serveur)
- CGI pour exécuter des scripts

**Serveurs HTTP réels en C :**
- **Nginx** : Serveur haute performance
- **Apache HTTP Server** : Le plus utilisé historiquement
- **lighttpd** : Léger et rapide
- **H2O** : HTTP/2 natif

Félicitations ! Vous avez créé votre propre serveur web. C'est une excellente base pour comprendre comment fonctionnent les serveurs web professionnels.

---

**Fin du chapitre 20 : Réseau (Sockets)**

⏭️ [Introduction à eBPF](/21-introduction-ebpf/README.md)
