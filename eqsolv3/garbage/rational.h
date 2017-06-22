#ifndef RATIONAL_H
#define RATIONAL_H
typedef struct st_rat{
	int num;
	int den;
}rat_t;

char *rat_toString(rat_t r);

#endif