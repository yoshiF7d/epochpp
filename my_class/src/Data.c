#include "Data.h"
#include "LinkedList.h"
#include <stdarg.h>
//#include "LeakDetector.h"

Data Data_create(int row, int column){
	int i;
	Data data = allocate(sizeof(Data_Sub));
	data->elem = allocate(row*sizeof(double*));
	data->elem[0] = allocate(row*column*sizeof(double));
	for(i=1;i<row;i++){data->elem[i] = data->elem[0] + i*column;}
	data->row = row;
	data->column = column;
	return data;
}

void Data_delete(Data data){
	if(data){
		deallocate(data->elem[0]);
		deallocate(data->elem);
		deallocate(data);
	}
}

void Data_vdelete(void *data){Data_delete(data);}

char *String_read2(char *file);

#include <time.h>
Data Data_load(char *file, int ofsr, int ofsc, int r, int c, char *sep){
	Data data;
	int i,j,k,cc,*len;
	char *buf,*p,*ends,*newl,*p2=NULL,*newl2=NULL;
	char **seps;

	//clock_t start,end;
	//printf("Data_load(%s,%d,%d,%d,%d,%s)\n",file,ofsr,ofsc,r,c,sep);
	if((buf = String_read(file))==NULL){return NULL;}
	
	//start = clock();
	ends = &buf[strlen(buf)];	
	p = String_skipchr(buf,'\n',ofsr);
	p = String_matchLine(p,sep,isFloat);
		
	if(r<0){
		r=String_countchr(p,'\n');
		r = r +1 - String_countBlankLinesAtEnd(p);
		/*printf("ends[-1] : %c\n",ends[-1]);*/
	}
	
	cc = String_countWordLine(p,sep);
	//printf("cc : %d\n",cc);
	if(c<0){c=cc-ofsc;}
	//end = clock();
	//printf("count : %lu [us]\n",end-start);
	
	data = Data_create(r,c);
	
	//start = clock();
	p = String_skipchr(buf,'\n',ofsr);
	newl = p = String_matchLine(p,sep,isFloat);
	
	seps = allocate(2*sizeof(char*));
	len = callocate(2,sizeof(int));
	seps[0]=sep; seps[1]="\n";
	
	for(i=0;i<r;i++){
		newl = strtok3(newl,&seps[1],len,1,&newl2);
		//fprintf(stdout,"%s\n",newl); 
		p = strtok3(newl,&seps[0],len,1,&p2);
		//loadbar(i,r,50);
		for(j=0;j<ofsc;j++){p = strtok3(NULL,&seps[0],len,1,&p2);}
		for(j=0;j<c;j++){
			if(p){data->elem[i][j] = atof(p);}
			else{data->elem[i][j] = 0;}
			//fprintf(stdout,"data->elem[%d][%d] : %e\t%s\n",i,j,data->elem[i][j],p);
			p=strtok3(NULL,&seps[0],len,1,&p2);
			//printf("%d,%d,%e\n",i,j,data->elem[i][j]);
		}
		newl = newl2; newl2=NULL; p2= NULL;
		/*
		if(c+ofsc<cc){
			for(i=c+ofsc;i<cc;i++){strtok2(NULL,seps,len,2);}
		}
		*/
		/*
		if(c+ofsc<cc){
			if(p){
				p = strtail(p)+1;
				if(p){
					p = String_endline(p)+1;
					if(p){p=strtok2(p,seps,len,2);}
				}
			}
		}
		*/
	}
	//end = clock();
	//printf("load : %lu [us]\n",end-start);
	deallocate(len);deallocate(seps);
	deallocate(buf);
	//printf("row : %d, column : %d\n",data->row,data->column);
	return data;
}
/*
Data Data_load2(char *file, int ofsr, int ofsc, int r, int c, char *sep){
	Data data;
	unsigned long size;
	int i,j,k,cc,*len,fd;
	char *buf,*p,*ends,*newl,*p2=NULL,*newl2=NULL;
	char **seps;

	//clock_t start,end;
	//printf("Data_load(%s,%d,%d,%d,%d,%s)\n",file,ofsr,ofsc,r,c,sep);
	size = getFileSizeBin(file);
	fd = open(file,O_RDONLY,0);
	if(fd==-1){fprintf(stderr,"Data_load : unable to read file %s",file); return NULL;}
	buf = mmap(NULL,size,PROT_READ,MAP_FILE|MAP_PRIVATE,fd,0);
	close(fd);
	fd = open("/dev/zero",O_RDONLY,0);
	if(fd==-1){return NULL;}
	mmap(buf+size,4096,PROT_READ,MAP_PRIVATE|MAP_FIXED,fd,0);
	close(fd);
	
	//start = clock();
	ends = &buf[strlen(buf)];	
	p = String_skipchr(buf,'\n',ofsr);
	p = String_matchLine(p,sep,isFloat);	
		
	if(r<0){
		r=String_countchr(p,'\n');
		r = r +1 - String_countBlankLinesAtEnd(p);
		printf("ends[-1] : %c\n",ends[-1]);
	}
	
	cc = String_countWordLine(p,sep);
	//printf("cc : %d\n",cc);
	if(c<0){c=cc-ofsc;}
	//end = clock();
	//printf("count : %lu [us]\n",end-start);
	
	data = Data_create(r,c);
	
	//start = clock();
	p = String_skipchr(buf,'\n',ofsr);
	newl = p = String_matchLine(p,sep,isFloat);
	
	seps = allocate(2*sizeof(char*));
	len = callocate(2,sizeof(int));
	seps[0]=sep; seps[1]="\n";
	
	for(i=0;i<r;i++){
		newl = strtok3(newl,&seps[1],len,1,&newl2);
		p = strtok3(newl,&seps[0],len,1,&p2);
		//loadbar(i,r,50);
		for(j=0;j<ofsc;j++){p = strtok3(NULL,&seps[0],len,1,&p2);}
		for(j=0;j<c;j++){
			if(p){data->elem[i][j] = atof(p);}
			else{data->elem[i][j] = 0;}
			p=strtok3(NULL,&seps[0],len,1,&p2);
		}
		newl = newl2; newl2=NULL; p2= NULL;
	}
	munmap(buf,size);
	//end = clock();
	//printf("load : %lu [us]\n",end-start);
	deallocate(len);deallocate(seps);
	//deallocate(buf);
	//printf("row : %d, column : %d\n",data->row,data->column);
	return data;
}
*/

Data Data_fload(FILE *fp, int ofsr, int ofsc, int r, int c, char *sep){
	Data data;
	int i,j,k,cc,*len;
	char *buf,*p,*ends,*newl,*p2=NULL,*newl2=NULL;
	char **seps;

	if((buf = String_fread(fp))==NULL){return NULL;}
	
	ends = &buf[strlen(buf)];	
	p = String_skipchr(buf,'\n',ofsr);
	p = String_matchLine(p,sep,isFloat);
	
	while(*p !=' ' && *p != '\n'){
		putchar(*p); p++;
	}
	putchar('\n');
	
	if(r<0){
		r=String_countchr(p,'\n');
		r = r +1 - String_countBlankLinesAtEnd(p);
	}
	
	cc = String_countWordLine(p,sep);
	if(c<0){c=cc-ofsc;}
	
	data = Data_create(r,c);
	
	p = String_skipchr(buf,'\n',ofsr);
	newl = p = String_matchLine(p,sep,isFloat);
	
	seps = allocate(2*sizeof(char*));
	len = callocate(2,sizeof(int));
	seps[0]=sep; seps[1]="\n";
	
	for(i=0;i<r;i++){
		newl = strtok3(newl,&seps[1],len,1,&newl2);
		p = strtok3(newl,&seps[0],len,1,&p2);
		loadbar(i,r,50);
		for(j=0;j<ofsc;j++){p = strtok3(NULL,&seps[0],len,1,&p2);}
		for(j=0;j<c;j++){
			if(p){data->elem[i][j] = atof(p);}
			else{data->elem[i][j] = 0;}
			p=strtok3(NULL,&seps[0],len,1,&p2);
		}
		newl = newl2; newl2=NULL; p2= NULL;
	}
	deallocate(len);deallocate(seps);
	deallocate(buf);
	return data;
}

void ReadOptions_init(ReadOptions options){
	options->rows = AUTO;
	options->columns = AUTO;
	options->rowOffset = 0;
	options->columnOffset = 0;
	options->lineSeparators = NULL;
	options->lineSeparatorsCount = 0;
	options->fieldSeparators = NULL;
	options->fieldSeparatorsCount = 0;
	options->repeatedSeparators = TRUE;
	options->ignoreEmptyLines = FALSE;
}
void ReadOptions_finish(ReadOptions options){
	int i;
	for(i=0;i<options->lineSeparatorsCount;i++){deallocate(options->lineSeparators[i]);}
	deallocate(options->lineSeparators);
	for(i=0;i<options->fieldSeparatorsCount;i++){deallocate(options->fieldSeparators[i]);}
	free(options->fieldSeparators);
}

