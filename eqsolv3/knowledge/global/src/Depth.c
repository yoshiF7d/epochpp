#include "knowledge.h"
int Depth_mod(Expr expr){
	Expr e;
	int depth=1,max=0,m;
	for(e=expr->child;e;e=e->next){
		max = 1;
		if(max < (m = Depth_mod(e))){
			max = m;
		}
	}
	depth += max;
	return depth;
}
Expr Depth(Expr expr){
	Expr e,child = expr->child;
	int depth;
	if(!child){return expr;}
	depth = Depth_mod(expr);
	for(e=child;e;e=e->next){
		Expr_deleteRoot(e);
	}
	e = Integer_createInt(depth);
	Expr_replace(expr,e);
	Expr_delete(expr);
	return e;
}
