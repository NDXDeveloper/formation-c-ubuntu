🔝 Retour au [Sommaire](/SOMMAIRE.md)

# D.3 Acronymes DevOps

## Glossaire des acronymes essentiels DevOps et CI/CD

Ce glossaire rassemble les acronymes couramment utilisés dans l'écosystème DevOps, l'intégration continue, et le déploiement automatisé. Chaque acronyme est accompagné de sa signification complète et d'explications accessibles pour faciliter votre compréhension.

---

## A

### **ABI**
**Application Binary Interface**
Interface définissant comment les programmes compilés interagissent au niveau binaire (format des appels de fonctions, passage de paramètres, organisation mémoire). Cruciale pour la compatibilité entre bibliothèques et exécutables compilés séparément.

### **ACL**
**Access Control List**
Liste de permissions définissant qui peut accéder à quelles ressources et de quelle manière. Utilisée pour les fichiers, réseaux, APIs.

### **AIO**
**Asynchronous Input/Output**
Mécanisme d'entrées/sorties asynchrones permettant à un programme de continuer son exécution pendant qu'une opération I/O se termine en arrière-plan.

### **API**
**Application Programming Interface**
Ensemble de fonctions, protocoles et outils permettant à des applications de communiquer entre elles. En DevOps, souvent des APIs REST ou GraphQL pour l'automatisation.

### **APT**
**Advanced Package Tool**
Gestionnaire de paquets pour Debian et Ubuntu. Commandes : `apt install`, `apt update`, `apt upgrade`.

### **ARM**
**Advanced RISC Machine / Acorn RISC Machine**
Architecture de processeurs largement utilisée dans l'embarqué, les smartphones et de plus en plus dans les serveurs (AWS Graviton, Apple Silicon).

### **ARN**
**Amazon Resource Name**
Identifiant unique pour les ressources AWS (EC2, S3, Lambda, etc.).

### **ASan**
**AddressSanitizer**
Outil de détection d'erreurs mémoire (buffer overflows, use-after-free). Activé avec `-fsanitize=address` dans GCC/Clang.

### **ASCII**
**American Standard Code for Information Interchange**
Standard d'encodage de caractères sur 7 bits (128 caractères). Base de l'encodage de texte.

### **AWS**
**Amazon Web Services**
Plateforme de cloud computing leader, offrant services d'infrastructure (EC2, S3, RDS, Lambda, etc.).

---

## B

### **BPF**
**Berkeley Packet Filter**
Technologie permettant d'exécuter du code dans le noyau Linux de manière sécurisée. **eBPF** (extended BPF) est la version moderne utilisée pour l'observabilité, le réseau et la sécurité.

### **BSD**
**Berkeley Software Distribution**
Famille de systèmes Unix dont certains concepts sont repris dans Linux (sockets BSD, utilitaires).

---

## C

### **CA**
**Certificate Authority**
Autorité de certification émettant des certificats numériques pour sécuriser les communications (HTTPS, SSH).

### **CAS**
**Compare-And-Swap**
Opération atomique fondamentale pour la programmation concurrente lock-free. Permet de modifier une valeur uniquement si elle n'a pas été modifiée par un autre thread.

### **CD**
**Continuous Delivery / Continuous Deployment**
- **Continuous Delivery** : Pratique où le code est toujours dans un état déployable
- **Continuous Deployment** : Extension où chaque modification validée est automatiquement déployée en production

### **CFS**
**Completely Fair Scheduler**
Ordonnanceur de processus historique du noyau Linux visant à répartir équitablement le temps CPU entre les tâches. Remplacé par **EEVDF** (Earliest Eligible Virtual Deadline First) depuis Linux 6.6.

### **CGI**
**Common Gateway Interface**
Standard permettant aux serveurs web d'exécuter des programmes externes et de renvoyer leur sortie au client.

### **CI**
**Continuous Integration**
Pratique DevOps consistant à intégrer fréquemment le code dans un dépôt partagé, avec tests automatiques à chaque commit.

