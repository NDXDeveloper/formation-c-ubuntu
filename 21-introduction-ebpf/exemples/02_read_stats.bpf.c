/* ============================================================================
   Section 21.2.1 : BPF Programs - Exemple complet
   Description : Compte les octets lus par chaque processus via sys_exit_read
   Fichier source : 02.1-bpf-programs.md
   ============================================================================ */
// SPDX-License-Identifier: GPL-2.0
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

// Definition locale de la structure tracepoint (non disponible via linux/bpf.h)
struct trace_event_raw_sys_exit {
    unsigned long long unused;
    long id;
    long ret;
};

// Structure pour stocker les statistiques par processus
struct stats {
    __u64 count;        // Nombre d'appels read()
    __u64 total_bytes;  // Total d'octets lus
};

// Map : PID -> Stats
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);  // Max 10240 processus
    __type(key, __u32);           // Cle = PID
    __type(value, struct stats);  // Valeur = statistiques
} read_stats SEC(".maps");

// Programme attache a l'appel systeme read()
SEC("tracepoint/syscalls/sys_exit_read")
int trace_read_exit(struct trace_event_raw_sys_exit *ctx)
{
    // Recuperer le PID du processus actuel
    __u64 pid_tgid = bpf_get_current_pid_tgid();
    __u32 pid = pid_tgid >> 32;  // Les 32 bits de poids fort = PID

    // Recuperer la valeur de retour (nombre d'octets lus)
    long bytes_read = ctx->ret;

    // Ignorer les erreurs (ret < 0)
    if (bytes_read < 0)
        return 0;

    // Chercher les stats existantes
    struct stats *stat = bpf_map_lookup_elem(&read_stats, &pid);

    if (stat) {
        // Mettre a jour les stats existantes
        __sync_fetch_and_add(&stat->count, 1);
        __sync_fetch_and_add(&stat->total_bytes, bytes_read);
    } else {
        // Creer une nouvelle entree
        struct stats new_stat = {
            .count = 1,
            .total_bytes = bytes_read,
        };
        bpf_map_update_elem(&read_stats, &pid, &new_stat, BPF_ANY);
    }

    return 0;
}

// Licence GPL obligatoire
char LICENSE[] SEC("license") = "GPL";