void ReadOptions_setLineSeparators(ReadOptions options, int lineSeparatorsCount, ...){
	int i;
	va_list list;
	
	options->lineSeparatorsCount = lineSeparatorsCount;
	options->lineSeparators = malloc(lineSeparatorsCount*sizeof(char*));
	va_start(list,lineSeparatorsCount);
	for(i=0;i<lineSeparatorsCount;i++){
		options->lineSeparators[i] = String_copy(va_arg(list,char*));
	}
	va_end(list);
}

void ReadOptions_setFieldSeparators(ReadOptions options, int fieldSeparatorsCount, ...){
	int i;
	va_list list;
	
	options->fieldSeparatorsCount = fieldSeparatorsCount;
	options->fieldSeparators = malloc(fieldSeparatorsCount*sizeof(char*));
	va_start(list,fieldSeparatorsCount);
	for(i=0;i<fieldSeparatorsCount;i++){
		options->fieldSeparators[i] = String_copy(va_arg(list,char*));
	}
	va_end(list);
}

void ReadOptions_print(ReadOptions options){
	int i;
	char *p;
	printf("rows : ");
	if(options->rows < 0){printf("Automatic\n");}
	else{printf("%d\n",options->rows);}
	printf("columns : ");
	if(options->columns < 0){printf("Automatic\n");}
	else{printf("%d\n",options->columns);}
	
	printf("row offset : %d\n",options->rowOffset);
	printf("column offset : %d\n",options->columnOffset);
	printf("line separators : ");
	printf("{");
	for(i=0;i<options->lineSeparatorsCount;i++){
		if(i>0){printf(",");}
		for(p=options->lineSeparators[i];*p;p++){
			switch(*p){
			  case '\r': printf("\\r"); break;
			  case '\n': printf("\\n"); break;
			  case '\t': printf("\\t"); break;
			  default: putchar(*p);
			}
		}
	}
	printf("}\n");
	printf("field separators : ");
	printf("{");
	for(i=0;i<options->fieldSeparatorsCount;i++){
		if(i>0){printf(",");}
		for(p=options->fieldSeparators[i];*p;p++){
			switch(*p){
			  case '\r': printf("\\r"); break;
			  case '\n': printf("\\n"); break;
			  case '\t': printf("\\t"); break;
			  default: putchar(*p);
			}
		}
	}
	printf("}\n");
	printf("repeatedSeparators : ");
	if(options->repeatedSeparators == TRUE){
		printf("True\n");
	}else{
		printf("False\n");
	}
	printf("ignoreEmptyLines : ");
	if(options->ignoreEmptyLines == TRUE){
		printf("True\n");
	}else{
		printf("False\n");
	}
}

Data Data_readFile(char *file, ReadOptions options){
	Data data;
	char *string;
	string = String_read(file);
	data = Data_read(string,options);
	deallocate(string);
	return data;
}

Data Data_read(char *string, ReadOptions options){
	StringList list,s,*listlist;
	int i,j,rows,columns,rofs,cofs,c,lsc,fsc;
	Data data=NULL;
	char *temp;
	
	//clock_t start,end;
	
	lsc=options->lineSeparatorsCount;
	for(i=0;i<lsc;i++){
		if(!strstr(string,options->lineSeparators[i])){
			temp = options->lineSeparators[i];
			options->lineSeparators[i] = options->lineSeparators[lsc-1];
			options->lineSeparators[lsc-1] = temp;
			lsc--;
			i--;
		}
	}
	fsc=options->fieldSeparatorsCount;
	for(i=0;i<fsc;i++){
		if(!strstr(string,options->fieldSeparators[i])){
			temp = options->fieldSeparators[i];
			options->fieldSeparators[i] = options->fieldSeparators[fsc-1];
			options->fieldSeparators[fsc-1] = temp;
			fsc--;
			i--;
		}
	}
	//printf("line separators count : %d\n",lsc);
	//printf("field separators count : %d\n",fsc);
	
	//start = clock();
	list = StringList_splitString(string,options->lineSeparators,lsc,options->ignoreEmptyLines);
	//end = clock();
	//printf("line split : %d\n",end-start);
	rows = LinkedList_getLength(list);
	
	if(options->rowOffset < 0){rofs = rows + options->rowOffset;}
	else{rofs = options->rowOffset;}
	
	if(options->rows < 0){
		if(rows < rofs){
			printf("Data_read : row offset %d exceeds the row count %d\n",rofs,rows);
			goto end;
		}
		rows = rows - rofs;
	}else{
		if(rows < options->rows + rofs){
			printf("Data_read : rows %d + row offset %d exceeds input data rows %d\n",options->rows,rofs,rows);
		}
		rows = options->rows;
	}
	
	listlist = allocate(rows*sizeof(StringList));
	
	//start = clock();
	for(i=0,columns=0,s=LinkedList_skip(list,rofs);i<rows && s;i++,s=s->next){
		listlist[i] = StringList_splitString(s->content,options->fieldSeparators,fsc,options->repeatedSeparators);
		c = LinkedList_getLength(listlist[i]);
		if(columns < c){columns=c;}
	}
	//end = clock();
	//printf("column split : %d\n",end-start);
	
	if(options->columnOffset < 0){cofs = columns + options->columnOffset;}
	else{cofs = options->columnOffset;}
	
	if(options->columns < 0){
		if(columns < cofs){
			printf("Data_read : column offset %d exceeds the input data columns%d\n",cofs,columns);
			goto end;
		}
		columns = columns - cofs;
	}else{
		if(columns < options->columns + cofs){
			printf("Data_read : columns %d + column offset %d exceeds input data columns %d\n",options->columns,cofs,columns);
		}
		columns = options->columns;
	}
	data = Data_create(rows,columns);
	
	for(i=0;i<rows;i++){
		for(j=0,s=LinkedList_skip(listlist[i],cofs);j<columns && s;j++,s=s->next){
			data->elem[i][j] = atof(s->content);
		}
	}
	
  end:
	for(i=0;i<rows;i++){LinkedList_deleteRoot(listlist[i],NULL);}
	deallocate(listlist);
	LinkedList_deleteRoot(list,NULL);
	return data;
}

double Data_get(Data data, int r, int c){
	/*
	if(r >= abs(data->row) || c >= abs(data->column)){
		printf("Data_get : invalid index\n");
		return 0;	
	}
	if(r < 0){r = r + data->row;}
	if(c < 0){c = c + data->column;}
	*/
	return data->elem[r][c];
} 

void Data_setall(Data data, double x){
	int i,j;
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column;j++){
			data->elem[i][j] = x;
		}
	}
}

void Data_set(Data data, int r, int c, double x){
	/*
	if(r >= abs(data->row) || c >= abs(data->column)){
		printf("Data_set : invalid index\n");
		return;	
	}
	if(r < 0){r = r + data->row;}
	if(c < 0){c = c + data->column;}
	*/
	data->elem[r][c] = x;
}

double Data_translate(Data data, int r, int c, Data data2, int r2, int c2){
	return data2->elem[r2][c2] = data->elem[r][c]; 
}
int Data_getSize(Data data){return data->row*data->column;}
int Data_getRow(Data data){return data->row;}
int Data_getColumn(Data data){return data->column;}

double Data_min(Data data, int c){
	int i;
	double min;
	
	min = data->elem[0][c];
	for(i=1;i<data->row;i++){
		if(min > data->elem[i][c]){
			min = data->elem[i][c];
		}
	}
	return min;
}

double Data_max(Data data, int c){
	int i;
	double max;
	
	max = data->elem[0][c];
	for(i=1;i<data->row;i++){
		if(max < data->elem[i][c]){
			max = data->elem[i][c];
		}
	}
	return max;
}

void Data_min2(Data data, int c, int imin, int imax, int *mini, double *min){
	int i;
	
	if(imin > imax){
		fprintf(stderr,"Data_min2 : rmin is bigger than rmax\n");
		return;
	}
	if(imin < 0){
		fprintf(stderr,"Data_min2 : rmin is smaller than 0\n");
		imin = 0;
	}
	if(imax > data->row){
		fprintf(stderr,"Data_min2 : rmax is bigger than row of data\n");
		imax = data->row;
	}
	
	*min = data->elem[imin][c];
	for(i=imin;i<=imax;i++){
		if(*min > data->elem[i][c]){
			*min = data->elem[i][c];
			*mini = i;
		}
	}
}

void Data_max2(Data data, int c, int imin, int imax, int *maxi, double *max){
	int i;
	
	if(imin > imax){
		fprintf(stderr,"Data_max2 : rmin is bigger than rmax\n");
	}
	if(imin < 0){
		fprintf(stderr,"Data_max2 : rmin is smaller than 0\n");
		imin = 0;
	}
	if(imax > data->row){
		fprintf(stderr,"Data_max2 : rmax is bigger than row of data\n");
		imax = data->row;
	}
	
	*max = data->elem[imin][c];
	for(i=imin;i<=imax;i++){
		if(*max < data->elem[i][c]){
			*max = data->elem[i][c];
			*maxi = i;
		}
	}
}

