/* ============================================================================
   Section 21.4.1 : libbpf - Architecture d'une application
   Description : Programme BPF kernel : compte les execve par PID via kprobe
   Fichier source : 04.1-libbpf.md
   ============================================================================ */
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

// Definir une BPF map
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __type(key, __u32);
    __type(value, __u64);
    __uint(max_entries, 1024);
} counts SEC(".maps");

// Programme eBPF
SEC("kprobe/sys_execve")
int trace_execve(struct pt_regs *ctx) {
    __u32 pid = bpf_get_current_pid_tgid() >> 32;
    __u64 *count = bpf_map_lookup_elem(&counts, &pid);

    if (count) {
        (*count)++;
    } else {
        __u64 one = 1;
        bpf_map_update_elem(&counts, &pid, &one, BPF_ANY);
    }

    return 0;
}

char LICENSE[] SEC("license") = "GPL";
