#include "knowledge.h"
/*
typedef struct st_ilist{
	int index;
	struct st_ilist *next;
}IList_Sub;
typedef IList_Sub *IList;

IList IList_create(int i);
void IList_delete(IList r);
void IList_set(IList r,int level,int index); 
IList IList_append(IList r, IList r2);

IList IList_create(int i){
	IList l;
	l = allocate(sizeof(IList_Sub));
	l->index = i;
	l->next = NULL;
	return l;
}

void IList_delete(IList l){
	if(l){
		IList_delete(l->next);
		deallocate(l);
	}
}

void IList_set(IList l,int level,int index){
	int i;
	IList s;
	for(i=1,s=l;i<level;i++,s=s->next){
		if(!s){fprintf(stderr,"route_set : level %d does not exist\n",i);}
		s->index = index;
	}
}

IList IList_append(IList l, IList l2){
	IList ll;
	if(!l){
		if(l2){return l2;}
	}
	if(!l2){return l;}
	for(ll=l;ll->next;ll=ll->next){}
	ll->next = l2;
	return l;
}
Expr get_index(Expr expr,IList list){
	IList l;
	Expr e,r;
	int i;
	for(l=list,r=expr;l;l=l->next){
		if(!r){return NULL;}
		for(e=r->child,i=1;e && i<l->index;e=e->next,i++){}
		if(!e){return NULL;}
		r=e->child;
	}
	return r;
}
*/
Expr Dot_mod_mod(Expr e1, Expr e2, int n1, int n2, int *index, int *dim, int len, int k);
Expr Dot_combine(Expr e1, Expr e2, int n1, int n2, int *index, int len);
Expr Dot_part(Expr expr,int *index, int i_min, int i_max, int k, int pos);
enum en_doterr{dotrect=1,dotsh};
Expr Dot(Expr expr){
	Expr e,e1,e2,rtn;
	int len1,len2,err=0;
	
	if(!expr->child){return expr;}
	e1 = expr->child;
	e2 = expr->child->next;
	if(!e2){
		Expr_replace(expr,e1);
		Expr_delete(expr);
		return e1;
	}
	rtn = Dot_mod(e1,e2,&err);
	if(!rtn){
		switch(err){
		  case dotrect:
			fprintf(stderr,"Dot::rect: Nonrectangular tensor encountered.\n");
			break;
		  case dotsh:
			fprintf(stderr,"Dot::dotsh: Tensors have incompatible shapes.\n");
			break;
		  default:
			break;
		}
		return expr;
	}
	Expr_replace(expr,rtn);
	Expr_deleteRoot(expr);
	return rtn;
}
Expr Dot_mod(Expr e1, Expr e2, int *err){
	Expr e,rtn;
	int *index, *dim, i,n1,n2,len1,len2;
	
	n1 = ArrayDepth_mod(e1);
	n2 = ArrayDepth_mod(e2);
	n1--; n2--;
	if(!ArrayQ_mod(e1) || !ArrayQ_mod(e2)){
		*err = dotrect;
		return NULL;
	}
	for(e=e1,i=0;e && i<n1;e=e->child,i++){}
	len1 = Expr_getLength(e->child);
	len2 = Expr_getLength(e2->child);
	/*
	fprintf(stderr,"len1 : %d\n",len1);
	fprintf(stderr,"len2 : %d\n",len2);
	*/
	if(len1 != len2){
		*err = dotsh; 
		return NULL;
	}
	index = allocate((n1+n2)*sizeof(int));
	dim = allocate((n1+n2)*sizeof(int));
	for(e=e1,i=0;i<n1;e=e->child,i++){
		dim[i] = Expr_getLength(e->child);
	}
	for(e=e2->child,i=n1;i<n1+n2;e=e->child,i++){
		dim[i] = Expr_getLength(e->child);
	}
	for(i=0;i<n1+n2;i++){index[i]=0;}
	/*
	fprintf(stderr,"e1\n");
	for(i=0;i<n1;i++){
		fprintf(stderr,"dim[%d]=%d\n",i,dim[i]);
	}
	fprintf(stderr,"e2\n");
	for(i=0;i<n2;i++){
		fprintf(stderr,"dim[%d]=%d\n",i,dim[i]);
	}
	*/
	rtn = Dot_mod_mod(e1,e2,n1,n2,index,dim,len1,0);
	deallocate(index);
	deallocate(dim);
	return rtn;
}
Expr Dot_mod_mod(Expr e1, Expr e2, int n1, int n2, int *index, int *dim, int len, int k){
	int i;
	Expr rtn;
	if(k<n1+n2){
		rtn = Expr_create(id_List);
		for(i=0;i<dim[k];i++){
			index[k] = i;
			rtn = Expr_appendChild(rtn,Dot_mod_mod(e1,e2,n1,n2,index,dim,len,k+1));
		}	
	}else{
		rtn=Dot_combine(e1,e2,n1,n2,index,len);
	}
	return rtn;
}
enum en_dotpos{pos_first, pos_last};
Expr Dot_combine(Expr e1, Expr e2, int n1, int n2, int *index, int len){
	int i;
	Expr rtn,term;
	rtn = Expr_create(id_Plus);
	/*
	fprintf(stderr,"Dot_combine : ");
	fprintf(stderr,"index = ");
	fprintf(stderr,"{");
	for(i=0;i<n1+n2;i++){
		fprintf(stderr,"%d",index[i]);
		if(i<n1+n2-1){fprintf(stderr,", ");}
	}
	fprintf(stderr,"}\n");
	*/
	for(i=0;i<len;i++){
		term = Expr_create(id_Times);
		term=Expr_appendChild(term,Expr_copyRoot(Dot_part(e1,index,0,n1,i,pos_last)));
		term=Expr_appendChild(term,Expr_copyRoot(Dot_part(e2,index,n1,n1+n2,i,pos_first)));
		rtn = Expr_appendChild(rtn,term);
	}
	return Evaluate(rtn);
}
/*e1 index : {i1,i2,...n1,k}*/
/*e2 index : {k,j2,...n2}*/
/*k is a index used in summation*/
/*e1 : k is at last position. pos -> pos_last*/
/*e2 : k is at first position. pos -> pos_first*/

/*e1.e2 index : {i1,i2,...j2,j3,...n1+n2}*/
/*e1 : imin : i1, imax : n1 (front part)*/
/*e2 : imin : n1, imax : n1+n2 (rear part)*/
Expr Dot_part(Expr expr,int *index, int i_min, int i_max, int k, int pos){
	Expr e,r=expr;
	int i,j;
	/*
	fprintf(stderr,"index = ");
	fprintf(stderr,"{");
	for(i=i_min;i<i_max;i++){
		fprintf(stderr,"%d, ",index[i]);
	}
	fprintf(stderr,"%d}\n",k);
	fprintf(stderr,"i_min : %d\n",i_min);
	fprintf(stderr,"i_max : %d\n",i_max);
	*/
	if(pos==pos_first){
		for(e=r->child,i=0;e&&i<k;e=e->next,i++){}
		r = e;
	}
	for(i=i_min;i<i_max;i++){
		for(e=r->child,j=0;e&&j<index[i];e=e->next,j++){}
		r = e;
	}
	if(pos==pos_last){
		for(e=r->child,i=0;e&&i<k;e=e->next,i++){}
		r = e;
	}
	/*
	fprintf(stderr,"expr : ");
	if(r){FullForm_mod2(stderr,r); fputc('\n',stderr);}
	else{fprintf(stderr,"null\n");}
	*/
	return r;
}