### **CI/CD**
**Continuous Integration / Continuous Delivery/Deployment**
Pipeline automatisé allant du commit de code jusqu'au déploiement en production, incluant build, tests et validation.

### **CLI**
**Command Line Interface**
Interface en ligne de commande permettant d'interagir avec un programme via du texte.

### **CNCF**
**Cloud Native Computing Foundation**
Fondation hébergeant des projets open-source cloud-native (Kubernetes, Prometheus, Envoy, etc.).

### **COW**
**Copy-On-Write**
Technique d'optimisation où la copie de données n'est effectuée que lors d'une modification. Utilisée par `fork()` sous Linux pour la duplication de processus.

### **CPU**
**Central Processing Unit**
Processeur principal exécutant les instructions.

### **CRUD**
**Create, Read, Update, Delete**
Quatre opérations de base de la gestion de données (bases de données, APIs REST).

### **CVE**
**Common Vulnerabilities and Exposures**
Système d'identification standardisé des vulnérabilités de sécurité (ex: CVE-2024-1234).

---

## D

### **DAG**
**Directed Acyclic Graph**
Graphe orienté sans cycle. Utilisé dans les systèmes de build (dépendances), les pipelines CI/CD, et Git.

### **DB**
**Database**
Base de données.

### **DDoS**
**Distributed Denial of Service**
Attaque visant à rendre un service indisponible en le saturant de requêtes depuis de multiples sources.

### **DLL**
**Dynamic Link Library**
Bibliothèque dynamique sous Windows. Équivalent Linux : `.so` (Shared Object).

### **DMA**
**Direct Memory Access**
Mécanisme permettant aux périphériques d'accéder directement à la RAM sans passer par le CPU.

### **DNS**
**Domain Name System**
Système de résolution de noms de domaine en adresses IP (`google.com` → `142.250.180.46`).

### **DoD**
**Definition of Done**
Critères définissant quand une tâche/story est considérée comme complète (tests passés, code reviewé, documenté, etc.).

### **DoS**
**Denial of Service**
Attaque visant à rendre un service indisponible.

### **DRY**
**Don't Repeat Yourself**
Principe de programmation : éviter la duplication de code, extraire et réutiliser.

### **DSL**
**Domain-Specific Language**
Langage dédié à un domaine particulier (ex: SQL pour bases de données, Makefile pour build).

### **DTO**
**Data Transfer Object**
Objet simple utilisé pour transférer des données entre couches/services.

---

## E

### **ECS**
**Elastic Container Service**
Service AWS d'orchestration de conteneurs Docker.

### **EKS**
**Elastic Kubernetes Service**
Service AWS de Kubernetes managé.

### **ELF**
**Executable and Linkable Format**
Format standard des binaires sous Linux (exécutables, bibliothèques, objets).

### **EOF**
**End Of File**
Marqueur de fin de fichier. En C, retourné par les fonctions de lecture quand le fichier est entièrement lu.

### **EOL**
**End Of Life**
Date après laquelle un logiciel/système n'est plus maintenu (plus de mises à jour de sécurité).

---

## F

### **FAQ**
**Frequently Asked Questions**
Foire aux questions.

### **FFI**
**Foreign Function Interface**
Mécanisme permettant d'appeler des fonctions écrites dans un langage depuis un autre (ex: appeler du C depuis Python/Rust).

### **FIFO**
**First In, First Out**
1. Structure de données : file d'attente
2. Named pipe sous Linux : mécanisme IPC

### **FOSS**
**Free and Open-Source Software**
Logiciel libre et open-source.

### **FTP**
**File Transfer Protocol**
Protocole de transfert de fichiers (souvent remplacé par SFTP sécurisé).

---

## G

### **GCC**
**GNU Compiler Collection**
Suite de compilateurs open-source incluant un compilateur C. Standard sous Linux.

### **GDB**
**GNU Debugger**
Débogueur en ligne de commande standard pour C/C++ sous Linux.

