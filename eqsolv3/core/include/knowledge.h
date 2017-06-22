#ifndef KNOWLEDGE_H
#define KNOWLEDGE_H
#define debug(...) do{}while(0)
#include "Expr.h"
#define TABLE_SIZE 512
/*====== proto ======*/
char *Knowledge_Expr_toString(Expr expr);
/*=== global ===*/
Expr ArcTan(Expr expr);
Expr ArrayDepth(Expr expr);
int ArrayDepth_mod(Expr expr);
Expr ArrayQ(Expr expr);
int ArrayQ_mod(Expr expr);
Expr AtomQ(Expr expr);
int AtomQ_mod(Expr expr);
Expr Attributes(Expr expr);
Expr Binomial(Expr expr);
int Binomial_int(int n, int k);
Expr Blank_pre(Expr expr);
char *Blank_toString(Expr expr);
void Blank_data_init(union un_data *data);
void Blank_data_copy(union un_data *datas, union un_data *datad);
char *Blank2_toString(Expr expr);
void Blank2_data_init(union un_data *data);
void Blank2_data_copy(union un_data *datas, union un_data *datad);
char *Blank3_toString(Expr expr);
void Blank3_data_init(union un_data *data);
void Blank3_data_copy(union un_data *datas, union un_data *datad);
Expr Column_mod(Expr expr, int alignment, int spacing);
Expr Column(Expr expr);
typedef struct st_form *Form;
Form Form_appendVertical(Form form, Form form2);
Form Form_appendVerticalCenter(Form form, Form form2);
Form Form_appendVerticalRight(Form form, Form form2);
Form Form_make(Expr expr, int flag);

Expr Cos(Expr expr);
Expr Cross(Expr expr);
int Depth_mod(Expr expr);
Expr Depth(Expr expr);
Expr Dimensions(Expr expr);
Expr Dimensions_mod(Expr rtn, Expr expr, int level);
Expr Divide(Expr expr);
Expr Dot(Expr expr);
Expr Dot_mod(Expr e1, Expr e2, int *err);
Expr Equal(Expr expr);
Expr Evaluate(Expr expr);
Expr Exp(Expr expr);
Expr First(Expr expr);
Expr First_mod(Expr expr);
extern int message_flag;
Expr Flatten_mod(Expr expr, Symbol head, int level, int levelmax);
Expr Flatten(Expr expr);
extern int output_flag;
Expr FullForm(Expr expr);
Expr FullForm_mod(Expr expr);
Expr FullForm_mod2(FILE *fp, Expr expr);
char *FullForm_mod_str(Expr expr);
Expr Grid_mod(Expr expr);
Expr Grid(Expr expr);
typedef struct st_form *Form;
Form Form_appendHorizontal(Form form, Form form2);
Form Form_appendVerticalCenter(Form form, Form form2);
Form Form_make(Expr expr, int flag);
extern int output_flag;
Expr Help(Expr expr);
extern int message_flag;
Expr Increment(Expr expr);
Expr Inner(Expr expr);
Expr Inner_mod(Symbol times, Expr e1, Expr e2, Symbol plus, int *err);
#define INT_PREC integer.prec
/*INT_MP : machine precision*/
#define INT_MP integer.data.mp
/*INT_AP : arbitrary precision*/
#define INT_AP integer.data.ap
#define LOG2 0.301029995663981

