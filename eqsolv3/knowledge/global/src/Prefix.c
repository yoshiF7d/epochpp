#include "knowledge.h"
Expr Prefix(Expr expr){
	Expr child=expr->child,next;
	if(!child){return expr;}
	next = child->next;
	if(!next){return expr;}
	Expr_isolate(child);
	Expr_appendChild(child,next);
	Expr_replace(expr,child);
	Expr_delete(expr);
	return Evaluate(child);
}
