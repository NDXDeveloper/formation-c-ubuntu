# Chapitre 20 - Réseau et Sockets : Compilation des exemples

Compilation standard :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o <programme> <source.c>
```

---

## 01_tcp_echo_server.c
- **Section** : 20.3 - Sockets TCP
- **Description** : Serveur echo TCP itératif - renvoie en echo tout ce qu'il reçoit
- **Fichier source** : 03-sockets-tcp.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 01_tcp_echo_server 01_tcp_echo_server.c`
- **Sortie attendue** : Affiche "Serveur en ecoute sur le port 8080..." puis echo des messages reçus. Serveur interactif (Ctrl+C pour arrêter). Tester avec `telnet localhost 8080` ou `02_tcp_echo_client`.

## 02_tcp_echo_client.c
- **Section** : 20.3 - Sockets TCP
- **Description** : Client echo TCP - se connecte au serveur et envoie des messages
- **Fichier source** : 03-sockets-tcp.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 02_tcp_echo_client 02_tcp_echo_client.c`
- **Sortie attendue** : Se connecte à 127.0.0.1:8080, invite à saisir des messages, affiche l'echo du serveur. Nécessite `01_tcp_echo_server` en cours d'exécution. Taper "quit" pour quitter.

## 03_udp_echo_server.c
- **Section** : 20.4 - Sockets UDP
- **Description** : Serveur echo UDP - renvoie en echo les datagrammes reçus
- **Fichier source** : 04-sockets-udp.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 03_udp_echo_server 03_udp_echo_server.c`
- **Sortie attendue** : Affiche "Serveur UDP en ecoute sur le port 8080..." puis affiche IP/port/taille/contenu de chaque datagramme reçu. Serveur interactif. Tester avec `04_udp_echo_client`.

## 04_udp_echo_client.c
- **Section** : 20.4 - Sockets UDP
- **Description** : Client echo UDP - envoie des messages au serveur UDP
- **Fichier source** : 04-sockets-udp.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 04_udp_echo_client 04_udp_echo_client.c`
- **Sortie attendue** : Invite à saisir des messages, envoie en UDP, affiche l'echo. Nécessite `03_udp_echo_server`. Taper "quit" pour quitter.

## 05_udp_client_retry.c
- **Section** : 20.4 - Sockets UDP
- **Description** : Client UDP avec retry et timeout en cas de non-réponse
- **Fichier source** : 04-sockets-udp.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 05_udp_client_retry 05_udp_client_retry.c`
- **Sortie attendue** : Tente 3 envois avec timeout de 2s. Sans serveur : "Tentative 1/3... Timeout : pas de reponse", etc. puis "Echec apres 3 tentatives". Avec serveur : affiche la réponse dès la première tentative.

## 06_tcp_client_robuste.c
- **Section** : 20.5 - Gestion des erreurs réseau
- **Description** : Client TCP robuste avec send_all, retry et gestion SIGPIPE
- **Fichier source** : 05-gestion-erreurs-reseau.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 06_tcp_client_robuste 06_tcp_client_robuste.c`
- **Sortie attendue** : Tentatives de connexion avec retry (backoff exponentiel). Sans serveur : 3 tentatives espacées de 2/4s, puis échec. Avec serveur : connexion réussie, boucle d'interaction avec gestion robuste des erreurs.

## 07_serveur_optimise.c
- **Section** : 20.6 - Options des sockets
- **Description** : Serveur TCP optimisé avec SO_REUSEADDR/PORT, keepalive, buffers
- **Fichier source** : 06-options-sockets.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 07_serveur_optimise 07_serveur_optimise.c`
- **Sortie attendue** : "Serveur optimise en ecoute sur le port 8080". Configure SO_REUSEADDR, SO_REUSEPORT, SO_KEEPALIVE, TCP_KEEPIDLE/INTVL/CNT, SO_RCVBUF/SNDBUF, TCP_NODELAY. Echo des messages clients.

## 08_socket_options.c
- **Section** : 20.6 - Options des sockets
- **Description** : Affichage diagnostic des options d'un socket TCP
- **Fichier source** : 06-options-sockets.md (code construit depuis extrait)
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 08_socket_options 08_socket_options.c`
- **Sortie attendue** :
```
Options par defaut :
=== Options du socket 3 ===
SO_REUSEADDR: OFF  
SO_KEEPALIVE: OFF  
SO_RCVBUF: 131072 octets  
SO_SNDBUF: 16384 octets  
TCP_NODELAY: OFF  
TCP_KEEPIDLE: 7200 secondes  
TCP_KEEPINTVL: 75 secondes  
TCP_KEEPCNT: 9  
===========================

Apres modifications :
=== Options du socket 3 ===
SO_REUSEADDR: ON  
SO_KEEPALIVE: ON  
...
TCP_NODELAY: ON
...
===========================
```

## 09_client_dns.c
- **Section** : 20.7 - Résolution de noms
- **Description** : Client DNS avec getaddrinfo - connexion HTTP à www.example.com
- **Fichier source** : 07-resolution-noms.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 09_client_dns 09_client_dns.c`
- **Sortie attendue** : Résout www.example.com, affiche les adresses IP tentées, se connecte, envoie GET HTTP/1.0 et affiche la réponse (200 OK + HTML). Nécessite une connexion internet.

