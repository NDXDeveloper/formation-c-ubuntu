/* ============================================================================
   Section 21.5.6 : Hooks specialises - LSM
   Description : Hook LSM file_open pour bloquer l'acces a certains fichiers
   Fichier source : 05.3-hooks-kernel.md
   ============================================================================ */
#include "vmlinux.h"
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

#define EPERM 1

char LICENSE[] SEC("license") = "GPL";

SEC("lsm/file_open")
int BPF_PROG(file_open_hook, struct file *file)
{
    // Autoriser ou refuser l'ouverture de fichiers
    // selon vos criteres

    char filename[256];
    bpf_probe_read_kernel_str(filename, sizeof(filename),
                              file->f_path.dentry->d_name.name);

    // Bloquer l'ouverture de /etc/shadow par exemple
    if (bpf_strncmp(filename, 10, "shadow") == 0) {
        return -EPERM;  // Permission denied
    }

    return 0;  // Autoriser
}
