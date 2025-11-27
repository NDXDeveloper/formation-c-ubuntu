🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 21.6 eBPF dans l'écosystème moderne

## Introduction

Dans les sections précédentes, vous avez découvert les fondamentaux d'eBPF : son architecture, le verifier, les BPF maps, et vous avez même écrit vos premiers programmes. Vous avez appris comment tracer des appels système, collecter des métriques, et attacher des programmes à différents hooks dans le kernel Linux.

Maintenant, il est temps de voir **la vraie magie** : comment eBPF transforme l'infrastructure moderne en production, à grande échelle, dans les clouds publics et les data centers du monde entier.

Cette section est votre fenêtre sur **l'écosystème eBPF en 2025**. Nous allons découvrir comment les plus grandes entreprises technologiques du monde (Google, Meta, Netflix, Microsoft) utilisent eBPF pour résoudre des problèmes que les technologies traditionnelles ne pouvaient pas adresser.

## Le contexte : Une révolution silencieuse

### Avant eBPF : Les limites de l'infrastructure traditionnelle

Pour comprendre pourquoi eBPF a pris une telle importance, il faut comprendre les défis de l'infrastructure cloud-native moderne :

**Le problème de la scalabilité :**
```
Cluster Kubernetes typique en 2025 :
├─ 1000+ nœuds
├─ 10 000+ pods
├─ 100 000+ connexions réseau/seconde
├─ 1 000 000+ requêtes HTTP/seconde
└─ Latence requise : < 10ms
```

Les technologies traditionnelles (iptables, netfilter, instrumentation manuelle) ne scalent pas :
- ❌ **iptables** : O(n) sur le nombre de règles → latence inacceptable à grande échelle
- ❌ **Instrumentation manuelle** : Nécessite de modifier chaque service
- ❌ **Agents/sidecars** : Overhead de 100+ MB de RAM par pod
- ❌ **Logging/monitoring traditionnel** : Coûts de storage prohibitifs
- ❌ **Sécurité réactive** : Détection après l'incident, pas de prévention en temps réel

### Avec eBPF : Un nouveau paradigme

eBPF change fondamentalement la donne :

```
┌─────────────────────────────────────────────────────────┐
│         L'infrastructure traditionnelle                 │
│                                                         │
│  Application → Agent → Collector → Storage → Query      │
│                                                         │
│  • Modifications de code nécessaires                    │
│  • Overhead significatif                                │
│  • Latence élevée                                       │
│  • Coûts importants                                     │
└─────────────────────────────────────────────────────────┘

                         ↓↓↓ eBPF ↓↓↓

┌─────────────────────────────────────────────────────────┐
│         L'infrastructure avec eBPF                      │
│                                                         │
│  Application ← [eBPF dans le kernel] → Observabilité    │
│               ↑                        ↓                │
│               └────── Sécurité ────────┘                │
│                       Networking                        │
│                                                         │
│  • Zéro modification de code                            │
│  • Overhead minimal                                     │
│  • Temps réel                                           │
│  • Efficacité maximale                                  │
└─────────────────────────────────────────────────────────┘
```

**Ce qui change :**
- ✅ **Performance** : O(1) au lieu de O(n), latence en microsecondes
- ✅ **Efficacité** : Pas de sidecars, pas d'agents lourds
- ✅ **Visibilité** : Tout est observable sans instrumentation
- ✅ **Sécurité** : Enforcement en temps réel dans le kernel
- ✅ **Économies** : Réduction drastique des coûts d'infrastructure

## L'écosystème eBPF en 2025

eBPF n'est plus une technologie expérimentale : c'est devenu un **standard de facto** dans l'infrastructure moderne.

### Les piliers de l'écosystème

```
┌────────────────────────────────────────────────────────┐
│                 ÉCOSYSTÈME eBPF 2025                   │
│                                                        │
│  ┌──────────────┐  ┌──────────────┐  ┌─────────────┐   │
│  │  NETWORKING  │  │OBSERVABILITÉ │  │  SÉCURITÉ   │   │
│  │              │  │              │  │             │   │
│  │  • Cilium    │  │  • Pixie     │  │  • Falco    │   │
│  │  • Katran    │  │  • Parca     │  │  • Tetragon │   │
│  │  • XDP       │  │  • BCC/      │  │  • Tracee   │   │
│  │  • eBPF      │  │    bpftrace  │  │  • KubeArmor│   │
│  │    TC        │  │  • Hubble    │  │             │   │
│  └──────────────┘  └──────────────┘  └─────────────┘   │
│                                                        │
│  ┌────────────────────────────────────────────────┐    │
│  │           CLOUD PROVIDERS                      │    │
│  │                                                │    │
│  │  GKE (Google) · EKS (AWS) · AKS (Azure)        │    │
│  │  DOKS (DigitalOcean) · Linode · OVH            │    │
│  │                                                │    │
│  │  Tous adoptent eBPF massivement !              │    │
│  └────────────────────────────────────────────────┘    │
└────────────────────────────────────────────────────────┘
```

