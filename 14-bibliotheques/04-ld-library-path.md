🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 14.4 Gestion du LD_LIBRARY_PATH

## Introduction

Lorsque vous exécutez un programme qui utilise des bibliothèques dynamiques (fichiers `.so`), le système doit **localiser ces bibliothèques** avant de pouvoir démarrer votre programme. Mais où le système va-t-il chercher ces fichiers `.so` ?

C'est là qu'intervient **LD_LIBRARY_PATH**, une variable d'environnement cruciale qui indique au **dynamic linker** (le composant système qui charge les bibliothèques) où chercher les bibliothèques partagées.

Dans cette section, nous allons comprendre :
- Comment fonctionne la recherche de bibliothèques dynamiques
- Qu'est-ce que LD_LIBRARY_PATH et comment l'utiliser
- Les alternatives plus robustes
- Les problèmes courants et leurs solutions

---

## Le problème : "Library not found"

### Scénario typique

Vous venez de compiler votre programme qui utilise une bibliothèque dynamique personnalisée :

```bash
$ gcc main.c -L./lib -lmylib -o mon_programme
$ ./mon_programme
./mon_programme: error while loading shared libraries: libmylib.so: cannot open shared object file: No such file or directory
```

❌ **Erreur !** Pourtant, la compilation s'est bien passée. Que s'est-il passé ?

**Explication** :
- **À la compilation** : GCC a trouvé `libmylib.so` dans `./lib` grâce à l'option `-L./lib`
- **À l'exécution** : Le dynamic linker (`ld.so`) ne sait pas où chercher `libmylib.so`

---

## Comment fonctionne la recherche de bibliothèques ?

Lorsque vous lancez un programme, le **dynamic linker** suit un ordre de recherche précis pour localiser les bibliothèques `.so` :

### 1. Les chemins codés en dur (RPATH/RUNPATH)
```bash
# Intégrés dans l'exécutable lors de la compilation
gcc main.c -L./lib -lmylib -Wl,-rpath,./lib -o mon_programme
```

### 2. La variable LD_LIBRARY_PATH
```bash
export LD_LIBRARY_PATH=/home/user/mylibs:$LD_LIBRARY_PATH
```

### 3. Les chemins système standards
```
/lib
/lib64
/usr/lib
/usr/lib64
/usr/local/lib
```

### 4. Le cache système (/etc/ld.so.cache)
```bash
# Mis à jour avec ldconfig
sudo ldconfig
```

---

## Qu'est-ce que LD_LIBRARY_PATH ?

### Définition

**LD_LIBRARY_PATH** est une **variable d'environnement** qui contient une liste de répertoires (séparés par `:`) dans lesquels le dynamic linker doit chercher les bibliothèques partagées.

### Syntaxe

```bash
# Définir un seul chemin
export LD_LIBRARY_PATH=/home/user/mylibs

# Définir plusieurs chemins (séparés par :)
export LD_LIBRARY_PATH=/home/user/mylibs:/opt/custom/lib

# Ajouter un chemin en préservant les chemins existants
export LD_LIBRARY_PATH=/home/user/mylibs:$LD_LIBRARY_PATH
```

### Portée

```bash
# Variable d'environnement du shell courant
$ export LD_LIBRARY_PATH=/home/user/mylibs
$ ./mon_programme  # ✅ Utilise cette variable

# Nouvelle session shell
$ ./mon_programme  # ❌ Variable perdue
```

---

## Utilisation pratique de LD_LIBRARY_PATH

### Cas 1 : Bibliothèque dans un répertoire personnalisé

**Structure du projet** :
```
mon_projet/
├── bin/
│   └── mon_programme
├── lib/
│   └── libmylib.so
└── src/
    └── main.c
```

**Solution temporaire** :
```bash
$ cd mon_projet
$ export LD_LIBRARY_PATH=$(pwd)/lib:$LD_LIBRARY_PATH
$ ./bin/mon_programme
Hello from libmylib!  ✅
```

**Vérification** :
```bash
$ echo $LD_LIBRARY_PATH
/home/user/mon_projet/lib:/usr/local/lib
```

---

### Cas 2 : Lancer un programme avec LD_LIBRARY_PATH temporaire

Au lieu de modifier la variable globalement, vous pouvez la définir **uniquement pour une commande** :

```bash
# Syntaxe : VARIABLE=valeur commande
$ LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH ./mon_programme
```

