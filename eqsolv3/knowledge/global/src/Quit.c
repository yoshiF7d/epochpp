#include "knowledge.h"
Expr Quit(Expr expr){
	Expr_deleteRoot(expr);
	return NULL;
}
