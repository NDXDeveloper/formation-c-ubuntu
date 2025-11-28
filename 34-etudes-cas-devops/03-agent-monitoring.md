🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 34.3 Agent de Monitoring Système

## Introduction

Dans ce chapitre, nous allons construire un **agent de monitoring système** complet en C. Ce projet d'étude de cas DevOps combine plusieurs concepts que nous avons vus dans cette formation : programmation système, gestion de la mémoire, sockets réseau, et intégration dans un pipeline moderne.

Un agent de monitoring est un programme qui observe en permanence l'état d'un système (CPU, mémoire, disque, réseau) et rend ces informations disponibles à des outils de supervision comme Prometheus, Grafana, ou Datadog.

---

## Qu'est-ce qu'un Agent de Monitoring ?

### Définition

Un **agent de monitoring** (ou "exporter" dans le vocabulaire Prometheus) est un programme qui :

1. **Collecte** des métriques système à intervalles réguliers
2. **Expose** ces métriques dans un format standardisé
3. **Fonctionne en continu** comme un service système (daemon)
4. **Consomme peu de ressources** pour ne pas perturber le système surveillé

### Exemples d'agents existants

| Agent              | Langage | Description                           |
|--------------------|---------|---------------------------------------|
| **node_exporter**  | Go      | Agent Prometheus pour métriques Linux |
| **telegraf**       | Go      | Agent InfluxDB, multi-plateformes     |
| **collectd**       | C       | Daemon de collecte modulaire          |
| **netdata**        | C       | Monitoring temps réel avec interface  |
| **osquery**        | C++     | SQL pour interroger l'OS              |

### Pourquoi créer notre propre agent en C ?

**Avantages du C pour un agent de monitoring :**

- ✅ **Performance** : Très faible empreinte mémoire (quelques Mo vs centaines de Mo pour Go/Python)
- ✅ **Pas de runtime** : Pas de JVM, pas d'interpréteur, pas de garbage collector
- ✅ **Accès système direct** : Lecture native de `/proc`, `/sys`, appels système
- ✅ **Portabilité** : Binaire statique facilement déployable
- ✅ **Apprentissage** : Comprendre Linux en profondeur
- ✅ **DevOps moderne** : Intégrable dans un écosystème Cloud Native (Kubernetes, Prometheus)

**Cas d'usage typiques :**
- Systèmes embarqués avec ressources limitées
- Conteneurs Docker ultra-légers
- Environnements à haute densité (milliers de conteneurs)
- Monitoring de systèmes critiques nécessitant une fiabilité maximale

---

## Architecture Globale de notre Agent

Notre agent de monitoring suivra cette architecture :

```
┌─────────────────────────────────────────────────────────────┐
│                    AGENT DE MONITORING                      │
│                                                             │
│  ┌─────────────────┐                                        │
│  │  Daemonisation  │  ◄── systemd ou daemonisation manuelle │
│  └────────┬────────┘                                        │
│           │                                                 │
│           ▼                                                 │
│  ┌─────────────────┐      ┌──────────────────┐              │
│  │  Boucle         │──────┤  Collecte de     │              │
│  │  Principale     │      │  Métriques       │              │
│  │  (main loop)    │      │                  │              │
│  └────────┬────────┘      │  - CPU           │              │
│           │               │  - Mémoire       │              │
│           │               │  - Disque        │              │
│           │               │  - Réseau        │              │
│           │               │  - Processus     │              │
│           │               └──────────────────┘              │
│           │                                                 │
│           ▼                                                 │
│  ┌─────────────────┐                                        │
│  │  Serveur HTTP   │ ◄── Port 8080 (configurable)           │
│  │                 │                                        │
│  │  GET /metrics   │──────► Exposition format Prometheus    │
│  │  GET /health    │──────► Health check                    │
│  └─────────────────┘                                        │
│                                                             │
└─────────────────────────────────────────────────────────────┘
                         │
                         │ HTTP GET (scrape)
                         ▼
              ┌──────────────────┐
              │   Prometheus     │
              │     Server       │
              └────────┬─────────┘
                       │
                       ▼
              ┌──────────────────┐
              │     Grafana      │
              │  (Visualisation) │
              └──────────────────┘
```

### Composants principaux

Notre agent sera structuré en **trois modules principaux** :

#### 1. **Module de Collecte** (Section 34.3.1)
- Lecture des fichiers `/proc/stat`, `/proc/meminfo`, `/proc/diskstats`, `/proc/net/dev`
- Calcul des métriques dérivées (pourcentages, taux)
- Gestion de l'historique pour les calculs différentiels
- API de collecte unifiée

