#include "Array.h"
#include "LinkedList.h"
#include "Array_loadSDF.h"

void Array_vdelete(void *array){Array_delete(array);}

int main(int argc, char *argv[]){
	int len,i;
	char *filein, *dirout,*buf,*buf2;
	Array array;
	LinkedList arraylist,idlist=NULL,s,ss;
	
#ifdef LEAKDETECT
	LeakDetector_set(stdout);
#endif
	
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
	arraylist = Array_loadSDFList2(filein,idlist);
	
	for(s=idlist,ss=arraylist;s && ss;s=LinkedList_increment(s),ss=LinkedList_increment(ss)){
		buf = LinkedList_get(s);
		String_replace(buf,"/",'_');
		len = strlen(dirout) + 1 + strlen(buf) + 5 + 1;
		buf2 = allocate(len);
		sprintf(buf2,"%s/%s.bmat",dirout,buf);
		//printf("making %s\n",buf2);
		array = LinkedList_get(ss);
		Array_output(array,buf2,p_float);
		deallocate(buf2);
	}
	LinkedList_deleteRoot(idlist,deallocate);
	LinkedList_deleteRoot(arraylist,Array_vdelete);
	
#ifdef LEAKDETECT
	LeakDetector_finish();
#endif
	
	return 0;
}
