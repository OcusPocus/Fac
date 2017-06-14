#include "parser.h"


void init_domain(Domain * dom, int nb_var, int nb_value_domain){
	dom->nb_var 			= nb_var;
	dom->nb_value_domain 	= nb_value_domain;

	for(int i = 1; i <= dom->nb_var; i++){
		for(int j = 1; j < dom->nb_value_domain; j++){
			dom->domain_matrice[i][j] = -1;
		} 
	} 
}

void init_ensemblerelation(EnsembleRelation * ens_relation, int nb_value_domain){
	ens_relation->nb_value_domain 	= nb_value_domain;

	for(int i = 1; i <= ens_relation->nb_value_domain; i++){
		for(int j = 1; j <= ens_relation->nb_value_domain; j++){
			ens_relation->relation[i][j] = -1;
		}
	}
}

void init_constraintvar(ConstraintVar * cons_var, int nb_var){
	cons_var->nb_var = nb_var;

	for(int i = 1; i <= cons_var->nb_var; i++){
		for(int j = 1; j <= cons_var->nb_var; j++){
			cons_var->constraint_matrice[i][j] = NULL;
		}
	}

}

void init_stack(Stack * stack, int nb_var){
	stack->nb_var = nb_var;

	for(int i = 1; i <= stack->nb_var; i++){
		stack->stack_var[i] = 0;
	}

	for(int i = 1; i <= stack->nb_var; i++){
		stack->stack_value[i] = 0;
	}
}

void init_statevector(StateVector * sv, int nb_var){
	sv->nb_var 			= nb_var;

	for(int i = 1; i <= sv->nb_var; i++){
		sv->is_instantiated[i] 		 = 0;
		sv->nb_times_instantiated[i] = 0;
	}
}

//=========================== Pigeons ===============================

int parse_pigeondata(FILE * fd, Domain * dom, ConstraintVar * cons_var, EnsembleRelation * ens_relation, StateVector * sv, Stack * stack){
	int nb_var 	= 0;
	int test 	= fscanf(fd,"%d\n", &nb_var);
	if( test != 1){
		echec_reading("nb_var", test);
	}else{
		//printf("Dom->nb_var : %d\n", nb_var);
	}

	int nb_constraint = 0;
	test = fscanf(fd,"%d\n", &nb_constraint);
	if(test != 1){
		echec_reading("nb_constraint", test);
	}else{
		//printf("nb_constraint : %d\n", nb_constraint);
	}

	int nb_value_domain = -1;
	int temp 			= -1;
	for(int i = 1; i <= nb_var; i++){
		test = fscanf(fd, "%d : ",&temp);
		if(test!= 1){
			echec_reading("var : ", test);
		}else{
			//printf("Var : %d\n", temp);
		}
		for(int j = 1; j <= nb_var - 1; j++){
			test = fscanf(fd,"%d ", &nb_value_domain);
			if(test != 1){
				echec_reading("value_in_domain", test);
			}else{
				//printf("Var Value : %d\n", nb_value_domain);
			}
		}
	}

	init_domain(dom, nb_var, nb_value_domain);
	if(dom == NULL){ printf("allocation domain failed\n"); exit(-1);}

	init_ensemblerelation(ens_relation, nb_value_domain);
	init_constraintvar(cons_var, nb_var); 

	init_stack(stack, nb_var);
	if(stack == NULL){printf("Allocation Stack failed\n"); exit(-1);}
	init_statevector(sv, nb_var);
	if(sv == NULL){printf("Allocation StateVector failed\n"); exit(-1);}

	int var1 = 0, var2 = 0, value1 = 0, value2 = 0;
	char trump1, trump2, trump3;
	for(int i = 0; i < nb_constraint; i++){
		test = fscanf(fd, "%d %d:\n", &var1, &var2);
		if(test != 2){
			echec_reading("couple of var in constraint", test);
		}else{
			//printf("Var1 : %d  Var 2 : %d\n", var1, var2);
		}

		int nb_relation = nb_value_domain * (nb_value_domain - 1);
		for(int j = 1; j <= nb_relation; j++){
			test = fscanf(fd, "%c%d%c%d%c\n", &trump1, &value1, &trump2, &value2, &trump3);
			if(test != 5){
				echec_reading("couple compatible", test);
			}else{
				//printf("Couple compatible : %c %d %c %d %c\n", trump1, value1, trump2, value2, trump3);
			}
			
			dom->domain_matrice[var1][value1] 		= 1;
			dom->domain_matrice[var2][value2] 		= 1;
			ens_relation->relation[value1][value2] 	= 1;
		}
		cons_var->constraint_matrice[var1][var2] = ens_relation;
	}
/*
	for(int i = 1; i <= dom->nb_var; i++){
		for(int k = 1; k <= dom->nb_var; k++){
			for(int j = 1; j <= nb_value_domain; j++){
				for(int m = 1; m <= nb_value_domain; m++){
					if(cons_var->constraint_matrice[i][k] == NULL) continue;
					printf("cons_var->constraint_matrice[%d][%d]->relation[%d][%d] : %d\n",
										i, k, j, m,
										cons_var->constraint_matrice[i][k]->relation[j][m]);
				}
			}
		}
	}
	for(int i = 1; i <= dom->nb_var; i++){
		for(int k = 1; k <= dom->nb_value_domain; k++){
			printf("dom->domain_matrice[%d][%d] : %d\n", i, k, dom->domain_matrice[i][k]);
		}
	}*/

	return 1;
}

