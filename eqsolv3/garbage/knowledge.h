#ifndef KNOWLEDGE_H
#define KNOWLEDGE_H

#include "utilities.h"
#include "HashTable.h"

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

enum en_ktype{
	k_symbol,
	k_alias,
	k_sign
};

typedef struct st_knowledge{
	char *name;
	enum en_ktype type;
	HashTable table;
	struct st_knowledge *next;
}Knowledge_Sub;
typedef struct st_knowledge *Knowledge;
 
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
void Symbol_delete(Symbol sd);
Symbol Symbol_addAttributes(Symbol sd,enum en_attributes attributes);

Knowledge Knowledge_create(enum en_ktype ktype, char *name, HashTable table);
void Knowledge_delete(Knowledge k);
Knowledge Knowledge_add(Knowledge k, enum en_ktype ktype, char *name, HashTable table);
void Knowledge_print(Knowledge k);

Symbol Symbol_call(Knowledge k, char *name);
Symbol Sign_call(Knowledge k, char *name);
char *Alias_call(Knowledge k, char *name);

#endif