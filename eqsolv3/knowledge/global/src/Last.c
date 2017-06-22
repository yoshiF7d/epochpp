#include "knowledge.h"
Expr Last(Expr expr){
	Expr e;
	if(!expr->child){return expr;}
	Expr_replace(expr,e=Expr_copyRoot(Last_mod(expr->child)));
	Expr_deleteRoot(expr);
	return e;
}
Expr Last_mod(Expr expr){
	for(;expr->next;expr=expr->next){}
	return expr;
}
