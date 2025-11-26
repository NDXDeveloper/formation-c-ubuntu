🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 14.3 Différences statiques vs dynamiques

## Introduction

Lorsque vous développez en C, vous utiliserez inévitablement des bibliothèques externes (comme `math.h`, `pthread`, ou vos propres bibliothèques). Ces bibliothèques peuvent être liées de deux manières fondamentalement différentes : **statiquement** ou **dynamiquement**. Comprendre ces différences est essentiel pour choisir la meilleure approche selon votre contexte.

Dans cette section, nous allons comparer ces deux types de liaison en détail, avec leurs avantages, inconvénients, et cas d'usage typiques.

---

## Vue d'ensemble

### Bibliothèque Statique (.a)
- **Extension** : `.a` sous Linux (archive)
- **Principe** : Le code de la bibliothèque est **copié intégralement** dans votre exécutable lors de la compilation
- **Résultat** : Un seul fichier exécutable autonome et indépendant

### Bibliothèque Dynamique (.so)
- **Extension** : `.so` sous Linux (shared object), `.dll` sous Windows
- **Principe** : Le code de la bibliothèque reste **externe** et est chargé en mémoire au moment de l'exécution
- **Résultat** : Un exécutable + un ou plusieurs fichiers `.so` séparés

---

## Comparaison détaillée

### 1. **Taille des fichiers**

#### Bibliothèque Statique
```
Programme 1 → 500 Ko (inclut libmath)
Programme 2 → 450 Ko (inclut libmath)
Programme 3 → 480 Ko (inclut libmath)
───────────────────────────────────
Total : 1430 Ko
```

- ✅ **Avantage** : Un seul fichier à distribuer par programme
- ❌ **Inconvénient** : Duplication du code de la bibliothèque dans chaque exécutable

#### Bibliothèque Dynamique
```
Programme 1 → 50 Ko
Programme 2 → 40 Ko
Programme 3 → 45 Ko
libmath.so  → 200 Ko (partagée)
───────────────────────────────────
Total : 335 Ko
```

- ✅ **Avantage** : Code partagé = gain de place disque
- ❌ **Inconvénient** : Nécessite de gérer plusieurs fichiers

---

### 2. **Utilisation de la mémoire (RAM)**

#### Bibliothèque Statique
Chaque processus charge **sa propre copie** du code de la bibliothèque en mémoire :

```
Processus A : [Code du programme A + libmath]
Processus B : [Code du programme B + libmath]
Processus C : [Code du programme C + libmath]
```

**Mémoire totale** : Code dupliqué pour chaque processus actif.

#### Bibliothèque Dynamique
Le système charge **une seule copie** de la bibliothèque en mémoire, partagée entre tous les processus :

```
Processus A : [Code du programme A] ───┐
Processus B : [Code du programme B] ───┼──→ [libmath.so en mémoire]
Processus C : [Code du programme C] ───┘
```

**Mémoire totale** : Une seule copie de `libmath.so` même avec 10 programmes qui l'utilisent.

- ✅ **Avantage dynamique** : Économie massive de RAM sur un système multi-processus
- ✅ **Avantage statique** : Pas de gestion complexe de la mémoire partagée

---

### 3. **Mises à jour et corrections de bugs**

#### Bibliothèque Statique

Si une faille de sécurité est découverte dans `libmath.a` :

```
❌ Il faut recompiler TOUS les programmes qui l'utilisent
❌ Redistribuer tous les exécutables mis à jour
❌ Les utilisateurs doivent réinstaller chaque programme
```

**Exemple concret** : Une faille dans OpenSSL nécessite de recompiler des milliers d'applications.

#### Bibliothèque Dynamique

Si une faille est découverte dans `libmath.so` :

```
✅ Mettre à jour uniquement libmath.so sur le système
✅ Tous les programmes bénéficient automatiquement du correctif
✅ Pas besoin de recompiler ou redistribuer les programmes
```

**Exemple concret** : Un simple `apt upgrade` corrige la faille pour toutes les applications.

---

### 4. **Portabilité et distribution**

#### Bibliothèque Statique

✅ **Avantages** :
- L'exécutable est **autonome** : un seul fichier à copier
- Aucune dépendance externe à gérer
- Fonctionne sur n'importe quel système Linux (même sans la bibliothèque installée)
- Idéal pour les **binaires standalone**

❌ **Inconvénients** :
- Fichier exécutable volumineux
- Problèmes de licence (certaines bibliothèques ne permettent pas la liaison statique)

