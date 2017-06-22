#include "knowledge.h"
Expr ArrayQ(Expr expr){
	Expr child = expr->child;
	if(!child){return expr;}
	if(ArrayQ_mod(expr->child)){
		expr->symbol = SymbolTable_get(id_True);
	}else{
		expr->symbol = SymbolTable_get(id_False);
	}
	expr->child=NULL;
	Expr_deleteRoot(child);
	return expr;
}
int ArrayQ_mod(Expr expr){
	Expr e;
	int length;
	if(!expr){return 0;}
	if(expr->symbol->id!=id_List){return 0;}
	
	length = ArrayQ_mod(expr->child);
	for(e=expr->child->next;e;e=e->next){if(length != ArrayQ_mod(e)){return 0;}}
	return Expr_getLength(expr->child);
}
