#include "utilities.h"

#ifdef LEAKDETECT

typedef struct st_pointerlist{
	void *pointer;
	size_t size;	
	const char *funcname;
	char deallocated;
	int index;
	struct st_pointerlist *next;
}PList_Sub;
typedef struct st_pointerlist *PList;

enum Mode{set, print,printall, append, check, finish};

void LeakDetector_set(FILE *fp);
void LeakDetector_print(PList list);
void LeakDetector_printall(void);
void LeakDetector_append(void *pointer,size_t size, const char *funcname);
void LeakDetector_check(void *pointer);
void LeakDetector_finish(void);
void LeakDetector_mod(FILE *fp, void *pointer, size_t size, const char *funcname, enum Mode mode);

PList PList_mod(FILE *fp, void *pointer, size_t size,const char *funcname, enum Mode mode);
PList PList_create(void *pointer, size_t size, const char *funcname, int index);
void PList_delete(PList list);
void PList_fprint(FILE *fp, PList list);
void PList_enum(PList list, void (*func)(PList list));
PList PList_append(PList root, PList list);
int PList_check(PList list, void *pointer, int *index, size_t *size, char **funcname);

PList  PList_create(void *pointer, size_t size, const char *funcname, int index){
	PList list = malloc(sizeof(PList_Sub));
	if(list==NULL){fprintf(stderr,"malloc error\n"); exit(1);}
	list->pointer = pointer;
	list->size = size;
	list->funcname = funcname;
	list->deallocated = 0;
	list->index = index;
	list->next = NULL;
	return list;
}

void PList_delete(PList list){free(list);}

void PList_fprint(FILE *fp, PList list){
	if(!list){fprintf(fp,"null\n");return;}
	fprintf(fp,"index : %d\t",list->index);
	fprintf(fp,"size : %ld\t",list->size);		
	fprintf(fp,"function : %s\t",list->funcname);
	fprintf(fp,"pointer : %x\t",(unsigned int)list->pointer);
	if(list->deallocated){fprintf(fp, KRED "deallocated\n" RESET);}
	else{fprintf(fp, KCYN "allocated\n" RESET);}
}

void PList_enum(PList list, void (*func)(PList list)){
	for(;list;list=list->next){func(list);}
}

PList PList_append(PList root,PList list){
	PList p;
	if(!root){return list;}
	for(p=root;p->next;p=p->next){}
	p->next = list;
	return root;
}

int PList_check(PList list, void *pointer, int *index, size_t *size, char **funcname){
	*index=-1; *size = 0;
	if(!pointer){return *index;}
	for(;list;list=list->next){
		if(list->pointer == pointer){
			list->deallocated=1;
			*index = list->index;
			*size = list->size;
			*funcname = (char*)(list->funcname);
			list->pointer = NULL;
			break;
		}
	}
	return *index;
}

static char *mystery="?";

void LeakDetector_mod(FILE *fp, void *pointer, size_t size, const char *funcname, enum Mode mode){
	static FILE *fpi;
	static PList list;
	static PList list2;
	static int count,mys;
	
	int index=-1,remain=0;
	size_t ss;
	char *fn=NULL;
	PList p;
	
	switch(mode){
	  case set:
		fpi = fp;
		break;
	  case print:
		PList_fprint(fpi,pointer);
		break;
	  case printall:
		if(!fpi){printf("LEAKDETECT is defined. you must use LeakDetector_set and LeakDetector_finish\n");}
		fprintf(fpi,LONGBAR);
		fprintf(fpi,"allocated pointers\n");
		fprintf(fpi,LONGBAR);
		PList_enum(list,LeakDetector_print);
		fprintf(fpi,LONGBAR "\n");
		fprintf(fpi,LONGBAR);
		fprintf(fpi,"deallocated mysterious pointers\n");
		fprintf(fpi,LONGBAR);
		PList_enum(list2,LeakDetector_print);
		fprintf(fpi,LONGBAR "\n");
		break;
	  case append:
		if(!fpi){printf("LEAKDETECT is defined\n");}
		count++;
#if LEAKDETECT==1
		fprintf(fpi,  KCYN "allocate" RESET " : %x\tindex : %d\tsize : %ld\tfunc : %s\n",(unsigned int) pointer,count,size,funcname);
#endif
		list = PList_append(list,PList_create(pointer,size,funcname,count));
		break;
	  case check:
		if(!fpi){printf("LEAKDETECT is defined\n");}
		if(pointer && PList_check(list,pointer,&index,&ss,&fn)<0){
			p = PList_create(pointer,sizeof(pointer),mystery,-1);
			p->deallocated = 1;
			list2 = PList_append(list2,p);
			mys++;
		}
		if(pointer){
#if LEAKDETECT==1
			fprintf(fpi,  KRED "deallocate" RESET " : %x\tindex : %d\tsize : %ld\tfunc : %s\n",(unsigned int) pointer,index,ss,fn);
#endif
		}
		break;
	  case finish:
		for(p=list;p;p=p->next){if(!p->deallocated){remain++;}}
		fprintf(fpi,LONGBAR);
		fprintf(fpi,"%d allocated pointers are not deallocated\n",remain);
		fprintf(fpi,LONGBAR);
		for(p=list;p;p=p->next){if(!p->deallocated){PList_fprint(fpi,p);}}
		fprintf(fpi,LONGBAR "\n");
		fprintf(fpi,LONGBAR);
		fprintf(fpi,"%d mysterious pointers are deallocated\n",mys);
		fprintf(fpi,LONGBAR);
		PList_enum(list2,LeakDetector_print);
		fprintf(fpi,LONGBAR "\n");
		PList_enum(list,PList_delete);
		PList_enum(list2,PList_delete);
		count=0;
		break;
	}
}

void LeakDetector_set(FILE *fp){LeakDetector_mod(fp, NULL,0,NULL,set);}
void LeakDetector_print(PList list){LeakDetector_mod(NULL,list,0,NULL, print);}
void LeakDetector_printall(void){LeakDetector_mod(NULL,NULL,0,NULL, printall);}
void LeakDetector_append(void *pointer,size_t size,const char *funcname){LeakDetector_mod(NULL,pointer,size,funcname,append);}
void LeakDetector_check(void *pointer){LeakDetector_mod(NULL,pointer,0,NULL,check);}
void LeakDetector_finish(void){LeakDetector_mod(NULL,NULL,0,NULL,finish);}
#endif

void printface(char *name, char *author, char *version, char *etc){
	char *p = "";
	printf(
		"%-10s" "%s\n"
		"%-10s" "Version : %s\n\n"
		"%-10s" "%s\n\n"
		"%s\n"
		,p,name
		,p,version
		,p,author
		,etc
	);
}

void *allocate2(size_t size){
	void *p;
	if((p = malloc(size))==NULL){
		fprintf(stderr,"allocate2 error\n");
		exit(1);
	}
#ifdef LEAKDETECT
	LeakDetector_append(p,size,"allocate2");
#endif
	return p;
}

void *callocate2(size_t n,size_t size){
	void *p;
	if((p = calloc(n,size))==NULL){
		fprintf(stderr,"callocate2 error\n");
		exit(1);
	}
#ifdef LEAKDETECT
	LeakDetector_append(p,size,"callocate2");
#endif
	return p;
}

