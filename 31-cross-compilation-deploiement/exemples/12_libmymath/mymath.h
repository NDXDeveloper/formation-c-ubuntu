/* ============================================================================
   Section 31.9 : Distribution de bibliotheques
   Description : Header de la bibliotheque mymath (.so)
   Fichier source : 09-distribution-bibliotheques.md
   ============================================================================ */
#ifndef MYMATH_H
#define MYMATH_H

int add(int a, int b);
int multiply(int a, int b);
int power(int base, int exponent);
const char *mymath_version(void);

#endif
