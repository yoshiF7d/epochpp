#include "knowledge.h"
typedef struct st_gnode{
	int cs;
	int cm;
	int ce;
	int isleaf;
	char *color;
	char *string;
	Expr original;
}GNode_Sub;
typedef GNode_Sub *GNode;
GNode GNode_create();
void GNode_delete(GNode g);
void GNode_print(GNode g);

GNode GNode_create(Expr expr){
	GNode g;
	g = allocate(sizeof(GNode_Sub));
	g->cs = 0;
	g->cm = 0;
	g->ce = 0;
	g->isleaf =1;
	g->string = Expr_toString(expr);
	g->color = Expr_getColor(expr);
	g->original = expr;
	return g;
}

void GNode_delete(GNode g){
	if(g){deallocate(g->string);}
	deallocate(g);
}

void GNode_print(GNode g){
	if(g){printf("%s : %d %d %d\n",g->string, g->cs, g->cm, g->ce);}
	else{printf("null\n");}
}

typedef struct st_gnode_column{
	GNode node;
	struct st_gnode_column *next;
}GColumn_Sub;
typedef GColumn_Sub *GColumn;

GColumn GColumn_create(GNode g);
void GColumn_delete(GColumn gc);
GColumn GColumn_append(GColumn gc, GColumn gc2);

GColumn GColumn_create(GNode g){
	GColumn gc = allocate(sizeof(GColumn_Sub));
	gc->node = g;
	gc->next = NULL;
	return gc;
}
void GColumn_delete(GColumn gc){
	GNode_delete(gc->node);
	deallocate(gc);
}

void GColumn_deleteRoot(GColumn gc){
	GColumn column,next;
	for(column=gc;column;column=next){
		next = column->next;
		GColumn_delete(column);
	}
}

GColumn GColumn_append(GColumn gc, GColumn gc2){
	GColumn column;
	if(!gc){return gc2;}
	for(column=gc;column->next;column=column->next){}
	column->next = gc2;
	return gc;
}

typedef struct st_gnode_row{
	GColumn column;
	struct st_gnode_row *next;
}GRow_Sub;
typedef GRow_Sub *GRow;

GRow GRow_create(GColumn gc);
void GRow_delete(GRow gr);
GRow GRow_append(GRow gr, GRow gr2);

GRow GRow_create(GColumn gc){
	GRow gr = allocate(sizeof(GRow_Sub));
	gr->column = gc;
	gr->next = NULL;
	return gr;
}

void GRow_delete(GRow gr){
	GColumn_deleteRoot(gr->column);
	deallocate(gr);
}

void GRow_deleteRoot(GRow gr){
	GRow row,next;
	for(row=gr;row;row=next){
		next = row->next;
		GRow_delete(row);
	}
}


GRow GRow_append(GRow gr, GRow gr2){
	GRow row;
	if(!gr){return gr2;}
	for(row=gr;gr->next;gr=gr->next);
	row->next = gr2;
	return gr;
}


GColumn findEldestSon(GRow root, Expr expr);
void graph_slideChild(GRow current, Expr expr,int slide);

#define MAX_BUF_LENGTH 12

GNode graphset(Expr expr, GRow current,int cs, int *ce, int cd){
	GRow row=NULL, next=NULL;
	GColumn column=NULL;
	GNode g=NULL;
	Expr child;
	int ce2,len;
	
	if(expr){
		if(expr->child){
			next = current->next;
			if(!next){next=current->next=GRow_create(NULL);}
			column = next->column;
			for(child=expr->child;child;child=child->next){
				column = GColumn_append(column,GColumn_create(graphset(child,next,*ce,ce,cd)));
			}
			column = GColumn_append(column,GColumn_create(NULL));
			next->column = column; /*in case column was NULL at first*/
		}
		g = GNode_create(expr);
		if(g->string){
			if(*(g->string) & 0x80){
				len = mbstowcs(NULL,g->string,0);
			}else{
				len = strlen(g->string);
			}
		}else{
			len=5; /*"NULL"*/
		}
		ce2 = cs + cd + len;
		
		g->cs = cs + cd;
		if(expr->child){
			if(*ce < ce2){
				graph_slideChild(next,expr,(ce2-*ce)/2);
				g->ce = ce2;
			}else{
				g->ce = *ce;
			}
			g->isleaf = 0;
		}else{g->ce = ce2;}
		g->cm = (g->cs+g->ce)/2;
		*ce = g->ce;
	}
	return g;
}

void graph_slideChild(GRow current,Expr expr, int slide){
	GNode g,glast;
	GColumn column, eldest = findEldestSon(current,expr);

	//printf("slide : %d\n",slide); GraphNode_print(LinkedList_get(eldest));
	for(column=eldest;column;column=column->next){
		if(!(column->node)){break;}
		column->node->cs += slide;
		column->node->ce += slide;
		column->node->cm += slide;
		if(column->node->original->child){
			graph_slideChild(current->next,column->node->original,slide);
		}
	}
}

