#include "ForwardChecking.h"

int fc_all_values_used_for(int var, int values_already_used_for[MAX_VAR][MAX_VALUE], int nb_value){
	int count = 0;
	for(int i = 1; i <= nb_value; i++){
		if(values_already_used_for[var][i] != 1) continue;
		count++;
	}
	return (count == nb_value);
}

void fc_reset_values_used(int values_already_used_for[MAX_VAR][MAX_VALUE], int nb_var, int current_value){
	for(int i = 1; i <= nb_var; i++){
		values_already_used_for[i][current_value] = 0;
		
	}
}

void fc_init_neighbour_tab(ConstraintVar * cons_var, int neighbour_tab[MAX_VAR][MAX_VAR], Domain * dom){
	for(int i = 1; i<= dom->nb_var; i++){
		for(int j = 1; j <= dom->nb_var; j++){
			if(cons_var->constraint_matrice[i][j] != NULL){
				neighbour_tab[i][j] = 1;
			}
		}
	}
}

int fc_share_constraint_with(int var, ConstraintVar * cons_var, Domain * dom, int neighbour_tab[MAX_VAR][MAX_VAR]){
	for(int i = 1; i<= dom->nb_var; i++){
		if(cons_var->constraint_matrice[var][i] != NULL){
			if(neighbour_tab[var][i] == 1)
				return i;
		}
	}
	return 0;
}

void fc_save_initial_domain(Domain * dom, int initial_stateof_domain[MAX_VAR][MAX_VALUE]){
	for(int i = 1; i <= dom->nb_var; i++){
		for(int j = 1; j <= dom->nb_value_domain; j++){
			initial_stateof_domain[i][j] = dom->domain_matrice[i][j];
		}
	}
}

void fc_save_domain_before_filter(Domain * dom, int current_stateof_domain[MAX_VAR][MAX_VALUE]){
	for(int i = 1; i <= dom->nb_var; i++){
		for(int j = 1; j <= dom->nb_value_domain; j++){
			current_stateof_domain[i][j] = dom->domain_matrice[i][j];
		}
	}
}

void fc_load_initial_domain(int initial_stateof_domain[MAX_VAR][MAX_VALUE], Domain * dom){
	for(int i = 1; i <= dom->nb_var; i++){
		for(int j = 1; j <= dom->nb_value_domain; j++){
			dom->domain_matrice[i][j] = initial_stateof_domain[i][j];
		}
	}
}

void fc_load_domain_before_filter(int current_stateof_domain[MAX_VAR][MAX_VALUE], Domain * dom){
	for(int i = 1; i <= dom->nb_var; i++){
		for(int j = 1; j <= dom->nb_value_domain; j++){
			dom->domain_matrice[i][j] = current_stateof_domain[i][j];
		}
	}
}
void fc_delete_from_domain(int current_var, int current_value, int neighbour_tab[MAX_VAR][MAX_VAR], Domain * dom){
	for(int i = 1; i <= dom->nb_var; i++){
		if(neighbour_tab[current_var][i] == 1){
			dom->domain_matrice[i][current_value] = 0;
		}
	}
}

int fc_domain_is_empty(Domain * dom, int neighbour_tab[MAX_VAR][MAX_VAR], int var){
	int count = 0, neighbour = 0;
	for(int i = 1; i <= dom->nb_var; i++){
		if(neighbour_tab[var][i] == 1){
			neighbour = i;
		}
	}
	for(int i = 1; i <= dom->nb_value_domain; i++){
		if(dom->domain_matrice[neighbour][i] == 0)
			count++;
		else
			count = 0;
	}
	return count == dom->nb_value_domain;
}

void fc_print(Domain * dom){
	for(int i = 1; i <= dom->nb_var; i++){
		for(int j = 1; j <= dom->nb_value_domain; j++){
			printf("dom->domain_matrice[%d][%d] : %d\n", i, j, dom->domain_matrice[i][j]);
		}
	}
}

int fc_next_value_domain(Domain * dom, int var, int is_used){
	for(int i = 1; i <= dom->nb_value_domain; i++){
		if(dom->domain_matrice[var][i] != 0){
			if(is_used) return 0;
			return i;
		}
	}
	return 0;
}

