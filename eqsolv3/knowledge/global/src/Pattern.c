#include "knowledge.h"
void Pattern_data_init(union un_data *data){
	data->pattern.symbol = NULL;
	data->pattern.object = NULL;
	data->pattern.capture = NULL;
}
void Pattern_data_finish(union un_data *data){
	Expr_deleteRoot(data->pattern.object);
	Expr_deleteRoot(data->pattern.capture);
}
void Pattern_data_copy(union un_data *datas, union un_data *datad){
	datad->pattern.symbol = datas->pattern.symbol;
	datad->pattern.object = Expr_copyRoot(datas->pattern.object);
	datad->pattern.capture = Expr_copyRoot(datas->pattern.capture);
}
char *Pattern_toString(Expr expr){
	char *buf=NULL,*buf2=NULL;
	int needed;
	if(expr->data.pattern.symbol && expr->data.pattern.object){
		buf2=Expr_toString(expr->data.pattern.object);
		needed = snprintf(buf,0,"%s : %s", expr->data.pattern.symbol->name,buf2);
		buf = allocate(needed+1);
		snprintf(buf,needed+1,"%s : %s", expr->data.pattern.symbol->name,buf2);
		deallocate(buf2);
	}else if(expr->data.pattern.symbol){
		needed = snprintf(buf,0,"%s : ",expr->data.pattern.symbol->name);
		buf = allocate(needed+1);
		snprintf(buf,needed+1,"%s : ",expr->data.pattern.symbol->name);
	}else{
		return String_copy("Pattern");
	}
	return buf;
}
