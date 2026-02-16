🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 31.6 Packaging Linux : .rpm

## Introduction

Après avoir vu le packaging **.deb** pour Debian/Ubuntu, explorons maintenant le monde **RPM** (Red Hat Package Manager), utilisé par les distributions Red Hat, CentOS, Fedora, openSUSE, et bien d'autres.

Tout comme .deb facilite la distribution sur Debian/Ubuntu, le format .rpm simplifie l'installation sur les systèmes Red Hat et dérivés.

### Qu'est-ce qu'un package RPM ?

Un fichier **.rpm** est une archive qui contient :
- ✅ Vos programmes compilés (binaires)
- ✅ Les fichiers de configuration
- ✅ La documentation
- ✅ Les métadonnées (nom, version, dépendances)
- ✅ Les scripts d'installation/désinstallation

### Différences .deb vs .rpm

| Aspect | .deb | .rpm |
|--------|------|------|
| **Distributions** | Debian, Ubuntu, Mint | Red Hat, CentOS, Fedora, openSUSE |
| **Gestionnaire** | dpkg / apt | rpm / yum / dnf |
| **Configuration** | Fichier control | Fichier .spec |
| **Build** | dpkg-deb | rpmbuild |
| **Philosophie** | Simplicité | Flexibilité |

### Pourquoi créer un package RPM ?

**Sans package :**
```bash
# L'utilisateur doit faire :
git clone https://github.com/user/projet.git  
cd projet  
make  
sudo make install  
```

**Avec package :**
```bash
# L'utilisateur fait simplement :
sudo dnf install mon-programme
# Ou
sudo rpm -i mon-programme-1.0.0-1.x86_64.rpm
```

## Environnement de build

### Distributions pour tester

Pour créer et tester des packages RPM, vous pouvez utiliser :

**Fedora (recommandé pour débuter) :**
```bash
# Dans un conteneur Docker
docker run -it fedora:latest bash
```

**CentOS Stream :**
```bash
docker run -it quay.io/centos/centos:stream9 bash
```

**Sur Ubuntu (avec alien - moins recommandé) :**
```bash
sudo apt-get install alien rpm
# Permet de créer des RPM mais avec limitations
```

### Installation des outils

**Sur Fedora :**
```bash
# Installer les outils de build RPM
sudo dnf install rpm-build rpmdevtools rpmlint

# Vérifier
rpmbuild --version
```

**Sur CentOS/RHEL :**
```bash
# Installer les outils de développement
sudo yum groupinstall "Development Tools"  
sudo yum install rpm-build rpmdevtools rpmlint  
```

## Structure RPM : rpmbuild

### Initialiser l'environnement

```bash
# Créer la structure de répertoires
rpmdev-setuptree

# Cela crée :
~/rpmbuild/
├── BUILD/      # Répertoire de compilation temporaire
├── RPMS/       # Packages RPM générés
├── SOURCES/    # Archives sources et patches
├── SPECS/      # Fichiers .spec
└── SRPMS/      # Source RPMs
```

### Comprendre la structure

