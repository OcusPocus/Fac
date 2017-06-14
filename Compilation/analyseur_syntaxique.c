#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "symboles.h"
#include "analyseur_syntaxique.h"
#include "util.h"

int unite_courante; 

char yytext[100];
char nom[100];
char valeur[100];
int trace_xml = 0;




n_prog * programme(){
	affiche_balise_ouvrante(__func__, trace_xml);
	unite_courante = yylex();
	n_l_dec *variables = NULL;
	n_l_dec *fonctions = malloc(sizeof(n_l_dec));
	n_dec * fonction = NULL;
	n_l_dec * listeFonc = fonctions;
	while(unite_courante != FIN){
		if(unite_courante == ENTIER)
			variables = instructionDeclVar(variables);
		if(unite_courante == ID_FCT){
			fonction = declarationFonction();
			fonctions = cree_n_l_dec_bis(fonction,fonctions);
		}
		else
			erreur("Neither a variable declaration or a function declaration");
		unite_courante = yylex(); 
	}
	affiche_balise_fermante(__func__,trace_xml);

	return cree_n_prog(variables,listeFonc);
}


n_dec * declarationFonction(){
	affiche_balise_ouvrante(__func__, trace_xml);
	affiche_element_courant();
	n_l_dec *param = NULL;
	n_l_dec *variables = NULL;
	n_instr *corps = NULL;
	char * function_name = malloc(strlen(yytext));
	strcpy(function_name,yytext);
	unite_courante = yylex();
	if(unite_courante == PARENTHESE_OUVRANTE){
		affiche_element_courant();
		param = listArgDeclar();
	
		if(unite_courante != PARENTHESE_FERMANTE){
			erreur("missing ')'");
		}
		unite_courante = yylex();
		if(unite_courante == ENTIER)
			variables = instructionDeclVar(variables);
		if(unite_courante == ACCOLADE_OUVRANTE){
			corps = instructionBlocFonction();
		}
		else
			erreur("missing '{'");
		affiche_element_courant();
		affiche_balise_fermante(__func__,trace_xml);
		return cree_n_dec_fonc(function_name,param,variables,corps);
	}
	return NULL;
}
n_l_dec * listArgDeclar(){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_l_dec * param = NULL;
	unite_courante = yylex();
	if(unite_courante == PARENTHESE_FERMANTE){
		affiche_balise_fermante(__func__,trace_xml);
		return param;
	}
	else{
		param = listArgPrimeDeclar();
		affiche_balise_fermante(__func__,trace_xml);
	}
	return param;
}
n_l_dec * listArgPrimeDeclar(){ 
	affiche_balise_ouvrante(__func__, trace_xml);
	n_l_dec * param = NULL;
	param = instructionDeclVar(param);
	if(unite_courante == PARENTHESE_FERMANTE){
			affiche_balise_fermante(__func__,trace_xml);
			return param;
	}
	affiche_balise_fermante(__func__,trace_xml);
	return NULL;
}

n_instr * instructionBlocFonction(){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_l_instr * list_instruction;
	affiche_element_courant();
	unite_courante = yylex();
	list_instruction = listInstruction();
	if(unite_courante != ACCOLADE_FERMANTE)
		erreur("missing }");
	affiche_element_courant();
	affiche_balise_fermante(__func__,trace_xml);
	return cree_n_instr_bloc(list_instruction);
}

n_var * check_tableau(){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_exp * indice = NULL;
	char * variable = malloc(strlen(yytext));
	strcpy(variable,yytext);
	unite_courante = yylex();
	if(unite_courante == CROCHET_OUVRANT){
		affiche_element_courant();
		unite_courante = yylex();
		indice = expression();
		if(unite_courante == CROCHET_FERMANT){
			affiche_element_courant();
			unite_courante = yylex();
			affiche_balise_fermante(__func__,trace_xml);
			return cree_n_var_indicee(variable,indice);
		}
		else{
			erreur("missing ]");
		}
	}
	affiche_balise_fermante(__func__,trace_xml);
	return cree_n_var_simple(variable);
}

