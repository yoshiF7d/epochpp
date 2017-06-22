#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include "utilities.h"
#include "HashTable.h"

#define TABLESIZE_DEFAULT 64

#define ATT_TABLE(ATT_FUNC) \
ATT_FUNC(Orderless,0x0001) \
ATT_FUNC(Flat,0x0002) \
ATT_FUNC(OneIdentity,0x0004) \
ATT_FUNC(Listable,0x0008) \
ATT_FUNC(Constant,0x0010) \
ATT_FUNC(NumericFunction,0x0020) \
ATT_FUNC(Protected,0x0040) \
ATT_FUNC(Locked,0x0080) \
ATT_FUNC(ReadProtected,0x0100) \
ATT_FUNC(HoldFirst,0x0200) \
ATT_FUNC(HoldRest,0x0400) \
ATT_FUNC(HoldAll,0x0800) \
ATT_FUNC(HoldAllComplete,0x1000) \
ATT_FUNC(SequenceHold,0x2000) \
ATT_FUNC(Temporary,0x4000)

#define ATT_ENUM(ATT,VAL) \
ATT=VAL,

enum en_attributes{
ATT_TABLE(ATT_ENUM)
};

enum en_associativity{
	bi_left=1,
	bi_right,
	bi_none,
	bi_one,
	un_pre,
	un_post,
	un_one,
	asso_special,
};

#define ASSO_PRINT(tag) \
switch(tag){\
case 0:printf("no asso");break;\
case bi_left:printf("binary left\n");break;\
case bi_right:printf("binary right\n");break;\
case bi_one:printf("binary one pair\n");break;\
case bi_none:printf("binary no asso\n");break;\
case un_pre:printf("unary pre");break;\
case un_post:printf("unary post");break;\
case un_one:printf("one identity");break;\
default:break;\
}

typedef struct st_expr *Expr; /*defined in Expr.h*/
typedef HashTable SymbolTable;
typedef Expr (*Func)(Expr);

typedef struct st_context{
	char *name;
	void (*doc)(int);
	SymbolTable table;
	struct st_context *next;
}Context_Sub;
typedef struct st_context *Context;

typedef struct st_symbol{
	char *name;
	int id;
	int precedence;	
	char *sign;
	Func function;
	enum en_attributes attributes;
	enum en_associativity associativity;
	Expr builtinrules;
	void *staticfield;
	Expr assigned;
	Expr alias;
}Symbol_Sub;
typedef struct st_symbol *Symbol;

typedef struct st_alias{
	char *alias;
	Symbol symbol;
}Alias_Sub;
typedef struct st_alias *Alias;
Alias Alias_create(char *alias, Symbol s);
void Alias_delete(Alias a);

Symbol Symbol_create(char *name,int id,Func function,enum en_attributes attributes, enum en_associativity associativity, int precedence, char *sign, Expr builtinrules, Expr alias);
void Symbol_delete(Symbol sd);
Symbol Symbol_addAttributes(Symbol sd,enum en_attributes attributes);

void SymbolTable_init();
void SymbolTable_init_build_internal();
SymbolTable SymbolTable_call();
Symbol SymbolTable_getByName(char *name);
Symbol SymbolTable_get(int id);
Context SymbolTable_getContext();
Symbol SymbolTable_getInternal(char *name);
void printAttributes(enum en_attributes attributes);
void Context_print(Context c);
void SymbolTable_printall();
void SymbolTable_print(Symbol s);
void SymbolTable_append(Func function, enum en_attributes attributes);
void SymbolTable_finish();
char **SymbolTable_callNameTable();
char *Symbol_vtoString(void *s);
void SymbolTable_addContext(char *name, void (*doc)(int),SymbolTable table);
#endif