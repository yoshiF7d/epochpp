#include "knowledge.h"
Expr PointerHead_create(Expr e){
	Expr rtn;
	rtn = Expr_create(id_PointerHead);
	rtn->data.expr = e;
	return rtn;
}
char *PointerHead_toString(Expr expr){
	return OutputForm_mod_str(expr->data.expr);
}
void PointerHead_data_init(union un_data *data){data->expr = NULL;}
void PointerHead_data_finish(union un_data *data){Expr_deleteRoot(data->expr); data->expr = NULL;}
void PointerHead_data_copy(union un_data *datas, union un_data *datad){
	datad->expr = Expr_copyRoot(datas->expr);
}
