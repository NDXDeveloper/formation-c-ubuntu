/* ============================================================================
   Section 21.5.3 : Hooks kernel - Kprobe
   Description : Trace les envois TCP via kprobe sur tcp_sendmsg
   Fichier source : 05.3-hooks-kernel.md
   ============================================================================ */
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

char LICENSE[] SEC("license") = "GPL";

// Attacher a la fonction tcp_sendmsg du kernel
SEC("kprobe/tcp_sendmsg")
int BPF_KPROBE(trace_tcp_send, struct sock *sk, struct msghdr *msg, size_t size)
{
    __u32 pid = bpf_get_current_pid_tgid() >> 32;

    // sk contient les informations de la socket
    // size contient le nombre d'octets envoyes

    bpf_printk("TCP send: PID=%d, bytes=%lu\n", pid, size);

    return 0;
}
