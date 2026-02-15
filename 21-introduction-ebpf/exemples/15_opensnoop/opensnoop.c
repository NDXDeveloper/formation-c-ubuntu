/* ============================================================================
   Section 21.4.1 : libbpf - Loader opensnoop avec ringbuffer
   Description : Charge opensnoop.bpf.o et affiche les ouvertures de fichiers
   Fichier source : 04.1-libbpf.md
   ============================================================================ */
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <errno.h>
#include <sys/resource.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>

// Structure identique au code kernel
struct event {
    unsigned int pid;
    unsigned int uid;
    int ret;
    char comm[16];
    char filename[256];
};

static volatile bool exiting = false;

static void sig_handler(int sig) {
    exiting = true;
}

// Callback pour traiter les evenements du ringbuffer
static int handle_event(void *ctx, void *data, size_t data_sz) {
    const struct event *e = data;

    printf("%-6d %-6d %-16s %-4d %s\n",
           e->pid, e->uid, e->comm, e->ret, e->filename);

    return 0;
}

int main(int argc, char **argv) {
    struct bpf_object *obj;
    struct ring_buffer *rb = NULL;
    int err;

    // Augmenter la limite de memoire verrouillee
    struct rlimit rlim = {
        .rlim_cur = RLIM_INFINITY,
        .rlim_max = RLIM_INFINITY,
    };
    setrlimit(RLIMIT_MEMLOCK, &rlim);

    // Gerer Ctrl-C proprement
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    // Ouvrir et charger le programme eBPF
    obj = bpf_object__open_file("opensnoop.bpf.o", NULL);
    if (!obj) {
        fprintf(stderr, "Failed to open BPF object\n");
        return 1;
    }

    err = bpf_object__load(obj);
    if (err) {
        fprintf(stderr, "Failed to load BPF object: %d\n", err);
        goto cleanup;
    }

    // Attacher tous les programmes
    struct bpf_program *prog;
    bpf_object__for_each_program(prog, obj) {
        struct bpf_link *link = bpf_program__attach(prog);
        if (!link) {
            fprintf(stderr, "Failed to attach: %s\n", bpf_program__name(prog));
            err = -1;
            goto cleanup;
        }
    }

    // Creer le ringbuffer reader
    int map_fd = bpf_object__find_map_fd_by_name(obj, "events");
    rb = ring_buffer__new(map_fd, handle_event, NULL, NULL);
    if (!rb) {
        fprintf(stderr, "Failed to create ring buffer\n");
        goto cleanup;
    }

    printf("%-6s %-6s %-16s %-4s %s\n",
           "PID", "UID", "COMM", "RET", "FILENAME");

    // Boucle principale
    while (!exiting) {
        err = ring_buffer__poll(rb, 100 /* timeout ms */);
        if (err == -EINTR) {
            err = 0;
            break;
        }
        if (err < 0) {
            fprintf(stderr, "Error polling ring buffer: %d\n", err);
            break;
        }
    }

cleanup:
    ring_buffer__free(rb);
    bpf_object__close(obj);
    return err != 0;
}
