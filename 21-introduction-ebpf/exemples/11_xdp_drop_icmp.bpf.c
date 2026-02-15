/* ============================================================================
   Section 21.5.6 : Hooks specialises - XDP
   Description : Programme XDP qui droppe tous les paquets ICMP (ping)
   Fichier source : 05.3-hooks-kernel.md
   ============================================================================ */
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#define ETH_P_IP    0x0800
#define IPPROTO_ICMP 1

char LICENSE[] SEC("license") = "GPL";

SEC("xdp")
int xdp_drop_icmp(struct xdp_md *ctx)
{
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    struct ethhdr *eth = data;

    // Verifier qu'on a assez de donnees
    if ((void *)(eth + 1) > data_end) {
        return XDP_PASS;
    }

    // Dropper tous les paquets ICMP
    if (eth->h_proto == bpf_htons(ETH_P_IP)) {
        struct iphdr *ip = (void *)(eth + 1);
        if ((void *)(ip + 1) > data_end) {
            return XDP_PASS;
        }

        if (ip->protocol == IPPROTO_ICMP) {
            return XDP_DROP;  // Drop le paquet
        }
    }

    return XDP_PASS;  // Laisser passer le paquet
}
