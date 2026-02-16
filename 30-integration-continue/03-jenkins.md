🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 30.3 Jenkins : pipelines déclaratifs

## Introduction

**Jenkins** est l'outil d'intégration continue le plus ancien et le plus utilisé au monde (créé en 2011). Contrairement à GitHub Actions et GitLab CI qui sont des services cloud intégrés à des plateformes Git, Jenkins est un **serveur d'automatisation auto-hébergé** que vous installez et configurez vous-même.

Pour les projets C, Jenkins est particulièrement adapté aux environnements d'entreprise qui nécessitent un **contrôle total** sur l'infrastructure CI/CD, des **intégrations complexes**, ou des besoins spécifiques (compilation pour hardware propriétaire, tests sur environnements physiques, pipelines très personnalisés).

---

## Pourquoi Jenkins pour les projets C ?

### Les avantages uniques de Jenkins

1. **Contrôle total** : Vous possédez et gérez l'infrastructure
2. **Personnalisation illimitée** : Plus de 1800 plugins disponibles
3. **Pipelines complexes** : Gestion de workflows très élaborés
4. **Pas de quotas** : Ressources limitées uniquement par votre matériel
5. **Intégrations enterprise** : LDAP, Active Directory, SSO
6. **Hardware spécifique** : Tests sur matériel embarqué, architectures exotiques
7. **Legacy** : Intégration avec systèmes existants

### Cas d'usage idéaux pour Jenkins

- **Grandes entreprises** : Infrastructure CI/CD complexe et contrôlée
- **Embedded/Hardware** : Tests nécessitant du matériel physique
- **Environnements air-gapped** : Réseau isolé sans accès Internet
- **Pipelines très complexes** : Dizaines d'étapes conditionnelles
- **Multi-projets** : Orchestration de centaines de projets C
- **Sécurité stricte** : Tout reste dans le réseau interne

### Quand préférer GitHub Actions ou GitLab CI ?

Jenkins n'est **pas** le meilleur choix si :
- ❌ Vous voulez commencer rapidement sans configuration
- ❌ Vous n'avez pas de ressources pour maintenir un serveur
- ❌ Votre projet est simple (build + test standard)
- ❌ Vous préférez un SaaS clé en main

**Jenkins brille** pour les projets complexes et les grandes organisations.

---

## Architecture de Jenkins

### Les composants principaux

#### 1. Master (Controller)

Le **master** est le serveur Jenkins central qui :
- Héberge l'interface web
- Planifie et distribue les jobs
- Stocke la configuration
- Gère les plugins

#### 2. Agents (Nodes)

Les **agents** sont des machines (physiques ou virtuelles) qui exécutent les jobs.

**Types d'agents** :
- **Permanent** : Machine dédiée toujours disponible
- **Cloud** : Machines créées/détruites à la demande (Docker, Kubernetes)
- **SSH** : Connexion via SSH à des machines distantes

#### 3. Workspace

Le **workspace** est le répertoire de travail où le code est checkout et où le build s'exécute.

**Exemple** : `/var/jenkins/workspace/mon-projet-c/`

#### 4. Job/Pipeline

Un **job** ou **pipeline** est une tâche automatisée définie dans Jenkins.

### Hiérarchie

```
Master Jenkins
  ├── Agent 1 (Linux x64)
  │    ├── Workspace projet-A
  │    └── Workspace projet-B
  ├── Agent 2 (Linux ARM)
  │    └── Workspace projet-C
  └── Agent 3 (Windows)
       └── Workspace projet-D
```

---

## Pipelines déclaratifs vs scriptés

Jenkins propose deux syntaxes pour définir les pipelines :

### Pipeline scripté (Groovy - ancien)

```groovy
node {
    stage('Build') {
        sh 'gcc -o app main.c'
    }
    stage('Test') {
        sh './app --test'
    }
}
```

**Caractéristiques** :
- Langage : Groovy pur
- Flexibilité maximale
- Plus difficile à lire
- Moins de validation

### Pipeline déclaratif (recommandé)

```groovy
pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh 'gcc -o app main.c'
            }
        }
        stage('Test') {
            steps {
                sh './app --test'
            }
        }
    }
}
```

