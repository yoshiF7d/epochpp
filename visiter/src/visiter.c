#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
//#include "LeakDetector.h"
#include <time.h>
#include "HashTable.h"
#include "ArrayList.h"
#include "LinkedList.h"
#include "Data.h"
#include "visiterfunc.h"
#include "utilities.h"
#include <sys/types.h>
#include <pwd.h>

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT "\x1B[37m"

#define NOTE  "          get help : type \"help\"\n          quit : type \"quit\"\n          force quit : ctrl + c\n"
//#define HISTORYFILE "~/.visiter_history"
int main(int argc, char *argv[]){
	FILE *fp;
	int n,c=0,r=0, promptok=0;
	char prompt[256];
	char *fname, *buf, *sep=strdup(SEPDEF);
	char *comdef=NULL,*comfile=NULL;
	int opt;
	
	Data data=NULL;
	HashTable ht=NULL;
	LinkedList arglist=NULL;
	FuncContainer fc=NULL;
	VisiterInfo vi=NULL;
	Func func=NULL;
	#ifdef LEAKDETECT
	LeakDetector_set(stdout);
	#endif
	struct passwd *pw = getpwuid(getuid());
	char historyfile[256];
	snprintf(historyfile,256,"%s/.visiter_histoy",pw->pw_dir);
		
	while((opt= getopt(argc,argv,"c:h"))!=-1){
		switch(opt){
		  case 'c':
			comdef = String_stripdq(optarg); break;
		  case 'f':
			comfile = String_stripdq(optarg); break;
		  case 'h':
			printf("usage : %s [options] <filename> <row offset> <column offset> <row> <column> <\"separator\">\n",argv[0]);
			printf("\noptions\n");
			printf("-c <command> : input command\n");
			return 0;
		}
	}
	
	for(;optind<argc;optind++){arglist = LinkedList_append(arglist,argv[optind]);}
	//LinkedList_enum(arglist,putstr);
	
	ht = HashTable_create(64);
	FUNCLIST(APPEND)	
	vi = VisiterInfo_create(ht,data,NULL);
	/*VisiterInfo_print(vi);*/
	
	if(arglist){
		arglist = LinkedList_prepend(arglist,vi);
		visiter_load(arglist); /*data -> vi*/
		LinkedList_deleteRoot(arglist,NULL); arglist = NULL;
	}
	if(comfile){
		comfile = String_read(comfile);
		for(buf=strtok(comfile,"\n");buf;buf=strtok(NULL,"\n")){
			comdef = String_copy(buf);
			if((arglist=parsefunc(comdef)) == NULL){printf("%s?\n",buf);continue;}
			fname = LinkedList_get(arglist); LinkedList_setIndex(arglist,0,vi);
			if(NULL==(func=(Func)HashTable_get(ht,fname))){
				printf("%s ?\n",fname);
				continue;
			}
			func(arglist); LinkedList_deleteRoot(arglist,NULL);
			deallocate(comdef);
		}
		deallocate(comfile);
		goto visiter_finish;
	}
	if(comdef){
		if((arglist=parsefunc(comdef)) == NULL){printf("%s ?\n",comdef); goto visiter_finish;}
		fname = LinkedList_getIndex(arglist,0); LinkedList_setIndex(arglist,0,vi);
		if(NULL==(func=(Func)HashTable_get(ht,fname))){
			printf("%s ?\n",fname);
			goto visiter_finish;
		}
		func(arglist); LinkedList_deleteRoot(arglist,NULL);
		goto visiter_finish;
	}
	
	printface("VISITER","Hayashi Yoshiaki","0.0",NOTE);
	read_history(historyfile);
	while(1){
		//if(data){Data_print(data);}
		if(!promptok){
			VisiterInfo_printprompt(vi,prompt,256);
			promptok = 1;
		}
		buf = readline(prompt);
		if(buf &&*buf){add_history(buf);}
		if(!strcmp(buf,"quit") || !strcmp(buf,"exit")){deallocate(buf); break;}
		if(!strcmp(buf,"help")){printbasicdoc(); HashTable_enum(ht,printdoc); goto loopend;}		
		if(!strcmp(buf,"show")){VisiterInfo_print(vi); goto loopend;} 
		if(!strcmp(buf,"showHashTable")){HashTable_print(ht,NULL);goto loopend;}
		if(!strcmp(buf,"showsep")){ReadOptions_print(&(vi->options)); goto loopend;}
		if(!strcmp(buf,"readoptions")){ReadOptions_print(&(vi->options)); goto loopend;}
		if(!strcmp(buf,"ls")){system("ls"); goto loopend;}
		if(!strcmp(buf,"pwd")){system("pwd"); goto loopend;}
		if((arglist=parsefunc(buf)) == NULL){printf("%s ?\n",buf);goto loopend;}
		
		/*function name is stored in 1st slot. read it and replace it by VisiterInfo*/
		fname = LinkedList_getIndex(arglist,0);
		LinkedList_setIndex(arglist,0,vi);
		
		if(NULL==(func=(Func)HashTable_get(ht,fname))){
			printf("%s ?\n",fname);
			goto loopend;
		}
		if(func == visiter_load || func==visiter_unload ||
		   func == visiter_shape || func == visiter_rslice ||
		   func == visiter_cslice || func == visiter_rappend ||
		   func == visiter_cappend || func == visiter_rfourier ||
		   func == visiter_aveload || func == visiter_transpose || 
		   func == visiter_rrotate || func == visiter_lrotate || 
		   func == visiter_vreflect || func == visiter_hreflect
		   )
		{promptok=0;}
		
		func(arglist);	/*main process*/
		
	  loopend:		
		LinkedList_deleteRoot(arglist,NULL); arglist=NULL;
		deallocate(buf);
#ifdef LEAKDETECT
		LeakDetector_printall();
#endif
	}
	write_history(historyfile);
  visiter_finish:
	Data_delete(VisiterInfo_getData(vi));
	deallocate(VisiterInfo_getFileName(vi));
	HashTable_delete(ht,NULL);
	VisiterInfo_delete(vi);
	#ifdef LEAKDETECT
	LeakDetector_printall();
	LeakDetector_finish();
	#endif
	return 0;
}

