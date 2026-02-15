# Chapitre 21 - Introduction à eBPF : Guide de compilation

## Prérequis

```bash
# Outils nécessaires
sudo apt install clang libbpf-dev linux-tools-common linux-tools-$(uname -r)

# Générer vmlinux.h (nécessaire pour les programmes utilisant les types kernel)
bpftool btf dump file /sys/kernel/btf/vmlinux format c > vmlinux.h
```

## Programmes BPF standalone (01-12)

Ces fichiers sont des programmes BPF kernel uniquement (pas de loader userspace).  
Ils produisent des fichiers `.bpf.o` (bytecode BPF).  

### Compilation générale

**Programmes utilisant `<linux/bpf.h>` :**
```bash
clang -O2 -g -target bpf -D__TARGET_ARCH_x86 -I/usr/include/x86_64-linux-gnu -c fichier.bpf.c -o fichier.bpf.o
```

**Programmes utilisant `vmlinux.h` :**
```bash
clang -O2 -g -target bpf -D__TARGET_ARCH_x86 -c fichier.bpf.c -o fichier.bpf.o
```

> **Note :** `vmlinux.h` et `<linux/bpf.h>` ne peuvent pas être utilisés ensemble (conflits de types).

### 01_test_toolchain.bpf.c

| Champ | Valeur |
|-------|--------|
| **Section** | 21.4 : Toolchain eBPF |
| **Description** | Programme BPF minimal pour tester la chaîne de compilation |
| **Fichier source** | 04-toolchain-ebpf.md |
| **Includes** | `<linux/bpf.h>` |
| **Compilation** | `clang -O2 -g -target bpf -D__TARGET_ARCH_x86 -I/usr/include/x86_64-linux-gnu -c 01_test_toolchain.bpf.c -o 01_test_toolchain.bpf.o` |
| **Sortie attendue** | Fichier .bpf.o généré sans erreur |

### 02_read_stats.bpf.c

| Champ | Valeur |
|-------|--------|
| **Section** | 21.2.1 : BPF Programs |
| **Description** | Compte les octets lus par chaque processus via tracepoint sys_exit_read |
| **Fichier source** | 02.1-bpf-programs.md |
| **Includes** | `<linux/bpf.h>` + définition locale `trace_event_raw_sys_exit` |
| **Compilation** | `clang -O2 -g -target bpf -D__TARGET_ARCH_x86 -I/usr/include/x86_64-linux-gnu -c 02_read_stats.bpf.c -o 02_read_stats.bpf.o` |
| **Sortie attendue** | Fichier .bpf.o généré sans erreur |

### 03_xdp_filter.bpf.c

| Champ | Valeur |
|-------|--------|
| **Section** | 21.2.1 : BPF Programs - XDP |
| **Description** | Filtre XDP anti-DDoS : bloque les paquets non-IPv4 et une IP spécifique |
| **Fichier source** | 02.1-bpf-programs.md |
| **Includes** | `vmlinux.h` + `<bpf/bpf_endian.h>` |
| **Compilation** | `clang -O2 -g -target bpf -D__TARGET_ARCH_x86 -c 03_xdp_filter.bpf.c -o 03_xdp_filter.bpf.o` |
| **Sortie attendue** | Fichier .bpf.o généré sans erreur |

### 04_latency_monitor.bpf.c

| Champ | Valeur |
|-------|--------|
| **Section** | 21.2.2 : BPF Maps |
| **Description** | Monitoring de latence syscall avec 3 types de maps (HASH + ARRAY + RINGBUF) |
| **Fichier source** | 02.2-bpf-maps.md |
| **Includes** | `<linux/bpf.h>` + définitions locales tracepoint |
| **Compilation** | `clang -O2 -g -target bpf -D__TARGET_ARCH_x86 -I/usr/include/x86_64-linux-gnu -c 04_latency_monitor.bpf.c -o 04_latency_monitor.bpf.o` |
| **Sortie attendue** | Fichier .bpf.o généré sans erreur |

### 05_drop_syn_flood.bpf.c

| Champ | Valeur |
|-------|--------|
| **Section** | 21.3.2 : Networking avec eBPF |
| **Description** | Programme XDP de protection DDoS : détecte et bloque les SYN floods TCP |
| **Fichier source** | 03.2-networking.md |
| **Includes** | `vmlinux.h` + `<bpf/bpf_endian.h>` |
| **Compilation** | `clang -O2 -g -target bpf -D__TARGET_ARCH_x86 -c 05_drop_syn_flood.bpf.c -o 05_drop_syn_flood.bpf.o` |
| **Sortie attendue** | Fichier .bpf.o généré sans erreur |

### 06_opensnoop_ringbuf.bpf.c

| Champ | Valeur |
|-------|--------|
| **Section** | 21.5.3 : Hooks kernel - Tracepoint |
| **Description** | Trace les ouvertures de fichiers via tracepoint + ring buffer |
| **Fichier source** | 05.3-hooks-kernel.md |
| **Includes** | `<linux/bpf.h>` + définition locale `trace_event_raw_sys_enter` |
| **Compilation** | `clang -O2 -g -target bpf -D__TARGET_ARCH_x86 -I/usr/include/x86_64-linux-gnu -c 06_opensnoop_ringbuf.bpf.c -o 06_opensnoop_ringbuf.bpf.o` |
| **Sortie attendue** | Fichier .bpf.o généré sans erreur |

