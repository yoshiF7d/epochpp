#include "knowledge.h"
long Integer_toInt(Expr expr){
	if(expr->symbol->id == id_Integer){
		if(expr->data.INT_PREC <= I32_LIMIT){
			return expr->data.INT_MP;
		}else{
			return mpz_get_si(expr->data.INT_AP);
		}
	}
	return 0;
}
int Integer_sign(Expr expr){
	if(expr->symbol->id == id_Integer){
		if(expr->data.INT_PREC <= I32_LIMIT){
			return (expr->data.INT_MP > 0) - (expr->data.INT_MP < 0);
		}else{
			return mpz_sgn(expr->data.INT_AP);
		}
	}
	return 0;
}
char *Integer_toString(Expr expr){
	char *buf = NULL;
	int needed;
	if(expr->flag & ef_full){
		return String_copy(expr->symbol->name);
	}else{
		if(expr->data.INT_PREC>0){
			if(expr->data.INT_PREC <= I32_LIMIT){
				return itoa(expr->data.INT_MP);
			}else{
				return mpz_get_str(NULL,10,expr->data.INT_AP);
			}
		}
	}
	return NULL;
}

long calcprecint(const char *string){
	long k = String_countmatch((char*)string,isdigit);
	return k/LOG2+1;
}

Expr Integer_create(char *string){
	Expr expr = Expr_create(id_Integer);
	expr->data.INT_PREC = calcprecint(string);
	if(expr->data.INT_PREC <= I32_LIMIT){
		expr->data.INT_MP = atoi(string);
	}else{
		mpz_init_set_str(expr->data.INT_AP,string,10);
	}
	return expr;
}

Expr Integer_createExp(char *string){
	char *p,*psav,sav;
	mpz_t z;
	int e;

	Expr expr = Expr_create(id_Integer);
	
	for(p=string;*p;p++){
		if(*p=='e' || *p=='E'){
			psav=p; sav=*p; *p='\0';
			p++;
			if(*p=='+'){p++;}
			break;
		}
	}
	if(p){e=atoi(p);}
	else{e=1;}
	//printf("e : %d\n",e);
	//printf("string : %s\n",string);
	expr->data.INT_PREC = e*calcprecint(string);
	
	if(expr->data.INT_PREC <= I32_LIMIT){
		expr->data.INT_MP = atoi(string)*pow(10,e);
		//printf("integer32 : %d\n",expr->data.integer32);
	}else{
		mpz_init_set_str(expr->data.INT_AP,string,10);
		mpz_init(z);
		mpz_ui_pow_ui(z,10,e);
		mpz_mul(expr->data.INT_AP,expr->data.INT_AP,z);
		mpz_clear(z);
	}
	*psav=sav;
	return expr;
}

Expr Integer_createInt(int k){
	Expr expr = Expr_create(id_Integer);
	expr->data.INT_PREC = I32_LIMIT;
	expr->data.INT_MP = k;
	return expr;
}

Expr Integer_createLong(long k){
	Expr expr = Expr_create(id_Integer);
	expr->data.INT_PREC = I64_LIMIT;
	mpz_init_set_ui(expr->data.INT_AP,k);
	return expr;
}

void Integer_data_init(union un_data *data){
	data->INT_PREC = I32_LIMIT;
	data->INT_MP = 0;
}
void Integer_data_finish(union un_data *data){
	if(data->INT_PREC > I32_LIMIT){mpz_clear(data->INT_AP);}
}
void Integer_data_copy(union un_data *datas, union un_data *datad){
	if(datas->INT_PREC <= I32_LIMIT){
		datad->INT_MP = datas->INT_MP;
	}else{
		mpz_init_set(datad->INT_AP,datas->INT_AP);
	}
	datad->INT_PREC = datas->INT_PREC;
}
int Integer_data_equals(union un_data *d1, union un_data *d2){
	if(d1->INT_PREC <= I32_LIMIT){
		if(d2->INT_PREC <= I32_LIMIT){
			if(d1->INT_MP == d2->INT_MP){return 1;}
		}else{
			if(mpz_cmp_si(d2->INT_AP,d1->INT_MP)==0){return 1;}
		}
	}else{
		if(d2->INT_PREC <= I32_LIMIT){
			if(mpz_cmp_si(d1->INT_AP,d2->INT_MP)==0){return 1;}
		}else{
			if(mpz_cmp(d1->INT_AP,d2->INT_AP)==0){return 1;}
		}
	}
	return 0;
}
