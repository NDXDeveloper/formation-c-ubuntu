🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 31.7 AppImage

## Introduction

Après avoir exploré les packages **.deb** et **.rpm** spécifiques à certaines distributions, découvrons **AppImage** : un format de packaging **universel** qui fonctionne sur pratiquement toutes les distributions Linux.

### Qu'est-ce qu'AppImage ?

**AppImage** est un format de distribution d'applications Linux qui encapsule une application et toutes ses dépendances dans **un seul fichier exécutable**.

**Concept clé :**
```
Une application = Un fichier
```

Pas d'installation, pas de privilèges root nécessaires, juste :
```bash
chmod +x MonApplication.AppImage
./MonApplication.AppImage
```

### La philosophie "One app = One file"

**Comparaison avec d'autres formats :**

| Format | Fichiers | Installation | Root requis | Universel |
|--------|----------|--------------|-------------|-----------|
| **.deb** | Package + dépendances | `apt install` | ✅ Oui | ❌ Debian/Ubuntu |
| **.rpm** | Package + dépendances | `dnf install` | ✅ Oui | ❌ Red Hat/Fedora |
| **AppImage** | 1 fichier autonome | Aucune | ❌ Non | ✅ Toutes les distros |

### Avantages et inconvénients

**Avantages :**

- ✅ **Portable** : Fonctionne sur Ubuntu, Fedora, Arch, openSUSE, etc.
- ✅ **Autonome** : Toutes les dépendances incluses
- ✅ **Pas d'installation** : Télécharger et exécuter
- ✅ **Pas de pollution système** : N'installe rien dans /usr ou /lib
- ✅ **Plusieurs versions** : Garder différentes versions côte à côte
- ✅ **Pas besoin de root** : Utilisateur standard
- ✅ **Intégration optionnelle** : Menu système via appimaged
- ✅ **Mise à jour intégrée** : AppImageUpdate pour les mises à jour

**Inconvénients :**

- ❌ **Taille** : Plus gros (dépendances dupliquées)
- ❌ **Pas de gestion centralisée** : Pas comme apt/dnf
- ❌ **Partage de bibliothèques** : Chaque AppImage inclut ses propres libs
- ❌ **Mises à jour manuelles** : Sauf si AppImageUpdate implémenté
- ❌ **Pas toujours bien intégré** : Dépend de la distribution

## Comment fonctionne AppImage ?

### Structure d'une AppImage

Une AppImage est un **fichier ISO9660 compressé avec squashfs** contenant :

```
MonApplication.AppImage (fichier exécutable)
│
├── [Runtime ELF]         # Petit programme d'extraction et lancement
│
└── [Système de fichiers compressé - squashfs]
    ├── AppRun            # Script de lancement
    ├── mon-app           # Votre binaire
    ├── lib/              # Bibliothèques nécessaires
    │   ├── libc.so.6
    │   ├── libcurl.so.4
    │   └── ...
    ├── usr/
    │   ├── bin/
    │   ├── lib/
    │   └── share/
    ├── mon-app.desktop   # Fichier desktop pour intégration
    └── mon-app.png       # Icône
```

### Cycle d'exécution

```
1. L'utilisateur lance : ./MonApplication.AppImage

2. Le runtime ELF s'exécute :
   - Monte le système de fichiers squashfs en mémoire
   - Extrait (temporairement) dans /tmp
   - Définit les variables d'environnement (LD_LIBRARY_PATH, etc.)

3. Exécute AppRun :
   - Lance le binaire principal avec le bon environnement

4. L'application s'exécute normalement

5. À la fermeture :
   - Démonte le système de fichiers
   - Nettoie /tmp
```

### Variables d'environnement

Quand une AppImage s'exécute, plusieurs variables sont définies :

```bash
APPIMAGE=/path/to/MonApplication.AppImage    # Chemin de l'AppImage  
APPDIR=/tmp/.mount_MonAppXXXXXX              # Répertoire monté  
OWD=/path/where/appimage/was/called          # Répertoire de lancement  
```

## Outils pour créer des AppImages

