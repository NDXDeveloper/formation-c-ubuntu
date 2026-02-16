🔝 Retour au [Sommaire](/SOMMAIRE.md)

# 30.5 Notifications et reporting

## Introduction

Les **notifications et rapports** sont essentiels en CI/CD pour tenir l'équipe informée de l'état du projet. Plutôt que de consulter manuellement l'interface CI après chaque commit, les notifications vous **alertent automatiquement** en cas de problème, et les rapports vous donnent une **vue d'ensemble de la qualité** du code.

Pour les projets C, où les bugs (fuites mémoire, segfaults, comportements indéfinis) peuvent être subtils et critiques, un système de notification et reporting bien configuré peut **faire la différence entre détecter un bug immédiatement ou le découvrir en production**.

---

## Pourquoi les notifications sont cruciales ?

### Le problème sans notifications

**Scénario classique** :
1. Développeur pousse un commit qui casse le build
2. Pipeline CI échoue
3. Personne ne le remarque immédiatement
4. D'autres développeurs pullent le code cassé
5. Toute l'équipe est bloquée
6. 2 heures plus tard, quelqu'un remarque le problème

### Avec notifications

**Scénario amélioré** :
1. Développeur pousse un commit qui casse le build
2. Pipeline CI échoue
3. **Notification immédiate** sur Slack : "❌ Build échoué - commit abc123"
4. Développeur voit l'alerte en 30 secondes
5. Correction en 5 minutes
6. Équipe jamais bloquée

**Gain** : 2 heures → 5 minutes

---

## Types de notifications

### 1. Par gravité

| Type | Quand | Exemple |
|------|-------|---------|
| **Critique** | Build échoué, tests échoués | "❌ Build échoué sur main" |
| **Avertissement** | Tests flaky, coverage baisse | "⚠️ Coverage passé de 85% à 80%" |
| **Info** | Build réussi, deploy terminé | "✅ Build réussi - v1.2.3" |
| **Debug** | Détails techniques | "Tests exécutés en 3m 45s" |

### 2. Par canal

| Canal | Usage | Avantages |
|-------|-------|-----------|
| **Email** | Notifications individuelles | Persistant, traçable |
| **Slack/Teams** | Communication d'équipe | Temps réel, discussion |
| **Discord** | Projets open-source | Communauté, accessible |
| **GitHub/GitLab** | Intégré à la plateforme | Contexte complet |
| **SMS** | Alertes critiques | Urgence, garantie réception |
| **Webhook** | Intégrations custom | Flexible, programmable |

### 3. Par destinataire

- **Auteur du commit** : Notification personnelle si son commit casse
- **Équipe** : Notification sur canal d'équipe
- **Leads/Managers** : Rapports hebdomadaires
- **Public** : Badge de statut sur README

---

## Notifications avec GitHub Actions

### Notifications natives GitHub

Par défaut, GitHub envoie des notifications :
- Dans l'onglet "Actions" du dépôt
- Notifications web GitHub (cloche 🔔)
- Emails (si configuré dans les settings)

### Configuration des notifications par défaut

Dans `Settings → Notifications` de votre compte GitHub :
- ✅ Actions workflows
- ✅ Failed workflows only (recommandé)

### Notifications Slack

#### Installation

1. Créer une Slack App avec webhook
2. Ajouter le webhook URL dans `Secrets` GitHub
3. Utiliser une action Slack

**Workflow avec Slack** :

