🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 31.8 Conteneurisation : Docker multi-stage

## Introduction

Après avoir exploré les différentes méthodes de packaging (.deb, .rpm, AppImage), découvrons la **conteneurisation avec Docker**, une approche moderne pour distribuer et déployer des applications.

### Qu'est-ce que Docker ?

**Docker** permet d'empaqueter une application avec **tout son environnement d'exécution** (bibliothèques, dépendances, configuration) dans un **conteneur** léger et portable.

**Analogie :**
- Un **package .deb/rpm** = Un fichier d'installation
- Une **AppImage** = Un fichier exécutable autonome
- Un **conteneur Docker** = Une mini-machine virtuelle avec votre application

### Docker vs Machines Virtuelles

| Aspect | Machine Virtuelle | Conteneur Docker |
|--------|-------------------|------------------|
| **Taille** | Plusieurs Go | Quelques Mo |
| **Démarrage** | Minutes | Secondes |
| **Isolation** | OS complet | Processus isolé |
| **Performance** | Overhead important | Quasi-natif |
| **Partage ressources** | Limité | Efficace (layers) |

### Pourquoi Docker pour les applications C ?

- ✅ **Environnement reproductible** : Même comportement partout
- ✅ **Gestion des dépendances** : Tout est dans l'image
- ✅ **Déploiement simplifié** : `docker run` et ça marche
- ✅ **Isolation** : L'application ne pollue pas le système hôte
- ✅ **Scalabilité** : Déployer sur Kubernetes, Docker Swarm
- ✅ **CI/CD** : Build et tests dans des environnements propres
- ✅ **Multi-architecture** : Cross-compilation facilitée

## Qu'est-ce qu'un build multi-stage ?

### Le problème sans multi-stage

**Dockerfile naïf :**

```dockerfile
FROM ubuntu:22.04

# Installer les outils de build
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    git \
    cmake

# Copier et compiler
COPY . /app  
WORKDIR /app  
RUN make  

CMD ["./mon-programme"]
```

**Problème :** L'image finale contient :
- ❌ GCC (200+ Mo)
- ❌ Make, CMake
- ❌ Headers de développement
- ❌ Fichiers intermédiaires de compilation
- ✅ Votre binaire (2 Mo)

**Résultat : Image de 500 Mo pour un programme de 2 Mo !**

### La solution : Multi-stage builds

**Dockerfile multi-stage :**

```dockerfile
# === STAGE 1 : Build ===
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y gcc make  
COPY . /app  
WORKDIR /app  
RUN make  

# === STAGE 2 : Runtime ===
FROM ubuntu:22.04

# Copier UNIQUEMENT le binaire du stage builder
COPY --from=builder /app/mon-programme /usr/local/bin/

CMD ["mon-programme"]
```

**Résultat : Image de 80 Mo (seulement Ubuntu minimal + binaire) !**

### Concept clé

```
Stage 1 (builder)          Stage 2 (runtime)
┌─────────────────┐       ┌─────────────────┐
│  Ubuntu + GCC   │       │  Ubuntu minimal │
│  + Make + Code  │  ───> │  + Binaire seul │
│  Compilation    │       │                 │
│  500 Mo         │       │  80 Mo          │
└─────────────────┘       └─────────────────┘
     Jeté                     Conservé
```

Seul le **résultat** du build est copié dans l'image finale.

## Premier exemple : Application simple

### Code source

**main.c**

```c
#include <stdio.h>

int main(void) {
    printf("Hello from Docker!\n");
    printf("Application compilée dans un conteneur multi-stage\n");
    return 0;
}
```

**Makefile**

```makefile
CC = gcc  
CFLAGS = -Wall -O2  

all: app

app: main.c
	$(CC) $(CFLAGS) main.c -o app

clean:
	rm -f app
```

### Dockerfile multi-stage

**Dockerfile**

```dockerfile
# === Stage 1: Build ===
FROM ubuntu:22.04 AS builder

# Installer les outils de compilation
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    && rm -rf /var/lib/apt/lists/*

# Copier les sources
COPY . /build  
WORKDIR /build  

# Compiler
RUN make

# === Stage 2: Runtime ===
FROM ubuntu:22.04

# Copier uniquement le binaire
COPY --from=builder /build/app /usr/local/bin/app

# Commande par défaut
CMD ["app"]
```

