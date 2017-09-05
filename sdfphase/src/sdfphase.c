#include "utilities.h"
#include "Data.h"
#include "Data_loadSDF.h"
#include "LinkedList.h"
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
void fprinthist1D(Data data, FILE *fp, int hbin, int vbin){
	int i,j;
	double sum;	
	for(i=0;i<hbin;i++){
		fprintf(fp,"%e%s",Data_get(data,i*vbin,0),SEP);
		sum=0;
		for(j=0;j<vbin;j++){
			sum += Data_get(data,i*vbin+j,1)*Data_get(data,i*vbin+j,2);
		}
		fprintf(fp,"%e%s",sum,SEP);
		fprintf(fp,"\n");		
	}
}

int main(int argc, char *argv[]){
	char filesdf[256],filename[256],specname[256],specname2[256];
	char filehist[256],filehist2[256],plotfile[256];
	char command[512],command2[512]; 
	char htag[8],vtag[8],*hname,*vname,*hlegend,*vlegend;
	int mode=0, i,j,n,hbin,vbin,hi,vi,hii=0,vii=0;
	double total_energy1, total_energy2,px,py,pz,ek,we,hmin,hmax,vmin,vmax,wmin,wmax,temp;
	
	Data datagrid=NULL,datapx=NULL,datapy=NULL,datapz=NULL,dataek=NULL,dataw=NULL,data=NULL,datah=NULL,datav=NULL,hist=NULL;
	LinkedList datalist,namelist;
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
	
	namelist = NULL;
	namelist = LinkedList_append(namelist,String_join("grid/",specname));
	namelist = LinkedList_append(namelist,String_join("px/",specname));
	namelist = LinkedList_append(namelist,String_join("py/",specname));
	namelist = LinkedList_append(namelist,String_join("pz/",specname));
	namelist = LinkedList_append(namelist,String_join("ek/",specname));
	namelist = LinkedList_append(namelist,String_join("weight/",specname));
	
	datalist = Data_loadSDFList2(filesdf,namelist);
	datagrid = LinkedList_getIndex(datalist,0);
	datapx = LinkedList_getIndex(datalist,1);
	datapy = LinkedList_getIndex(datalist,2);
	datapz = LinkedList_getIndex(datalist,3);
	dataek = LinkedList_getIndex(datalist,4);
	dataw = LinkedList_getIndex(datalist,5);

	/*
	snprintf(command,512,"phase/%s/weight",specname2); mkdir(command,0777); 
	snprintf(filew,256,"%s/%s.bmat",command,filename);
	if(access(filew,F_OK)==-1){
		snprintf(command,512,"sdf2bmat -cni -v weight/%s -f %s %s",specname,filew,filesdf);
		fprintf(stderr,"%s\n",command);
		system(command);
	}
	*/
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
	n = Data_getRow(dataw);
	hi = mode/10; vi = mode%10;
	//fprintf(stderr,"hi : %d, vi : %d\n",hi,vi);
	getname(&hname,hi); getname(&vname,vi);
	getlegend(&hlegend,hi); getlegend(&vlegend,vi);
	fprintf(stderr,"%s-%s\n",hname,vname);
	
	vii = 0; hii = 0;
	snprintf(command,512,"phase/%s/%s-%s",specname2,hname,vname);
	mkdir(command,0777); chmod(command,0777);
	snprintf(filehist,256,"%s/data/%s.txt",command,filename);
	snprintf(filehist2,256,"%s/data2/%s.txt",command,filename);
	fprintf(stdout,"%s",command);
	switch(hi){
		case 0: break;
		case 1:
		case 2:
		case 3:
			datah = datagrid;
			hii = hi - 1;
			break;
		case 4: datah = datapx; 
			break;
		case 5: datah = datapy;
			break;
		case 6: datah = datapz;
			break;
		default: datah = dataek;
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
		case 4: datav = datapx; 
			break;
		case 5: datav = datapy;
			break;
		case 6: datav = datapz;
			break;
		default: datav = dataek;
			Data_muls(dataek,C_keV_inv,dataek);
			break;
	}
	//fprintf(stderr,"hii : %d, vii : %d\n",hii,vii);
	snprintf(command2,512,"%s/data",command); mkdir(command2,0777); chmod(command2,0777);
	snprintf(command2,512,"%s/data2",command); mkdir(command2,0777); chmod(command2,0777);
	snprintf(command2,512,"%s/png",command); mkdir(command2,0777); chmod(command2,0777);
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
		hist = Data_histogram(data,hmin,hmax,vbin);
		wmin = Data_get(hist,0,1);
		wmax = Data_get(hist,0,1);
		for(i=0;i<Data_getRow(hist);i++){
			temp = Data_get(hist,i,1); 
			if(wmin > temp){wmin = temp;}
			if(wmax < temp){wmax = temp;}
		}
		Data_fprint(hist,fp,SEP);
		fclose(fp);
		snprintf(plotfile,256,"%s/plot.sh",command);
		fp = fopen(plotfile,"w");
		fprintf(fp,PLOTDIST,vlegend,hmin,hmax,wmin,wmax);
		fclose(fp);
		chmod(plotfile,0777);
		
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
		hist = Data_histogram2D(data,hmin,vmin,hmax,vmax,hbin,vbin);
		wmin = Data_get(hist,0,2);
		wmax = Data_get(hist,0,2);
		for(i=0;i<Data_getRow(hist);i++){
			temp = Data_get(hist,i,2); 
			if(wmin > temp){wmin = temp;}
			if(wmax < temp){wmax = temp;}
		}
		fprinthist2D(hist,fp,hbin,vbin);
		fclose(fp);
		fp = fopen(filehist2,"w");
		fprinthist1D(hist,fp,hbin,vbin);
		fclose(fp);
		snprintf(plotfile,256,"%s/plot.sh",command);
		fp = fopen(plotfile,"w");
		fprintf(fp,PLOTPHASE,hlegend,vlegend,hmin,hmax,vmin,vmax,wmin,wmax);
		fclose(fp);
		chmod(plotfile,0777);
	}
	Data_delete(data); data = NULL; 
	Data_delete(hist); hist = NULL;
	LinkedList_deleteRoot(datalist,Data_vdelete);
	LinkedList_deleteRoot(namelist,deallocate);
	return 0;
}
