#include "Data.h"
#include "LinkedList.h"
#include "Data_loadSDF.h"

void Data_vdelete(void *data){Data_delete(data);}

int main(int argc, char *argv[]){
	int len,i;
	char *filein, *dirout, *variable,*buf,*buf2;
	Data data;
	LinkedList datalist,idlist=NULL;
	
	if(argc < 4){
		fprintf(stdout,"usage : %s inputfile outputdir ...(space separated id list)\n",argv[0]);
		exit(1);
	}
	
	filein = argv[1];
	dirout = argv[2];
	mkdir(dirout,0777);
	
	for(i=3;i<argc;i++){
		idlist=LinkedList_append(idlist,String_copy(argv[i]));
	}
	datalist = Data_loadSDFList(filein,idlist);
	
	for(;idlist && datalist;datalist=LinkedList_increment(datalist),idlist=LinkedList_increment(idlist)){
		buf = LinkedList_get(idlist);
		String_replace(buf,"/",'_');
		len = strlen(dirout) + 1 + strlen(buf) + 5 + 1;
		buf2 = allocate(len);
		sprintf(buf2,"%s/%s.bmat",dirout,buf);
		//printf("making %s\n",buf2);
		data = LinkedList_get(datalist);
		Data_output(data,buf2,p_float);
		deallocate(buf2);
	}
	LinkedList_deleteRoot(idlist,deallocate);
	LinkedList_deleteRoot(datalist,Data_vdelete);
	return 0;
}
