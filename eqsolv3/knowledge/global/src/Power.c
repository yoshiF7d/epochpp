#include "knowledge.h"
Expr Power(Expr expr){
	Expr e,next,e0,e1;
	mpfr_t real;
	mpz_t num,den;
	
	int arglen;
	
	if(!expr->child){return expr;}
	arglen = Expr_getLength(expr->child);
	
	if(arglen>2){
		e = Expr_create(id_Power);
		next = expr->child->next;
		next->previous = NULL;
		expr->child->next = NULL;
		Expr_appendChild(expr,Expr_appendChild(e,next));
		return Power(expr);
	}else if(arglen==2){
		e0 = expr->child;
		e1 = expr->child->next;
		if(e0->symbol->id == id_Power && e0->child){
			/*(a^b)^c*/
			if(e1->symbol->id == id_Integer){
				/*(a^b)^(int) -> a^(int*b)*/
				Expr_replace(next=e0->child->next,e=Expr_create(id_Times));
				Expr_appendChild(e,next);
				e0->next = NULL; Expr_isolate(e1);
				Expr_appendChild(e,e1);
				Expr_replace(expr,e0);
				Expr_delete(expr);
				return Evaluate(e0);
			}
		}
		if(NumberQ_mod(e0)){
			if(NumberQ_mod(e1)){
				if(Sign_mod(e0)==0){
					/*reuse arglen*/
					arglen = Sign_mod(e1);
					if(arglen<0){
						/*0^(neg) = Infinity*/
						Expr_delete(e0);
						Expr_delete(e1);
						expr->symbol = SymbolTable_get(id_ComplexInfinity);
						expr->child=NULL;
						dividedbyzero();
						return expr;
					}else if(arglen==0){
						/*0^0 = Indeterminate*/
						Expr_delete(e0);
						Expr_delete(e1);
						expr->symbol = SymbolTable_get(id_Indeterminate);
						expr->child=NULL;
						return expr;
					}else{goto ignore_e1;}
				}else if(Power_mod(e0,e1)){
					e1->symbol = SymbolTable_get(id_Integer);
					e1->data.INT_PREC = I32_LIMIT;
					e1->data.INT_MP = -1;
				}else{
				  ignore_e1:
					Expr_delete(e1);
					e0->next = NULL;
					Expr_replace(expr,e0);
					Expr_delete(expr);
					expr=e0;
				}
			}
		}
	}
	return expr;
}
/*returns 1 when e2 is negative*/
int Power_mod(Expr e1, Expr e2){
  recast:
	Expr_cast(e1,e2);
	long prec;
	int i;
	double d;
	
	if(e1->symbol->id == id_Integer){
		if(e1->data.INT_PREC <= I32_LIMIT){
			/*overflow detection*/
			prec = fabs(e2->data.INT_MP*log2(fabs((double)e1->data.INT_MP)));
			if(prec > I32_LIMIT){
				e1->data.INT_PREC = e2->data.INT_PREC = prec;
				i = e1->data.INT_MP;
				mpz_init_set_si(e1->data.INT_AP,i);
				i = e2->data.INT_MP;
				mpz_init_set_si(e2->data.INT_AP,i);
				//printf(KCYN "Power : uppercast : I32 -> I%ld\n" KNRM,prec);
				goto uppercast_i;
			}
			if(e2->data.INT_MP < 0){
				e2->data.INT_MP = -e2->data.INT_MP;
				e1->data.INT_MP = ipow(e1->data.INT_MP,e2->data.INT_MP);
				return 1;
			}else{
				e1->data.INT_MP = ipow(e1->data.INT_MP,e2->data.INT_MP);
				return 0;
			}
		}else{
		  uppercast_i:
			if(mpz_sgn(e2->data.INT_AP)<0){
				mpz_neg(e2->data.INT_AP,e2->data.INT_AP);
				mpz_pow_ui(e1->data.INT_AP,e1->data.INT_AP,Integer_toInt(e2));
				mpz_clear(e2->data.INT_AP);
				return 1;
			}else{
				mpz_pow_ui(e1->data.INT_AP,e1->data.INT_AP,Integer_toInt(e2));
				return 0;
			}
		}
	}else if(e1->symbol->id == id_Real){
		if(e1->data.REAL_PREC <= R64_LIMIT){
			/*overflow detection*/
			prec = fabs(e2->data.REAL_MP*log2(fabs(e1->data.REAL_MP)));
			if(prec > R64_LIMIT){
				e1->data.REAL_PREC = prec;
				d = e1->data.REAL_MP;
				mpfr_init2(e1->data.REAL_AP,prec);
				mpfr_set_d(e1->data.REAL_AP,d,MPFR_RNDN);
				d = e2->data.REAL_MP;
				e2->data.REAL_PREC = prec;
				mpfr_init2(e2->data.REAL_AP,prec);
				mpfr_set_d(e2->data.REAL_AP,d,MPFR_RNDN);
				//printf(KCYN "Power : uppercast : R64 -> R%ld\n" KNRM,prec);
				goto uppercast_r;
			}
			e1->data.REAL_MP = pow(e1->data.REAL_MP,e2->data.REAL_MP);
			return 0;
		}else{
			uppercast_r:
			mpfr_pow(e1->data.REAL_AP,e1->data.REAL_AP,e2->data.REAL_AP,MPFR_RNDN);
			return 0;
		}
	}
	return 0;
}