```yaml
name: Build avec Notifications Slack

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest

    steps:
    - uses: actions/checkout@v4

    - name: Compiler
      run: |
        gcc -o app main.c

    - name: Tester
      run: ./app --test

    - name: Notification Slack - Succès
      if: success()
      uses: slackapi/slack-github-action@v1
      with:
        payload: |
          {
            "text": "✅ Build réussi !",
            "blocks": [
              {
                "type": "section",
                "text": {
                  "type": "mrkdwn",
                  "text": "*Build réussi* ✅\n*Projet:* ${{ github.repository }}\n*Branche:* ${{ github.ref_name }}\n*Commit:* ${{ github.sha }}\n*Auteur:* ${{ github.actor }}"
                }
              },
              {
                "type": "actions",
                "elements": [
                  {
                    "type": "button",
                    "text": {
                      "type": "plain_text",
                      "text": "Voir le build"
                    },
                    "url": "${{ github.server_url }}/${{ github.repository }}/actions/runs/${{ github.run_id }}"
                  }
                ]
              }
            ]
          }
      env:
        SLACK_WEBHOOK_URL: ${{ secrets.SLACK_WEBHOOK_URL }}

    - name: Notification Slack - Échec
      if: failure()
      uses: slackapi/slack-github-action@v1
      with:
        payload: |
          {
            "text": "❌ Build échoué !",
            "blocks": [
              {
                "type": "section",
                "text": {
                  "type": "mrkdwn",
                  "text": "*Build échoué* ❌\n*Projet:* ${{ github.repository }}\n*Branche:* ${{ github.ref_name }}\n*Commit:* <${{ github.event.head_commit.url }}|${{ github.sha }}>\n*Auteur:* ${{ github.actor }}\n*Message:* ${{ github.event.head_commit.message }}"
                }
              },
              {
                "type": "actions",
                "elements": [
                  {
                    "type": "button",
                    "text": {
                      "type": "plain_text",
                      "text": "Voir les logs"
                    },
                    "url": "${{ github.server_url }}/${{ github.repository }}/actions/runs/${{ github.run_id }}",
                    "style": "danger"
                  }
                ]
              }
            ]
          }
      env:
        SLACK_WEBHOOK_URL: ${{ secrets.SLACK_WEBHOOK_URL }}
```

### Notifications Discord

```yaml
- name: Notification Discord
  if: always()
  uses: sarisia/actions-status-discord@v1
  with:
    webhook: ${{ secrets.DISCORD_WEBHOOK }}
    status: ${{ job.status }}
    title: "Build ${{ github.repository }}"
    description: |
      Branche: ${{ github.ref_name }}
      Commit: ${{ github.sha }}
      Auteur: ${{ github.actor }}
    color: ${{ job.status == 'success' && '0x00ff00' || '0xff0000' }}
```

### Notifications Microsoft Teams

```yaml
- name: Notification Teams
  if: failure()
  uses: jdcargile/ms-teams-notification@v1.3
  with:
    github-token: ${{ github.token }}
    ms-teams-webhook-uri: ${{ secrets.MS_TEAMS_WEBHOOK_URI }}
    notification-summary: "Build échoué sur ${{ github.ref_name }}"
    notification-color: dc3545
```

### Notifications Email personnalisées

```yaml
- name: Envoyer email si échec
  if: failure()
  uses: dawidd6/action-send-mail@v3
  with:
    server_address: smtp.gmail.com
    server_port: 465
    username: ${{ secrets.MAIL_USERNAME }}
    password: ${{ secrets.MAIL_PASSWORD }}
    subject: "❌ Build échoué - ${{ github.repository }}"
    to: dev-team@example.com
    from: ci-bot@example.com
    body: |
      Le build a échoué sur la branche ${{ github.ref_name }}.

      Commit: ${{ github.sha }}
      Auteur: ${{ github.actor }}
      Message: ${{ github.event.head_commit.message }}

      Voir les détails: ${{ github.server_url }}/${{ github.repository }}/actions/runs/${{ github.run_id }}
```

---

## Notifications avec GitLab CI

### Notifications natives GitLab

GitLab envoie automatiquement des emails pour :
- Pipeline échoué
- Pipeline réussi (optionnel)

Configuration dans `Settings → Notifications`

### Notifications Slack dans GitLab

**Configuration du webhook** :

Dans GitLab : `Settings → Integrations → Slack notifications`

**Dans le pipeline** :