**Cas d'usage** : Outils CLI, applications embarquées, environnements sans gestionnaire de paquets

#### Bibliothèque Dynamique

✅ **Avantages** :
- Exécutables légers
- Respect des licences de type LGPL (qui exigent la liaison dynamique)
- Facilite les mises à jour de sécurité

❌ **Inconvénients** :
- L'utilisateur doit avoir la bibliothèque `.so` installée
- Risque de "dependency hell" (version incompatible)
- Erreur classique : `error while loading shared libraries: libfoo.so.1`

**Cas d'usage** : Applications système, distributions Linux, environnements gérés

---

### 5. **Temps de compilation et de chargement**

#### Bibliothèque Statique

- **Compilation** : Plus lente (le linker doit copier tout le code)
- **Démarrage du programme** : Légèrement plus rapide (tout est déjà dans l'exécutable)

#### Bibliothèque Dynamique

- **Compilation** : Plus rapide (le linker ne copie que les références)
- **Démarrage du programme** : Légèrement plus lent (le système doit localiser et charger les `.so`)

💡 **Note** : Ces différences sont souvent négligeables sur du matériel moderne.

---

### 6. **Versioning et compatibilité**

#### Bibliothèque Statique

```bash
# Votre programme utilise libfoo.a version 1.2
gcc main.c -L. -lfoo -o mon_programme

# Des années plus tard, libfoo.a est en version 3.0
# Votre programme fonctionne toujours avec la version 1.2 intégrée ✅
```

✅ Aucun problème de compatibilité : la version est "figée" dans l'exécutable

#### Bibliothèque Dynamique

```bash
# Votre programme utilise libfoo.so.1.2
$ ldd mon_programme
    libfoo.so.1 => /usr/lib/libfoo.so.1.2.0

# L'admin système met à jour vers libfoo.so.2.0
# Votre programme peut crasher si l'ABI a changé ❌
```

❌ Risque de rupture de compatibilité (d'où l'importance du **SONAME** et du versioning)

**Solution** : Utiliser le système de versioning SONAME (ex: `libfoo.so.1` reste rétrocompatible)

---

## Tableau récapitulatif

| Critère | Statique (.a) | Dynamique (.so) |
|---------|---------------|-----------------|
| **Taille exécutable** | ❌ Gros | ✅ Petit |
| **Taille totale disque** | ❌ Duplication | ✅ Code partagé |
| **Utilisation RAM** | ❌ Duplication | ✅ Code partagé |
| **Portabilité** | ✅ Autonome | ❌ Dépendances |
| **Mises à jour** | ❌ Recompilation | ✅ Automatique |
| **Démarrage** | ✅ Rapide | ⚠️ Légèrement plus lent |
| **Compilation** | ⚠️ Plus lente | ✅ Plus rapide |
| **Versioning** | ✅ Figé | ⚠️ Risques |
| **Licence** | ⚠️ Restrictions | ✅ Compatible LGPL |

---

## Quand choisir quoi ?

### Choisissez la liaison **statique** si :

- ✅ Vous développez un **outil CLI standalone** (ex: `htop`, `neofetch`)
- ✅ Vous ciblez des **systèmes embarqués** avec peu de RAM et pas de gestionnaire de paquets
- ✅ Vous voulez une **distribution simplifiée** (un seul fichier)
- ✅ La bibliothèque est **petite** et rarement mise à jour
- ✅ Vous avez besoin de **performances maximales** au démarrage

**Exemples** : BusyBox (outils Linux embarqués), Go (lie tout statiquement par défaut)

---

### Choisissez la liaison **dynamique** si :

- ✅ Vous développez une **application système** intégrée à Linux
- ✅ Vous utilisez des **bibliothèques volumineuses** (GTK, Qt, OpenSSL)
- ✅ Les **mises à jour de sécurité** sont critiques
- ✅ Plusieurs programmes utilisent la **même bibliothèque**
- ✅ La bibliothèque impose une licence **LGPL**

**Exemples** : Firefox, LibreOffice, serveurs web (Nginx, Apache)

---

## Hybride : Le meilleur des deux mondes ?

Il est possible de **mélanger les deux approches** dans un même projet :

```bash
# Lier statiquement vos bibliothèques custom
gcc main.c libmylib.a -o mon_programme

# Lier dynamiquement les bibliothèques système
gcc main.c libmylib.a -lpthread -lm -o mon_programme
#                      ^^^^^^^^ dynamique
```

**Stratégie courante** :
- **Statique** pour votre code propriétaire
- **Dynamique** pour les bibliothèques système standard (`libc`, `pthread`, `m`)

---

## Cas réels d'utilisation

### Exemple 1 : Serveur web (Nginx)

```
nginx (exécutable dynamique)
├── libz.so.1           (compression)
├── libssl.so.1.1       (HTTPS)
├── libcrypto.so.1.1    (cryptographie)
└── libpcre.so.3        (regex)
```

**Pourquoi dynamique ?**
- Mises à jour de sécurité critiques (OpenSSL)
- Code partagé avec Apache, curl, wget
- Économie de RAM avec des milliers de connexions

---

### Exemple 2 : Outil CLI personnalisé

```
mon_outil (exécutable statique)
└── [tout est intégré]
```

**Pourquoi statique ?**
- Distribution simplifiée : `curl -o mon_outil https://...`
- Pas de "dependency hell"
- Fonctionne sur Alpine Linux, CentOS, Ubuntu sans adaptation

---

### Exemple 3 : Application Desktop (GIMP)

```
gimp (exécutable dynamique)
├── libgtk-3.so.0       (interface graphique)
├── libglib-2.0.so.0    (utilitaires)
├── libcairo.so.2       (dessin vectoriel)
├── libpng16.so.16      (images PNG)
└── ... (50+ bibliothèques)
```

**Pourquoi dynamique ?**
- Partage du toolkit GTK avec tous les programmes GNOME
- Taille du code énorme si statique
- Mises à jour GTK bénéficient à toutes les apps

---

## Points d'attention

### ⚠️ Attention avec les bibliothèques statiques

1. **Problème de licence** : La GNU LGPL **interdit** la liaison statique dans certains cas
2. **Sécurité** : Un bug dans une bibliothèque statique nécessite de tout recompiler
3. **Taille** : Un programme lié statiquement avec Qt peut faire 50+ Mo

### ⚠️ Attention avec les bibliothèques dynamiques

1. **Dépendances manquantes** : `./mon_programme: error while loading shared libraries`
2. **Incompatibilités de versions** : Le système a `libfoo.so.2` mais le programme veut `libfoo.so.1`
3. **LD_LIBRARY_PATH** : Configuration nécessaire si les `.so` ne sont pas dans les chemins standard

---

## Comment vérifier le type de liaison ?

```bash
# Vérifier si un programme est statique ou dynamique
$ file mon_programme
mon_programme: ELF 64-bit LSB executable, dynamically linked

# Lister les bibliothèques dynamiques utilisées
$ ldd mon_programme
    linux-vdso.so.1 (0x00007fff...)
    libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6

# Si statique, ldd affiche :
$ ldd mon_programme_statique
    not a dynamic executable
```

---

## Conclusion

Le choix entre bibliothèques statiques et dynamiques n'est pas binaire : il dépend de votre contexte, de vos contraintes et de vos priorités.

**Règle générale** :
- **Statique** = Simplicité, autonomie, performance (au prix de la taille)
- **Dynamique** = Efficacité mémoire, mises à jour faciles (au prix de la complexité)

Dans la pratique, **la plupart des applications Linux modernes utilisent la liaison dynamique** pour profiter de l'écosystème partagé, tandis que les **outils CLI et embedded préfèrent souvent le statique** pour leur simplicité de déploiement.

---

## Pour aller plus loin

**Sections connexes recommandées** :
- **14.1** : Bibliothèques statiques (.a) - Compilation détaillée
- **14.2** : Bibliothèques dynamiques (.so) - Mécanismes de chargement
- **14.4** : Gestion du LD_LIBRARY_PATH
- **31.8** : Conteneurisation Docker multi-stage (gestion des dépendances)

**Ressources externes** :
- `man ld` : Documentation du linker
- `man ldd` : Lister les dépendances dynamiques
- *Advanced Linux Programming* : Chapitre sur la liaison dynamique

---

**🎯 Points clés à retenir** :
1. Statique = Code copié dans l'exécutable (autonome)
2. Dynamique = Code externe chargé au runtime (partagé)
3. Statique = Gros fichier, pas de dépendances
4. Dynamique = Petit fichier, économie de RAM, mises à jour faciles
5. Le choix dépend du contexte : embedded vs desktop, sécurité, licence

⏭️ [Gestion du LD_LIBRARY_PATH](/14-bibliotheques/04-ld-library-path.md)