int Integer_sign(Expr expr);
char *Integer_toString(Expr expr);
long Integer_toInt(Expr expr);
long calcprecint(const char *string);
Expr Integer_create(char *string);
Expr Integer_createInt(int );
Expr Integer_createLong(long k);
Expr Integer_createExp(char *string);
void Integer_data_init(union un_data *data);
void Integer_data_finish(union un_data *data);
void Integer_data_copy(union un_data *datas, union un_data *datad);
int Integer_data_equals(union un_data *d1, union un_data *d2);
/*
void Integer_plus(Expr e1, Expr e2);
void Integer_plus_real(Expr e1, Expr e2);
void Integer_times(Expr e1, Expr e2);
void Integer_power(Expr e1, Expr e2);
void Integer_power_real(Expr e1, Expr e2);
int intmsb(int i);
*/
void Lambda_data_init(union un_data *data);
void Lambda_data_finish(union un_data *data);
void Lambda_data_copy(union un_data *datas, union un_data *datad);
char *Lambda_toString(Expr expr);
Expr Lambda_pre(Expr expr);
Expr Lambda(Expr expr);
Expr ReplaceAll_mod_mod(Expr expr, Expr pat, Expr rep);
Expr Last(Expr expr);
Expr Last_mod(Expr expr);
Expr Length(Expr expr);
Expr Log(Expr expr);
extern int message_flag;
Expr Map(Expr expr);
Expr Map_mod(Expr func, Expr expr, int ns, int ne, int n);
extern int message_flag;
Expr MapAll(Expr expr);
Expr MatchQ(Expr expr);
int mark(Expr expr, Expr pattern, Expr head, int callback);
void showsub(Expr expr);
void unmark(Expr expr);
enum en_mqrtn{
	mqr_unmatch=0,
	mqr_match=1,
	mqr_missing=2,
};
extern int output_flag;
Expr MatrixForm_mod(Expr expr);
Expr MatrixForm(Expr expr);
Expr Minus(Expr expr);
extern int message_flag;
Expr N(Expr expr);
Expr N_mod(Expr expr, long prec);
void calce64(double *real, long prec);
void calce(mpfr_t real, long prec);
int NumberQ_mod(Expr expr);
Expr NumberQ(Expr expr);
extern int output_flag;
void parser_print();
Expr Operators(Expr expr);
Expr Order(Expr expr);
int Order_mod(Expr e1, Expr e2);
void coeff(Expr expr, Expr *v, Expr *e, Expr *c1, Expr *c2);
extern int output_flag;
Expr OutputForm(Expr expr);
Expr OutputForm_mod(Expr expr);
char *OutputForm_mod_str(Expr expr);
typedef struct st_form *Form;
typedef struct st_form{
	enum{FormS,FormV,FormH}tag;
	union{
		char *string;
		Form head;
	}content;	
	int height;
	int width;
	int level;
	Form next;
}Form_Sub;
enum en_fm{
	fm_sub=0x0001,
	fm_nosign=0x0002,
	fm_par=0x0004,
	fm_oneline=0x0008,
};
Form Form_appendHorizontal(Form form, Form form2);
Form Form_appendVertical(Form form, Form form2);
Form Form_appendVerticalRight(Form form, Form form2);
Form Form_appendHorizontalCenter(Form form, Form form2);
Form Form_appendVerticalCenter(Form form, Form form2);
Form Form_make(Expr expr, int flag);
void Form_print(Form form);
Form Form_create(char *string);
void Form_delete(Form form);
Expr Part(Expr expr);
Expr Part_mod(Expr rtn, Expr main , Expr spec, int *err);
void Pattern_data_init(union un_data *data);
void Pattern_data_finish(union un_data *data);
void Pattern_data_copy(union un_data *datas, union un_data *datad);
char *Pattern_toString(Expr expr);
extern int ofp_plus;
Expr Plus(Expr expr);
void Plus_mod(Expr *e1, Expr e2);
void coeff2(Expr expr, Expr *c, Expr *v);
Expr Postfix(Expr expr);
extern int ofp_power;
Expr Power(Expr expr);
int Power_mod(Expr e1, Expr e2);
Expr Precision(Expr expr);
long Precision_mod(Expr expr);
Expr Prefix(Expr expr);
Expr PreIncrement(Expr expr);
Expr Product(Expr expr);
Expr Quit(Expr expr);
Expr RandomInteger(Expr expr);
Expr RandomInteger_mod(long min, long max);
Expr RandomInteger_mod_list(long min, long max, Expr list);
Expr RandomReal(Expr expr);
Expr RandomReal_mod(double min, double max);
Expr RandomReal_mod_list(double min, double max, Expr list);
#include <float.h>
#define REAL_PREC real.prec
#define REAL_MP real.data.mp
#define REAL_AP real.data.ap
double Real_double(Expr expr);
int Real_sign(Expr expr);
char *Real_toString(Expr expr);
char *real64_toString(double d, long decimal);
char *mpfr_toString(mpfr_t op, long decimal);
Expr Real_create(char *string);
Expr Real_createDouble(double d);
long calcprec(const char *string);
void Real_data_init(union un_data *data);
void Real_data_finish(union un_data *data);
void Real_data_copy(union un_data *datas, union un_data *datad);
int Real_data_equals(union un_data *data1, union un_data *data2);
/*
void Real_plus(Expr e1, Expr e2);
void Real_power(Expr e1, Expr e2);
void Real_power_integer(Expr e1, Expr e2);
*/
extern int message_flag;
int mark(Expr expr, Expr pattern, Expr head, int callback);
Expr Replace(Expr expr);
Expr Replace_mod(Expr expr, Expr rules, int ls, int le);
Expr Replace_mod_mod(Expr expr, Expr pat, Expr rep, int level, int ls, int le);
Expr Replace_copy_mod(Expr list, Expr rep);
Expr Replace_makelist(Expr root, Expr pat);
extern int message_flag;
int mark(Expr expr, Expr pattern, Expr head, int callback);
Expr ReplaceAll(Expr expr);
Expr ReplaceAll_mod(Expr expr, Expr rules);
Expr ReplaceAll_mod_mod(Expr expr, Expr pat, Expr rep);
Expr Replace_makelist(Expr root, Expr pat);
Expr Replace_copy_mod(Expr list, Expr rep);

