#include "knowledge.h"
Expr Sin(Expr expr){
	Expr child = expr->child;
	if(!child){return expr;}
	switch(child->symbol->id){
	  case id_Real:
		if(child->data.REAL_PREC <= R64_LIMIT){
			child->data.REAL_MP = sin(child->data.REAL_MP);
		}else{
			mpfr_sin(child->data.REAL_AP,child->data.REAL_AP,MPFR_RNDN);
		}
		Expr_replace(expr,child);
		Expr_delete(expr);
		expr = child;
		break;
	  default:
		break;
	}
	return expr;
}
