/* ============================================================================
   Section 25.6 : Logging
   Description : Utilisation de syslog (POSIX) pour le logging systeme
   Fichier source : 06-logging.md
   ============================================================================ */

#include <stdio.h>
#include <syslog.h>
#include <errno.h>
#include <string.h>

int main(void) {
    /* Ouvrir la connexion syslog */
    openlog("mon_app", LOG_PID | LOG_CONS, LOG_USER);

    syslog(LOG_INFO, "Application demarree");
    syslog(LOG_DEBUG, "Mode debug actif");
    syslog(LOG_WARNING, "Avertissement de test");

    /* Simuler une erreur */
    FILE *f = fopen("inexistant.txt", "r");
    if (f == NULL) {
        syslog(LOG_ERR, "Erreur ouverture fichier: %s", strerror(errno));
    } else {
        fclose(f);
    }

    syslog(LOG_INFO, "Application terminee");

    /* Fermer */
    closelog();

    printf("Messages envoyes au syslog\n");
    printf("Verifier avec : journalctl --user -t mon_app --no-pager\n");

    return 0;
}
