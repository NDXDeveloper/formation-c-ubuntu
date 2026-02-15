/* ============================================================================
   Section 21.5.2 : Utilisation des maps - Programme userspace
   Description : Charge execcount via skeleton, affiche les stats par UID
   Fichier source : 05.2-utilisation-maps.md
   ============================================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <bpf/libbpf.h>
#include <bpf/bpf.h>
#include "execcount.skel.h"

static volatile sig_atomic_t keep_running = 1;

static void sig_handler(int sig)
{
    keep_running = 0;
}

int main(int argc, char **argv)
{
    struct execcount_bpf *skel;
    int err;

    // Installer le gestionnaire de signal
    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);

    printf("=== Compteur d'executions par utilisateur ===\n");
    printf("Appuyez sur CTRL+C pour arreter.\n\n");

    // 1. Ouvrir le squelette BPF
    skel = execcount_bpf__open();
    if (!skel) {
        fprintf(stderr, "Erreur lors de l'ouverture du squelette BPF\n");
        return 1;
    }

    // 2. Charger le programme dans le kernel
    err = execcount_bpf__load(skel);
    if (err) {
        fprintf(stderr, "Erreur lors du chargement : %d\n", err);
        goto cleanup;
    }

    // 3. Attacher le programme au tracepoint
    err = execcount_bpf__attach(skel);
    if (err) {
        fprintf(stderr, "Erreur lors de l'attachement : %d\n", err);
        goto cleanup;
    }

    printf("Programme eBPF attache avec succes !\n");
    printf("Comptage des executions en cours...\n\n");

    // Boucle principale : afficher les statistiques toutes les 2 secondes
    while (keep_running) {
        sleep(2);

        printf("\n=== Statistiques (mises a jour toutes les 2s) ===\n");

        // Recuperer le file descriptor de la map
        int map_fd = bpf_map__fd(skel->maps.exec_count);

        // Parcourir toutes les entrees de la map
        __u32 key = 0;
        __u32 next_key;
        __u64 value;

        while (bpf_map_get_next_key(map_fd, &key, &next_key) == 0) {
            // Lire la valeur associee a next_key
            if (bpf_map_lookup_elem(map_fd, &next_key, &value) == 0) {
                printf("  UID %5u : %llu executions\n", next_key, value);
            }
            key = next_key;
        }

        printf("---------------------------------------------\n");
    }

    printf("\n\nArret du compteur...\n");

cleanup:
    execcount_bpf__destroy(skel);
    printf("Programme detache. Au revoir !\n");

    return err != 0;
}
