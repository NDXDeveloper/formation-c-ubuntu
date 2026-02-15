🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 21.2 Architecture eBPF

## Introduction

Maintenant que vous comprenez **ce qu'est** eBPF et **pourquoi** il est révolutionnaire, il est temps de plonger dans les détails techniques : **comment** fonctionne eBPF ?

L'architecture d'eBPF est élégante et bien pensée. Elle repose sur quatre piliers fondamentaux qui travaillent ensemble pour offrir sécurité, performance et flexibilité. Dans cette section, nous allons explorer chacun de ces composants en profondeur.

---

## 🏛️ Vue d'Ensemble de l'Architecture

### Les Quatre Piliers d'eBPF

```
┌──────────────────────────────────────────────────────────────┐
│                     ARCHITECTURE eBPF                        │
│                                                              │
│  ┌────────────────────────────────────────────────────────┐  │
│  │                                                        │  │
│  │   1️⃣  PROGRAMMES BPF                                   │  │
│  │      • Code exécuté dans le noyau                      │  │
│  │      • Écrits en C, compilés en bytecode               │  │
│  │      • Attachés à des hooks (événements)               │  │
│  │                                                        │  │
│  └────────────────┬───────────────────────────────────────┘  │
│                   │                                          │
│                   │ Utilise                                  │
│                   ▼                                          │
│  ┌────────────────────────────────────────────────────────┐  │
│  │                                                        │  │
│  │   2️⃣  BPF MAPS                                         │  │
│  │      • Structures de données partagées                 │  │
│  │      • Communication kernel ↔ userspace                │  │
│  │      • Stockage persistant                             │  │
│  │                                                        │  │
│  └────────────────────────────────────────────────────────┘  │
│                                                              │
│  ┌────────────────────────────────────────────────────────┐  │
│  │                                                        │  │
│  │   3️⃣  VÉRIFICATEUR (Verifier)                          │  │
│  │      • Analyse statique du code                        │  │
│  │      • Garantie de sécurité                            │  │
│  │      • Validation avant exécution                      │  │
│  │                                                        │  │
│  └────────────────────────────────────────────────────────┘  │
│                                                              │
│  ┌────────────────────────────────────────────────────────┐  │
│  │                                                        │  │
│  │   4️⃣  JIT COMPILER                                     │  │
│  │      • Compilation Just-In-Time                        │  │
│  │      • Bytecode → Code machine natif                   │  │
│  │      • Performance maximale                            │  │
│  │                                                        │  │
│  └────────────────────────────────────────────────────────┘  │
└──────────────────────────────────────────────────────────────┘
```

Chacun de ces composants joue un rôle crucial, et nous allons les explorer un par un dans les sections suivantes.

---

## 🔄 Le Cycle de Vie Complet

Pour bien comprendre l'architecture, suivons le parcours complet d'un programme eBPF, de sa création à son exécution :

