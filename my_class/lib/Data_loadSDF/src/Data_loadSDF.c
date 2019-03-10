#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include <stdarg.h>
#include "sdf.h"
#include "LinkedList.h"

Data Data_loadSDF(char *file, char *variable){
	int i,err,len,block;
	struct stat statbuf;
	sdf_file_t *h;
	sdf_block_t *b;
	Data data=NULL;
	
	err = stat(file,&statbuf);
	if(err){
		fprintf(stderr,"Error opening file %s\n", file);
		return NULL;
	}
	
	h = sdf_open(file,0,SDF_READ,0);
	
	if(!h){
		fprintf(stderr,"Error opening file %s\n",file);
		return NULL;
	}
	
	h->use_summary = 0;
	sdf_read_header(h);
	h->current_block=NULL;
	
	if(h->nblocks < 0){
		block = (-h->nblocks) / 64;
		err = -h->nblocks - 64 * block;
		fprintf(stderr, "Error code %s found at block %i\n",sdf_error_codes_c[err], block);
	}
	sdf_read_summary(h);
	
	for(i=0;i<h->nblocks;i++){
		sdf_read_block_info(h);
	}
	
	free(h->buffer);
	h->buffer = NULL;

	b = h->current_block = h->blocklist;

	len = strlen(variable)+1;
	for(i=0;i<h->nblocks;i++,h->current_block = b->next){
		b = h->current_block;
		if(!memcmp(b->id,variable,len)){
			switch(b->blocktype){
			  case SDF_BLOCKTYPE_PLAIN_VARIABLE:
				data = Data_sdf_read_plain_variable(h);
				break;
			  case SDF_BLOCKTYPE_PLAIN_MESH:
				data = Data_sdf_read_plain_mesh(h);
				break;
			  case SDF_BLOCKTYPE_POINT_VARIABLE:
				data = Data_sdf_read_point_variable(h);
				break;
			  case SDF_BLOCKTYPE_POINT_MESH:
				data = Data_sdf_read_point_variable(h);
				break;
			  default:
				break;
			}
			break;
		}
	}
        sdf_free_blocklist_data(h);
	sdf_close(h);
	return data;
}

LinkedList Data_loadSDFList(char *file, int n, ...){
	int i,j,err,len,block;
	struct stat statbuf;
	sdf_file_t *h;
	sdf_block_t *b;
	Data data=NULL;
	LinkedList list,datalist=NULL;
	char **buflist;
	va_list ap;
	
	err = stat(file,&statbuf);
	if(err){
		fprintf(stderr,"Error opening file %s\n", file);
		return NULL;
	}
	
	h = sdf_open(file,0,SDF_READ,0);
	
	if(!h){
		fprintf(stderr,"Error opening file %s\n",file);
		return NULL;
	}
	
	h->use_summary = 0;
	sdf_read_header(h);
	h->current_block=NULL;
	
	if(h->nblocks < 0){
		block = (-h->nblocks) / 64;
		err = -h->nblocks - 64 * block;
		fprintf(stderr, "Error code %s found at block %i\n",sdf_error_codes_c[err], block);
	}
	sdf_read_summary(h);
	
	for(i=0;i<h->nblocks;i++){
		sdf_read_block_info(h);
	}
	
	buflist = allocate(n*sizeof(char*));
	va_start(ap,n);
	for(i=0;i<n;i++){
		buflist[i] = va_arg(ap,char*);
	}
	va_end(ap);
	
	free(h->buffer);
	h->buffer = NULL;
	for(j=0;j<n;j++){
		data = NULL;
		len = strlen(buflist[j])+1;
		b = h->current_block = h->blocklist;
		for(i=0;i<h->nblocks;i++,h->current_block = b->next){
			b = h->current_block;
			if(!memcmp(b->id,buflist[j],len)){
				switch(b->blocktype){
				  case SDF_BLOCKTYPE_PLAIN_VARIABLE:
					data = Data_sdf_read_plain_variable(h);
					break;
				  case SDF_BLOCKTYPE_PLAIN_MESH:
					data = Data_sdf_read_plain_mesh(h);
					break;
				  case SDF_BLOCKTYPE_POINT_VARIABLE:
					data = Data_sdf_read_point_variable(h);
					break;
				  case SDF_BLOCKTYPE_POINT_MESH:
					data = Data_sdf_read_point_mesh(h);
					break;
				  default:
					printf("Data_loadSDFList : data type of variable %s is not supported\n",buflist[j]);
					break;
				}
				break;
			}
		}
		//if(!data){printf("Data_loadSDFList : variable %s is not loaded\n",buflist[j]);}
		datalist=LinkedList_append(datalist,data);
	}
    	sdf_free_blocklist_data(h);
	sdf_close(h);
	deallocate(buflist);
	return datalist;
}

