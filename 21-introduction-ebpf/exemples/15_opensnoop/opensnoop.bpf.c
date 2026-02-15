/* ============================================================================
   Section 21.4.1 : libbpf - Exemple complet opensnoop
   Description : Trace les ouvertures de fichiers via tracepoint sys_enter/exit_openat
   Fichier source : 04.1-libbpf.md
   ============================================================================ */
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>
#include <bpf/bpf_core_read.h>

// Structure pour les evenements
struct event {
    u32 pid;
    u32 uid;
    int ret;
    char comm[16];
    char filename[256];
};

// Ringbuffer pour communiquer avec l'userspace
struct {
    __uint(type, BPF_MAP_TYPE_RINGBUF);
    __uint(max_entries, 256 * 1024);
} events SEC(".maps");

// Map temporaire pour stocker les arguments
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __type(key, u32);
    __type(value, const char *);
    __uint(max_entries, 10240);
} tmp_filename SEC(".maps");

// Hook a l'entree de sys_openat
SEC("tracepoint/syscalls/sys_enter_openat")
int trace_enter_openat(struct trace_event_raw_sys_enter *ctx) {
    u64 id = bpf_get_current_pid_tgid();
    u32 pid = id >> 32;

    // Sauvegarder le pointeur vers le filename
    const char *filename = (const char *)ctx->args[1];
    bpf_map_update_elem(&tmp_filename, &pid, &filename, BPF_ANY);

    return 0;
}

// Hook a la sortie de sys_openat
SEC("tracepoint/syscalls/sys_exit_openat")
int trace_exit_openat(struct trace_event_raw_sys_exit *ctx) {
    u64 id = bpf_get_current_pid_tgid();
    u32 pid = id >> 32;
    u32 uid = bpf_get_current_uid_gid();

    // Recuperer le filename sauvegarde
    const char **filename_ptr = bpf_map_lookup_elem(&tmp_filename, &pid);
    if (!filename_ptr)
        return 0;

    // Reserver de l'espace dans le ringbuffer
    struct event *e = bpf_ringbuf_reserve(&events, sizeof(*e), 0);
    if (!e)
        goto cleanup;

    // Remplir l'evenement
    e->pid = pid;
    e->uid = uid;
    e->ret = ctx->ret;

    bpf_get_current_comm(&e->comm, sizeof(e->comm));
    bpf_probe_read_user_str(&e->filename, sizeof(e->filename), *filename_ptr);

    // Soumettre l'evenement
    bpf_ringbuf_submit(e, 0);

cleanup:
    bpf_map_delete_elem(&tmp_filename, &pid);
    return 0;
}

char LICENSE[] SEC("license") = "GPL";
