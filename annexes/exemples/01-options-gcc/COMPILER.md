# Annexe B.1 - Options GCC : Guide de compilation

## Compilation par défaut

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -o programme fichier.c
```

---

## Liste des exemples

### test_wall.c
- **Section** : B.1 - Options de warning
- **Description** : Test du flag -Wall (variable non utilisée)
- **Fichier source** : 01-options-gcc.md
- **Compilation** : `gcc -Wall -c test_wall.c` (doit produire un warning)

### test_shadow.c
- **Section** : B.1 - Options de warning
- **Description** : Test du flag -Wshadow (variable masquée)
- **Fichier source** : 01-options-gcc.md
- **Compilation** : `gcc -Wshadow -c test_shadow.c` (doit produire un warning)

### test_conversion.c
- **Section** : B.1 - Options de warning
- **Description** : Test du flag -Wconversion (conversion implicite)
- **Fichier source** : 01-options-gcc.md
- **Compilation** : `gcc -Wconversion -c test_conversion.c` (doit produire un warning)

### test_pedantic.c
- **Section** : B.1 - Extensions GNU
- **Description** : Test de typeof (ne produit plus de warning avec GCC 13 -pedantic)
- **Fichier source** : 01-options-gcc.md
- **Compilation** : `gcc -std=gnu17 -pedantic -o test_pedantic test_pedantic.c`

### test_pedantic2.c
- **Section** : B.1 - Extensions GNU
- **Description** : Test de GNU statement expression (produit un warning avec -pedantic)
- **Fichier source** : 01-options-gcc.md
- **Compilation** : `gcc -std=gnu17 -pedantic -o test_pedantic2 test_pedantic2.c` (doit produire un warning)

### test_typeof.c
- **Section** : B.1 - Extensions GNU
- **Description** : Vérification que typeof ne produit plus de warning avec GCC 13
- **Fichier source** : 01-options-gcc.md
- **Compilation** : `gcc -std=gnu17 -pedantic -Werror -o test_typeof test_typeof.c`
