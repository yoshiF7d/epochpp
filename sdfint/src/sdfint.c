#include "utilities.h"
#include "Array.h"
#include "LinkedList.h"
#include "Array_loadSDF.h"

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

int main(int argc, char *argv[]){
	char *filein,*spstr,*epstr;
	
	int i,j,k,epset=0;
	int sp[3]={0,0,0},ep[3]={0,0,0};
	double ec,bc,bx,by,bz,ex,ey,ez;
	double area=1.0,energy;
	
	Array aryex,aryey,aryez,arybx,aryby,arybz;
	LinkedList namelist,arylist;
	FILE *gp;
	if(argc < 2){
		printf(
			   "usage : %s [inputfile name] "
			   "[start point : (0,0,0)] "
			   "[end point : (xmax,ymax,zmax)] "
			   "[area of cell (m^2)]\n",
			   argv[0]
			   );
		exit(0);
	}else{
		filein = String_copy(argv[1]);
	}
	area = 1;
	
	if(argc >= 3){
		parse_point(spstr=String_copy(argv[2]),&sp[0],&sp[1],&sp[2]);
		deallocate(spstr);
	}
	if(argc >= 4){
		parse_point(epstr=String_copy(argv[3]),&ep[0],&ep[1],&ep[2]);
		deallocate(epstr);
		epset=1;
	}
	
	if(argc >= 5){
		area = atof(argv[4]);
	}
	
	if(access(filein,F_OK)==-1){
		fprintf(stderr,"sdf file does not exist\n"); exit(1);
	}
	arylist = Array_loadSDFList(filein,6,"ex","ey","ez","bx","by","bz");
	
	aryex = LinkedList_getIndex(arylist,0);
	aryey = LinkedList_getIndex(arylist,1);
	aryez = LinkedList_getIndex(arylist,2);
	
	arybx = LinkedList_getIndex(arylist,3);
	aryby = LinkedList_getIndex(arylist,4);
	arybz = LinkedList_getIndex(arylist,5);
	
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
