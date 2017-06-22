#include "knowledge.h"
Expr Sequence(Expr expr){
	Expr e;
	Expr_replace(expr,e=expr->child);
	Expr_delete(expr);
	return e;
}