**Caractéristiques** :
- Structure claire et lisible
- Validation de syntaxe
- Meilleure intégration avec l'UI
- Recommandé pour 99% des cas

**Dans ce tutoriel, nous utiliserons exclusivement les pipelines déclaratifs.**

---

## Le Jenkinsfile

Le **Jenkinsfile** est le fichier qui définit votre pipeline. C'est l'équivalent de `.github/workflows/ci.yml` ou `.gitlab-ci.yml`.

**Emplacement** : À la racine du projet Git

**Nom** : `Jenkinsfile` (sans extension)

### Structure de base

```groovy
pipeline {
    agent any

    stages {
        stage('Checkout') {
            steps {
                checkout scm
            }
        }

        stage('Build') {
            steps {
                sh 'gcc -o app main.c'
            }
        }

        stage('Test') {
            steps {
                sh './app --test'
            }
        }
    }
}
```

---

## Sections d'un pipeline déclaratif

### 1. agent

Définit **où** le pipeline s'exécute :

```groovy
// Sur n'importe quel agent
agent any

// Sur un agent spécifique avec label
agent { label 'linux' }

// Avec Docker
agent {
    docker {
        image 'gcc:13'
    }
}

// Pas d'agent global (défini par stage)
agent none
```

### 2. environment

Définit des variables d'environnement :

```groovy
environment {
    CC = 'gcc-13'
    CFLAGS = '-Wall -Wextra -O2'
    BUILD_DIR = 'build'
}
```

### 3. stages

Contient la liste des stages (étapes) :

```groovy
stages {
    stage('Build') { ... }
    stage('Test') { ... }
    stage('Deploy') { ... }
}
```

### 4. stage

Une étape individuelle du pipeline :

```groovy
stage('Build') {
    steps {
        sh 'make'
    }
}
```

### 5. steps

Les commandes à exécuter dans un stage :

```groovy
steps {
    sh 'gcc --version'
    sh 'cmake -B build'
    sh 'cmake --build build'
}
```

### 6. post

Actions à exécuter après le pipeline (succès/échec) :

```groovy
post {
    always {
        echo 'Pipeline terminé'
    }
    success {
        echo 'Build réussi !'
    }
    failure {
        echo 'Build échoué !'
    }
}
```

---

## Exemple complet : Pipeline pour un projet C

Voici un Jenkinsfile complet pour un projet C :

**Jenkinsfile**

```groovy
pipeline {
    agent any

    environment {
        CC = 'gcc'
        BUILD_DIR = 'build'
        CFLAGS = '-Wall -Wextra'
    }

    stages {
        stage('Checkout') {
            steps {
                // Récupération du code depuis Git
                checkout scm
            }
        }

        stage('Environment') {
            steps {
                echo 'Vérification de l\'environnement'
                sh '''
                    gcc --version
                    cmake --version
                    uname -a
                '''
            }
        }

        stage('Configure') {
            steps {
                echo 'Configuration avec CMake'
                sh 'cmake -B ${BUILD_DIR} -DCMAKE_BUILD_TYPE=Release'
            }
        }

        stage('Build') {
            steps {
                echo 'Compilation du projet'
                sh 'cmake --build ${BUILD_DIR} -j $(nproc)'
            }
        }

        stage('Test') {
            steps {
                echo 'Exécution des tests'
                dir("${BUILD_DIR}") {
                    sh 'ctest --output-on-failure'
                }
            }
        }

        stage('Archive') {
            steps {
                echo 'Archivage des artifacts'
                archiveArtifacts artifacts: "${BUILD_DIR}/mon_app", fingerprint: true
            }
        }
    }

    post {
        always {
            echo 'Nettoyage du workspace'
            cleanWs()
        }
        success {
            echo 'Build réussi ! ✅'
        }
        failure {
            echo 'Build échoué ! ❌'
        }
    }
}
```

---

## Agents et exécution

### Agent global

```groovy
pipeline {
    agent any  // Tous les stages utilisent le même agent

    stages {
        stage('Build') { ... }
    }
}
```

### Agent par stage

```groovy
pipeline {
    agent none  // Pas d'agent global

    stages {
        stage('Build Linux') {
            agent { label 'linux' }
            steps {
                sh 'gcc -o app-linux main.c'
            }
        }

        stage('Build Windows') {
            agent { label 'windows' }
            steps {
                bat 'gcc -o app.exe main.c'
            }
        }
    }
}
```

