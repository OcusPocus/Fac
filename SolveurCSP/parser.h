#ifndef __PARSER__
#define __PARSER__

#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "ModelCSP.h"


//=========================== Pigeons ===============================

int parse_pigeondata(FILE * fd, Domain * dom, ConstraintVar * cons_var, EnsembleRelation * ens_relation, StateVector * sv, Stack * stack);

//===========================End Pigeons ============================

//================= Dames ==========================

int parse_damesdata(FILE * fd, Domain * dom, ConstraintVar * cons_var, EnsembleRelation * ens_relation, StateVector * sv, Stack * stack);

//================= Fin Dames ======================

int parse(char * filename, int option, Domain * dom, ConstraintVar * cons_var, EnsembleRelation * ens_relation, StateVector * sv, Stack * stack);

#endif