```yaml
notify:slack:
  stage: .post  # Stage spécial exécuté en dernier
  script:
    - |
      if [ "$CI_JOB_STATUS" == "success" ]; then
        EMOJI="✅"
        COLOR="good"
        MESSAGE="Build réussi"
      else
        EMOJI="❌"
        COLOR="danger"
        MESSAGE="Build échoué"
      fi

      curl -X POST -H 'Content-type: application/json' \
      --data "{
        \"username\": \"GitLab CI\",
        \"icon_emoji\": \":gitlab:\",
        \"attachments\": [{
          \"color\": \"$COLOR\",
          \"title\": \"$EMOJI $MESSAGE\",
          \"text\": \"Projet: $CI_PROJECT_NAME\nBranche: $CI_COMMIT_REF_NAME\nAuteur: $CI_COMMIT_AUTHOR\",
          \"fields\": [
            {\"title\": \"Commit\", \"value\": \"$CI_COMMIT_SHORT_SHA\", \"short\": true},
            {\"title\": \"Pipeline\", \"value\": \"#$CI_PIPELINE_ID\", \"short\": true}
          ],
          \"actions\": [{
            \"type\": \"button\",
            \"text\": \"Voir le pipeline\",
            \"url\": \"$CI_PIPELINE_URL\"
          }]
        }]
      }" $SLACK_WEBHOOK_URL
  when: always
```

### Notifications Discord

```yaml
notify:discord:
  stage: .post
  script:
    - |
      STATUS_EMOJI=$([ "$CI_JOB_STATUS" == "success" ] && echo "✅" || echo "❌")
      STATUS_COLOR=$([ "$CI_JOB_STATUS" == "success" ] && echo "3066993" || echo "15158332")

      curl -H "Content-Type: application/json" \
      -d "{
        \"embeds\": [{
          \"title\": \"$STATUS_EMOJI Pipeline $CI_JOB_STATUS\",
          \"description\": \"**Projet:** $CI_PROJECT_NAME\n**Branche:** $CI_COMMIT_REF_NAME\n**Commit:** $CI_COMMIT_SHORT_SHA\",
          \"url\": \"$CI_PIPELINE_URL\",
          \"color\": $STATUS_COLOR,
          \"fields\": [
            {\"name\": \"Auteur\", \"value\": \"$CI_COMMIT_AUTHOR\", \"inline\": true},
            {\"name\": \"Pipeline\", \"value\": \"#$CI_PIPELINE_ID\", \"inline\": true}
          ],
          \"timestamp\": \"$(date -u +%Y-%m-%dT%H:%M:%S.000Z)\"
        }]
      }" $DISCORD_WEBHOOK_URL
  when: always
```

### Notifications email personnalisées

```yaml
notify:email:
  stage: .post
  script:
    - |
      apt-get update && apt-get install -y mailutils

      if [ "$CI_JOB_STATUS" == "success" ]; then
        SUBJECT="✅ Build réussi - $CI_PROJECT_NAME"
      else
        SUBJECT="❌ Build échoué - $CI_PROJECT_NAME"
      fi

      echo "Pipeline: $CI_PIPELINE_URL
      Branche: $CI_COMMIT_REF_NAME
      Commit: $CI_COMMIT_SHORT_SHA
      Auteur: $CI_COMMIT_AUTHOR" | \
      mail -s "$SUBJECT" dev-team@example.com
  when: always
  only:
    - main
```

---

## Notifications avec Jenkins

### Configuration Email (Email Extension Plugin)

```groovy
pipeline {
    agent any

    stages {
        stage('Build') {
            steps {
                sh 'gcc -o app main.c'
            }
        }
    }

    post {
        success {
            emailext(
                subject: "✅ Build réussi - ${env.JOB_NAME} #${env.BUILD_NUMBER}",
                body: """
                    <h2>Build réussi</h2>
                    <p><strong>Job:</strong> ${env.JOB_NAME}</p>
                    <p><strong>Build:</strong> #${env.BUILD_NUMBER}</p>
                    <p><strong>Durée:</strong> ${currentBuild.durationString}</p>
                    <p><a href="${env.BUILD_URL}">Voir le build</a></p>
                """,
                to: 'dev-team@example.com',
                mimeType: 'text/html'
            )
        }

        failure {
            emailext(
                subject: "❌ Build échoué - ${env.JOB_NAME} #${env.BUILD_NUMBER}",
                body: """
                    <h2 style="color: red;">Build échoué</h2>
                    <p><strong>Job:</strong> ${env.JOB_NAME}</p>
                    <p><strong>Build:</strong> #${env.BUILD_NUMBER}</p>
                    <p><strong>Durée:</strong> ${currentBuild.durationString}</p>
                    <p><a href="${env.BUILD_URL}console">Voir les logs</a></p>
                """,
                to: 'dev-team@example.com',
                mimeType: 'text/html'
            )
        }
    }
}
```