### Adoption massive

Les chiffres parlent d'eux-mêmes :

**Cloud Providers :**
- 🟢 **Google Cloud (GKE)** : Cilium recommandé comme dataplane par défaut
- 🟠 **Amazon (EKS)** : Support natif de Cilium, déploiement en un clic
- 🔵 **Microsoft (AKS)** : Azure CNI powered by Cilium disponible
- 🔴 **DigitalOcean** : DOKS utilise Cilium par défaut depuis 2021

**Entreprises :**
- **Meta (Facebook)** : Katran (load balancer L4) en production depuis 2018
- **Google** : Utilise eBPF pour le networking dans leurs data centers
- **Netflix** : Profiling continu avec eBPF sur des millions de conteneurs
- **Datadog** : Agent basé sur eBPF pour l'observabilité
- **Cloudflare** : Protection DDoS avec XDP
- **Adobe, Capital One, GitLab, Sky, Bell Canada** : Production Cilium
- Et des milliers d'autres...

**Projets CNCF :**
- ✅ **Cilium** : Graduated (plus haut niveau)
- ✅ **Falco** : Graduated
- ✅ **Pixie** : Sandbox
- ✅ **Tetragon** : Sandbox

## Pourquoi maintenant ? L'eBPF a trouvé son moment

### Le timing parfait : Convergence de plusieurs facteurs

**1. Maturité du kernel Linux**
- eBPF introduit en 2014 (kernel 3.18)
- Stabilisation majeure en 2018-2019 (kernel 4.15+)
- Fonctionnalités avancées 2020-2024 (kernel 5.x+)
- En 2025 : Stable, performant, largement disponible

**2. Explosion de Kubernetes**
```
Adoption Kubernetes :
2015 :     5% des entreprises
2020 :    50% des entreprises
2025 :    85% des entreprises (prévision)
```
Kubernetes + eBPF = Match parfait

**3. Limites des approches traditionnelles**
- iptables ne scale pas pour des milliers de services
- Service mesh avec sidecars = overhead inacceptable
- Instrumentation manuelle = trop lent, trop coûteux

**4. Besoin de sécurité runtime**
- Attaques zero-day
- Supply chain attacks
- Runtime protection nécessaire

**5. Coûts cloud**
- Optimisation CPU = économies massives
- Réduction de l'overhead = moins d'instances
- Observabilité sans storage = coûts réduits

## Les trois piliers d'eBPF en production

### 1. Networking : Performance et flexibilité

**Problème résolu :**
Kubernetes a besoin de networking haute performance avec des milliers de pods et de services.

**Solution eBPF :**
- Load balancing en eBPF (O(1) vs O(n))
- Bypass de netfilter quand possible
- XDP pour traitement réseau ultra-rapide
- Network policies Layer 7

**Projet phare : Cilium** (section 21.6.1)

**Résultats concrets :**
- 🚀 10x moins d'utilisation CPU que kube-proxy
- 🚀 Latence réduite de 50%
- 🚀 Support de 10 000+ services sans dégradation

### 2. Observabilité : Visibilité totale sans instrumentation

**Problème résolu :**
Instrumenter manuellement des centaines de microservices est impraticable et coûteux.

**Solution eBPF :**
- Auto-instrumentation via uprobe/kprobe
- Capture de tout le trafic (HTTP, SQL, Redis, gRPC)
- Parsing de protocoles en temps réel
- Tracing distribué sans bibliothèques

**Projet phare : Pixie** (section 21.6.2)

**Résultats concrets :**
- 📊 Zéro modification de code nécessaire
- 📊 Visibilité instantanée sur tous les services
- 📊 Debugging en production en quelques minutes
- 📊 Overhead < 5% au lieu de 30-50%

### 3. Sécurité : Protection temps réel

**Problème résolu :**
La sécurité traditionnelle est réactive (logs, SIEM) et ne peut pas bloquer en temps réel.

