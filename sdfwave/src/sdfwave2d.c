#include "utilities.h"
#include "Data.h"
#include "LinkedList.h"
#include "Data_loadSDF.h"
#include <dirent.h>

#define C_e0 8.854187e-12
#define C_u0 1.256637e-06

#define BSPEC_LIST(FUNC) \
FUNC(xmax) FUNC(xmin) FUNC(ymax) FUNC(ymin)

#define FUNC1(entry) entry,
#define FUNC2(entry) \
if(!strcmp(str,#entry)){return entry;}\
else 

typedef enum en_bspec{
	BSPEC_LIST(FUNC1)
}Bspec;

int Bspec_parse(const char* str){
	BSPEC_LIST(FUNC2)
	{
		printf("Unknown bspec : %s\n",str);
		printf("Choose from xmax,xmin,ymax,ymin\n");
		return -1;
	}
}

int compare(void *c1, void *c2){
	return atoi((char*)c1)-atoi((char*)c2);
}

int main(int argc, char *argv[]){
	char *dirin,*filename,*fileout,*bspecstr;
	
	int i,j,n;
	int mat[2][2] = {{0,0},{0,0}};
	double energy,ec=0.5*C_e0,bc=0.5/C_u0,bx,by,bz,ex,ey,ez;
	double r=0;
	int bspec;
	
	Data dataex,dataey,dataez,databx,databy,databz,databd;
	LinkedList datalist,list,s;
	DIR *dp=NULL;
	struct dirent *entry;
	struct stat statbuf;
	
	if(argc < 2){
		printf(
			   "usage : %s [sdfdir] [bspec : xmax] [r : 0]\n"
			   "dir   : input directory name\n"
			   "bspec : boundary specification. bspec can be either one of\n"
			   "        {xmin,xmax,ymin,ymax,zmin,zmax};\n"
			   "r     : distance between detector plane and boundary plane.\n",
			   argv[0]
			   );
		exit(0);
	}else{
		dirin = String_copy(argv[1]);
	}
	
	if(argc >= 3){
		bspecstr = String_copy(argv[2]);
	}else{
		bspecstr = String_copy("xmin");
	}
	
	if((bspec = Bspec_parse(argv[2]))<0){
		exit(0);
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
	filename = LinkedList_get(list);
	dataex = Data_loadSDF(filename,"ex");
	
	switch(bspec){
	  case xmax:
		mat[1][0] = dataex->column-1;
	  case xmin:
		mat[0][0] = 0;
		mat[0][1] = 1;
		mat[1][1] = 0;
		n =  dataex->row;
		break;
	  case ymax:
		mat[0][0] = dataex->row-1;
	  case ymin:
		mat[0][1] = 0;
		mat[1][0] = 0;
		mat[1][1] = 1;
		n = dataex->column;
		break;
	}
	Data_delete(dataex);
	databd = Data_create(n,LinkedList_getLength(list));
	
	for(s=list,i=0;s;s=s->next,i++){
		filename = LinkedList_get(list);
		datalist = Data_loadSDFList(filename,6,"ex","ey","ez","bx","by","bz");
		dataex = LinkedList_getIndex(datalist,0);
		dataey = LinkedList_getIndex(datalist,1);
		dataez = LinkedList_getIndex(datalist,2);
		databx = LinkedList_getIndex(datalist,3);
		databy = LinkedList_getIndex(datalist,4);
		databz = LinkedList_getIndex(datalist,5);
		for(j=0;j<n;j++){
			ex = Data_get(dataex,mat[0][0]+j*mat[0][1],mat[1][0]+j*mat[1][1]);
			ey = Data_get(dataey,mat[0][0]+j*mat[0][1],mat[1][0]+j*mat[1][1]);
			ez = Data_get(dataez,mat[0][0]+j*mat[0][1],mat[1][0]+j*mat[1][1]);
			bx = Data_get(databx,mat[0][0]+j*mat[0][1],mat[1][0]+j*mat[1][1]);
			by = Data_get(databy,mat[0][0]+j*mat[0][1],mat[1][0]+j*mat[1][1]);
			bz = Data_get(databz,mat[0][0]+j*mat[0][1],mat[1][0]+j*mat[1][1]);
			energy = ec*(ex*ex+ey*ey+ez*ez)+bc*(bx*bx+by*by+bz*bz);
			Data_set(databd,i,j,energy);
		}
		LinkedList_deleteRoot(datalist,Data_vdelete);
	}
	fileout = allocate(10);
	snprintf(fileout,"%s.bmat",bspecstr,10);
	Data_output(databd,fileout,p_float);
	LinkedList_deleteRoot(list,deallocate);
	deallocate(fileout);
	deallocate(bspecstr);
	deallocate(dirin);
	return 0;
}