✅ **Avantage** : La variable ne persiste pas après l'exécution

```bash
$ echo $LD_LIBRARY_PATH
# Vide ou valeur précédente
```

---

### Cas 3 : Définir LD_LIBRARY_PATH de manière permanente

#### Dans votre session utilisateur

**Pour Bash** (~/.bashrc) :
```bash
# Ajouter à la fin du fichier ~/.bashrc
export LD_LIBRARY_PATH=/home/user/mylibs:$LD_LIBRARY_PATH

# Recharger la configuration
$ source ~/.bashrc
```

**Pour Zsh** (~/.zshrc) :
```bash
# Ajouter à la fin du fichier ~/.zshrc
export LD_LIBRARY_PATH=/home/user/mylibs:$LD_LIBRARY_PATH

# Recharger la configuration
$ source ~/.zshrc
```

⚠️ **Attention** : Cela affectera **tous les programmes** lancés depuis ce shell.

---

### Cas 4 : Créer un script wrapper

Pour distribuer votre application avec ses bibliothèques, créez un **script de lancement** :

**launch.sh** :
```bash
#!/bin/bash

# Obtenir le répertoire du script
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

# Définir LD_LIBRARY_PATH pour pointer vers nos bibliothèques
export LD_LIBRARY_PATH="${SCRIPT_DIR}/lib:${LD_LIBRARY_PATH}"

# Lancer le programme
exec "${SCRIPT_DIR}/bin/mon_programme" "$@"
```

**Utilisation** :
```bash
$ chmod +x launch.sh
$ ./launch.sh
Hello from libmylib!  ✅
```

✅ **Avantages** :
- Portable (fonctionne peu importe où le dossier est placé)
- N'affecte pas l'environnement global
- Passe les arguments au programme (`"$@"`)

---

## Vérifier et déboguer

### 1. Afficher les bibliothèques utilisées par un programme

```bash
$ ldd mon_programme
    linux-vdso.so.1 (0x00007ffc...)
    libmylib.so => /home/user/mylibs/libmylib.so (0x00007f...)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f...)
    /lib64/ld-linux-x86-64.so.2 (0x00007f...)
```

- ✅ Si vous voyez le chemin complet après `=>`, la bibliothèque est trouvée
- ❌ Si vous voyez `not found`, la bibliothèque est manquante

---

### 2. Afficher la valeur actuelle de LD_LIBRARY_PATH

```bash
$ echo $LD_LIBRARY_PATH
/home/user/mylibs:/opt/custom/lib
```

Si vide, seuls les chemins système standards sont utilisés.

---

### 3. Tester un chemin avant de l'ajouter

```bash
# Vérifier que le fichier .so existe
$ ls -lh /home/user/mylibs/libmylib.so
-rwxr-xr-x 1 user user 245K Nov 26 10:30 /home/user/mylibs/libmylib.so

# Tester avec ldd
$ LD_LIBRARY_PATH=/home/user/mylibs ldd mon_programme
```

---

### 4. Déboguer avec LD_DEBUG

Pour voir en détail comment le dynamic linker cherche les bibliothèques :

```bash
$ LD_DEBUG=libs ./mon_programme
    # Affiche tous les chemins testés et les bibliothèques chargées

$ LD_DEBUG=files ./mon_programme
    # Affiche les fichiers ouverts

$ LD_DEBUG=all ./mon_programme
    # Mode ultra-verbeux (beaucoup d'informations)
```

**Exemple de sortie** :
```
     18234:     find library=libmylib.so [0]; searching
     18234:      search path=/home/user/mylibs    (LD_LIBRARY_PATH)
     18234:      trying file=/home/user/mylibs/libmylib.so
```

---

## Problèmes courants et solutions

### Problème 1 : "Library not found" malgré LD_LIBRARY_PATH

**Symptôme** :
```bash
$ export LD_LIBRARY_PATH=/home/user/mylibs
$ ldd mon_programme
    libmylib.so => not found
```

**Causes possibles** :

#### a) Le chemin est incorrect
```bash
# Vérifier que le fichier existe vraiment
$ ls /home/user/mylibs/libmylib.so
ls: cannot access '/home/user/mylibs/libmylib.so': No such file or directory

# Solution : Corriger le chemin
$ ls /home/user/myproject/lib/libmylib.so  # ✅ Trouvé !
$ export LD_LIBRARY_PATH=/home/user/myproject/lib
```

