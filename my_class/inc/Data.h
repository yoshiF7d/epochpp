#ifndef DATA_H
#define DATA_H

#include "utilities.h"

#define TRUE 1
#define FALSE 0
#define AUTO -1
typedef struct st_data{
	double **elem;
	int row;
	int column;
}Data_Sub;
typedef struct st_data *Data;
typedef Data Data2D;

typedef struct st_readoptions{
	int rows;
	int columns;
	int rowOffset;
	int columnOffset;
	char **lineSeparators;
	int lineSeparatorsCount;
	char **fieldSeparators;
	int fieldSeparatorsCount;
	int repeatedSeparators;
	int ignoreEmptyLines;
}ReadOptions_Sub;
typedef struct st_readoptions *ReadOptions;

void ReadOptions_print(ReadOptions options);
void ReadOptions_init(ReadOptions options);
void ReadOptions_finish(ReadOptions options);
void ReadOptions_setLineSeparators(ReadOptions options, int lineSeparatorsCount, ...);
void ReadOptions_setFieldSeparators(ReadOptions options, int fieldSeparatorsCount, ...);

Data Data_create(int row, int column);
void Data_vdelete(void *data);
void Data_delete(Data data);

Data Data_load(char *file, int ofsr, int ofsc, int r, int c, char *sep);
Data Data_fload(FILE *fp, int ofsr, int ofsc, int r, int c, char *sep);
Data Data_sload(FILE *fp, int ofsr, int ofsc, int r, int c, char *fsep, char *lsep);

Data Data_read(char *string, ReadOptions options);
Data Data_readFile(char *file, ReadOptions options);
Data Data_shape(Data data, int r, int c); 

double Data_get(Data data, int r, int c);
void Data_set(Data data, int r, int c, double x);
void Data_setall(Data data, double x);
double Data_translate(Data data, int r, int c, Data data2, int r2, int c2);
int Data_getSize(Data data);
int Data_getRow(Data data);
int Data_getColumn(Data data);

double Data_min(Data data, int c);
int Data_minIndex(Data data, int c);
double Data_max(Data data, int c);
int Data_maxIndex(Data data, int c);
void Data_min2(Data data, int c, int imin, int imax, int *mini, double *min);
void Data_max2(Data data, int c, int imin, int imax, int *maxi, double *max);
double Data_fwhm(Data data, double xmin, double xmax, double *xl, double *xr);

void Data_print(Data data);
void Data_fprint(Data data, FILE *fp, char *sep);
enum precision{p_double,p_float,p_char};
void Data_output(Data data, char *fileout, enum precision p);
Data Data_input(char *filein);
Data Data_loadBMAT(char *filein);
Data Data_loadBARY(char *filein);
void Data_plot(Data data, int rs, int re, int cs, int ce, char *comfile);

Data Data_rslice(Data data, int cs, int ce);
Data Data_cslice(Data data, int rs, int re);
Data Data_slice(Data data, int rs, int cs, int re, int ce);

Data Data_copy(Data s);
Data Data_add(Data a, Data b, Data tg, int rs, int cs, int re, int ce);
Data Data_adds(Data a, double b, Data tg);
Data Data_sub(Data a, Data b, Data tg, int rs, int cs, int re, int ce);
Data Data_subs(Data a, double b, Data tg);
Data Data_mul(Data a, Data b, Data tg, int rs, int cs, int re, int ce);
Data Data_muls(Data a, double b, Data tg);
Data Data_pow(Data a, Data b, Data tg, int rs, int cs, int re, int ce);
Data Data_pows(Data a, double b, Data tg);

Data Data_radd(Data a, int index, double b);
Data Data_cadd(Data a, int index, double b);
Data Data_rmul(Data a, int index, double b);
Data Data_cmul(Data a, int index, double b);
Data Data_rpow(Data a, int index, double b);
Data Data_cpow(Data a, int index, double b);

Data Data_rswap(Data a, int r1, int r2);
Data Data_cswap(Data a, int c1, int c2);
Data Data_rappend(Data data, Data dataap);
Data Data_cappend(Data data, Data dataap);
Data Data_rtake(Data data, int rs, int re);
Data Data_ctake(Data data, int cs, int ce);

Data Data_transpose(Data data);
Data Data_rrotate(Data data);
Data Data_lrotate(Data data);
void Data_vreflect(Data data);
void Data_hreflect(Data data);

Data Data_histogram(Data data, double min, double max, int binc);
Data Data_histogram2D(Data data, double min1, double min2, double max1, double max2, int binc1, int binc2);

Data Data_fourier(Data data, int isgn); /*Copyright(C) 1996-2001 Takuya OOURA*/	
Data Data_fourier2(Data data, int isgn, int k);

/*Data2D methods*/
Data2D Data2D_create(int r);
void Data2D_delete(Data2D data);
Data2D Data2D_sort(Data2D data, int n);
Data2D Data2D_sort_mod(Data2D data, int i, int j);
Data2D Data2D_sort_partition(Data2D data, int i, int j, double a);
Data2D Data2D_sort_pivot(Data2D data, int i, int j, double a);
Data2D Data2D_swap(Data2D data, int i, int j);

typedef Data F2Data;
F2Data F2Data_convert(Data data, int k);
void F2Data_delete(F2Data data);
Data F2Data_powerSpectrum(F2Data f2data);
void F2Data_transpose(F2Data f2data);
void Data_fourier2D(F2Data f2data, int isgn);
#endif
