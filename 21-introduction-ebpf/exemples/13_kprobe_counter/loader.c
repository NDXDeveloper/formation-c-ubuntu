/* ============================================================================
   Section 21.4.1 : libbpf - Loader userspace
   Description : Charge program.bpf.o, attache le kprobe et lit la map counts
   Fichier source : 04.1-libbpf.md
   ============================================================================ */
#include <stdio.h>
#include <unistd.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>

int main(int argc, char **argv) {
    struct bpf_object *obj;
    struct bpf_program *prog;
    struct bpf_link *link;
    int map_fd;

    // 1. Ouvrir le fichier objet eBPF
    obj = bpf_object__open_file("program.bpf.o", NULL);
    if (!obj) {
        fprintf(stderr, "Failed to open BPF object\n");
        return 1;
    }

    // 2. Charger dans le kernel
    if (bpf_object__load(obj)) {
        fprintf(stderr, "Failed to load BPF object\n");
        return 1;
    }

    // 3. Trouver le programme
    prog = bpf_object__find_program_by_name(obj, "trace_execve");
    if (!prog) {
        fprintf(stderr, "Failed to find program\n");
        return 1;
    }

    // 4. Attacher au hook
    link = bpf_program__attach(prog);
    if (!link) {
        fprintf(stderr, "Failed to attach program\n");
        return 1;
    }

    printf("Successfully started! Press Ctrl-C to stop.\n");

    // 5. Obtenir le file descriptor de la map
    map_fd = bpf_object__find_map_fd_by_name(obj, "counts");

    // 6. Boucle principale : lire la map toutes les secondes
    while (1) {
        sleep(1);

        __u32 key, next_key;
        __u64 value;

        key = 0;
        while (bpf_map_get_next_key(map_fd, &key, &next_key) == 0) {
            bpf_map_lookup_elem(map_fd, &next_key, &value);
            printf("PID %u: %llu execve calls\n", next_key, value);
            key = next_key;
        }
    }

    // 7. Cleanup (jamais atteint ici, mais bonne pratique)
    bpf_link__destroy(link);
    bpf_object__close(obj);

    return 0;
}