### Agent Docker

```groovy
pipeline {
    agent {
        docker {
            image 'gcc:13'
            args '-v /tmp:/tmp'
        }
    }

    stages {
        stage('Build') {
            steps {
                sh 'gcc --version'
                sh 'gcc -o app main.c'
            }
        }
    }
}
```

**Avantage Docker** : Environnement reproductible, pas besoin d'installer GCC sur l'agent.

---

## Directives conditionnelles

### when

Exécuter un stage conditionnellement :

```groovy
stage('Deploy') {
    when {
        branch 'main'  // Seulement sur la branche main
    }
    steps {
        sh 'make deploy'
    }
}

stage('Release') {
    when {
        tag pattern: "v\\d+\\.\\d+\\.\\d+", comparator: "REGEXP"
    }
    steps {
        sh 'make release'
    }
}

stage('Windows Build') {
    when {
        expression { return env.OS == 'windows' }
    }
    steps {
        bat 'build.bat'
    }
}
```

**Conditions disponibles** :
- `branch` : Nom de la branche
- `tag` : Tag Git
- `expression` : Expression Groovy
- `environment` : Variable d'environnement
- `changelog` : Changement dans le commit

---

## Parallel (exécution parallèle)

Pour exécuter plusieurs stages en parallèle :

```groovy
stage('Build Matrix') {
    parallel {
        stage('GCC 11') {
            steps {
                sh 'gcc-11 -o app-gcc11 main.c'
            }
        }
        stage('GCC 12') {
            steps {
                sh 'gcc-12 -o app-gcc12 main.c'
            }
        }
        stage('Clang 15') {
            steps {
                sh 'clang-15 -o app-clang15 main.c'
            }
        }
    }
}
```

**Résultat** : Les 3 compilations s'exécutent simultanément.

---

## Intégration CMake complète

```groovy
pipeline {
    agent any

    environment {
        BUILD_TYPE = 'Release'
        BUILD_DIR = 'build'
    }

    stages {
        stage('Configure') {
            steps {
                sh """
                    cmake -B ${BUILD_DIR} \
                          -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
                          -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
                """
            }
        }

        stage('Build') {
            steps {
                sh "cmake --build ${BUILD_DIR} --config ${BUILD_TYPE} -j \$(nproc)"
            }
        }

        stage('Test') {
            steps {
                dir("${BUILD_DIR}") {
                    sh 'ctest --output-on-failure --verbose'
                }
            }
        }

        stage('Install') {
            steps {
                sh "cmake --install ${BUILD_DIR} --prefix install"
            }
        }
    }

    post {
        always {
            // Publier les résultats de tests
            junit "${BUILD_DIR}/test-results/*.xml"
        }
    }
}
```

---

## Tests unitaires avec JUnit

Jenkins peut afficher graphiquement les résultats de tests au format JUnit XML :

```groovy
stage('Test') {
    steps {
        sh 'cd build && ctest --output-junit test-results.xml'
    }
    post {
        always {
            junit 'build/test-results.xml'
        }
    }
}
```

**Résultat** : Graphiques de tendance des tests dans l'interface Jenkins.

---

## Sanitizers en pipeline

```groovy
pipeline {
    agent any

    stages {
        stage('Build with Sanitizers') {
            parallel {
                stage('AddressSanitizer') {
                    steps {
                        sh '''
                            cmake -B build-asan \
                                  -DCMAKE_C_FLAGS="-fsanitize=address -g"
                            cmake --build build-asan
                            cd build-asan && ctest
                        '''
                    }
                }

                stage('UndefinedBehaviorSanitizer') {
                    steps {
                        sh '''
                            cmake -B build-ubsan \
                                  -DCMAKE_C_FLAGS="-fsanitize=undefined -g"
                            cmake --build build-ubsan
                            cd build-ubsan && ctest
                        '''
                    }
                }

                stage('ThreadSanitizer') {
                    steps {
                        sh '''
                            cmake -B build-tsan \
                                  -DCMAKE_C_FLAGS="-fsanitize=thread -g"
                            cmake --build build-tsan
                            cd build-tsan && ctest
                        '''
                    }
                }
            }
        }
    }
}
```

