/* ============================================================================
   Section 33.5.2 : Comprendre le coding style
   Description : Comparaison des coding styles C (K&R, Allman, GNU, Google)
   Fichier source : 05.2-coding-style.md
   ============================================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================ */
/* Meme algorithme, 4 styles differents         */
/* ============================================ */

/*
 * Chaque fonction ci-dessous fait EXACTEMENT la meme chose :
 * compter les occurrences d'un caractere dans une chaine.
 * Seul le STYLE de code change.
 */

/* ------------------------------------------  */
/* Style 1 : K&R (Kernighan & Ritchie)         */
/* Utilise par : Linux Kernel, Git, Redis       */
/* ------------------------------------------  */

/*
 * Caracteristiques K&R :
 * - Tabs pour indentation (largeur 8)
 * - Accolade ouvrante : meme ligne (sauf fonctions)
 * - Accolade fermante : nouvelle ligne
 * - snake_case pour tout
 * - Commentaires style C89
 */
static int count_char_kr(const char *str, char target)
{
        int count = 0;
        int i;

        if (!str)
                return -1;

        for (i = 0; str[i] != '\0'; i++) {
                if (str[i] == target) {
                        count++;
                }
        }

        return count;
}

/* ------------------------------------------  */
/* Style 2 : Allman (BSD)                       */
/* Utilise par : certains projets C++/C#        */
/* ------------------------------------------  */

/*
 * Caracteristiques Allman :
 * - Espaces pour indentation (4 espaces)
 * - TOUTES les accolades sur nouvelle ligne
 * - snake_case
 * - Plus vertical (plus d'espace)
 */
static int count_char_allman(const char *str, char target)
{
    int count = 0;

    if (!str)
    {
        return -1;
    }

    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == target)
        {
            count++;
        }
    }

    return count;
}

/* ------------------------------------------  */
/* Style 3 : GNU                                */
/* Utilise par : GCC, Emacs, grep, sed          */
/* ------------------------------------------  */

/*
 * Caracteristiques GNU :
 * - 2 espaces d'indentation
 * - Accolades indentees de 2 espaces
 * - Espace entre nom de fonction et parenthese
 * - Type de retour sur ligne separee
 * - Tres commente
 */
static int
count_char_gnu (const char *str, char target)
{
  int count = 0;

  /* Verify that the string pointer is valid.
     Return -1 for null pointers to indicate error. */
  if (str == NULL)
    return -1;

  /* Iterate through each character in the string
     and count occurrences of the target character. */
  for (int i = 0; str[i] != '\0'; i++)
    {
      if (str[i] == target)
        {
          count++;
        }
    }

  return count;
}

/* ------------------------------------------  */
/* Style 4 : Google (adapte au C)               */
/* Utilise par : Chromium, projets Google        */
/* ------------------------------------------  */

/*
 * Caracteristiques Google :
 * - 2 espaces d'indentation
 * - Accolade ouvrante TOUJOURS sur meme ligne (meme fonctions)
 * - PascalCase pour fonctions
 * - snake_case pour variables
 * - Commentaires C++ (//)
 */
static int CountCharGoogle(const char* str, char target) {
  int count = 0;

  if (!str) {
    return -1;
  }

  // Iterate through the string
  for (int i = 0; str[i] != '\0'; i++) {
    if (str[i] == target) {
      count++;
    }
  }

  return count;
}

/* ============================================ */
/* Demonstration                                */
/* ============================================ */

/* Type pour la table de fonctions */
typedef int (*count_fn_t)(const char *, char);

typedef struct {
    const char *name;
    const char *projects;
    count_fn_t fn;
} style_entry_t;

int main(void)
{
    printf("=== Comparaison des Coding Styles en C ===\n\n");

    const char *test_str = "Hello, World! Programming in C is great!";
    char target = 'i';

    printf("Chaine : \"%s\"\n", test_str);
    printf("Cible  : '%c'\n\n", target);

    style_entry_t styles[] = {
        { "K&R",    "Linux Kernel, Git, Redis", count_char_kr },
        { "Allman", "BSD, certains C++",        count_char_allman },
        { "GNU",    "GCC, Emacs, grep",         count_char_gnu },
        { "Google", "Chromium, projets Google",  CountCharGoogle },
    };
    int num_styles = (int)(sizeof(styles) / sizeof(styles[0]));

    printf("--- Resultats (identiques, seul le style change) ---\n");
    for (int i = 0; i < num_styles; i++) {
        int result = styles[i].fn(test_str, target);
        printf("  Style %-8s : count('%c') = %d   [%s]\n",
               styles[i].name, target, result, styles[i].projects);
    }

    /* Test avec NULL */
    printf("\n--- Test avec NULL ---\n");
    for (int i = 0; i < num_styles; i++) {
        int result = styles[i].fn(NULL, target);
        printf("  Style %-8s : count(NULL, '%c') = %d (erreur)\n",
               styles[i].name, target, result);
    }

    /* Resume des differences */
    printf("\n--- Resume des differences ---\n");
    printf("  %-10s %-12s %-12s %-15s %s\n",
           "Style", "Indentation", "Accolades", "Nommage", "Utilise par");
    printf("  %-10s %-12s %-12s %-15s %s\n",
           "-----", "-----------", "---------", "-------", "----------");
    printf("  %-10s %-12s %-12s %-15s %s\n",
           "K&R", "Tabs (8)", "Meme ligne", "snake_case", "Linux, Git");
    printf("  %-10s %-12s %-12s %-15s %s\n",
           "Allman", "4 espaces", "Nvelle ligne", "snake_case", "BSD");
    printf("  %-10s %-12s %-12s %-15s %s\n",
           "GNU", "2 espaces", "Indent 2", "snake_case", "GCC, Emacs");
    printf("  %-10s %-12s %-12s %-15s %s\n",
           "Google", "2 espaces", "Meme ligne", "PascalCase", "Chromium");

    printf("\n--- Regle d'or ---\n");
    printf("\"When in Rome, do as the Romans do\"\n");
    printf("Toujours suivre le style du PROJET, pas votre preference.\n");
    printf("Utilisez clang-format avec le .clang-format du projet.\n");

    return EXIT_SUCCESS;
}
