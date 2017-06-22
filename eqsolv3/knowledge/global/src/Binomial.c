#include "knowledge.h"

int Binomial_int(int n, int k){
	int km = n-k > k ? k : n-k;
	if(km<=0){return 1;}
	return n*Binomial_int(n-1,km-1)/km;
}

#define LOG2PI_2 0.918939
double log2binomial(int m, int n){
	return ((m + 0.5)*log(m) - (n + 0.5)*log(n) - (m - n + 0.5)*log(m-n) - LOG2PI_2)/LOG2;
}

Expr Binomial(Expr expr){
	Expr child = expr->child;
	int prec,m;
	if(Expr_getLength(child)!=2){return expr;}
	switch(child->symbol->id){
	  case id_Integer:
		switch(child->next->symbol->id){
		  case id_Integer:
			if(child->data.INT_PREC <= I32_LIMIT){
				if(child->next->data.INT_PREC <= I32_LIMIT){
					/*MPMP*/
					if( ( prec = log2binomial(child->data.INT_MP,child->next->data.INT_MP) ) > I32_LIMIT){
						mpz_init_set_si(child->data.INT_AP,m=child->data.INT_MP);
						child->data.INT_PREC = prec;
						mpz_bin_uiui(child->data.INT_AP,m,child->next->data.INT_MP);
					}else{
						child->data.INT_MP = Binomial_int(child->data.INT_MP,child->next->data.INT_MP);
					}
				}else{
					/*MPAP*/
					mpz_init_set_si(child->data.INT_AP,child->data.INT_MP);
					child->data.INT_PREC = child->next->data.INT_PREC;
					mpz_bin_ui(child->data.INT_AP,child->data.INT_AP,mpz_get_si(child->next->data.INT_AP));
				}
			}else{
				if(child->next->data.INT_PREC <= I32_LIMIT){
					/*APMP*/
					mpz_bin_ui(child->data.INT_AP,child->data.INT_AP,child->next->data.INT_MP);
				}else{
					/*APMP*/
					mpz_bin_ui(child->data.INT_AP,child->data.INT_AP,mpz_get_si(child->next->data.INT_AP));
				}
			}
			Expr_delete(child->next);
			Expr_replace(expr,child);
			Expr_delete(expr);
			expr = child;
			break;
		  default:
			break;
		}
		break;
	  default:
		break;
	}
	return expr;
}

