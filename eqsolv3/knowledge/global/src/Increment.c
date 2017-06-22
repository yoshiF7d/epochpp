#include "knowledge.h"
Expr Increment(Expr expr){
	Expr e,esav;
	if(!expr->child){return expr;}
	if(!expr->child->symbol->assigned){
		goto rvalue;
	}
	esav = Expr_copyRoot(expr->child->symbol->assigned);
	e = Expr_binary("Plus",expr->child->symbol->assigned,Integer_createInt(1));
	expr->child->symbol->assigned = Evaluate(e);
	Expr_deleteRoot(expr);
	return esav;
  rvalue:
	if(message_flag){
		fprintf(stderr,"Increment::rvalue: %s is not a variable with a value, so its value cannot be changed.",expr->child->symbol->name);
	}
	return expr;
}
