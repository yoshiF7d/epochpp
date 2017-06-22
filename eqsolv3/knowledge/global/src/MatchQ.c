#include "knowledge.h"
enum en_mqflag{
	mqf_start=1,
	mqf_orderless=2,
};
/*
meaning of return value
0 : no match
1 : match
2 : all nodes have matched but not enough nodes.
*/

Expr MatchQ(Expr expr){
	Expr e;
	if(!expr->child){return expr;}
	if(!expr->child->next){
		fprintf(stderr,"MatchQ::argr: MatchQ called with 1 argument; 2 arguments are expected.\n");
		return expr;
	}
	if(mark(expr->child,expr->child->next,NULL,0)==mqr_match){
		expr->symbol = SymbolTable_get(id_True);
	}else{
		expr->symbol = SymbolTable_get(id_False);
	}
	showsub(expr);
	unmark(expr);
	TreeForm_mod(expr);
	for(e=expr->child;e;e=e->next){Expr_deleteRoot(e);}
	expr->child=NULL;
	return expr;
}
void showsub(Expr expr){
	Expr e;
	if(!expr){return;}
	if(expr->symbol->id == id_Pattern){
		if(expr->data.pattern.symbol){printf("Symbol : %s\n",expr->data.pattern.symbol->name);}
		printf("Capture : \n");
		for(e=expr->data.pattern.capture;e;e=e->next){
			if(e->symbol->id==id_Pointer){FullForm_mod(e->data.expr);}
			putchar('\n');
		}
	}
	for(e=expr->child;e;e=e->next){showsub(e);}
}

void unmark(Expr expr){
	Expr e;
	if(!expr){return;}
	expr->flag &= (~ ef_mark);
	for(e=expr->child;e;e=e->next){unmark(e);}
}

Expr unmarked(Expr expr){
	Expr e;
	for(e=expr;e;e=e->next){
		if(!(e->flag & ef_mark)){
			return e;
		}
	}
	return NULL;
}
int allmarked(Expr expr){
	Expr e;
	for(e=expr;e;e=e->next){
		if(!(e->flag & ef_mark)){return 0;}
	}
	return 1;
}
#define SYMBCHK(e) if(symb){if(symb != e->symbol){return mqr_unmatch;}}
#define SETCONT(e) if(cont){*cont=Expr_append(*cont,Pointer_create(e));}
#define MARKSERIES(e) \
SYMBCHK(e);\
for(;e;e=e->next){\
	SETCONT(e);\
	e->flag |= ef_mark;\
}\
debug2("mqr_match\n");\
return mqr_match

#define SKIP_IF_NEXT_IS_BLANK23 \
switch(pattern->next->symbol->id){\
  case id_Pattern:\
	switch(pattern->next->data.pattern.object->symbol->id){\
	  case id_Blank2:\
	  case id_Blank3:\
		e = e->next;\
	  default:\
		break;\
	}\
  case id_Blank2:\
  case id_Blank3:\
	e = e->next;\
  default:\
	break;\
}

//#define debug2(...) fprintf(stderr,__VA_ARGS__)
#define debug2(...) do{}while(0)

