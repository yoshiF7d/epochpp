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
ATT_FUNC(Temporary,0x8000)

#define ATT_ENUM(ATT,VAL) \
ATT=VAL,

enum en_attributes{
ATT_TABLE(ATT_ENUM)
};

enum en_associativity{
	bi_left=1,
	bi_right,
	bi_none,
	un_pre,
	un_post
};

typedef struct st_expr *Expr; /*defined in Expr.h*/

typedef struct symboltable{
	HashTable nametable;
	HashTable symboltable;
}SymbolTable;

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
	char *symbol;
	Func function;
	enum en_attributes attributes;
	enum en_associativity associativity;
	int precedence;
	Expr expression;
}Symbol_Sub;

typedef struct st_symbol *Symbol;
Symbol Symbol_create(char *name,int id,Func function,enum en_attributes attributes, enum en_associativity associativity, int precedence, char *symbol,Expr expression);
void Symbol_delete(Symbol s);
Symbol Symbol_addAttributes(Symbol s,enum en_attributes attributes);

void SymbolTable_init();
SymbolTable SymbolTable_call();
Symbol SymbolTable_get(char *name);
void SymbolTable_print();
void SymbolTable_append(Func function, enum en_attributes attributes);
void SymbolTable_finish();

#endif