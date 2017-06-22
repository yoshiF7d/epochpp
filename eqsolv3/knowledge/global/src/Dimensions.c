#include "knowledge.h"
Expr Dimensions(Expr expr){
	Expr rtn;
	int level=-1;
	if(!expr->child){return expr;}
	if(expr->child->next){
		if(expr->child->next->symbol->id == id_Integer && !(expr->child->next->flag & ef_func)){
			level=Integer_toInt(expr->child->next);
		}else if(expr->child->next->symbol->id == id_Infinity){
			level=-1;
		}else{
			goto innf;
		}
	}
	rtn = Expr_create(id_List);
	if(level<0){level = ArrayDepth_mod(expr->child);}
	Expr_replace(expr,rtn=Dimensions_mod(rtn,expr->child,level));
	Expr_deleteRoot(expr);
	return rtn;
  innf:
	fprintf(stderr,"Dimensions::innf: Non-negative integer or infinity expected at position 2 in ");
	FullForm_mod2(stderr,expr);
	return expr;
}
Expr Dimensions_mod(Expr rtn, Expr expr, int level){
	Expr e;
	int length;
	if(!expr){return rtn;}
	if(!expr->child || level==0){return rtn;}
	length = Expr_getLength(expr->child->child);
	for(e=expr->child->next;e;e=e->next){
		if(length != Expr_getLength(e->child)){
			length=0; break;
		}
	}
	rtn = Expr_appendChild(rtn,Integer_createInt(Expr_getLength(expr->child)));
	if(length==0){return rtn;}
	return Dimensions_mod(rtn,expr->child,level-1);
}
