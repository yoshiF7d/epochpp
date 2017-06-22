#include "knowledge.h"
int isint(char **str){
	char *p=*str;
	int rtn=0;
	
	if(isdigit(*p)){
		if(*p=='0'){
			if(isdigit(*++p)){
				rtn=0; goto end;
			}else{
				rtn=1; goto end;
			}
		}
	}else{goto end;}
	while(isdigit(*p)){p++;}
	rtn=1;
  end:
	if(rtn){
		*str = p; 
		//printf("int\n"); putchar(*p); putchar('\n');
	}
	return rtn;
}

/*not exp : 0*/
/*exp is plus : 1*/
/*exp is minus : 2*/
int isexp(char **str){
	char *p=*str;
	int rtn=0;
	int minus=0;
	
	if(*p=='e' || *p=='E'){
		p++;
		if(*p=='-' || *p=='+'){
			if(*p=='-'){minus=1;}
			p++;
			if(isdigit(*p)){
				goto mid;
			}
		}else if(isdigit(*p)){
			goto mid;
		}
	}
	rtn=0;
	goto end;
  mid:
	while(isdigit(*p)){p++;}
	if(minus){rtn=2;}
	else{rtn=1;}
	goto end;
  end:
	if(rtn){
		*str = p;
		//printf("exp\n"); putchar(*p); putchar('\n');
	}
	return rtn;
}

/*not number : 0*/
/*int : 1 */
/*float : 2*/
/*int expressed by expression using exponent : 3*/
int isnum(char **str){
	char *p=*str;
	int rtn=0,e;
	
	if(isint(&p)){
		if(*p=='.'){rtn=2;p++;}
		else{rtn=1;}
	}else if(*p=='.'){
		if(!isdigit(*++p)){goto end;}
		else{rtn=2;}
	}
	while(isdigit(*p)){p++;}
  end:
	if(rtn){
		*str = p;
		e=isexp(str);
		if(rtn==1){
			if(e==1){rtn=3;}
			else if(e==2){rtn=2;}
		}
	}
	return rtn;
}

int issymbol(char **str){
	char *p = *str, rtn=0;
	
	for(;;){
		if(*p & 0x80){
			/*when most significant bit is up, it's a multibyte character*/
			rtn=2; while(*p & 0x80){p++;}
		}else if(isalpha(*p)){/*<- isalpha ALPHABET ONLY*/
		  read:
			rtn=1; while(isalnum(*p)){p++;}  /*<- isalnum ALPHABET OR NUMBER*/
		}else if(rtn==2){
			if(isalnum(*p)){goto read;}
			break;
		}else if(rtn==0){
			return 0;
		}else{
			break;
		}
	}
	*str=p; 
	return 1;
}

typedef struct token{
	Expr expr;
	char *p;
}Token;

enum en_tkm{
	tkm_init,
	tkm_setfunc,
	tkm_get,
	tkm_build,
	tkm_finish,
	tkm_print,
	tkm_calltree
};

void tokenizer_init(Context context);
void tokenizer_setfunc(Expr (*create)(char *));
void tokenizer_get(Token *token);
void tokenizer_vbuild(void *s);
void tokenizer_print();
void tokenizer_finish();

