// Ligne 807-821 : alarm handler (modifié pour test rapide)
#include <signal.h>
#include <unistd.h>
#include <stdio.h>

void alarm_handler(int sig) {
    printf("Temps écoulé ! (signal %d)\n", sig);
}

int main() {
    signal(SIGALRM, alarm_handler);
    alarm(1);  // SIGALRM dans 1 seconde (réduit de 5 pour le test)

    pause();  // Attendre
    return 0;
}
