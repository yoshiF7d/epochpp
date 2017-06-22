#include "knowledge.h"
Expr SlotSequence_pre(Expr expr){
	if(!expr->child){
		Expr_appendChild(expr,Integer_createInt(1));
	}
	return expr;
}
