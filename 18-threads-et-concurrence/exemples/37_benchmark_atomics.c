/* ============================================================================
   Section 18.10.6 : Performance et cas d'usage
   Description : Benchmark comparatif relaxed / seq_cst / mutex
   Fichier source : 10.6-performance-cas-usage.md
   ============================================================================ */

#include <stdio.h>
#include <stdatomic.h>
#include <pthread.h>
#include <time.h>

#define NB_THREADS 4
#define NB_OPS 1000000

typedef struct {
    double temps_sec;
    long ops_par_sec;
} resultat_bench_t;

atomic_int counter_relaxed = 0;
atomic_int counter_seq_cst = 0;
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;
int counter_mutex = 0;

void *bench_relaxed(void *arg) {
    (void)arg;
    for (int i = 0; i < NB_OPS; i++) {
        atomic_fetch_add_explicit(&counter_relaxed, 1, memory_order_relaxed);
    }
    return NULL;
}

void *bench_seq_cst(void *arg) {
    (void)arg;
    for (int i = 0; i < NB_OPS; i++) {
        atomic_fetch_add(&counter_seq_cst, 1);
    }
    return NULL;
}

void *bench_mutex(void *arg) {
    (void)arg;
    for (int i = 0; i < NB_OPS; i++) {
        pthread_mutex_lock(&mutex_lock);
        counter_mutex++;
        pthread_mutex_unlock(&mutex_lock);
    }
    return NULL;
}

resultat_bench_t run_benchmark(void *(*fonction)(void *)) {
    pthread_t threads[NB_THREADS];
    struct timespec debut, fin;

    clock_gettime(CLOCK_MONOTONIC, &debut);

    for (int i = 0; i < NB_THREADS; i++) {
        pthread_create(&threads[i], NULL, fonction, NULL);
    }

    for (int i = 0; i < NB_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &fin);

    double temps = (double)(fin.tv_sec - debut.tv_sec) +
                   (double)(fin.tv_nsec - debut.tv_nsec) / 1e9;

    long total_ops = (long)NB_THREADS * NB_OPS;

    resultat_bench_t res = {
        .temps_sec = temps,
        .ops_par_sec = (long)((double)total_ops / temps)
    };

    return res;
}

int main(void) {
    printf("=== Benchmark des methodes de comptage ===\n");
    printf("(%d threads, %d ops/thread)\n\n", NB_THREADS, NB_OPS);

    resultat_bench_t r1 = run_benchmark(bench_relaxed);
    printf("Atomique relaxed:  %.3f sec  (%ld ops/sec)\n",
           r1.temps_sec, r1.ops_par_sec);

    resultat_bench_t r2 = run_benchmark(bench_seq_cst);
    printf("Atomique seq_cst:  %.3f sec  (%ld ops/sec)\n",
           r2.temps_sec, r2.ops_par_sec);

    resultat_bench_t r3 = run_benchmark(bench_mutex);
    printf("Mutex:             %.3f sec  (%ld ops/sec)\n",
           r3.temps_sec, r3.ops_par_sec);

    printf("\nSpeedup relaxed/mutex: %.1fx\n",
           (double)r1.ops_par_sec / (double)r3.ops_par_sec);

    return 0;
}
