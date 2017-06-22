#include "LinkedList.h"
#include "Array.h"
#include "Array_loadSDF.h"

int main(int argc, char *argv[]){
	int len;
	char *filein, *fileout, *variable;
	Array array;
	
	if(argc != 4){
		fprintf(stdout,"usage : %s inputfile variable_id outputfile\n",argv[0]);
		exit(1);
	}
#ifdef LEAKDETECT
	LeakDetector_set(stdout);	
#endif
	len = strlen(argv[1])+1;
	filein = malloc(len);
	strncpy(filein,argv[1],len);
	
	len = strlen(argv[2])+1;
	variable = malloc(len);
	strncpy(variable,argv[2],len);
	
	len = strlen(argv[3])+1;
	fileout = malloc(len);
	strncpy(fileout,argv[3],len);
	
	array = Array_loadSDF(filein,variable);
	Array_print(array);
	if(array){Array_output(array,fileout,p_float);}
	Array_delete(array);
	
	free(filein);
	free(variable);
	free(fileout);
#ifdef LEAKDETECT
	LeakDetector_finish();	
#endif
	return 0;
}
