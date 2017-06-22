#include "knowledge.h"
Expr Length(Expr expr){
	Expr e;
	int k;
	if(!expr->child){return expr;}
	e = Integer_createInt(Expr_getLength(expr->child->child));
	Expr_replace(expr,e);
	Expr_deleteRoot(expr);
	return e;
}
