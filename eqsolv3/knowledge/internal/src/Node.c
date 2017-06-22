#include "knowledge.h"
Expr Node(Expr expr){
	Expr e;
	if(!expr->child){return expr;}
	if(expr->child->next){
		fprintf(stderr,"Node : Node shoudn't have more than 1 child\n");
		return expr;
	}
	Expr_replace(expr,e=expr->child);
	Expr_delete(expr);
	return e;
}
char *Node_toString(Expr expr){
	char *buf = NULL;
	int needed;
	needed = snprintf(buf,0,"(%c)",expr->data.letter);
	buf = allocate(needed+1);
	snprintf(buf,needed+1,"(%c)",expr->data.letter);
	return buf;
}
void Node_data_init(union un_data *data){
	data->letter = ' ';
}
void Node_data_copy(union un_data *datas, union un_data *datad){
	datad->letter = datas->letter;
}
