#include "knowledge.h"
Expr AtomQ(Expr expr){
	Expr child = expr->child;
	if(!child){return expr;}
	if(AtomQ_mod(expr->child)){
		expr->symbol = SymbolTable_get(id_True);
	}else{
		expr->symbol = SymbolTable_get(id_False);
	}
	expr->child=NULL;
	Expr_deleteRoot(child);
	return expr;
}

int AtomQ_mod(Expr expr){
	switch(expr->symbol->id){
	  case id_Integer:
	  case id_Real:
	  case id_String:
		return 1;
	  default:
		break;
	}
	return 0;
}