GColumn findEldestSon(GRow root,Expr expr){
	GRow row;
	GColumn column;
	Expr son = expr->child;
	GNode g;
	for(row=root;row;row=row->next){
		for(column=row->column;column;column=column->next){
			if(!(column->node)){break;}
			if(column->node->original==son){goto end;}
		}
	}
  end:
	return column;
}

void graphdraw(GRow graphroot){
	int i,j,clast,flag=0,flag_first=1;
	GRow row;
	GColumn column;
	GNode g;
	
	/*
	for(row=graphroot,i=0;row;row=row->next,i++){
		printf("column %d\n",i);
		for(column=row->column;column;column=column->next){
			GNode_print(column->node);
		}
	}*/
	for(row=graphroot;row;row=row->next){
		clast = 0; flag=0;
		if(!flag_first){
			for(column=row->column;column;column=column->next){
				g = column->node;
				if(!g){flag=0;continue;}
				if(!flag){
					for(i=clast;i<g->cm;i++){printf(" ");}
					flag=1;
				}else{
					for(i=clast;i<g->cm;i++){printf("-");}
				}
				printf("|");
				clast = g->cm+1;
			}
			printf("\n");
			clast=0;
			for(column=row->column;column;column=column->next){
				g = column->node;
				if(!g){continue;}
				for(i=clast;i<g->cm;i++){printf(" ");}
				printf("|");
				clast = g->cm+1;
			}
		}else{flag_first = 0;}
		printf("\n");
		clast=0;
		for(column=row->column;column;column=column->next){
			g = column->node;
			if(!g){continue;}
			j = g->string ? strlen(g->string) : 5;
			for(i=clast;i < g->cm - j/2; i++){printf(" ");}
			printf("%s%s%s",g->color,g->string,KNRM);
			clast = i + j;
		}
		printf("\n");
		clast=0;
		flag = 0;
		for(column=row->column;column;column=column->next){
			g = column->node;
			if(!g){continue;}
			for(i=clast;i<g->cm;i++){printf(" ");}
			if(g->isleaf){printf(" ");}
			else{printf("|");}
			clast = g->cm+1;
		}
		printf("\n");
	}
}

Expr TreeForm_mod(Expr expr){
	GRow graphroot=NULL,row;
	GColumn column;
	int ce=0;
	if(!expr){printf("TreeForm_mod : null\n"); return expr;}
	graphroot = GRow_create(GColumn_create(NULL));
	
	graphroot->column->node=graphset(expr,graphroot,0,&ce,1);
	graphdraw(graphroot);
	GRow_deleteRoot(graphroot);
	return expr;
}

#define TREEFORM_H_MAXDEPTH 256
Expr TreeForm_modh_mod(Expr expr, char *branch){
	Expr e;
	int i,len,blen;
	char *p,*nbranch;
	if(!expr){
		printf("null\n");
		return expr;
	}
	printf(" %s%s%s ",Expr_getColor(expr),p=Expr_toString(expr),KNRM);
	len = (int)strlen(p) + 2;
	if(branch){blen = strlen(branch);}
	else{blen=0;}
	deallocate(p);
	
	nbranch = allocate(blen+len+4);
	if(branch){strncpy(nbranch,branch,blen);}
	for(i=blen;i<blen+len;i++){
		nbranch[i]=' ';
	}
	if(expr->child){
		if(expr->child->next){
			nbranch[blen+len]=' ';
			nbranch[blen+len+1]='|';
			nbranch[blen+len+2]=' ';
			nbranch[blen+len+3]='\0';
			printf(KWHT "-+-" KNRM);
			TreeForm_modh_mod(expr->child,nbranch);
			for(e=expr->child->next;e->next;e=e->next){
				if(branch)printf(KWHT "%s" KNRM,branch);
				for(i=0;i<len;i++){putchar(' ');}
				printf(KWHT " +-" KNRM);
				TreeForm_modh_mod(e,nbranch);
			}
			nbranch[blen+len]=' ';
			nbranch[blen+len+1]=' ';
			nbranch[blen+len+2]=' ';
			nbranch[blen+len+3]='\0';
			if(branch)printf(KWHT "%s" KNRM,branch);
			for(i=0;i<len;i++){putchar(' ');}
			printf(KWHT " \\-" KNRM);
			TreeForm_modh_mod(e,nbranch);
		}else{
			nbranch[blen+len]=' ';
			nbranch[blen+len+1]=' ';
			nbranch[blen+len+2]=' ';
			nbranch[blen+len+3]='\0';
			printf(KWHT "---" KNRM);
			TreeForm_modh_mod(expr->child,nbranch);
		}
	}else{printf("\n");}
	deallocate(nbranch);
	return expr;
}

Expr TreeForm_modh(Expr expr){
	char branch[TREEFORM_H_MAXDEPTH];
	branch[0]='\0';
	return TreeForm_modh_mod(expr,NULL);
}

Expr TreeForm(Expr expr){
	if(!expr->child){return expr;}
	TreeForm_mod(expr->child);
	output_flag = 0;
	return expr;
}
