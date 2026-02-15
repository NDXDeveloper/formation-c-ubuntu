🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 21.5 Premier programme eBPF

## Introduction

Après avoir découvert les concepts théoriques d'eBPF dans les sections précédentes (architecture, verifier, BPF maps, toolchain), il est temps de passer à la pratique ! Dans cette section, nous allons construire nos premiers programmes eBPF fonctionnels, étape par étape.

Cette section est divisée en trois parties progressives qui vous guideront de votre premier "Hello World" jusqu'à la maîtrise des différents points d'accroche dans le kernel Linux.

## Objectifs de cette section

À la fin de cette section, vous serez capable de :

- ✅ Écrire, compiler et exécuter un programme eBPF complet
- ✅ Comprendre l'architecture kernel/userspace des programmes eBPF
- ✅ Utiliser les BPF maps pour communiquer entre kernel et userspace
- ✅ Attacher des programmes à différents hooks kernel (tracepoints, kprobes, etc.)
- ✅ Déboguer vos programmes eBPF
- ✅ Construire des outils d'observabilité simples mais fonctionnels

## Vue d'ensemble des sous-sections

### 21.5.1 - Hello World : tracer les appels système

Dans cette première partie, nous créerons un programme eBPF minimal qui trace les appels système `execve` (exécution de programmes). C'est le "Hello World" du monde eBPF.

**Ce que vous apprendrez :**
- Structure d'un programme eBPF (partie kernel + partie userspace)
- Utilisation des helpers eBPF de base
- Compilation avec clang et bpftool
- Utilisation de `bpf_printk()` pour le débogage
- Chargement et attachement d'un programme au kernel

**Résultat attendu :** Un programme qui affiche en temps réel les noms des programmes lancés sur votre système.

### 21.5.2 - Utilisation de maps

Dans la deuxième partie, nous allons améliorer notre programme en utilisant des **BPF maps** - le mécanisme de communication principal entre le code kernel et userspace.

**Ce que vous apprendrez :**
- Les différents types de BPF maps (hash, array, ringbuf, etc.)
- Comment créer et manipuler des maps
- Communication bidirectionnelle kernel ↔ userspace
- Agrégation et comptage de données
- Configuration dynamique via les maps

**Résultat attendu :** Un compteur d'exécutions par utilisateur qui affiche des statistiques en temps réel.

### 21.5.3 - Attacher à des hooks kernel

Dans la troisième partie, nous explorerons les différents types de **points d'accroche** (hooks) disponibles dans le kernel Linux.

**Ce que vous apprendrez :**
- Tracepoints : Points d'instrumentation stables
- Kprobes/Kretprobes : Instrumenter n'importe quelle fonction kernel
- Fentry/Fexit : Alternative moderne aux kprobes
- Uprobes : Tracer des programmes userspace
- USDT : Tracepoints dans les applications
- Hooks spécialisés : XDP (réseau), LSM (sécurité)

**Résultat attendu :** Comprendre quand utiliser chaque type de hook et savoir instrumenter précisément le système.

## Prérequis techniques

Avant de commencer, assurez-vous d'avoir un environnement Ubuntu fonctionnel avec les outils nécessaires installés.

### Système requis

- **OS** : Ubuntu 20.04 LTS ou plus récent (ou toute distribution avec kernel >= 5.4)
- **Kernel Linux** : >= 5.4 (idéalement >= 5.8 pour toutes les fonctionnalités)
- **Privilèges** : Accès root (via `sudo`)
- **Espace disque** : ~500 MB pour les outils et dépendances

### Vérifier votre version de kernel

```bash
uname -r
```

Vous devriez voir quelque chose comme `5.15.0-91-generic` ou plus récent.

**Note :** eBPF existe depuis Linux 3.18, mais les fonctionnalités modernes nécessitent un kernel récent. Avec un kernel >= 5.4, vous aurez accès à toutes les fonctionnalités que nous utiliserons dans ce tutoriel.

### Installation des outils eBPF

Installez la toolchain complète :