---

## Valgrind automatisé

```groovy
stage('Valgrind Analysis') {
    steps {
        sh '''
            valgrind --leak-check=full \
                     --xml=yes \
                     --xml-file=valgrind-report.xml \
                     ./build/mon_app
        '''
    }
    post {
        always {
            // Plugin Valgrind pour visualisation
            publishValgrind pattern: 'valgrind-report.xml'
        }
    }
}
```

**Note** : Nécessite le plugin "Valgrind Plugin"

---

## Analyse statique

### clang-tidy

```groovy
stage('Static Analysis - clang-tidy') {
    steps {
        sh '''
            cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
            clang-tidy -p build src/*.c > clang-tidy-report.txt || true
        '''
    }
    post {
        always {
            archiveArtifacts 'clang-tidy-report.txt'
        }
    }
}
```

### cppcheck

```groovy
stage('Static Analysis - cppcheck') {
    steps {
        sh '''
            cppcheck --enable=all \
                     --xml \
                     --xml-version=2 \
                     src/ 2> cppcheck-report.xml
        '''
    }
    post {
        always {
            // Plugin cppcheck pour visualisation
            publishCppcheck pattern: 'cppcheck-report.xml'
        }
    }
}
```

**Note** : Nécessite le plugin "Cppcheck Plugin"

---

## Couverture de code (gcov/lcov)

```groovy
stage('Coverage') {
    steps {
        sh '''
            cmake -B build -DCMAKE_C_FLAGS="--coverage -O0"
            cmake --build build
            cd build && ctest
            lcov --capture --directory . --output-file coverage.info
            lcov --remove coverage.info '/usr/*' '*/tests/*' -o coverage.info
            genhtml coverage.info --output-directory coverage-html
        '''
    }
    post {
        always {
            // Plugin Cobertura pour visualisation
            publishCoverage adapters: [lcovAdapter('build/coverage.info')]
            publishHTML target: [
                reportDir: 'build/coverage-html',
                reportFiles: 'index.html',
                reportName: 'Coverage Report'
            ]
        }
    }
}
```

**Note** : Nécessite le plugin "Code Coverage API Plugin"

---

## Archivage d'artifacts

### archiveArtifacts

```groovy
stage('Archive') {
    steps {
        archiveArtifacts artifacts: 'build/mon_app', fingerprint: true
        archiveArtifacts artifacts: 'build/lib/*.so', fingerprint: true
        archiveArtifacts artifacts: '*.log', allowEmptyArchive: true
    }
}
```

**Options** :
- `artifacts` : Pattern des fichiers à archiver
- `fingerprint` : Calcule une empreinte MD5
- `allowEmptyArchive` : Ne pas échouer si aucun fichier

### stash/unstash

Pour passer des fichiers entre stages sur différents agents :

```groovy
stage('Build') {
    steps {
        sh 'gcc -o app main.c'
        stash name: 'binary', includes: 'app'
    }
}

stage('Test') {
    agent { label 'test-machine' }
    steps {
        unstash 'binary'
        sh './app --test'
    }
}
```

---

## Credentials (secrets)

Jenkins gère les secrets de manière sécurisée :

### Définir un credential

Dans Jenkins UI : `Manage Jenkins → Credentials → Add Credentials`

Types :
- Username/Password
- Secret text
- SSH key
- Certificate

### Utiliser un credential

```groovy
environment {
    CODECOV_TOKEN = credentials('codecov-token-id')
}

stages {
    stage('Upload Coverage') {
        steps {
            sh 'pip install codecov-cli && codecovcli upload-process -f coverage.info -t $CODECOV_TOKEN'
        }
    }
}
```

### withCredentials

Pour des credentials plus complexes :

```groovy
stage('Deploy') {
    steps {
        withCredentials([
            usernamePassword(
                credentialsId: 'ssh-credentials',
                usernameVariable: 'SSH_USER',
                passwordVariable: 'SSH_PASS'
            )
        ]) {
            sh 'sshpass -p "$SSH_PASS" scp app $SSH_USER@server:/opt/app'
        }
    }
}
```

---

## Notifications

### Email

