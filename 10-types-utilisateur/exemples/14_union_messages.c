/* ============================================================================
   Section 10.4 : union - Optimisation memoire
   Description : Systeme de messages avec types differents via union
   Fichier source : 04-unions.md
   ============================================================================ */

#include <stdio.h>
#include <string.h>

enum TypeMessage {
    MSG_TEXTE,
    MSG_NOMBRE,
    MSG_ERREUR
};

struct Message {
    enum TypeMessage type;
    union {
        char texte[256];
        int nombre;
        struct {
            int code;
            char description[128];
        } erreur;
    } contenu;
};

void traiter_message(struct Message *msg) {
    switch (msg->type) {
        case MSG_TEXTE:
            printf("[TEXTE] %s\n", msg->contenu.texte);
            break;
        case MSG_NOMBRE:
            printf("[NOMBRE] %d\n", msg->contenu.nombre);
            break;
        case MSG_ERREUR:
            printf("[ERREUR %d] %s\n",
                   msg->contenu.erreur.code,
                   msg->contenu.erreur.description);
            break;
    }
}

int main(void) {
    struct Message msg1 = {MSG_TEXTE, {{0}}};
    strcpy(msg1.contenu.texte, "Connexion etablie");
    traiter_message(&msg1);

    struct Message msg2 = {MSG_NOMBRE, {.nombre = 42}};
    traiter_message(&msg2);

    struct Message msg3 = {MSG_ERREUR, {{0}}};
    msg3.contenu.erreur.code = 404;
    strcpy(msg3.contenu.erreur.description, "Fichier introuvable");
    traiter_message(&msg3);

    return 0;
}