int Data_minIndex(Data data, int c){
	int i,mini;
	double min;
	
	min = data->elem[0][c];
	for(i=1;i<data->row;i++){
		if(min > data->elem[i][c]){
			min = data->elem[i][c];
			mini = i;
		}
	}
	return mini;
}

int Data_maxIndex(Data data, int c){
	int i,maxi;
	double max;
	
	max = data->elem[0][c];
	for(i=1;i<data->row;i++){
		if(max < data->elem[i][c]){
			max = data->elem[i][c];
			maxi = i;
		}
	}
	return maxi;
}

double Data_fwhm(Data data, double xmin, double xmax, double *xl, double *xr){
	int i,xmini,xmaxi,ymaxi;
	double diff,ymax,lxl,lxr;

	if(xmin==xmax){
		xmin = Data_min(data,0);
		xmax = Data_max(data,0);
	}
	for(i=data->row-1;i>=0;i--){
		if(xmin <= data->elem[i][0]){xmini=i;}
	}
	for(i=0;i<data->row;i++){
		if(data->elem[i][0] <= xmax){xmaxi=i;}
	}
	Data_max2(data,1,xmini,xmaxi,&ymaxi,&ymax);
	diff = data->elem[xmini][1]-ymax*0.5;
	for(i=xmini+1;i<=ymaxi;i++){
		if(diff*(data->elem[i][1]-ymax*0.5) < 0){
			lxl = data->elem[i][0];
			break;
		}
	}
	diff = data->elem[xmaxi][1]-ymax*0.5;
	for(i=xmaxi-1;i>=ymaxi;i--){
		if(diff*(data->elem[i][1]-ymax*0.5) < 0){
			lxr = data->elem[i][0];
			break;
		}
	}
	if(xl){*xl = lxl;}
	if(xr){*xr = lxr;}
	return fabs(lxr-lxl);
}

Data Data_shape(Data data, int r, int c){
	int i,j,k,n,oc=data->column,size=data->row*oc;
	Data ndata = Data_create(r,c);
	for(i=0;i<r;i++){
		k=i*c;
		for(j=0;j<c;j++){
			if((n=k+j)<size){ndata->elem[i][j]=data->elem[n/oc][n%oc];}
 			else{ndata->elem[i][j]=0;}
		}
	}
	return ndata;
}
void Data_print(Data data){
	int i,j;
	if(!data){return;}
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column;j++){
			printf("%e\t",data->elem[i][j]);
		}
		printf("\n");
	}
}

void Data_fprint(Data data, FILE *fp, char *sep){
	int i,j;
	if(!data){return;}
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column;j++){
			fprintf(fp,"%e%s",data->elem[i][j],sep);
		}
		fprintf(fp,"\n");
	}
}

void Data_output(Data data, char *fileout, enum precision p){
	FILE *fp=fopen(fileout,"wb");
	int w1,i,j;
	float *f;
	if(!fp){fprintf(stderr,"Data_output : fopen error\n"); exit(1);}
	fwrite(&p,sizeof(enum precision),1,fp);
	fwrite(&data->row,sizeof(int),1,fp);
	fwrite(&data->column,sizeof(int),1,fp);
	switch(p){
	  case p_double:
		if((w1=fwrite(data->elem[0],sizeof(double),data->row*data->column,fp))!=data->row*data->column){
			fprintf(stderr,"Data_output : output error. trying to write %d bytes, but %d bytes are written \n",data->row*data->column*8,w1*8); exit(1);
		}
		break;
	  case p_float:
        f = allocate(data->row*data->column*sizeof(float));
        for(i=0;i<data->row;i++){
			for(j=0;j<data->column;j++){
				f[i*data->column+j]=data->elem[i][j];
			}
		}
        if((w1=fwrite(f,sizeof(float),data->row*data->column,fp))!=data->row*data->column){
            fprintf(stderr,"Data_output : output error. trying to write %d bytes, but %d bytes are written \n",data->row*data->column*4,w1*4); exit(1);
        }
        deallocate(f);
		break;
	}
	fclose(fp);
}

Data Data_input(char *filein){
	Data data=NULL;
	unsigned long size = getFileSizeBin(filein);
	FILE *fp=fopen(filein,"rb");
	int r=0,c=0,count=0,i,j;
	float *f;
	enum precision p;
	if(!fp){fprintf(stderr,"Data_input : fopen error\n"); exit(1);}
	fread(&p,sizeof(enum precision),1,fp);
	if(p != p_double && p != p_float){fprintf(stderr,"Data_input : data type infomation is corrupted.\n"); exit(1);}
	fread(&r,sizeof(int),1,fp);
	fread(&c,sizeof(int),1,fp);
	//fprintf(stderr,"row : %d\n",r);
	//fprintf(stderr,"column : %d\n",c);
	switch(p){
	  case p_double:
		if(size!=sizeof(enum precision)+2*sizeof(int)+r*c*sizeof(double)){
			fprintf(stderr,"Data_input : data size should be %lu byte, but is %lu byte\n",sizeof(enum precision)+2*sizeof(int)+r*c*sizeof(double),size);
			return NULL;
		}
		data = Data_create(r,c);
		fread(data->elem[0],sizeof(double),r*c,fp);	
		break;
	  case p_float:
		if(size!=sizeof(enum precision)+2*sizeof(int)+r*c*sizeof(float)){
			fprintf(stderr,"Data_input : data size should be %lu byte, but is %lu byte\n",sizeof(enum precision)+2*sizeof(int)+r*c*sizeof(float),size);
		}
		data = Data_create(r,c);
        f = allocate(r*c*sizeof(float));
        fread(f,sizeof(float),r*c,fp);
        for(i=0;i<data->row;i++){
			for(j=0;j<data->column;j++){
				data->elem[i][j] = f[i*c+j];
			}
		}
        deallocate(f);
		break;
	}
	fclose(fp);
	return data;
}
Data Data_loadBMAT(char *filein){return Data_input(filein);}
Data Data_loadBARY(char *filein){
	FILE *fp=fopen(filein,"rb");
	Data data;
	int depth=0,dim[2]={0,0},len,i,j;
	unsigned long size = getFileSizeBin(filein);
	float *f;
	double *elem;
	enum precision p;
	if(!fp){fprintf(stderr,"Data_loadBARY : fopen error\n"); exit(1);}
	fread(&p,sizeof(enum precision),1,fp);
	if(p != p_double && p != p_float){fprintf(stderr,"Data_loadBARY : array type infomation is corrupted.\n"); exit(1);}
	fread(&depth,sizeof(int),1,fp);
	if(depth > 2){
		fprintf(stderr,"Data_loadBARY : array dimension is more than 2. This data cannot be converted into Data format\n"); exit(1);
	}
	fread(dim,sizeof(int),2,fp);
	for(i=0,len=1;i<depth;i++){
		len *= dim[i];
	}
	elem = allocate(len*sizeof(double));
	switch(p){
	  case p_double:
		if(size != sizeof(enum precision) + depth*sizeof(int) + sizeof(int) + len*sizeof(double)){
			fprintf(stderr,"Data_loadBARY : array size should be %lu byte, but is %lu byte\n",sizeof(enum precision) + depth*sizeof(int) + sizeof(int) + len*sizeof(double),size);
			return NULL;
		}
		fread(elem,sizeof(double),len,fp);
		break;
	  case p_float:
		if(size != sizeof(enum precision) + depth*sizeof(int) + sizeof(int) + len*sizeof(float)){
			fprintf(stderr,"Data_loadBARY : array size should be %lu byte, but is %lu byte\n",sizeof(enum precision) + depth*sizeof(int) + sizeof(int) +len*sizeof(float),size);
			return NULL;
		}
		f = allocate(len*sizeof(float));
		fread(f,sizeof(float),len,fp);
		for(i=0;i<len;i++){elem[i] = f[i];}
		deallocate(f);
		break;
	}
	data = Data_create(dim[1],dim[0]);
	for(i=0;i<dim[1];i++){
		for(j=0;j<dim[0];j++){
			Data_set(data,i,j,elem[i*dim[0]+j]);
		}
	}
	deallocate(elem);
	fclose(fp);
	return data;
}
int isEqualDimension(Data a, Data b);
int isEqualDimension(Data a, Data b){
	if((a->row != b->row) || (a->column != b->column)){return 0;
	}else{return 1;}
}

#define checkcombine \
{\
	if(!a || !b){return NULL;}\
	if(!isEqualDimension(a,b)){\
		printf("Data_sub : Data with unequal dimensions cannot be combined\n");\
		return NULL;\
	}\
	if(tg){\
		if(!isEqualDimension(a,tg)){\
			printf("Data_sub : Data with unequal dimensions cannot be combined\n");\
			return NULL;\
		}\
	}else{tg = Data_copy(a);}\
}

