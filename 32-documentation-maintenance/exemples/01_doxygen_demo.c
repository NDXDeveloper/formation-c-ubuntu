/*
 * Section  : 32.1.1 - Syntaxe des commentaires Doxygen
 * Description : Programme documenté avec la syntaxe Doxygen (Javadoc-style)
 * Fichier source : 01.1-syntaxe-commentaires.md
 */

/**
 * @file 01_doxygen_demo.c
 * @brief Démonstration de la documentation Doxygen en C.
 * @author Formation C Ubuntu
 * @version 1.0
 * @date 2024
 *
 * Ce programme illustre les bonnes pratiques de documentation
 * avec Doxygen : commentaires de fichier, fonctions, structures
 * et macros.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/**
 * @def MAX_NAME_LEN
 * @brief Longueur maximale d'un nom.
 */
#define MAX_NAME_LEN 64

/**
 * @struct Student
 * @brief Représente un étudiant.
 *
 * Structure contenant les informations de base
 * d'un étudiant inscrit dans un cours.
 */
typedef struct {
    char name[MAX_NAME_LEN];  /**< Nom complet de l'étudiant */
    int age;                   /**< Âge en années */
    float grade;               /**< Note sur 20 */
} Student;

/**
 * @brief Crée un nouvel étudiant.
 *
 * Alloue et initialise une structure Student avec
 * les valeurs fournies.
 *
 * @param name  Nom de l'étudiant (max MAX_NAME_LEN - 1 caractères)
 * @param age   Âge de l'étudiant (doit être > 0)
 * @param grade Note de l'étudiant (entre 0.0 et 20.0)
 * @return Pointeur vers le Student créé, ou NULL en cas d'erreur
 *
 * @note L'appelant doit libérer la mémoire avec free().
 *
 * @code
 * Student *s = student_create("Alice", 20, 15.5f);
 * if (s != NULL) {
 *     student_print(s);
 *     free(s);
 * }
 * @endcode
 */
static Student *student_create(const char *name, int age, float grade) {
    if (name == NULL || age <= 0 || grade < 0.0f || grade > 20.0f) {
        return NULL;
    }

    Student *s = malloc(sizeof(*s));
    if (s == NULL) {
        return NULL;
    }

    snprintf(s->name, sizeof(s->name), "%s", name);
    s->age = age;
    s->grade = grade;

    return s;
}

/**
 * @brief Affiche les informations d'un étudiant.
 *
 * @param s Pointeur vers l'étudiant à afficher
 *
 * @warning Le pointeur @p s ne doit pas être NULL.
 */
static void student_print(const Student *s) {
    if (s == NULL) {
        fprintf(stderr, "Erreur: etudiant NULL\n");
        return;
    }

    printf("Etudiant: %s\n", s->name);
    printf("  Age   : %d ans\n", s->age);
    printf("  Note  : %.1f/20\n", s->grade);
}

/**
 * @brief Vérifie si un étudiant a la moyenne.
 *
 * @param s Pointeur vers l'étudiant
 * @return true si la note >= 10.0, false sinon
 *
 * @see student_create()
 */
static bool student_has_passed(const Student *s) {
    if (s == NULL) {
        return false;
    }
    return s->grade >= 10.0f;
}

/**
 * @brief Point d'entrée du programme.
 *
 * Crée plusieurs étudiants et affiche leurs informations
 * avec leur statut de réussite.
 *
 * @return EXIT_SUCCESS en cas de succès
 */
int main(void) {
    printf("=== Demonstration documentation Doxygen ===\n\n");

    /* Créer des étudiants */
    Student *alice = student_create("Alice Dupont", 20, 15.5f);
    Student *bob = student_create("Bob Martin", 22, 8.0f);
    Student *charlie = student_create("Charlie Durand", 19, 12.0f);

    /* Tableau d'étudiants */
    Student *students[] = {alice, bob, charlie};
    int count = (int)(sizeof(students) / sizeof(students[0]));

    for (int i = 0; i < count; i++) {
        if (students[i] != NULL) {
            student_print(students[i]);
            printf("  Reussi: %s\n\n",
                   student_has_passed(students[i]) ? "Oui" : "Non");
        }
    }

    /* Libérer la mémoire */
    for (int i = 0; i < count; i++) {
        free(students[i]);
    }

    return EXIT_SUCCESS;
}
