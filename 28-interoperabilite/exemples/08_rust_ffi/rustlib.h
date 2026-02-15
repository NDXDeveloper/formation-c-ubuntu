/* ============================================================================
   Section 28.3 : Interfacage avec Rust (FFI)
   Description : Header C pour la bibliotheque Rust (factorielle, fibonacci,
                 division securisee)
   Fichier source : 03-interfacage-rust.md
   ============================================================================ */

#ifndef RUSTLIB_H
#define RUSTLIB_H

#include <stdint.h>

uint64_t factorielle_rust(uint32_t n);
uint64_t fibonacci_rust(uint32_t n);
int diviser_entiers(int a, int b, int *resultat);

#endif
