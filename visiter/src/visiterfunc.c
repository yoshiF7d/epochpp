#include "visiterfunc.h"
//#include "LeakDetector.h"

#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>

/*
typedef struct st_funccontainer{
	Func func;
}FuncContainer_Sub;

FuncContainer FuncContainer_create(Func func){
	FuncContainer fc = allocate(sizeof(FuncContainer_Sub));
	fc->func = func;
	return fc;
}

char *(*FuncContainer_get(FuncContainer fc))(LinkedList list){
	return fc->func;
}
void FuncContainer_delete(void *fc){deallocate(fc);}
*/

VisiterInfo VisiterInfo_create(HashTable ht, Data data, char *file){
	VisiterInfo vi;
	vi = allocate(sizeof(VisiterInfo_Sub));
	vi->hashtable = ht;
	vi->data = data;
	vi->filename = file;
	ReadOptions_init(&(vi->options));
	ReadOptions_setLineSeparators(&(vi->options),3,"\r","\n","\r\n");
	ReadOptions_setFieldSeparators(&(vi->options),3," ","\t",",");
	vi->series=0;
	return vi;
}

void VisiterInfo_setHashTable(VisiterInfo vi, HashTable ht){vi->hashtable = ht;}
void VisiterInfo_setData(VisiterInfo vi, Data data){vi->data = data;}
void VisiterInfo_setFileName(VisiterInfo vi, char *file){vi->filename = file;}
void VisiterInfo_setSeriesOn(VisiterInfo vi){vi->series = 1;}
void VisiterInfo_setSeriesOff(VisiterInfo vi){vi->series = 0;}

void VisiterInfo_setLineSeparators(VisiterInfo vi,int n, char **separators){
	int i;
	for(i=0;i<vi->options.lineSeparatorsCount;i++){deallocate(vi->options.lineSeparators[i]);}
	deallocate(vi->options.lineSeparators);
	vi->options.lineSeparators = separators;
	vi->options.lineSeparatorsCount = n;
}
void VisiterInfo_setFieldSeparators(VisiterInfo vi,int n, char **separators){
	int i;
	for(i=0;i<vi->options.fieldSeparatorsCount;i++){deallocate(vi->options.fieldSeparators[i]);}
	deallocate(vi->options.fieldSeparators);
	vi->options.fieldSeparators = separators;
	vi->options.fieldSeparatorsCount = n;
}
StringList StringList_splitString(char *string, char **separators, int separatorsCount,int repeatedSeparators);

char **parseSeparators(char *string, int *length){
	StringList list,s;
	char **separators,**rtn;
	separators = allocate(sizeof(char*));
	separators[0] = ",";
	list = StringList_splitString(string,separators,1,1);
	for(s=list;s;s=s->next){
		s->content = String_copy(s->content);
	}
	for(s=list;s;s=s->next){
		if(!strcmp((char*)(s->content),"\\n")){
			deallocate(s->content);
			s->content = String_copy("\n");
		}if(!strcmp((char*)(s->content),"\\t")){
			deallocate(s->content);
			s->content = String_copy("\t");
		}if(!strcmp((char*)(s->content),"\\r")){
			deallocate(s->content);
			s->content = String_copy("\r");
		}
	}
	*length = LinkedList_length(list);
	rtn = StringList_toArray(list);
	deallocate(separators);
	return rtn;
}

HashTable VisiterInfo_getHashTable(VisiterInfo vi){return vi->hashtable;}
Data VisiterInfo_getData(VisiterInfo vi){return vi->data;}
char *VisiterInfo_getFileName(VisiterInfo vi){return vi->filename;}

unsigned char VisiterInfo_isSeriesOn(VisiterInfo vi){return vi->series;}

char *VisiterInfo_printprompt(VisiterInfo vi,char *str,int len){
	if(vi->data){snprintf(str,len,"file : %s, row : %d, column : %d >",getFileName(vi->filename),Data_getRow(vi->data),Data_getColumn(vi->data));}
	else{snprintf(str,len,"no data >");}
	return str;
}
void VisiterInfo_print(VisiterInfo vi){
	if(vi->data){
		printf("file : %s, row : %d, column : %d\n",getFileName(vi->filename),Data_getRow(vi->data),Data_getColumn(vi->data));
		ReadOptions_print(&(vi->options));
	}else{printf("no data\n");}
}
void VisiterInfo_delete(VisiterInfo vi){
	ReadOptions_finish(&(vi->options));
	deallocate(vi);
}

int check(Data data, int rs, int cs, int re, int ce){
	int r,c;
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return 0;}
	r = Data_getRow(data); c = Data_getColumn(data);
	if(r <= rs){printf("index error. rowmax (%d) < rs (%d)\n",r-1,rs); return 0;}
	if(rs < 0){printf("index error. rs (%d) < 0\n", rs); return 0;}
	if(r <= re){printf("index error. rowmax (%d) < re (%d)\n",r-1,re); return 0;}
	if(re < 0){printf("index error. re (%d) < 0\n", re); return 0;}
	if(c <= cs){printf("index error. columnmax (%d) < cs (%d)\n",c-1,cs); return 0;}
	if(rs < 0){printf("index error. cs (%d) < 0\n", cs); return 0;}
	if(c <= ce){printf("index error. columnmax (%d) <= ce (%d)\n",c-1,ce); return 0;}
	if(re < 0){printf("index error. re (%d) < 0\n", re); return 0;}	
	if(rs > re){printf("rs (%d) must be smaller than re (%d)\n",rs,re); return 0;} 
	if(cs > ce){printf("cs (%d) must be smaller than ce (%d)\n",cs,ce); return 0;}
	return 1;
}
#define loadargs() {\
	rs = atoi((char*)LinkedList_getIndex(list,1));\
	cs = atoi((char*)LinkedList_getIndex(list,2));\
	re = atoi((char*)LinkedList_getIndex(list,3));\
	ce = atoi((char*)LinkedList_getIndex(list,4));\
}

#define PROCESSARGS(func) {\
	len = LinkedList_getLength(list);\
	if(len != 5 && len != 1 && len != 3 &&len != 2 && len != 6){\
		printdoc_imp(func); return;\
	}\
	vi = LinkedList_getIndex(list,0); data = VisiterInfo_getData(vi);\
	if(len==1){\
		if(data){rs=0;cs=0;re=Data_getRow(data)-1;ce=Data_getColumn(data)-1;}\
	}else if(len==2){\
		if(data){rs=0;cs=0;re=Data_getRow(data)-1;ce=Data_getColumn(data)-1;}\
		if(VisiterInfo_isSeriesOn(vi)){if((fp = fopen(String_stripdq(LinkedList_getIndex(list,1)),"a"))==NULL){return;}}\
		else{fp = wopen(String_stripdq(LinkedList_getIndex(list,1)));}\
	}else if(len==3){\
		if(data){rs=0;cs=0;re=Data_getRow(data)-1;ce=Data_getColumn(data)-1;}\
		if(VisiterInfo_isSeriesOn(vi)){if((fp = fopen(String_stripdq(LinkedList_getIndex(list,1)),"a"))==NULL){return;}}\
		else{fp = wopen(String_stripdq(LinkedList_getIndex(list,1)));}\
		step = atoi(LinkedList_getIndex(list,2));\
	}else{loadargs();}\
	if(len==6){\
		if(VisiterInfo_isSeriesOn(vi)){if((fp = fopen(String_stripdq(LinkedList_getIndex(list,5)),"a"))==NULL){return;}}\
		else{fp = wopen(String_stripdq(LinkedList_getIndex(list,5)));}\
	}\
	if(re<0){re += Data_getRow(data);}\
	if(ce<0){ce += Data_getColumn(data);}\
	if(!check(data,rs,cs,re,ce)){return;}\
	if(VisiterInfo_isSeriesOn(vi)){fprintf(fp,"%s\t",getFileName(VisiterInfo_getFileName(vi)));}\
}

void visiter_ave(LinkedList list){
	int i,j,len,n=0;
	int rs=0,cs=0,re=0,ce=0,step=1;
	double ave=0;
	FILE *fp=stdout;
	
	VisiterInfo vi=NULL;
	Data data=NULL;
	
	PROCESSARGS(visiter_ave);
	
	for(i=rs;i<=re;i++){
		for(j=cs;j<=ce;j++){
			if((i*(ce-cs+1)+j)%step==0){ave += Data_get(data,i,j); n++;}
		}
	}
	ave /= n;
	fprintf(fp,"%1.6e\n",ave);
	if(len == 6 || len == 2 || len == 3){fclose(fp);}
}

void visiter_sum(LinkedList list){
	int i,j,len;
	int rs=0,cs=0,re=0,ce=0,step=1;
	double sum=0;
	FILE *fp=stdout;
	
	VisiterInfo vi=NULL;
	Data data=NULL;
	
	PROCESSARGS(visiter_sum);
	
	for(i=rs;i<=re;i++){
		for(j=cs;j<=ce;j++){
			if((i*(ce-cs+1)+j)%step==0){sum += Data_get(data,i,j);}
		}
	}
	fprintf(fp,"%1.6e\n",sum);
	if(len == 6 || len == 2 || len == 3){fclose(fp);}
}
void visiter_min(LinkedList list){
	int i,j,len,rmin=0,cmin=0;
	int rs=0,cs=0,re=0,ce=0,step=1;
	double min;
	FILE *fp=stdout;
	
	VisiterInfo vi=NULL;
	Data data=NULL;
	
	PROCESSARGS(visiter_min);
	
	min = Data_get(data,rs,cs);
	for(i=rs;i<=re;i++){
		for(j=cs;j<=ce;j++){
			if(Data_get(data,i,j) < min && ((i*(ce-cs+1)+j)%step==0)){min = Data_get(data,i,j); rmin = i; cmin = j;} 
		}
	}
	fprintf(fp,"%1.6e\t%d\t%d\n",min,rmin,cmin); 
	if(len == 6 || len == 2 || len == 3){fclose(fp);}
}

void visiter_max(LinkedList list){
	int i,j,len,rmax=0,cmax=0;
	int rs=0,cs=0,re=0,ce=0,step=1;
	double max;
	FILE *fp=stdout;
	
	VisiterInfo vi=NULL;
	Data data=NULL;

	PROCESSARGS(visiter_max);
	
	max = Data_get(data,rs,cs);
	for(i=rs;i<=re;i++){
		for(j=cs;j<=ce;j++){
			if(max < Data_get(data,i,j) && ((i*(ce-cs+1)+j)%step==0)){max = Data_get(data,i,j); rmax =i; cmax = j;}
		}
	}
	fprintf(fp,"%1.6e\t%d\t%d\n",max,rmax,cmax);
	if(len == 6 || len == 2 || len == 3){fclose(fp);}
}

void visiter_print(LinkedList list){
	int i,j,len;
	int rs=0,cs=0,re=0,ce=0,step=0;
	double max;
	FILE *fp = stdout;
	
	VisiterInfo vi=NULL;	
	Data data=NULL;
	
	len = LinkedList_getLength(list);

	PROCESSARGS(visiter_print);

	for(i=rs;i<=re;i++){
		for(j=cs;j<=ce;j++){
			fprintf(fp,"%1.6e%s",Data_get(data,i,j),vi->options.fieldSeparators[0]);
		}
		fprintf(fp,"\n");
		if(step){
			if((i*(ce-cs+1)+j)%step==0){fprintf(fp,"\n\n");}
		}
	}
	if(len == 6 || len == 2 || len == 3){fclose(fp);}
}

