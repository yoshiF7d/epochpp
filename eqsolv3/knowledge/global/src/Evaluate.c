#include "knowledge.h"
Expr Evaluate(Expr expr){
	Expr e=NULL,prev;
	if(!expr){return NULL;}
	if(expr->symbol->assigned){
		for(e=expr->symbol->assigned;e;e=e->next){
			//TreeForm_mod(e);
			expr = ReplaceAll_mod(expr,prev=Expr_copyRoot(e));
			Expr_deleteRoot(prev);
		}
	}
  reeval:
	if(!expr->symbol){return expr;}
	if(!(expr->symbol->attributes & SequenceHold) && expr->symbol->id == id_Sequence){
		expr = Sequence(expr); goto reeval;
	}
	if(!(expr->symbol->attributes & HoldAllComplete)){
		for(e = expr->child;e;e=e->next){
			if(e->symbol->id == id_Sequence){e = Sequence(e);}
		}
	}
	if(expr->symbol->attributes & Listable){
		//Expr_printTree(expr);
		e=Thread_mod(expr,NULL,NULL);
		if(e!=expr){
			expr=e;
			goto reeval;
		}
		expr=e;
	}
	if(expr->symbol->attributes & Flat){
		expr = Flatten_mod(expr,NULL,0,-1);
		//Expr_printTree(expr);
	}
	//printAttributes(sd->attributes);
	//puts("e1");
	if(!((expr->symbol->attributes & HoldAllComplete) || (expr->symbol->attributes & HoldAll)) && expr->child){
		if(!(expr->symbol->attributes & HoldFirst)){
			expr->child = Evaluate(expr->child);
		}
		if(!(expr->symbol->attributes & HoldRest) && (e=expr->child->next)){
			for(;e;e=e->next){
				e = Evaluate(e);
			}
		}
	}
	//puts("e2");
	if(expr->symbol->attributes & Orderless){
		expr->child = Sort_mod(expr->child);
	}
	if(expr->symbol->builtinrules){
		/*Builtin Rule*/
		e = Expr_copyRoot(expr->symbol->builtinrules);
		do{
			prev = expr;
			expr = Evaluate(Replace_mod(expr,e,0,0));
		}while(SameQ_mod(prev,expr));
		Expr_deleteRoot(e);
		//TreeForm_mod(expr);
		//TreeForm_mod(expr);
	}
	//puts("e3");
	if(!expr->symbol->function){return expr;}
	if(expr->symbol->id!=id_Evaluate){
		//printf("Symbol:%s is defined\n",node->data.symbol);
		expr = expr->symbol->function(expr);
	}else{
		Expr_replace(expr,e=expr->child);
		Expr_delete(expr);
		expr = e;
	}
	return expr;
}
