#include "Expr.h"

Expr breed(int i);

int main(void){
	Expr root;
	int i,j,k;
	double d;
	k = 0;
	
#ifdef LEAKDETECT
	LeakDetector_set(stdout);
#endif	
	//StringList sl=StringList_init();
	mp_set_memory_functions(allocate2,reallocate2,deallocate2);
	root =  breed(4);
	//Node_toString_set(sl);
	//Expr_debug(root,0);
	Expr_printTree(root);
	//Node_toString_finish();
	Expr_deleteRoot(root);
#ifdef LEAKDETECT
	LeakDetector_finish();
#endif
}

Expr breed(int i){
	int k;
	//printf("breed %d\n",i);
	Expr root=Expr_createInteger(i);
	for(k=0;k<i;k++){root=Expr_appendChild(root,breed(i-1));}
	return root;
}