```bash
# Mettre à jour les paquets
sudo apt update

# Installer les compilateurs et outils
sudo apt install -y \
    clang \
    llvm \
    gcc \
    make

# Installer les bibliothèques eBPF
sudo apt install -y \
    libbpf-dev \
    libelf-dev \
    linux-headers-$(uname -r)

# Installer bpftool (outil de debug et génération de skeleton)
sudo apt install -y linux-tools-common linux-tools-generic
# OU si disponible :
sudo apt install -y bpftool

# Vérifier les installations
clang --version  
llvm-objdump --version  
bpftool version  
```

**Si bpftool n'est pas disponible via apt**, compilez-le depuis les sources :

```bash
# Cloner les sources du kernel
git clone --depth 1 https://github.com/torvalds/linux.git  
cd linux/tools/bpf/bpftool  
make  
sudo make install  
```

### Vérifier que tout fonctionne

```bash
# Vérifier que le système de fichiers BPF est monté
mount | grep bpf

# Devrait afficher :
# bpf on /sys/fs/bpf type bpf (rw,nosuid,nodev,noexec,relatime,mode=700)

# Vérifier l'accès aux tracepoints
ls /sys/kernel/debug/tracing/events/ 2>/dev/null

# Si vous obtenez "Permission denied", montez debugfs :
sudo mount -t debugfs none /sys/kernel/debug
```

### Créer un répertoire de travail

```bash
mkdir -p ~/ebpf-tutorial  
cd ~/ebpf-tutorial  
```

Vous êtes maintenant prêt à commencer !

## Architecture d'un programme eBPF : Récapitulatif

Avant de plonger dans le code, rappelons l'architecture générale d'un programme eBPF que nous avons vue dans les sections théoriques précédentes.

### Les deux composants

Un programme eBPF complet se compose de **deux parties** qui communiquent entre elles :

```
┌────────────────────────────────────────────────────────┐
│                  ESPACE UTILISATEUR                    │
│                                                        │
│  ┌──────────────────────────────────────────┐          │
│  │  Programme Userspace (C)                 │          │
│  │                                          │          │
│  │  • Charge le programme eBPF              │          │
│  │  • Configure les maps                    │          │
│  │  • Lit les données collectées            │          │
│  │  • Affiche les résultats                 │          │
│  └───────────────┬──────────────────────────┘          │
│                  │                                     │
│                  │ libbpf (bibliothèque)               │
│                  │ • bpf() syscall                     │
│                  │ • File descriptors                  │
│                  │                                     │
└──────────────────┼─────────────────────────────────────┘
                   │
        ═══════════╪═══════════ Frontière kernel/user
                   │
┌──────────────────┼────────────────────────────────────┐
│                  ↓          NOYAU LINUX               │
│                                                       │
│  ┌──────────────────────────────────────────┐         │
│  │  Programme eBPF (Bytecode)               │         │
│  │                                          │         │
│  │  • S'exécute dans le kernel              │         │
│  │  • Intercepte des événements             │         │
│  │  • Collecte des données                  │         │
│  │  • Écrit dans les BPF maps               │         │
│  └──────────────────────────────────────────┘         │
│                                                       │
│  ┌──────────────────────────────────────────┐         │
│  │  BPF Maps (Structures de données)        │         │
│  │                                          │         │
│  │  Clé   │  Valeur                         │         │
│  │  ──────┼─────────                        │         │
│  │  1000  │  42                             │         │
│  │  1001  │  15                             │         │
│  └──────────────────────────────────────────┘         │
│                                                       │
│  ┌──────────────────────────────────────────┐         │
│  │  Hooks / Points d'attachement            │         │
│  │                                          │         │
│  │  • Tracepoints (appels système, etc.)    │         │
│  │  • Kprobes (fonctions kernel)            │         │
│  │  • Sockets (XDP, TC)                     │         │
│  │  • LSM (sécurité)                        │         │
│  └──────────────────────────────────────────┘         │
└───────────────────────────────────────────────────────┘
```

### Le workflow de développement

Voici les étapes typiques pour créer un programme eBPF :