### 1. appimagetool (outil officiel)

**appimagetool** transforme un répertoire AppDir en fichier .AppImage.

**Installation :**
```bash
# Télécharger appimagetool
wget https://github.com/AppImage/AppImageKit/releases/download/continuous/appimagetool-x86_64.AppImage  
chmod +x appimagetool-x86_64.AppImage  

# Optionnel : déplacer dans PATH
sudo mv appimagetool-x86_64.AppImage /usr/local/bin/appimagetool
```

### 2. linuxdeploy (recommandé)

**linuxdeploy** automatise la collecte des dépendances et la création de l'AppDir.

**Installation :**
```bash
# Télécharger linuxdeploy
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage  
chmod +x linuxdeploy-x86_64.AppImage  

# Optionnel : déplacer dans PATH
sudo mv linuxdeploy-x86_64.AppImage /usr/local/bin/linuxdeploy
```

### 3. Plugins linuxdeploy

Des plugins existent pour différents frameworks :

```bash
# Plugin Qt
wget https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage  
chmod +x linuxdeploy-plugin-qt-x86_64.AppImage  

# Plugin GTK (via appimage-builder)
# Plugin Python
```

## Méthode 1 : Création manuelle simple

### Étape 1 : Créer la structure AppDir

```bash
# Créer la structure
mkdir -p MonApp.AppDir/usr/bin  
mkdir -p MonApp.AppDir/usr/lib  
mkdir -p MonApp.AppDir/usr/share/applications  
mkdir -p MonApp.AppDir/usr/share/icons/hicolor/256x256/apps  
```

### Étape 2 : Copier votre application

```bash
# Copier le binaire
cp mon-programme MonApp.AppDir/usr/bin/

# Copier les bibliothèques nécessaires (exemple)
# Trouver les dépendances avec ldd
ldd mon-programme

# Copier les bibliothèques non standard
cp /usr/lib/x86_64-linux-gnu/libcurl.so.4 MonApp.AppDir/usr/lib/
# etc.
```

### Étape 3 : Créer le fichier .desktop

**MonApp.AppDir/mon-app.desktop**

```ini
[Desktop Entry]
Name=Mon Application  
Exec=mon-programme  
Icon=mon-app  
Type=Application  
Categories=Utility;  
Comment=Mon application formidable  
```

**Important :** Le fichier .desktop doit être à la racine de AppDir.

### Étape 4 : Ajouter une icône

```bash
# Copier l'icône
cp mon-app.png MonApp.AppDir/usr/share/icons/hicolor/256x256/apps/
# Et à la racine pour la vignette
cp mon-app.png MonApp.AppDir/
```

### Étape 5 : Créer le script AppRun

**MonApp.AppDir/AppRun**

```bash
#!/bin/bash

# Obtenir le répertoire de l'AppImage
HERE="$(dirname "$(readlink -f "${0}")")"

# Définir les chemins des bibliothèques
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"

# Lancer l'application
exec "${HERE}/usr/bin/mon-programme" "$@"
```

```bash
# Rendre exécutable
chmod +x MonApp.AppDir/AppRun
```

### Étape 6 : Créer l'AppImage

```bash
# Utiliser appimagetool
appimagetool MonApp.AppDir

# Résultat : MonApp-x86_64.AppImage
```

### Étape 7 : Tester

```bash
# Rendre exécutable
chmod +x MonApp-x86_64.AppImage

# Lancer
./MonApp-x86_64.AppImage

# Vérifier qu'elle fonctionne
```

## Méthode 2 : Avec linuxdeploy (recommandé)

### Exemple simple

```bash
# Compiler votre application
gcc mon-programme.c -o mon-programme

# Créer l'AppImage avec linuxdeploy
linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    --executable mon-programme \
    --desktop-file mon-app.desktop \
    --icon-file mon-app.png \
    --output appimage

# Résultat : Mon_Application-x86_64.AppImage
```

**linuxdeploy fait automatiquement :**
- ✅ Crée la structure AppDir
- ✅ Copie le binaire
- ✅ Détecte et copie les dépendances (ldd)
- ✅ Crée le script AppRun
- ✅ Génère l'AppImage

