#include "utilities.h"
#include <dirent.h>
#include <ctype.h>

/*program to make sources and include files from seed files*/
/*this code prodcuce a hardcord*/

#define TABLE_SIZE 512
#define INDENT 26
enum en_tag{t_none,t_name,t_alias,t_precedence,t_associativity,t_sign,t_builtinrules,t_attributes,t_proto,t_source,t_data,t_doc};
char *readcontent(char **ptr,char *ps, char *tag, int skipc){
	char *p,*pss,*pe,sav;
	
	for(p=*ptr;*p;p++){
		/*skip comment*/
		if(skipc){
			if(*p=='/'){
				p++;
				if(*p=='/'){
					for(;*p;p++){if(*p=='\n'){break;}}
				}else if(*p=='*'){
					for(;*p;p++){
						if(*p=='*'){
							p++;
							if(*p=='/'){break;}
						}
					}
				}
			}
		}
		pe = p;
		/*read end tag*/
		if(*p=='<'){
			p++;
			if(*p=='/'){
				p++;
				pss=p; 
				for(;*p;p++){if(*p=='>'){break;}}
				sav = *p;
				*p = '\0';
				if(strcmp(tag,pss)!=0){
					//printf("tag unmatch : tag : %s\t pss : %s\n",tag,pss); 
					*p = sav;
					*ptr = p; 
					return NULL;
				}
				*p = sav;
				/*this break end main loop*/
				break;
			}
		}
	}
	sav = *pe;
	*pe='\0';
	if(*ps == '\0'){
		//printf("no content : %s\n",tag);
		pss = NULL;
	}
	else{pss = String_copy(ps);}
	*pe = sav;
	*ptr = p;
	return pss;
}

typedef struct st_slist{
	char *filein;
	char *name;
	char *doc;
	char *sfunc;
	char *proto;
	char *src;
	char *data;
	int sflag;
	struct st_slist *next;
}SList_Sub;
typedef SList_Sub *SList;

SList SList_create(char *filein, char *name, char *doc, char *sfunc, char *proto, char *src, char *data, int sflag){
	SList s = allocate(sizeof(SList_Sub));
	s->filein = filein;
	s->name = name;
	s->doc = doc;
	s->sfunc = sfunc;
	s->proto = proto;
	s->src = src;
	s->data = data;
	s->sflag = sflag;
	s->next = NULL;
	return s;
}

void SList_delete(SList s){
	if(s){
		deallocate(s->filein);
		deallocate(s->name);
		deallocate(s->doc);
		deallocate(s->sfunc);
		deallocate(s->proto);
		deallocate(s->src);
		deallocate(s->data);
		deallocate(s);
	}
}

void SList_deleteRoot(SList s){
	if(s){
		SList_delete(s);
		SList_deleteRoot(s->next);
	}
}


