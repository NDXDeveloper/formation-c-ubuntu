/* ============================================================================
   Section 23.7 : X-Macros
   Description : Registres materiels avec X-Macros
   Fichier source : 07-x-macros.md
   ============================================================================ */

#include <stdio.h>
#include <stdint.h>

// Liste des registres avec adresse, nom et description
#define REGISTER_LIST \
    X(REG_STATUS,   0x00, "RW", "Registre de statut") \
    X(REG_CONTROL,  0x04, "RW", "Registre de controle") \
    X(REG_DATA,     0x08, "RW", "Registre de donnees") \
    X(REG_ERROR,    0x0C, "RO", "Registre d'erreur") \
    X(REG_VERSION,  0x10, "RO", "Version du materiel")

// Génération de l'enum
#define X(name, addr, perm, desc) name,
typedef enum {
    REGISTER_LIST
    REG_COUNT
} Register;
#undef X

// Génération du tableau d'adresses
#define X(name, addr, perm, desc) [name] = addr,
static const uint32_t register_addresses[] = {
    REGISTER_LIST
};
#undef X

// Génération du tableau de permissions
#define X(name, addr, perm, desc) [name] = perm,
static const char* register_permissions[] = {
    REGISTER_LIST
};
#undef X

// Génération du tableau de descriptions
#define X(name, addr, perm, desc) [name] = desc,
static const char* register_descriptions[] = {
    REGISTER_LIST
};
#undef X

// Génération du tableau de noms
#define X(name, addr, perm, desc) [name] = #name,
static const char* register_names[] = {
    REGISTER_LIST
};
#undef X

// Fonction pour afficher les informations d'un registre
void register_info(Register reg) {
    printf("Registre: %s\n", register_names[reg]);
    printf("  Adresse:     0x%04X\n", register_addresses[reg]);
    printf("  Permissions: %s\n", register_permissions[reg]);
    printf("  Description: %s\n", register_descriptions[reg]);
}

// Génération de la documentation
void print_register_map(void) {
    printf("=== MAP DES REGISTRES ===\n");
    printf("%-14s %-8s %-6s %s\n", "Nom", "Addr", "Perm", "Description");
    printf("%-14s %-8s %-6s %s\n", "-----------", "------", "----", "-------------------");

#define X(name, addr, perm, desc) \
    printf("%-14s 0x%04X   %s   %s\n", #name, addr, perm, desc);
    REGISTER_LIST
#undef X

    printf("=========================\n");
}

int main(void) {
    print_register_map();
    printf("\n");
    register_info(REG_STATUS);

    return 0;
}
