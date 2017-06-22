#include "knowledge.h"
Expr PatternB3(Expr expr){
	Expr e,child;
	if(!(child=expr->child)){
		expr->symbol = SymbolTable_get(id_Blank3);
		expr = Blank_pre(expr);
	}else if(!child->next){
		expr->symbol = SymbolTable_get(id_Pattern);
		expr->data.pattern.symbol = child->symbol;
		expr->data.pattern.object = Expr_create(id_Blank3);
		for(e=child;e;e=e->next){Expr_deleteRoot(e);}
		expr->child = NULL;
	}else{
		expr->symbol = SymbolTable_get(id_Pattern);
		expr->data.pattern.symbol = child->symbol;
		expr->data.pattern.object = Blank_pre(Expr_appendChild(Expr_create(id_Blank3),child->next));
		Expr_deleteRoot(expr->child);
		expr->child = NULL;
	}
	return expr;
}