### Build et exécution

```bash
# Builder l'image
docker build -t mon-app:latest .

# Vérifier la taille
docker images mon-app

# Résultat typique :
# REPOSITORY   TAG      SIZE
# mon-app      latest   77.8MB

# Exécuter
docker run --rm mon-app:latest

# Résultat :
# Hello from Docker!
# Application compilée dans un conteneur multi-stage
```

## Optimisation avancée : Image minimale

### Utiliser Alpine Linux

Alpine est une distribution Linux ultra-légère (~5 Mo au lieu de ~77 Mo pour Ubuntu).

**Dockerfile avec Alpine**

```dockerfile
# === Stage 1: Build ===
FROM alpine:3.18 AS builder

# Installer GCC et outils
RUN apk add --no-cache gcc musl-dev make

# Copier et compiler
COPY . /build  
WORKDIR /build  
RUN make  

# === Stage 2: Runtime ===
FROM alpine:3.18

# Copier le binaire
COPY --from=builder /build/app /usr/local/bin/app

CMD ["app"]
```

**Taille résultante : ~7-10 Mo !**

### Utiliser scratch (image vide)

Pour un binaire **statique**, on peut utiliser `scratch` (image vide = 0 Mo).

**Dockerfile avec scratch**

```dockerfile
# === Stage 1: Build ===
FROM alpine:3.18 AS builder

RUN apk add --no-cache gcc musl-dev make

COPY . /build  
WORKDIR /build  

# Compiler statiquement
RUN gcc -static -O2 main.c -o app

# === Stage 2: Runtime ===
FROM scratch

# Copier le binaire statique
COPY --from=builder /build/app /app

CMD ["/app"]
```

**Taille résultante : ~2 Mo (uniquement le binaire) !**

**Important :** Le binaire doit être **statique** (toutes les bibliothèques incluses).

## Projet complet avec dépendances

### Application avec bibliothèques externes

**main.c (utilise libcurl)**

```c
#include <stdio.h>
#include <curl/curl.h>

int main(void) {
    CURL *curl;

    curl = curl_easy_init();
    if (curl) {
        printf("libcurl version: %s\n", curl_version());
        curl_easy_cleanup(curl);
        printf("Application avec dépendances externes!\n");
        return 0;
    } else {
        fprintf(stderr, "Erreur d'initialisation curl\n");
        return 1;
    }
}
```

**Makefile**

```makefile
CC = gcc  
CFLAGS = -Wall -O2  
LDFLAGS = -lcurl  

all: app

app: main.c
	$(CC) $(CFLAGS) main.c -o app $(LDFLAGS)

clean:
	rm -f app
```

### Dockerfile multi-stage avec dépendances

**Dockerfile**

```dockerfile
# === Stage 1: Build ===
FROM ubuntu:22.04 AS builder

# Installer les dépendances de build
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    libcurl4-openssl-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . /build  
WORKDIR /build  

RUN make

# === Stage 2: Runtime ===
FROM ubuntu:22.04

# Installer uniquement les bibliothèques runtime (pas les headers -dev)
RUN apt-get update && apt-get install -y \
    libcurl4 \
    && rm -rf /var/lib/apt/lists/*

# Copier le binaire
COPY --from=builder /build/app /usr/local/bin/app

CMD ["app"]
```

**Points clés :**
- Stage 1 : `libcurl4-openssl-dev` (headers pour compiler)
- Stage 2 : `libcurl4` (bibliothèque runtime uniquement)
- Taille optimisée : ~100 Mo au lieu de 300+ Mo

## Multi-stage avec CMake

### Projet CMake

**CMakeLists.txt**

```cmake
cmake_minimum_required(VERSION 3.10)  
project(MonApp C)  

set(CMAKE_C_STANDARD 17)

# Trouver libcurl
find_package(CURL REQUIRED)

add_executable(app main.c)

target_link_libraries(app CURL::libcurl)
```

### Dockerfile pour CMake

**Dockerfile**

