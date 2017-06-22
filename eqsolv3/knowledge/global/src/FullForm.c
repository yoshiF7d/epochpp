#include "knowledge.h"
Expr FullForm_mod(Expr expr){
	Expr e;
	char *buf;
	if(!expr){printf("null"); return expr;}
	buf = Expr_toString(expr);
	printf("%s",buf);
	deallocate(buf);
	if(expr->child){
		printf("[");
		for(e=expr->child;e;e=e->next){
			FullForm_mod(e);
			if(e->next){printf(",");}
		}
		printf("]");
	}
	return expr;
}

Expr FullForm_mod2(FILE *fp,Expr expr){
	Expr e;
	char *buf;
	buf = Expr_toString(expr);
	fprintf(fp,"%s",buf);
	deallocate(buf);
	if(expr->child){
		fprintf(fp,"[");
		for(e=expr->child;e;e=e->next){
			FullForm_mod2(fp,e);
			if(e->next){fprintf(fp,",");}
		}
		fprintf(fp,"]");
	}
	return expr;
}
LinkedList FullForm_mod_str_mod(Expr expr){
	Expr e;
	LinkedList list = NULL;
	list = LinkedList_append(list,Expr_toString(expr));
	if(expr->child){
		list = LinkedList_append(list,String_copy("["));
		for(e=expr->child;e;e=e->next){
			list = LinkedList_concatenate(list,FullForm_mod_str_mod(e));
			if(e->next){list = LinkedList_append(list,String_copy(","));}
		}
		list = LinkedList_append(list,String_copy("]"));
	}
	return list;
}

char *FullForm_mod_str(Expr expr){
	char *buf,*str,*p;
	int len=0;
	LinkedList list,s;
	if(!expr){return String_copy("null");}
	list = FullForm_mod_str_mod(expr);
	for(s=list;s;s=LinkedList_increment(s)){
		str = LinkedList_get(s);
		if(str){len += strlen(str);}
	}
	p = buf = allocate(len+1);
	for(s=list;s;s=LinkedList_increment(s)){
		str = LinkedList_get(s);
		if(str){
			for(;*str;str++,p++){
				*p = *str;
			}
		}
	}
	buf[len]='\0';
	LinkedList_deleteRoot(list,deallocate);
	return buf;
}

Expr FullForm(Expr expr){
	if(!expr->child){return expr;}
	FullForm_mod(expr->child);
	putchar('\n');
	output_flag = 0;
	return expr;
}