/*this code became complex because I have to deal with a case of orderless matching.*/
/*this code searches each leaves of expression tree one by one, and mark matched leaf by setting up ef_mark flag in expr->flag.*/
int mark(Expr expr, Expr pattern, Expr head, int callback){
	Expr e=expr,pat=pattern,*cont=NULL,repat=NULL;
	Symbol symb=NULL;
	int blank=0;
	if(expr){
		if(expr->flag & ef_mark){return mark(expr->next,pattern,head,callback);}
	}
	if(!pattern){
		if(!expr){return mqr_match;}
		else if(callback){return mqr_match;}
		else{return mqr_missing;}
	}
	
	if(pattern->symbol->id == id_Pattern){
		pat = pattern->data.pattern.object;
		cont = &(pattern->data.pattern.capture);
	}
	
	switch(pat->symbol->id){
	  case id_Blank:
	  case id_Blank2:
	  case id_Blank3:
		symb = pat->data.symbol;
	  default:
		break;
	}
	
	//debug2("expr : "); FullForm_mod(expr); putchar('\n');
	//debug2("pattern : "); FullForm_mod(pat); putchar('\n');
	
	switch(pat->symbol->id){
	  case id_Blank:
		if(!expr){
			debug2("mqr_unmatch\n");
			return mqr_unmatch;
		}
		blank=1;
		if(!head){
			SYMBCHK(expr);
			SETCONT(expr);
			break;
		}else{
			if(head->symbol->attributes & Orderless){
				if(!pattern->next){
					SYMBCHK(expr);
					if(allmarked(expr) || !expr->next || callback==1){
						SETCONT(expr);
						break;
					}
				}else{
					SKIP_IF_NEXT_IS_BLANK23
					if(mark(e,pattern->next,head,1)){
						if((e=unmarked(expr))){
							SYMBCHK(e);
							SETCONT(e);
							e->flag |= ef_mark;
							if(allmarked(expr)){break;}
							else{
								debug2("mqr_missing\n");
								return mqr_missing;
							}
						}
					}
				}
				debug2("mqr_unmatch\n");
				return mqr_unmatch;
			}else if(mark(expr->next,pattern->next,head,callback)==mqr_match){
				SYMBCHK(expr);
				SETCONT(expr);
				break;
			}
		}
		debug2("mqr_unmatch_\n");
		return mqr_unmatch;
	  case id_Blank2:
		blank=1;
		if(!expr){
			debug2("mqr_unmatch\n");
			return mqr_unmatch;
		}
		if(head){if(head->symbol->attributes & Orderless){goto blank_od;}}
		SETCONT(e);
		e->flag |= ef_mark;
	  bs_next:
		SYMBCHK(e);
		e = e->next;
	  case id_Blank3:
		blank=1;
		if(!e){	
			debug2("mqr_match\n");
			return mqr_match;
		}
		if(head){
			if(head->symbol->attributes & Orderless){
			  blank_od:
				if(!pattern->next){
					MARKSERIES(e);
				}else if(mark(expr,pattern->next,head,1)){
					if((e=unmarked(expr))){
						MARKSERIES(e);
					}
				}
				debug2("mqr_unmatch\n");
				return mqr_unmatch;
			}else{
				SETCONT(e);
				e->flag |= ef_mark;
			}
		}else{break;}
		if(mark(e,pattern->next,head,callback)==mqr_match){break;}
		else{goto bs_next;}
	  default:
		if(!expr){
			debug2("mqr_unmatch\n");
			return mqr_unmatch;
		}
		if(Expr_equals(expr,pattern)){
			if(head){
				if(mark(expr->next,pattern->next,head,callback)==mqr_match){break;}
			}else{break;}
		}else if(head){
			if(head->symbol->attributes & Orderless){
				for(e=expr->next;e;e=e->next){
					if(e->flag & ef_mark){continue;}
					if(e->symbol == pattern->symbol){
						e->flag |= ef_mark;
						if(mark(e->child,pattern->child,e,callback)==mqr_match){
							return mark(expr,pattern->next,head,callback);
						}else{
							break;
						}
					}
				}
			}
		}
		debug2("mqr_unmatch\n");
		return mqr_unmatch;
	}
	expr->flag |= ef_mark;
	if(blank && !pattern->child){
		debug2("mqr_match\n");
		return mqr_match;
	}
	return mark(expr->child,pattern->child,expr,callback);
}

