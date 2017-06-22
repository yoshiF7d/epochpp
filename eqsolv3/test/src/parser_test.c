#include "Expr.h"
#include "global.h"

Expr parse(char *str);
Expr Evaluate(Expr expr);

int main(int argc, char *argv[]){
	char *str = NULL;
	if(argc>=2){
		str = String_stripdq(argv[1]);
	}
	mp_set_memory_functions(allocate2,reallocate2,deallocate2);
#ifdef LEAKDETECT
	LeakDetector_set(stdout);
#endif
	SymbolTable_init();
	
	printf("input : %s\n",str);
	parse(str);
	
	SymbolTable_finish();
#ifdef LEAKDETECT
	LeakDetector_finish();
#endif
	return 1;
}

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

int isfloat(char **str){
	char *p=*str;
	int rtn=0;
	
	if(isint(&p)){
		if(*p=='.'){p++;}
	}else if(*p=='.'){
		if(!isdigit(*++p)){
			rtn=0; goto end;
		}
	}
	while(isdigit(*p)){p++;}
	rtn=1;
  end:
	if(rtn){
		*str = p;
		//printf("float\n"); putchar(*p); putchar('\n');
	}
	return rtn;
}

int isexp(char **str){
	char *p=*str;
	int rtn=0;
	
	if(*p=='e' || *p=='E'){
		p++;
		if(*p=='-' || *p=='+'){
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
	rtn=1;
	goto end;
  end:
	if(rtn){
		*str = p;
		//printf("exp\n"); putchar(*p); putchar('\n');
	}
	return rtn;
}

int isreal(char **str){
	if(isfloat(str)){
		isexp(str);
		return 1;
	}else{
		return 0;
	}
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
tt_decrement,tt_increment,tt_ll,tt_return,tt_span,tt_compoundexpression,tt_derivative
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
	char *p = token->p, *p2 = token->p;
	char sav;
	int escape=0;
	token->tag = *p;
	token->expr = NULL;
	
	if(*p==0){return;}
	while(isblank(*p)){p++;}
	if(issymbol(&p)){
		sav = *p; *p='\0';
		token->expr = Expr_create(tag_symbol,token->p);
		*p = sav;
	}else if(isint(&p)){
		isreal(&p2);
		if(p2==p){
			sav = *p; *p='\0';
			token->expr = Expr_create(tag_integer,token->p);
			*p = sav;
		}else{
			p=p2;
			sav = *p; *p='\0';
			token->expr = Expr_create(tag_real,token->p);
			*p = sav;
		}
	}else if(*p=='\n'){
		token->tag = tt_return;
		token->expr = NULL;
	}else{
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
					token->expr = Expr_create(tag_string,StringList_release());
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
				token->expr = Expr_create(tag_symbol,"Equal");
				p++;
			}else{
				token->tag = tt_set;
				token->expr = Expr_create(tag_symbol,"Set");
			}
			break;
		  case '<':
			p++;
			if(*p=='='){
				token->tag = tt_lessequal;
				token->expr = Expr_create(tag_symbol,"LessEqual");
				p++;
			}else{
				token->tag = tt_less;
				token->expr = Expr_create(tag_symbol,"Less");
			}
			break;
		  case '>':
			p++;
			if(*p=='='){
				token->tag = tt_lessequal;
				token->expr = Expr_create(tag_symbol,"GreaterEqual");
				p++;
			}else{
				token->tag = tt_less;
				token->expr = Expr_create(tag_symbol,"Greater");
			}
			break;
		  case '/':
			p++;
			switch(*p){
			  case '@':
				token->tag = tt_map;
				token->expr = Expr_create(tag_symbol,"Map");
				p++;
				break;
			  case '=':
				token->tag = tt_divideby;
				token->expr = Expr_create(tag_symbol,"DivideBy");
				p++;
				break;
			  case '/':
				p++;
				if(*p=='@'){
					token->tag = tt_mapall;
					token->expr = Expr_create(tag_symbol,"MapAll");
					p++;
				}else{
					token->tag = tt_postfix;
					token->expr = Expr_create(tag_symbol,"PostFix");
				}
				break;
			  case '.':
				p++;
				if(*p=='.'){
					token->tag = tt_replacerepeated;
					token->expr = Expr_create(tag_symbol,"ReplaceRepeated");
					p++;
				}else{
					token->tag = tt_replaceall;
					token->expr = Expr_create(tag_symbol,"ReplaceAll");
				}
				break;
			  default:
				token->tag = tt_divide;
				token->expr = Expr_create(tag_symbol,"Divide");
				break;
			}
			break;
		  case '|':
			token->tag = *p++;
			if(*p=='|'){
				token->tag = tt_or;
				token->expr = Expr_create(tag_symbol,"Or");
				p++;
			}
			break;
		  case '&':
			token->tag = *p++;
			if(*p=='&'){
				token->tag = tt_and;
				token->expr = Expr_create(tag_symbol,"And");
				p++;
			}
			break;
		  case '^':
			token->tag = tt_power;
			token->expr = Expr_create(tag_symbol,"Power");
			p++;
			break;
		  case ':':
			token->tag = *p++;
			switch(*p){
			  case '=':
				token->tag = tt_setdelayed;
				token->expr = Expr_create(tag_symbol,"SetDelayed");
				p++;
				break;
			  case '>':
				token->tag = tt_ruledelayed;
				token->expr = Expr_create(tag_symbol,"RuleDelayed");
				p++;
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
				token->expr = Expr_create(tag_symbol,"SubtractFrom");
				p++;
				break;
			  case '>':
				token->tag = tt_rule;
				token->expr = Expr_create(tag_symbol,"Rule");
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
				token->expr = Expr_create(tag_symbol,"AddTo");
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
				token->expr = Expr_create(tag_symbol,"TimesBy");
				p++;
			}else{
				token->tag = tt_times;
				token->expr = Expr_create(tag_symbol,"Times");
			}
			break;
		  case '!':
			token->tag = *p++;
			switch(*p){
			  case '!':
				token->tag = tt_factorial2;
				token->expr = Expr_create(tag_symbol,"Factorial2");
				p++;
				break;
			  case '=':
				token->tag = tt_unequal;
				token->expr = Expr_create(tag_symbol,"UnEqual");
				p++;
			  default:
				break;
			}
			break;
		  case '@':
			p++;
			if(*p=='@'){
				token->tag = tt_apply;
				token->expr = Expr_create(tag_symbol,"Apply");
				p++;
			}else{
				token->tag = tt_prefix;
				token->expr = Expr_create(tag_symbol,"Prefix");
			}
			break;
		  case ';':
			p++;
			if(*p==';'){
				token->tag = tt_span;
				token->expr = Expr_create(tag_symbol,"Span");
				p++;
			}else{
				token->tag = tt_compoundexpression;
				token->expr = Expr_create(tag_symbol,"CompoundExpression");
			}
			break;
		  case '\'':
			p++; escape=1; /*recycle*/
			while(*p=='\''){escape++; p++;}
			token->tag = tt_derivative;
			token->expr = Expr_unary("Derivative",Expr_createInteger(escape));
			break;
		  case '~':
			token->tag = tt_infix;
			token->expr = Expr_create(tag_symbol,"Infix");
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

Expr buildexpr(char **str, int mode){
	Token token = {0,NULL,*str};
	Expr root=NULL, last=NULL,e,e2;
	int lastasso=0,p1,p2;
	//printf("%s\n",*str);
	while(*(token.p)){
		gettoken(&token);
		switch(token.tag){
		  case '+':
			if(!root || lastasso == bi_left || lastasso == bi_right){continue;}
			else{token.expr = Expr_create(tag_symbol,"Plus");}
			break;
		  case '-':
			if(!root || lastasso == bi_left || lastasso == bi_right){token.expr = Expr_create(tag_symbol,"Minus");}
			else{token.expr = Expr_create(tag_symbol,"Subtract");}
			break;
		  case tt_increment:
			if(!root || lastasso == bi_left || lastasso == bi_right){token.expr = Expr_create(tag_symbol,"PreIncrement");}
			else{token.expr = Expr_create(tag_symbol,"Increment");}
			break;
		  case tt_decrement:
			if(!root || lastasso == bi_left || lastasso == bi_right){token.expr = Expr_create(tag_symbol,"PreDecrement");}
			else{token.expr = Expr_create(tag_symbol,"Decrement");}
			break;
		  case '!':
			if(!root || lastasso == bi_left || lastasso == bi_right){token.expr = Expr_create(tag_symbol,"Not");}
			else{token.expr = Expr_create(tag_symbol,"Factorial");}
			break;
		  case '[':
			*str = token.p;
			if(!last){perror("this expression have no head. more input is needed\n"); return root;}
			else{
				Expr_insert(e2=last->parent,last,e=Expr_create(tag_symbol,"Sequence"));
			}
			for(;**str;){
				Expr_appendChild(last,buildexpr(str,2));
				if(*(*str-1)==']'){break;}
			}
			if(!e2){root = last = e;}
			else{last = e;}
			token.p = *str;
			continue;
		  case '{':
			*str = token.p;
			Expr_appendChild(last,e=Expr_create(tag_symbol,"List"));
			if(!last){root = last = e;}
			last = e;
			Expr_insert(e2=last->parent,last,e=Expr_create(tag_symbol,"Sequence"));
			for(;**str;){
				Expr_appendChild(last,buildexpr(str,3));
				if(*(*str-1)=='}'){break;}
			}
			if(!e2){root = last = e;}
			else{last = e;}
			token.p = *str;
			continue;
		  case ',':
			if(mode==2 || mode==3){goto end;}
		  case ']':
			if(mode==2){goto end;}
		  case '}':
			if(mode==3){goto end;}
			break;
		  case '(':
			*str = token.p;
			Expr_appendChild(last,e=buildexpr(str,1));
			last = e;
			token.p = *str;
			continue;
		  case ')':
			if(mode!=1){
				perror("unclosed parenthesis\n");
			}
			goto end;
		  default:
			break;
		}
		/*Precedence climbing*/
		p1 = Expr_getPrecedence(token.expr);
		for(e=last;;e=e->parent){
			p2 = Expr_getPrecedence(e);
			if(p1 >= p2){
				if(p2==1000){
					if(root->tag==tag_symbol){
						if(root->data.symbol->id==id_Times){
							root = Expr_appendChild(root,token.expr);
							goto loop;
						}
					}
					root = Expr_binary("Times",root,token.expr);
					goto loop;
				}
				break;
			}
			last = e;
		}
		if(!e){root = last = Expr_appendChild(token.expr,last);}
		else{
			if(e->tag==tag_symbol){
				if(e->data.symbol == token.expr->data.symbol){
					switch(Expr_getAssociativity(token.expr)){
					  case bi_left:
						if(!e->parent){root = last = Expr_appendChild(token.expr,e);}
						else{Expr_insert(e->parent,e,token.expr); last = token.expr;}
						goto loop;
					  case bi_none:
						last = e; Expr_delete(token.expr);
						goto loop;
					  default:
						break;
					}
				}
			}
			Expr_insert(e,last,token.expr); last = token.expr;
		}
	  loop:
		lastasso=Expr_getAssociativity(last);
	}
  end:
	if(mode==1){
		root = Expr_appendChild(Expr_create(tag_symbol,"Sequence"),root);
	}
	*str = token.p;
	return root;
}

Expr parse(char *str){	
	Expr expr;
	return buildexpr(&str,0);
}

