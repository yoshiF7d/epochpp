#include "knowledge.h"
Expr Lambda_pre(Expr expr){
	Expr args,body;
	if(expr->data.lambda.body){
		return expr;
	}
	args = Expr_getChild(expr,0);
	body = Expr_getChild(expr,1);
	if(!body){
		body=args;
		args=NULL;
	}
	expr->data.lambda.args = Expr_isolate(args);
	expr->data.lambda.body = Expr_isolate(body);
	expr->child = NULL;
	return expr;
}
Expr Lambda(Expr expr){
	Expr args,body,e,e2;
	int len,arglen=0;
	char *buf1,*buf2;
	args = expr->data.lambda.args;
	body = expr->data.lambda.body;
	
	len = Expr_getLength(expr->child);
	if(!len){return expr;}
	
	if(args){
		if(args->symbol->id==id_List){
			for(e=args->child;e;e=e->next){
				if(e->child){goto flpar;}
				else{
					switch(e->symbol->id){
					  case id_Integer:
					  case id_Real:
					  case id_String:
						goto flpar;
					  default:
						break;
					}
				}
			}
			arglen = Expr_getLength(args->child);
			if(len < arglen){
				goto fpct;
			}else{
				for(e=args->child,e2=expr->child;e;e=e->next,e2=e2->next){
					body = ReplaceAll_mod_mod(body,e,e2);
				}
			}
		}else{
			arglen = 1;
			body = ReplaceAll_mod_mod(body,args->child,expr->child);
		}
	}else{
		for(e=expr->child,arglen=1;e;e=e->next,arglen++){
			e2 = Expr_create(id_Slot);
			e2 = Expr_appendChild(e2,Integer_createInt(arglen));
			body = ReplaceAll_mod_mod(body,e2,e);
			Expr_deleteRoot(e2);
		}
	}
	Expr_replace(expr,body=Evaluate(body));
	expr->data.lambda.body=NULL;
	Expr_deleteRoot(expr);
	expr = body;
	return expr;
  flpar:
	fprintf(stderr,"Lambda :: flpar : Parameter specification in Lambda should be a symbol or a list of symbols.");
	return expr;
  fpct:
	buf1 = OutputForm_mod_str(args);
	buf2 = OutputForm_mod_str(expr);
	fprintf(stderr,"Lambda :: fpct : Too many parameters in %s to be filled from %s\n",buf1,buf2);
	return expr;
}
void Lambda_data_init(union un_data *data){
	data->lambda.args=NULL;
	data->lambda.body=NULL;
}
void Lambda_data_finish(union un_data *data){
	Expr_deleteRoot(data->lambda.args);
	Expr_deleteRoot(data->lambda.body);
}
void Lambda_data_copy(union un_data *datas, union un_data *datad){
	datad->lambda.args = Expr_copyRoot(datas->lambda.args);
	datad->lambda.body = Expr_copyRoot(datas->lambda.body);
}
char *Lambda_toString(Expr expr){
	char *buf=NULL,*buf2=NULL,*buf3=NULL;
	int needed;
	buf2 = OutputForm_mod_str(expr->data.lambda.body);
	if(expr->data.lambda.args){
		buf3 = OutputForm_mod_str(expr->data.lambda.args);
		needed = snprintf(buf,0,"Lambda[%s,%s]",buf3,buf2);
		buf = allocate(needed+1);
		snprintf(buf,needed+1,"Lambda[%s,%s]",buf3,buf2);
		deallocate(buf2);
		deallocate(buf3);
	}else{
		needed = snprintf(buf,0,"%s &",buf2);
		buf = allocate(needed+1);
		snprintf(buf,needed+1,"%s &",buf2);
		deallocate(buf2);
	}
	return buf;
}