```
┌─────────────────────────────────────────────────────────────┐
│                    ESPACE UTILISATEUR                       │
│                                                             │
│  1. Développeur écrit le code                               │
│     program.bpf.c (en C)                                    │
│     ↓                                                       │
│  2. Compilation avec clang                                  │
│     $ clang -O2 -target bpf -c program.bpf.c                │
│     ↓                                                       │
│  3. Bytecode eBPF généré                                    │
│     program.bpf.o (fichier ELF)                             │
│     ↓                                                       │
│  4. Chargement via bpf() syscall                            │
│     (via libbpf, bpftool, etc.)                             │
│                                                             │
└──────────────────────┬──────────────────────────────────────┘
                       │
═══════════════════════╪═══════════════════════════════════════
                       │ Frontière Kernel
                       ▼
┌─────────────────────────────────────────────────────────────┐
│                     NOYAU LINUX                             │
│                                                             │
│  5. VÉRIFICATEUR (Verifier)                                 │
│     ┌─────────────────────────────────────────┐             │
│     │ • Analyse statique exhaustive           │             │
│     │ • Vérifie la sécurité                   │             │
│     │ • Garantit la terminaison               │             │
│     │ • Valide tous les accès mémoire         │             │
│     └──────────────┬──────────────────────────┘             │
│                    │                                        │
│            ┌───────┴────────┐                               │
│            │                │                               │
│       ✅ ACCEPTÉ       ❌ REJETÉ                            │
│            │                │                               │
│            │                └──▶ Erreur retournée           │
│            ▼                    (programme non chargé)      │
│                                                             │
│  6. JIT COMPILER                                            │
│     ┌─────────────────────────────────────────┐             │
│     │ • Compile bytecode → code natif         │             │
│     │ • Optimisations                         │             │
│     │ • Architecture spécifique               │             │
│     │   (x86-64, ARM64, etc.)                 │             │
│     └──────────────┬──────────────────────────┘             │
│                    ▼                                        │
│                                                             │
│  7. Programme chargé en mémoire kernel                      │
│     ┌─────────────────────────────────────────┐             │
│     │ Code machine natif prêt                 │             │
│     │ Maps créées et initialisées             │             │
│     └──────────────┬──────────────────────────┘             │
│                    ▼                                        │
│                                                             │
│  8. Attachement au hook                                     │
│     ┌─────────────────────────────────────────┐             │
│     │ Programme attaché à un point            │             │
│     │ d'événement (tracepoint, kprobe,        │             │
│     │ XDP, socket, etc.)                      │             │
│     └──────────────┬──────────────────────────┘             │
│                    │                                        │
│                    ▼                                        │
│  9. EXÉCUTION                                               │
│     ┌─────────────────────────────────────────┐             │
│     │ Événement se produit                    │             │
│     │  ↓                                      │             │
│     │ Programme BPF déclenché                 │             │
│     │  ↓                                      │             │
│     │ Exécution en code natif                 │             │
│     │  ↓                                      │             │
│     │ Interaction avec BPF Maps               │             │
│     │  ↓                                      │             │
│     │ Retour au noyau                         │             │
│     └─────────────────────────────────────────┘             │
│                    │                                        │
│                    ▼                                        │
│  10. Communication avec userspace                           │
│     • Via BPF Maps (lecture/écriture)                       │
│     • Via Ring Buffers (streaming d'événements)             │
│     • Via Perf Events                                       │
│                                                             │
└─────────────────────────────────────────────────────────────┘
```

---

## 🎯 Analogie Globale : L'Usine de Production

Pour mieux comprendre comment tous ces composants interagissent, imaginez une **usine de production ultra-sécurisée** :

```
┌──────────────────────────────────────────────────────────┐
│                  USINE eBPF                              │
│                                                          │
│  1️⃣  PLANS (Programmes BPF)                              │
│      Les instructions pour fabriquer quelque chose       │
│      • Ce qu'il faut faire                               │
│      • Comment le faire                                  │
│      • Quand le faire                                    │
│                                                          │
│  2️⃣  ENTREPÔT (BPF Maps)                                 │
│      Stockage des matériaux et produits finis            │
│      • Entrées/Sorties                                   │
│      • Stock persistant                                  │
│      • Accessible de partout                             │
│                                                          │
│  3️⃣  INSPECTEUR DE SÉCURITÉ (Vérificateur)               │
│      Vérifie que tout est sûr AVANT production           │
│      • Les plans sont-ils corrects ?                     │
│      • Y a-t-il des risques ?                            │
│      • Tout se terminera-t-il bien ?                     │
│      ✓ Approuvé → Production                             │
│      ✗ Rejeté → Refaire les plans                        │
│                                                          │
│  4️⃣  MACHINES OPTIMISÉES (JIT Compiler)                  │
│      Transforme les plans en machines ultra-rapides      │
│      • Adapté à l'usine (architecture CPU)               │
│      • Optimisé pour la vitesse                          │
│      • Prêt pour production intensive                    │
│                                                          │
│  🔄 PRODUCTION (Exécution)                               │
│      Les machines tournent en continu                    │
│      • Réagissent aux commandes (événements)             │
│      • Utilisent l'entrepôt (maps)                       │
│      • Performance maximale garantie                     │
│      • Sécurité garantie (inspecteur a approuvé)         │
└──────────────────────────────────────────────────────────┘
```

**L'avantage** : Vous pouvez changer les plans (programmes), ajouter des entrepôts (maps), **sans jamais arrêter l'usine** (sans redémarrer le système) !

---

## 🔍 Les Quatre Composants en Détail

Maintenant, donnons un aperçu de chaque composant avant de les explorer en profondeur dans les sections suivantes.

### 1️⃣ Programmes BPF : Les Acteurs

