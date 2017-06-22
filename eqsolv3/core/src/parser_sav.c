#include "knowledge.h"

Expr parse(char *str);

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
	char *p = *str;
	int rtn=0;
	if(isalpha(*p)){
		p++;
		while(isalnum(*p)){p++;}
		rtn=1;
	}
	if(rtn){*str = p;}
	return rtn;
}

typedef enum ttag{
tt_symbol=256,tt_string,tt_integer,tt_real,tt_power,tt_map,tt_mapall,tt_apply,
tt_factorial,tt_factorial2,tt_times,tt_divide,tt_equal,tt_unequal,tt_greater,tt_less,
tt_greaterequal,tt_lessequal,tt_not,tt_and,tt_or,tt_rule,tt_ruledelayed,tt_replaceall,
tt_replacerepeated,tt_addto,tt_subtractfrom,tt_timesby,tt_divideby,tt_prefix,tt_infix,tt_postfix,tt_set,tt_setdelayed,
tt_decrement,tt_increment,tt_ll,tt_return,tt_span,tt_compoundexpression,tt_derivative,tt_pattern,tt_help
}TTag;


typedef struct token{
	int tag;
	Expr expr;
	char *p;
}Token;

/*tokenize input string. In most cases, Expr object is built in this phase*/
/*In some cases (like below), Expr object is built in pasing phase. (in function buildexpr()) */
/*ex1 : - a -> Minus[a] . a - b -> Subtract[a,b]*/
/*ex2 : ++ a -> PreIncrement[a]. a ++ Increment[a]*/