```
1. ÉCRITURE DU CODE
   ├─ prog.bpf.c     (Code eBPF qui s'exécute dans le kernel)
   └─ prog.c         (Code userspace qui charge et gère le programme)

2. COMPILATION
   ├─ clang -target bpf -c prog.bpf.c → prog.bpf.o (Bytecode eBPF)
   └─ bpftool gen skeleton prog.bpf.o → prog.skel.h (Squelette C)

3. COMPILATION USERSPACE
   └─ gcc prog.c -lbpf -o prog (Binaire final)

4. EXÉCUTION
   └─ sudo ./prog

5. LE PROGRAMME EST ACTIF !
   ├─ Code eBPF chargé dans le kernel
   ├─ Attaché au hook spécifié
   ├─ Collecte des données
   └─ Communique via les maps
```

### Pourquoi deux parties ?

Cette séparation est **fondamentale** pour plusieurs raisons :

**Sécurité** : Le code kernel eBPF est vérifié par le verifier et ne peut faire que des opérations sûres. Le code userspace, lui, a tous les droits mais ne s'exécute pas dans le kernel.

**Performance** : Le code kernel est ultra-optimisé et s'exécute en contexte kernel (pas de context switch). Les traitements lourds se font en userspace.

**Flexibilité** : Vous pouvez changer la logique userspace (affichage, agrégation, export) sans recharger le code kernel.

**Portabilité** : Le bytecode eBPF est portable entre différentes architectures (x86, ARM, etc.).

## Concepts clés à retenir

Avant de passer aux sous-sections pratiques, gardez en tête ces principes fondamentaux :

### 1. Le verifier est votre ami (et votre ennemi)

Le **BPF verifier** analyse votre code eBPF avant de le charger dans le kernel. Il vérifie :

- ✅ Pas de boucles infinies
- ✅ Accès mémoire sécurisés
- ✅ Taille du programme respectée
- ✅ Pas d'appels de fonctions non autorisées
- ✅ Tous les chemins d'exécution se terminent

Si votre code ne passe pas le verifier, il sera rejeté. Les messages d'erreur peuvent être cryptiques au début, mais vous apprendrez à les déchiffrer.

### 2. Les helpers eBPF sont vos seuls outils

Dans le code kernel eBPF, vous ne pouvez pas utiliser les fonctions standard de la libc (`printf`, `malloc`, `strcmp`, etc.). Vous devez utiliser les **BPF helpers** fournis par le kernel :

- `bpf_printk()` : Afficher un message de debug
- `bpf_get_current_pid_tgid()` : Obtenir le PID
- `bpf_get_current_uid_gid()` : Obtenir l'UID
- `bpf_get_current_comm()` : Obtenir le nom du processus
- `bpf_map_lookup_elem()` : Chercher dans une map
- `bpf_map_update_elem()` : Mettre à jour une map
- `bpf_ktime_get_ns()` : Obtenir un timestamp
- `bpf_probe_read()` : Lire de la mémoire en toute sécurité
- Et bien d'autres...

La liste complète est dans `man 7 bpf-helpers` ou sur https://man7.org/linux/man-pages/man7/bpf-helpers.7.html

### 3. Les BPF maps sont votre canal de communication

Les **BPF maps** sont le pont entre le kernel et userspace :

- Le code kernel **écrit** des données dans les maps
- Le code userspace **lit** ces données
- Le code userspace peut aussi **écrire** des données de configuration
- Les maps sont typées et performantes

Sans les maps, vous ne pourriez que déboguer avec `bpf_printk()` (ce que nous ferons dans 21.5.1), mais pas construire d'outils réels.

### 4. Le code eBPF doit être simple et rapide

Le code qui s'exécute dans le kernel doit être :

- **Rapide** : Il s'exécute potentiellement des millions de fois par seconde
- **Sûr** : Pas de crashs, pas de boucles infinies
- **Déterministe** : Pas d'attente, pas de sleeps
- **Limité en taille** : Historiquement 4096 instructions, aujourd'hui beaucoup plus mais toujours limité

La règle d'or : **Collectez dans le kernel, traitez en userspace**.

### 5. Privilèges root requis

Charger des programmes eBPF nécessite des **privilèges élevés** car vous interagissez avec le kernel. Vous devrez utiliser `sudo` pour exécuter vos programmes.

Depuis Linux 5.8, il existe `CAP_BPF` qui permet des privilèges plus granulaires, mais pour débuter, nous utiliserons simplement `sudo`.

## Outils de debugging

