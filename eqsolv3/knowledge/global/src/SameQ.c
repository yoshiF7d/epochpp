#include "knowledge.h"
Expr SameQ(Expr expr){
	Expr child = expr->child,e,e2;
	if(!child){return expr;}
	if(SameQ_mod(child,child->next)){
		expr->symbol = SymbolTable_get(id_True);
	}else{
		expr->symbol = SymbolTable_get(id_False);
	}	
	for(e=child;e;e=e2){
		e2 = e->next;
		Expr_deleteRoot(e);
	}
	expr->child=NULL;
	return expr;
}

int SameQ_mod(Expr e1, Expr e2){
	Expr s1,s2;
	if(Expr_equals(e1,e2)){
		for(s1=e1->child,s2=e2->child;s1&&s2;s1=s1->next,s2=s2->next){
			if(!SameQ_mod(s1,s2)){return 0;}
		}
		if(s1 || s2){return 0;}
		return 1;
	}
	return 0;
}