```dockerfile
# === Stage 1: Build ===
FROM ubuntu:22.04 AS builder

# Installer CMake et dépendances
RUN apt-get update && apt-get install -y \
    cmake \
    gcc \
    g++ \
    make \
    libcurl4-openssl-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . /build  
WORKDIR /build  

# Build avec CMake
RUN mkdir build && cd build && \
    cmake -DCMAKE_BUILD_TYPE=Release .. && \
    cmake --build . -j$(nproc)

# === Stage 2: Runtime ===
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    libcurl4 \
    && rm -rf /var/lib/apt/lists/*

# Copier le binaire depuis le répertoire de build CMake
COPY --from=builder /build/build/app /usr/local/bin/app

CMD ["app"]
```

## Cross-compilation dans Docker

### Cross-compiler pour ARM dans Docker

**Dockerfile multi-stage avec cross-compilation**

```dockerfile
# === Stage 1: Build pour ARM64 ===
FROM ubuntu:22.04 AS builder

# Installer la toolchain ARM64
RUN apt-get update && apt-get install -y \
    gcc-aarch64-linux-gnu \
    make \
    && rm -rf /var/lib/apt/lists/*

COPY . /build  
WORKDIR /build  

# Cross-compiler pour ARM64
RUN aarch64-linux-gnu-gcc -o app main.c

# === Stage 2: Runtime ARM64 ===
FROM arm64v8/ubuntu:22.04

COPY --from=builder /build/app /usr/local/bin/app

CMD ["app"]
```

**Build et utilisation :**

```bash
# Builder pour ARM64 (peut nécessiter QEMU)
docker buildx build --platform linux/arm64 -t mon-app:arm64 .

# Ou builder sur x86_64 et copier le binaire
docker build -t mon-app-builder .  
docker create --name temp mon-app-builder  
docker cp temp:/usr/local/bin/app ./app-arm64  
docker rm temp  
```

### Buildx pour multi-architecture

**Docker Buildx** permet de créer des images pour plusieurs architectures :

```bash
# Créer un builder multi-plateforme
docker buildx create --name multiarch --use

# Builder pour plusieurs architectures
docker buildx build \
    --platform linux/amd64,linux/arm64,linux/arm/v7 \
    -t username/mon-app:latest \
    --push \
    .
```

**Dockerfile compatible multi-arch**

```dockerfile
# === Stage 1: Build ===
FROM --platform=$BUILDPLATFORM ubuntu:22.04 AS builder

# Arguments automatiques de buildx
ARG TARGETARCH  
ARG BUILDPLATFORM  

RUN echo "Building on $BUILDPLATFORM for $TARGETARCH"

# Installer le bon compilateur selon l'architecture cible
RUN apt-get update && \
    if [ "$TARGETARCH" = "arm64" ]; then \
        apt-get install -y gcc-aarch64-linux-gnu; \
    elif [ "$TARGETARCH" = "arm" ]; then \
        apt-get install -y gcc-arm-linux-gnueabihf; \
    else \
        apt-get install -y gcc; \
    fi && \
    rm -rf /var/lib/apt/lists/*

COPY . /build  
WORKDIR /build  

# Compiler selon l'architecture
RUN if [ "$TARGETARCH" = "arm64" ]; then \
        aarch64-linux-gnu-gcc -o app main.c; \
    elif [ "$TARGETARCH" = "arm" ]; then \
        arm-linux-gnueabihf-gcc -o app main.c; \
    else \
        gcc -o app main.c; \
    fi

# === Stage 2: Runtime ===
FROM ubuntu:22.04

COPY --from=builder /build/app /usr/local/bin/app

CMD ["app"]
```

## Conteneur avec plusieurs binaires

### Projet avec plusieurs outils

**Structure :**

```
projet/
├── src/
│   ├── server.c
│   ├── client.c
│   └── utils.c
├── Makefile
└── Dockerfile
```

**Dockerfile**

```dockerfile
# === Stage 1: Build ===
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y gcc make && \
    rm -rf /var/lib/apt/lists/*

COPY . /build  
WORKDIR /build  

# Compiler tous les binaires
RUN make all

# === Stage 2: Runtime ===
FROM ubuntu:22.04

# Copier tous les binaires
COPY --from=builder /build/server /usr/local/bin/  
COPY --from=builder /build/client /usr/local/bin/  
COPY --from=builder /build/utils /usr/local/bin/  

# Point d'entrée flexible
ENTRYPOINT ["/bin/bash", "-c"]  
CMD ["server"]  
```

**Utilisation :**

```bash
# Lancer le serveur (par défaut)
docker run -p 8080:8080 mon-app

# Lancer le client
docker run mon-app "client"

# Lancer utils
docker run mon-app "utils --help"
```

