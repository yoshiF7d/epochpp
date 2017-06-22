#include "knowledge.h"
Expr Slot_pre(Expr expr){
	if(!expr->child){
		Expr_appendChild(expr,Integer_createInt(1));
	}
	return expr;
}