## 10_serveur_getaddrinfo.c
- **Section** : 20.7 - Résolution de noms
- **Description** : Serveur TCP avec getaddrinfo et AI_PASSIVE (IPv4/IPv6)
- **Fichier source** : 07-resolution-noms.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 10_serveur_getaddrinfo 10_serveur_getaddrinfo.c`
- **Sortie attendue** : "Serveur en ecoute sur le port 8080". Fonctionne en IPv4 et IPv6 automatiquement. Echo des messages clients.

## 11_http_client.c
- **Section** : 20.7 - Résolution de noms
- **Description** : Client HTTP simple avec résolution DNS et arguments CLI
- **Fichier source** : 07-resolution-noms.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 11_http_client 11_http_client.c`
- **Sortie attendue** : `./11_http_client www.example.com /` - Résout le DNS, se connecte, envoie GET HTTP/1.1, affiche la réponse complète. Nécessite une connexion internet.

## 12_serveur_fork.c
- **Section** : 20.8 - Serveur concurrent
- **Description** : Serveur multi-processus avec fork et gestion SIGCHLD
- **Fichier source** : 08-serveur-concurrent.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 12_serveur_fork 12_serveur_fork.c`
- **Sortie attendue** : "Serveur multi-processus en ecoute sur le port 8080". Chaque client est géré par un processus fils. Affiche PID pour chaque opération. Gestion des zombies via SIGCHLD.

## 13_serveur_pthread.c
- **Section** : 20.8 - Serveur concurrent
- **Description** : Serveur multi-threads avec pthreads et threads détachés
- **Fichier source** : 08-serveur-concurrent.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -pthread -o 13_serveur_pthread 13_serveur_pthread.c`
- **Note** : Nécessite `-pthread`
- **Sortie attendue** : "Serveur multi-threads en ecoute sur le port 8080". Chaque client est géré par un thread détaché. Affiche le thread ID pour chaque opération.

## 14_serveur_select.c
- **Section** : 20.8 - Serveur concurrent
- **Description** : Serveur avec multiplexage I/O via select() et fd_set
- **Fichier source** : 08-serveur-concurrent.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 14_serveur_select 14_serveur_select.c`
- **Sortie attendue** : "Serveur select() en ecoute sur le port 8080" + "Clients max : 30". Gère jusqu'à 30 clients simultanés dans un seul thread.

## 15_serveur_epoll.c
- **Section** : 20.8 - Serveur concurrent
- **Description** : Serveur avec epoll (Level-Triggered) haute performance
- **Fichier source** : 08-serveur-concurrent.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 15_serveur_epoll 15_serveur_epoll.c`
- **Sortie attendue** : "Serveur epoll en ecoute sur le port 8080". Haute performance avec epoll_create1/epoll_ctl/epoll_wait. Affiche socket ID pour chaque événement.

## 16_serveur_epoll_et.c
- **Section** : 20.9 - Non-blocking I/O et epoll
- **Description** : Serveur echo epoll Edge-Triggered avec machine à états
- **Fichier source** : 09-non-blocking-io-epoll.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 16_serveur_epoll_et 16_serveur_epoll_et.c`
- **Sortie attendue** : "Serveur epoll ET sur port 8080". Mode Edge-Triggered avec sockets non-bloquants, machine à états (READING/WRITING/CLOSING) par connexion.

## 17_mini_http_server.c
- **Section** : 20.10 - Mini serveur HTTP
- **Description** : Serveur HTTP minimal avec parsing de requêtes et types MIME
- **Fichier source** : 10-mini-serveur-http.md
- **Compilation** : `gcc -Wall -Wextra -Werror -pedantic -std=c17 -o 17_mini_http_server 17_mini_http_server.c`
- **Sortie attendue** : Crée `./www/index.html` si inexistant, puis "Mini HTTP Server running on http://localhost:8080". Supporte GET et HEAD, détection MIME, pages d'erreur 400/404/405. Tester avec `curl http://localhost:8080/`.

---

## Paires serveur/client

| Serveur | Client | Protocole |
|---------|--------|-----------|
| 01_tcp_echo_server | 02_tcp_echo_client | TCP |
| 03_udp_echo_server | 04_udp_echo_client | UDP |
| 03_udp_echo_server | 05_udp_client_retry | UDP (avec retry) |
| 01_tcp_echo_server | 06_tcp_client_robuste | TCP (robuste) |

## Notes
- Tous les serveurs utilisent le port 8080 par défaut
- Les serveurs sont interactifs (Ctrl+C pour arrêter)
- 09_client_dns et 11_http_client nécessitent une connexion internet
- 08_socket_options est le seul programme non-interactif (s'exécute et affiche les résultats)
- 17_mini_http_server crée un répertoire `www/` dans le répertoire courant
