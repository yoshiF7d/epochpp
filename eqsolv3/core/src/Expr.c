#include "Expr.h"
#include "knowledge.h"
/*
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

long calcprecint(const char *string){
	long k = String_countmatch((char*)string,isdigit);
	return k/LOG2+1;
}
*/

Expr Expr_createByName(char *string){
	//fprintf(stderr,"Expr_create\n");
	Expr expr = allocate(sizeof(Expr_Sub));
	expr->symbol = SymbolTable_getByName(string);
	/*rational,complex is created by combining integer or real numbers. some function will do this*/
	/*you should set all connections coherent.*/
	/*don't make situations like following, "parent's parent is me." "next's previous is not me" ...etc*/
	expr->flag = 0;
	expr->next = NULL;
	expr->previous = NULL;	
	expr->child = NULL;
	expr->parent = NULL;
	data_init(&(expr->data),expr->symbol->id);
	debug(KCYN "creating %s\n" KNRM,expr->symbol->name);
	return expr;
}

Expr Expr_createBySymbol(Symbol symbol){
	Expr expr = allocate(sizeof(Expr_Sub));	
	expr->symbol = symbol;
	expr->flag = 0;
	expr->next = NULL;
	expr->previous = NULL;	
	expr->child = NULL;
	expr->parent = NULL;
	data_init(&(expr->data),symbol->id);
	debug(KCYN "creating %s\n" KNRM,expr->symbol->name);
	return expr;
}

Expr Expr_create(int id){
	Expr expr = allocate(sizeof(Expr_Sub));	
	expr->symbol = SymbolTable_get(id);
	expr->flag = 0;
	expr->next = NULL;
	expr->previous = NULL;	
	expr->child = NULL;
	expr->parent = NULL;
	data_init(&(expr->data),id);
	debug(KCYN "creating %s\n" KNRM,expr->symbol->name);
	return expr;
}

Expr Expr_createString(char *string){
	Expr expr = allocate(sizeof(Expr_Sub));
	expr->symbol = SymbolTable_get(id_String);
	expr->data.string = String_copy(string);
	expr->next = NULL;
	expr->previous = NULL;	
	expr->child = NULL;
	expr->parent = NULL;
	return expr;
}

/*
Expr Expr_createString(char *string){
	Expr expr = allocate(sizeof(Expr_Sub));
	expr->symbol = SymbolTable_get(id_String);
	expr->data.string = string;
	expr->next = NULL;
	expr->previous = NULL;	
	expr->child = NULL;
	expr->parent = NULL;
	return expr;
}

Expr Expr_createExpInteger(char *string){
	char *p,*psav,sav;
	mpz_t z;
	int e;

	Expr expr = allocate(sizeof(Expr_Sub));
	expr->symbol = SymbolTable_get(id_Integer);
	
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
	expr->prec = e*calcprecint(string);
	
	if(expr->prec <= I32_LIMIT){
		expr->data.integer32 = atoi(string)*pow(10,e);
		//printf("integer32 : %d\n",expr->data.integer32);
	}else{
		mpz_init_set_str(expr->data.integer,string,10);
		mpz_init(z);
		mpz_ui_pow_ui(z,10,e);
		mpz_mul(expr->data.integer,expr->data.integer,z);
		mpz_clear(z);
	}
	*psav=sav;
	expr->next = NULL;
	expr->previous = NULL;	
	expr->child = NULL;
	expr->parent = NULL;
	return expr;
}

Expr Expr_createReal(char *string){
	Expr expr = allocate(sizeof(Expr_Sub));
	expr->symbol = SymbolTable_get(id_Real);
	expr->prec = calcprec(string);
	if(expr->prec <= R64_LIMIT){
		expr->data.real64 = atof(string);
	}else{
		mpfr_init2(expr->data.real,expr->prec);
		mpfr_set_str(expr->data.real,string,0,MPFR_RNDN);
	}
	expr->next = NULL;
	expr->previous = NULL;	
	expr->child = NULL;
	expr->parent = NULL;
	return expr;
}

Expr Expr_createInteger32(int k){
	Expr expr = allocate(sizeof(Expr_Sub));
	expr->symbol = SymbolTable_get(id_Integer);
	expr->prec = I32_LIMIT;
	expr->data.integer32 = k;
	
	expr->next = NULL;
	expr->previous = NULL;
	expr->child = NULL;
	expr->parent = NULL;
	return expr;
}

Expr Expr_createReal64(double d){
	Expr expr = allocate(sizeof(Expr_Sub));
	expr->symbol = SymbolTable_get(id_Real);
	expr->prec = R64_LIMIT;
	expr->data.real64 = d;
	
	expr->next = NULL;
	expr->previous = NULL;
	expr->child = NULL;
	expr->parent = NULL;
	return expr;
}
*/

