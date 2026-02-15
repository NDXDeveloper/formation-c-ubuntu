/* ============================================================================
   Section 21.5.3 : Hooks kernel - Tracepoint avec ringbuf
   Description : Trace les ouvertures de fichiers via tracepoint + ring buffer
   Fichier source : 05.3-hooks-kernel.md
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

char LICENSE[] SEC("license") = "GPL";

// Structure pour stocker les informations d'ouverture
struct open_event {
    __u32 pid;
    __u32 uid;
    char comm[16];
    char filename[256];
    int flags;
};

struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);
} events SEC(".maps");

SEC("tp/syscalls/sys_enter_open")
int trace_open_enter(struct trace_event_raw_sys_enter *ctx)
{
    struct open_event *event;

    // Reserver de l'espace dans le ring buffer
    event = bpf_ringbuf_reserve(&events, sizeof(*event), 0);
    if (!event) {
        return 0;
    }

    // Remplir la structure
    event->pid = bpf_get_current_pid_tgid() >> 32;
    event->uid = bpf_get_current_uid_gid();
    bpf_get_current_comm(&event->comm, sizeof(event->comm));

    // Lire le nom du fichier depuis les arguments du syscall
    bpf_probe_read_user_str(&event->filename, sizeof(event->filename),
                            (void *)ctx->args[0]);
    event->flags = ctx->args[1];

    // Envoyer l'evenement
    bpf_ringbuf_submit(event, 0);

    return 0;
}
