#include "knowledge.h"
Expr NumberQ(Expr expr){
	Expr e;
	if(NumberQ_mod(expr->child)){
		e = Expr_create(id_True);
		Expr_replace(expr,e);
		Expr_deleteRoot(expr);
		expr = e;
	}else{
		e = Expr_create(id_False);
		Expr_replace(expr,e);
		Expr_deleteRoot(expr);
		expr = e;
	}
	return expr;
}

int NumberQ_mod(Expr expr){
	if(!expr){return 0;}
	switch(expr->symbol->id){
	  case id_Integer:
	  case id_Real:
		if(!(expr->flag & ef_full)){
			return 1;
		}
	  default:
		return 0;
	}
}