/*
Integer vs Integer
i32 && i -> i && i

Integer vs Real
i32 && r64 -> r64 && r64
i32 && r -> r && r
i && r64 -> r64 && r64
i && r -> r && r

Real vs Real
r64 && r -> r && r
*/
#define IVSI(e1,e2) \
if(e1->data.INT_PREC <= I32_LIMIT){ \
 	if(e2->data.INT_PREC <= I32_LIMIT){ \
		return; \
	}else{ \
		e1->data.INT_PREC = e2->data.INT_PREC; \
		i = e1->data.INT_MP; \
		mpz_init_set_si(e1->data.INT_AP,i); \
	} \
}

#define IVSR(e1,e2) \
if(e1->symbol->id == id_Integer){ \
	e1->symbol = SymbolTable_get(id_Real); \
	if(e1->data.INT_PREC <= I32_LIMIT){ \
		i = e1->data.INT_MP; \
		if(e2->data.REAL_PREC <= R64_LIMIT){ \
			e1->data.REAL_MP = (double)i; \
		}else{ \
			mpfr_init2(e1->data.REAL_AP,e2->data.REAL_PREC); \
			mpfr_set_si(e1->data.REAL_AP,i,MPFR_RNDN); \
		} \
	}else{ \
		if(e2->data.REAL_PREC <= R64_LIMIT){ \
			d = mpz_get_d(e1->data.INT_AP); \
			mpz_clear(e1->data.INT_AP); \
			e1->data.REAL_MP = d; \
		}else{ \
			mpz_init_set(mpz,e1->data.INT_AP); \
			mpz_clear(e1->data.INT_AP); \
			mpfr_init2(e1->data.REAL_AP,e2->data.REAL_PREC); \
			mpfr_set_z(e1->data.REAL_AP,mpz,MPFR_RNDN); \
			mpz_clear(mpz); \
		} \
	} \
	e1->data.REAL_PREC = e2->data.REAL_PREC; \
}
#define RVSR(e1,e2) \
if(e1->data.REAL_PREC <= R64_LIMIT){ \
 	if(e2->data.REAL_PREC <= R64_LIMIT){ \
		return; \
	}else{ \
		e1->data.REAL_PREC = e2->data.REAL_PREC; \
		d = e1->data.REAL_MP; \
		mpfr_init2(e1->data.REAL_AP,e1->data.REAL_PREC); \
		mpfr_set_d(e1->data.REAL_AP,d,MPFR_RNDN); \
	} \
}

void Expr_cast(Expr e1, Expr e2){
	int i;
	double d;
	mpz_t mpz;
	if(e1->symbol->id == id_Integer && e2->symbol->id == id_Integer){
		/*Integer vs Integer*/
		IVSI(e1,e2)
		else IVSI(e2,e1)
	}else if(
		(e1->symbol->id == id_Integer && e2->symbol->id == id_Real) ||
		(e2->symbol->id == id_Integer && e1->symbol->id == id_Real)
	){
		/*Integer vs Real*/
		IVSR(e1,e2)
		else IVSR(e2,e1)
	}else if(e1->symbol->id == id_Real && e2->symbol->id == id_Real){
		/*Real vs Real*/
		RVSR(e1,e2)
		else RVSR(e2,e1)
	}
}

#undef IVSI
#undef IVSR
#undef RVSR
void Expr_deleteString(Expr expr){
	if(expr){
		deallocate(expr->data.string);
		deallocate(expr);
	}
}
void Expr_delete(Expr expr){
	if(expr){
		//if(!expr->symbol->name){fprintf(stderr,"Expr_delete : NULL SYMBOL\n");}
		debug(KRED "deleting %s\n" KNRM,expr->symbol->name);
		data_finish(&(expr->data),expr->symbol->id);
		deallocate(expr);
	}
}

void Expr_deleteRoot(Expr expr){
	Expr child,next;
	if(!expr){return;}
	for(child=expr->child;child;child=next){
		next = child->next;
		Expr_deleteRoot(child);
	}
	Expr_delete(expr);
}

void Expr_deleteChild(Expr expr, Expr child){
	Expr e,last;
	for(e=expr->child;e;e=e->next){
		if(e==child){
			if(!e->previous){expr->child=e->next;}
			else{e->previous->next = e->next;}
			if(e->next){e->next->previous=e->previous;}
			Expr_delete(e);
			break;
		}
	}
}