### Exemple avec bibliothèques supplémentaires

```bash
linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    --executable mon-programme \
    --library /usr/lib/x86_64-linux-gnu/libspeciale.so \
    --desktop-file mon-app.desktop \
    --icon-file mon-app.png \
    --output appimage
```

### Exemple avec plugins

```bash
# Définir les variables pour les plugins
export QMAKE=/usr/bin/qmake  # Pour Qt

# Créer l'AppImage avec plugin Qt
linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    --executable mon-app-qt \
    --desktop-file mon-app.desktop \
    --icon-file mon-app.png \
    --plugin qt \
    --output appimage
```

## Projet complet exemple

### Structure du projet

```
mon-projet/
├── src/
│   └── main.c
├── assets/
│   ├── mon-app.desktop
│   └── mon-app.png
├── Makefile
└── build-appimage.sh
```

### Code source

**src/main.c**

```c
#include <stdio.h>

int main(int argc, char *argv[]) {
    printf("=== Mon Application ===\n");
    printf("Version: 1.0.0\n");

    if (argc > 1) {
        printf("Arguments reçus:\n");
        for (int i = 1; i < argc; i++) {
            printf("  %d: %s\n", i, argv[i]);
        }
    }

    printf("Application exécutée avec succès!\n");
    return 0;
}
```

### Makefile

**Makefile**

```makefile
CC = gcc  
CFLAGS = -Wall -O2  
TARGET = mon-app  

all: $(TARGET)

$(TARGET): src/main.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(TARGET)
	rm -rf AppDir
	rm -f *.AppImage

appimage: $(TARGET)
	./build-appimage.sh

.PHONY: all clean appimage
```

### Fichier .desktop

**assets/mon-app.desktop**

```ini
[Desktop Entry]
Type=Application  
Name=Mon Application  
Comment=Application de démonstration AppImage  
Exec=mon-app  
Icon=mon-app  
Categories=Utility;Development;  
Terminal=false  
```

### Script de build

**build-appimage.sh**

```bash
#!/bin/bash

set -e

# Configuration
APP_NAME="mon-app"  
VERSION="1.0.0"  
ARCH="x86_64"  

echo "=== Création de l'AppImage de ${APP_NAME} ==="

# 1. Compiler l'application
echo "Compilation..."  
make clean  
make  

# 2. Créer l'AppImage avec linuxdeploy
echo "Création de l'AppImage..."

# Télécharger linuxdeploy si nécessaire
if [ ! -f linuxdeploy-x86_64.AppImage ]; then
    echo "Téléchargement de linuxdeploy..."
    wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    chmod +x linuxdeploy-x86_64.AppImage
fi

# Créer l'AppImage
./linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    --executable ${APP_NAME} \
    --desktop-file assets/${APP_NAME}.desktop \
    --icon-file assets/${APP_NAME}.png \
    --output appimage

# 3. Renommer avec la version
if [ -f ${APP_NAME}-${ARCH}.AppImage ]; then
    mv ${APP_NAME}-${ARCH}.AppImage ${APP_NAME}-${VERSION}-${ARCH}.AppImage
fi

echo ""  
echo "=== AppImage créée avec succès ==="  
ls -lh ${APP_NAME}-${VERSION}-${ARCH}.AppImage  
echo ""  
echo "Pour tester :"  
echo "  chmod +x ${APP_NAME}-${VERSION}-${ARCH}.AppImage"  
echo "  ./${APP_NAME}-${VERSION}-${ARCH}.AppImage"  
```

```bash
# Rendre le script exécutable
chmod +x build-appimage.sh
```

### Build et test

```bash
# Builder l'AppImage
./build-appimage.sh

# Tester
./mon-app-1.0.0-x86_64.AppImage

# Tester avec arguments
./mon-app-1.0.0-x86_64.AppImage arg1 arg2
```

## Gestion des dépendances

### Problème des dépendances système

AppImage doit inclure les bibliothèques, mais **pas toutes** :

