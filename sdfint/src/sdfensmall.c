#include "utilities.h"
#include "Array.h"
#include "LinkedList.h"
#include "Array_loadSDF.h"
#include <unistd.h>
#include <dirent.h>
 
#define C_e0 8.854187e-12
#define C_u0 1.256637e-06

/*een : total electric field energy per grid*/
/*ben : total magnetic field energy per grid*/
/*ken : total kinetic energy per grid*/

typedef struct st_spec{
	char *name;
	Array ary;
	double energy;
}Spec_Sub;
typedef struct st_spec *Spec;

Spec Spec_create(char *name){
	Spec spec = allocate(sizeof(Spec_Sub));
	spec->name = name;
	spec->ary = NULL;
	spec->energy = 0;
	return spec;
}

void Spec_delete(Spec spec){
	deallocate(spec->name);
}

void Spec_vdelete(void *spec){Spec_delete(spec);}

void setdomain(int domain[][2], char *string){
	char *buf,*p,*ps;
	int i=0,read=0,index=0;
	buf = String_copy(string);
	for(ps=p=string;*p;p++){
		switch(*p){
			case ',':
				*p='\0';
				if(index/2>2){printf("subdomain specification %s have more than 6 inputs\n",string); return;}
				domain[index/2][index%2]=atoi(ps);
				read=0; index++;
			case ')':
				*p='\0';
				break;
		}
		if(!read){
			if(isdigit(*p)){ps=p;read=1;}
		}
	}
	if(index/2>2){printf("subdomain specification %s have more than 6 inputs\n",string); return;}
	domain[index/2][index%2]=atoi(ps);
	for(i=0;i<6;i++){
		printf("domain[%d][%d] : %d\n",i/2,i%2,domain[i/2][i%2]);
	}	
	deallocate(buf);
}

int main(int argc, char *argv[]){
	char *dirin,*filein=NULL,*fileout;
	
	int i,j,k,s,len,domsetflag=0,initialflag=0;
	int domain[3][2]={{0,1},{0,1},{0,1}};
	double ec,bc,bx,by,bz,ex,ey,ez,ek;
	double een=0,ben=0,ken=0;	
	LinkedList list=NULL,namelist=NULL,arylist=NULL,specarylist=NULL,speclist=NULL;
	Array aryex,aryey,aryez,arybx,aryby,arybz,aryek;
	Spec spec;
	FILE *fp=stdout;;
	DIR *dp;
	int opt;
	struct dirent *entry;

	while((opt=getopt(argc,argv,"d:s:"))!=-1){
		switch(opt){
			case 'd':
				setdomain(domain,optarg);
				domsetflag=1;
				break;
			case 's':
				speclist = LinkedList_append(speclist,Spec_create(String_join("ekbar/",optarg)));
				break;
			default:
				goto usage;
				
		}
	}

	if(optind >= argc){
		usage:
		printf(
			   "usage : %s [inputdir name] [outputfile name] (-d [subdomain specification]) (-s [species name])\n"
			   "subdomain specification : \"(irs,ire,ics,ice,(ihs,ihe))\". irs,ire,ics,ice is start,end index of row,column of subdomain\n"
			   "ihs,ihe is needed in epoch3d data.\n"
			   "species name : when there is ekbar/[species] in sdf file\n" 
			   "use this command line option : example : \"-s electron -s proton\"\n",
			   argv[0]
		);
		exit(0);	
	}else if(argc == optind+1){
		dirin = String_copy(argv[optind]);
	}else{
		fileout = String_copy(argv[optind+1]);		
		fp = fopen(fileout,"w");
	}
	
	namelist = LinkedList_append(namelist,"ex");
	namelist = LinkedList_append(namelist,"ey");
	namelist = LinkedList_append(namelist,"ez");
	namelist = LinkedList_append(namelist,"bx");
	namelist = LinkedList_append(namelist,"by");
	namelist = LinkedList_append(namelist,"bz");
	namelist = LinkedList_append(namelist,"ekbar");
	for(list=speclist;list;list=list->next){
		spec = list->content;
		LinkedList_append(namelist,spec->name);
	}
	ec = 0.5*C_e0; bc = 0.5/C_u0;
	if((dp=opendir(dirin))==NULL){fprintf(stderr,"%s opendir error",argv[0]); return -1;}
	while((entry=readdir(dp))!=NULL){
		if(entry->d_name[0]=='.'){continue;}
		if(strcmp(getFileExtension(entry->d_name),".sdf")==0){
			len = sizeof(dirin) + sizeof(entry->d_name)+2;
			if(!filein){filein = allocate(len);}
			snprintf(filein,len,"%s/%s",dirin,entry->d_name);
			printf("processing %s\n",filein);
			printf("\033[F\033[J");
			arylist = Array_loadSDFList2(filein,namelist);
			aryex=LinkedList_getIndex(arylist,0);
			aryey=LinkedList_getIndex(arylist,1);
			aryez=LinkedList_getIndex(arylist,2);
			arybx=LinkedList_getIndex(arylist,3);
			aryby=LinkedList_getIndex(arylist,4);
			arybz=LinkedList_getIndex(arylist,5);
			aryek=LinkedList_getIndex(arylist,6);
			for(list=speclist,i=7;list;list=list->next,i++){
				spec=list->content;
				spec->ary = LinkedList_getIndex(arylist,i);
			}
			if(!domsetflag){
				for(i=0;i<aryex->depth;i++){
					domain[i][1]=aryex->dim[i];
				}
				for(i=0;i<6;i++){
					printf("domain[%d][%d] : %d\n",i/2,i%2,domain[i/2][i%2]);
				}	
				domsetflag=1;
			}
			if(!initialflag){
				fprintf(fp,"filename\telectric energy [J]\tmagnetic energy [J]\tkinetic energy [J]\t");
				for(list=speclist;list;list=list->next){spec=list->content;fprintf(fp,"kinetic energy (%s) [J]\t",spec->name);}
				fprintf(fp,"\n");
				initialflag=1;
			}
			een=0;ben=0;ken=0;
			for(i=domain[0][0];i<domain[0][1];i++){
				for(j=domain[1][0];j<domain[1][1];j++){
					for(k=domain[2][0];k<domain[2][1];k++){
						ex = Array_get(aryex,i,j,k);
						ey = Array_get(aryey,i,j,k);
						ez = Array_get(aryez,i,j,k);
						bx = Array_get(arybx,i,j,k);
						by = Array_get(aryby,i,j,k);
						bz = Array_get(arybz,i,j,k);
						ek = Array_get(aryek,i,j,k);
						een += ec*(ex*ex+ey*ey+ez*ez);
						ben += bc*(bx*bx+by*by+bz*bz);
						ken += ek;
					}
				}	
			}
			fprintf(fp,"%s\t%e\t%e\t%e\t",entry->d_name,een,ben,ken);
			for(list=specarylist;list;list=list->next){
				spec = list->content; 
				spec->energy = 0;
				for(i=domain[0][0];i<domain[0][1];i++){
					for(j=domain[1][0];j<domain[1][1];j++){
						for(k=domain[1][0];k<domain[1][1];k++){
							spec->energy += Array_get(spec->ary,i,j,k);}
					}	
				}
				fprintf(fp,"%e\t",spec->energy);
			}
			fprintf(fp,"\n");
			LinkedList_deleteRoot(arylist,Array_vdelete);
		}
	}
	fclose(fp);
	LinkedList_deleteRoot(speclist,Spec_vdelete);
	deallocate(dirin);
	deallocate(filein);
	return 0;
}
