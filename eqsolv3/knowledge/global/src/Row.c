#include "knowledge.h"
Expr Row_mod(Expr expr, Expr sep){
	Form f = NULL;
	Expr e;
	for(e=expr->child;e;e=e->next){
		f=Form_appendHorizontal(f,Form_make(e,0));
		if(sep && e->next){f=Form_appendHorizontal(f,Form_make(sep,0));}
	}
	Form_print(f);
	Form_delete(f);
	return expr;
}
Expr Row(Expr expr){
	Expr child = expr->child;
	if(!child){return expr;}
	if(child->symbol->id != id_List){return expr;}
	Row_mod(child,child->next);
	output_flag = 0;
	return expr;
}