**À inclure :**
- ✅ Bibliothèques spécifiques à votre application
- ✅ Bibliothèques récentes non présentes partout
- ✅ Bibliothèques non standard

**À NE PAS inclure :**
- ❌ libc (glibc) - toujours présente sur le système
- ❌ libGL, libX11 - fournies par le système
- ❌ Drivers graphiques

### Lister les dépendances

```bash
# Voir les dépendances d'un binaire
ldd mon-programme

# Exemple de sortie :
# linux-vdso.so.1 (0x00007fff)
# libcurl.so.4 => /usr/lib/x86_64-linux-gnu/libcurl.so.4
# libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6
# ...

# Identifier ce qui doit être inclus
```

### Exclure des bibliothèques avec linuxdeploy

```bash
# Créer une liste d'exclusions
cat > excludelist << 'EOF'  
libc.so.6  
libdl.so.2  
libm.so.6  
libpthread.so.0  
librt.so.1  
libstdc++.so.6  
libgcc_s.so.1  
EOF  

# Utiliser avec linuxdeploy
linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    --executable mon-programme \
    --exclude-library "libc.so.6,libdl.so.2,libm.so.6" \
    --desktop-file mon-app.desktop \
    --icon-file mon-app.png \
    --output appimage
```

### AppImage avec de nombreuses dépendances

Pour des applications complexes avec beaucoup de dépendances :

**Utiliser appimage-builder :**

```bash
# Installer appimage-builder
sudo apt install python3-pip  
pip3 install appimage-builder  

# Créer un fichier de configuration
appimage-builder --generate
```

**AppImageBuilder.yml**

```yaml
version: 1

AppDir:
  path: ./AppDir

  app_info:
    id: com.example.mon-app
    name: Mon Application
    icon: mon-app
    version: 1.0.0
    exec: usr/bin/mon-app

  apt:
    arch: amd64
    sources:
      - sourceline: 'deb http://archive.ubuntu.com/ubuntu/ focal main universe'

    include:
      - libcurl4
      - libssl1.1

    exclude:
      - libc6

  files:
    include:
      - usr/bin/mon-app

  runtime:
    env:
      APPDIR_LIBRARY_PATH: $APPDIR/usr/lib/x86_64-linux-gnu

  test:
    debian:
      image: ubuntu:20.04
      command: ./AppRun
      use_host_x: true

AppImage:
  arch: x86_64
  update-information: guess
  sign-key: None
```

```bash
# Builder
appimage-builder --recipe AppImageBuilder.yml
```

## Intégration système

### AppRun personnalisé

Pour une meilleure intégration, personnalisez AppRun :

**AppDir/AppRun**

```bash
#!/bin/bash

# Répertoire de l'AppImage
HERE="$(dirname "$(readlink -f "${0}")")"

# Configuration de l'environnement
export LD_LIBRARY_PATH="${HERE}/usr/lib:${LD_LIBRARY_PATH}"  
export PATH="${HERE}/usr/bin:${PATH}"  
export XDG_DATA_DIRS="${HERE}/usr/share:${XDG_DATA_DIRS}"  

# Support du thème GTK
export GTK_THEME="${GTK_THEME:-Adwaita}"  
export GTK2_RC_FILES="${HERE}/usr/share/themes/Adwaita/gtk-2.0/gtkrc"  

# Support Qt
export QT_PLUGIN_PATH="${HERE}/usr/plugins:${QT_PLUGIN_PATH}"

# Variables spécifiques à l'application
export MY_APP_HOME="${HERE}"

# Lancer l'application
exec "${HERE}/usr/bin/mon-app" "$@"
```

### Intégration avec appimaged

**appimaged** est un daemon qui intègre automatiquement les AppImages dans le menu système.

```bash
# Installer appimaged (optionnel)
wget https://github.com/probonopd/go-appimage/releases/download/continuous/appimaged-x86_64.AppImage  
chmod +x appimaged-x86_64.AppImage  
./appimaged-x86_64.AppImage --install

# Les AppImages dans ~/Applications ou ~/Downloads seront automatiquement intégrées
mkdir -p ~/Applications  
mv MonApp.AppImage ~/Applications/  
```

