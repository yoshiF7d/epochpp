#include "knowledge.h"
#define CONST_TABLE(CONST_FUNC) \
CONST_FUNC("c","speed of light in vaccum [m/s].",299792458) \
CONST_FUNC("e","elementary charge [C].",1.602176565e-19) \
CONST_FUNC("me","electron mass [kg].",9.10938291e-31) \
CONST_FUNC("mp","proton mass [kg].",1.672621777e-27) \
CONST_FUNC("e0","dielectric constant (vaccum permittivity) [F/m].",8.854187817e-12) \
CONST_FUNC("u0","magnetic constant (vaccum permeability) [N/A^2].",1.256637061e-06) \
CONST_FUNC("kb","Bolzman constant [J/K].",1.3806488e-23) \
CONST_FUNC("G","Newtonian constant of gravitation [m^3/(kg.s^2)].",6.67384e-11) \
CONST_FUNC("R","gas constant [J/(K.mol)].",8.3144621) \
CONST_FUNC("Na","Avogadro's number [/mol].",6.02214129e+23) \
CONST_FUNC("g","standard acceleration of gravity [m/s^2].",9.80665) \
CONST_FUNC("h","Plank constant [J.s].",6.62606957e-34) \
CONST_FUNC("h2","reduced Plank constant [J.s].",1.054571726e-34) \
CONST_FUNC("ppl","Plank lengh [m].",1.616199e-35) \
CONST_FUNC("tpl","Plank time [s].",5.39106e-44) \
CONST_FUNC("cpl","Plank charge [C].",1.875545956e-18) \
CONST_FUNC("kpl","Plank temperature [K].",1.416833e-32) \
CONST_FUNC("mpl","Plank mass [kg].",1.416833e-32) \
CONST_FUNC("a0","Bohr radius [m].",5.2917721092e-11) \
CONST_FUNC("re","classical electron radius [m].",2.8179403267e-15) \
CONST_FUNC("G0","conductance quantum [/Ohm].",7.7480917346e-05) \
CONST_FUNC("ub","classical electron radius [m].",9.27400968e-24) \
CONST_FUNC("un","nuclear magneton [J/T].",5.05078353e-27) \
CONST_FUNC("G0i","inverse conductance quantum [Ohm].",129606.4037217) \
CONST_FUNC("Z0","characteristic impedance of vacuum [Ohm].",376.730313461) \
CONST_FUNC("Ri","Rydberg constant [/m].",10973731.568539) \
CONST_FUNC("Rk","von Klitzing constant [Ohm].",25812.8074434) \
CONST_FUNC("ph0","magnetic flux quantum [Wb].",2.067833758e-15) \
CONST_FUNC("femto",NULL,1e-15) \
CONST_FUNC("pico",NULL,1e-12) \
CONST_FUNC("nano",NULL,1e-09) \
CONST_FUNC("micro",NULL,1e-06) \
CONST_FUNC("milli",NULL,1e-03) \
CONST_FUNC("kilo",NULL,1e+03) \
CONST_FUNC("mega",NULL,1e+06) \
CONST_FUNC("giga",NULL,1e+09) \
CONST_FUNC("tera",NULL,1e+12) \
CONST_FUNC("peta",NULL,1e+15) \
CONST_FUNC("exa",NULL,1e+18) \
CONST_FUNC("eV",NULL,1.602176565e-19) \
CONST_FUNC("gauss",NULL,1e-4) \
CONST_FUNC("rpe","proton to electron mass ratio.",1836.15267389) \
CONST_FUNC("rcm","electron charge to mass ratio.",1.758820024e+11)
#define CONST_APPEND(name,doc,value) table = HashTable_append(table,name,Const_create(name,doc,value));
	
typedef struct st_constant{
	char *name;
	char *doc;
	double value;
}Const_Sub;
typedef Const_Sub *Const;

Const Const_create(char *name, char *doc, double value){
	Const c;
	c = allocate(sizeof(Const_Sub));
	c->name = String_copy(name);
	c->doc = String_copy(doc);
	c->value = value;
	return c;
}
char *Const_printName(Const c){return c->name;}
char *Const_vprintName(void *c){return Const_printName(c);}
int Const_comp(Const c1, Const c2){return *(c1->name)-*(c2->name);}
int Const_vcomp(void *c1, void *c2){return Const_comp(c1,c2);}

void Const_print(Const c){
	int i;
	char *p;
	for(i=0;i<DOC_INDENT;i++){putchar(' ');}
	//printf("%x\n",(unsigned int)c);
	//printf("%x\n",(unsigned int)(c->name));
	//for(p=c->name;*p&&p;p++){printf("%c",*p);}
	//printf("\n");
	//for(p=c->doc;*p&&p;p++){printf("%c",*p);}
	//printf("\n");
	//printf("%x\n",(unsigned int)(c->doc));
	
	printf(KWHT "| " KCYN "%-5s" KWHT "  |  ",c->name);
	if(c->doc){
		printf(KNRM "%-52s" KWHT "  |  ",c->doc);
	}else{
		printf(KNRM "%-52c" KWHT "  |  ",' ');
	}
	printf(KGRN "%-12e" KWHT  "  |\n" KNRM,c->value);
}
void Const_vprint(void *c){Const_print(c);}
void Const_delete(Const c){
	if(c){
		deallocate(c->name);
		deallocate(c->doc);
		deallocate(c);
	}
}
void Const_vdelete(void *c){Const_delete((Const)c);}

void *P_static_create(){
	HashTable table = HashTable_create2(64,Const_vprintName);
	CONST_TABLE(CONST_APPEND)
	//fprintf(stderr,"P_static_create create table at %x\n",(unsigned int)table);
	//HashTable_print(table,Const_vprintName);
	return table;
}
void P_static_delete(void *c){
	HashTable_delete(c,Const_vdelete);
}
void P_doc(int id){
	int i;
	char buf[24];
	Symbol s=SymbolTable_get(id);
	HashTable h = s->staticfield;
	//fprintf(stderr,"P_doc found table at %x\n",(unsigned int)s->staticfield);
	//HashTable_print(h,Const_vprintName);
	for(i=0;i<DOC_INDENT;i++){putchar(' ');}
	printf("P[expr] or expr//P replace names in expr to physical constant values.\n\n");
	for(i=0;i<DOC_INDENT;i++){putchar(' ');}
	printf(KWHT "------------------------------------------------------------------------------------\n" KNRM);
	HashTable_enumSorted(h,Const_vprint,Const_vcomp);
	//HashTable_enumSorted(h,Const_vprint,Const_vcomp);
	for(i=0;i<DOC_INDENT;i++){putchar(' ');}
	printf(KWHT "------------------------------------------------------------------------------------\n" KNRM);
}
Expr P(Expr expr){
	Expr e;
	if(!expr->child){
		return expr;
	}
	Expr_replace(expr,e=P_mod(expr->child,expr->symbol));
	Expr_delete(expr);
	return Evaluate(N_mod(Evaluate(e),R64_LIMIT));
}
	
Expr P_mod(Expr expr, Symbol s){
	Expr e;
	char *name;
	double value;
	int hit=0;
	Const c;
	
	for(e=expr->child;e;e=e->next){
		e=P_mod(e,s);
	}
	c = HashTable_get(s->staticfield,expr->symbol->name);
	if(c){
		expr->symbol = SymbolTable_get(id_Real);
		expr->data.REAL_PREC = R64_LIMIT/2;
		expr->data.REAL_MP = c->value;
		expr->flag=0;
	}
	return expr;
}
