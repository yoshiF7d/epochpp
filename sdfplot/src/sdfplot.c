#include "utilities.h"
#include "LinkedList.h"
#include "Data.h"
#include "Data_loadSDF.h"
#include <dirent.h>

int compare(void *c1, void *c2);
void Data_pngout(Data data,char *dirout,char *index,char *comfile);

int main(int argc, char *argv[]){
	DIR *dp=NULL;
	struct dirent *entry;
	struct stat statbuf;

	char *dirsdf,*variable,*dirout=NULL,*comfile=NULL,*filename,index[5]={0,0,0,0,0};
	int skip=1;
	double multiplier = 1.0;
	
	LinkedList s,list=NULL;
	Data data;
	
	if(argc < 3){
		printf(
			   "usage : %s [inputdir name]"
			   " [variable]"
			   " [outputdir name : (variable)]"
			   " [gnuplot command file : (default)]",
			   " [skip : 1]"
			   " [multiplier : 1]\n",	   
			   argv[0]
			   );
		exit(0);
	}else{
		dirsdf = String_copy(argv[1]);
		variable = String_copy(argv[2]);
	}
	if(argc >= 4){
		dirout = String_copy(argv[3]);
	}else{
		dirout = String_copy(variable);
	}
	if(argc >= 5){
		comfile = String_copy(argv[4]);
	}
	if(argc >= 6){
		skip = atoi(argv[5]);
	}
	if(argc >= 7){
		multiplier = atof(argv[6]);
	}
	dp = opendir(dirsdf);	
	if(!dp){perror("sdfplot");exit(1);} 
	while((entry=readdir(dp))!=NULL){
		
		if(!strcmp(".sdf",getFileExtension(entry->d_name))){
			list = LinkedList_append(list,String_copy(entry->d_name));
		}
	}
	closedir(dp);
	mkdir(dirout,0777);
	list = LinkedList_sort(list,compare);
	for(s=list;s;s=s->next){
		filename = LinkedList_get(s);
		printf("filename : %s\n",filename);
		snprintf(index,5,"%s",filename);
		index[4]='\0';
		data=Data_loadSDF(filename,variable);
		Data_muls(data,multiplier,data);
		Data_pngout(data,dirout,index,comfile);
		Data_delete(data);
	}
	LinkedList_deleteRoot(list,deallocate);
	deallocate(dirsdf);
	deallocate(dirout);
	deallocate(variable);
	deallocate(comfile);
	return 0;
}

int compare(void *c1, void *c2){
	return atoi((char*)c1)-atoi((char*)c2);
}

void Data_pngout(Data data,char *dirout,char *index,char *comfile){
	FILE *gp;
	double x;
	int i,j;
	
	gp = popen("gnuplot","w");
	if(gp==NULL){perror("Data_gnuplot"); exit(1);}
	fprintf(gp,"set term png size %d,%d\n",data->column,data->row);
	fprintf(gp,"set xr[%d:%d]\n",0,data->column);
	fprintf(gp,"set yr[%d:%d]\n",0,data->row);
	fprintf(gp,"set size ratio %f\n",(double)(data->row)/(data->column));
	if(comfile){fprintf(gp,"load \"%s\"\n",comfile);}
	fprintf(gp,"set output \"%s/%s.png\"\n",dirout,index);
	fprintf(gp,
			"plot '-' binary array=(%d,%d) format=\"%%float64\""
			"endian=little w image notitle\n",
			data->column,data->row
	);
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column;j++){
			x = Data_get(data,i,j);
			fwrite(&x,sizeof(double),1,gp);
		}
	}
	fflush(gp);
	pclose(gp);
}