### **GID**
**Group ID**
Identifiant numérique d'un groupe d'utilisateurs sous Unix/Linux.

### **Git**
**Global Information Tracker** *(nom récursif humoristique)*
Système de contrôle de version distribué, standard de l'industrie.

### **GNU**
**GNU's Not Unix** *(acronyme récursif)*
Projet de système d'exploitation libre créant les outils utilisés avec Linux (gcc, bash, coreutils, etc.).

### **GPL**
**GNU General Public License**
Licence libre copyleft utilisée par Linux et de nombreux logiciels GNU.

### **GPG / GnuPG**
**GNU Privacy Guard**
Implémentation libre du standard OpenPGP pour le chiffrement et la signature numérique.

### **GPU**
**Graphics Processing Unit**
Processeur spécialisé pour le calcul parallèle, utilisé pour le graphisme et de plus en plus pour le calcul scientifique/ML.

### **GRUB**
**GRand Unified Bootloader**
Bootloader standard de la plupart des distributions Linux.

### **GUI**
**Graphical User Interface**
Interface graphique (fenêtres, boutons, menus). Opposé de CLI.

---

## H

### **HA**
**High Availability**
Haute disponibilité : système conçu pour minimiser les temps d'arrêt (objectifs : 99.9%, 99.99%, 99.999%).

### **HDD**
**Hard Disk Drive**
Disque dur mécanique (plateaux rotatifs). Remplacé progressivement par les SSD.

### **HTML**
**HyperText Markup Language**
Langage de balisage pour les pages web.

### **HTTP**
**HyperText Transfer Protocol**
Protocole de communication du web. Version sécurisée : **HTTPS** (HTTP Secure).

### **HTTPS**
**HyperText Transfer Protocol Secure**
HTTP avec chiffrement TLS/SSL.

---

## I

### **IaaS**
**Infrastructure as a Service**
Modèle cloud fournissant des ressources d'infrastructure virtualisées (serveurs, stockage, réseau). Ex: AWS EC2, Azure VMs.

### **IAM**
**Identity and Access Management**
Système de gestion des identités et des accès (utilisateurs, permissions, rôles).

### **ICMP**
**Internet Control Message Protocol**
Protocole réseau pour les messages de diagnostic (utilisé par `ping`, `traceroute`).

### **IDE**
**Integrated Development Environment**
Environnement de développement intégré (éditeur + débogueur + outils). Ex: VS Code, CLion, Eclipse.

### **IOPS**
**Input/Output Operations Per Second**
Métrique de performance de stockage : nombre d'opérations I/O par seconde.

### **IoT**
**Internet of Things**
Internet des objets : réseau d'objets physiques connectés.

### **IP**
**Internet Protocol**
Protocole d'adressage et de routage des paquets sur Internet. Versions : IPv4 (32 bits), IPv6 (128 bits).

### **IPC**
**Inter-Process Communication**
Communication entre processus (pipes, sockets, shared memory, message queues, signaux).

### **IPv4**
**Internet Protocol version 4**
Adresses IP 32 bits (format : `192.168.1.1`). 4.3 milliards d'adresses possibles.

### **IPv6**
**Internet Protocol version 6**
Adresses IP 128 bits (format : `2001:0db8::1`). 340 undécillions d'adresses.

### **ISO**
**International Organization for Standardization / ISO image**
1. Organisation de normalisation internationale
2. Format d'image disque pour installer des OS

### **IT**
**Information Technology**
Technologies de l'information.

---

## J

### **JAR**
**Java Archive**
Format d'archive pour applications Java.

### **JIT**
**Just-In-Time**
Compilation à la volée pendant l'exécution. Utilisée en eBPF et dans les machines virtuelles (JVM, V8).

### **JSON**
**JavaScript Object Notation**
Format d'échange de données léger et lisible. Standard des APIs modernes.
```json
{
  "nom": "Alice",
  "age": 30,
  "actif": true
}
```

