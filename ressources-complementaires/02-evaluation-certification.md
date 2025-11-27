🔝 Retour au [Sommaire](/SOMMAIRE.md)

# Critères d'Évaluation et Certification

## Introduction

Cette section vous explique **comment évaluer votre progression** dans l'apprentissage du langage C et **quels critères vous permettent de valider vos compétences** à chaque niveau. Que vous appreniez en autodidacte ou dans un cadre formel, ces critères vous donnent des repères concrets pour mesurer vos acquis.

> **Note** : Ces critères sont des suggestions pour vous auto-évaluer. Il ne s'agit pas d'une certification officielle, mais de jalons clairs pour structurer votre apprentissage.

---

## 🎯 Vue d'Ensemble des Niveaux

La formation est structurée en **trois niveaux progressifs**, chacun correspondant à un ensemble de modules et de compétences :

| Niveau | Modules Couverts | Durée Estimée | Projet Type |
|--------|------------------|---------------|-------------|
| **Débutant** | Modules 1-4 | 4-8 semaines | Gestionnaire de contacts |
| **Intermédiaire** | Modules 1-6 | 12-16 semaines | Serveur TCP ou CLI system tool |
| **Avancé** | Modules 1-8 | 18-25 semaines | Contribution open-source |

---

## 🌱 Niveau Débutant (Modules 1-4)

### Objectif du Niveau

À ce niveau, vous devez **maîtriser les fondamentaux du langage C** et être capable d'écrire des programmes structurés qui gèrent la mémoire correctement.

### Modules Concernés

- **Module 1** : L'Écosystème C sous Linux
- **Module 2** : Fondamentaux du Langage
- **Module 3** : La Gestion de la Mémoire
- **Module 4** : Structures de Données et Modularité

---

### ✅ Critères de Validation

#### 1. Projet : Gestionnaire de Contacts

**Description** : Créez une application en ligne de commande qui permet de :
- Ajouter un contact (nom, téléphone, email)
- Afficher tous les contacts
- Rechercher un contact par nom
- Supprimer un contact
- Sauvegarder les contacts dans un fichier
- Charger les contacts depuis un fichier

**Concepts mis en œuvre** :
- Structures (`struct`) pour représenter un contact
- Allocation dynamique de mémoire (`malloc`, `free`)
- Manipulation de fichiers (lecture/écriture)
- Tableaux dynamiques ou listes chaînées
- Gestion des entrées utilisateur

**Pourquoi ce projet ?** Il vous oblige à utiliser tous les concepts fondamentaux : structures de données personnalisées, gestion de la mémoire, persistance des données, et organisation du code.

---

#### 2. Compilation Sans Warnings

**Critère** : Votre code doit compiler **sans aucun warning** avec les options strictes :

```bash
gcc -Wall -Werror -Wextra -std=c11 main.c -o gestionnaire
```

**Explication des options** :
- `-Wall` : Active tous les warnings courants
- `-Werror` : Transforme les warnings en erreurs (la compilation échoue)
- `-Wextra` : Active des warnings supplémentaires
- `-std=c11` : Utilise la norme C11

**Pourquoi c'est important ?** Les warnings indiquent souvent des bugs potentiels. Un code professionnel ne doit jamais ignorer les warnings du compilateur.

**Comment vérifier ?**
- Si la compilation réussit avec ces options, vous avez validé ce critère
- Si vous obtenez des erreurs, corrigez-les une par une

---

#### 3. Tests Unitaires Basiques

**Critère** : Écrivez au moins **3-5 fonctions de test** qui vérifient le bon fonctionnement de votre code.

**Exemple de tests** :
```c
// Test 1 : Création d'un contact
void test_creation_contact() {
    Contact* c = creer_contact("Alice", "0612345678", "alice@email.com");
    assert(c != NULL);
    assert(strcmp(c->nom, "Alice") == 0);
    liberer_contact(c);
    printf("✓ Test création contact : OK\n");
}

// Test 2 : Ajout dans la liste
void test_ajout_contact() {
    ListeContacts* liste = creer_liste();
    ajouter_contact(liste, "Bob", "0698765432", "bob@email.com");
    assert(liste->taille == 1);
    liberer_liste(liste);
    printf("✓ Test ajout contact : OK\n");
}
```

