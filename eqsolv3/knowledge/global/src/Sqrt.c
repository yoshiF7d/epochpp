#include "knowledge.h"
Expr Sqrt(Expr expr){
	Expr tn;
	int arglen;
	
	if(!expr->child){return expr;}
	arglen = Expr_getLength(expr->child);
	if(arglen!=1){return expr;}
	expr->symbol = SymbolTable_get(id_Power);
	Expr_appendChild(expr,Expr_binary("Divide",Integer_createInt(1),Integer_createInt(2)));
	expr = Evaluate(expr);
	return expr;
}
