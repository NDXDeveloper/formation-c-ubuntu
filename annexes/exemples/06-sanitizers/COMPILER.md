# Annexe B.6 - Flags de sanitizers et Valgrind : Guide de compilation

## Compilation spécifique par sanitizer

Chaque programme utilise des flags différents selon le sanitizer démontré. **Ne pas utiliser** `-Werror` : les bugs sont intentionnels.

---

## Liste des exemples

### bug_asan.c
- **Section** : B.6 - AddressSanitizer (ASan)
- **Description** : Heap buffer overflow intentionnel (écriture hors limites)
- **Fichier source** : 06-flags-sanitizers-valgrind.md (lignes 101-110)
- **Compilation** : `gcc -fsanitize=address -g -O0 bug_asan.c -o bug_asan`
- **Sortie attendue** : `ERROR: AddressSanitizer: heap-buffer-overflow`
- **Note** : Avec `-O1`, le compilateur peut optimiser l'écriture morte ; utiliser `-O0`

### leak_asan.c
- **Section** : B.6 - LeakSanitizer (LSan)
- **Description** : Fuite mémoire intentionnelle (malloc sans free)
- **Fichier source** : 06-flags-sanitizers-valgrind.md (lignes 131-138)
- **Compilation** : `gcc -fsanitize=address -g leak_asan.c -o leak_asan`
- **Exécution** : `ASAN_OPTIONS=detect_leaks=1 ./leak_asan`
- **Sortie attendue** : `ERROR: LeakSanitizer: detected memory leaks` (100 bytes)

### undef_ubsan.c
- **Section** : B.6 - UndefinedBehaviorSanitizer (UBSan)
- **Description** : 3 comportements indéfinis : integer overflow, out of bounds, null deref
- **Fichier source** : 06-flags-sanitizers-valgrind.md (lignes 216-230)
- **Compilation** : `gcc -fsanitize=undefined -g undef_ubsan.c -o undef_ubsan`
- **Sortie attendue** : 3 erreurs runtime puis segfault sur null deref

### race_tsan.c
- **Section** : B.6 - ThreadSanitizer (TSan)
- **Description** : Race condition intentionnelle sur compteur partagé non protégé
- **Fichier source** : 06-flags-sanitizers-valgrind.md (lignes 289-310)
- **Compilation** : `gcc -fsanitize=thread -g -O1 race_tsan.c -o race_tsan -pthread`
- **Sortie attendue** : `WARNING: ThreadSanitizer: data race`

### leak_valgrind.c
- **Section** : B.6 - Valgrind Memcheck
- **Description** : Fuite mémoire + accès hors limites (pour démonstration Valgrind)
- **Fichier source** : 06-flags-sanitizers-valgrind.md (lignes 487-501)
- **Compilation** : `gcc -g -O0 leak_valgrind.c -o leak_valgrind`
- **Exécution** : `valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./leak_valgrind`
- **Sortie attendue** : `Invalid write of size 4` + `definitely lost: 100 bytes`

### race_helgrind.c
- **Section** : B.6 - Helgrind
- **Description** : Race condition pour démonstration Helgrind
- **Fichier source** : 06-flags-sanitizers-valgrind.md (lignes 561-578)
- **Compilation** : `gcc -g -pthread race_helgrind.c -o race_helgrind`
- **Exécution** : `valgrind --tool=helgrind ./race_helgrind`
- **Sortie attendue** : `Possible data race during read/write of size 4`
