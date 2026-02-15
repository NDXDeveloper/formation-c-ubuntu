/* ============================================================================
   Section 21.4 : Toolchain eBPF
   Description : Programme BPF minimal pour tester la toolchain
   Fichier source : 04-toolchain-ebpf.md
   ============================================================================ */
#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

SEC("kprobe/sys_clone")
int test(void *ctx) {
    return 0;
}

char LICENSE[] SEC("license") = "GPL";
