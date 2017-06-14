#ifndef __GENERATOR__
#define __GENERATOR__

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "ModelCSP.h"

void nb_var_generator(FILE * fd, int nb_var);
void nb_constraint_generator(FILE * fd, int nb_var);
void domain_generator(FILE * fd, int nb_var, int option);

//================= Pigeons ==========================
void domain_pigeongenerator(FILE * fd, int nb_var);
void resolvecouple_pigeongenerator(FILE * fd, int nb_var);
void constraint_pigeongenerator(FILE * fd, int nb_var);
int pigeons_generator(FILE * fd, int nb_var);

//================= Fin Pigeons ======================

//================= Dames ==========================
void resolvecouple_damesgenerator(FILE * fd, int nb_var, int first_var, int second_var);
void constraint_damesgenerator(FILE * fd, int nb_var);
void domain_damesgenerator(FILE * fd, int nb_var);
int dames_generator(FILE * fd, int nb_var);
//================= Fin Dames ======================

void echec_generation(int option);
void generate(FILE * fd, int option, int nb_var);
#endif
