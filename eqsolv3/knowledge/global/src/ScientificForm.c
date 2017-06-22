#include "knowledge.h"
Expr ScientificForm(Expr expr){
	char *buf;
	Expr child;
	int decimal;
	if(!expr->child){return expr;}	
	switch(expr->child->symbol->id){
	  case id_Integer:
		break;
	  case id_Real:
		if(expr->child->next){
			if(expr->child->next->symbol->id==id_Integer){
				decimal = (int)Integer_toInt(expr->child->next);
				if(expr->child->data.REAL_PREC <= R64_LIMIT){
					buf = real64_toString(expr->child->data.REAL_MP,decimal);
				}else{
					buf = mpfr_toString(expr->child->data.REAL_AP,decimal);
				}
				printf("%s\n",buf);
				deallocate(buf);
				output_flag = 0;
			}
		}
		break;
	  default:
		break;
	}
	if(expr->child->next){Expr_deleteRoot(expr->child->next);}
	expr->child->next=NULL;
	Expr_replace(expr,child=expr->child);
	Expr_delete(expr);
	return child;
}