Pendant votre apprentissage, ces outils seront essentiels :

### 1. bpftool - L'outil tout-en-un

```bash
# Lister les programmes chargés
sudo bpftool prog list

# Voir les détails d'un programme
sudo bpftool prog show id 123

# Lister les maps
sudo bpftool map list

# Voir le contenu d'une map
sudo bpftool map dump id 456

# Voir le bytecode d'un programme
sudo bpftool prog dump xlated id 123
```

### 2. trace_pipe - Voir les bpf_printk()

Les messages de `bpf_printk()` apparaissent dans un fichier spécial :

```bash
sudo cat /sys/kernel/debug/tracing/trace_pipe
```

**Astuce :** Ouvrez ce fichier dans un terminal séparé pendant vos tests !

### 3. bpftrace - Script rapide

`bpftrace` est un langage de script pour eBPF (nous ne l'utiliserons pas dans ce tutoriel car nous faisons du C, mais c'est bon à connaître) :

```bash
sudo apt install -y bpftrace

# One-liner pour tracer execve
sudo bpftrace -e 'tracepoint:syscalls:sys_enter_execve { printf("%s called execve\n", comm); }'
```

### 4. Vérifier les erreurs de compilation

Si votre programme eBPF ne compile pas ou ne se charge pas :

```bash
# Compiler avec verbose pour voir les erreurs détaillées
clang -g -O2 -target bpf -D__TARGET_ARCH_x86 -c prog.bpf.c -o prog.bpf.o

# Charger manuellement pour voir les erreurs du verifier
sudo bpftool prog load prog.bpf.o /sys/fs/bpf/myprog
```

## Conventions de nommage dans ce tutoriel

Pour maintenir la cohérence, nous suivrons ces conventions :

### Fichiers

- `progname.bpf.c` : Code eBPF (kernel)
- `progname.c` : Code userspace
- `progname.bpf.o` : Bytecode eBPF compilé
- `progname.skel.h` : Squelette généré par bpftool
- `progname` : Binaire final exécutable

### Variables