```
┌─────────────────────────────────────────┐
│     PROGRAMMES BPF                      │
│                                         │
│  • Code écrit en C                      │
│  • Compilé en bytecode eBPF             │
│  • Attaché à des événements (hooks)     │
│  • Exécuté dans le noyau                │
│                                         │
│  Types de programmes :                  │
│  • Tracepoint    : Événements statiques │
│  • Kprobe        : Fonctions kernel     │
│  • Uprobe        : Fonctions userspace  │
│  • XDP           : Paquets réseau       │
│  • TC            : Traffic control      │
│  • LSM           : Sécurité             │
│  • Et bien d'autres...                  │
└─────────────────────────────────────────┘
```

**Exemple simple** :
```c
SEC("tracepoint/syscalls/sys_enter_open")  
int trace_open(void *ctx)  
{
    bpf_printk("Fichier ouvert!\n");
    return 0;
}
```

Ce programme s'exécute **chaque fois** qu'un fichier est ouvert sur le système.

**Section détaillée** : [21.2.1 BPF Programs](./02.1-bpf-programs.md)

---

### 2️⃣ BPF Maps : La Mémoire Partagée

```
┌─────────────────────────────────────────┐
│     BPF MAPS                            │
│                                         │
│  • Structures de données génériques     │
│  • Partagées kernel ↔ userspace         │
│  • Persistantes                         │
│                                         │
│  Types de maps :                        │
│  • HASH       : Dictionnaire clé-valeur │
│  • ARRAY      : Tableau indexé          │
│  • RINGBUF    : Buffer circulaire       │
│  • PERCPU_*   : Une copie par CPU       │
│  • LRU_HASH   : Cache auto-nettoyant    │
│  • Et plus...                           │
└─────────────────────────────────────────┘
```

**Exemple** : Compter les appels par PID
```c
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10000);
    __type(key, __u32);    // PID
    __type(value, __u64);  // Compteur
} counts SEC(".maps");
```

Les programmes BPF écrivent dans cette map, et les programmes userspace peuvent la lire.

**Section détaillée** : [21.2.2 BPF Maps](./02.2-bpf-maps.md)

---

### 3️⃣ Le Vérificateur : Le Gardien de la Sécurité

```
┌─────────────────────────────────────────┐
│     VÉRIFICATEUR                        │
│                                         │
│  Garantit mathématiquement que :        │
│  ✓ Le programme se termine toujours     │
│  ✓ Pas d'accès mémoire invalide         │
│  ✓ Pas de boucles infinies              │
│  ✓ Pas de crash possible                │
│  ✓ Pas de fuite d'informations          │
│                                         │
│  Méthode :                              │
│  • Analyse statique exhaustive          │
│  • Simulation symbolique                │
│  • Vérification de tous les chemins     │
│                                         │
│  Résultat :                             │
│  ✅ Programme sûr → Chargé              │
│  ❌ Doute de sécurité → Rejeté          │
└─────────────────────────────────────────┘
```

**C'est LE composant** qui rend eBPF révolutionnaire : impossible de crasher le noyau avec un programme eBPF.

**Exemple de rejet** :
```c
int *ptr = NULL;
*ptr = 42;  // ❌ REJETÉ : déréférencement NULL détecté
```

Le vérificateur détectera ce problème et refusera de charger le programme.

**Section détaillée** : [21.2.3 Le Vérificateur](./02.3-verifier.md)

---

### 4️⃣ JIT Compiler : Le Booster de Performance

```
┌─────────────────────────────────────────┐
│     JIT COMPILER                        │
│                                         │
│  Transformation :                       │
│  Bytecode eBPF → Code Machine Natif     │
│                                         │
│  Architectures supportées :             │
│  • x86-64, ARM64, ARM32                 │
│  • PowerPC, s390x, MIPS                 │
│  • RISC-V, SPARC                        │
│                                         │
│  Performance :                          │
│  • Équivalent au C natif (95-100%)      │
│  • 10-50x plus rapide que l'interpréter │
│  • Optimisations automatiques           │
│                                         │
│  Sécurité :                             │
│  • Code JIT aussi vérifié               │
│  • Hardening disponible                 │
└─────────────────────────────────────────┘
```

**Transformation** :
```
Bytecode eBPF         →    Code x86-64  
mov r0, 42            →    mov rax, 42  
add r0, 10            →    add rax, 10  
exit                  →    ret  
```

Exécution **directe** par le CPU = performance maximale.

**Section détaillée** : [21.2.4 JIT Compilation](./02.4-jit-compilation.md)

---

## 🔗 Comment les Composants Interagissent

### Scénario : Tracer les Appels open()

Voyons concrètement comment les quatre composants travaillent ensemble :

