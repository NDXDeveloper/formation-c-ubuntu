# Chapitre 04 - Exemples compilables

## Fichiers sans exemples compilables

Les fichiers suivants sont purement conceptuels ou ne contiennent que des extraits illustratifs non autonomes :

- **README.md** : Introduction et structure du chapitre (contient un programme avec debordement signe volontaire — UB, non extrait)

**Note** : Le programme du README.md (lignes 212-226) contient un debordement d'entier signe volontaire (`max + 1` ou `max = INT_MAX`), qui constitue un comportement indefini (UB). Il n'a pas ete extrait en tant qu'exemple compilable car les concepts de sizeof et limites sont couverts par les exemples 01 et 04.

---

## Exemples compilables

### 01_sizeof_types.c

- **Section** : 4.1 Types primitifs et leur taille
- **Fichier source** : 01-types-primitifs.md (lignes 119-130)
- **Description** : Verification de la taille des types avec sizeof
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 01_sizeof_types.c -o 01_sizeof_types
```
- **Sortie attendue** :
```
Taille de char : 1 octets
Taille de int : 4 octets
Taille de float : 4 octets
Taille de double : 8 octets
Taille de long long : 8 octets
```

---

### 02_declarations_types.c

- **Section** : 4.1 Types primitifs et leur taille
- **Fichier source** : 01-types-primitifs.md (lignes 200-225)
- **Description** : Exemples pratiques de declaration avec types classiques et largeur fixe
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 02_declarations_types.c -o 02_declarations_types
```
- **Sortie attendue** :
```
Initiale : J
Age : 30 ans
Taille : 1.75 m
Salaire : 45000.50 €
Timestamp : 1704067200
Population : 8000000000 personnes
```

---

### 03_signed_unsigned.c

- **Section** : 4.2 Signed vs Unsigned
- **Fichier source** : 02-signed-vs-unsigned.md (lignes 317-353)
- **Description** : Exemple complet et comparatif signed/unsigned avec underflow et limites
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 03_signed_unsigned.c -o 03_signed_unsigned
```
- **Sortie attendue** :
```
=== Types classiques ===
Négatif (int) : -100
Positif (unsigned int) : 100

=== Underflow ===
5 - 10 en unsigned : 4294967291
(devrait être -5, mais unsigned boucle)

=== Types à largeur fixe ===
int32_t : -2000000000
uint32_t : 4000000000

=== Limites ===
INT_MAX : 2147483647
UINT_MAX : 4294967295
```

---

### 04_limites_types.c

- **Section** : 4.2 Signed vs Unsigned
- **Fichier source** : 02-signed-vs-unsigned.md (lignes 380-398)
- **Description** : Macros de limites des types entiers (<limits.h>)
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 04_limites_types.c -o 04_limites_types
```
- **Sortie attendue** :
```
CHAR_MIN : -128
CHAR_MAX : 127
UCHAR_MAX : 255
SHRT_MIN : -32768
SHRT_MAX : 32767
USHRT_MAX : 65535
INT_MIN : -2147483648
INT_MAX : 2147483647
UINT_MAX : 4294967295
```
- **Note** : Le .md utilise `%u` pour `UCHAR_MAX` et `USHRT_MAX`, adapte en `%d` car ces macros sont de type `int` et `%u` provoquerait un warning avec `-Werror`.

---

### 05_macros_predefinies.c

- **Section** : 4.3 Les constantes et les litteraux
- **Fichier source** : 03-constantes-litteraux.md (lignes 554-566)
- **Description** : Constantes predefinies du preprocesseur (__FILE__, __LINE__, __func__, __DATE__, __TIME__)
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 05_macros_predefinies.c -o 05_macros_predefinies
```
- **Sortie attendue** (valeurs dynamiques) :
```
Fichier : 05_macros_predefinies.c
Ligne : 10
Fonction : afficher_debug
Compilé le : <DATE> à <HEURE>
```

---

### 06_constantes_complet.c

- **Section** : 4.3 Les constantes et les litteraux
- **Fichier source** : 03-constantes-litteraux.md (lignes 599-661)
- **Description** : Exemple complet : litteraux (bases), constantes, enums, limites, macros
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 06_constantes_complet.c -o 06_constantes_complet
```
- **Sortie attendue** :
```
Décimal : 255
Hexa : 255
Octal : 255

Vitesse de la lumière : 3.00e+08 m/s
Prix : 19.99 €

Lettre : A (code ASCII : 65)

Pi : 3.1415926536
Version : 1.0.0

Aujourd'hui : jour 3 de la semaine

INT_MAX : 2147483647
UINT_MAX : 4294967295

Compilé le <DATE> à <HEURE>
```
- **Note** : Ajout de `(void)TAILLE_BUFFER; (void)SECONDES_PAR_MINUTE;` pour eviter les warnings `-Wunused` avec `-Werror`.

---

### 07_conversions.c

- **Section** : 4.4 Conversion de types
- **Fichier source** : 04-conversion-types.md (lignes 603-663)
- **Description** : Exemple complet des conversions (division, promotion, troncature, precision, signed/unsigned)
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 07_conversions.c -o 07_conversions -lm
```
- **Sortie attendue** :
```
=== Conversions de types ===

1. Division
   Entière : 7 / 2 = 3
   Flottante : 7 / 2 = 3.50

