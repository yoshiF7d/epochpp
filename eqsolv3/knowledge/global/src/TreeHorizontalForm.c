#include "knowledge.h"
Expr TreeHorizontalForm(Expr expr){
	if(!expr->child){return expr;}
	TreeForm_modh(expr->child);
	output_flag = 0;
	return expr;
}
