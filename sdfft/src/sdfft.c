#include "utilities.h"
#include "Data.h"
#include "LinkedList.h"
#include "Data_loadSDF.h"
#include "Data_FFT.h"

#define C_e0 8.854187e-12
#define C_u0 1.256637e-06
/*een : total electric field energy per grid*/
/*ben : total magnetic field energy per grid*/
/*ken : total kinetic energy per grid*/

#define MIN(x, y) (((x) < (y)) ? (x) : (y))
Data radialProfile(Data datab, Data datae, Data datak, Data dataa, int n, double w, double dk){
	Data profile;
	int i,j,k,rmax;
	int center[2];
	double r,norm,weight,factor;
	int count;

	center[0] = datab->row/2;
	center[1] = datab->column/2;

	rmax = MIN(datab->row,datab->column);
	profile = Data_create(n,4);

	for(i=0;i<n;i++){
		profile->elem[i][0] = (dk*i*rmax)/n;
		profile->elem[i][1] = profile->elem[i][2] = profile->elem[i][3] = 0;
		r = (double)(i*rmax)/n;
		weight = 0;
		for(j=0;j<datab->row;j++){
			for(k=0;k<datab->column;k++){
				norm = sqrt((j-center[0])*(j-center[0]) + (k-center[1])*(k-center[1]));
				factor = exp(-(norm-r)*(norm-r)/(w*w));
				profile->elem[i][1] += datab->elem[j][k] * factor;
				profile->elem[i][2] += datae->elem[j][k] * factor;
				profile->elem[i][3] += datak->elem[j][k] * factor;
				profile->elem[i][4] += dataa->elem[j][k] * factor;
				weight += factor;
			}
		}	
		profile->elem[i][1] /= weight;
		profile->elem[i][2] /= weight;
		profile->elem[i][3] /= weight;
		profile->elem[i][4] /= weight;
	}

	return profile;
}

void fft_abs_shift(Data data, Data dataim){
	int i,j;
	for(i=0;i<dataim->row;i++){
		for(j=0;j<dataim->column;j++){
			dataim->elem[i][j] = 0;
		}
	}
	Data_FFT2D(data,dataim);
	Data_abs(data,dataim);
	Data_shift(data);
}

