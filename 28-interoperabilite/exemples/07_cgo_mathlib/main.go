// ============================================================================
//   Section 28.2 : Appeler du C depuis Go
//   Description : Programme Go appelant une bibliotheque C via cgo
//                 (factorielle et racine cubique)
//   Fichier source : 02-c-depuis-go.md
// ============================================================================

package main

/*
#cgo CFLAGS: -I.
#cgo LDFLAGS: -L. -lmathlib -lm

#include "mathlib.h"
*/
import "C"
import "fmt"

func main() {
	fmt.Println("=== cgo : C depuis Go ===")
	fmt.Println()

	// Factorielle
	for i := 0; i <= 10; i++ {
		fact := C.factorielle(C.int(i))
		fmt.Printf("  %d! = %d\n", i, fact)
	}

	// Racine cubique
	fmt.Println()
	valeurs := []float64{8.0, 27.0, 64.0, 125.0, 1000.0}
	for _, v := range valeurs {
		racine := C.racine_cubique(C.double(v))
		fmt.Printf("  cbrt(%.0f) = %.2f\n", v, racine)
	}

	fmt.Println()
	fmt.Println("=== Fin ===")
}
