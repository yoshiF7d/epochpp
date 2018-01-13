#include "Data.h"
#include "LinkedList.h"
#include "Data_loadSDF.h"

void Data_vdelete(void *data){Data_delete(data);}

int main(int argc, char *argv[]){
	int len, i;
	char *filein, *fileout, *buf;
	Data data;
	LinkedList datalist,idlist=NULL,s,ss;
	FILE *fp;
	
	if(argc < 4){
		fprintf(stdout,"usage : %s inputfile outputfile ...(space separated idname list)\n",argv[0]);
		exit(1);
	}

	
	filein = argv[1];
	fileout = argv[2];
	idlist = LinkedList_create(String_copy("grid"));
	
	for(i=3;i<argc;i++){idlist = LinkedList_append(idlist,String_copy(argv[i]));}
	
	datalist = Data_loadSDFList2(filein,idlist);
	fp = fopen(fileout,"w");
	for(s=idlist;s;s=LinkedList_increment(s)){
		buf = LinkedList_get(s);
		String_replace(buf,"/",'_');
		fprintf(fp,"%s\t",buf);
	}
	fprintf(fp,"\n");
	
	data = LinkedList_get(datalist);
	len = Data_getColumn(data);
/*	
	for(s=datalist;s;s=LinkedList_increment(s)){
		data = LinkedList_get(s);
		printf("%e\t",Data_get(data,0,0));
	}
	printf("\n");
*/
	for(i=0;i<len;i++){
		for(s=datalist;s;s=LinkedList_increment(s)){
			data = LinkedList_get(s);
			fprintf(fp,"%e\t",Data_get(data,0,i));
		}
		fprintf(fp,"\n");
	}
	
	fclose(fp);
	
	LinkedList_deleteRoot(idlist,deallocate);
	LinkedList_deleteRoot(datalist,Data_vdelete);
}
