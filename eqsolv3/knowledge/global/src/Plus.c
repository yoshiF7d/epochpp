#include "knowledge.h"
#define DELETE_AND_PROCEED(p,e) \
if(e->previous){\
e->previous->next=e->next;\
prev=e->previous;\
if(e->next){e->next->previous=prev;}\
}else{\
prev=p->child=e->next;\
if(prev){e->next->previous=NULL;}\
else{perror("DELETE_AND_PROCEED : error\n");}\
}\
Expr_deleteRoot(e);\
e=prev

Expr Plus(Expr expr){
	Expr prev,e;
	Expr ei=NULL,ei32=NULL,er=NULL,er64=NULL;
	Expr es=NULL,ct=NULL,cs=NULL,vs=NULL,c=NULL,v=NULL,c1=NULL,c2=NULL;
	
	int flag=0;
	int i;
	long prec;
	
	if(!expr->child){return expr;}
	for(e=expr->child;e;e=e->next){
		switch(e->symbol->id){
		  case id_Integer:
			if(e->data.INT_PREC <= I32_LIMIT){
				if(!ei32){ei32 = e;}
				else{
					ei32->data.INT_MP += e->data.INT_MP;
					DELETE_AND_PROCEED(expr,e);
					/*overflow prediction*/
					prec = (int)fabs(log2(fabs((double)ei32->data.INT_MP)));
					if(prec == I32_LIMIT){
						/*danger of overflow approches*/
						/*add ei32 content to ei and clear ei32*/
						if(ei){
							if(ei32->data.INT_MP>0){
								mpz_add_ui(ei->data.INT_AP,ei->data.INT_AP,ei32->data.INT_MP);
							}else{
								mpz_sub_ui(ei->data.INT_AP,ei->data.INT_AP,-ei32->data.INT_MP);
							}
							Expr_deleteChild(expr,ei32);
						}else{
							i=ei32->data.INT_MP;
							ei32->data.INT_PREC=prec*2;
							mpz_init_set_si(ei32->data.INT_AP,i);
							ei=ei32;
						}
						ei32=NULL;
					}
				}
			}else{
				if(!ei){ei = e;}
				else{
					mpz_add(ei->data.INT_AP,ei->data.INT_AP,e->data.INT_AP);
					DELETE_AND_PROCEED(expr,e);
				}
			}
			break;
		  case id_Real:
			if(e->data.REAL_PREC <= R64_LIMIT){
				if(!er64){er64 = e;}
				else{
					er64->data.REAL_MP += e->data.REAL_MP;
					DELETE_AND_PROCEED(expr,e);
				}
			}else{
				if(!er){er = e;}
				else{
					mpfr_add(er->data.REAL_AP,er->data.REAL_AP,e->data.REAL_AP,MPFR_RNDN);
					DELETE_AND_PROCEED(expr,e);
				}
			}
			break;
		  default:
			break;
		}
	}
	/*every data are gatherd in ei*/
	Plus_mod(&ei,ei32);
	Plus_mod(&ei,er64);
	Plus_mod(&ei,er);
	if(ei!=ei32){Expr_deleteChild(expr,ei32); ei32=NULL;}
	if(ei!=er){Expr_deleteChild(expr,er); er=NULL;}
	if(ei!=er64){Expr_deleteChild(expr,er64); er64=NULL;}
	ei=NULL;
	do{
		es = NULL; cs=NULL; vs=NULL; flag = 0;
		for(e=expr->child;e;e=e->next){
			if(e->parent && !AtomQ_mod(e)){
				if(!es){
					coeff2(e,&cs,&vs);
					es = e;
				}else{
					c=NULL; v=NULL;
					coeff2(e,&c,&v);
					if(es->child && e->child){
						for(c1=vs,c2=v;c1 || c2;c1=c1->next, c2=c2->next){
							if(Order_mod(c1,c2)!=0){goto jump;}
						}
					}else if(Order_mod(v,vs)!=0){
						goto jump;
					}else if((es->child && vs->next) || (e->child && v->next)){
					  jump:
						continue;
					}
					flag=1;
					c1 = cs;
					c2 = c;
					if(!c1){cs = Integer_createInt(1);}
					if(!c2){c = Integer_createInt(1);}
					Plus_mod(&cs,c);
					if(!c2){Expr_delete(c);}
					DELETE_AND_PROCEED(expr,e);
				}
			}
		}
		if(flag){
			if(es->symbol->id!=id_Times){
				Expr_replace(es,e=Expr_create(id_Times));
				es->next=NULL;
				Expr_appendChild(e,cs);
				Expr_appendChild(e,es);
				es = e;
			}else{
				for(e=es->child;e;e=e->next){if(e==cs){flag=0;}}
				if(flag){Expr_prependChild(es,cs);}
			}
			es = Times(es);
			es->parent=NULL;
		}else if(es){
			es->parent=NULL;
		}else{
			for(e=expr->child;e;e=e->next){e->parent = expr;}
			break;
		}
	}while(1);
	//TreeForm_mod(expr);
	if(expr->child->next){
		for(e=expr->child;e;e=e->next){
			switch(e->symbol->id){
			  case id_Integer:
				if(Integer_toInt(e)==0){DELETE_AND_PROCEED(expr,e);}
				break;
			  case id_ComplexInfinity:
			  case id_Infinity:
				e = Expr_copy(e);
				Expr_replace(expr,e);
				Expr_deleteRoot(expr);
				break;
			  default:
				break;
			}
		}
	}
	if(!expr->child->next){
		Expr_replace(expr,e=expr->child);
		Expr_delete(expr);
		expr=e;
	}
	return expr;
}

void Plus_mod(Expr *e1, Expr e2){
	if(!(*e1)){*e1 = e2; return;}
	if(!e2){return;}
	Expr_cast(*e1,e2);
	if((*e1)->symbol->id == id_Integer){
		if((*e1)->data.INT_PREC <= I32_LIMIT){
			(*e1)->data.INT_MP += e2->data.INT_MP;
		}else{
			mpz_add((*e1)->data.INT_AP,(*e1)->data.INT_AP,e2->data.INT_AP);
		}
	}else if((*e1)->symbol->id == id_Real){
		if((*e1)->data.REAL_PREC <= R64_LIMIT){
			(*e1)->data.REAL_MP += e2->data.REAL_MP;
		}else{
			mpfr_add((*e1)->data.REAL_AP,(*e1)->data.REAL_AP,e2->data.REAL_AP,MPFR_RNDN);
		}
	}
}

void coeff2(Expr expr, Expr *c, Expr *v){
	*c = NULL; 
	*v = expr;
	if(!expr){return;}
	if(expr->symbol->id==id_Times){
		if(NumberQ_mod(expr->child)){
			*c = expr->child;
			*v = expr->child->next;
		}else{
			*c = NULL;
			*v = expr->child;
		}
	}else if(NumberQ_mod(expr)){
		*c = expr;
		*v = NULL;
	}
	return;
}

