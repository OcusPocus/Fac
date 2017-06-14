#include "generator.h"

void nb_var_generator(FILE * fd, int nb_var){
	fprintf(fd, "%d\n", nb_var);
}

void nb_constraint_generator(FILE * fd, int nb_var){
	int nb_constraint = (nb_var * (nb_var - 1)) / 2;
	fprintf(fd, "%d\n", nb_constraint);
}

void domain_generator(FILE * fd, int nb_var, int option){
	if(option == 1){ domain_pigeongenerator(fd, nb_var);}
	if(option == 2){ domain_damesgenerator(fd, nb_var);}
}

//=========================== Pigeons ===============================

void domain_pigeongenerator(FILE * fd, int nb_var){
	for(int i = 1; i <= nb_var; i++){
		fprintf(fd, "%d : ", i);
		for (int value = 1; value < nb_var; value++){
			fprintf(fd, "%d ", value);
		}
		fprintf(fd, "\n");
	}
}

void resolvecouple_pigeongenerator(FILE * fd, int nb_var){
	for(int i = 1; i < nb_var; i++){
		for(int j = 1; j < nb_var; j++){
			if(j == i){
				continue;
			}
			else
				fprintf(fd, "(%d,%d)\n", i, j);
		}
	}
}

void constraint_pigeongenerator(FILE * fd, int nb_var){
	for (int i = 1; i <= nb_var ; i++){
		for(int j = 1; j <= nb_var; j++){
			if(j == i){
				continue;
			}
			if(i > j){
				continue;
			}
			else{
				fprintf(fd, "%d %d:\n", i, j);
				resolvecouple_pigeongenerator(fd, nb_var);
			}
		}
	}
}

int pigeons_generator(FILE * fd, int nb_var){
	printf("Génération d'une instance du problème des pigeons en cours...\n");
	nb_var_generator(fd, nb_var);
	nb_constraint_generator(fd, nb_var);
	domain_generator(fd, nb_var, 1);
	constraint_pigeongenerator(fd, nb_var);
	printf("Génération terminée\n");
	return 1;

}
//=========================== Fin Pigeons ===========================
//=========================== Dames =================================

void domain_damesgenerator(FILE * fd, int nb_var){
	for(int i = 1; i <= nb_var; i++){
		fprintf(fd, "%d : ", i);
		for (int value = 1; value <= nb_var; value++){
			fprintf(fd, "%d ", value);
		}
		fprintf(fd, "\n");
	}
}

void resolvecouple_damesgenerator(FILE * fd, int nb_var, int first_var, int second_var){ // Je ne sais pas s'il fonctionne
	int knigh_path, diagonal, anti_diagonal;
	for(int i = 1; i <= nb_var; i++){
		for(int j = 1; j <= nb_var; j++){
			knigh_path = first_var - second_var;
			if(knigh_path < 0) knigh_path = - knigh_path;

			diagonal = i + knigh_path;
			anti_diagonal = i - knigh_path;

			if(i != j && j != diagonal && j != anti_diagonal){
				fprintf(fd, "(%d,%d)\n", i, j);
				continue;
			}
			else
				continue;
		}
	}
}

void constraint_damesgenerator(FILE * fd, int nb_var){
	for (int i = 1; i <= nb_var ; i++){
		for(int j = 1; j <= nb_var; j++){
			if(j == i) 		continue;
			if(i > j)		continue;
			else{
				fprintf(fd, "%d %d:\n", i,j);
				resolvecouple_damesgenerator(fd, nb_var, i, j);
			}
		}
	}
}

int dames_generator(FILE * fd, int nb_var){
	printf("Génération d'une instance du problème des dames en cours...\n");
	nb_var_generator(fd, nb_var);
	nb_constraint_generator(fd, nb_var);
	domain_generator(fd, nb_var, 2);
	constraint_damesgenerator(fd, nb_var);
	printf("Génération terminée\n");
	return 1;
}
//=========================== Fin Dames =============================

void echec_generation(int option){
	if(option == 1){ printf("Echec de la generation d'une instance du problème des pigeons\n");}
	if(option == 2){ printf("Echec de la generation d'une instance du problème des dames\n");}
}

void generate(FILE * fd, int option, int nb_var){
	int result = 0;
	if(option == 1){
		result = pigeons_generator(fd, nb_var);
		if(result < 1) echec_generation(option);
	}
	else if(option == 2){
		result = dames_generator(fd, nb_var);
		if(result < 1) echec_generation(option);
	}
	else{
		printf("option is not equal to 1 or 2\n");
		exit(-1);
	}
}
