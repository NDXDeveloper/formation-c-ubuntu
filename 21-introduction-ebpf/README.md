🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 21. Introduction à eBPF ⭐🔥

## Vue d'ensemble

Bienvenue dans l'un des chapitres les plus passionnants de cette formation : **eBPF** (extended Berkeley Packet Filter). Si vous vous demandez pourquoi ce chapitre mérite deux étoiles d'importance, c'est parce qu'eBPF représente une révolution dans l'écosystème Linux et change fondamentalement la façon dont nous observons, sécurisons et optimisons nos systèmes.

## Pourquoi eBPF est révolutionnaire ?

eBPF permet d'exécuter du code **en toute sécurité** directement dans le noyau Linux sans avoir à modifier le code du noyau ou charger des chapitres kernel. C'est comparable à l'ajout d'un "superpouvoir" à Linux qui permet de :

- **Observer** le système avec une granularité sans précédent
- **Sécuriser** les applications et le réseau de manière native
- **Optimiser** les performances en agissant au cœur du système
- **Déboguer** et tracer n'importe quelle partie du système

Imaginez pouvoir intercepter n'importe quel appel système, analyser chaque paquet réseau, tracer chaque allocation mémoire, ou surveiller chaque opération de fichier... **sans impacter les performances de façon significative et sans risquer de crasher le système**. C'est exactement ce qu'eBPF offre.

## eBPF : Au cœur de l'infrastructure moderne

eBPF n'est pas qu'une curiosité technique. C'est devenu la fondation de solutions de production critiques :

- **Kubernetes networking** : Cilium utilise eBPF pour remplacer iptables et offrir des performances réseau supérieures
- **Observabilité** : Pixie, Datadog, New Relic utilisent eBPF pour la surveillance en temps réel
- **Sécurité** : Falco, Tracee utilisent eBPF pour la détection d'intrusions et l'audit de sécurité
- **Performance monitoring** : Les outils de profiling modernes (comme bpftrace) reposent sur eBPF

En 2025, **maîtriser eBPF est devenu un atout majeur** pour tout ingénieur DevOps, SRE, ou développeur système Linux.

## Pré-requis pour ce chapitre

Avant de plonger dans eBPF, assurez-vous d'avoir une solide compréhension de :

✅ **Programmation système Linux** (Chapitres 16-20)
- Appels système et kernel space vs user space
- Gestion des processus et threads
- Concepts réseau (sockets, TCP/IP)

✅ **Programmation C avancée** (Chapitres 1-15)
- Pointeurs et gestion mémoire
- Structures de données
- Compilation et linking

✅ **Ligne de commande Linux**
- Permissions et sécurité
- Compilation de programmes
- Outils de debugging (GDB)

Si vous n'êtes pas à l'aise avec ces concepts, nous vous recommandons de revoir les chapitres précédents avant de continuer.

## Ce que vous allez apprendre

Ce chapitre est structuré pour vous faire passer de zéro connaissance en eBPF à la capacité d'écrire et déployer vos propres programmes eBPF :

### 🎯 **Section 21.1 : Qu'est-ce qu'eBPF ?**
- L'histoire : de BPF classique (filtres de paquets) à eBPF moderne
- Pourquoi eBPF change la donne pour Linux
- Les use cases concrets en production

### 🏗️ **Section 21.2 : Architecture eBPF**
- Comment fonctionnent les programmes eBPF
- Le système de maps pour communiquer entre kernel et user space
- Le verifier : garantir la sécurité du code
- La compilation JIT pour des performances optimales

### 💼 **Section 21.3 : Use Cases DevOps et Observabilité**
- Tracing et monitoring d'applications
- Optimisation réseau avec eBPF
- Sécurité et audit système
- Analyse de performance en production

### 🛠️ **Section 21.4 : Toolchain eBPF**
- libbpf : la bibliothèque de référence
- bpftool : l'outil en ligne de commande
- Workflow de compilation et chargement de programmes eBPF

### 👨‍💻 **Section 21.5 : Premier Programme eBPF**
- Hello World : tracer les appels système
- Utiliser les maps pour agréger des données
- Attacher des programmes à différents hooks kernel

### 🌐 **Section 21.6 : eBPF dans l'Écosystème Moderne**
- Cilium : networking Kubernetes de nouvelle génération
- Pixie : observabilité sans instrumentation
- Ressources pour continuer votre apprentissage

## Approche pédagogique

Ce chapitre adopte une approche **progressive et pratique** :

1. **Concepts d'abord** : comprendre *pourquoi* et *comment* avant de coder
2. **Exemples concrets** : chaque concept est illustré par du code réel
3. **Hands-on** : vous écrirez vos propres programmes eBPF
4. **Production-ready** : focus sur les patterns utilisés en entreprise

## Configuration requise

Pour suivre ce chapitre, vous aurez besoin de :

