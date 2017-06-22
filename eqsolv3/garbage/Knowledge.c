#include "Knowledge.h"

Symbol Symbol_create(char *name,int id,Func function,enum en_attributes attributes,enum en_associativity associativity,int precedence, char *symbol,Expr expression){
	Symbol s = allocate(sizeof(Symbol_Sub));
	s->name = String_copy(name);
	s->id = id;
	s->symbol = String_copy(symbol);
	s->function = function;
	s->attributes = attributes;
	s->associativity = associativity;
	s->precedence = precedence;
	s->expression = expression;
	return s;
}

void Symbol_delete(Symbol s){
	deallocate(s->name);
	deallocate(s->symbol);	
	Expr_deleteRoot(s->expression);
	deallocate(s);
}

#define ATT_PRINT(ATT,VAL) \
if(attributes & ATT){ \
	if(flag){printf(", ");} \
	else{flag=1;} \
	printf(#ATT); \
}

void printAttributes(enum en_attributes attributes){
	int flag=0;
	printf("{");
	ATT_TABLE(ATT_PRINT)
	printf("}\n");
}

void Symbol_print_mod(Symbol s){
	char buf[24];
	
	if(s->symbol){
		snprintf(buf,24,"%s (%s)",s->name,s->symbol);
	}else{
		snprintf(buf,24,"%s",s->name);
	}
	printf(KCYN "%-24s" KNRM, buf);
	if(s->attributes){
		printf(" : ");
		printf(KYEL); printAttributes(s->attributes); printf(KNRM);
	}else{putchar('\n');}
	if(s->expression){
		OutputForm_mod(s->expression);
	}
	doc(s->id);
	printf(KWHT LONGLINE KNRM);
}

void Symbol_print_set(void (*doc)(int)){Symbol_print_mod(NULL,doc);}
void Symbol_vprint(void *s){Symbol_print_mod(s,NULL);}
char *Symbol_toString(Symbol s){return s->name;}
void Symbol_vdelete(void *s){Symbol_delete(s);}
char *Symbol_vtoString(void *s){return Symbol_toString(s);}
Symbol Symbol_addAttributes(Symbol sd,enum en_attributes attributes){sd->attributes |= attributes; return sd;}

Knowledge Knowledge_create(enum en_ktype type, char *name, HashTable table){
	Knowledge k = allocate(sizeof(Knowledge_Sub));
	k->type = type;
	k->name = name;
	k->table = table;
	k->next = NULL;
	return k;
}
void Knowledge_delete(Knowledge k){
	if(!k){return;}
	deallocate(k->name);
	switch(k->type){
	  case k_symbol:
		HashTable_delete(k->table,Symbol_vdelete);
		break;
	  case k_alias:
		HashTable_delete(k->table,deallocate);
		break;
	  case k_sign:
		HashTable_delete(k->table,NULL);
		break;
	  default:
		break;
	}
	return;
}

void Knowledge_deleteRoot(Knowledge k){
	Knowledge s;
	for(s=k;s;s=s->next){
		Knowledge_delete(k);
	}
}

Knowledge Knowledge_add(Knowledge k, enum en_ktype type, char *name, HashTable table){
	Knowledge s=Knowledge_create(type,name,table);
	if(k){
		for(;k->next;k=k->next){}
		k->next = s;
	}
	return s;
}

Symbol Symbol_call(Knowledge k, char *name){
	Knowledge s;
	Symbol symb,fs=NULL;
	for(s=k;s;s=s->next){
		if(s->type == k_symbol){
			if(!fs){fs = symb;}
			symb = HashTable_get(s->table,name);
		}
	}
	if(!symb){
		fs->table = HashTable_append(fs->table,name,symb=Symbol_create(name,-1,NULL,0,0,1000,NULL,NULL));
	}
	return symb;
}

Symbol Sign_call(Knowledge k, char *name){
	Symbol symb;
	Knowledge s;
	
	for(s=k;s;s=s->next){
		if(s->type == k_sign){
			symb = HashTable_get(s->table,name);
		}
	}
	return symb;
}

char *Alias_call(Knowledge k, char *name){
	Knowledge s;
	char *alias;
	for(s=k;s;s=s->next){
		if(s->type == k_alias){
			alias = HashTable_get(s->table,name);
		}
	}
	if(!alias){alias = name;}
	return alias;
}

void Knowledge_print(Knowledge k){
	for(s=k;s;s=s->next){
		printf(KRED "%s\n" KNRM,s->name);
		printf(LONGBAR);
		HashTable_enum(s->table,Symbol_vprint);
		printf("\n");
	}
}

Symbol Symbol_call(Knowledge k, char *name);
char *Sign_call(Knowledge k, char *name);
char *Alias_call(Knowledge k, char *name);