| Répertoire | Rôle |
|------------|------|
| **SPECS/** | Contient les fichiers .spec (recettes de build) |
| **SOURCES/** | Archives tar.gz des sources + patches |
| **BUILD/** | Décompression et compilation (temporaire) |
| **RPMS/** | Packages binaires générés (.rpm) |
| **SRPMS/** | Source packages (.src.rpm) |

## Le fichier .spec

### Qu'est-ce qu'un fichier .spec ?

Le fichier **.spec** est le cœur du packaging RPM. C'est un **script** qui décrit :
- Les métadonnées du package
- Comment compiler le programme
- Quels fichiers installer et où
- Les scripts à exécuter lors de l'installation/désinstallation

### Structure d'un fichier .spec

**~/rpmbuild/SPECS/mon-programme.spec**

```spec
# === Métadonnées ===
Name:           mon-programme  
Version:        1.0.0  
Release:        1%{?dist}  
Summary:        Description courte en une ligne  

License:        MIT  
URL:            https://github.com/user/mon-programme  
Source0:        %{name}-%{version}.tar.gz  

BuildRequires:  gcc  
Requires:       glibc  

%description
Description longue et détaillée du programme.  
Peut s'étendre sur plusieurs lignes.  

# === Préparation ===
%prep
%setup -q

# === Compilation ===
%build
gcc %{optflags} src/main.c -o %{name}

# === Installation ===
%install
mkdir -p %{buildroot}%{_bindir}  
install -m 755 %{name} %{buildroot}%{_bindir}/%{name}  

# === Liste des fichiers ===
%files
%{_bindir}/%{name}
%doc README.md
%license LICENSE

# === Changelog ===
%changelog
* Mon Nov 28 2024 Votre Nom <email@example.com> - 1.0.0-1
- Initial release
```

### Explication des sections

#### En-tête (métadonnées)

```spec
Name:           mon-programme           # Nom du package  
Version:        1.0.0                   # Version du logiciel  
Release:        1%{?dist}               # Numéro de release (1.fc39, 1.el9, etc.)  
Summary:        Description courte      # Une ligne, < 80 caractères  

License:        MIT                     # Licence du logiciel  
URL:            https://...             # Site web du projet  
Source0:        %{name}-%{version}.tar.gz  # Archive source  

BuildRequires:  gcc                     # Dépendances de build  
Requires:       glibc                   # Dépendances d'exécution  
```

**Tags importants :**

- **Name** : Nom unique du package (minuscules, tirets)
- **Version** : Version du logiciel (format : X.Y.Z)
- **Release** : Numéro de build du package (commence à 1)
- **%{?dist}** : Macro ajoutant l'identifiant de distribution (.fc39, .el9)
- **Summary** : Description très courte
- **License** : Licence SPDX (MIT, GPL-3.0, Apache-2.0, etc.)
- **Source0** : Archive principale (peut avoir Source1, Source2, etc.)

#### Section %description

```spec
%description
Description complète du programme sur plusieurs lignes.  
Peut inclure :  
- Fonctionnalités principales
- Cas d'usage
- Informations importantes

Cette section est affichée lors de "rpm -qi mon-programme"
```

#### Section %prep (préparation)

```spec
%prep
%setup -q          # Décompresse Source0 dans BUILD/
                   # -q = quiet mode

# Si vous avez des patches :
%patch0 -p1        # Applique patch0
```

**Macros utiles :**
- `%setup -q` : Décompresse l'archive source
- `%setup -q -n dossier` : Si le dossier a un nom différent
- `%patch0 -p1` : Applique un patch

#### Section %build (compilation)

```spec
%build
# Compilation classique
gcc %{optflags} src/main.c -o %{name}

# Ou avec configure
%configure
make %{?_smp_mflags}

# Ou avec CMake
%cmake .
%cmake_build
```

**Macros utiles :**
- `%{optflags}` : Flags d'optimisation standards (-O2 -g, etc.)
- `%configure` : Lance ./configure avec les bonnes options
- `%{?_smp_mflags}` : make -jN avec N = nombre de CPUs
- `%cmake` / `%cmake_build` : Macros CMake

#### Section %install (installation)

```spec
%install
# Créer les répertoires dans buildroot
mkdir -p %{buildroot}%{_bindir}  
mkdir -p %{buildroot}%{_mandir}/man1  
mkdir -p %{buildroot}%{_docdir}/%{name}  

# Copier les fichiers
install -m 755 %{name} %{buildroot}%{_bindir}/%{name}  
install -m 644 %{name}.1 %{buildroot}%{_mandir}/man1/%{name}.1  
install -m 644 README.md %{buildroot}%{_docdir}/%{name}/  

# Ou utiliser make install
make install DESTDIR=%{buildroot}
```

**Macros de chemins :**
- `%{buildroot}` : Racine temporaire d'installation
- `%{_bindir}` : `/usr/bin`
- `%{_sbindir}` : `/usr/sbin`
- `%{_libdir}` : `/usr/lib64` ou `/usr/lib`
- `%{_includedir}` : `/usr/include`
- `%{_mandir}` : `/usr/share/man`
- `%{_docdir}` : `/usr/share/doc`
- `%{_sysconfdir}` : `/etc`
- `%{_localstatedir}` : `/var`

#### Section %files (liste des fichiers)

```spec
%files
%{_bindir}/%{name}                    # Binaire principal
%doc README.md CHANGELOG.md           # Documentation
%license LICENSE                      # Fichier de licence
%config(noreplace) %{_sysconfdir}/%{name}/config.conf  # Config
%{_mandir}/man1/%{name}.1*            # Page de manuel
```

**Attributs :**
- `%doc` : Fichier de documentation
- `%license` : Fichier de licence
- `%config` : Fichier de configuration
- `%config(noreplace)` : Ne pas écraser lors des mises à jour
- `%attr(mode, user, group)` : Permissions spécifiques
- `%dir` : Répertoire (sans son contenu)

#### Section %changelog

```spec
%changelog
* Mon Nov 28 2024 Votre Nom <email@example.com> - 1.0.0-1
- Initial release
- Add feature X
- Fix bug Y

* Fri Nov 15 2024 Votre Nom <email@example.com> - 0.9.0-1
- Beta release
```

**Format strict :**
- `*` en début de ligne
- Date au format `Day Mon DD YYYY`
- Nom et email du mainteneur
- Version-Release
- Changements précédés de `-` et un espace

## Exemple complet : Programme simple

### Créer le projet source

```bash
# Créer la structure du projet
mkdir -p mon-programme-1.0.0/src  
cd mon-programme-1.0.0  

# Code source simple
cat > src/main.c << 'EOF'
#include <stdio.h>

int main(void) {
    printf("Mon Programme v1.0.0\n");
    printf("Hello from RPM!\n");
    return 0;
}
EOF

# Makefile simple
cat > Makefile << 'EOF'  
CC = gcc  
CFLAGS = -Wall -O2  
PREFIX = /usr  
BINDIR = $(PREFIX)/bin  

all: mon-programme

mon-programme: src/main.c
	$(CC) $(CFLAGS) src/main.c -o mon-programme

install:
	install -D -m 755 mon-programme $(DESTDIR)$(BINDIR)/mon-programme

clean:
	rm -f mon-programme

.PHONY: all install clean
EOF

# README
cat > README.md << 'EOF'
# Mon Programme

Programme exemple pour démonstration RPM.

## Installation

```bash
sudo dnf install mon-programme
```
EOF

# Licence
cat > LICENSE << 'EOF'  
MIT License  

Copyright (c) 2024 Votre Nom

Permission is hereby granted...  
EOF  
```

### Créer l'archive source

```bash
# Remonter d'un niveau
cd ..

# Créer l'archive tar.gz
tar czf mon-programme-1.0.0.tar.gz mon-programme-1.0.0/

# Copier dans SOURCES
cp mon-programme-1.0.0.tar.gz ~/rpmbuild/SOURCES/
```

### Créer le fichier .spec

**~/rpmbuild/SPECS/mon-programme.spec**

```spec
Name:           mon-programme  
Version:        1.0.0  
Release:        1%{?dist}  
Summary:        Programme de démonstration RPM  

License:        MIT  
URL:            https://github.com/user/mon-programme  
Source0:        %{name}-%{version}.tar.gz  

BuildRequires:  gcc  
BuildRequires:  make  
Requires:       glibc  

%description
Programme simple de démonstration pour le packaging RPM.  
Illustre les concepts de base de création de packages RPM.  

%prep
%setup -q

%build
make %{?_smp_mflags}

%install
make install DESTDIR=%{buildroot}

%files
%{_bindir}/%{name}
%doc README.md
%license LICENSE

%changelog
* Mon Nov 28 2024 Votre Nom <email@example.com> - 1.0.0-1
- Initial release
```

### Builder le package

```bash
# Builder le package RPM
cd ~/rpmbuild/SPECS  
rpmbuild -ba mon-programme.spec  

# Options :
# -ba : Build All (binaire + source)
# -bb : Build Binary only
# -bs : Build Source only

# Les packages sont créés dans :
# ~/rpmbuild/RPMS/x86_64/mon-programme-1.0.0-1.fc39.x86_64.rpm
# ~/rpmbuild/SRPMS/mon-programme-1.0.0-1.fc39.src.rpm
```

### Vérifier et installer

```bash
# Inspecter le package
rpm -qip ~/rpmbuild/RPMS/x86_64/mon-programme-1.0.0-1.*.rpm

# Lister le contenu
rpm -qlp ~/rpmbuild/RPMS/x86_64/mon-programme-1.0.0-1.*.rpm

# Installer
sudo rpm -ivh ~/rpmbuild/RPMS/x86_64/mon-programme-1.0.0-1.*.rpm

# Tester
mon-programme

# Vérifier l'installation
rpm -qi mon-programme  
rpm -ql mon-programme  

# Désinstaller
sudo rpm -e mon-programme
```

## Scripts de maintenance

Comme .deb, RPM supporte des scripts exécutés lors de l'installation/désinstallation.

### Types de scripts

| Script | Moment d'exécution | Équivalent .deb |
|--------|-------------------|-----------------|
| **%pre** | Avant l'installation | preinst |
| **%post** | Après l'installation | postinst |
| **%preun** | Avant la désinstallation | prerm |
| **%postun** | Après la désinstallation | postrm |

### Variables disponibles dans les scripts

```bash
$1 = Nombre de packages après l'opération
     Installation : $1 = 1
     Mise à jour : $1 = 2
     Désinstallation : $1 = 0
```

### Exemple : Package avec daemon

**mon-daemon.spec**

```spec
Name:           mon-daemon  
Version:        1.0.0  
Release:        1%{?dist}  
Summary:        Daemon de démonstration  

License:        MIT  
URL:            https://github.com/user/mon-daemon  
Source0:        %{name}-%{version}.tar.gz  
Source1:        %{name}.service  

BuildRequires:  gcc systemd-rpm-macros  
Requires:       glibc  
Requires(post): systemd  
Requires(preun): systemd  
Requires(postun): systemd  

%description
Un daemon de démonstration avec service systemd.

%prep
%setup -q

%build
gcc %{optflags} src/daemon.c -o %{name}

%install
# Installer le binaire
mkdir -p %{buildroot}%{_bindir}  
install -m 755 %{name} %{buildroot}%{_bindir}/%{name}  

# Installer le fichier service
mkdir -p %{buildroot}%{_unitdir}  
install -m 644 %{SOURCE1} %{buildroot}%{_unitdir}/%{name}.service  

# Créer les répertoires
mkdir -p %{buildroot}%{_localstatedir}/log/%{name}  
mkdir -p %{buildroot}%{_sysconfdir}/%{name}  

%pre
# Créer l'utilisateur système
getent group %{name} >/dev/null || groupadd -r %{name}  
getent passwd %{name} >/dev/null || \  
    useradd -r -g %{name} -d %{_localstatedir}/lib/%{name} \
    -s /sbin/nologin -c "Daemon user" %{name}
exit 0

%post
%systemd_post %{name}.service

%preun
%systemd_preun %{name}.service

%postun
%systemd_postun_with_restart %{name}.service

%files
%{_bindir}/%{name}
%{_unitdir}/%{name}.service
%dir %{_sysconfdir}/%{name}
%dir %{_localstatedir}/log/%{name}
%doc README.md
%license LICENSE

%changelog
* Mon Nov 28 2024 Votre Nom <email@example.com> - 1.0.0-1
- Initial release
```

**Macros systemd :**
- `%systemd_post` : Configure le service après installation
- `%systemd_preun` : Arrête le service avant désinstallation
- `%systemd_postun_with_restart` : Redémarre après mise à jour

### Scripts personnalisés avancés

```spec
%pre
# Avant installation
if [ $1 -eq 1 ]; then
    # Nouvelle installation
    echo "Première installation"
elif [ $1 -eq 2 ]; then
    # Mise à jour
    echo "Mise à jour depuis une version existante"
fi

%post
# Après installation
if [ $1 -eq 1 ]; then
    # Nouvelle installation
    # Activer et démarrer le service
    systemctl enable --now mon-daemon.service
elif [ $1 -eq 2 ]; then
    # Mise à jour
    # Redémarrer le service
    systemctl try-restart mon-daemon.service
fi

%preun
# Avant désinstallation
if [ $1 -eq 0 ]; then
    # Désinstallation complète (pas une mise à jour)
    systemctl --no-reload disable --now mon-daemon.service
fi

%postun
# Après désinstallation
if [ $1 -eq 0 ]; then
    # Nettoyage après désinstallation complète
    rm -rf /var/log/mon-daemon
fi
```

## Gestion des dépendances

### BuildRequires vs Requires

```spec
# Dépendances de build (compilation)
BuildRequires:  gcc  
BuildRequires:  make  
BuildRequires:  cmake  
BuildRequires:  pkgconfig(openssl)  

# Dépendances d'exécution (runtime)
Requires:       glibc  
Requires:       openssl-libs >= 3.0.0  

# Dépendances spécifiques pour les scripts
Requires(pre):    shadow-utils  
Requires(post):   systemd  
Requires(preun):  systemd  
Requires(postun): systemd  
```

### Opérateurs de version

```spec
Requires: paquet                    # N'importe quelle version  
Requires: paquet >= 1.0.0           # Version minimale  
Requires: paquet <= 2.0.0           # Version maximale  
Requires: paquet = 1.5.0            # Version exacte  
Requires: paquet < 3.0.0            # Strictement inférieure  
Requires: paquet > 0.5.0            # Strictement supérieure  
```

### Dépendances automatiques

RPM détecte automatiquement certaines dépendances :

```spec
# Si votre binaire utilise libcurl.so.4
# RPM ajoutera automatiquement :
Requires: libcurl.so.4()(64bit)

# Pour voir les dépendances automatiques :
rpm -qpR mon-programme.rpm
```

### Provides et Conflicts

```spec
# Fournir une fonctionnalité virtuelle
Provides: webserver  
Provides: mon-ancienne-lib = 2.0  

# Conflits avec d'autres packages
Conflicts: autre-webserver  
Conflicts: ancien-programme  

# Obsolète (remplace) un ancien package
Obsoletes: ancien-nom < 2.0
```

## Cross-compilation et multi-architectures

### Architectures RPM

| Architecture RPM | Description |
|------------------|-------------|
| `x86_64` | Intel/AMD 64 bits |
| `i686` | Intel 32 bits |
| `aarch64` | ARM 64 bits |
| `armv7hl` | ARM 32 bits hard float |
| `ppc64le` | PowerPC 64 bits little-endian |
| `s390x` | IBM System z |
| `noarch` | Indépendant de l'architecture |

### Spécifier l'architecture

```spec
# Dans le .spec
BuildArch:      noarch        # Pour scripts, données
# Ou
ExcludeArch:    i686          # Exclure certaines architectures  
ExclusiveArch:  x86_64 aarch64  # Limiter aux architectures listées  
```

### Cross-compiler pour ARM

**Sur Fedora avec cross-compiler :**

```bash
# Installer le cross-compiler
sudo dnf install gcc-aarch64-linux-gnu

# Modifier le .spec pour cross-compilation
cat > mon-programme.spec << 'EOF'  
Name:           mon-programme  
Version:        1.0.0  
Release:        1%{?dist}  
Summary:        Programme cross-compilé pour ARM  

License:        MIT  
Source0:        %{name}-%{version}.tar.gz  

BuildRequires:  gcc-aarch64-linux-gnu

%description
Programme cross-compilé pour architecture ARM64.

%prep
%setup -q

%build
aarch64-linux-gnu-gcc %{optflags} src/main.c -o %{name}

%install
mkdir -p %{buildroot}%{_bindir}  
install -m 755 %{name} %{buildroot}%{_bindir}/%{name}  

%files
%{_bindir}/%{name}
%doc README.md
%license LICENSE

%changelog
* Mon Nov 28 2024 Votre Nom <email@example.com> - 1.0.0-1
- Initial ARM64 build
EOF

# Builder
rpmbuild -ba mon-programme.spec --target aarch64
```

## Mock : Builds propres et isolés

### Qu'est-ce que Mock ?

**Mock** est un outil qui crée un **environnement chroot** propre pour builder les packages RPM, garantissant :
- ✅ Builds reproductibles
- ✅ Isolation des dépendances
- ✅ Support multi-distributions
- ✅ Pas de pollution de votre système

### Installation

```bash
# Sur Fedora
sudo dnf install mock

# Ajouter votre utilisateur au groupe mock
sudo usermod -a -G mock $USER

# Recharger les groupes
newgrp mock

# Vérifier
groups | grep mock
```

### Utiliser Mock

```bash
# Lister les configurations disponibles
mock --list-chroots

# Exemples de chroots :
# - fedora-39-x86_64
# - fedora-39-aarch64
# - centos-stream-9-x86_64
# - rhel-9-x86_64

# Builder un package avec Mock
mock -r fedora-39-x86_64 ~/rpmbuild/SRPMS/mon-programme-1.0.0-1.src.rpm

# Le package sera dans :
# /var/lib/mock/fedora-39-x86_64/result/
```

### Avantages de Mock

```bash
# Builder pour plusieurs distributions
mock -r fedora-39-x86_64 mon-programme.src.rpm  
mock -r fedora-38-x86_64 mon-programme.src.rpm  
mock -r centos-stream-9-x86_64 mon-programme.src.rpm  

# Builder pour plusieurs architectures (si configuré)
mock -r fedora-39-aarch64 mon-programme.src.rpm
```

## Vérification avec rpmlint

### Installation et utilisation

```bash
# Installer rpmlint
sudo dnf install rpmlint

# Vérifier un fichier .spec
rpmlint mon-programme.spec

# Vérifier un package RPM
rpmlint mon-programme-1.0.0-1.fc39.x86_64.rpm

# Vérifier un source RPM
rpmlint mon-programme-1.0.0-1.fc39.src.rpm
```

### Comprendre les messages

```
E: error          → Erreur critique (doit être corrigée)  
W: warning        → Avertissement (devrait être corrigé)  
I: info           → Information  
```

**Exemples de messages courants :**

```bash
# Bon
rpmlint mon-programme.rpm  
mon-programme.x86_64: I: checking  
mon-programme.x86_64: W: no-documentation  
1 packages and 0 specfiles checked; 0 errors, 1 warnings.

# Erreurs à corriger
E: invalid-spec-name    # Nom du .spec incorrect  
E: no-description       # Description manquante  
W: no-manual-page-for-binary  # Pas de page man  
W: unstripped-binary-or-object  # Binaire non strippé  
```

### Configuration personnalisée

**~/.config/rpmlint**

```ini
# Ignorer certains warnings
addFilter("no-manual-page-for-binary")  
addFilter("no-documentation")  
```

## Distribution des packages RPM

### Méthode 1 : Distribution directe

```bash
# Sur GitHub Releases ou site web
wget https://example.com/mon-programme-1.0.0-1.fc39.x86_64.rpm  
sudo dnf install ./mon-programme-1.0.0-1.fc39.x86_64.rpm  
```

### Méthode 2 : Dépôt YUM/DNF

#### Structure d'un dépôt

```
mon-repo/
├── x86_64/
│   ├── mon-programme-1.0.0-1.fc39.x86_64.rpm
│   └── autre-package-2.0.0-1.fc39.x86_64.rpm
├── aarch64/
│   └── mon-programme-1.0.0-1.fc39.aarch64.rpm
└── repodata/
    ├── repomd.xml
    ├── primary.xml.gz
    ├── filelists.xml.gz
    └── other.xml.gz
```

#### Créer un dépôt avec createrepo

```bash
# Installer createrepo
sudo dnf install createrepo_c

# Créer la structure
mkdir -p mon-repo/x86_64

# Copier les packages
cp ~/rpmbuild/RPMS/x86_64/*.rpm mon-repo/x86_64/

# Créer les métadonnées
createrepo_c mon-repo/x86_64/

# Mettre à jour après ajout de packages
createrepo_c --update mon-repo/x86_64/
```

#### Publier le dépôt

**Sur un serveur web :**

```bash
# Copier le dépôt sur le serveur
scp -r mon-repo/ user@server:/var/www/html/repo/
```

**Configuration client :**

**/etc/yum.repos.d/mon-repo.repo**

