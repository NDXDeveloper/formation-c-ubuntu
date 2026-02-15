/* ============================================================================
   Section 28.5 : extern "C" pour C++
   Description : Header compatible C et C++ pour operations mathematiques
   Fichier source : 05-extern-c.md
   ============================================================================ */

#ifndef MATH_H
#define MATH_H

#ifdef __cplusplus
extern "C" {
#endif

int addition(int a, int b);
int soustraction(int a, int b);

#ifdef __cplusplus
}
#endif

#endif
