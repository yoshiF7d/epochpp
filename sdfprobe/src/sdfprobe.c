#include <stdio.h>
#include "script.h"
#include "Data.h"
#include "LinkedList.h"
#include "Data_loadSDF.h"
//#include <time.h>

#define SEP " "
#define XOFS 1
#define YOFS 3
#define C_q 1.60217657e-19
#define C_pi 3.14159265358973
#define C_c  2.997925e+08
#define C_me 9.109383e-31 
#define N 181
#define N0 90
#define RADIUS 2

#define toev(joule) ((joule)/C_q)
#define todeg(radian) ((radian)*180/C_pi)
/*unit*/
/*energy : kev*/
/*angle : degree*/

/*axis (last argument)*/


	/*row : deg1 -> r*/
	/*column : deg2 -> theta*/
/*
Data topolar(Data data,int nr,int nc){
	int i,j,k,n,ix,iy,ii,jj;
	double r,r0,t,x,y,x0,y0,val;
	Data nd;
	n = Data_getRow(data);
	nd = Data_create(N,N);
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){		
			Data_set(nd,i,j,0);
		}
	}
	printf("n : %d\n",n);
	r0 = ((double)180/(N-1))*RADIUS;

	for(i=0;i<nr;i++){
		for(j=0;j<nc;j++){
			r = Data_get(data,i*nc+j,0);
			t = Data_get(data,i*nc+j,1);
			val = Data_get(data,i*nc+j,2);
			x = r*cos((C_pi*t)/180);
			y = r*sin((C_pi*t)/180);
			ix = ((N0*x/90)+N0);
			iy = (-(N0*y/90)+N0);
			if(ix>=0 && iy>=0 && ix<N && iy<N){
				Data_set(nd,ix,iy,val);
			}	
			
		}
	}
	return nd;
}
*/
/*
for(ii=ix-RADIUS;ii<ix+RADIUS;ii++){
	for(jj=iy-RADIUS;jj<iy+RADIUS;jj++){
		x0 = (double)(ii + 0.5 - N0)*90/N0;
		y0 = -(double)(jj + 0.5 - N0)*90/N0;
		if(ii>=0 && jj>=0 && ii<N && jj<N){
			Data_set(nd,ii,jj,val*exp(-(((x-x0)*(x-x0)+(y-y0)*(y-y0))/(r0*r0))));
		}
	}
}
*/
/*
void fprintangd(Data data, FILE *fp){
	int i,j;
	for(i=0;i<N;i++){
		for(j=0;j<N;j++){
			fprintf(fp,"%e%s",(double)(i + 0.5 - N0)*90/N0,SEP);
			fprintf(fp,"%e%s",-(double)(j + 0.5 - N0)*90/N0,SEP);
			fprintf(fp,"%e\n",Data_get(data,i,j));
		}
		fprintf(fp,"\n");	
	}
}
*/
typedef struct st_string{
	char *buf;
	size_t len;
}String_sub;
typedef String_sub *String;

String String_create(char *buf, size_t len){
	String s;
	s = allocate(sizeof(String_sub));
	s->buf = buf;
	s->len = len;
	return s;
}

void String_delete(String s){
	deallocate(s->buf);
	deallocate(s);	
}

void fprintPolar2D_slow(Data data,FILE *fp, int addbinc, int adebinc){
	int i,j,binc;
	unsigned long len=0;
	size_t needed;
	char *buf,*p;
	String string;
	LinkedList s,list=NULL;
	binc = adebinc*addbinc;
	for(i=0;i<addbinc;i++){
		for(j=0;j<adebinc;j++){
			needed = snprintf(NULL,0,"%e%s%e%s%e%s\n",
				Data_get(data,i*adebinc+j,0),SEP,
				Data_get(data,i*adebinc+j,1),SEP,
				Data_get(data,i*adebinc+j,2),SEP
			);
			buf = allocate(needed+1);
			snprintf(buf,needed+1,"%e%s%e%s%e%s\n",
				Data_get(data,i*adebinc+j,0),SEP,
				Data_get(data,i*adebinc+j,1),SEP,
				Data_get(data,i*adebinc+j,2),SEP
			);
			list = LinkedList_append(list,String_create(buf,needed));
			len += needed;	
		}
	}
	p = buf = callocate(len+1,1);
	for(s=list;s;s=LinkedList_increment(s)){
		string = LinkedList_get(s);
		memcpy(p,string->buf,string->len);	
		String_delete(string);
	}	
	fwrite(p,len+1,1,fp);	
} 