void visiter_plot(LinkedList list){visiter_draw(list);}
void visiter_draw(LinkedList list){
	int i,j,len;
	int rs=0,cs=0,re=0,ce=0;
	double ratio,x;
	int r,c;
	char *comfile=NULL;
	FILE *gp=NULL;
	
	Data data=NULL;
	VisiterInfo vi=NULL;
	
	len = LinkedList_getLength(list);
	vi = LinkedList_getIndex(list,0); data = VisiterInfo_getData(vi);
	if(data->column <= 2){visiter_draw1d(list); return;}
	if(len != 5 && len != 1 && len != 2 && len != 6){printdoc_imp(visiter_draw);return;}
	if(len==1 || len==2){
		if(data){rs=0;cs=0;re=Data_getRow(data)-1;ce=Data_getColumn(data)-1;}
		else{return;}
		if(len==2){comfile = String_stripdq(LinkedList_getIndex(list,1));}
	}else{loadargs();}
	if(len==6){
		comfile = String_stripdq(LinkedList_getIndex(list,5));
	}
	if(!check(data,rs,cs,re,ce)){return;}
	r = Data_getRow(data);
	c = Data_getColumn(data);
	
	gp = popen("gnuplot -persist\n","w");
	if(gp == NULL){perror("gnuplot");return;}
	ratio = (re-rs+1)/(ce-cs+1);
	fprintf(gp,"set size ratio %f\n",ratio);
	if(comfile){fprintf(gp,"load \"%s\"\n",comfile);}
	fprintf(gp,"plot '-' binary array=(%d,%d) format=\"%%float64\" endian=little w image notitle\n",ce-cs+1,re-rs+1);	
	
	for(i=rs;i<=re;i++){
		for(j=cs;j<=ce;j++){
			x = Data_get(data,i,j);
			fwrite(&x,sizeof(double),1,gp);
		}
	}
	pclose(gp);
}
void visiter_plot1d(LinkedList list){visiter_draw1d(list);}
void visiter_draw1d(LinkedList list){
	int i,k,len,oned=0;
	int rs=0,re=0;
	double x,y;
	FILE *gp=NULL;
	
	Data data=NULL;
	VisiterInfo vi=NULL;
	
	len = LinkedList_getLength(list);
	vi = LinkedList_getIndex(list,0); data = VisiterInfo_getData(vi);
	if(len != 3 && len != 1){printdoc_imp(visiter_draw1d);return;}
	if(len==1){
		if(data){rs=0;re=Data_getRow(data)-1;}
	}else{
		rs = atoi(LinkedList_getIndex(list,1));
		re = atoi(LinkedList_getIndex(list,2));
	}
	if(data && Data_getColumn(data)==1){
		if(!check(data,rs,0,re,0)){return;}
		oned=1;
	}else{
		if(!check(data,rs,0,re,1)){return;}
	}
	//printf("row : %d\tcolumn : %d\n",Data_getRow(data),Data_getColumn(data));
	gp = popen("gnuplot -persist\n","w");
	if(gp == NULL){perror("gnuplot");return;}
	//printf("plot '-' binary record=(%d,%d) format=\"%%int32%%float64\" endian=little using 1:2 notitle\n",1,re-rs+1);
	
	if(oned){
		fprintf(gp,"plot '-' binary record=(%d,%d) format=\"%%int32%%float64\" endian=little using 1:2 notitle\n",1,re-rs+1);
		for(i=rs;i<=re;i++){
			k = i-rs; fwrite(&k,sizeof(int),1,gp);
			x = Data_get(data,i,0); fwrite(&x,sizeof(double),1,gp);
			//printf("%d\t%f\n",k,x);
		}
	}else{
		fprintf(gp,"plot '-' binary record=(%d,%d) format=\"%%float64%%float64\" endian=little using 1:2 notitle\n",1,re-rs+1);
		for(i=rs;i<=re;i++){
			x = Data_get(data,i,0); fwrite(&x,sizeof(double),1,gp);
			y = Data_get(data,i,1); fwrite(&y,sizeof(double),1,gp);
			//printf("%d\t%f\n",k,x);
		}
	}
	pclose(gp);
}

void visiter_plotpng(LinkedList list){visiter_drawpng(list);}
void visiter_drawpng(LinkedList list){
	int i,j,len;
	int rs=0,cs=0,re=0,ce=0;
	double ratio,x;
	int r,c;
	char *comfile=NULL,*name=NULL,*filename=NULL,*dirname=NULL;
	FILE *gp=NULL;
	
	Data data=NULL;
	VisiterInfo vi=NULL;
	
	len = LinkedList_getLength(list);
	vi = LinkedList_getIndex(list,0); data = VisiterInfo_getData(vi);
	if(!data){printf("no data is loaded. please use \"load\" function\n"); return;}
	if(data->column <= 2){visiter_draw1dpng(list); return;}
	if(len != 1 && len != 2 && len != 3){printdoc_imp(visiter_drawpng);return;}
	if(data){rs=0;cs=0;re=Data_getRow(data)-1;ce=Data_getColumn(data)-1;}
	else{return;}
	if(len==2){
		dirname = String_stripdq(LinkedList_getIndex(list,1));
	}
	if(len==3){
		dirname = String_stripdq(LinkedList_getIndex(list,1));
		comfile = String_stripdq(LinkedList_getIndex(list,2));
	}
	if(!check(data,rs,cs,re,ce)){return;}
	r = Data_getRow(data);
	c = Data_getColumn(data);
	
	gp = popen("gnuplot -persist\n","w");
	if(gp == NULL){perror("gnuplot");return;}
	fprintf(gp,"set term png size %d,%d\n",c,r);
	filename = getFileName(vi->filename);
	name=String_ncopy(filename,strlen(filename)-strlen(getFileExtension(filename)));
	if(dirname){
		mkdir(dirname,0777);
		fprintf(gp,"set output \"%s/%s.png\"\n",dirname,name);
	}else{
		fprintf(gp,"set output \"%s.png\"\n",name);
	}
	ratio = (re-rs+1)/(ce-cs+1);
	fprintf(gp,"set size ratio %f\n",ratio);
	if(comfile){fprintf(gp,"load \"%s\"\n",comfile);}
	fprintf(gp,"plot '-' binary array=(%d,%d) format=\"%%float64\" endian=little w image notitle\n",ce-cs+1,re-rs+1);	
	for(i=rs;i<=re;i++){
		for(j=cs;j<=ce;j++){
			x = Data_get(data,i,j);
			fwrite(&x,sizeof(double),1,gp);
		}
	}
	pclose(gp);
	deallocate(name);
}

void visiter_plot1dpng(LinkedList list){visiter_draw1dpng(list);}
void visiter_draw1dpng(LinkedList list){
	int i,k,len,oned=0;
	int rs=0,re=0;
	double x,y;
	FILE *gp=NULL;
	
	Data data=NULL;
	VisiterInfo vi=NULL;
	char *name=NULL,*dirname=NULL,*filename=NULL,*comfile=NULL;
	
	len = LinkedList_getLength(list);
	vi = LinkedList_getIndex(list,0); data = VisiterInfo_getData(vi);
	if(len > 3){printdoc_imp(visiter_draw1d);return;}
	rs=0;re=Data_getRow(data)-1;	
	if(len==2){
		dirname = String_stripdq(LinkedList_getIndex(list,1));
	}
	if(len==3){
		dirname = String_stripdq(LinkedList_getIndex(list,1));
		comfile = String_stripdq(LinkedList_getIndex(list,2));
	}
	if(data && Data_getColumn(data)==1){
		if(!check(data,rs,0,re,0)){return;}
		oned=1;
	}else{
		if(!check(data,rs,0,re,1)){return;}
	}
	gp = popen("gnuplot -persist\n","w");
	if(gp == NULL){perror("gnuplot");return;}
	filename = getFileName(vi->filename);
	name=String_ncopy(filename,strlen(filename)-strlen(getFileExtension(filename)));
	if(dirname){
		mkdir(dirname,0777);
		fprintf(gp,"set output \"%s/%s.png\"\n",dirname,name);
	}else{
		fprintf(gp,"set output \"%s.png\"\n",name);
	}
	if(comfile){fprintf(gp,"load \"%s\"\n",comfile);}
	if(oned){
		fprintf(gp,"plot '-' binary record=(%d,%d) format=\"%%int32%%float64\" endian=little using 1:2 notitle\n",1,re-rs+1);
		for(i=rs;i<=re;i++){
			k = i-rs; fwrite(&k,sizeof(int),1,gp);
			x = Data_get(data,i,0); fwrite(&x,sizeof(double),1,gp);
			//printf("%d\t%f\n",k,x);
		}
	}else{
		fprintf(gp,"plot '-' binary record=(%d,%d) format=\"%%float64%%float64\" endian=little using 1:2 notitle\n",1,re-rs+1);
		for(i=rs;i<=re;i++){
			x = Data_get(data,i,0); fwrite(&x,sizeof(double),1,gp);
			y = Data_get(data,i,1); fwrite(&y,sizeof(double),1,gp);
			//printf("%d\t%f\n",k,x);
		}
	}
	pclose(gp);
}

