#include "knowledge.h"
Form Form_makeMatrix(Expr expr, int d){
	Expr e;
	Form f=NULL;
	int len;
	if(!expr){printf("No Form\n"); return NULL;}
	if(expr->symbol->id == id_List){
		if(expr->child){
			/*0 when expr->child->child is null (expr is a vector)*/
			len = Expr_getLength(expr->child->child);
			for(e=expr->child->next;e;e=e->next){
				if(len!=Expr_getLength(e->child)){
					len=0; break;
				}
			}
			switch(d){
				/*len == 0 when list is not array or vector*/
			  case FormV:
				if(len==0){
					for(e=expr->child;e;e=e->next){
						f = Form_appendVerticalCenter(f,Form_make(e,0));
					}
				}else{
					for(e=expr->child;e;e=e->next){
						f = Form_appendVerticalCenter(f,Form_create(String_char(' ')));
						f = Form_appendVerticalCenter(f,Form_makeMatrix(e,FormH));
					}
				}
				break;
			  case FormH:
				if(len==0){
					for(e=expr->child;e;e=e->next){
						f = Form_appendHorizontalCenter(f,Form_make(e,0));
					}
				}else{
					for(e=expr->child;e;e=e->next){
						f = Form_appendHorizontalCenter(f,Form_create(String_char(' ')));
						f = Form_appendHorizontalCenter(f,Form_makeMatrix(e,FormV));
					}
				}
				break;
			  default:
				fprintf(stderr,"Form_makeMatrix : error\n");
				/*never happen*/
				break;
			}
			return f;
		}
	}
  end:
	return Form_make(expr,0);
}

Expr MatrixForm_mod(Expr expr){
	Form f;
	if(!expr){printf("Null\n"); return NULL;}
	f = Form_makeMatrix(expr,FormV);
	Form_print(f);
	putchar('\n');
	Form_delete(f);
	return expr;
}

Expr MatrixForm(Expr expr){
	if(!expr->child){return expr;}
	MatrixForm_mod(expr->child);
	output_flag = 0;
	return expr;
}