**Fichiers :** `metrics.c`, `metrics.h`

#### 2. **Module d'Export Prometheus** (Section 34.3.2)
- Serveur HTTP minimal en C (sockets POSIX)
- Génération du format Prometheus (texte simple)
- Endpoint `/metrics` exposant les données
- Gestion des requêtes concurrentes

**Fichiers :** `http_server.c`, `http_server.h`

#### 3. **Module de Daemonisation** (Section 34.3.3)
- Transformation en processus système
- Gestion des signaux (SIGTERM, SIGHUP)
- Fichier PID pour éviter les instances multiples
- Intégration systemd (service Linux)

**Fichiers :** `daemon.c`, `daemon.h`, `monitoring-agent.service`

---

## Métriques à Collecter

Notre agent collectera les métriques système suivantes :

### Métriques CPU

| Métrique                     | Type    | Description                          |
|------------------------------|---------|--------------------------------------|
| `node_cpu_usage_percent`     | Gauge   | Utilisation CPU globale (%)          |
| `node_cpu_user_percent`      | Gauge   | Temps CPU en mode utilisateur        |
| `node_cpu_system_percent`    | Gauge   | Temps CPU en mode kernel             |
| `node_cpu_idle_percent`      | Gauge   | Temps CPU inactif                    |
| `node_cpu_iowait_percent`    | Gauge   | Temps d'attente I/O                  |

**Source :** `/proc/stat`

### Métriques Mémoire

| Métrique                          | Type    | Description                     |
|-----------------------------------|---------|---------------------------------|
| `node_memory_total_bytes`         | Gauge   | Mémoire RAM totale              |
| `node_memory_available_bytes`     | Gauge   | Mémoire disponible              |
| `node_memory_used_bytes`          | Gauge   | Mémoire utilisée                |
| `node_memory_cached_bytes`        | Gauge   | Mémoire en cache                |
| `node_swap_total_bytes`           | Gauge   | Swap total                      |
| `node_swap_used_bytes`            | Gauge   | Swap utilisé                    |

**Source :** `/proc/meminfo`, appel système `sysinfo()`

### Métriques Disque

| Métrique                              | Type    | Description                     |
|---------------------------------------|---------|---------------------------------|
| `node_disk_read_bytes_total`          | Counter | Octets lus (cumulatif)          |
| `node_disk_written_bytes_total`       | Counter | Octets écrits (cumulatif)       |
| `node_disk_reads_completed_total`     | Counter | Nombre de lectures              |
| `node_disk_writes_completed_total`    | Counter | Nombre d'écritures              |
| `node_disk_io_time_seconds_total`     | Counter | Temps I/O cumulé                |

**Source :** `/proc/diskstats`
**Label :** `device` (ex: sda, nvme0n1)

### Métriques Réseau

| Métrique                                  | Type    | Description                     |
|-------------------------------------------|---------|---------------------------------|
| `node_network_receive_bytes_total`        | Counter | Octets reçus (cumulatif)        |
| `node_network_transmit_bytes_total`       | Counter | Octets transmis (cumulatif)     |
| `node_network_receive_packets_total`      | Counter | Paquets reçus                   |
| `node_network_transmit_packets_total`     | Counter | Paquets transmis                |
| `node_network_receive_errors_total`       | Counter | Erreurs de réception            |
| `node_network_transmit_errors_total`      | Counter | Erreurs de transmission         |

**Source :** `/proc/net/dev`
**Label :** `interface` (ex: eth0, wlan0)

### Métriques Système

| Métrique                     | Type    | Description                          |
|------------------------------|---------|--------------------------------------|
| `node_load1`                 | Gauge   | Load average 1 minute                |
| `node_load5`                 | Gauge   | Load average 5 minutes               |
| `node_load15`                | Gauge   | Load average 15 minutes              |
| `node_uptime_seconds`        | Counter | Temps depuis le démarrage            |
| `node_procs_running`         | Gauge   | Nombre de processus actifs           |
| `node_procs_blocked`         | Gauge   | Nombre de processus bloqués          |

**Source :** `/proc/loadavg`, `/proc/uptime`, appel système `sysinfo()`

---

## Technologies et Standards Utilisés

### Standards de Monitoring

#### Prometheus
**Prometheus** est le système de monitoring le plus utilisé dans les environnements Cloud Native (Kubernetes).

