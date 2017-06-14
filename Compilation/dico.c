#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include "dico.h"
#include "util.h"

int contexte;
int retour;
int adresseLocaleCourante;
int adresseArgumentCourant;
int reg_tab_free[8] = {0};
char *reg_tab[8] = {"$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7"};
int var_loc = 0;
int branchCounter = 0;
int trace_abs = 0;
int nb_param = 0;
/*-------------------------------------------------------------------------*/
void empile( char *reg ){
	printf( "\tsubi\t$sp, $sp, 4\t# alloue un mot sur la pile\n" );
	printf( "\tsw\t%s, ($sp)\t# copie reg vers sommet de pile\n", reg );
}
void depile( char *reg ){
	printf( "\tlw\t%s, ($sp)\t# copie sommet de pile vers reg\n", reg );
	printf( "\taddi\t$sp, $sp, 4\t# desalloue un mot sur la pile\n" );
}

char * new_register(){

	for(int i = 0; i <= 8; i++){
		if(reg_tab_free[i] == 0){
			reg_tab_free[i] = 1;
			return reg_tab[i];
		}
	}
	exit(-1);

}

void liberer_registre(char * registre){

	for(int i = 0; i <= 8; i++)
		if(registre == reg_tab[i])
			reg_tab_free[i] = 0;
}