Data Data_rslice(Data data, int cs, int ce){
	int i,j;
	double temp;
	Data data2;
	if((cs < 0 || ce > data->column) || ce < cs){printf("Data_rslice : index error\n"); return NULL;}
	data2 = Data_create(data->row,2);
	for(i=0;i<data->row;i++){
		temp = 0;
		for(j=cs;j<=ce;j++){
			temp += data->elem[i][j];
		}
		data2->elem[i][0] = i;
		data2->elem[i][1] = temp;
	}
	for(i=0;i<data->row;i++){
		data2->elem[i][1] /= (double)(ce-cs+1);
	}
	return data2;
}

Data Data_cslice(Data data, int rs, int re){
	int i,j;
	double temp;
	Data data2;
	if((rs < 0 || re > data->row) || re < rs){printf("Data_rslice : index error\n"); return NULL;}
	data2 = Data_create(data->column,2);
	
	for(j=0;j<data->column;j++){
		temp = 0;
		for(i=rs;i<=re;i++){
			temp += data->elem[i][j];
		}
		data2->elem[j][0] = j;
		data2->elem[j][1] = temp;
	}
	for(i=0;i<data->column;i++){
		data2->elem[i][1] /= (double)(re-rs+1);
	}
	return data2;
}
void Data_plot(Data data, int rs, int cs, int re, int ce, char *comfile){
	int i,j,len;
	double ratio,x;
	int r,c;
	FILE *gp;
	
	r = Data_getRow(data)-1;
	c = Data_getColumn(data)-1;
	
	if(re < 0){re = r;}
	if(ce < 0){ce = c;}
	
	gp = popen("gnuplot -persist\n","w");
	if(gp == NULL){perror("gnuplot");return;}
	ratio = (re-rs+1)/(ce-cs+1);
	fprintf(gp,"set size ratio %f\n",ratio);
	fprintf(gp,"set xr[%d:%d]\n",cs,ce);
	fprintf(gp,"set yr[%d:%d]\n",rs,re);
	if(comfile){fprintf(gp,"%s",comfile);}
	//if(comfile){fprintf(gp,"load \"%s\"\n",comfile);}
	fprintf(gp,"plot '-' binary array=(%d,%d) format=\"%%float64\" endian=little w image notitle\n",ce-cs+1,re-rs+1);
	for(i=rs;i<=re;i++){
		for(j=cs;j<=ce;j++){
			x = Data_get(data,i,j);
			fwrite(&x,sizeof(double),1,gp);
		}
	}
	pclose(gp);
}
/*suspended*/
Data Data_slice(Data data, int rs, int cs, int re, int ce){
	return data;
}

Data Data_copy(Data s){
	int i,j;
	Data data;
	if(!s){return s;}
	data = Data_create(s->row,s->column);
	for(i=0;i<s->row;i++){
		for(j=0;j<s->column;j++){
			data->elem[i][j] = s->elem[i][j];
		}
	}
	return data;
}

Data Data_add(Data a, Data b, Data tg, int rs, int cs, int re, int ce){
	int i,j;
	checkcombine;
	if(re<0){re=a->row-1;}
	if(ce<0){ce=a->column-1;}
	for(i=rs;i<=re;i++){
		for(j=cs;j<=ce;j++){
			tg->elem[i][j] = a->elem[i][j] + b->elem[i][j];
		}
	}
	return tg;
}

Data Data_sub(Data a, Data b, Data tg, int rs, int cs, int re, int ce){
	int i,j;
	checkcombine;
	if(re<0){re=a->row-1;}
	if(ce<0){ce=a->column-1;}
	for(i=rs;i<=re;i++){
		for(j=cs;j<=ce;j++){
			tg->elem[i][j] = a->elem[i][j] - b->elem[i][j];
		}
	}
	return tg;
}
Data Data_mul(Data a, Data b, Data tg, int rs, int cs, int re, int ce){
	int i,j;
	checkcombine;
	if(re<0){re=a->row-1;}
	if(ce<0){ce=a->column-1;}
	for(i=rs;i<=re;i++){
		for(j=cs;j<=ce;j++){
			tg->elem[i][j] = a->elem[i][j] * b->elem[i][j];
		}
	}
	return tg;
	return tg;
}
Data Data_pow(Data a, Data b, Data tg, int rs, int cs, int re, int ce){
	int i,j;
	checkcombine;
	if(re<0){re=a->row-1;}
	if(ce<0){ce=a->column-1;}
	for(i=rs;i<=re;i++){
		for(j=cs;j<=ce;j++){
			tg->elem[i][j] = pow(a->elem[i][j],b->elem[i][j]);
		}
	}
	return tg;
	return tg;
}

#define checkcombine2 \
{\
	if(tg){\
		if(!isEqualDimension(a,tg)){\
			printf("Data with unequal dimensions cannot be combined\n");\
			return NULL;\
		}\
	}else{tg = Data_create(a->row, a->column);}\
}

Data Data_adds(Data a, double b, Data tg){
	int i,j;
	checkcombine2;
	for(i=0;i<a->row;i++){
		for(j=0;j<a->column;j++){
			tg->elem[i][j] = a->elem[i][j] + b;
		}
	}
	return tg;
}
Data Data_subs(Data a, double b, Data tg){
	int i,j;
	checkcombine2;
	for(i=0;i<a->row;i++){
		for(j=0;j<a->column;j++){
			tg->elem[i][j] = a->elem[i][j] - b;
		}
	}
	return tg;
}

Data Data_muls(Data a, double b, Data tg){
	int i,j;
	checkcombine2;
	for(i=0;i<a->row;i++){
		for(j=0;j<a->column;j++){
			tg->elem[i][j] = a->elem[i][j] * b;
		}
	}
	return tg;
}

Data Data_pows(Data a, double b, Data tg){
	int i,j;
	checkcombine2;
	for(i=0;i<a->row;i++){
		for(j=0;j<a->column;j++){
			tg->elem[i][j] = pow(a->elem[i][j],b);
		}
	}
	return tg;
}

Data Data_radd(Data a, int index, double b){
	int i;
	if(abs(index) >= a->row){printf("Data_radd : invalid index\n"); return NULL;}
	if(index < 0){index += a->row;}
	for(i=0;i<a->column;i++){a->elem[index][i] += b;}
	return a;
}

Data Data_cadd(Data a, int index, double b){
	int i;
	if(abs(index) >= a->column){printf("Data_cadd : invalid index\n"); return NULL;}
	if(index < 0){index += a->column;}
	for(i=0;i<a->row;i++){a->elem[i][index] += b;}
	return a;
}

Data Data_rmul(Data a, int index, double b){
	int i;
	if(abs(index) >= a->row){printf("Data_rmul : invalid index\n"); return NULL;}
	if(index < 0){index += a->row;}
	for(i=0;i<a->column;i++){a->elem[index][i] *= b;}
	return a;
}

Data Data_cmul(Data a, int index, double b){
	int i;
	if(abs(index) >= a->column){printf("Data_cmul : invalid index\n"); return NULL;}
	if(index < 0){index += a->column;}
	for(i=0;i<a->row;i++){a->elem[i][index] *= b;}
	return a;
}

Data Data_rpow(Data a, int index, double b){
	int i;
	if(abs(index) >= a->row){printf("Data_rpow : invalid index\n"); return NULL;}
	if(index < 0){index += a->row;}
	for(i=0;i<a->column;i++){a->elem[index][i] = pow(a->elem[index][i],b);}
	return a;
}

Data Data_cpow(Data a, int index, double b){
	int i;
	if(abs(index) >= a->column){printf("Data_cpow : invalid index\n"); return NULL;}
	if(index < 0){index += a->column;}
	for(i=0;i<a->row;i++){a->elem[i][index] = pow(a->elem[i][index],b);}
	return a;
}

Data Data_rswap(Data a, int r1, int r2){
	int i;
	double tmp;
	if(abs(r1) >= a->row || abs(r2) >= a->row){printf("Data_rswap : invalid index\n"); return NULL;}
	if(r1<0){r1 += a->row;} if(r2<0){r2 += a->row;}
	for(i=0;i<a->column;i++){
		tmp = a->elem[r1][i];
		a->elem[r1][i] = a->elem[r2][i];
		a->elem[r2][i] = tmp;
	}
	return a;
}

Data Data_cswap(Data a, int c1, int c2){
	int i;
	double tmp;
	if(abs(c1) >= a->column || abs(c2) >= a->column){printf("Data_cswap : invalid index\n"); return NULL;}
	if(c1<0){c1 += a->column;} if(c2<0){c2 += a->column;}
	for(i=0;i<a->row;i++){
		tmp = a->elem[i][c1];
		a->elem[i][c1] = a->elem[i][c2];
		a->elem[i][c2] = tmp;
	}
	return a;
}

Data Data_rappend(Data data, Data dataap){
	int i,j,n;
	Data datanew = Data_create(data->row+dataap->row,data->column);
	
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column;j++){
			datanew->elem[i][j] = data->elem[i][j];
		}
	}
	
	for(i=0;i<dataap->row;i++){
		for(j=0;j<data->column;j++){
			if(j<dataap->column){
				datanew->elem[i+data->row][j] = dataap->elem[i][j];
			}else{
				datanew->elem[i+data->row][j] = 0;
			}
		}
	}
	return datanew;
}

