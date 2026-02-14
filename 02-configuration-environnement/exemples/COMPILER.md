# Chapitre 02 - Exemples compilables

## Fichiers sans exemples compilables

Les fichiers suivants sont purement conceptuels et ne contiennent aucun programme compilable :

- **README.md** : Introduction et structure du chapitre
- **02-versions-gcc.md** : Gestion des versions de GCC (commandes bash uniquement)
- **04-extensions-et-outils.md** : Configuration des extensions VS Code (JSON, configs)
- **05-devcontainers.md** : Introduction aux DevContainers (conceptuel)
- **05.1-quest-ce-quun-devcontainer.md** : Concepts et fonctionnement des DevContainers
- **05.2-structure-devcontainer-json.md** : Structure du fichier devcontainer.json (JSON)
- **05.3-configuration-outils.md** : Configuration GCC/GDB/CMake/Valgrind (extraits illustratifs)
- **05.4-integration-vscode.md** : Integration VS Code et onboarding (conceptuel)
- **05.5-devcontainers-source-verite.md** : DevContainers comme source de verite (conceptuel)

**Note** : Le fichier 05.3-configuration-outils.md contient un programme C complet (lignes 1346-1370) qui a ete extrait en tant qu'exemple compilable ci-dessous (03_test_toolchain.c). Les autres extraits de code C dans ce fichier sont des exemples illustratifs pour Valgrind (volontairement bugges).

---

## Exemples compilables

### 01_test_installation.c

- **Section** : 2.1 Installation de la toolchain
- **Fichier source** : 01-installation-toolchain.md (lignes 190-197)
- **Description** : Test de verification de l'installation de GCC
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 01_test_installation.c -o 01_test_installation
```
- **Sortie attendue** :
```
Installation réussie ! GCC fonctionne.
```

---

### 02_test_environnement.c

- **Section** : 2.3 Choix de l'IDE/editeur
- **Fichier source** : 03-choix-ide-editeur.md (lignes 578-585)
- **Description** : Test de verification de l'environnement de developpement
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 02_test_environnement.c -o 02_test_environnement
```
- **Sortie attendue** :
```
Mon environnement est prêt !
```

---

### 03_test_toolchain.c

- **Section** : 2.5.3 Configuration GCC/GDB/CMake/Valgrind
- **Fichier source** : 05.3-configuration-outils.md (lignes 1346-1370)
- **Description** : Test complet de la toolchain avec allocation memoire (malloc/free)
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 03_test_toolchain.c -o 03_test_toolchain
```
- **Sortie attendue** :
```
=== Test de l'environnement ===
numbers[0] = 0
numbers[1] = 10
numbers[2] = 20
numbers[3] = 30
numbers[4] = 40
=== Test réussi ! ===
```
- **Note** : Le code original dans le .md utilise `int main(int argc, char *argv[])` avec les parametres non utilises. L'exemple a ete adapte en `int main(void)` pour compiler avec `-Werror`.

---

## Resume

| # | Fichier | Section | Standard | Resultat |
|---|---------|---------|----------|----------|
| 1 | 01_test_installation.c | 2.1 Installation toolchain | C17 | OK |
| 2 | 02_test_environnement.c | 2.3 Choix IDE/editeur | C17 | OK |
| 3 | 03_test_toolchain.c | 2.5.3 Config GCC/GDB/CMake/Valgrind | C17 | OK |

**Total : 3 programmes, 0 corrections necessaires dans les .md.**
