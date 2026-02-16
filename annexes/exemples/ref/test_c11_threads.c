// Ligne 174-187 : threads C11
#include <threads.h>
#include <stdio.h>

int thread_function(void* arg) {
    (void)arg;
    printf("Hello from thread!\n");
    return 0;
}

int main(void) {
    thrd_t thread;
    thrd_create(&thread, thread_function, NULL);
    thrd_join(thread, NULL);
    return 0;
}