LinkedList Data_loadSDFList2(char *file, LinkedList namelist){
	int i,err,len,block;
	struct stat statbuf;
	sdf_file_t *h;
	sdf_block_t *b;
	Data data=NULL;
	LinkedList list,datalist=NULL;
	char *buf;
	
	err = stat(file,&statbuf);
	if(err){
		fprintf(stderr,"Error opening file %s\n", file);
		return NULL;
	}
	
	h = sdf_open(file,0,SDF_READ,0);
	
	if(!h){
		fprintf(stderr,"Error opening file %s\n",file);
		return NULL;
	}
	
	h->use_summary = 0;
	sdf_read_header(h);
	h->current_block=NULL;
	
	if(h->nblocks < 0){
		block = (-h->nblocks) / 64;
		err = -h->nblocks - 64 * block;
		fprintf(stderr, "Error code %s found at block %i\n",sdf_error_codes_c[err], block);
	}
	sdf_read_summary(h);
	
	for(i=0;i<h->nblocks;i++){
		sdf_read_block_info(h);
	}
	
	free(h->buffer);
	h->buffer = NULL;
	for(list=namelist;list;list=LinkedList_increment(list)){
		buf = LinkedList_get(list);
		len = strlen(buf)+1;
		b = h->current_block = h->blocklist;
		data = NULL;
		for(i=0;i<h->nblocks;i++,h->current_block = b->next){
			b = h->current_block;
			if(!memcmp(b->id,buf,len)){
				switch(b->blocktype){
				  case SDF_BLOCKTYPE_PLAIN_VARIABLE:
					data = Data_sdf_read_plain_variable(h);
					break;
				  case SDF_BLOCKTYPE_PLAIN_MESH:
					data = Data_sdf_read_plain_mesh(h);
					break;
				  case SDF_BLOCKTYPE_POINT_VARIABLE:
					data = Data_sdf_read_point_variable(h);
					break;  
				  case SDF_BLOCKTYPE_POINT_MESH:
					data = Data_sdf_read_point_mesh(h);
					break;
				default:
	 				printf("Data_loadSDFList2 : data type of variable %s is not supported\n",buf);	
					break;
				}
				break;
			}
		}
	 	//if(!data){printf("Data_loadSDFList2 : variable %s is not loaded\n",buf);}
		datalist=LinkedList_append(datalist,data);
	}
        sdf_free_blocklist_data(h);
	sdf_close(h);
	return datalist;
}

Data Data_loadSDFPoint(char *file, LinkedList namelist){
	int i,j,k,n,err,len,block;
	struct stat statbuf;
	sdf_file_t *h;
	sdf_block_t *b;
	Data data=NULL;
	LinkedList list,datalist=NULL;
	char *buf;
	union un_array{
		double *d;
		float *f;
	}a;

	err = stat(file,&statbuf);
	if(err){
		fprintf(stderr,"Error opening file %s\n", file);
		return NULL;
	}
	
	h = sdf_open(file,0,SDF_READ,0);
	
	if(!h){
		fprintf(stderr,"Error opening file %s\n",file);
		return NULL;
	}
	
	h->use_summary = 0;
	sdf_read_header(h);
	h->current_block=NULL;
	
	if(h->nblocks < 0){
		block = (-h->nblocks) / 64;
		err = -h->nblocks - 64 * block;
		fprintf(stderr, "Error code %s found at block %i\n",sdf_error_codes_c[err], block);
	}
	sdf_read_summary(h);
	
	for(i=0;i<h->nblocks;i++){
		sdf_read_block_info(h);
	}
	
	free(h->buffer);
	h->buffer = NULL;
	for(list=namelist,j=0;list;list=LinkedList_increment(list),j++){
		buf = LinkedList_get(list);
		len = strlen(buf)+1;
		b = h->current_block = h->blocklist;
		for(i=0;i<h->nblocks;i++,h->current_block = b->next){
			b = h->current_block;
			if(!memcmp(b->id,buf,len)){
				switch(b->blocktype){
				  case SDF_BLOCKTYPE_POINT_VARIABLE:
						if (b->done_data) return NULL;
						if (!b->done_info) sdf_read_blocklist(h);
						b->nelements_local = b->dims[0];
						h->current_location = b->data_location;
						n = b->nelements_local;
						if(!data){
							data = Data_create(LinkedList_getLength(namelist),b->nelements_local);
						}
					switch(b->datatype){
					  case SDF_DATATYPE_REAL8:
							fseeko(h->filehandle, h->current_location, SEEK_SET);
							fread(data->elem[j], SDF_TYPE_SIZES[b->datatype],b->nelements_local,h->filehandle);
						break;
					  case SDF_DATATYPE_REAL4:
							b->data = malloc(b->nelements_local * SDF_TYPE_SIZES[b->datatype]);
							fseeko(h->filehandle, h->current_location, SEEK_SET);
							fread(b->data, SDF_TYPE_SIZES[b->datatype], b->nelements_local, h->filehandle);
							a.f = b->data;
						for(k=0;k<n;k++){Data_set(data,j,k,(double)(a.f[k]));}
						break;
					  default:
						break;
					}
					b->done_data = 1;
					break;
				default:
					 printf("Data_loadPoint : data type of variable %s is not supported\n",buf);
					break;
				}
				break;
			}
		}
		 //if(!data){printf("Data_loadSDFList2 : variable %s is not loaded\n",buf);}
	}
	sdf_free_blocklist_data(h);
	sdf_close(h);
	return data;
}