n_dec * check_declaration_tableau(){
	affiche_balise_ouvrante(__func__, trace_xml);
	char * nom_var = malloc(strlen(yytext));
	strcpy(nom_var,yytext);
	int taille;
	unite_courante = yylex();
	if(unite_courante == CROCHET_OUVRANT){
		affiche_element_courant();
		unite_courante = yylex();
		if(unite_courante == NOMBRE){
			affiche_element_courant();
			taille = atoi(yytext);
			unite_courante = yylex();
		}
		else
			erreur("Number expected");
			
		if(unite_courante == CROCHET_FERMANT){
			affiche_element_courant();
			unite_courante = yylex();
			affiche_balise_fermante(__func__,trace_xml);
			return cree_n_dec_tab(nom_var,taille);
		}
		else
			erreur("missing ]");
	}
	affiche_balise_fermante(__func__,trace_xml);
	return cree_n_dec_var(nom_var);
}


/////////////////////////////////////////////////////////////////INSTRUCTION  /////////////////////////////////////////////////////////////////////////////////////////////////////
n_instr * instruction(){
	affiche_balise_ouvrante(__func__, trace_xml);
	switch(unite_courante){
		case ACCOLADE_OUVRANTE : instructionBloc(); break;
		case ID_VAR : return instructionAffect(); break;
		case TANTQUE : return instructionWhile(); break;
		case SI : return instructionIf(); break;
		case ENTIER : return NULL; break;
		case RETOUR : return instructionReturn(); break;
		case ECRIRE : return instructionWrite(); break;
		case ID_FCT : return instructionFonction(); break;
		default : return instructionVide();
	}
	affiche_balise_fermante(__func__,trace_xml);
	return NULL;
}


n_appel * appelFonction(){ // pour l'appel
	affiche_balise_ouvrante(__func__, trace_xml);
	n_l_exp * list_arg = NULL;
	char * function = malloc(strlen(yytext));
	strcpy(function,yytext);
	affiche_element_courant();
	unite_courante = yylex();
	if(unite_courante == PARENTHESE_OUVRANTE){
		affiche_element_courant();
		list_arg = listArg();
		if(unite_courante != PARENTHESE_FERMANTE ){
			erreur("missing ')' moi ?");
		}
		affiche_element_courant();
		affiche_balise_fermante(__func__,trace_xml);
		return cree_n_appel(function,list_arg);
	}
	return NULL;
}

n_l_exp * listArg(){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_l_exp * list_arg = NULL;
	unite_courante = yylex();
	if(unite_courante == PARENTHESE_FERMANTE){
		affiche_balise_fermante(__func__,trace_xml);
		return list_arg;
	}
	else{
		list_arg = listArgPrime(list_arg);
		affiche_balise_fermante(__func__,trace_xml);
	}
	return list_arg;
}

n_l_exp * listArgPrime(n_l_exp * list_arg){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_exp * exp = NULL;
	if(unite_courante != ENTIER && unite_courante != ID_VAR && unite_courante != ID_FCT && unite_courante != NOMBRE)
		return NULL;
	exp = expression();
	list_arg = cree_n_l_exp(exp,list_arg);
	if(unite_courante == VIRGULE){
		unite_courante = yylex();
		list_arg = listArgPrime(list_arg);
		affiche_balise_fermante(__func__,trace_xml);
	}
		//unite_courante = yylex();
	affiche_balise_fermante(__func__,trace_xml);
	return list_arg;
}

/*
n_l_exp * listArgPrime(n_l_exp * list_arg){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_exp * exp = NULL;
	exp = expression();
	list_arg = cree_n_l_exp(exp,list_arg);
	listArg2(list_arg);
	affiche_balise_fermante(__func__,trace_xml);
	return list_arg;
}
*/
void listArg2(n_l_exp * list_arg){
	affiche_balise_ouvrante(__func__, trace_xml);
	if(unite_courante == PARENTHESE_FERMANTE){
		affiche_balise_fermante(__func__,trace_xml);
		return;
	}
	if(unite_courante == VIRGULE){
		affiche_element_courant();
		unite_courante = yylex();
		listArgPrime(list_arg);
	}
	else {
	
		affiche_balise_fermante(__func__,trace_xml);
		erreur("missing ')' or argument");
	}
}

n_instr * instructionBloc(){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_l_instr * list_instruction;
	affiche_element_courant();
	unite_courante = yylex();
	list_instruction = listInstruction();
	if(unite_courante != ACCOLADE_FERMANTE)
		erreur("missing }");
	affiche_element_courant();
	affiche_balise_fermante(__func__,trace_xml);
	unite_courante = yylex();
	return cree_n_instr_bloc(list_instruction);
}