```
┌────────────────────────────────────────────────────────────┐
│  ÉTAPE 1 : DÉVELOPPEMENT                                   │
│                                                            │
│  Développeur écrit un programme BPF :                      │
│                                                            │
│  SEC("tracepoint/syscalls/sys_enter_open")                 │
│  int trace_open(void *ctx) {                               │
│      u32 pid = bpf_get_current_pid_tgid() >> 32;           │
│      u64 *count = bpf_map_lookup_elem(&counts, &pid);      │
│      if (count) (*count)++;                                │
│      return 0;                                             │
│  }                                                         │
└────────────────────────────────────────────────────────────┘
                           ↓
┌────────────────────────────────────────────────────────────┐
│  ÉTAPE 2 : COMPILATION                                     │
│                                                            │
│  clang -O2 -target bpf -c program.bpf.c                    │
│  → Génère du bytecode eBPF                                 │
└────────────────────────────────────────────────────────────┘
                           ↓
┌────────────────────────────────────────────────────────────┐
│  ÉTAPE 3 : CHARGEMENT ET VÉRIFICATION                      │
│                                                            │
│  1️⃣ Programme envoyé au noyau via bpf() syscall            │
│  2️⃣ VÉRIFICATEUR analyse le code :                         │
│     ✓ Lookup vérifié (peut retourner NULL)                 │
│     ✓ Pointeur vérifié avant déréférencement               │
│     ✓ Pas de boucles infinies                              │
│     ✓ Taille raisonnable                                   │
│  3️⃣ Verdict : ✅ PROGRAMME SÛR                             │
└────────────────────────────────────────────────────────────┘
                           ↓
┌────────────────────────────────────────────────────────────┐
│  ÉTAPE 4 : JIT COMPILATION                                 │
│                                                            │
│  4️⃣ JIT COMPILER transforme :                              │
│     Bytecode → Code x86-64 natif                           │
│  5️⃣ Code chargé en mémoire kernel                          │
│  6️⃣ BPF MAP "counts" créée (HASH, PID → compteur)          │
└────────────────────────────────────────────────────────────┘
                           ↓
┌────────────────────────────────────────────────────────────┐
│  ÉTAPE 5 : ATTACHEMENT                                     │
│                                                            │
│  7️⃣ Programme attaché au tracepoint                        │
│     "syscalls/sys_enter_open"                              │
│  8️⃣ Prêt à s'exécuter à chaque appel open()                │
└────────────────────────────────────────────────────────────┘
                           ↓
┌────────────────────────────────────────────────────────────┐
│  ÉTAPE 6 : EXÉCUTION EN PRODUCTION                         │
│                                                            │
│  Application : open("/etc/passwd", O_RDONLY)               │
│      ↓                                                     │
│  Noyau : syscall sys_open()                                │
│      ↓                                                     │
│  Tracepoint déclenché                                      │
│      ↓                                                     │
│  🚀 Programme BPF exécuté (code natif)                     │
│      • Récupère PID : 1234                                 │
│      • Lookup dans MAP "counts"                            │
│      • Incrémente compteur : counts[1234]++                │
│      • Retourne 0                                          │
│      ↓                                                     │
│  Noyau continue normalement                                │
│                                                            │
│  ⏱️  Overhead : ~0.5 µs (imperceptible)                    │
└────────────────────────────────────────────────────────────┘
                           ↓
┌────────────────────────────────────────────────────────────┐
│  ÉTAPE 7 : LECTURE DES RÉSULTATS (USERSPACE)               │
│                                                            │
│  Programme utilisateur lit la MAP :                        │
│                                                            │
│  for (u32 pid; iterate_map(&counts, &pid, &count);) {      │
│      printf("PID %d: %llu open()\n", pid, count);          │
│  }                                                         │
│                                                            │
│  Résultat :                                                │
│  PID 1234: 42 open()                                       │
│  PID 5678: 15 open()                                       │
│  ...                                                       │
└────────────────────────────────────────────────────────────┘
```

**Points clés** :
1. **Vérificateur** garantit la sécurité avant même l'exécution
2. **JIT** assure la performance (code natif)
3. **Maps** permettent la communication et la persistance
4. **Programmes** réagissent aux événements en temps réel

---

## 🎨 Vue d'Architecture Complète

Voici une vue d'ensemble de tous les composants et leurs interactions :

