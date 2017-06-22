#include "utilities.h"
#include "Data.h"
#include "script.h"

#define C_keV_inv 6.241509e+15
#define C_me 9.10938291e-31
#define SEP " "
void getname(char **name,int mode){
	switch(mode){
		case 0: *name = "dist"; break;
		case 1: *name = "x"; break;
		case 2: *name = "y"; break;
		case 3: *name = "z"; break;
		case 4: *name = "px"; break;
		case 5: *name = "py"; break;
		case 6: *name = "pz"; break;
		default : *name = "energy"; break;
	}
}
void getlegend(char **legend,int mode){
	switch(mode){
		case 0: *legend = "Number [a.u]"; break;
		case 1: *legend = "X [um]"; break;
		case 2: *legend = "Y [um]"; break;
		case 3: *legend = "Z [um]"; break;
		case 4: *legend = "Px [kg m s^-1]"; break;
		case 5: *legend = "Py [kg m s^-1]"; break;
		case 6: *legend = "Pz [kg m s^-1]"; break;
		default : *legend = "Energy [keV]"; break;
	}
}
void fprinthist2D(Data data,FILE *fp, int hbin, int vbin){
	int i,j;
	
	for(i=0;i<hbin;i++){
		for(j=0;j<vbin;j++){
			fprintf(fp,"%e%s",Data_get(data,i*vbin+j,0),SEP);
			fprintf(fp,"%e%s",Data_get(data,i*vbin+j,1),SEP);
			fprintf(fp,"%e%s",Data_get(data,i*vbin+j,2),SEP);
			fprintf(fp,"\n");
		}
		fprintf(fp,"\n");
	}
}
int main(int argc, char *argv[]){
	char filesdf[256],filename[256],specname[256],specname2[256];
	char filegrid[256],filepx[256],filepy[256],filepz[256],fileek[256],filew[256];
	char filehist[256],plotfile[256];
	char command[512],command2[512]; 
	char htag[8],vtag[8],*hname,*vname,*hlegend,*vlegend;
	int mode=0, i,j,n,hbin,vbin,hi,vi,hii=0,vii=0,xf=0,yf=0,zf=0;
	double temp,hmin,hmax,vmin,vmax,xmin,xmax,ymin,ymax,zmin,zmax;
	
	Data datagrid=NULL,datapx=NULL,datapy=NULL,datapz=NULL,dataek=NULL,dataw=NULL,data=NULL,datah=NULL,datav=NULL,hist=NULL;
	FILE *fp;	

	if(argc >= 10){
		strncpy(filesdf,argv[1],256);
		strncpy(specname,argv[2],256);		
		hmin = atof(argv[3]);
		hmax = atof(argv[4]);
		hbin = atoi(argv[5]);
		vmin = atof(argv[6]);
		vmax = atof(argv[7]);
		vbin = atoi(argv[8]);
		mode = atoi(argv[9]);
		if(argc >= 12){
			xmin = atof(argv[10]);
			xmax = atof(argv[11]);
			xf = 1;
			if(argc >= 14){
				ymin = atof(argv[12]);
				ymax = atof(argv[13]);
				yf = 1;
				if(argc >= 16){
					zmin = atof(argv[14]);
					zmax = atof(argv[15]);
					zf = 1;
				}
			}
		}
	}else{
		fprintf(stderr,"usage : %s sdffile specname hmin hmax hbin vmin vmax vbin mode\n\n",argv[0]);
		fprintf(stderr,"hbin,vbin is a horizontal & vertical bin count.\n");
		fprintf(stderr,"mode specifies axis of phasespace.\n");
		fprintf(stderr,"mode is a integer value interpreted as below\n");
		fprintf(stderr,"1D distribution fucntion\n");
		fprintf(stderr,"01 : f(x)\n");
		fprintf(stderr,"02 : f(y)\n");
		fprintf(stderr,"03 : f(z)\n");
		fprintf(stderr,"04 : f(px)\n");
		fprintf(stderr,"05 : f(py)\n");
		fprintf(stderr,"06 : f(pz)\n");
		fprintf(stderr,"07 : f(e)\n");		
		fprintf(stderr,"2D phase space\n");
		fprintf(stderr,"11 : x-x\n");
		fprintf(stderr,"12 : x-y\n");
		fprintf(stderr,"13 : x-z\n");
		fprintf(stderr,"14 : x-px\n");
		fprintf(stderr,"15 : x-py\n");
		fprintf(stderr,"16 : x-pz\n");
		fprintf(stderr,"17 : x-e\n");
		fprintf(stderr,"example\n");
		fprintf(stderr,"25 : y-py\n");
		return 0;
	}

	if(xf){
		fprintf(stderr,"xmin : %f um\n",xmin);
		fprintf(stderr,"xmax : %f um\n",xmax);
	}
	if(yf){
		fprintf(stderr,"ymin : %f um\n",ymin);
		fprintf(stderr,"ymax : %f um\n",ymax);
	}
	if(zf){
		fprintf(stderr,"zmin : %f um\n",zmin);
		fprintf(stderr,"zmax : %f um\n",zmax);
	}

	snprintf(filename,256,"%s",getFileName(filesdf));
	snprintf(command,strlen(filename)-3,"%s",filename);
	snprintf(filename,256,"%s",command);
	for(i=0;;i++){
		if(specname[i]=='/'){
			specname2[i] = '_';
		}else{
			specname2[i] = specname[i];
		}
		if(specname[i]=='\0'){break;}	
	}
	mkdir("phase",0777); snprintf(command,512,"phase/%s",specname2); mkdir(command,0777);
#pragma omp parallel
#pragma omp sections private(command)
{
	#pragma omp section
	{
	snprintf(command,512,"phase/%s/grid",specname2); mkdir(command,0777); 
	snprintf(filegrid,256,"%s/%s.bmat",command,filename);
	if(access(filegrid,F_OK)==-1){
		snprintf(command,512,"sdf2bmat -cni -v grid/%s -f %s %s",specname,filegrid,filesdf);
		fprintf(stderr,"%s\n",command);
		system(command);
	}
	}
	#pragma omp section
	{	
	snprintf(command,512,"phase/%s/px",specname2); mkdir(command,0777); 
	snprintf(filepx,256,"%s/%s.bmat",command,filename);
	if(access(filepx,F_OK)==-1){
		snprintf(command,512,"sdf2bmat -cni -v px/%s -f %s %s",specname,filepx,filesdf);
		fprintf(stderr,"%s\n",command);
		system(command);
	}
	}
	#pragma omp section
	{	
	snprintf(command,512,"phase/%s/py",specname2); mkdir(command,0777); 
	snprintf(filepy,256,"%s/%s.bmat",command,filename);
 	if(access(filepy,F_OK)==-1){
		snprintf(command,512,"sdf2bmat -cni -v py/%s -f %s %s",specname,filepy,filesdf);
		fprintf(stderr,"%s\n",command);
		system(command);
	}
	}
	#pragma omp section
	{
	snprintf(command,512,"phase/%s/pz",specname2); mkdir(command,0777); 
	snprintf(filepz,256,"%s/%s.bmat",command,filename);
	if(access(filepz,F_OK)==-1){
		snprintf(command,512,"sdf2bmat -cni -v pz/%s -f %s %s",specname,filepz,filesdf);
		fprintf(stderr,"%s\n",command);
		system(command);
	}
	}
	#pragma omp section
	{
	snprintf(command,512,"phase/%s/ek",specname2); mkdir(command,0777); 
	snprintf(fileek,256,"%s/%s.bmat",command,filename);
	if(access(fileek,F_OK)==-1){
		snprintf(command,512,"sdf2bmat -cni -v ek/%s -f %s %s",specname,fileek,filesdf);
		fprintf(stderr,"%s\n",command);
		system(command);
	}
	}
	#pragma omp section
	{
	snprintf(command,512,"phase/%s/weight",specname2); mkdir(command,0777); 
	snprintf(filew,256,"%s/%s.bmat",command,filename);
	if(access(filew,F_OK)==-1){
		snprintf(command,512,"sdf2bmat -cni -v weight/%s -f %s %s",specname,filew,filesdf);
		fprintf(stderr,"%s\n",command);
		system(command);
	}
	}
}
	/*
	datagrid = Data_input(filegrid);
	datapx = Data_input(filepx); datapy = Data_input(filepy); datapz = Data_input(filepz);
       dataek = Data_input(fileek); dataw = Data_input(filew);	
	total_energy1 = total_energy2 = 0;
	for(i=0;i<n;i++){
		px = Data_get(datapx,i,0); 
		py = Data_get(datapy,i,0); 
		pz = Data_get(datapz,i,0); 
		we = Data_get(dataw,i,0);
		total_energy1 += (px*px + py*py + pz*pz)/(2*we*C_me);
	}	
	for(i=0;i<n;i++){
		ek = Data_get(dataek,i,0);
		we = Data_get(dataw,i,0);		
		total_energy2 += ek/we;
	}
	
	fprintf(stderr,"total_energy1 : %e\n",total_energy1/n);
	fprintf(stderr,"total_energy2 : %e\n",total_energy2/n);	
	Data_delete(datagrid);
	Data_delete(datapx); Data_delete(datapy); Data_delete(datapz);
	Data_delete(dataek); Data_delete(dataw);
	exit(1);	
	*/
	dataw = Data_input(filew);
	n = Data_getRow(dataw);
	hi = mode/10; vi = mode%10;
	getname(&hname,hi); getname(&vname,vi);
	getlegend(&hlegend,hi); getlegend(&vlegend,vi);
	fprintf(stderr,"%s-%s\n",hname,vname);
	vii = 0; hii = 0;
	snprintf(command,512,"phase/%s/%s-%s",specname2,hname,vname);
	mkdir(command,0777);
	snprintf(filehist,256,"%s/data/%s.txt",command,filename);
	fprintf(stdout,"%s\n",command);
	datagrid = Data_input(filegrid);
	Data_muls(datagrid,1e+06,datagrid);
#pragma omp parallel
{
	if(xf){
		#pragma omp for
		for(i=0;i<n;i++){
			temp = Data_get(datagrid,i,0);
			if(temp < xmin || temp > xmax){
				Data_set(dataw,i,0,0);
			}	
		}
	}	
	if(yf){
		#pragma omp for
		for(i=0;i<n;i++){
			temp = Data_get(datagrid,i,1);
			if(temp < ymin || temp > ymax){
				Data_set(dataw,i,0,0);
			}	
		}

	}
	if(zf){
		#pragma omp for
		for(i=0;i<n;i++){
			temp = Data_get(datagrid,i,2);
			if(temp < zmin || temp > zmax){
				Data_set(dataw,i,0,0);
			}	
		}

	}
}
	switch(hi){
		case 0: break;
		case 1:
		case 2:
		case 3:
			datah = datagrid;
			hii = hi - 1;
			break;
		case 4: datah = datapx ? datapx : (datapx = Data_input(filepx)); 
			break;
		case 5: datah = datapy ? datapy : (datapy = Data_input(filepy));
			break;
		case 6: datah = datapz ? datapz : (datapz = Data_input(filepz));
			break;
		default: datah = dataek ? dataek : (dataek = Data_input(fileek));
			Data_muls(dataek,C_keV_inv,dataek);
			break;
	}
	switch(vi){
		case 0: break;
		case 1:
		case 2:
		case 3:
			datav = datagrid;			
			vii = vi - 1;
			break;
		case 4: datav = datapx ? datapx : (datapx = Data_input(filepx)); 
			break;
		case 5: datav = datapy ? datapy : (datapy = Data_input(filepy));
			break;
		case 6: datav = datapz ? datapz : (datapz = Data_input(filepz));
			break;
		default: datav = dataek ? dataek : (dataek = Data_input(fileek));
			Data_muls(dataek,C_keV_inv,dataek);
			break;
	}
	//fprintf(stderr,"hii : %d, vii : %d\n",hii,vii);
	snprintf(command2,512,"%s/data",command); mkdir(command2,0777);
	snprintf(command2,512,"%s/png",command); mkdir(command2,0777);
	if((fp=fopen(filehist,"w"))==NULL){perror("phase");exit(1);}
	
	if(hi==0){
		data = Data_create(n,2);
		for(i=0;i<n;i++){
			Data_translate(datav,i,vii,data,i,0);
			Data_translate(dataw,i,0,data,i,1);
		}
		if(hmin==-1){hmin = Data_min(data,0);}
		if(hmax==-1){hmax = Data_max(data,0);}
		fprintf(stderr,"hmin : %e, hmax : %e, vbin : %d\n",hmin,hmax,vbin);
		hist = Data_histogram2(data,hmin,hmax,vbin);
		Data_fprint(hist,fp,SEP);
		fclose(fp);
		snprintf(plotfile,256,"%s/plot.sh",command);
		fp = fopen(plotfile,"w");
		fprintf(fp,PLOTDIST,vlegend,hmin,hmax);
		fclose(fp);
		chmod(plotfile,0777);
		Data_delete(datav); datav = NULL;
		
	}else{
		data = Data_create(n,3);
		for(i=0;i<n;i++){
			Data_translate(datah,i,hii,data,i,0);
			Data_translate(datav,i,vii,data,i,1);
			Data_translate(dataw,i,0,data,i,2);
		}
		if(hmin==-1){hmin = Data_min(data,0);}
		if(hmax==-1){hmax = Data_max(data,0);}
		if(vmin==-1){vmin = Data_min(data,1);}
		if(vmax==-1){vmax = Data_max(data,1);}
		fprintf(stderr,"hmin : %e, hmax : %e, hbin : %d\n",hmin,hmax,hbin);
		fprintf(stderr,"vmin : %e, vmax : %e, vbin : %d\n",vmin,vmax,vbin);
		hist = Data_histogram2D2(data,hmin,vmin,hmax,vmax,hbin,vbin);
		fprinthist2D(hist,fp,hbin,vbin);
		fclose(fp);
		snprintf(plotfile,256,"%s/plot.sh",command);
		fp = fopen(plotfile,"w");
		fprintf(fp,PLOTPHASE,hlegend,vlegend,hmin,hmax,vmin,vmax);
		fclose(fp);
		chmod(plotfile,0777);
		Data_delete(datah); datah = NULL;
		Data_delete(datav); datav = NULL;			
	}
	
	Data_delete(data); data = NULL; 
	Data_delete(hist); hist = NULL;
	Data_delete(dataw); dataw = NULL;
	return 0;
}