**Solution eBPF :**
- Enforcement au niveau kernel
- Détection et blocage en temps réel
- Visibilité sur tous les syscalls, network, file operations
- Zero-trust par défaut

**Projets phares : Falco, Tetragon, Tracee**

**Résultats concrets :**
- 🔒 Détection d'intrusion en temps réel
- 🔒 Blocage de comportements malveillants instantané
- 🔒 Audit complet sans overhead
- 🔒 Compliance continue

## Vue d'ensemble des sous-sections

Cette section 21.6 est divisée en trois parties complémentaires :

### 21.6.1 - Cilium : Networking, sécurité et observabilité avec eBPF

Dans cette première sous-section, nous explorerons **Cilium**, la solution de networking la plus avancée pour Kubernetes.

**Ce que vous découvrirez :**
- Comment Cilium remplace iptables par eBPF
- Network policies Layer 7 (HTTP, gRPC, Kafka)
- Service mesh sans sidecar
- Hubble : observabilité réseau native
- Architecture et utilisation en production
- Pourquoi Google, AWS, et Microsoft l'ont adopté

**Pourquoi c'est important :**
Cilium est l'exemple parfait de comment eBPF peut révolutionner une partie entière de l'infrastructure. C'est devenu le standard de facto pour le networking Kubernetes haute performance.

**Public cible :** DevOps, SRE, Platform Engineers, Architects

### 21.6.2 - Pixie : Observabilité automatique pour Kubernetes

Dans cette deuxième sous-section, nous découvrirons **Pixie**, qui apporte l'observabilité instantanée sans instrumentation.

**Ce que vous découvrirez :**
- Auto-instrumentation avec eBPF (uprobes, kprobes)
- Capture automatique de HTTP, SQL, Redis, gRPC
- PxL : langage de query pour interroger les données
- Live debugging en production
- Architecture et cas d'usage

**Pourquoi c'est important :**
Pixie démontre comment eBPF peut éliminer complètement le besoin d'instrumentation manuelle. C'est un game-changer pour les équipes qui gèrent des dizaines ou centaines de microservices.

**Public cible :** Développeurs, DevOps, SRE

### 21.6.3 - Ressources pour aller plus loin

Dans cette dernière sous-section, nous vous fournirons une **feuille de route complète** pour continuer votre apprentissage.

**Ce que vous y trouverez :**
- Documentation officielle et livres de référence
- Tutoriels et guides pratiques
- Communautés et forums (Slack, GitHub, mailing lists)
- Conférences et événements (eBPF Summit, KubeCon)
- Cours en ligne et certifications
- Comment contribuer à l'écosystème
- Outils et frameworks complémentaires
- Checklist de progression

**Pourquoi c'est important :**
L'écosystème eBPF évolue rapidement. Cette section vous donne tous les outils pour rester à jour et continuer à progresser.

**Public cible :** Tous les niveaux

## L'impact d'eBPF : Quelques exemples concrets

### Cas #1 : Réduction des coûts cloud de 30%

**Entreprise :** Scale-up SaaS avec 500 nœuds Kubernetes

**Avant eBPF (avec kube-proxy + Calico) :**
- Utilisation CPU : 2 cores/nœud pour le networking
- Coût mensuel : 50 000 $

**Après eBPF (avec Cilium) :**
- Utilisation CPU : 0.3 cores/nœud pour le networking
- Coût mensuel : 35 000 $

**Économie annuelle : 180 000 $**

### Cas #2 : Debugging 10x plus rapide

**Entreprise :** E-commerce avec 200 microservices

**Avant (instrumentation manuelle) :**
- Incident : "Latence élevée sur le checkout"
- Temps pour identifier la cause : 2-3 heures
- Nécessite : Ajout de logs, redéploiement, attente

**Après (avec Pixie + eBPF) :**
- Même incident
- Temps pour identifier : 10 minutes
- Action : Query PxL pour voir requêtes SQL lentes
- Cause identifiée : Requête sans index sur une table de 10M lignes

**Temps sauvé : 90% du temps de debugging**

### Cas #3 : Sécurité renforcée

**Entreprise :** Institution financière

**Avant eBPF :**
- Sécurité basée sur logs et SIEM
- Détection d'intrusion : 30 minutes à 2 heures après l'incident
- Réponse : Manuelle, lente

**Après eBPF (avec Falco + Tetragon) :**
- Détection en temps réel (< 1 seconde)
- Blocage automatique des comportements suspects
- Exemple : Pod qui essaie d'exécuter `/bin/bash` → bloqué instantanément

