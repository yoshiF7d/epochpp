#include "utilities.h"
#include <dirent.h>

#define INDENT 26
int main(int argc, char *argv[]){
	DIR *dp=NULL;
	FILE *fout;

	struct dirent *entry;
	char str[256],*buf,*buf2=NULL;
	
#ifdef LEAKDETECT
	LeakDetector_set(stdout);
#endif
	char *spaces=repeatedString(' ',INDENT);
	
	if(argc<3){perror("usage : makedoc inputdir outputfile\n"); return 0;}
	if((dp=opendir(argv[1]))==NULL){perror("makedoc"); return -1;}
	if((fout=fopen(argv[2],"w"))==NULL){perror("makedoc"); return -1;}
	fprintf(fout,"#ifndef %s_H\n#define %s_H\n#include \"SymbolTable.h\"\nvoid %s(int id){\n",argv[3],argv[3],argv[3]);
	
	while((entry=readdir(dp))!=NULL){
		if(entry->d_name[0]=='.'){continue;}
		snprintf(str,256,"%s/%s",argv[1],entry->d_name);
		buf = String_read(str);
		fprintf(fout,"\tif(id==id_%s){\n",entry->d_name);
		fprintf(fout,"\t\tprintf(\n");
		buf2 = strtok(buf,"\n");
		while(buf2){
			fprintf(fout,"\t\t\t\"%s%s\\n\"\n",spaces,buf2);
			buf2 = strtok(NULL,"\n");
		}
		fprintf(fout,"\t\t);\n\t}\n");
		deallocate(buf);
	}
	fprintf(fout,"}\n#endif\n");
	fclose(fout);
	deallocate(spaces);
	
#ifdef LEAKDETECT
	LeakDetector_finish();
#endif
	return 0;
}	
