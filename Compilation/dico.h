#ifndef __DICO__
#define __DICO__

#include "syntabs.h"

#define maxDico 100

#define C_VARIABLE_GLOBALE 1
#define C_VARIABLE_LOCALE 2
#define C_ARGUMENT 3

#define T_ENTIER 1
#define T_TABLEAU_ENTIER 2
#define T_FONCTION 3

typedef struct {
  char *identif;
  int classe; // portée
  int type;
  int adresse;
  int complement; // taille d'un tableau, nombre d'arguments d'une fonction
} desc_identif;

typedef struct {
  desc_identif tab[maxDico];
  int base;
  int sommet;
} dico_;

void affiche_n_prog(n_prog *n);
void affiche_l_instr(n_l_instr *n);
void affiche_instr(n_instr *n);
void affiche_instr_si(n_instr *n);
void affiche_instr_tantque(n_instr *n);
void affiche_instr_faire(n_instr *n);      /* MODIFIE POUR EVAL */
void affiche_instr_pour(n_instr *n);       /* MODIFIE POUR EVAL */
void affiche_instr_affect(n_instr *n);
void affiche_instr_appel(n_instr *n);
void affiche_instr_retour(n_instr *n);
void affiche_instr_ecrire(n_instr *n);
void affiche_l_exp(n_l_exp *n);
void affiche_exp(n_exp *n);
void affiche_varExp(n_exp *n);
void affiche_opExp(n_exp *n);
void affiche_intExp(n_exp *n);
void affiche_lireExp(n_exp *n);
void affiche_appelExp(n_exp *n);
void affiche_l_dec(n_l_dec *n);
void affiche_dec(n_dec *n);
void affiche_foncDec(n_dec *n);
void affiche_varDec(n_dec *n);
void affiche_tabDec(n_dec *n);
void affiche_var(n_var *n);
void affiche_var_simple(n_var *n);
void affiche_var_indicee(n_var *n);
void affiche_appel(n_appel *n);

void entreeFonction(void);
void sortieFonction(void);
int ajouteIdentificateur(char *identif,  int classe, int type, int adresse, int complement);
int rechercheExecutable(char *identif);
int rechercheDeclarative(char *identif);
void affiche_dico(void);

dico_ dico;
#endif