Expr Row_mod(Expr expr, Expr sep);
Expr Row(Expr expr);
typedef struct st_form *Form;
Form Form_appendHorizontal(Form form, Form form2);
Form Form_make(Expr expr, int flag);
Expr SameQ(Expr expr);
int SameQ_mod(Expr e1, Expr e2);
extern int output_flag;
Expr ScientificForm(Expr expr);
char *Integer_toString(Expr expr);
char *Real_toString(Expr expr);
long Integer_toInt(Expr expr);
char *real64_toString(double d, long decimal);
char *mpfr_toString(mpfr_t op, long decimal);
extern int output_flag;
Expr SeedRandom(Expr expr);
Expr Sequence(Expr expr);
Expr Set(Expr expr);
Expr SetDelayed(Expr expr);
int Sign_mod(Expr expr);
Expr Sign(Expr expr);
Expr Sin(Expr expr);
Expr Integer_createInt(int k);
Expr Slot_pre(Expr expr);
Expr Integer_createInt(int k);
Expr SlotSequence_pre(Expr expr);
Expr Sort(Expr expr);
Expr Sort_mod(Expr expr);
Expr Sort_merge(Expr a, Expr b);
void Sort_split(Expr source, Expr *front, Expr *back);
Expr Span(Expr expr);
Expr Sqrt(Expr expr);
char *String_toString(Expr expr);
Expr String_create(char *string);
void String_data_init(union un_data *data);
void String_data_finish(union un_data *data);
void String_data_copy(union un_data *datas,union un_data *datad);
int String_data_equals(union un_data *d1, union un_data *d2);
Expr Subtract(Expr expr);
Expr Sum(Expr expr);
extern int message_flag;
Expr Table(Expr expr);
Expr Tan(Expr expr);
extern int message_flag;
Expr Thread(Expr expr);
Expr Thread_mod(Expr expr, Symbol head, int *seq);
int *getseq(Expr expr, int len);
extern int ofp_times;
Expr Times(Expr expr);
void Times_mod(Expr *e, Expr e1, Expr e2);
void coeff3(Expr expr, Expr *v, Expr *e);
extern int output_flag;
Expr TreeForm_mod(Expr expr);
Expr TreeForm_modh(Expr expr);
Expr TreeForm(Expr expr);
extern int output_flag;
Expr TreeHorizontalForm(Expr expr);

