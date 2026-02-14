/* ============================================================================
   Section 4.5 : Endianness et portabilite
   Description : Portabilite reseau avec htons/htonl/ntohs/ntohl
   Fichier source : 05-endianness.md
   ============================================================================ */
#include <stdio.h>
#include <stdint.h>
#include <arpa/inet.h>

/* Simulation d'un paquet reseau */
struct Paquet {
    uint16_t type;
    uint16_t longueur;
    uint32_t id_session;
    uint32_t timestamp;
};

/* Preparation pour envoi (host -> network) */
void preparer_envoi(struct Paquet* p) {
    p->type = htons(p->type);
    p->longueur = htons(p->longueur);
    p->id_session = htonl(p->id_session);
    p->timestamp = htonl(p->timestamp);
}

/* Traitement apres reception (network -> host) */
void traiter_reception(struct Paquet* p) {
    p->type = ntohs(p->type);
    p->longueur = ntohs(p->longueur);
    p->id_session = ntohl(p->id_session);
    p->timestamp = ntohl(p->timestamp);
}

int main(void) {
    /* Creation d'un paquet */
    struct Paquet paquet_hote = {
        .type = 0x0001,
        .longueur = 100,
        .id_session = 0x12345678,
        .timestamp = 1704067200
    };

    printf("=== Paquet original (format hôte) ===\n");
    printf("Type: 0x%04X\n", paquet_hote.type);
    printf("Longueur: %u\n", paquet_hote.longueur);
    printf("ID Session: 0x%08X\n", paquet_hote.id_session);
    printf("Timestamp: %u\n\n", paquet_hote.timestamp);

    /* Preparation pour envoi reseau */
    struct Paquet paquet_reseau = paquet_hote;
    preparer_envoi(&paquet_reseau);

    printf("=== Paquet préparé (format réseau) ===\n");
    printf("Octets bruts : ");
    unsigned char* ptr = (unsigned char*)&paquet_reseau;
    for (size_t i = 0; i < sizeof(paquet_reseau); i++) {
        printf("%02X ", ptr[i]);
    }
    printf("\n\n");

    /* Simulation de reception */
    struct Paquet paquet_recu = paquet_reseau;
    traiter_reception(&paquet_recu);

    printf("=== Paquet reçu (format hôte) ===\n");
    printf("Type: 0x%04X\n", paquet_recu.type);
    printf("Longueur: %u\n", paquet_recu.longueur);
    printf("ID Session: 0x%08X\n", paquet_recu.id_session);
    printf("Timestamp: %u\n", paquet_recu.timestamp);

    return 0;
}