n_instr * instructionAffect(){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_exp * exp = NULL;
	n_var * var = NULL;
	affiche_element_courant();
	var = check_tableau();
	if(unite_courante == EGAL){
		affiche_element_courant();
		unite_courante = yylex();
		exp = expression();
	}
	if(unite_courante != POINT_VIRGULE)
		erreur("missing ';'");
	instruction();
	return cree_n_instr_affect(var,exp);
}

n_instr *  instructionFonction(){
	n_appel * appel = NULL;
	affiche_balise_ouvrante(__func__, trace_xml);
	appel = appelFonction();
	//unite_courante = yylex();
	instruction();
	affiche_balise_fermante(__func__,trace_xml);
	return cree_n_instr_appel(appel);
}

n_instr *  instructionWhile(){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_exp * test = NULL;
	n_instr * faire = NULL;
	affiche_element_courant();
	unite_courante = yylex();
	test = expression();
	if(unite_courante == FAIRE){
		affiche_element_courant();
		unite_courante = yylex();
		faire = instructionBloc();
		affiche_balise_fermante(__func__,trace_xml);
	}
	else
		erreur("missing 'faire' keyword");
	affiche_balise_fermante(__func__,trace_xml);
	return cree_n_instr_tantque(test,faire);
}

n_instr * instructionIf(){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_instr * si = NULL;
	n_instr * alors = NULL;
	n_instr * sinon = NULL;
	n_exp * test = NULL;
	affiche_element_courant();
	unite_courante = yylex();
	test = expression();
	if(unite_courante == ALORS){
		affiche_element_courant();
		unite_courante = yylex();
		alors = instructionBloc();
		if(unite_courante == SINON){
			affiche_element_courant();
			unite_courante = yylex();
			sinon = instructionBloc();
			affiche_balise_fermante(__func__,trace_xml);
		}
	}
	else
		erreur("erreur missing 'alors' keyword \n");
	si = cree_n_instr_si(test,alors,sinon);
	affiche_balise_fermante(__func__,trace_xml);
	return si;
}

n_l_dec * instructionDeclVar(n_l_dec * list_variable){
	n_dec * variable = NULL;
	if(unite_courante != ENTIER)
		return NULL;
	affiche_balise_ouvrante(__func__, trace_xml);
	affiche_element_courant();
	unite_courante = yylex();
	if(unite_courante == ID_VAR){
		affiche_element_courant();
		char * variable_name = malloc(strlen(yytext));
		strcpy(variable_name,yytext);
		variable = check_declaration_tableau();
		list_variable = cree_n_l_dec(variable,list_variable);
		if(unite_courante == VIRGULE){
			affiche_element_courant();
			unite_courante = yylex();
			list_variable = instructionDeclVar(list_variable);
			/* ou  simplement  instructionDeclVar(list_variable);, je suis pas sur */
			affiche_balise_fermante(__func__,trace_xml);
		}
		else if(unite_courante == POINT_VIRGULE){
			affiche_element_courant();
			unite_courante = yylex();
			affiche_balise_fermante(__func__,trace_xml);
			return list_variable;
		}
	}
	else
		erreur("missing $");
	affiche_balise_fermante(__func__,trace_xml);
	return list_variable;
}

n_l_instr * listInstruction(){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_l_instr *liste_instruction = NULL;
	n_instr * instr = NULL;
	if(unite_courante == ACCOLADE_FERMANTE){
		affiche_balise_fermante(__func__,trace_xml);
		return liste_instruction;
	}
	instr = instruction();
	liste_instruction = listInstruction();
	liste_instruction = cree_n_l_instr(instr,liste_instruction);
	affiche_balise_fermante(__func__,trace_xml);
	return liste_instruction;
}

n_instr * instructionVide(){
	affiche_balise_ouvrante(__func__, trace_xml);
	if(unite_courante == POINT_VIRGULE){
		affiche_element_courant();
		unite_courante = yylex();
		affiche_balise_fermante(__func__,trace_xml);
		return cree_n_instr_vide();
	}
	else{
				//printf("ici : %s \n",yytext);
		//erreur("instruction not correct");
		unite_courante = yylex();
		return NULL;
	}
	affiche_element_courant();
	affiche_balise_fermante(__func__,trace_xml);
	exit(-1);
}

