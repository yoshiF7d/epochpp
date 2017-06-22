#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define INPUTFILE(filein,buflen) {	\
if(argc > optind){	\
	snprintf(filein,buflen,"%s",argv[optind]); \
	}else{	\
		puts("input file name =");	\
		fgets(filein,buflen,stdin);	\
		filein[strlen(filein)-1] = '\0';	\
	}	\
}	

int main(int argc, char *argv[]){
	FILE *fp;
	char c; 
	char filein[256];
	int opt;
	enum {m_string,m_array,m_format} mode=m_string;
		
	while((opt=getopt(argc,argv,"saf"))!=-1){
		switch(opt){
		  case 's':
			mode = m_string;
		  case 'a':
			mode = m_array;
		  case 'f':
			mode = m_format;
		}
	}
	INPUTFILE(filein,256);
	fp = fopen(filein,"r");
	if(!fp){perror("fopen error"); exit(1);}

	if(mode==m_string || mode==m_format){putchar('\"');}
	else{putchar('{');}
	while((c = fgetc(fp)) != EOF){
		switch(c){
		  case '\\':
			putchar('\\');
			putchar('\\');
			break;
		  case '\'':
			putchar('\\');
			putchar('\'');
			break;
		  case '\"':
			putchar('\\');
			putchar('\"');
			break;
		  case '\n':
			putchar('\\');
			putchar('n');
			if(mode==m_string || mode==m_format){
				putchar('\\');
				putchar('\n');
			}
			break;
		  case '\r':
			putchar('\\');
			putchar('r');
			break;
		  case '\t':
			putchar('\\');
			putchar('t');
			break;
		  case '%':
			if(mode==m_format){
				putchar('%');
				putchar('%');
				break;
			}
		  default :
			putchar(c);
		}
		if(mode==m_array){
			putchar('\"');
			putchar(',');
		}
	}
	if(mode==m_string || mode==m_format){putchar('\"');}
	else{putchar('\'');putchar('\\');putchar('0');putchar('\'');putchar('}');}
	fclose(fp);
	return 0;
}