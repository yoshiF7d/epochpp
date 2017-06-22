#ifndef DATA_H
#define DATA_H

#include "utilities.h"

typedef struct st_data *Data;
typedef Data Data2D;

Data Data_create(int row, int column);
void Data_delete(Data data);

Data Data_load(char *file, int ofsr, int ofsc, int r, int c, char *sep);
Data Data_fload(FILE *fp, int ofsr, int ofsc, int r, int c, char *sep);
//Data Data_load2(char *file, int ofsr, int ofsc, int r, int c, char *separator);
Data Data_shape(Data data, int r, int c); 

double Data_get(Data data, int r, int c);
void Data_set(Data data, int r, int c, double x); 
double Data_translate(Data data, int r, int c, Data data2, int r2, int c2);
int Data_getSize(Data data);
int Data_getRow(Data data);
int Data_getColumn(Data data);

double Data_min(Data data, int c);
double Data_max(Data data, int c);

void Data_print(Data data);
void Data_fprint(Data data, FILE *fp, char *sep);
enum precision{p_double,p_float};
void Data_output(Data data, char *fileout, enum precision p);
Data Data_input(char *filein);

Data Data_rslice(Data data, int cs, int ce);
Data Data_cslice(Data data, int rs, int re);
Data Data_slice(Data data, int rs, int cs, int re, int ce);

Data Data_copy(Data s);
Data Data_add(Data a, Data b, Data tg);
Data Data_adds(Data a, double b, Data tg);
Data Data_sub(Data a, Data b, Data tg);
Data Data_subs(Data a, double b, Data tg);
Data Data_mul(Data a, Data b, Data tg);
Data Data_muls(Data a, double b, Data tg);
Data Data_pow(Data a, Data b, Data tg);
Data Data_pows(Data a, double b, Data tg);

Data Data_radd(Data a, int index, double b);
Data Data_cadd(Data a, int index, double b);
Data Data_rmul(Data a, int index, double b);
Data Data_cmul(Data a, int index, double b);

Data Data_rswap(Data a, int r1, int r2);
Data Data_cswap(Data a, int c1, int c2);
Data Data_rappend(Data data, Data dataap);
Data Data_cappend(Data data, Data dataap);
Data Data_rtake(Data data, int rs, int re);
Data Data_ctake(Data data, int cs, int ce);

Data Data_histogram(Data data, double min, double max, int binc);
Data Data_histogram2D(Data data, double min1, double min2, double max1, double max2, int binc1, int binc2);

Data Data_fourier(Data data, int isgn); /*Copyright(C) 1996-2001 Takuya OOURA*/	
void Data_sort(Data data, int key);
void Data_sort_module(Data data, int i, int j, int key);
int Data_sort_partition(Data data, double a, int i, int j, int key);
int Data_sort_pivot(Data data, int i, int j, int key);

/*Data2D methods*/
Data2D Data2D_create(int r);
void Data2D_delete(Data2D data);
Data2D Data2D_sort(Data2D data, int n);
Data2D Data2D_sort_mod(Data2D data, int i, int j);
Data2D Data2D_sort_partition(Data2D data, int i, int j, double a);
Data2D Data2D_sort_pivot(Data2D data, int i, int j, double a);
Data2D Data2D_swap(Data2D data, int i, int j);


#endif
