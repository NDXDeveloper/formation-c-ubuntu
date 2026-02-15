🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 21.4 Toolchain eBPF

## Introduction

Maintenant que vous comprenez **ce qu'est eBPF** (section 21.1-21.2) et **à quoi il sert** (section 21.3), il est temps de découvrir **comment développer avec eBPF**. C'est là qu'intervient la **toolchain eBPF** : l'ensemble des outils, bibliothèques et utilitaires qui vous permettent de transformer votre code C en programmes eBPF fonctionnels.

Si eBPF était une voiture de course, la toolchain serait l'atelier avec tous les outils nécessaires pour la construire, la régler, et la maintenir. Vous ne pouvez pas construire la voiture avec vos mains nues — vous avez besoin des bons outils.

---

## Qu'est-ce qu'une toolchain ?

### Définition simple

Une **toolchain** (chaîne d'outils) est un ensemble coordonné d'outils logiciels utilisés pour développer, compiler, tester et déployer des applications. Dans le contexte eBPF, la toolchain comprend :

1. **Compilateurs** : Transformer le code C en bytecode eBPF
2. **Bibliothèques** : Simplifier le développement et le chargement des programmes
3. **Utilitaires** : Inspecter, debugger, et gérer les programmes eBPF
4. **Générateurs** : Créer du code automatiquement (skeletons, headers)

### Analogie avec le développement C classique

Pour comprendre la toolchain eBPF, comparons avec le développement C traditionnel :

**Développement C classique :**
```
┌────────────────────────────────────────────────────┐
│  Code source (hello.c)                             │
└────────────────┬───────────────────────────────────┘
                 │
                 ▼
┌────────────────────────────────────────────────────┐
│  Compilateur : GCC / Clang                         │
│  ├─ Préprocesseur                                  │
│  ├─ Compilation                                    │
│  ├─ Assemblage                                     │
│  └─ Linkage                                        │
└────────────────┬───────────────────────────────────┘
                 │
                 ▼
┌────────────────────────────────────────────────────┐
│  Binaire exécutable (./hello)                      │
└────────────────────────────────────────────────────┘
```

**Développement eBPF :**
```
┌────────────────────────────────────────────────────┐
│  Code kernel (program.bpf.c)                       │
└────────────────┬───────────────────────────────────┘
                 │
                 ▼
┌────────────────────────────────────────────────────┐
│  Compilateur : Clang avec backend BPF              │
│  └─ Sortie : Bytecode eBPF (program.bpf.o)         │
└────────────────┬───────────────────────────────────┘
                 │
                 ▼
┌────────────────────────────────────────────────────┐
│  Code userspace (loader.c)                         │
│  ├─ Utilise libbpf pour charger le bytecode        │
│  └─ Interagit avec les BPF maps                    │
└────────────────┬───────────────────────────────────┘
                 │
                 ▼
┌────────────────────────────────────────────────────┐
│  Chargement dans le kernel Linux                   │
│  ├─ Vérification (verifier)                        │
│  ├─ JIT compilation                                │
│  └─ Attachement au hook                            │
└────────────────────────────────────────────────────┘
```

**Différence clé :** En eBPF, il y a **deux programmes** distincts :
1. Le code **kernel** (qui s'exécute dans le kernel)
2. Le code **userspace** (qui charge et gère le programme kernel)

---

## L'écosystème de la toolchain eBPF

### Vue d'ensemble des composants

```
┌─────────────────────────────────────────────────────────────┐
│                    DÉVELOPPEMENT                            │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │   Éditeur    │  │   Headers    │  │ Générateurs  │       │
│  │              │  │              │  │              │       │
│  │  VS Code     │  │ vmlinux.h    │  │ bpftool gen  │       │
│  │  Vim/Neovim  │  │ linux/bpf.h  │  │ skeleton     │       │
│  │  CLion       │  │ bpf/bpf.h    │  │              │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    COMPILATION                              │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────────────────────────────────────┐           │
│  │           Clang/LLVM avec backend BPF        │           │
│  │  ├─ Clang : Compilateur C                    │           │
│  │  ├─ LLVM : Infrastructure de compilation     │           │
│  │  ├─ Backend BPF : Génération bytecode eBPF   │           │
│  │  └─ BTF : BPF Type Format                    │           │
│  └──────────────────────────────────────────────┘           │
│                                                             │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    CHARGEMENT & GESTION                     │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │   libbpf     │  │   bpftool    │  │  bpftrace    │       │
│  │              │  │              │  │              │       │
│  │ Bibliothèque │  │ CLI pour     │  │ Langage de   │       │
│  │ C pour       │  │ inspecter    │  │ scripting    │       │
│  │ charger eBPF │  │ et gérer     │  │ haut niveau  │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
                            │
                            ▼
┌─────────────────────────────────────────────────────────────┐
│                    DEBUGGING & ANALYSE                      │
├─────────────────────────────────────────────────────────────┤
│                                                             │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐       │
│  │  bpf_printk  │  │   Verifier   │  │   perf       │       │
│  │              │  │     logs     │  │              │       │
│  │ Logging      │  │              │  │ Profiling    │       │
│  │ kernel       │  │ Validation   │  │ système      │       │
│  └──────────────┘  └──────────────┘  └──────────────┘       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

### Les trois piliers de la toolchain

La toolchain eBPF repose sur trois composants essentiels que nous allons explorer dans les sections suivantes :

#### 1. **libbpf** (Section 21.4.1)

**Qu'est-ce que c'est ?**
- Bibliothèque C officielle pour développer avec eBPF
- Permet de charger, gérer, et interagir avec les programmes eBPF
- Implémente CO-RE (Compile Once - Run Everywhere)

**Pourquoi c'est important ?**
- Abstrait la complexité des syscalls eBPF
- Rend le code portable entre versions kernel
- Standard de facto pour le développement eBPF moderne

**Cas d'usage :**
- Développer des outils eBPF personnalisés
- Créer des applications production-ready
- Base de Cilium, Falco, et la plupart des outils eBPF

#### 2. **bpftool** (Section 21.4.2)

**Qu'est-ce que c'est ?**
- Utilitaire en ligne de commande pour gérer eBPF
- Inclus dans le kernel Linux (tools/bpf/bpftool)
- Le "couteau suisse" de l'inspection eBPF

**Pourquoi c'est important ?**
- Inspecter les programmes et maps actifs
- Debugger les problèmes de chargement
- Générer du code (skeletons, vmlinux.h)

**Cas d'usage :**
- Monitoring des programmes eBPF en production
- Debugging lors du développement
- Analyse forensique de sécurité

#### 3. **Compilation et Chargement** (Section 21.4.3)

**Qu'est-ce que c'est ?**
- Le processus complet : code C → bytecode → kernel
- Clang/LLVM pour la compilation
- Verifier pour la validation
- JIT pour l'exécution native

**Pourquoi c'est important ?**
- Comprendre le workflow de développement
- Optimiser les performances
- Debugger les erreurs de compilation/chargement

**Cas d'usage :**
- Mettre en place un build system
- Résoudre les erreurs du verifier
- Optimiser la taille et performance du code

---

## Évolution de la toolchain eBPF

### Première génération (2014-2016) : Syscalls bruts

**Approche :** Écrire directement dans le kernel via syscalls.

```c
// Code extrêmement verbeux
struct bpf_insn prog[] = {
    BPF_MOV64_IMM(BPF_REG_0, 0),
    BPF_EXIT_INSN(),
};

union bpf_attr attr = {
    .prog_type = BPF_PROG_TYPE_SOCKET_FILTER,
    .insns = ptr_to_u64(prog),
    .insn_cnt = 2,
    .license = ptr_to_u64("GPL"),
};

int fd = syscall(__NR_bpf, BPF_PROG_LOAD, &attr, sizeof(attr));
```

**Problèmes :**
- ❌ Extrêmement difficile à écrire et maintenir
- ❌ Sujet aux erreurs
- ❌ Pas portable
- ❌ Presque personne ne l'utilisait

**Usage aujourd'hui :** Obsolète, uniquement dans le kernel interne.

### Deuxième génération (2016-2020) : BCC

**Approche :** Python + C inline, compilation à la volée.

```python
from bcc import BPF

prog = """  
int hello(void *ctx) {  
    bpf_trace_printk("Hello!\\n");
    return 0;
}
"""

b = BPF(text=prog)  
b.attach_kprobe(event="sys_clone", fn_name="hello")  
b.trace_print()  
```

**Avantages :**
- ✅ Très facile à utiliser
- ✅ Parfait pour prototypage rapide
- ✅ Riche écosystème d'outils (70+ outils prêts à l'emploi)

**Problèmes :**
- ❌ Dépendances lourdes (LLVM/Clang runtime)
- ❌ Démarrage lent (compilation à chaque lancement)
- ❌ Consommation mémoire élevée
- ❌ Difficile à distribuer (binaires standalone impossibles)

**Usage aujourd'hui :** Prototypage, scripts de debugging, apprentissage.

### Troisième génération (2020-aujourd'hui) : libbpf + CO-RE

**Approche :** Compilation ahead-of-time, binaires portables.

```c
// program.bpf.c
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>

SEC("kprobe/sys_clone")  
int hello(struct pt_regs *ctx) {  
    bpf_printk("Hello!\n");
    return 0;
}

char LICENSE[] SEC("license") = "GPL";
```

```c
// loader.c
#include <bpf/libbpf.h>
#include "program.skel.h"

int main() {
    struct program_bpf *skel = program_bpf__open_and_load();
    program_bpf__attach(skel);
    pause();
    program_bpf__destroy(skel);
    return 0;
}
```

**Avantages :**
- ✅ Binaires standalone
- ✅ Portabilité entre kernels (CO-RE)
- ✅ Démarrage instantané
- ✅ Léger (pas de dépendance LLVM runtime)
- ✅ Performance optimale

**Usage aujourd'hui :** **Standard pour la production**, tous les outils modernes (Cilium, Falco, Tetragon).

---

## Comparaison des approches de développement

### Tableau comparatif

| Aspect | Syscalls bruts | BCC (Python) | libbpf + CO-RE |
|--------|----------------|--------------|----------------|
| **Facilité** | ⭐☆☆☆☆ | ⭐⭐⭐⭐⭐ | ⭐⭐⭐☆☆ |
| **Courbe apprentissage** | Très difficile | Facile | Moyenne |
| **Performance** | Excellente | Bonne | Excellente |
| **Démarrage** | Instantané | 1-5 secondes | < 50 ms |
| **Portabilité** | Aucune | Limitée | Excellente (CO-RE) |
| **Distribution** | Impossible | Difficile | Facile |
| **Dépendances** | Aucune | LLVM/Clang | libbpf uniquement |
| **Taille binaire** | N/A | Script + LLVM | 1-5 MB |
| **Usage production** | ❌ | ⚠️ | ✅ |
| **Prototypage** | ❌ | ✅ | ⚠️ |
| **Quand utiliser** | Jamais | Scripts, debug | Applications prod |

### Recommandations par cas d'usage

**Pour apprendre eBPF :**
```
Commencez par : BCC (Python)  
Raison : Courbe d'apprentissage douce, résultats immédiats  

Exemple : Utiliser execsnoop-bpfcc pour voir les processus
```

**Pour prototyper rapidement :**
```
Utilisez : bpftrace  
Raison : One-liners puissants, syntaxe simple  

Exemple :
$ bpftrace -e 'kprobe:sys_open { printf("open: %s\n", str(arg1)); }'
```

**Pour développer un outil production :**
```
Utilisez : libbpf + CO-RE  
Raison : Performance, portabilité, distribution  

Exemple : Cilium, Falco, outils système
```

**Pour inspecter/debugger :**
```
Utilisez : bpftool  
Raison : Outil CLI standard, inclus partout  

Exemple :
$ sudo bpftool prog list
$ sudo bpftool map dump id 5
```

---

## Installation de la toolchain complète

### Ubuntu/Debian

```bash
# Compilateurs
sudo apt update  
sudo apt install -y \  
    clang \
    llvm \
    gcc

# libbpf et headers
sudo apt install -y \
    libbpf-dev \
    linux-headers-$(uname -r) \
    libelf-dev

# bpftool
sudo apt install -y \
    linux-tools-common \
    linux-tools-generic \
    linux-tools-$(uname -r)

# BCC (optionnel, pour prototypage)
sudo apt install -y \
    bpfcc-tools \
    python3-bpfcc

# bpftrace (optionnel)
sudo apt install -y bpftrace
```

### Fedora/RHEL

```bash
# Compilateurs
sudo dnf install -y \
    clang \
    llvm \
    gcc

# libbpf et headers
sudo dnf install -y \
    libbpf-devel \
    kernel-devel \
    elfutils-libelf-devel

# bpftool
sudo dnf install -y bpftool

# BCC (optionnel)
sudo dnf install -y \
    bcc-tools \
    python3-bcc

# bpftrace (optionnel)
sudo dnf install -y bpftrace
```

### Vérification de l'installation

```bash
# Vérifier clang avec support BPF
clang --version  
llc --version | grep bpf  

# Vérifier libbpf
pkg-config --modversion libbpf

# Vérifier bpftool
bpftool version

# Vérifier le kernel support
ls /sys/kernel/btf/vmlinux  # BTF requis pour CO-RE

# Tester avec un simple programme
echo '
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
SEC("kprobe/sys_clone")  
int test(void *ctx) { return 0; }  
char LICENSE[] SEC("license") = "GPL";  
' > test.bpf.c

clang -O2 -target bpf -c test.bpf.c -o test.bpf.o  
rm test.bpf.c test.bpf.o  
echo "✅ Toolchain OK!"  
```

---

## Workflow de développement moderne

### 1. Setup initial

```bash
# Créer la structure du projet
mkdir my-ebpf-tool && cd my-ebpf-tool  
mkdir src include  

# Générer vmlinux.h (une fois)
bpftool btf dump file /sys/kernel/btf/vmlinux format c > include/vmlinux.h
```

### 2. Développement

```bash
# Écrire le code kernel
vim src/program.bpf.c

# Écrire le code userspace
vim src/loader.c

# Créer un Makefile
vim Makefile
```

### 3. Build

```bash
# Compiler le programme eBPF
clang -O2 -g -target bpf -Iinclude -c src/program.bpf.c -o program.bpf.o

# Générer le skeleton
bpftool gen skeleton program.bpf.o > program.skel.h

# Compiler le loader
gcc -Wall -O2 src/loader.c -o loader -lbpf -lelf -lz
```

### 4. Test

```bash
# Tester le chargement
sudo ./loader

# Inspecter avec bpftool
sudo bpftool prog list  
sudo bpftool map list  
```

### 5. Debug

```bash
# Si erreur au chargement
sudo bpftool --debug prog load program.bpf.o /sys/fs/bpf/test

# Voir les logs kernel
sudo cat /sys/kernel/debug/tracing/trace_pipe
```

---

## Ressources de la toolchain

### Documentation officielle

| Composant | Documentation | URL |
|-----------|--------------|-----|
| **libbpf** | API docs | https://libbpf.readthedocs.io/ |
| **bpftool** | Man pages | `man bpftool`, `man bpftool-prog` |
| **BCC** | Tutorial | https://github.com/iovisor/bcc/blob/master/docs/tutorial.md |
| **bpftrace** | Reference | https://github.com/iovisor/bpftrace/blob/master/docs/reference_guide.md |
| **Kernel** | BPF docs | https://www.kernel.org/doc/html/latest/bpf/ |

### Projets exemples

**libbpf-bootstrap :**
```bash
git clone https://github.com/libbpf/libbpf-bootstrap  
cd libbpf-bootstrap/examples/c  
make  
```

**BCC tools :**
```bash
# Voir les outils disponibles
ls /usr/share/bcc/tools/

# Exemples
sudo execsnoop-bpfcc      # Tracer les executions  
sudo opensnoop-bpfcc      # Tracer les open()  
sudo biolatency-bpfcc     # Latence I/O disque  
```

### Communautés

- **eBPF Slack** : https://ebpf.io/slack
- **eBPF Summit** : Conférence annuelle
- **Linux Plumbers Conference** : Track eBPF
- **GitHub Discussions** : libbpf, bpftool, BCC

---

## Prérequis techniques

### Version kernel minimale

| Feature | Kernel minimum | Recommandé |
|---------|---------------|------------|
| **eBPF de base** | 3.18+ | 5.4+ |
| **BTF (CO-RE)** | 5.2+ | 5.10+ |
| **libbpf moderne** | 5.4+ | 5.15+ |
| **Features complètes** | 5.10+ | 6.1+ |

**Vérifier votre kernel :**
```bash
uname -r
# Si < 5.4 → Envisager une mise à jour pour CO-RE
```

### Permissions

**En développement :**
- Besoin de `root` ou `sudo`

**En production (kernel 5.8+) :**
- `CAP_BPF` : Charger des programmes eBPF
- `CAP_PERFMON` : Attacher à des tracepoints
- `CAP_NET_ADMIN` : Programmes réseau (XDP, TC)

```bash
# Donner les capabilities (au lieu de root)
sudo setcap cap_bpf,cap_perfmon,cap_net_admin+ep ./loader
```

### Configuration kernel requise

```bash
# Vérifier les options kernel nécessaires
grep CONFIG_BPF /boot/config-$(uname -r)

# Requis :
CONFIG_BPF=y  
CONFIG_BPF_SYSCALL=y  
CONFIG_BPF_JIT=y  

# Recommandé :
CONFIG_DEBUG_INFO_BTF=y  # Pour CO-RE  
CONFIG_BPF_LSM=y         # Pour sécurité  
```

---

## Structure des sections suivantes

Dans les trois sous-sections à venir, nous allons explorer en profondeur chaque composant de la toolchain :

### **21.4.1 libbpf**

Nous verrons :
- L'architecture d'une application libbpf
- Les concepts clés : bpf_object, bpf_program, bpf_link, bpf_map
- CO-RE (Compile Once - Run Everywhere) expliqué en détail
- Skeleton generation
- Exemple complet : créer un outil de tracing

**Ce que vous apprendrez :**
- Développer des programmes eBPF portables
- Utiliser libbpf pour charger et gérer les programmes
- Comprendre comment fonctionne CO-RE

### **21.4.2 bpftool**

Nous verrons :
- Toutes les commandes essentielles (prog, map, link, btf, gen)
- Inspecter les programmes et maps actifs
- Debugger les problèmes de chargement
- Générer du code (skeletons, vmlinux.h)
- Cas d'usage pratiques en production

**Ce que vous apprendrez :**
- Inspecter et gérer eBPF en ligne de commande
- Debugger efficacement les programmes eBPF
- Utiliser bpftool dans vos workflows

### **21.4.3 Compilation et Chargement**

Nous verrons :
- Le processus complet : C → bytecode → kernel
- Options de compilation Clang/LLVM
- Le verifier eBPF : ce qu'il vérifie et comment
- JIT compilation
- Workflow complet avec Makefile
- Résoudre les erreurs courantes

**Ce que vous apprendrez :**
- Compiler correctement des programmes eBPF
- Comprendre les erreurs du verifier
- Optimiser pour la performance et la taille
- Mettre en place un build system moderne

---

## Conclusion de l'introduction

La **toolchain eBPF** est votre boîte à outils pour développer avec eBPF de manière professionnelle. Les trois composants essentiels — **libbpf**, **bpftool**, et le **processus de compilation** — forment un écosystème complet qui vous permet de :

- ✅ **Développer** des programmes eBPF portables (libbpf + CO-RE)
- ✅ **Inspecter** et debugger en temps réel (bpftool)
- ✅ **Compiler** et optimiser efficacement (Clang/LLVM)
- ✅ **Déployer** des binaires standalone en production

Bien que la courbe d'apprentissage puisse sembler raide au début, maîtriser ces outils est **essentiel** pour travailler sérieusement avec eBPF. Une fois ces fondations acquises, vous serez capable de :

- Créer vos propres outils d'observabilité
- Contribuer à des projets open-source (Cilium, Falco)
- Comprendre comment fonctionnent les outils eBPF sous le capot
- Résoudre les problèmes de performance en production
- Implémenter des solutions de sécurité avancées

**L'approche moderne** (libbpf + CO-RE) est maintenant le standard de l'industrie, utilisée par tous les projets sérieux. C'est celle que nous allons explorer dans les sections suivantes.

**Prêt à plonger dans la toolchain ?** Commençons par libbpf (section 21.4.1), la bibliothèque qui alimente la révolution eBPF moderne.

---

**💡 À retenir :**
- La toolchain eBPF comprend : compilateurs (Clang/LLVM), bibliothèques (libbpf), utilitaires (bpftool)
- Trois générations : syscalls bruts → BCC → libbpf + CO-RE
- libbpf + CO-RE est le standard moderne pour la production
- BCC reste excellent pour le prototypage et l'apprentissage
- bpftool est le couteau suisse pour inspecter et debugger
- Kernel 5.4+ recommandé pour profiter de toutes les features

---

⏭️ [libbpf](/21-introduction-ebpf/04.1-libbpf.md)