```ini
[mon-repo]
name=Mon Dépôt Personnel  
baseurl=https://example.com/repo/x86_64/  
enabled=1  
gpgcheck=0  
```

Puis :

```bash
# Mettre à jour la liste des packages
sudo dnf makecache

# Installer
sudo dnf install mon-programme
```

### Méthode 3 : COPR (Fedora)

**COPR** est le service d'hébergement de dépôts communautaires de Fedora :

```bash
# 1. Créer un compte sur https://copr.fedorainfracloud.org/
# 2. Créer un projet
# 3. Builder le package

# Installation de copr-cli
sudo dnf install copr-cli

# Uploader un SRPM
copr-cli build username/projet mon-programme-1.0.0-1.src.rpm

# Les utilisateurs peuvent installer avec :
sudo dnf copr enable username/projet  
sudo dnf install mon-programme  
```

## Intégration CI/CD

### GitHub Actions

**.github/workflows/build-rpm.yml**

```yaml
name: Build RPM Packages

on:
  push:
    tags:
      - 'v*'
  workflow_dispatch:

jobs:
  build-rpm:
    runs-on: ubuntu-latest
    container: fedora:latest

    strategy:
      matrix:
        arch: [x86_64, aarch64]

    steps:
    - uses: actions/checkout@v4

    - name: Install build dependencies
      run: |
        dnf install -y rpm-build rpmdevtools gcc make
        if [ "${{ matrix.arch }}" = "aarch64" ]; then
          dnf install -y gcc-aarch64-linux-gnu
        fi

    - name: Setup RPM build tree
      run: rpmdev-setuptree

    - name: Get version
      id: version
      run: |
        VERSION=${GITHUB_REF#refs/tags/v}
        echo "version=$VERSION" >> $GITHUB_OUTPUT

    - name: Create source tarball
      run: |
        mkdir mon-programme-${{ steps.version.outputs.version }}
        cp -r src Makefile README.md LICENSE \
          mon-programme-${{ steps.version.outputs.version }}/
        tar czf mon-programme-${{ steps.version.outputs.version }}.tar.gz \
          mon-programme-${{ steps.version.outputs.version }}
        cp mon-programme-${{ steps.version.outputs.version }}.tar.gz \
          ~/rpmbuild/SOURCES/

    - name: Copy spec file
      run: cp rpm/mon-programme.spec ~/rpmbuild/SPECS/

    - name: Build RPM
      run: |
        cd ~/rpmbuild/SPECS
        if [ "${{ matrix.arch }}" = "x86_64" ]; then
          rpmbuild -ba mon-programme.spec
        else
          rpmbuild -ba mon-programme.spec --target ${{ matrix.arch }}
        fi

    - name: Upload RPM artifacts
      uses: actions/upload-artifact@v4
      with:
        name: rpm-packages-${{ matrix.arch }}
        path: ~/rpmbuild/RPMS/**/*.rpm

    - name: Create Release
      if: startsWith(github.ref, 'refs/tags/')
      uses: softprops/action-gh-release@v2
      with:
        files: ~/rpmbuild/RPMS/**/*.rpm
      env:
        GITHUB_TOKEN: ${{ secrets.GITHUB_TOKEN }}
```

