#include "knowledge.h"
Expr Divide(Expr expr){
	Expr e1,eo;
	int arglen;
	
	if(!expr->child){return expr;}
	arglen = Expr_getLength(expr->child);
	if(arglen!=2){return expr;}
	expr->symbol=SymbolTable_get(id_Times);
	e1 = Expr_create(id_Power);
	
	Expr_replace(eo=expr->child->next,e1);
	Expr_isolate(eo);
	Expr_appendChild(e1,eo);
	Expr_appendChild(e1,Integer_createInt(-1));
	return expr;
}
