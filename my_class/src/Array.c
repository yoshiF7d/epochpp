#include "Array.h"
Array Array_create(int depth , ...){
	va_list ap;
	int i,len=1;
	
	Array array = allocate(sizeof(Array_Sub));
	array->depth = depth;
	array->dim = allocate(depth*sizeof(int));

	va_start(ap,depth);
	for(i=0;i<depth;i++){
		array->dim[i] = va_arg(ap,int);
		len *= array->dim[i];
	}
	va_end(ap);
	array->len = len;
	array->elem = callocate(len,sizeof(double));
	return array;
}

Array Array_create2(int depth, int *dim){
	Array array = allocate(sizeof(Array_Sub));
	int i,len = 1;
	array->depth = depth;
	array->dim = allocate(sizeof(int));
	memcpy(array->dim,dim,depth*sizeof(int));
	for(i=0;i<depth;i++){
		len *= array->dim[i];
	}
	array->len = len;
	array->elem = callocate(len,sizeof(double));
	return array;
}

void Array_delete(Array array){
	if(array){
		deallocate(array->elem);
		deallocate(array->dim);
		deallocate(array);
	}
}
void Array_vdelete(void *array){Array_delete(array);}

double Array_get(Array array,...){
	int i,j,index,len=array->len;
	va_list ap;
	va_start(ap,array);
	
	for(i=0,index=0;i<array->depth;i++){
		j = va_arg(ap,int);
		len /= array->dim[i];
		index += j*len;
	}
	va_end(ap);
	
	return array->elem[index];
}

void Array_set(Array array, double x, ...){
	int i,j,index,len=array->len;
	va_list ap;
	va_start(ap,x);
	
	for(i=0,index=0;i<array->depth;i++){
		j = va_arg(ap,int);
		len /= array->dim[i];
		index += j*len;
	}
	va_end(ap);
	
	array->elem[index] = x;
}

Array Array_copy(Array array){
	Array newary=Array_create2(array->depth,array->dim);
	memcpy(newary->elem,array->elem,(array->len)*sizeof(double));
	return newary;
}

void Array_print(Array array){
	int i;
	if(!array){return;}
	Array_fprint(array,stdout,"\t");
}


void Array_fprint(Array array, FILE *fp, char *sep){
	int i,step=1;
	printf("depth : %d\n",array->depth);
	printf("dimension : ");
	for(i=0;i<array->depth;i++){
		step *= array->dim[i];
		printf("%d ",array->dim[i]);
	}
	printf("\n");
	printf("len : %d\n",array->len);
	Array_fprint_mod(array,fp,sep,0,step/array->dim[array->depth-1],array->depth-1);
}

void Array_fprint_mod(Array array, FILE *fp, char *sep, int offset , int step, int depth){
	int i,j,index;
	if(depth == 1){
		for(i=0;i<array->dim[1];i++){
			for(j=0;j<array->dim[0];j++){
				index = i*step+j;
				fprintf(fp,"%e%s",array->elem[offset+index],sep);
			}
			fprintf(fp,"\n");
		}
		fprintf(fp,"\n");
	}else if(depth == 0){
		for(i=0;i<array->dim[0];i++){fprintf(fp,"%e%s",array->elem[offset+i],sep);}
		fprintf(fp,"\n");
	}else{
		for(i=0;i<array->dim[depth];i++){
			Array_fprint_mod(array,fp,sep,offset+i*step, step/array->dim[depth-1], depth-1);
			fprintf(fp,"\n");
		}
	}
}

