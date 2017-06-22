#include "SymbolTable.h"

#define SYMBOL_APPEND(name,precedence,associativity,symbol,attributes) \
table.nametable =  HashTable_append(table.nametable,#name,s=Symbol_create(#name,id_##name,name,attributes,associativity,precedence,symbol,NULL));\
table.symboltable =  HashTable_append(table.symboltable,symbol,s);

SymbolTable SymbolTable_global(){
	SymbolTable table=
			{
				HashTable_create2(TABLESIZE_GLOBAL,Symbol_vtoString),
				HashTable_create2(TABLESIZE_GLOBAL,Symbol_vtoString)
			};
	Symbol s;
	SYMBOL_GLOBAL(SYMBOL_APPEND)
	return table;
}

#include "physics.h"
SymbolTable SymbolTable_physics(){
	SymbolTable table=
			{
				HashTable_create2(TABLESIZE_PHYSICS,Symbol_vtoString),
				HashTable_create2(TABLESIZE_PHYSICS,Symbol_vtoString)
			};
	Symbol s;
	SYMBOL_PHYSICS(SYMBOL_APPEND)
	return table;
}
#define ADD_CONTEXT \
	SymbolTable_addContext("Global`",doc_global,SymbolTable_global());
	SymbolTable_addContext("Physics`",doc_physics,SymbolTable_physics());


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

void printAttributes(enum en_attributes attributes);
void Symbol_print_mod(Symbol s, void (*doc)(int)){
	static void (*doc_imp)(int);
	char buf[24];
	
	if(doc){
		doc_imp=doc;
	}else{
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
		doc_imp(s->id);
		printf(KWHT LONGLINE KNRM);
	}
}
void Symbol_print_set(void (*doc)(int)){
	Symbol_print_mod(NULL,doc);
}

void Symbol_vprint(void *s){
	Symbol_print_mod(s,NULL);
}

char *Symbol_toString(Symbol s){
	return s->name;
}

void Symbol_vdelete(void *s){Symbol_delete(s);}

char *Symbol_vtoString(void *s){return Symbol_toString(s);}

Symbol Symbol_addAttributes(Symbol sd,enum en_attributes attributes){
	sd->attributes |= attributes;
	return sd;
}

Context Context_create(char *name, void (*doc)(int), SymbolTable st){
	Context c=allocate(sizeof(Context_Sub));
	c->name = String_copy(name);
	c->doc = doc;
	c->table = st;
	c->next = NULL;
	return c;
}

void Context_print(Context c){
	Context cc;
	for(cc=c;cc;cc=cc->next){
		Symbol_print_set(cc->doc);
		printf(KRED "%s\n" KNRM,cc->name);
		printf(LONGBAR);
		HashTable_enum(cc->table.nametable,Symbol_vprint);
		//HashTable_print(cc->table,Symbol_vtoString);
		printf("\n");
	}
}

void Context_delete(Context c){
	deallocate(c->name);
	HashTable_delete(c->table.nametable,Symbol_vdelete);
	HashTable_delete(c->table.symboltable,NULL);	
	deallocate(c);
}

void Context_deleteRoot(Context c){
	Context next;
	for(;c;c=next){next=c->next;Context_delete(c);}
}

Context Context_prepend(Context c0, Context c1){
	if(!c1){return c0;}
	c1->next = c0;
	return c1;
}

SymbolTable SymbolTable_init_imp(int size);
enum en_stm_mode{
	stm_init,
	stm_call,
	stm_get,
	stm_print,
	stm_add_symbol,
	stm_add_context,
	stm_select_context,
	stm_finish,
};

void SymbolTable_addContext(char *name, void (*doc)(int),SymbolTable table);
SymbolTable SymbolTable_selectContext(char *name);

void SymbolTable_mod(char *name, enum en_stm_mode mode, SymbolTable *table ,Symbol *symbol, void (*doc)(int)){
	static Context context;
	static Context global;
	Context c;
	SymbolTable st;
	switch(mode){
	  case stm_init:
		ADD_CONTEXT
		for(c=context;c;c=c->next){global = c;} /*last one is a global*/
		break;
	  case stm_call:
		*table = context->table;
		break;
	  case stm_get:
		for(c=context;c;c=c->next){
			//printf("name : %s\n",name);
			if((*symbol=HashTable_get(c->table.nametable,name))!=NULL){
				break;
			}
		}
		if(*symbol==NULL){
			global->table.nametable=HashTable_append(global->table.nametable,name,*symbol=Symbol_create(name,id_global_end,NULL,0,0,1000,NULL,NULL));
		}
		break;
	  case stm_print:
		Context_print(context);
		break;
	  case stm_add_symbol:
		context->table.nametable=HashTable_append(context->table.nametable,name,*symbol);
		break;
	  case stm_add_context:
		context=Context_prepend(context,Context_create(name,doc,*table));
		break;
	  case stm_select_context:
		for(c=context;c;c=c->next){
			if(!strcmp(c->name,name)){
				*table = c->table;
				break;
			}
		}
		break;
	  case stm_finish:
		Context_deleteRoot(context);
		context=NULL;
		break;
	}
}
void SymbolTable_init(){
	SymbolTable_mod(NULL,stm_init,NULL,NULL,NULL);
}

SymbolTable SymbolTable_call(){
	SymbolTable table;
	SymbolTable_mod(NULL,stm_call,&table,NULL,NULL);
	return table;
}

Symbol SymbolTable_get(char *name){
	Symbol sd=NULL;
	SymbolTable_mod(name,stm_get,NULL,&sd,NULL);
	return sd;
}

void SymbolTable_print(){
	SymbolTable_mod(NULL,stm_print,NULL,NULL,NULL);
}

void SymbolTable_addSymbol(char *name, int id, Func function, enum en_attributes attributes, enum en_associativity associativity, int precedence, char *symbol,Expr expression){
	Symbol sd = Symbol_create(name,id,function,attributes,associativity,precedence,symbol,expression);
	SymbolTable_mod(name,stm_add_symbol,NULL,&sd,NULL);
}

void SymbolTable_addContext(char *name, void (*doc)(int), SymbolTable table){
	SymbolTable_mod(name,stm_add_context,&table,NULL,doc);
}

SymbolTable SymbolTable_selectContext(char *name){
	SymbolTable t;
	SymbolTable_mod(name,stm_add_context,&t,NULL,NULL);
	return t;
}

void SymbolTable_finish(){
	SymbolTable_mod(NULL,stm_finish,NULL,NULL,NULL);
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
