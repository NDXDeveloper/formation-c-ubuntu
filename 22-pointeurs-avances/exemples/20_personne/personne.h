/* ============================================================================
   Section 22.4 : Pointeurs opaques
   Description : Interface publique du type opaque Personne (header)
   Fichier source : 04-pointeurs-opaques.md
   ============================================================================ */

#ifndef PERSONNE_H
#define PERSONNE_H

// Declaration forward : on dit que la structure existe,
// mais on ne revele pas son contenu
typedef struct Personne Personne;

// Fonctions publiques (API)
Personne* creer_personne(const char *nom, const char *prenom, int age);
void definir_salaire(Personne *p, float salaire);
float obtenir_salaire(const Personne *p);
int obtenir_age(const Personne *p);
void afficher_personne(const Personne *p);
void liberer_personne(Personne *p);

#endif
