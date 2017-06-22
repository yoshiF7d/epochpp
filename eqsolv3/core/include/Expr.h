#ifndef Expr_H
#define Expr_H

#include "utilities.h"
#include "SymbolTable.h"
#include "data.h"

/*double : 64 = 1 + 11 + 52 (sign,exponent,prec)*/
/*used for overflow prediction*/
#define R64_LIMIT 52
#define I32_LIMIT 31
#define LOG2 0.301029995663981
#define PREC_SYMBOL -1
#define PREC_MATCH -2

enum en_eflag{
	ef_func=0x0001, /*to distinguish expressions like a[] and a*/
	ef_full=0x0002, /*to distinguish expressions like ^ and Power*/
	ef_mark=0x0004, /*this flag can be used in any way. marked node become red color when printed with TreeForm_mod*/
};

/*Any data can be stored in expr->data.*/
/*
When you define un_data, data type with large size should not be directly defined instead 
pointer of it should be defined, otherwise all Expr nodes become large size.
*/
/*
data type of expr->data must be known from symbol->id.
accessing expr->data as different data type is not blocked and won't produce compile error,
and might even run time error, but must be avoided.
*/

typedef struct st_expr{
	Symbol symbol;
	union un_data data; /*defined in data.h*/
	int flag; /*when you need to distinguish exprs having the same symbol, this flag is used*/
	struct st_expr *next;
	struct st_expr *previous;
	struct st_expr *child;
	struct st_expr *parent;
}Expr_Sub;
//typedef struct st_expr *Expr;

Expr Expr_create(int id);
Expr Expr_createByName(char *string);
Expr Expr_createBySymbol(Symbol symbol);
Expr Expr_createString(char *string);
/*
Expr Expr_createSymbol(Symbol symbol);
Expr Expr_createInternal(char *string);
Expr Expr_createString(char *string);
Expr Expr_createInteger(char *string);
Expr Expr_createExpInteger(char *string);
Expr Expr_createReal(char *string);
Expr Expr_createInteger32(int k);
Expr Expr_createReal64(double d);
*/
void Expr_cast(Expr e1,Expr e2);

void Expr_delete(Expr expr);
void Expr_deleteRoot(Expr expr);
void Expr_deleteChild(Expr expr, Expr child);
void Expr_deleteString(Expr expr);
#define Expr_unmember(expr,e) \
if(expr){\
	if(expr->previous){\
		expr->previous->next = e->next;\
		if(e->next){e->next->previous = expr;}\
	}else{expr=expr->next;}\
}
void Expr_replace(Expr oldme, Expr newme);
void Expr_replacelist(Expr oldme, Expr newlist);
Expr Expr_isolate(Expr expr);
int Expr_getLength(Expr expr);
int Expr_equals(Expr e1, Expr e2);

void Expr_debug(Expr expr);
char *Expr_toString(Expr expr);
char *Expr_getColor(Expr expr);
Expr Expr_append(Expr expr, Expr expr2);
Expr Expr_appendChild(Expr expr, Expr expr2);
Expr Expr_prependChild(Expr expr, Expr expr2);
Expr Expr_getChild(Expr expr, int i);
Expr Expr_getParent(Expr expr);
Expr Expr_insert(Expr p, Expr c, Expr e);
Expr Expr_copy(Expr expr);
Expr Expr_copyRoot(Expr expr);
Expr Expr_binary(char *symbol, Expr expr, Expr expr2);
Expr Expr_unary(char *symbol, Expr expr);
#endif
