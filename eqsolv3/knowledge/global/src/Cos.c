#include "knowledge.h"
Expr Cos(Expr expr){
	Expr child = expr->child;
	if(!child){return expr;}
	switch(child->symbol->id){
	  case id_Real:
		if(child->data.REAL_PREC <= R64_LIMIT){
			child->data.REAL_MP = cos(child->data.REAL_MP);
		}else{
			mpfr_cos(child->data.REAL_AP,child->data.REAL_AP,MPFR_RNDN);
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
