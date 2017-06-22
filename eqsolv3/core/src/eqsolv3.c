#include <readline/readline.h>
#include <readline/history.h>
#include <pwd.h>
#include "knowledge.h"

int output_flag=1;
int message_flag=1;
Expr TreeForm_mod(Expr expr);
Expr FullForm_mod(Expr expr);
Expr OutputForm_mod(Expr expr);
Expr Evaluate(Expr expr);
void parser_init();
void parser_finish();
Expr parser_calltree();
Expr parse(char *str);

void initialize_readline();
void eqsolv3_completion_init(Expr tree);
char **eqsolv3_completion(const char *text, int start, int end);
char *eqslolv3_command_generator(const char *text, int state);

int main(int argc, char *argv[]){
	char *buf=NULL, *com=NULL, *prompt="eqsolv3>",historyfile[256];
	Expr expr=NULL;
	int opt,oneline=0;
	struct passwd *pw = getpwuid(getuid());
	while((opt= getopt(argc,argv,"c:h"))!=-1){
		switch(opt){
		  case 'c':
			com = String_stripdq(optarg); break;
		  case 'h':
			printf("options\n");
			printf("-c <command> : input command\n");
			return 0;
		}
	}
	snprintf(historyfile,256,"%s/.eqsolv3_histoy",pw->pw_dir);
	read_history(historyfile);
	mp_set_memory_functions(allocate2,reallocate2,deallocate2);
#ifdef LEAKDETECT
	LeakDetector_set(stdout);
#endif
	randomSeed((unsigned)time(NULL));
	SymbolTable_init();
	parser_init(SymbolTable_getContext());
	SymbolTable_init_build_internal();
	initialize_readline();
	/*fprintf(stderr,"size of data : %ld byte\n",sizeof(union un_data));
	fprintf(stderr,"size of Expr : %ld byte\n",sizeof(Expr));
	fprintf(stderr,"size of Expr_Sub : %ld byte\n",sizeof(Expr_Sub));
	fprintf(stderr,"size of int : %ld byte\n",sizeof(int));*/
	if(com){
		expr=parse(com);
		if(!expr){
			printf("parse error\n");
		}else{
			output_flag = 1;
			expr = Evaluate(expr);
			if(expr){
				if(output_flag){OutputForm_mod(expr);}
				Expr_deleteRoot(expr); expr = NULL;
			}
		}
	}else{
		for(;;){
			buf = readline(prompt);
			if(buf && *buf){
				expr = parse(buf);
				if(!expr){
					printf("parse error\n");
					free(buf);
					continue;
				}
				output_flag = 1;
				//Expr_debug(expr,0);
				expr = Evaluate(expr);
				if(!expr){break;}
				add_history(buf);
				if(output_flag && expr->symbol->id != id_Null){
					//putchar('\n');
					//TreeForm_mod(expr);
					//putchar('\n');
					//putchar('\n');
					OutputForm_mod(expr);
					//putchar('\n');
					//TreeForm_mod(expr);
				}
				Expr_deleteRoot(expr); expr = NULL;
				free(buf);
			}
		}
	}
	parser_finish();
	SymbolTable_finish();
#ifdef LEAKDETECT
	LeakDetector_finish();
#endif
	write_history(historyfile);
	return 0;
}

void initialize_readline(){
	rl_attempted_completion_function = eqsolv3_completion;
}
/*
char **eqsolv3_completion_mod(const char *text, int start, int end, Expr tree){
	char **matches=NULL;
	static Expr root;
	Expr e;
	//printf("text : %s\n",text);
	if(tree){
		root = tree;
	}else{
		if(start==0){
			matches = rl_completion_matches(text,eqslolv3_command_generator);
		}else{
			switch(rl_line_buffer[start-1]){
			  case '[':
			  case '(':
			  case ' ':
				matches = rl_completion_matches(text,eqslolv3_command_generator);
				break;
			  default:
				for(e=root->child;e;e=e->next){
					if(e->data.letter==rl_line_buffer[start-1]){
						matches = rl_completion_matches(text,eqslolv3_command_generator);
						break;
					}
				}
				break;
			}
		}
		rl_attempted_completion_over=1;
	}
	return matches;
}
void eqsolv3_completion_init(Expr tree){
	eqsolv3_completion_mod(NULL,0,0,tree);
}
*/

char **eqsolv3_completion(const char *text, int start, int end){
	char **matches=NULL;
	matches = rl_completion_matches(text,eqslolv3_command_generator);
	rl_attempted_completion_over=1;
	return matches;
}

char *eqslolv3_command_generator(const char *text, int state){
	static int i,len;
	static char **nametable;
	char *name;
	
	if(!state){
		i = 0;
		len = strlen(text);
		nametable = SymbolTable_callNameTable();
	}
	for(;i<id_end;i++){
		name = nametable[i];
		if(name){
			if(!strncmp(name,text,len)){
				i++;
				return String_copy(name);
			}
		}
	}
	return NULL;
}