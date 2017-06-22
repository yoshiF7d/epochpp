#include "knowledge.h"
Expr ArrayDepth(Expr expr){
	Expr e;
	if(!expr->child){return expr;}
	Expr_replace(expr,e=Integer_createInt(ArrayDepth_mod(expr->child)));
	Expr_deleteRoot(expr);
	return e;
}
int ArrayDepth_mod(Expr expr){
	Expr e;
	int length,min;
	if(!expr->child){return 0;}
	length = Expr_getLength(expr->child->child);
	for(e=expr->child->next;e;e=e->next){
		if(length != Expr_getLength(e->child)){
			length=0; break;
		}
	}
	if(length==0){return 1;}
	min = ArrayDepth_mod(expr->child);
	for(e=expr->child->next;e;e=e->next){
		length = ArrayDepth_mod(e);
		if(length < min){min = length;}
	}
	return min+1;
}
