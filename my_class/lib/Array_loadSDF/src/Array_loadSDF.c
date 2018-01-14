#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include <stdarg.h>
#include "sdf.h"
#include "Array.h"
#include "LinkedList.h"
#include "Array_loadSDF.h"

Array Array_loadSDF(char *file, char *variable){
	int i,err,len,block;
	struct stat statbuf;
	sdf_file_t *h;
	sdf_block_t *b;
	Array array=NULL;
	
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
		block = (h->nblocks) / 64;
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
				array = Array_sdf_read_plain_variable(h);
				break;
			  case SDF_BLOCKTYPE_POINT_VARIABLE:
			  	array = Array_sdf_read_point_variable(h);
				break;
			  case SDF_BLOCKTYPE_PLAIN_MESH:
				array = Array_sdf_read_plain_mesh(h);
				break;
			  //case SDF_BLOCKTYPE_POINT_MESH:
			  //	array = sdf_read_point_mesh(h);
			  //	break;
			  default:
				break;
			}
			break;
		}
	}
    sdf_free_blocklist_data(h);
	sdf_close(h);
	return array;
}

LinkedList Array_loadSDFList(char *file, int n, ...){
	int i,j,err,len,block;
	struct stat statbuf;
	sdf_file_t *h;
	sdf_block_t *b;
	Array array=NULL;
	LinkedList list,arraylist=NULL;
	char **buflist;
	va_list ap;
	
	//printf("file name : %s\n",file);
		
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
		array = NULL;
		len = strlen(buflist[j])+1;
		b = h->current_block = h->blocklist;
		for(i=0;i<h->nblocks;i++,h->current_block = b->next){
			b = h->current_block;
			if(!memcmp(b->id,buflist[j],len)){
				switch(b->blocktype){
				  case SDF_BLOCKTYPE_PLAIN_VARIABLE:
					array = Array_sdf_read_plain_variable(h);
					break;
				  case SDF_BLOCKTYPE_PLAIN_MESH:
					array = Array_sdf_read_plain_mesh(h);
					break;
				  case SDF_BLOCKTYPE_POINT_VARIABLE:
					array = Array_sdf_read_point_variable(h);
					break;
				  default:
					break;
				}
			}
		}
		if(!array){printf("Array_loadSDFList : variable %s is not found in %s\n",buflist[j],file);}
		arraylist=LinkedList_append(arraylist,array);
	}
    sdf_free_blocklist_data(h);
	sdf_close(h);
	deallocate(buflist);
	return arraylist;
}

LinkedList Array_loadSDFList2(char *file, LinkedList namelist){
	int i,j,err,len,block;
	struct stat statbuf;
	sdf_file_t *h;
	sdf_block_t *b;
	Array array=NULL;
	LinkedList list,arraylist=NULL;
	char *buf;
	
	//printf("file name : %s\n",file);
	
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
		array = NULL;
		buf = LinkedList_get(list);
		len = strlen(buf)+1;
		b = h->current_block = h->blocklist;
		for(i=0;i<h->nblocks;i++,h->current_block = b->next){
			b = h->current_block;
			if(!memcmp(b->id,buf,len)){
				switch(b->blocktype){
				  case SDF_BLOCKTYPE_PLAIN_VARIABLE:
					array = Array_sdf_read_plain_variable(h);
					break;
				  case SDF_BLOCKTYPE_PLAIN_MESH:
					array = Array_sdf_read_plain_mesh(h);
					break;
				  case SDF_BLOCKTYPE_POINT_VARIABLE:
					array = Array_sdf_read_point_variable(h);
					break;
				  default:
					break;
				}
			}
		}	
		if(!array){printf("Array_loadSDFList : variable %s is not found in %s\n",buf,file);}
		arraylist=LinkedList_append(arraylist,array);
	}
    sdf_free_blocklist_data(h);
	sdf_close(h);
	return arraylist;
}