```groovy
post {
    failure {
        emailext(
            subject: "Build Failed: ${env.JOB_NAME} - ${env.BUILD_NUMBER}",
            body: "Le build a échoué. Voir ${env.BUILD_URL}",
            to: 'dev-team@example.com'
        )
    }
}
```

### Slack

```groovy
post {
    success {
        slackSend(
            color: 'good',
            message: "Build réussi: ${env.JOB_NAME} #${env.BUILD_NUMBER}"
        )
    }
    failure {
        slackSend(
            color: 'danger',
            message: "Build échoué: ${env.JOB_NAME} #${env.BUILD_NUMBER}"
        )
    }
}
```

**Note** : Nécessite le plugin "Slack Notification Plugin"

---

## Pipeline complet production-ready

Voici un Jenkinsfile complet et professionnel :

**Jenkinsfile**

```groovy
pipeline {
    agent any

    environment {
        CC = 'gcc-13'
        BUILD_DIR = 'build'
        INSTALL_DIR = 'install'
    }

    options {
        buildDiscarder(logRotator(numToKeepStr: '10'))
        timestamps()
        timeout(time: 1, unit: 'HOURS')
    }

    stages {
        stage('Checkout') {
            steps {
                echo 'Récupération du code source'
                checkout scm
            }
        }

        stage('Environment Check') {
            steps {
                echo 'Vérification de l\'environnement'
                sh '''
                    echo "Node: ${NODE_NAME}"
                    echo "Workspace: ${WORKSPACE}"
                    gcc --version
                    cmake --version
                '''
            }
        }

        stage('Build Matrix') {
            parallel {
                stage('Debug Build') {
                    steps {
                        echo 'Build Debug'
                        sh '''
                            cmake -B build-debug -DCMAKE_BUILD_TYPE=Debug
                            cmake --build build-debug -j $(nproc)
                        '''
                    }
                }

                stage('Release Build') {
                    steps {
                        echo 'Build Release'
                        sh '''
                            cmake -B build-release -DCMAKE_BUILD_TYPE=Release
                            cmake --build build-release -j $(nproc)
                        '''
                    }
                }
            }
        }

        stage('Tests') {
            parallel {
                stage('Unit Tests') {
                    steps {
                        dir('build-debug') {
                            sh 'ctest --output-on-failure --output-junit test-results.xml'
                        }
                    }
                    post {
                        always {
                            junit 'build-debug/test-results.xml'
                        }
                    }
                }

                stage('Sanitizers') {
                    steps {
                        sh '''
                            cmake -B build-asan -DCMAKE_C_FLAGS="-fsanitize=address,undefined -g"
                            cmake --build build-asan
                            cd build-asan && ctest --output-on-failure
                        '''
                    }
                }
            }
        }

        stage('Analysis') {
            parallel {
                stage('Valgrind') {
                    steps {
                        sh '''
                            valgrind --leak-check=full \
                                     --xml=yes \
                                     --xml-file=valgrind-report.xml \
                                     ./build-debug/mon_app || true
                        '''
                    }
                    post {
                        always {
                            publishValgrind pattern: 'valgrind-report.xml'
                        }
                    }
                }

                stage('Static Analysis') {
                    steps {
                        sh '''
                            cmake -B build-analysis -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
                            clang-tidy -p build-analysis src/*.c > clang-tidy-report.txt || true
                            cppcheck --enable=all --xml --xml-version=2 src/ 2> cppcheck-report.xml
                        '''
                    }
                    post {
                        always {
                            archiveArtifacts 'clang-tidy-report.txt'
                            publishCppcheck pattern: 'cppcheck-report.xml'
                        }
                    }
                }

                stage('Coverage') {
                    steps {
                        sh '''
                            cmake -B build-cov -DCMAKE_C_FLAGS="--coverage -O0"
                            cmake --build build-cov
                            cd build-cov && ctest
                            lcov --capture --directory . --output-file coverage.info
                            lcov --remove coverage.info '/usr/*' '*/tests/*' -o coverage.info
                            genhtml coverage.info --output-directory coverage-html
                        '''
                    }
                    post {
                        always {
                            publishCoverage adapters: [lcovAdapter('build-cov/coverage.info')]
                            publishHTML target: [
                                reportDir: 'build-cov/coverage-html',
                                reportFiles: 'index.html',
                                reportName: 'Coverage Report'
                            ]
                        }
                    }
                }
            }
        }

        stage('Package') {
            when {
                branch 'main'
            }
            steps {
                echo 'Création du package'
                sh '''
                    cmake --install build-release --prefix ${INSTALL_DIR}
                    tar -czf mon-app-${BUILD_NUMBER}.tar.gz ${INSTALL_DIR}
                '''
            }
        }

        stage('Archive') {
            steps {
                archiveArtifacts artifacts: 'build-release/mon_app', fingerprint: true
                archiveArtifacts artifacts: 'mon-app-*.tar.gz', fingerprint: true, allowEmptyArchive: true
            }
        }
    }

    post {
        always {
            echo 'Pipeline terminé'
            cleanWs()
        }
        success {
            echo '✅ Build réussi !'
            slackSend color: 'good', message: "Build #${BUILD_NUMBER} réussi"
        }
        failure {
            echo '❌ Build échoué !'
            emailext(
                subject: "Build Failed: ${JOB_NAME} #${BUILD_NUMBER}",
                body: "Le build a échoué. Voir ${BUILD_URL}",
                to: 'dev-team@example.com'
            )
            slackSend color: 'danger', message: "Build #${BUILD_NUMBER} échoué"
        }
    }
}
```