### Fichier .desktop intégré

Pour une meilleure intégration, incluez des informations complètes :

```ini
[Desktop Entry]
Type=Application  
Name=Mon Application  
GenericName=Outil de démonstration  
Comment=Application exemple pour AppImage  
Exec=mon-app %U  
Icon=mon-app  
Terminal=false  
Categories=Utility;Development;  
MimeType=text/plain;  
Keywords=exemple;demo;appimage;  
StartupNotify=true  
```

## Mises à jour avec AppImageUpdate

### Ajouter le support des mises à jour

**Dans le fichier .desktop :**

```ini
[Desktop Entry]
# ... autres champs ...
X-AppImage-Update-Url=https://example.com/mon-app-latest-x86_64.AppImage
```

**Ou utiliser zsync :**

```bash
# Créer un fichier .zsync
zsyncmake mon-app-1.0.0-x86_64.AppImage

# Héberger mon-app-1.0.0-x86_64.AppImage.zsync sur un serveur

# Dans le fichier .desktop
X-AppImage-Update-Information=zsync|https://example.com/mon-app-latest-x86_64.AppImage.zsync
```

### Utiliser AppImageUpdate

```bash
# Télécharger AppImageUpdate
wget https://github.com/AppImage/AppImageUpdate/releases/download/continuous/AppImageUpdate-x86_64.AppImage  
chmod +x AppImageUpdate-x86_64.AppImage  

# Mettre à jour une AppImage
./AppImageUpdate-x86_64.AppImage mon-app-1.0.0-x86_64.AppImage
```

## Cross-compilation pour différentes architectures

### AppImage pour ARM64

```bash
# Sur un système x86_64 avec cross-compilation
aarch64-linux-gnu-gcc mon-programme.c -o mon-programme

# Télécharger linuxdeploy pour ARM64
wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-aarch64.AppImage  
chmod +x linuxdeploy-aarch64.AppImage  

# Créer l'AppImage ARM64 (nécessite QEMU pour l'exécution)
./linuxdeploy-aarch64.AppImage \
    --appdir AppDir \
    --executable mon-programme \
    --desktop-file mon-app.desktop \
    --icon-file mon-app.png \
    --output appimage
```

## Distribution des AppImages

### Hébergement direct

**GitHub Releases :**

```bash
# Les utilisateurs téléchargent
wget https://github.com/user/projet/releases/download/v1.0.0/MonApp-1.0.0-x86_64.AppImage  
chmod +x MonApp-1.0.0-x86_64.AppImage  
./MonApp-1.0.0-x86_64.AppImage
```

### AppImageHub

Soumettre votre AppImage sur https://www.appimagehub.com/

### Instructions pour les utilisateurs

**README.md**

```markdown
## Installation

### Téléchargement

Téléchargez la dernière version depuis [Releases](https://github.com/user/projet/releases).

### Utilisation

```bash
# Rendre exécutable
chmod +x MonApp-1.0.0-x86_64.AppImage

# Lancer
./MonApp-1.0.0-x86_64.AppImage
```

### Intégration au système (optionnel)

Déplacez le fichier dans `~/Applications` :

```bash
mkdir -p ~/Applications  
mv MonApp-1.0.0-x86_64.AppImage ~/Applications/  
```

Si vous avez appimaged installé, l'application apparaîtra dans votre menu.
```

## Intégration CI/CD

### GitHub Actions

**.github/workflows/appimage.yml**

