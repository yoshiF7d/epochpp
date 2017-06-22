#include "knowledge.h"
Expr DebugForm(Expr expr){
	Expr e;
	if(!expr->child){return expr;}
	Expr_debug(expr->child);
	putchar('\n');
	output_flag=0;
	return expr;
}
