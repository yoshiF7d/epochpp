#include "utilities.h"
#include "rational.h"

char *rat_toString(rat_t r){
	size_t n,d;
	char *buf;
	n = snprintf(NULL,0,"%d",r.num);
	d = snprintf(NULL,0,"%d",r.den);
	buf = allocate(n+d+2);
	snprintf(buf,n+d+2,"%d/%d",r.num,r.den);
	return buf;
}