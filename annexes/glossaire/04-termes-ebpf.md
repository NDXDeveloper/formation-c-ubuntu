🔝 Retour au [Sommaire](/SOMMAIRE.md)

# D.4 Termes eBPF

## Glossaire des termes essentiels d'eBPF (extended Berkeley Packet Filter)

Ce glossaire rassemble les termes fondamentaux relatifs à eBPF, une technologie Linux révolutionnaire permettant d'exécuter du code en toute sécurité dans le noyau Linux. eBPF est devenu essentiel pour l'observabilité moderne, le réseau et la sécurité des systèmes Linux.

---

## 🎯 Introduction rapide à eBPF

**eBPF (extended Berkeley Packet Filter)** est une technologie du noyau Linux permettant d'exécuter des programmes en mode sécurisé (sandboxed) directement dans le noyau, sans avoir à modifier le code du noyau ou charger des modules noyau traditionnels.

**Cas d'usage principaux :**
- 🔍 **Observabilité** : Tracing système, monitoring de performance, profiling
- 🌐 **Réseau** : Filtrage de paquets avancé, load balancing, accélération réseau
- 🔒 **Sécurité** : Détection d'intrusions, contrôle d'accès, audit système
- ⚡ **Performance** : Analyse des goulots d'étranglement, optimisation

---

## A

### **Attach point / Attachment**
Point d'accrochage où un programme eBPF est rattaché au noyau. Détermine quand le programme eBPF sera exécuté. Exemples : appel système, fonction du noyau, événement réseau, point de tracing.

### **Attach type**
Type d'attachement définissant comment et où un programme eBPF se connecte. Exemples :
- `BPF_PROG_TYPE_KPROBE` : Attache à une fonction du noyau
- `BPF_PROG_TYPE_SOCKET_FILTER` : Attache à un socket réseau
- `BPF_PROG_TYPE_TRACEPOINT` : Attache à un tracepoint statique

---

## B

### **BCC (BPF Compiler Collection)**
Framework facilitant l'écriture de programmes eBPF en Python ou Lua, avec compilation à la volée du code eBPF (écrit en C restreint). Développé par IO Visor Project.
```python
# Exemple BCC simple
from bcc import BPF
prog = """
int hello(void *ctx) {
    bpf_trace_printk("Hello, eBPF!\\n");
    return 0;
}
"""
b = BPF(text=prog)
```

### **Berkeley Packet Filter (BPF classique)**
Technologie originale (années 1990) permettant de filtrer les paquets réseau dans le noyau. eBPF en est l'extension moderne beaucoup plus puissante et généraliste.

### **BPF bytecode**
Langage d'assemblage virtuel pour eBPF. Les programmes eBPF sont compilés en bytecode eBPF (instructions 64 bits) avant d'être chargés dans le noyau.

### **BPF CO-RE (Compile Once – Run Everywhere)**
Technologie permettant de compiler un programme eBPF une seule fois et de l'exécuter sur différentes versions du noyau Linux sans recompilation. Résout le problème de portabilité entre versions du noyau. Utilise BTF et libbpf.

### **BPF filesystem (bpffs)**
Système de fichiers virtuel (généralement monté sur `/sys/fs/bpf/`) permettant de rendre persistants les maps et programmes eBPF au-delà de la durée de vie du processus qui les a créés.
```bash
mount -t bpf bpffs /sys/fs/bpf/
```

### **BPF helper functions**
Fonctions fournies par le noyau que les programmes eBPF peuvent appeler pour interagir avec le système (lecture/écriture de maps, obtention du temps, envoi de logs, etc.). Exemples : `bpf_map_lookup_elem()`, `bpf_trace_printk()`, `bpf_get_current_pid_tgid()`.

### **BPF maps**
Structures de données permettant le partage d'informations entre programmes eBPF et entre l'espace noyau et l'espace utilisateur. Différents types : hash, array, perf event array, etc. Voir **Maps**.

