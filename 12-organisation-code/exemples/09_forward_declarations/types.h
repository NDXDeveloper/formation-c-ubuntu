/* ============================================================================
   Section 12.6 : Forward declarations et dependances circulaires
   Description : Systeme de gestion de bibliotheque - header commun de types
                 avec forward declarations et typedefs
   Fichier source : 06-forward-declarations.md
   ============================================================================ */

#ifndef TYPES_H
#define TYPES_H

/* Forward declarations communes */
struct Book;
struct Member;
struct Loan;

/* Typedefs */
typedef struct Book Book;
typedef struct Member Member;
typedef struct Loan Loan;

#endif /* TYPES_H */
