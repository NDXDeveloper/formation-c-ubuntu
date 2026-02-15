🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 27.1 Flags d'optimisation GCC

## Introduction

Les **flags d'optimisation** sont des options que vous passez au compilateur GCC pour lui indiquer comment transformer votre code source en code machine. Ces transformations ont pour but d'améliorer les performances (vitesse d'exécution, taille du binaire) tout en préservant le comportement de votre programme.

### Pourquoi optimiser ?

Lorsque vous compilez un programme C sans options particulières, GCC génère du code machine qui fonctionne correctement mais qui n'est pas nécessairement le plus rapide ou le plus compact possible. Le compilateur peut appliquer de nombreuses transformations pour :

- **Accélérer l'exécution** : éliminer des calculs redondants, réorganiser les instructions, utiliser des instructions CPU plus efficaces
- **Réduire la taille** : supprimer le code mort, factoriser les opérations similaires
- **Améliorer l'utilisation du cache** : organiser le code pour mieux exploiter la hiérarchie mémoire moderne

⚠️ **Important** : L'optimisation est un compromis. Un code plus rapide peut être plus gros, et inversement. De plus, certaines optimisations agressives peuvent rendre le débogage difficile.

---

## Les niveaux d'optimisation standards

GCC propose plusieurs niveaux d'optimisation prédéfinis, de `-O0` (aucune optimisation) à `-O3` (optimisations agressives).

### `-O0` : Aucune optimisation (par défaut)

```bash
gcc -O0 mon_programme.c -o mon_programme
# ou simplement (O0 est le défaut)
gcc mon_programme.c -o mon_programme
```

**Caractéristiques :**
- Code généré très proche du code source
- Temps de compilation le plus rapide
- Débogage facile : correspondance directe entre code C et assembleur
- **Performance : la plus faible**

**Quand l'utiliser :**
- ✅ Pendant le développement actif
- ✅ Pour le débogage avec GDB
- ✅ Pour comprendre le comportement exact du programme

---

### `-O1` : Optimisations basiques

```bash
gcc -O1 mon_programme.c -o mon_programme
```

**Caractéristiques :**
- Active les optimisations "sans risque" qui ne consomment pas trop de temps de compilation
- Réduit la taille du code et améliore la vitesse
- Débogage encore relativement facile
- Compromis intéressant entre `-O0` et `-O2`

**Optimisations typiques activées :**
- Élimination des sous-expressions communes
- Suppression du code mort (code jamais exécuté)
- Réutilisation de registres CPU

**Quand l'utiliser :**
- ✅ Pour une première amélioration des performances
- ✅ Quand vous voulez optimiser sans perdre la traçabilité du code

---

### `-O2` : Optimisations standards (recommandé pour la production)

```bash
gcc -O2 mon_programme.c -o mon_programme
```

**Caractéristiques :**
- **Niveau le plus utilisé en production**
- Active presque toutes les optimisations qui n'impliquent pas de compromis espace/vitesse
- Bon équilibre entre performance, taille et sécurité
- Temps de compilation modéré

**Optimisations supplémentaires par rapport à `-O1` :**
- Inlining de fonctions (les petites fonctions sont intégrées directement)
- Optimisation des boucles
- Réorganisation des instructions (instruction scheduling)
- Optimisation des accès mémoire
- Déroulage de boucles (loop unrolling) limité

**Impact :**
- ⚡ **Amélioration typique : 20-50% plus rapide que `-O0`**
- 📦 Taille du binaire : généralement plus grosse que `-O0` (à cause de l'inlining)
- 🐛 Débogage : plus difficile mais encore possible

**Quand l'utiliser :**
- ✅ **Pour les builds de production**
- ✅ Quand vous voulez de bonnes performances sans risques
- ✅ Pour les tests de performance

---

### `-O3` : Optimisations agressives

```bash
gcc -O3 mon_programme.c -o mon_programme
```

**Caractéristiques :**
- Active toutes les optimisations de `-O2`
- Ajoute des optimisations plus agressives qui peuvent augmenter la taille du code
- Peut parfois rendre le code **plus lent** (à cause de la taille du code qui dépasse le cache)
- Temps de compilation plus long

**Optimisations supplémentaires :**
- Inlining plus agressif
- Déroulage de boucles plus important
- Vectorisation automatique (utilisation des instructions SIMD)
- Prédiction de branchement plus sophistiquée
- Optimisations inter-procédurales

**Impact :**
- ⚡ **Gain typique : 5-15% plus rapide que `-O2`** (mais pas garanti)
- 📦 Binaire souvent beaucoup plus gros
- 🐛 Débogage très difficile

**Quand l'utiliser :**
- ✅ Pour du code critique en performance (boucles de calcul intensif)
- ✅ Après avoir mesuré que `-O3` est vraiment plus rapide que `-O2`
- ⚠️ À éviter par défaut : commencez toujours par `-O2`

**⚠️ Attention :** Certaines optimisations de `-O3` peuvent exposer des bugs subtils dans du code qui viole les standards C (undefined behavior).

---

### `-Os` : Optimisation pour la taille

```bash
gcc -Os mon_programme.c -o mon_programme
```

**Caractéristiques :**
- Basé sur `-O2` mais privilégie la **réduction de la taille** du binaire
- Désactive les optimisations qui augmentent la taille (comme l'inlining agressif)
- Idéal pour les systèmes embarqués avec peu de mémoire

**Impact :**
- 📦 Binaire le plus petit possible
- ⚡ Performance : généralement entre `-O1` et `-O2`

**Quand l'utiliser :**
- ✅ Systèmes embarqués avec contraintes mémoire
- ✅ Microcontrôleurs
- ✅ Firmware

---

### `-Ofast` : Optimisations maximales (non strictes)

```bash
gcc -Ofast mon_programme.c -o mon_programme
```

**Caractéristiques :**
- Active toutes les optimisations de `-O3`
- **Désactive certaines garanties du standard C** (notamment pour les calculs en virgule flottante)
- Active `-ffast-math` qui peut changer le comportement mathématique

**⚠️ DANGER :** Peut produire des résultats **incorrects** pour certains programmes, notamment ceux qui :
- Font des calculs en virgule flottante sensibles
- Dépendent de l'ordre des opérations mathématiques
- Utilisent des valeurs spéciales (NaN, Inf)

**Quand l'utiliser :**
- ✅ Calculs scientifiques où la précision exacte n'est pas critique
- ✅ Jeux vidéo (graphismes, physique approximative)
- ⚠️ **À éviter pour du code financier ou médical**

---

## Comparaison visuelle des niveaux

| Flag | Vitesse | Taille binaire | Temps compil. | Débogage | Usage recommandé |
|------|---------|----------------|---------------|----------|------------------|
| `-O0` | ⭐ | ⭐⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐⭐⭐ | Développement |
| `-O1` | ⭐⭐ | ⭐⭐ | ⭐⭐⭐ | ⭐⭐⭐ | Compromis dev |
| `-O2` | ⭐⭐⭐ | ⭐⭐ | ⭐⭐ | ⭐⭐ | **Production** |
| `-O3` | ⭐⭐⭐⭐ | ⭐ | ⭐ | ⭐ | Perf critique |
| `-Os` | ⭐⭐ | ⭐⭐⭐⭐ | ⭐⭐ | ⭐⭐ | Embarqué |
| `-Ofast` | ⭐⭐⭐⭐⭐ | ⭐ | ⭐ | ⭐ | Calculs approx. |

*(Plus d'étoiles = meilleur dans cette catégorie)*

---

## Flags d'optimisation individuels utiles

Au-delà des niveaux prédéfinis, GCC offre des centaines de flags individuels. Voici les plus importants pour les débutants.

### `-march=native` : Optimiser pour votre CPU

```bash
gcc -O2 -march=native mon_programme.c -o mon_programme
```

**Effet :**
- Active les instructions spécifiques à votre processeur (AVX2, SSE4, etc.)
- Peut améliorer significativement les performances (10-30% dans certains cas)
- ⚠️ **Le binaire ne fonctionnera que sur des CPUs similaires**

**Quand l'utiliser :**
- ✅ Pour votre machine locale
- ✅ Dans des environnements homogènes (datacenter avec les mêmes CPUs)
- ❌ À éviter pour distribuer un logiciel à d'autres personnes

---

### `-flto` : Link-Time Optimization

```bash
# Compilation
gcc -O2 -flto -c fichier1.c -o fichier1.o  
gcc -O2 -flto -c fichier2.c -o fichier2.o  

# Linkage
gcc -O2 -flto fichier1.o fichier2.o -o programme
```

**Effet :**
- Permet l'optimisation **entre plusieurs fichiers** lors du linkage
- Peut éliminer du code mort à l'échelle du projet complet
- Amélioration typique : 5-10%

**Coût :**
- Temps de compilation/linkage beaucoup plus long
- Consommation mémoire élevée pendant le linkage

**Quand l'utiliser :**
- ✅ Pour les builds de release finaux
- ✅ Sur des projets multi-fichiers

---

### `-fomit-frame-pointer` : Libérer un registre

```bash
gcc -O2 -fomit-frame-pointer mon_programme.c -o mon_programme
```

**Effet :**
- Supprime le "frame pointer" pour libérer un registre CPU supplémentaire
- Gain typique : 1-3%
- ⚠️ Rend le débogage avec GDB plus difficile (backtrace incomplet)

**Note :** Activé automatiquement sur x86-64 avec `-O2` et supérieur.

---

### `-funroll-loops` : Dérouler les boucles

```bash
gcc -O2 -funroll-loops mon_programme.c -o mon_programme
```

**Effet :**
- Déplie les boucles pour réduire les sauts conditionnels
- Peut améliorer les performances de 5-20% sur du code avec beaucoup de boucles
- Augmente la taille du code

**Exemple de transformation :**
```c
// Code original
for (int i = 0; i < 4; i++) {
    tableau[i] = 0;
}

// Après déroulage
tableau[0] = 0;  
tableau[1] = 0;  
tableau[2] = 0;  
tableau[3] = 0;  
```

---

## Flags de débogage combinés avec l'optimisation

### `-g` : Informations de débogage

```bash
gcc -O2 -g mon_programme.c -o mon_programme
```

**Effet :**
- Ajoute les symboles de débogage au binaire
- Permet d'utiliser GDB même avec un code optimisé
- N'affecte **pas** les performances du programme final
- Augmente la taille du binaire

**⭐ Recommandation :** Toujours compiler avec `-g` en développement, même avec `-O2`.

---

### `-Og` : Optimisations compatibles avec le débogage

```bash
gcc -Og -g mon_programme.c -o mon_programme
```

**Effet :**
- Niveau d'optimisation intermédiaire entre `-O0` et `-O1`
- Désactive les optimisations qui compliquent le débogage
- Meilleur compromis pour déboguer un programme qui doit être "rapide"

**Quand l'utiliser :**
- ✅ Quand vous devez déboguer un bug qui n'apparaît qu'avec les optimisations activées
- ✅ Pour avoir une expérience de débogage correcte avec de meilleures performances que `-O0`

---

## Stratégie recommandée pour les débutants

### Phase de développement
```bash
gcc -Wall -Wextra -Werror -g -O0 mon_programme.c -o mon_programme
```
- Tous les warnings activés
- Aucune optimisation pour faciliter le débogage
- Symboles de débogage inclus

### Tests et validation
```bash
gcc -Wall -Wextra -g -O2 mon_programme.c -o mon_programme
```
- Optimisations standards activées
- Toujours avec les symboles pour déboguer si nécessaire

### Build de production
```bash
gcc -Wall -Wextra -O2 -march=native mon_programme.c -o mon_programme
```
- Optimisations standards
- Optimisations spécifiques au CPU (si environnement homogène)
- Pas forcément besoin de `-g` (sauf si vous voulez analyser des core dumps)

---

## Vérifier les optimisations appliquées

### Voir les flags activés par un niveau d'optimisation

```bash
# Affiche tous les flags activés par -O2
gcc -O2 -Q --help=optimizers

# Comparer deux niveaux
gcc -O1 -Q --help=optimizers > o1.txt  
gcc -O2 -Q --help=optimizers > o2.txt  
diff o1.txt o2.txt  
```

### Inspecter le code assembleur généré

```bash
# Générer l'assembleur
gcc -O0 -S mon_programme.c -o mon_programme_O0.s  
gcc -O2 -S mon_programme.c -o mon_programme_O2.s  

# Comparer
diff mon_programme_O0.s mon_programme_O2.s
```

Cela vous permet de voir concrètement les transformations appliquées par le compilateur.

---

## Erreurs courantes à éviter

### ❌ Utiliser `-O3` par défaut
**Pourquoi c'est une erreur :**
- `-O3` n'est pas toujours plus rapide que `-O2`
- Peut augmenter excessivement la taille du code
- Risque d'exposer des bugs subtils

**✅ Solution :** Commencez toujours par `-O2`, et ne passez à `-O3` qu'après avoir mesuré un gain réel.

---

### ❌ Compiler sans `-g` pendant le développement
**Pourquoi c'est une erreur :**
- Impossible de déboguer efficacement avec GDB
- Pas d'impact sur les performances, seulement sur la taille du binaire

**✅ Solution :** Toujours utiliser `-g` en développement.

---

### ❌ Utiliser `-Ofast` sans comprendre les implications
**Pourquoi c'est une erreur :**
- Peut produire des résultats mathématiquement incorrects
- Viole les standards IEEE 754 pour les flottants

**✅ Solution :** Réservez `-Ofast` aux cas où vous avez validé que votre code tolère les approximations.

---

### ❌ Ne jamais mesurer les performances
**Pourquoi c'est une erreur :**
- L'optimisation sans mesure est de l'optimisation prématurée
- `-O3` peut être plus lent que `-O2` dans certains cas

**✅ Solution :** Utilisez des outils de profiling (voir section 27.3) avant d'optimiser.

---

## Résumé des bonnes pratiques

| Situation | Flags recommandés |
|-----------|-------------------|
| 💻 **Développement quotidien** | `-O0 -g -Wall -Wextra` |
| 🧪 **Tests / CI/CD** | `-O2 -g -Wall -Wextra -Werror` |
| 🚀 **Production (général)** | `-O2 -march=native` |
| 📱 **Embarqué (mémoire limitée)** | `-Os` |
| ⚡ **Performance critique** | `-O3` (après benchmark vs `-O2`) |
| 🐛 **Débogage avec optim** | `-Og -g` |

---

## Pour aller plus loin

### Documentation officielle
```bash
# Lister toutes les options d'optimisation
man gcc | grep -A 1000 "Optimize Options"

# Ou en ligne
info gcc
```

### Ressources
- **GCC Manual - Optimize Options** : https://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html
- **Godbolt Compiler Explorer** : https://godbolt.org/ (pour visualiser l'assembleur généré)
- **Quick Bench** : https://quick-bench.com/ (pour comparer les performances)

---

## Conclusion

Les flags d'optimisation GCC sont un outil puissant mais qui nécessite de la prudence :

1. **Commencez simple** : `-O0` en dev, `-O2` en prod
2. **Mesurez toujours** : ne présumez pas qu'une optimisation est meilleure
3. **Compilez avec `-g`** : cela n'a pas de coût en performance
4. **Utilisez `-O3`/`-Ofast` uniquement après validation**
5. **Documentez vos choix** : expliquez pourquoi vous utilisez telle ou telle option

L'optimisation est un processus itératif : commencez conservateur, profilez, puis optimisez les parties critiques identifiées.

---

*Prochaine section : 27.2 Comprendre l'optimiseur*

⏭️ [Comprendre l'optimiseur](/27-optimisation-performance/02-comprendre-optimiseur.md)