void visiter_spot(LinkedList list){
	int i,j,len;
	int rs=0,cs=0,re=0,ce=0;
	double ratio,x,value=0;
	int r,c;
	FILE *gp=NULL;
	
	Data data=NULL;
	VisiterInfo vi=NULL;
	
	len = LinkedList_getLength(list);
	vi = LinkedList_getIndex(list,0);
	
	if(len != 5 && len != 6){printdoc_imp(visiter_spot);return;}
	data = VisiterInfo_getData(vi);
	loadargs();
	if(!check(data,rs,cs,re,ce)){return;}
	if(len==6){value = atof(LinkedList_getIndex(list,5));}
	r = Data_getRow(data);
	c = Data_getColumn(data);
	
	
	gp = popen("gnuplot -persist\n","w");
	if(gp == NULL){perror("gnuplot");return;}
	ratio = r/c;
	fprintf(gp,"set size ratio %f\n",ratio);
	//fprintf(gp,"set object rectangle from graph %d,%d to graph %d,%d front fs empty border linewidth 10\n",rs,cs,re,ce);
	fprintf(gp,"plot '-' binary array=%dx%d format=\"%%float64\" endian=little w image notitle\n",c,r);
	//fprintf(gp,"plot '-' matrix w image notitle\n"); 
	for(i=0;i<r;i++){
		for(j=0;j<c;j++){
			if((rs <= i && i <= re)&&(abs(j-cs)<3 || abs(j-ce)<3)){x=value;}
			else if((cs <= j && j <= ce)&&(abs(i-rs)<3 || abs(i-re)<3)){x=value;}
			else{x = Data_get(data,i,j);}
			fwrite(&x,sizeof(double),1,gp);
		}
	}
	//fprintf(gp,"replot\n");
	//fprintf(gp,"e\n");
	pclose(gp);
}
void visiter_load(LinkedList list){
	char *file,**array;
	int len,count;
	
	VisiterInfo vi=NULL;
	
	len = LinkedList_getLength(list);
	vi = LinkedList_getIndex(list,0);
	
	if(len !=2 &&  len != 3 && len != 4 && len != 5 && len != 6 && len != 7){
		printf("argcount : %d\n",len-1); printdoc_imp(visiter_load); return;
	}
	
	file = String_stripdq(LinkedList_getIndex(list,1));
	if(len==3){
#ifdef LOAD_SDF
	if(!strcmp(".sdf",getFileExtension(file))){
		deallocate(VisiterInfo_getFileName(vi));
		VisiterInfo_setFileName(vi,strdup(file));
		Data_delete(VisiterInfo_getData(vi));
		VisiterInfo_setData(vi,Data_loadSDF(file,String_stripdq(LinkedList_getIndex(list,2))));
		goto loaded;
	}
#else
		array = parseSeparators(String_stripdq(LinkedList_getIndex(list,2)),&count);
		VisiterInfo_setFieldSeparators(vi,count,array);
		
#endif
	}
	if(len==5){
		array = parseSeparators(String_stripdq(LinkedList_getIndex(list,4)),&count);
		VisiterInfo_setFieldSeparators(vi,count,array);
	}
	if(len>=4){
		vi->options.rowOffset = atof(LinkedList_getIndex(list,2));
		vi->options.columnOffset = atof(LinkedList_getIndex(list,3));
	}
	if(len==7){
		array = parseSeparators(String_stripdq(LinkedList_getIndex(list,6)),&count);
		VisiterInfo_setFieldSeparators(vi,count,array);
	}
	if(len>=6){
		vi->options.rows = atof(LinkedList_getIndex(list,4));
		vi->options.columns = atof(LinkedList_getIndex(list,5));
	}
	
	deallocate(VisiterInfo_getFileName(vi));
	VisiterInfo_setFileName(vi,strdup(file));
	Data_delete(VisiterInfo_getData(vi));
#ifdef LOAD_BMAT
	if(!strcmp(".bmat",getFileExtension(file))){
		VisiterInfo_setData(vi,Data_loadBMAT(file));
		goto loaded;
	}
#endif
#ifdef LOAD_BARY
	if(!strcmp(".bary",getFileExtension(file))){
		VisiterInfo_setData(vi,Data_loadBARY(file));
		goto loaded;
	}
#endif
#ifdef LOAD_PNG
	if(!strcmp(".png",getFileExtension(file))){
		VisiterInfo_setData(vi,Data_loadPNG(file));
		goto loaded;
	}
#endif
#ifdef LOAD_TIFF
	if(!strcmp(".tif",getFileExtension(file)) || !strcmp(".tiff",getFileExtension(file)) || 
	   !strcmp(".gel",getFileExtension(file))){
		VisiterInfo_setData(vi,Data_loadTIFF(file));
		goto loaded;
	}
#endif
	VisiterInfo_setData(vi,Data_readFile(file,&(vi->options)));
  loaded:
	return;
	//if(len == 3 && len == 5 && len == 7){deallocate(sep);}
}
void visiter_unload(LinkedList list){
	VisiterInfo vi=NULL;
	vi = LinkedList_getIndex(list,0);
	Data_delete(VisiterInfo_getData(vi));
	deallocate(VisiterInfo_getFileName(vi));
	VisiterInfo_setFileName(vi,NULL);
	VisiterInfo_setData(vi,NULL);
}

int compare(void *c1, void *c2){
	//printf("c1:%s\tc2:%s\n",(char*)c1,(char*)c2);
	//printf("c1:%d\tc2:%d\n",atoi((char*)c1),atoi((char*)c2));
	//printf("compare : %d\n",atoi((char*)c1)-atoi((char*)c2));
	return atoi((char*)c1)-atoi((char*)c2);
}

void visiter_aveload(LinkedList list){
	DIR *dp=NULL;
	struct dirent *entry;
	char optdef[128],pathname[256];
	struct stat statbuf;
	char *dir,*file,*funcstr,*optstr = optdef,*fname; 
	int len,loadlen,fopened=0;
	size_t needed;
	
	VisiterInfo vi = NULL,vi2 = NULL;
	HashTable ht=NULL;
	Data data=NULL;
	LinkedList arglist=NULL,optlist=NULL,dirlist=NULL,templist=NULL;
	Func func=NULL,loader=NULL;

	len = LinkedList_getLength(list);
	vi=LinkedList_getIndex(list,0);
	if(len != 2 && len != 3){printdoc_imp(visiter_aveload); return;}
	ht = VisiterInfo_getHashTable(vi);
	
	snprintf(optdef,128,"%s%s%s","load(0,0,-1,-1,\"",vi->options.fieldSeparators[0],"\")");
	if((dp = opendir(dir = String_stripdq(LinkedList_getIndex(list,1))))==NULL){perror("visiter_aveload"); return;}
	if(len==3){optstr = LinkedList_getIndex(list,2);}
	if((optlist = parsefunc(optstr))==NULL){return;}
	/*optlist : (0 : VisiterInfo) -> ...*/
	optlist = LinkedList_prepend(optlist,vi);
	
	dirlist = StringList_init();
	while((entry=readdir(dp))!=NULL){
		if(entry->d_name[0]=='.'){continue;}
		StringList_copy(dirlist,entry->d_name);
	}
	closedir(dp);
	templist = dirlist = LinkedList_sort(dirlist,compare);
	
	while(templist){
		file = LinkedList_get(templist);
		snprintf(pathname,256,"%s/%s",dir,file);
		LinkedList_setIndex(optlist,1,pathname);
		visiter_load(optlist);
		vi2 = LinkedList_get(optlist);
		if(!data){data = Data_copy(vi2->data);}
		else{Data_add(data,vi2->data,data,0,0,-1,-1);}
		visiter_unload(optlist);
		templist = LinkedList_increment(templist);
	}
	
	len = LinkedList_getLength(dirlist);
	printf("%d\n",len);
	Data_muls(data,(1.0/len),data);
	VisiterInfo_setData(vi,data);
	LinkedList_deleteRoot(arglist,NULL);
	LinkedList_deleteRoot(optlist,NULL);
	StringList_finish(dirlist);
	VisiterInfo_setFileName(vi,String_copy(pathname));
}

int wildcard_match(const char *wildcard, const char *target){
    const char *pw = wildcard, *pt = target;

    while(1){
        if(*pt == 0) return *pw == 0;
        else if(*pw == 0) return 0;
        else if(*pw == '*'){
            return *(pw+1) == 0 || wildcard_match(pw, pt + 1)
                                || wildcard_match(pw + 1, pt);
        }
        else if(*pw == '?' || (*pw == *pt)){
            pw++;
            pt++;
            continue;
        }
        else return 0;
    }
}

char *getfullpath(char *path){
	char *p,*buf;
	int len;
	p = strchr(path,'/');
	if(p){
		buf = String_copy(path);
	}else{
		p=getcwd(NULL,0);
		if(!p){
			perror("getfullpath");
			exit(1);
		}
		len += 1 + strlen(path) + 1;
		buf = allocate(len*sizeof(char*));
		snprintf(buf,len,"%s/%s",p,path);
		free(p);
	}
	return buf;
}

void dir_name_split(char *path, char **dir, char **name){
	char sav,*p;
	int len,dlen,nlen;
	p = strchr(path,'\0');
	for(nlen=0;path<p;nlen++,p--){
		if(*p=='/'){break;}
	}
	len = strlen(path);
	dlen = len - nlen;
	*dir = String_ncopy(path,dlen);
	*name = String_ncopy(p+1,nlen);
}

void visiter_series(LinkedList list){
	DIR *dp=NULL;
	struct dirent *entry;
	char optdef[128],pathname[256];
	
	struct stat statbuf;
	char *full,*dir,*name,*file,*funcstr,*optstr,*fname; 
	int len,loadlen,fopened=0;
	size_t needed;
	
	VisiterInfo vi = NULL;
	HashTable ht=NULL;
	Data data;
	LinkedList arglist=NULL,optlist=NULL,dirlist=NULL,tail=NULL,s,t;
	Func func=NULL,loader=NULL;

	len = LinkedList_getLength(list);
	vi=LinkedList_getIndex(list,0);
	/*list[3,4] :  (0 : VisiterInfo ) -> (1 : dirname ) -> (2 : funcstr ) -> (3 : funcstr2)
	 -> ... [ -> (n : optstr )]*/
	if(len < 3){printdoc_imp(visiter_series); return;}
	
	ht = VisiterInfo_getHashTable(vi);
	snprintf(optdef,128,"load");
		
	full = getfullpath(String_stripdq(LinkedList_getIndex(list,1)));
	//printf("full : %s\n",full);
	dir_name_split(full,&dir,&name);
	//printf("dir : %s\n",dir);
	//printf("name : %s\n",name);
		
	if((dp = opendir(dir))==NULL){perror("visiter_series"); return;}
	
	/*(3 : optstr)*/
	optstr = LinkedList_get(tail=LinkedList_tail(list));
	if(strncmp(optstr,"load",4)){optstr=optdef;tail=NULL;}
	if((optlist = parsefunc(optstr))==NULL){return;}
	/*optlist : (0 : VisiterInfo) -> ...*/
	VisiterInfo_setSeriesOn(vi);
	optlist = LinkedList_prepend(optlist,vi);
	
	/*dir : input dir*/
	/*func : main function*/
	/*arglist : main function args*/
	/*optlist : load function args*/
	dirlist = StringList_init();
	while((entry=readdir(dp))!=NULL){
		if(entry->d_name[0]=='.'){continue;}
		if(!wildcard_match(name,entry->d_name)){continue;}
		StringList_copy(dirlist,entry->d_name);
	}
	closedir(dp);
	/*sort directory name list*/
	dirlist = LinkedList_sort(dirlist,compare);
	//StringList_print(templist);
	/*visiter_load(optlist)*/
	/*optlist :  (0 : VisiterInfo <= data is loaded in here)  -> ...*/
	
	/*func(arglist) */
	/*arglist :  (0 : VisiterInfo <= optlist[0] ) -> ...*/
	
	//LinkedList_print(list,LinkedList_vprintf);
	for(s=list->next->next;s!=tail;s=s->next){
		s->content = parsefunc(funcstr = s->content);
	}
	
	for(t=dirlist;t;t=t->next){
		file = LinkedList_get(t); 
		snprintf(pathname,256,"%s/%s",dir,file);
		LinkedList_setIndex(optlist,1,pathname);
		visiter_load(optlist);
		for(s=list->next->next;s!=tail;s=s->next){
			arglist = LinkedList_get(s);
			fname = LinkedList_getIndex(arglist,0);
			//LinkedList_print(arglist,LinkedList_vprintf);
			if(NULL==(func=HashTable_get(ht,fname))){
				printf("visiter_series : I don't understand\n");
				return;
			}
			if(func==visiter_load || func == visiter_unload || 
			   func == visiter_draw || func == visiter_spot || 
			   func == visiter_series){
				printf("visiter_series : %s is unsupported in visiter_series\n",fname);
				return;
			}
			LinkedList_setIndex(arglist,0,LinkedList_getIndex(optlist,0));
			func(arglist);
			LinkedList_setIndex(arglist,0,fname);
		}
		visiter_unload(optlist);
	}
	for(s=list->next->next;s!=tail;s=s->next){
		LinkedList_deleteRoot(s->content,NULL);
	}
	deallocate(dir);
	deallocate(name);
	deallocate(full);
	LinkedList_deleteRoot(optlist,NULL);
	StringList_finish(dirlist);
	VisiterInfo_setFileName(vi,NULL);
	VisiterInfo_setSeriesOff(vi);
}