void gettoken(Token *token){
	char *p = token->p;
	char sav;
	int escape=0;
	
	token->expr = NULL;
	
	while(isblank(*p)){p++;}
	token->tag = *p;
	token->p = p;
	if(*p==0){return;}
	if(issymbol(&p)){
		sav = *p; *p='\0';
		token->expr = Expr_create(token->p);
		*p = sav;
	}else if((sav=isnum(&p))){
		switch(sav){
		  case 1:
			//puts("int");
			sav = *p; *p='\0';
			token->expr = Expr_createInteger(token->p);
			*p = sav;
			break;
		  case 2:
			//puts("real");
			sav = *p; *p='\0';
			token->expr = Expr_createReal(token->p);
			*p = sav;
			break;
		  case 3:
			//puts("expint");
			sav = *p; *p='\0';
			token->expr = Expr_createExpInteger(token->p);
			*p = sav;
			break;
		}
	}/*else if(*p=='\n'){
		token->tag = tt_return;
		token->expr = NULL;
	}*/else{
		switch(*p){
		  case '"':
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
				}else if(*p=='\\'){escape=1;continue;
				}else if(*p=='"'){
					StringList_collect('\0');
					token->tag = tt_string;
					token->expr = Expr_createString(StringList_release());
					p++;
					break;
				}else{
					StringList_collect(*p);
				}
			}
			if(token->expr==NULL){perror("error : unclosed double quotes\n"); deallocate(StringList_release());}
			break;
		  case '=':
			p++;
			if(*p=='='){
				token->tag = tt_equal;
				token->expr = Expr_create("Equal");
				p++;
			}else{
				token->tag = tt_set;
				token->expr = Expr_create("Set");
			}
			break;
		  case '<':
			p++;
			if(*p=='='){
				token->tag = tt_lessequal;
				token->expr = Expr_create("LessEqual");
				p++;
			}else{
				token->tag = tt_less;
				token->expr = Expr_create("Less");
			}
			break;
		  case '>':
			p++;
			if(*p=='='){
				token->tag = tt_lessequal;
				token->expr = Expr_create("GreaterEqual");
				p++;
			}else{
				token->tag = tt_less;
				token->expr = Expr_create("Greater");
			}
			break;
		  case '/':
			p++;
			switch(*p){
			  case '@':
				token->tag = tt_map;
				token->expr = Expr_create("Map");
				p++;
				break;
			  case '=':
				token->tag = tt_divideby;
				token->expr = Expr_create("DivideBy");
				p++;
				break;
			  case '/':
				p++;
				if(*p=='@'){
					token->tag = tt_mapall;
					token->expr = Expr_create("MapAll");
					p++;
				}else{
					token->tag = tt_postfix;
					token->expr = Expr_create("PostFix");
				}
				break;
			  case '.':
				p++;
				if(*p=='.'){
					token->tag = tt_replacerepeated;
					token->expr = Expr_create("ReplaceRepeated");
					p++;
				}else{
					token->tag = tt_replaceall;
					token->expr = Expr_create("ReplaceAll");
				}
				break;
			  default:
				token->tag = tt_divide;
				token->expr = Expr_create("Divide");
				break;
			}
			break;
		  case '|':
			token->tag = *p++;
			if(*p=='|'){
				token->tag = tt_or;
				token->expr = Expr_create("Or");
				p++;
			}
			break;
		  case '&':
			token->tag = *p++;
			if(*p=='&'){
				token->tag = tt_and;
				token->expr = Expr_create("And");
				p++;
			}
			break;
		  case '^':
			token->tag = tt_power;
			token->expr = Expr_create("Power");
			p++;
			break;
		  case ':':
			token->tag = *p++;
			switch(*p){
			  case '=':
				token->tag = tt_setdelayed;
				token->expr = Expr_create("SetDelayed");
				p++;
				break;
			  case '>':
				token->tag = tt_ruledelayed;
				token->expr = Expr_create("RuleDelayed");
				p++;
				break;
			  default:
				token->tag = tt_pattern;
				token->expr = Expr_create("Pattern");
				break;
			}
			break;
		  case '[':
			token->tag = *p++;
			if(*p=='['){
				token->tag = tt_ll; 
				p++;
			}
			break;
		  case '-':
			token->tag = *p++;
			switch(*p){
			  case '-':
				token->tag = tt_decrement;
				p++;
				break;
			  case '=':
				token->tag = tt_subtractfrom;
				token->expr = Expr_create("SubtractFrom");
				p++;
				break;
			  case '>':
				token->tag = tt_rule;
				token->expr = Expr_create("Rule");
				p++;
			  default:
				break;
			}
			break;
		  case '+':
			token->tag = *p++;
			switch(*p){
			  case '+':
				token->tag = tt_increment;
				p++;
				break;
			  case '=':
				token->tag = tt_addto;
				token->expr = Expr_create("AddTo");
				p++;
				break;
			  default:
				break;
			}
			break;
		  case '*':
			p++;
			if(*p=='='){
				token->tag = tt_timesby;
				token->expr = Expr_create("TimesBy");
				p++;
			}else{
				token->tag = tt_times;
				token->expr = Expr_create("Times");
			}
			break;
		  case '!':
			token->tag = *p++;
			switch(*p){
			  case '!':
				token->tag = tt_factorial2;
				token->expr = Expr_create("Factorial2");
				p++;
				break;
			  case '=':
				token->tag = tt_unequal;
				token->expr = Expr_create("UnEqual");
				p++;
			  default:
				break;
			}
			break;
		  case '@':
			p++;
			if(*p=='@'){
				token->tag = tt_apply;
				token->expr = Expr_create("Apply");
				p++;
			}else{
				token->tag = tt_prefix;
				token->expr = Expr_create("Prefix");
			}
			break;
		  case ';':
			p++;
			if(*p==';'){
				token->tag = tt_span;
				token->expr = Expr_create("Span");
				p++;
			}else{
				token->tag = tt_compoundexpression;
				token->expr = Expr_create("CompoundExpression");
			}
			break;
		  case '\'':
			p++; escape=1; /*recycle*/
			while(*p=='\''){escape++; p++;}
			token->tag = tt_derivative;
			token->expr = Expr_unary("Derivative",Expr_createInteger32(escape));
			break;
		  case '~':
			token->tag = tt_infix;
			token->expr = Expr_create("Infix");
			p++;
			break;
		  case '?':
			token->tag = tt_help;
			token->expr = Expr_create("Help");
			p++;
			break;
		  default:
			token->tag = *p++;
			break;
		}
	}
	token->p = p;
}

