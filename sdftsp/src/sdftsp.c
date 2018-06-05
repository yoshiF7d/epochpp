#include "utilities.h"
#include "Data.h"
#include "LinkedList.h"
#include "Data_loadSDF.h"
#include <unistd.h>
#include <dirent.h> 

typedef struct st_lineProfile{
	char *fileName;
	int len;
	double *array;
}LineProfile_Sub;
typedef LineProfile_Sub *LineProfile;

LineProfile LineProfile_create(char *fileName){
	LineProfile lp = allocate(sizeof(LineProfile_Sub));
	lp->fileName = fileName;
	lp->len = 0;
	lp->array = NULL;
	return lp;
}

int LineProfile_compare(LineProfile lp1,LineProfile lp2){
	return strcmp(lp1->fileName,lp2->fileName);
}

int LineProfile_vcompare(void *v1, void *v2){
	return LineProfile_compare(v1,v2);
}

void LineProfile_delete(LineProfile lp){
	deallocate(lp->fileName);
	deallocate(lp->array);
	deallocate(lp);
}

void LineProfile_vdelete(void *lp){
	return LineProfile_delete(lp);
}

int main(int argc, char *argv[]){
	char *dirin,*filein=NULL,*fileout,*specname;
	int row=-1,i,j,k,s,len,count,filecount;
	double time;
	LinkedList mainlist=NULL,list; 
	Data data,tsdata;
	DIR *dp;
	LineProfile lineProfile;
	int opt;
	struct dirent *entry;
	clock_t start,end;
	//LeakDetector_set(stdout);
    while((opt=getopt(argc,argv,"r:"))!=-1){
        switch(opt){
            case 'r':
				row = atoi(optarg);
                break;
            default:
                goto usage;
        }
    }

    if(optind >= argc){
        usage:
        printf(
               "usage : %s [input directory name] [species name] [output file name] (-r row number)\n"
			   "this program makes a time-spce-plot by taking a line profile at\n"
			   "middle row of each time slices and stack them vertically  a time-spce-plot\n"
			   "-r : set row number\n",
			   argv[0]
        );
        exit(0);	
    }
    if(argc >= optind+1){
        dirin = String_copy(argv[optind]);
    }else{
		goto usage;
	}
    if(argc >= optind+2){
		specname = String_copy(argv[optind+1]);
	}else{
		goto usage;
	}
    if(argc >= optind+3){
        fileout = String_copy(argv[optind+2]);
    }else{
		fileout = String_join(dirin,"/tsp.bmat");
	}
    filecount=0;
    if((dp=opendir(dirin))==NULL){fprintf(stderr,"%s opendir error",argv[0]); return -1;}
    while((entry=readdir(dp))!=NULL){
        if(entry->d_name[0]=='.'){continue;}
        if(strcmp(getFileExtension(entry->d_name),".sdf")==0){
            filecount++;
            len = strlen(dirin) + strlen(entry->d_name)+1;
            filein = allocate(len+1);
            snprintf(filein,len+1,"%s/%s",dirin,entry->d_name);
            mainlist = LinkedList_append(mainlist,LineProfile_create(filein));
        }
    }
    closedir(dp);
	mainlist=LinkedList_sort(mainlist,LineProfile_vcompare);
    lineProfile = mainlist->content;
	data = Data_loadSDF(lineProfile->fileName,specname);
	if(data == NULL){
		printf("Output \"%s\" is missing in %s\n",specname,filein);
        exit(1);
    }
	if(row < 0){
		row = (data->row-1) >> 1;
	}else if(row > data->row){
		printf("input row is larger than data row\n");
		exit(1);
	}
    
	tsdata = Data_create(filecount,data->column);
	Data_delete(data);
	
	for(list=mainlist,count=0;list;list=list->next,count++){
        start = clock();
        lineProfile = list->content;
        printf("processing %s (%d/%d)\n",lineProfile->fileName,count,filecount);
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
		data = Data_loadSDF(lineProfile->fileName,specname);
        //end = clock();
        lineProfile->array = allocate(data->column*sizeof(double));
		for(i=0;i<data->column;i++){
			lineProfile->array[i] = data->elem[row][i];
		}
		Data_delete(data);
        //end = clock();
        end = clock();
        time = (double)(end-start)/CLOCKS_PER_SEC;
        printf("total : %.2f sec\n",time);
		printf("\033[F\033[J");
        printf("\033[F\033[J");
        printf("\033[F\033[J");
    }
	
	for(list=mainlist,count=0;list;list=list->next,count++){
		lineProfile = list->content;
		memcpy(tsdata->elem[count],lineProfile->array,(tsdata->column)*sizeof(double));
	}
	Data_output(tsdata,fileout,p_float);
	Data_delete(tsdata);
    LinkedList_deleteRoot(mainlist,LineProfile_vdelete);
    deallocate(dirin);
    deallocate(fileout);
    return 0;
}