Data Data_cappend(Data data, Data dataap){
	int i,j,n;
	Data datanew = Data_create(data->row,data->column + dataap->column);
	
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column;j++){
			datanew->elem[i][j] = data->elem[i][j];
		}
	}
	
	for(i=0;i<data->row;i++){
		for(j=0;j<dataap->column;j++){
			if(i<dataap->row){
				datanew->elem[i][j+data->column] = dataap->elem[i][j];
			}else{
				datanew->elem[i][j+data->column] = 0;
			}
		}
	}
	return datanew;
}

Data Data_rtake(Data data, int rs, int re){
	int i,j;
	Data datan;
	if(rs < 0 || re > data->row || rs > re){
		printf("Data_rtake : invalid index\n"); return NULL;
	}
	datan = Data_create(re-rs+1,data->column);
	for(j=0;j<data->column;j++){
		for(i=rs;i<re;i++){
			datan->elem[i-rs][j] = data->elem[i][j];
		}
	}
	return datan;
}

Data Data_ctake(Data data, int cs, int ce){
	int i,j;
	Data datan;
	if(cs < 0 || ce > data->column || cs > ce){
		printf("Data_ctake : invalid index\n"); return NULL;
	}
	datan = Data_create(data->row,ce-cs+1);
	for(i=0;i<data->row;i++){
		for(j=cs;j<ce;j++){
			datan->elem[i][j-cs] = data->elem[i][j];
		}
	}
	return datan;	
}

Data Data_histogram(Data data, double min, double max, int binc){
	int i,j;
	double *weight = allocate(binc*sizeof(double));
	double dx = (max-min)/binc;
	double coeff = 1.0/dx;
	
	Data data2 = Data2D_create(binc);
	
	for(i=0;i<binc;i++){
		data2->elem[i][0] = min + (i+1)*dx;
		weight[i] = 0;
	}
	
	for(i=0;i<data->row;i++){
		if((j=floor((data->elem[i][0]-min)/dx))<binc && j >= 0){
			weight[j] += data->elem[i][1];
		}
	}
	for(i=0;i<binc;i++){data2->elem[i][1] = weight[i]*coeff;}
	deallocate(weight);
	return data2;
}


Data Data_histogram2D(Data data, double min1, double min2, double max1, double max2, int binc1, int binc2){
	int i,j,k, binc = binc1*binc2;
	double *weight = allocate(binc*sizeof(double));
	double dx = (max1-min1)/binc1;
	double dy = (max2-min2)/binc2;
	double coeff = 1.0/(dx*dy);
	
	Data data2 = Data_create(binc,3);
	for(i=0;i<binc;i++){
		data2->elem[i][0] = min1 + (i/binc2+1)*dx;
		data2->elem[i][1] = min2 + (i%binc2+1)*dy;
		weight[i] = 0;
	}
	
	for(i=0;i<data->row;i++){
		if((j=floor((data->elem[i][0]-min1)/dx))<binc1 && j >= 0){
			if((k=floor((data->elem[i][1]-min2)/dy))<binc2 && k >= 0){
				weight[binc2*j+k] += data->elem[i][2];
				/*printf("weight[%d] : %f\n",j+binc2*k,weight[j+binc2*k]);*/
			}
		}
	}
	
	/*printf("binc %d\n",binc);*/
	
	for(i=0;i<binc;i++){data2->elem[i][2] = weight[i]*coeff; /*printf("%f\n",data2->elem[i][2]);*/}
	deallocate(weight);
	return data2;
}

/*Data2D*/

Data2D Data2D_create(int r){return Data_create(r,2);}
void Data2D_delete(Data2D data){Data_delete(data);}
Data2D Data2D_load(char *file, int ofsr, int ofsc, int r, char *sep){
	return Data_load(file,ofsr,ofsc,r,2,sep);
}


void bitrv2(int n, double *a);
void bitrv2conj(int n, double *a);
void cftfsub(int n, double *a);
void cftbsub(int n, double *a);
void cft1st(int n, double *a);
void cftmdl(int n, int l, double *a);

/*Copyright(C) 1996-2001 Takuya OOURA*/	
Data Data_fourier(Data data, int isgn){
	/*input*/
	/*row : n (prefer 2^k)*/
	/*column : 2*/
	/*elem[i][0] : Re*/
	/*elem[i][1] : Im*/
	
	/*if n is not equal to 2^k zero is automatically filled*/
	Data datao;
	int i,n;
	double *a;
	for(n=1;data->row>n;n<<=1){}
	//printf("size:%d\tn:%d\n",data->row,n);
	a = allocate(2*n*sizeof(double));
	for(i=0;i<data->row;i++){
		a[2*i] = data->elem[i][0];
		a[2*i+1] = data->elem[i][1];
	}
	/*padding*/
	for(i=data->row;i<n;i++){
		a[2*i] = 0;
		a[2*i+1] = 0;
	}
	/*
	for(i=0;i<n;i++){
		printf("a[%d]\ta[%d] : %e\t%e\n",2*i, 2*i+1, a[2*i],a[2*i+1]);
	}
	*/
	if(n>4){
		if(isgn >= 0){
			bitrv2(2*n,a);
			cftfsub(2*n,a);			
		}else{
			bitrv2conj(2*n,a);
			cftbsub(2*n,a);
			for(i=0;i<2*n;i++){a[i] /= n;}
		}
	}else if(n==4){cftfsub(n,a);}
	/*
	for(i=0;i<n;i++){
		printf("b[%d]\tb[%d] : %e\t%e\n",2*i, 2*i+1, a[2*i],a[2*i+1]);
	}
	*/
	datao = Data_create(n,2);
	for(i=0;i<n;i++){
		datao->elem[i][0] = a[2*i];
		datao->elem[i][1] = a[2*i+1];
	}
	deallocate(a);
	return datao;
}

Data Data_fourier2(Data data, int isgn, int k){
	/*input*/
	/*row : n (prefer 2^k)*/
	/*column : 2*/
	/*elem[i][0] : Re*/
	/*elem[i][1] : Im*/
	
	/*if n is not equal to 2^k zero is automatically filled*/
	Data datao;
	int i,n;
	double *a;
	n=1<<k;
	if(k<=0 || n<=data->row){
		//fprintf("Data_fourier2 : bincout : %d is too small.\n",k);
		return Data_fourier(data,isgn);
	}
	//printf("size:%d\tn:%d\n",data->row,n);
	a = allocate(2*n*sizeof(double));
	for(i=0;i<data->row;i++){
		a[2*i] = data->elem[i][0];
		a[2*i+1] = data->elem[i][1];
	}
	/*padding*/
	for(i=data->row;i<n;i++){
		a[2*i] = 0;
		a[2*i+1] = 0;
	}
	/*
	for(i=0;i<n;i++){
		printf("a[%d]\ta[%d] : %e\t%e\n",2*i, 2*i+1, a[2*i],a[2*i+1]);
	}
	*/
	if(n>4){
		if(isgn >= 0){
			bitrv2(2*n,a);
			cftfsub(2*n,a);
		}else{
			bitrv2conj(2*n,a);
			cftbsub(2*n,a);
			for(i=0;i<2*n;i++){a[i] /= n;}
		}
	}else if(n==4){cftfsub(n,a);}
	/*
	for(i=0;i<n;i++){
		printf("b[%d]\tb[%d] : %e\t%e\n",2*i, 2*i+1, a[2*i],a[2*i+1]);
	}
	*/
	datao = Data_create(n,2);
	for(i=0;i<n;i++){
		datao->elem[i][0] = a[2*i];
		datao->elem[i][1] = a[2*i+1];
	}
	deallocate(a);
	return datao;
}

/*F2Data : row = column = 2*n */
/*elem[2*i] : Real Part*/
/*elem[2*i+1] : Imaginal Part*/

F2Data F2Data_convert(Data data, int k){
	int i,j,n,max;
	F2Data f2data;
	max = data->row > data->column ? data->row : data->column;
	
	/*if n is power of 2*/
	n=1<<k;
	if(k<=0 || n<=max){
		for(n=1;n<max;n<<=1){}
	}
	f2data = Data_create(n,2*n);
	for(i=0;i<n;i++){
		for(j=0;j<2*n;j++){
			f2data->elem[i][j] = 0;
		}
	}
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column;j++){
			f2data->elem[i][2*j] = data->elem[i][j];
			f2data->elem[i][2*j+1] = 0;
		}
	}
	return f2data;
}

void F2Data_delete(F2Data data){Data_delete(data);}