**Résultat : Zéro incident de sécurité réussi depuis le déploiement**

### Cas #4 : Migration transparente

**Entreprise :** Grande plateforme avec 100+ équipes de développement

**Challenge :**
- Migration de Calico vers Cilium
- Ne pas perturber 100+ équipes
- Zero downtime requis

**Résultat :**
- Migration en rolling update (nœud par nœud)
- Aucune modification de code nécessaire
- Aucun downtime
- Les développeurs n'ont même pas remarqué !

**Temps de migration : 1 semaine pour 1000 nœuds**

## eBPF et l'avenir de l'infrastructure

### Tendances émergentes

**1. eBPF devient la couche d'infrastructure universelle**
```
                    Applications
                        │
                        ↓
        ┌───────────────────────────────┐
        │       Couche eBPF             │
        │                               │
        │  • Networking                 │
        │  • Observabilité              │
        │  • Sécurité                   │
        │  • Storage (futur)            │
        │  • Scheduling (expérimental)  │
        └───────────────┬───────────────┘
                        │
                        ↓
                  Kernel Linux
```

**2. eBPF sur d'autres OS**
- **Windows** : Microsoft développe eBPF for Windows
- **BSD** : Projets en cours
- **macOS** : Discussions préliminaires

**3. Hardware offloading**
- Cartes réseau SmartNIC avec support eBPF
- Traitement encore plus rapide (bypass CPU)

**4. Standardisation**
- eBPF Foundation (sous Linux Foundation)
- Standardisation de l'ISA (Instruction Set Architecture)
- Portabilité entre systèmes

**5. IA/ML et eBPF**
- Détection d'anomalies avec ML sur données eBPF
- Optimisation automatique de policies
- Prédiction de pannes

## Qui devrait s'intéresser à eBPF ?

### Profils concernés

**DevOps Engineers**
- Gérer l'infrastructure Kubernetes
- Optimiser les performances
- Réduire les coûts cloud
- **Intérêt principal :** Cilium pour networking, Pixie pour observabilité

**SRE (Site Reliability Engineers)**
- Maintenir la disponibilité
- Déboguer les incidents rapidement
- Monitoring et alerting
- **Intérêt principal :** Observabilité temps réel, profiling continu

**Platform Engineers**
- Construire des plateformes internes
- Offrir des capabilities aux équipes de dev
- Standardiser l'infrastructure
- **Intérêt principal :** Solutions eBPF clé-en-main pour toute l'entreprise

**Security Engineers**
- Protéger les applications en production
- Détection d'intrusions
- Compliance et audit
- **Intérêt principal :** Falco, Tetragon, runtime security

**Performance Engineers**
- Optimiser les applications
- Réduire la latence
- Profiling CPU/Memory
- **Intérêt principal :** BPF performance tools, flame graphs

**Développeurs d'infrastructure**
- Créer des outils internes
- Contribuer à l'écosystème
- Innovation
- **Intérêt principal :** Écrire des programmes eBPF custom

**Architects**
- Décisions stratégiques d'infrastructure
- Choix de technologies
- Vision long terme
- **Intérêt principal :** Comprendre l'impact d'eBPF sur l'architecture

## Prérequis pour cette section

### Connaissances requises

Avant d'aborder les sous-sections de cette section 21.6, assurez-vous d'avoir :

**✅ Complété les sections précédentes :**
- 21.1 à 21.4 : Architecture, verifier, maps, toolchain
- 21.5 : Premiers programmes eBPF (Hello World, maps, hooks)

**✅ Connaissances Kubernetes (niveau basique) :**
- Concepts : Pods, Services, Deployments
- kubectl : Commandes de base
- Networking K8s : Comment les pods communiquent
- *Note :* Si vous n'êtes pas familier avec K8s, ce n'est pas bloquant, mais certains exemples seront plus clairs si vous connaissez les bases

**✅ Connaissances réseau (niveau basique) :**
- Modèle OSI / TCP/IP
- HTTP, TCP, UDP
- Load balancing (concept)

**✅ Connaissances Linux :**
- Ligne de commande
- Processus, syscalls (vus dans les sections précédentes)

**Niveau recommandé :** Intermédiaire en infrastructure cloud-native

**Si vous n'avez pas toutes ces connaissances :** Ce n'est pas grave ! Les sections 21.6.1 et 21.6.2 sont écrites pour être accessibles. Certains détails avancés pourront être survolés et revisités plus tard.

## Objectifs de cette section