#### b) Mauvais nom de fichier
```bash
# Votre programme cherche libmylib.so mais le fichier s'appelle libmylib.so.1
$ ls /home/user/mylibs/
libmylib.so.1.0.0

# Solution : Créer un lien symbolique
$ ln -s libmylib.so.1.0.0 libmylib.so
$ ls -l
libmylib.so -> libmylib.so.1.0.0  ✅
```

#### c) Problème de permissions
```bash
$ ls -l /home/user/mylibs/libmylib.so
-rw------- 1 user user 245K Nov 26 10:30 libmylib.so  # ❌ Pas de permission d'exécution

# Solution : Ajouter les permissions
$ chmod 755 /home/user/mylibs/libmylib.so
```

---

### Problème 2 : LD_LIBRARY_PATH ne persiste pas

**Symptôme** :
```bash
$ export LD_LIBRARY_PATH=/home/user/mylibs
$ ./mon_programme  # ✅ Fonctionne

# Nouvelle session terminal
$ ./mon_programme  # ❌ Library not found
$ echo $LD_LIBRARY_PATH
# Vide
```

**Solution** : Ajouter à ~/.bashrc ou ~/.zshrc

```bash
$ echo 'export LD_LIBRARY_PATH=/home/user/mylibs:$LD_LIBRARY_PATH' >> ~/.bashrc
$ source ~/.bashrc
```

---

### Problème 3 : Conflit de versions

**Symptôme** :
```bash
$ export LD_LIBRARY_PATH=/tmp/test_lib
$ ./mon_programme
Segmentation fault (core dumped)
```

**Cause** : Vous avez une version incompatible de la bibliothèque dans `/tmp/test_lib`

**Solution** : Vérifier les versions
```bash
$ ldd mon_programme
    libmylib.so.2 => /tmp/test_lib/libmylib.so.2  # ⚠️ Version 2

# Votre programme a été compilé avec libmylib.so.1
# Solution : Utiliser la bonne version
$ export LD_LIBRARY_PATH=/usr/local/lib  # Contient libmylib.so.1
```

---

### Problème 4 : LD_LIBRARY_PATH affecte tous les programmes

**Symptôme** :
```bash
$ export LD_LIBRARY_PATH=/home/user/mylibs
$ ls
ls: error while loading shared libraries: libc.so.6: version GLIBC_2.34 not found
```

❌ **Danger** : Vous avez écrasé les chemins système !

**Cause** :
```bash
# ❌ MAUVAIS (écrase tout)
export LD_LIBRARY_PATH=/home/user/mylibs

# ✅ BON (ajoute sans écraser)
export LD_LIBRARY_PATH=/home/user/mylibs:$LD_LIBRARY_PATH
```

**Solution immédiate** :
```bash
$ unset LD_LIBRARY_PATH
$ source ~/.bashrc  # Recharger la configuration correcte
```

---

## Alternatives à LD_LIBRARY_PATH

### Pourquoi éviter LD_LIBRARY_PATH ?

⚠️ **Inconvénients** :
- Affecte **tous** les programmes lancés depuis ce shell
- Risques de conflits de versions
- Problèmes de sécurité (un attaquant pourrait injecter une bibliothèque malveillante)
- Non recommandé pour la production

💡 **Règle** : LD_LIBRARY_PATH est excellent pour le **développement**, mais évitez-le en **production**.

---

### Alternative 1 : RPATH (Recommended Path)

Intégrer le chemin de la bibliothèque **directement dans l'exécutable** lors de la compilation :

```bash
$ gcc main.c -L./lib -lmylib -Wl,-rpath,'$ORIGIN/../lib' -o mon_programme
```

**Explication** :
- `-Wl,-rpath,'$ORIGIN/../lib'` : Ajoute un RPATH à l'exécutable
- `$ORIGIN` : Répertoire contenant l'exécutable (résolu au runtime)
- `../lib` : Chemin relatif depuis l'exécutable

**Structure** :
```
mon_projet/
├── bin/
│   └── mon_programme  (cherchera dans ../lib)
└── lib/
    └── libmylib.so
```

**Vérification** :
```bash
$ readelf -d mon_programme | grep RPATH
 0x000000000000000f (RPATH)    Library rpath: [$ORIGIN/../lib]
```

✅ **Avantages** :
- Pas besoin de LD_LIBRARY_PATH
- Portable (fonctionne peu importe où le dossier est copié)
- N'affecte pas les autres programmes

---

