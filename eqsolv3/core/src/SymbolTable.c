#include "knowledge.h"
/*fields of Symbol class*/
/*name : print and parser functions referes this field*/
/*id : a unique number associated with a symbol.*/
/*sign : an operator mark (+,-,*,...etc), this field is used in parsing phase.*/
/*staticfield : builtin rules of symbol this rules are applied by evaluate using replace. */
/*assigned : local field used by Set.*/
/*this field is initialized at start or later time in the main program, with void* 'SYMBOL NAME'_static_create() function. 
and deleted at the end of main program with void 'SYMBOL NAME'_static_delete(void*) function.*/
Expr parse(char *str);
void parser_setfunc(Expr (*create)(char *));
Expr EvaluateBuiltinRules(Expr expr);

Symbol Symbol_create(char *name,int id,Func function,enum en_attributes attributes, enum en_associativity associativity, int precedence, char *sign, Expr builtinrules, Expr alias){
	Symbol s = allocate(sizeof(Symbol_Sub));
	s->name = String_copy(name);
	s->id = id;
	s->sign = String_copy(sign);
	s->function = function;
	s->attributes = attributes;
	s->associativity = associativity;
	s->precedence = precedence;
	s->builtinrules = builtinrules;
	s->staticfield = static_create(id);
	s->alias = alias;
	s->assigned = NULL;
	return s;
}

void Symbol_delete_fields(Symbol s){
	Expr e;
	if(s->builtinrules){debug("deleting builtinrules\n");}
	static_delete(s->staticfield,s->id);
	for(e=s->assigned;e;e=e->next){
		Expr_deleteRoot(e);
	}
	Expr_deleteRoot(s->builtinrules);
	Expr_deleteRoot(s->alias);
}

void Symbol_delete(Symbol s){
	deallocate(s->name);
	deallocate(s->sign);
	deallocate(s);
}

void printAttributes(enum en_attributes attributes);
void Symbol_print_info(Symbol s){
	printf("Symbol @ %x\n",(unsigned int)s);
	printf("id : %d @ %x\n",s->id,(unsigned int)s->id);
	printf("name : %s @ %x\n",s->name,(unsigned int)s->name);
	printf("sign : %s @ %x\n",s->sign,(unsigned int)s->sign);
	printf("function @ %x\n",(unsigned int)s->function);
	printf("attributes @ %x",(unsigned int)s->attributes);
	if(s->attributes){
		printf(" : ");
		printf(KYEL); printAttributes(s->attributes); printf(KNRM);
	}else{putchar('\n');}
	printf("associativity : %d @ %x\n",s->associativity,(unsigned int)s->associativity);
	printf("precedence : %d @ %x\n",s->precedence,(unsigned int)s->precedence);
	printf("buitinrules @ %x :\n",(unsigned int)s->builtinrules);
	TreeForm_modh(s->builtinrules);
	printf("staticfield @ %x\n",(unsigned int)s->staticfield);
	printf("assigned @ %x : \n",(unsigned int)s->assigned);
	TreeForm_modh(s->assigned);
	printf("alias @ %x : \n",(unsigned int)s->alias);
	TreeForm_modh(s->alias);
}

void Symbol_print_mod(Symbol s, void (*doc)(int)){
	static void (*doc_imp)(int);
	char buf[26];
	Expr e;
	if(doc){
		doc_imp=doc;
	}else{
		//Symbol_print_info(s);
		if(s->sign){
			snprintf(buf,26,"%s (%s)",s->name,s->sign);
		}else{
			snprintf(buf,26,"%s",s->name);
		}
		printf(KCYN "%-26s" KNRM, buf);
		if(s->alias){
			printf(KRED); printf("Alias : ");
			printf(KYEL);
			for(e=s->alias->child;e;e=e->next){
				printf("%s",e->data.string);
				if(e->next){printf(",");}
			}
			printf(KNRM);
		}
		putchar('\n');
		/*
		if(s->attributes){
			printf(" : ");
			printf(KYEL); printAttributes(s->attributes); printf(KNRM);
		}else{putchar('\n');}
		*/
		
		doc_imp(s->id);
		printf(KWHT LONGLINE128 KNRM);
	}
}
int Symbol_comp(Symbol s1, Symbol s2){
	if(s1 && s2){
		return *(s1->name) - *(s2->name);
	}
	if(s1 && !s2){return 1;}
	if(!s1 && s2){return -1;}
	return 0;
}
int Symbol_vcomp(void *s1, void *s2){return Symbol_comp(s1,s2);}
void Symbol_print_set(void (*doc)(int)){Symbol_print_mod(NULL,doc);}
void Symbol_vprint(void *s){Symbol_print_mod(s,NULL);}
char *Symbol_toString(Symbol s){return s->name;}
void Symbol_vdelete(void *s){Symbol_delete(s);}
void Symbol_vdelete_fields(void *s){Symbol_delete_fields(s);}
char *Symbol_vtoString(void *s){return Symbol_toString(s);}