**Caractéristiques :**
- **Architecture Pull** : Prometheus interroge les agents (vs Push)
- **Modèle de données** : Time series avec labels multidimensionnels
- **PromQL** : Langage de requête puissant
- **Alerting** : Intégration avec AlertManager
- **Écosystème riche** : Grafana, exporters, service discovery

**Format d'exposition :**
```
# HELP node_cpu_usage_percent CPU usage percentage
# TYPE node_cpu_usage_percent gauge
node_cpu_usage_percent 23.45

# HELP node_memory_available_bytes Available memory in bytes
# TYPE node_memory_available_bytes gauge
node_memory_available_bytes 8589934592
```

Le format est **textuel**, **lisible**, et **simple à générer** en C.

#### OpenMetrics
**OpenMetrics** est l'évolution standardisée du format Prometheus (CNCF).

Notre agent sera compatible avec les deux formats.

### Protocoles Réseau

#### HTTP/1.1
Notre serveur HTTP implémentera un subset minimal de HTTP/1.1 :
- Méthode `GET` uniquement
- Routes : `/metrics`, `/health`
- Headers essentiels : `Content-Type`, `Content-Length`
- Connexions courtes (pas de keep-alive pour simplifier)

#### Sockets TCP
Utilisation de l'API POSIX pour les sockets :
- `socket()`, `bind()`, `listen()`, `accept()`
- Communication synchrone bloquante (suffisant pour un exporter)

### Intégration Système Linux

#### Pseudo-systèmes de fichiers

**`/proc`** - Process Information Pseudo-filesystem
- Interface vers le noyau Linux
- Fichiers virtuels générés dynamiquement
- Pas d'écriture disque réelle
- Lecture très rapide (en mémoire)

**`/sys`** - Sysfs
- Exposition du modèle d'objets du kernel
- Accès aux informations matérielles
- Complémentaire à `/proc`

#### systemd
**systemd** est le gestionnaire de services standard sur Ubuntu moderne.

**Avantages pour notre agent :**
- Démarrage automatique au boot
- Redémarrage en cas de crash
- Gestion des dépendances (réseau, etc.)
- Logging centralisé (journald)
- Isolation de sécurité

---

## Flux de Données

### Collecte → Exposition → Supervision

Voici le parcours complet d'une métrique :

```
1. SYSTÈME LINUX
   └─> /proc/stat contient : "cpu 12345 678 9012 3456 ..."

2. COLLECTE (metrics.c)
   ├─> Ouvrir /proc/stat
   ├─> Lire la ligne CPU
   ├─> Parser les valeurs (sscanf)
   ├─> Calculer le pourcentage d'utilisation
   └─> Stocker dans une structure : cpu_usage = 23.45%

3. EXPOSITION (http_server.c)
   ├─> Client Prometheus envoie : GET /metrics HTTP/1.1
   ├─> Serveur génère le format Prometheus :
   │   "# HELP node_cpu_usage_percent CPU usage\n"
   │   "# TYPE node_cpu_usage_percent gauge\n"
   │   "node_cpu_usage_percent 23.45\n"
   └─> Réponse HTTP avec Content-Type: text/plain

4. PROMETHEUS
   ├─> Scrape toutes les 15 secondes
   ├─> Stocke dans sa TSDB (Time Series Database)
   └─> Données disponibles pour requêtes PromQL

5. GRAFANA
   ├─> Requête : rate(node_cpu_usage_percent[5m])
   ├─> Affichage graphique
   └─> Alertes si CPU > 80%
```

### Cycle de vie de l'agent

