#include "knowledge.h"
char *String_toString(Expr expr){
	return String_copy(expr->data.string);
}
Expr String_create(char *string){
	Expr expr = Expr_create(id_String);
	expr->data.string = string;
	expr->next = NULL;
	expr->previous = NULL;	
	expr->child = NULL;
	expr->parent = NULL;
	return expr;
}
void String_data_init(union un_data *data){
	data->string = NULL;
}
void String_data_finish(union un_data *data){
	deallocate(data->string);
	data->string = NULL;
}
void String_data_copy(union un_data *datas, union un_data *datad){
	datad->string = String_copy(datas->string);
}
int String_data_equals(union un_data *d1, union un_data *d2){
	if(!strcmp(d1->string,d2->string)){return 1;}
	else{return 0;}
}