À la fin de cette section 21.6, vous serez capable de :

✅ **Comprendre l'écosystème eBPF en production**
- Connaître les principaux projets (Cilium, Pixie, Falco, etc.)
- Comprendre leurs cas d'usage respectifs
- Savoir lequel choisir selon vos besoins

✅ **Expliquer comment eBPF transforme l'infrastructure moderne**
- Networking : Pourquoi Cilium remplace kube-proxy
- Observabilité : Comment Pixie élimine l'instrumentation manuelle
- Sécurité : Pourquoi eBPF est l'avenir de la runtime security

✅ **Évaluer l'adoption d'eBPF dans votre organisation**
- ROI (coûts, temps, performance)
- Risques et limitations
- Stratégie de migration

✅ **Continuer votre apprentissage de façon autonome**
- Connaître les ressources essentielles
- Savoir où trouver de l'aide
- Pouvoir contribuer à l'écosystème

## Comment aborder cette section

### Approche recommandée

**1. Lisez les trois sous-sections dans l'ordre**
- 21.6.1 Cilium → 21.6.2 Pixie → 21.6.3 Ressources
- Chaque sous-section s'appuie sur les précédentes

**2. Prenez le temps d'expérimenter**
- Déployez Cilium dans un cluster de test (kind, minikube)
- Installez Pixie et explorez votre cluster
- Testez les outils recommandés

**3. Pensez à vos cas d'usage**
- Comment Cilium pourrait améliorer votre infrastructure ?
- Quels problèmes Pixie résoudrait dans votre équipe ?
- Quelles sont vos prochaines étapes ?

**4. Rejoignez la communauté**
- Posez vos questions sur Slack
- Participez aux conférences (eBPF Summit)
- Partagez vos apprentissages

### Durée estimée

- **21.6.1 - Cilium** : 1-2 heures (lecture + expérimentation)
- **21.6.2 - Pixie** : 1-2 heures (lecture + expérimentation)
- **21.6.3 - Ressources** : 30 minutes (lecture de référence)

**Total :** 3-5 heures pour une compréhension complète

**Note :** Vous pouvez aussi survoler rapidement les sections pour avoir une vue d'ensemble, puis y revenir pour approfondir.

## État d'esprit à avoir

### Ce n'est pas juste une technologie de plus

eBPF n'est pas "juste un outil de plus" dans votre boîte à outils. C'est un **changement de paradigme** :

**Avant eBPF :**
```
Problème → Solution custom → Déploiement → Maintenance
```

**Avec eBPF :**
```
Problème → Solution eBPF existante → Deploy en 5 minutes → Bénéfices immédiats
```

### Pensez "plateforme", pas "outil"

eBPF est une **plateforme** sur laquelle sont construites de nombreuses solutions :
- Cilium pour networking
- Pixie pour observabilité
- Falco pour sécurité
- Et des dizaines d'autres...

Vous n'avez pas besoin de devenir un expert eBPF pour bénéficier de ces solutions. Elles sont conçues pour être utilisées par des équipes qui ne connaissent pas les détails d'eBPF.

### L'adoption est progressive

Vous n'avez pas à tout migrer d'un coup :

**Phase 1 :** Expérimenter
- Déployer dans un cluster de dev
- Tester les fonctionnalités
- Mesurer les gains

**Phase 2 :** Pilote
- Déployer en staging
- Valider en conditions réelles
- Former les équipes

**Phase 3 :** Production
- Rolling update progressif
- Monitoring intensif
- Optimisation continue

## Comparaison : eBPF vs approches traditionnelles

### Tableau récapitulatif

| Aspect | Approche traditionnelle | Approche eBPF |
|--------|------------------------|---------------|
| **Networking K8s** | iptables (O(n)) | eBPF (O(1)) |
| **Overhead CPU** | 1-2 cores/nœud | 0.1-0.3 cores/nœud |
| **Observabilité** | Instrumentation manuelle | Auto-instrumentation |
| **Temps setup** | Semaines/mois | Minutes/heures |
| **Modification code** | Obligatoire | Aucune |
| **Sidecars** | Nécessaires (50-100MB/pod) | Optionnels ou zéro |
| **Latence ajoutée** | 5-10ms | < 0.5ms |
| **Visibilité** | Partielle (ce qui est instrumenté) | Complète (tout est visible) |
| **Sécurité** | Réactive (logs) | Proactive (enforcement temps réel) |
| **Coûts** | Élevés (infra lourde) | Réduits (efficacité) |
| **Complexité** | Élevée (multiples outils) | Simplifiée (solutions intégrées) |

