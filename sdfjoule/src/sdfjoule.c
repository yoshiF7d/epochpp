#include "utilities.h"
#include "Data.h"

#define C_Pi 3.14159265358979
#define C_Lambda 10

#define PNG_X_SIZE 750
#define PNG_Y_SIZE 2250
/*een : total electric field energy per grid*/
/*ben : total magnetic field energy per grid*/
/*ken : total kinetic energy per grid*/

int main(int argc, char *argv[]){
	char filesdf[256];	
	char filename[256];	
	
	char filejx[256], filejy[256], filejz[256];
	char filete[256], filene[256];
	char fileeta[256], filejoule[256];	
	char dirpng[256];
	char command[1024];
	
	int i,j,n=0,r,c;
	double jx,jy,jz,j2,te,ne,eta,joule;
	double emin,emax;
	double x,y,z,total;
	Data datajx,datajy,datajz,dataeta,datate,datane,datajoule;
	FILE *gp;
	
	if(argc >= 2){
		strncpy(filesdf,argv[1],256);
	}else{
		loadfile(filesdf,256,"input sdf file");
	}
	if(argc >= 4){
		emin = atof(argv[2]);
		emax = atof(argv[3]);
	}else{
	  again2:
		puts("input energy min");
		if(scanf("%lf",&emin)!=1){
			scanf("%*s");
			goto again2;
		}
		puts("input energy max");
		if(scanf("%lf",&emax)!=1){
			scanf("%*s");
			goto again2;
		}
	}
	if(access(filesdf,F_OK)==-1){
		fprintf(stderr,"sdf file does not exist\n"); exit(1);
	}

	snprintf(command,256,"%s",getFileName(filesdf));
	snprintf(filename,strlen(command)-3,"%s",command);
	
	snprintf(filejx,256,"jx/%s.bmat",filename);
	snprintf(filejy,256,"jy/%s.bmat",filename);
	snprintf(filejz,256,"jz/%s.bmat",filename);
	
	snprintf(filete,256,"te/%s.bmat",filename);
	snprintf(filene,256,"ne/%s.bmat",filename);

	snprintf(fileeta,256,"joule/eta/%s.bmat",filename);
	snprintf(filejoule,256,"joule/joule/%s.bmat",filename);
	
	mkdir("jx",0777); mkdir("jy",0777); mkdir("jz",0777);
	mkdir("te",0777);
	mkdir("ne",0777);	
	mkdir("joule",0777);
	mkdir("joule/eta",0777);
	mkdir("joule/joule",0777);
	mkdir("joule/pngs",0777);
	
	if(access(filejx,F_OK)==-1){
		snprintf(command,1024,"sdf2bmat -cni -v jx -f %s %s",filejx,filesdf);
		fprintf(stdout,"%s\n",command);
		system(command);
	}
	if(access(filejy,F_OK)==-1){
		snprintf(command,1024,"sdf2bmat -cni -v jy -f %s %s",filejy,filesdf);
		fprintf(stdout,"%s\n",command);	
		system(command);
	}
	if(access(filejz,F_OK)==-1){
		snprintf(command,1024,"sdf2bmat -cni -v jz -f %s %s",filejz,filesdf);
		fprintf(stdout,"%s\n",command);	
		system(command);
	}	
	if(access(filete,F_OK)==-1){
		snprintf(command,1024,"sdf2bmat -cni -v temperature/electron -f %s %s",filete,filesdf);
		fprintf(stdout,"%s\n",command);	
		system(command);
	}
	if(access(filene,F_OK)==-1){
		snprintf(command,1024,"sdf2bmat -cni -v number_density/electron -f %s %s",filene,filesdf);
		fprintf(stdout,"%s\n",command);	
		system(command);
	}
	datajx = Data_input(filejx); datajy = Data_input(filejy); datajz = Data_input(filejz);
	datate = Data_input(filete); datane = Data_input(filene);	
	r = Data_getRow(datate); c = Data_getColumn(datate);
	dataeta = Data_create(r,c); datajoule = Data_create(r,c);
	
	for(i=0;i<r;i++){
		for(j=0;j<c;j++){
			jx = Data_get(datajx,i,j);
			jy = Data_get(datajy,i,j);
			jz = Data_get(datajz,i,j);
			te = Data_get(datate,i,j);
			ne = Data_get(datane,i,j);			
			/*spitzer*/
			eta = C_Lambda*9.6946548e-5/pow(te,1.5);
			Data_set(dataeta,i,j,eta);
			j2 = jx*jx + jy*jy + jz*jz;
			//if((i%100)*(j%100)==0){fprintf(stderr,"eta : %e\n",eta);}
			//if((i%100)*(j%100)==0){fprintf(stderr,"j2 : %e\n",j2);}
			Data_set(datajoule,i,j,(eta*j2/ne)*(1e-15/1.602176565e-19));	
		}
	}

	Data_output(datajoule,filejoule,p_float);
	Data_output(dataeta,fileeta,p_float);
	
	gp = popen("gnuplot -persist\n","w");
	if(gp==NULL){perror("gnuplot"); exit(1);}
	fprintf(gp,"set term png size %d,%d\n",c,r);
	fprintf(gp,"set cbr[%e:%e]\n",emin,emax);
	fprintf(gp,"set xr[%d:%d]\n",0,c);
	fprintf(gp,"set yr[%d:%d]\n",0,r);
	fprintf(gp,"set size ratio %f\n",(double)r/c);	
	fprintf(gp,"set output \"joule/pngs/%s.png\"\n",filename);
	fprintf(gp,"plot '-' binary array=(%d,%d) format=\"%%float64\" endian=little w image notitle\n",c,r);
	for(i=0;i<r;i++){
		for(j=0;j<c;j++){
			x = Data_get(datajoule,i,j);
			fwrite(&x,sizeof(double),1,gp);
		}
	}	
	pclose(gp);
		
	Data_delete(datajx); Data_delete(datajy); Data_delete(datajz);
	Data_delete(datate); Data_delete(dataeta); Data_delete(datajoule);
	
	return 0;
}