### **BPF program**
Programme écrit en C restreint (subset du C), compilé en bytecode eBPF, vérifié, et chargé dans le noyau pour exécution.

### **BPF syscall**
Appel système `bpf()` permettant de charger des programmes eBPF, créer des maps, attacher des programmes à des hooks, et gérer les objets eBPF depuis l'espace utilisateur.

### **BPF Type Format**
Voir **BTF**.

### **bpf()**
Appel système Linux (`sys_bpf`) servant d'interface unique pour toutes les opérations eBPF : chargement de programmes, création de maps, attachement, etc.
```c
int bpf(int cmd, union bpf_attr *attr, unsigned int size);
```

### **bpftool**
Outil en ligne de commande pour inspecter et manipuler les objets eBPF (programmes, maps, liens). Indispensable pour le débogage et l'exploration.
```bash
bpftool prog list        # Liste les programmes eBPF chargés
bpftool map dump id 42   # Affiche le contenu d'une map
bpftool prog load obj.o /sys/fs/bpf/mon_prog
```

### **BTF (BPF Type Format)**
Format de métadonnées décrivant les types de données (structures, enums, etc.) utilisés dans les programmes eBPF et le noyau. Permet BPF CO-RE et améliore le débogage. Généré par le compilateur et stocké dans les binaires ELF.

### **BTF-enabled kernel**
Noyau Linux compilé avec le support BTF (option `CONFIG_DEBUG_INFO_BTF`). Requis pour BPF CO-RE et les fonctionnalités modernes d'eBPF. Disponible dans les noyaux récents (5.2+).

---

## C

### **Cilium**
Projet open-source CNCF utilisant eBPF pour fournir du networking, de la sécurité et de l'observabilité dans Kubernetes. Remplace iptables par eBPF pour des performances supérieures.

### **Clang**
Compilateur basé sur LLVM utilisé pour compiler les programmes eBPF (code C vers bytecode eBPF). Supporte les attributs et restrictions spécifiques à eBPF.
```bash
clang -O2 -target bpf -c program.bpf.c -o program.bpf.o
```

### **Context (ctx)**
Paramètre passé à un programme eBPF lors de son exécution. Contient des informations spécifiques au type de programme (pointeur vers `sk_buff` pour le réseau, `pt_regs` pour les kprobes, etc.).
```c
SEC("kprobe/sys_execve")
int my_kprobe(struct pt_regs *ctx) {
    // ctx contient l'état des registres CPU
}
```

### **CO-RE**
Voir **BPF CO-RE**.

---

## E

### **eBPF (extended Berkeley Packet Filter)**
Extension moderne et généraliste de BPF, permettant d'exécuter des programmes sécurisés dans le noyau Linux pour l'observabilité, le réseau, et la sécurité.

### **eBPF program types**
Catégories de programmes eBPF définissant où et comment ils s'exécutent :
- **Socket filters** : Filtrage de paquets réseau
- **Kprobes/Kretprobes** : Instrumentation dynamique du noyau
- **Tracepoints** : Points de traçage statiques
- **XDP** : Traitement de paquets ultra-rapide
- **Cgroup** : Contrôle et monitoring de cgroups
- Et bien d'autres...

