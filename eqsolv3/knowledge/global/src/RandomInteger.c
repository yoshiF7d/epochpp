#include "knowledge.h"
Expr RandomInteger(Expr expr){
	Expr e;
	int len;
	
	long min=0, max=1;
	
	if(expr->child){
		switch(expr->child->symbol->id){
		  case id_List:
			len = Expr_getLength(expr->child->child);
			if(len > 2){
				goto parb;
			}else{
				switch(expr->child->child->symbol->id){
				  case id_Integer:
					min = Integer_toInt(expr->child->child);
					break;
				  default:
					goto unitfr;
				}
				switch(expr->child->child->symbol->id){
				  case id_Integer:
					max = Integer_toInt(expr->child->child->next);
					break;
				  default:
					goto unitfr;
				}
			}
			break;
		  case id_Integer:
			max = Integer_toInt(expr->child);
		  case id_Null:
			break;
		  default:
			goto unitfr;
		}
		if(expr->child->next){
			if(expr->child->next->next){
				goto noopt;
			}
			switch(expr->child->next->symbol->id){
			  case id_Integer:
				e = RandomInteger_mod_list(min,max,expr->child->next);
				break;
			  case id_List:
				e = RandomInteger_mod_list(min,max,expr->child->next->child);
				break;
			  default:
				goto noopt;
			}
		}else{
			e = RandomInteger_mod(min,max);
		}
		Expr_replace(expr,e);
		Expr_deleteRoot(expr);
		expr = e;
	}
	return expr;
  unitfr:
	fprintf(stderr,"RandomInteger::unitfr : The endpoints of the uniform distribution range are not integers.");
	return expr;
  parb:
	fprintf(stderr,"RandomInteger::parb : RandomInteger should have between 0 and 2 parameters.");
	return expr;
  noopt:
	fprintf(stderr,"RandomInteger::noopt : Options expected beyond position 2 in RandomInteger.");
	return expr;
}
Expr RandomInteger_mod_list(long min, long max, Expr list){
	Expr e,expr;
	int k;
	if(list){
		if(list->symbol->id == id_Integer){
			k = Integer_toInt(list);
			if(k<0){goto array;}
			expr = Expr_create(id_List);
			for(;k>0;k--){
				Expr_appendChild(expr,RandomInteger_mod_list(min,max,list->next));
			}
		}else{
			goto array;
		}
	}else{
		expr = RandomInteger_mod(min,max);
	}
	return expr;
  array:
	fprintf(stderr,"RandomInteger::array : The array dimensions should be a list of non-negative machine-sized integers giving the dimensions for the result.");
	return NULL;
}
Expr RandomInteger_mod(long min, long max){
	unsigned long ran=0;
	while((max-min+1) > ran){
		ran += randomInteger();
	}
	return Integer_createLong(min + ran % (max-min+1));
}