void Expr_replace(Expr oldme, Expr newme){
	Expr e,previous=NULL,next=NULL,parent=NULL;
	parent = oldme->parent;
	if(parent){
		e=parent->child;
		for(e=parent->child;e;e=e->next){
			if(e==oldme){
				previous = oldme->previous;
				next = oldme->next;
				break;
			}
		}
	}
	
	if(newme!=next){newme->next = next;}
	if(newme!=previous){newme->previous = previous;}
	if(newme!=parent){newme->parent = parent;}
	
	if(previous){
		if(newme!=previous){previous->next = newme;}
	}else{
		if(parent && newme!=parent){parent->child = newme;}
	}
	if(next){
		if(newme!=next){next->previous = newme;}
	}
}

void Expr_replacelist(Expr oldme, Expr newlist){
	Expr e,previous=NULL,next=NULL,parent=NULL,newtail=NULL;
	parent = oldme->parent;
	if(parent){
		for(e=parent->child;e;e=e->next){
			if(e==oldme){
				previous = oldme->previous;
				next = oldme->next;
				break;
			}
		}
	}
	
	for(e=newlist;e;e=e->next){
		e->parent = parent;
		newtail=e;
	}
	
	if(newtail!=next){newtail->next = next;}
	if(newlist!=previous){newlist->previous = previous;}
	if(newlist!=parent){newlist->parent = parent;}
	
	if(previous){
		if(newlist!=previous){previous->next = newlist;}
	}else{
		if(parent && newlist!=parent){parent->child = newlist;}
	}
	if(next){
		if(newtail!=next){next->previous = newtail;}
	}
}

Expr Expr_isolate(Expr expr){
	if(expr){
		expr->parent = expr->previous = expr->next = NULL;
	}
	return expr;
}

int Expr_getLength(Expr expr){
	Expr list;
	int i=0;
	for(list=expr;list;list=list->next,i++){}
	return i;
}

void Expr_debug_mod(Expr expr,int i){
	Expr child;
	char *buf;
	printf("level : %d\t",i);
	printf("length : %d\t",Expr_getLength(expr));
	printf("%x\t",(unsigned int)expr);
	printf("symbol : %s\t",expr->symbol->name);
	buf = Expr_toString(expr);
	printf("%s\t",buf);
	printf("asso : "); 
	ASSO_PRINT((int)expr->symbol->associativity);
	putchar('\n');
	deallocate(buf);
	for(child=expr->child;child;child=child->next){
		Expr_debug_mod(child,i+1);
	}
}
void Expr_debug(Expr expr){
	Expr_debug_mod(expr,0);
}


/*
char *real64_toString(double d, long prec){
	int needed;
	char *buf, *com;
	needed = snprintf(NULL,0,"%%.%ldg",(long)((prec+1)*LOG2));
	com = allocate(needed+1);
	snprintf(com,needed+1,"%%.%ldg",(long)((prec+1)*LOG2));
	needed = snprintf(NULL,0,com,d);
	buf = allocate(needed+1);
	snprintf(buf,needed+1,com,d);
	deallocate(com);
	return buf;
}
*/

/*double : 64 = 1 + 11 + 52 (sign,exponent,prec)*/
/*52*log2 = 15.6*/
/*prec+1 rather than prec <- don't know why*/
/*
char *real64_toString(double d, long prec){
	int needed;
	char *buf;
	needed = snprintf(NULL,0,"%.15g",d);
	buf = allocate(needed+1);
	snprintf(buf,needed+1,"%.15g",d);
	return buf;
}
*/
char *Expr_toString(Expr expr){
	return Knowledge_Expr_toString(expr);
}
char *Expr_getColor(Expr expr){
	char *s=KNRM;
	if(expr->flag & ef_full){s=KCYN;}
	if(expr->flag & ef_func){s=KGRN;}
	if(expr->flag & ef_mark){s=KRED;}
	return s;
}
/*
char *Expr_toString(Expr expr){
	char *buf=NULL;
	int needed;
	switch(expr->symbol->id){
	  case id_Blank:
		needed = snprintf(buf,0,"_%s",expr->data.symbol->name);
		buf = allocate(needed+1);
		snprintf(buf,needed+1,"_%s",expr->data.symbol->name);
		return buf;
	  case id_Blank2:
		needed = snprintf(buf,0,"__%s",expr->data.symbol->name);
		buf = allocate(needed+1);
		snprintf(buf,needed+1,"__%s",expr->data.symbol->name);
		return buf;
	  case id_Blank3:
		needed = snprintf(buf,0,"___%s",expr->data.symbol->name);
		buf = allocate(needed+1);
		snprintf(buf,needed+1,"___%s",expr->data.symbol->name);
		return buf;
	  case id_Node:
		needed = snprintf(buf,0,"(%c)",expr->data.integer32);
		buf = allocate(needed+1);
		snprintf(buf,needed+1,"(%c)",expr->data.integer32);
		return buf;
	  case id_String:
		if(expr->prec>0){return String_copy(expr->data.string);}
		break;
	  case id_Integer:
		if(expr->prec>0){
			if(expr->prec <= I32_LIMIT){
				return itoa(expr->data.integer32);
			}else{
				return mpz_get_str(NULL,10,expr->data.integer);
			}
		}
		break;
	  case id_Real:
		if(expr->prec>0){
			if(expr->prec <= R64_LIMIT){
				return real64_toString(expr->data.real64,expr->prec);
			}else{
				return mpfr_toString(expr->data.real,expr->prec);
			}
		}
		break;
	  default:
		break;
	}
	return String_copy(expr->symbol->name);
}
*/
Expr Expr_append(Expr expr, Expr expr2){
	Expr list;
	if(!expr){
		if(expr2){expr2->previous=NULL;}
		return expr2;
	}
	for(list=expr;list->next;list=list->next){}
	list->next = expr2;
	if(expr2){expr2->previous=list;}
	return expr;
}

