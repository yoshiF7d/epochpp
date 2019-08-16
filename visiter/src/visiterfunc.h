#ifndef VISITERFUNC_H
#define VISITERFUNC_H

#include <LinkedList.h>
#include <Data.h>
#include <Data_FFT.h>
#include <HashTable.h>
#include <utilities.h>
#define LOAD_BARY
#define LOAD_BMAT
#include "Data_loadSDF.h"
#define LOAD_SDF
#include "Data_loadPNG.h"
#define LOAD_PNG

typedef struct st_funccontainer *FuncContainer;
typedef void (*Func)(LinkedList list);

FuncContainer FuncContainer_create(Func func);
Func FuncContainer_get(FuncContainer fc);
void FuncContainer_delete(void *fc);

typedef struct st_visiterinfo{
	HashTable hashtable;
	ReadOptions_Sub options;
	Data data;
	char *filename;
	unsigned char series;
}VisiterInfo_Sub;
typedef struct st_visiterinfo *VisiterInfo;

VisiterInfo VisiterInfo_create(HashTable ht, Data data, char *file);
HashTable VisiterInfo_getHashTable(VisiterInfo vi);
Data VisiterInfo_getData(VisiterInfo vi);
void VisiterInfo_setSeriesOn(VisiterInfo vi);
void VisiterInfo_setSeriesOff(VisiterInfo vi);
void VisiterInfo_setSeparator(VisiterInfo vi, char *sep);

unsigned char VisiterInfo_isSeriesOn(VisiterInfo vi);
char *VisiterInfo_getFileName(VisiterInfo vi);
char *VisiterInfo_getSeparator(VisiterInfo vi);

void VisiterInfo_delete(VisiterInfo vi);
char *VisiterInfo_printprompt(VisiterInfo vi,char *str,int len);
void VisiterInfo_print(VisiterInfo vi);

#define PROTOTYPE(func) void visiter_##func(LinkedList list);
#define APPEND(func) HashTable_append(ht,#func,visiter_##func);
#define SEPDEF " "

#define FUNCLIST(FUNC) \
FUNC(ave) \
FUNC(aveload) \
FUNC(sum) \
FUNC(min) \
FUNC(max) \
FUNC(print) \
FUNC(draw) \
FUNC(draw1d) \
FUNC(drawpng) \
FUNC(draw1dpng) \
FUNC(plot) \
FUNC(plot1d) \
FUNC(plotpng) \
FUNC(plot1dpng) \
FUNC(spot) \
FUNC(load) \
FUNC(unload) \
FUNC(series) \
FUNC(setsep) \
FUNC(shape) \
FUNC(add) \
FUNC(sub) \
FUNC(mul) \
FUNC(pow) \
FUNC(cslice) \
FUNC(rslice) \
FUNC(radd) \
FUNC(cadd) \
FUNC(rmul) \
FUNC(cmul) \
FUNC(cswap) \
FUNC(rswap) \
FUNC(rappend) \
FUNC(cappend) \
FUNC(rinsert) \
FUNC(cinsert) \
FUNC(rfourier) \
FUNC(cfourier) \
FUNC(fourier) \
FUNC(fourier2D) \
FUNC(fourierMask) \
FUNC(rpow) \
FUNC(cpow) \
FUNC(fwhm) \
FUNC(rrotate) \
FUNC(lrotate) \
FUNC(transpose) \
FUNC(vreflect) \
FUNC(hreflect) \
FUNC(export)

FUNCLIST(PROTOTYPE) /*prototype declaration*/

int check(Data data, int xs, int ys, int xe, int ye);
LinkedList parsefunc(char *buf);
char *funcdoc(void *content);
void printdoc(void *content);
void printbasicdoc();
void printdoc_imp(Func func);

#endif