void fprintPolar2D(Data data,FILE *fp, int addbinc, int adebinc){
	int i,j,binc;
	
	binc = adebinc*addbinc;
	for(i=0;i<addbinc;i++){
		fprintf(fp,"%e%s%e%s%e%s\n",
			Data_get(data,i*adebinc,0),SEP,
			-Data_get(data,(i+1)*adebinc-1,1),SEP,
			Data_get(data,(i+1)*adebinc-1,2),SEP
		);
		for(j=0;j<adebinc;j++){
			fprintf(fp,"%e%s",Data_get(data,i*adebinc+j,0),SEP);
			fprintf(fp,"%e%s",Data_get(data,i*adebinc+j,1),SEP);
			fprintf(fp,"%e%s",Data_get(data,i*adebinc+j,2),SEP);
			fprintf(fp,"\n");
			//fprintf(fp,"%e%s",Data_get(data,i*adebinc+j,0),SEP);
			//fprintf(fp,"%e%s",Data_get(data,i*adebinc+j,1),SEP);
			//fprintf(fp,"%e%s",Data_get(data,i*adebinc+j,2),SEP);
			//fprintf(fp,"\n");
		}
		fprintf(fp,"\n");
	}
}

void parse_axis(char *axis,double *theta,double *phi){
	int i;
	double v[3] = {1,0,0};
	char *p,*s;
	for(s=p=axis,i=0;*p;p++){
		switch(*p){
			case '(':
				*p = '\0';
				s = p+1;
				break;
			case ',':
				*p = '\0';
				v[i] = atof(s);
				i++;
				s = p+1;
				break;
			case ')':
				*p = '\0';
				goto forend;
		}
	}
	forend:
	v[i] = atof(s);
	*theta = atan2(sqrt(v[0]*v[0]+v[1]*v[1]),v[2]);
	*phi = atan2(v[1],v[0]);
	printf("axis : ");
	for(i=0;i<3;i++){printf("%f\t",v[i]);}
	printf("\n");
}
/*
void rotate(Data data,double th, double ph){
	int i,n;
	double px1,py1,pz1,px2,py2,pz2;
	double a00,a01,a02,a10,a11,a12,a20,a21,a22;
	a00 = cos(th)*cos(ph); a01 = -sin(th); a02 = cos(th)*sin(ph);
	a10 = cos(ph)*sin(th); a11 = cos(th); a12 = sin(th)*sin(ph);
	a21 = -sin(ph); a21 = 0; a22 = cos(ph);
	
	n = Data_getRow(data);
	for(i=0;i<n;i++){
		px1 = Data_get(data,i,1);
		py1 = Data_get(data,i,2);
		pz1 = Data_get(data,i,3);
		px2 = a00*px1 + a01*py1 + a02*pz1;
		py2 = a10*px1 + a11*py1 + a12*pz1;
		pz2 = a20*px1 + a21*py1 + a22*pz1;
		Data_set(data,i,1,px2);
		Data_set(data,i,2,py2);
		Data_set(data,i,3,pz2);
	}
}
*/