### **JWT**
**JSON Web Token**
Standard pour créer des tokens d'accès sécurisés pour l'authentification d'APIs.

---

## K

### **K8s**
**Kubernetes** *(K + 8 lettres + s)*
Plateforme d'orchestration de conteneurs open-source. Standard de l'industrie pour déployer et gérer des applications conteneurisées.

### **KPI**
**Key Performance Indicator**
Indicateur clé de performance pour mesurer l'atteinte d'objectifs.

### **KVM**
**Kernel-based Virtual Machine**
Solution de virtualisation intégrée au noyau Linux.

---

## L

### **LAN**
**Local Area Network**
Réseau local (typiquement dans un bâtiment ou campus).

### **LDAP**
**Lightweight Directory Access Protocol**
Protocole d'accès à des annuaires (gestion d'utilisateurs, authentification centralisée).

### **LIFO**
**Last In, First Out**
Structure de données : pile (stack). Le dernier élément ajouté est le premier retiré.

### **LLVM**
**Low Level Virtual Machine** *(nom historique, désormais juste LLVM)*
Infrastructure de compilateur modulaire. Clang est le compilateur C/C++ basé sur LLVM.

### **LRU**
**Least Recently Used**
Algorithme de remplacement de cache : supprime l'élément le moins récemment utilisé.

### **LSan**
**LeakSanitizer**
Détecteur de fuites mémoire. Intégré dans ASan ou utilisable seul.

### **LTO**
**Link-Time Optimization**
Optimisation effectuée au moment de l'édition de liens plutôt que lors de la compilation des fichiers individuels. Permet des optimisations inter-modules.

### **LTS**
**Long Term Support**
Version d'un logiciel maintenue sur une longue durée (5-10 ans). Ex: Ubuntu 22.04 LTS, Linux kernel LTS.

---

## M

### **MAC**
**Media Access Control / Mandatory Access Control**
1. **Media Access Control** : Adresse matérielle unique d'une carte réseau (ex: `00:1A:2B:3C:4D:5E`)
2. **Mandatory Access Control** : Système de contrôle d'accès obligatoire (SELinux, AppArmor)

### **MIT**
**Massachusetts Institute of Technology**
Université américaine ayant donné son nom à la populaire **licence MIT** (permissive, open-source).

### **MMU**
**Memory Management Unit**
Composant matériel du CPU gérant la mémoire virtuelle (traduction adresses virtuelles → physiques, protection mémoire).

### **MR**
**Merge Request**
Demande de fusion de code (équivalent GitLab de la Pull Request GitHub).

### **MSan**
**MemorySanitizer**
Détecteur de lecture de mémoire non initialisée.

### **MTBF**
**Mean Time Between Failures**
Temps moyen entre pannes. Métrique de fiabilité.

### **MTTR**
**Mean Time To Repair/Recovery**
Temps moyen de réparation/récupération après une panne.

### **MVC**
**Model-View-Controller**
Pattern architectural séparant données (Model), interface (View) et logique (Controller).

---

## N

### **NAT**
**Network Address Translation**
Traduction d'adresses réseau permettant à plusieurs machines d'un réseau privé de partager une IP publique.

### **NFS**
**Network File System**
Protocole de partage de fichiers en réseau.

### **NGINX**
**Engine X**
Serveur web et reverse proxy haute performance.

### **NIC**
**Network Interface Card**
Carte réseau.

### **NTP**
**Network Time Protocol**
Protocole de synchronisation de l'heure via le réseau.

---

## O

### **OCI**
**Open Container Initiative**
Standard pour les formats de conteneurs et runtimes (Docker, containerd, podman).

### **OOM**
**Out Of Memory**
Situation où le système manque de mémoire. Le noyau peut alors tuer des processus via l'**OOM Killer**.

### **OOP**
**Object-Oriented Programming**
Programmation orientée objet.

### **OS**
**Operating System**
Système d'exploitation (Linux, Windows, macOS, etc.).

