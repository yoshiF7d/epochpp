#include "knowledge.h"
Expr N(Expr expr){
	Expr child,e;
	int arglen;
	int prec=R64_LIMIT*LOG2;
	if(!expr->child){return expr;}
	arglen = Expr_getLength(child=expr->child);
	if(arglen>2){return expr;}
	if(arglen==2){
		if(!NumberQ_mod(child->next)){
			goto precbd;
		}
		prec = Integer_toInt(child->next);
	}
	child=N_mod(child,prec/LOG2);
	Expr_deleteRoot(child->next);
	Expr_replace(expr,child);
	Expr_delete(expr);
	return Evaluate(child);
  precbd:
	if(message_flag){
		fprintf(stderr,"N::precbd: Requested precision a is not a machine-sized integer\n");
	}
	return expr;
}
Expr N_mod(Expr expr, long prec){
	Expr e;
	int i;
	double d;
	mpz_t z;
	//fprintf(stderr,"N_mod : prec : %ld\n",prec);
	switch(expr->symbol->id){
	  case id_Pi:
		if(prec <= R64_LIMIT){
			expr->data.REAL_MP=M_PI;
		}else{
			mpfr_init2(expr->data.REAL_AP,prec);
			mpfr_const_pi(expr->data.REAL_AP,MPFR_RNDN);
		}
		break;
	  case id_E:
		if(prec <= R64_LIMIT){
			calce64(&expr->data.REAL_MP,prec);
		}else{
			mpfr_init2(expr->data.REAL_AP,prec);
			calce(expr->data.REAL_AP,prec);
		}
		break;
	  case id_Integer:
		if(prec <= R64_LIMIT){
			if(expr->data.INT_PREC <= I32_LIMIT){
				i = expr->data.INT_MP;
				expr->data.REAL_MP = (double)i;
			}else{
				mpz_init_set(z,expr->data.INT_AP);
				mpz_clear(expr->data.INT_AP);
				expr->data.REAL_MP = mpz_get_d(z);
				mpz_clear(z);
			}
		}else{
			if(expr->data.INT_PREC <= I32_LIMIT){
				i = expr->data.INT_MP;
				mpfr_init2(expr->data.REAL_AP,prec);
				mpfr_set_si(expr->data.REAL_AP,i,MPFR_RNDN);
			}else{
				mpz_init_set(z,expr->data.INT_AP);
				mpz_clear(expr->data.INT_AP);
				mpfr_init2(expr->data.REAL_AP,prec);
				mpfr_set_z(expr->data.REAL_AP,z,MPFR_RNDN);
				mpz_clear(z);
			}
		}
		break;
	  case id_Real:
		if(prec <= R64_LIMIT){
			if(expr->data.REAL_PREC > R64_LIMIT){
				d=mpfr_get_d(expr->data.REAL_AP,MPFR_RNDN);
				mpfr_clear(expr->data.REAL_AP);
				expr->data.REAL_MP=d;
			}else{
				
			}
		}else{
			if(expr->data.REAL_PREC <= R64_LIMIT){
				d=expr->data.REAL_MP;
				mpfr_init2(expr->data.REAL_AP,prec);
				mpfr_set_d(expr->data.REAL_AP,d,MPFR_RNDN);
			}else{
				
			}
		}
		break;
	  default:
		for(e=expr->child;e;e=e->next){e=N_mod(e,prec);}
		return expr;
	}
	expr->symbol=SymbolTable_get(id_Real);
	expr->data.REAL_PREC=prec;
	expr->flag &=  (~ ef_full);
	return expr;
}
void calce64(double *real, long prec){
	unsigned long i;
	unsigned long ic=1;
	double e=2;
	for(i=2;i<ULONG_MAX;i++){
		ic *= i;
		e += 1.0/ic;
		if(i>2){if(*real==e){break;}}
		*real=e;
	}
}
void calce(mpfr_t real, long prec){
	mpz_t a;
	mpfr_t e,b;
	unsigned long i;
	
	mpz_init(a);
	mpfr_init2(e,prec);
	mpfr_init2(b,prec);
	mpfr_set_ui(e,2,MPFR_RNDN);
	
	for(i=2;i<ULONG_MAX;i++){
		mpz_fac_ui(a,i);
		mpfr_set_ui(b,1,MPFR_RNDN);
		mpfr_div_z(b,b,a,MPFR_RNDN);
		mpfr_add(e,e,b,MPFR_RNDN);
		if(i>2){if(mpfr_cmp(real,e)==0){break;}}
		mpfr_set(real,e,MPFR_RNDN);
	}
	mpz_clear(a);
	mpfr_clear(b);
	mpfr_clear(e);
}