## Variables d'environnement et configuration

### Dockerfile avec configuration

```dockerfile
# === Stage 1: Build ===
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y gcc && \
    rm -rf /var/lib/apt/lists/*

COPY . /build  
WORKDIR /build  
RUN gcc -o app main.c  

# === Stage 2: Runtime ===
FROM ubuntu:22.04

COPY --from=builder /build/app /usr/local/bin/app

# Variables d'environnement par défaut
ENV APP_MODE=production  
ENV APP_PORT=8080  
ENV APP_LOG_LEVEL=info  

# Créer les répertoires nécessaires
RUN mkdir -p /var/log/app /etc/app

# Exposer le port
EXPOSE 8080

# Utilisateur non-root (sécurité)
RUN useradd -m -u 1000 appuser  
USER appuser  

CMD ["app"]
```

**Utilisation :**

```bash
# Utiliser les valeurs par défaut
docker run -p 8080:8080 mon-app

# Override des variables
docker run -e APP_MODE=debug -e APP_PORT=9000 -p 9000:9000 mon-app
```

## Volumes et persistance

### Application avec données persistantes

**Dockerfile**

```dockerfile
# === Build stage ===
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y gcc && \
    rm -rf /var/lib/apt/lists/*

COPY . /build  
WORKDIR /build  
RUN gcc -o app main.c  

# === Runtime stage ===
FROM ubuntu:22.04

COPY --from=builder /build/app /usr/local/bin/app

# Créer les répertoires pour les volumes
RUN mkdir -p /data /config

# Déclarer les volumes
VOLUME ["/data", "/config"]

# Fichier de config par défaut
COPY config.default.conf /config/app.conf

CMD ["app"]
```

**Utilisation avec volumes :**

```bash
# Créer des volumes Docker
docker volume create app-data  
docker volume create app-config  

# Lancer avec volumes
docker run -d \
    -v app-data:/data \
    -v app-config:/config \
    mon-app

# Ou avec bind mount (lier à un répertoire local)
docker run -d \
    -v $(pwd)/data:/data \
    -v $(pwd)/config:/config \
    mon-app
```

## Health checks et monitoring

### Dockerfile avec health check

```dockerfile
# === Build stage ===
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y gcc && \
    rm -rf /var/lib/apt/lists/*

COPY . /build  
WORKDIR /build  
RUN gcc -o server server.c  

# === Runtime stage ===
FROM ubuntu:22.04

# Installer curl pour les health checks
RUN apt-get update && apt-get install -y curl && \
    rm -rf /var/lib/apt/lists/*

COPY --from=builder /build/server /usr/local/bin/server

EXPOSE 8080

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/health || exit 1

CMD ["server"]
```

**Vérifier l'état :**

```bash
# Lancer le conteneur
docker run -d --name mon-serveur mon-app

# Vérifier le health check
docker ps
# STATUS affiche "healthy" ou "unhealthy"

# Voir les logs du health check
docker inspect --format='{{json .State.Health}}' mon-serveur
```

## Docker Compose pour applications multi-conteneurs

### Application serveur + client

**docker-compose.yml**

```yaml
version: '3.8'

services:
  server:
    build:
      context: .
      dockerfile: Dockerfile.server
    ports:
      - "8080:8080"
    environment:
      - SERVER_MODE=production
    volumes:
      - server-data:/data
    networks:
      - app-network
    healthcheck:
      test: ["CMD", "curl", "-f", "http://localhost:8080/health"]
      interval: 30s
      timeout: 3s
      retries: 3

  client:
    build:
      context: .
      dockerfile: Dockerfile.client
    depends_on:
      - server
    environment:
      - SERVER_URL=http://server:8080
    networks:
      - app-network

volumes:
  server-data:

networks:
  app-network:
    driver: bridge
```

**Dockerfile.server**

```dockerfile
# === Build ===
FROM ubuntu:22.04 AS builder  
RUN apt-get update && apt-get install -y gcc && rm -rf /var/lib/apt/lists/*  
COPY server.c /build/  
WORKDIR /build  
RUN gcc -o server server.c  

# === Runtime ===
FROM ubuntu:22.04  
RUN apt-get update && apt-get install -y curl && rm -rf /var/lib/apt/lists/*  
COPY --from=builder /build/server /usr/local/bin/  
EXPOSE 8080  
CMD ["server"]  
```

