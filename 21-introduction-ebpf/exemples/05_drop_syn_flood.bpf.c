/* ============================================================================
   Section 21.3.2 : Networking avec eBPF - Protection DDoS
   Description : Programme XDP qui detecte les paquets SYN TCP (anti SYN flood)
   Fichier source : 03.2-networking.md
   ============================================================================ */
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_endian.h>

#define ETH_P_IP    0x0800
#define IPPROTO_TCP 6

// Map pour compter les SYN par IP source
struct {
    __uint(type, BPF_MAP_TYPE_HASH);
    __uint(max_entries, 10240);
    __type(key, __u32);    // IP source
    __type(value, __u64);  // Compteur SYN
} syn_count SEC(".maps");

// Seuil de SYN par IP (configurable)
#define SYN_THRESHOLD 100

SEC("xdp")
int drop_syn_flood(struct xdp_md *ctx)
{
    void *data = (void *)(long)ctx->data;
    void *data_end = (void *)(long)ctx->data_end;

    struct ethhdr *eth = data;
    if ((void *)(eth + 1) > data_end)
        return XDP_PASS;

    if (eth->h_proto != bpf_htons(ETH_P_IP))
        return XDP_PASS;

    struct iphdr *iph = (void *)(eth + 1);
    if ((void *)(iph + 1) > data_end)
        return XDP_PASS;

    if (iph->protocol != IPPROTO_TCP)
        return XDP_PASS;

    struct tcphdr *tcph = (void *)(iph + 1);
    if ((void *)(tcph + 1) > data_end)
        return XDP_PASS;

    // Si c'est un SYN (nouveau) sans ACK
    if (tcph->syn && !tcph->ack) {
        __u32 src_ip = iph->saddr;

        // Incrementer compteur dans la BPF map
        __u64 *count = bpf_map_lookup_elem(&syn_count, &src_ip);
        if (count) {
            __sync_fetch_and_add(count, 1);
            if (*count > SYN_THRESHOLD)
                return XDP_DROP;
        } else {
            __u64 init = 1;
            bpf_map_update_elem(&syn_count, &src_ip, &init, BPF_ANY);
        }
    }

    return XDP_PASS;
}

char LICENSE[] SEC("license") = "GPL";
