#include "knowledge.h"
char *Blank3_toString(Expr expr){
	char *buf=NULL;
	int needed;
	if(expr->data.symbol){
		needed = snprintf(buf,0,"___%s",expr->data.symbol->name);
		buf = allocate(needed+1);
		snprintf(buf,needed+1,"___%s",expr->data.symbol->name);
	}else{
		return String_copy("___");
	}
	return buf;
}
void Blank3_data_init(union un_data *data){data->symbol = NULL;}
void Blank3_data_copy(union un_data *datas, union un_data *datad){datad->symbol = datas->symbol;}
