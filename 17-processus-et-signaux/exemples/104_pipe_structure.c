/* ============================================================================
   Section 17.8 : Pipes anonymes
   Description : Transmettre une structure via un pipe
   Fichier source : 08-pipes-anonymes.md
   ============================================================================ */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

typedef struct {
    int id;
    char name[50];
    float price;
} Product;

int main(void) {
    int pipefd[2];
    pipe(pipefd);

    if (fork() == 0) {
        /* ENFANT : Lecteur */
        close(pipefd[1]);

        Product received;
        read(pipefd[0], &received, sizeof(Product));

        printf("[Enfant] Produit recu:\n");
        printf("  ID:    %d\n", received.id);
        printf("  Nom:   %s\n", received.name);
        printf("  Prix:  %.2f EUR\n", (double)received.price);

        close(pipefd[0]);
        return 0;
    }
    else {
        /* PARENT : Emetteur */
        close(pipefd[0]);

        Product product = {
            .id = 42,
            .name = "Laptop",
            .price = 799.99f
        };

        printf("[Parent] Envoi du produit:\n");
        printf("  ID:    %d\n", product.id);
        printf("  Nom:   %s\n", product.name);
        printf("  Prix:  %.2f EUR\n", (double)product.price);

        write(pipefd[1], &product, sizeof(Product));

        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}
