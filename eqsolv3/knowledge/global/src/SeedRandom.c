#include "knowledge.h"
Expr SeedRandom(Expr expr){
	Expr e;
	int len;
	double min=0,max=1;
	
	if(expr->child){
		switch(expr->child->symbol->id){
		  case id_Integer:
			randomSeed(Integer_toInt(expr->child));
		  case id_Null:
			randomSeed((unsigned)time(NULL));
		  default:
			goto seed;
		}
	}else{
		randomSeed((unsigned)time(NULL));
	}
	output_flag=0;
	return expr;
  seed:
	fprintf(stderr,"Argument in SeedRandom should be an integer or a string.\n");
	return expr;
}