2. Promotion char → int
   char 100 → int 100

3. Troncature double → int
   Original : 3.14159
   Tronqué : 3
   Arrondi : 3

4. Perte de précision int → float
   Original (int) : 123456789
   Converti (float) : 123456792

5. Conversion signed → unsigned
   Signé : -1
   Non signé : 4294967295

6. Vérification des limites
   3000000000 dépasse INT_MAX (2147483647)
   Conversion en int non recommandée

7. Calcul de pourcentage
   42 / 50 = 84.00%
```
- **Note** : Necessite `-lm` pour `round()` de `<math.h>`. Ajout de cast explicite `(float)` et `(unsigned int)` pour eviter les warnings de conversion implicite avec `-Werror`.

---

### 08_endianness_union.c

- **Section** : 4.5 Endianness et portabilite
- **Fichier source** : 05-endianness.md (lignes 168-190)
- **Description** : Detection de l'endianness avec une union
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 08_endianness_union.c -o 08_endianness_union
```
- **Sortie attendue** (sur x86/x64) :
```
Premier octet en mémoire : 0x78
Système Little-Endian
```

---

### 09_endianness_pointeur.c

- **Section** : 4.5 Endianness et portabilite
- **Fichier source** : 05-endianness.md (lignes 200-221)
- **Description** : Detection de l'endianness avec un pointeur
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 09_endianness_pointeur.c -o 09_endianness_pointeur
```
- **Sortie attendue** (sur x86/x64) :
```
Octets en mémoire : 0x78 0x56 0x34 0x12
Little-Endian
```

---

### 10_portabilite_reseau.c

- **Section** : 4.5 Endianness et portabilite
- **Fichier source** : 05-endianness.md (lignes 612-679)
- **Description** : Portabilite reseau avec htons/htonl/ntohs/ntohl (simulation paquet)
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c17 10_portabilite_reseau.c -o 10_portabilite_reseau
```
- **Sortie attendue** :
```
=== Paquet original (format hôte) ===
Type: 0x0001
Longueur: 100
ID Session: 0x12345678
Timestamp: 1704067200

=== Paquet préparé (format réseau) ===
Octets bruts : 00 01 00 64 12 34 56 78 65 92 00 80

=== Paquet reçu (format hôte) ===
Type: 0x0001
Longueur: 100
ID Session: 0x12345678
Timestamp: 1704067200
```
- **Note** : Les octets bruts du paquet reseau montrent la conversion big-endian. Les valeurs du paquet recu sont identiques a l'original, confirmant la conversion aller-retour.

---

### 11_nouveautes_c23.c

- **Section** : 4.6 Nouveautes C23
- **Fichier source** : 06-nouveautes-c23.md (lignes 445-495)
- **Description** : Demo des nouveautes C23 (bool natif, nullptr, litteraux binaires, typeof, attributs)
- **Compilation** :
```bash
gcc -Wall -Wextra -Werror -pedantic -std=c2x 11_nouveautes_c23.c -o 11_nouveautes_c23
```
- **Sortie attendue** :
```
=== Démo des nouveautés C23 ===

1. Booléens natifs: vrai
2. Pointeur est null
3. Flags binaires: 0xAA
4. Population: 67000000 habitants
5. Âge valide
6. typeof: x=42, y=100
```
- **Note** : Necessite GCC 13+ avec `-std=c2x` ou GCC 14+ avec `-std=c23`. Le separateur de chiffres (`67'000'000`) a ete remplace par `67000000L` car GCC 13.3 ne supporte pas completement les separateurs de chiffres.

---

## Resume

| # | Fichier | Section | Standard | Resultat |
|---|---------|---------|----------|----------|
| 1 | 01_sizeof_types.c | 4.1 Types primitifs | C17 | OK |
| 2 | 02_declarations_types.c | 4.1 Types primitifs | C17 | OK |
| 3 | 03_signed_unsigned.c | 4.2 Signed vs Unsigned | C17 | OK |
| 4 | 04_limites_types.c | 4.2 Signed vs Unsigned | C17 | OK |
| 5 | 05_macros_predefinies.c | 4.3 Constantes/litteraux | C17 | OK |
| 6 | 06_constantes_complet.c | 4.3 Constantes/litteraux | C17 | OK |
| 7 | 07_conversions.c | 4.4 Conversion de types | C17 | OK (necessite -lm) |
| 8 | 08_endianness_union.c | 4.5 Endianness | C17 | OK |
| 9 | 09_endianness_pointeur.c | 4.5 Endianness | C17 | OK |
| 10 | 10_portabilite_reseau.c | 4.5 Endianness | C17 | OK |
| 11 | 11_nouveautes_c23.c | 4.6 Nouveautes C23 | C23 | OK (necessite -std=c2x) |

**Total : 11 programmes, 0 corrections necessaires dans les .md.**

### Adaptations pour -Werror :
- **04_limites_types.c** : `UCHAR_MAX` et `USHRT_MAX` affiches avec `%d` au lieu de `%u` (type `int` en GCC)
- **06_constantes_complet.c** : Ajout `(void)` pour variables/macros inutilisees
- **07_conversions.c** : Casts explicites ajoutes pour eviter les warnings de conversion implicite
- **11_nouveautes_c23.c** : Separateur de chiffres non utilise (support partiel GCC 13.3)
