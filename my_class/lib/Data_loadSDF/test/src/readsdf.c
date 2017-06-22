#include "LinkedList.h"
#include "Data.h"
#include "Data_loadSDF.h"

int main(int argc, char *argv[]){
	int len;
	char *filein, *fileout, *variable;
	FILE *fp;
	Data data;
	
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
	
	data = Data_loadSDF(filein,variable);
	if((fp = fopen(fileout,"w"))==NULL){perror("readsdf"); exit(1);};
	if(data){Data_fprint(data,fp," ");}
	fclose(fp);
	Data_delete(data);
	
	free(filein);
	free(variable);
	free(fileout);
#ifdef LEAKDETECT
	LeakDetector_finish();	
#endif
	return 0;
}