void Symbol_parseBuiltinRules(Symbol s){
	Expr e;
	if(s->builtinrules){
		//printf("%s\n",s->builtinrules->data.string);
		e = EvaluateBuiltinRules(parse(s->builtinrules->data.string));
		//TreeForm_modh(e);
		Expr_deleteString(s->builtinrules);
		s->builtinrules = e;
	}
}
void Symbol_parseAlias(Symbol s){
	Expr e;
	if(s->alias){
		//printf("%s\n",s->alias->data.string);
		e = parse(s->alias->data.string);
		//TreeForm_modh(e);
		Expr_deleteString(s->alias);
		s->alias = e;
	}
}
void Symbol_vparseBuiltinRules(void *s){Symbol_parseBuiltinRules(s);}
void Symbol_vparseAlias(void *s){Symbol_parseAlias(s);}

Alias Alias_create(char *alias, Symbol s){
	Alias a = allocate(sizeof(Alias_Sub));
	a->alias = alias;
	a->symbol = s;
	return a;
}

Symbol Alias_getSymbol(Alias a){
	if(a){return a->symbol;}
	else{return NULL;}
}
void Alias_delete(Alias a){
	deallocate(a);
}
void Alias_vdelete(void *a){Alias_delete(a);}
char *Alias_toString(Alias a){return a->alias;}
char *Alias_vtoString(void *a){return Alias_toString(a);}

void appendAlias_mod(HashTable *ht, Symbol s){
	static HashTable *htimp;
	Expr e;
	if(ht){
		htimp = ht;
	}else if(s){
		if(s->alias){
			for(e=s->alias->child;e;e=e->next){
				//printf("appending : %s\n",e->data.string);
				HashTable_append(*htimp,e->data.string,Alias_create(e->data.string,s));
			}
		}
	}
}
void appendAlias_set(HashTable *ht){appendAlias_mod(ht,NULL);}
void appendAlias(Symbol s){appendAlias_mod(NULL,s);}
void vappendAlias(void *s){appendAlias(s);}

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
	Symbol_print_set(c->doc);
	printf(KRED "%s\n" KNRM,c->name);
	printf(LONGBAR128);
	HashTable_enumSorted(c->table,Symbol_vprint,Symbol_vcomp);
	printf("\n");
}

void Context_printall(Context c){
	Context cc;
	for(cc=c;cc;cc=cc->next){
		Context_print(cc);
	}
}

void Context_delete(Context c){
	deallocate(c->name);
	HashTable_enum(c->table,Symbol_vdelete_fields);
	HashTable_delete(c->table,Symbol_vdelete);
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
	stm_call_nametable,
	stm_get_by_name,
	stm_get,
	stm_get_context,
	//stm_get_internal,
	stm_init_internal,
	stm_printall,
	stm_print,
	stm_add_symbol,
	stm_add_context,
	stm_select_context,
	stm_finish,
};

SymbolTable SymbolTable_selectContext(char *name);
void Context_doc_default(int id){return;}

