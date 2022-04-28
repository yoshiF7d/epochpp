#include "utilities.h"
#include "Data.h"
#include "Data_FFT.h"

int main(int argc, char *argv[]){
	ReadOptions_Sub ro;
	Data data,dataim;
	char filein[256];	
	char fileout[256];	
	char command[1024];

	int i,j;
	double x;
	FILE *fp;

	if(argc < 2){
		printf("usage : %s [filein] [fileout]\n",argv[0]);
	}else{
		strncpy(filein,argv[1],256);
	}
	
	if(argc >= 3){
		strncpy(fileout,argv[2],256);
	}

	if(access(filein,F_OK)==-1){
		fprintf(stderr,"sdf file does not exist\n"); exit(1);
	}

	ReadOptions_init(&ro);
	ReadOptions_setLineSeparators(&ro,3,"\n","\r","\r\n");
	ReadOptions_setFieldSeparators(&ro,2," ","\t");
	data = Data_readFile(filein,&ro);
	dataim = Data_create(data->row,data->column);
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column;j++){
			dataim->elem[i][j] = 0;
		}
	}
	Data_FFT2D(data,dataim);
	fp = fopen(fileout,"w");
	fprintf(fp,"re\n"); Data_fprint(data,fp," ");
	fprintf(fp,"im\n"); Data_fprint(dataim,fp," ");
	fclose(fp);
	return 0;
}