---

## Options du pipeline

```groovy
options {
    // Limite de builds à conserver
    buildDiscarder(logRotator(numToKeepStr: '10'))

    // Afficher timestamps dans les logs
    timestamps()

    // Timeout global du pipeline
    timeout(time: 1, unit: 'HOURS')

    // Ne pas autoriser builds parallèles
    disableConcurrentBuilds()

    // Retry automatique en cas d'échec
    retry(2)

    // Éviter le checkout automatique
    skipDefaultCheckout()
}
```

---

## Triggers (déclencheurs)

```groovy
triggers {
    // Polling SCM toutes les 5 minutes
    pollSCM('H/5 * * * *')

    // Build périodique (nightly)
    cron('H 2 * * *')

    // Déclenché par un autre job
    upstream(
        upstreamProjects: 'autre-projet',
        threshold: hudson.model.Result.SUCCESS
    )
}
```

**Syntaxe cron** :
```
# Minute Heure Jour Mois JourSemaine
H 2 * * *        # Tous les jours à 2h (H = aléatoire pour répartir la charge)  
H/15 * * * *     # Toutes les 15 minutes  
H 0 * * 1-5      # Lundi à vendredi à minuit  
```

---

## Parameters (paramètres)

Pour des pipelines paramétrables :

```groovy
pipeline {
    agent any

    parameters {
        choice(
            name: 'BUILD_TYPE',
            choices: ['Debug', 'Release', 'RelWithDebInfo'],
            description: 'Type de build CMake'
        )

        booleanParam(
            name: 'RUN_TESTS',
            defaultValue: true,
            description: 'Exécuter les tests ?'
        )

        string(
            name: 'COMPILER',
            defaultValue: 'gcc-13',
            description: 'Compilateur à utiliser'
        )
    }

    stages {
        stage('Build') {
            steps {
                sh """
                    cmake -B build \
                          -DCMAKE_BUILD_TYPE=${params.BUILD_TYPE} \
                          -DCMAKE_C_COMPILER=${params.COMPILER}
                    cmake --build build
                """
            }
        }

        stage('Test') {
            when {
                expression { return params.RUN_TESTS }
            }
            steps {
                sh 'cd build && ctest'
            }
        }
    }
}
```

**Utilisation** : L'utilisateur peut choisir les paramètres avant de lancer le build.

---

## Multi-branch Pipelines

Jenkins peut automatiquement créer des pipelines pour chaque branche :

### Configuration

1. Créer un "Multibranch Pipeline" dans Jenkins
2. Configurer la source Git
3. Jenkins détecte automatiquement les branches avec un `Jenkinsfile`

### Jenkinsfile adapté

```groovy
pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                echo "Building branch: ${env.BRANCH_NAME}"
                sh 'make'
            }
        }

        stage('Deploy to Staging') {
            when {
                branch 'develop'
            }
            steps {
                sh 'deploy-staging.sh'
            }
        }

        stage('Deploy to Production') {
            when {
                branch 'main'
            }
            steps {
                sh 'deploy-production.sh'
            }
        }
    }
}
```