### GitLab CI

**.gitlab-ci.yml**

```yaml
image: fedora:latest

stages:
  - build
  - package
  - deploy

variables:
  PACKAGE_NAME: mon-programme
  VERSION: "1.0.0"

before_script:
  - dnf install -y rpm-build rpmdevtools gcc make rpmlint

build:x86_64:
  stage: build
  script:
    - gcc src/main.c -o ${PACKAGE_NAME} -O2 -Wall
  artifacts:
    paths:
      - ${PACKAGE_NAME}

setup:
  stage: package
  script:
    - rpmdev-setuptree
    - mkdir ${PACKAGE_NAME}-${VERSION}
    - cp src/* Makefile README.md LICENSE ${PACKAGE_NAME}-${VERSION}/
    - tar czf ${PACKAGE_NAME}-${VERSION}.tar.gz ${PACKAGE_NAME}-${VERSION}
    - cp ${PACKAGE_NAME}-${VERSION}.tar.gz ~/rpmbuild/SOURCES/
    - cp rpm/${PACKAGE_NAME}.spec ~/rpmbuild/SPECS/
  artifacts:
    paths:
      - ~/rpmbuild/SOURCES/*
      - ~/rpmbuild/SPECS/*

package:rpm:
  stage: package
  dependencies:
    - setup
  script:
    - cd ~/rpmbuild/SPECS
    - rpmbuild -ba ${PACKAGE_NAME}.spec
    - rpmlint ~/rpmbuild/RPMS/*/*.rpm
  artifacts:
    paths:
      - ~/rpmbuild/RPMS/*/*.rpm
      - ~/rpmbuild/SRPMS/*.rpm

deploy:repo:
  stage: deploy
  script:
    - echo "Deploy to repository"
  only:
    - tags
```

