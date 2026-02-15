// ============================================================================
//   Section 28.3 : Interfacage avec Rust (FFI)
//   Description : Bibliotheque Rust exposant des fonctions via FFI pour C
//                 (factorielle, fibonacci, division securisee)
//   Fichier source : 03-interfacage-rust.md
// ============================================================================

use std::os::raw::c_int;

#[no_mangle]
pub extern "C" fn factorielle_rust(n: u32) -> u64 {
    if n <= 1 {
        1
    } else {
        n as u64 * factorielle_rust(n - 1)
    }
}

#[no_mangle]
pub extern "C" fn fibonacci_rust(n: u32) -> u64 {
    match n {
        0 => 0,
        1 => 1,
        _ => {
            let mut a: u64 = 0;
            let mut b: u64 = 1;
            for _ in 2..=n {
                let temp = a + b;
                a = b;
                b = temp;
            }
            b
        }
    }
}

const SUCCESS: c_int = 0;
const ERROR_INVALID_INPUT: c_int = -1;

#[no_mangle]
pub extern "C" fn diviser_entiers(a: c_int, b: c_int, resultat: *mut c_int) -> c_int {
    if resultat.is_null() {
        return ERROR_INVALID_INPUT;
    }

    if b == 0 {
        return ERROR_INVALID_INPUT;
    }

    unsafe {
        *resultat = a / b;
    }

    SUCCESS
}
