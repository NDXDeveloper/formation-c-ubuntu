# Annexe B.2 - Commandes GDB : Guide de compilation

## Compilation par défaut

```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 -g -o programme fichier.c
```

**Note** : Tous les programmes nécessitent `-g` pour les symboles de débogage (usage avec GDB).

---

## Liste des exemples

### ex1_null.c
- **Section** : B.2 - Débogage de crash
- **Description** : Programme avec déréférencement de pointeur NULL (crash intentionnel)
- **Fichier source** : 02-commandes-gdb.md
- **Sortie attendue** : Segmentation fault (pour analyse avec GDB)

### ex2_watch.c
- **Section** : B.2 - Watchpoints
- **Description** : Programme avec compteur pour démontrer les watchpoints GDB
- **Fichier source** : 02-commandes-gdb.md
- **Sortie attendue** : `Counter: 99`

### ex3_array.c
- **Section** : B.2 - Accès mémoire
- **Description** : Programme avec accès tableau hors limites (bug intentionnel)
- **Fichier source** : 02-commandes-gdb.md
- **Sortie attendue** : Comportement indéfini (pour analyse avec GDB)

### ex4_cond.c
- **Section** : B.2 - Breakpoints conditionnels
- **Description** : Programme avec boucle pour démontrer les breakpoints conditionnels
- **Fichier source** : 02-commandes-gdb.md
- **Sortie attendue** : Carrés de 0 à 9 (0, 1, 4, 9, 16, 25, 36, 49, 64, 81)