**Pourquoi c'est important ?** Les tests unitaires vous permettent de vérifier que chaque fonction fait bien ce qu'elle est censée faire, et de détecter les régressions lors des modifications.

**Outils recommandés** :
- Tests manuels avec `assert()` (suffisant pour le niveau débutant)
- Framework simple comme [Unity](http://www.throwtheswitch.org/unity) (optionnel)

---

### 📊 Grille d'Auto-Évaluation Niveau Débutant

Cochez les compétences que vous maîtrisez :

- [ ] Je comprends la chaîne de compilation (préprocesseur, compilateur, assembleur, linker)
- [ ] Je sais utiliser GCC avec les options de base
- [ ] Je maîtrise les types de données primitifs (int, char, float, double)
- [ ] Je sais déclarer et utiliser des fonctions
- [ ] Je comprends ce qu'est un pointeur et comment l'utiliser
- [ ] Je sais allouer et libérer de la mémoire dynamiquement (`malloc`/`free`)
- [ ] Je peux créer et manipuler des structures (`struct`)
- [ ] Je sais organiser mon code en plusieurs fichiers (.c et .h)
- [ ] Je comprends les fuites mémoire et comment les éviter
- [ ] Je peux lire et écrire dans des fichiers

**Score minimum pour valider** : 8/10 compétences cochées

---

## 🌿 Niveau Intermédiaire (Modules 1-6)

### Objectif du Niveau

À ce niveau, vous devez être capable de **créer des applications système** robustes, de les déboguer efficacement, et de les intégrer dans un processus de développement moderne (CI/CD).

### Modules Concernés

- **Module 5** : Outillage DevOps et Automatisation
- **Module 6** : Programmation Système Linux

---

### ✅ Critères de Validation

#### 1. Projet : Serveur TCP Concurrent OU CLI System Tool

**Option A : Serveur TCP Multi-Client**

Créez un serveur TCP qui :
- Écoute sur un port réseau
- Accepte plusieurs clients simultanément (avec threads ou processus)
- Répond aux requêtes des clients (echo server, chat, ou protocole simple)
- Gère proprement la déconnexion des clients

**Concepts mis en œuvre** :
- Sockets (création, bind, listen, accept)
- Threads POSIX (`pthread`)
- Synchronisation (mutex, variables de condition)
- Gestion des signaux (SIGINT pour arrêt propre)

**Option B : Outil CLI Système**

Créez un outil en ligne de commande qui :
- Interagit avec le système de fichiers ou les processus
- Parse des arguments (avec `getopt`)
- Produit un output formaté (JSON, tableau, etc.)
- Gère les erreurs de manière robuste

**Exemples** :
- Moniteur de processus (affichage CPU/RAM par processus)
- Analyseur de logs (statistiques, filtrage)
- Gestionnaire de tâches planifiées

---

#### 2. Pipeline CI/CD Fonctionnel

**Critère** : Configurez un pipeline d'intégration continue (GitHub Actions, GitLab CI, ou Jenkins) qui :

1. **Compile le projet** automatiquement à chaque commit
2. **Exécute les tests** unitaires
3. **Génère un rapport** de succès/échec

**Exemple de fichier GitHub Actions** (`.github/workflows/ci.yml`) :

```yaml
name: CI

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v3
      - name: Install dependencies
        run: sudo apt-get install -y gcc make
      - name: Compile
        run: gcc -Wall -Werror src/*.c -o app
      - name: Run tests
        run: ./run_tests.sh
```

**Pourquoi c'est important ?** Le CI/CD automatise la détection des bugs et garantit que le code compile toujours. C'est un standard en entreprise.

---

#### 3. Valgrind Clean (0 Leaks)

**Critère** : Exécutez votre programme avec Valgrind et obtenez **zéro fuite mémoire**.

**Commande** :
```bash
valgrind --leak-check=full --show-leak-kinds=all ./votre_programme
```

**Résultat attendu** :
```
==12345== HEAP SUMMARY:
==12345==     in use at exit: 0 bytes in 0 blocks
==12345==   total heap usage: 150 allocs, 150 frees, 8,024 bytes allocated
==12345==
==12345== All heap blocks were freed -- no leaks are possible
```

**Pourquoi c'est important ?** Les fuites mémoire dans des programmes long-running (serveurs, daemons) causent des crashs en production. Valgrind est l'outil de référence pour les détecter.

**Comment corriger les fuites ?**
- Chaque `malloc` doit avoir un `free` correspondant
- Utilisez Valgrind pour localiser précisément les fuites
- Vérifiez les chemins d'exécution avec erreurs (branches if/else)

---

#### 4. Documentation Doxygen

**Critère** : Documentez votre code avec des commentaires Doxygen et générez la documentation HTML.

**Exemple de documentation** :
```c
/**
 * @brief Crée un nouveau serveur TCP
 *
 * @param port Le port d'écoute (1024-65535)
 * @param max_clients Nombre maximum de clients simultanés
 * @return Pointeur vers la structure Serveur, ou NULL en cas d'erreur
 *
 * @note Le serveur doit être détruit avec detruire_serveur()
 */
Serveur* creer_serveur(int port, int max_clients);
```

**Génération de la documentation** :
```bash
doxygen Doxyfile
```

**Pourquoi c'est important ?** La documentation facilite la maintenance et la collaboration. Doxygen génère automatiquement une doc navigable à partir des commentaires.

---

### 📊 Grille d'Auto-Évaluation Niveau Intermédiaire

- [ ] Je sais utiliser CMake pour compiler un projet
- [ ] Je maîtrise GDB pour déboguer (breakpoints, backtrace, inspection)
- [ ] Je sais détecter les fuites mémoire avec Valgrind
- [ ] Je comprends les descripteurs de fichiers et les appels système
- [ ] Je peux créer et gérer des processus (fork, exec, wait)
- [ ] Je sais gérer les signaux (SIGINT, SIGTERM, etc.)
- [ ] Je maîtrise les threads POSIX et la synchronisation
- [ ] Je peux créer un serveur TCP avec sockets
- [ ] J'ai configuré un pipeline CI/CD fonctionnel
- [ ] Je documente mon code de manière professionnelle

**Score minimum pour valider** : 8/10 compétences cochées

---

## 🌳 Niveau Avancé (Modules 1-8)

### Objectif du Niveau

À ce niveau, vous êtes capable de **contribuer à des projets professionnels**, d'optimiser du code pour la performance, et de maîtriser les techniques avancées de C et de l'écosystème Linux moderne.

### Modules Concernés

- **Module 7** : Techniques Avancées et Optimisation
- **Module 8** : C dans un Pipeline CI/CD

---

### ✅ Critères de Validation

#### 1. Contribution Open-Source Acceptée

**Critère** : Soumettez une Pull Request à un projet open-source en C qui soit **acceptée et mergée**.

**Comment procéder ?**

1. **Choisir un projet** :
   - Projets débutant-friendly : [First Timers Only](https://www.firsttimersonly.com/)
   - Projets C populaires : Git, Redis, Nginx, systemd, curl
   - Filtrez les issues marquées "good first issue" ou "help wanted"

2. **Types de contributions acceptables** :
   - Correction de bug simple
   - Amélioration de la documentation
   - Ajout de tests unitaires
   - Refactoring mineur
   - Nouvelle fonctionnalité simple

3. **Processus** :
   - Fork le projet
   - Créez une branche pour votre modification
   - Respectez le style de code du projet (utilisez leur `.clang-format`)
   - Écrivez des tests si nécessaire
   - Soumettez la PR avec une description claire

**Pourquoi c'est important ?** Contribuer à l'open-source démontre votre capacité à travailler en équipe, à lire du code existant, et à respecter des standards professionnels.

---

#### 2. Code Review d'un Pair

**Critère** : Effectuez une revue de code approfondie du projet d'un autre apprenant et fournissez des commentaires constructifs.

**Éléments à vérifier** :
- **Correction** : Le code fait-il ce qu'il est censé faire ?
- **Lisibilité** : Le code est-il clair et bien organisé ?
- **Mémoire** : Y a-t-il des fuites ou des accès invalides ?
- **Sécurité** : Y a-t-il des vulnérabilités (buffer overflow, format string, etc.) ?
- **Performance** : Y a-t-il des optimisations évidentes ?
- **Tests** : Le code est-il suffisamment testé ?

**Format de la revue** :
- Commentaires ligne par ligne sur GitHub/GitLab
- Document récapitulatif avec points positifs et axes d'amélioration
- Suggestions concrètes de modifications

**Pourquoi c'est important ?** La code review est une compétence essentielle en entreprise. Savoir critiquer constructivement et accepter les critiques est crucial.

---

#### 3. Analyse de Performance avec Profiling

**Critère** : Utilisez des outils de profiling pour analyser et **optimiser les performances** d'un programme.

**Outils à utiliser** :

1. **gprof** (profiling CPU)
   ```bash
   gcc -pg -O2 programme.c -o programme
   ./programme
   gprof programme gmon.out > analyse.txt
   ```

2. **perf** (profiling système)
   ```bash
   perf record ./programme
   perf report
   ```

3. **Valgrind Callgrind** (analyse des appels de fonctions)
   ```bash
   valgrind --tool=callgrind ./programme
   kcachegrind callgrind.out.*
   ```

**Livrables attendus** :
- Identification des fonctions gourmandes en CPU
- Mesure du temps d'exécution avant/après optimisation
- Rapport expliquant les optimisations effectuées (algorithme, cache, etc.)

**Pourquoi c'est important ?** L'optimisation basée sur des mesures réelles (et non des intuitions) est une compétence clé pour le code haute performance.

---

#### 4. Implémentation eBPF OU Projet Système Complexe

**Option A : Programme eBPF**

Créez un programme eBPF qui :
- Trace des appels système (ex : tous les `open()`)
- Collecte des statistiques (ex : latence réseau)
- Utilise des BPF maps pour partager des données
- S'interface avec l'espace utilisateur

**Option B : Projet Système Avancé**

Exemples :
- Shell interactif (parsing, pipes, redirections)
- Allocateur mémoire personnalisé
- Système de fichiers FUSE simple
- Débogueur minimaliste (utilisant ptrace)

**Pourquoi c'est important ?** Ces projets démontrent votre maîtrise des technologies Linux avancées et votre capacité à résoudre des problèmes complexes.

---

### 📊 Grille d'Auto-Évaluation Niveau Avancé

- [ ] Je maîtrise les pointeurs de fonctions et les callbacks
- [ ] Je comprends le préprocesseur en profondeur (macros, conditional compilation)
- [ ] Je sais écrire du code sécurisé (validation entrées, prévention buffer overflow)
- [ ] Je peux optimiser du code avec flags `-O2`, `-O3` et comprendre l'impact
- [ ] Je sais utiliser des outils de profiling (gprof, perf, Valgrind Callgrind)
- [ ] Je maîtrise la compilation croisée (cross-compilation)
- [ ] J'ai contribué à un projet open-source en C
- [ ] Je peux effectuer des code reviews constructives
- [ ] Je comprends eBPF et ses applications
- [ ] Je sais packager et déployer des applications C (Docker, .deb, .rpm)

**Score minimum pour valider** : 8/10 compétences cochées

---

## 🏆 Certification et Reconnaissance

### Certification Informelle

Bien que cette formation ne délivre pas de certification officielle, vous pouvez **valider vos compétences** de plusieurs manières :

1. **Portfolio GitHub**
   - Créez un repository `c-portfolio` avec vos meilleurs projets
   - Incluez des README détaillés pour chaque projet
   - Ajoutez des badges de CI/CD (build passing, coverage)

2. **Blog Technique**
   - Écrivez des articles sur vos apprentissages
   - Partagez vos solutions à des problèmes complexes
   - Démontrez votre compréhension en expliquant aux autres

3. **Contributions Open-Source**
   - Vos contributions sont visibles et vérifiables
   - Elles démontrent votre niveau réel

### Certifications Officielles (Optionnelles)

Si vous souhaitez une certification reconnue, considérez :

- **LPI (Linux Professional Institute)** : Certifications Linux (incluent du C)
- **Red Hat Certified Specialist** : Pour les compétences système
- **Contributions reconnues** : Devenir contributeur régulier sur des projets majeurs (Git, Linux kernel, etc.)

---

## 📈 Tableau Récapitulatif des Critères

| Niveau | Projet Principal | Critères Techniques | Critères Qualité | Critères Professionnels |
|--------|------------------|---------------------|------------------|-------------------------|
| **Débutant** | Gestionnaire de contacts | Compilation `-Wall -Werror` | Tests unitaires basiques | - |
| **Intermédiaire** | Serveur TCP ou CLI tool | Valgrind clean (0 leaks) | Pipeline CI/CD | Documentation Doxygen |
| **Avancé** | Contribution open-source | Profiling + optimisation | Code review | eBPF ou projet complexe |

---

## 🎯 Comment Valider Votre Niveau ?

### Validation Auto-Dirigée

1. **Réalisez tous les projets** du niveau ciblé
2. **Vérifiez les critères techniques** (compilation, Valgrind, etc.)
3. **Complétez la grille d'auto-évaluation** (minimum 8/10)
4. **Publiez votre travail** sur GitHub pour feedback communautaire

### Validation par les Pairs

1. Rejoignez des communautés C (r/C_Programming, Discord Linux)
2. Partagez vos projets et demandez des reviews
3. Participez à des code reviews d'autres développeurs
4. Échangez avec des développeurs expérimentés

### Validation Professionnelle

1. Utilisez vos projets en entretien d'embauche
2. Contribuez à des projets open-source reconnus
3. Participez à des hackathons ou compétitions (ex : Advent of Code en C)

---

## 💡 Conseils pour Réussir les Validations

### 1. Ne Brulez Pas les Étapes

- ❌ Ne passez pas au niveau intermédiaire si vous avez moins de 7/10 au niveau débutant
- ✅ Consolidez vos bases avant de progresser

### 2. Privilégiez la Qualité à la Quantité

- Un projet bien fait vaut mieux que trois projets bâclés
- Prenez le temps de faire du code propre et testé

### 3. Documentez Tout

- README clairs avec captures d'écran
- Commentaires de code utiles (pas évidents)
- Diagrammes pour architectures complexes

### 4. Demandez du Feedback

- Plus tôt vous obtenez des retours, mieux c'est
- N'ayez pas peur de montrer du code imparfait

### 5. Restez Humble et Curieux

- Il y a toujours à apprendre en C
- Les meilleurs développeurs C ont 20+ ans d'expérience
- Chaque bug est une opportunité d'apprentissage

---

## 📚 Ressources pour Aller Plus Loin

### Après la Validation Niveau Avancé

1. **Contribuer au Linux Kernel**
   - [Kernel Newbies](https://kernelnewbies.org/)
   - Mailing list linux-kernel

2. **Maîtriser eBPF**
   - [eBPF.io](https://ebpf.io/)
   - Livre "Learning eBPF" (O'Reilly)

3. **Expertise Sécurité**
   - [CERT C Coding Standard](https://wiki.sei.cmu.edu/confluence/display/c/)
   - Participer à des CTF (Capture The Flag)

4. **Performance Extrême**
   - [Agner Fog's Optimization Manuals](https://www.agner.org/optimize/)
   - Étudier les optimisations de compilateurs

---

## 🎓 Conclusion

Ces critères d'évaluation vous fournissent une **roadmap claire** pour progresser en C, du débutant à l'expert. L'essentiel n'est pas d'obtenir une certification officielle, mais de **démontrer vos compétences par des projets concrets** et des contributions vérifiables.

**Rappelez-vous** :
- ✅ La pratique régulière est plus importante que la théorie
- ✅ Les erreurs sont vos meilleures enseignantes
- ✅ La communauté C est là pour vous aider
- ✅ Chaque projet terminé est une victoire

**Bonne chance dans votre parcours de maîtrise du langage C ! 🚀**

⏭️ [Conseils pour réussir](/ressources-complementaires/03-conseils-reussite.md)