int main(int argc, char *argv[]){
	char filesdf[256];	
	char pname[256];
	char fileespc[256];
	//char filepolar[256];
	char fileaeangd[256];	
	char fileeangd[256];
	char fileiangd[256];	
	char fileeiangd[256];
	
	char filename[256];
	
	char plotespc[256];
	char plotaeangd[256];	
	char ploteangd[256];
	char plotiangd[256];
	char ploteiangd[256];
	
	char command[1024];
	char *straxis=NULL;
	
	int i,j,k,n=0,r,c,esbinc,adebinc,addbinc,piecutn;
	double min,max,px1,py1,pz1,px2,py2,pz2,minp,maxp,th,ph,weight,area,mass;
	double a00,a01,a02,a10,a11,a12,a20,a21,a22;
	Data datapx,datapy,datapz,dataw,data,hist,angd,aeangd,eangd,iangd,eiangd;
	LinkedList namelist=NULL,datalist=NULL;

	FILE *fp,*fp2;

	min = -1;
	max = -1;
	mass = C_me;
	esbinc = 512;
	minp = -1;
	maxp = -1;
	adebinc = 128;
	addbinc = 128;
	area = 1;
	piecutn = 1;	
	th = 0;
	ph = 0;

	//clock_t start,end,s1,e1,s2,e2,s3,e3;

	//start = clock();	
	if(argc >= 3){
		strncpy(filesdf,argv[1],256);
		strncpy(pname,argv[2],256);
	}else{
		printf(
		   "usage : %s [inputfile name] [probe name] [axis : (0,0,1)] [energy min (ev) : auto] [energy max (ev) : auto] [particle mass (kg) : 9.109383e-31]"
		   "[energy bin count : 256] \n" 
		   "[energy min (ev) : auto] [energy max (ev) : auto] [polarbin number : 128] [energybin number : 128] [area : 1] [pie-cutting number : 1]\n",
		   argv[0]
		);
		printf("axis : axis normal to probe plane. x : (1,0,0), y : (0,1,0), z : (0,0,1)\n");
		printf("area : area of probe plane\n");
		printf("pie-cutting number : number of the pie-cutting of the probe plane. (number greater than 1 is only meaningful in 3Di)\n");
		/*
		loadfile(filesdf,256,"input sdf file");
		loadfile(pname,256,"input probe name");
		*/
	}
	if(access(filesdf,F_OK)==-1){fprintf(stderr,"sdf file does not exist\n"); exit(1);}

	snprintf(command,256,"%s",getFileName(filesdf));
	snprintf(filename,strlen(command)-3,"%s",command);

	if(argc >= 4){
		straxis = String_copy(argv[3]);
	}
	if(argc >= 6){
		min = atof(argv[4]);
		max = atof(argv[5]);	
	}
	if(argc >= 7){
		mass = atof(argv[6]);
	}
	if(argc >= 8){
		esbinc = atoi(argv[7]);	
	}
	if(argc >= 10){
		minp = atof(argv[8]);
		maxp = atof(argv[9]);
	}
	if(argc >= 12){
		adebinc = atoi(argv[10]);
		addbinc = atoi(argv[11]);
	}
	if(argc >= 13){
		area = atof(argv[12]);	
	}
	if(argc >= 14){
		piecutn = atoi(argv[13]);
	}
	snprintf(fileespc,256,"%s/espc/data/%s.txt",pname,filename);
	snprintf(fileaeangd,256,"%s/aeangd/data/%s.txt",pname,filename);	
	snprintf(fileeangd,256,"%s/eangd/data/%s.txt",pname,filename);	
	snprintf(fileiangd,256,"%s/iangd/data/%s.txt",pname,filename);	
	
	snprintf(plotespc,256,"%s/espc/plot.sh",pname);
	snprintf(plotaeangd,256,"%s/aeangd/plot.sh",pname);
	snprintf(ploteangd,256,"%s/eangd/plot.sh",pname);
	snprintf(plotiangd,256,"%s/iangd/plot.sh",pname);
	
	snprintf(command,1024,"%s",pname);
	mkdir(command,0777);
	snprintf(command,1024,"%s/espc",pname);
	mkdir(command,0777);
	snprintf(command,1024,"%s/espc/data",pname);
	mkdir(command,0777);
	snprintf(command,1024,"%s/aeangd",pname);
	mkdir(command,0777);
	snprintf(command,1024,"%s/aeangd/data",pname);
	mkdir(command,0777);
	snprintf(command,1024,"%s/eangd",pname);
	mkdir(command,0777);
	snprintf(command,1024,"%s/eangd/data",pname);
	mkdir(command,0777);
	snprintf(command,1024,"%s/iangd",pname);
	mkdir(command,0777);
	snprintf(command,1024,"%s/iangd/data",pname);
	mkdir(command,0777);
	snprintf(command,1024,"%s/eiangd",pname);
	mkdir(command,0777);

	namelist = LinkedList_append(namelist,String_join(pname,"/Px"));
	namelist = LinkedList_append(namelist,String_join(pname,"/Py"));
	namelist = LinkedList_append(namelist,String_join(pname,"/Pz"));
	namelist = LinkedList_append(namelist,String_join(pname,"/weight"));
	
	datalist = Data_loadSDFList2(filesdf,namelist);
		
	/*
	LinkedList_deleteRoot(namelist,deallocate);
	for(list=datalist;list;list=LinkedList_increment(list)){
		data = LinkedList_get(list);
		printf("Row : %d Column : %d\n",Data_getRow(data),Data_getColumn(data));
	}
	*/
	datapx = LinkedList_getIndex(datalist,0);
	datapy = LinkedList_getIndex(datalist,1);
	datapz = LinkedList_getIndex(datalist,2);
	dataw = LinkedList_getIndex(datalist,3);
	
	if(!dataw){fprintf(stderr,"sdfprobe : data load error\n");exit(1);}	
	n = Data_getRow(dataw);	
	printf("particle count : %d\n",n);	
	//end = clock();
	//printf("load time : %lu\n",(end-start));	
	
	parse_axis(straxis,&th,&ph);
	/*set rotation matrix*/
	a00 = cos(th)*cos(ph); a01 = cos(th)*sin(ph); a02 = -sin(th);
	a10 = -sin(ph); a11 = cos(ph); a12 = 0;
	a20 = sin(th)*cos(ph); a21 = sin(th)*sin(ph); a22 = cos(th);
	//printf("%e %e %e\n",a00,a01,a02);
	//printf("%e %e %e\n",a10,a11,a12);
	//printf("%e %e %e\n",a20,a21,a22);
	/*cos(th)*cos(ph) -sin(th) cos(th)*sin(ph)*/
	/*cos(ph)*sin(th) cos(th) sin(th)*sin(ph)*/
	/*-sin(ph) 0 cos(ph)*/
	
	data = Data_create(n,5);
	
	for(i=0;i<n;i++){
		px1 = Data_get(datapx,i,0);
		py1 = Data_get(datapy,i,0);
		pz1 = Data_get(datapz,i,0);
		
		/*rotate coorinates to make pz normal to probe plane*/
		px2 = a00*px1 + a01*py1 + a02*pz1;
		py2 = a10*px1 + a11*py1 + a12*pz1;
		pz2 = a20*px1 + a21*py1 + a22*pz1;
	
		//printf("p1 -> p2 : (%e,%e,%e) -> (%e,%e,%e)\n",px1,py1,pz1,px2,py2,pz2);

		Data_set(data,i,2,px2);
		Data_set(data,i,3,py2);
		Data_set(data,i,4,pz2);
		
		weight = Data_get(dataw,i,0);
		Data_set(data,i,1,weight/area);
		/* E =sqrt( (m*c^2)^2 + (c*p)^2 ) - m*c^2*/
		Data_set(data,i,0,toev(sqrt( (mass*C_c*C_c)*(mass*C_c*C_c) + (px1*px1+py1*py1+pz1*pz1)*C_c*C_c ) - mass*C_c*C_c));
	}
	
	//Data_print(data);
	
	if(min<0){min = Data_min(data,0);}
	if(max<0){max = Data_max(data,0);}
	if(minp<0){minp = (min > 0) ? min : Data_min(data,0);}
	if(maxp<0){maxp = (max > 0) ? max : Data_max(data,0);}
	
	printf("min : %e\n",min);
	printf ("max : %e\n",max);
	printf("minp : %e\n",minp);
	printf ("maxp : %e\n",maxp);	
	printf("area : %g\n",area);
	printf("theta : %f deg\n",th*180/M_PI);
	printf("phi : %f deg\n",ph*180/M_PI);
		
	angd = Data_create(n,3);
	/*make aeangd*/
	//start = clock();
	//s1 = clock();
	//fp = fopen("phth.txt","w");
	//fp2 = fopen("pxpypz.txt","w");
	
	for(i=0;i<n;i++){
		px1 = Data_get(data,i,2);
		py1 = Data_get(data,i,3);
		pz1 = Data_get(data,i,4);
		th=atan2(sqrt(px1*px1+py1*py1),pz1);
		Data_set(angd,i,0,todeg(th)); /*polar angle*/
		Data_set(angd,i,1,todeg(atan2(py1,px1))); /**/
		Data_set(angd,i,2,Data_get(data,i,0)*Data_get(data,i,1)/sin(th)); /*energy * weight / sin(th) */	
		//fprintf(fp2,"%e\t%e\t%e\n",px1,py1,pz1);
		//fprintf(fp,"%e\t%e\n",ph,th);
	}
	//fclose(fp);
	//fclose(fp2);
	
	//e1 = clock();
	//printf("eangd making base data : %lu\n",e1-s1);
	//s2 = clock();
	aeangd = Data_histogram2D(angd,0,-180,90,180,addbinc,addbinc);
	//e2 = clock();
	//printf("eangd making histogram : %lu\n",e2-s2);
	//s3 = clock();
	fp = fopen(fileaeangd,"w");
	if(!fp){perror(fileaeangd); exit(1);}
	fprintPolar2D(aeangd,fp,addbinc,addbinc);
	fclose(fp);
	//e3 = clock();
	//printf("eangd making output file : %lu\n",e3-s3);
	//end = clock();
	//printf("eangd making time : %lu\n",end-start);
		
	/*make iangd*/
	//start = clock();
	for(i=0;i<n;i++){
		th = M_PI*Data_get(angd,i,0)/180;
		Data_set(angd,i,2,Data_get(data,i,1)/sin(th)); /*weight / sin(th)*/
	}

	iangd = Data_histogram2D(angd,0,-180,90,180,addbinc,addbinc);
	fp = fopen(fileiangd,"w");
	if(!fp){perror(fileiangd); exit(1);}
	fprintPolar2D(iangd,fp,addbinc,addbinc);
	fclose(fp);
	//end = clock();
	//printf("iangd making time : %lu\n",end-start);
	/*make eangd*/
	for(i=0;i<n;i++){
		Data_translate(data,i,0,angd,i,0);
		Data_translate(data,i,1,angd,i,2);
	}
	eangd = Data_histogram2D(angd,minp,-180,maxp,180,adebinc,addbinc); 
	fp = fopen(fileeangd,"w");
	if(!fp){perror(fileeangd); exit(1);}
	fprintPolar2D(eangd,fp,adebinc,addbinc);
	fclose(fp);
	/*make eiangd*/
	//start = clock();
	for(k=0;k<piecutn;k++){
		ph = k*M_PI/piecutn;
		/*rotate around the z axis*/
		a00 = cos(ph); a01 = sin(ph);
		//a10 = -sin(ph); a11 = cos(ph);	
		for(i=0;i<n;i++){
			px1 = Data_get(data,i,2);
			py1 = Data_get(data,i,3);
			pz1 = Data_get(data,i,4);
			
			px2 = a00*px1 + a01*py1;
			//py2 = a10*px1 + a11*py1;
			th = atan2(px2,pz1);
			//printf("momentum : %e\t%e\t%e\n",px1,py1,pz1);
			//printf("ph : %f\n",todeg(ph));
			//printf("th : %f\n",todeg(th));
			//printf("energy : %e\n",Data_get(data,i,0));
			Data_set(angd,i,0,todeg(th));
			Data_translate(data,i,0,angd,i,1); /*energy*/
			Data_set(angd,i,2,Data_get(data,i,1)); /*weight*/
		}
		eiangd = Data_histogram2D(angd,-90,minp,90,maxp,addbinc,adebinc);  
		
		snprintf(command,1024,"%s/eiangd/%ddeg",pname,180*k/piecutn);
		mkdir(command,0777);
		snprintf(command,1024,"%s/eiangd/%ddeg/data",pname,180*k/piecutn);
		mkdir(command,0777);
		
		snprintf(fileeiangd,256,"%s/eiangd/%ddeg/data/%s.txt",pname,180*k/piecutn,filename);	
		fp = fopen(fileeiangd,"w");
		if(!fp){perror(fileeiangd); exit(1);}
		fprintPolar2D(eiangd,fp,addbinc,adebinc);
		fclose(fp);
		
		snprintf(ploteiangd,256,"%s/eiangd/%ddeg/plot.sh",pname,180*k/piecutn);
	 	fp = fopen(ploteiangd,"w");		
		if(!fp){perror(ploteiangd); exit(1);}
		fprintf(fp,PLOTPOLAR2D,minp,maxp);
		fclose(fp);
		chmod(ploteiangd,0777);
	}
	//end = clock();
	//printf("eiang making time : %lu\n",end-start);
	/*make espc*/	
	//start = clock();
	hist = Data_histogram(data,min,max,esbinc);
	fp = fopen(fileespc,"w");
	if(!fp){perror(fileespc); exit(1);}
	Data_fprint(hist,fp,SEP);
	fclose(fp);
	//end = clock();
	//printf("espc making time : %lu\n",end-start);	

	//start = clock();	
	fp = fopen(plotespc,"w");
	if(!fp){perror(plotespc); exit(1);}
	fprintf(fp,PLOTESPC,min,max);
	fclose(fp);
	chmod(plotespc,0777);
		
	fp = fopen(plotaeangd,"w");
	if(!fp){perror(plotaeangd); exit(1);}
	fprintf(fp,PLOTPOLAR,"Angle [degree]",-90.0,90.0,-90.0,90.0);
	fclose(fp);
	chmod(plotaeangd,0777);

	fp = fopen(ploteangd,"w");
	if(!fp){perror(ploteangd); exit(1);}
	fprintf(fp,PLOTPOLAR,"Energy [eV]",-maxp,maxp,-maxp,maxp);
	fclose(fp);
	chmod(ploteangd,0777);

	fp = fopen(plotiangd,"w");
	if(!fp){perror(plotiangd); exit(1);}
	fprintf(fp,PLOTPOLAR,"Angle [degree]",-90.0,90.0,-90.0,90.0);
	fclose(fp);
	chmod(plotiangd,0777);

	//end = clock();
	//printf("plot file making time : %lu\n",end-start);	
	/*
	Data_delete(datapx);
	Data_delete(datapy);
	Data_delete(datapz);
	Data_delete(dataw);
	*/
	LinkedList_deleteRoot(datalist,Data_vdelete);
	LinkedList_deleteRoot(namelist,deallocate);
	Data_delete(data);
	Data_delete(hist);
	Data_delete(angd);
	Data_delete(aeangd);
	Data_delete(eangd);
	Data_delete(iangd);
	Data_delete(eiangd);
	return 0;
}
