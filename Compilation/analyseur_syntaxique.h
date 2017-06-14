#ifndef __SYNTAXE__
#define __SYNTAXE__

#include "stdio.h"
#include "analyseur_lexical.h"
#include "syntabs.h"

#include "dico.h"

n_prog * programme();


n_dec * declarationFonction();
n_l_dec * listArgDeclar();
n_l_dec * listArgPrimeDeclar();

n_instr * instructionBlocFonction();
n_var * check_tableau();

n_dec * check_declaration_tableau();
n_instr * instruction();

n_appel * appelFonction();
n_l_exp * listArg();
n_l_exp * listArgPrime(n_l_exp * list_arg);

void listArg2(n_l_exp * list_arg);

n_instr * instructionBloc();
n_instr * instructionAffect();
n_instr *  instructionFonction();
n_instr *  instructionWhile();
n_instr * instructionIf();
n_l_dec * instructionDeclVar(n_l_dec * list_variable);

n_l_instr * listInstruction();

n_instr * instructionVide();

n_instr * instructionReturn();
n_instr *  instructionWrite();

n_exp * expression();
n_exp * negation();
n_exp * exprComp();
n_exp * expOr(n_exp * op1);
n_exp * expAnd();
n_exp * And(n_exp * op1);
n_exp * equalityExp();
n_exp * expEqual(n_exp * op1);
n_exp * arithmeticExp();
n_exp * expPlus(n_exp * op1);
n_exp * expProd();
n_exp * produit(n_exp * op1);

n_exp * term();

void error();

void affiche_element_courant();
void test_analyse_syntaxique(FILE *yyin);

#endif