**Dockerfile.client**

```dockerfile
# === Build ===
FROM ubuntu:22.04 AS builder  
RUN apt-get update && apt-get install -y gcc libcurl4-openssl-dev && \  
    rm -rf /var/lib/apt/lists/*
COPY client.c /build/  
WORKDIR /build  
RUN gcc -o client client.c -lcurl  

# === Runtime ===
FROM ubuntu:22.04  
RUN apt-get update && apt-get install -y libcurl4 && rm -rf /var/lib/apt/lists/*  
COPY --from=builder /build/client /usr/local/bin/  
CMD ["client"]  
```

**Utilisation :**

```bash
# Lancer tous les services
docker-compose up -d

# Voir les logs
docker-compose logs -f

# Arrêter
docker-compose down

# Rebuild et relancer
docker-compose up -d --build
```

## Intégration CI/CD

### GitHub Actions avec Docker multi-stage

**.github/workflows/docker.yml**

```yaml
name: Build Docker Image

on:
  push:
    branches: [ main ]
    tags: [ 'v*' ]
  pull_request:
    branches: [ main ]

env:
  REGISTRY: ghcr.io
  IMAGE_NAME: ${{ github.repository }}

jobs:
  build-and-push:
    runs-on: ubuntu-latest
    permissions:
      contents: read
      packages: write

    steps:
    - name: Checkout
      uses: actions/checkout@v4

    - name: Set up Docker Buildx
      uses: docker/setup-buildx-action@v3

    - name: Log in to Container Registry
      if: github.event_name != 'pull_request'
      uses: docker/login-action@v3
      with:
        registry: ${{ env.REGISTRY }}
        username: ${{ github.actor }}
        password: ${{ secrets.GITHUB_TOKEN }}

    - name: Extract metadata
      id: meta
      uses: docker/metadata-action@v5
      with:
        images: ${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}
        tags: |
          type=ref,event=branch
          type=ref,event=pr
          type=semver,pattern={{version}}
          type=semver,pattern={{major}}.{{minor}}
          type=sha

    - name: Build and push
      uses: docker/build-push-action@v6
      with:
        context: .
        platforms: linux/amd64,linux/arm64
        push: ${{ github.event_name != 'pull_request' }}
        tags: ${{ steps.meta.outputs.tags }}
        labels: ${{ steps.meta.outputs.labels }}
        cache-from: type=gha
        cache-to: type=gha,mode=max

    - name: Test image
      if: github.event_name != 'pull_request'
      run: |
        docker pull ${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}:${{ github.sha }}
        docker run --rm ${{ env.REGISTRY }}/${{ env.IMAGE_NAME }}:${{ github.sha }} --version
```

### GitLab CI avec Docker

**.gitlab-ci.yml**

```yaml
variables:
  DOCKER_DRIVER: overlay2
  IMAGE_TAG: $CI_REGISTRY_IMAGE:$CI_COMMIT_REF_SLUG

stages:
  - build
  - test
  - push

build:
  stage: build
  image: docker:latest
  services:
    - docker:dind
  script:
    - docker build -t $IMAGE_TAG .
    - docker save $IMAGE_TAG > image.tar
  artifacts:
    paths:
      - image.tar
    expire_in: 1 hour

test:
  stage: test
  image: docker:latest
  services:
    - docker:dind
  dependencies:
    - build
  script:
    - docker load < image.tar
    - docker run --rm $IMAGE_TAG --version
    - docker run --rm $IMAGE_TAG --help

push:
  stage: push
  image: docker:latest
  services:
    - docker:dind
  dependencies:
    - build
  script:
    - docker load < image.tar
    - docker login -u $CI_REGISTRY_USER -p $CI_REGISTRY_PASSWORD $CI_REGISTRY
    - docker push $IMAGE_TAG
  only:
    - main
    - tags
```

## Bonnes pratiques

### 1. Utiliser des images de base officielles

```dockerfile
# ✓ Bon - image officielle
FROM ubuntu:22.04

# ✓ Bon - image officielle Alpine
FROM alpine:3.18

# ✗ Mauvais - image non officielle
FROM random-user/ubuntu-custom
```

### 2. Minimiser le nombre de layers