### **OSI**
**Open Systems Interconnection**
Modèle en 7 couches pour les communications réseau (Physique, Liaison, Réseau, Transport, Session, Présentation, Application).

### **OSS**
**Open-Source Software**
Logiciel dont le code source est publiquement accessible.

---

## P

### **PaaS**
**Platform as a Service**
Modèle cloud fournissant une plateforme pour développer et déployer des applications (Heroku, Google App Engine).

### **PGO**
**Profile-Guided Optimization**
Optimisation guidée par profil : le compilateur utilise des données d'exécution réelle pour optimiser le code.

### **PIC**
**Position-Independent Code**
Code relocatable pouvant s'exécuter quelle que soit son adresse en mémoire. Requis pour les bibliothèques partagées (`.so`). Compilé avec `-fPIC`.

### **PID**
**Process ID**
Identifiant numérique unique d'un processus sous Unix/Linux.

### **PKI**
**Public Key Infrastructure**
Infrastructure à clés publiques pour la gestion de certificats et le chiffrement asymétrique.

### **POC**
**Proof of Concept**
Preuve de concept : prototype démontrant la faisabilité d'une idée.

### **POSIX**
**Portable Operating System Interface**
Famille de standards définissant l'API des systèmes Unix-like.

### **PR**
**Pull Request**
Demande d'intégration de code dans un dépôt Git (GitHub, Bitbucket). Inclut généralement code review.

### **PTY**
**Pseudo-Terminal**
Terminal virtuel utilisé par les émulateurs de terminal et SSH.

---

## Q

### **QA**
**Quality Assurance**
Assurance qualité : processus garantissant la qualité d'un produit logiciel.

### **QoS**
**Quality of Service**
Garantie de qualité de service réseau (bande passante, latence, priorité des flux).

---

## R

### **RAID**
**Redundant Array of Independent Disks**
Technique combinant plusieurs disques pour performance et/ou redondance.

### **RAM**
**Random Access Memory**
Mémoire vive volatile.

### **RBAC**
**Role-Based Access Control**
Contrôle d'accès basé sur les rôles (utilisateurs assignés à des rôles, rôles ayant des permissions).

### **README**
**Read Me** *(littéralement "lis-moi")*
Fichier documentation décrivant un projet (installation, utilisation, contribution).

### **REST**
**Representational State Transfer**
Style d'architecture pour APIs web utilisant HTTP (GET, POST, PUT, DELETE).

### **RFC**
**Request for Comments**
Document de spécification des standards Internet (ex: RFC 793 définit TCP).

### **RHEL**
**Red Hat Enterprise Linux**
Distribution Linux commerciale pour entreprises.

### **RISC**
**Reduced Instruction Set Computer**
Architecture de processeur avec jeu d'instructions réduit (ARM, RISC-V). Opposé : CISC (x86).

### **ROM**
**Read-Only Memory**
Mémoire non-volatile en lecture seule.

### **RPM**
**RPM Package Manager** *(récursif : "RPM Package Manager")*
Gestionnaire de paquets pour Red Hat, Fedora, CentOS (`.rpm`).

### **RPS / RPC**
**Requests Per Second / Remote Procedure Call**
1. **RPS** : Requêtes par seconde (métrique de charge)
2. **RPC** : Appel de procédure distant (ex: gRPC)

### **RSA**
**Rivest-Shamir-Adleman**
Algorithme de chiffrement asymétrique largement utilisé (clés publiques/privées).

### **RSS**
**Resident Set Size**
Quantité de mémoire RAM effectivement utilisée par un processus (excluant le swap).

### **RTT**
**Round-Trip Time**
Temps aller-retour d'un paquet réseau.

---

## S

### **SaaS**
**Software as a Service**
Modèle cloud où les applications sont fournies via Internet (Gmail, Salesforce, Slack).

### **SBOM**
**Software Bill of Materials**
Liste exhaustive de tous les composants d'un logiciel (dépendances, versions, licences). Crucial pour la sécurité.