## Bonnes pratiques

### 1. Utiliser les macros RPM

```spec
# ❌ Mauvais : chemins en dur
/usr/bin/mon-programme
/usr/share/doc/mon-programme

# ✅ Bon : utiliser les macros
%{_bindir}/%{name}
%{_docdir}/%{name}
```

### 2. Respecter le Filesystem Hierarchy Standard

```spec
# Binaires utilisateur
%{_bindir}           # /usr/bin

# Binaires système
%{_sbindir}          # /usr/sbin

# Bibliothèques
%{_libdir}           # /usr/lib64 ou /usr/lib

# Configuration
%{_sysconfdir}       # /etc

# Données variables
%{_localstatedir}    # /var

# Documentation
%{_docdir}           # /usr/share/doc
```

### 3. Documentation complète

```spec
%files
%doc README.md CHANGELOG.md AUTHORS
%license LICENSE
%{_mandir}/man1/%{name}.1*
```

### 4. Support systemd

```spec
BuildRequires: systemd-rpm-macros

%post
%systemd_post %{name}.service

%preun
%systemd_preun %{name}.service

%postun
%systemd_postun_with_restart %{name}.service
```

### 5. Changelog détaillé

```spec
%changelog
* Mon Nov 28 2024 Nom <email> - 1.0.1-1
- Fix security vulnerability CVE-2024-1234
- Improve performance by 20%%
- Add new --verbose flag

* Fri Nov 15 2024 Nom <email> - 1.0.0-1
- Initial stable release
```