n_instr * instructionReturn(){
	n_exp * valeur_retour = NULL;
	affiche_element_courant();
	unite_courante = yylex();
	valeur_retour = expression();
	if(unite_courante == POINT_VIRGULE){
		affiche_element_courant();
		unite_courante = yylex();
		return cree_n_instr_retour(valeur_retour);
	}
	else{
		erreur("missing ';'");	
		return NULL;
	}
}

n_instr * instructionWrite(){
	affiche_element_courant();
	unite_courante = yylex();
	n_exp * ecrire_exp = NULL;
	if(unite_courante == PARENTHESE_OUVRANTE){
		affiche_element_courant();
		unite_courante = yylex();
		ecrire_exp = expression();
		if(unite_courante == PARENTHESE_FERMANTE){
			affiche_element_courant();
			unite_courante = yylex();
		}
		else
			erreur("missing ')'");
			
	}
	else
		erreur("missing (");
		
	if(unite_courante == POINT_VIRGULE){
		affiche_element_courant();
		unite_courante = yylex();
		return cree_n_instr_ecrire(ecrire_exp);
	}
	else
		erreur("missing ';'");
	return NULL;

}

/////////////////////////////////////////////////////void check_declaration_tableau(){////////////EXPRESSION//////////////////////////////////////////////////////////////////////////////////////////////////
n_exp * expression(){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_exp * op = NULL;
	op = exprComp();
	affiche_balise_fermante(__func__,trace_xml);
	return op;
}

n_exp * negation(){
	n_exp * op1 = NULL;
	affiche_balise_ouvrante(__func__, trace_xml);
	if(unite_courante == NON){
		affiche_element_courant();
		unite_courante = yylex();
		op1 = expAnd();
		affiche_balise_fermante(__func__,trace_xml);
		return cree_n_exp_op(negatif,op1,NULL);
	}
	else{
		op1 = equalityExp();
		affiche_balise_fermante(__func__,trace_xml);
		return op1;
	}
	affiche_balise_fermante(__func__,trace_xml);
}

n_exp * exprComp(){
	n_exp * op1 = NULL;
	affiche_balise_ouvrante(__func__, trace_xml);
	op1 = expAnd();
	op1 = expOr(op1);
	affiche_balise_fermante(__func__,trace_xml);
	return op1;
}

n_exp * expOr(n_exp * op1){
	affiche_balise_ouvrante(__func__, trace_xml);
	if(unite_courante == OU){
		affiche_element_courant();
		unite_courante = yylex();
		n_exp * op2 = exprComp();
		affiche_balise_fermante(__func__,trace_xml);
		return cree_n_exp_op(ou,op1,op2);
	}
	else{
		affiche_balise_fermante(__func__,trace_xml);
		return op1;
	}
}

n_exp * expAnd(){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_exp * op1 = negation();
	op1 = And(op1);
	affiche_balise_fermante(__func__,trace_xml);
	return op1;
}

n_exp * And(n_exp * op1){
	affiche_balise_ouvrante(__func__, trace_xml);
	if(unite_courante == ET){
		affiche_element_courant();
		unite_courante = yylex();
		n_exp * op2 = expAnd();
		affiche_balise_fermante(__func__,trace_xml);
		return cree_n_exp_op(et,op1,op2);
	}
	else{
		affiche_balise_fermante(__func__,trace_xml);
		return op1;
	}
}

n_exp * equalityExp(){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_exp * op1 = NULL;
	op1 = arithmeticExp();
	op1 = expEqual(op1);
	affiche_balise_fermante(__func__,trace_xml);
	return op1;
}

n_exp * expEqual(n_exp * op1){
	affiche_balise_ouvrante(__func__, trace_xml);
	if(unite_courante == EGAL || unite_courante == INFERIEUR){
		n_exp * op2 = NULL;
		int flag = 0;
		if(unite_courante == EGAL)
			flag = 1;
		affiche_element_courant();
		unite_courante = yylex();
		op2 = equalityExp();
		affiche_balise_fermante(__func__,trace_xml);
		return cree_n_exp_op(flag == 0? inf:egal,op1,op2);
	}
	else{
		affiche_balise_fermante(__func__,trace_xml);
		return op1;
	}
}