void *reallocate2(void *ptr, size_t oldsize ,size_t size){
	void *p;
	if((p = realloc(ptr,size))==NULL){
		fprintf(stderr,"reallocate2 error\n");
		exit(1);
	}
#ifdef LEAKDETECT
	if(ptr != p){
		LeakDetector_check(ptr);
		LeakDetector_append(p,size,"reallocate2");
	}
#endif
	return p;
}

void *allocate_imp(size_t size, const char *funcname){
	void *p;
		if((p = malloc(size))==NULL){
		fprintf(stderr,"%s : malloc error\n",funcname);
		exit(1);
	}
#ifdef LEAKDETECT
	LeakDetector_append(p,size,funcname);
#endif
	return p;
}

void *callocate_imp(size_t n, size_t size, const char *funcname){
	void *p;
	if((p = calloc(n,size))==NULL){
		fprintf(stderr,"%s : realloc error\n",funcname);
		exit(1);
	}
#ifdef LEAKDETECT
	LeakDetector_append(p,size,funcname);
#endif
	return p;
}

void *reallocate_imp(void *ptr, size_t size, const char *funcname){
	void *p;
	if((p = realloc(ptr,size))==NULL){
		fprintf(stderr,"%s : realloc error\n",funcname);
		exit(1);
	}
#ifdef LEAKDETECT
	if(ptr != p){
		LeakDetector_check(ptr);
		LeakDetector_append(p,size,funcname);
	}
#endif
	return p;
}

void deallocate2(void *ptr,size_t size){
#ifdef LEAKDETECT
	LeakDetector_check(ptr);
#endif
	free(ptr);
}

void deallocate(void *ptr){
#ifdef LEAKDETECT
	LeakDetector_check(ptr);
#endif
	free(ptr);
}

FILE *wopen(char *path){
	FILE *fp = NULL; 
	char c[3];
	if(access(path,F_OK) != -1){
		printf("file \"%s\" already exists\n",path);
		printf("overwrite ? (y/n)\n");
		do{
			fgets(c,3,stdin);
			//printf("%c%c\n",c[0],c[1]);
			if(c[0] == 'n'){return NULL;}
			else if(c[0] == 'y'){fp = fopen(path,"w"); break;}
		}while(1);
	}else{
		fp = fopen(path,"w");
	}
	return fp;
} 
void putstr(void *s){puts((char*)s);}
void printerr(char *message, char *funcname){
	fprintf(stderr,"%s : %s\n",funcname,message);
}

char *ftoa(double a){
	size_t needed = snprintf(NULL,0,"%e",a);
	char *buf = allocate(needed+1);
	snprintf(buf,needed+1,"%e",a);
	return buf;
}

char *itoa(int a){
	size_t needed = snprintf(NULL,0,"%d",a);
	char *buf = allocate(needed+1);
	snprintf(buf,needed+1,"%d",a);
	return buf;
}

char *strhead(char *str){
	while((*str==' ' || *str == '\t') && str != '\0'){str++;}
	return str;
} 

char *strtail(char *str){return strchr(str,'\0');}

void strrev(char *str){
    if (str == 0){return;}
    if (*str == 0){return;}
    char *start = str;
    char *end = start + strlen(str) - 1; /* -1 for \0 */
    char temp;
    while (end > start){
    	temp = *start;
    	*start = *end;
    	*end = temp;
    	++start;
    	--end;
	}
}

char *rstrtok(char *s,  const char *delim){
	char *spanp;
	char c, sc;
	char *tok;
	static char *head;
	static char *last;	
	
	if(s == NULL){
		if(last == NULL){return NULL;}
		s = last;
	}else{
		head = s;
	}
	s = strtail(s);
	s--;
	
  cont:
	c = *s--;
	for(spanp = (char*)delim; (sc = *spanp++) != 0;){		
		if(c == sc){s[1] = 0; goto cont;}
		if(s+1 == head){last = NULL; return NULL;}
	}
	for(;;){
		c = *s--;
		spanp = (char*)delim;
		if(s+1 == head){last = NULL; return head;}
		do{
			if((sc = *spanp++) == c){
				s[1] = 0;
				last = s;
				return  &s[2];
			}
		}while(sc != 0);
	}
}

char *strtok2(char *str,char **seps, int *len, int nsep){
	char *spanp;
	int i,j,c,slen;
	char *tok,*p;
	static char *last;
	State state;
	
	for(i=0;i<nsep;i++){len[i] = 0;}
	
	if(str == NULL && (str=last)==NULL){return NULL;}
  cont2:
	c = *str++;
	for(i=0;i<nsep;i++){
		if(fsmstr(c,seps[i],&len[i])==accept){goto cont2;}
	}
	if(c == 0){
		last = NULL;
		return NULL;
	}
	tok = str-1;
	for(;;){
		c = *str++;
		for(i=0;i<nsep;i++){
			if(fsmstr(c,seps[i], &len[i])==accept || c == 0){
				if(c == 0){str = NULL;}
				else{
					slen = strlen(seps[i]);
					for(p=str-1;str-1-p<slen;--p){*p = '\0';}
				}
				last = str;
				return tok;
			}
		}
	}
}

char *strtok3(char *str,char **seps, int *len, int nsep, char **last){
	char *spanp;
	int i,j,c,slen;
	char *tok,*p;
	State state;
	
	for(i=0;i<nsep;i++){len[i] = 0;}
	
	if(str == NULL && (str=*last)==NULL){return NULL;}
  cont2:
	c = *str++;
	for(i=0;i<nsep;i++){
		if(fsmstr(c,seps[i],&len[i])==accept){goto cont2;}
	}
	if(c == 0){
		last = NULL;
		return NULL;
	}
	tok = str-1;
	for(;;){
		c = *str++;
		for(i=0;i<nsep;i++){
			if(fsmstr(c,seps[i], &len[i])==accept || c == 0){
				if(c == 0){str = NULL;}
				else{
					slen = strlen(seps[i]);
					for(p=str-1;str-1-p<slen;--p){*p = '\0';}
				}
				*last = str;
				return tok;
			}
		}
	}
}

int isInt(char *s){
	if(*s == '-' || *s == '+'){
		if(!isdigit(*++s)) return 0;
	}
	if(!isdigit(*s)){return 0;}
	while(isdigit(*s)) s++;
	if(*s == '\0') return 1;
	return 0;
}

int isFloat(char *s){
	if(*s == '-' || *s == '+'){
		if(!isdigit(*++s)) return 0;
	}
	if(!isdigit(*s)){return 0;}
	while(isdigit(*s)) s++;
	if(*s == '.'){
		if(!isdigit(*++s)) return 0;
	}
	while(isdigit(*s)) s++;
	if(*s == 'e' || *s == 'E'){
		s++;
		if(*s == '+' || *s == '-'){
			s++;
			if(!isdigit(*s)) return 0;
		}else if(!isdigit(*s)){
			return 0;
		}
	}
	while(isdigit(*s)) s++;
	if(*s == '\0') return 1;
	return 0;
}
#include <time.h>