Data F2Data_powerSpectrum(F2Data f2data){
	int i,j,n;
	double re,im;
	Data data;
	
	n = f2data->row/2;
	data = Data_create(2*n,2*n);
	
	for(i=0;i<n;i++){
		for(j=0;j<n;j++){
			re = f2data->elem[i][2*j];
			im = f2data->elem[i][2*j+1];
			data->elem[n+i][n+j] = sqrt(re*re+im*im);
		}
	}
	for(i=n;i<2*n;i++){
		for(j=0;j<n;j++){
			re = f2data->elem[i][2*j];
			im = f2data->elem[i][2*j+1];
			data->elem[i-n][n+j] = sqrt(re*re+im*im);
		}
	}
	for(i=0;i<n;i++){
		for(j=n;j<2*n;j++){
			re = f2data->elem[i][2*j];
			im = f2data->elem[i][2*j+1];
			data->elem[n+i][j-n] = sqrt(re*re+im*im);
		}
	}
	for(i=n;i<2*n;i++){
		for(j=n;j<2*n;j++){
			re = f2data->elem[i][2*j];
			im = f2data->elem[i][2*j+1];
			data->elem[i-n][j-n] = sqrt(re*re+im*im);
		}
	}
	return data;
}

void F2Data_transpose(F2Data f2data){
	int i,j,n;
	double temp;
	n = f2data->row;
	
	for(i=0;i<n;i++){
		for(j=0;j<i;j++){
			temp = f2data->elem[i][2*j];
			f2data->elem[i][2*j] = f2data->elem[j][2*i];
			f2data->elem[j][2*i] = temp;
			temp = f2data->elem[i][2*j+1];
			f2data->elem[i][2*j+1] = f2data->elem[j][2*i+1];
			f2data->elem[j][2*i+1] = temp;
		}
	}
}

void Data_fourier2D(F2Data f2data, int isgn){
	/*input*/
	/*row : 2*n (n = 2^k)*/
	/*column : 2*n*/
	/*elem[2*i] : Real part*/
	/*elem[2*i+1] : Imaginal part*/
	
	int i,j,n;
	n = f2data->row;
	
	if(n>4){
		if(isgn >= 0){
			for(i=0;i<n;i++){
				bitrv2(2*n,f2data->elem[i]);
				cftfsub(2*n,f2data->elem[i]);
			}
		}else{
			for(i=0;i<n;i++){
				bitrv2conj(2*n,f2data->elem[i]);
				cftbsub(2*n,f2data->elem[i]);
			}
			for(i=0;i<n;i++){
				for(j=0;j<2*n;j++){f2data->elem[i][j] /= n;}
			}
		}
	}else if(n==4){
		for(i=0;i<n;i++){cftfsub(n,f2data->elem[i]);}
	}
	F2Data_transpose(f2data);
	if(n>4){
		if(isgn >= 0){
			for(i=0;i<n;i++){
				bitrv2(2*n,f2data->elem[i]);
				cftfsub(2*n,f2data->elem[i]);
			}
		}else{
			for(i=0;i<n;i++){
				bitrv2conj(2*n,f2data->elem[i]);
				cftbsub(2*n,f2data->elem[i]);
			}
			for(i=0;i<n;i++){
				for(j=0;j<2*n;j++){f2data->elem[i][j] /= n;}
			}
		}
	}else if(n==4){
		for(i=0;i<n;i++){cftfsub(n,f2data->elem[i]);}
	}
	F2Data_transpose(f2data);
}

Data Data_transpose(Data data){
	Data data2 = NULL;
	int i,j;
	
	data2 = Data_create(data->column,data->row);
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column;j++){
			data2->elem[j][i] = data->elem[i][j];
		}
	}
	return data2;
}

Data Data_rrotate(Data data){
	Data data2 = NULL;
	int i,j;
	
	data2 = Data_create(data->column,data->row);
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column;j++){
			data2->elem[j][data->row-1-i] = data->elem[i][j];
		}
	}
	return data2;
}

Data Data_lrotate(Data data){
	Data data2 = NULL;
	int i,j;
	
	data2 = Data_create(data->column,data->row);
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column-1;j++){
			data2->elem[data->column-1-j][i] = data->elem[i][j];
		}
	}
	return data2;
}

void Data_vreflect(Data data){
	int i,j,k;
	double temp;
	
	for(i=0;i<data->column;i++){
		for(j=0,k=data->row-1;j<k;j++,k--){
			temp = data->elem[j][i];
			data->elem[j][i] = data->elem[k][i];
			data->elem[k][i] = temp;
		}
	}
}

void Data_hreflect(Data data){
	int i,j,k;
	double temp;
	
	for(i=0;i<data->row;i++){
		for(j=0,k=data->column-1;j<k;j++,k--){
			temp = data->elem[i][j];
			data->elem[i][j] = data->elem[i][k];
			data->elem[i][k] = temp;
		}
	}
}

//double square_trim(double r,double th,int x,int y,char corners){}

/*corners*/
/*0000 : none*/
/*1111 : full*/
/*0001 : (00)*/
/*0010 : (01)*/
/*0100 : (10)*/
/*1000 : (11)*/

enum en_corners{
	co_none = 0x00,
	co_full = 0x0f,
	
	co_00 = 0x01,
	co_01 = 0x02,
	co_10 = 0x04,
	co_11 = 0x08,
	
	co_m00 = 0x0e, /*1110*/
	co_m01 = 0x0d, /*1101*/
	co_m10 = 0x0b, /*1011*/
	co_m11 = 0x07, /*0111*/
	
	co_00_01 = 0x03, /*0011*/
	co_00_10 = 0x05, /*0101*/
	co_11_01 = 0x0a, /*1010*/
	co_11_10 = 0x0c, /*1100*/
	
	co_00_11 = 0x09, /*1001*/
	co_10_01 = 0x06, /*0110*/
};

#define cross_x(r,y) (((r)-(y)*sinth)/costh)
#define cross_y(r,x) (((r)-(x)*costh)/sinth)
/*
double Data_lineIntegrate(Data data, double r, double th, double (*func)(Data data, int i, int j, double th)){
	int i,j,k,x,y;
	char corners=0;
	double center[2],costh,sinth,proj,area,rm,rp;
	
	center[0] = data->row & 1 ? (data->row - 1)/2 : data->row/2 - 0.5;
	center[1] = data->column & 1 ? (data->column - 1)/2 : data->column/2 - 0.5;
	
	costh = cos(th);
	sinth = sin(th);
	rm = r - 0.5;
	rp = r + 0.5;
	
	for(i=0;i<data->row;i++){
		for(j=0;j<data->column;j++){
			x = i - center[0];
			y = j - center[1];
			corners = 0
			proj = (x-0.5) costh + (y-0.5) sinth;
			if(rm <= proj && proj <= rp){
				corners |= co_00;
			}
			proj = (x-0.5) costh + (y+0.5) sinth;
			if(rm <= proj && proj <= rp){
				corners |= co_01;
			}
			proj = (x+0.5) costh + (y-0.5) sinth;
			if(rm <= proj && proj <= rp){
				corners |= co_10;
			}
			proj = (x+0.5) costh + (y+0.5) sinth;
			if(rm <= proj && proj <= rp){
				corners |= co_11;
			}
			switch(corners){
			  case co_none:
				area=0;
				break;
			  case co_full:
				area=1;
				break;
			  case co_00:
				area = fabs( 0.5 * (cross_x(y-0.5,rp) - (x-0.5)) * (cross_y(x-0.5,rp) - (y-0.5)) );
				break;
			  case co_01:
				area = fabs( 0.5 * (cross_x(y+0.5,rp) - (x-0.5)) * (cross_y(x-0.5,rp) - (y+0.5)) );
				break;
			  case co_10:
				area = fabs( 0.5 * (cross_x(y-0.5,rm) - (x+0.5)) * (cross_y(x+0.5,rm) - (y-0.5)) );
				break;
			  case co_11:
				area = fabs( 0.5 * (cross_x(y+0.5,rm) - (x+0.5)) * (cross_y(x+0.5,rm) - (y+0.5)) );
				break;
			  case co_m00:
				break;
			  case co_m01:
				break;
			  case co_m10:
				break;
			  case co_m11:
				break;
				
			  default:
				break;
			}
		}
	}
}
*/

/*
-------- Complex DFT (Discrete Fourier Transform) --------
    [definition]
        <case1>
            X[k] = sum_j=0^n-1 x[j]*exp(2*pi*i*j*k/n), 0<=k<n
        <case2>
            X[k] = sum_j=0^n-1 x[j]*exp(-2*pi*i*j*k/n), 0<=k<n
        (notes: sum_j=0^n-1 is a summation from j=0 to n-1)
    [usage]
        <case1>
            cdft(2*n, 1, a);
        <case2>
            cdft(2*n, -1, a);
    [parameters]
        2*n            :data length (int)
                        n >= 1, n = power of 2
        a[0...2*n-1]   :input/output data (double *)
                        input data
                            a[2*j] = Re(x[j]), 
                            a[2*j+1] = Im(x[j]), 0<=j<n
                        output data
                            a[2*k] = Re(X[k]), 
                            a[2*k+1] = Im(X[k]), 0<=k<n
    [remark]
        Inverse of 
            cdft(2*n, -1, a);
        is 
            cdft(2*n, 1, a);
            for (j = 0; j <= 2 * n - 1; j++) {
                a[j] *= 1.0 / n;
            }
        .
*/