### **Event**
Occurrence déclenchant l'exécution d'un programme eBPF (appel système, arrivée d'un paquet, appel de fonction noyau, etc.).

---

## H

### **Helper function**
Voir **BPF helper functions**.

### **Hook**
Point d'entrée dans le noyau où un programme eBPF peut être attaché. Les hooks déterminent quand et dans quel contexte le programme eBPF s'exécute.

---

## J

### **JIT (Just-In-Time compilation)**
Compilation à la volée du bytecode eBPF en code machine natif du processeur (x86_64, ARM64, etc.). Améliore considérablement les performances par rapport à l'interprétation. Activé par défaut dans les noyaux modernes.
```bash
# Vérifier si le JIT est activé
sysctl net.core.bpf_jit_enable
# Activer le JIT
sysctl -w net.core.bpf_jit_enable=1
```

---

## K

### **Katran**
Load balancer Layer 4 haute performance développé par Facebook, utilisant XDP et eBPF.

### **Kernel space**
Espace d'exécution privilégié du noyau Linux. Les programmes eBPF s'exécutent dans le kernel space mais de manière sécurisée (grâce au verifier).

### **Kprobe (Kernel probe)**
Point d'instrumentation dynamique permettant d'attacher un programme eBPF à l'entrée de (presque) n'importe quelle fonction du noyau Linux.
```c
SEC("kprobe/do_sys_open")
int trace_open(struct pt_regs *ctx) {
    char filename[256];
    bpf_probe_read_user_str(filename, sizeof(filename), (void *)PT_REGS_PARM2(ctx));
    bpf_trace_printk("Opening: %s\n", filename);
    return 0;
}
```

### **Kretprobe (Kernel return probe)**
Point d'instrumentation dynamique s'attachant à la sortie (return) d'une fonction du noyau. Permet de capturer la valeur de retour.
```c
SEC("kretprobe/do_sys_open")
int trace_open_return(struct pt_regs *ctx) {
    int ret = PT_REGS_RC(ctx); // Valeur de retour
    bpf_trace_printk("File descriptor: %d\n", ret);
    return 0;
}
```

---

## L

### **libbpf**
Bibliothèque C officielle pour interagir avec eBPF depuis l'espace utilisateur. Gère le chargement, l'attachement, et la communication avec les programmes eBPF. Alternative moderne à BCC, plus légère et portable.
```c
#include <bpf/libbpf.h>

struct bpf_object *obj = bpf_object__open("program.bpf.o");
bpf_object__load(obj);
struct bpf_link *link = bpf_program__attach(prog);
```

### **Linux kernel**
Système d'exploitation contenant l'infrastructure eBPF. Le support eBPF s'est considérablement amélioré depuis le kernel 4.x, avec des fonctionnalités majeures dans les versions 5.x et 6.x.

### **LLVM**
Infrastructure de compilateur utilisée pour compiler les programmes eBPF. Clang (compilateur C de LLVM) génère le bytecode eBPF.

### **LSM (Linux Security Module) BPF**
Type de programme eBPF permettant d'implémenter des politiques de sécurité personnalisées en s'attachant aux hooks LSM du noyau (contrôle d'accès, audit).

---

## M

### **Map**
Structure de données eBPF permettant de stocker et partager des informations. Types de maps :
- **BPF_MAP_TYPE_HASH** : Table de hachage
- **BPF_MAP_TYPE_ARRAY** : Tableau indexé
- **BPF_MAP_TYPE_PERF_EVENT_ARRAY** : Buffer circulaire pour événements
- **BPF_MAP_TYPE_LRU_HASH** : Hash avec éviction LRU
- **BPF_MAP_TYPE_RINGBUF** : Ring buffer moderne (kernel 5.8+)

Exemple de définition :
```c
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);
    __type(key, u32);
    __type(value, u64);
} my_map SEC(".maps");
```

### **Map pinning**
Action de rendre une map persistante en l'ancrant dans le BPF filesystem (`/sys/fs/bpf/`), permettant à d'autres programmes d'y accéder même après la terminaison du programme créateur.

### **Map-in-map**
Map contenant d'autres maps comme valeurs. Permet des structures de données complexes et la mise à jour atomique de configurations.

---

## O

### **Observability**
Capacité à comprendre l'état interne d'un système en examinant ses sorties. eBPF excelle en observabilité : tracing, métriques, profiling, sans surcharge significative.

---

## P

### **Perf event**
Mécanisme du noyau Linux pour le profiling de performance. eBPF peut s'attacher à des perf events (cycles CPU, cache misses, page faults, etc.).

### **Perf event array**
Type de map eBPF optimisée pour transférer de gros volumes de données du noyau vers l'espace utilisateur (événements de tracing, logs).

### **Pixie**
Plateforme d'observabilité Kubernetes utilisant eBPF pour capturer automatiquement des données applicatives sans instrumentation.

### **Program type**
Voir **eBPF program types**.

### **pt_regs**
Structure contenant l'état des registres CPU au moment de l'appel d'une fonction. Passée comme contexte aux kprobes/kretprobes.

---

## R

### **Restrict C**
Sous-ensemble restreint du langage C utilisé pour écrire les programmes eBPF. Limitations :
- Pas de boucles non bornées
- Pas d'appels de fonction non-inline (sauf helpers)
- Pas de variables globales non initialisées
- Stack limitée (512 octets max)
- Pas d'accès direct à la mémoire arbitraire

Ces restrictions garantissent que le verifier peut prouver la terminaison et la sécurité du programme.

### **Ring buffer (BPF_MAP_TYPE_RINGBUF)**
Type de map moderne (kernel 5.8+) pour transférer efficacement des données du noyau vers l'espace utilisateur. Remplace progressivement les perf event arrays avec de meilleures performances.

---

## S

### **SEC() macro**
Macro utilisée dans les programmes eBPF pour spécifier le type et le point d'attachement du programme via les sections ELF.
```c
SEC("kprobe/sys_execve")  // Attache à l'entrée de sys_execve
SEC("xdp")                 // Programme XDP
SEC("tracepoint/syscalls/sys_enter_open")  // Tracepoint
```

### **Socket filter**
Type de programme eBPF permettant de filtrer les paquets au niveau socket (équivalent moderne de tcpdump/BPF classique).

### **Stack size limit**
Limitation de la taille de la pile pour les programmes eBPF : 512 octets maximum. Force l'utilisation de maps pour les structures volumineuses.

### **Static tracepoint**
Voir **Tracepoint**.

---

## T

### **Tail call**
Mécanisme permettant à un programme eBPF d'appeler un autre programme eBPF, permettant de chaîner des programmes et de contourner les limites de complexité.
```c
// Map contenant les programmes à appeler
struct {
    __uint(type, BPF_MAP_TYPE_PROG_ARRAY);
    __uint(max_entries, 10);
    __uint(key_size, sizeof(u32));
    __uint(value_size, sizeof(u32));
} prog_array SEC(".maps");

// Effectuer un tail call
bpf_tail_call(ctx, &prog_array, index);
```

### **TC (Traffic Control) BPF**
Type de programme eBPF s'attachant à l'infrastructure Linux Traffic Control pour contrôler et modifier le trafic réseau (ingress/egress).

### **Tracepoint**
Point d'instrumentation statique dans le noyau Linux, défini explicitement par les développeurs du noyau. Plus stables que les kprobes (ne changent pas entre versions du noyau).
```c
SEC("tracepoint/syscalls/sys_enter_execve")
int trace_execve(struct trace_event_raw_sys_enter *ctx) {
    // Tracer les appels à execve
    return 0;
}
```

### **Tracing**
Processus d'enregistrement détaillé des événements système (appels de fonctions, syscalls, événements réseau) pour l'analyse et le débogage.

---

## U

### **Uprobe (User-space probe)**
Point d'instrumentation dynamique dans les programmes en espace utilisateur. Permet d'attacher un programme eBPF à l'entrée de fonctions d'applications ou de bibliothèques.
```c
SEC("uprobe//usr/lib/x86_64-linux-gnu/libc.so.6:malloc")
int trace_malloc(struct pt_regs *ctx) {
    size_t size = PT_REGS_PARM1(ctx);
    bpf_trace_printk("malloc(%zu)\n", size);
    return 0;
}
```

### **Uretprobe (User-space return probe)**
Version uprobe s'attachant à la sortie d'une fonction en espace utilisateur.

### **User space**
Espace d'exécution non-privilégié où s'exécutent les applications. Les programmes eBPF peuvent être chargés depuis le user space mais s'exécutent dans le kernel space.

### **USDT (User-level Statically Defined Tracing)**
Points de tracing définis statiquement dans les applications utilisateur (équivalent user-space des tracepoints). Utilisés par des applications comme PostgreSQL, MySQL, Node.js.

---

## V

### **Verifier**
Composant crucial du noyau Linux qui analyse statiquement chaque programme eBPF avant son chargement pour garantir :
- Terminaison (pas de boucles infinies)
- Sécurité mémoire (pas d'accès invalides)
- Pas d'opérations dangereuses
- Complexité limitée (nombre d'instructions)

Si le verifier rejette un programme, celui-ci n'est pas chargé dans le noyau.

### **vmlinux.h**
Fichier d'en-tête généré automatiquement contenant toutes les structures de données du noyau Linux, grâce à BTF. Simplifie considérablement l'écriture de programmes eBPF CO-RE.
```c
#include "vmlinux.h"  // Une seule ligne remplace des dizaines d'includes

SEC("kprobe/do_sys_open")
int my_prog(struct pt_regs *ctx) {
    struct task_struct *task = (struct task_struct *)bpf_get_current_task();
    // Accès direct aux structures du noyau
}
```

Génération :
```bash
bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h
```

---

## X

### **XDP (eXpress Data Path)**
Type de programme eBPF s'exécutant au plus tôt possible dans le chemin de réception réseau (au niveau du driver réseau, avant l'allocation de sk_buff). Permet du traitement ultra-rapide de paquets avec latence minimale.

Actions XDP possibles :
- **XDP_PASS** : Laisser passer le paquet (traitement normal)
- **XDP_DROP** : Supprimer le paquet (DDoS mitigation ultra-rapide)
- **XDP_TX** : Renvoyer le paquet sur la même interface (load balancing)
- **XDP_REDIRECT** : Rediriger vers une autre interface
- **XDP_ABORTED** : Erreur, supprimer le paquet

```c
SEC("xdp")
int xdp_drop_tcp(struct xdp_md *ctx) {
    void *data = (void *)(long)ctx->data;
    void *data_end = (void *)(long)ctx->data_end;

    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end)
        return XDP_PASS;

    if (eth->h_proto != htons(ETH_P_IP))
        return XDP_PASS;

    struct iphdr *ip = (void *)(eth + 1);
    if ((void *)(ip + 1) > data_end)
        return XDP_PASS;

    if (ip->protocol == IPPROTO_TCP)
        return XDP_DROP;  // Bloquer tout TCP

    return XDP_PASS;
}
```

### **XDP offload**
Capacité de certaines cartes réseau SmartNIC à exécuter directement des programmes XDP sur le matériel de la carte, déchargeant complètement le CPU.

---

## 🔧 Outils de l'écosystème eBPF

### **Framework de haut niveau**
- **BCC** : Framework Python/Lua pour prototypage rapide
- **bpftrace** : Langage de haut niveau pour traçage (syntaxe DTrace-like)
- **libbpf** : Bibliothèque C officielle pour applications en production

### **Outils de debugging et inspection**
- **bpftool** : CLI officielle pour inspecter et manipuler objets eBPF
- **bpf_dbg** : Débogueur pour bytecode eBPF
- **llvm-objdump** : Désassembleur de bytecode eBPF

### **Projets majeurs utilisant eBPF**
- **Cilium** : Networking et sécurité Kubernetes
- **Falco** : Détection d'intrusions et anomalies
- **Katran** : Load balancer L4 (Facebook)
- **Pixie** : Observabilité Kubernetes automatique
- **Calico** : Networking Kubernetes avec option eBPF
- **Hubble** : Observabilité réseau (Cilium)

---

## 📊 Comparaison BCC vs libbpf

| Aspect | BCC | libbpf |
|--------|-----|--------|
| **Langage** | Python/Lua + C | C |
| **Compilation** | Runtime (JIT) | Ahead-of-time (AOT) |
| **Dépendances** | LLVM/Clang requis sur le système cible | Binaire standalone |
| **Portabilité** | Recompile pour chaque noyau | CO-RE : compile once, run everywhere |
| **Performance** | Démarrage lent (compilation) | Démarrage instantané |
| **Usage** | Prototypage, scripts one-shot | Production, applications critiques |
| **Courbe d'apprentissage** | Plus simple (Python) | Plus technique (C pur) |

---

## 🎯 Concepts clés à retenir pour débuter

### **1. Les trois composants essentiels**
- **Programme eBPF** : Code C restreint exécuté dans le noyau
- **Maps** : Structures de données partagées
- **Helper functions** : API pour interagir avec le noyau

### **2. Le workflow eBPF**
```
1. Écrire programme en C restreint
   ↓
2. Compiler avec Clang → bytecode eBPF
   ↓
3. Charger dans le noyau (vérification par le verifier)
   ↓
4. JIT → code machine natif
   ↓
5. Attacher à un hook (kprobe, XDP, tracepoint...)
   ↓
6. Exécution automatique sur événements
```

### **3. Garanties de sécurité**
Le verifier garantit qu'un programme eBPF :
- ✅ Se termine toujours (pas de boucles infinies)
- ✅ N'accède qu'à la mémoire autorisée
- ✅ Ne plante pas le noyau
- ✅ A une complexité limitée

### **4. Limitations importantes**
- 📏 Stack limité à 512 octets
- 🔢 Nombre d'instructions limité (1M par programme)
- 🔄 Pas de boucles non bornées (depuis kernel 5.3 : boucles bornées OK)
- 📞 Pas d'appels de fonctions arbitraires (seulement helpers et inline)

---

## 💡 Cas d'usage typiques

### **🔍 Observabilité**
```
• Tracer tous les appels système
• Monitorer les performances d'une application
• Profiler l'utilisation CPU par fonction
• Mesurer la latence des requêtes réseau
```

### **🌐 Réseau**
```
• Filtrage DDoS ultra-rapide (XDP)
• Load balancing L4 sans kernel bypass
• Accélération de connexions (sockops)
• Monitoring du trafic en temps réel
```

### **🔒 Sécurité**
```
• Détection d'intrusions comportementales
• Audit système en temps réel
• Contrôle d'accès granulaire (LSM BPF)
• Protection contre les exploits
```

### **⚡ Performance**
```
• Identification des goulots d'étranglement
• Analyse de cache misses
• Détection de contentions de locks
• Profiling de latence I/O
```

---

## 🚀 Ressources pour débuter

### **Documentation officielle**
- **Kernel.org** : Documentation eBPF du noyau Linux
- **eBPF.io** : Site officiel de la communauté eBPF
- **Cilium Documentation** : Excellents tutoriels eBPF

### **Livres**
- **"BPF Performance Tools"** par Brendan Gregg (référence)
- **"Learning eBPF"** par Liz Rice (débutants)
- **"Linux Observability with BPF"** par David Calavera & Lorenzo Fontana

### **Projets pour apprendre**
1. **bpf-examples** : Exemples simples de programmes eBPF
2. **libbpf-bootstrap** : Template pour commencer avec libbpf
3. **bcc/tools** : Collection d'outils eBPF prêts à l'emploi

### **Tutoriels en ligne**
- **eBPF Summit** : Conférences annuelles (vidéos disponibles)
- **Cilium Labs** : Labs interactifs
- **IO Visor BCC Tutorial** : Guide pas à pas

---

## 🔗 Commandes essentielles

```bash
# Vérifier le support eBPF du noyau
uname -r  # Kernel 4.8+ minimum, 5.x+ recommandé

# Installer les outils
apt install linux-tools-generic bpftool  # Ubuntu/Debian

# Lister les programmes eBPF chargés
bpftool prog list
bpftool prog show id 42

# Lister les maps
bpftool map list
bpftool map dump name my_map

# Afficher les logs eBPF (bpf_trace_printk)
cat /sys/kernel/debug/tracing/trace_pipe

# Désassembler un programme
bpftool prog dump xlated id 42
llvm-objdump -d program.bpf.o

# Générer vmlinux.h
bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h

# Vérifier BTF support
bpftool btf list
```

---

## ⚠️ Pièges courants pour débutants

### **1. Oublier les vérifications de bornes**
```c
// ❌ MAUVAIS : Le verifier va rejeter
void *data = (void *)(long)ctx->data;
struct iphdr *ip = data + sizeof(struct ethhdr);
// Accès sans vérification !

// ✅ BON : Toujours vérifier avant d'accéder
void *data_end = (void *)(long)ctx->data_end;
if ((void *)(ip + 1) > data_end)
    return XDP_PASS;
// Maintenant l'accès est safe
```

### **2. Stack overflow (> 512 octets)**
```c
// ❌ MAUVAIS
char buffer[1024];  // Trop gros pour le stack !

// ✅ BON : Utiliser une map
struct {
    __uint(type, BPF_MAP_TYPE_PERCPU_ARRAY);
    __uint(max_entries, 1);
    __type(key, u32);
    __type(value, char[1024]);
} buffer_map SEC(".maps");
```

### **3. Boucles non bornées (ancien noyaux)**
```c
// ❌ MAUVAIS (kernel < 5.3)
while (condition) {  // Boucle potentiellement infinie
    // ...
}

// ✅ BON : Limiter explicitement
#pragma unroll
for (int i = 0; i < 10; i++) {  // Borne fixe
    // ...
}
```

### **4. Oublier de déclarer la section ELF**
```c
// ❌ MAUVAIS
int my_program(struct xdp_md *ctx) { }

// ✅ BON
SEC("xdp")
int my_program(struct xdp_md *ctx) { }
```

---

## 🎓 Niveaux de compétence eBPF

### **Débutant**
- ✓ Comprendre le concept et les cas d'usage
- ✓ Utiliser bpftrace pour du tracing simple
- ✓ Lire et modifier des programmes BCC existants
- ✓ Utiliser bpftool pour inspecter les programmes

### **Intermédiaire**
- ✓ Écrire des programmes BCC personnalisés
- ✓ Comprendre les différents types de programmes (XDP, kprobe, tracepoint)
- ✓ Utiliser les maps pour le partage de données
- ✓ Déboguer avec bpf_trace_printk

### **Avancé**
- ✓ Écrire des programmes avec libbpf et CO-RE
- ✓ Comprendre le verifier et ses contraintes
- ✓ Optimiser les performances (minimiser les copies, utiliser maps efficaces)
- ✓ Intégrer eBPF dans des applications C en production

### **Expert**
- ✓ Contribuer au noyau Linux (helpers, fonctionnalités eBPF)
- ✓ Implémenter des solutions complexes (load balancers, security policies)
- ✓ Optimiser au niveau du bytecode
- ✓ Développer des frameworks eBPF

---

## 🌟 Pourquoi eBPF est révolutionnaire

### **Avant eBPF**
```
• Modifier le noyau → recompiler, redémarrer (risqué)
• Modules noyau → risque de crash, difficile à déboguer
• Observabilité limitée → outils externes lourds
• Performance overhead significatif
```

### **Avec eBPF**
```
✨ Extensibilité sûre du noyau sans le modifier
✨ Programmabilité dynamique à chaud
✨ Performance native (JIT → code machine)
✨ Sécurité garantie (verifier)
✨ Observabilité sans overhead
```

**eBPF est à Linux ce que JavaScript est au navigateur** : un moteur d'exécution sécurisé permettant d'étendre les capacités sans modifier le cœur du système.

---

*Ce glossaire est maintenu à jour avec les dernières versions du noyau Linux (6.x) et les pratiques eBPF modernes 2025.*

⏭️ [Parcours recommandés selon le profil](/parcours-recommandes/README.md)