/*fastest*/
int countRow(FILE *fp){
	int n = 0, lastrtn = 0;
	char str[BLOCKSIZE],*p;
	//clock_t start,end;
	//start = clock();
	
	fseek(fp,0,SEEK_SET);
	skipUntilMatch(fp," ",isFloat);
	while(1){
		if(fgets(str,BLOCKSIZE,fp)==NULL){
			if(NULL != strchr(str,'\n')){n++;}
			break;
		}
		if(NULL!=(p = strchr(str,'\n'))){*p=0x0;n++;}
	}
	//end = clock();
	//printf("countRow : %d\n",n);
	//printf("countRow : %lu [us]\n",end-start); 
	
	return n;
}
/*fastest*/
int countColumn(FILE *fp, int r, char *sep){
	int i,n=0,len=0;
	char str[BLOCKSIZE],*p;
	
	//clock_t start,end;
	//start = clock();
	
	fseek(fp,0,SEEK_SET);
	skipUntilMatch(fp," ",isFloat);
	for(i=0;i<r-1;i++) skipLine(fp);
	while(1){
		if(fgets(str,BLOCKSIZE,fp)==NULL){n += String_countstr(str,sep);break;}
		if(strchr(str,'\n')!=NULL){n += String_countstr(str,sep);break;}
		n += String_countstr(str,sep);
	}
	//end = clock();
	//printf("countColumn : %d\n",n); 
	//printf("countColumn : %lu [us]\n",end-start); 
	return n;
}

void skipLine(FILE *fp){
	char c, str[BLOCKSIZE];
	while(1){
		if(fgets(str,BLOCKSIZE,fp)==NULL){break;}
		if(strchr(str,'\n')!=NULL){break;}
	}
}

int skipSeparator(FILE *fp, char *sep){
	char c; fpos_t pos;
	int len=0;
	fgetpos(fp,&pos);
	c = fgetc(fp);
	if(fsmstr(c,sep,&len)==accept || c=='\n'){return 1;}
	if(c=='\r'){if(fgetc(fp)=='\n'){return 1;}}
	fsetpos(fp,&pos);
	return 0;
}

void skipOneString(FILE *fp, char *sep){
	int len;
	char c; fpos_t pos;
	while((c=fgetc(fp)) != EOF){
		if(fsmstr(c,sep,&len)==accept || c=='\n'){break;}
		if(c=='\r'){
			fgetpos(fp,&pos);
			if(fgetc(fp)=='\n'){break;}
			else{fsetpos(fp,&pos); break;}
		}
	}
}

void skipUntilMatch(FILE *fp, char *sep, int (*matcher)(char *s)){
	fpos_t pos;
	char *buf=NULL;
	while(1){
		fgetpos(fp,&pos);
		if((buf=readAllocBlock(fp,sep))==NULL || matcher(buf)){fsetpos(fp,&pos); break;}
		deallocate(buf);
	}
	deallocate(buf);
}

State fsmstr(char c, char *str, int *len){
	if(str == NULL){*len = 0; return start;}
	if(str[*len] == '\0'){
		*len = 0; return accept;
	}
	if(c==str[*len]){
		(*len)++;
		if(str[*len]=='\0'){
			*len=0; return accept;
		}else{return transition;}
	}else{
		*len=0;
		if(str[*len]=='\0'){
			return transition;
		}else{return start;}
	}
}

char *getFileExtension(char *filename){
	char *p, *tail;
	
	tail = strtail(filename);
	for(p=tail;p>=filename;p--){
		if('/'==*p){return tail;}
		if('.'==*p){return p;}  
	}
	return tail;
} 

char *getFileName(char *filename){
	char *p, *tail;
	tail = strtail(filename);
	for(p=tail;p>=filename;p--){
		if('/'==*p){return p+1;} 
	}
	return filename;
}

unsigned long getFileSizeBin(const char *filename){
	struct stat st;
	if(stat(filename,&st)==0){return st.st_size;}
	return 0;
}

unsigned long getFileSize(FILE *fp){	
	unsigned long fsize = ftell(fp);
	fpos_t p; 
	fgetpos(fp,&p);
	fseek(fp,0,SEEK_END);
	fsize = ftell(fp) - fsize;
	fsetpos(fp,&p);
	return (unsigned long)fsize;
} 

char *getSeparator(char *filename){
	char *ext = getFileExtension(filename);
	if(strcmp(ext,".csv")==0 || strcmp(ext,".CSV")==0){return ",";}
	return "\t";
}

char *String_copy(const char *str){
	if(!str){return NULL;}
	return String_ncopy(str,-1);
}

char *String_ncopy(const char *str, int max){
	int len = (max >0) ? max : strlen(str);
	char *buf=NULL;
	buf = allocate(len+1);
	strncpy(buf,str,len);
	buf[len]='\0';
	return buf;
}

char *String_read(char *file){
	char *buf;
	//clock_t stime, etime;
	//stime = clock();
	
	FILE *fp = fopen(file,"r");
	if(fp==NULL){
		printf("%s\n",file);
		perror("String_read");return NULL;
	} 
	buf = String_fread(fp);
	fclose(fp);
	//etime = clock();
	//printf("String_read : %f\n",(double)(etime-stime)/CLOCKS_PER_SEC);
	return buf;
}

char *String_tail(char *str){return strchr(str ,'\0');}
char *String_endline(char *str){return strchr(str, '\n');}

char *String_fread(FILE *fp){
	char *buf;
	char c; unsigned long size;
	size = getFileSize(fp);
	buf = allocate(size+1);
	fread(buf,1,size,fp); buf[size] = '\0';
	return buf;
}

void String_write(char *file, char *buf){
	FILE *fp;
	char *p=buf;
	fp=fopen(file,"w");
	while(*p){fputc(*p,fp);p++;}
	fclose(fp);
}

int String_countstr(char *str, char *str2){
	char *p; int n=0,len=0;
	for(p=str;*p;p++){
		if(fsmstr(*p,str2,&len)==accept){n++;}
	}
	//printf("String_countstr : %d\n",n);
	return n;
}

int String_countBlankLinesAtEnd(char *str){
	char *p = strtail(str);
	int count=0;
	 --p;
	while((*p == '\r' || *p == '\n') && p != str){
		if(*p == '\n'){count++;}
		p--;
	}
	return count;
}

int String_countWordLine(char *str, char *sep){
	char *p,*prem = NULL; 
	int len=0,n=0,sepl; 
	State s=start;
	sepl = strlen(sep);
	for(p=str;*p;p++){
		/*
		switch(s){
		  case start:
			printf("%c\tstart\t%d\n",*p,n);
			break;
		  case transition:
			printf("%c\ttransition\t%d\n",*p,n);
			break;
		  case accept:
			printf("%c\taccept\t%d\n",*p,n);
			break;
		}
		*/
		s = fsmstr(*p,sep,&len);
		//printf("%c\tn:%d\ts:%d\n",*p,n,s);
		
		if(s==accept){
			if(prem!=p-sepl){n++;}
			prem = p;
		}
		if(*p == '\n'){
			if(*(p-1) == '\r'){p--;}
			if(prem!=p-sepl){n++;}
			break;
		}
	}
	//printf("String_countwords : %d\n",n);
	return n;
}

int String_countchr(char *str, char c){
	char *p; int n=0;
	for(p=str;*p;p++){if(*p==c){n++;}}
	//printf("String_countchr : %d\n",n);
	return n;
}

char *String_skipchr(char *str, char c, int n){
	char *p; int k=0;
	for(p=str;*p;p++){
		if(k==n){break;}
		if(*p==c){k++;}
	}
	return p;
}

char *String_skipstr(char *str,char *str2, int n){
	char *p; int k=0,len=0;
	for(p=str;*p;p++){ 
		if(k==n){break;} 
		if(fsmstr(*p,str2,&len)==accept){k++;} 
	}
	return p;
}

