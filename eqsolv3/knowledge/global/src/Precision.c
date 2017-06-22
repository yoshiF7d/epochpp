#include "knowledge.h"
Expr Precision(Expr expr){
	Expr e;
	if(!expr->child){return expr;}
	switch(expr->child->symbol->id){
	  case id_Integer:
		e=Integer_createInt(expr->child->data.INT_PREC);
		break;
	  case id_Real:
		e=Integer_createInt(expr->child->data.REAL_PREC);
		break;
	  default:
		e=Expr_create(id_Infinity);
		break;
	}
	Expr_replace(expr,e);
	Expr_deleteRoot(expr);
	return e;
}
