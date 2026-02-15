/* ============================================================================
   Section 28.5 : extern "C" pour C++
   Description : Header compatible C/C++ pour code legacy (ifdef __cplusplus)
   Fichier source : 05-extern-c.md
   ============================================================================ */

#ifndef LEGACY_H
#define LEGACY_H

#ifdef __cplusplus
extern "C" {
#endif

void ancienne_fonction(int x);
int calcul_complexe(int a, int b);

#ifdef __cplusplus
}
#endif

#endif
