#include "knowledge.h"
Expr Pointer_create(Expr e){
	Expr rtn;
	rtn = Expr_create(id_Pointer);
	rtn->data.expr = e;
	return rtn;
}
void Pointer_data_init(union un_data *data){data->expr = NULL;}
void Pointer_data_finish(union un_data *data){data->expr = NULL;}
void Pointer_data_copy(union un_data *datas, union un_data *datad){datad->expr = datas->expr;}