//===========================End Pigeons ============================

//============================ Dames ================================

int parse_damesdata(FILE * fd, Domain * dom, ConstraintVar * cons_var, EnsembleRelation * ens_relation, StateVector * sv, Stack * stack){
	int nb_var 	= 0;
	int test 	= fscanf(fd,"%d\n", &nb_var);
	if( test != 1){
		echec_reading("nb_var", test);
	}else{
		printf("Dom->nb_var : %d\n", nb_var);
	}

	int nb_constraint = 0;
	test = fscanf(fd,"%d\n", &nb_constraint);
	if(test != 1){
		echec_reading("nb_constraint", test);
	}else{
		printf("nb_constraint : %d\n", nb_constraint);
	}

	int nb_value_domain = -1;
	int temp 			= -1;
	for(int i = 1; i <= nb_var; i++){
		test = fscanf(fd, "%d : ",&temp);
		if(test!= 1){
			echec_reading("var : ", test);
		}else{
			printf("Var : %d\n", temp);
		}
		for(int j = 1; j <= nb_var; j++){
			test = fscanf(fd,"%d ", &nb_value_domain);
			if(test != 1){
				echec_reading("value_in_domain", test);
			}else{
				printf("Var Value : %d\n", nb_value_domain);
			}
		}
	}

	init_domain(dom, nb_var, nb_value_domain);
	if(dom == NULL){ printf("allocation domain failed\n"); exit(-1);}

	init_ensemblerelation(ens_relation, nb_value_domain);
	init_constraintvar(cons_var, nb_var); 

	init_stack(stack, nb_var);
	if(stack == NULL){printf("Allocation Stack failed\n"); exit(-1);}
	init_statevector(sv, nb_var);
	if(sv == NULL){printf("Allocation StateVector failed\n"); exit(-1);}

	int var1 = 0, var2 = 0, value1 = 0, value2 = 0;
	char trump1, trump2, trump3;
	for(int i = 0; i < nb_constraint; i++){
		test = fscanf(fd, "%d %d:\n", &var1, &var2);
		if(test != 2){
			echec_reading("couple of var in constraint", test);
		}else{
			printf("Var1 : %d  Var 2 : %d\n", var1, var2);
		}

		int nb_relation = (nb_value_domain * (nb_value_domain - 1));
		for(int j = 1; j <= nb_relation; j++){
			test = fscanf(fd, "%c%d%c%d%c\n", &trump1, &value1, &trump2, &value2, &trump3);
			if(test != 5){
				echec_reading("couple compatible", test);
			}else{
				printf("Couple compatible : %c %d %c %d %c\n", trump1, value1, trump2, value2, trump3);
			}
			
			dom->domain_matrice[var1][value1] 		= 1;
			dom->domain_matrice[var2][value2] 		= 1;
			ens_relation->relation[value1][value2] 	= 1;
		}
		cons_var->constraint_matrice[var1][var2] = ens_relation;
	}

	return 1;
}

//============================End Dames =============================

int parse(char * filename, int option, Domain * dom, ConstraintVar * cons_var, EnsembleRelation * ens_relation, StateVector * sv, Stack * stack){

	FILE * fd = fopen(filename, "r");
	if(fd == NULL){
		printf("open file failed\n");
		exit(-1);
	}

	if(option == 1){
		if(parse_pigeondata(fd, dom, cons_var, ens_relation, sv, stack) != 1) 
			echec_parse(option);
	}
	if(option == 2){
		if(parse_damesdata(fd, dom, cons_var, ens_relation, sv, stack) != 1) 
			echec_parse(option);
	}

	return 1;
}