void visiter_shape(LinkedList list){
	int len,i,j,k, r, c, oc,or,size,n;
	VisiterInfo vi = NULL;
	Data data =	NULL;
	Data ndata = NULL;
	len = LinkedList_getLength(list);
	vi = LinkedList_getIndex(list,0); data = VisiterInfo_getData(vi);
	
	if(len != 2 && len != 3){printdoc_imp(visiter_shape); return;}
	or = Data_getRow(data);
	oc = Data_getColumn(data);
	size = Data_getSize(data);
	if(len == 2){
		c = atoi(LinkedList_getIndex(list,1));
		r = size/c;
	}else{
		r = atoi(LinkedList_getIndex(list,1));
		c = atoi(LinkedList_getIndex(list,2));
	}
	ndata = Data_shape(data,r,c);
	Data_delete(data);
	VisiterInfo_setData(vi,ndata);
}

void visiter_setsep(LinkedList list){
	int len,count;
	char **array;
	VisiterInfo vi = NULL;
	len = LinkedList_getLength(list);
	
	if(len != 2 && len != 1){printdoc_imp(visiter_setsep); return;}
	if(len == 2){
		vi = LinkedList_getIndex(list,0);
		array = parseSeparators(String_stripdq(LinkedList_getIndex(list,1)),&count);
		VisiterInfo_setFieldSeparators(vi,count,array);
	}
}

#define visiter_binop(func) void visiter_##func(LinkedList list){\
	int len,rs=0,cs=0,re=-1,ce=-1;\
	char *buf,*buf2;\
	double value;\
	FILE *fp=NULL;\
	VisiterInfo vi = NULL;\
	Data data = NULL, data2 = NULL, data3 = NULL;\
	len = LinkedList_getLength(list);\
	if(len != 2 && len != 3 && len != 6 && len != 7){printdoc_imp(visiter_##func); return;}\
	vi = LinkedList_getIndex(list,0);\
	data3 = data = VisiterInfo_getData(vi);\
	buf = LinkedList_getIndex(list,1);\
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}\
	if(len == 6 || len == 7){\
		rs = atoi((char*)LinkedList_getIndex(list,2));\
		cs = atoi((char*)LinkedList_getIndex(list,3));\
		re = atoi((char*)LinkedList_getIndex(list,4));\
		ce = atoi((char*)LinkedList_getIndex(list,5));\
	}\
	if(re<0){re = Data_getRow(data)-1;}\
	if(ce<0){ce = Data_getColumn(data)-1;}\
	if(!check(data,rs,cs,re,ce)){return;}\
	if(isFloat(buf)){\
		value = atof(buf);\
	}else{\
		buf2 = String_stripdq(buf);\
		if(!strcmp(".bmat",getFileExtension(buf2))){\
			data2 = Data_loadBMAT(buf2);\
		}\
		else if(!strcmp(".bary",getFileExtension(buf2))){\
			data2 = Data_loadBARY(buf2);\
		}\
		else{\
			data2 = Data_load(buf2,0,0,-1,-1,vi->options.fieldSeparators[0]);\
		}\
	}\
	if(len == 3){\
		fp = wopen(String_stripdq(LinkedList_getIndex(list,2)));\
		data3 = NULL;\
	}\
	else if(len == 7){\
		fp = wopen(String_stripdq(LinkedList_getIndex(list,6)));\
		data3 = NULL;\
	}\
	data3 = ((data2) ? Data_##func(data,data2,data3,rs,cs,re,ce) :  Data_##func##s(data,value,data3));\
	if(fp){Data_fprint(data3,fp,vi->options.fieldSeparators[0]);fclose(fp);}\
	Data_delete(data2);\
}

visiter_binop(add)
visiter_binop(sub)
visiter_binop(mul)
visiter_binop(pow)

void visiter_rslice(LinkedList list){
	int len, cs=0, ce=0;
	FILE *fp = NULL;
	char *buf,*sep;
	VisiterInfo vi = NULL;
	Data data = NULL, data2 = NULL;
	len = LinkedList_getLength(list);
	if(len != 1 && len != 2 && len != 3 && len != 4){printdoc_imp(visiter_rslice); return;}
	vi = LinkedList_getIndex(list,0);
	data = VisiterInfo_getData(vi);
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}
	
	if(len == 1){
		cs = 0;
		ce = Data_getColumn(data)-1;
	}
	if(len == 2){
		buf = LinkedList_getIndex(list,1);
		if(isInt(buf)){cs = ce = atoi(buf);}
		else{
			cs=0;
			ce = Data_getColumn(data)-1;
			fp = wopen(String_stripdq(buf));
		}
	}
	if(len == 3){
		buf = LinkedList_getIndex(list,2);
		if(isInt(buf)){cs = atoi(LinkedList_getIndex(list,1)); ce = atoi(buf);}
		else{
			cs = ce = atoi(LinkedList_getIndex(list,1));
			fp = wopen(String_stripdq(buf));
		}
	}
	if(len == 4){
		cs = atoi(LinkedList_getIndex(list,1));
		ce = atoi(LinkedList_getIndex(list,2));
		buf = LinkedList_getIndex(list,3);
		fp = wopen(String_stripdq(buf));
	}
	data2 = Data_rslice(data,cs,ce);
	if(fp){Data_fprint(data2,fp,vi->options.fieldSeparators[0]); fclose(fp); Data_delete(data2);}
	else{Data_delete(data); VisiterInfo_setData(vi,data2);}
}

void visiter_cslice(LinkedList list){
	int len, rs=0, re=0;
	
	FILE *fp = NULL;
	char *buf;
	VisiterInfo vi = NULL;
	Data data = NULL, data2 = NULL;
	len = LinkedList_getLength(list);
	if(len != 1 && len != 2 && len != 3 && len != 4){printdoc_imp(visiter_cslice); return;}
	vi = LinkedList_getIndex(list,0);
	data = VisiterInfo_getData(vi);
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}

	if(len == 1){
		rs = 0;
		re = Data_getRow(data)-1;
	}
	if(len == 2){
		buf = LinkedList_getIndex(list,1);
		if(isInt(buf)){rs = re = atoi(buf);}
		else{
			rs=0;
			re = Data_getRow(data)-1;
			fp = wopen(String_stripdq(buf));
		}
	}
	if(len == 3){
		buf = LinkedList_getIndex(list,2);
		if(isInt(buf)){rs = atoi(LinkedList_getIndex(list,1)); re = atoi(buf);}
		else{
			rs = re = atoi(LinkedList_getIndex(list,1));
			fp = wopen(String_stripdq(buf));
		}
	}
	if(len == 4){
		rs = atoi(LinkedList_getIndex(list,1));
		re = atoi(LinkedList_getIndex(list,2));
		buf = LinkedList_getIndex(list,3);
		fp = wopen(String_stripdq(buf));
	}
	
	data2 = Data_cslice(data,rs,re);
	if(fp){Data_fprint(data2,fp,vi->options.fieldSeparators[0]); fclose(fp); Data_delete(data2);}
	else{Data_delete(data); VisiterInfo_setData(vi,data2);}	
}

#define visiter_binop2(func) void visiter_##func(LinkedList list){\
	int len, index;\
	char *sep;\
	double value;\
	FILE *fp=NULL;\
	VisiterInfo vi = NULL;\
	Data data = NULL, data2 = NULL;\
	len = LinkedList_getLength(list);\
	if(len != 2 && len != 3 && len != 4){printdoc_imp(visiter_##func); return;}\
	vi = LinkedList_getIndex(list,0);\
	data = VisiterInfo_getData(vi);\
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}\
	index = atoi(LinkedList_getIndex(list,1));\
	value = atof(LinkedList_getIndex(list,2));\
	if(len == 4){\
		fp = wopen(String_stripdq(LinkedList_getIndex(list,3)));\
		data2 = Data_copy(data);\
		Data_##func(data2,index,value);\
		Data_fprint(data2,fp,vi->options.fieldSeparators[0]);fclose(fp); Data_delete(data2);\
	}else{\
		Data_##func(data,index,value);\
	}\
}

visiter_binop2(radd)
visiter_binop2(cadd)
visiter_binop2(rmul)
visiter_binop2(cmul)
visiter_binop2(rpow)
visiter_binop2(cpow)

#define visiter_swapop(func) void visiter_##func(LinkedList list){\
	int len, index1, index2;\
	double value;\
	FILE *fp=NULL;\
	VisiterInfo vi = NULL;\
	Data data = NULL, data2 = NULL;\
	len = LinkedList_getLength(list);\
	if(len != 2 && len != 3 && len != 4){printdoc_imp(visiter_##func); return;}\
	vi = LinkedList_getIndex(list,0);\
	data = VisiterInfo_getData(vi);\
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}\
	index1 = atoi(LinkedList_getIndex(list,1));\
	index2 = atoi(LinkedList_getIndex(list,2));\
	if(len == 4){\
		fp = wopen(String_stripdq(LinkedList_getIndex(list,3)));\
		data2 = Data_copy(data);\
		Data_##func(data2, index1, index2);\
		Data_fprint(data2,fp,vi->options.fieldSeparators[0]);fclose(fp); Data_delete(data2);\
	}else{\
		Data_##func(data, index1, index2);\
	}\
}

visiter_swapop(rswap)
visiter_swapop(cswap)

void visiter_rappend(LinkedList list){
	int len,i;
	char *buf;
	double value;
	FILE *fp=NULL;
	VisiterInfo vi = NULL;
	Data data = NULL, dataout = NULL, dataap=NULL;
	len = LinkedList_getLength(list);
	if(len != 2 && len != 3 && len != 4){printdoc_imp(visiter_rappend); return;}	
	vi = LinkedList_getIndex(list,0);
	data = VisiterInfo_getData(vi);
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}
	buf = LinkedList_getIndex(list,1);
	if(isFloat(buf)){
		value = atof(buf);
		dataap = Data_create(1,Data_getColumn(data));
		for(i=0;i<Data_getColumn(dataap);i++){Data_set(dataap,0,i,value);}
	}else{
		dataap = Data_load(String_stripdq(buf),0,0,-1,-1,vi->options.fieldSeparators[0]);
	}
	if(len == 3){
		fp = wopen(String_stripdq(LinkedList_getIndex(list,2)));
		dataout = Data_copy(data);
		Data_fprint(dataout,fp,vi->options.fieldSeparators[0]); fclose(fp);
		Data_delete(dataout);
	}else{
		VisiterInfo_setData(vi,Data_rappend(data,dataap));
		Data_delete(dataap);
		Data_delete(data);
	}
}

