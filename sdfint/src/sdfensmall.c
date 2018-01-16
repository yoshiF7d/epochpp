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
	char *ekname;
	char *ndname;
	Array ekary;
	Array ndary;
}Spec_Sub;
typedef struct st_spec *Spec;

Spec Spec_create(char *name){
	Spec spec = allocate(sizeof(Spec_Sub));
	spec->name = name;
	spec->ekname = String_join("ekbar/",name);
 	spec->ndname = String_join("number_density/",name);
	spec->ekary = NULL;
	spec->ndary = NULL;
	return spec;
}

void Spec_delete(Spec spec){
	deallocate(spec->ekname);
	deallocate(spec->ndname);
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

void setbox(double box[],char *string){
	char *buf,*p,*ps;
	int read=0,index=0;
	buf = String_copy(string);
	for(ps=p=string;*p;p++){
		switch(*p){
			case ',':
				*p='\0';
				if(index>3){printf("box size specification %s have more than 3 inputs\n",string); return;}
				box[index]=atof(ps);
				read=0; index++;
			case ')':
				*p='\0';
				break;
		}
		if(!read){
			if(isdigit(*p)){ps=p;read=1;}
		}
	}
	if(index>3){printf("box size specification %s have more than 3 inputs\n",string); return;}
	box[index]=atof(ps);
	deallocate(buf);
}

typedef struct st_energy{
	char *filename;
	double een;
	double ben;
	double ken;
	double *kenlist;
	int klen;
}Energy_Sub;

typedef Energy_Sub *Energy;

Energy Energy_create(char *filename){
    Energy energy = allocate(sizeof(Energy_Sub));
    energy->filename = filename;
    return energy;
}
/*
Energy Energy_create(char *filename,double een,double ben,double ken,int klen,double *kenlist){
	Energy energy = allocate(sizeof(Energy_Sub));
	energy->filename = filename;
	energy->een = een;
	energy->ben = ben;
	energy->ken = ken;
	energy->klen = klen;
	energy->kenlist = kenlist;
	return energy;
}
*/
void Energy_delete(Energy energy){
	deallocate(energy->filename);
	deallocate(energy->kenlist);
	deallocate(energy);
}

void Energy_vdelete(void *energy){
	Energy_delete(energy);
}

void Energy_fprint(Energy energy, FILE *fp){
	int i;
	fprintf(fp,"%s\t%e\t%e\t%e\t",getFileName(energy->filename),energy->een,energy->ben,energy->ken);
	for(i=0;i<energy->klen;i++){
		fprintf(fp,"%e\t",energy->kenlist[i]);
	}
	fprintf(fp,"\n");
}

int Energy_compare(Energy e1, Energy e2){
	return strcmp(e1->filename,e2->filename);
}

int Energy_vcompare(void *e1, void *e2){
	return Energy_compare(e1,e2);
}

int main(int argc, char *argv[]){
	char *dirin,*filein=NULL,*fileout;
	
	int i,j,k,s,len,speclen=0,domsetflag=0,namelistlen,initialflag=0,count,filecount;
	double box[3]={1,1,1};
	int domain[3][2]={{0,1},{0,1},{0,1}};
    double time;
	double ec,bc,bx,by,bz,ex,ey,ez,ek,nd;
	double een=0,ben=0,ken=0,area=1;	
	double *kenlist;
	LinkedList list0=NULL,list=NULL,list2=NULL,namelist=NULL,arylist=NULL,speclist=NULL,mainlist=NULL;
	Array aryex,aryey,aryez,arybx,aryby,arybz,aryek,arynd;
	Spec spec;
    Energy energy;
	FILE *fp=stdout;;
	DIR *dp;
	int opt;
	struct dirent *entry;
    clock_t start,end,start2,end2;
    
	//LeakDetector_set(stdout);

    while((opt=getopt(argc,argv,"d:s:"))!=-1){
        switch(opt){
            case 'd':
                setdomain(domain,optarg);
                domsetflag=1;
                break;
            case 's':
                speclist = LinkedList_append(speclist,Spec_create(optarg));
                speclen = LinkedList_getLength(speclist);
                break;
            default:
                goto usage;
                
        }
    }

    if(optind >= argc){
        usage:
        printf(
               "usage : %s [inputdir name] [box size] ([outputfile name]) (-d [subdomain specification]) (-s [species name])\n"
               "box size : \"xlen,ylen,(zlen)\" length of the simulation box in meter"
               "subdomain specification : \"irs,ire,ics,ice,(ihs,ihe)\". irs,ire,ics,ice is start,end index of row,column of subdomain\n"
               "ihs,ihe is needed in epoch3d data.\n"
               "species name : when there is ekbar/[species] in sdf file\n" 
               "use this command line option : example : \"-s electron -s proton\"\n",
               argv[0]
        );
        exit(0);	
    }
    if(argc >= optind+1){
        dirin = String_copy(argv[optind]);
    }
    if(argc >= optind+2){
        setbox(box,argv[optind+1]);
    }
    if(argc >= optind+3){
        fileout = String_copy(argv[optind+2]);		
        fp = fopen(fileout,"w");
    }

    area = box[0]*box[1]*box[2];	
    namelist = LinkedList_append(namelist,"ex");
    namelist = LinkedList_append(namelist,"ey");
    namelist = LinkedList_append(namelist,"ez");
    namelist = LinkedList_append(namelist,"bx");
    namelist = LinkedList_append(namelist,"by");
    namelist = LinkedList_append(namelist,"bz");
    namelist = LinkedList_append(namelist,"ekbar");
    namelist = LinkedList_append(namelist,"number_density");
    
    for(list=speclist;list;list=list->next){
        spec = list->content;
        LinkedList_append(namelist,spec->ekname);
        LinkedList_append(namelist,spec->ndname);
    }
    namelistlen = LinkedList_getLength(namelist);
    ec = 0.5*C_e0; bc = 0.5/C_u0;
    printf("box size : (%g,%g,%g)\n",box[0],box[1],box[2]);
    filecount=0;
    if((dp=opendir(dirin))==NULL){fprintf(stderr,"%s opendir error",argv[0]); return -1;}
    while((entry=readdir(dp))!=NULL){
        if(entry->d_name[0]=='.'){continue;}
        if(strcmp(getFileExtension(entry->d_name),".sdf")==0){
            filecount++;
            len = strlen(dirin) + strlen(entry->d_name)+1;
            filein = allocate(len+1);
            snprintf(filein,len+1,"%s/%s",dirin,entry->d_name);
            mainlist = LinkedList_append(mainlist,Energy_create(filein));
            
        }
    }
    closedir(dp);
    mainlist=LinkedList_sort(mainlist,Energy_vcompare);
    /*
    for(list=mainlist;list;list=list->next){
        energy = list->content;
        printf("%s\n",energy->filename);
    }
    */
    energy = mainlist->content;
    start2=clock();
    arylist = Array_loadSDFList2(energy->filename,namelist);
    end2=clock();
    time = (double)(end2-start2)/CLOCKS_PER_SEC;
    aryex = arylist->content;
    for(i=0,list=arylist;list;i++,list=list->next){
        if(list->content == NULL){
            printf("Output \"%s\" is missing in %s\n",LinkedList_getIndex(namelist,i),filein);
            exit(1);
        }
    }
    if(!domsetflag){
        for(i=0;i<aryex->depth;i++){
            domain[i][1]=aryex->dim[i];
        }
        for(i=0;i<6;i++){
            printf("domain[%d][%d] : %d\n",i/2,i%2,domain[i/2][i%2]);
        }
    }
    LinkedList_deleteRoot(arylist,Array_vdelete);
    
    fprintf(fp,"filename\telectric energy [J]\tmagnetic energy [J]\tkinetic energy [J]\t");
    for(list=speclist;list;list=list->next){
        spec=list->content;fprintf(fp,"kinetic energy (%s) [J]\t",spec->name);
    }
    fprintf(fp,"\n");
    for(list0=mainlist,count=0;list0;list0=list0->next,count++){
        start2 = clock();
        energy = list0->content;
        printf("processing %s (%d/%d)\n",energy->filename,count,filecount);
        printf("[");
        for(i=0;i<100;i++){
            if(i<100*count/filecount){printf("=");}
            else if(i==100*count/filecount){printf("🐌");}
            else{printf(" ");}
            
        }
        printf("]");
        printf(" %.1f %%",100*(double)count/filecount);
        printf(" ETA %d min\n",(int)((filecount-count)*time/60));
        //start = clock();
        arylist = Array_loadSDFList2(energy->filename,namelist);
        //end = clock();
        //printf("loading : %.2f sec\n",(double)(end-start)/CLOCKS_PER_SEC);
        list = arylist;
        aryex=list->content; list=list->next;
        aryey=list->content; list=list->next;
        aryez=list->content; list=list->next;
        arybx=list->content; list=list->next;
        aryby=list->content; list=list->next;
        arybz=list->content; list=list->next;
        aryek=list->content; list=list->next;
        arynd=list->content; list=list->next;
        
        for(list2=speclist;list2;list2=list2->next,list=list->next){
            spec=list2->content;
            spec->ekary = list->content;
            list=list->next;
            spec->ndary = list->content;
        }
        start = clock();
        een=0;ben=0;ken=0;
        if(!domsetflag){
            for(i=0;i<domain[0][1]*domain[1][1]*domain[2][1];i++){
                ex = aryex->elem[i];
                ey = aryey->elem[i];
                ez = aryez->elem[i];
                bx = arybx->elem[i];
                by = aryby->elem[i];
                bz = arybz->elem[i];
                ek = aryek->elem[i];
                nd = arynd->elem[i];
                een += ec*(ex*ex+ey*ey+ez*ez);
                ben += bc*(bx*bx+by*by+bz*bz);
                ken += ek*nd;
            }
        }else{
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
                        nd = Array_get(arynd,i,j,k);
                        een += ec*(ex*ex+ey*ey+ez*ez);
                        ben += bc*(bx*bx+by*by+bz*bz);
                        ken += ek*nd;
                    }
                }
            }
        }
        //end = clock();
        //printf("heavy loop : %.2f sec\n",(double)(end-start)/CLOCKS_PER_SEC);
        kenlist = speclen > 0 ? allocate(speclen*sizeof(double)) : NULL;
        for(list=speclist,s=0;list;list=list->next,s++){
            spec = list->content; 
            kenlist[s] = 0;
            for(i=domain[0][0];i<domain[0][1];i++){
                for(j=domain[1][0];j<domain[1][1];j++){
                    for(k=domain[2][0];k<domain[2][1];k++){
                        kenlist[s] += Array_get(spec->ekary,i,j,k)*Array_get(spec->ndary,i,j,k);}
                }	
            }
            kenlist[s] *= area;
        }
        een *= area;
        ben *= area;
        ken *= area;
        energy->een = een;
        energy->ben = ben;
        energy->ken = ken;
        energy->klen = speclen;
        energy->kenlist = kenlist;
        LinkedList_deleteRoot(arylist,Array_vdelete);
        end2 = clock();
        time = (double)(end2-start2)/CLOCKS_PER_SEC;
        printf("total : %.2f sec\n",time);
	printf("\033[F\033[J");
        printf("\033[F\033[J");
        printf("\033[F\033[J");
        Energy_fprint(energy,fp);
	//printf("\033[F\033[J");
        //printf("\033[F\033[J");
    }
    LinkedList_deleteRoot(namelist,NULL);
    LinkedList_deleteRoot(mainlist,Energy_vdelete);
    LinkedList_deleteRoot(speclist,Spec_vdelete);
    deallocate(dirin);
    deallocate(fileout);
    //LeakDetector_finish();	
    //printf("len : %d\n",len);
    //printf("speclen : %d\n",speclen);
    fclose(fp);
    return 0;
}