**Note :** Échapper les `%` avec `%%` dans le changelog.

### 6. Vérification avant publication

```bash
# Vérifier le .spec
rpmlint mon-programme.spec

# Vérifier le package
rpmlint mon-programme.rpm

# Tester l'installation
sudo dnf install ./mon-programme.rpm

# Tester la désinstallation
sudo dnf remove mon-programme
```

## Dépannage

### Erreur : "File not found"

**Problème :** Fichier dans %files mais pas installé dans %install

**Solution :**
```bash
# Lister ce qui est installé dans buildroot
find %{buildroot} -type f

# Vérifier que tous les fichiers de %files sont installés
```

### Erreur : "Installed (but unpackaged) file(s) found"

**Problème :** Fichiers installés mais pas listés dans %files

**Solution :**
```spec
# Ajouter les fichiers manquants dans %files
# Ou supprimer les fichiers non désirés dans %install
```

### Erreur : "Bad exit status"

**Problème :** Échec lors de la compilation

**Solution :**
```bash
# Débugger en mode verbeux
rpmbuild -ba mon-programme.spec --verbose

# Vérifier les logs dans :
# ~/rpmbuild/BUILD/
```

### Warning : "no-documentation"

**Solution :**
```spec
%files
%doc README.md    # Ajouter de la documentation
```

