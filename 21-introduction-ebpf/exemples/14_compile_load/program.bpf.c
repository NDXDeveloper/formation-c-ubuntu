/* ============================================================================
   Section 21.4.3 : Compilation et chargement - Programme minimal
   Description : Programme eBPF minimal pour illustrer le pipeline de compilation
   Fichier source : 04.3-compilation-chargement.md
   ============================================================================ */
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

// Section speciale qui indique le type de programme et ou l'attacher
SEC("kprobe/sys_execve")
int trace_execve(struct pt_regs *ctx) {
    // Logique du programme
    bpf_printk("execve() called!\n");
    return 0;
}

// Licence obligatoire (GPL pour acceder a certaines fonctions kernel)
char LICENSE[] SEC("license") = "GPL";
