#ifndef __UTIL__
#define __UTIL__

#include <stdio.h>
#include <stdlib.h>
#include "ModelCSP.h"

void echec_reading(char * elt_read, int value);
void echec_parse(int option);

void empty_tab(int * tab1, int * tab2, int size, int size2);
void print_tab(int * tab1, int * tab2, int size, int size2);
void test_allocation_tab(int result, int * tab1, int * tab2, int size, int size2);

void empty_mat(int ** matrice, int size, int size2);
void print_mat(int ** matrice, int size, int size2);
void test_allocation_mat(int result, int ** mat, int size, int size2);

void pop_var(Stack * stack);
void pop_value(Stack * stack);
void push_var(Stack * stack, int var);
void push_value(Stack * stack, int value);


#endif