**Avantage** : Chaque branche a son propre pipeline, les PRs sont testées automatiquement.

---

## Installation et configuration de Jenkins

### Installation (Ubuntu)

```bash
# Ajouter la clé GPG
curl -fsSL https://pkg.jenkins.io/debian-stable/jenkins.io-2023.key | sudo tee \
  /usr/share/keyrings/jenkins-keyring.asc > /dev/null

# Ajouter le repository
echo deb [signed-by=/usr/share/keyrings/jenkins-keyring.asc] \
  https://pkg.jenkins.io/debian-stable binary/ | sudo tee \
  /etc/apt/sources.list.d/jenkins.list > /dev/null

# Installer Jenkins
sudo apt-get update  
sudo apt-get install jenkins  

# Démarrer Jenkins
sudo systemctl start jenkins  
sudo systemctl enable jenkins  
```

### Premier accès

1. Ouvrir `http://localhost:8080`
2. Récupérer le mot de passe initial :
   ```bash
   sudo cat /var/lib/jenkins/secrets/initialAdminPassword
   ```
3. Installer les plugins recommandés
4. Créer un utilisateur admin

### Plugins essentiels pour C

- **Pipeline** : Support des pipelines déclaratifs
- **Git Plugin** : Intégration Git
- **Docker Pipeline** : Support Docker
- **JUnit Plugin** : Affichage résultats tests
- **Cobertura Plugin** : Couverture de code
- **Valgrind Plugin** : Rapports Valgrind
- **Cppcheck Plugin** : Analyse statique
- **Warnings Next Generation** : Warnings compilateur
- **HTML Publisher** : Publication HTML
- **Email Extension** : Notifications email
- **Slack Notification** : Notifications Slack

---

## Différences avec GitHub Actions et GitLab CI