```
┌──────────────────────────────────────────────────────────────────┐
│                        ESPACE UTILISATEUR                        │
│                                                                  │
│  ┌──────────────┐    ┌──────────────┐    ┌──────────────┐        │
│  │  Votre App   │    │   bpftool    │    │   libbpf     │        │
│  │  (Python, Go,│    │  (CLI tool)  │    │  (library)   │        │
│  │   C, Rust)   │    │              │    │              │        │
│  └──────┬───────┘    └──────┬───────┘    └──────┬───────┘        │
│         │                   │                   │                │
│         └───────────────────┴───────────────────┘                │
│                             │                                    │
│                             │ bpf() syscall                      │
│                             │ (LOAD, ATTACH, MAP_OPS...)         │
└─────────────────────────────┼────────────────────────────────────┘
══════════════════════════════╪═════════════════════════════════════
                              │ Frontière Kernel
                              ▼
┌──────────────────────────────────────────────────────────────────┐
│                        NOYAU LINUX                               │
│                                                                  │
│  ┌────────────────────────────────────────────────────────────┐  │
│  │                  SOUS-SYSTÈME eBPF                         │  │
│  │                                                            │  │
│  │  ┌──────────────────────────────────────────────────────┐  │  │
│  │  │  1. Chargement de Programme                          │  │  │
│  │  │     • Réception du bytecode                          │  │  │
│  │  │     • Validation format ELF                          │  │  │
│  │  └─────────────────┬────────────────────────────────────┘  │  │
│  │                    ▼                                       │  │
│  │  ┌──────────────────────────────────────────────────────┐  │  │
│  │  │  2. VÉRIFICATEUR                                     │  │  │
│  │  │     • Analyse CFG (Control Flow Graph)               │  │  │
│  │  │     • Simulation symbolique                          │  │  │
│  │  │     • Vérification des accès mémoire                 │  │  │
│  │  │     • Validation des types                           │  │  │
│  │  │     ✅ Sûr → Continue | ❌ Dangereux → Rejet         │  │  │
│  │  └─────────────────┬────────────────────────────────────┘  │  │
│  │                    ▼                                       │  │
│  │  ┌──────────────────────────────────────────────────────┐  │  │
│  │  │  3. JIT COMPILER                                     │  │  │
│  │  │     • Bytecode eBPF → Code natif                     │  │  │
│  │  │     • Optimisations                                  │  │  │
│  │  │     • Architecture-specific                          │  │  │
│  │  └─────────────────┬────────────────────────────────────┘  │  │
│  │                    ▼                                       │  │
│  │  ┌──────────────────────────────────────────────────────┐  │  │
│  │  │  4. Programme en Mémoire Kernel                      │  │  │
│  │  │     • Code machine natif                             │  │  │
│  │  │     • Prêt pour exécution                            │  │  │
│  │  └──────────────────────────────────────────────────────┘  │  │
│  │                                                            │  │
│  └────────────────────────────────────────────────────────────┘  │
│                                                                  │
│  ┌────────────────────────────────────────────────────────────┐  │
│  │                       BPF MAPS                             │  │
│  │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐        │  │
│  │  │  HASH   │  │  ARRAY  │  │ RINGBUF │  │ PERCPU  │        │  │
│  │  │  MAP    │  │   MAP   │  │         │  │  ARRAY  │ ...    │  │
│  │  └─────────┘  └─────────┘  └─────────┘  └─────────┘        │  │
│  └────────────────────────────────────────────────────────────┘  │
│                             ▲                                    │
│                             │                                    │
│  ┌──────────────────────────┴─────────────────────────────────┐  │
│  │                    HOOKS / ATTACH POINTS                   │  │
│  │                                                            │  │
│  │  Tracepoints  Kprobes  Uprobes  XDP  TC  Sockets  LSM ...  │  │
│  │       │          │        │      │    │      │       │     │  │
│  └───────┼──────────┼────────┼──────┼────┼──────┼───────┼─────┘  │
│          │          │        │      │    │      │       │        │
│          ▼          ▼        ▼      ▼    ▼      ▼       ▼        │
│  ┌──────────────────────────────────────────────────────────┐    │
│  │  ÉVÉNEMENTS SYSTÈME                                      │    │
│  │  • Appels système                                        │    │
│  │  • Fonctions kernel                                      │    │
│  │  • Paquets réseau                                        │    │
│  │  • Opérations fichiers                                   │    │
│  │  • Changements de contexte                               │    │
│  │  • ...                                                   │    │
│  └──────────────────────────────────────────────────────────┘    │
└──────────────────────────────────────────────────────────────────┘
```

---

## 🎯 Ce que Vous Allez Apprendre

