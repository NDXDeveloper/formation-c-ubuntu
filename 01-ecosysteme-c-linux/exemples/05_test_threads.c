/* ============================================================================
   Section 1.3.2 : Verification du support compilateur
   Description : Test du support des threads natifs (C11)
   Fichier source : 03.2-verification-support-compilateur.md
   ============================================================================ */
#include <stdio.h>
#include <threads.h>

int thread_func(void *arg) {
    (void)arg;
    printf("Thread C11 fonctionne !\n");
    return 0;
}

int main(void) {
    thrd_t thread;
    thrd_create(&thread, thread_func, NULL);
    thrd_join(thread, NULL);
    return 0;
}
