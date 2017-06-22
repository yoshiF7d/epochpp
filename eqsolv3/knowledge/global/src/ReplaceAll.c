#include "knowledge.h"
Expr ReplaceAll(Expr expr){
	Expr e,child=expr->child,rules;
	if(!child){return expr;}
	rules = child->next;
	if(!rules){goto argbu;}	
	Expr_replace(expr,child=ReplaceAll_mod(child,rules));
	for(e=rules;e;e=e->next){Expr_deleteRoot(e);}
	Expr_delete(expr);
	return Evaluate(child);
  argbu:
	if(message_flag){
		fprintf(stderr,"Replace::argbu: Replace called with 1 argument; between 2 and 4 arguments are expected.\n");
	}
	return expr;
}

Expr ReplaceAll_mod(Expr expr, Expr rules){
	Expr r,pat,rep;
	int i;
	if(!rules){return expr;}
	if(rules->symbol->id == id_List){
		for(r=rules->child;r;r=r->next){
			expr = ReplaceAll_mod(expr,r);
		}
		return expr;
	}
	if(Expr_getLength(rules->child)!=2){return expr;}
	pat = rules->child;
	rep = rules->child->next;
	expr = ReplaceAll_mod_mod(expr,pat,rep);
	return expr;
}
Expr ReplaceAll_mod_mod(Expr expr, Expr pat, Expr rep){
	Expr e,cpy,list=NULL,child=expr->child;
	if(mark(expr,pat,NULL,0)==mqr_match){
		Expr_replace(expr,cpy=Replace_copy_mod(list=Replace_makelist(list,pat),rep));
		Expr_deleteRoot(expr);
		for(e=list;e;e=e->next){Expr_delete(e);}
		expr = cpy;
	}else{
		unmark(expr);
		expr->child=NULL;
		if(mark(expr,pat,NULL,0)==mqr_match){
			Expr_replace(expr,cpy=Replace_copy_mod(list=Replace_makelist(list,pat),rep));
			Expr_appendChild(cpy,child);
			Expr_delete(expr);
			for(e=list;e;e=e->next){Expr_delete(e);}
			expr = cpy;
		}else{
			expr->child = child;
		}
		unmark(expr);
	  gochild:
		for(e=child;e;e=e->next){
			e = ReplaceAll_mod_mod(e,pat,rep);
		}
	}
	return expr;
}
