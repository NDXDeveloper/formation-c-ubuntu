/* ============================================================================
   Section 21.5.4 : Hooks userspace - Uprobe/Uretprobe
   Description : Trace les appels a malloc() dans libc via uprobe/uretprobe
   Fichier source : 05.3-hooks-kernel.md
   ============================================================================ */
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char LICENSE[] SEC("license") = "GPL";

// Attacher a la fonction malloc de la libc
SEC("uprobe//lib/x86_64-linux-gnu/libc.so.6:malloc")
int trace_malloc_enter(struct pt_regs *ctx)
{
    __u64 size = PT_REGS_PARM1(ctx);  // Premier argument = taille
    __u32 pid = bpf_get_current_pid_tgid() >> 32;

    bpf_printk("malloc: PID=%d, size=%lu\n", pid, size);
    return 0;
}

// Tracer le retour de malloc (pour avoir l'adresse allouee)
SEC("uretprobe//lib/x86_64-linux-gnu/libc.so.6:malloc")
int trace_malloc_exit(struct pt_regs *ctx)
{
    void *ptr = (void *)PT_REGS_RC(ctx);  // Valeur de retour
    __u32 pid = bpf_get_current_pid_tgid() >> 32;

    bpf_printk("malloc returned: PID=%d, ptr=%p\n", pid, ptr);
    return 0;
}