- **Kernel Linux ≥ 5.4** (certaines fonctionnalités nécessitent des versions plus récentes)
- **Ubuntu 22.04 ou 24.04** recommandé
- Droits **root/sudo** (eBPF nécessite des privilèges élevés)
- Au moins **4 GB de RAM** pour compiler et tester
- **Environnement DevContainer** (optionnel mais recommandé, voir Section 2.5)

Vérifiez la version de votre kernel :
```bash
uname -r
```

Si votre kernel est trop ancien, vous devrez le mettre à jour avant de continuer.

## Pourquoi ce chapitre est important pour votre carrière ?

En 2025, eBPF est devenu incontournable :

🚀 **Opportunités professionnelles**
- Les entreprises cloud-native recherchent activement des compétences eBPF
- Cilium (basé sur eBPF) est le CNI Kubernetes le plus performant
- Les outils d'observabilité modernes reposent massivement sur eBPF

💡 **Compréhension approfondie du système**
- eBPF vous force à comprendre le kernel Linux en profondeur
- Vous apprendrez comment fonctionnent vraiment les appels système
- Vous développerez une intuition sur les performances système

🔧 **Résolution de problèmes avancée**
- Debugging de problèmes réseau complexes
- Analyse de performance en production sans arrêter les services
- Détection d'anomalies de sécurité en temps réel

## Ressources complémentaires

Avant de commencer, voici quelques ressources utiles (nous y reviendrons en détail) :

- **Documentation officielle** : [ebpf.io](https://ebpf.io)
- **Kernel docs** : Documentation/bpf/ dans le code source du kernel
- **Livre de référence** : "Learning eBPF" par Liz Rice (O'Reilly, 2023)
- **Blog Cilium** : Articles techniques de haute qualité sur eBPF

## Structure des fichiers du chapitre

```
21-introduction-ebpf/
├── README.md                          # Ce fichier
├── 01-quest-ce-que-ebpf.md
│   ├── 01.1-histoire-bpf.md
│   ├── 01.2-pourquoi-ebpf-revolutionne.md
├── 02-architecture-ebpf.md
│   ├── 02.1-bpf-programs.md
│   ├── 02.2-bpf-maps.md
│   ├── 02.3-verifier.md
│   ├── 02.4-jit-compilation.md
├── 03-use-cases-devops.md
│   ├── 03.1-tracing-monitoring.md
│   ├── 03.2-networking.md
│   ├── 03.3-securite.md
│   ├── 03.4-performance-analysis.md
├── 04-toolchain-ebpf.md
│   ├── 04.1-libbpf.md
│   ├── 04.2-bpftool.md
│   ├── 04.3-compilation-chargement.md
├── 05-premier-programme.md
│   ├── 05.1-hello-world-tracing.md
│   ├── 05.2-utilisation-maps.md
│   ├── 05.3-hooks-kernel.md
└── 06-ecosysteme-moderne.md
    ├── 06.1-cilium.md
    ├── 06.2-pixie.md
    └── 06.3-ressources.md
```

## Comment aborder ce chapitre ?

### 🎓 Si vous êtes débutant en eBPF
Suivez l'ordre des sections. Ne sautez pas d'étapes, chaque concept s'appuie sur le précédent.

### 🔧 Si vous avez déjà touché à eBPF
Vous pouvez survoler les sections 21.1-21.2 et vous concentrer sur les sections 21.4-21.6 pour la pratique.

### 🚀 Si vous êtes pressé
Focus minimum : 21.1, 21.2, 21.5 (comprendre les bases et écrire votre premier programme).

## Temps estimé

- **Lecture complète** : 6-8 heures
- **Avec exercices pratiques** : 12-16 heures
- **Maîtrise approfondie** : 40+ heures (inclut expérimentation et projets)

---

## Prêt à démarrer ?

eBPF peut sembler intimidant au premier abord. C'est normal : nous parlons de code qui s'exécute dans le kernel Linux, avec toutes les contraintes que cela implique. Mais ne vous inquiétez pas, nous allons démystifier tout cela ensemble, pas à pas.

**À la fin de ce chapitre, vous serez capable de :**
- ✅ Expliquer ce qu'est eBPF et pourquoi c'est important
- ✅ Comprendre l'architecture et les mécanismes de sécurité
- ✅ Écrire, compiler et charger vos propres programmes eBPF
- ✅ Utiliser eBPF pour le tracing, le monitoring et l'optimisation
- ✅ Comprendre comment Cilium, Pixie et autres outils utilisent eBPF

**Alors, êtes-vous prêt à donner des superpouvoirs à votre kernel Linux ? 🚀**

Passons à la première section : **21.1 - Qu'est-ce qu'eBPF ?**

---

⏭️ [Qu'est-ce qu'eBPF ?](/21-introduction-ebpf/01-quest-ce-que-ebpf.md)
