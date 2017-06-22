#include "knowledge.h"
Expr Minus(Expr expr){
	if(!expr->child){return expr;}
	expr->symbol = SymbolTable_get(id_Times);
	Expr_prependChild(expr,Integer_createInt(-1));
	return expr;
}
