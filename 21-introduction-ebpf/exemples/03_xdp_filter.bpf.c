/* ============================================================================
   Section 21.2.1 : BPF Programs - XDP
   Description : Filtrage anti-DDoS simple avec XDP (bloque non-IPv4 et IP)
   Fichier source : 02.1-bpf-programs.md
   ============================================================================ */
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#define ETH_P_IP 0x0800

SEC("xdp")
int xdp_filter(struct xdp_md *ctx)
{
    void *data_end = (void *)(long)ctx->data_end;
    void *data = (void *)(long)ctx->data;

    struct ethhdr *eth = data;

    // Verifier qu'on a assez de donnees
    if ((void *)(eth + 1) > data_end)
        return XDP_PASS;

    // Bloquer tous les paquets non-IPv4
    if (eth->h_proto != bpf_htons(ETH_P_IP))
        return XDP_DROP;

    struct iphdr *ip = (void *)(eth + 1);
    if ((void *)(ip + 1) > data_end)
        return XDP_PASS;

    // Bloquer une IP specifique (exemple: 192.168.1.100)
    if (ip->saddr == bpf_htonl(0xC0A80164))  // 192.168.1.100 en hex
        return XDP_DROP;

    return XDP_PASS;
}

char LICENSE[] SEC("license") = "GPL";