| Aspect | Jenkins | GitHub Actions | GitLab CI |
|--------|---------|----------------|-----------|
| **Hébergement** | Self-hosted | Cloud (ou self-hosted) | Cloud ou self-hosted |
| **Configuration** | Complexe | Simple | Moyenne |
| **Fichier config** | Jenkinsfile | .github/workflows/*.yml | .gitlab-ci.yml |
| **Syntaxe** | Groovy | YAML | YAML |
| **Plugins** | 1800+ | Marketplace | Limité |
| **UI** | Riche mais datée | Moderne | Moderne |
| **Courbe d'apprentissage** | Élevée | Faible | Moyenne |
| **Gratuit** | ✅ Open-source | ✅ Open-source illimité | 400 min/mois |
| **Maintenance** | Nécessaire | Aucune | Aucune (SaaS) |
| **Multi-projets** | ✅ Excellent | ⚠️ Moyen | ✅ Bon |

**Résumé** :
- **Jenkins** : Contrôle total, complexe, nécessite maintenance
- **GitHub Actions** : Simple, intégré GitHub, gratuit open-source
- **GitLab CI** : Équilibre entre les deux, bon pour entreprise

---

## Bonnes pratiques Jenkins pour C

### 1. Utilisez Docker pour les agents

```groovy
agent {
    docker {
        image 'gcc:13'
        args '-v /tmp:/tmp'
    }
}
```

**Avantage** : Environnement reproductible, pas de configuration manuelle.

### 2. Shared Libraries

Pour réutiliser du code entre pipelines :

**vars/buildC.groovy**
```groovy
def call(String buildType = 'Release') {
    sh """
        cmake -B build -DCMAKE_BUILD_TYPE=${buildType}
        cmake --build build -j \$(nproc)
    """
}
```

**Jenkinsfile**
```groovy
@Library('my-shared-lib') _

pipeline {
    agent any
    stages {
        stage('Build') {
            steps {
                buildC('Release')
            }
        }
    }
}
```

### 3. Nettoyez les workspaces

```groovy
post {
    always {
        cleanWs()  // Nettoie le workspace après le build
    }
}
```

### 4. Timeouts

```groovy
options {
    timeout(time: 30, unit: 'MINUTES')
}
```

### 5. Logs structurés

```groovy
steps {
    echo '====== COMPILATION ======'
    sh 'make'
    echo '====== FIN COMPILATION ======'
}
```

### 6. Archivez sélectivement

```groovy
archiveArtifacts artifacts: 'build/mon_app', fingerprint: true
// Ne pas archiver tout build/ pour économiser l'espace
```

### 7. Parallélisez intelligemment

```groovy
// ✅ Bon : stages indépendants en parallèle
parallel {
    stage('GCC') { ... }
    stage('Clang') { ... }
}

// ❌ Mauvais : stages dépendants en parallèle
```

---

## Sécurité Jenkins

### 1. Authentification

- Activez l'authentification (LDAP, Active Directory, ou DB interne)
- Désactivez la création de comptes publics

### 2. Autorisations

- Utilisez Matrix Authorization Strategy
- Principe du moindre privilège

### 3. CSRF Protection

Activé par défaut, ne pas désactiver.

### 4. Credentials

- Stockez les secrets dans Jenkins Credentials
- Ne jamais hardcoder dans les Jenkinsfile

### 5. Mises à jour

- Mettez à jour régulièrement Jenkins et les plugins
- Abonnez-vous aux alertes de sécurité

### 6. Agents sécurisés

- Utilisez JNLP ou SSH pour les agents
- Évitez les agents sur Internet public

---

## Troubleshooting

### Pipeline ne démarre pas

**Causes possibles** :
- Aucun agent disponible avec le label requis
- Workspace verrouillé

**Solution** :
```groovy
agent any  // Utilisez n'importe quel agent
```

### Erreur "script not permitted"

**Cause** : Groovy sandboxing bloque le script

**Solution** : Approuver le script dans `Manage Jenkins → In-process Script Approval`

### Build très lent

**Solutions** :
- Utilisez le cache
- Parallélisez les stages
- Utilisez des agents plus puissants
- Nettoyez les anciens builds

### Workspace plein

**Solution** :
```groovy
options {
    buildDiscarder(logRotator(numToKeepStr: '5'))  // Garde seulement 5 builds
}
```

---

## Résumé

**Jenkins = Serveur CI/CD auto-hébergé puissant et flexible**

**Jenkinsfile** : Fichier Groovy définissant le pipeline

**Structure** :
```groovy
pipeline {
    agent { ... }
    environment { ... }
    stages {
        stage('Build') {
            steps { ... }
        }
    }
    post { ... }
}
```

**Avantages** :
- Contrôle total de l'infrastructure
- Personnalisation illimitée (1800+ plugins)
- Pas de quotas de minutes
- Parfait pour entreprise

**Inconvénients** :
- Configuration complexe
- Maintenance nécessaire
- Courbe d'apprentissage élevée
- UI datée

**Cas d'usage idéaux** :
- Grandes entreprises
- Pipelines très complexes
- Hardware spécifique
- Environnements air-gapped

---

## Prochaines étapes

Maintenant que vous connaissez Jenkins, vous pouvez :

- Installer votre propre serveur Jenkins
- Créer des pipelines déclaratifs
- Configurer des agents (Docker, SSH)
- Intégrer tous les outils C (CMake, sanitizers, Valgrind)
- Mettre en place des multi-branch pipelines
- Créer des shared libraries

---

## Ressources complémentaires

- [Documentation Jenkins](https://www.jenkins.io/doc/)
- [Pipeline Syntax Reference](https://www.jenkins.io/doc/book/pipeline/syntax/)
- [Jenkins Plugins Index](https://plugins.jenkins.io/)
- [Jenkins User Handbook](https://www.jenkins.io/doc/book/)

---

**🎯 Points clés à retenir :**

- ✅ Jenkins = Serveur CI/CD auto-hébergé
- ✅ Jenkinsfile en Groovy (pipeline déclaratif recommandé)
- ✅ Structure : agent → stages → steps
- ✅ Plugins essentiels pour C (JUnit, Cobertura, Valgrind, Cppcheck)
- ✅ Docker recommandé pour agents reproductibles
- ✅ Parfait pour entreprise et pipelines complexes
- ✅ Nécessite maintenance mais contrôle total
- ✅ Parallélisation avec `parallel` pour matrix builds

⏭️ [Gestion des caches](/30-integration-continue/04-gestion-caches.md)
