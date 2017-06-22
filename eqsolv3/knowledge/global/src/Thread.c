#include "knowledge.h"
Expr Thread(Expr expr){
	Expr e,en,child;
	Symbol head=NULL;
	int i,len=0,arglen,k,*seq=NULL;
	
	child = expr->child;
	if(!child){return expr;}
	arglen = Expr_getLength(child);
	len = Expr_getLength(child->child);
	
	if(arglen>3){return expr;}
	if(arglen>=2){
		e = Expr_getChild(expr,1);
		head = e->symbol;
	}
	if(arglen==3){
		seq = getseq(Expr_getChild(expr,2),len);
		if(!seq){goto uneval;}
		for(i=0,k=0;i<len;i++){k+=seq[i];}
		if(k==0){
			deallocate(seq); 
			goto through;
		}
	}
	en=Thread_mod(child,head,seq);
	
	if(en!=child){
	  through:
		for(e=child->next;e;e=e->next){Expr_deleteRoot(e);}
		Expr_replace(expr,en);
		Expr_delete(expr);
		expr = en;
	}
	return expr;
  uneval:
	if(message_flag){
		fprintf(stderr,"Thread::uneval\n Thread is unevaluated due to improper input\n");
	}
	return expr;
}

Expr Thread_mod(Expr expr, Symbol head, int *seq){
	Expr e,en=NULL,ecpy=NULL;
	int i,k,nchild=0,flag=1;
		
	if(!head){head=SymbolTable_get(id_List);}
	if(!seq){
		seq = allocate((k=Expr_getLength(expr->child))*sizeof(int));
		for(i=0;i<k;i++){seq[i]=1;}
	}
	
	for(e=expr->child,i=0;e;e=e->next,i++){
		if(seq[i]){
			if(head==e->symbol){
				k = Expr_getLength(e->child);
				if(k>0){
					if(!en){en =e; nchild = k;}
					else if(nchild != k){flag=0;break;}
				}
			}
		}
	}
	if(en && flag){
		ecpy = Expr_copy(en);
		for(i=0;i<nchild;i++){
			Expr_appendChild(ecpy,Expr_copy(expr));
		}
		for(e=expr->child,i=0;e;e=e->next,i++){
			if(seq[i]){
				if(head==e->symbol){
					for(k=0;k<nchild;k++){
						Expr_appendChild(Expr_getChild(ecpy,k),Expr_copyRoot(Expr_getChild(e,k)));
					}
					continue;
				}
			}
			for(k=0;k<nchild;k++){
				Expr_appendChild(Expr_getChild(ecpy,k),Expr_copyRoot(e));
			}
		}
		Expr_replace(expr,ecpy);
		Expr_deleteRoot(expr);
		expr = ecpy;
	}
	deallocate(seq);
	return expr;
}
int *getseq(Expr expr, int len){
	Expr e,child;
	
	int i,k,min=0,max=len,step=1;
	int *seq=NULL;
	
	if(expr->symbol->id==id_Integer){
		k = Integer_toInt(expr);
		if(k<0){
			k = len + k;
			if(k>0){
				seq = allocate(len*sizeof(int));
				for(i=0;i<k;i++){seq[i]=0;}
				for(i=k;i<len;i++){seq[i]=1;}
			}
		}else if(k <= len){
			seq = allocate(len*sizeof(int));
			for(i=0;i<k;i++){seq[i]=1;}
			for(i=k;i<len;i++){seq[i]=0;}
		}
		return seq;
	}else if(!expr->child){
		if(expr->symbol->id==id_None){
			seq = allocate(len*sizeof(int));
			for(i=0;i<len;i++){seq[i]=0;}
		}else if(expr->symbol->id==id_All){
			seq = allocate(len*sizeof(int));
			for(i=0;i<len;i++){seq[i]=1;}
		}
	}
	child = expr->child;
	k = Expr_getLength(child);
	if(k <= 3){
		if(expr->symbol->id==id_List){
			for(e=child;e;e=e->next){
				if(e->symbol->id!=id_Integer){return NULL;}
			}
			max=len;
			switch(k){
			  case 1:
				min=max=Integer_toInt(Expr_getChild(expr,0))-1;
				break;
			  case 3:
				step = Integer_toInt(Expr_getChild(expr,2));
			  case 2:
				min = Integer_toInt(Expr_getChild(expr,0))-1;
				max = Integer_toInt(Expr_getChild(expr,1))-1;
				if(min < 0 || max > len){return NULL;}
				break;
			  default:
				break;
			}
			printf("min:%d\n",min);
			printf("max:%d\n",max);
			printf("step:%d\n",step);
			seq = allocate(len*sizeof(int));
			for(i=0;i<len;i++){seq[i]=0;}
			for(i=min;i<=max;i+=step){seq[i]=1;}
		}
	}
	return seq;
}

void printseq(int *seq, int len){
	int i;
	fprintf(stderr,"seq = {");
	if(seq){
		for(i=0;i<len;i++){
			fprintf(stderr,"%d, ",seq[i]);
		}
	}else{fprintf(stderr,"null");}
	fprintf(stderr,"}\n");
}

