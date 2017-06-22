#include "knowledge.h"
double Real_double(Expr expr){
	if(expr->symbol->id == id_Real){
		if(expr->data.REAL_PREC <= R64_LIMIT){
			return expr->data.REAL_MP;
		}else{
			return mpfr_get_d(expr->data.REAL_AP,MPFR_RNDN);
		}
	}
	return 0;
}
int Real_sign(Expr expr){
	if(expr->symbol->id == id_Real){
		if(expr->data.REAL_PREC <= R64_LIMIT){
			return (expr->data.REAL_MP > 0) - (expr->data.REAL_MP < 0);
		}else{
			return mpfr_sgn(expr->data.REAL_AP);
		}
	}
	return 0;
}

char *Real_toString(Expr expr){
	if(expr->flag & ef_full){
		return String_copy(expr->symbol->name);
	}else{
		if(expr->data.REAL_PREC>0){
			if(expr->data.REAL_PREC <= R64_LIMIT){
				//return real64_toString(expr->data.REAL_MP,expr->data.REAL_PREC);
				return real64_toString(expr->data.REAL_MP,6);
			}else{
				return mpfr_toString(expr->data.REAL_AP,(long)((expr->data.REAL_PREC)*LOG2+1));
			}
		}
	}
	return NULL;
}

char *real64_toString(double d, long decimal){
	int needed;
	char *buf,*com;
	//printf(KWHT "real64 : %ld\n" KNRM,prec);
		if(fabs(log10(d)) > 6){
		//needed = snprintf(NULL,0,"%%.%lde",(long)(prec*LOG2+1));
		needed = snprintf(NULL,0,"%%.%lde",decimal);
		com = allocate(needed+1);
		//snprintf(com,needed+1,"%%.%lde",(long)(prec*LOG2+1));
		snprintf(com,needed+1,"%%.%lde",decimal);
		needed = snprintf(NULL,0,com,d);
		buf = allocate(needed+1);
		snprintf(buf,needed+1,com,d);
		deallocate(com);
	}else{
		needed = snprintf(NULL,0,"%.6g",d);
		buf = allocate(needed+1);
		snprintf(buf,needed+1,"%.6g",d);
	}
	return buf;
}

char *mpfr_toString(mpfr_t op, long decimal){
	int needed; 
	char *buf,*com;
	long exp = mpfr_get_exp(op);
	//printf(KWHT "mpfr : %ld\n" KNRM,exp);
	exp = (labs(exp) > 3*LOG2) ? 'e' : 'g';
	//needed = snprintf(NULL,0,"%%.%ldRN%c",(long)(prec*LOG2+1),(char)exp);
	needed = snprintf(NULL,0,"%%.%ldRN%c",decimal,(char)exp);
	com = allocate(needed+1);
	snprintf(com,needed+1,"%%.%ldRN%c",decimal,(char)exp);
	//printf("%s\n",com);
	needed = mpfr_snprintf(NULL,0,com,op);
	buf = allocate(needed+1);
	mpfr_snprintf(buf,needed+1,com,op);
	deallocate(com);
	return buf;
}

long calcprec(const char *string){
	char *p=strrchr(string,'.');
	long k=1;
	if(p){p++;}
	else{p=(char*)string;}
	for(;*p;p++){
		if(isdigit(*p)){k++;}
		else{break;}
	}
	return k/LOG2;
}
Expr Real_create(char *string){
	Expr expr = Expr_create(id_Real);
	expr->data.REAL_PREC = calcprec(string);
	if(expr->data.REAL_PREC <= R64_LIMIT){
		expr->data.REAL_MP = atof(string);
	}else{
		mpfr_init2(expr->data.REAL_AP,expr->data.REAL_PREC);
		mpfr_set_str(expr->data.REAL_AP,string,0,MPFR_RNDN);
	}
	expr->flag = 0;
	return expr;
}

Expr Real_createDouble(double d){
	Expr expr = Expr_create(id_Real);
	expr->data.REAL_PREC = R64_LIMIT;
	expr->data.REAL_MP = d;
	expr->flag = 0;
	return expr;
}
void Real_data_init(union un_data *data){
	data->REAL_PREC= R64_LIMIT;
	data->REAL_MP = 0;
}
void Real_data_finish(union un_data *data){
	if(data->REAL_PREC > R64_LIMIT){mpfr_clear(data->REAL_AP);}
}
void Real_data_copy(union un_data *datas, union un_data *datad){
	if(datas->REAL_PREC <= R64_LIMIT){
		datad->REAL_MP = datas->REAL_MP;
	}else{
		mpfr_init_set(datad->REAL_AP,datas->REAL_AP,MPFR_RNDN);
	}
	datad->REAL_PREC = datas->REAL_PREC;
}
int Real_data_equals(union un_data *d1, union un_data *d2){
	if(d1->REAL_PREC <= R64_LIMIT){
		if(d2->REAL_PREC <= R64_LIMIT){
			if(fabs(d1->REAL_MP-d2->REAL_MP) < DBL_EPSILON){return 1;}
		}else{
			if(mpfr_cmp_si(d2->REAL_AP,d1->REAL_MP)==0){return 1;}
		}
	}else{
		if(d2->REAL_PREC <= R64_LIMIT){
			if(mpfr_cmp_si(d1->REAL_AP,d2->REAL_MP)==0){return 1;}
		}else{
			if(mpfr_cmp(d1->REAL_AP,d2->REAL_AP)==0){return 1;}
		}
	}
	return 0;
}