### Warning : "unstripped-binary-or-object"

**Solution :**
```bash
# Strip automatique (par défaut)
# Ou désactiver si vous voulez garder les symboles de debug
%global _enable_debug_packages 1
%global debug_package %{nil}
```

## Comparaison .deb vs .rpm

| Aspect | .deb | .rpm |
|--------|------|------|
| **Configuration** | Fichier control | Fichier .spec |
| **Build** | dpkg-deb | rpmbuild |
| **Scripts** | preinst, postinst, prerm, postrm | %pre, %post, %preun, %postun |
| **Dépendances** | Depends, Recommends | Requires, BuildRequires |
| **Macros** | Limitées | Très nombreuses |
| **Isolation** | sbuild, pbuilder | mock |
| **Vérification** | lintian | rpmlint |
| **Dépôts** | apt repository | yum/dnf repository |

## Résumé des commandes

```bash
# Setup
rpmdev-setuptree  
rpmdev-newspec mon-programme  

# Build
rpmbuild -ba mon-programme.spec         # Build all  
rpmbuild -bb mon-programme.spec         # Binary only  
rpmbuild -bs mon-programme.spec         # Source only  

# Vérification
rpm -qip package.rpm                    # Info  
rpm -qlp package.rpm                    # Contenu  
rpmlint package.rpm                     # Validation  

# Installation
sudo rpm -ivh package.rpm               # Install  
sudo rpm -Uvh package.rpm               # Upgrade  
sudo rpm -e package                     # Erase (remove)  

# Gestion avec DNF
sudo dnf install ./package.rpm  
sudo dnf remove package  

# Queries
rpm -qa                                 # Tous les packages  
rpm -qi package                         # Info d'un package installé  
rpm -ql package                         # Fichiers d'un package  
rpm -qf /usr/bin/command                # Quel package fournit ce fichier?  

# Mock
mock -r fedora-39-x86_64 package.src.rpm

# Dépôt
createrepo_c /path/to/repo
```

## Conclusion

Le packaging RPM est un processus structuré mais flexible qui permet de distribuer professionnellement vos programmes sur les systèmes Red Hat et dérivés.

**Points clés :**

1. **Fichier .spec** : Recette complète du build
2. **rpmbuild** : Outil de création de packages
3. **Mock** : Builds isolés et reproductibles
4. **rpmlint** : Vérification de qualité
5. **createrepo** : Création de dépôts YUM/DNF
6. **COPR** : Hébergement communautaire Fedora

**Workflow recommandé :**

```
1. Créer le .spec avec métadonnées complètes
2. Builder avec rpmbuild
3. Vérifier avec rpmlint
4. Tester avec Mock
5. Distribuer via dépôt ou COPR
6. Automatiser avec CI/CD
```

Le packaging RPM demande un peu plus de configuration initiale que .deb, mais offre une grande flexibilité et puissance grâce à son système de macros.

---


⏭️ [AppImage](/31-cross-compilation-deploiement/07-appimage.md)
