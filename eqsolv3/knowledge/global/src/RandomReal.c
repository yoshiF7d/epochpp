#include "knowledge.h"
Expr RandomReal(Expr expr){
	Expr e;
	int len;
	double min=0,max=1;
	
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
				  case id_Real:
					min = Real_double(expr->child->child);
					break;
				  default:
					goto unitfr;
				}
				switch(expr->child->child->symbol->id){
				  case id_Integer:
					max = Integer_toInt(expr->child->child->next);
					break;
				  case id_Real:
					max = Real_double(expr->child->child->next);
					break;
				  default:
					goto unitfr;
				}
			}
			break;
		  case id_Real:
			max = Real_double(expr->child);
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
				e = RandomReal_mod_list(min,max,expr->child->next);
				break;
			  case id_List:
				e = RandomReal_mod_list(min,max,expr->child->next->child);
				break;
			  default:
				goto noopt;
			}
		}else{
			e = RandomReal_mod(min,max);
		}
		Expr_replace(expr,e);
		Expr_deleteRoot(expr);
		expr = e;
	}
	return expr;
  unitfr:
	fprintf(stderr,"The endpoints of the uniform distribution range are not real valued.");
	return expr;
  parb:
	fprintf(stderr,"RandomReal::parb : RandomReal should have between 0 and 2 parameters.");
	return expr;
  noopt:
	fprintf(stderr,"RandomReal::noopt : Options expected beyond position 2 in RandomReal.");
	return expr;
}
Expr RandomReal_mod_list(double min, double max, Expr list){
	Expr e,expr;
	int k;
	if(list){
		if(list->symbol->id == id_Integer){
			k = Integer_toInt(list);
			if(k<0){goto array;}
			expr = Expr_create(id_List);
			for(;k>0;k--){
				Expr_appendChild(expr,RandomReal_mod_list(min,max,list->next));
			}
		}else{
			goto array;
		}
	}else{
		expr = RandomReal_mod(min,max);
	}
	return expr;
  array:
	fprintf(stderr,"The array dimensions should be a list of non-negative machine-sized integers giving the dimensions for the result.");
	return NULL;
}
Expr RandomReal_mod(double min, double max){
	return Real_createDouble( min + (max-min)*randomReal() );
}