**Verdict :** eBPF est objectivement supérieur pour presque tous les aspects de l'infrastructure moderne. Les seules raisons de ne pas l'adopter sont les contraintes legacy (vieux kernels, environnements non-Linux).

## Questions fréquentes avant de commencer

**Q: Est-ce que j'ai besoin de maîtriser la programmation eBPF pour utiliser Cilium ou Pixie ?**

R: **Non !** C'est comme utiliser Kubernetes sans écrire du code Go. Cilium et Pixie sont des solutions prêtes à l'emploi. Comprendre eBPF aide, mais n'est pas requis.

**Q: Mon kernel est-il assez récent ?**

R: Vérifiez avec `uname -r`. Pour Cilium et Pixie :
- **Minimum** : Kernel 4.9
- **Recommandé** : Kernel 5.4+
- **Optimal** : Kernel 5.10+

La plupart des distributions modernes (Ubuntu 20.04+, RHEL 8+, etc.) sont OK.

**Q: Est-ce stable pour la production ?**

R: **Oui !** Cilium est utilisé par Google, AWS, Microsoft en production. Pixie par des milliers d'entreprises. eBPF est production-grade.

**Q: Quel est l'overhead d'eBPF ?**

R: **Très faible** :
- Cilium : ~0.1-0.3 cores/nœud
- Pixie : ~1-2GB RAM/nœud
- Impact latence : < 0.5ms dans la plupart des cas

**Q: Comment migrer depuis notre solution actuelle ?**

R: Les deux sous-sections couvriront les stratégies de migration. En général : déploiement progressif, tests approfondis, rollback plan.

**Q: Combien ça coûte ?**

R: Cilium et Pixie sont **open-source et gratuits**. Des versions commerciales avec support existent (chez Isovalent et New Relic), mais l'open-source est pleinement fonctionnel.

**Q: Est-ce que eBPF va remplacer iptables/kube-proxy ?**

R: **Progressivement, oui.** Google et AWS recommandent déjà Cilium. C'est la direction de l'industrie.

## Conclusion de l'introduction

Vous êtes maintenant prêt à découvrir comment eBPF transforme l'infrastructure moderne en production !

**Ce que vous allez voir dans les prochaines sous-sections :**

→ **21.6.1 - Cilium** : Comment eBPF révolutionne le networking Kubernetes
→ **21.6.2 - Pixie** : L'observabilité sans instrumentation devient réalité
→ **21.6.3 - Ressources** : Votre feuille de route pour continuer l'aventure

**Rappelez-vous :**
- eBPF n'est pas qu'une technologie, c'est un **nouveau paradigme**
- Vous n'avez pas besoin d'être un expert eBPF pour bénéficier de l'écosystème
- Les solutions comme Cilium et Pixie sont **production-ready**
- L'adoption peut être **progressive**
- Le ROI est **mesurable** (performance, coûts, productivité)

---

🚀 **Prêt ?** Plongeons dans Cilium dans la section 21.6.1 !

---

## Statistiques clés à retenir

Avant de commencer, gardez ces chiffres en tête pour comprendre l'impact d'eBPF :

**Performance :**
- 📊 10x moins d'utilisation CPU (networking)
- 📊 50% de réduction de latence
- 📊 Support de 10 000+ services sans dégradation

**Économies :**
- 💰 30-50% de réduction des coûts cloud
- 💰 Élimination de l'overhead des sidecars
- 💰 Réduction des coûts de storage (observabilité)

**Productivité :**
- ⏱️ Debugging 10x plus rapide
- ⏱️ Setup en minutes vs semaines
- ⏱️ Zéro modification de code

**Adoption :**
- 🌍 85% des nouveaux déploiements K8s utiliseront eBPF d'ici 2026
- 🌍 3 des 4 plus grands cloud providers recommandent Cilium
- 🌍 Projets CNCF : Cilium, Falco, Pixie, Tetragon

Ces chiffres ne sont pas du marketing, ce sont des résultats mesurés en production ! Vous allez voir comment dans les sections suivantes.

---

*Note : Cette section suppose que vous avez un cluster Kubernetes disponible pour les expérimentations (local avec kind/minikube ou cloud). Si ce n'est pas le cas, de nombreux exemples restent compréhensibles sans hands-on, et nous fournirons des liens vers des environnements interactifs.*

⏭️ [Cilium](/21-introduction-ebpf/06.1-cilium.md)