```
┌──────────────────────────────────────────────────────────┐
│ DÉMARRAGE                                                │
├──────────────────────────────────────────────────────────┤
│ 1. Lecture configuration (/etc/monitoring-agent.conf)    │
│ 2. Daemonisation (fork, setsid, chdir)                   │
│ 3. Création fichier PID (/var/run/monitoring-agent.pid)  │
│ 4. Configuration signaux (SIGTERM, SIGHUP)               │
│ 5. Initialisation logging (syslog/journald)              │
│ 6. Création socket serveur HTTP (bind port 8080)         │
│ 7. Log : "Agent démarré sur :8080"                       │
└──────────────────────────────────────────────────────────┘
                         │
                         ▼
┌──────────────────────────────────────────────────────────┐
│ BOUCLE PRINCIPALE (tant que keep_running == 1)           │
├──────────────────────────────────────────────────────────┤
│ while (keep_running) {                                   │
│     // Attendre connexion HTTP (timeout 1 seconde)       │
│     client_fd = accept(server_fd, ...);                  │
│                                                          │
│     if (client_fd > 0) {                                 │
│         // Lire requête HTTP                             │
│         read(client_fd, buffer, ...);                    │
│                                                          │
│         if (request == "GET /metrics") {                 │
│             // Collecter métriques                       │
│             collect_all_metrics(&metrics);               │
│                                                          │
│             // Générer format Prometheus                 │
│             generate_prometheus_output(&metrics, buffer);│
│                                                          │
│             // Envoyer réponse HTTP                      │
│             send_http_response(client_fd, buffer);       │
│         }                                                │
│                                                          │
│         close(client_fd);                                │
│     }                                                    │
│                                                          │
│     // Vérifier si rechargement config demandé (SIGHUP)  │
│     if (reload_config_flag) {                            │
│         reload_configuration();                          │
│     }                                                    │
│ }                                                        │
└──────────────────────────────────────────────────────────┘
                         │
                         ▼
┌──────────────────────────────────────────────────────────┐
│ ARRÊT (Signal SIGTERM reçu)                              │
├──────────────────────────────────────────────────────────┤
│ 1. keep_running = 0 (sortie de la boucle)                │
│ 2. Fermeture socket serveur (close)                      │
│ 3. Suppression fichier PID                               │
│ 4. Log : "Agent arrêté proprement"                       │
│ 5. Fermeture syslog                                      │
│ 6. Exit avec code 0                                      │
└──────────────────────────────────────────────────────────┘
```

---

## Structure du Projet

### Organisation des fichiers

```
monitoring-agent/
├── src/
│   ├── main.c              # Point d'entrée, gestion arguments
│   ├── daemon.c            # Daemonisation, signaux, PID
│   ├── daemon.h
│   ├── metrics.c           # Collecte de métriques système
│   ├── metrics.h
│   ├── http_server.c       # Serveur HTTP, format Prometheus
│   ├── http_server.h
│   └── config.c            # Parsing configuration (optionnel)
│   └── config.h
├── include/
│   └── common.h            # Définitions communes
├── config/
│   └── monitoring-agent.conf   # Fichier de configuration
├── systemd/
│   └── monitoring-agent.service  # Unit file systemd
├── scripts/
│   ├── install.sh          # Script d'installation
│   └── uninstall.sh
├── tests/
│   ├── test_metrics.c      # Tests unitaires
│   └── test_http.c
├── docs/
│   ├── README.md
│   └── ARCHITECTURE.md
├── Makefile                # Compilation
├── CMakeLists.txt          # Alternative avec CMake
└── .gitignore
```

### Dépendances

Notre agent aura **zéro dépendances externes** :
- Uniquement la libc standard (glibc sur Ubuntu)
- API POSIX pour les sockets
- Pas de bibliothèques tierces

**Avantages :**
- Compilation triviale sur n'importe quel Linux
- Binaire statique possible (`gcc -static`)
- Empreinte disque minimale (~100 Ko)

---

## Prérequis Techniques

### Connaissances nécessaires

Pour suivre ce chapitre, vous devez maîtriser :

✅ **Fondamentaux C** (Modules 1-4)
- Pointeurs et allocation dynamique
- Structures de données
- Compilation multi-fichiers

✅ **Programmation système** (Module 6)
- Descripteurs de fichiers
- Processus et signaux
- Sockets TCP/IP

✅ **Outils DevOps** (Module 5)
- Compilation avec Make/CMake
- Debugging avec GDB/Valgrind
- Tests et CI/CD

### Environnement de développement

**Système d'exploitation :**
- Ubuntu 20.04 LTS ou supérieur
- Debian 11+ également compatible
- Adaptable à d'autres distributions Linux

**Outils requis :**
```bash
# Compiler et debugger
sudo apt install build-essential gdb valgrind

# Outils système
sudo apt install net-tools procps

# Test et monitoring
sudo apt install curl prometheus grafana
```

**Éditeur recommandé :**
- VS Code avec extensions C/C++
- Vim/Neovim avec plugins
- CLion (JetBrains)

---

## Objectifs Pédagogiques

À la fin de ce chapitre, vous saurez :

### Compétences techniques

- 🎯 **Lire et parser les pseudo-fichiers Linux** (`/proc`, `/sys`)
- 🎯 **Créer un serveur HTTP minimal** en C avec sockets POSIX
- 🎯 **Générer le format d'exposition Prometheus** (texte)
- 🎯 **Transformer un programme en daemon** système
- 🎯 **Gérer les signaux** pour arrêt/rechargement propre
- 🎯 **Intégrer avec systemd** (service Linux moderne)
- 🎯 **Éviter les fuites mémoire** dans un programme long-running
- 🎯 **Logger correctement** avec syslog/journald