### **SCM**
**Source Control Management / Software Configuration Management**
Gestion de version du code source (Git, SVN).

### **SDK**
**Software Development Kit**
Kit de développement logiciel : ensemble d'outils pour développer sur une plateforme.

### **SEGV**
**Segmentation Violation**
Signal envoyé lors d'une erreur d'accès mémoire invalide (SIGSEGV = segmentation fault).

### **SELinux**
**Security-Enhanced Linux**
Système de contrôle d'accès obligatoire (MAC) pour Linux, développé par la NSA.

### **SFTP**
**SSH File Transfer Protocol**
Protocole de transfert de fichiers sécurisé sur SSH.

### **SIGSEGV**
**Signal Segmentation Violation**
Signal Unix indiquant une segmentation fault (accès mémoire invalide). Numéro : 11.

### **SIMD**
**Single Instruction, Multiple Data**
Technique de parallélisme traitant plusieurs données avec une seule instruction (vectorisation). Extensions CPU : SSE, AVX.

### **SLA**
**Service Level Agreement**
Accord contractuel sur le niveau de service (uptime, temps de réponse, support).

### **SLI**
**Service Level Indicator**
Métrique quantitative d'un aspect du service (latence, disponibilité, taux d'erreur).

### **SLO**
**Service Level Objective**
Objectif de niveau de service (ex: 99.9% uptime, 95% des requêtes < 200ms).

### **SMB**
**Server Message Block**
Protocole de partage de fichiers Windows (aussi appelé CIFS).

### **SMTP**
**Simple Mail Transfer Protocol**
Protocole d'envoi d'emails.

### **SOLID**
**Single responsibility, Open-closed, Liskov substitution, Interface segregation, Dependency inversion**
Cinq principes de conception orientée objet.

### **SonarQube**
**Source Analysis Quality**
Plateforme d'analyse de qualité de code.

### **SONAME**
**Shared Object Name**
Nom interne d'une bibliothèque partagée Linux utilisé pour la gestion de versions (ex: `libfoo.so.1`).

### **SQL**
**Structured Query Language**
Langage de requête pour bases de données relationnelles.

### **SRE**
**Site Reliability Engineering**
Discipline appliquant les principes d'ingénierie logicielle aux opérations IT (Google).

### **SSH**
**Secure Shell**
Protocole de connexion sécurisée à distance (remplace telnet).

### **SSD**
**Solid-State Drive**
Disque à mémoire flash (pas de parties mécaniques). Beaucoup plus rapide que les HDD.

### **SSL**
**Secure Sockets Layer**
Protocole de sécurisation des communications réseau. Remplacé par TLS mais le terme SSL reste utilisé.

### **stdout / stderr / stdin**
**Standard Output / Standard Error / Standard Input**
Flux standards Unix : sortie standard (1), sortie d'erreur (2), entrée standard (0).

---

## T

### **TAR**
**Tape Archive**
Format d'archive regroupant plusieurs fichiers. Souvent combiné avec compression (`.tar.gz`, `.tar.bz2`).

### **TCP**
**Transmission Control Protocol**
Protocole de transport fiable, orienté connexion, avec garantie d'ordre (HTTP, SSH, FTP utilisent TCP).

### **TDD**
**Test-Driven Development**
Développement dirigé par les tests : écrire les tests avant le code.

### **TLS**
**Transport Layer Security**
Protocole de chiffrement des communications réseau (successeur de SSL).

### **TOTP**
**Time-based One-Time Password**
Mot de passe à usage unique basé sur le temps (authentification à deux facteurs).

### **TSan**
**ThreadSanitizer**
Détecteur de race conditions et problèmes de concurrence dans les programmes multithreadés.

### **TTL**
**Time To Live**
1. Durée de vie d'une donnée en cache
2. Nombre de sauts réseau qu'un paquet peut effectuer avant d'être éliminé

### **TTY**
**TeleTYpewriter**
Terminal sous Unix/Linux.

---

## U