void Array_output(Array array, char *fileout, enum precision p){
	FILE *fp=fopen(fileout,"wb");
	int i,w1;
	float *f;
	if(!fp){fprintf(stderr,"Array_output : fopen error\n"); exit(1);}
	fwrite(&p,sizeof(enum precision),1,fp);
	fwrite(&(array->depth),sizeof(int),1,fp);
	fwrite(array->dim,sizeof(int),array->depth,fp);
	switch(p){
	  case p_double:
		if((w1=fwrite(array->elem,sizeof(double),array->len,fp))!=array->len){
			fprintf(stderr,"Array_output : output error. trying to write %d bytes, but %d bytes are written \n",array->len*8,w1*8); exit(1);
		}
		break;
	  case p_float:
		f = allocate((array->len)*sizeof(float));
		for(i=0;i<array->len;i++){f[i] = (float)array->elem[i];}
		if((w1=fwrite(f,sizeof(float),array->len,fp))!=array->len){
			fprintf(stderr,"Array_output : output error. trying to write %d bytes, but %d bytes are written \n",array->len*4,w1*4); exit(1);
		}
		deallocate(f);
		break;
	}
	fclose(fp);
}
Array Array_inputBMAT(char *filein){
	Array array=NULL;
	unsigned long size = getFileSizeBin(filein);
	FILE *fp =fopen(filein,"rb");
	int r=0,c=0,count=0,i,j;
	float *f;
	enum precision p;
	if(!fp){fprintf(stderr,"Array_input : fopen error\n"); exit(1);}
	fread(&p,sizeof(enum precision),1,fp);
	if(p != p_double && p != p_float){fprintf(stderr,"Array_input : array type infomation is corrupted.\n"); exit(1);}
	fread(&r,sizeof(int),1,fp);
	fread(&c,sizeof(int),1,fp);
	switch(p){
	  case p_double:
		if(size != sizeof(enum precision) + 2*sizeof(int) +r*c*sizeof(double)){
			fprintf(stderr,"Array_input : array size should be %lu byte, but is %lu byte\n",sizeof(enum precision) + 2*sizeof(int) +r*c*sizeof(double),size);
			return NULL;
		}
		array = Array_create(2,r,c);
		fread(array->elem,sizeof(double),r*c,fp);
		break;
	  case p_float:
		if(size != sizeof(enum precision) + 2*sizeof(int) +r*c*sizeof(float)){
			fprintf(stderr,"Array_input : array size should be %lu byte, but is %lu byte\n",sizeof(enum precision) + 2*sizeof(int) +r*c*sizeof(float),size);
			return NULL;
		}
		array = Array_create(2,r,c);
		f = allocate(r*c*sizeof(float));
		fread(f,sizeof(float),r*c,fp);
		for(i=0;i<r*c;i++){array->elem[i] = f[i];}
		deallocate(f);
		break;
	}
	fclose(fp);
	return array;
}
Array Array_input(char *filein){
	Array array=NULL;
	unsigned long size = getFileSizeBin(filein);
	FILE *fp =fopen(filein,"rb");
	int depth=0,len=1,count=0,i,j,*dim;
	float *f;
	enum precision p;
	if(!fp){fprintf(stderr,"Array_input : fopen error\n"); exit(1);}
	fread(&p,sizeof(enum precision),1,fp);
	if(p != p_double && p != p_float){fprintf(stderr,"Array_input : array type infomation is corrupted.\n"); exit(1);}
	fread(&depth,sizeof(int),1,fp);
	dim = allocate(depth*sizeof(int));
	fread(dim,sizeof(int),depth,fp);
	for(i=0;i<depth;i++){len *= dim[i];}
	switch(p){
	  case p_double:
		if(size != sizeof(enum precision) + sizeof(int) +depth*sizeof(int) + len*sizeof(double)){
			fprintf(stderr,"Array_input : array size should be %lu byte, but is %lu byte\n",sizeof(enum precision) +sizeof(int) +  depth*sizeof(int) + len*sizeof(double),size);
			return NULL;
		}
		array = Array_create2(depth,dim);
		fread(array->elem,sizeof(double),len,fp);
		break;
	  case p_float:
		if(size != sizeof(enum precision) + sizeof(int) + depth*sizeof(int) + len*sizeof(float)){
			fprintf(stderr,"Array_input : array size should be %lu byte, but is %lu byte\n",sizeof(enum precision) + sizeof(int) + depth*sizeof(int) + len*sizeof(float),size);
			return NULL;
		}
		array = Array_create2(depth,dim);
		f = allocate(len*sizeof(float));
		fread(f,sizeof(float),len,fp);
		for(i=0;i<len;i++){array->elem[i] = f[i];}
		deallocate(f);
		break;
	}
	fclose(fp);
	return array;
}