### Notifications Slack (Slack Plugin)

```groovy
post {
    success {
        slackSend(
            channel: '#dev',
            color: 'good',
            message: "✅ Build réussi: ${env.JOB_NAME} #${env.BUILD_NUMBER}\n<${env.BUILD_URL}|Voir le build>"
        )
    }

    failure {
        slackSend(
            channel: '#dev',
            color: 'danger',
            message: "❌ Build échoué: ${env.JOB_NAME} #${env.BUILD_NUMBER}\n<${env.BUILD_URL}console|Voir les logs>"
        )
    }
}
```

### Notifications Discord

```groovy
post {
    always {
        script {
            def status = currentBuild.result ?: 'SUCCESS'
            def emoji = status == 'SUCCESS' ? '✅' : '❌'
            def color = status == 'SUCCESS' ? '3066993' : '15158332'

            sh """
                curl -H "Content-Type: application/json" \
                -d '{
                  "embeds": [{
                    "title": "${emoji} Build ${status}",
                    "description": "**Job:** ${env.JOB_NAME}\\n**Build:** #${env.BUILD_NUMBER}",
                    "url": "${env.BUILD_URL}",
                    "color": ${color}
                  }]
                }' ${env.DISCORD_WEBHOOK_URL}
            """
        }
    }
}
```

---

## Rapports de tests

### GitHub Actions - JUnit Reports

```yaml
- name: Exécuter les tests
  run: |
    cd build
    ctest --output-junit test-results.xml

- name: Publier les résultats de tests
  if: always()
  uses: dorny/test-reporter@v1
  with:
    name: Test Results
    path: build/test-results.xml
    reporter: java-junit
    fail-on-error: true
```

**Résultat** : Tests affichés dans l'interface GitHub avec statistiques détaillées.

### GitLab CI - JUnit Reports

```yaml
test:
  stage: test
  script:
    - cd build && ctest --output-junit test-results.xml
  artifacts:
    when: always
    reports:
      junit: build/test-results.xml
```

**Résultat** : Tests affichés dans l'onglet "Tests" du pipeline avec graphiques.

### Jenkins - JUnit Reports

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

**Résultat** : Graphiques de tendance des tests dans Jenkins.

---

## Rapports de couverture de code

### GitHub Actions - Codecov

```yaml
- name: Générer la couverture
  run: |
    cmake -B build -DCMAKE_C_FLAGS="--coverage"
    cmake --build build
    cd build && ctest
    lcov --capture --directory . --output-file coverage.info

- name: Uploader vers Codecov
  uses: codecov/codecov-action@v4
  with:
    files: build/coverage.info
    token: ${{ secrets.CODECOV_TOKEN }}
    fail_ci_if_error: true
```

### GitLab CI - Coverage native

```yaml
test:coverage:
  stage: test
  script:
    - cmake -B build -DCMAKE_C_FLAGS="--coverage"
    - cmake --build build
    - cd build && ctest
    - lcov --capture --directory . --output-file coverage.info
    - lcov --list coverage.info
  coverage: '/lines\.*: (\d+\.\d+)%/'
```

**Résultat** : Pourcentage de couverture affiché dans l'interface GitLab.

### Jenkins - Cobertura Plugin