int fc_all_values_in_current_domain_used(Domain * dom, int var){
	int count = 0;

	for(int i = 1; i <= dom->nb_value_domain; i++){
		if(dom->domain_matrice[var][i] == 0)
			count ++;
	}
	return (count == dom->nb_value_domain);
}

int fc_all_values_used(Domain *dom, int values_already_used_for[MAX_VAR][MAX_VALUE]){ // a tester
	int count = 0;
	for(int k = 1; k <= dom->nb_var; k++){
		for(int i = 1; i <= dom->nb_value_domain; i++){
			if(values_already_used_for[k][i] != 1) continue;
			count++;
		}
	}
	if(count != (dom->nb_value_domain * dom->nb_var)) return 0;

	return 1;
}
int fc_all_var_assigned(Domain * dom, StateVector * sv){
	int count = 0;
	for(int i = 1; i <= dom->nb_var; i++){
		if(sv->is_instantiated[i] != 1) continue;
		count++;
	}
	if(count != dom->nb_var) return 0;

	return 1;
}


void forward_checking(Domain * dom, ConstraintVar * cons_var, StateVector * sv, Stack * stack){
	printf("------------------------Debut ForwardChecking------------------------\n");
	printf("------------------------Sans Heuristique ------------------------\n");
	(void)stack;
	int first_var   = 1;
	int first_value = 1;

	int current_var = first_var;
	int current_value = first_value;

	int previous_var 	= 1;
	int previous_value 	= 1;

	int nb_solution = 0;
	int values_already_used_for[MAX_VAR][MAX_VALUE];
	values_already_used_for[first_var][first_value] = 1;
	sv->is_instantiated[first_var] = 1;

	int neighbour_tab[MAX_VAR][MAX_VAR];
	fc_init_neighbour_tab(cons_var, neighbour_tab, dom);

	int initial_stateof_domain[MAX_VAR][MAX_VALUE];
	int current_stateof_domain[MAX_VAR][MAX_VALUE];

	fc_save_initial_domain(dom, initial_stateof_domain);
	while(!fc_all_values_used_for(first_var, values_already_used_for, dom->nb_value_domain)){

		if(fc_share_constraint_with(current_var, cons_var, dom ,neighbour_tab) != 0){

			fc_save_domain_before_filter(dom, current_stateof_domain);
			fc_delete_from_domain(current_var, current_value, neighbour_tab, dom);
			printf("Après filtre\n");
			fc_print(dom);
			if(fc_domain_is_empty(dom, neighbour_tab, current_var)){
				fc_load_domain_before_filter(current_stateof_domain, dom);
				current_value = fc_next_value_domain(dom, current_var, values_already_used_for[current_var][current_value]);
				if(values_already_used_for[current_var][current_value]){
					fc_load_initial_domain(initial_stateof_domain, dom);
					fc_reset_values_used(values_already_used_for, dom->nb_var, current_value);
					current_value += 1;
					if(current_value > dom->nb_value_domain || previous_value > dom->nb_value_domain){
						current_value -= 1;
						break;
					}
					values_already_used_for[current_var][current_value] = 1;
				}
				values_already_used_for[current_var][current_value] = 1;
			}else{
				previous_var 	= current_var;
				previous_value 	= current_value;
				current_var++;
				sv->is_instantiated[current_var] = 1;
				current_value = fc_next_value_domain(dom, current_var, values_already_used_for[current_var][current_value]);
				values_already_used_for[current_var][current_value] = 1;

			}
			if(current_value == 0){
				previous_value += 1;
				if(previous_value > dom->nb_value_domain){
					previous_value = fc_next_value_domain(dom, current_var, values_already_used_for[current_var][previous_value]);
				}
				current_var = previous_var;
				current_value = previous_value;
			}
			if(fc_all_values_used(dom, values_already_used_for) && fc_all_var_assigned(dom, sv)){
		  		nb_solution++;
		  		break;
		  	}
		}else{
			current_var 	= first_var;
			current_value	= first_value += 1;
			values_already_used_for[first_var][current_value] = 1;
		}
	}	
	if(nb_solution != 0){
		printf("Nombre de solution : %d\n", nb_solution);
	}
	else
		printf("Absence de solution ou algorithme pas adapté au problème\n");
	printf("----------------------Fin Sans Heuristique ----------------------\n");

	printf("------------------------Fin ForwardChecking------------------------\n");
}
