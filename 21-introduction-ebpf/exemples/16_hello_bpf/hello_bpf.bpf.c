/* ============================================================================
   Section 21.5.1 : Hello World - Programme BPF kernel
   Description : Trace les appels execve avec PID, TID, UID et nom de commande
   Fichier source : 05.1-hello-world-tracing.md
   ============================================================================ */
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

// Definition locale de la structure tracepoint (non disponible via linux/bpf.h)
struct trace_event_raw_sys_enter {
    unsigned long long unused;
    long id;
    unsigned long args[6];
};

// Definition de la licence (obligatoire pour eBPF)
char LICENSE[] SEC("license") = "GPL";

// Section qui indique ou attacher ce programme
// SEC("tp/syscalls/sys_enter_execve") signifie :
// - tp = tracepoint
// - syscalls = categorie des appels systeme
// - sys_enter_execve = point d'entree de l'appel execve
SEC("tp/syscalls/sys_enter_execve")
int trace_execve(struct trace_event_raw_sys_enter *ctx)
{
    // Recuperer l'ID du processus actuel
    __u64 pid_tgid = bpf_get_current_pid_tgid();
    __u32 pid = pid_tgid >> 32;  // Process ID
    __u32 tid = (__u32)pid_tgid;  // Thread ID

    // Recuperer l'UID (User ID) de l'utilisateur
    __u64 uid_gid = bpf_get_current_uid_gid();
    __u32 uid = uid_gid;

    // Buffer pour stocker le nom de la commande
    char comm[16];
    bpf_get_current_comm(&comm, sizeof(comm));

    // Afficher les informations via bpf_printk
    // (visible dans /sys/kernel/debug/tracing/trace_pipe)
    bpf_printk("Programme lance : %s (PID=%d, UID=%d)\n", comm, pid, uid);

    // Supprimer les warnings pour les variables non utilisees dans bpf_printk
    (void)tid;

    return 0;
}