int mark2(Expr expr, Expr pattern, Expr head, int callback){
	Expr e=expr,pat=pattern,*cont=NULL,repat=NULL;
	Symbol symb=NULL;
	int blank=0;
	if(expr){
		if(expr->flag & ef_mark){return mark2(expr->next,pattern,head,callback);}
	}
	if(!pattern){
		if(!expr){return mqr_match;}
		else if(callback){return mqr_match;}
		else{return mqr_missing;}
	}
	
	if(pattern->symbol->id == id_Pattern){
		pat = pattern->data.pattern.object;
		cont = &(pattern->data.pattern.capture);
	}
	
	switch(pat->symbol->id){
	  case id_Blank:
	  case id_Blank2:
	  case id_Blank3:
		symb = pat->data.symbol;
	  default:
		break;
	}
	
	//debug2("expr : "); FullForm_mod(expr); putchar('\n');
	//debug2("pattern : "); FullForm_mod(pat); putchar('\n');
	
	switch(pat->symbol->id){
	  case id_Blank:
		if(!expr){
			debug2("mqr_unmatch\n");
			return mqr_unmatch;
		}
		blank=1;
		if(!head){
			SYMBCHK(expr);
			SETCONT(expr);
			break;
		}else{
			if(head->symbol->attributes & Orderless){
				if(!pattern->next){
					SYMBCHK(expr);
					if(allmarked(expr) || !expr->next || callback==1){
						SETCONT(expr);
						break;
					}
				}else{
					SKIP_IF_NEXT_IS_BLANK23
					if(mark2(e,pattern->next,head,1)){
						if((e=unmarked(expr))){
							SYMBCHK(e);
							SETCONT(e);
							e->flag |= ef_mark;
							if(allmarked(expr)){break;}
							else{
								debug2("mqr_missing\n");
								return mqr_missing;
							}
						}
					}
				}
				debug2("mqr_unmatch\n");
				return mqr_unmatch;
			}else if(mark2(expr->next,pattern->next,head,callback)==mqr_match){
				SYMBCHK(expr);
				SETCONT(expr);
				break;
			}
		}
		debug2("mqr_unmatch_\n");
		return mqr_unmatch;
	  case id_Blank2:
		blank=1;
		if(!expr){
			debug2("mqr_unmatch\n");
			return mqr_unmatch;
		}
		if(head){if(head->symbol->attributes & Orderless){goto blank_od;}}
		SETCONT(e);
		e->flag |= ef_mark;
	  bs_next:
		SYMBCHK(e);
		e = e->next;
	  case id_Blank3:
		blank=1;
		if(!e){	
			debug2("mqr_match\n");
			return mqr_match;
		}
		if(head){
			if(head->symbol->attributes & Orderless){
			  blank_od:
				if(!pattern->next){
					MARKSERIES(e);
				}else if(mark2(expr,pattern->next,head,1)){
					if((e=unmarked(expr))){
						MARKSERIES(e);
					}
				}
				debug2("mqr_unmatch\n");
				return mqr_unmatch;
			}else{
				SETCONT(e);
				e->flag |= ef_mark;
			}
		}else{break;}
		if(mark2(e,pattern->next,head,callback)==mqr_match){break;}
		else{goto bs_next;}
	  default:
		if(!expr){
			debug2("mqr_unmatch\n");
			return mqr_unmatch;
		}
		if(Expr_equals(expr,pattern)){
			if(head){
				if(mark2(expr->next,pattern->next,head,callback)==mqr_match){break;}
			}else{break;}
		}else if(head){
			if(head->symbol->attributes & Orderless){
				for(e=expr->next;e;e=e->next){
					if(e->flag & ef_mark){continue;}
					if(e->symbol == pattern->symbol){
						e->flag |= ef_mark;
						if(mark(e->child,pattern->child,e,callback)==mqr_match){
							return mark(expr,pattern->next,head,callback);
						}else{
							break;
						}
					}
				}
			}
		}
		debug2("mqr_unmatch\n");
		return mqr_unmatch;
	}
	expr->flag |= ef_mark;
	if(blank && !pattern->child){
		debug2("mqr_match\n");
		return mqr_match;
	}
	return mark(expr->child,pattern->child,expr,callback);
}
/*
int mark2(Expr ehead, Expr phead){
	Expr e,p,*cont=NULL,repat=NULL;
	Symbol symb=NULL;
	
	if(pattern->symbol->id == id_Pattern){
		pat = pattern->data.pattern.object;
		cont = &(pattern->data.pattern.capture);
	}
	if(!pattern->child){
		if(!expr->child){return mqr_match;}
		else{return mqr_unmatch;}
	}
	
	for(p=pattern->child;p;p=p->next){
		switch(pat->symbol->id){
		  case id_Blank:
			for(e=expr->child;e;e=e->next){
				
			}
		  case id_Blank2:
			
		  case id_Blank3:
			
			symb = pat->data.symbol;
		  default:
			break;
		}
		switch(pat->symbol->id){
		  case id_Blank:
		  case id_Blank2:
		  case id_Blank3:
		}
	}
}
*/