Dans les sections suivantes, nous allons explorer chaque composant en profondeur :

### **21.2.1 BPF Programs**
- Types de programmes (tracepoint, kprobe, XDP, etc.)
- Structure d'un programme
- Cycle de vie
- Restrictions et contraintes
- Helper functions
- Exemples concrets

### **21.2.2 BPF Maps**
- Types de maps (HASH, ARRAY, RINGBUF, etc.)
- Opérations (lookup, update, delete)
- Communication kernel ↔ userspace
- Choix du bon type de map
- Bonnes pratiques

### **21.2.3 Le Vérificateur**
- Fonctionnement de l'analyse statique
- Ce qui est vérifié (terminaison, accès mémoire, etc.)
- Erreurs courantes et solutions
- Techniques pour passer la vérification
- Debugging des rejets

### **21.2.4 JIT Compilation**
- Transformation bytecode → code natif
- Architectures supportées
- Performance comparée (JIT vs interprété)
- Configuration et inspection
- Optimisations

---

## 💡 Pourquoi Cette Architecture Est Brillante

L'architecture d'eBPF est un chef-d'œuvre d'ingénierie logicielle :

### 1. Séparation des Responsabilités

```
PROGRAMMES     → La logique métier  
MAPS           → Le stockage de données  
VÉRIFICATEUR   → La sécurité  
JIT            → La performance  
```

Chaque composant a un rôle clair et bien défini.

### 2. Sécurité par Design

```
Vérification AVANT exécution = Impossible de crasher
```

Contrairement aux modules noyau classiques qui sont dangereux, eBPF garantit la sécurité.

### 3. Performance Sans Compromis

```
JIT compilation = Vitesse du code C natif
```

Pas besoin de choisir entre sécurité et performance : eBPF offre les deux.

### 4. Flexibilité Maximale

```
• Chargement dynamique (pas de reboot)
• Multiples types de programmes
• Architecture-agnostic (bytecode)
• Extensible (nouveaux types régulièrement)
```

### 5. Portabilité

```
Bytecode eBPF → Code natif (toute architecture)
```

Le même bytecode fonctionne sur x86-64, ARM64, RISC-V, etc.

---

## 🎓 Résumé

L'**architecture eBPF** repose sur quatre piliers :

**1️⃣ Programmes BPF** :
- Code exécuté dans le noyau
- Réagissent aux événements
- Écrits en C, compilés en bytecode

**2️⃣ BPF Maps** :
- Structures de données partagées
- Communication kernel ↔ userspace
- Stockage persistant

**3️⃣ Vérificateur** :
- Garantit la sécurité mathématiquement
- Analyse statique exhaustive
- Rejet de tout code dangereux

**4️⃣ JIT Compiler** :
- Compilation Just-In-Time
- Code machine natif
- Performance maximale

**Cycle de vie** :
```
Code C → Bytecode → Vérification → JIT → Exécution
```

**Avantages** :
- 🛡️ Sécurité garantie
- ⚡ Performance native
- 🔄 Dynamisme (chargement sans reboot)
- 🌐 Portabilité (multi-architecture)

---

## 🚀 Prêt à Plonger Plus Profond ?

Maintenant que vous avez une vue d'ensemble de l'architecture, il est temps d'explorer chaque composant en détail.

**👉 Commençons par les programmes BPF :**  
**21.2.1 BPF Programs**  

Dans cette section, vous découvrirez :
- Les différents types de programmes
- Comment écrire votre premier programme BPF
- Les restrictions et contraintes
- Les helper functions disponibles
- Des exemples concrets pour chaque type

**Allons-y !** 🎯

---

## 📚 Références Préliminaires

### Documentation Officielle
- [eBPF Documentation](https://ebpf.io/what-is-ebpf/)
- [Kernel BPF Documentation](https://www.kernel.org/doc/html/latest/bpf/)

### Ressources Visuelles
- [eBPF Architecture Diagram](https://ebpf.io/what-is-ebpf/#hook-overview)
- [BPF Internals (LWN.net)](https://lwn.net/Articles/740157/)

### Livres
- **"Learning eBPF"** par Liz Rice (O'Reilly) - Chapitre 2 : eBPF Architecture
- **"BPF Performance Tools"** par Brendan Gregg - Architecture Overview

---

**Prochaine étape : Comprendre les Programmes BPF en profondeur ! 📖**

⏭️ [BPF programs](/21-introduction-ebpf/02.1-bpf-programs.md)
