#ifndef __CSP__
#define __CSP__

#include <stdio.h>
#include <stdlib.h>

#define TRUE 1
#define FALSE 0
#define MAX_VAR   200
#define MAX_VALUE 200

typedef struct{
	int domain_matrice[MAX_VAR][MAX_VALUE];
	int nb_var;
	int nb_value_domain;
}Domain;

typedef struct{
	int relation[MAX_VALUE][MAX_VALUE];
	int nb_value_domain;
}EnsembleRelation;

typedef struct{
	EnsembleRelation * constraint_matrice[MAX_VAR][MAX_VAR];
	int nb_var;
}ConstraintVar;

typedef struct{
	int stack_var[MAX_VAR];
	int stack_value[MAX_VALUE];
	int nb_var;
}Stack;

typedef struct{
	int is_instantiated[MAX_VAR]; 		  // tableau de booléen 1 = variable instanciée 0 = pas instanciée
	int nb_times_instantiated[MAX_VAR];   // possède le nombre de fois qu'une variable est instanciée
	int nb_var;
}StateVector;

#endif
