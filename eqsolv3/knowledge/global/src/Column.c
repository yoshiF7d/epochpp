#include "knowledge.h"
Expr Column(Expr expr){
	Expr child = expr->child;
	int alignment=0,spacing=0;
	if(!child){return expr;}
	if(child->symbol->id != id_List){return expr;}
	if(child->next){
		switch(child->next->symbol->id){
		  case id_Left:
			alignment = -1;
			break;
		  case id_Right:
			alignment = 1;
			break;
		  default:
			break;
		}
		if(child->next->next){
			if(child->next->next->symbol->id == id_Integer){
				spacing = Integer_toInt(child->next->next);
			}
		}
	}
	Column_mod(child,alignment,spacing);
	output_flag = 0;
	return expr;
}
Expr Column_mod(Expr expr,int alignment, int spacing){
	Expr e;
	Form f=NULL;
	Form (*appendVertical)(Form,Form);
	if(alignment < 0){
		appendVertical = Form_appendVertical;
	}else if(alignment > 0){
		appendVertical = Form_appendVerticalRight;
	}else{
		appendVertical = Form_appendVerticalCenter;
	}
	for(e=expr->child;e;e=e->next){
		f = appendVertical(f,Form_make(e,0));
		if(spacing){f=appendVertical(f,Form_create(repeatedString(' ',spacing)));}
	}
	Form_print(f);
	Form_delete(f);
	return expr;
}