```dockerfile
# ✗ Mauvais - 3 layers
RUN apt-get update  
RUN apt-get install -y gcc  
RUN apt-get install -y make  

# ✓ Bon - 1 layer
RUN apt-get update && apt-get install -y \
    gcc \
    make \
    && rm -rf /var/lib/apt/lists/*
```

### 3. Nettoyer les caches

```dockerfile
# Ubuntu/Debian
RUN apt-get update && apt-get install -y package \
    && rm -rf /var/lib/apt/lists/*

# Alpine
RUN apk add --no-cache package

# Fichiers temporaires
RUN make && rm -rf /tmp/*
```

### 4. Utiliser .dockerignore

**.dockerignore**

```
# Fichiers de build
*.o
*.a
build/
*.so

# Git
.git/
.gitignore

# Documentation
*.md
docs/

# CI/CD
.github/
.gitlab-ci.yml

# IDE
.vscode/
.idea/

# Tests
tests/
*.test
```

### 5. Ne pas copier inutilement

```dockerfile
# ✗ Mauvais - copie tout
COPY . /build

# ✓ Bon - copie seulement ce qui est nécessaire
COPY src/ /build/src/  
COPY Makefile /build/  
COPY CMakeLists.txt /build/  
```

### 6. Exécuter en tant qu'utilisateur non-root

```dockerfile
# Créer un utilisateur
RUN useradd -m -u 1000 appuser

# Changer les permissions
RUN chown -R appuser:appuser /app

# Changer d'utilisateur
USER appuser

CMD ["app"]
```

### 7. Scanner les vulnérabilités

```bash
# Installer Trivy
sudo apt-get install trivy

# Scanner une image
trivy image mon-app:latest

# Scanner avec sévérité haute/critique uniquement
trivy image --severity HIGH,CRITICAL mon-app:latest
```

### 8. Utiliser des tags spécifiques

```dockerfile
# ✗ Mauvais - tag 'latest' implicite
FROM ubuntu

# ✓ Bon - version spécifique
FROM ubuntu:22.04

# ✓ Encore mieux - digest SHA256
FROM ubuntu:22.04@sha256:abc123...
```

### 9. Ordre optimal des instructions

```dockerfile
# Les instructions qui changent rarement en premier
FROM ubuntu:22.04

# Installation des dépendances (change rarement)
RUN apt-get update && apt-get install -y gcc

# Copie des dépendances de build (change parfois)
COPY Makefile /build/

# Copie du code source (change souvent)
COPY src/ /build/src/

# Build (invalidé à chaque changement de code)
RUN make
```

### 10. Labels pour métadonnées

```dockerfile
FROM ubuntu:22.04

LABEL org.opencontainers.image.title="Mon Application"  
LABEL org.opencontainers.image.description="Application C de démonstration"  
LABEL org.opencontainers.image.version="1.0.0"  
LABEL org.opencontainers.image.authors="Votre Nom <email@example.com>"  
LABEL org.opencontainers.image.source="https://github.com/user/projet"  

# ... reste du Dockerfile
```

## Optimisation de la taille

### Comparaison des tailles

```dockerfile
# Test 1 : Ubuntu avec build complet
FROM ubuntu:22.04  
RUN apt-get update && apt-get install -y gcc make git cmake  
COPY . /app  
WORKDIR /app  
RUN make  
CMD ["./app"]  
# Taille : ~400 MB

# Test 2 : Ubuntu multi-stage
FROM ubuntu:22.04 AS builder  
RUN apt-get update && apt-get install -y gcc make  
COPY . /app  
WORKDIR /app  
RUN make  

FROM ubuntu:22.04  
COPY --from=builder /app/app /usr/local/bin/  
CMD ["app"]  
# Taille : ~77 MB

# Test 3 : Alpine multi-stage
FROM alpine:3.18 AS builder  
RUN apk add --no-cache gcc musl-dev make  
COPY . /app  
WORKDIR /app  
RUN make  

FROM alpine:3.18  
COPY --from=builder /app/app /usr/local/bin/  
CMD ["app"]  
# Taille : ~8 MB

# Test 4 : Scratch avec binaire statique
FROM alpine:3.18 AS builder  
RUN apk add --no-cache gcc musl-dev make  
COPY . /app  
WORKDIR /app  
RUN gcc -static -o app main.c  

FROM scratch  
COPY --from=builder /app/app /app  
CMD ["/app"]  
# Taille : ~2 MB
```

