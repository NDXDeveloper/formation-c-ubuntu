/* ============================================================================
   Section 18.13 : Barrieres de threads
   Description : Simulation iterative de grille avec barrieres entre phases
   Fichier source : 13-barrieres-threads.md
   ============================================================================ */

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define GRID_SIZE 100
#define NUM_THREADS 4
#define ITERATIONS 10

double grille[GRID_SIZE][GRID_SIZE];
double nouvelle_grille[GRID_SIZE][GRID_SIZE];
pthread_barrier_t barriere;

void initialiser_grille(void) {
    for (int i = 0; i < GRID_SIZE; i++) {
        for (int j = 0; j < GRID_SIZE; j++) {
            grille[i][j] = (double)rand() / (double)RAND_MAX * 100.0;
        }
    }
}

double calculer_moyenne(int i, int j) {
    double somme = 0.0;
    int count = 0;

    for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
            int ni = i + di;
            int nj = j + dj;

            if (ni >= 0 && ni < GRID_SIZE &&
                nj >= 0 && nj < GRID_SIZE) {
                somme += grille[ni][nj];
                count++;
            }
        }
    }

    return somme / count;
}

void *simuler_region(void *arg) {
    int id = *(int *)arg;
    int debut = id * (GRID_SIZE / NUM_THREADS);
    int fin = (id + 1) * (GRID_SIZE / NUM_THREADS);

    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (int i = debut; i < fin; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                nouvelle_grille[i][j] = calculer_moyenne(i, j);
            }
        }

        pthread_barrier_wait(&barriere);

        for (int i = debut; i < fin; i++) {
            for (int j = 0; j < GRID_SIZE; j++) {
                grille[i][j] = nouvelle_grille[i][j];
            }
        }

        int result = pthread_barrier_wait(&barriere);

        if (result == PTHREAD_BARRIER_SERIAL_THREAD) {
            printf("Iteration %d terminee\n", iter + 1);
        }
    }

    return NULL;
}

int main(void) {
    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    initialiser_grille();

    pthread_barrier_init(&barriere, NULL, NUM_THREADS);

    printf("Demarrage simulation (%dx%d, %d threads)\n",
           GRID_SIZE, GRID_SIZE, NUM_THREADS);

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, simuler_region, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_barrier_destroy(&barriere);

    printf("Simulation terminee\n");

    return 0;
}
