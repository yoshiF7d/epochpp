#include "knowledge.h"
Expr PatternB(Expr expr){
	Expr e,child;
	if(!(child=expr->child)){
		expr->symbol = SymbolTable_get(id_Blank);
		expr = Blank_pre(expr);
	}else if(!child->next){
		expr->symbol = SymbolTable_get(id_Pattern);
		expr->data.pattern.symbol = child->symbol;
		expr->data.pattern.object = Expr_create(id_Blank);
		Expr_deleteRoot(expr->child);
		expr->child = NULL;
	}else if(expr->flag & ef_func){
		expr->symbol = SymbolTable_get(id_Pattern);
		expr->data.pattern.symbol = child->symbol;
		expr->data.pattern.object = Expr_create(id_Blank);
		Expr_replace(expr->child,expr->child->next);
		Expr_delete(child);
	}else{
		expr->symbol = SymbolTable_get(id_Pattern);
		expr->data.pattern.symbol = child->symbol;
		expr->data.pattern.object = Blank_pre(Expr_appendChild(Expr_create(id_Blank),child->next));
		Expr_deleteRoot(expr->child);
		expr->child = NULL;
	}
	return expr;
}