```yaml
name: Build AppImage

on:
  push:
    tags:
      - 'v*'
  workflow_dispatch:

jobs:
  build-appimage:
    runs-on: ubuntu-22.04

    steps:
    - uses: actions/checkout@v4

    - name: Install dependencies
      run: |
        sudo apt-get update
        sudo apt-get install -y gcc make wget file

    - name: Get version
      id: version
      run: |
        VERSION=${GITHUB_REF#refs/tags/v}
        echo "version=$VERSION" >> $GITHUB_OUTPUT

    - name: Build application
      run: make

    - name: Download linuxdeploy
      run: |
        wget https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
        chmod +x linuxdeploy-x86_64.AppImage

    - name: Create AppImage
      run: |
        ./linuxdeploy-x86_64.AppImage \
          --appdir AppDir \
          --executable mon-app \
          --desktop-file assets/mon-app.desktop \
          --icon-file assets/mon-app.png \
          --output appimage

        # Renommer avec version
        mv mon-app-x86_64.AppImage mon-app-${{ steps.version.outputs.version }}-x86_64.AppImage

    - name: Test AppImage
      run: |
        chmod +x mon-app-${{ steps.version.outputs.version }}-x86_64.AppImage
        ./mon-app-${{ steps.version.outputs.version }}-x86_64.AppImage --version

    - name: Upload artifact
      uses: actions/upload-artifact@v4
      with:
        name: appimage
        path: '*.AppImage'

    - name: Create Release
      if: startsWith(github.ref, 'refs/tags/')
      uses: softprops/action-gh-release@v2
      with:
        files: '*.AppImage'
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

### GitLab CI

**.gitlab-ci.yml**

```yaml
image: ubuntu:22.04

stages:
  - build
  - package

variables:
  APP_NAME: mon-app
  VERSION: "1.0.0"

before_script:
  - apt-get update -qq
  - apt-get install -y -qq gcc make wget file

build:
  stage: build
  script:
    - make
  artifacts:
    paths:
      - ${APP_NAME}

package:appimage:
  stage: package
  dependencies:
    - build
  script:
    - wget -q https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage
    - chmod +x linuxdeploy-x86_64.AppImage
    - ./linuxdeploy-x86_64.AppImage
        --appdir AppDir
        --executable ${APP_NAME}
        --desktop-file assets/${APP_NAME}.desktop
        --icon-file assets/${APP_NAME}.png
        --output appimage
    - mv ${APP_NAME}-x86_64.AppImage ${APP_NAME}-${VERSION}-x86_64.AppImage
  artifacts:
    paths:
      - "*.AppImage"
```

## Bonnes pratiques

### 1. Tester sur plusieurs distributions

```bash
# Tester avec Docker
docker run -it --rm -v $(pwd):/test ubuntu:20.04 /test/MonApp.AppImage  
docker run -it --rm -v $(pwd):/test fedora:latest /test/MonApp.AppImage  
docker run -it --rm -v $(pwd):/test archlinux:latest /test/MonApp.AppImage  
```

### 2. Minimiser la taille

```bash
# Strip les binaires
strip mon-programme

# Compiler en mode Release
gcc -O2 -DNDEBUG mon-programme.c -o mon-programme

# Utiliser upx (optionnel, peut causer des problèmes)
upx --best --lzma mon-programme
```

### 3. Fournir un nom descriptif

```
✓ MonApp-1.0.0-x86_64.AppImage
✓ MonSuperOutil-2.5.3-x86_64.AppImage
✗ app.AppImage
✗ build.AppImage
```

### 4. Inclure les métadonnées

```bash
# Fichier .desktop complet
# Icône de bonne qualité (256x256 minimum)
# Fichier README dans l'AppImage
```

### 5. Documenter le processus

**BUILD.md**

```markdown
## Construction de l'AppImage

### Prérequis

- GCC
- Make
- wget (pour télécharger linuxdeploy)

### Build

```bash
./build-appimage.sh
```

### Structure

- `src/` : Code source
- `assets/` : Ressources (icônes, .desktop)
- `build-appimage.sh` : Script de build
```

### 6. Gérer les versions

```bash
# Inclure la version dans le nom du fichier
MonApp-1.0.0-x86_64.AppImage

# Ajouter l'info de version dans l'application
mon-app --version
```

### 7. Support des mises à jour

```bash
# Implémenter AppImageUpdate
# Héberger les fichiers .zsync
# Tester le processus de mise à jour
```

## Dépannage

### Erreur : "cannot execute binary file"

**Problème :** AppImage non exécutable ou mauvaise architecture

**Solution :**
```bash
# Rendre exécutable
chmod +x MonApp.AppImage