### Alternative 2 : Installer dans un chemin système

Copier votre bibliothèque dans un répertoire standard :

```bash
# Copier dans /usr/local/lib (nécessite sudo)
$ sudo cp libmylib.so /usr/local/lib/
$ sudo chmod 755 /usr/local/lib/libmylib.so

# Mettre à jour le cache
$ sudo ldconfig

# Vérifier
$ ldconfig -p | grep libmylib
    libmylib.so (libc6,x86-64) => /usr/local/lib/libmylib.so
```

✅ **Avantages** :
- Disponible pour tous les utilisateurs
- Pas besoin de configuration supplémentaire

❌ **Inconvénients** :
- Nécessite les droits administrateur
- Peut entrer en conflit avec les paquets système

---

### Alternative 3 : Utiliser /etc/ld.so.conf

Ajouter un répertoire personnalisé au cache système :

```bash
# Créer un fichier de configuration
$ sudo nano /etc/ld.so.conf.d/myapp.conf

# Contenu :
/home/user/mylibs

# Recharger le cache
$ sudo ldconfig

# Vérifier
$ ldconfig -p | grep libmylib
```

✅ **Avantages** :
- Configuration permanente
- Pas besoin de LD_LIBRARY_PATH

❌ **Inconvénients** :
- Nécessite les droits administrateur
- Affecte tout le système

---

### Alternative 4 : Créer un package (.deb ou .rpm)

Pour une distribution propre, créez un package qui installe votre bibliothèque correctement :

```bash
# Exemple avec un .deb
$ dpkg -i mon-application_1.0.deb

# Le package installe automatiquement :
# - L'exécutable dans /usr/bin
# - Les bibliothèques dans /usr/lib
# - Lance ldconfig automatiquement
```

✅ **Idéal pour la production**

---

## Comparaison des méthodes

| Méthode | Facilité | Portabilité | Production | Sécurité |
|---------|----------|-------------|------------|----------|
| **LD_LIBRARY_PATH** | ⭐⭐⭐ | ⭐⭐ | ❌ | ⚠️ |
| **RPATH** | ⭐⭐ | ⭐⭐⭐ | ✅ | ✅ |
| **/usr/local/lib** | ⭐⭐ | ⭐ | ✅ | ✅ |
| **/etc/ld.so.conf** | ⭐⭐ | ⭐ | ✅ | ✅ |
| **Package système** | ⭐ | ⭐ | ⭐⭐⭐ | ⭐⭐⭐ |

---

## Bonnes pratiques

### ✅ À faire

1. **Développement** : Utilisez LD_LIBRARY_PATH sans hésitation
   ```bash
   export LD_LIBRARY_PATH=$(pwd)/lib:$LD_LIBRARY_PATH
   ```

2. **Distribution** : Préférez RPATH
   ```bash
   gcc ... -Wl,-rpath,'$ORIGIN/../lib' -o programme
   ```

3. **Production** : Installez dans les chemins système ou créez un package
   ```bash
   sudo cp libmylib.so /usr/local/lib/
   sudo ldconfig
   ```

4. **Toujours ajouter au lieu d'écraser**
   ```bash
   # ✅ BON
   export LD_LIBRARY_PATH=/mon/chemin:$LD_LIBRARY_PATH

   # ❌ MAUVAIS
   export LD_LIBRARY_PATH=/mon/chemin
   ```

5. **Documenter** : Si votre application nécessite LD_LIBRARY_PATH, expliquez-le clairement dans le README

---

### ❌ À éviter

1. Ne jamais définir LD_LIBRARY_PATH dans `/etc/profile` ou `/etc/bash.bashrc`
2. Ne pas inclure de chemins système dans LD_LIBRARY_PATH
3. Ne pas utiliser LD_LIBRARY_PATH pour des scripts lancés par cron ou systemd
4. Ne pas distribuer des applications qui nécessitent LD_LIBRARY_PATH

---

## Cas d'usage pratiques

### Scénario 1 : Développement d'une application

**Situation** : Vous développez une application qui utilise plusieurs bibliothèques custom.

**Solution** :
```bash
# Structure
myapp/
├── build/
│   └── myapp
├── lib/
│   ├── libcore.so
│   └── libutils.so
└── src/

# Script de développement (run_dev.sh)
#!/bin/bash
export LD_LIBRARY_PATH=$(pwd)/lib:$LD_LIBRARY_PATH
./build/myapp "$@"
```

---