#ifndef M_PI_2
#define M_PI_2      1.570796326794896619231321691639751442098584699687
#endif
#ifndef WR5000  /* cos(M_PI_2*0.5000) */
#define WR5000      0.707106781186547524400844362104849039284835937688
#endif
#ifndef WR2500  /* cos(M_PI_2*0.2500) */
#define WR2500      0.923879532511286756128183189396788286822416625863
#endif
#ifndef WI2500  /* sin(M_PI_2*0.2500) */
#define WI2500      0.382683432365089771728459984030398866761344562485
#endif


#ifndef RDFT_LOOP_DIV  /* control of the RDFT's speed & tolerance */
#define RDFT_LOOP_DIV 64
#endif

#ifndef DCST_LOOP_DIV  /* control of the DCT,DST's speed & tolerance */
#define DCST_LOOP_DIV 64
#endif

void bitrv2(int n, double *a){
    int j0, k0, j1, k1, l, m, i, j, k;
    double xr, xi, yr, yi;
    
    l = n >> 2;
	m = 2;
	
	while (m < l) {
        l >>= 1;
        m <<= 1;
	}
    if (m == l) {
        j0 = 0;
        for (k0 = 0; k0 < m; k0 += 2) {
            k = k0;
            for (j = j0; j < j0 + k0; j += 2) {
                xr = a[j];
                xi = a[j + 1];
                yr = a[k];
                yi = a[k + 1];
                a[j] = yr;
                a[j + 1] = yi;
                a[k] = xr;
                a[k + 1] = xi;
                j1 = j + m;
                k1 = k + 2 * m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 -= m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 += 2 * m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                for (i = n >> 1; i > (k ^= i); i >>= 1);
            }
            j1 = j0 + k0 + m;
            k1 = j1 + m;
            xr = a[j1];
            xi = a[j1 + 1];
            yr = a[k1];
            yi = a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            for (i = n >> 1; i > (j0 ^= i); i >>= 1);
        }
    } else {
        j0 = 0;
        for (k0 = 2; k0 < m; k0 += 2) {
            for (i = n >> 1; i > (j0 ^= i); i >>= 1);
            k = k0;
            for (j = j0; j < j0 + k0; j += 2) {
                xr = a[j];
                xi = a[j + 1];
                yr = a[k];
                yi = a[k + 1];
                a[j] = yr;
                a[j + 1] = yi;
                a[k] = xr;
                a[k + 1] = xi;
                j1 = j + m;
                k1 = k + m;
                xr = a[j1];
                xi = a[j1 + 1];
                yr = a[k1];
                yi = a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                for (i = n >> 1; i > (k ^= i); i >>= 1);
            }
        }
    }
}

void bitrv2conj(int n, double *a)
{
    int j0, k0, j1, k1, l, m, i, j, k;
    double xr, xi, yr, yi;
    
    l = n >> 2;
    m = 2;
    while (m < l) {
        l >>= 1;
        m <<= 1;
    }
    if (m == l) {
        j0 = 0;
        for (k0 = 0; k0 < m; k0 += 2) {
            k = k0;
            for (j = j0; j < j0 + k0; j += 2) {
                xr = a[j];
                xi = -a[j + 1];
                yr = a[k];
                yi = -a[k + 1];
                a[j] = yr;
                a[j + 1] = yi;
                a[k] = xr;
                a[k + 1] = xi;
                j1 = j + m;
                k1 = k + 2 * m;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 -= m;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                j1 += m;
                k1 += 2 * m;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                for (i = n >> 1; i > (k ^= i); i >>= 1);
            }
            k1 = j0 + k0;
            a[k1 + 1] = -a[k1 + 1];
            j1 = k1 + m;
            k1 = j1 + m;
            xr = a[j1];
            xi = -a[j1 + 1];
            yr = a[k1];
            yi = -a[k1 + 1];
            a[j1] = yr;
            a[j1 + 1] = yi;
            a[k1] = xr;
            a[k1 + 1] = xi;
            k1 += m;
            a[k1 + 1] = -a[k1 + 1];
            for (i = n >> 1; i > (j0 ^= i); i >>= 1);
        }
    } else {
        a[1] = -a[1];
        a[m + 1] = -a[m + 1];
        j0 = 0;
        for (k0 = 2; k0 < m; k0 += 2) {
            for (i = n >> 1; i > (j0 ^= i); i >>= 1);
            k = k0;
            for (j = j0; j < j0 + k0; j += 2) {
                xr = a[j];
                xi = -a[j + 1];
                yr = a[k];
                yi = -a[k + 1];
                a[j] = yr;
                a[j + 1] = yi;
                a[k] = xr;
                a[k + 1] = xi;
                j1 = j + m;
                k1 = k + m;
                xr = a[j1];
                xi = -a[j1 + 1];
                yr = a[k1];
                yi = -a[k1 + 1];
                a[j1] = yr;
                a[j1 + 1] = yi;
                a[k1] = xr;
                a[k1 + 1] = xi;
                for (i = n >> 1; i > (k ^= i); i >>= 1);
            }
            k1 = j0 + k0;
            a[k1 + 1] = -a[k1 + 1];
            a[k1 + m + 1] = -a[k1 + m + 1];
        }
    }
}

void cftfsub(int n, double *a)
{
    int j, j1, j2, j3, l;
    double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    l = 2;
    if (n > 8) {
        cft1st(n, a);
        l = 8;
        while ((l << 2) < n) {
            cftmdl(n, l, a);
            l <<= 2;
        }
    }
    if ((l << 2) == n) {
        for (j = 0; j < l; j += 2) {
            j1 = j + l;
            j2 = j1 + l;
            j3 = j2 + l;
            x0r = a[j] + a[j1];
            x0i = a[j + 1] + a[j1 + 1];
            x1r = a[j] - a[j1];
            x1i = a[j + 1] - a[j1 + 1];
            x2r = a[j2] + a[j3];
            x2i = a[j2 + 1] + a[j3 + 1];
            x3r = a[j2] - a[j3];
            x3i = a[j2 + 1] - a[j3 + 1];
            a[j] = x0r + x2r;
            a[j + 1] = x0i + x2i;
            a[j2] = x0r - x2r;
            a[j2 + 1] = x0i - x2i;
            a[j1] = x1r - x3i;
            a[j1 + 1] = x1i + x3r;
            a[j3] = x1r + x3i;
            a[j3 + 1] = x1i - x3r;
        }
    } else {
        for (j = 0; j < l; j += 2) {
            j1 = j + l;
            x0r = a[j] - a[j1];
            x0i = a[j + 1] - a[j1 + 1];
            a[j] += a[j1];
            a[j + 1] += a[j1 + 1];
            a[j1] = x0r;
            a[j1 + 1] = x0i;
        }
    }
}

void cftbsub(int n, double *a)
{
    int j, j1, j2, j3, l;
    double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    l = 2;
    if (n > 8) {
        cft1st(n, a);
        l = 8;
        while ((l << 2) < n) {
            cftmdl(n, l, a);
            l <<= 2;
        }
    }
    if ((l << 2) == n) {
        for (j = 0; j < l; j += 2) {
            j1 = j + l;
            j2 = j1 + l;
            j3 = j2 + l;
            x0r = a[j] + a[j1];
            x0i = -a[j + 1] - a[j1 + 1];
            x1r = a[j] - a[j1];
            x1i = -a[j + 1] + a[j1 + 1];
            x2r = a[j2] + a[j3];
            x2i = a[j2 + 1] + a[j3 + 1];
            x3r = a[j2] - a[j3];
            x3i = a[j2 + 1] - a[j3 + 1];
            a[j] = x0r + x2r;
            a[j + 1] = x0i - x2i;
            a[j2] = x0r - x2r;
            a[j2 + 1] = x0i + x2i;
            a[j1] = x1r - x3i;
            a[j1 + 1] = x1i - x3r;
            a[j3] = x1r + x3i;
            a[j3 + 1] = x1i + x3r;
        }
    } else {
        for (j = 0; j < l; j += 2) {
            j1 = j + l;
            x0r = a[j] - a[j1];
            x0i = -a[j + 1] + a[j1 + 1];
            a[j] += a[j1];
            a[j + 1] = -a[j + 1] - a[j1 + 1];
            a[j1] = x0r;
            a[j1 + 1] = x0i;
        }
    }
}

