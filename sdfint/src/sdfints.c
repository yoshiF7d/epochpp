#include "utilities.h"
#include "Array.h"
#include "LinkedList.h"
#include "Array_loadSDF.h"
#include <unistd.h>
#include <dirent.h> 

#define C_e0 8.854187e-12
#define C_u0 1.256637e-06
/*een : total electric field energy per grid*/
/*ben : total magnetic field energy per grid*/
/*ken : total kinetic energy per grid*/

int vstrcmp(void *a, void *b){return strcmp(a,b);}

double calcen(char *file, Array mask){
	int i;
	double ec,bc,bx,by,bz,ex,ey,ez;
	double energy,total;
	Array aryex,aryey,aryez,arybx,aryby,arybz;
	LinkedList arylist;
	arylist = Array_loadSDFList(file,6,"ex","ey","ez","bx","by","bz");
	aryex = LinkedList_getIndex(arylist,0);
	aryey = LinkedList_getIndex(arylist,1);
	aryez = LinkedList_getIndex(arylist,2);
	arybx = LinkedList_getIndex(arylist,3);
	aryby = LinkedList_getIndex(arylist,4);
	arybz = LinkedList_getIndex(arylist,5);
	ec = 0.5*C_e0; bc = 0.5/C_u0;
	total = 0;
	for(i=0;i<aryex->len;i++){
		ex = aryex->elem[i];
		ey = aryey->elem[i];
		ez = aryez->elem[i];
		bx = arybx->elem[i];
		by = aryby->elem[i];
		bz = arybz->elem[i];
		energy = ec*(ex*ex+ey*ey+ez*ez)+bc*(bx*bx+by*by+bz*bz);
		if(mask){energy *= mask->elem[i];}
		total += energy;
	}
	LinkedList_deleteRoot(arylist,Array_vdelete);
	return total;
}

int main(int argc, char *argv[]){
	char *dirin,*file,*filebg,*fileout,*dirbg=NULL,*filemask=NULL;
	
	int i,j,k,opt,count,filecount,len,maxcount=-1;
	double duration=0;
	double en,enbg,ennet;
	DIR *dp;
	FILE *fp;
	struct dirent *entry;
	time_t start,end;

	LinkedList list,mainlist=NULL;
	Array mask=NULL;

	FILE *gp;
	while((opt=getopt(argc,argv,"b:m:"))!=-1){
		switch(opt){
			case 'b':
				dirbg = String_join(optarg,"/");
				break;
			case 'm':
				filemask = String_join(optarg,"/");
				break;
			default:
				goto usage;
		}
	}

	if(optind+1 >= argc){
		usage:
		printf(
			   "usage : %s [inputdir name] [outputfile name] [-b bg dir name] [-m mask file name (.bary)]\n",
			   argv[0]
		);
		exit(0);	
	}else{
		dirin = String_join(argv[optind],"/");
		fileout = String_copy(argv[optind+1]);
	}
	
	if(filemask){
		if(!strcmp(".bmat",getFileExtension(filemask))){
			mask = Array_inputBMAT(filemask);
		}else{
			mask = Array_input(filemask);
		}
	}
	
	filecount=0;
	if((dp=opendir(dirin))==NULL){fprintf(stderr,"%s opendir error",argv[0]); return -1;}
	while((entry=readdir(dp))!=NULL){
		if(entry->d_name[0]=='.'){continue;}
		if(strcmp(getFileExtension(entry->d_name),".sdf")==0){
			filecount++;
			mainlist = LinkedList_append(mainlist,String_copy(entry->d_name));
		}
	}
	closedir(dp);
	mainlist=LinkedList_sort(mainlist,vstrcmp);
	if(maxcount > 0){
		for(i=0,list=mainlist;i<maxcount-1 && list->next;i++,list=list->next){}
		LinkedList_deleteRoot(list->next,deallocate);
		filecount = maxcount;
	}
	
	fp = fopen(fileout,"w");
	if(!fp){perror(fileout); exit(1);}
	fprintf(fp,"net\tbg\tall\n");
	
	time(&start);
	for(list=mainlist,count=0;list;list=list->next,count++){
		if(count % 10 == 0){
			time(&end);
			duration = (end - start)*0.1;
			time(&start);
		}
		file = String_join(dirin,list->content);
		en=calcen(file,mask);
		deallocate(file);
		if(dirbg){
			filebg = String_join(dirbg,list->content);
			enbg=calcen(filebg,mask);
			deallocate(filebg);
		}else{
			enbg=0;
		}
		ennet=en-enbg; 
		
		fprintf(fp,"%e\t%e\t%e\n",ennet,enbg,en);
		
		printf("processing %s (%d/%d)\n",list->content,count,filecount);
		printf("[");
		for(i=0;i<100;i++){
			if(i<100*count/filecount){printf("=");}
			else if(i==100*count/filecount){printf("🐌");}
			else{printf(" ");}
		}
		printf("]");
		printf(" %.1f %%",100*(double)count/filecount);
		printf(" ETA %.1f min\n",(double)(filecount-count)*duration/60);
	}
	
	fclose(fp);	
	Array_delete(mask);
	LinkedList_deleteRoot(mainlist,deallocate);
	deallocate(dirin);
	deallocate(dirbg);
	return 0;
}
