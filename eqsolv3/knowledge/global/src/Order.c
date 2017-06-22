#include "knowledge.h"
/*
Integer vs Integer
i32 vs i
Integer vs Real
i32 vs r64
i32 vs r
i vs r64
i vs r
Real vs Real
r64 && r
*/
#define IVSI(e1,e2) \
if(e1->data.INT_PREC <= I32_LIMIT){ \
	if(e2->data.INT_PREC <= I32_LIMIT){ \
		return (e1->data.INT_MP > e2->data.INT_MP) - (e2->data.INT_MP > e1->data.INT_MP); \
	}else{ \
		return -1*mpz_cmp_si(e2->data.INT_AP,e1->data.INT_MP); \
	} \
}else{ \
	if(e2->data.INT_PREC <= I32_LIMIT){ \
		return mpz_cmp_si(e1->data.INT_AP,e2->data.INT_MP); \
	}else{ \
		return mpz_cmp(e1->data.INT_AP,e2->data.INT_AP); \
	} \
}

#define IVSR(e1,e2,s) \
if(e1->data.INT_PREC <= I32_LIMIT){ \
	if(e2->data.REAL_PREC <= R64_LIMIT){ \
		return s*((e1->data.INT_MP > e2->data.REAL_MP) - (e2->data.REAL_MP > e1->data.INT_MP)); \
	}else{ \
		return -1*s*mpfr_cmp_si(e2->data.REAL_AP,e1->data.INT_MP); \
	} \
}else{ \
	if(e2->data.REAL_PREC <= R64_LIMIT){ \
		return s*mpz_cmp_d(e1->data.INT_AP,e2->data.REAL_MP); \
	}else{ \
		return -1*s*mpfr_cmp_z(e2->data.REAL_AP,e1->data.INT_AP); \
	} \
}

#define RVSR(e1,e2) \
if(e1->data.REAL_PREC <= R64_LIMIT){ \
	if(e2->data.REAL_PREC <= R64_LIMIT){ \
		return (e1->data.REAL_MP > e2->data.REAL_MP) - (e2->data.REAL_MP > e1->data.REAL_MP); \
	}else{ \
		return -1*mpfr_cmp_d(e2->data.REAL_AP,e1->data.REAL_MP); \
	} \
}else{ \
	if(e2->data.REAL_PREC <= R64_LIMIT){ \
		return mpfr_cmp_d(e1->data.REAL_AP,e2->data.REAL_MP); \
	}else{ \
		return mpfr_cmp(e1->data.REAL_AP,e2->data.REAL_AP); \
	} \
}
int Order_mod(Expr e1, Expr e2){
	int tie=0;
	char *c1,*c2;
	Expr e,x1=NULL,x2=NULL;
	Expr v1=NULL,v2=NULL,cf1=NULL,cb1=NULL,cf2=NULL,cb2=NULL;
	//puts("==5");
	if(!e1 && !e2){return 0;}
	else if(e1 && !e2){return 1;}
	else if(!e1 && e2){return -1;}
	else{
		switch(e1->symbol->id){
		  default:
			if(e1->symbol->id==id_Times || e1->symbol->id==id_Power ||
			   e2->symbol->id==id_Times || e2->symbol->id==id_Power){
					coeff(e1,&v1,&x1,&cf1,&cb1);
					/*printf(KBLU "1c1\n" KNRM); OutputForm_mod(cf1);
					printf(KBLU "1c2\n" KNRM); OutputForm_mod(cb1);
					printf(KBLU "1v\n" KNRM); OutputForm_mod(v1);
					printf(KBLU "1e\n" KNRM); OutputForm_mod(x1);
					putchar('\n');*/
					coeff(e2,&v2,&x2,&cf2,&cb2);
					/*printf(KBLU "2c1\n" KNRM); OutputForm_mod(cf2);
					printf(KBLU "2c2\n" KNRM); OutputForm_mod(cb2);
					printf(KBLU "2v\n" KNRM); OutputForm_mod(v2);
					printf(KBLU "2e\n" KNRM); OutputForm_mod(x2);
					putchar('\n');*/
					//puts("==5.1");
					tie = Order_mod(v1,v2); if(tie!=0){return tie;}
					//puts("==5.2");
					tie = Order_mod(x1,x2); if(tie!=0){return tie;}
					//puts("==5.3");
					tie = Order_mod(cf1,cf2); if(tie!=0){return tie;}
					//puts("==5.4");
					tie = Order_mod(cb1,cb2); if(tie!=0){return tie;}
			}
		  case id_String:
			/*fall through*/ if(e1->symbol->id != id_String){c1 = e1->symbol->name;}
			else{c1 = e1->data.string;}
			switch(e2->symbol->id){
			  case id_String:
				c2 = e2->data.string;
				break;
			  case id_Integer:
			  case id_Real:
				return 1;
			  default:
				c2 = e2->symbol->name;
			}
			tie = strcmp(c1,c2);
			if(tie==0){tie=1;break;}
			else{return tie>0 ? 1 : -1;}
		  case id_Integer:
			switch(e2->symbol->id){
			  case id_Integer:
				IVSI(e1,e2)
			  case id_Real:
				IVSR(e1,e2,1)
			  default:
				return -1;
			}
		  case id_Real:
			switch(e2->symbol->id){
			  case id_Integer:
				IVSR(e2,e1,-1)
			  case id_Real:
				RVSR(e1,e2)
			  default:
				return -1;
			}
			break;
		  case id_Pattern:
			c1 = e1->data.pattern.symbol->name;
			switch(e2->symbol->id){
			  case id_Pattern:
				c2 = e2->data.pattern.symbol->name;
				tie =strcmp(c1,c2);
				if(tie==0){
					tie = Order_mod(e1->data.pattern.object,e2->data.pattern.object);
					if(tie==0){tie=1;break;}
					else{return tie>0 ? 1 : -1;}
				}else{return tie>0 ? 1 : -1;}
			  case id_Integer:
			  case id_Real:
				return 1;
			  case id_String:
				c2 = e2->data.string;
			  default:
				c2 = e2->symbol->name;
				tie =strcmp(c1,c2);
				if(tie==0){tie=1;break;}
				else{return tie>0 ? 1 : -1;}
			}
		}
	}
	if(tie){
		if(!e1->child && !e2->child){return 0;}
		else if(e1->child && !e2->child){return 1;}
		else if(!e1->child && e2->child){return -1;}
		else{
			tie = Expr_getLength(e1->child) - Expr_getLength(e2->child);
			if(tie == 0){
				for(x1=e1->child,x2=e2->child;x1&&x2;x1=x1->next,x2=x2->next){
					tie = Order_mod(x1,x2);
					if(tie!=0){return tie;}
				}
			}
		}
	}
	return tie;
}

