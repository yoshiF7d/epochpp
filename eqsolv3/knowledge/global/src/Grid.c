#include "knowledge.h"
Expr Grid_mod(Expr expr){
	Form f = NULL, g = NULL;
	Expr e,d;
	for(e=expr->child;e;e=e->next){
		g = NULL;
		for(d=e->child;d;d=d->next){
			g=Form_appendHorizontal(g,Form_make(d,0));
		}
		f=Form_appendVerticalCenter(f,g);
	}
	Form_print(f);
	Form_delete(f);
	return expr;
}
Expr Grid(Expr expr){
	Expr e,child = expr->child;
	if(!child){return expr;}
	if(child->symbol->id != id_List){return expr;}
	for(e=child;e;e=e->next){
		if(e->symbol->id != id_List){return expr;}
	}
	Grid_mod(child);
	output_flag = 0;
	return expr;
}