```groovy
stage('Coverage') {
    steps {
        sh '''
            cmake -B build -DCMAKE_C_FLAGS="--coverage"
            cmake --build build
            cd build && ctest
            lcov --capture --directory . --output-file coverage.info
            genhtml coverage.info --output-directory coverage-html
        '''
    }
    post {
        always {
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

---

## Rapports d'analyse statique

### clang-tidy dans GitHub Actions

```yaml
- name: Analyse statique
  run: |
    cmake -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
    clang-tidy -p build src/*.c --format-style=file > clang-tidy-report.txt || true

- name: Commenter la PR avec les résultats
  if: github.event_name == 'pull_request'
  uses: actions/github-script@v7
  with:
    script: |
      const fs = require('fs');
      const report = fs.readFileSync('clang-tidy-report.txt', 'utf8');

      if (report.trim()) {
        github.rest.issues.createComment({
          owner: context.repo.owner,
          repo: context.repo.repo,
          issue_number: context.issue.number,
          body: '## 🔍 Analyse statique clang-tidy\n\n```\n' + report + '\n```'
        });
      }
```

### Warnings Next Generation (Jenkins)

```groovy
stage('Static Analysis') {
    steps {
        sh '''
            clang-tidy -p build src/*.c > clang-tidy.log || true
            cppcheck --xml --xml-version=2 src/ 2> cppcheck.xml
        '''
    }
    post {
        always {
            recordIssues(
                tools: [
                    clangTidy(pattern: 'clang-tidy.log'),
                    cppCheck(pattern: 'cppcheck.xml')
                ]
            )
        }
    }
}
```

---

## Badges de statut

Les **badges** affichent l'état du projet directement dans le README.

### GitHub Actions

```markdown
![CI](https://github.com/username/repo/actions/workflows/ci.yml/badge.svg)

![CI](https://github.com/username/repo/actions/workflows/ci.yml/badge.svg?branch=main)
```

### GitLab CI

```markdown
[![pipeline status](https://gitlab.com/username/project/badges/main/pipeline.svg)](https://gitlab.com/username/project/-/commits/main)

[![coverage report](https://gitlab.com/username/project/badges/main/coverage.svg)](https://gitlab.com/username/project/-/commits/main)
```

### Jenkins

Utiliser le plugin "Embeddable Build Status Plugin" :

```markdown
[![Build Status](https://jenkins.example.com/buildStatus/icon?job=mon-projet)](https://jenkins.example.com/job/mon-projet/)
```

### Codecov

```markdown
[![codecov](https://codecov.io/gh/username/repo/branch/main/graph/badge.svg)](https://codecov.io/gh/username/repo)
```

### Badges personnalisés (shields.io)

```markdown
![Build](https://img.shields.io/badge/build-passing-brightgreen)
![Coverage](https://img.shields.io/badge/coverage-85%25-yellow)
![License](https://img.shields.io/badge/license-MIT-blue)
```

---

## Notifications intelligentes

### Notifier seulement sur certaines branches

**GitHub Actions** :
```yaml
- name: Notification (main uniquement)
  if: github.ref == 'refs/heads/main' && failure()
  uses: slackapi/slack-github-action@v1
  with:
    webhook-url: ${{ secrets.SLACK_WEBHOOK }}
    payload: '{"text": "❌ Build échoué sur main !"}'
```

**GitLab CI** :
```yaml
notify:
  script:
    - send_notification.sh
  only:
    - main
  when: on_failure
```

### Notifier seulement l'auteur du commit

**GitHub Actions** :
```yaml
- name: Notifier l'auteur
  if: failure()
  uses: actions/github-script@v7
  with:
    script: |
      github.rest.repos.createCommitComment({
        owner: context.repo.owner,
        repo: context.repo.repo,
        commit_sha: context.sha,
        body: '@${{ github.actor }} Ton commit a cassé le build ! 🔥'
      })
```

### Notifier sur changement d'état

Notifier seulement quand le build passe de "réussi" à "échoué" ou vice-versa :

**Jenkins** :
```groovy
post {
    changed {
        emailext(
            subject: "⚠️ État du build changé - ${env.JOB_NAME}",
            body: "Le build est maintenant: ${currentBuild.result}",
            to: 'dev-team@example.com'
        )
    }
}
```

### Rate limiting des notifications

Pour éviter le spam :

```yaml
- name: Notification avec rate limit
  if: failure()
  run: |
    LAST_NOTIF=$(cat last_notification.txt 2>/dev/null || echo 0)
    NOW=$(date +%s)

    # Notifier seulement si > 1 heure depuis dernière notification
    if [ $((NOW - LAST_NOTIF)) -gt 3600 ]; then
      curl -X POST $SLACK_WEBHOOK -d '{"text":"Build échoué"}'
      echo $NOW > last_notification.txt
    fi
```

---

## Rapports périodiques

### Rapport hebdomadaire (GitLab Scheduled Pipelines)

```yaml
weekly_report:
  stage: report
  script:
    - |
      echo "📊 Rapport hebdomadaire" > report.txt
      echo "Builds réussis: $(get_success_count)" >> report.txt
      echo "Builds échoués: $(get_failure_count)" >> report.txt
      echo "Coverage moyenne: $(get_avg_coverage)" >> report.txt

      curl -X POST $SLACK_WEBHOOK \
        -d "{\"text\": \"$(cat report.txt)\"}"
  only:
    - schedules
```

### Rapport mensuel (GitHub Actions Scheduled)

```yaml
name: Rapport Mensuel

on:
  schedule:
    - cron: '0 9 1 * *'  # 1er du mois à 9h

jobs:
  report:
    runs-on: ubuntu-latest
    steps:
    - name: Générer le rapport
      run: |
        # Récupérer les statistiques via l'API GitHub
        curl -H "Authorization: token ${{ secrets.GITHUB_TOKEN }}" \
          "https://api.github.com/repos/${{ github.repository }}/actions/runs" \
          | jq '.workflow_runs | group_by(.conclusion) | map({conclusion: .[0].conclusion, count: length})' \
          > stats.json

    - name: Envoyer le rapport
      run: |
        # Formater et envoyer par email
        python format_report.py stats.json | \
        mail -s "📊 Rapport mensuel CI/CD" dev-team@example.com
```

---

## Dashboard et visualisation

### GitHub Actions Dashboard

Créer un workflow qui génère un dashboard HTML :

```yaml
name: Generate Dashboard

on:
  schedule:
    - cron: '0 */6 * * *'  # Toutes les 6 heures

