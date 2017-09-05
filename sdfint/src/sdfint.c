#include "utilities.h"
#include "Array.h"
#include "LinkedList.h"
#include "Array_loadSDF.h"
#include <unistd.h>
 
#define C_e0 8.854187e-12
#define C_u0 1.256637e-06
/*een : total electric field energy per grid*/
/*ben : total magnetic field energy per grid*/
/*ken : total kinetic energy per grid*/

int main(int argc, char *argv[]){
	char *filein,*fileout,*filebg=NULL,*filemask=NULL;
	
	int i,j,k,opt;
	int ep[3] = {1,1,1};
	double ec,bc,bx,by,bz,ex,ey,ez;
	double energy;
	
	LinkedList namelist=NULL,arylist=NULL,arybglist=NULL;
	Array aryex,aryey,aryez,arybx,aryby,arybz;
	Array arybgex,arybgey,arybgez,arybgbx,arybgby,arybgbz;
	Array aryout;
	Array mask=NULL;

	FILE *gp;
	while((opt=getopt(argc,argv,"b:m:"))!=-1){
		switch(opt){
			case 'b':
				filebg = String_copy(optarg);
				break;
			case 'm':
				filemask = String_copy(optarg);
				break;
			default:
				goto usage;
				
		}
	}

	if(optind+1 >= argc){
		usage:
		printf(
			   "usage : %s [inputfile name] [outputfile name] [-b bg file name] [-m mask file name (.bary)]\n",
			   argv[0]
		);
		exit(0);	
	}else{
		filein = String_copy(argv[optind]);
		fileout = String_copy(argv[optind+1]);		
	}
	
	arylist = Array_loadSDFList(filein,6,"ex","ey","ez","bx","by","bz");
	aryex = LinkedList_getIndex(arylist,0);
	aryey = LinkedList_getIndex(arylist,1);
	aryez = LinkedList_getIndex(arylist,2);
	arybx = LinkedList_getIndex(arylist,3);
	aryby = LinkedList_getIndex(arylist,4);
	arybz = LinkedList_getIndex(arylist,5);
	
	if(filebg){
		arybglist = Array_loadSDFList(filebg,6,"ex","ey","ez","bx","by","bz");
		arybgex = LinkedList_getIndex(arybglist,0);
		arybgey = LinkedList_getIndex(arybglist,1);
		arybgez = LinkedList_getIndex(arybglist,2);
		arybgbx = LinkedList_getIndex(arybglist,3);
		arybgby = LinkedList_getIndex(arybglist,4);
		arybgbz = LinkedList_getIndex(arybglist,5);
	}

	if(filemask){
		if(!strcmp(".bmat",getFileExtension(filemask))){
			mask = Array_inputBMAT(filemask);	
		}else{
			mask = Array_input(filemask);
		}
	}
	
	aryout = Array_copy(aryex);
	for(i=0;i<aryex->depth;i++){ep[i]=aryex->dim[i];}
	
	ec = 0.5*C_e0; bc = 0.5/C_u0;
	energy = 0;
	
	if(filebg){
		for(i=0;i<ep[0];i++){
			for(j=0;j<ep[1];j++){
				for(k=0;k<ep[2];k++){
					ex = Array_get(aryex,i,j,k) - Array_get(arybgex,i,j,k);
					ey = Array_get(aryey,i,j,k) - Array_get(arybgey,i,j,k);
					ez = Array_get(aryez,i,j,k) - Array_get(arybgez,i,j,k);
					bx = Array_get(arybx,i,j,k) - Array_get(arybgbx,i,j,k);
					by = Array_get(aryby,i,j,k) - Array_get(arybgby,i,j,k);
					bz = Array_get(arybz,i,j,k) - Array_get(arybgbz,i,j,k);	
					energy=ec*(ex*ex+ey*ey+ez*ez)+bc*(bx*bx+by*by+bz*bz);
					if(filemask){
					 	energy *= Array_get(mask,i,j,k);
					}
					Array_set(aryout,energy,i,j,k);
				}
			}
		}
	}else{
		for(i=0;i<ep[0];i++){
			for(j=0;j<ep[1];j++){
				for(k=0;k<ep[2];k++){
					ex = Array_get(aryex,i,j,k);
					ey = Array_get(aryey,i,j,k);
					ez = Array_get(aryez,i,j,k);
					bx = Array_get(arybx,i,j,k);
					by = Array_get(aryby,i,j,k);
					bz = Array_get(arybz,i,j,k);
					energy=ec*(ex*ex+ey*ey+ez*ez)+bc*(bx*bx+by*by+bz*bz);
					if(filemask){
						energy *= Array_get(mask,i,j,k);	
					}
					Array_set(aryout,energy,i,j,k);
				}
			}
		}
	}
	LinkedList_deleteRoot(arylist,Array_vdelete);
	LinkedList_deleteRoot(arybglist,Array_vdelete);	
	//Array_print(aryout);
	Array_output(aryout,fileout,p_double);
	Array_delete(aryout);
	Array_delete(mask);
	deallocate(filein);
	deallocate(fileout);
	deallocate(filebg);
	return 0;
}
