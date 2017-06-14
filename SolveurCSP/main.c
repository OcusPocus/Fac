#include "generator.h"
#include "parser.h"
#include "ForwardChecking.h"

void usage(){
	printf("Le programme prend deux arguments : \n");
	printf("- Type de problème : \n");
	printf("1 = generation d une instance du problème des pigeons\n");
	printf("2 = generation d une instance du problème des dames\n");
	printf("- Nombre de Pigeons/Dames\n");
	exit(-1);
}

int main(int argc, char const *argv[])
{
	if(argc != 1 + 2){
		usage();
	}

	FILE * fd = fopen("fileprob.txt", "w");
	if(fd == NULL){
		printf("open file failed\n");
		exit(-1);
	}

	int option = atoi(argv[1]);

	if(option > 2 || option < 1){
		usage();
	}

	int nb_var = atoi(argv[2]);

	generate(fd, option, nb_var);
	fclose(fd);
	
	Domain * dom					= malloc(sizeof(Domain));
	EnsembleRelation * ens_relation = malloc(sizeof(EnsembleRelation));
	ConstraintVar * cons_var 		= malloc(sizeof(ConstraintVar));
	Stack * stack 					= malloc(sizeof(Stack));
	StateVector * sv 				= malloc(sizeof(StateVector));

	if(parse("fileprob.txt", option, dom, cons_var, ens_relation, sv, stack) != 1){
		printf("Parsing Failed\n");
	}
	forward_checking(dom, cons_var, sv, stack);
	return 0;
}