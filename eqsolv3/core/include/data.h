#ifndef DATA_H
#define DATA_H
#define R64_LIMIT 52
#define I32_LIMIT 31
#define I64_LIMIT 63
#include <gmp.h>
#include <mpfr.h>
/*====== data types ======*/
union un_data{
	Symbol symbol;
	struct st_int{
		long prec;
		union un_int{
			int mp;
			mpz_t ap;
		}data;
	}integer;
	struct{
		Expr args;
		Expr body;
	}lambda;
	struct{
		Symbol symbol;
		Expr object;
		Expr capture;
	}pattern;
	struct st_real{
		long prec;
		union un_real{
			double mp;
			mpfr_t ap;
		}data;
	}real;
	char *string;
	int letter;
	Expr expr;
	
};/*====== data initialization function ======*/
void data_init(union un_data *data, int id);
/*====== data finish function ======*/
void data_finish(union un_data *data, int id);
/*====== data copy function ======*/
void data_copy(union un_data *datas,union un_data *datad,int id);
/*====== data equals function ======*/
int data_equals(union un_data *d1,union un_data *d2,int id);


#endif
