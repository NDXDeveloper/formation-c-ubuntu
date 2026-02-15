/* ============================================================================
   Section 21.5.1 : Hello World - Programme userspace
   Description : Charge hello_bpf via skeleton, attache au tracepoint execve
   Fichier source : 05.1-hello-world-tracing.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <bpf/libbpf.h>
#include "hello_bpf.skel.h"

// Variable globale pour gerer l'arret propre du programme
static volatile sig_atomic_t keep_running = 1;

// Gestionnaire de signal pour CTRL+C
static void sig_handler(int sig)
{
    keep_running = 0;
}

int main(int argc, char **argv)
{
    struct hello_bpf_bpf *skel;
    int err;

    // Activer le mode verbose de libbpf pour le debogage
    libbpf_set_print(NULL);

    // Installer le gestionnaire de signal
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    printf("Demarrage du traceur eBPF...\n");
    printf("Appuyez sur CTRL+C pour arreter.\n\n");

    // 1. Ouvrir le fichier BPF (genere par bpftool)
    skel = hello_bpf_bpf__open();
    if (!skel) {
        fprintf(stderr, "Erreur lors de l'ouverture du squelette BPF\n");
        return 1;
    }

    // 2. Charger et verifier le programme BPF dans le kernel
    err = hello_bpf_bpf__load(skel);
    if (err) {
        fprintf(stderr, "Erreur lors du chargement du programme BPF : %d\n", err);
        goto cleanup;
    }

    // 3. Attacher le programme au tracepoint
    err = hello_bpf_bpf__attach(skel);
    if (err) {
        fprintf(stderr, "Erreur lors de l'attachement du programme : %d\n", err);
        goto cleanup;
    }

    printf("Programme eBPF attache avec succes !\n");
    printf("Tracage des appels systeme execve en cours...\n");
    printf("Consultez /sys/kernel/debug/tracing/trace_pipe pour voir les traces\n\n");

    // Boucle principale : attendre que l'utilisateur arrete le programme
    while (keep_running) {
        sleep(1);
    }

    printf("\n\nArret du traceur...\n");

cleanup:
    // 4. Nettoyer et detacher le programme
    hello_bpf_bpf__destroy(skel);
    printf("Programme eBPF detache. Au revoir !\n");

    return err != 0;
}
