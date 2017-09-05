#include "utilities.h"
#include "Array.h"
#include "LinkedList.h"
#include "Array_loadSDF.h"
#include <unistd.h>
 
#define C_u0 1.256637e-06
/*een : total electric field energy per grid*/
/*ben : total magnetic field energy per grid*/
/*ken : total kinetic energy per grid*/

void calculate_poynting(double *poynting, double *E, double *B){
	poynting[0] = (E[1]*B[2] - E[2]*B[1])/C_u0;
	poynting[1] = (E[2]*B[0] - E[0]*B[2])/C_u0;
	poynting[2] = (E[0]*B[1] - E[1]*B[0])/C_u0;
}

double dot(double *a, double *b){
	return a[0]*b[0] + a[1]*b[1] + a[2]*b[2];
}

int main(int argc, char *argv[]){
	char *filein,*fileout,*filebg=NULL,*filemask=NULL;
	
	int h,i,j,k;
	int sp[3] = {0,0,0};
	int ep[3] = {1,1,1};
	int n[3] = {1,1,1};
	double normal[3] = {0,0,0};
	double poynting[3] = {0,0,0};
	double E[3] = {0,0,0};
	double B[3] = {0,0,0};
	double cellsize,energy;
	
	LinkedList namelist=NULL,arylist=NULL;
	Array aryex,aryey,aryez,arybx,aryby,arybz;

	FILE *gp;

	if(argc > 3){
		filein = String_copy(argv[1]);	
		cellsize = atof(argv[2]);
	}else{
		printf(
			   "usage : %s [inputfile name] [cellsize]\n"
			   "this program calculate flow rate of the electromagnetic energy through boundary.\n"
			   "output : flow rate [J/s]\n",
			   argv[0]
		);
		exit(0);
	}

	arylist = Array_loadSDFList(filein,6,"ex","ey","ez","bx","by","bz");
	aryex = LinkedList_getIndex(arylist,0);
	aryey = LinkedList_getIndex(arylist,1);
	aryez = LinkedList_getIndex(arylist,2);
	arybx = LinkedList_getIndex(arylist,3);
	aryby = LinkedList_getIndex(arylist,4);
	arybz = LinkedList_getIndex(arylist,5);
	
	for(i=0;i<aryex->depth;i++){ep[i]=n[i]=aryex->dim[i];}
	
	energy = 0;

	for(h=0;h<aryex->depth;h++){
		normal[h] = 1;
		ep[h] = 1;
		for(i=sp[0];i<ep[0];i++){
			for(j=sp[1];j<ep[1];j++){
				for(k=sp[2];k<ep[2];k++){
					E[0] = Array_get(aryex,i,j,k);
					E[1] = Array_get(aryey,i,j,k);
					E[2] = Array_get(aryez,i,j,k);
					B[0] = Array_get(arybx,i,j,k);
					B[1] = Array_get(aryby,i,j,k);
					B[2] = Array_get(arybz,i,j,k);
					calculate_poynting(poynting,E,B);
					energy += cellsize*dot(poynting,normal);
				}
			}
		}
		ep[h] = n[h];

		normal[h] = -1;
		sp[h] = n[h] - 1;
		for(i=sp[0];i<ep[0];i++){
			for(j=sp[1];j<ep[1];j++){
				for(k=sp[2];k<ep[2];k++){
					E[0] = Array_get(aryex,i,j,k);
					E[1] = Array_get(aryey,i,j,k);
					E[2] = Array_get(aryez,i,j,k);
					B[0] = Array_get(arybx,i,j,k);
					B[1] = Array_get(aryby,i,j,k);
					B[2] = Array_get(arybz,i,j,k);
					calculate_poynting(poynting,E,B);
					energy += cellsize*dot(poynting,normal);
				}	
			}
		}	
		sp[h] = 0;
		normal[h] = 0;
	}
	printf("%e\n",energy);
	LinkedList_deleteRoot(arylist,Array_vdelete);
	deallocate(filein);
	return 0;
}
