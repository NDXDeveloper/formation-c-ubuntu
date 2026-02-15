/* ============================================================================
   Section 21.5.2 : Utilisation des maps - Programme BPF kernel
   Description : Compte les executions (execve) par UID via hash map
   Fichier source : 05.2-utilisation-maps.md
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

// Definition de la map : Hash table pour compter les executions par UID
struct {
    __uint(type, BPF_MAP_TYPE_HASH);     // Type : Hash map
    __uint(max_entries, 10240);           // Maximum 10240 utilisateurs
    __type(key, __u32);                   // Cle = UID (User ID)
    __type(value, __u64);                 // Valeur = Compteur d'executions
} exec_count SEC(".maps");

// Programme eBPF attache a l'appel systeme execve
SEC("tp/syscalls/sys_enter_execve")
int count_execve(struct trace_event_raw_sys_enter *ctx)
{
    __u64 uid_gid = bpf_get_current_uid_gid();
    __u32 uid = uid_gid;  // Extraire l'UID (32 bits bas)

    // Chercher si cet UID existe deja dans la map
    __u64 *count = bpf_map_lookup_elem(&exec_count, &uid);

    if (count) {
        // L'UID existe deja : incrementer le compteur
        __sync_fetch_and_add(count, 1);
    } else {
        // Nouvel UID : initialiser le compteur a 1
        __u64 initial_count = 1;
        bpf_map_update_elem(&exec_count, &uid, &initial_count, BPF_ANY);
    }

    return 0;
}