Expr Expr_appendChild(Expr expr, Expr expr2){
	Expr list;
	if(!expr){return NULL;}
	if(expr2){
		for(list=expr2;list;list=list->next){
			list->parent = expr;
		}
	}
	expr->child = Expr_append(expr->child,expr2);
	return expr;
}

Expr Expr_prependChild(Expr expr, Expr expr2){
	Expr list;
	if(!expr){return NULL;}
	if(expr2){
		for(list=expr2;list;list=list->next){
			list->parent = expr;
		}
	}
	expr->child = Expr_append(expr2,expr->child);
	return expr;
}

Expr Expr_insert(Expr p, Expr c, Expr e){
	Expr list;
	if(!p){
		return Expr_appendChild(e,c);
	}else if(!c){
		return Expr_appendChild(p,e);
	}else if(p==c){
		return Expr_appendChild(p,e);
	}else if(c->parent != p){
		perror("Expr_insert : p and c are unrelated\n");
	}
	Expr_replace(c,e);
	c->next = c->previous = NULL;
	Expr_appendChild(e,c);
	return p;
}

Expr Expr_getParent(Expr expr){
	if(!expr){return NULL;}
	return expr->parent;
}

Expr Expr_getChild(Expr expr, int i){
	Expr e,rtn=NULL;
	int k;
	if(!expr){return NULL;}
	for(e=expr->child,k=0;e;e=e->next,k++){
		if(k==i){
			rtn=e;
			break;
		}
	}
	return rtn;
}

Expr Expr_copy(Expr expr){
	Expr ecpy;
	if(!expr){return NULL;}
	/*all user-defined symbol have same id s, so i cannot use Expr_create(id) here*/ 
	ecpy = Expr_createBySymbol(expr->symbol);
	data_copy(&(expr->data),&(ecpy->data),expr->symbol->id);
	//ecpy->flag = expr->flag;
	ecpy->flag = 0;
	ecpy->next = NULL;
	ecpy->previous = NULL;
	ecpy->child = NULL;
	ecpy->parent = NULL;
	return ecpy;
}

int Expr_equals(Expr e1, Expr e2){
	if(!e1){
		if(!e2){return 1;}
		else{return 0;}
	}else{
		if(!e2){return 0;}
	}
	if(e1->symbol==e2->symbol){
		return data_equals(&(e1->data),&(e2->data),e1->symbol->id);
	}
	return 0;
}

Expr Expr_copyRoot(Expr expr){
	Expr e,ecpy;
	if(!expr){return NULL;}
	ecpy = Expr_copy(expr);
	for(e=expr->child;e;e=e->next){
		Expr_appendChild(ecpy,Expr_copyRoot(e));
	}
	return ecpy;
}

Expr Expr_tail(Expr expr){
	Expr e;
	if(!expr){return NULL;}
	for(e=expr;e->next;e=e->next){}
	return e;
}

Expr Expr_binary(char *symbol, Expr expr, Expr expr2){
	Expr head = Expr_createByName(symbol);
	return Expr_appendChild(head,Expr_append(expr,expr2));
}

Expr Expr_unary(char *symbol, Expr expr){
	Expr head = Expr_createByName(symbol);
	return Expr_appendChild(head,expr);
}
