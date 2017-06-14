#include "util.h"

void echec_reading(char * elt_read, int value){
	printf("Fscanf %s failed\n", elt_read);
	printf("Fscanf return value : %d\n", value);
	exit(-1);
}

void echec_parse(int option){
	if(option == 1){
		printf("Parsing data for Pigeons failed\n");
		exit(-1);
	}
	if(option == 2){
		printf("Parsing data for Dame failed\n");
		exit(-1);
	}
}

void empty_tab(int * tab1, int * tab2, int size, int size2){
	for(int i = 0; i < size; i++){
		tab1[i] = 0;
	}
	for(int k = 0; k < size2; k++){
		tab2[k] = 0;
	}
}

void print_tab(int * tab1, int * tab2, int size, int size2){
	for(int i = 0; i < size; i++){
		printf("tableau1[%d] : %d\n", i+1, tab1[i]);
	}
	for(int k = 0; k < size2; k++){
		printf("tableau2[%d] : %d\n", k+1, tab2[k]);
	}
}

void test_allocation_tab(int result, int * tab1, int * tab2, int size, int size2){
	if(result){
		empty_tab(tab1, tab2, size, size2);
		print_tab(tab1, tab2, size, size2);
	}
	else{
		printf("allocation Failed\n");
		exit(-1);
	}
}

void empty_mat(int ** matrice, int size, int size2){
	for (int i = 0; i < size; i++){
		for(int j = 0; j < size2; j++){
			matrice[i][j] = 0;
		}
	}
}

void print_mat(int ** matrice, int size, int size2){
	for (int i = 0; i < size; i++){
		for(int j = 0; j < size2; j++){
			printf("matrice[%d][%d] = %d\n", i+1, j+1, matrice[i][j]);
		}
	}
}

void test_allocation_mat(int result, int ** mat, int size, int size2){
	if(result){
		empty_mat(mat, size, size2);
		print_mat(mat, size, size2);
	}
	else{
		printf("allocation Failed\n");
		exit(-1);
	}
}

void pop_var(Stack * stack){
	for(int i = 1 ; i <= stack->nb_var ; i++){
		if(stack->stack_var[ i ] != 0){
			stack->stack_var[i-1] = stack->stack_var[i];
			stack->stack_var[i] = 0;
		}
		else{
			printf("Sommet de pile vide\n");
			printf("Pop impossible\n");
			exit(-1);
		}
	}
}

void pop_value(Stack * stack){
	for(int i = 1 ; i <= stack->nb_var ; i++){
		if(stack->stack_value[ i ] != 0){
			stack->stack_value[i-1] = stack->stack_value[i];
			stack->stack_value[i] = 0;
		}
		else{
			printf("Sommet de pile vide\n");
			printf("Pop impossible\n");
			exit(-1);
		}
	}
}

void push_var(Stack * stack, int var){
	if(stack->stack_var[stack->nb_var] !=0){
		printf("impossible de push_var\n"); 
		exit(-1);
	}
	
	int temp = var;
	for(int i = 1; i <= stack->nb_var; i++){
		if(stack->stack_var[i] == 0){
			for (int k = i; k >= 1; k--){
				stack->stack_var[k] = stack->stack_var[k - 1];
			}
		}
	}
	stack->stack_var[1] = temp;
}

void push_value(Stack * stack, int value){
	if(stack->stack_value[stack->nb_var] != 0){
		printf("impossible de push_value\n"); 
		exit(-1);
	}
	
	int temp = value;
	for(int i = 1; i <= stack->nb_var; i++){
		if(stack->stack_value[i] == 0){
			for (int k = i; k >= 1; k--){
				stack->stack_value[k] = stack->stack_value[k - 1];
			}
		}
	}
	stack->stack_value[1] = temp;
}
