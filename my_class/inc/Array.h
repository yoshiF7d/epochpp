#ifndef ARRAY_H
#define ARRAY_H

#include "utilities.h"
#include <stdarg.h>

typedef struct st_array{
	double *elem;
	int *dim;
	int len;
	int depth;
}Array_Sub;
typedef struct st_array *Array;
enum precision{p_double,p_float};

Array Array_create(int depth , ...);
Array Array_create2(int depth , int *dim);
Array Array_copy(Array array);
void Array_delete(Array array);
double Array_get(Array array,...);
void Array_set(Array array, double x, ...);
void Array_output(Array array, char *fileout, enum precision p);
Array Array_input(char *filein);
void Array_print(Array array);
void Array_fprint(Array array, FILE *fp, char *sep);
void Array_fprint_mod(Array array, FILE *fp, char *sep, int offset , int step, int depth);
void Array_vdelete(void *array);
#endif
