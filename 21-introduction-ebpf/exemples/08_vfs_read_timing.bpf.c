/* ============================================================================
   Section 21.5.3 : Hooks kernel - Kprobe + Kretprobe
   Description : Mesure la duree de vfs_read via kprobe/kretprobe combines
   Fichier source : 05.3-hooks-kernel.md
   ============================================================================ */
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char LICENSE[] SEC("license") = "GPL";

struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);
    __type(key, __u32);    // PID
    __type(value, __u64);  // Timestamp de debut
} start_times SEC(".maps");

SEC("kprobe/vfs_read")
int BPF_KPROBE(trace_read_enter)
{
    __u64 pid_tgid = bpf_get_current_pid_tgid();
    __u32 pid = pid_tgid >> 32;
    __u64 ts = bpf_ktime_get_ns();

    bpf_map_update_elem(&start_times, &pid, &ts, BPF_ANY);
    return 0;
}

SEC("kretprobe/vfs_read")
int BPF_KRETPROBE(trace_read_exit, ssize_t ret)
{
    __u64 pid_tgid = bpf_get_current_pid_tgid();
    __u32 pid = pid_tgid >> 32;
    __u64 *start_ts;

    start_ts = bpf_map_lookup_elem(&start_times, &pid);
    if (!start_ts) {
        return 0;
    }

    __u64 duration_ns = bpf_ktime_get_ns() - *start_ts;
    __u64 duration_us = duration_ns / 1000;

    bpf_printk("vfs_read: PID=%d, duration=%llu us, bytes=%ld\n",
               pid, duration_us, ret);

    bpf_map_delete_elem(&start_times, &pid);
    return 0;
}