### **UB / UBSan**
**Undefined Behavior / UndefinedBehaviorSanitizer**
1. **UB** : Comportement indéfini du langage C
2. **UBSan** : Détecteur de comportements indéfinis (`-fsanitize=undefined`)

### **UDP**
**User Datagram Protocol**
Protocole de transport non fiable, sans connexion, faible latence (streaming, DNS, gaming).

### **UID**
**User ID**
Identifiant numérique d'un utilisateur sous Unix/Linux. Root = 0.

### **UI / UX**
**User Interface / User Experience**
Interface utilisateur / Expérience utilisateur.

### **URI / URL / URN**
**Uniform Resource Identifier / Locator / Name**
- **URI** : Identifiant générique de ressource
- **URL** : Localisateur (adresse complète, ex: `https://example.com/page`)
- **URN** : Nom (identifiant persistant indépendant de la localisation)

### **UTC**
**Coordinated Universal Time**
Temps universel coordonné (référence temporelle mondiale).

### **UTF-8**
**Unicode Transformation Format - 8 bits**
Encodage de caractères Unicode dominant. Compatible ASCII. Taille variable (1-4 octets).

### **UUID**
**Universally Unique Identifier**
Identifiant unique universel (128 bits, ex: `550e8400-e29b-41d4-a716-446655440000`).

---

## V

### **VCS**
**Version Control System**
Système de gestion de versions (Git, SVN, Mercurial).

### **VFS**
**Virtual File System**
Couche d'abstraction du noyau Linux pour supporter différents systèmes de fichiers.

### **VM**
**Virtual Machine**
Machine virtuelle : émulation logicielle d'un ordinateur.

### **VPN**
**Virtual Private Network**
Réseau privé virtuel sécurisé sur Internet.

### **VRAM**
**Video RAM**
Mémoire dédiée à la carte graphique.

---

## W

