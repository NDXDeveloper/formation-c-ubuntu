/* ============================================================================
   Section 23.7 : X-Macros
   Description : Generation de serialiseur avec X-Macros
   Fichier source : 07-x-macros.md
   ============================================================================ */

#include <stdio.h>

// Liste des champs d'une structure Personne
// Note : les types tableau (char[50]) ne fonctionnent pas bien ici
// car le paramètre "name" recevrait "name[50]", ce qui casse p->name
#define PERSON_FIELDS \
    X(int,          id,     "%d") \
    X(const char*,  name,   "%s") \
    X(int,          age,    "%d") \
    X(double,       salary, "%.2f")

// Génération de la structure
#define X(type, name, fmt) type name;
typedef struct {
    PERSON_FIELDS
} Person;
#undef X

// Génération de la fonction print
void person_print(const Person* p) {
    printf("Person {\n");
#define X(type, name, fmt) \
    printf("  " #name ": " fmt "\n", p->name);
    PERSON_FIELDS
#undef X
    printf("}\n");
}

int main(void) {
    // Remplir la structure
    Person p = {
        .id = 12345,
        .name = "Alice Dupont",
        .age = 30,
        .salary = 45000.50
    };

    // Afficher
    person_print(&p);

    return 0;
}