- `ctx` : Contexte du hook (paramètre d'entrée)
- `pid` : Process ID
- `uid` : User ID
- `ts` : Timestamp
- `comm` : Nom de la commande (16 caractères max)
- `skel` : Instance du squelette dans le code userspace

### Sections (SEC macro)

- `SEC("tp/...")` : Tracepoint
- `SEC("kprobe/...")` : Kprobe
- `SEC("kretprobe/...")` : Kretprobe
- `SEC(".maps")` : Définition de map

## Structure de fichiers recommandée

Pour chaque programme, organisez vos fichiers ainsi :

```
ebpf-tutorial/
├── 01-hello-world/
│   ├── hello.bpf.c        # Code eBPF
│   ├── hello.c            # Code userspace
│   ├── Makefile           # Automatisation
│   └── README.md          # Notes sur le programme
├── 02-execcount/
│   ├── execcount.bpf.c
│   ├── execcount.c
│   └── Makefile
└── 03-opensnoop/
    ├── opensnoop.bpf.c
    ├── opensnoop.c
    └── Makefile
```

## Cycle de développement typique

Voici comment vous allez itérer pendant le développement :

```
1. Écrire le code eBPF (prog.bpf.c)
   ↓
2. Compiler
   └─ make
   ↓
3. Exécuter
   └─ sudo ./prog
   ↓
4. Observer les résultats
   ├─ Terminal principal : sortie du programme
   └─ Terminal secondaire : sudo cat /sys/kernel/debug/tracing/trace_pipe
   ↓
5. Déboguer si nécessaire
   ├─ Erreurs de compilation → Vérifier la syntaxe
   ├─ Erreurs du verifier → Simplifier le code
   └─ Pas de données → Vérifier les filtres et la logique
   ↓
6. Itérer
   └─ Modifier le code et recommencer à l'étape 2
```

**Conseil** : Commencez toujours simple ! Ajoutez de la complexité progressivement après avoir vérifié que la base fonctionne.

## Ressources pour cette section

Pendant que vous progressez dans les sous-sections, gardez ces références à portée de main :

### Documentation

- **Man pages** : `man 2 bpf`, `man 7 bpf-helpers`
- **Kernel docs** : https://www.kernel.org/doc/html/latest/bpf/
- **libbpf docs** : https://github.com/libbpf/libbpf

### Exemples

- **libbpf-bootstrap** : https://github.com/libbpf/libbpf-bootstrap
- **BCC tools** : https://github.com/iovisor/bcc/tree/master/tools
- **bpftrace examples** : https://github.com/iovisor/bpftrace/tree/master/tools

### Communauté

- **eBPF.io** : https://ebpf.io (site officiel)
- **Cilium Slack** : https://cilium.io/slack (communauté active)
- **Mailing list eBPF** : bpf@vger.kernel.org

## Conseils avant de commencer

### ✅ À faire

1. **Ouvrez deux terminaux** : Un pour exécuter le programme, un pour `trace_pipe`
2. **Lisez les erreurs du verifier** : Elles sont détaillées (même si cryptiques au début)
3. **Testez en isolation** : Commencez par des filtres stricts (votre UID uniquement)
4. **Commentez votre code** : Vous vous remercierez plus tard
5. **Sauvegardez vos programmes** : Créez un répertoire Git local

### ❌ À éviter

1. **Ne testez pas en production** : Utilisez une VM ou un conteneur
2. **N'ignorez pas les warnings** : Compilez avec `-Wall`
3. **Ne copiez-collez pas sans comprendre** : Lisez et expérimentez
4. **N'oubliez pas sudo** : Les programmes eBPF nécessitent root
5. **Ne vous découragez pas** : Les messages d'erreur sont difficiles au début, c'est normal !

## Progression recommandée

Cette section 21.5 est conçue pour être suivie **dans l'ordre** :

1. **21.5.1 - Hello World** : Commencez ici obligatoirement
   - C'est votre premier contact avec le code eBPF
   - Vous apprendrez la structure de base
   - Vous verrez votre premier programme fonctionner

2. **21.5.2 - Utilisation de maps** : Ensuite, apprenez les maps
   - Indispensable pour tout programme réel
   - Vous comprendrez la communication kernel/userspace
   - Vous construirez un outil d'agrégation

3. **21.5.3 - Attacher à des hooks kernel** : Enfin, explorez les hooks
   - Vue d'ensemble complète des possibilités
   - Vous saurez choisir le bon hook pour chaque besoin
   - Vous aurez la palette complète des outils eBPF

**Durée estimée** : 2-4 heures pour l'ensemble de la section 21.5 (en prenant le temps d'expérimenter).

## Dépannage courant

### Problème : "Permission denied"

```bash
# Solution
sudo ./prog
```

### Problème : "Failed to load program: Invalid argument"

Le verifier a rejeté votre code. Voir les logs détaillés :

```bash
sudo dmesg | tail -50
```

### Problème : "bpftool not found"

```bash
sudo apt install linux-tools-common linux-tools-$(uname -r)
```

### Problème : Aucune sortie dans trace_pipe

Vérifiez que :
1. Le programme est bien lancé (`sudo`)
2. Vous lisez le bon fichier : `sudo cat /sys/kernel/debug/tracing/trace_pipe`
3. Des événements sont bien générés (lancez des commandes dans un autre terminal)

## Prêt à commencer ?

Vous avez maintenant toutes les bases nécessaires pour démarrer votre voyage dans la programmation eBPF !

Dans les sous-sections qui suivent, nous allons mettre en pratique tous ces concepts. Chaque sous-section s'appuie sur la précédente, alors suivez-les dans l'ordre.

**Rappel des sous-sections :**

1. **21.5.1** - Hello World : tracer les appels système *(votre premier programme eBPF)*
2. **21.5.2** - Utilisation de maps *(communication kernel/userspace)*
3. **21.5.3** - Attacher à des hooks kernel *(explorer tous les types de hooks)*

---

🚀 **Rendez-vous dans la section 21.5.1 pour écrire votre premier programme eBPF !**

---

*Note : Si vous rencontrez des problèmes d'installation ou de configuration, consultez la section 21.4 (Toolchain eBPF) qui détaille l'installation et la vérification des outils.*

⏭️ [Hello World : tracer les appels système](/21-introduction-ebpf/05.1-hello-world-tracing.md)