Expr tokenizer(Context context, Token *token, Symbol symbol,Expr (*create)(char *) ,enum en_tkm mode){
	static Expr expr;
	static Expr (*create_imp)(char*) = Expr_createByName;
	Expr e=NULL,last=NULL;
	char *p,*pint=NULL;
	char sav,escape=0;
	Context c;
	
	switch(mode){
	  case tkm_init:
		expr = last = Expr_create(id_Node);
		for(c=context;c;c=c->next){HashTable_enum(c->table,tokenizer_vbuild);}
		//TreeForm_mod(expr);
		break;
	  case tkm_setfunc:
		create_imp = create;
		break;
	  case tkm_get:
		token->expr=NULL;
		p = token->p;
		while(isblank(*p)){p++;}
		token->p = p;
		if(*p==0){return NULL;}
		if(issymbol(&p)){
			sav=*p; *p='\0';
			token->expr = create_imp(token->p);
			token->expr->flag |= ef_full;
			*p=sav;
		}else if((sav=isnum(&p))){
			switch(sav){
			  case 1:
				//puts("int");
				sav = *p; *p='\0';
				token->expr = Integer_create(token->p);
				*p = sav;
				break;
			  case 2:
				//puts("real");
				sav = *p; *p='\0';
				token->expr = Real_create(token->p);
				*p = sav;
				break;
			  case 3:
				//puts("expint");
				sav = *p; *p='\0';
				token->expr = Integer_createExp(token->p);
				*p = sav;
				break;
			}
		}else if(*p=='"'){
			while(*++p){
				if(escape){
					switch(*p){
					  case '"':
						StringList_collect('"');
						break;
					  case 'n':
						StringList_collect('\n');
						break;
					  case 't':
						StringList_collect('\t');
						break;
					  case '\\':
						StringList_collect('\\');
						break;
					}
					escape=0;
				}else if(*p=='\\'){
					escape=1;
					continue;
				}else if(*p=='"'){
					StringList_collect('\0');
					token->expr = String_create(StringList_release());
					p++;
					break;
				}else{
					StringList_collect(*p);
				}
			}
			if(token->expr==NULL){perror("error : unclosed double quotes\n");deallocate(StringList_release());}
		}else{
			/*string matching in automaton way*/
			/*tokenizer have a tree consisting of leafs having a character*/
			/*scanning string is done by checking leafs one by one comparing given character and a character in the leaf*/
			/*if a character matches at one leaf, move on to next character and update tree to the leaf*/
			/*if the leaf also have leafs, matching progress.*/
			/*Node represents a transition state*/
			/*other symbol represents an accepted state. the symbol means that given string matched with that symbol*/
			if(!expr){goto get_end;}
			last=expr;
		  get_loophead:
			for(e=last->child;e;e=e->next){
				if(e->symbol->id==id_Node){
					if(e->data.letter==*p){
						last = e;
						p++;
						goto get_loophead;
					}
				}else{
					token->expr=e;
				}
			}
		}
	  get_end:
		token->p=p;
		break;
	  case tkm_build:
		if(symbol->sign){
			for(last=expr,p=symbol->sign;;){
				for(e=last->child;e;e=e->next){
					if(e->data.letter == *p){
						last = e;
						goto build_loopend;
					}
				}
				Expr_appendChild(last,e=Expr_create(id_Node));
				e->data.letter = *p;
				last = e;
				 build_loopend:
				p++;
				if(*p=='\0'){break;}
			}
			Expr_appendChild(last,Expr_createBySymbol(symbol));
		}
		break;
	  case tkm_finish:
		Expr_deleteRoot(expr); expr=NULL;
		break;
	  case tkm_print:
		TreeForm_modh(expr);
		break;
	  case tkm_calltree:
		return expr;
	}
	return NULL;
}

void tokenizer_init(Context context){tokenizer(context,NULL,NULL,NULL,tkm_init);}
void tokenizer_setfunc(Expr (*create)(char *)){tokenizer(NULL,NULL,NULL,create,tkm_setfunc);}
void tokenizer_get(Token *token){tokenizer(NULL,token,NULL,NULL,tkm_get);}
void tokenizer_vbuild(void *s){tokenizer(NULL,NULL,s,NULL,tkm_build);}
void tokenizer_finish(){tokenizer(NULL,NULL,NULL,NULL,tkm_finish);}
void tokenizer_print(){tokenizer(NULL,NULL,NULL,NULL,tkm_print);}
Expr tokenizer_calltree(){return tokenizer(NULL,NULL,NULL,NULL,tkm_calltree);}
/*making tree conserning associativity of operators*/
/*given node is appended to last node at first.*/
/*usually nodes with higher precedence have lower position in a tree (far from root) except the case when parenthesis comes in.*/
/*to make correct precedence order, newly appended node checks its parent and parent of parent and etc..*/
/*to find correct position in a tree, after that, it climbs up the tree toward the root. yes this tree lives in upside down manner*/
int climb(Expr *last, Expr *expr, int p1){
	Expr e;
	int p2;
	//printf("%d\n",p1);
	for(e=*last;;e=e->parent){
		p2 = e ? e->symbol->precedence : 0;
		//c = e ? e->child : NULL;
		//asso = e ? e->symbol->associativity : 0;
		//puts("1");
		if(p1>=p2){
			if(p2==1000){
				/*a b -> a*b */
				*expr = e;
				return 1;
			}
			break;
		}
		*last = e;
	}
	*expr = e;
	return 0;
}

