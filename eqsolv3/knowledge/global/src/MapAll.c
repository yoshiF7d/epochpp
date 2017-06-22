#include "knowledge.h"

Expr MapAll(Expr expr){
	Expr e,func,body;
	int depth;
	func = expr->child;
	if(!func){return expr;}
	body = func->next;
	if(!body){goto argtu;}
	
	e = Map_mod(func,body,1,Depth_mod(expr),1);
	Expr_replace(expr,e);
	Expr_deleteRoot(expr);
	return e;
  argtu:
	if(message_flag){
		fprintf(stderr,"MapAll ::argtu : MapAll called with 1 argument; 2 or 3 arguments are expected.");
	}
	return expr;
}
 