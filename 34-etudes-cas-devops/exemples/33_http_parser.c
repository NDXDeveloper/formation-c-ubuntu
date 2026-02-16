/* ============================================================================
   Section 34.4.2 : HTTP Parsing
   Description : Parsing de requetes HTTP (request line + headers)
   Fichier source : 04.2-http-parsing.md
   ============================================================================ */
#include <stdio.h>
#include <string.h>

#define MAX_METHOD_LEN 16
#define MAX_URI_LEN 2048
#define MAX_VERSION_LEN 16
#define MAX_HEADERS 20
#define MAX_HEADER_NAME 64
#define MAX_HEADER_VALUE 256

typedef struct {
    char method[MAX_METHOD_LEN];
    char uri[MAX_URI_LEN];
    char version[MAX_VERSION_LEN];
} http_request_line_t;

typedef struct {
    char name[MAX_HEADER_NAME];
    char value[MAX_HEADER_VALUE];
} http_header_t;

typedef struct {
    http_request_line_t request_line;
    http_header_t headers[MAX_HEADERS];
    int header_count;
} http_request_t;

int parse_request_line(const char *line, http_request_line_t *req_line) {
    if (line == NULL || line[0] == '\0') return -1;

    int parsed = sscanf(line, "%15s %2047s %15s",
                       req_line->method,
                       req_line->uri,
                       req_line->version);

    if (parsed != 3) return -1;

    if (strcmp(req_line->method, "GET") != 0 &&
        strcmp(req_line->method, "HEAD") != 0 &&
        strcmp(req_line->method, "POST") != 0) {
        return -1;
    }

    if (strcmp(req_line->version, "HTTP/1.0") != 0 &&
        strcmp(req_line->version, "HTTP/1.1") != 0) {
        return -1;
    }

    if (req_line->uri[0] != '/') return -1;

    return 0;
}

int parse_header(const char *line, http_header_t *header) {
    const char *colon = strchr(line, ':');
    if (colon == NULL) return -1;

    size_t name_len = (size_t)(colon - line);
    if (name_len >= MAX_HEADER_NAME) name_len = MAX_HEADER_NAME - 1;

    strncpy(header->name, line, name_len);
    header->name[name_len] = '\0';

    const char *value = colon + 1;
    while (*value == ' ') value++;

    size_t val_len = strlen(value);
    while (val_len > 0 && (value[val_len-1] == '\r' || value[val_len-1] == '\n')) {
        val_len--;
    }
    if (val_len >= MAX_HEADER_VALUE) val_len = MAX_HEADER_VALUE - 1;

    strncpy(header->value, value, val_len);
    header->value[val_len] = '\0';

    return 0;
}

int parse_request(const char *raw, http_request_t *request) {
    request->header_count = 0;

    // Trouver la fin de la request line
    const char *end_of_line = strstr(raw, "\r\n");
    if (end_of_line == NULL) return -1;

    char line_buf[4096];
    size_t line_len = (size_t)(end_of_line - raw);
    if (line_len >= sizeof(line_buf)) return -1;
    strncpy(line_buf, raw, line_len);
    line_buf[line_len] = '\0';

    if (parse_request_line(line_buf, &request->request_line) != 0) {
        return -1;
    }

    // Parser les headers
    const char *p = end_of_line + 2;

    while (*p != '\0' && request->header_count < MAX_HEADERS) {
        if (p[0] == '\r' && p[1] == '\n') break;  // Ligne vide = fin des headers

        end_of_line = strstr(p, "\r\n");
        if (end_of_line == NULL) break;

        line_len = (size_t)(end_of_line - p);
        if (line_len >= sizeof(line_buf)) break;
        strncpy(line_buf, p, line_len);
        line_buf[line_len] = '\0';

        if (parse_header(line_buf, &request->headers[request->header_count]) == 0) {
            request->header_count++;
        }

        p = end_of_line + 2;
    }

    return 0;
}

int main(void) {
    const char *requete =
        "GET /api/users?page=2 HTTP/1.1\r\n"
        "Host: www.example.com\r\n"
        "User-Agent: curl/7.68.0\r\n"
        "Accept: application/json\r\n"
        "Connection: close\r\n"
        "\r\n";

    http_request_t request;

    printf("=== Parsing requete HTTP ===\n\n");

    if (parse_request(requete, &request) == 0) {
        printf("Methode: %s\n", request.request_line.method);
        printf("URI: %s\n", request.request_line.uri);
        printf("Version: %s\n", request.request_line.version);
        printf("\nHeaders (%d):\n", request.header_count);
        for (int i = 0; i < request.header_count; i++) {
            printf("  %s: %s\n", request.headers[i].name, request.headers[i].value);
        }
    } else {
        printf("Erreur de parsing\n");
    }

    // Test requete POST
    printf("\n=== Parsing requete POST ===\n\n");
    const char *requete_post =
        "POST /api/login HTTP/1.1\r\n"
        "Host: api.example.com\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: 27\r\n"
        "\r\n";

    if (parse_request(requete_post, &request) == 0) {
        printf("Methode: %s\n", request.request_line.method);
        printf("URI: %s\n", request.request_line.uri);
        printf("Headers (%d):\n", request.header_count);
        for (int i = 0; i < request.header_count; i++) {
            printf("  %s: %s\n", request.headers[i].name, request.headers[i].value);
        }
    }

    return 0;
}
