#include "knowledge.h"
Expr Help(Expr expr){
	Context c;
	if(expr->child){
		if(expr->child->symbol->id==id_String && !(expr->child->flag & ef_full)){
			for(c=SymbolTable_getContext();c;c=c->next){
				if(!strcmp(c->name,expr->child->data.string)){
					Context_print(c);
					break;
				}
			}
		}else{
			SymbolTable_print(expr->child->symbol);
		}
	}else{
		SymbolTable_printall();
	}
	output_flag=0;
	return expr;
}