void climbPrecedence(Expr *root, Expr *last, Expr expr){
	Expr e,c;
		/*climbing phase*/
	
	if(climb(last,&e,expr->symbol->precedence)){
		climbPrecedence(root,last,Expr_create(id_Times));
		climbPrecedence(root,last,expr);
		return;
	}
	//puts("bond");
	/*bonding phase*/
	if(e){
		if(!AtomQ_mod(e)){
			switch(e->symbol->associativity){
			  case bi_left:
				if(e->symbol->precedence==expr->symbol->precedence){
					if(!e->parent){*root = *last = Expr_appendChild(expr,e);}
					else{Expr_insert(e->parent,e,expr); *last = expr;}
					return;
				}
				break;
			  case bi_none:
				if(e->symbol==expr->symbol){
					*last = e;
					Expr_delete(expr); 
					return;
				}
				break;
			  case un_post:
			  //case un_pre:
				/*make sure unary operator won't bond more than 1 leaf*/
				/*"a++b" should not be converted to Increment[a,b]*/
				if(e==*last && e->child){
					*last = Expr_create(id_Node);
					if(!e->parent){*root = Expr_appendChild(*last,e);}
					else{Expr_insert(e->parent,e,*last);}
					climbPrecedence(root,last,expr);
					return;
				}
				break;
			  case bi_one:
				/*make sure bi_one operator won't have more than 2 leafs*/
				if(e==*last && e->child){
					if(e->child->next){
						*last = Expr_create(id_Node);
						if(!e->parent){*root = Expr_appendChild(*last,e);}
						else{Expr_insert(e->parent,e,*last);}
						climbPrecedence(root,last,expr);
						return;
					}
				}
			  default:
				break;
			}
		}
		Expr_insert(e,*last,expr); 
		*last = expr;
	}else{
		*root = *last = Expr_appendChild(expr,*last);
	}
}

#define TAG_PART -1