/*=== internal ===*/
extern int output_flag;
Expr DebugForm(Expr expr);
extern int message_flag;
Expr EvaluateBuiltinRules(Expr expr);
Expr Node(Expr expr);
char *Node_toString(Expr expr);
void Node_data_init(union un_data *data);
void Node_data_copy(union un_data *datas, union un_data *datad);
Expr PatternB(Expr expr);
Expr PatternB2(Expr expr);
Expr PatternB3(Expr expr);
Expr Pointer_create(Expr e);
void Pointer_data_init(union un_data *data);
void Pointer_data_finish(union un_data *data);
void Pointer_data_copy(union un_data *datas, union un_data *datad);
char *PointerHead_toString(Expr expr);
Expr PointerHead_create(Expr e);
void PointerHead_data_init(union un_data *data);
void PointerHead_data_finish(union un_data *data);
void PointerHead_data_copy(union un_data *datas, union un_data *datad);

/*=== physics ===*/
Expr P(Expr expr);
Expr P_mod(Expr expr, Symbol s);
void *P_static_create();
void P_static_delete(void*);
void P_doc(int id);

/*=== user ===*/
extern int output_flag;
Expr Cow(Expr expr);
extern int output_flag;
Expr Fish(Expr expr);
extern int output_flag;
Expr Hello(Expr expr);
extern int output_flag;
Expr Hi(Expr expr);
extern int output_flag;
Expr Matrix(Expr expr);
extern int output_flag;
Expr Neko(Expr expr);
extern int output_flag;
Expr Pokemon(Expr expr);
extern int output_flag;
Expr Train(Expr expr);
extern int output_flag;
Expr Yojyo(Expr expr);

