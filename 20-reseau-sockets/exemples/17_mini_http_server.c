/* ============================================================================
   Section 20.10 : Mini serveur HTTP
   Description : Serveur HTTP minimal avec parsing de requetes et types MIME
   Fichier source : 10-mini-serveur-http.md
   ============================================================================ */

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

/* Structures */
typedef struct {
    char method[16];
    char path[256];
    char version[16];
} http_request_t;

/* Ignorer SIGPIPE */
void setup_signals(void) {
    signal(SIGPIPE, SIG_IGN);
}

/* Parser la ligne de requete */
int parse_request_line(const char *line, http_request_t *req) {
    int n = sscanf(line, "%15s %255s %15s",
                   req->method, req->path, req->version);
    return (n == 3) ? 0 : -1;
}

/* Obtenir le type MIME */
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

/* Envoyer une reponse HTTP */
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

    send(fd, header, (size_t)hlen, 0);
    if (body && len > 0) {
        send(fd, body, len, 0);
    }
}

/* Envoyer une erreur 404 */
void send_404(int fd) {
    const char *body =
        "<!DOCTYPE html><html><head><title>404</title></head>"
        "<body><h1>404 Not Found</h1></body></html>";
    send_response(fd, 404, "Not Found", "text/html", body, strlen(body));
}

/* Envoyer une erreur 400 */
void send_400(int fd) {
    const char *body =
        "<!DOCTYPE html><html><head><title>400</title></head>"
        "<body><h1>400 Bad Request</h1></body></html>";
    send_response(fd, 400, "Bad Request", "text/html", body, strlen(body));
}

/* Servir un fichier (head_only=1 pour HEAD : en-tetes sans corps) */
void serve_file(int client_fd, const char *req_path, int head_only) {
    char filepath[512];

    /* Construire le chemin complet */
    snprintf(filepath, sizeof(filepath), "%s%s", WEB_ROOT, req_path);

    /* Si c'est un repertoire, ajouter /index.html */
    struct stat st;
    if (stat(filepath, &st) == 0 && S_ISDIR(st.st_mode)) {
        strncat(filepath, "/index.html", sizeof(filepath) - strlen(filepath) - 1);
    }

    printf("Serving: %s\n", filepath);

    /* Ouvrir le fichier */
    int fd = open(filepath, O_RDONLY);
    if (fd < 0) {
        send_404(client_fd);
        return;
    }

    /* Obtenir la taille */
    if (fstat(fd, &st) < 0) {
        close(fd);
        send_404(client_fd);
        return;
    }

    size_t size = (size_t)st.st_size;

    /* Lire le contenu */
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

    /* Envoyer la reponse */
    const char *mime = get_mime_type(filepath);
    if (head_only) {
        /* HEAD : en-tetes avec Content-Length, mais pas de corps */
        send_response(client_fd, 200, "OK", mime, NULL, size);
    } else {
        send_response(client_fd, 200, "OK", mime, content, size);
    }

    free(content);
}

/* Gerer un client */
void handle_client(int client_fd) {
    char buffer[BUFFER_SIZE];

    /* Lire la requete */
    ssize_t bytes = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        close(client_fd);
        return;
    }

    buffer[bytes] = '\0';

    /* Parser la premiere ligne */
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

    /* Supporter GET et HEAD */
    if (strcmp(req.method, "GET") != 0 && strcmp(req.method, "HEAD") != 0) {
        send_response(client_fd, 405, "Method Not Allowed",
                     "text/plain", "Method not allowed", 18);
        close(client_fd);
        return;
    }

    /* HEAD : memes en-tetes que GET, mais sans le corps */
    int head_only = (strcmp(req.method, "HEAD") == 0);

    /* Servir le fichier */
    serve_file(client_fd, req.path, head_only);

    close(client_fd);
}

int main(void) {
    int server_fd;
    struct sockaddr_in addr;

    setup_signals();

    /* Creer le repertoire www s'il n'existe pas */
    mkdir(WEB_ROOT, 0755);

    /* Creer un fichier index.html par defaut */
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

    /* Creer socket */
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /* SO_REUSEADDR */
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    /* Bind */
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    /* Listen */
    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Mini HTTP Server running on http://localhost:%d\n", PORT);
    printf("Serving files from: %s\n", WEB_ROOT);
    printf("Press Ctrl+C to stop\n\n");

    /* Boucle principale */
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