void visiter_cappend(LinkedList list){
	int len,i;
	char *buf;
	double value;
	FILE *fp=NULL;
	VisiterInfo vi = NULL;
	Data data = NULL, dataout = NULL, dataap=NULL;
	len = LinkedList_getLength(list);
	if(len != 2 && len != 3 && len != 4){printdoc_imp(visiter_cappend); return;}	
	vi = LinkedList_getIndex(list,0);
	data = VisiterInfo_getData(vi);
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}
	buf = LinkedList_getIndex(list,1);
	if(isFloat(buf)){
		value = atof(buf);
		dataap = Data_create(Data_getRow(data),1);
		for(i=0;i<Data_getRow(dataap);i++){Data_set(dataap,i,0,value);}
	}else{
		dataap = Data_load(String_stripdq(buf),0,0,-1,-1,vi->options.fieldSeparators[0]);
	}
	if(len == 3){
		fp = wopen(String_stripdq(LinkedList_getIndex(list,2)));
		dataout = Data_copy(data);
		Data_fprint(dataout,fp,vi->options.fieldSeparators[0]); fclose(fp);
		Data_delete(dataout);
	}else{
		VisiterInfo_setData(vi,Data_cappend(data,dataap));
		Data_delete(dataap);
		Data_delete(data);
	}
}

void visiter_rinsert(LinkedList list){}
void visiter_cinsert(LinkedList list){}

void visiter_rfourier(LinkedList list){
	FILE *fp = NULL;
	VisiterInfo vi = NULL;
	Data data=NULL, data2=NULL, data3=NULL;	
	int len, cs=0, ce=0,i,n;
	double real, imaginal,dk;
	char *buf;
	
	len = LinkedList_getLength(list);
	if(len != 1 && len != 2 && len != 3 && len != 4){printdoc_imp(visiter_rfourier); return;}
	vi = LinkedList_getIndex(list,0);
	data = VisiterInfo_getData(vi);
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}
	
	if(len == 1){
		cs = 0;
		ce = Data_getColumn(data)-1;
	}
	if(len == 2){
		buf = LinkedList_getIndex(list,1);
		if(isInt(buf)){cs = ce = atoi(buf);}
		else{
			cs=0;
			ce = Data_getColumn(data)-1;
			fp = wopen(String_stripdq(buf));
		}
	}
	if(len == 3){
		buf = LinkedList_getIndex(list,2);
		if(isInt(buf)){cs = atoi(LinkedList_getIndex(list,1)); ce = atoi(buf);}
		else{
			cs = ce = atoi(LinkedList_getIndex(list,1));
			fp = wopen(String_stripdq(buf));
		}
	}
	if(len == 4){
		cs = atoi(LinkedList_getIndex(list,1));
		ce = atoi(LinkedList_getIndex(list,2));
		buf = LinkedList_getIndex(list,3);
		fp = wopen(String_stripdq(buf));
	}
	data2 = Data_rslice(data,cs,ce);
	if(!data2){goto data2null;}
	n = Data_getRow(data2);
	dk = 2*M_PI/(Data_get(data2,data2->row-1,1)-Data_get(data2,0,1));
	Data_cswap(data2,0,1);
	for(i=0;i<n;i++){Data_set(data2,i,1,0);}
	Data_FFT(data2);
	data3 = Data_create(n,3);
	for(i=0;i<n;i++){
		Data_set(data3,i,0,i*dk);
		real = Data_get(data2,i,0);
		imaginal = Data_get(data2,i,1);
		Data_set(data3,i,1,sqrt(real*real+imaginal*imaginal));
		Data_set(data3,i,2,atan2(imaginal,real));
	}
	Data_delete(data2);
  data2null:
	if(fp){Data_fprint(data3,fp,vi->options.fieldSeparators[0]); fclose(fp); Data_delete(data3);}
	else{Data_delete(data); VisiterInfo_setData(vi,data3);}	
}
void visiter_cfourier(LinkedList list){
	FILE *fp = NULL;
	VisiterInfo vi = NULL;
	Data data=NULL, data2=NULL, data3=NULL;	
	int len, rs=0, re=0,i,n;
	double real,imaginal,dk;
	char *buf;
	
	len = LinkedList_getLength(list);
	if(len != 1 && len != 2 && len != 3 && len != 4){printdoc_imp(visiter_cfourier); return;}
	vi = LinkedList_getIndex(list,0);
	data = VisiterInfo_getData(vi);
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}
	
	if(len == 1){
		rs = 0;
		re = Data_getColumn(data)-1;
	}
	if(len == 2){
		buf = LinkedList_getIndex(list,1);
		if(isInt(buf)){rs = re = atoi(buf);}
		else{
			rs=0;
			re = Data_getColumn(data)-1;
			fp = wopen(String_stripdq(buf));
		}
	}
	if(len == 3){
		buf = LinkedList_getIndex(list,2);
		if(isInt(buf)){rs = atoi(LinkedList_getIndex(list,1)); re = atoi(buf);}
		else{
			rs = re = atoi(LinkedList_getIndex(list,1));
			fp = wopen(String_stripdq(buf));
		}
	}
	if(len == 4){
		rs = atoi(LinkedList_getIndex(list,1));
		re = atoi(LinkedList_getIndex(list,2));
		buf = LinkedList_getIndex(list,3);
		fp = wopen(String_stripdq(buf));
	}
	data2 = Data_cslice(data,rs,re);
	if(!data2){goto data2null;}
	n = Data_getRow(data2);
	dk = 2*M_PI/(Data_get(data2,data2->row-1,1)-Data_get(data2,0,1));
	Data_cswap(data2,0,1);
	for(i=0;i<n;i++){Data_set(data2,i,1,0);}

	Data_FFT(data2);
	data3 = Data_create(n,3);
	
	for(i=0;i<n;i++){
		Data_set(data3,i,0,i*dk);
		real = Data_get(data2,i,0);
		imaginal = Data_get(data2,i,1);
		Data_set(data3,i,1,sqrt(real*real+imaginal*imaginal));
		Data_set(data3,i,2,atan2(imaginal,real));
	}
	Data_delete(data2);
  data2null:
	if(fp){Data_fprint(data3,fp,vi->options.fieldSeparators[0]); fclose(fp); Data_delete(data3);}
	else{Data_delete(data); VisiterInfo_setData(vi,data3);}
}

void visiter_fourier(LinkedList list){
	FILE *fp = NULL;
	VisiterInfo vi = NULL;
	Data data=NULL, data2=NULL, data3=NULL;	
	int len,i,n;
	double real, imaginal;
	char *buf;
	
	len = LinkedList_getLength(list);
	if(len != 1 && len != 2){printdoc_imp(visiter_fourier); return;}
	vi = LinkedList_getIndex(list,0);
	data = VisiterInfo_getData(vi);
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}
	
	if(len == 2){
		buf = LinkedList_getIndex(list,1);
		fp = wopen(String_stripdq(buf));
	}
	data2 = Data_create(n = Data_getRow(data),2);
	if(Data_getColumn(data)==1){
		for(i=0;i<n;i++){
			Data_translate(data,i,0,data2,i,0);
			Data_set(data2,i,1,0);
		}
	}else{
		for(i=0;i<n;i++){
			Data_translate(data,i,1,data2,i,0);
			Data_set(data2,i,1,0);
		}
	}
	
	Data_FFT(data2);
	data3 = Data_create(n,3);
	for(i=0;i<n;i++){
		Data_set(data3,i,0,i);
		real = Data_get(data2,i,0);
		imaginal = Data_get(data2,i,1);
		Data_set(data3,i,1,sqrt(real*real+imaginal*imaginal));
		Data_set(data3,i,2,atan2(imaginal,real));
	}
	Data_delete(data2);
	if(fp){Data_fprint(data3,fp,vi->options.fieldSeparators[0]); fclose(fp); Data_delete(data3);}
	else{Data_delete(data); VisiterInfo_setData(vi,data3);}
}

void visiter_fourier2D(LinkedList list){
	FILE *fp = NULL;
	VisiterInfo vi = NULL;
	Data data=NULL,re=NULL,im=NULL;
	int len,i,j,n;
	double real, imaginal;
	char *buf;
	
	len = LinkedList_getLength(list);
	if(len != 1 && len != 2){printdoc_imp(visiter_fourier2D); return;}
	vi = LinkedList_getIndex(list,0);
	data = VisiterInfo_getData(vi);
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}
	
	if(len == 2){
		buf = LinkedList_getIndex(list,1);
		fp = wopen(String_stripdq(buf));
	}
	re = Data_copy(data);
	im = Data_create(data->row,data->column);
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column;j++){
			im->elem[i][j] = 0;
		}
	}
	Data_FFT2D(re,im);
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column;j++){
			re->elem[i][j] = (re->elem[i][j])*(re->elem[i][j]) + (im->elem[i][j])*(im->elem[i][j])/(data->row*data->column);
		}
	}
	for(i=0;i<re->row/2;i++){
		for(j=0;j<re->column;j++){
			n = i + re->row/2;
			real = re->elem[n][j];
			re->elem[n][j] = re->elem[i][j];
			re->elem[i][j] = real;	
		} 
	}
	for(j=0;j<re->column/2;j++){
		for(i=0;i<re->row;i++){
			n = j + re->column/2;
			real = re->elem[i][n];
			re->elem[i][n] = re->elem[i][j];
			re->elem[i][j] = real;	
		} 
	}
	if(fp){Data_fprint(re,fp,vi->options.fieldSeparators[0]); fclose(fp); Data_delete(re);}
	else{Data_delete(data); VisiterInfo_setData(vi,re);}
	Data_delete(im);
}

void visiter_fourierMask(LinkedList list){
	FILE *fp = NULL;
	VisiterInfo vi = NULL;
	Data data=NULL,re=NULL,im=NULL,mask=NULL;
	int len,i,j,n;
	double real, imaginal;
	char *buf,*maskfile;
	
	len = LinkedList_getLength(list);
	if(len != 2){printdoc_imp(visiter_fourierMask); return;}
	vi = LinkedList_getIndex(list,0);
	data = VisiterInfo_getData(vi);
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}
	
	maskfile = LinkedList_getIndex(list,1);
	re = Data_copy(data);
	im = Data_create(data->row,data->column);
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column;j++){
			im->elem[i][j] = 0;
		}
	}
	Data_FFT2D(re,im);
	for(i=0;i<re->row/2;i++){
		for(j=0;j<re->column;j++){
			n = i + re->row/2;
			real = re->elem[n][j];
			imaginal = im->elem[n][j];
			re->elem[n][j] = re->elem[i][j];
			im->elem[n][j] = im->elem[i][j];
			re->elem[i][j] = real;
			im->elem[i][j] = imaginal;
		} 
	}
	for(j=0;j<re->column/2;j++){
		for(i=0;i<re->row;i++){
			n = j + re->column/2;
			real = re->elem[i][n];
			imaginal = im->elem[i][n];
			re->elem[i][n] = re->elem[i][j];
			im->elem[i][n] = im->elem[i][j];
			re->elem[i][j] = real;
			im->elem[i][j] = imaginal;
		} 
	}
#ifdef LOAD_BMAT
	if(!strcmp(".bmat",getFileExtension(maskfile))){
		mask = Data_loadBMAT(maskfile);
		goto loaded;
	}
#endif
#ifdef LOAD_BARY
	if(!strcmp(".bary",getFileExtension(maskfile))){
		mask = Data_loadBARY(maskfile);
		goto loaded;
	}
