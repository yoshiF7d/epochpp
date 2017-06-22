#include "knowledge.h"
int Sign_mod(Expr expr){
	Expr e,prev,r=NULL;
	int s=1;
	
	if(!expr){return 0;}
	switch(expr->symbol->id){
	  case id_Integer:
		return Integer_sign(expr);
	  case id_Real:
		return Real_sign(expr);
	  case id_Times:
		for(e=expr->child;e;e=e->next){
			s*=Sign_mod(e);
		}
		break;
	  default:
		break;
	}
	return s;
}
