/* ============================================================================
   Section 21.4.3 : Compilation et chargement - Loader minimal
   Description : Charge program.bpf.o et attache le kprobe avec gestion d'erreurs
   Fichier source : 04.3-compilation-chargement.md
   ============================================================================ */
#include <stdio.h>
#include <unistd.h>
#include <bpf/libbpf.h>

int main(void) {
    struct bpf_object *obj;
    struct bpf_program *prog;
    struct bpf_link *link;
    int err;

    // 1. Ouvrir le fichier objet
    obj = bpf_object__open_file("program.bpf.o", NULL);
    if (libbpf_get_error(obj)) {
        fprintf(stderr, "Failed to open BPF object\n");
        return 1;
    }

    // 2. Charger dans le kernel
    err = bpf_object__load(obj);
    if (err) {
        fprintf(stderr, "Failed to load BPF object: %d\n", err);
        goto cleanup;
    }

    // 3. Trouver le programme
    prog = bpf_object__find_program_by_name(obj, "trace_execve");
    if (!prog) {
        fprintf(stderr, "Failed to find program\n");
        goto cleanup;
    }

    // 4. Attacher
    link = bpf_program__attach(prog);
    if (libbpf_get_error(link)) {
        fprintf(stderr, "Failed to attach program\n");
        goto cleanup;
    }

    printf("Successfully attached! Press Ctrl-C to stop.\n");

    // 5. Garder actif
    pause();  // ou while(1) sleep(1);

    // 6. Cleanup
    bpf_link__destroy(link);
cleanup:
    bpf_object__close(obj);
    return err != 0;
}