### Compétences DevOps

- 🎯 **Monitorer une infrastructure** avec Prometheus + Grafana
- 🎯 **Créer des dashboards** de visualisation
- 🎯 **Configurer des alertes** basées sur des métriques
- 🎯 **Packager et déployer** un service système
- 🎯 **Comprendre l'observabilité** moderne (métriques, logs, traces)

---

## Approche de Développement

### Méthodologie itérative

Nous construirons l'agent **par étapes incrémentales** :

**Itération 1 : Version minimale (MVP)**
```c
// Programme qui lit /proc/stat et affiche le CPU usage
int main() {
    cpu_stats_t stats;
    read_cpu_stats(&stats);
    printf("CPU Usage: %.2f%%\n", stats.usage);
}
```

**Itération 2 : Ajout du serveur HTTP**
```c
// Programme qui expose les métriques sur :8080/metrics
int main() {
    int server = create_server(8080);
    while (1) {
        handle_request(server);
    }
}
```

**Itération 3 : Daemonisation**
```c
// Programme qui tourne en arrière-plan
int main(int argc, char *argv[]) {
    if (daemon_mode) {
        daemonize();
    }
    run_server();
}
```

**Itération 4 : Intégration systemd**
```ini
# Fichier monitoring-agent.service
[Service]
ExecStart=/usr/local/bin/monitoring-agent
Restart=always
```

### Tests continus

À chaque étape, nous validerons :

- ✅ **Compilation sans warnings** (`-Wall -Wextra -Werror`)
- ✅ **Pas de fuites mémoire** (Valgrind)
- ✅ **Fonctionnement correct** (tests manuels avec curl)
- ✅ **Format Prometheus valide** (validation avec promtool)

---

## Cas d'Usage Réels

### Scénario 1 : Monitoring d'un serveur web

**Contexte :** Vous gérez 50 serveurs web Nginx.

**Solution avec notre agent :**
1. Déployer l'agent sur chaque serveur (systemd)
2. Configurer Prometheus pour scraper tous les agents
3. Créer un dashboard Grafana "Vue d'ensemble serveurs web"
4. Alerter si CPU > 80% ou mémoire < 10%

**Requêtes Prometheus utiles :**
```promql
# CPU moyen sur tous les serveurs
avg(node_cpu_usage_percent)

# Top 5 serveurs par utilisation mémoire
topk(5, node_memory_used_bytes / node_memory_total_bytes * 100)

# Débit réseau total
sum(rate(node_network_transmit_bytes_total[5m]))
```

### Scénario 2 : Cluster Kubernetes

**Contexte :** Cluster Kubernetes avec 100 pods répartis sur 10 nœuds.

**Solution :**
- DaemonSet Kubernetes pour déployer l'agent sur chaque nœud
- Service Discovery Prometheus automatique
- Métriques par nœud avec labels `node=xxx`
- Corrélation avec métriques applicatives (kube-state-metrics)

**Exemple de DaemonSet :**
```yaml
apiVersion: apps/v1
kind: DaemonSet
metadata:
  name: monitoring-agent
spec:
  selector:
    matchLabels:
      app: monitoring-agent
  template:
    spec:
      containers:
      - name: agent
        image: mycompany/monitoring-agent:1.0
        ports:
        - containerPort: 8080
          name: metrics
```

### Scénario 3 : Systèmes embarqués

**Contexte :** Dispositifs IoT avec ARM CPU et 512 Mo RAM.

**Solution :**
- Cross-compiler l'agent pour ARM
- Binaire statique ultra-léger (<200 Ko)
- Exposition locale (pas de Prometheus central)
- Collecte périodique par un gateway

**Commande de cross-compilation :**
```bash
arm-linux-gnueabihf-gcc -static -Os \
    -o monitoring-agent-arm \
    main.c metrics.c http_server.c daemon.c
```

---

## Évolutions Possibles

Une fois le MVP terminé, l'agent peut être étendu :

### Fonctionnalités avancées

🔧 **Métriques supplémentaires**
- Température CPU/GPU
- Statistiques par processus (top 10 CPU/RAM)
- Métriques Docker/containerd
- État des services systemd

🔧 **Support multi-plateforme**
- Adaptation pour BSD (FreeBSD, OpenBSD)
- Portage macOS (via sysctls)
- Windows (via WMI)

