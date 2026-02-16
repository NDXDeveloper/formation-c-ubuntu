# Annexe B.5 - CMake commandes principales : Guide de compilation

## Compilation avec CMake

```bash
mkdir build && cd build  
cmake ..  
cmake --build .  
ctest --verbose
```

**Note** : Ce dossier contient un projet CMake complet reproduisant l'exemple structuré du .md (lignes 690-782).

---

## Structure du projet

```
05-cmake/
├── CMakeLists.txt          # CMake principal (C17, option BUILD_TESTS)
├── include/
│   └── mon_projet.h        # Header de la bibliothèque
├── src/
│   ├── CMakeLists.txt       # Exécutable mon_programme
│   ├── main.c               # Point d'entrée
│   └── utils.c              # Fonctions utilitaires
├── lib/
│   ├── CMakeLists.txt       # Bibliothèque statique ma_lib
│   └── ma_lib.c             # Implémentation de la bibliothèque
└── tests/
    ├── CMakeLists.txt       # Test unitaire
    └── test_main.c          # Test de ma_lib_add()
```

## Sortie attendue

### Programme principal
```
Résultat: 7
10 + 20 = 30
```

### Tests (ctest)
```
test_main ........................   Passed
100% tests passed, 0 tests failed out of 1
```
