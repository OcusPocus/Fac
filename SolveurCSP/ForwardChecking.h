#ifndef __FC__
#define __FC__

#include <stdio.h>
#include <stdlib.h>
#include "ModelCSP.h"
#include "util.h"

void forward_checking(Domain * dom, ConstraintVar * cons_var, StateVector * sv, Stack * stack);

#endif