/*
int getatom(Token *token, Expr *expr){
	switch(token->tag){
	  case tt_integer:
	  case tt_real:
	  case tt_symbol:
	  case tt_string:
		expr = token->expr;
		gettoken(token);
		return 1;
	  default:
		expr = NULL;
		return 0;
	}
}

int getterm(Token *token, Expr *expr){
	Expr e1,e2;
	if(getfactor(token,expr)){
		e1 = *expr;
		gettoken(token);
		if(token->tag == tt_times){
			e2 = token->expr;
			if(getterm(token,expr)){
				Expr_appendChild(e2,e1);
				*expr = Expr_appendChild(e2,*expr);
			}
		}
	}
}

int getfactor(Token *token, Expr *expr){
	
}

int getexpr(Token *token, Expr *expr){
	Expr e=token->expr;
	if(getatom(token,&e)){
		if(getexpr(token,expr)){
			*expr = Expr_binary("Times",e,*expr);
		}
		goto accept;
	}else{
		switch(token->tag){
		  case tt_decrement:
			if(getexpr(token,expr)){
				*expr = Expr_unary("PreDecurement",*expr);
				goto accept;
			}
			goto error;
		  case tt_increment:
			if(getexpr(token,expr)){
				*expr = Expr_unary("PreIncrement",*expr);
				goto accept;
			}
			goto error;
		case '(':
			if(getexpr(token,expr)){
				if(token->tag==')'){
					goto accept;
				}
			}
			perror("unclosed parenthesis\n");
			goto error_delete;
		}
	}
	if(getexprend(token,*expr)){
		*expr = Expr_prependChild(*expr,e);
	}
	
  accept:
	gettoken(token);
	return 1;
  error_delete:
	Expr_deleteRoot(token->expr);
  error:
	*expr = NULL;
	return 0;
}

int getexprend(Token *token, Expr *expr){
	Expr e=token->expr;
	switch(token->tag){
	  case tt_decrement:
		*expr = Expr_create(tag_symbol,"Decrement");
		goto accept;
	  case tt_increment:
		*expr = Expr_create(tag_symbol,"Increment");
		goto accept;
	  case tt_power:
		goto mid;
	  case '+':
		if(getexpr(token,expr)){
			Expr_unary("Plus",*expr);
			goto accept;
		}
		goto error_delete;
	  case '-':
		if(getexpr(token,expr)){
			Expr_unary("Subtract",*expr);
			goto accept;
		}
		goto error_delete;
	  case tt_map:
		goto mid;
	  default:
		break;
	}
	if(getexprend(token,expr)){
		*expr = Expr_prependChild(*expr,e);
	}
  mid:
	if(getexpr(token,expr)){
		Expr_appendChild(e,*expr);
		goto accept;
	}
	goto error_delete;
  accept:
	gettoken(token);
	return 1;
  error_delete:
	Expr_deleteRoot(token->expr);
  error:
	*expr = NULL;
	return 0;
}
*/
/*
		  case tt_not:
			if(getexpr(token,expr)){
				*expr = Expr_appendChild(e,*expr);
				goto accept;
			}
			goto error_delete;
		  case '+':
			if(getexpr(token,expr)){
				goto accept;
			}
			goto error_delete;
		  case '-':
			if(getexpr(token,expr)){
				*expr = Evaluate(Expr_unary("Minus",*expr));
				goto accept;
			}
			goto error_delete;
		  case '(':
			if(getexpr(token,expr)){
				if(token->tag==')'){
					goto accept;
				}
			}
			perror("unclosed parenthesis\n");
			goto error_delete;
		  case '{':
			if(getexpr(str,tag,expr)){
				if(*tag=='}'){
					*expr = Expr_unary("List",*expr);
					goto accept;
				}
			}
			perror("unclosed curly parenthesis\n");
			goto error_delete;
		  default:
			break;
		}
		*/