# Vérifier l'architecture
file MonApp.AppImage
```

### Erreur : "FUSE not found"

**Problème :** FUSE non installé sur le système

**Solution :**
```bash
# Installer FUSE
sudo apt-get install fuse libfuse2

# Ou extraire manuellement
./MonApp.AppImage --appimage-extract
./squashfs-root/AppRun
```

### Erreur : "library not found"

**Problème :** Dépendance manquante dans l'AppImage

**Solution :**
```bash
# Lister les dépendances
ldd mon-binaire

# Inclure les bibliothèques manquantes
linuxdeploy ... --library /path/to/missing.so
```

### AppImage ne s'intègre pas au menu

**Solution :**
```bash
# Installer appimaged
# Ou copier manuellement le .desktop
desktop-file-install --dir=$HOME/.local/share/applications MonApp.AppImage.desktop
```

### Taille trop importante

**Solutions :**
```bash
# Strip les binaires
strip --strip-all mon-binaire

# Exclure les bibliothèques système
linuxdeploy --exclude-library "liste,de,libs"

# Compiler en mode Release avec optimisations
gcc -O3 -DNDEBUG ...
```

## Comparaison des formats

| Aspect | .deb/.rpm | AppImage | Snap/Flatpak |
|--------|-----------|----------|--------------|
| **Installation** | root requis | Aucune | root requis |
| **Universalité** | Distribution spécifique | Toutes | Toutes (avec runtime) |
| **Taille** | Petite (dépendances partagées) | Moyenne | Grande (sandboxing) |
| **Isolation** | Aucune | Aucune | Forte (sandbox) |
| **Mises à jour** | Gestionnaire de paquets | Manuelle ou AppImageUpdate | Automatique |
| **Intégration** | Parfaite | Bonne | Bonne |
| **Portabilité** | Limitée | Excellente | Bonne |
| **Complexité** | Moyenne | Faible | Élevée |

## Résumé des commandes

```bash
# Créer manuellement
mkdir -p AppDir/usr/bin  
cp mon-app AppDir/usr/bin/  
# ... copier ressources ...
appimagetool AppDir

# Avec linuxdeploy
linuxdeploy-x86_64.AppImage \
    --appdir AppDir \
    --executable mon-app \
    --desktop-file mon-app.desktop \
    --icon-file mon-app.png \
    --output appimage

# Lancer une AppImage
chmod +x MonApp.AppImage
./MonApp.AppImage

# Extraire le contenu
./MonApp.AppImage --appimage-extract

# Inspecter
./MonApp.AppImage --appimage-help

# Mise à jour (si supporté)
./MonApp.AppImage --appimage-update
```

## Conclusion

**AppImage** offre une solution simple et élégante pour distribuer des applications Linux de manière universelle.

**Points clés :**

1. **Un fichier = Une application** : Simplicité maximale
2. **Portable** : Fonctionne partout
3. **Pas d'installation** : Télécharger et exécuter
4. **Autonome** : Toutes les dépendances incluses
5. **linuxdeploy** : Outil recommandé pour automatiser la création

**Quand utiliser AppImage ?**

- ✅ Distribution grand public multi-distributions
- ✅ Applications portables
- ✅ Versions multiples côte à côte
- ✅ Pas besoin de root
- ✅ Déploiement rapide

**Quand préférer .deb/.rpm ?**

- Intégration système complète
- Distribution via dépôts officiels
- Mises à jour automatiques système
- Partage de bibliothèques

**Workflow recommandé :**

```
1. Compiler l'application
2. Utiliser linuxdeploy pour créer l'AppImage
3. Tester sur plusieurs distributions
4. Distribuer via GitHub Releases
5. Implémenter AppImageUpdate pour les mises à jour
6. Automatiser avec CI/CD
```

AppImage démocratise la distribution d'applications Linux en la rendant aussi simple que sur d'autres plateformes : un fichier à télécharger et exécuter, sans complications.

---


⏭️ [Conteneurisation : Docker multi-stage](/31-cross-compilation-deploiement/08-docker-multi-stage.md)