SList SList_append(SList s, SList s2){
	SList ss;
	if(!s){return s2;}
	for(ss=s;ss->next;ss=ss->next){}
	ss->next = s2;
	return s;
}
enum en_sflag{
	sf_init=0x0001,
	sf_finish=0x0002,
	sf_screate=0x0004,
	sf_sdelete=0x0008,
	sf_copy=0x0010,
	sf_print=0x0020,
	sf_docfunc=0x0040,
	sf_equals=0x0080
};
SList learn(SList list,char *filein){
	char tbuf[32];
	char *name=NULL,*alias=NULL,*fname=NULL,*precedence=NULL,*associativity=NULL,*sign=NULL,*builtinrules=NULL,*attributes=NULL,*proto=NULL,*source=NULL,*doc=NULL,*data=NULL;
	char sav;
	char *buf,*p,*ps,*sfunc,*id;
	enum en_tag tag=t_none;
	int sfunclen,namelen,sflag=0,skipc=0;
	
	//printf("filein : %s\n",filein);
	buf = String_read(filein);
	for(p=buf;*p;p++){
		skipc=0;
		switch(tag){
		  case t_none:
			if(*p=='<'){
				ps = ++p;
				for(;*p;p++){if(*p=='>'){break;}}
				sav = *p;
				*p='\0';
				snprintf(tbuf,32,"%s",ps);
				*p=sav;
				ps=p+1;
				if(*ps=='\n'){ps++;}
				if(strcmp(tbuf,"NAME")==0){tag=t_name;break;}
				if(strcmp(tbuf,"ALIAS")==0){tag=t_alias;break;}
				if(strcmp(tbuf,"PRECEDENCE")==0){tag=t_precedence;break;}
				if(strcmp(tbuf,"ASSOCIATIVITY")==0){tag=t_associativity;break;}
				if(strcmp(tbuf,"SIGN")==0){tag=t_sign;break;}
				if(strcmp(tbuf,"BUILTINRULES")==0){tag=t_builtinrules;break;}
				if(strcmp(tbuf,"ATTRIBUTES")==0){tag=t_attributes;break;}
				if(strcmp(tbuf,"PROTO")==0){tag=t_proto;skipc=1;break;}
				if(strcmp(tbuf,"SOURCE")==0){tag=t_source;skipc=1;break;}
				if(strcmp(tbuf,"DOC")==0){tag=t_doc;break;}
				if(strcmp(tbuf,"DATA")==0){tag=t_data;skipc=1;break;}
				fprintf(stderr,"undefined tag %s is found\n",tbuf);
			}
			break;
		  case t_name:
			name = readcontent(&p,ps,tbuf,skipc);
			tag = t_none;
			break;
		  case t_precedence:
			precedence = readcontent(&p,ps,tbuf,skipc);
			tag = t_none;
			break;
		  case t_associativity:
			associativity = readcontent(&p,ps,tbuf,skipc);
			tag = t_none;
			break;
		  case t_sign:
			sign = readcontent(&p,ps,tbuf,skipc);
			tag = t_none;
			break;
		  case t_builtinrules:
			builtinrules = readcontent(&p,ps,tbuf,skipc);
			String_replace(builtinrules,"\t\r\n\\",' ');
			tag = t_none;
			break;
		  case t_alias:
			alias = readcontent(&p,ps,tbuf,skipc);
			String_replace(alias,"\t\r\n\\",' ');
			tag = t_none;
			break;
		  case t_attributes:
			attributes = readcontent(&p,ps,tbuf,skipc);
			tag = t_none;
			break;
		  case t_source:
			source = readcontent(&p,ps,tbuf,skipc);
			tag = t_none;
			break;
		  case t_proto:
			proto = readcontent(&p,ps,tbuf,skipc);
			tag = t_none;
			break;
		  case t_doc:
			doc = readcontent(&p,ps,tbuf,skipc);
			tag = t_none;
			break;
		  case t_data:
			data = readcontent(&p,ps,tbuf,skipc);
			tag = t_none;
			break;
		  default:
			break;
		}
	}
	if(!attributes){attributes = String_copy("0");}
	if(!precedence){precedence = String_copy("0");}
	if(!associativity){associativity = String_copy("0");}
	if(!sign){sign = String_copy("NULL");}
	if(!builtinrules){builtinrules = String_copy("NULL");}
	else{
		fname = allocate(sfunclen=(strlen(builtinrules)+22));
		snprintf(fname,sfunclen,"Expr_createString(\"%s\")",builtinrules);
		deallocate(builtinrules);
		builtinrules = fname;
	}
	if(!alias){alias = String_copy("NULL");}
	else{
		fname = allocate(sfunclen=(strlen(alias)+22));
		snprintf(fname,sfunclen,"Expr_createString(\"%s\")",alias);
		deallocate(alias);
		alias = fname;
	}
	
	/*
	printf("name : %s\n",name);
	printf("attributes : %s\n",attributes);
	printf("precedence : %s\n",precedence);
	printf("associativity : %s\n",associativity);
	printf("sign : %s\n",sign);
	printf("builtinrules : %s\n",builtinrules);
	printf("doc : %s\n",doc);
	*/
	//printf("name : %s\n",name);
	//printf("data : %s\n",data);
	namelen=strlen(name);
	if(source){
		/*set init flag when data initialization function is found*/
		fname = allocate(namelen+14);
		snprintf(fname,namelen+14,"%s_static_create(",name);
		if(strstr(source,fname)){sflag |= sf_screate;}
		deallocate(fname);
		/*set init flag when data initialization function is found*/
		fname = allocate(namelen+16);
		snprintf(fname,namelen+16,"%s_static_delete(",name);
		if(strstr(source,fname)){sflag |= sf_sdelete;}
		deallocate(fname);
		/*set init flag when data initialization function is found*/
		fname = allocate(namelen+12);
		snprintf(fname,namelen+12,"%s_data_init(",name);
		if(strstr(source,fname)){sflag |= sf_init;}
		deallocate(fname);
		/*set del flag when data copy function is found*/
		fname = allocate(namelen+14);
		snprintf(fname,namelen+14,"%s_data_finish(",name);
		if(strstr(source,fname)){sflag |= sf_finish;}
		deallocate(fname);
		/*set copy flag when data copy function is found*/
		fname = allocate(namelen+12);
		snprintf(fname,namelen+12,"%s_data_copy(",name);
		if(strstr(source,fname)){sflag |= sf_copy;}
		deallocate(fname);
		/*set doc flag when toString function is found*/
		fname = allocate(namelen+6);
		snprintf(fname,namelen+6,"%s_doc(",name);
		if(strstr(source,fname)){sflag |= sf_docfunc;}
		deallocate(fname);
		/*set print flag when toString function is found*/
		fname = allocate(namelen+11);
		snprintf(fname,namelen+11,"%s_toString(",name);
		if(strstr(source,fname)){sflag |= sf_print;}
		deallocate(fname);
		/*set equal flag when data equals function is found*/
		fname = allocate(namelen+14);
		snprintf(fname,namelen+14,"%s_data_equals(",name);
		if(strstr(source,fname)){sflag |= sf_equals;}
		deallocate(fname);
		/*check if "name()" function is exist in source*/
		namelen = strlen(name);
		fname = allocate(namelen+3);
		snprintf(fname,namelen+3," %s(",name);
		if(strstr(source,fname)==NULL){
			deallocate(fname);
			fname = String_copy("NULL");
		}else{
			deallocate(fname);
			fname = String_copy(name);
		}
	  funcnull:
		sfunc = allocate(sfunclen = (20 + strlen(name) + strlen(fname) + strlen(attributes) + strlen(precedence) + strlen(associativity) + strlen(sign) + strlen(builtinrules) + strlen(alias) + 4));
		snprintf(sfunc,sfunclen,"SYMBOL_FUNC(%s,%s,%s,%s,%s,%s,%s,%s)",name,fname,attributes,precedence,associativity,sign,builtinrules,alias);
	}else{
		fname = String_copy("NULL");
		goto funcnull;
	}
	
	deallocate(fname);
	deallocate(buf);
	deallocate(precedence);
	deallocate(associativity);
	deallocate(sign);
	deallocate(builtinrules);
	deallocate(attributes);
	deallocate(alias);
	/*
	if(proto && proto[0] == '\0'){
		//fprintf(stderr,"proto null\n");
		deallocate(proto);
		proto = NULL;
	}
	if(source && source[0] == '\0'){
		//fprintf(stderr,"source null\n");
		deallocate(source);
		source = NULL;
	}
	*/
	return SList_append(list,SList_create(String_copy(filein),name,doc,sfunc,proto,source,data,sflag));
}