enum en_bemode{
	bem_default=0,
	bem_par,
	bem_square,
	bem_curly,
	bem_part
};
int parse_span(Expr *root, Expr *last, int lastasso);
Expr PreEvaluate(Expr expr);
Expr buildexpr(char **str, int mode){
	Token token = {NULL,*str};
	Expr root=NULL, last=NULL,e,e2,e3;
	int lastasso=0,p1,p2,timesflag=0;
	char c;
	/*
	switch(mode){
	  case bem_default:
		fprintf(stderr,"bem_default\n"); break;
	  case bem_par:
		fprintf(stderr,"bem_par\n"); break;
	  case bem_square:
		fprintf(stderr,"bem_square\n"); break;
	  case bem_curly:
		fprintf(stderr,"bem_curly\n"); break;
	  case bem_part:
		fprintf(stderr,"bem_part\n"); break;
	}
	*/
	while(*(token.p)){
		//fprintf(stderr,"%c\n",*(token.p));
		tokenizer_get(&token);
		if(token.expr){
			if(token.expr->symbol->associativity == asso_special){
				if(token.expr->flag & ef_full){goto climb;}
				if(token.expr->symbol->id == id_Span){
					//puts("span");
					if(parse_span(&root,&last,lastasso)){
						//puts("span c");
						token.expr = Expr_create(id_Span);
						goto climb;
					}else{
						//puts("span nc");
						goto noclimb;
					}
				}
			}else{
				switch(token.expr->symbol->id){
				/*when token is an atom, token is already allocated in tokenize phase. so go to next phase.*/
				  case id_Integer:
				  case id_Real:
				  case id_String:
					break;
				  default:
					if(token.expr->flag & ef_full){
						break;
					}
					/*whent token is an operators like + -.*/
					/*simple operators are handeled automatically using the information of SIGN>,<ASSOCIATIVITY> tags written in seed files*/
					/*if token have multiple interpretations, decide which one to use according to operator associativity*/
					if(!root || lastasso == bi_left || lastasso == bi_right || lastasso == bi_none || lastasso==bi_one){
						//printf("if\n");
						//TreeForm_mod(token.expr->parent);
						for(e=token.expr;e;e=e->previous){
							if(e->symbol->associativity==un_pre){
								//printf("pre\n");
								token.expr = Expr_copy(e);
								goto climb;
							}
						}
					}else{
						for(e=token.expr;e;e=e->previous){
							if(e->symbol->id != id_Node && e->symbol->associativity!=un_pre){
								//printf("not pre\n");
								token.expr = Expr_copy(e);
								goto climb;
							}
						}
					}
					token.expr = Expr_copy(token.expr);
					break;
				}
			}
		}else{
			/*exceptional characters that needs a tokenizer.*/
			c=*(token.p)++;
			switch(c){
			  case '[':
				*str = token.p;
				if(*(token.p)=='['){
					*str = token.p+1;
					Expr_insert(e2=last->parent,last,e=Expr_create(id_Part));
					for(;**str;){
						e3=buildexpr(str,bem_part);
						if(!e3){e3=Expr_create(id_Null);}
						Expr_appendChild(e,e3);
						if(*(*str-1)==']'){
							if(**str==']'){break;}
						}
					}
					if(!e2){root = last = e;}
					else{last = e;}
					token.p = *str+1;
					goto noclimb;
				}
				if(!last){perror("this expression have no head. more input is needed\n"); return root;}
				else if(last->child){
					/*expression like (++b)[a]*/
					/*The more complicate the code becomes, The more generally the program behaves ?*/
					/*=== thin veneer code ===*/
					if(last->symbol->id != id_Node){
						if(last->parent){
							Expr_insert(last->parent,last,e=Expr_create(id_Node));
							last = e;
						}else{
							root = Expr_appendChild(Expr_create(id_Node),last);
							last = root;
						}
					}
					last->child = PreEvaluate(last->child);
					if(last->child->symbol->id!=id_Lambda){
						if(last->child->child || last->child->next){
							last->child->parent = NULL;
							last->child = PointerHead_create(last->child);
						}
					}
					e2=last->parent; e=last; last = last->child;
					/*=== thin veneer code ===*/
				}else{
					/*prevent additional node won't climb up higher than last node.*/
					Expr_insert(e2=last->parent,last,e=Expr_create(id_Node));
				}
				last->flag |= ef_func; /*function flag is used to distinguish expression like a[] and a*/
			for(;**str;){
					e3=buildexpr(str,bem_square);
					if(!e3){e3=Expr_create(id_Null);}
					Expr_appendChild(last,e3);
					if(*(*str-1)==']'){break;}
				}
				if(!e2){root = last = e;}
				else{last = e;}
				token.p = *str;
				goto noclimb;
			  case '{':
				*str = token.p;
				Expr_appendChild(last,e=Expr_create(id_List));
				if(!last){root = last = e;}
				last = e;
				Expr_insert(e2=last->parent,last,e=Expr_create(id_Node));
				for(;**str;){
					e3=buildexpr(str,bem_curly);
					if(!e3){e3=Expr_create(id_Null);}
					Expr_appendChild(last,e3);
					if(*(*str-1)=='}'){break;}
				}
				if(!e2){root = last = e;}
				else{last = e;}
				token.p = *str;
				goto noclimb;
			  case '(':
				*str = token.p;
				token.expr=Expr_appendChild(Expr_create(id_Node),buildexpr(str,bem_par));
				token.p = *str;
				//TreeForm_mod(root);
				//TreeForm_mod(token.expr);
				break;
			  case ',':
				if((mode!=bem_square && mode!=bem_curly && mode!=bem_part)){fprintf(stderr,"buildexpr : unclosed comma\n");}
				goto build_end;
			  case ']':
				if(mode!=bem_square && mode!=bem_part){fprintf(stderr,"buildexpr : unclosed square bracket\n");}
				goto build_end;
			  case '}':
				if(mode!=bem_curly){fprintf(stderr,"buildexpr : unclosed curly bracket\n");}
				goto build_end;
			  case ')':
				if(mode!=bem_par){fprintf(stderr,"buildexpr : unclosed parenthesis\n");}
				goto build_end;
			  case '\0':
				if(!token.expr){goto build_end;}
			  default:
				/*skip token*/
				if(!token.expr){continue;}
				break;
			}
		}
	  climb:
		/*if token is a full form symbol like Plus it's treated as normal symbol*/
		climbPrecedence(&root,&last,token.expr);
	  noclimb:
		lastasso=last->symbol->associativity;
	}
  build_end:
	if(mode==bem_par){root =  Expr_appendChild(Expr_create(id_Node),root);}
	if(last){
		if(last->symbol->id == id_Span){
			if(last->child){
				if(last->child->next == NULL){
					Expr_appendChild(last,Expr_create(id_All));
				}
			}
		}
	}
	*str = token.p;
	return root;
}

