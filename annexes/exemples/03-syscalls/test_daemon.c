// Ligne 1545-1592 : daemon (modifié pour test - juste compiler)
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>

void daemonize() {
    // 1. Fork et terminer le parent
    pid_t pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0);  // Parent se termine

    // 2. Créer une nouvelle session
    if (setsid() < 0) exit(1);

    // 3. Fork à nouveau pour ne pas être session leader
    pid = fork();
    if (pid < 0) exit(1);
    if (pid > 0) exit(0);

    // 4. Changer le répertoire courant
    chdir("/");

    // 5. Fermer les descripteurs standard
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    // 6. Rediriger vers /dev/null
    open("/dev/null", O_RDONLY);  // stdin
    open("/dev/null", O_WRONLY);  // stdout
    open("/dev/null", O_WRONLY);  // stderr

    // 7. Définir umask
    umask(0);
}

int main() {
    // Ne pas lancer le daemon pour le test - juste compiler
    // daemonize();
    return 0;
}
