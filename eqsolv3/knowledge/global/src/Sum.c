#include "knowledge.h"
Expr Sum_replace(Expr expr, Expr local, Expr rhs){
	//puts("replace");
	Expr root,e;
	if(!expr){return expr;}
	if(expr->symbol == local->symbol){
		root = Expr_copy(rhs);
	}else{
		root = Expr_copy(expr);
	}
	if(expr->child){
		for(e=expr->child;e;e=e->next){
			Expr_appendChild(root,Sum_replace(e,local,rhs));
		}
	}
	return root;
}
Expr Sum_replaceInt(Expr expr, Expr local, int k){
	//puts("replaceInt");
	Expr root,e;
	if(!expr){return expr;}
	if(expr->symbol == local->symbol){
		root = Integer_createInt(k);
	}else{
		root = Expr_copy(expr);
	}
	if(expr->child){
		for(e=expr->child;e;e=e->next){
			Expr_appendChild(root,Sum_replaceInt(e,local,k));
		}
	}
	return root;
}
Expr Sum_iterate(Expr expr, Expr local,int imin,int imax,int di){
	Expr root;
	int i;
	//puts("replaceIterate");
	if(!expr){return expr;}
	root = Expr_create(id_Plus);
	for(i=imin;i<=imax;i+=di){
		Expr_appendChild(root,Evaluate(Sum_replaceInt(expr,local,i)));
	}
	return root;
}
Expr Sum(Expr expr){
	Expr e,rtn,spec,main=expr->child,local;
	int i,slen,imax,imin=1,di=1;
	if(!main){return expr;}
	spec = main->next;
	if(spec->next){
		main->next = spec->next;
		spec->next->previous = main;
		Expr_isolate(spec);
		rtn = Expr_create(id_Table);
		Expr_insert(expr->parent,expr,rtn);
		Expr_appendChild(rtn,spec);
		return Evaluate(rtn);
	}
	if(spec->symbol->id != id_List){goto itform;}
	if(!spec->child){goto itform;}
	slen = Expr_getLength(spec->child);
	if(slen==1){
		e = Evaluate(spec->child);
		if(e->symbol->id != id_Integer){goto iterb;}
		imax = Integer_toInt(e);
		rtn = Expr_create(id_Plus);
		main->next = NULL;
		for(i=0;i<imax;i++){
			Expr_appendChild(rtn,Evaluate(Expr_copyRoot(main)));
		}
		goto end;
	}
	local = spec->child;
	if(local->symbol->attributes & Protected){goto itwrite;}
	if(AtomQ_mod(local)){goto itraw;}
	if(spec->child->next->symbol->id==id_List){
		rtn = Expr_create(id_Plus);
		main->next = NULL;
		for(e=spec->child->next->child;e;e=e->next){
			Expr_appendChild(rtn,Evaluate(Sum_replace(main,local,e)));
		}
		goto end;
	}
	if(slen==2){
		e = Evaluate(spec->child->next);
		if(e->symbol->id != id_Integer){goto itform;}
		imax = Integer_toInt(e);
	}
	if(slen>=3){
		e = Evaluate(spec->child->next);
		if(e->symbol->id != id_Integer){goto itform;}
		imin = Integer_toInt(e);
		e = Evaluate(spec->child->next->next);
		if(e->symbol->id != id_Integer){goto itform;}
		imax = Integer_toInt(e);
	}
	if(slen==4){
		e = Evaluate(spec->child->next->next->next);
		if(e->symbol->id != id_Integer){goto itform;}
		di = Integer_toInt(e);
	}
	main->next = NULL;
	rtn = Sum_iterate(main,local,imin,imax,di);
  end:
	main->next = spec;
	Expr_deleteRoot(expr);
	return Evaluate(rtn);
  itraw:
	if(message_flag){
		fprintf(stderr,"Sum::itraw: Raw object ");
		FullForm_mod2(stderr,local);
		fprintf(stderr," cannot be used as an iterator\n");
	}
	return expr;
  itwrite:
	if(message_flag){
		fprintf(stderr,"Sum::write: Tag ");
		FullForm_mod2(stderr,local);
		fprintf(stderr," in ");
		FullForm_mod2(stderr,spec);
		fprintf(stderr," is protected\n");
	}
	return expr;
  itform:
	if(message_flag){
		fprintf(stderr,"Sum::itform: Argument ");
		FullForm_mod2(stderr,spec);
		fprintf(stderr," does not have the correct form for an iterator.\n");
	}
	return expr;
  iterb:
	if(message_flag){
		fprintf(stderr,"Sum::iterb: Iterator ");
		FullForm_mod2(stderr,spec);
		fprintf(stderr," does not have appropriate bounds.\n");
	}
	return expr;
}