jobs:
  dashboard:
    runs-on: ubuntu-latest
    steps:
    - name: Générer le dashboard
      run: |
        cat > index.html << EOF
        <!DOCTYPE html>
        <html>
        <head>
          <title>CI/CD Dashboard</title>
          <style>
            body { font-family: Arial, sans-serif; margin: 20px; }
            .metric { display: inline-block; padding: 20px; margin: 10px; background: #f0f0f0; border-radius: 8px; }
            .success { background: #d4edda; }
            .failure { background: #f8d7da; }
          </style>
        </head>
        <body>
          <h1>📊 CI/CD Dashboard</h1>
          <div class="metric success">
            <h2>✅ Builds réussis</h2>
            <p>95%</p>
          </div>
          <div class="metric">
            <h2>⏱️ Temps moyen</h2>
            <p>5m 32s</p>
          </div>
          <div class="metric success">
            <h2>📈 Coverage</h2>
            <p>87%</p>
          </div>
        </body>
        </html>
        EOF

    - name: Publier sur GitHub Pages
      uses: peaceiris/actions-gh-pages@v4
      with:
        github_token: ${{ secrets.GITHUB_TOKEN }}
        publish_dir: .
```

### Grafana + Prometheus (avancé)

Pour les grandes organisations, utiliser Grafana pour visualiser :
- Temps de build
- Taux de succès
- Coverage dans le temps
- Nombre de warnings

---

## Templates de notifications

### Template Slack riche

```json
{
  "username": "CI Bot",
  "icon_emoji": ":robot_face:",
  "attachments": [
    {
      "color": "#36a64f",
      "pretext": "Build terminé",
      "author_name": "John Doe",
      "author_icon": "https://github.com/johndoe.png",
      "title": "✅ Build #123 réussi",
      "title_link": "https://github.com/org/repo/actions/runs/123",
      "fields": [
        {
          "title": "Projet",
          "value": "mon-projet-c",
          "short": true
        },
        {
          "title": "Branche",
          "value": "main",
          "short": true
        },
        {
          "title": "Durée",
          "value": "3m 45s",
          "short": true
        },
        {
          "title": "Coverage",
          "value": "87%",
          "short": true
        }
      ],
      "footer": "GitHub Actions",
      "footer_icon": "https://github.githubassets.com/images/modules/logos_page/GitHub-Mark.png",
      "ts": 1234567890
    }
  ]
}
```

### Template Discord embed

```json
{
  "embeds": [
    {
      "title": "✅ Build réussi",
      "description": "Le build a été complété avec succès",
      "url": "https://github.com/org/repo/actions/runs/123",
      "color": 3066993,
      "fields": [
        {
          "name": "📦 Projet",
          "value": "mon-projet-c",
          "inline": true
        },
        {
          "name": "🌿 Branche",
          "value": "main",
          "inline": true
        },
        {
          "name": "⏱️ Durée",
          "value": "3m 45s",
          "inline": true
        },
        {
          "name": "📊 Coverage",
          "value": "87%",
          "inline": true
        },
        {
          "name": "✅ Tests",
          "value": "42/42 passés",
          "inline": true
        },
        {
          "name": "⚠️ Warnings",
          "value": "0",
          "inline": true
        }
      ],
      "author": {
        "name": "John Doe",
        "icon_url": "https://github.com/johndoe.png"
      },
      "footer": {
        "text": "GitHub Actions"
      },
      "timestamp": "2024-01-15T10:30:00.000Z"
    }
  ]
}
```

---

## Bonnes pratiques

### 1. Notifier intelligemment, pas spam

```yaml
# ✅ Bon : notifier seulement sur échec sur main
if: failure() && github.ref == 'refs/heads/main'

# ❌ Mauvais : notifier à chaque build
if: always()
```

### 2. Canaux appropriés

- **#dev-alerts** : Échecs critiques (main, production)
- **#dev-builds** : Tous les builds (info)
- **#dev-spam** : Tests, branches feature (optionnel)

### 3. Contenu pertinent

```json
{
  "text": "❌ Build échoué",
  "details": {
    "projet": "mon-app",
    "branche": "main",
    "erreur": "Tests échoués : 3/42",
    "auteur": "@johndoe",
    "commit": "abc123",
    "lien": "https://..."
  }
}
```

### 4. Distinguer sévérité

- 🔴 **Critique** : main cassé, déploiement échoué
- 🟡 **Avertissement** : Tests flaky, coverage baisse
- 🟢 **Info** : Build réussi, nouveau tag

### 5. Actions rapides

Inclure des boutons dans les notifications :
- "Voir les logs"
- "Relancer le build"
- "Créer une issue"

### 6. Rate limiting

```python
# Limiter à 1 notification / heure
if time.time() - last_notification > 3600:
    send_notification()
    last_notification = time.time()
```

### 7. Contextualiser

```
❌ Build échoué
━━━━━━━━━━━━━━━━━━
Projet: mon-projet-c  
Branche: feature/nouvelle-fonction  
Commit: abc123 "Ajouter nouvelle API"  
Auteur: @johndoe  
Erreur: Tests unitaires échoués (3/42)  
━━━━━━━━━━━━━━━━━━
🔗 Voir les logs
```

### 8. Notifications graduées

```yaml
# Première échec : avertissement
if: failure() && github.run_attempt == 1
  # Notification discrète

# Échec persistant : alerte
if: failure() && github.run_attempt > 2
  # Notification urgente + mention @channel
```

---

## Métriques à surveiller

### Métriques de build

| Métrique | Objectif | Action si hors objectif |
|----------|----------|------------------------|
| **Taux de succès** | > 95% | Enquêter sur les causes |
| **Temps de build** | < 10 min | Optimiser (cache, parallélisation) |
| **Temps de tests** | < 5 min | Paralléliser ou réduire |
| **Coverage** | > 80% | Ajouter des tests |
| **Warnings** | 0 | Corriger immédiatement |

### Alertes automatiques

```yaml
- name: Vérifier les métriques
  run: |
    BUILD_TIME=${{ steps.build.outputs.duration }}

    if [ $BUILD_TIME -gt 600 ]; then
      echo "⚠️ Build trop lent : ${BUILD_TIME}s > 600s"
      # Envoyer alerte
    fi

    COVERAGE=$(cat coverage.txt)
    if (( $(echo "$COVERAGE < 80" | bc -l) )); then
      echo "⚠️ Coverage trop basse : ${COVERAGE}% < 80%"
      # Envoyer alerte
    fi
```

---

## Debugging des notifications

### Tester localement

```bash
# Tester un webhook Slack
curl -X POST -H 'Content-type: application/json' \
  --data '{"text":"Test de notification"}' \
  https://hooks.slack.com/services/YOUR/WEBHOOK/URL

# Tester un webhook Discord
curl -H "Content-Type: application/json" \
  -d '{"content": "Test de notification"}' \
  https://discord.com/api/webhooks/YOUR/WEBHOOK
```

### Logs de debugging

```yaml
- name: Debug notification
  run: |
    echo "Event: ${{ github.event_name }}"
    echo "Ref: ${{ github.ref }}"
    echo "Status: ${{ job.status }}"
    echo "Webhook URL (first 20 chars): ${SLACK_WEBHOOK_URL:0:20}"
```

### Variables disponibles

**GitHub Actions** :
- `${{ github.actor }}` : Auteur du commit
- `${{ github.event_name }}` : push, pull_request, etc.
- `${{ job.status }}` : success, failure, cancelled
- `${{ github.sha }}` : Hash du commit

**GitLab CI** :
- `$CI_COMMIT_AUTHOR` : Auteur
- `$CI_PIPELINE_SOURCE` : push, merge_request, etc.
- `$CI_JOB_STATUS` : success, failed
- `$CI_COMMIT_SHA` : Hash du commit

---

## Résumé

**Les notifications gardent l'équipe informée, les rapports mesurent la qualité.**

**Types de notifications** :
- **Email** : Persistant, individuel
- **Slack/Teams/Discord** : Temps réel, équipe
- **GitHub/GitLab** : Intégré, contextualisé

**Rapports essentiels** :
- Tests (JUnit)
- Couverture (Codecov, lcov)
- Analyse statique (clang-tidy, cppcheck)
- Métriques (temps, taux de succès)

**Bonnes pratiques** :
- Notifier intelligemment (pas spam)
- Canaux appropriés par sévérité
- Contenu pertinent et actionnable
- Inclure des liens directs
- Rate limiting
- Badges dans le README

**Configuration minimale** :
```yaml
# GitHub Actions
- name: Notification Slack
  if: failure()
  run: |
    curl -X POST $SLACK_WEBHOOK \
      -d '{"text":"❌ Build échoué"}'
```

---

## Prochaines étapes

Maintenant que vous maîtrisez les notifications et reporting, vous pouvez :

- Configurer des notifications pour votre équipe
- Ajouter des badges dans votre README
- Mettre en place des rapports de tests/couverture
- Créer des dashboards personnalisés
- Configurer des alertes sur métriques critiques

---

## Ressources complémentaires

- [Slack Incoming Webhooks](https://api.slack.com/messaging/webhooks)
- [Discord Webhooks Guide](https://discord.com/developers/docs/resources/webhook)
- [GitHub Actions Status Badge](https://docs.github.com/en/actions/monitoring-and-troubleshooting-workflows/adding-a-workflow-status-badge)
- [GitLab CI Badges](https://docs.gitlab.com/ee/user/project/badges.html)
- [Codecov Documentation](https://docs.codecov.com/)

---

**🎯 Points clés à retenir :**

- ✅ Notifications = feedback immédiat sur l'état du build
- ✅ Rapports = mesure de la qualité dans le temps
- ✅ Notifier intelligemment : échecs critiques, pas spam
- ✅ Canaux appropriés : Slack/Teams pour équipe, email pour individuel
- ✅ Rapports essentiels : tests (JUnit), couverture, analyse statique
- ✅ Badges dans README pour visibilité publique
- ✅ Métriques à surveiller : taux de succès, temps, coverage
- ✅ Templates riches avec contexte et actions rapides

⏭️ [Cross-compilation et Déploiement](/31-cross-compilation-deploiement/README.md)
