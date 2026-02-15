/* ============================================================================
   Section 18.3 : Passage de parametres
   Description : Communication bidirectionnelle - transformation de message
   Fichier source : 03-passage-parametres.md
   ============================================================================ */

#include <stdio.h>
#include <pthread.h>
#include <string.h>

typedef struct {
    char message[100];
    int longueur;
    int modifie;
} MessageData;

void *transformer_message(void *arg) {
    MessageData *data = (MessageData *)arg;

    for (int i = 0; i < data->longueur; i++) {
        if (data->message[i] >= 'a' && data->message[i] <= 'z') {
            data->message[i] = (char)(data->message[i] - 'a' + 'A');
        }
    }

    data->modifie = 1;
    return NULL;
}

int main(void) {
    pthread_t tid;
    MessageData data;

    strcpy(data.message, "hello world");
    data.longueur = (int)strlen(data.message);
    data.modifie = 0;

    printf("Avant : %s\n", data.message);

    pthread_create(&tid, NULL, transformer_message, &data);
    pthread_join(tid, NULL);

    printf("Apres : %s\n", data.message);
    printf("Modifie : %s\n", data.modifie ? "Oui" : "Non");

    return 0;
}