#endif
#ifdef LOAD_PNG
	if(!strcmp(".png",getFileExtension(maskfile))){
		mask = Data_loadPNG(maskfile);
		goto loaded;
	}
#endif
#ifdef LOAD_TIFF
	if(!strcmp(".tif",getFileExtension(maskfile)) || !strcmp(".tiff",getFileExtension(maskfile)) || 
	   !strcmp(".gel",getFileExtension(maskfile))){
		mask = Data_loadTIFF(maskfile);
		goto loaded;
	}
#endif
  loaded:
	if(mask->row != re->row || mask->column != re->column){
		fprintf(stdout,
				"visiter_fourierMask : The dimension of mask array (%d,%d) is not equal to that of data array (%d,%d)\n",
				mask->row,mask->column,re->row,re->column
				);
		Data_delete(mask); return;
	}
	for(i=0;i<re->row;i++){
		for(j=0;j<re->column;j++){
			re->elem[i][j] *= mask->elem[i][j];
			im->elem[i][j] *= mask->elem[i][j];
		}
	}
	for(i=0;i<re->row/2;i++){
		for(j=0;j<re->column;j++){
			n = i + re->row/2;
			real = re->elem[n][j];
			imaginal = im->elem[n][j];
			re->elem[n][j] = re->elem[i][j];
			im->elem[n][j] = im->elem[i][j];
			re->elem[i][j] = real;
			im->elem[i][j] = imaginal;
		} 
	}
	for(j=0;j<re->column/2;j++){
		for(i=0;i<re->row;i++){
			n = j + re->column/2;
			real = re->elem[i][n];
			imaginal = im->elem[i][n];
			re->elem[i][n] = re->elem[i][j];
			im->elem[i][n] = im->elem[i][j];
			re->elem[i][j] = real;
			im->elem[i][j] = imaginal;
		} 
	}
	Data_IFFT2D(re,im);
	if(fp){Data_fprint(re,fp,vi->options.fieldSeparators[0]); fclose(fp); Data_delete(re);}
	else{Data_delete(data); VisiterInfo_setData(vi,re);}
	Data_delete(im);
}

void visiter_fwhm(LinkedList list){
	VisiterInfo vi = NULL;
	Data data = NULL;
	FILE *fp=stdout;
	double xmin=0,xmax=0,fwhm,xl,xr;
	int len;
	
	len = LinkedList_getLength(list);
	if(len != 1 && len != 2 && len != 3 && len != 4){printdoc_imp(visiter_fwhm);return;}
	
	vi = LinkedList_getIndex(list,0);
	data = VisiterInfo_getData(vi);
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}
	if(len == 2){
		if(VisiterInfo_isSeriesOn(vi)){
			if((fp = fopen(String_stripdq(LinkedList_getIndex(list,1)),"a"))==NULL){return;}
			fprintf(fp,"%s\t",getFileName(VisiterInfo_getFileName(vi)));
		}else{
			fp = wopen(String_stripdq(LinkedList_getIndex(list,1)));
		}
	}
	if(len >= 3){
		xmin = atof(LinkedList_getIndex(list,1));
		xmax = atof(LinkedList_getIndex(list,2));
	}
	if(len == 4){
		if(VisiterInfo_isSeriesOn(vi)){
			if((fp = fopen(String_stripdq(LinkedList_getIndex(list,3)),"a"))==NULL){return;}
			fprintf(fp,"%s\t",getFileName(VisiterInfo_getFileName(vi)));
		}else{
			fp = wopen(String_stripdq(LinkedList_getIndex(list,3)));
		}
	}
	fwhm = Data_fwhm(data,xmin,xmax,&xl,&xr);
	fprintf(fp,"%e\t",fwhm);
	fprintf(fp,"%e\t",xl);
	fprintf(fp,"%e\n",xr);
	if(len==2 || len==4){fclose(fp);}
}

#define visiter_transform(func) void visiter_##func(LinkedList list){\
	VisiterInfo vi = NULL;\
	Data data = NULL,data2 = NULL;\
	FILE *fp=NULL;\
	char *sep;\
	int len;\
	len = LinkedList_getLength(list);\
	if(len != 1 && len != 2){printdoc_imp(visiter_##func);return;}\
	vi = LinkedList_getIndex(list,0);\
	data = VisiterInfo_getData(vi);\
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}\
	if(len == 2){\
		fp = wopen(String_stripdq(LinkedList_getIndex(list,1)));\
	}\
	data2 = Data_##func(data);\
	if(fp){\
		Data_fprint(data2,fp,vi->options.fieldSeparators[0]);\
		fclose(fp);\
		Data_delete(data2);\
	}else{\
		Data_delete(data); VisiterInfo_setData(vi,data2);\
	}\
}

visiter_transform(rrotate)
visiter_transform(lrotate)
visiter_transform(transpose)