🔧 **Configuration dynamique**
- Rechargement sans redémarrage (SIGHUP)
- API de configuration (endpoints REST)
- Activation/désactivation de métriques

🔧 **Performance**
- Threading pour collecte parallèle
- Cache des métriques avec TTL
- Batch processing pour I/O

🔧 **Sécurité**
- Authentification (tokens)
- TLS/HTTPS avec OpenSSL
- Filtrage IP (whitelist)

🔧 **Observabilité**
- Métriques internes de l'agent lui-même
- Traces OpenTelemetry
- Profiling intégré

### Intégrations

🔗 **Backends alternatifs**
- InfluxDB (format line protocol)
- Graphite (format carbon)
- StatsD (UDP)
- OpenTelemetry

🔗 **Service Discovery**
- Consul
- Etcd
- Kubernetes API

🔗 **Notification**
- Webhook pour alertes
- Email/Slack intégration
- PagerDuty

---

## Plan des Sous-Sections

Les trois prochaines sections détaillent l'implémentation :

### **34.3.1 Collecte de Métriques**
- Sources de données Linux (`/proc`, `/sys`)
- Lecture et parsing de fichiers système
- Calculs de métriques dérivées (CPU %, taux réseau)
- Structures de données pour stocker les métriques
- Gestion de l'historique (delta entre échantillons)

**Livrables :** `metrics.c`, `metrics.h`

### **34.3.2 Export Prometheus**
- Format d'exposition Prometheus (HELP, TYPE, labels)
- Serveur HTTP minimal en C (sockets TCP)
- Génération du payload texte
- Gestion des requêtes GET /metrics
- Validation avec promtool

**Livrables :** `http_server.c`, `http_server.h`

### **34.3.3 Daemonisation**
- Processus de daemonisation (fork, setsid)
- Gestion des signaux (SIGTERM, SIGHUP)
- Fichier PID (éviter instances multiples)
- Logging avec syslog
- Intégration systemd (fichier .service)

**Livrables :** `daemon.c`, `daemon.h`, `monitoring-agent.service`

---

## Ressources Complémentaires

### Documentation Linux

- **man proc** : Documentation complète de `/proc`
- **man sysinfo** : Appel système sysinfo()
- **man daemon** : Guide de création de daemons
- **man systemd.service** : Format des unit files

### Standards et Spécifications

- **Prometheus Exposition Format** : https://prometheus.io/docs/instrumenting/exposition_formats/
- **OpenMetrics** : https://openmetrics.io/
- **Linux Filesystem Hierarchy** : https://www.pathname.com/fhs/

### Projets Open Source de Référence

- **node_exporter** (Go) : https://github.com/prometheus/node_exporter
- **collectd** (C) : https://github.com/collectd/collectd
- **netdata** (C) : https://github.com/netdata/netdata

### Livres

- Stevens & Rago, "Advanced Programming in the UNIX Environment"
- Kerrisk, "The Linux Programming Interface"
- Gregg, "Systems Performance: Enterprise and the Cloud"

---

## Conclusion de l'Introduction

Nous allons maintenant construire cet agent **pas à pas**, en commençant par la collecte des métriques système. Chaque section est autonome mais s'appuie sur la précédente pour construire un système complet.

**Prêt ?** Passons à la section **34.3.1 : Collecte de Métriques** pour commencer à interroger le système Linux !

---

## Vue d'Ensemble du Code Final

Pour vous donner une idée de la complexité, voici la taille approximative du projet final :

```
monitoring-agent/
├── main.c              (~150 lignes)   # Point d'entrée, CLI
├── daemon.c            (~200 lignes)   # Daemonisation
├── metrics.c           (~400 lignes)   # Collecte métriques
├── http_server.c       (~300 lignes)   # Serveur HTTP
├── config.c            (~100 lignes)   # Configuration (optionnel)
└── Total: ~1150 lignes de code C
```

**Temps de développement estimé :** 2-3 jours pour un développeur C intermédiaire.
**Empreinte binaire :** ~150 Ko (statique), ~50 Ko (dynamique)
**Consommation RAM :** ~5-10 Mo en fonctionnement
**Charge CPU :** <1% en moyenne sur un système moderne

C'est un projet **réaliste et déployable en production** tout en restant pédagogique et compréhensible.

---

**Prochaine section :** 34.3.1 Collecte de Métriques

⏭️ [Collecte de métriques](/34-etudes-cas-devops/03.1-collecte-metriques.md)