char *String_match(char *str, char *str2){
	char *p ,*we; int len;
	State s;
	for(p=str;*p;p++){
		s = fsmstr(*p,str2,&len);
		if(s==start){we=p;} 
		if(fsmstr(*p,str2,&len)==accept){break;}
	}
	return ++we;
}

char *String_getword(char *str, char *sep){
	char *buf=NULL,*p,*ws=str,*we=str;
	int len=0;
	State s;
	for(p=str;*p;p++){
		s=fsmstr(*p,sep,&len);
		if(s==start){we=p;}
		else if(s==accept){
			if(we==str){ws = p+1; continue;}
			else{buf = String_clip(ws,we); break;}
		} 
	}
	if(*p=='\0'){buf = String_clip(ws,p-1);}
	//printf("str[0] : %c\tbuf : %s\n",str[0],buf);
	return buf;
}

char *String_matchLine(char *str, char *sep, int (*matcher)(char *s)){
	char *p,*ws,*we,*buf,*prem=NULL;
	int len=0,sepl;
	State s=start;
	sepl = strlen(sep);
	for(p=ws=we=str;*p;p++){
		s = fsmstr(*p,sep,&len);
		//printf("%c\ts:%d\t",*p,s); 	
		if(s == accept || *p == '\n'){
			if(prem != p - sepl){
				we = p - sepl;
				if(ws <= we){
					buf = String_clip(ws,we);
					//printf("buf : %s\t",buf);
					if(matcher(buf)){/*printf("match\n");*/deallocate(buf);break;}
					else{deallocate(buf);}
				}
			}
			prem = p;
			ws = p+1;
		}
		//printf("\n");
	}
	return ws;
}

char *String_clip(char *s, char *e){
	char *buf;int len;
	len = e-s+2;
	//printf("len : %d\n",len );
	buf = malloc(len);
	strncpy(buf,s,len);
	buf[len-1]='\0';
	return buf;
}

void String_removeBlankLine(char *buf){
	char *p = buf;
	char *sav;
	while(*p){
		switch(*p){
		  case '\n':
			if(*++p == '\n'){
				sav = p;
				while(*p){*p = *(p+1);p++;}
				p = sav-1;
				break;
			}
			p++;
			break;
		  case '\r':
			if(*++p == '\r'){
				sav = p;
				while(*p){*p = *(p+1);p++;}
				p = sav-1;
				break;
			}
			if(*++p == '\n'){
				if(*++p == '\r'){
					sav = p;
					while(*p){*p = *(p+1);p++;}
					p = sav;
					while(*p){*p = *(p+1);p++;}
					p = sav-1;
					break;
				}
			}
			p++;
			break;
		  default:
			p++;
			break;
		}
	}
}

char *String_stripdq(char *buf){
	int len = strlen(buf);
	char *p;
	if(buf[0] == '\"' && strtail(buf)[-1] =='\"'){
		for(p=buf;*p;p++){*p = p[1];}
		p[-2]='\0';
	}
	return buf;
}


/*http://stackoverflow.com/questions/12167946/how-do-i-read-an-arbitrarily-long-line-in-c*/
/*http://stackoverflow.com/users/1428679/lserni*/
char *readAllocLine(FILE *fp){
	char *line = NULL;
	size_t maxlength = 0;
	//assert(fp != NULL);
	fpos_t pos;
	fgetpos(fp,&pos);
	
	for(;;){ //Read the line in BLOCKSIZE - blocks.
		char *crlf, *block;
		maxlength += BLOCKSIZE;
		if(maxlength > BLOCKSIZE_MAX){
			printf("buf size reached its max size\n");
				fsetpos(fp,&pos);
				deallocate(line); return NULL;
		}
		//This exploits realloc behavior upon hitting NULL
		if(NULL == (line = realloc(line,maxlength+1))){
			break; //realloc error returns NULL.
		}
		block = line + maxlength - BLOCKSIZE; /*yoshi : remember pointer addition rule*/
		//BLOCKSIZE+1 to accmmodate final zero
		if(NULL == fgets(block,BLOCKSIZE+1,fp)){
			//TODO: rewind fp in case of error.
			if(block == line){ /*yoshi : this condition is satisfied only at first while loop*/
				//Error.
				fsetpos(fp,&pos);
				deallocate(line); line = NULL;
			}
			break;
		}
		//This was the las block if we find a CRLF inside.
		if (NULL != (crlf = strchr(block, '\n'))) {
			*crlf = 0x0;
            if (crlf != block) {
                if ('\r' == *(--crlf))
                    *crlf = 0x0;
            }
            break;
		}
	}
	return line;
}

char *readAllocBlock(FILE *fp, char *sep){
	char *line = NULL;
	char *crlf, *p, *block;
	int len=0;
	size_t maxlength = 0;
	//assert(fp != NULL);
	fpos_t pos;
	fgetpos(fp,&pos);
	
	for(;;){ //Read the line in BLOCKSIZE - blocks.
		maxlength += BLOCKSIZE;
		if(maxlength > BLOCKSIZE_MAX){
			printf("buf size reached its max size\n");
				fsetpos(fp,&pos);
				deallocate(line); return NULL;
		}
		//This exploits realloc behavior upon hitting NULL
		if(NULL == (line = realloc(line,maxlength+1))){
			fsetpos(fp,&pos);
			deallocate(line); break; //realloc error returns NULL.
		}
		block = line + maxlength - BLOCKSIZE; /*yoshi : remember pointer addition rule*/
		//BLOCKSIZE+1 to accmmodate final zero
		if(NULL == fgets(block,BLOCKSIZE+1,fp)){
			//TODO: rewind fp in case of error.
			if(block == line){ /*yoshi : this condition is satisfied only at first while loop*/
				//Error.
				fsetpos(fp,&pos);
				deallocate(line); line = NULL; break;
			}
		}
		
		for(p=block;*p;p++){
			if(fsmstr(*p,sep,&len)==accept){*p=0x0; goto endloop;}
		}
		
		//This was the las block if we find a CRLF inside.
		if (NULL != (crlf = strchr(block, '\n'))) {
			*crlf = 0x0;
            if (crlf != block) {
                if ('\r' == *(--crlf))
                    *crlf = 0x0;
            }
            break;
		}
	}
  endloop:
	return line;
}

void freadString(FILE *fp, char *str, int maxlen, char *sep){
	char c; int n = 0,len=0;
	fpos_t pos;
	while((c=fgetc(fp))!=EOF){
		if(fsmstr(c,sep,&len)==accept || c=='\n'){break;}
		if(c=='\r'){
			fgetpos(fp,&pos);
			if(fgetc(fp)=='\n'){break;}
			else{fsetpos(fp,&pos);break;}
		}
		str[n]=c; n++; if(n==maxlen){break;}
	}
	str[n]='\0';
}

int freadLine(FILE *fp, char *buf, int maxlen){
	char *p = buf;
	char c; int i=0;
	
	while(1){
		if(maxlen <= i){
			*p='\0'; return 0;
		}
		c = fgetc(fp);
		if(c==EOF){
			*p='\0'; return EOF;
		}
		if(c=='\n' || c=='\r'){
			*p = '\0'; return 1;
		}
		*p = c; p++; i++;
	}
}

