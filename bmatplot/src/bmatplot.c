#include "utilities.h"
#include "Data.h"

int main(int argc, char *argv[]){
	char filebmat[256];	
	char filename[256];
	char filename2[256];
	char outdir[256];	
	char commandfile[256];
	char *buf;
	
	int i,j,n=0,r,c;
	double emin=0,emax=1e-5,x;

	Data data;
	FILE *gp;
	
	if(argc >= 2){
		strncpy(filebmat,argv[1],256);
	}else{
		loadfile(filebmat,256,"input input file name");
	}
	if(argc >= 3){
		strncpy(outdir,argv[2],256);
	}else{
		loadfile(outdir,256,"input output directory name");
	}
	
	if(argc >= 5){
		emin = atof(argv[3]);
		emax = atof(argv[4]);
	}else{
		printf("emin=\n"); scanf("%lf",&emin);
		printf("emax=\n"); scanf("%lf",&emax);
	}
	
	if(argc == 6){
		strncpy(commandfile,argv[5],256);
	}else{
		loadfile(commandfile,256,"input command file name");
	}
	
	if(access(filebmat,F_OK)==-1){
		fprintf(stderr,"bmatplot file does not exist\n"); exit(1);
	}
	mkdir(outdir,0777);

	snprintf(filename,256,"%s",getFileName(filebmat));
	snprintf(filename2,strlen(filename)-4,"%s",filename);
	
	if(!strcmp(getFileExtension(filebmat),".bmat")){
		data = Data_loadBMAT(filebmat);
	}else if(!strcmp(getFileExtension(filebmat),".bary")){
		data = Data_loadBARY(filebmat);	
	}
	r = Data_getRow(data);
	c = Data_getColumn(data);
	buf = String_read(commandfile);
	
	gp = popen("gnuplot -persist\n","w");
	if(gp==NULL){perror("gnuplot"); exit(1);}
	fprintf(gp,"set term png size %d,%d\n",c,r);
	fprintf(gp,"set cbr[%e:%e]\n",emin,emax);
	fprintf(gp,"set xr[%d:%d]\n",0,c);
	fprintf(gp,"set yr[%d:%d]\n",0,r);
	fprintf(gp,"set size ratio %f\n",(double)r/c);
	fprintf(gp,"%s",buf);
	fprintf(gp,"set output \"%s/%s.png\"\n",outdir,filename2);
	fprintf(gp,"plot '-' binary array=(%d,%d) format=\"%%float64\" endian=little w image notitle\n",c,r);
	for(i=0;i<r;i++){
			for(j=0;j<c;j++){
					x = Data_get(data,i,j);
					fwrite(&x,sizeof(double),1,gp);
			}
	}
	Data_delete(data);
	deallocate(buf);
	return 0;
}
