#include "LinkedList.h"
#include "Data.h"
#include "Data_loadSDF.h"

int main(int argc, char *argv[]){
	int len;
	char *filein, *fileout, *variable;
	Data data;
	
	if(argc != 4){
		fprintf(stdout,"usage : %s inputfile variable_id outputfile\n",argv[0]);
		exit(1);
	}
	
	len = strlen(argv[1])+1;
	filein = malloc(len);
	strncpy(filein,argv[1],len);
	
	len = strlen(argv[2])+1;
	variable = malloc(len);
	strncpy(variable,argv[2],len);
	
	len = strlen(argv[3])+1;
	fileout = malloc(len);
	strncpy(fileout,argv[3],len);
	
	data = Data_loadSDF(filein,variable);
	if(data){Data_output(data,fileout,p_float);}
	Data_delete(data);
	
	free(filein);
	free(variable);
	free(fileout);
	return 0;
}
