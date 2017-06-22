#include "knowledge.h"
#define ATT_LIST(ATT,VAL) if(expr->child->symbol->attributes & ATT){Expr_appendChild(list,Expr_create(id_##ATT));}
Expr Attributes(Expr expr){
	Expr list;
	if(!expr->child){return expr;}
	list = Expr_create(id_List);
	ATT_TABLE(ATT_LIST)
	Expr_replace(expr,list);
	Expr_delete(expr);
	return list;
}
