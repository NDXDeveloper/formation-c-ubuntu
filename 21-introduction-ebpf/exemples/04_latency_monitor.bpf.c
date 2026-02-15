/* ============================================================================
   Section 21.2.2 : BPF Maps - Monitoring de latence
   Description : Combine HASH + ARRAY + RINGBUF pour monitorer la latence syscall
   Fichier source : 02.2-bpf-maps.md
   ============================================================================ */
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

// Definitions locales des structures tracepoint (non disponibles via linux/bpf.h)
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

// Structure pour stocker le debut d'une operation
struct op_start {
    __u64 start_ts;  // Timestamp de debut
};

// Structure pour les statistiques de latence
struct latency_stats {
    __u64 count;
    __u64 total_ns;
    __u64 min_ns;
    __u64 max_ns;
};

// Map 1 : Stocke les timestamps de debut (HASH)
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);
    __type(key, __u32);  // PID
    __type(value, struct op_start);
} start_map SEC(".maps");

// Map 2 : Statistiques par syscall (ARRAY)
struct {
    __uint(type, BPF_MAP_TYPE_ARRAY);
    __uint(max_entries, 512);
    __type(key, __u32);  // Syscall number
    __type(value, struct latency_stats);
} stats_map SEC(".maps");

// Map 3 : Events ring buffer
struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);
} events SEC(".maps");

// Structure d'evenement pour le ring buffer
struct event {
    __u32 pid;
    __u32 syscall_id;
    __u64 duration_ns;
    char comm[16];
};

// Tracepoint : Entree dans un syscall
SEC("tracepoint/raw_syscalls/sys_enter")
int trace_enter(struct trace_event_raw_sys_enter *ctx)
{
    __u32 pid = bpf_get_current_pid_tgid() >> 32;

    // Stocker le timestamp de debut
    struct op_start info = {
        .start_ts = bpf_ktime_get_ns(),
    };
    bpf_map_update_elem(&start_map, &pid, &info, BPF_ANY);

    return 0;
}

// Tracepoint : Sortie d'un syscall
SEC("tracepoint/raw_syscalls/sys_exit")
int trace_exit(struct trace_event_raw_sys_exit *ctx)
{
    __u32 pid = bpf_get_current_pid_tgid() >> 32;
    __u32 syscall_id = ctx->id;

    // Recuperer le timestamp de debut
    struct op_start *info = bpf_map_lookup_elem(&start_map, &pid);
    if (!info)
        return 0;

    // Calculer la duree
    __u64 duration_ns = bpf_ktime_get_ns() - info->start_ts;

    // Mettre a jour les statistiques
    if (syscall_id < 512) {
        struct latency_stats *stats = bpf_map_lookup_elem(&stats_map, &syscall_id);
        if (stats) {
            __sync_fetch_and_add(&stats->count, 1);
            __sync_fetch_and_add(&stats->total_ns, duration_ns);

            // Min/Max (attention : race possible, mais acceptable)
            if (duration_ns < stats->min_ns || stats->min_ns == 0)
                stats->min_ns = duration_ns;
            if (duration_ns > stats->max_ns)
                stats->max_ns = duration_ns;
        }
    }

    // Envoyer un evenement si latence > 1ms
    if (duration_ns > 1000000) {  // 1ms
        struct event *e = bpf_ringbuf_reserve(&events, sizeof(*e), 0);
        if (e) {
            e->pid = pid;
            e->syscall_id = syscall_id;
            e->duration_ns = duration_ns;
            bpf_get_current_comm(&e->comm, sizeof(e->comm));
            bpf_ringbuf_submit(e, 0);
        }
    }

    // Nettoyer
    bpf_map_delete_elem(&start_map, &pid);

    return 0;
}

char LICENSE[] SEC("license") = "GPL";