typedef struct st_clist{
	SList s;
	char *name;
	struct st_clist *next;
}CList_Sub;
typedef CList_Sub *CList;

CList CList_create(SList s, char *name){
	CList c = allocate(sizeof(CList_Sub));
	c->s = s;
	c->name = String_copy(name);
	c->next = NULL;
	return c;
}

void CList_delete(CList c){
	if(c){
		SList_deleteRoot(c->s);
		deallocate(c->name);
		deallocate(c);
	}
}

void CList_deleteRoot(CList c){
	if(c){
		CList_delete(c);
		CList_deleteRoot(c->next);
	}
}


CList CList_append(CList c, CList c2){
	CList cc;
	if(!c){return c2;}
	for(cc=c;cc->next;cc=cc->next){}
	cc->next = c2;
	return c;
}


CList master(CList list, char *dirin, char *fileout){
	SList ss,s=NULL;
	DIR *dp=NULL;
	FILE *fp=NULL;
	struct dirent *entry;
	struct stat stfi,stfo;
	char seeddir[256],srcdir[256],filein[256],srcfile[256];
	//fprintf(stderr,"%s\n",getFileName(dirin));
	snprintf(seeddir,256,"%s/seeds",dirin);
	snprintf(srcdir,256,"%s/src",dirin);
	if((dp = opendir(seeddir))==NULL){perror("master"); return NULL;}
	while((entry=readdir(dp))!=NULL){
		if(entry->d_name[0]=='.'){continue;}
		if(strcmp(getFileExtension(entry->d_name),".xml")==0){
			snprintf(filein,256,"%s/%s",seeddir,entry->d_name);
			//fprintf(stderr,"%s\n",filein);
			s=learn(s,filein);
		}
	}
	closedir(dp);
	/*if source file info is written in seed file, make source file*/
	for(ss=s;ss;ss=ss->next){
		if(ss->src){
			snprintf(srcfile,256,"%s/%s.c",srcdir,ss->name);
			if(stat(srcfile,&stfo) != -1){
				if(stat(ss->filein,&stfi) != -1){
					if(stfi.st_mtime <= stfo.st_mtime){continue;}
				}
			}
			fprintf(stdout,"making %s\n",srcfile);
			fp = fopen(srcfile,"w");
			fprintf(fp,"#include \"%s\"\n",getFileName(fileout));
			String_fwrite(fp,ss->src);
			fclose(fp);
		}
	}
	return CList_append(list,CList_create(s,getFileName(dirin)));
}