int main(int argc, char *argv[]){
	char filesdf[256];	
	char filename[256];	
	
	char dirout[256];
	/*
	char dirouta[256];
	char diroutb[256];
	char diroute[256];
	char diroutk[256];
	*/
	char file[256];
	/*
	char filea[256];
	char fileb[256];
	char filee[256];
	char filek[256];
	*/	
	char command[1024];
	
	int i,j,n=0,r,c;
	double ec,bc,bx,by,bz,ex,ey,ez;
	double area,ben,een,ken;
	double xmax,ymax,dk,sigma=2,binc=500;
	
	Data databx,databy,databz,dataex,dataey,dataez,datak,datad,databen,dataeen,dataken,datag;
	Data data,dataim,profile;
	LinkedList namelist,datalist;
	FILE *fp;

	if(argc < 2){
		printf("usage : %s [inputfile name]\n",argv[0]);
	}else{
		strncpy(filesdf,argv[1],256);
	}
		
	if(argc >= 3){
		sigma = atof(argv[2]);
	}
	
	if(argc >= 4){
		binc = atoi(argv[3]);
	}
	
	if(access(filesdf,F_OK)==-1){
		fprintf(stderr,"sdf file does not exist\n"); exit(1);
	}
	snprintf(filename,256,"%s",getFileName(filesdf));
	snprintf(command,strlen(filename)-3,"%s",filename);
	sprintf(filename,"%s",command);	
	
	snprintf(command,getFileName(filesdf)-filesdf,"%s",filesdf);
	snprintf(dirout,256,"%s/fft",command);
	/*
	snprintf(dirouta,256,"%s/fft/all",command);
	snprintf(diroutb,256,"%s/fft/ben",command);
	snprintf(diroute,256,"%s/fft/een",command);
	snprintf(diroutk,256,"%s/fft/ken",command);
	*/
	
	snprintf(file,256,"%s/%s.txt",dirout,filename);
	
	/*
	snprintf(filea,256,"%s/%s.txt",dirouta,filename);
	snprintf(fileb,256,"%s/%s.txt",diroutb,filename);
	snprintf(filee,256,"%s/%s.txt",diroute,filename);
	snprintf(filek,256,"%s/%s.txt",diroutk,filename);
	*/
	
	mkdir(dirout,0777);
	/*
	mkdir(dirouta,0777);
	mkdir(diroutb,0777);
	mkdir(diroute,0777);
	mkdir(diroutk,0777);
	*/
	namelist = NULL;
	namelist = LinkedList_append(namelist,String_copy("ex"));
	namelist = LinkedList_append(namelist,String_copy("ey"));
	namelist = LinkedList_append(namelist,String_copy("ez"));
	
	namelist = LinkedList_append(namelist,String_copy("bx"));
	namelist = LinkedList_append(namelist,String_copy("by"));
	namelist = LinkedList_append(namelist,String_copy("bz"));
	
	namelist = LinkedList_append(namelist,String_copy("number_density"));
	namelist = LinkedList_append(namelist,String_copy("ekbar"));
	namelist = LinkedList_append(namelist,String_copy("grid"));
	
	datalist = Data_loadSDFList2(filesdf,namelist);
	dataex = LinkedList_getIndex(datalist,0);
	dataey = LinkedList_getIndex(datalist,1);
	dataez = LinkedList_getIndex(datalist,2);
	databx = LinkedList_getIndex(datalist,3);
	databy = LinkedList_getIndex(datalist,4);
	databz = LinkedList_getIndex(datalist,5);
	datad = LinkedList_getIndex(datalist,6);
	datak = LinkedList_getIndex(datalist,7);
	datag = LinkedList_getIndex(datalist,8);
	
	r = Data_getRow(datak); 
	c = Data_getColumn(datak);

	data = Data_create(r,c);
	dataim = Data_create(r,c);
	databen = Data_create(r,c); 
	dataeen = Data_create(r,c);
	dataken = Data_create(r,c);
	
	xmax = datag->elem[0][c] - datag->elem[0][0];
	ymax = datag->elem[1][r] - datag->elem[1][0];
	
	area = xmax * ymax;
	
	if(xmax > ymax){
		dk = 2*M_PI/ymax;
	}else{
		dk = 2*M_PI/xmax;
	}
	
	ec = 0.5*C_e0; bc = 0.5/C_u0;
	
	for(i=0;i<r;i++){
		for(j=0;j<c;j++){
			ex = Data_get(dataex,i,j); ey = Data_get(dataey,i,j); ez = Data_get(dataez,i,j);
			bx = Data_get(databx,i,j); by = Data_get(databy,i,j); bz = Data_get(databz,i,j);
			
			een = area*ec*(ex*ex+ey*ey+ez*ez);
			ben = area*bc*(bx*bx+by*by+bz*bz);
			ken = area*Data_get(datak,i,j)*Data_get(datad,i,j);
			
			Data_set(dataeen,i,j,een);
			Data_set(databen,i,j,ben);
			Data_set(dataken,i,j,ken);
			Data_set(data,i,j,een+ben+ken);
			Data_set(dataim,i,j,0);
			
			/*	if(i%100==0 && j%100==0){
				printf("bx by bz : %e %e %e\n",bx,by,bz);
				printf("ex ey ez : %e %e %e\n",ex,ey,ez);
				printf("een : %e\n",Data_get(dataeen,i,j));
				printf("ben : %e\n",Data_get(databen,i,j));
				printf("ken : %e\n",Data_get(dataken,i,j));
			}*/
		}
	}
	
	//Data_output(data,file,p_double);
	fft_abs_shift(databen,dataim);
	fft_abs_shift(dataeen,dataim);
	fft_abs_shift(dataken,dataim);
	fft_abs_shift(data,dataim);
	
	profile = radialProfile(databen,dataeen,dataken,data,binc,sigma,dk);
	fp = fopen(file,"w");
	fprintf(fp,"k ben een ken all\n");
	Data_fprint(profile,fp," ");
	fclose(fp);

	LinkedList_deleteRoot(datalist,Data_vdelete);
	
	Data_delete(databen);
	Data_delete(dataeen); 
	Data_delete(dataken);
	Data_delete(data);
	Data_delete(dataim);
	return 0;
}