### 07_tcpsnoop.bpf.c

| Champ | Valeur |
|-------|--------|
| **Section** | 21.5.3 : Hooks kernel - Kprobe |
| **Description** | Trace les envois TCP via kprobe sur tcp_sendmsg |
| **Fichier source** | 05.3-hooks-kernel.md |
| **Includes** | `vmlinux.h` + `<bpf/bpf_tracing.h>` |
| **Compilation** | `clang -O2 -g -target bpf -D__TARGET_ARCH_x86 -c 07_tcpsnoop.bpf.c -o 07_tcpsnoop.bpf.o` |
| **Sortie attendue** | Fichier .bpf.o généré sans erreur |

### 08_vfs_read_timing.bpf.c

| Champ | Valeur |
|-------|--------|
| **Section** | 21.5.3 : Hooks kernel - Kprobe + Kretprobe |
| **Description** | Mesure la durée de vfs_read via kprobe/kretprobe combinés avec map HASH |
| **Fichier source** | 05.3-hooks-kernel.md |
| **Includes** | `vmlinux.h` + `<bpf/bpf_tracing.h>` |
| **Compilation** | `clang -O2 -g -target bpf -D__TARGET_ARCH_x86 -c 08_vfs_read_timing.bpf.c -o 08_vfs_read_timing.bpf.o` |
| **Sortie attendue** | Fichier .bpf.o généré sans erreur |

### 09_fentry_tcp.bpf.c

| Champ | Valeur |
|-------|--------|
| **Section** | 21.5.3 : Hooks kernel - Fentry/Fexit |
| **Description** | Trace tcp_sendmsg avec fentry/fexit (alternative moderne aux kprobes) |
| **Fichier source** | 05.3-hooks-kernel.md |
| **Includes** | `vmlinux.h` + `<bpf/bpf_tracing.h>` |
| **Compilation** | `clang -O2 -g -target bpf -D__TARGET_ARCH_x86 -c 09_fentry_tcp.bpf.c -o 09_fentry_tcp.bpf.o` |
| **Sortie attendue** | Fichier .bpf.o généré sans erreur |

### 10_malloc_trace.bpf.c

| Champ | Valeur |
|-------|--------|
| **Section** | 21.5.4 : Hooks userspace - Uprobe/Uretprobe |
| **Description** | Trace les appels à malloc() dans libc via uprobe/uretprobe |
| **Fichier source** | 05.3-hooks-kernel.md |
| **Includes** | `vmlinux.h` + `<bpf/bpf_tracing.h>` |
| **Compilation** | `clang -O2 -g -target bpf -D__TARGET_ARCH_x86 -c 10_malloc_trace.bpf.c -o 10_malloc_trace.bpf.o` |
| **Sortie attendue** | Fichier .bpf.o généré sans erreur |

### 11_xdp_drop_icmp.bpf.c

| Champ | Valeur |
|-------|--------|
| **Section** | 21.5.6 : Hooks spécialisés - XDP |
| **Description** | Programme XDP qui droppe tous les paquets ICMP (ping) |
| **Fichier source** | 05.3-hooks-kernel.md |
| **Includes** | `vmlinux.h` + `<bpf/bpf_endian.h>` |
| **Compilation** | `clang -O2 -g -target bpf -D__TARGET_ARCH_x86 -c 11_xdp_drop_icmp.bpf.c -o 11_xdp_drop_icmp.bpf.o` |
| **Sortie attendue** | Fichier .bpf.o généré sans erreur |

### 12_lsm_file_open.bpf.c

| Champ | Valeur |
|-------|--------|
| **Section** | 21.5.6 : Hooks spécialisés - LSM |
| **Description** | Hook LSM file_open pour bloquer l'accès à certains fichiers |
| **Fichier source** | 05.3-hooks-kernel.md |
| **Includes** | `vmlinux.h` + `<bpf/bpf_tracing.h>` |
| **Compilation** | `clang -O2 -g -target bpf -D__TARGET_ARCH_x86 -c 12_lsm_file_open.bpf.c -o 12_lsm_file_open.bpf.o` |
| **Sortie attendue** | Fichier .bpf.o généré sans erreur |
| **Note** | Nécessite `CONFIG_BPF_LSM=y` dans le kernel pour être chargé |

---

## Projets multi-fichiers (13-17)

Ces projets contiennent un programme BPF kernel + un loader userspace + un Makefile.

### 13_kprobe_counter/

