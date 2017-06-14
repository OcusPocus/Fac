#include "ForwardChecking.h"

void save_domain_before_filter(int domain[MAX_VAR][MAX_VALUE], int save[MAX_VAR][MAX_VALUE]){
	for(int i = 0; i < MAX_VAR; i++){
		for(int j = 0; j < MAX_VALUE; j++){
			save[i][j] = domain[i][j];
		}
	}
}

void filter(int domain[MAX_VAR][MAX_VALUE], int current_var, int current_value){
	for(int i = 0; i < MAX_VAR; i++){
		if(domain[current_var][current_value] != 0){
			domain[current_var][current_value] = -current_var;
		}
	} 
}

void print_dom(int domain[MAX_VAR][MAX_VALUE]){
	printf("=======================================================\n");
	for(int i = 0; i < MAX_VAR; i++){
		for(int j = 0; j < MAX_VALUE; j++){
			printf("domain[%d][%d] = %d\n", i, j, domain[i][j]);
		}
	}
	printf("=======================================================\n");
}


void forward_checking(Domain * dom, ConstraintVar * cons_var, StateVector * sv, Stack * stack){
	printf("================================ Start of ForwardChecking ==============================\n");

	printf("================================ End of ForwardChecking ================================\n");
}