void climbPrecedence(Expr *root, Expr *last, Expr expr){
	int p1,p2;
	Expr e;
	
	p1 = expr ? expr->symbol->precedence : 0;
	/*climbing phase*/
	for(e=*last;;e=e->parent){
		p2 = e ? e->symbol->precedence : 0;
		if(p1>=p2){
			if(p2==1000){
				/*a b -> a*b */
				climbPrecedence(root,last,Expr_create("Times"));
				climbPrecedence(root,last,expr);
				return;
			}
			break;
		}
		*last = e;
	}
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
			  case un_pre:
				/*make sure unary operator won't bond more than 2 leafs*/
				/*"a++b" should not be converted to Increment[a,b]*/
				/*unary operator got a child when child is inserted into tree (unary operator was in tree without a child)*/ 
				if(e->child){
					/*ep->Node->e->ec*/
					/*        ->expr*/
					*last = Expr_createInternal("Node");
					if(!e->parent){*root = Expr_appendChild(*last,e);}
					else{Expr_insert(e->parent,e,*last);}
					climbPrecedence(root,last,expr);
					return;
				}
				return;
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

Expr buildexpr(char **str, int mode){
	Token token = {0,NULL,*str};
	Expr root=NULL, last=NULL,e,e2;
	int lastasso=0,p1,p2,timesflag=0;
	//printf("%s\n",*str);
	while(*(token.p)){
		gettoken(&token);
		//if(token.expr){printf("expr\n");OutputForm_mod(token.expr);}
		//else{printf("no expr\n");putchar(token.tag);putchar('\n');}
		
		
		/*tokenizing phase 2*/
		switch(token.tag){
		  case '+':
			//if(!root || lastasso == bi_left || lastasso == bi_right){continue;}
			//else{token.expr = Expr_create("Plus");}
			token.expr = Expr_create("Plus");
			break;
		  case '-':
			if(!root || lastasso == bi_left || lastasso == bi_right){token.expr = Expr_create("Minus");}
			else{token.expr = Expr_create("Subtract");}
			break;
			/*
			token.expr=Expr_unary("Times",Expr_createInteger32(-1));
			if(!root || lastasso == bi_left || lastasso == bi_right){
				break;
			}else{
				climbPrecedence(&root,&last,Expr_create("Plus"));
				break;
			}
			break;
			*/
		  case tt_increment:
			if(!root || lastasso == bi_left || lastasso == bi_right){token.expr = Expr_create("PreIncrement");}
			else{token.expr = Expr_create("Increment");}
			break;
		  case tt_decrement:
			if(!root || lastasso == bi_left || lastasso == bi_right){token.expr = Expr_create("PreDecrement");}
			else{token.expr = Expr_create("Decrement");}
			break;
		  case '!':
			if(!root || lastasso == bi_left || lastasso == bi_right){token.expr = Expr_create("Not");}
			else{token.expr = Expr_create("Factorial");}
			break;
		  case '[':
			*str = token.p;
			if(!last){perror("this expression have no head. more input is needed\n"); return root;}
			else{
				/*prevent additional node won't climb up higher than last node.*/
				Expr_insert(e2=last->parent,last,e=Expr_createInternal("Node"));
			}
			for(;**str;){
				Expr_appendChild(last,buildexpr(str,2));
				if(*(*str-1)==']'){break;}
			}
			if(!e2){root = last = e;}
			else{last = e;}
			token.p = *str;
			goto loopend;
		  case '{':
			*str = token.p;
			Expr_appendChild(last,e=Expr_create("List"));
			if(!last){root = last = e;}
			last = e;
			Expr_insert(e2=last->parent,last,e=Expr_createInternal("Node"));
			for(;**str;){
				Expr_appendChild(last,buildexpr(str,3));
				if(*(*str-1)=='}'){break;}
			}
			if(!e2){root = last = e;}
			else{last = e;}
			token.p = *str;
			goto loopend;
		  case ',':
			if(mode==2 || mode==3){goto end;}
		  case ']':
			if(mode==2){goto end;}
		  case '}':
			if(mode==3){goto end;}
			break;
		  case '(':
			*str = token.p;
			token.expr=buildexpr(str,1);
			token.p = *str;
			break;
		  case ')':
			if(mode!=1){
				perror("unclosed parenthesis\n");
			}
			goto end;
		  case '\0':
			if(!token.expr){goto end;}
			break;
		  default:
			if(!token.expr){goto loopend;}
			break;
		}
		/*Precedence climbing*/
		climbPrecedence(&root,&last,token.expr);
	  loopend:
		lastasso=last->symbol->associativity;
	}
  end:
	if(mode==1){root = Expr_appendChild(Expr_createInternal("Node"),root);}
	*str = token.p;
	return root;
}

Expr PreEvaluate(Expr expr){
	Expr e=NULL;
	if(!expr){return NULL;}
	for(e=expr->child;e;e=e->next){
		PreEvaluate(e);
	}
	switch(expr->symbol->id){
	  case id_Minus:
		return Minus(expr);
	  case id_Subtract:
		return Subtract(expr);
	  case id_Divide:
		return Divide(expr);
	  case id_Node:
		return Node(expr);
	  default:
		break;
	}
	return expr;
}

Expr parse(char *str){	
	Expr expr = buildexpr(&str,0);
	TreeForm_mod(expr);
	expr = PreEvaluate(expr);
	TreeForm_mod(expr);
	return expr;
}

/*
times_re:
	p1 = Expr_getPrecedence(token.expr);
	for(e=last;;e=e->parent){
		p2 = Expr_getPrecedence(e);
		if(p1 >= p2){
			if(p2==1000 && !timesflag){
				timesflag=1;
				e2 = token.expr;
				token.expr = Expr_create(tag_symbol,"Times");
				goto times_re;
			}
			break;
		}
		last = e;
	}
	if(!e){root = last = Expr_appendChild(token.expr,last);}
	else{
		if(e->tag==tag_symbol){
			if(p1 == p2){
				switch(Expr_getAssociativity(e)){
				  case bi_left:
					if(!e->parent){root = last = Expr_appendChild(token.expr,e);}
					else{Expr_insert(e->parent,e,token.expr); last = token.expr;}
					goto loop;
				  case bi_none:
					if(e->data.symbol==token.expr->data.symbol){
						last = e; Expr_delete(token.expr);
						goto loop;
					}
					break;
				  default:
					break;
				}
			}
		}
		Expr_insert(e,last,token.expr); last = token.expr;
}
*/
		
