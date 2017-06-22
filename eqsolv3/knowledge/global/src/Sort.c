#include "knowledge.h"
Expr Sort(Expr expr){
	Expr e;
	if(!expr->child){return expr;}
	expr->child->child = Sort_mod(expr->child->child);
	Expr_replace(expr,e=expr->child);
	Expr_delete(expr);
	return e;
}
Expr Sort_mod(Expr expr){
	Expr a,b;
	if((expr==NULL) || (expr->next==NULL)){return expr;}
	Sort_split(expr,&a,&b);
	a = Sort_mod(a);
	b = Sort_mod(b);
	return Sort_merge(a,b);
}
Expr Sort_merge(Expr a, Expr b){
	Expr result=NULL;
	if(a==NULL){return b;}
	else if(b==NULL){return a;}
	if(Order_mod(a,b)>0){
		result = b; result->next = Sort_merge(a,b->next); result->next->previous = result;
	}else{
		result = a; result->next = Sort_merge(a->next,b); result->next->previous = result;
	}
	return result;
}
void Sort_split(Expr source, Expr *front, Expr *back){
	Expr fast,slow;
	if(source==NULL || source->next==NULL){
		*front = source;
		*back = NULL;
	}else{
		slow = source;
		fast = source->next;
		while(fast != NULL){
			fast = fast->next;
			if(fast != NULL){
				slow = slow->next;
				fast = fast->next;
			}
		}
		*front = source;
		*back = slow->next;
		(*back)->previous = NULL;
		slow->next = NULL;
	}
}
