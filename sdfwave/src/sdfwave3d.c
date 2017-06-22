#include "utilities.h"
#include "Array.h"
#include "LinkedList.h"
#include "Array_loadSDF.h"
#include <dirent.h>

#define C_e0 8.854187e-12
#define C_u0 1.256637e-06
/*een : total electric field energy per grid*/
/*ben : total magnetic field energy per grid*/
/*ken : total kinetic energy per grid*/

void parse_point(char *point, int *x, int *y, int *z){
	int i;
	int v[3] = {0,0,0};
	char *p,*s;
	for(s=p=point,i=0;*p;p++){
		switch(*p){
		  case '(':
			*p = '\0';
			s = p+1;
			break;
		  case ',':
			*p = '\0';
			v[i] = atoi(s);
			i++;
			s = p+1;
			break;
		  case ')':
			*p = '\0';
			goto forend;
		}
	}
  forend:
	v[i] = atoi(s);
	*x = v[0];
	*y = v[1];
	*z = v[2];
}
/*100001*/
#define BSPEC_LIST(FUNC) \
FUNC(xmax)
FUNC(xmin)
FUNC(ymax) 
FUNC(ymin)
FUNC(zmax)
FUNC(zmin) 

#define FUNC1(entry) entry,
#define FUNC2(entry) \
if(!strcmp(str,#entry)){return entry;}\
else 

typedef enum en_bspec{
	BSPEC_LIST(FUNC1)
}Bspec;

Bspec Bspec_parse(const char* str,int sp[], int ep[]){
	BSPEC_LIST(FUNC2)
	{
		printf("Unknown bspec : %s\n",str);
		printf("Choose from xmin,ymin,zmin,xmax,ymax,zmax\n");
		return -1;
	}
}

void spep_set(Bspec bspec, Array ary, int sp[], int ep[]){
	for(i=0;i<ary->depth;i++){
		if(bspec == i*2){
			sp[i] = dim[i];
		}else if (bspec == i*2 + 1){
			ep[i] = 0;
		}
		ep[i] = ary->dim[i];
	}
}


int compare(void *c1, void *c2){
	return atoi((char*)c1)-atoi((char*)c2);
}

int main(int argc, char *argv[]){
	char *dirin,*filename;
	
	int i,j,k,epset=0;
	int sp[3]={0,0,0},ep[3]={0,0,0};
	double ec,bc,bx,by,bz,ex,ey,ez;
	double r=0;
	Bspec bspec;
	
	Array aryex,aryey,aryez,arybx,aryby,arybz,arybd;
	LinkedList namelist,arylist,list,s;
	DIR *dp=NULL;
	struct dirent *entry;
	struct stat statbuf;
	
	if(argc < 2){
		printf(
			   "usage : %s [sdfdir] [bspec : xmax] [r : 0]\n"
			   "dir   : input directory name\n"
			   "bspec : boundary specification. bspec can be either one of\n"
			   "        {xmin,xmax,ymin,ymax,zmin,zmax};\n"
			   "r     : distance between detector plane and boundary plane.\n"
			   argv[0]
			   );
		exit(0);
	}else{
		dirin = String_copy(argv[1]);
	}
	
	if(argc >= 3){
		if((bspec = Bspec_parse(argv[2]))<0){
			exit(0);
		}
	}else{
		bspec = xmin;
	}
	
	if(argc >= 4){
		r = atof(argv[3]);
	}else{
		r = 0;
	}
	
	while((entry=readdir(dp))!=NULL){
		if(!strcmp(".sdf",getFileExtension(entry->d_name))){
			list = LinkedList_append(list,String_copy(entry->d_name));
		}
	}
	list = LinkedList_sort(list,compare);
	for(s=list;s;s=s->next){
		filename = LinkedList_get(list);
		arylist = Array_loadSDFList(filename,6,"ex","ey","ez","bx","by","bz");
		aryex = LinkedList_getIndex(arylist,0);
		aryey = LinkedList_getIndex(arylist,1);
		aryez = LinkedList_getIndex(arylist,2);
		arybx = LinkedList_getIndex(arylist,3);
		aryby = LinkedList_getIndex(arylist,4);
		arybz = LinkedList_getIndex(arylist,5);
		spep_set(bspec,aryex,sp,ep);
		for(i=sp[0];i<ep[0];i++){
			for(j=sp[1];j<ep[1];j++){
				for(k=sp[2];k<ep[2];k++){
					ex = Array_get(aryex,i,j,k); 
					ey = Array_get(aryey,i,j,k); 
					ez = Array_get(aryez,i,j,k);
					bx = Array_get(arybx,i,j,k);
					by = Array_get(aryby,i,j,k);
					bz = Array_get(arybz,i,j,k);
					energy += area*(ec*(ex*ex+ey*ey+ez*ez)+bc*(bx*bx+by*by+bz*bz));
				}
			}
		}
	}

	
	if(!epset){
		for(i=0;i<aryex->depth;i++){ep[i]=aryex->dim[i];}
	}
	
	ec = 0.5*C_e0; bc = 0.5/C_u0;
	energy = 0;
	
	for(i=sp[0];i<ep[0];i++){
		for(j=sp[1];j<ep[1];j++){
			for(k=sp[2];k<ep[2];k++){
				ex = Array_get(aryex,i,j,k); 
				ey = Array_get(aryey,i,j,k); 
				ez = Array_get(aryez,i,j,k);
				bx = Array_get(arybx,i,j,k);
				by = Array_get(aryby,i,j,k);
				bz = Array_get(arybz,i,j,k);
				energy += area*(ec*(ex*ex+ey*ey+ez*ez)+bc*(bx*bx+by*by+bz*bz));
			}
		}
	}
	printf("%g\n",energy);
	LinkedList_deleteRoot(arylist,Array_vdelete);
	deallocate(filein);
	return 0;
}