### Scénario 2 : Tester plusieurs versions d'une bibliothèque

**Situation** : Vous voulez tester votre application avec différentes versions de libssl.

**Solution** :
```bash
# Version 1.1
$ LD_LIBRARY_PATH=/usr/local/lib/openssl-1.1 ./myapp
OpenSSL 1.1.1  ✅

# Version 3.0
$ LD_LIBRARY_PATH=/usr/local/lib/openssl-3.0 ./myapp
OpenSSL 3.0.0  ✅
```

---

### Scénario 3 : Distribuer une application standalone

**Situation** : Vous voulez distribuer votre application avec toutes ses dépendances.

**Solution** : RPATH + bibliothèques embarquées
```bash
myapp-1.0/
├── myapp (avec RPATH=$ORIGIN/lib)
└── lib/
    ├── libmylib.so
    └── libdep.so

# Compilation
$ gcc ... -Wl,-rpath,'$ORIGIN/lib' -o myapp

# L'utilisateur peut exécuter sans configuration
$ ./myapp  ✅
```

---

## Résumé : Quand utiliser quoi ?

```
┌─────────────────────────────────────────────────────────────┐
│                    Choix de la méthode                      │
├─────────────────────────────────────────────────────────────┤
│ Développement local ?                                       │
│  → LD_LIBRARY_PATH                                          │
│                                                             │
│ Tests temporaires ?                                         │
│  → LD_LIBRARY_PATH=... ./programme                          │
│                                                             │
│ Distribution d'application ?                                │
│  → RPATH ($ORIGIN/../lib)                                   │
│                                                             │
│ Installation système ?                                      │
│  → /usr/local/lib + ldconfig                                │
│                                                             │
│ Package professionnel ?                                     │
│  → .deb/.rpm avec installation standard                     │
└─────────────────────────────────────────────────────────────┘
```

---

## Commandes essentielles à retenir

```bash
# Définir LD_LIBRARY_PATH
export LD_LIBRARY_PATH=/chemin/vers/lib:$LD_LIBRARY_PATH

# Vérifier la valeur
echo $LD_LIBRARY_PATH

# Lister les dépendances
ldd mon_programme

# Déboguer la recherche
LD_DEBUG=libs ./mon_programme

# Vérifier le cache système
ldconfig -p | grep libmylib

# Mettre à jour le cache
sudo ldconfig

# Voir le RPATH d'un exécutable
readelf -d mon_programme | grep RPATH

# Désactiver temporairement
unset LD_LIBRARY_PATH
```

---

## Conclusion

**LD_LIBRARY_PATH** est un outil puissant mais à utiliser avec précaution :

✅ **Excellent pour** :
- Le développement et le débogage
- Les tests rapides
- Les environnements contrôlés

⚠️ **À éviter pour** :
- La production
- Les scripts système
- Les distributions publiques

💡 **Règle d'or** : Utilisez LD_LIBRARY_PATH pour **développer rapidement**, mais migrez vers **RPATH** ou **installation système** pour la **production**.

---

## Pour aller plus loin

**Sections connexes recommandées** :
- **14.1** : Bibliothèques statiques (.a)
- **14.2** : Bibliothèques dynamiques (.so) - SONAME et versioning
- **14.3** : Différences statiques vs dynamiques
- **14.5** : ldd et ldconfig
- **31.8** : Conteneurisation Docker (gestion des dépendances)

**Commandes à explorer** :
- `man ld.so` : Documentation du dynamic linker
- `man ldconfig` : Configuration du cache
- `man readelf` : Inspection des ELF
- `man patchelf` : Modifier RPATH après compilation

**Ressources externes** :
- *Linux System Programming* : Chapitre sur les bibliothèques partagées
- Documentation GNU ld : Section RPATH/RUNPATH

---

**🎯 Points clés à retenir** :
1. LD_LIBRARY_PATH indique au dynamic linker où chercher les `.so`
2. Syntaxe : `export LD_LIBRARY_PATH=/chemin:$LD_LIBRARY_PATH`
3. Toujours ajouter (`:$LD_LIBRARY_PATH`) pour ne pas écraser les chemins système
4. Utilisez `ldd` pour déboguer les problèmes de bibliothèques
5. En production : préférez RPATH ou installation système
6. LD_LIBRARY_PATH = développement, RPATH = distribution

⏭️ [ldd et ldconfig](/14-bibliotheques/05-ldd-ldconfig.md)