void removeBlankLine(char *file){
	char *buf = String_read(file);
	String_removeBlankLine(buf);
	String_write(file,buf);
}

/*http://www.rosshemsley.co.uk/2011/02/creating-a-progress-bar-in-c-or-any-other-console-app/*/
/*\033[F go up one line*/
/*\033[J clear a line*/
void loadbar(unsigned int x, unsigned int n, unsigned int w)
{
    if ( (x != n) && (x % (n/100+1) != 0) ) return;
 
    float ratio  =  x/(float)n;
    int   c      =  ratio * w;
	printf("%3d%% [", (int)(ratio*100) );
    for (x=0; x<c; x++){ printf("=");}
    for (
	x=c; x<w; x++){printf(" ");}
	printf("]\n\033[F\033[J");
}

typedef struct st_cell{
	double val;
	double nval;
}Cell;

#define split 3
#define alpha 0.2
#define beta 0.3
#define WWVAL 1000000
#define WYVAL 100000
#define YWVAL 10000
#define YYVAL 1000
#define RYVAL 100
#define RRVAL 10
#define RVAL 1

void printcolor(double value){
	 if(value>WWVAL){
		printf(BWHT KWHT);
		printf("**" KNRM);
	}else if(value>WYVAL){
		printf(BWHT KYEL);
		printf("**" KNRM);
	}else if(value>YWVAL){
		printf(BWHT KYEL);
		printf("**" KNRM);
	}else if(value>YYVAL){
		printf(BYEL KYEL);
		printf("**" KNRM);
	}else if(value>RYVAL){
		printf(BYEL KRED);
		printf("**" KNRM);
	}else if(value>RRVAL){
		printf(BRED KRED);
		printf("**" KNRM);
	}else if(value>RVAL){
		printf(KRED);
		printf("**" KNRM);
	}else{
		putchar(' ');putchar(' ');
	}
	printf(BNRM);
}

void dividedbyzero(){
	int i;
	printf("%s",dbz_divided);
	sleep(1);
	for(i=0;i<DBZ_LINE_DIVIDED;i++){printf("\033[F\033[2K");}
	printf("%s",dbz_by);
	sleep(1);
	for(i=0;i<DBZ_LINE_BY;i++){printf("\033[F\033[2K");}	
	printf("%s",dbz_zero);
	sleep(1);
	for(i=0;i<DBZ_LINE_ZERO;i++){printf("\033[F\033[2K");}
	bomb(30);
}
void bomb(int size){
	int i,j,k,val,s,c;
	int I,J;
	double ran;	
	
	Cell **cell;
	cell = allocate(size*sizeof(Cell*));
	for(i=0;i<size;i++){
		cell[i] = allocate(size*sizeof(Cell));
		for(j=0;j<size;j++){
			cell[i][j].val = 0;
			cell[i][j].nval = 0;
		}
	}
	cell[(size-1)/2][(size-1)/2].val=10000;
	randomSeed((unsigned)time(NULL));
	for(k=0;k<50;k++){
		//printf("%d\n",k);
		//printf("{\n");
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				for(s=0;s<split;s++){
					do{c = randomInteger()%9;}while(c==4);
					I = i+c/3-1;
					J = j+c%3-1;
					//printf("{%d,%d},",c/3-1,c%3-1);
					if(0 <= I && I < size && 0 <= J && J <size){
						ran = randomReal();
						cell[I][J].nval += ran*alpha*cell[i][j].val;
						cell[i][j].nval -= beta*ran*alpha*cell[i][j].val;
					}
				}
			}
		}
		//printf("}\n");
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				cell[i][j].val = cell[i][j].nval;
				printcolor(cell[i][j].val);
				//printf("%.1e ",cell[i][j].val);
			}
			printf("\n");
		}
		/*
		for(i=0;i<size;i++){
			for(j=0;j<size;j++){
				putchar(k + 'a');
			}
			putchar('\n');
		}
		*/
		usleep(100000);
		for(i=0;i<size;i++){printf("\033[F\033[2K");}
	}
	for(i=0;i<size;i++){
		deallocate(cell[i]);
	}
	deallocate(cell);
}

/* 
   A C-program for MT19937, with initialization improved 2002/1/26.
   Coded by Takuji Nishimura and Makoto Matsumoto.

   Before using, initialize the state by using init_genrand(seed)  
   or init_by_array(init_key, key_length).

   Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   All rights reserved.                          

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

     1. Redistributions of source code must retain the above copyright
        notice, this list of conditions and the following disclaimer.

     2. Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.

     3. The names of its contributors may not be used to endorse or promote 
        products derived from this software without specific prior written 
        permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
   EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
   PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


   Any feedback is very welcome.
   http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
*/

/*
   The original version of http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/MT2002/CODES/mt19937ar.c was modified by Takahiro Omi as
   - delete line 47 "#include<stdio.h>"
   - delete line 174 int main(void){...}
   - change N -> MT_N
   - change N -> MT_N
   - change the file name "mt19937ar.c" -> "MT.h"
*/


/* Period parameters */  
#define MT_N 624
#define MT_M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */

static unsigned long mt[MT_N]; /* the array for the state vector  */
static int mti=MT_N+1; /* mti==MT_N+1 means mt[MT_N] is not initialized */

