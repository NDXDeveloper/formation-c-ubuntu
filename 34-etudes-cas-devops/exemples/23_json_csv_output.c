/* ============================================================================
   Section 34.1.3 : Output formate
   Description : Generation de sorties JSON et CSV structurees
   Fichier source : 01.3-output-formate.md
   ============================================================================ */
#include <stdio.h>
#include <string.h>

void afficher_json_objet(void) {
    printf("{\n");
    printf("  \"nom\": \"Alice\",\n");
    printf("  \"age\": 28,\n");
    printf("  \"actif\": true,\n");
    printf("  \"email\": \"alice@example.com\"\n");
    printf("}\n");
}

void afficher_json_tableau(void) {
    printf("[\n");
    printf("  {\"id\": 1, \"nom\": \"Alice\"},\n");
    printf("  {\"id\": 2, \"nom\": \"Bob\"},\n");
    printf("  {\"id\": 3, \"nom\": \"Charlie\"}\n");
    printf("]\n");
}

void afficher_csv_header(void) {
    printf("nom,age,ville,salaire\n");
}

void afficher_csv_ligne(const char *nom, int age, const char *ville, float salaire) {
    // Gérer les champs avec virgules
    if (strchr(nom, ',') || strchr(ville, ',')) {
        printf("\"%s\",%d,\"%s\",%.2f\n", nom, age, ville, (double)salaire);
    } else {
        printf("%s,%d,%s,%.2f\n", nom, age, ville, (double)salaire);
    }
}

int main(void) {
    printf("=== Objet JSON ===\n");
    afficher_json_objet();

    printf("\n=== Tableau JSON ===\n");
    afficher_json_tableau();

    printf("\n=== Sortie CSV ===\n");
    afficher_csv_header();
    afficher_csv_ligne("Alice", 28, "Paris", 45000.0f);
    afficher_csv_ligne("Bob", 35, "Lyon", 52000.0f);
    afficher_csv_ligne("Charlie, Jr.", 42, "Marseille", 61500.0f);

    return 0;
}