Context SymbolTable_mod(char *name, enum en_stm_mode mode, SymbolTable *table ,Symbol *symbol, void (*doc)(int),int id,char ***nt){
	static Context context;
	static Context user;
	static Context internal;
	static HashTable alias;
	static Symbol idtable[id_end];
	static char *nametable[id_end];
	
	Context c;
	SymbolTable st;
	Symbol s;
	switch(mode){
	  case stm_init:
		ADD_CONTEXT(idtable,nametable)
		for(c=context;c;c=c->next){
			if(!strcmp(c->name,"Internal`")){internal=c;}
			if(!strcmp(c->name,"User`")){user=c;}
		}
		if(!user){Context_prepend(context,user=Context_create("User`",Context_doc_default,HashTable_create2(TABLESIZE_DEFAULT,Symbol_vtoString)));}
		break;
	  case stm_init_internal:
		alias = HashTable_create2(TABLESIZE_DEFAULT,Alias_vtoString);
		appendAlias_set(&(alias));
		for(c=context;c;c=c->next){
			if(c==user){continue;}
			HashTable_enum(c->table,Symbol_vparseBuiltinRules);
			parser_setfunc(Expr_createString);
			HashTable_enum(c->table,Symbol_vparseAlias);
			parser_setfunc(Expr_createByName);
			HashTable_enum(c->table,vappendAlias);
		}
		break;
	  case stm_call:
		*table = context->table;
		break;
	  case stm_call_nametable:
		*nt = nametable;
		break;
	  case stm_get_context:
		break;
	  case stm_get_by_name:
		for(c=context;c;c=c->next){
			if(c==internal){continue;}
			if((*symbol=HashTable_get(c->table,name))!=NULL){
				break;
			}
		}
		if(*symbol==NULL){
			if(alias){*symbol=Alias_getSymbol(HashTable_get(alias,name));}
			//if(*symbol){Symbol_print_info(*symbol);}
		}
		if(*symbol==NULL){
			user->table=HashTable_append(user->table,name,*symbol=Symbol_create(name,id_end,NULL,0,0,1000,NULL,NULL,NULL));
		}
		break;
	  case stm_get:
		*symbol=idtable[id];
		break;
		/*
	  case stm_get_internal:
		*symbol=HashTable_get(internal->table,name);
		if(*symbol==NULL){fprintf(stderr,"SymbolTable_getIntenal : internal symbol \"%s\" is not found.\n",name);}
		break;
		*/
	  case stm_printall:
		Context_printall(context);
		break;
	  case stm_print:
		s=NULL;
		for(c=context;c;c=c->next){
			if((s=HashTable_get(c->table,name))!=NULL){
				break;
			}
		}
		if(s){
			Symbol_print_set(c->doc);
			Symbol_vprint(s);
		}
		break;
	  case stm_add_symbol:
		context->table=HashTable_append(context->table,name,*symbol);
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
		//if(context==NULL){fprintf(stderr,"stm_finish : context null\n");}
		Context_deleteRoot(context);
		HashTable_delete(alias,Alias_vdelete);
		alias=NULL;
		context=NULL;
		break;
	  default:
		break;
	}
	return context;
}
void SymbolTable_init(){
	SymbolTable_mod(NULL,stm_init,NULL,NULL,NULL,0,NULL);
}

void SymbolTable_init_build_internal(){
	SymbolTable_mod(NULL,stm_init_internal,NULL,NULL,NULL,0,NULL);
}

Context SymbolTable_getContext(){
	return SymbolTable_mod(NULL,stm_get_context,NULL,NULL,NULL,0,NULL);
}

SymbolTable SymbolTable_call(){
	SymbolTable table;
	SymbolTable_mod(NULL,stm_call,&table,NULL,NULL,0,NULL);
	return table;
}

Symbol SymbolTable_getByName(char *name){
	Symbol sd=NULL;
	SymbolTable_mod(name,stm_get_by_name,NULL,&sd,NULL,0,NULL);
	return sd;
}

Symbol SymbolTable_get(int id){
	Symbol sd=NULL;
	SymbolTable_mod(NULL,stm_get,NULL,&sd,NULL,id,NULL);
	return sd;
}

char **SymbolTable_callNameTable(){
	char **nametable;
	SymbolTable_mod(NULL,stm_call_nametable,NULL,NULL,NULL,0,&nametable);
	return nametable;
}
/*
Symbol SymbolTable_getInternal(char *name){
	Symbol sd=NULL;
	SymbolTable_mod(name,stm_get_internal,NULL,&sd,NULL,0);
	return sd;
}
*/
void SymbolTable_printall(){
	SymbolTable_mod(NULL,stm_printall,NULL,NULL,NULL,0,NULL);
}

void SymbolTable_print(Symbol s){
	SymbolTable_mod(s->name,stm_print,NULL,NULL,NULL,0,NULL);
}

void SymbolTable_addContext(char *name, void (*doc)(int), SymbolTable table){
	SymbolTable_mod(name,stm_add_context,&table,NULL,doc,0,NULL);
}
/*
void SymbolTable_addSymbol(char *name, int id, Func function, enum en_attributes attributes, enum en_associativity associativity, int precedence, char *sign, Expr preset){
	Symbol sd = Symbol_create(name,id,function,attributes,associativity,precedence,sign,preset);
	SymbolTable_mod(name,stm_add_symbol,NULL,&sd,NULL);
}

SymbolTable SymbolTable_selectContext(char *name){
	SymbolTable t;
	SymbolTable_mod(name,stm_add_context,&t,NULL,NULL);
	return t;
}
*/
void SymbolTable_finish(){
	SymbolTable_mod(NULL,stm_finish,NULL,NULL,NULL,0,NULL);
}

#define ATT_PRINT(ATT,VAL) \
if(attributes & ATT){ \
	if(flag){printf(", ");} \
	else{flag=1;} \
	printf(#ATT); \
}
#define ATT_PRINT2 \
printf("}\n"); \
for(flag=0x8000;flag >>= 1;flag){ \
	if(flag & attributes){putchar('1');} \
	else{putchar('0');} \
} \
putchar('\n');

void printAttributes(enum en_attributes attributes){
	int flag=0;
	printf("{");
	ATT_TABLE(ATT_PRINT)
	printf("}\n");
}