| Champ | Valeur |
|-------|--------|
| **Section** | 21.4.1 : libbpf - Architecture d'une application |
| **Description** | Compte les appels execve par PID via kprobe + map HASH, avec loader qui itère la map |
| **Fichier source** | 04.1-libbpf.md |
| **Fichiers** | `program.bpf.c`, `loader.c`, `Makefile` |
| **Compilation** | `make` (ou manuellement ci-dessous) |
| **BPF** | `clang -O2 -g -target bpf -I/usr/include/x86_64-linux-gnu -c program.bpf.c -o program.bpf.o` |
| **Loader** | `gcc -Wall -O2 loader.c -o loader -lbpf -lelf -lz` |
| **Exécution** | `sudo ./loader` (Ctrl-C pour arrêter) |
| **Sortie attendue** | `PID <N>: <M> execve calls` toutes les secondes |

### 14_compile_load/

| Champ | Valeur |
|-------|--------|
| **Section** | 21.4.3 : Compilation et chargement |
| **Description** | Programme BPF minimal + loader avec gestion d'erreurs libbpf_get_error |
| **Fichier source** | 04.3-compilation-chargement.md |
| **Fichiers** | `program.bpf.c`, `loader.c`, `Makefile` |
| **Compilation** | `make` |
| **BPF** | `clang -O2 -g -target bpf -I/usr/include/x86_64-linux-gnu -c program.bpf.c -o program.bpf.o` |
| **Loader** | `gcc -Wall -O2 loader.c -o loader -lbpf -lelf -lz` |
| **Exécution** | `sudo ./loader` (Ctrl-C pour arrêter) |
| **Sortie attendue** | `Successfully attached! Press Ctrl-C to stop.` + traces dans trace_pipe |

### 15_opensnoop/

| Champ | Valeur |
|-------|--------|
| **Section** | 21.4.1 : libbpf - Exemple complet |
| **Description** | Trace les ouvertures de fichiers via tracepoint sys_enter/exit_openat + ringbuffer |
| **Fichier source** | 04.1-libbpf.md |
| **Fichiers** | `opensnoop.bpf.c`, `opensnoop.c`, `Makefile` |
| **Compilation** | `make` |
| **BPF** | `clang -O2 -g -target bpf -I.. -c opensnoop.bpf.c -o opensnoop.bpf.o` |
| **Loader** | `gcc -Wall -O2 opensnoop.c -o opensnoop -lbpf -lelf -lz` |
| **Exécution** | `sudo ./opensnoop` (Ctrl-C pour arrêter) |
| **Sortie attendue** | Tableau PID/UID/COMM/RET/FILENAME en temps réel |

### 16_hello_bpf/

| Champ | Valeur |
|-------|--------|
| **Section** | 21.5.1 : Hello World - Tracing |
| **Description** | Premier programme eBPF complet avec skeleton : trace execve |
| **Fichier source** | 05.1-hello-world-tracing.md |
| **Fichiers** | `hello_bpf.bpf.c`, `hello_bpf.c`, `Makefile` |
| **Compilation** | `make` (compile BPF → génère skeleton → compile userspace) |
| **Pipeline** | 1. `clang` → `.bpf.o` 2. `bpftool gen skeleton` → `.skel.h` 3. `gcc` → binaire |
| **Exécution** | `sudo ./hello_bpf` puis dans un autre terminal : `sudo cat /sys/kernel/debug/tracing/trace_pipe` |
| **Sortie attendue** | `Programme lance : <cmd> (PID=<N>, UID=<M>)` pour chaque execve |

### 17_execcount/

| Champ | Valeur |
|-------|--------|
| **Section** | 21.5.2 : Utilisation des maps |
| **Description** | Compteur d'exécutions par UID avec skeleton + hash map |
| **Fichier source** | 05.2-utilisation-maps.md |
| **Fichiers** | `execcount.bpf.c`, `execcount.c`, `Makefile` |
| **Compilation** | `make` (compile BPF → génère skeleton → compile userspace) |
| **Pipeline** | 1. `clang` → `.bpf.o` 2. `bpftool gen skeleton` → `.skel.h` 3. `gcc` → binaire |
| **Exécution** | `sudo ./execcount` (Ctrl-C pour arrêter) |
| **Sortie attendue** | `UID <N> : <M> executions` mis à jour toutes les 2 secondes |

---

## Notes techniques

### vmlinux.h vs linux/bpf.h

| Approche | Quand l'utiliser |
|----------|-----------------|
| `vmlinux.h` | Accès aux structs kernel (ethhdr, iphdr, sock, file, pt_regs...) |
| `<linux/bpf.h>` | Programmes simples n'utilisant que les types BPF de base |

Les deux approches **ne peuvent pas** être combinées (conflits de types redéfinis).

### Structures tracepoint

Quand on utilise `<linux/bpf.h>` avec des tracepoints, les structs `trace_event_raw_sys_enter` et `trace_event_raw_sys_exit` ne sont pas disponibles. Il faut les définir localement :

```c
struct trace_event_raw_sys_enter {
    unsigned long long unused;
    long id;
    unsigned long args[6];
};

struct trace_event_raw_sys_exit {
    unsigned long long unused;
    long id;
    long ret;
};
```

### Exécution des programmes eBPF

Tous les programmes eBPF nécessitent les **privilèges root** (`sudo`) pour être chargés dans le kernel. Les traces `bpf_printk` sont visibles dans :

```bash
sudo cat /sys/kernel/debug/tracing/trace_pipe
```
