#include "knowledge.h"
Expr ArcTan(Expr expr){
	Expr child = expr->child,e;
	if(!child){return expr;}
	if(child->next){
		if(child->symbol->id == child->next->symbol->id){
			if(child->symbol->id == id_Real){
				Expr_cast(child,child->next);
				if(child->data.REAL_PREC <= R64_LIMIT){
					child->data.REAL_MP = atan2(child->next->data.REAL_MP,child->data.REAL_MP);
				}else{
					mpfr_atan2(child->data.REAL_AP,child->next->data.REAL_AP,child->data.REAL_AP,MPFR_RNDN);
				}
				for(e=child->next;e;e=e->next){
					Expr_deleteRoot(e);
				}
				Expr_replace(expr,child);
				Expr_delete(expr);
				expr = child;
			}
		}
	}else{
		switch(child->symbol->id){
		  case id_Integer:
			/*do nothing*/ break;
		  case id_Real:
			if(child->data.REAL_PREC <= R64_LIMIT){
				child->data.REAL_MP = atan(child->data.REAL_MP);
			}else{
				mpfr_atan(child->data.REAL_AP,child->data.REAL_AP,MPFR_RNDN);
			}
			Expr_replace(expr,child);
			Expr_delete(expr);
			expr = child;
			break;
		  case id_Times:
			break;
		}
	}
	return expr;
}