### **WAN**
**Wide Area Network**
Réseau étendu (Internet, liaisons inter-sites d'entreprise).

### **WAR**
**Web Application Archive**
Format d'archive pour applications web Java.

### **WASM**
**WebAssembly**
Format binaire portable pour exécuter du code dans les navigateurs web (alternative performante à JavaScript).

### **WYSIWYG**
**What You See Is What You Get**
Ce que vous voyez est ce que vous obtenez (éditeurs visuels).

---

## X

### **X11 / X Window System**
**X version 11**
Système de fenêtrage historique pour Unix/Linux. Remplacé progressivement par Wayland.

### **XaaS**
**X as a Service**
Modèle générique "X en tant que service" (IaaS, PaaS, SaaS, etc.).

### **XML**
**eXtensible Markup Language**
Langage de balisage extensible pour structurer des données. Remplacé progressivement par JSON.

### **XSS**
**Cross-Site Scripting**
Vulnérabilité web permettant l'injection de scripts malveillants.

---

## Y

### **YAML**
**YAML Ain't Markup Language** *(acronyme récursif)*
Format de configuration et sérialisation de données lisible par l'humain. Utilisé par Kubernetes, Docker Compose, CI/CD.
```yaml
services:
  web:
    image: nginx:latest
    ports:
      - "80:80"
```

### **YAGNI**
**You Aren't Gonna Need It**
Principe : n'implémentez pas des fonctionnalités dont vous n'avez pas besoin maintenant.

---

## Z

### **ZFS**
**Zettabyte File System**
Système de fichiers avancé combinant gestionnaire de volumes et filesystem (snapshots, compression, RAID intégré).

### **Zsh**
**Z Shell**
Shell Unix avancé et populaire, alternative à Bash (framework Oh My Zsh).

---

## 📊 Catégories principales

### **🔧 Outils de build et compilation**
- **GCC**, **LLVM**, **CMake**, **Make**, **Ninja**
- **LTO**, **PGO**, **PIC**
- **ELF**, **SONAME**

### **🧪 Tests et Qualité**
- **CI/CD**, **TDD**
- **ASan**, **UBSan**, **TSan**, **LSan**, **MSan**
- **SonarQube**, **Code Coverage**

### **☁️ Cloud et Infrastructure**
- **AWS**, **IaaS**, **PaaS**, **SaaS**
- **Docker**, **K8s**, **ECS**, **EKS**
- **CNCF**, **OCI**

### **🌐 Réseau**
- **TCP**, **UDP**, **HTTP/HTTPS**, **DNS**, **SSH**
- **API**, **REST**, **gRPC**, **JSON**
- **IP**, **IPv4**, **IPv6**, **NAT**

### **🔒 Sécurité**
- **TLS/SSL**, **PKI**, **RSA**, **JWT**
- **CVE**, **XSS**, **SELinux**
- **RBAC**, **IAM**, **ACL**

### **⚙️ Système Linux**
- **POSIX**, **GNU**, **ELF**, **VFS**
- **IPC**, **PID**, **UID**, **GID**
- **systemd**, **cgroups**, **namespaces**

### **📈 Monitoring et SRE**
- **SLA**, **SLO**, **SLI**, **SRE**
- **MTBF**, **MTTR**, **KPI**
- **RPS**, **IOPS**, **RTT**

---

## 💡 Conseils d'utilisation

Ce glossaire d'acronymes est conçu pour :

1. **Décrypter la documentation technique** : Comprendre rapidement les termes DevOps
2. **Communiquer efficacement** : Utiliser le vocabulaire standard de l'industrie
3. **Préparer des entretiens** : Maîtriser les acronymes essentiels
4. **Veille technologique** : Suivre les discussions sur les nouvelles technologies

### **Comment mémoriser efficacement**

- **Par catégorie** : Regroupez les acronymes par domaine (réseau, build, cloud)
- **Par usage** : Identifiez ceux que vous utilisez quotidiennement vs rarement
- **En contexte** : Apprenez-les en les rencontrant dans des projets réels
- **Progressive** : Commencez par les essentiels (CI/CD, API, REST, TCP, SSH, Git)

### **Les 20 acronymes DevOps essentiels à connaître absolument**

Pour débuter en DevOps avec C sur Linux, maîtrisez en priorité :

1. **CI/CD** - L'essence du DevOps
2. **API** - Communication entre services
3. **REST** - Architecture d'APIs moderne
4. **HTTP/HTTPS** - Protocole web
5. **SSH** - Accès sécurisé distant
6. **Git** - Contrôle de version
7. **JSON** - Format d'échange de données
8. **TCP/UDP** - Protocoles de transport
9. **DNS** - Résolution de noms
10. **VM** - Virtualisation
11. **Docker** - Conteneurisation
12. **K8s** - Orchestration de conteneurs
13. **AWS/GCP/Azure** - Plateformes cloud
14. **TLS/SSL** - Chiffrement réseau
15. **SLA/SLO** - Garanties de service
16. **ASan/Valgrind** - Débogage mémoire
17. **GCC/LLVM** - Compilation
18. **POSIX** - Standards Unix
19. **ELF** - Format binaire Linux
20. **PID/UID** - Identifiants système

---

## 🔗 Glossaires complémentaires

Pour une compréhension complète :

- **D.1 Terminologie C** : Concepts du langage C
- **D.2 Terminologie système Linux** : Concepts système Unix/Linux
- **D.4 Termes eBPF** : Terminologie spécifique à eBPF

---

## 📚 Ressources pour approfondir

- **DevOps Roadmap** : [roadmap.sh/devops](https://roadmap.sh/devops)
- **RFC Editor** : Standards Internet officiels
- **CNCF Landscape** : Cartographie de l'écosystème cloud-native
- **The Phoenix Project** : Livre de référence DevOps
- **Google SRE Book** : [sre.google](https://sre.google/books/)

---

*Ce glossaire est maintenu à jour avec les standards et pratiques DevOps 2025.*

⏭️ [Termes eBPF](/annexes/glossaire/04-termes-ebpf.md)
