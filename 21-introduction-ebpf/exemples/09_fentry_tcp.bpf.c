/* ============================================================================
   Section 21.5.3 : Hooks kernel - Fentry/Fexit
   Description : Trace tcp_sendmsg avec fentry/fexit (alternative moderne aux kprobes)
   Fichier source : 05.3-hooks-kernel.md
   ============================================================================ */
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char LICENSE[] SEC("license") = "GPL";

SEC("fentry/tcp_sendmsg")
int BPF_PROG(trace_tcp_send_entry, struct sock *sk, struct msghdr *msg, size_t size)
{
    __u32 pid = bpf_get_current_pid_tgid() >> 32;
    bpf_printk("TCP send: PID=%d, bytes=%lu\n", pid, size);
    return 0;
}

SEC("fexit/tcp_sendmsg")
int BPF_PROG(trace_tcp_send_exit, struct sock *sk, struct msghdr *msg, size_t size, int ret)
{
    __u32 pid = bpf_get_current_pid_tgid() >> 32;
    bpf_printk("TCP send done: PID=%d, ret=%d\n", pid, ret);
    return 0;
}
