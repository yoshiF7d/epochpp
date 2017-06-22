#include "knowledge.h"
Expr Map_mod(Expr func, Expr expr, int ns, int ne, int n){
	Expr e,head,root;
	if(n==0){
		head = Evaluate(
			Expr_appendChild(Expr_copy(func),root=Expr_copy(expr))
		);
	}else{
		head = root = Expr_copy(expr);
	}
	if((ns <= n) && (n <= ne)){
		for(e=expr->child;e;e=e->next){
			Expr_appendChild(
				root,
				Evaluate(
					Expr_appendChild(Expr_copy(func),Map_mod(func,e,ns,ne,n+1))
				)
			);
		}
	}else if(n <= ne){
		for(e=expr->child;e;e=e->next){
			Expr_appendChild(root,Map_mod(func,e,ns,ne,n+1));
		}
	}
	return head;
}

Expr Map(Expr expr){
	Expr e,func,body,lspec;
	int ns=1,ne=1,length;
	func = expr->child;
	if(!func){return expr;}
	body = func->next;
	if(!body){goto argtu;}
	lspec = body->next;
	
	if(lspec){
		switch(lspec->symbol->id){
		  case id_List:
			length=Expr_getLength(lspec->child);
			switch(length){
			  case 0:
				break;
			  case 1:
				if(lspec->child->symbol->id!=id_Integer){goto level;}
				ns = ne = Integer_toInt(lspec->child);
				break;
			  case 2:
				if(lspec->child->symbol->id!=id_Integer){goto level;}
				ns = Integer_toInt(lspec->child);
				if(lspec->child->next->symbol->id!=id_Integer){
					if(lspec->child->next->symbol->id==id_Infinity){
						ne = Depth_mod(body);
					}else{
						goto level;
					}
				}else{
					ne = Integer_toInt(lspec->child->next);
				}
				break;
			  default:
				goto level;
			}
			break;
		  case id_Integer:
			ns = 1;
			ne = Integer_toInt(lspec);
			break;
		  case id_Infinity:
			ns = 1;
			ne = Depth_mod(body);
			break;
		  default:
			goto level;
		}
	}
	if(ns < 0){ns = Depth_mod(body) + ns;}
	if(ne < 0){ne = Depth_mod(body) + ne;}
	e = Map_mod(func,body,ns,ne,ns);
	Expr_replace(expr,e);
	Expr_deleteRoot(expr);
	return e;
  argtu:
	if(message_flag){
		fprintf(stderr,"Map::argtu : Map called with 1 argument; 2 or 3 arguments are expected.");
	}
	return expr;
  level:
	if(message_flag){
		fprintf(stderr,"Map::level : Level specification ");
		FullForm_mod(lspec);
		fprintf(stderr," is not of the form n, {n}, or {m,n}.");
	}
	return expr;
}
 