#define visiter_transform2(func) void visiter_##func(LinkedList list){\
	VisiterInfo vi = NULL;\
	Data data = NULL,data2 = NULL;\
	FILE *fp=NULL;\
	int len;\
	len = LinkedList_getLength(list);\
	if(len != 1 && len != 2){printdoc_imp(visiter_##func);return;}\
	vi = LinkedList_getIndex(list,0);\
	data = VisiterInfo_getData(vi);\
	if(data==NULL){printf("no data is loaded. please use \"load\" function\n"); return;}\
	if(len == 2){\
		fp = wopen(String_stripdq(LinkedList_getIndex(list,1)));\
	}\
	if(fp){\
		Data_##func(data2=Data_copy(data));\
		Data_fprint(data2,fp,vi->options.fieldSeparators[0]);\
		fclose(fp);\
		Data_delete(data2);\
	}else{\
		Data_##func(data);\
	}\
}

visiter_transform2(vreflect)
visiter_transform2(hreflect)

LinkedList parsefunc(char *buf){
	int level=0,qlevel=0, dqlevel=0;
	
	LinkedList list = LinkedList_create(buf);
	while(*buf){
//		printf("%c ",*buf);
		if(*buf == '('){
			if(level==0){*buf ='\0'; LinkedList_append(list,buf+1);}
			level++;
		}
		if(*buf==')'){
			if(level==1){level--; *buf ='\0'; break;}
			level--;
		}
		if(*buf==','){
			if(level==1 && !dqlevel && !qlevel){*buf = '\0'; LinkedList_append(list,buf+1);}
		}
		if(*buf == '\"'){
			if(dqlevel){dqlevel--;}
			else{dqlevel++;}
		}
		if(*buf == '\''){
			if(qlevel){qlevel--;}
			else{qlevel++;}
		}
		buf++;
	}
	//printf("\n"); 
	if(level != 0){
		printf("parsefunc : error. Number of left and right parenthesis are not equal\n");
		LinkedList_deleteRoot(list,NULL); list = NULL;
	}
	if(qlevel != 0 || dqlevel != 0){
		printf("parsefunc : error. non closed quoted or double quoted string\n");
		LinkedList_deleteRoot(list,NULL); list = NULL;
	}
	//LinkedList_enum(list,putstr);
	return list;
}

void visiter_export(LinkedList list){
	int i,j,len;
	double max;
	FILE *fp = stdout;	
	VisiterInfo vi=NULL;
	Data data=NULL;
	char *name,*filein,*fileout;
	
	len = LinkedList_getLength(list);
	vi = LinkedList_getIndex(list,0);
	data = VisiterInfo_getData(vi);
	if(!data){printf("no data is loaded. please use \"load\" function\n");return;}
	if(len != 1 && len != 2){printdoc_imp(visiter_export);return;}
	if(len==1){
		filein = getFileName(vi->filename);
		name=String_ncopy(filein,strlen(filein)-strlen(getFileExtension(filein)));
		fileout=String_join(name,".bmat");
		Data_output(data,fileout,p_float);
		deallocate(name);
		deallocate(fileout);
	}
	if(len==2){
		fileout = String_stripdq(LinkedList_getIndex(list,1));
		Data_output(data,fileout,p_float);
	}
}
#define doc(NAME,print) if(func==visiter_##NAME){print; return;}
/*http://stackoverflow.com/questions/3585846/color-text-in-terminal-aplications-in-unix*/
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KMAG  "\x1B[35m"
#define KYEL  "\x1B[33m"
#define BOLD   "\x1B[1m" 
#define RESET "\x1B[0m"

#define BOLDBLUE "\x1B[1m\x1B[34m"
#define USAGE KNRM KRED "usage : " KNRM BOLD
#define EXAMPLE KNRM KMAG "example : " KNRM BOLD

void printdoc_imp(Func func){
	char *p = "";
	doc(ave,
		printf(
		BOLD "%-10s" USAGE "ave[([rs,cs,re,ce][,\"OUTPUTFILE\"])]\n"
		"%-10s" EXAMPLE "ave, ave(\"OUTPUTFILE\"), ave(10,10,20,20), ave(10,10,20,20,\"OUTPUTFILE\")\n"					
		RESET
		"%-10s" KNRM "return average value of data in rectangle region (rs,cs,re,ce)\n"
		"%-10s" "whole region is calculated when you simply type \"ave\" or \"ave(\"OUTPUTFILE\")\" \n\n"
		,"ave", p,p,p
		)
	)
	doc(min,
		printf(
		BOLD "%-10s" USAGE "min[([rs,cs,re,ce][,\"OUTPUTFILE\"])]\n"
		"%-10s" EXAMPLE "min, min(\"OUTPUTFILE\"), min(10,10,20,20), min(10,10,20,20,\"OUTPUTFILE\")\n"					
		RESET
		"%-10s" KNRM "return minimun value and position of data in rectangle region (rs,cs,re,ce)\n"
		"%-10s" "whole region is calculated when you simply type \"min\" or \"min(\"OUTPUTFILE\")\" \n\n"
		,"min",p,p,p
		)
	)
	doc(max,
		printf(
		BOLD "%-10s" USAGE "max[([rs,cs,re,ce][,\"OUTPUTFILE\"])]\n"
		"%-10s" EXAMPLE  "max, max(\"OUTPUTFILE\"), max(10,10,20,20), max(10,10,20,20,\"OUTPUTFILE\")\n"					
		RESET
		"%-10s" KNRM "return maximum value and position of data in rectangle region (rs,cs,re,ce)\n"
		"%-10s" "whole region is calculated when you simply type \"max\" or \"max(\"OUTPUTFILE\")\" \n\n"
		,"max",p,p,p
		)
	)
	doc(sum,
		printf(
		BOLD "%-10s" USAGE "sum[([rs,cs,re,ce][,\"OUTPUTFILE\"])]\n"
		"%-10s" EXAMPLE "sum, sum(\"OUTPUTFILE\"), sum(10,10,20,20), sum(10,10,20,20,\"OUTPUTFILE\")\n"					
		RESET
		"%-10s" KNRM "return total sum of data in rectangle region (rs,cs,re,ce)\n"
		"%-10s" "whole region is calculated when you simply type \"sum\" or \"sum(\"OUTPUTFILE\")\" \n\n"
		,"sum",p,p,p
		)
	)
	doc(print,
		printf(
		BOLD "%-10s" USAGE "print[([rs,cs,re,ce][,\"OUTPUTFILE\"])]\n"
		"%-10s" EXAMPLE "print, print(\"OUTPUTFILE\"), print(10,10,20,20), print(10,10,20,20,\"OUTPUTFILE\")\n"					
		RESET
		"%-10s" KNRM "print out rectangle region (rs,cs,re,ce) in stdout or in \"OUTPUTFILE\"\n"
		"%-10s" "whole region is calculated when you simply type \"print\" or \"print(\"OUTPUTFILE\")\" \n\n"
		,"print",p,p,p
		)
	)
	doc(draw,
		printf(
		BOLD "%-10s" USAGE "draw[(rs,cs,re,ce,[\"commandfile\"])], draw(\"commandfile\"),\n"
		"%-10s" EXAMPLE "draw, draw(10,10,20,20)\n"					
		RESET
		"%-10s" KNRM "draw rectangle region (rs,cs,re,ce) with gnuplot\n"
		"%-10s" "whole region is calculated when you simply type \"draw\"\n\n"
		"%-10s" "if commandfile is specified, command writen in the file is excuted in gnuplot\n"
		,"draw",p,p,p,p
		)
	)
	doc(draw1d,
		printf(
		BOLD "%-10s" USAGE "draw1d[(rs,re)]\n"
		"%-10s" EXAMPLE "draw, draw(10,20)\n"					
		RESET
		"%-10s" KNRM "draw 1d line from rs to re with gnuplot\n"
		"%-10s" "whole region is calculated when you simply type \"draw1d\"\n\n"
		,"draw1d",p,p,p
		)
	)
	doc(spot,
		printf(
		BOLD "%-10s" USAGE "spot(rs,cs,re,ce [,value])\n"
		"%-10s" EXAMPLE "spot(10,10,20,20), spot(10,10,20,20,1e-03)\n"
		RESET
		"%-10s" KNRM "draw whole region and a rectangle to show (rs,cs,re,ce) region with gnuplot\n"
		"%-10s" KNRM "if rectangle is difficult to see, set value to adjust color of rectangle\n\n"
		,"spot",p,p,p
		)
	)
	doc(aveload,
		printf(
		BOLD "%-10s" USAGE "aveload(\"INPUTDIR\" [,\"LOADCOMMAND\"])\n"
		"%-10s" EXAMPLE "aveload(\"INPUTDIR\")\n"
		RESET
		"%-10s" KNRM "aveload file. default separator is \"SEPARATOR\" = \"%s\"\n"
		"%-10s" "setting row(or column) to -1 tells loader to load till the end of row(or column)\n\n"
		,"aveload",p,p,SEPDEF,p
		)
	)
	doc(load,
		printf(
		BOLD "%-10s" USAGE "load(\"INPUTFILE\"[,row offset,column offset[,row,column]] [,\"SEPARATOR\"])\n"
		"%-10s" EXAMPLE "load(\"INPUTFILE\"), load(\"INPUTFILE\",10,10,-1,-1), load(\"INPUTFILE\",10,10,-1,-1, \",\")\n"
		RESET
		"%-10s" KNRM "load file. default separator is \"SEPARATOR\" = \"%s\"\n"
		"%-10s" "setting row(or column) to -1 tells loader to load till the end of row(or column)\n\n"
		,"load",p,p,SEPDEF,p
		)
	)
	doc(unload,
		printf(
		BOLD "%-10s" USAGE "unload[()]\n"
		"%-10s" EXAMPLE "unload\n"
		RESET
		"%-10s" KNRM "unload file\n\n"
		,"unload",p,p
		)
	)
	doc(series,
		printf(
			   BOLD "%-10s" USAGE "series(\"FILENAME WILDCARD\",func(args[,\"OUTPUTFILE\"]),func2(args[,\"OUTPUTFILE\"]),... [,load(row offset, column offset[,row, column] [,\"SEPARATOR\"])])\n"
			   "%-10s" EXAMPLE "series(\"FILENAME WILDCARD\",ave)\n"
			   "%-10s" EXAMPLE "series(\"FILENAME WILDCARD\",ave(10,10,20,20,\"OUTPUTFILE\"),load(10,10,-1,-1,\",\"))\n"
		RESET
			   "%-10s" KNRM "files that matches to \"FILENAME WILDCARD\" is processed in series, using function command at 2nd slot.\n"
		"%-10s" "you can use most of visiter functions like max, ave, etc..\n"
		"%-10s" "load options can be set by adding load(args) commadn at last. see help of load command\n\n"
		,"series",p,p,p,p,p
		)
	)
	doc(setsep,
		printf(
		BOLD "%-10s" USAGE "setsep[(\"SEPARATOR\")]\n"
		"%-10s" EXAMPLE "setsep(\",\")\n"
		RESET
		"%-10s" KNRM "set separator used in loading and displaying data\n"
		"%-10s" KNRM "if you want to use tab separated file, type : setsep(\"\\t\")\n\n"
		,"setsep",p,p,p
		)
	)
	doc(shape,
		printf(
		BOLD "%-10s" USAGE "shape([row,]column)\n"
		"%-10s" EXAMPLE "shape(10), shape(100,200)\n"
		RESET
		"%-10s" KNRM "set new row and column size of the data matrix\n"
		"%-10s"  "if only 1 input is given, it's taken as a column, and makes datasize/column * column matrix\n"		
		"%-10s"  "if row*column is bigger than datasize, unfilled data points are filled with zeros\n"
		"%-10s"  "if row*column is smaller than datasize, exceed data points are deleted\n\n"
		,"shape",p,p,p,p,p
		)
	)
	doc(add,
		printf(
		BOLD "%-10s" USAGE "add(\"INPUTFILE\" or value [,r0,r1,c0,c1] [,\"OUTPUTFILE\"])\n"
		"%-10s" EXAMPLE "add(\"INPUTFILE\"), add(10), add(\"INPUTFILE\",0,0,0,20,\"OUTPUTFILE\"), add(\"INPUTFILE\",\"OUTPUTFILE\")\n"
		RESET
		"%-10s" "add each elements of data in \"INPUTFILE\" or a value to each elements of data currently loaded\n"
		"%-10s" "please use double quote in input inputfile name if it sounds like a value\n"				
		"%-10s" "if output file is specified, result is written to outputifle, otherwise currently loaded data is updated\n\n"		
		,"add ",p,p,p,p
		)
	)
	doc(sub,
		printf(
		BOLD "%-10s" USAGE "sub(\"INPUTFILE\" or value [,rs,cs,re,ce] [,\"OUTPUTFILE\"])\n"
		"%-10s" EXAMPLE "sub(\"INPUTFILE\"), sub(10), sub(\"INPUTFILE\",0,0,0,20,\"OUTPUTFILE\"), sub(\"INPUTFILE\", \"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "subtract each elements of data in \"INPUTFILE\" or a value from each elements of data currently loaded\n"
		"%-10s" "please use double quote in input inputfile name if it sounds like a value\n"				
		"%-10s" "if output file is specified, result is written into outputifle, otherwise currently loaded data is updated\n\n"		
					
		,"sub",p,p,p,p
		)
	)
	doc(mul,
		printf(
		BOLD "%-10s" USAGE "mul(\"INPUTFILE\" or value [,rs,cs,re,ce] [,\"OUTPUTFILE\"])\n"
		"%-10s" EXAMPLE "mul(\"INPUTFILE\"), mul(10), mul(\"INPUTFILE\",0,0,0,20,\"OUTPUTFILE\"), mul(\"INPUTFILE\", \"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "multiply each elements of data in \"INPUTFILE\" or a value to each elements of data currently loaded\n"
		"%-10s" "please use double quote in input inputfile name if it sounds like a value\n"				
		"%-10s" "if output file is specified, result is written into outputifle, otherwise currently loaded data is updated\n\n"		
		,"mul",p,p,p,p
		)
	)
	doc(pow,
		printf(
		BOLD "%-10s" USAGE "pow[exp,(\"OUTPUTFILE\")]\n"
		"%-10s" EXAMPLE "pow(2), pow(2,\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "calculate exp th power of each elements of data currently loaded.\n"
		"%-10s" KNRM "result is written to outputfile or currently loaded data is updated.\n\n"
		,"pow",p,p,p
		)
	)
	doc(rslice,
		printf(
		BOLD "%-10s" USAGE "rslice[(cs[,ce] [,\"OUTPUTFILE\"])]\n"
		"%-10s" EXAMPLE "rslice, rslice(5), rslice(0,10), rslice(0, 10,\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "create 1D data by picking a column or averaging over column\n"
		"%-10s" "if no arguments are used, then average over whole column\n"
		"%-10s" "if 1 argument is provided, only single column is used. no averaging takes place\n"		
		"%-10s" "if 2 arguments are provided, average from cs to ce\n\n"		
		,"rslice",p,p,p,p,p
		)
	)
	doc(cslice,
		printf(
		BOLD "%-10s" USAGE "cslice[(rs[,re] [,\"OUTPUTFILE\"])]\n"
		"%-10s" EXAMPLE "cslice, cslice(5), cslice(0,10), cslice(0, 10,\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "create 1D data by picking a row or averaging over row\n"
		"%-10s" "if no arguments are used, then average over whole row\n"				
		"%-10s" "if 1 argument is provided, only single row is used. no averaging takes place\n"
		"%-10s" "if 2 arguments are provided, average from rs to re\n\n"		
		,"cslice",p,p,p,p,p
		)
	)
	doc(radd,
		printf(
		BOLD "%-10s" USAGE "radd(index,value [,\"OUTPUTFILE\"])\n"
		"%-10s" EXAMPLE "radd(1,1.2e+20),radd(0,1.2e+20,\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "add the value to all the elements in the selected row\n"
		"%-10s" KNRM "the index of the 1st row is '0' not '1'\n\n"
		,"radd",p,p,p
		)
	)
	doc(cadd,
		printf(
		BOLD "%-10s" USAGE "cadd(index,value [,\"OUTPUTFILE\"])\n"
		"%-10s" EXAMPLE "cadd(1,1.2e+20),cadd(0,1.2e+20,\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "add the value to all the elements in the selected column\n"
		"%-10s" KNRM "the index of the 1st column is '0' not '1'\n\n"
		,"cadd",p,p,p
		)
	)
	doc(rmul,
		printf(
		BOLD "%-10s" USAGE "rmul(index,value [,\"OUTPUTFILE\"])\n"
		"%-10s" EXAMPLE "rmul(1,1.2e+20),rmul(0,1.2e+20,\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "multiply the value to all the elements in the selected row\n"
		"%-10s" KNRM "the index of the 1st row is '0' not '1'\n\n"
		,"rmul",p,p,p
		)
	)
	doc(cmul,
		printf(
		BOLD "%-10s" USAGE "cmul(index,value [,\"OUTPUTFILE\"])\n"
		"%-10s" EXAMPLE "cmul(1,1.2e+20),cmul(0,1.2e+20,\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "multiply the value to all the elements in the selected column\n"
		"%-10s" KNRM "the index of the 1st column is '0' not '1'\n\n"
		,"cmul",p,p,p
		)
	)
	doc(rpow,
		printf(
		BOLD "%-10s" USAGE "rpow(index,value [,\"OUTPUTFILE\"])\n"
		"%-10s" EXAMPLE "rpow(1,1.2e+20),rpow(0,1.2e+20,\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "pow the value to all the elements in the selected row\n"
		"%-10s" KNRM "the index of the 1st row is '0' not '1'\n\n"
		,"rpow",p,p,p
		)
	)
	doc(cpow,
		printf(
		BOLD "%-10s" USAGE "cpow(index,value [,\"OUTPUTFILE\"])\n"
		"%-10s" EXAMPLE "cpow(1,1.2e+20),cpow(0,1.2e+20,\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "pow the value to all the elements in the selected column\n"
		"%-10s" KNRM "the index of the 1st row is '0' not '1'\n\n"
		,"cpow",p,p,p
		)
	)
	doc(rswap,
		printf(
		BOLD "%-10s" USAGE "rswap(r1,r2 [,\"OUTPUTFILE\"])\n"
		"%-10s" EXAMPLE "rswap(0,2),rswap(0,2,\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "swap row\n"
		"%-10s" KNRM "the index of the 1st row is '0' not '1'\n\n"
		,"rswap",p,p,p
		)
	)
	doc(cswap,
		printf(
		BOLD "%-10s" USAGE "cswap(c1,c2 [,\"OUTPUTFILE\"])\n"
		"%-10s" EXAMPLE "cswap(0,2),cswap(0,2,\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "swap column\n"
		"%-10s" KNRM "the index of the 1st column is '0' not '1'\n\n"
		,"cswap",p,p,p
		)
	)
	doc(rappend,
		printf(
		BOLD "%-10s" USAGE "rappend(load(args),\"OUTPUTFILE\")\n"
		"%-10s" EXAMPLE "rappend(1e+5), rappend(load(\"INPUTFILE\"))\n"
		RESET
		"%-10s" KNRM "append row data\n"
		"%-10s" KNRM "if input is a value, append new row data which all points have that value is created\n"
		"%-10s" KNRM "if input is a filename, append the row data loaded from the \"INPUTFILE\"\n"
		"%-10s" KNRM "if row length doesn't match, either skipping or filling zero will occur\n"
		"%-10s" KNRM "you can also append matrix data as long as row length matches\n\n"
		,"rappend",p,p,p,p,p,p
		)
	)
	doc(cappend,
		printf(
		BOLD "%-10s" USAGE "cappend(load(args),\"OUTPUTFILE\")\n"
		"%-10s" EXAMPLE "cappend(1e+5), cappend(load(\"INPUTFILE\"))\n"
		RESET
		"%-10s" KNRM "append column data\n"
		"%-10s" KNRM "if input is a value, append new column data which all points have that value is created\n"
		"%-10s" KNRM "if input is a filename, append the column data loaded from the \"INPUTFILE\"\n"
		"%-10s" KNRM "if column length doesn't match, either skipping or filling zero will occur\n"
		"%-10s" KNRM "you can also append matrix data as long as column length matches\n\n"
		,"cappend",p,p,p,p,p,p
		)
	)
	doc(rinsert,
		printf(
		BOLD "%-10s" USAGE "rinsert(r,\"INPUTFILE\",\"OUTPUTFILE\")\n"
		"%-10s" EXAMPLE "rinsert(2,1e+5), rinsert(2,\"INPUTFILE\")\n"
		RESET
		"%-10s" KNRM "insert row data after the row r\n"
		"%-10s" KNRM "the index of the 1st row is '0' not '1'\n\n"
		,"rinsert",p,p,p
		)
	)
	doc(cinsert,
		printf(
		BOLD "%-10s" USAGE "cinsert(c,\"INPUTFILE\",\"OUTPUTFILE\")\n"
		"%-10s" EXAMPLE "cinsert(2,1e+5),cinsert(2,\"INPUTFILE\")\n"
		RESET
		"%-10s" KNRM "insert column data after the column c\n"
		"%-10s" KNRM "the index of the 1st column is '0' not '1'\n\n"
		,"cinsert",p,p,p
		)
	)
	doc(rfourier,
		printf(
		BOLD "%-10s" USAGE "rfourier[(cs[,ce] [,\"OUTPUTFILE\"])]\n"
		"%-10s" EXAMPLE "rfourier, rfourier(5), rfourier(0,10), rfourier(0,10,\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "calculate fourier transform of by picking a column or averaging over columns\n"
		"%-10s" "if no arguments are used, then average over whole columns\n"
		"%-10s" "if 1 argument is provided, only single column is used. no averaging takes place\n"		
		"%-10s" "if 2 arguments are provided, average from cs to ce\n\n"		
		,"rfourier",p,p,p,p,p
		)
	)
	doc(cfourier,
		printf(
		BOLD "%-10s" USAGE "cfourier[(rs[,re] [,\"OUTPUTFILE\"])]\n"
		"%-10s" EXAMPLE "cfourier, cfourier(5), cfourier(0,10), cfourier(0, 10,\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "calculate fourier transform at a specified row or averaging over serveral rows\n"
		"%-10s" "if no arguments are used, then average over whole rows\n"				
		"%-10s" "if 1 argument is filled, only single row is used. no averaging takes place\n"
		"%-10s" "if 2 arguments are filled, average from rs to re\n\n"		
		,"cfourier",p,p,p,p,p
		)
	)
	doc(fourier,
		printf(
		BOLD "%-10s" USAGE "fourier[([,\"OUTPUTFILE\"])]\n"
		"%-10s" EXAMPLE "fourier, fourier(\"OUTPUTFILE\")\n"
		RESET
	   "%-10s" KNRM "calculate fourier transform of 1d data\n\n"
		,"fourier",p,p
		)
	)
	doc(fourier2D,
		printf(
		BOLD "%-10s" USAGE "fourier2D([,\"OUTPUTFILE\"])\n"
		"%-10s" EXAMPLE "fourier2D, fourier2D(\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "calculate 2d fourier power spectrum of 2d data\n\n"
		,"fourier2D",p,p
		)
	)
	doc(fourierMask,
		printf(
	   BOLD "%-10s" USAGE "fourierMask(,\"MASKFILE\")\n"
		    "%-10s" EXAMPLE "fourierMask(\"MASKFILE\")\n"
	   RESET
			"%-10s" KNRM "multiply the mask array element-wise to the 2d fourier transform of the current array,\n"
			"%-10s" KNRM "and return 2d inverse transform of the obtained array.\n"
		    "%-10s" KNRM "mask array is obtained from \"MASKFILE\", The dimension of the mask array must be same as\n"
			"%-10s" KNRM "that of the current array.\n"
			"%-10s" KNRM "Each element of the mask array should be a real value ranges from 0 to 1\n\n"
			   ,"fourierMask",p,p,p,p,p,p
	   )
	)
	doc(fwhm,
		printf(
		BOLD "%-10s" USAGE "fwhm([xmin,xmax][,\"OUTPUTFILE\"])\n"
		"%-10s" EXAMPLE "fwhm(\"OUTPUTFILE\"), fwhm(-10,10,\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "return full width at half maximum of 2d data\n"
		"%-10s" KNRM "if xmin and xmax is specified datapoints inside the range is used to calculate fwhm\n\n"
		,"fwhm",p,p,p
		)
	)
	doc(transpose,
		printf(
		BOLD "%-10s" USAGE "transpose[(\"OUTPUTFILE\")]\n"
		"%-10s" EXAMPLE "transpose, transpose(\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "transpose a matrix\n"
		,"fwhm",p,p
		)
	)
	doc(vreflect,
		printf(
		BOLD "%-10s" USAGE "vreflect[(\"OUTPUTFILE\")]\n"
		"%-10s" EXAMPLE "vreflect, vreflect(\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "reverse iamge by top-bottom mirror reflection\n"
		,"fwhm",p,p
		)
	)
	doc(hreflect,
		printf(
		BOLD "%-10s" USAGE "hreflect[(\"OUTPUTFILE\")]\n"
		"%-10s" EXAMPLE "hreflect, hreflect(\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "reverse image by left-right mirror reflection\n"
		,"fwhm",p,p
		)
	)
	doc(rrotate,
		printf(
		BOLD "%-10s" USAGE "rrotate[(\"OUTPUTFILE\")]\n"
		"%-10s" EXAMPLE "rrotate, rrotate(\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "rotate image clockwise by 90 degrees\n"
		,"fwhm",p,p
		)
	)
	doc(lrotate,
		printf(
		BOLD "%-10s" USAGE "lrotate[(\"OUTPUTFILE\")]\n"
		"%-10s" EXAMPLE "lrotate, lrotate(\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "rotate image counterclockwise by 90 degrees\n"
		,"fwhm",p,p
		)
	)
	doc(export,
		printf(
		BOLD "%-10s" USAGE "export[(\"OUTPUTFILE\"])]\n"
		"%-10s" EXAMPLE "export, export(\"OUTPUTFILE\")\n"
		RESET
		"%-10s" KNRM "export data to \"OUTPUTFILE\" in bmat format\n"
		"%-10s" KNRM "bmat format :\n"
		"%-10s" KNRM "\tmeta data : 12 bytes\n"
		"%-10s" KNRM "\tmain data : row*column*(4 or 8) bytes\n"
		"%-10s" KNRM "data type (int32),row (int32),column (int32),data (Real32 or Real64)...\n"
		"%-10s" KNRM "data type :\n\t0 : Real64\n\t1 : Real32\n"
		"\n\n"
		,"export",p,p,p,p,p,p,p,p
		)
	)
	doc(drawpng,
		printf(
		BOLD "%-10s" USAGE "drawpng[[\"directory name\"],[\"command file\"])], draw(\"command file\"),\n"
		"%-10s" EXAMPLE "drawpng, drawpng(\"png/\",\"gnucom.txt\")\n"					
		RESET
		"%-10s" KNRM "draw 2d map with gnuplot in png file\n"
		"%-10s" "you can provide gnuplot command file in the last argument\n"
		,"drawpng",p,p,p,p
		)
	)
	else{printf("no documentation is provided\n"); return;}
/*	doc(select,
		printf(
		BOLD "%-10s" USAGE "select(rs,cs,re,ce [,value])\n"
		"%-10s" EXAMPLE "select(10,10,20,20), select(10,10,20,20,1e-03)\n"
		RESET
		"%-10s" KNRM "draw whole region and a rectangle to show (rs,cs,re,ce) region with gnuplot\n"
		"%-10s" KNRM "if rectangle is difficult to see, set value to adjust color of rectangle\n\n"
		,"spot",p,p,p
		)
	)
*/}

void printdoc(void *content){
	printdoc_imp((Func)content);
}
#define KCYN  "\x1B[36m"

void printbasicdoc(){
	char *p ="";
	printf( 
	"\x1B[50m"	
	KCYN "============VISITER USAGE============" KNRM "\n"
	"%-10s" USAGE "visiter [filename] [row offset] [column offset] [row] [column] [\"SEPARATOR\"]\n"
	"%-10s" EXAMPLE "visiter \"filename\", visiter \"filename\" 10 10 10 10 \",\"\n"
	"%-10s" KNRM "comandline options\n" 
	"%-10s" "-c [command] : excute command just once and quit\n\n"
	"%-10s" "type \"quit\" to quit program\n"
	KCYN "=============COMMANDS=============" KNRM "\n"
	"arguments in [] can be omitted\n"
	"double quote or single quote is usually omitted\n\n"
	,"visiter",p,p,p,p
	);
}
#undef KCYN 

#undef KNRM 
#undef KRED 
#undef KMAG 
#undef KWHT
#undef doc
