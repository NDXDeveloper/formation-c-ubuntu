/* ============================================================================
   Section 28.2 : Appeler du C depuis Go
   Description : Header de la bibliotheque C mathematique utilisee via cgo
   Fichier source : 02-c-depuis-go.md
   ============================================================================ */

#ifndef MATHLIB_H
#define MATHLIB_H

int factorielle(int n);
double racine_cubique(double x);

#endif
