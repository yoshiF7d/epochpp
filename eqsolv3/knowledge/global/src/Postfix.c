#include "knowledge.h"
Expr Postfix(Expr expr){
	Expr e;
	if(!expr->child){return expr;}
	if(!expr->child->next){return expr;}
	e = expr->child->next;
	Expr_isolate(expr->child);
	Expr_appendChild(e,expr->child);
	Expr_replace(expr,e);
	Expr_delete(expr);
	return Evaluate(e);
}
