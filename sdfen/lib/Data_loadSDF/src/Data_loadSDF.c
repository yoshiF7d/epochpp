#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <sys/stat.h>
#include "sdf.h"
#include "LinkedList.h"

Data Data_loadSDF(char *file, char *variable){
	int i,err,len,block;
	struct stat statbuf;
	sdf_file_t *h;
	sdf_block_t *b;
	Data data=NULL;
	
	printf("file name : %s\n",file);
	printf("variable name : %s\n",variable);
	
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
			  case SDF_BLOCKTYPE_POINT_VARIABLE:
				data = sdf_read_point_variable(h);
				break;
			  case SDF_BLOCKTYPE_PLAIN_VARIABLE:
				data = sdf_read_plain_variable(h);
				break;
			  case SDF_BLOCKTYPE_PLAIN_MESH:
				data = sdf_read_plain_mesh(h);
				break;
			  case SDF_BLOCKTYPE_POINT_MESH:
				data = sdf_read_point_variable(h);
				break;
			  default:
				break;
			}
			break;
		}
	}
	if(!data){fprintf(stderr,"Data_loadSDFList : error loading variable %s\n",variable);}
	return data;
}

LinkedList Data_loadSDFList(char *file, LinkedList namelist){
	int i,err,len,block;
	struct stat statbuf;
	sdf_file_t *h;
	sdf_block_t *b;
	Data data=NULL;
	LinkedList list,datalist=NULL;
	char *buf;
	/*
	printf("file name : %s\n",file);
	printf("variable name list:");
	for(list=namelist;list;list=LinkedList_increment(list)){printf("\t%s",LinkedList_get(list));}
	printf("\n");
	*/
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
		for(i=0;i<h->nblocks;i++,h->current_block = b->next){
			b = h->current_block;
			data = NULL;
			if(!memcmp(b->id,buf,len)){
				switch(b->blocktype){
				  case SDF_BLOCKTYPE_PLAIN_VARIABLE:
					data = sdf_read_plain_variable(h);
					break;
				  case SDF_BLOCKTYPE_POINT_VARIABLE:
					data = sdf_read_point_variable(h);
					break;
				  case SDF_BLOCKTYPE_PLAIN_MESH:
					data = sdf_read_plain_mesh(h);
					break;
				  case SDF_BLOCKTYPE_POINT_MESH:
					data = sdf_read_point_variable(h);
					break;
				  default:
					break;
				}
				break;
			}
		}
		if(!data){fprintf(stderr,"Data_loadSDFList : error loading variable %s\n",buf);}
		datalist=LinkedList_append(datalist,data);
	}
	return datalist;
}