void cft1st(int n, double *a)
{
    int j, kj, kr;
    double ew, wn4r, wk1r, wk1i, wk2r, wk2i, wk3r, wk3i;
    double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    x0r = a[0] + a[2];
    x0i = a[1] + a[3];
    x1r = a[0] - a[2];
    x1i = a[1] - a[3];
    x2r = a[4] + a[6];
    x2i = a[5] + a[7];
    x3r = a[4] - a[6];
    x3i = a[5] - a[7];
    a[0] = x0r + x2r;
    a[1] = x0i + x2i;
    a[4] = x0r - x2r;
    a[5] = x0i - x2i;
    a[2] = x1r - x3i;
    a[3] = x1i + x3r;
    a[6] = x1r + x3i;
    a[7] = x1i - x3r;
    wn4r = WR5000;
    x0r = a[8] + a[10];
    x0i = a[9] + a[11];
    x1r = a[8] - a[10];
    x1i = a[9] - a[11];
    x2r = a[12] + a[14];
    x2i = a[13] + a[15];
    x3r = a[12] - a[14];
    x3i = a[13] - a[15];
    a[8] = x0r + x2r;
    a[9] = x0i + x2i;
    a[12] = x2i - x0i;
    a[13] = x0r - x2r;
    x0r = x1r - x3i;
    x0i = x1i + x3r;
    a[10] = wn4r * (x0r - x0i);
    a[11] = wn4r * (x0r + x0i);
    x0r = x3i + x1r;
    x0i = x3r - x1i;
    a[14] = wn4r * (x0i - x0r);
    a[15] = wn4r * (x0i + x0r);
    ew = M_PI_2 / n;
    kr = 0;
    for (j = 16; j < n; j += 16) {
        for (kj = n >> 2; kj > (kr ^= kj); kj >>= 1);
        wk1r = cos(ew * kr);
        wk1i = sin(ew * kr);
        wk2r = 1 - 2 * wk1i * wk1i;
        wk2i = 2 * wk1i * wk1r;
        wk3r = wk1r - 2 * wk2i * wk1i;
        wk3i = 2 * wk2i * wk1r - wk1i;
        x0r = a[j] + a[j + 2];
        x0i = a[j + 1] + a[j + 3];
        x1r = a[j] - a[j + 2];
        x1i = a[j + 1] - a[j + 3];
        x2r = a[j + 4] + a[j + 6];
        x2i = a[j + 5] + a[j + 7];
        x3r = a[j + 4] - a[j + 6];
        x3i = a[j + 5] - a[j + 7];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        x0r -= x2r;
        x0i -= x2i;
        a[j + 4] = wk2r * x0r - wk2i * x0i;
        a[j + 5] = wk2r * x0i + wk2i * x0r;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j + 2] = wk1r * x0r - wk1i * x0i;
        a[j + 3] = wk1r * x0i + wk1i * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j + 6] = wk3r * x0r - wk3i * x0i;
        a[j + 7] = wk3r * x0i + wk3i * x0r;
        x0r = wn4r * (wk1r - wk1i);
        wk1i = wn4r * (wk1r + wk1i);
        wk1r = x0r;
        wk3r = wk1r - 2 * wk2r * wk1i;
        wk3i = 2 * wk2r * wk1r - wk1i;
        x0r = a[j + 8] + a[j + 10];
        x0i = a[j + 9] + a[j + 11];
        x1r = a[j + 8] - a[j + 10];
        x1i = a[j + 9] - a[j + 11];
        x2r = a[j + 12] + a[j + 14];
        x2i = a[j + 13] + a[j + 15];
        x3r = a[j + 12] - a[j + 14];
        x3i = a[j + 13] - a[j + 15];
        a[j + 8] = x0r + x2r;
        a[j + 9] = x0i + x2i;
        x0r -= x2r;
        x0i -= x2i;
        a[j + 12] = -wk2i * x0r - wk2r * x0i;
        a[j + 13] = -wk2i * x0i + wk2r * x0r;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j + 10] = wk1r * x0r - wk1i * x0i;
        a[j + 11] = wk1r * x0i + wk1i * x0r;
        x0r = x1r + x3i;
        x0i = x1i - x3r;
        a[j + 14] = wk3r * x0r - wk3i * x0i;
        a[j + 15] = wk3r * x0i + wk3i * x0r;
    }
}


void cftmdl(int n, int l, double *a)
{
    int j, j1, j2, j3, k, kj, kr, m, m2;
    double ew, wn4r, wk1r, wk1i, wk2r, wk2i, wk3r, wk3i;
    double x0r, x0i, x1r, x1i, x2r, x2i, x3r, x3i;
    
    m = l << 2;
    for (j = 0; j < l; j += 2) {
        j1 = j + l;
        j2 = j1 + l;
        j3 = j2 + l;
        x0r = a[j] + a[j1];
        x0i = a[j + 1] + a[j1 + 1];
        x1r = a[j] - a[j1];
        x1i = a[j + 1] - a[j1 + 1];
        x2r = a[j2] + a[j3];
        x2i = a[j2 + 1] + a[j3 + 1];
        x3r = a[j2] - a[j3];
        x3i = a[j2 + 1] - a[j3 + 1];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        a[j2] = x0r - x2r;
        a[j2 + 1] = x0i - x2i;
        a[j1] = x1r - x3i;
        a[j1 + 1] = x1i + x3r;
        a[j3] = x1r + x3i;
        a[j3 + 1] = x1i - x3r;
    }
    wn4r = WR5000;
    for (j = m; j < l + m; j += 2) {
        j1 = j + l;
        j2 = j1 + l;
        j3 = j2 + l;
        x0r = a[j] + a[j1];
        x0i = a[j + 1] + a[j1 + 1];
        x1r = a[j] - a[j1];
        x1i = a[j + 1] - a[j1 + 1];
        x2r = a[j2] + a[j3];
        x2i = a[j2 + 1] + a[j3 + 1];
        x3r = a[j2] - a[j3];
        x3i = a[j2 + 1] - a[j3 + 1];
        a[j] = x0r + x2r;
        a[j + 1] = x0i + x2i;
        a[j2] = x2i - x0i;
        a[j2 + 1] = x0r - x2r;
        x0r = x1r - x3i;
        x0i = x1i + x3r;
        a[j1] = wn4r * (x0r - x0i);
        a[j1 + 1] = wn4r * (x0r + x0i);
        x0r = x3i + x1r;
        x0i = x3r - x1i;
        a[j3] = wn4r * (x0i - x0r);
        a[j3 + 1] = wn4r * (x0i + x0r);
    }
    ew = M_PI_2 / n;
    kr = 0;
    m2 = 2 * m;
    for (k = m2; k < n; k += m2) {
        for (kj = n >> 2; kj > (kr ^= kj); kj >>= 1);
        wk1r = cos(ew * kr);
        wk1i = sin(ew * kr);
        wk2r = 1 - 2 * wk1i * wk1i;
        wk2i = 2 * wk1i * wk1r;
        wk3r = wk1r - 2 * wk2i * wk1i;
        wk3i = 2 * wk2i * wk1r - wk1i;
        for (j = k; j < l + k; j += 2) {
            j1 = j + l;
            j2 = j1 + l;
            j3 = j2 + l;
            x0r = a[j] + a[j1];
            x0i = a[j + 1] + a[j1 + 1];
            x1r = a[j] - a[j1];
            x1i = a[j + 1] - a[j1 + 1];
            x2r = a[j2] + a[j3];
            x2i = a[j2 + 1] + a[j3 + 1];
            x3r = a[j2] - a[j3];
            x3i = a[j2 + 1] - a[j3 + 1];
            a[j] = x0r + x2r;
            a[j + 1] = x0i + x2i;
            x0r -= x2r;
            x0i -= x2i;
            a[j2] = wk2r * x0r - wk2i * x0i;
            a[j2 + 1] = wk2r * x0i + wk2i * x0r;
            x0r = x1r - x3i;
            x0i = x1i + x3r;
            a[j1] = wk1r * x0r - wk1i * x0i;
            a[j1 + 1] = wk1r * x0i + wk1i * x0r;
            x0r = x1r + x3i;
            x0i = x1i - x3r;
            a[j3] = wk3r * x0r - wk3i * x0i;
            a[j3 + 1] = wk3r * x0i + wk3i * x0r;
        }
        x0r = wn4r * (wk1r - wk1i);
        wk1i = wn4r * (wk1r + wk1i);
        wk1r = x0r;
        wk3r = wk1r - 2 * wk2r * wk1i;
        wk3i = 2 * wk2r * wk1r - wk1i;
        for (j = k + m; j < l + (k + m); j += 2) {
            j1 = j + l;
            j2 = j1 + l;
            j3 = j2 + l;
            x0r = a[j] + a[j1];
            x0i = a[j + 1] + a[j1 + 1];
            x1r = a[j] - a[j1];
            x1i = a[j + 1] - a[j1 + 1];
            x2r = a[j2] + a[j3];
            x2i = a[j2 + 1] + a[j3 + 1];
            x3r = a[j2] - a[j3];
            x3i = a[j2 + 1] - a[j3 + 1];
            a[j] = x0r + x2r;
            a[j + 1] = x0i + x2i;
            x0r -= x2r;
            x0i -= x2i;
            a[j2] = -wk2i * x0r - wk2r * x0i;
            a[j2 + 1] = -wk2i * x0i + wk2r * x0r;
            x0r = x1r - x3i;
            x0i = x1i + x3r;
            a[j1] = wk1r * x0r - wk1i * x0i;
            a[j1 + 1] = wk1r * x0i + wk1i * x0r;
            x0r = x1r + x3i;
            x0i = x1i - x3r;
            a[j3] = wk3r * x0r - wk3i * x0i;
            a[j3 + 1] = wk3r * x0i + wk3i * x0r;
        }
    }
}