/* initializes mt[MT_N] with a seed */
void init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<MT_N; mti++) {
        mt[mti] = 
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length */
/* slight change for C++, 2004/2/26 */
void init_by_array(unsigned long init_key[], int key_length)
{
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (MT_N>key_length ? MT_N : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=MT_N) { mt[0] = mt[MT_N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=MT_N-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=MT_N) { mt[0] = mt[MT_N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= MT_N) { /* generate N words at one time */
        int kk;

        if (mti == MT_N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<MT_N-MT_M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<MT_N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(MT_M-MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[MT_N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[MT_N-1] = mt[MT_M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }
  
    y = mt[mti++];

    /* Tempering */
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

/* generates a random number on [0,0x7fffffff]-interval */
long genrand_int31(void)
{
    return (long)(genrand_int32()>>1);
}

/* generates a random number on [0,1]-real-interval */
double genrand_real1(void)
{
    return genrand_int32()*(1.0/4294967295.0); 
    /* divided by 2^32-1 */ 
}

/* generates a random number on [0,1)-real-interval */
double genrand_real2(void)
{
    return genrand_int32()*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on (0,1)-real-interval */
double genrand_real3(void)
{
    return (((double)genrand_int32()) + 0.5)*(1.0/4294967296.0); 
    /* divided by 2^32 */
}

/* generates a random number on [0,1) with 53-bit resolution*/
double genrand_res53(void) 
{ 
    unsigned long a=genrand_int32()>>5, b=genrand_int32()>>6; 
    return(a*67108864.0+b)*(1.0/9007199254740992.0); 
} 
/* These real versions are due to Isaku Wada, 2002/01/09 added */
void randomSeed(unsigned long s){init_genrand(s);}
double randomReal(void){return genrand_real1();}
unsigned long randomInteger(void){return genrand_int32();}

char dbz_divided[] = {
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', '\n', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D',
'D', 'D', 'D', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'i', 'i', 'i', 'i', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'i', 'i', 'i', 'i', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
'd', ':', ':', ':', ':', ':', ':', 'd', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'd', ':', ':', ':', ':', ':', ':', 'd', '\n', 'D', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'D', 'D',
'D', ' ', ' ', ' ', ' ', ' ', ' ', 'i', ':', ':', ':', ':', 'i', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'i', ':', ':', ':', ':', 'i', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'd', ':', ':', ':', ':',
':', ':', 'd', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', 'd', ':', ':', ':', ':', ':', ':', 'd', '\n', 'D', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'D', 'D', ' ', ' ',
' ', ' ', ' ', 'i', 'i', 'i', 'i', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', 'i', 'i', 'i', 'i', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'd', ':', ':', ':', ':', ':', ':', 'd', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'd', ':',
':', ':', ':', ':', ':', 'd', '\n', 'D', 'D', 'D', ':', ':', ':', ':', ':', 'D', 'D', 'D', 'D', 'D', ':', ':', ':', ':', ':', 'D', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'd', ':', ':', ':', ':', ':', 'd', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'd', ':', ':', ':', ':', ':', 'd',
' ', '\n', ' ', ' ', 'D', ':', ':', ':', ':', ':', 'D', ' ', ' ', ' ', ' ', 'D', ':', ':', ':', ':', ':', 'D', ' ', 'i', 'i', 'i', 'i', 'i', 'i', 'i', ' ', 'v',
'v', 'v', 'v', 'v', 'v', 'v', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'v', 'v', 'v', 'v', 'v', 'v', 'v', 'i', 'i', 'i', 'i', 'i', 'i', 'i', ' ',
' ', ' ', ' ', ' ', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', ':', ':', ':', ':', ':', 'd', ' ', ' ', ' ', ' ', ' ', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e',
'e', 'e', 'e', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', ':', ':', ':', ':', ':', 'd', ' ', '\n', ' ', ' ', 'D',
':', ':', ':', ':', ':', 'D', ' ', ' ', ' ', ' ', ' ', 'D', ':', ':', ':', ':', ':', 'D', 'i', ':', ':', ':', ':', ':', 'i', ' ', ' ', 'v', ':', ':', ':', ':',
':', 'v', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'v', ':', ':', ':', ':', ':', 'v', ' ', 'i', ':', ':', ':', ':', ':', 'i', ' ', ' ', ' ', 'd', 'd', ':',
':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'd', ' ', ' ', ' ', 'e', 'e', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'e',
'e', ' ', ' ', ' ', ' ', 'd', 'd', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'd', ' ', '\n', ' ', ' ', 'D', ':', ':', ':', ':', ':',
'D', ' ', ' ', ' ', ' ', ' ', 'D', ':', ':', ':', ':', ':', 'D', ' ', 'i', ':', ':', ':', ':', 'i', ' ', ' ', ' ', 'v', ':', ':', ':', ':', ':', 'v', ' ', ' ',
' ', ' ', ' ', ' ', ' ', 'v', ':', ':', ':', ':', ':', 'v', ' ', ' ', ' ', 'i', ':', ':', ':', ':', 'i', ' ', ' ', 'd', ':', ':', ':', ':', ':', ':', ':', ':',
':', ':', ':', ':', ':', ':', ':', ':', 'd', ' ', ' ', 'e', ':', ':', ':', ':', ':', ':', 'e', 'e', 'e', 'e', 'e', ':', ':', ':', ':', ':', 'e', 'e', ' ', 'd',
':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'd', ' ', '\n', ' ', ' ', 'D', ':', ':', ':', ':', ':', 'D', ' ', ' ', ' ', ' ',
' ', 'D', ':', ':', ':', ':', ':', 'D', ' ', 'i', ':', ':', ':', ':', 'i', ' ', ' ', ' ', ' ', 'v', ':', ':', ':', ':', ':', 'v', ' ', ' ', ' ', ' ', ' ', 'v',
':', ':', ':', ':', ':', 'v', ' ', ' ', ' ', ' ', 'i', ':', ':', ':', ':', 'i', ' ', 'd', ':', ':', ':', ':', ':', ':', ':', 'd', 'd', 'd', 'd', 'd', ':', ':',
':', ':', ':', 'd', ' ', 'e', ':', ':', ':', ':', ':', ':', 'e', ' ', ' ', ' ', ' ', ' ', 'e', ':', ':', ':', ':', ':', 'e', 'd', ':', ':', ':', ':', ':', ':',
':', 'd', 'd', 'd', 'd', 'd', ':', ':', ':', ':', ':', 'd', ' ', '\n', ' ', ' ', 'D', ':', ':', ':', ':', ':', 'D', ' ', ' ', ' ', ' ', ' ', 'D', ':', ':', ':',
':', ':', 'D', ' ', 'i', ':', ':', ':', ':', 'i', ' ', ' ', ' ', ' ', ' ', 'v', ':', ':', ':', ':', ':', 'v', ' ', ' ', ' ', 'v', ':', ':', ':', ':', ':', 'v',
' ', ' ', ' ', ' ', ' ', 'i', ':', ':', ':', ':', 'i', ' ', 'd', ':', ':', ':', ':', ':', ':', 'd', ' ', ' ', ' ', ' ', 'd', ':', ':', ':', ':', ':', 'd', ' ',
'e', ':', ':', ':', ':', ':', ':', ':', 'e', 'e', 'e', 'e', 'e', ':', ':', ':', ':', ':', ':', 'e', 'd', ':', ':', ':', ':', ':', ':', 'd', ' ', ' ', ' ', ' ',
'd', ':', ':', ':', ':', ':', 'd', ' ', '\n', ' ', ' ', 'D', ':', ':', ':', ':', ':', 'D', ' ', ' ', ' ', ' ', ' ', 'D', ':', ':', ':', ':', ':', 'D', ' ', 'i',
':', ':', ':', ':', 'i', ' ', ' ', ' ', ' ', ' ', ' ', 'v', ':', ':', ':', ':', ':', 'v', ' ', 'v', ':', ':', ':', ':', ':', 'v', ' ', ' ', ' ', ' ', ' ', ' ',
'i', ':', ':', ':', ':', 'i', ' ', 'd', ':', ':', ':', ':', ':', 'd', ' ', ' ', ' ', ' ', ' ', 'd', ':', ':', ':', ':', ':', 'd', ' ', 'e', ':', ':', ':', ':',
':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'e', ' ', 'd', ':', ':', ':', ':', ':', 'd', ' ', ' ', ' ', ' ', ' ', 'd', ':', ':', ':', ':',
':', 'd', ' ', '\n', ' ', ' ', 'D', ':', ':', ':', ':', ':', 'D', ' ', ' ', ' ', ' ', ' ', 'D', ':', ':', ':', ':', ':', 'D', ' ', 'i', ':', ':', ':', ':', 'i',
' ', ' ', ' ', ' ', ' ', ' ', ' ', 'v', ':', ':', ':', ':', ':', 'v', ':', ':', ':', ':', ':', 'v', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'i', ':', ':', ':', ':',
'i', ' ', 'd', ':', ':', ':', ':', ':', 'd', ' ', ' ', ' ', ' ', ' ', 'd', ':', ':', ':', ':', ':', 'd', ' ', 'e', ':', ':', ':', ':', ':', ':', 'e', 'e', 'e',
'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', ' ', ' ', 'd', ':', ':', ':', ':', ':', 'd', ' ', ' ', ' ', ' ', ' ', 'd', ':', ':', ':', ':', ':', 'd', ' ', '\n', ' ',
' ', 'D', ':', ':', ':', ':', ':', 'D', ' ', ' ', ' ', ' ', 'D', ':', ':', ':', ':', ':', 'D', ' ', ' ', 'i', ':', ':', ':', ':', 'i', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', 'v', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'v', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'i', ':', ':', ':', ':', 'i', ' ', 'd', ':', ':',
':', ':', ':', 'd', ' ', ' ', ' ', ' ', ' ', 'd', ':', ':', ':', ':', ':', 'd', ' ', 'e', ':', ':', ':', ':', ':', ':', ':', 'e', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', 'd', ':', ':', ':', ':', ':', 'd', ' ', ' ', ' ', ' ', ' ', 'd', ':', ':', ':', ':', ':', 'd', ' ', '\n', 'D', 'D', 'D', ':', ':', ':',
':', ':', 'D', 'D', 'D', 'D', 'D', ':', ':', ':', ':', ':', 'D', ' ', ' ', 'i', ':', ':', ':', ':', ':', ':', 'i', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'v',
':', ':', ':', ':', ':', ':', ':', 'v', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'i', ':', ':', ':', ':', ':', ':', 'i', 'd', ':', ':', ':', ':', ':', ':', 'd',
'd', 'd', 'd', 'd', ':', ':', ':', ':', ':', ':', 'd', 'd', 'e', ':', ':', ':', ':', ':', ':', ':', ':', 'e', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
'd', ':', ':', ':', ':', ':', ':', 'd', 'd', 'd', 'd', 'd', ':', ':', ':', ':', ':', ':', 'd', 'd', '\n', 'D', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':',
':', ':', ':', ':', ':', 'D', 'D', ' ', ' ', ' ', 'i', ':', ':', ':', ':', ':', ':', 'i', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'v', ':', ':', ':', ':',
':', 'v', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'i', ':', ':', ':', ':', ':', ':', 'i', ' ', 'd', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':',
':', ':', ':', ':', ':', ':', 'd', ' ', 'e', ':', ':', ':', ':', ':', ':', ':', ':', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', ' ', ' ', ' ', 'd', ':', ':', ':',
':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'd', '\n', 'D', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'D', 'D', 'D',
' ', ' ', ' ', ' ', ' ', 'i', ':', ':', ':', ':', ':', ':', 'i', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'v', ':', ':', ':', 'v', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', 'i', ':', ':', ':', ':', ':', ':', 'i', ' ', ' ', 'd', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'd', 'd', 'd', ':', ':', ':',
':', 'd', ' ', ' ', 'e', 'e', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'e', ' ', ' ', ' ', ' ', 'd', ':', ':', ':', ':', ':', ':', ':',
':', ':', 'd', 'd', 'd', ':', ':', ':', ':', 'd', '\n', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', 'D', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
'i', 'i', 'i', 'i', 'i', 'i', 'i', 'i', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'v', 'v', 'v', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', 'i', 'i', 'i', 'i', 'i', 'i', 'i', 'i', ' ', ' ', ' ', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', ' ', ' ', ' ', 'd', 'd', 'd', 'd', 'd', ' ', ' ', ' ',
' ', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', 'e', ' ', ' ', ' ', ' ', ' ', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', 'd', ' ', ' ', ' ',
'd', 'd', 'd', 'd', 'd', '\n', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
'\n','\0'};
char dbz_by[] = {
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n', 'B', ':', ':', ':', ':', ':', ':', ':', ':',
':', ':', ':', ':', ':', ':', ':', ':', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', '\n', 'B', ':', ':', ':', ':', ':', ':', 'B', 'B', 'B', 'B', 'B', 'B', ':', ':', ':', ':', ':', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n', 'B', 'B', ':', ':', ':', ':', ':', 'B', ' ', ' ', ' ', ' ', ' ',
'B', ':', ':', ':', ':', ':', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
'\n', ' ', ' ', 'B', ':', ':', ':', ':', 'B', ' ', ' ', ' ', ' ', ' ', 'B', ':', ':', ':', ':', ':', 'B', 'y', 'y', 'y', 'y', 'y', 'y', 'y', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', 'y', 'y', 'y', 'y', 'y', 'y', 'y', '\n', ' ', ' ', 'B', ':', ':', ':', ':', 'B', ' ', ' ', ' ', ' ', ' ', 'B', ':', ':', ':',
':', ':', 'B', ' ', 'y', ':', ':', ':', ':', ':', 'y', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'y', ':', ':', ':', ':', ':', 'y', ' ', '\n', ' ', ' ', 'B',
':', ':', ':', ':', 'B', 'B', 'B', 'B', 'B', 'B', ':', ':', ':', ':', ':', 'B', ' ', ' ', ' ', 'y', ':', ':', ':', ':', ':', 'y', ' ', ' ', ' ', ' ', ' ', ' ',
' ', 'y', ':', ':', ':', ':', ':', 'y', ' ', ' ', '\n', ' ', ' ', 'B', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'B', 'B', ' ', ' ', ' ',
' ', ' ', 'y', ':', ':', ':', ':', ':', 'y', ' ', ' ', ' ', ' ', ' ', 'y', ':', ':', ':', ':', ':', 'y', ' ', ' ', ' ', '\n', ' ', ' ', 'B', ':', ':', ':', ':',
'B', 'B', 'B', 'B', 'B', 'B', ':', ':', ':', ':', ':', 'B', ' ', ' ', ' ', ' ', ' ', 'y', ':', ':', ':', ':', ':', 'y', ' ', ' ', ' ', 'y', ':', ':', ':', ':',
':', 'y', ' ', ' ', ' ', ' ', '\n', ' ', ' ', 'B', ':', ':', ':', ':', 'B', ' ', ' ', ' ', ' ', ' ', 'B', ':', ':', ':', ':', ':', 'B', ' ', ' ', ' ', ' ', ' ',
'y', ':', ':', ':', ':', ':', 'y', ' ', 'y', ':', ':', ':', ':', ':', 'y', ' ', ' ', ' ', ' ', ' ', '\n', ' ', ' ', 'B', ':', ':', ':', ':', 'B', ' ', ' ', ' ',
' ', ' ', 'B', ':', ':', ':', ':', ':', 'B', ' ', ' ', ' ', ' ', ' ', ' ', 'y', ':', ':', ':', ':', ':', 'y', ':', ':', ':', ':', ':', 'y', ' ', ' ', ' ', ' ',
' ', ' ', '\n', ' ', ' ', 'B', ':', ':', ':', ':', 'B', ' ', ' ', ' ', ' ', ' ', 'B', ':', ':', ':', ':', ':', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'y', ':',
':', ':', ':', ':', ':', ':', ':', ':', 'y', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n', 'B', 'B', ':', ':', ':', ':', ':', 'B', 'B', 'B', 'B', 'B', 'B', ':', ':',
':', ':', ':', ':', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'y', ':', ':', ':', ':', ':', ':', ':', 'y', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n', 'B',
':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'y', ':', ':', ':',
':', ':', 'y', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n', 'B', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'B', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'y', ':', ':', ':', ':', ':', 'y', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n', 'B', 'B', 'B', 'B', 'B',
'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', 'B', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'y', ':', ':', ':', ':', ':', 'y', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', 'y', ':', ':', ':', ':', ':', 'y', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'y', ':', ':', ':', ':', ':', 'y', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', '\n', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'y', ':', ':',
':', ':', ':', 'y', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'y', ':', ':', ':', ':', ':', 'y', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
'\n', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'y', 'y', 'y', 'y', 'y', 'y', 'y', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n','\0'};
char dbz_zero[] = {
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '\n', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z',
'Z', 'Z', 'Z', 'Z', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'R', 'R', 'R', 'R', 'R', 'R',
'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', ' ', ' ', ' ', ' ',
' ', '\n', 'Z', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'Z', 'E', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':',
':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'E', 'R', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'R', ' ', ' ', ' ',
' ', ' ', 'O', 'O', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'O', 'O', ' ', ' ', ' ', '\n', 'Z', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':',
':', ':', ':', ':', ':', 'Z', 'E', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'E', 'R', ':', ':', ':',
':', ':', ':', 'R', 'R', 'R', 'R', 'R', 'R', ':', ':', ':', ':', ':', 'R', ' ', ' ', 'O', 'O', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':',
'O', 'O', ' ', '\n', 'Z', ':', ':', ':', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', ':', ':', ':', ':', ':', 'Z', ' ', 'E', 'E', ':', ':', ':', ':', ':', ':', 'E',
'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', ':', ':', ':', ':', 'E', 'R', 'R', ':', ':', ':', ':', ':', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ':', ':', ':', ':', ':',
'R', 'O', ':', ':', ':', ':', ':', ':', ':', 'O', 'O', 'O', ':', ':', ':', ':', ':', ':', ':', 'O', '\n', 'Z', 'Z', 'Z', 'Z', 'Z', ' ', ' ', ' ', ' ', ' ', 'Z',
':', ':', ':', ':', ':', 'Z', ' ', ' ', ' ', ' ', 'E', ':', ':', ':', ':', ':', 'E', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'E', 'E', 'E', 'E', 'E', 'E', ' ', ' ',
'R', ':', ':', ':', ':', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ':', ':', ':', ':', ':', 'R', 'O', ':', ':', ':', ':', ':', ':', 'O', ' ', ' ', ' ', 'O', ':', ':',
':', ':', ':', ':', 'O', '\n', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'Z', ':', ':', ':', ':', ':', 'Z', ' ', ' ', ' ', ' ', ' ', ' ', 'E', ':', ':', ':', ':',
':', 'E', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'R', ':', ':', ':', ':', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ':', ':', ':',
':', ':', 'R', 'O', ':', ':', ':', ':', ':', 'O', ' ', ' ', ' ', ' ', ' ', 'O', ':', ':', ':', ':', ':', 'O', '\n', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'Z', ':',
':', ':', ':', ':', 'Z', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'E', ':', ':', ':', ':', ':', ':', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', ' ', ' ', ' ',
' ', ' ', 'R', ':', ':', ':', ':', 'R', 'R', 'R', 'R', 'R', 'R', ':', ':', ':', ':', ':', 'R', ' ', 'O', ':', ':', ':', ':', ':', 'O', ' ', ' ', ' ', ' ', ' ',
'O', ':', ':', ':', ':', ':', 'O', '\n', ' ', ' ', ' ', ' ', ' ', ' ', 'Z', ':', ':', ':', ':', ':', 'Z', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'E', ':', ':',
':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'E', ' ', ' ', ' ', ' ', ' ', 'R', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':',
':', 'R', 'R', ' ', ' ', 'O', ':', ':', ':', ':', ':', 'O', ' ', ' ', ' ', ' ', ' ', 'O', ':', ':', ':', ':', ':', 'O', '\n', ' ', ' ', ' ', ' ', ' ', 'Z', ':',
':', ':', ':', ':', 'Z', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'E', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'E', ' ',
' ', ' ', ' ', ' ', 'R', ':', ':', ':', ':', 'R', 'R', 'R', 'R', 'R', 'R', ':', ':', ':', ':', ':', 'R', ' ', 'O', ':', ':', ':', ':', ':', 'O', ' ', ' ', ' ',
' ', ' ', 'O', ':', ':', ':', ':', ':', 'O', '\n', ' ', ' ', ' ', ' ', 'Z', ':', ':', ':', ':', ':', 'Z', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'E',
':', ':', ':', ':', ':', ':', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', ' ', ' ', ' ', ' ', ' ', 'R', ':', ':', ':', ':', 'R', ' ', ' ', ' ', ' ', ' ',
'R', ':', ':', ':', ':', ':', 'R', 'O', ':', ':', ':', ':', ':', 'O', ' ', ' ', ' ', ' ', ' ', 'O', ':', ':', ':', ':', ':', 'O', '\n', ' ', ' ', ' ', 'Z', ':',
':', ':', ':', ':', 'Z', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'E', ':', ':', ':', ':', ':', 'E', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
' ', ' ', ' ', ' ', ' ', ' ', 'R', ':', ':', ':', ':', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ':', ':', ':', ':', ':', 'R', 'O', ':', ':', ':', ':', ':', 'O', ' ',
' ', ' ', ' ', ' ', 'O', ':', ':', ':', ':', ':', 'O', '\n', 'Z', 'Z', 'Z', ':', ':', ':', ':', ':', 'Z', ' ', ' ', ' ', ' ', ' ', 'Z', 'Z', 'Z', 'Z', 'Z', ' ',
' ', 'E', ':', ':', ':', ':', ':', 'E', ' ', ' ', ' ', ' ', ' ', ' ', ' ', 'E', 'E', 'E', 'E', 'E', 'E', ' ', ' ', 'R', ':', ':', ':', ':', 'R', ' ', ' ', ' ',
' ', ' ', 'R', ':', ':', ':', ':', ':', 'R', 'O', ':', ':', ':', ':', ':', ':', 'O', ' ', ' ', ' ', 'O', ':', ':', ':', ':', ':', ':', 'O', '\n', 'Z', ':', ':',
':', ':', ':', ':', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', ':', ':', ':', 'Z', 'E', 'E', ':', ':', ':', ':', ':', ':', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E',
':', ':', ':', ':', ':', 'E', 'R', 'R', ':', ':', ':', ':', ':', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ':', ':', ':', ':', ':', 'R', 'O', ':', ':', ':', ':', ':',
':', ':', 'O', 'O', 'O', ':', ':', ':', ':', ':', ':', ':', 'O', '\n', 'Z', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':',
'Z', 'E', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'E', 'R', ':', ':', ':', ':', ':', ':', 'R', ' ',
' ', ' ', ' ', ' ', 'R', ':', ':', ':', ':', ':', 'R', ' ', 'O', 'O', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'O', 'O', ' ', '\n', 'Z',
':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'Z', 'E', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':', ':',
':', ':', ':', ':', ':', ':', ':', 'E', 'R', ':', ':', ':', ':', ':', ':', 'R', ' ', ' ', ' ', ' ', ' ', 'R', ':', ':', ':', ':', ':', 'R', ' ', ' ', ' ', 'O',
'O', ':', ':', ':', ':', ':', ':', ':', ':', ':', 'O', 'O', ' ', ' ', ' ', '\n', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z', 'Z',
'Z', 'Z', 'Z', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'E', 'R', 'R', 'R', 'R', 'R', 'R', 'R',
'R', ' ', ' ', ' ', ' ', ' ', 'R', 'R', 'R', 'R', 'R', 'R', 'R', ' ', ' ', ' ', ' ', ' ', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', 'O', ' ', ' ', ' ', ' ', ' ',
'\n','\0'};