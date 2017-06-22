#include "knowledge.h"
Expr Blank_pre(Expr expr){
	if(!(expr->flag & ef_func) && expr->child){
		expr->data.symbol = expr->child->symbol;
		Expr_delete(expr->child);
		expr->child=NULL;
	}else{
		expr->data.symbol = NULL;
	}
	return expr;
}
char *Blank_toString(Expr expr){
	char *buf=NULL;
	int needed;
	if(expr->data.symbol){
		needed = snprintf(buf,0,"_%s",expr->data.symbol->name);
		buf = allocate(needed+1);
		snprintf(buf,needed+1,"_%s",expr->data.symbol->name);
	}else{
		return String_copy("_");
	}
	return buf;
}
void Blank_data_init(union un_data *data){data->symbol = NULL;}
void Blank_data_copy(union un_data *datas, union un_data *datad){datad->symbol = datas->symbol;}