### Techniques d'optimisation

```dockerfile
# 1. Strip les binaires
RUN gcc -o app main.c && strip app

# 2. Compiler avec optimisations
RUN gcc -O3 -DNDEBUG -o app main.c

# 3. Utiliser upx (compression)
FROM alpine AS builder  
RUN apk add --no-cache gcc musl-dev upx  
RUN gcc -o app main.c  
RUN upx --best --lzma app  

# 4. Multi-stage avec plusieurs builders
FROM alpine AS deps-builder  
RUN # ... build des dépendances  

FROM alpine AS app-builder  
COPY --from=deps-builder /libs /libs  
RUN # ... build de l'app  

FROM alpine  
COPY --from=deps-builder /libs /usr/lib  
COPY --from=app-builder /app /usr/local/bin  
```

## Dépannage

### Erreur : "layer does not exist"

**Problème :** Cache Docker corrompu

**Solution :**
```bash
# Rebuild sans cache
docker build --no-cache -t mon-app .

# Nettoyer le cache
docker builder prune -a
```

### Erreur : "not found" dans COPY --from

**Problème :** Stage builder mal nommé ou fichier inexistant

**Solution :**
```dockerfile
# Vérifier le nom du stage
FROM ubuntu:22.04 AS builder
# ...
COPY --from=builder /correct/path /dest
```

### Conteneur qui se ferme immédiatement

**Problème :** Commande qui se termine immédiatement

**Solution :**
```bash
# Voir les logs
docker logs nom-conteneur

# Lancer avec shell interactif
docker run -it mon-app /bin/bash

# Ou override la commande
docker run -it mon-app tail -f /dev/null
```

### Image trop grosse

**Solutions :**
```bash
# Analyser les layers
docker history mon-app:latest

# Utiliser dive pour exploration
docker run --rm -it \
    -v /var/run/docker.sock:/var/run/docker.sock \
    wagoodman/dive:latest mon-app:latest
```

## Résumé des commandes

```bash
# Build
docker build -t mon-app:latest .  
docker build --no-cache -t mon-app .  

# Run
docker run --rm mon-app  
docker run -d --name mon-conteneur mon-app  
docker run -p 8080:8080 mon-app  

# Gestion
docker ps                    # Conteneurs actifs  
docker ps -a                # Tous les conteneurs  
docker images               # Lister les images  
docker logs conteneur       # Voir les logs  
docker exec -it conteneur bash  # Shell dans conteneur  

# Nettoyage
docker stop conteneur  
docker rm conteneur  
docker rmi image  
docker system prune -a      # Nettoyage complet  

# Registry
docker login  
docker push username/image:tag  
docker pull username/image:tag  

# Multi-architecture
docker buildx create --name multiarch --use  
docker buildx build --platform linux/amd64,linux/arm64 -t image .  
```

## Conclusion

Les **builds multi-stage Docker** sont essentiels pour créer des images de conteneurs **optimisées** et **sécurisées** pour les applications C.

**Points clés :**

1. **Séparation build/runtime** : Ne garder que le nécessaire
2. **Taille optimisée** : De 500 Mo à quelques Mo
3. **Sécurité** : Pas d'outils de développement en production
4. **Portabilité** : Fonctionne partout où Docker tourne
5. **Reproductibilité** : Même environnement partout

**Recommandations :**

- ✅ **Ubuntu** : Pour compatibilité maximale (~80 Mo)
- ✅ **Alpine** : Pour taille réduite (~10 Mo)
- ✅ **Scratch** : Pour taille minimale (~2 Mo) avec binaires statiques

**Workflow recommandé :**

```
1. Développer localement
2. Créer Dockerfile multi-stage
3. Tester avec docker build
4. Optimiser la taille
5. Scanner les vulnérabilités
6. Pousser sur registry
7. Déployer (Kubernetes, Docker Swarm, etc.)
8. Automatiser avec CI/CD
```

Docker multi-stage transforme la distribution d'applications C en rendant le déploiement aussi simple qu'un `docker run`, tout en maintenant des images légères et sécurisées.

---


⏭️ [Distribution de bibliothèques partagées](/31-cross-compilation-deploiement/09-distribution-bibliotheques.md)