int main(int argc, char *argv[]){
	FILE *fp=stdout,*fp2=stdout,*fp3=stdout,*fp4=stdout;
	CList c,list=NULL;
	SList s=NULL;
	DIR *dp=NULL;
	struct dirent *entry;
	char dirin[256],*buf,*spaces,*p;
	int dbflag=0;
	
	if(argc<5){fprintf(stderr,"usage : %s knowledgedir knowledge.c knowledge.h data.c data.h dbflag\n",argv[0]); return -1;}
#ifdef LEAKDETECT
	LeakDetector_set(stdout);
#endif
	/*fp2 : source file*/
	/*fp : header file*/
	/*fp4 : source file 2*/	
	/*fp3 : header file 2*/
	dbflag=atoi(argv[6]);
	if((fp2 = fopen(argv[2],"w"))==NULL){perror("learn"); return -1;}
	if((fp = fopen(argv[3],"w"))==NULL){perror("learn"); return -1;}
	if((fp4 = fopen(argv[4],"w"))==NULL){perror("learn"); return -1;}
	if((fp3 = fopen(argv[5],"w"))==NULL){perror("learn"); return -1;}
	if((dp = opendir(argv[1]))==NULL){perror("learn"); return -1;}
	while((entry=readdir(dp))!=NULL){
		if(entry->d_name[0]=='.'){continue;}
		snprintf(dirin,256,"%s/%s",argv[1],entry->d_name);
		list = master(list,dirin,argv[3]);
	}
	closedir(dp);
	
	/*for(c=list;c;c=c->next){fprintf(stderr,"%s\n",c->name);}*/
	fprintf(fp,"#ifndef ");
	for(p=getFileName(argv[3]);*p;p++){
		if(*p=='.'){break;}
		fputc(toupper(*p),fp);
	}
	fprintf(fp,"_H\n");
	fprintf(fp,"#define ");
	for(p=getFileName(argv[3]);*p;p++){
		if(*p=='.'){break;}
		fputc(toupper(*p),fp);
	}
	fprintf(fp,"_H\n");
	if(dbflag){
		fprintf(fp,"#define debug(...) fprintf(stderr,__VA_ARGS__);\n");
	}else{
		fprintf(fp,"#define debug(...) do{}while(0)\n");
	}
	fprintf(fp,"#include \"Expr.h\"\n");
	fprintf(fp,"#define TABLE_SIZE %d\n",TABLE_SIZE);
	
	
	fprintf(fp,"/*====== proto ======*/\n");
	fprintf(fp,"char *Knowledge_Expr_toString(Expr expr);\n");
	/*write proto*/
	for(c=list;c;c=c->next){
		fprintf(fp,"/*=== %s ===*/\n",c->name);
		for(s=c->s;s;s=s->next){
			if(s->proto){
				fprintf(fp,"%s",s->proto);
			}
		}
		fprintf(fp,"\n");
	}
	/*write symbol table*/
	fprintf(fp,"/*====== symbol table ======*/\n");
	for(c=list;c;c=c->next){
		fprintf(fp,"/*=== %s ===*/\n",c->name);
		fprintf(fp,"#define SYMBOL_");
		/*capitalize c->name*/ for(p=c->name;*p;p++){fputc(toupper(*p),fp);}
		fprintf(fp,"(SYMBOL_FUNC)");
		for(s=c->s;s;s=s->next){
			fprintf(fp," \\\n%s",s->sfunc);
		}
		fprintf(fp,"\n\n");
	}
	/*write enum*/
	fprintf(fp,"/*====== symbol id ======*/\n");
	fprintf(fp,"enum symbol_id{\n");
	for(c=list;c;c=c->next){
		fprintf(fp,"id_%s_start",c->name);
		for(s=c->s;s;s=s->next){
			fprintf(fp,",\nid_%s",s->name);
		}
		fprintf(fp,",\nid_%s_end,\n",c->name);
	}
	fprintf(fp,"id_end,\n");
	fprintf(fp,"};\n\n");
	/*write prototype of print function*/
	fprintf(fp,"/*proto of print function*/\n");
	fprintf(fp,"char *Expr_toString(Expr expr);\n\n");
	/*write prototype of doc function*/
	fprintf(fp,"#define DOC_INDENT %d\n",INDENT);
	fprintf(fp,"/*proto of doc function*/\n");
	for(c=list;c;c=c->next){
		fprintf(fp,"void doc_%s(int id);\n",c->name);
	}
	fprintf(fp,"\n");
	fprintf(fp,"/*proto of static field create function*/\n");
	fprintf(fp,"void *static_create(int id);\n\n");
	fprintf(fp,"/*proto of static field finish function*/\n");
	fprintf(fp,"void static_delete(void *data, int id);\n\n");
	fprintf(fp,"\n");
	fprintf(fp,"/*proto of SymbolTable initialize function*/\n");
	/*write prototype of SymbolTable initialize function*/
	for(c=list;c;c=c->next){fprintf(fp,"SymbolTable SymbolTable_%s(Symbol idtable[], char *nametable[]);\n",c->name);}
	fprintf(fp,"\n#define ADD_CONTEXT(idtable,nametable)");
	for(c=list;c;c=c->next){
		fprintf(fp," \\\nSymbolTable_addContext(\"");
		p=c->name;
		fputc(toupper(*p),fp); p++;
		/*capitalize*/ for(;*p;p++){fputc(*p,fp);}
		fprintf(fp,"`\",doc_%s,SymbolTable_%s(idtable,nametable));",c->name,c->name);
	}
	fprintf(fp,"\n\n#endif\n");
	
	/*write source file*/
	fprintf(fp2,"#include \"%s\"\n",getFileName(argv[3]));
	/*write print func*/
	fprintf(fp2,"/*====== print func ======*/\n");
	fprintf(fp2,"char *Knowledge_Expr_toString(Expr expr){\n");
	fprintf(fp2,"\tif(!expr){return String_copy(\"null\");}\n");
	fprintf(fp2,"\tswitch(expr->symbol->id){\n");
	for(c=list;c;c=c->next){
		for(s=c->s;s;s=s->next){
			if(s->sflag & sf_print){
				fprintf(fp2,"\t  case id_%s:\n",s->name);
				fprintf(fp2,"\t\treturn %s_toString(expr);\n\t\tbreak;\n",s->name);
			}
		}
	}
	fprintf(fp2,"\t  default:\n\t\tbreak;\n\t}\n");
	fprintf(fp2,"\treturn String_copy(expr->symbol->name);\n}\n");
	
	/*write doc*/
	spaces =repeatedString(' ',INDENT);
	fprintf(fp2,"/*====== symbol doc ======*/\n");
	for(c=list;c;c=c->next){
		fprintf(fp2,"/*=== %s ===*/\n",c->name);
		fprintf(fp2,"void doc_%s(int id){\n",c->name);
		for(s=c->s;s;s=s->next){
			fprintf(fp2,"\tif(id==id_%s){\n",s->name);
			if(s->sflag & sf_docfunc){
				fprintf(fp2,"\t\t%s_doc(id);\n",s->name);
			}else if(s->doc){
				fprintf(fp2,"\t\tprintf(\n");
				buf = strtok(s->doc,"\n");
				/* " -> /"  % -> %%*/
				while(buf){
					fprintf(fp2,"\t\t\t\"%s",spaces);
					for(p=buf;*p;p++){
						if(*p == '\"'){fputc('\\',fp2);}
						if(*p == '%'){fputc('%',fp2);}
						fputc(*p,fp2);
					}
					fprintf(fp2,"\\n\"\n");
					buf = strtok(NULL,"\n");
				}
				fprintf(fp2,"\t\t);\n");
			}
			fprintf(fp2,"\t}\n");
		}
		fprintf(fp2,"}\n\n");
	}
	fprintf(fp2,"/*static field create function*/\n");
	fprintf(fp2,"void *static_create(int id){\n");
	fprintf(fp2,"\tswitch(id){\n");
	for(c=list;c;c=c->next){
		for(s=c->s;s;s=s->next){
			if(s->sflag & sf_screate){
				fprintf(fp2,"\t  case id_%s:\n",s->name);
				fprintf(fp2,"\t\treturn %s_static_create();\n",s->name);
			}
		}
	}
	fprintf(fp2,"\t  default:\n");
	fprintf(fp2,"\t\tbreak;\n");
	fprintf(fp2,"\t}\n");
	fprintf(fp2,"\treturn NULL;\n");
	fprintf(fp2,"}\n\n");
	
	fprintf(fp2,"/*static field delete function*/\n");
	fprintf(fp2,"void static_delete(void *data, int id){\n");
	fprintf(fp2,"\tswitch(id){\n");
	for(c=list;c;c=c->next){
		for(s=c->s;s;s=s->next){
			if(s->sflag & sf_screate){
				fprintf(fp2,"\t  case id_%s:\n",s->name);
				fprintf(fp2,"\t\t%s_static_delete(data);\n\t\tbreak;\n",s->name);
			}
		}
	}
	fprintf(fp2,"\t  default:\n");
	fprintf(fp2,"\t\tbreak;\n");
	fprintf(fp2,"\t}\n");
	fprintf(fp2,"}\n\n");
	/*write table generate funcntion*/
	/*names defined in SymbolTable.h : Symbol_create, Symbol_vtoString , SymbolTable ADD_CONTEXT*/
	/*names defined in HashTable.h : HashTable_create2, HashTable_append*/
	fprintf(fp2,"#define SYMBOL_APPEND(name,func,attributes,precedence,associativity,sign,builtinrules,alias) \\\n");
	fprintf(fp2,"table = HashTable_append(table,#name,symbol=Symbol_create(#name,id_##name,func,attributes,associativity,precedence,sign,builtinrules,alias));\\\n");
	fprintf(fp2,"idtable[id_##name]=symbol;\\\n");
	fprintf(fp2,"nametable[id_##name]=#name;\n\n");	
	for(c=list;c;c=c->next){
		fprintf(fp2,"Symbol symbol;\n");
		fprintf(fp2,"SymbolTable SymbolTable_%s(Symbol idtable[], char *nametable[]){\n",c->name);
		fprintf(fp2,"\tSymbolTable table = HashTable_create2(%d,Symbol_vtoString);\n",TABLE_SIZE);
		fprintf(fp2,"\tSYMBOL_");
		/*capitalize*/ for(p=c->name;*p;p++){fputc(toupper(*p),fp2);}
		fprintf(fp2,"(SYMBOL_APPEND)\n");
		fprintf(fp2,"\treturn table;\n}\n");
	}
	
	/*write data types*/
	fprintf(fp3,"#ifndef ");
	for(p=getFileName(argv[4]);*p;p++){
		if(*p=='.'){break;}
		fputc(toupper(*p),fp3);
	}
	fprintf(fp3,"_H\n");
	fprintf(fp3,"#define ");
	for(p=getFileName(argv[4]);*p;p++){
		if(*p=='.'){break;}
		fputc(toupper(*p),fp3);
	}
	fprintf(fp3,"_H\n");
	fprintf(fp3,"#define R64_LIMIT 52\n");
	fprintf(fp3,"#define I32_LIMIT 31\n");
	fprintf(fp3,"#define I64_LIMIT 63\n");
	fprintf(fp3,"#include <gmp.h>\n");
	fprintf(fp3,"#include <mpfr.h>\n");
	fprintf(fp3,"/*====== data types ======*/\n");
	fprintf(fp3,"union un_data{\n\t");
	for(c=list;c;c=c->next){
		for(s=c->s;s;s=s->next){
			if(s->data){
				for(p=s->data;*p;p++){
					fputc(*p,fp3);
					if(*p=='\n'){fputc('\t',fp3);}
				}
			}
		}
	}
	fputc('\n',fp3);
	fprintf(fp3,"};");
	fprintf(fp3,"/*====== data initialization function ======*/\n");
	fprintf(fp3,"void data_init(union un_data *data, int id);\n");
	fprintf(fp3,"/*====== data finish function ======*/\n");
	fprintf(fp3,"void data_finish(union un_data *data, int id);\n");
	fprintf(fp3,"/*====== data copy function ======*/\n");
	fprintf(fp3,"void data_copy(union un_data *datas,union un_data *datad,int id);\n");
	fprintf(fp3,"/*====== data equals function ======*/\n");
	fprintf(fp3,"int data_equals(union un_data *d1,union un_data *d2,int id);\n");	
	fprintf(fp3,"\n\n#endif\n");
	fprintf(fp4,"#include \"%s\"\n",getFileName(argv[3]));
	fprintf(fp4,"#include \"%s\"\n",getFileName(argv[5]));
	/*write data initialization function*/	
	fprintf(fp4,"/*====== data initialization function ======*/\n");
	fprintf(fp4,"void data_init(union un_data *data, int id){\n");
	fprintf(fp4,"\tswitch(id){\n");
	for(c=list;c;c=c->next){
		for(s=c->s;s;s=s->next){
			if(s->sflag & sf_init){
				fprintf(fp4,"\t  case id_%s:\n",s->name);
				fprintf(fp4,"\t\t%s_data_init(data);\n\t\tbreak;\n",s->name);
			}
		}
	}
	fprintf(fp4,"\t  default:\n");
	fprintf(fp4,"\t\tmemset(data,0,sizeof(union un_data));\n");
	fprintf(fp4,"\t}\n");
	fprintf(fp4,"}\n");
	/*write data finish function*/
	fprintf(fp4,"/*====== data finish function ======*/\n");
	fprintf(fp4,"void data_finish(union un_data *data,int id){\n");
	fprintf(fp4,"\tswitch(id){\n");
	for(c=list;c;c=c->next){
		for(s=c->s;s;s=s->next){
			if(s->sflag & sf_finish){
				fprintf(fp4,"\t  case id_%s:\n",s->name);
				fprintf(fp4,"\t\t%s_data_finish(data);\n\t\tbreak;\n",s->name);
			}
		}
	}
	fprintf(fp4,"\t}\n");
	fprintf(fp4,"}\n");
	/*write data copy function*/
	fprintf(fp4,"/*====== data copy function ======*/\n");
	fprintf(fp4,"void data_copy(union un_data *datas,union un_data *datad,int id){\n");
	fprintf(fp4,"\tswitch(id){\n");
	for(c=list;c;c=c->next){
		for(s=c->s;s;s=s->next){
			if(s->sflag & sf_copy){
				fprintf(fp4,"\t  case id_%s:\n",s->name);
				fprintf(fp4,"\t\t%s_data_copy(datas,datad);\n\t\tbreak;\n",s->name);
			}
		}
	}
	fprintf(fp4,"\t  default:\n");
	fprintf(fp4,"\t\tmemcpy(datad,datas,sizeof(union un_data));\n\t\tbreak;\n");
	fprintf(fp4,"\t}\n");
	fprintf(fp4,"}\n");
	/*write data equals function*/
	fprintf(fp4,"/*====== data equals function ======*/\n");
	fprintf(fp4,"int data_equals(union un_data *d1,union un_data *d2,int id){\n");
	fprintf(fp4,"\tswitch(id){\n");
	for(c=list;c;c=c->next){
		for(s=c->s;s;s=s->next){
			if(s->sflag & sf_equals){
				fprintf(fp4,"\t  case id_%s:\n",s->name);
				fprintf(fp4,"\t\treturn %s_data_equals(d1,d2);\n",s->name);
			}
		}
	}
	fprintf(fp4,"\t}\n");
	fprintf(fp4,"\treturn 1;\n");
	fprintf(fp4,"}\n");
	CList_deleteRoot(list);
	deallocate(spaces);
	fclose(fp2);
	fclose(fp);
	fclose(fp3);
	fclose(fp4);
#ifdef LEAKDETECT
	LeakDetector_finish();
#endif
	return 0;
}