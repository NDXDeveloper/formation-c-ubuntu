/* ============================================================================
   Section 34.3.2 : Export Prometheus
   Description : Serveur HTTP exposant les metriques au format Prometheus
   Fichier source : 03.2-export-prometheus.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "metrics.h"

#define SERVER_PORT 8080
#define BUFFER_SIZE 4096

static volatile int keep_running = 1;

void signal_handler(int sig) {
    (void)sig;
    keep_running = 0;
}

int create_server_socket(int port) {
    int server_fd;
    struct sockaddr_in address;
    int opt = 1;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        return -1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        return -1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons((uint16_t)port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        return -1;
    }

    if (listen(server_fd, 10) < 0) {
        perror("listen");
        close(server_fd);
        return -1;
    }

    printf("Serveur HTTP demarre sur le port %d\n", port);
    printf("Metriques disponibles sur: http://localhost:%d/metrics\n", port);
    return server_fd;
}

void handle_metrics_request(int client_fd) {
    char metrics[8192];

    generate_metrics(metrics, sizeof(metrics));

    size_t metrics_len = strlen(metrics);

    char response_header[512];
    snprintf(response_header, sizeof(response_header),
             "HTTP/1.1 200 OK\r\n"
             "Content-Type: text/plain; version=0.0.4; charset=utf-8\r\n"
             "Content-Length: %zu\r\n"
             "\r\n",
             metrics_len);

    write(client_fd, response_header, strlen(response_header));
    write(client_fd, metrics, metrics_len);
}

void handle_not_found(int client_fd) {
    const char *response =
        "HTTP/1.1 404 Not Found\r\n"
        "Content-Type: text/plain\r\n"
        "Content-Length: 13\r\n"
        "\r\n"
        "404 Not Found";

    write(client_fd, response, strlen(response));
}

void run_server(int server_fd) {
    struct sockaddr_in client_address;
    socklen_t client_len = sizeof(client_address);
    char buffer[BUFFER_SIZE];

    while (keep_running) {
        int client_fd = accept(server_fd,
                               (struct sockaddr *)&client_address,
                               &client_len);

        if (client_fd < 0) {
            if (keep_running) {
                perror("accept");
            }
            continue;
        }

        ssize_t bytes_read = read(client_fd, buffer, BUFFER_SIZE - 1);
        if (bytes_read > 0) {
            buffer[bytes_read] = '\0';

            if (strncmp(buffer, "GET /metrics", 12) == 0) {
                handle_metrics_request(client_fd);
            } else {
                handle_not_found(client_fd);
            }
        }

        close(client_fd);
    }
}

int main(void) {
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    int server_fd = create_server_socket(SERVER_PORT);
    if (server_fd < 0) {
        return EXIT_FAILURE;
    }

    run_server(server_fd);

    close(server_fd);
    printf("\nServeur arrete proprement\n");
    return EXIT_SUCCESS;
}
