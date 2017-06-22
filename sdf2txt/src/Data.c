#include "Data.h"
//#include "LeakDetector.h"

typedef struct st_data{
	double **elem;
	int row;
	int column;
}Data_Sub;

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

/*
Data Data_load2(char *file, int ofsr, int ofsc, int r, int c, char *sep){
	FILE *fp;
	Data data;
	int i,j,k,cc;
	
	//clock_t start,end;
	//printf("CLOCK_PER_SEC : %d\n",CLOCKS_PER_SEC);
	
	//start = clock();
	fp = fopen(file, "r");
	if(fp==NULL){perror("Data_load"); exit(1);}
	//end = clock();
	//printf("fopen : %lu [us]\n", end - start);
	
	//start = clock();
	if(r<0){r=countRow(fp)-ofsr;}
	cc = countColumn(fp,ofsr+1,sep);
	if(c<0){c=cc-ofsc;}
	//end = clock();
	//printf("count : %lu [us]\n",end-start);
	
	data = Data_create(r,c);
	fseek(fp,0,SEEK_SET);
	
	//start = clock();
	skipUntilMatch(fp," ",isFloat);
	for(i=0;i<ofsr;i++){skipLine(fp);}
	for(i=0;i<r;i++){
		loadbar(i,r,50);
		for(j=0;j<ofsc;j++){skipOneString(fp,sep);}
		for(j=0;j<c;j++){
			//printf("%d,%d,%e\n",i,j,data->elem[i][j]);
			skipSeparator(fp,sep);
		}
		if(c!=cc-ofsc){skipLine(fp);}
	}
	//end = clock();
	//printf("load : %lu [us]\n",end-start);
	fclose(fp);
	return data;
}
*/
double Data_get(Data data, int r, int c){
	if(r >= abs(data->row) || c >= abs(data->column)){
		printf("Data_get : invalid index\n");
		return 0;	
	}
	if(r < 0){r = r + data->row;}
	if(c < 0){c = c + data->column;}
	return data->elem[r][c];
} 

void Data_set(Data data, int r, int c, double x){
	if(r >= abs(data->row) || c >= abs(data->column)){
		printf("Data_set : invalid index\n");
		return;	
	}
	if(r < 0){r = r + data->row;}
	if(c < 0){c = c + data->column;}
	data->elem[r][c] = x;
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
	float f;
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
		for(i=0;i<data->row;i++){
			for(j=0;j<data->column;j++){
				f = (float)data->elem[i][j];
				if((fwrite(&f,sizeof(float),1,fp))!=1){
					fprintf(stderr,"Data_output : output error. trying to write %d bytes, but %d bytes are written \n",4,w1*4); exit(1);
				}
			}
		}
		break;
	}
	fclose(fp);
}
Data Data_input(char *filein){
	Data data=NULL;
	unsigned long size = getFileSizeBin(filein);
	FILE *fp=fopen(filein,"rb");
	int r=0,c=0,count=0,i,j;
	float f;
	enum precision p;
	if(!fp){fprintf(stderr,"Data_input : fopen error\n"); exit(1);}
	fread(&p,sizeof(enum precision),1,fp);
	if(p != p_double && p != p_float){fprintf(stderr,"Data_inpu : data type infomation is corrupted.\n"); exit(1);}
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
		for(i=0;i<data->row;i++){
			for(j=0;j<data->column;j++){
				fread(&f,sizeof(float),1,fp);
				data->elem[i][j] = (double)f;
			}
		}
		break;
	}
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
	}else{tg = Data_create(a->row, a->column);}\
}

Data Data_rslice(Data data, int cs, int ce){
	int i,j;
	double temp;
	Data data2;
	if((cs < 0 || ce > data->column) || ce < cs){printf("Data_cslice : index error\n"); return NULL;}
	data2 = Data_create(data->row,2);
	for(i=0;i<data->row;i++){
		temp = 0;
		for(j=cs;j<=ce;j++){
			temp += data->elem[i][j];
		}
		data2->elem[i][0] = i;
		data2->elem[i][1] = temp;
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
	return data2;
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

Data Data_add(Data a, Data b, Data tg){
	int i,j;
	checkcombine;
	for(i=0;i<a->row;i++){
		for(j=0;j<a->column;j++){
			tg->elem[i][j] = a->elem[i][j] + b->elem[i][j];
		}
	}
	return tg;
}
Data Data_sub(Data a, Data b, Data tg){
	int i,j;
	checkcombine;
	for(i=0;i<a->row;i++){
		for(j=0;j<a->column;j++){
			tg->elem[i][j] = a->elem[i][j] - b->elem[i][j];
		}
	}
	return tg;
}
Data Data_mul(Data a, Data b, Data tg){
	int i,j;
	checkcombine;
	for(i=0;i<a->row;i++){
		for(j=0;j<a->column;j++){
			tg->elem[i][j] = a->elem[i][j] * b->elem[i][j];
		}
	}
	return tg;
} 

#define checkcombine2 \
{\
	if(tg){\
		if(!isEqualDimension(a,tg)){\
			printf("Data_muls : Data with unequal dimensions cannot be combined\n");\
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
	double sum=0;
	
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
	for(i=0;i<binc;i++){sum += weight[i];}
	for(i=0;i<binc;i++){data2->elem[i][1] = weight[i]/sum;}
	deallocate(weight);
	return data2;
}


Data Data_histogram2D(Data data, double min1, double min2, double max1, double max2, int binc1, int binc2){
	int i,j,k, binc = binc1*binc2;
	double *weight = allocate(binc*sizeof(double));
	double dx = (max1-min1)/binc1;
	double dy = (max2-min2)/binc2;
	double sum=0;
	
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
	
	for(i=0;i<binc;i++){sum += weight[i];}
	for(i=0;i<binc;i++){data2->elem[i][2] = weight[i]/sum; /*printf("%f\n",data2->elem[i][2]);*/}
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