n_exp * arithmeticExp(){
	affiche_balise_ouvrante(__func__, trace_xml);
	n_exp * op1 = NULL;
	op1 = expProd();
	op1 = expPlus(op1);
	affiche_balise_fermante(__func__,trace_xml);
	return op1;
}

n_exp * expPlus(n_exp * op1){
	affiche_balise_ouvrante(__func__, trace_xml);
	if(unite_courante == PLUS || unite_courante == MOINS){
		int flag = 0;
		if(unite_courante == PLUS)
			flag = 1;
		affiche_element_courant();
		n_exp * op2 = NULL;
		unite_courante = yylex();
		op2 = expression();
		affiche_balise_fermante(__func__,trace_xml);
		return cree_n_exp_op(flag == 0 ? moins:plus,op1,op2);
	}
	else{
		affiche_balise_fermante(__func__,trace_xml);
		return op1;
	}
}

n_exp * expProd(){
	n_exp * op1 = NULL;
	affiche_balise_ouvrante(__func__, trace_xml);
	op1 = term();
	op1 = produit(op1);
	affiche_balise_fermante(__func__,trace_xml);
	return op1;	
}

n_exp * produit(n_exp * op1){
	affiche_balise_ouvrante(__func__, trace_xml);
	if(unite_courante == FOIS || unite_courante == DIVISE){
		n_exp * op2 = NULL;
		int flag = 0;
		if(unite_courante == FOIS)
			flag = 1;
		affiche_element_courant();
		unite_courante = yylex();
		op2 = expression();
		affiche_balise_fermante(__func__,trace_xml);
		return cree_n_exp_op(flag == 0 ? divise:fois,op1,op2);
	}
	else{
		affiche_balise_fermante(__func__,trace_xml);
		return op1;
	}
}

void error(){
		delireCar();
		nom_token(unite_courante,nom,valeur);
		printf("Invalid type argument of unary '%s' %s\n",valeur, nom); 
		exit(-1);
}

n_exp * term(){
	affiche_balise_ouvrante(__func__, trace_xml);
	if(unite_courante == ID_FCT){
		n_appel * app = NULL;
		app = appelFonction();
		unite_courante = yylex();
		return cree_n_exp_appel(app);
	}
	if(unite_courante == LIRE){
		affiche_element_courant();
		unite_courante = yylex();
		if(unite_courante == PARENTHESE_OUVRANTE){
			affiche_element_courant();
			unite_courante = yylex();
			if(unite_courante == PARENTHESE_FERMANTE){
				affiche_element_courant();
				unite_courante = yylex();
				return cree_n_exp_lire();
			}
		}
		else{
			erreur("Error : Missing right parenthesis\n");
			return NULL;
		}
	}
	if(unite_courante == PARENTHESE_OUVRANTE){
		affiche_element_courant();
		n_exp * exp = NULL;
		unite_courante = yylex();
		exp = expression();
		if(unite_courante == PARENTHESE_FERMANTE){
			unite_courante = yylex();
			affiche_element_courant();
			return exp;
		}
		else{
			erreur("Error : Missing right parenthesis\n");
			return NULL;
		}
	}
	else if(unite_courante == NOMBRE){
		affiche_element_courant();
		int entier = atoi(yytext);
		unite_courante = yylex();
		return cree_n_exp_entier(entier);
	}
	else if(unite_courante == ID_VAR){
		n_var * var = NULL;
		affiche_element_courant();
		var = check_tableau();
		affiche_balise_fermante(__func__,trace_xml);
		return cree_n_exp_var(var);
	}
	else{
		erreur("incorrect expression");
		return NULL;
	}
	affiche_balise_fermante(__func__,trace_xml);
}

void affiche_element_courant(){
	nom_token(unite_courante, nom, valeur);
	affiche_element(nom,valeur,trace_xml);
}

void test_analyse_syntaxique(FILE *yyin){
  unite_courante = yylex(); 
  while (unite_courante != FIN) {
    unite_courante = yylex(); 
  } 
}


FILE *yyin;

int main(int argc, char **argv) {  
  yyin = fopen(argv[1], "r");
  if(yyin == NULL){
    fprintf(stderr, "impossible d'ouvrir le fichier %s\n", argv[1]);
    exit(1);
  }
	n_prog * prog;
  //test_yylex_internal(yyin);
  //test_analyse_syntaxique(yyin);
  prog = programme();
  affiche_n_prog(prog);
  return 0;
}

