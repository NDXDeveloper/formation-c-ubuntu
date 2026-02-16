# Annexe D.1 - Standards C officiels : Guide de compilation

## Compilation par défaut

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o programme fichier.c
```

---

## Liste des exemples

### test_c89.c
- **Section** : D.1 - C89/C90
- **Description** : Programme C89 avec prototype de fonction et appel
- **Fichier source** : 01-standards-c-officiels.md (lignes 63-76)
- **Sortie attendue** : `Résultat : 7`

### test_c99.c
- **Section** : D.1 - C99
- **Description** : Types C99 (int64_t, bool) avec stdint.h et stdbool.h
- **Fichier source** : 01-standards-c-officiels.md (lignes 121-129)
- **Sortie attendue** : `big_number = 9223372036854775807, is_active = 1`

### test_c11_threads.c
- **Section** : D.1 - C11
- **Description** : Threads C11 natifs avec threads.h (thrd_create/thrd_join)
- **Fichier source** : 01-standards-c-officiels.md (lignes 174-187)
- **Sortie attendue** : `Hello from thread!`

### test_stdc_version.c
- **Section** : D.1 - Détection du standard
- **Description** : Détection du standard C via la macro __STDC_VERSION__
- **Fichier source** : 01-standards-c-officiels.md (lignes 386-411)
- **Sortie attendue** : `Standard C détecté : C17` (avec -std=c17)