#undef IVSI
#undef IVSR
#undef RVSR

void coeff(Expr expr, Expr *v, Expr *e, Expr *c1, Expr *c2){
	Expr x, vs=NULL,es=NULL,vss=NULL;
	*v = NULL; *e = NULL; *c1 = NULL; *c2 = NULL;
	if(!expr){return;}
	
	switch(expr->symbol->id){
	  case id_String:
		*v = expr;
		*c1 = NULL;
		*c2 = NULL;
		*e = NULL;
		break;
	  case id_Integer:
	  case id_Real:
		*v = NULL;
		*c1 = expr;
		*c2 = NULL;
		*e = NULL;
		break;
		/*this code expect terms are sorted*/
		/*a*c*b <- not sorted.  a*b*c <- sorted*/
	  case id_Times:
		if(expr->child){
			/*find most significant term*/
			/*"a * b * c^2" -> "c^2 is most significant"*/
			for(x=expr->child;x;x=x->next){
				coeff(x,v,e,c1,c2);
				if(Order_mod(*e,es)>0){
					vs = *v; 
					es = *e;
				}
				/*save first variable*/
				if(!vss){vss=*v;}
			}
			if(es){*v=vs;}
			*e = es;
			/*if vss is not first v, then there are number in front of v. store it in c1 (front coeffcient)*/
			if(vss !=expr->child){*c1 = expr->child;}
			else{*c1 = NULL;}
			/*store terms succeeding the significant term*/
			if(vs){*c2 = vs->next;}
			else{*c2 = NULL;}
		}
		return;
	  case id_Power:
		if(expr->child){
			if(expr->child->next){
				*v = expr->child;
				*c1 = NULL;
				*c2 = NULL;
				*e = expr->child->next;
				break;
			}
		}
		*v = expr;
		*c1 = NULL;
		*c2 = NULL;
		*e = NULL;
		break;
	}
}


Expr Order(Expr expr){
	Expr e;
	if(!expr->child){return expr;}
	if(!expr->child->next){
		fprintf(stderr,"Order::argr: Order called with 1 argument; 2 arguments are expected.");
		return expr;
	}
	e = Integer_createInt(Order_mod(expr->child,expr->child->next));
	Expr_replace(expr,e);
	Expr_isolate(expr);
	Expr_deleteRoot(expr);
	return e;
}
