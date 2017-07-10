#include "utilities.h"
#include "Data.h"
#include "LinkedList.h"
#include "Data_loadSDF.h"

#define C_e0 8.854187e-12
#define C_u0 1.256637e-06
#define PNG_X_SIZE 750
#define PNG_Y_SIZE 2250
/*een : total electric field energy per grid*/
/*ben : total magnetic field energy per grid*/
/*ken : total kinetic energy per grid*/


int main(int argc, char *argv[]){
	char filesdf[256];	
	char filename[256];	
	
	char filebx[256], fileby[256], filebz[256];
	char fileex[256], fileey[256], fileez[256];
	char fileben[256], fileeen[256], fileken[256];
	
	char filek[256], filed[256];
	char dirpng[256];
		
	char command[1024];
	
	int i,j,n=0,r,c;
	double ec,bc,bx,by,bz,ex,ey,ez;
	double area,emin,emax,ben,een,ken;
	double x;
	Data databx,databy,databz,dataex,dataey,dataez,datak,datad,databen,dataeen,dataken;
	LinkedList namelist,datalist;
	FILE *gp;


	if(argc < 2){
		printf("usage : %s [inputfile name] [energy min (ev) : auto] [energy max (ev) : auto] [area of cell (m^2)]\n",argv[0]);
	}else{
		strncpy(filesdf,argv[1],256);
	}
	
	emin = -1;
	emax = -1;
	area = 1;
	
	
	if(argc >= 4){
		emin = atof(argv[2]);
		emax = atof(argv[3]);	
	}	

	if(argc >= 5){
		area = atof(argv[4]);
	}
	
	if(access(filesdf,F_OK)==-1){
		fprintf(stderr,"sdf file does not exist\n"); exit(1);
	}
	snprintf(filename,256,"%s",getFileName(filesdf));
	snprintf(command,strlen(filename)-3,"%s",filename);
	sprintf(filename,"%s",command);	

	snprintf(fileben,256,"en/ben/bmat/%s.bmat",filename);
	snprintf(fileeen,256,"en/een/bmat/%s.bmat",filename);
	snprintf(fileken,256,"en/ken/bmat/%s.bmat",filename);
	
	mkdir("en",0777);
	mkdir("en/ben",0777); mkdir("en/een",0777); mkdir("en/ken",0777);
	mkdir("en/ben/bmat",0777); mkdir("en/een/bmat",0777); mkdir("en/ken/bmat",0777); 
	mkdir("en/ben/png",0777); mkdir("en/een/png",0777); mkdir("en/ken/png",0777);

	namelist = NULL;
	namelist = LinkedList_append(namelist,String_copy("ex"));
	namelist = LinkedList_append(namelist,String_copy("ey"));
	namelist = LinkedList_append(namelist,String_copy("ez"));
	
	namelist = LinkedList_append(namelist,String_copy("bx"));
	namelist = LinkedList_append(namelist,String_copy("by"));
	namelist = LinkedList_append(namelist,String_copy("bz"));
	
	namelist = LinkedList_append(namelist,String_copy("number_density"));
	namelist = LinkedList_append(namelist,String_copy("ekbar"));
	
	datalist = Data_loadSDFList(filesdf,namelist);
	
	dataex = LinkedList_getIndex(datalist,0);
	dataey = LinkedList_getIndex(datalist,1);
	dataez = LinkedList_getIndex(datalist,2);
	databx = LinkedList_getIndex(datalist,3);
	databy = LinkedList_getIndex(datalist,4);
	databz = LinkedList_getIndex(datalist,5);
	datad = LinkedList_getIndex(datalist,6);
	datak = LinkedList_getIndex(datalist,7);
	
	r = Data_getRow(datak); 
	c = Data_getColumn(datak);

	databen = Data_create(r,c); 
	dataeen = Data_create(r,c);
	dataken = Data_create(r,c);
	
	ec = 0.5*C_e0; bc = 0.5/C_u0;
	
	for(i=0;i<r;i++){
		for(j=0;j<c;j++){
			ex = Data_get(dataex,i,j); ey = Data_get(dataey,i,j); ez = Data_get(dataez,i,j);
			bx = Data_get(databx,i,j); by = Data_get(databy,i,j); bz = Data_get(databz,i,j);
				
			Data_set(dataeen,i,j,area*ec*(ex*ex+ey*ey+ez*ez));
			Data_set(databen,i,j,area*bc*(bx*bx+by*by+bz*bz));
			Data_set(dataken,i,j,area*Data_get(datak,i,j)*Data_get(datad,i,j));
			/*
			if(i%100==0 && j%100==0){
				printf("bx by bz : %e %e %e\n",bx,by,bz);
				printf("ex ey ez : %e %e %e\n",ex,ey,ez);
				printf("een : %e\n",Data_get(dataeen,i,j));
				printf("ben : %e\n",Data_get(databen,i,j));
				printf("ken : %e\n",Data_get(dataken,i,j));
			}*/
		}
	}
	
	Data_output(databen,fileben,p_float); 
	Data_output(dataeen,fileeen,p_float); 
	Data_output(dataken,fileken,p_float);

	if(emin<0){
		emin = Data_get(databen,0,0);
		for(i=0;i<r;i++){
			for(j=0;j<c;j++){
				ben = Data_get(databen,i,j);
				een = Data_get(dataeen,i,j);
				ken = Data_get(dataken,i,j);
				if(emin > ben){emin = ben;}
				if(emin > een){emin = een;}
				if(emin > ken){emin = ken;}
			}
		}
	}

	if(emax<0){
		emax = Data_get(databen,0,0);
		for(i=0;i<r;i++){
			for(j=0;j<c;j++){
				ben = Data_get(databen,i,j);
				een = Data_get(dataeen,i,j);
				ken = Data_get(dataken,i,j);
				if(emax < ben){emax = ben;}
				if(emax < een){emax = een;}
				if(emax < ken){emax = ken;}
			}
		}
	}		
	
	gp = popen("gnuplot -persist\n","w");
	if(gp==NULL){perror("gnuplot"); exit(1);}
	fprintf(gp,"set term png size %d,%d\n",c,r);
	fprintf(gp,"set cbr[%e:%e]\n",emin,emax);
	fprintf(gp,"set xr[%d:%d]\n",0,c);
	fprintf(gp,"set yr[%d:%d]\n",0,r);
	fprintf(gp,"set size ratio %f\n",(double)r/c);	
	fprintf(gp,"set output \"en/ben/png/%s.png\"\n",filename);
	fprintf(gp,"plot '-' binary array=(%d,%d) format=\"%%float64\" endian=little w image notitle\n",c,r);
	for(i=0;i<r;i++){
		for(j=0;j<c;j++){
			x = Data_get(databen,i,j);
			fwrite(&x,sizeof(double),1,gp);
		}
	}
	fprintf(gp,"set output \"en/een/png/%s.png\"\n",filename);
	fprintf(gp,"plot '-' binary array=(%d,%d) format=\"%%float64\" endian=little w image notitle\n",c,r);
	for(i=0;i<r;i++){
		for(j=0;j<c;j++){
			x = Data_get(dataeen,i,j);
			fwrite(&x,sizeof(double),1,gp);
		}
	}
	fprintf(gp,"set output \"en/ken/png/%s.png\"\n",filename);
	fprintf(gp,"plot '-' binary array=(%d,%d) format=\"%%float64\" endian=little w image notitle\n",c,r);
	for(i=0;i<r;i++){
		for(j=0;j<c;j++){
			x = Data_get(dataken,i,j);
			fwrite(&x,sizeof(double),1,gp);
		}
	}
	LinkedList_deleteRoot(datalist,Data_vdelete);
	
	Data_delete(databen);
	Data_delete(dataeen); 
	Data_delete(dataken);
	
	return 0;
}
