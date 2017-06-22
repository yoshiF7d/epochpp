#include "knowledge.h"
Expr Set(Expr expr){
	Expr e,next,rtn;
	if(!expr->child){return expr;}
	if(!expr->child->next){return expr;}
	if(!AtomQ_mod(expr->child)){
		if(expr->child->symbol->attributes & Protected){
			fprintf(stderr,"Set::wrsym: symbol %s is protected\n",expr->child->symbol->name);
			return expr;
		}
		if(expr->child->symbol->assigned){
			for(e=expr->child->symbol->assigned;e;e=e->next){
				if(e->symbol->id != id_Rule){
					fprintf(stderr,"symbol->assigned supposed to have list of rules.\n");
				}
				if(SameQ_mod(expr->child,e->child)){
					Expr_deleteRoot(e->child->next);
					e->child->next=NULL;
					Expr_appendChild(e,Expr_copyRoot(expr->child->next));
					goto end;
				}
			}
		}
		expr->child->symbol->assigned = 
			Expr_append(expr->child->symbol->assigned,
				Expr_appendChild(
					Expr_create(id_Rule),
					Expr_append(Expr_copyRoot(expr->child),Expr_copyRoot(expr->child->next))
				)
		);
	  end:
		rtn = expr->child->next;
		next = expr->child->next->next;
		expr->child->next->next = NULL;
		Expr_deleteRoot(expr->child);
		for(e=next;e;e=next){
			next = e->next;
			Expr_deleteRoot(e);
		}
		Expr_replace(expr,rtn);
		Expr_delete(expr);
		expr = rtn;
	}else{
		fprintf(stderr,"Set::setraw: Cannot assign to raw object\n");
	}
	return expr;
}