#define GO_CLIMB 1
#define NO_CLIMB 0
#define PRECEDENCE_SPAN 305
int parse_span(Expr *root, Expr *last, int lastasso){
	Expr e,e1,e2;
	int childlen=0,p;
	climb(last,&e,PRECEDENCE_SPAN);
	//TreeForm_mod(e);
	if(e){
		if(e->symbol->id == id_Span){
			if(e->child){
				childlen = Expr_getLength(e->child);
				if(childlen==1){
					/*i ;; ;;*/
					Expr_appendChild(e,Expr_create(id_All));
					*last = e;
					//puts("span1");
				}else if(childlen==2){
					/*i ;; j ;;*/
					//puts("span2");
					*last = e;
					return NO_CLIMB;
				}else{
					/*(i ;; j ;; k) ;;*/
					//puts("span3");
					Expr_insert(e2=e->parent,e,e1=Expr_create(id_Times));
					*last = e;
					if(!e2){*root = *last = e1;}
					return NO_CLIMB;
				}
			}else{
				/*;; ;;*/
				Expr_appendChild(e,Integer_createInt(1));
				Expr_appendChild(e,Expr_create(id_All));
				//puts("span4");
			}
			*last = e;
			return NO_CLIMB;
		}
	}
	/* ;; */
	if(!(*root) || lastasso == bi_left || lastasso == bi_right || lastasso == bi_none || lastasso==bi_one){
		if(!(*root)){
			*root = *last = Expr_create(id_Span);
			Expr_appendChild(*last,Integer_createInt(1));
		}else{
			Expr_appendChild(*last,e=Expr_create(id_Span));
			Expr_appendChild(e,Integer_createInt(1));
			*last = e;
		}
		//puts("span5");
		return NO_CLIMB;
	}
	/*i ;; */
	//puts("span6");
	return GO_CLIMB;
}
#undef GO_CLIMB
#undef NO_CLIMB

/*remove internal nodes and intermediate representations*/
Expr PreEvaluate(Expr expr){
	Expr e=NULL;
	if(!expr){return NULL;}
	for(e=expr->child;e;e=e->next){
		e=PreEvaluate(e);
	}
	switch(expr->symbol->id){
	  case id_Minus:
		return Minus(expr);
	  case id_Subtract:
		return Subtract(expr);
	  case id_Divide:
		return Divide(expr);
	  case id_Blank:
	  case id_Blank2:
	  case id_Blank3:
		return Blank_pre(expr);
	  case id_PatternB:
		return PatternB(expr);
	  case id_PatternB2:
		return PatternB2(expr);
	  case id_PatternB3:
		return PatternB3(expr);
	  case id_Node:
		return Node(expr);
	  case id_Lambda:
		return Lambda_pre(expr);
	  case id_Slot:
		return Slot_pre(expr);
	  case id_SlotSequence:
		return SlotSequence_pre(expr);
	  default:
		break;
	}
	return expr;
}

void parser_init(Context context){
	tokenizer_init(context);
}

void parser_setfunc(Expr (*create)(char *)){
	tokenizer_setfunc(create);
}

Expr parser_calltree(){
	return tokenizer_calltree();
}
void parser_finish(){
	tokenizer_finish();
}
void parser_print(){
	tokenizer_print();
}

Expr parse(char *str){	
	Expr expr = buildexpr(&str,bem_default);
	//TreeForm_mod(expr);
	expr = PreEvaluate(expr);
	/*using LEAKDETECT improperly I'm lazy*/
  	/*
#ifdef LEAKDETECT
	TreeForm_mod(expr);
#endif
	*/
	return expr;
}