void affiche_n_prog(n_prog *n)
{
  char *fct = "prog";
  contexte = C_VARIABLE_GLOBALE;
  affiche_balise_ouvrante(fct, trace_abs);
  printf("\t.data\n");
  affiche_l_dec(n->variables);
  printf("\n");
  printf("\t.text\n");
  printf("__start:\n");
  printf("\tjal main\n");
  printf("\tli $v0, 10\n");
  printf("\tsyscall\n");
  affiche_l_dec(n->fonctions); 
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/
/*-------------------------------------------------------------------------*/

void affiche_l_instr(n_l_instr *n)
{
  char *fct = "l_instr";
  if(n){
	  affiche_balise_ouvrante(fct, trace_abs);
	  affiche_instr(n->tete);
	  affiche_l_instr(n->queue);
	  affiche_balise_fermante(fct, trace_abs);
  }
}

/*-------------------------------------------------------------------------*/

void affiche_instr(n_instr *n)
{
  if(n){
	if(n->type == blocInst) affiche_l_instr(n->u.liste);
	else if(n->type == affecteInst) affiche_instr_affect(n);
	else if(n->type == siInst) affiche_instr_si(n);
	else if(n->type == tantqueInst) affiche_instr_tantque(n);
	else if(n->type == faireInst) affiche_instr_faire(n);
	else if(n->type == pourInst) affiche_instr_pour(n);    
	else if(n->type == appelInst) affiche_instr_appel(n);
	else if(n->type == retourInst) affiche_instr_retour(n);
	else if(n->type == ecrireInst) affiche_instr_ecrire(n);
  }
}

/*-------------------------------------------------------------------------*/

void affiche_instr_si(n_instr *n)
{  
  char *fct = "instr_si";
  char * registre;
  int etiquette,etiquette_sinon;
  affiche_balise_ouvrante(fct, trace_abs);

  affiche_exp(n->u.si_.test);
  
  
  if(n->u.si_.sinon){
	etiquette = branchCounter;
	branchCounter++;
	etiquette_sinon = branchCounter;
	branchCounter++;
  	registre = new_register();
	depile(registre);
	printf("\tbeq %s, $zero, e%d\n",registre,etiquette_sinon);
	liberer_registre(registre);
	affiche_instr(n->u.si_.alors);
	printf("\tj e%d\n",etiquette);
	printf("e%d :\n",etiquette_sinon);
	affiche_instr(n->u.si_.sinon);
	printf("e%d :\n",etiquette);
  }
  else{
	etiquette = branchCounter;
	branchCounter++;
	registre = new_register();
	depile(registre);
	printf("\tbeq %s, $zero, e%d\n",registre,etiquette);
	liberer_registre(registre);
	affiche_instr(n->u.si_.alors);
	printf("e%d :\n",etiquette);
 }
 
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_instr_tantque(n_instr *n)
{
  char *fct = "instr_tantque";
  char * registre;
  int debutBoucle, newBoucle;
  affiche_balise_ouvrante(fct, trace_abs);
  printf("e%d : \n",branchCounter);
  debutBoucle = branchCounter;
  branchCounter++;
  affiche_exp(n->u.tantque_.test);
  registre = new_register();
  depile(registre);
  printf("\tbeq %s, $zero, e%d\n",registre,branchCounter);
  liberer_registre(registre);
  newBoucle = branchCounter;
  branchCounter++;
  affiche_instr(n->u.tantque_.faire);
  printf("\tj e%d\n", debutBoucle);
  printf("e%d:\n",newBoucle);
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_instr_faire(n_instr *n)          /* MODIFIE POUR EVAL */
{                                             /* MODIFIE POUR EVAL */
  char *fct = "instr_faire";                  /* MODIFIE POUR EVAL */
  affiche_balise_ouvrante(fct, trace_abs);    /* MODIFIE POUR EVAL */
  affiche_instr(n->u.faire_.faire);           /* MODIFIE POUR EVAL */
  affiche_exp(n->u.faire_.test);              /* MODIFIE POUR EVAL */
  affiche_balise_fermante(fct, trace_abs);    /* MODIFIE POUR EVAL */
}                                             /* MODIFIE POUR EVAL */

/*-------------------------------------------------------------------------*/

void affiche_instr_pour(n_instr *n)                /* MODIFIE POUR EVAL */
{                                                  /* MODIFIE POUR EVAL */
  char *fct = "instr_pour";                        /* MODIFIE POUR EVAL */
  affiche_balise_ouvrante(fct, trace_abs);         /* MODIFIE POUR EVAL */
  affiche_instr(n->u.pour_.init);                  /* MODIFIE POUR EVAL */
  affiche_exp(n->u.pour_.test);                    /* MODIFIE POUR EVAL */
  affiche_instr(n->u.pour_.faire);                 /* MODIFIE POUR EVAL */
  affiche_instr(n->u.pour_.incr);                  /* MODIFIE POUR EVAL */
  affiche_balise_fermante(fct, trace_abs);         /* MODIFIE POUR EVAL */
}                                                  /* MODIFIE POUR EVAL */

/*-------------------------------------------------------------------------*/

void affiche_instr_affect(n_instr *n)
{
  char *fct = "instr_affect";
  affiche_balise_ouvrante(fct, trace_abs);
  
  //affiche_var(n->u.affecte_.var);
  int adresse;
  affiche_exp(n->u.affecte_.exp);
    adresse = rechercheExecutable(n->u.affecte_.var->nom);
  if(dico.tab[adresse].classe == C_VARIABLE_GLOBALE){
	  if(n->u.affecte_.var->type != indicee){
		char * registre  = new_register();
		depile(registre);
		printf("\tsw %s, %s\n",registre,n->u.affecte_.var->nom);
		liberer_registre(registre);
	  }
	  else{
		affiche_var(n->u.affecte_.var);
			char * indice = new_register();
		depile(indice);
		printf("\tadd %s, %s, %s\n",indice,indice,indice);
		printf("\tadd %s, %s, %s\n",indice,indice,indice);
		char * registre  = new_register();
		depile(registre);
		printf("\tsw %s, %s(%s)\n",registre,n->u.affecte_.var->nom,indice);
		liberer_registre(indice);
		liberer_registre(registre);
	  }
  }
  if(dico.tab[adresse].classe == C_VARIABLE_LOCALE){
		char * registre  = new_register();
		depile(registre);
		printf("\tsw %s, %d($fp)\n",registre,-8-dico.tab[adresse].adresse);
		liberer_registre(registre);
  }
  if(dico.tab[adresse].classe == C_ARGUMENT){
		char * registre  = new_register();
		depile(registre);
		printf("\tsw %s, %d($fp)\n",registre, - 4 * nb_param - dico.tab[adresse].adresse);
		liberer_registre(registre);
  }
  
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_instr_appel(n_instr *n)
{
	int adresse;
	retour = 0;
  char *fct = "instr_appel";
  affiche_balise_ouvrante(fct, trace_abs);
  printf("\tsubi \t$sp $sp 4\t# allocation valeur de retour\n");
  affiche_appel(n->u.appel);
  
  printf("\tjal \t%s\n", n->u.appel->fonction);
  adresse =  rechercheExecutable(n->u.appel->fonction);
		printf("\taddi $sp, $sp, 4 #valeur de retour ignoré\n");
  if(dico.tab[adresse].complement > 0)
		printf("\taddi $sp, $sp, %d #desallocation parametre\n",4*dico.tab[adresse].complement);
retour = 0;
  affiche_balise_fermante(fct, trace_abs);
}

void affiche_appel(n_appel *n)
{

	if(!rechercheDeclarative(n->fonction))
		erreur("fonction non declarée\n");	
	char *fct = "appel";
	affiche_balise_ouvrante(fct, trace_abs);
	affiche_texte( n->fonction, trace_abs);
	contexte = C_ARGUMENT;
	affiche_l_exp(n->args);
	affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_instr_retour(n_instr *n)
{
  char *fct = "instr_retour";
  char * registre;
  affiche_balise_ouvrante(fct, trace_abs);
  affiche_exp(n->u.retour_.expression);
registre = new_register();

depile(registre);
printf("\tsw %s, %d($fp)\n",registre,4*( nb_param + 1)); 
  depile("$ra");
  depile("$fp");
  printf("\tjr $ra\n");
  retour = 1;
  liberer_registre(registre);
  affiche_balise_fermante(fct, trace_abs);

}

/*-------------------------------------------------------------------------*/

void affiche_instr_ecrire(n_instr *n)
{
  char *fct = "instr_ecrire";
  affiche_balise_ouvrante(fct, trace_abs);
  affiche_exp(n->u.ecrire_.expression);
  depile("$a0");
  printf("\tli $v0, 1\n");
  printf("\tsyscall\n");

	printf("\tli	$a0, '\\n'\n");
	printf("\tli	$v0, 11\n");
	printf("\tsyscall #ecrire char\n");
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_l_exp(n_l_exp *n)
{
  char *fct = "l_exp";
  affiche_balise_ouvrante(fct, trace_abs);
  if(n != NULL){
	affiche_exp(n->tete);
	affiche_l_exp(n->queue);
  }
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_exp(n_exp *n)
{

  if(n->type == varExp) affiche_varExp(n);
  else if(n->type == opExp) affiche_opExp(n);
  else if(n->type == intExp) affiche_intExp(n);
  else if(n->type == appelExp) affiche_appelExp(n);
  else if(n->type == lireExp) affiche_lireExp(n);
}

/*-------------------------------------------------------------------------*/

void affiche_varExp(n_exp *n)
{
 int adresse;
  char *fct = "varExp";
  affiche_balise_ouvrante(fct, trace_abs);
  char * reg;
  adresse = rechercheExecutable(n->u.var->nom);
  if(dico.tab[adresse].classe == C_VARIABLE_GLOBALE){
		if(n->u.var->type != indicee){
			reg = new_register();
			printf("\tlw %s, %s \n",reg,n->u.var->nom);	
			affiche_var(n->u.var);
			empile(reg);
			liberer_registre(reg);
		}
		else{
			affiche_var(n->u.var);
			char * indice = new_register();
			depile(indice);
			printf("\tadd %s, %s, %s\n",indice,indice,indice);
			printf("\tadd %s, %s, %s\n",indice,indice,indice);
			reg  = new_register();
			printf("\tlw %s, %s(%s)\n",reg,n->u.var->nom,indice);
			liberer_registre(indice);
			empile(reg);
			liberer_registre(reg);
		}
	}
	if(dico.tab[adresse].classe == C_VARIABLE_LOCALE){
		reg = new_register();
		printf("\tlw %s, %d($fp) \n",reg,-8-dico.tab[adresse].adresse);	
		affiche_var(n->u.var);
		empile(reg);
		liberer_registre(reg);
	}
	if(dico.tab[adresse].classe == C_ARGUMENT){
		reg = new_register();
		printf("\tlw %s,%d($fp) \n",reg, 4 * nb_param - dico.tab[adresse].adresse);	
		//printf("nb_param %d \n",nb_param);
		affiche_var(n->u.var);
		empile(reg);
		liberer_registre(reg);
	}
  
affiche_balise_fermante(fct, trace_abs);  
}

/*-------------------------------------------------------------------------*/
void affiche_opExp(n_exp *n)
{
  char *fct = "opExp";
  char * op1;
  char * op2;
  char * result;
  affiche_balise_ouvrante(fct, trace_abs);

  if( n->u.opExp_.op1 != NULL ) {
	affiche_exp(n->u.opExp_.op1);
  }
  if( n->u.opExp_.op2 != NULL ) {
	affiche_exp(n->u.opExp_.op2);
  }

   if(n->u.opExp_.op == plus){
    op1 = new_register();
	op2 = new_register();   
	depile(op2);
	depile(op1);
 	result = new_register();
	printf("\tadd %s, %s, %s\n",result, op1,op2);
	empile(result);
	liberer_registre(op1);
	liberer_registre(op2);
	liberer_registre(result);  
   }
  else if(n->u.opExp_.op == moins){
    op1 = new_register();
	op2 = new_register();
	depile(op2);
	depile(op1);
  	result = new_register();
	printf("\tsub %s, %s, %s\n",result, op1,op2);
	empile(result);
	liberer_registre(op1);
	liberer_registre(op2);
	liberer_registre(result);
  }
  else if(n->u.opExp_.op == fois){
    op1 = new_register();
	op2 = new_register();
	depile(op2);
	depile(op1);
	result = new_register();
	printf("\tmult %s, %s\n",op1,op2);
	printf("\tmflo %s\n",result);
	empile(result);
	liberer_registre(op1);
	liberer_registre(op2);
	liberer_registre(result);
  }
  else if(n->u.opExp_.op == divise){
    op1 = new_register();
	op2 = new_register();
	depile(op2);
	depile(op1);
  	result = new_register();
	printf("\tdiv %s, %s\n",op1,op2);
	printf("\tmflo %s\n",result);
	empile(result);
	liberer_registre(op1);
	liberer_registre(op2);
	liberer_registre(result);
  }
  else if(n->u.opExp_.op == egal){
	op1 = new_register();
	op2 = new_register();
	depile(op2);
	depile(op1);
  	result = new_register();
	printf("\tli %s, -1 #egal\n",result);
	printf("\tbeq %s, %s, e%d\n",op1,op2,branchCounter);
	printf("\tli %s, 0\n",result);
	printf("e%d :\n",branchCounter);
	branchCounter++;
	empile(result);
	liberer_registre(op1);
	liberer_registre(op2);
	liberer_registre(result);
  }
  else if(n->u.opExp_.op == diff){
  	op1 = new_register();
	op2 = new_register();
	depile(op2);
	depile(op1);
  	result = new_register();
	printf("\tli %s, -1 #diff\n",result);
	printf("\tbne %s, %s, e%d\n",op1,op2,branchCounter);
	printf("\tli %s, 0\n",result);
	printf("e%d :\n",branchCounter);
	branchCounter++;
	empile(result);
	liberer_registre(op1);
	liberer_registre(op2);
	liberer_registre(result);
  }
  else if(n->u.opExp_.op == inf){
	  	op1 = new_register();
	op2 = new_register();
	depile(op2);
	depile(op1);
  	result = new_register();
	printf("\tli %s, -1 #inf\n",result);
	printf("\tblt %s, %s, e%d\n",op1,op2,branchCounter);
	printf("\tli %s, 0\n",result);
	printf("e%d :\n",branchCounter);
	branchCounter++;
	empile(result);
	liberer_registre(op1);
	liberer_registre(op2);
	liberer_registre(result);
  }
  else if(n->u.opExp_.op == infeg){
  	op1 = new_register();
	op2 = new_register();
	depile(op2);
	depile(op1);
  	result = new_register();
	printf("\tli %s, -1 #infeg\n",result);
	printf("\tble %s, %s, e%d\n",op1,op2,branchCounter);
	printf("\tli %s, 0\n",result);
	printf("e%d :\n",branchCounter);
	branchCounter++;
	empile(result);
	liberer_registre(op1);
	liberer_registre(op2);
	liberer_registre(result);
  }
  else if(n->u.opExp_.op == ou){
    op1 = new_register();
	op2 = new_register();
	depile(op2);
	depile(op1);
  	result = new_register();
	printf("\tand %s, %s, %s\n",result,op1,op2);
	empile(result);
	liberer_registre(op1);
	liberer_registre(op2);
	liberer_registre(result);
  }
  else if(n->u.opExp_.op == et){
    op1 = new_register();
	op2 = new_register();
	depile(op2);
	depile(op1);
  	result = new_register();
	printf("\tand %s, %s, %s\n",result,op1,op2);
	empile(result);
	liberer_registre(op1);
	liberer_registre(op2);
	liberer_registre(result);
  }
  else if(n->u.opExp_.op == non){
	op1 = new_register();
	depile(op1);
	result = new_register();
	printf("\t li %s, -1 #Negation = Xor(Registre,-1)\n ", result);
	printf("\t xor %s, %s, %s\n",result, op1, result);
	empile(result);
	liberer_registre(op1);
	liberer_registre(result);
  }

 
  
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_intExp(n_exp *n)
{
	char * registre = new_register();
  char texte[ 50 ]; // Max. 50 chiffres
  sprintf(texte, "%d", n->u.entier);
  printf("\tli %s, %d\n",registre,n->u.entier);
  empile(registre);
  liberer_registre(registre);
  affiche_element( "intExp", texte, trace_abs );
}

/*-------------------------------------------------------------------------*/
void affiche_lireExp(n_exp *n)
{
  char *fct = "lireExp";
  affiche_balise_ouvrante(fct, trace_abs);
  printf("\tli $v0, 5\n");
  printf("\tsyscall\n");
  empile("$v0");
  affiche_balise_fermante(fct, trace_abs);

}

/*-------------------------------------------------------------------------*/

void affiche_appelExp(n_exp *n)
{
int adresse;
retour = 0;
  char *fct = "appelExp";
  affiche_balise_ouvrante(fct, trace_abs);
    printf("\tsubi \t$sp $sp 4\t# allocation valeur de retour\n");
  affiche_appel(n->u.appel);
    printf("\tjal \t%s\n", n->u.appel->fonction);
	  adresse =  rechercheExecutable(n->u.appel->fonction);
	  if(retour == 1)
		printf("\taddi $sp, $sp, 4 #valeur de retour ignoré\n");
	 if(dico.tab[adresse].complement > 0)
		printf("\taddi $sp, $sp, %d #desallocation parametre\n",4*dico.tab[adresse].complement);
		
	retour = 0;
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_l_dec(n_l_dec *n)
{
  char *fct = "l_dec";

  if( n ){
	affiche_balise_ouvrante(fct, trace_abs);
	affiche_dec(n->tete);
	affiche_l_dec(n->queue);
	affiche_balise_fermante(fct, trace_abs);
  }
}

/*-------------------------------------------------------------------------*/

void affiche_dec(n_dec *n)
{

  if(n){
	if(n->type == foncDec) {
	  affiche_foncDec(n);
	}
	else if(n->type == varDec) {
	  affiche_varDec(n);
	}
	else if(n->type == tabDec) { 
	  affiche_tabDec(n);
	}
  }
}

/*-------------------------------------------------------------------------*/

void affiche_foncDec(n_dec *n)
{
  char *fct = "foncDec";
  affiche_balise_ouvrante(fct, trace_abs);
  affiche_texte( n->nom, trace_abs );
  
  if(rechercheDeclarative(n->nom) != -1)
  erreur("function redifinition\n");
  n_dec * new = malloc(50);
  memcpy(new,n,50);
  nb_param = 0;
  while(new->u.foncDec_.param != NULL){
	new->u.foncDec_.param = new->u.foncDec_.param->queue;
	nb_param++;
  }
  
  ajouteIdentificateur(n->nom, contexte, T_FONCTION,0,nb_param);
  entreeFonction();
  printf("%s:\n",n->nom);
  empile("$fp");
  printf("\tmove \t$fp, $sp\n");
  empile("$ra");
  contexte = C_ARGUMENT;
  affiche_l_dec(n->u.foncDec_.param);
  contexte = C_VARIABLE_LOCALE;
  affiche_l_dec(n->u.foncDec_.variables);
  if(var_loc != 0)
    printf("\tsubi \t$sp, $sp, %d \t#allocation variables locales\n", var_loc*4);
  affiche_instr(n->u.foncDec_.corps);
  
  sortieFonction();
  if(var_loc > 0)
	printf("\taddi $sp, $sp, %d\n",var_loc*4);	
  depile("$ra");
  depile("$fp");
  printf("\tjr $ra\n");
  
  
  var_loc = 0;
  affiche_balise_fermante(fct, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_varDec(n_dec *n)
{
	if(contexte == C_VARIABLE_GLOBALE){
		printf("%s: .space 4\n",n->nom);
		ajouteIdentificateur(n->nom, contexte, T_ENTIER,dico.base,-1);
		dico.base +=4;
	}
	if(contexte == C_VARIABLE_LOCALE){
		ajouteIdentificateur(n->nom, contexte, T_ENTIER,adresseLocaleCourante,-1);
		adresseLocaleCourante += 4;
		var_loc++;
	}
	if(contexte == C_ARGUMENT){
		ajouteIdentificateur(n->nom, contexte, T_ENTIER,adresseArgumentCourant,-1);
		adresseArgumentCourant += 4;
	}
  
  affiche_element("varDec", n->nom, trace_abs);
}

/*-------------------------------------------------------------------------*/

void affiche_tabDec(n_dec *n)
{
  char texte[100]; // Max. 100 chars nom tab + taille
  sprintf(texte, "%s[%d]", n->nom, n->u.tabDec_.taille);
	  printf("%s: .space %d\n",n->nom, 4*n->u.tabDec_.taille);
  affiche_element( "tabDec", texte, trace_abs );
  
 ajouteIdentificateur(n->nom, C_VARIABLE_GLOBALE, T_TABLEAU_ENTIER,dico.base++,n->u.tabDec_.taille);
  
}

/*-------------------------------------------------------------------------*/

void affiche_var(n_var *n)
{
  if(n->type == simple) {
	affiche_var_simple(n);
  }
  else if(n->type == indicee) {
	affiche_var_indicee(n);
  }
}

/*-------------------------------------------------------------------------*/
void affiche_var_simple(n_var *n)
{
  int position = rechercheExecutable(n->nom);
  if(position == -1)
		erreur("Variable simple non declaré\n");
  if(dico.tab[position].complement != -1)
		erreur("Tableau utilisé comme une variable simple\n");
  affiche_element("var_simple", n->nom, trace_abs);
}

/*-------------------------------------------------------------------------*/
void affiche_var_indicee(n_var *n)
{
	int position = rechercheExecutable(n->nom);
	if(position == -1)
		erreur("Variable indicée non declaré\n");
	if(dico.tab[position].complement == -1)
		erreur("Variable simple utilisé comme un tableau\n");

  char *fct = "var_indicee";

  
  affiche_balise_ouvrante(fct, trace_abs);
  affiche_element("var_base_tableau", n->nom, trace_abs);
  affiche_exp( n->u.indicee_.indice );
  affiche_balise_fermante(fct, trace_abs);
}
/*-------------------------------------------------------------------------*/





/*-------------------------------------------------------------------------*/

void entreeFonction(void){
  dico.base = dico.sommet;
  contexte = C_VARIABLE_LOCALE;
  adresseLocaleCourante = 0;
  adresseArgumentCourant = 0;
}

/*-------------------------------------------------------------------------*/

void sortieFonction(void){
affiche_dico();
  dico.sommet = dico.base;
  dico.base = 0;
  contexte = C_VARIABLE_GLOBALE;  
}

/*-------------------------------------------------------------------------*/

int ajouteIdentificateur(char *identif, int classe, int type, int adresse, int complement)
{
  dico.tab[dico.sommet].identif = duplique_chaine(identif);
  dico.tab[dico.sommet].classe = classe;
  dico.tab[dico.sommet].type = type;
  dico.tab[dico.sommet].adresse = adresse;
  dico.tab[dico.sommet].complement = complement;
  dico.sommet++;

  if(dico.sommet == maxDico){
	fprintf(stderr, "attention, plus de place dans le dictionnaire des \
					 identificateurs, augmenter maxDico\n");
	exit(1);
  }
  //affiche_dico(); 
  return dico.sommet - 1;
}

/*-------------------------------------------------------------------------*/

int rechercheExecutable(char *identif)
{
  int i;
  for(i = dico.sommet - 1; i >= 0; i--){
	if(!strcmp(identif, dico.tab[i].identif))
	  return i;
  }
  return -1;
}

/*-------------------------------------------------------------------------*/

int rechercheDeclarative(char *identif)
{
  int i;
  for(i = dico.base; i < dico.sommet; i++){
	if(!strcmp(identif, dico.tab[i].identif))
	  return i;
  }
  return -1;
}

/*-------------------------------------------------------------------------*/

void affiche_dico(void)
{
  int i;
  return;
  printf("------------------------------------------\n");
  printf("base = %d\n", dico.base);
  printf("sommet = %d\n", dico.sommet);
  for(i=0; i < dico.sommet; i++){
	printf("%d ", i);
	printf("%s ", dico.tab[i].identif);
	if(dico.tab[i].classe == C_VARIABLE_GLOBALE)
	  printf("GLOBALE ");
	else
	if(dico.tab[i].classe == C_VARIABLE_LOCALE)
	  printf("LOCALE ");
	else
	if(dico.tab[i].classe == C_ARGUMENT)
	  printf("ARGUMENT ");

	if(dico.tab[i].type == T_ENTIER)
	  printf("ENTIER ");
	else if(dico.tab[i].type == T_TABLEAU_ENTIER)
	  printf("TABLEAU ");
/*     else if(dico.tab[i].type == _ARGUMENT) */
/*       printf("ARGUMENT "); */
	else if(dico.tab[i].type == T_FONCTION)
	  printf("FONCTION ");
	
	printf("%d ", dico.tab[i].adresse);
	printf("%d\n", dico.tab[i].complement);
  }
  printf("------------------------------------------\n");
}


//////////////////////////////////////RON/////////////////////////////////////
/*

int var_loc = 0;

void affiche_instr_appel(n_instr *n)
{
  char *fct = "instr_appel";
  affiche_balise_ouvrante(fct, trace_abs);
  printf("\tsubi \t$sp $sp 4\t# allocation valeur de retour\n");
  printf("\tjal \t%s\n", n->u.appel->fonction);
  affiche_appel(n->u.appel);
  affiche_balise_fermante(fct, trace_abs);
}

void affiche_appel(n_appel *n)
{

	if(!rechercheExecutable(n->fonction))
		erreur("fonction non declarée\n");	
	char *fct = "appel";
	affiche_balise_ouvrante(fct, trace_abs);
	affiche_texte( n->fonction, trace_abs);
	contexte = C_ARGUMENT;
	affiche_l_exp(n->args);
	affiche_balise_fermante(fct, trace_abs);
}
void affiche_instr_retour(n_instr *n)
{
  char *fct = "instr_retour";
  affiche_balise_ouvrante(fct, trace_abs);
  affiche_exp(n->u.retour_.expression);

  depile("$ra");
  depile("$fp");
  printf("\tjr $ra\n");
  affiche_balise_fermante(fct, trace_abs);

}
void affiche_foncDec(n_dec *n)
{
  char *fct = "foncDec";
  affiche_balise_ouvrante(fct, trace_abs);
  affiche_texte( n->nom, trace_abs );
  
  if(rechercheDeclarative(n->nom) != -1)
    erreur("function redifinition\n");
  n_dec * new = malloc(50);
  memcpy(new,n,50);
  int nb_param = 0;
    empile("$fp");
  printf("\tmove \t$fp $sp\n");
  empile("$ra");
  while(new->u.foncDec_.param != NULL){
  	new->u.foncDec_.param = new->u.foncDec_.param->queue;
  	nb_param++;
  }
  
  ajouteIdentificateur(n->nom, contexte, T_FONCTION,0,nb_param);
  entreeFonction();
  contexte = C_ARGUMENT;
  affiche_l_dec(n->u.foncDec_.param);
  contexte = C_VARIABLE_LOCALE;
  affiche_l_dec(n->u.foncDec_.variables);
  if(var_loc != 0)
    printf("\tsubi \t$fp  %d($sp) \t#allocation variables locales\n", var_loc*4);
  affiche_instr(n->u.foncDec_.corps);
  var_loc = 0;
  sortieFonction();
  affiche_balise_fermante(fct, trace_abs);
}

void affiche_varDec(n_dec *n)
{
	if(contexte == C_VARIABLE_GLOBALE){
		ajouteIdentificateur(n->nom, contexte, T_ENTIER,dico.base,-1);
		dico.base +=4;
	}
	if(contexte == C_VARIABLE_LOCALE){
		ajouteIdentificateur(n->nom, contexte, T_ENTIER,adresseLocaleCourante,-1);
    var_loc++;
		adresseLocaleCourante += 4;
  }
  if(contexte == C_ARGUMENT){
		ajouteIdentificateur(n->nom, contexte, T_ENTIER,adresseArgumentCourant,-1);
		adresseArgumentCourant += 4;
  }
  
  affiche_element("varDec", n->nom, trace_abs);
}

*/





