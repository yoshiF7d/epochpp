#include "knowledge.h"
Expr First(Expr expr){
	Expr e;
	if(!expr->child){return expr;}
	Expr_replace(expr,e=Expr_copyRoot(expr->child));
	Expr_deleteRoot(expr);
	return e;
}
Expr First_mod(Expr expr){
	Expr e;
	for(e=expr;e->previous;e=e->previous){}
	return e;
}