/*====== symbol table ======*/
/*=== global ===*/
#define SYMBOL_GLOBAL(SYMBOL_FUNC) \
SYMBOL_FUNC(All,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(ArcTan,ArcTan,Listable|NumericFunction|Protected,670,0,NULL,NULL,Expr_createString("{逆正接}")) \
SYMBOL_FUNC(ArrayDepth,ArrayDepth,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(ArrayQ,ArrayQ,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(AtomQ,AtomQ,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Attributes,Attributes,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Binomial,Binomial,Listable | NumericFunction | Protected | ReadProtected ,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Blank,NULL,Protected,730,un_pre,"_",NULL,NULL) \
SYMBOL_FUNC(Blank2,NULL,Protected,730,un_pre,"__",NULL,NULL) \
SYMBOL_FUNC(Blank3,NULL,Protected,730,un_pre,"___",NULL,NULL) \
SYMBOL_FUNC(Bottom,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Center,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Column,Column,Protected,670,0,NULL,NULL,Expr_createString("{列}")) \
SYMBOL_FUNC(ComplexInfinity,NULL,Protected,1000,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Constant,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Cos,Cos,Listable|NumericFunction|Protected,670,0,NULL,Expr_createString("{  Cos[0]->1,  Cos[Pi]->-1,  Cos[Pi*n_Integer]->(-1)^n,  Cos[Pi/2]->0,  Cos[Pi/3]->1/2,  Cos[2*Pi/3]->-1/2,  Cos[Pi/4]->1/Sqrt[2],  Cos[3*Pi/4]->-1/Sqrt[2],  Cos[Pi/5]->(1 + Sqrt[5])/4,  Cos[2*Pi/5]->(-1 + Sqrt[5])/4,  Cos[3*Pi/5]->(1 - Sqrt[5])/4,  Cos[4*Pi/4]->(-1 - Sqrt[5])/4,  Cos[Pi/6]->Sqrt[3]/2,  Cos[5*Pi/6]->-Sqrt[3]/2 } "),Expr_createString("{余弦}")) \
SYMBOL_FUNC(Cross,Cross,Protected,500,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Depth,Depth,Protected,0,0,NULL,NULL,Expr_createString("{深さ}")) \
SYMBOL_FUNC(Dimensions,Dimensions,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Divide,Divide,Listable|NumericFunction|Protected,470,bi_left,"/",NULL,Expr_createString("{除算}")) \
SYMBOL_FUNC(Dot,Dot,Flat|OneIdentity|Protected,490,bi_none,".",NULL,NULL) \
SYMBOL_FUNC(E,NULL,Constant|Protected|ReadProtected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Equal,Equal,Protected,290,0,"==",NULL,NULL) \
SYMBOL_FUNC(Evaluate,Evaluate,Locked|Protected,670,0,NULL,NULL,Expr_createString("{評価}")) \
SYMBOL_FUNC(Exp,Exp,Listable|NumericFunction|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(False,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(First,First,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Flat,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Flatten,Flatten,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(FullForm,FullForm,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Grid,Grid,Protected,670,0,NULL,NULL,Expr_createString("{格子}")) \
SYMBOL_FUNC(Help,Help,Locked|Protected|HoldAll,1,un_post,"?",NULL,Expr_createString("{help,Info,情報,助けて}")) \
SYMBOL_FUNC(HoldAll,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(HoldAllComplete,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(HoldFirst,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(HoldRest,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Increment,Increment,HoldFirst|Protected|ReadProtected,660,un_post,"++",NULL,NULL) \
SYMBOL_FUNC(Indeterminate,NULL,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Infinity,NULL,Protected|ReadProtected,1000,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Inner,Inner,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Integer,NULL,Protected,1000,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Lambda,Lambda,HoldAll|Protected,90,un_post,"&",NULL,NULL) \
SYMBOL_FUNC(Last,Last,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Left,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Length,Length,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(List,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Listable,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Locked,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Log,Log,Listable|NumericFunction|Protected,670,0,NULL,NULL,Expr_createString("{対数}")) \
SYMBOL_FUNC(Map,Map,Protected,620,bi_right,"/@",NULL,NULL) \
SYMBOL_FUNC(MapAll,MapAll,Protected,620,bi_right,"//@",NULL,NULL) \
SYMBOL_FUNC(MatchQ,MatchQ,Protected,150,0,NULL,NULL,NULL) \
SYMBOL_FUNC(MatrixForm,MatrixForm,Protected,670,0,NULL,NULL,Expr_createString("{行列表現}")) \
SYMBOL_FUNC(Minus,Minus,Listable|NumericFunction|Protected,480,un_pre,"-",NULL,NULL) \
SYMBOL_FUNC(N,N,Locked|Protected,670,0,NULL,NULL,Expr_createString("{数値}")) \
SYMBOL_FUNC(None,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Null,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(NumberQ,NumberQ,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(NumericFunction,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(OneIdentity,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Operators,Operators,Locked|Protected|HoldAll,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Order,Order,Protected,670,0,NULL,NULL,Expr_createString("{順序}")) \
SYMBOL_FUNC(Orderless,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(OutputForm,OutputForm,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Part,Part,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Pattern,NULL,HoldFirst | Protected,150,bi_one,":",NULL,NULL) \
SYMBOL_FUNC(Pi,NULL,Constant|Protected|ReadProtected,1000,0,NULL,NULL,Expr_createString("{円周率}")) \
SYMBOL_FUNC(Plus,Plus,Flat | Listable | NumericFunction | OneIdentity | Orderless | Protected,310,bi_none,"+",NULL,Expr_createString("{加算}")) \
SYMBOL_FUNC(Postfix,Postfix,Protected|HoldAll,70,bi_left,"//",NULL,NULL) \
SYMBOL_FUNC(Power,Power,Listable | NumericFunction | OneIdentity | Protected,590,bi_right,"^",NULL,Expr_createString("{冪算}")) \
SYMBOL_FUNC(Precision,Precision,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Prefix,Prefix,Protected|HoldAll,640,bi_right,"@",NULL,NULL) \
SYMBOL_FUNC(PreIncrement,PreIncrement,HoldFirst|Protected|ReadProtected,660,un_pre,"++",NULL,NULL) \
SYMBOL_FUNC(Product,Product,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Protected,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Quit,Quit,Locked|Protected,670,0,NULL,NULL,Expr_createString("{quit,Exit,exit,終了}")) \
SYMBOL_FUNC(RandomInteger,RandomInteger,Protected,670,0,NULL,NULL,Expr_createString("{乱整数}")) \
SYMBOL_FUNC(RandomReal,RandomReal,Protected,670,0,NULL,NULL,Expr_createString("{乱実数}")) \
SYMBOL_FUNC(ReadProtected,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Real,NULL,Protected,1000,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Replace,Replace,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(ReplaceAll,ReplaceAll,Protected,110,bi_left,"/.",NULL,NULL) \
SYMBOL_FUNC(Right,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Row,Row,Protected,670,0,NULL,NULL,Expr_createString("{列}")) \
SYMBOL_FUNC(Rule,NULL,Locked|Protected,120,bi_right,"->",NULL,NULL) \
SYMBOL_FUNC(SameQ,SameQ,Protected,290,0,"===",NULL,NULL) \
SYMBOL_FUNC(ScientificForm,ScientificForm,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(SeedRandom,SeedRandom,Protected,670,0,NULL,NULL,Expr_createString("{乱数源}")) \
SYMBOL_FUNC(Sequence,Sequence,Protected,1000,0,NULL,NULL,NULL) \
SYMBOL_FUNC(SequenceHold,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Set,Set,HoldFirst|Protected|SequenceHold,40,bi_right,"=",NULL,Expr_createString("{設定}")) \
SYMBOL_FUNC(SetDelayed,SetDelayed,HoldAll|Protected|SequenceHold,40,bi_right,":=",NULL,Expr_createString("{遅延設定}")) \
SYMBOL_FUNC(Sign,NULL,Listable|Protected|ReadProtected|NumericFunction,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Sin,Sin,Listable|NumericFunction|Protected,670,0,NULL,Expr_createString("{  Sin[0]->0,  Sin[Pi]->0,  Sin[Pi*n_Integer]->0,  Sin[Pi/2]->1,  Sin[Pi/3]->Sqrt[3]/2,  Sin[2*Pi/3]->Sqrt[3]/2,  Sin[Pi/4]->1/Sqrt[2],  Sin[3*Pi/4]->1/Sqrt[2],  Sin[Pi/5]->Sqrt[(5 - Sqrt[5])/8],  Sin[2*Pi/5]->Sqrt[(5 + Sqrt[5])/8],  Sin[3*Pi/5]->Sqrt[(5 + Sqrt[5])/8],  Sin[4*Pi/4]->Sqrt[(5 - Sqrt[5])/8],  Sin[Pi/6]->Sqrt[3]/2,  Sin[5*Pi/6]->-Sqrt[3]/2 } "),Expr_createString("{正弦}")) \
SYMBOL_FUNC(Slot,NULL,Protected,740,un_pre,"#",NULL,NULL) \
SYMBOL_FUNC(SlotSequence,NULL,Protected,740,un_pre,"##",NULL,NULL) \
SYMBOL_FUNC(Sort,Sort,Protected,670,0,NULL,NULL,Expr_createString("{整列}")) \
SYMBOL_FUNC(Span,Span,Protected,305,asso_special,";;",NULL,NULL) \
SYMBOL_FUNC(Sqrt,Sqrt,Listable|NumericFunction|Protected,670,un_pre,"^/",NULL,NULL) \
SYMBOL_FUNC(String,NULL,Protected,1000,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Subtract,Subtract,Listable|NumericFunction|Protected,310,bi_left,"-",NULL,Expr_createString("{減算}")) \
SYMBOL_FUNC(Sum,Sum,Protected,670,0,NULL,NULL,Expr_createString("{総和}")) \
SYMBOL_FUNC(Symbol,NULL,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Table,Table,HoldAll|Protected,670,0,NULL,NULL,Expr_createString("{表}")) \
SYMBOL_FUNC(Tan,Tan,Listable|NumericFunction|Protected,670,0,NULL,Expr_createString("{  Tan[0]->0,  Tan[Pi]->0,  Tan[Pi*n_Integer]->0,  Tan[Pi/2]->ComplexInfinity,  Tan[Pi/3]->Sqrt[3],  Tan[2*Pi/3]->-Sqrt[3],  Tan[Pi/4]->1,  Tan[3*Pi/4]->-1,  Tan[Pi/5]->Sqrt[5 - 2*Sqrt[5]],  Tan[2*Pi/5]->Sqrt[5 + 2*Sqrt[5]],  Tan[3*Pi/5]->-Sqrt[5 + 2*Sqrt[5]],  Tan[4*Pi/4]->-Sqrt[5 - 2*Sqrt[5]],  Tan[Pi/6]->1/Sqrt[3],  Tan[5*Pi/6]->-Sqrt[3]/2 } "),Expr_createString("{正接}")) \
SYMBOL_FUNC(Temporary,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Thread,Thread,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Times,Times,Flat | Listable | NumericFunction | OneIdentity | Orderless | Protected,400,bi_none,"*",NULL,Expr_createString("{乗算}")) \
SYMBOL_FUNC(Top,NULL,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(TreeForm,TreeForm,Protected,670,0,NULL,NULL,Expr_createString("木")) \
SYMBOL_FUNC(TreeHorizontalForm,TreeHorizontalForm,Protected,670,0,NULL,NULL,Expr_createString("{木横}")) \
SYMBOL_FUNC(True,NULL,Protected,670,0,NULL,NULL,NULL)

/*=== internal ===*/
#define SYMBOL_INTERNAL(SYMBOL_FUNC) \
SYMBOL_FUNC(DebugForm,DebugForm,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(EvaluateBuiltinRules,EvaluateBuiltinRules,Locked|Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Node,Node,Protected,1000,0,NULL,NULL,NULL) \
SYMBOL_FUNC(PatternB,PatternB,Protected,730,bi_one,"_",NULL,NULL) \
SYMBOL_FUNC(PatternB2,PatternB2,Protected,730,bi_one,"__",NULL,NULL) \
SYMBOL_FUNC(PatternB3,PatternB3,Protected,730,bi_one,"___",NULL,NULL) \
SYMBOL_FUNC(Pointer,NULL,Protected,1000,0,NULL,NULL,NULL) \
SYMBOL_FUNC(PointerHead,NULL,Protected,1000,0,NULL,NULL,NULL) \
SYMBOL_FUNC(PreSymbolMark,NULL,Protected,660,un_pre,"$",NULL,NULL) \
SYMBOL_FUNC(SymbolMark,NULL,Protected,660,un_post,"$",NULL,NULL)

/*=== physics ===*/
#define SYMBOL_PHYSICS(SYMBOL_FUNC) \
SYMBOL_FUNC(P,P,Protected|Constant,670,0,NULL,NULL,Expr_createString("{物理定数}")) \
SYMBOL_FUNC(Wce,NULL,Protected|Constant,670,0,NULL,Expr_createString("{  Wce[B_] -> (e B)/me,  Wce -> (e B)/me } "),Expr_createString("{電子サイクロトロン角振動数}")) \
SYMBOL_FUNC(Wpe,NULL,Protected|Constant,670,0,NULL,Expr_createString("{  Wpe[n_] -> Sqrt[(n e^2)/(e0 me)],  Wpe -> Sqrt[(n e^2)/(e0 me)] } "),Expr_createString("{電子プラズマ角振動数}"))

/*=== user ===*/
#define SYMBOL_USER(SYMBOL_FUNC) \
SYMBOL_FUNC(Cow,Cow,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Fish,Fish,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Hello,Hello,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Hi,Hi,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Matrix,Matrix,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Neko,Neko,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Pokemon,Pokemon,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Train,Train,Protected,670,0,NULL,NULL,NULL) \
SYMBOL_FUNC(Yojyo,Yojyo,Protected,670,0,NULL,NULL,NULL)

/*====== symbol id ======*/
enum symbol_id{
id_global_start,
id_All,
id_ArcTan,
id_ArrayDepth,
id_ArrayQ,
id_AtomQ,
id_Attributes,
id_Binomial,
id_Blank,
id_Blank2,
id_Blank3,
id_Bottom,
id_Center,
id_Column,
id_ComplexInfinity,
id_Constant,
id_Cos,
id_Cross,
id_Depth,
id_Dimensions,
id_Divide,
id_Dot,
id_E,
id_Equal,
id_Evaluate,
id_Exp,
id_False,
id_First,
id_Flat,
id_Flatten,
id_FullForm,
id_Grid,
id_Help,
id_HoldAll,
id_HoldAllComplete,
id_HoldFirst,
id_HoldRest,
id_Increment,
id_Indeterminate,
id_Infinity,
id_Inner,
id_Integer,
id_Lambda,
id_Last,
id_Left,
id_Length,
id_List,
id_Listable,
id_Locked,
id_Log,
id_Map,
id_MapAll,
id_MatchQ,
id_MatrixForm,
id_Minus,
id_N,
id_None,
id_Null,
id_NumberQ,
id_NumericFunction,
id_OneIdentity,
id_Operators,
id_Order,
id_Orderless,
id_OutputForm,
id_Part,
id_Pattern,
id_Pi,
id_Plus,
id_Postfix,
id_Power,
id_Precision,
id_Prefix,
id_PreIncrement,
id_Product,
id_Protected,
id_Quit,
id_RandomInteger,
id_RandomReal,
id_ReadProtected,
id_Real,
id_Replace,
id_ReplaceAll,
id_Right,
id_Row,
id_Rule,
id_SameQ,
id_ScientificForm,
id_SeedRandom,
id_Sequence,
id_SequenceHold,
id_Set,
id_SetDelayed,
id_Sign,
id_Sin,
id_Slot,
id_SlotSequence,
id_Sort,
id_Span,
id_Sqrt,
id_String,
id_Subtract,
id_Sum,
id_Symbol,
id_Table,
id_Tan,
id_Temporary,
id_Thread,
id_Times,
id_Top,
id_TreeForm,
id_TreeHorizontalForm,
id_True,
id_global_end,
id_internal_start,
id_DebugForm,
id_EvaluateBuiltinRules,
id_Node,
id_PatternB,
id_PatternB2,
id_PatternB3,
id_Pointer,
id_PointerHead,
id_PreSymbolMark,
id_SymbolMark,
id_internal_end,
id_physics_start,
id_P,
id_Wce,
id_Wpe,
id_physics_end,
id_user_start,
id_Cow,
id_Fish,
id_Hello,
id_Hi,
id_Matrix,
id_Neko,
id_Pokemon,
id_Train,
id_Yojyo,
id_user_end,
id_end,
};

/*proto of print function*/
char *Expr_toString(Expr expr);

#define DOC_INDENT 26
/*proto of doc function*/
void doc_global(int id);
void doc_internal(int id);
void doc_physics(int id);
void doc_user(int id);

/*proto of static field create function*/
void *static_create(int id);

/*proto of static field finish function*/
void static_delete(void *data, int id);


/*proto of SymbolTable initialize function*/
SymbolTable SymbolTable_global(Symbol idtable[], char *nametable[]);
SymbolTable SymbolTable_internal(Symbol idtable[], char *nametable[]);
SymbolTable SymbolTable_physics(Symbol idtable[], char *nametable[]);
SymbolTable SymbolTable_user(Symbol idtable[], char *nametable[]);

#define ADD_CONTEXT(idtable,nametable) \
SymbolTable_addContext("Global`",doc_global,SymbolTable_global(idtable,nametable)); \
SymbolTable_addContext("Internal`",doc_internal,SymbolTable_internal(idtable,nametable)); \
SymbolTable_addContext("Physics`",doc_physics,SymbolTable_physics(idtable,nametable)); \
SymbolTable_addContext("User`",doc_user,SymbolTable_user(idtable,nametable));

#endif
