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

void parseLineString(char *lineString, double o[2], double t[2]){
	if(lineString){
		for(;isalpha(*lineString) && *lineString;lineString++){}
		o[0] = atof(strtok(lineString,","));
		o[1] = atof(strtok(NULL,","));
		t[0] = atof(strtok(NULL,","));
		t[1] = atof(strtok(NULL,","));
	}
}

int setTSSize(int xsize,int ysize,double o[2],double t[2],double s[2],double e[2]){
	double ys,ye,xs,xe;
	ys = o[1] + (0 - o[0])*(t[1]/t[0]);
	ye = o[1] + (xsize - o[0])*(t[1]/t[0]);
	xs = o[0] + (0 - o[1])*(t[0]/t[1]);
	xe = o[0] + (ysize - o[1])*(t[0]/t[1]);
	
	if(ys < 0){
		s[0] = xs;
		s[1] = 0;
	}else if(ys > ysize){
		s[0] = xe;
		s[1] = ysize;
	}else{
		s[0] = 0;
		s[1] = ys;
	}
	
	if(ye < 0){
		e[0] = xs;
		e[1] = 0;
	}else if(ye > ysize){
		e[0] = xe;
		e[1] = ysize;
	}else{
		e[0] = xsize;
		e[1] = ye;
	}
	
	return (int)sqrt((e[0] - s[0])*(e[0] - s[0]) + (e[1] - s[1])*(e[1] - s[1]));
}

int resetSE(double rt[2],double o[2],double t[2],double s[2],double e[2]){
	double t0,t1,n0,n1,ta;
	ta = sqrt(t[0]*t[0] + t[1]*t[1]);
	t0 = ((s[0] - o[0])*t[0] + (s[1] - o[1])*t[1])/ta;
	t1 = ((e[0] - o[0])*t[0] + (e[1] - o[1])*t[1])/ta;
	
	if(t0 < rt[0]){
		s[0] = o[0] + rt[0]*t[0];
		s[1] = o[1] + rt[0]*t[1];
	}
	if(rt[1] < t1){
		e[0] = o[0] + rt[1]*t[0];
		e[1] = o[1] + rt[1]*t[1];
	}
	return (int)sqrt((e[0] - s[0])*(e[0] - s[0]) + (e[1] - s[1])*(e[1] - s[1]));
}
double *takeLineProfile(Data data, int len, double s[2], double e[2]){
	int i;
	double *array = allocate(len*sizeof(double));
	double x,y,length = sqrt((e[0] - s[0])*(e[0] - s[0]) + (e[1] - s[1])*(e[1] - s[1]));
	
	for(i=0;i<len;i++){
		x = s[0] + i*(e[0]-s[0])/length;
		y = s[1] + i*(e[1]-s[1])/length;
		if(x >= 0 && x < data->column && y >= 0 && y < data->row){
			array[i] = data->elem[(int)y][(int)x];
		}else{
			array[i] = 0;
		}
	}
	return array;
}
double *takeAveragedLineProfile(Data data, int len, double s[2], double e[2], double rn[2], double width){
	int i,j,k;
	int count=0;
	double *array = allocate(len*sizeof(double));
	double x,y,length = sqrt((e[0] - s[0])*(e[0] - s[0]) + (e[1] - s[1])*(e[1] - s[1]));
	double h,v,q[2],p[2] = {e[0]-s[0],e[1]-s[1]};
	double qq,pq;

	for(i=0;i<len;i++){
		x = s[0] + i*(e[0]-s[0])/length;
		y = s[1] + i*(e[1]-s[1])/length;
		array[i] = 0;
		count = 0;
		for(j=rn[0];j<=rn[1];j++){
			for(k=rn[0];k<=rn[1];k++){
				q[0] = x+j - s[0];
				q[1] = y+k - s[1];
				pq = p[0]*q[0] + p[1]*q[1];
				qq = q[0]*q[0] + q[1]*q[1];
				h = pq/length;
				v = (-q[0]*p[1] + q[1]*p[0])/length;
				if( fabs(h-i)<width && v >= rn[0] && v <= rn[1] ){
					if(0 <= x + j && x + j < data->column){
						if(0 <= y + k && y + k < data->row){
							//if(i%100<1){data->elem[(int)(y+k)][(int)(x+j)] = 1e+30;}
							array[i] += data->elem[(int)(y+k)][(int)(x+j)];
							count++;
						}
					}
				}
			}
		}
		//if(i%10==0){printf("array[%d] : %e, count : %d\n",i,array[i],count);}
		array[i] /= count;
	}
	//Data_output(data,"dbg.bmat",p_float);
	return array;
}

void drawRegion(Data data,double o[2],double t[2], double rt[2],double rn[2]){
	double n[2] = {-t[1], t[0]};
	double max = data->elem[0][0],tl,nl;
	double x,y;
	int i,j;
	for(i=0;i<data->row*data->column;i++){
		if(max < data->elem[0][i]){max = data->elem[0][i];}
	}
	for(i=0;i<data->row;i++){
		y = i;
		for(j=0;j<data->column;j++){
			x = j;
			tl = ((x-o[0])*t[0] + (y-o[1])*t[1])/(t[0]*t[0]+t[1]*t[1]);
			nl = ((x-o[0])*n[0] + (y-o[1])*n[1])/(n[0]*n[0]+n[1]*n[1]);
			
			if( ((rt[0] < tl) && (tl < rt[1])) && ((rn[0] < nl) && (nl < rn[1])) ){
				data->elem[i][j] = max;
			}
		}
	}
}

void drawLine(Data data,double s[2], double e[2]){
	int i,j;
	double x,y,Y;
	double max = data->elem[0][0];
	for(i=0;i<data->row*data->column;i++){
		if(max < data->elem[0][i]){max = data->elem[0][i];}
	}
	for(i=0;i<data->row;i++){
		y = i;
		for(j=0;j<data->column;j++){
			x = j;
			Y = (x-s[0])*(e[1]-s[1])/(e[0]-s[0]) + s[1];
			if(abs(y-Y)<=2){
				data->elem[i][j] = max;
			}
		}
	}
}

int main(int argc, char *argv[]){
	char *dirin,*filein=NULL,*fileout,*specname,*lineString=NULL,*dfile=NULL,*regionString;
	int row=-1,i,j,k,len,count,filecount,maxcount=-1,isLineSet=0,isRegionSet=0;
	double duration=0,width=0.5;
	double o[2],t[2],s[2],e[2];
	double rn[2],rt[2];
	LinkedList mainlist=NULL,list; 
	Data data,tsdata;
	DIR *dp;
	LineProfile lineProfile;
	int opt;
	struct dirent *entry;
	time_t start,end;
		//LeakDetector_set(stdout);
    while((opt=getopt(argc,argv,"r:l:n:d:a:w:"))!=-1){
        switch(opt){
            case 'r':
				row = atoi(optarg);
                break;
			case 'w':
				width = atof(optarg);
				break;
			case 'l':
				lineString = String_copy(optarg);
				isLineSet = 1;
				break;
			case 'd':
				dfile = String_copy(optarg);
				break;
			case 'n':
				maxcount = atoi(optarg);
				break;
			case 'a':
				regionString = String_copy(optarg);
				isRegionSet = 1;
				break;
			default:
                goto usage;
        }
    }

    if(optind >= argc){
        usage:
        printf(
               "usage : %s [input directory name] [species name] [output file name]\n"
			   "this program makes a time-spce-plot by taking a line profile at\n"
			   "middle row of each time slices and stack them vertically  a time-spce-plot\n"
			   "options :\n"
			   "-r [row index]: set row index\n"
			   "-l [\"ox,oy,tx,ty\"] : set a line used to make a line profile\n"
			   "	(ox,oy) is origin point of the line\n"
			   "	(tx,ty) is tangent vector of the line\n"
			   "	line equation : tx*(y - oy) - ty*(x - ox) = 0\n"
			   "-d [file name]: output bmat file for checking the line\n"
			   "-n [maximum file number]: set maximum number of data files\n"
			   "-a [\"ts,te,ns,ne\"] : set averaging region\n"
			   "	(ts,te) is start and end points of the region in a direction tangent to the main line\n"
			   "	(ns,ne) is start and end points of the region in a direction normal to the main line\n"
			   "	To specify these points, use the distance from the origin point of the main line\n"
			   ,
			   argv[0]
        );
        exit(0);
    }
    if(argc >= optind+1){
        dirin = String_copy(argv[optind]);
    }
    if(argc >= optind+2){
		specname = String_copy(argv[optind+1]);
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
	if(maxcount > 0){
		for(i=0,list=mainlist;i<maxcount-1 && list->next;i++,list=list->next){}
		LinkedList_deleteRoot(list->next,LineProfile_vdelete);
		list->next = NULL;
		filecount = maxcount;
	}
	if(isLineSet){
		printf("-l %s\n",lineString);
		parseLineString(lineString,o,t);
		printf("(o[0],o[1]) : (%e,%e)\n",o[0],o[1]);
		printf("(t[0],t[1]) : (%e,%e)\n",t[0],t[1]);
	}
	if(isRegionSet){
		printf("-a %s\n",regionString);
		parseLineString(regionString,rt,rn);
		printf("(rt[0],rt[1]) : (%e,%e)\n",rt[0],rt[1]);
		printf("(rn[0],rn[1]) : (%e,%e)\n",rn[0],rn[1]);
	}
    lineProfile = mainlist->content;
	data = Data_loadSDF(lineProfile->fileName,specname);
	if(data == NULL){
		printf("Output \"%s\" is missing in %s\n",specname,filein);
        exit(1);
    }
	
	if(isLineSet){
		len = setTSSize(data->column,data->row,o,t,s,e);
		if(isRegionSet){
			len = resetSE(rt,o,t,s,e);
		}
	}else{
		len = data->column;
		if(row < 0){
			row = (data->row-1) >> 1;
		}else if(row > data->row){
			printf("input row is larger than data row\n");
			exit(1);
		}
		s[0] = 0;
		s[1] = row;
		t[0] = data->column;
		t[1] = row;
	}
	
	tsdata = Data_create(filecount,len);
	if(dfile){
		 drawLine(data,s,e);
		 if(isRegionSet){drawRegion(data,o,t,rt,rn);}
		 Data_output(data,dfile,p_float);
	}
	Data_delete(data);
	time(&start);
	for(list=mainlist,count=0;list;list=list->next,count++){
		if(count % 10 == 0){
			time(&end);
			duration = (end - start)*0.1;
			time(&start);
		}
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
		printf(" ETA %.1f min\n",(double)(filecount-count)*duration/60);
        //start = clock();
		data = Data_loadSDF(lineProfile->fileName,specname);
        //end = clock();
		if(isLineSet){
			if(isRegionSet){
				lineProfile->array = takeAveragedLineProfile(data,len,s,e,rn,width);
			}else{
				lineProfile->array = takeLineProfile(data,len,s,e);
			}
		}else{
			lineProfile->array = allocate(data->column*sizeof(double));
			for(i=0;i<data->column;i++){
				lineProfile->array[i] = data->elem[row][i];
			}
		}
		Data_delete(data);
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
    deallocate(lineString);
	deallocate(dirin);
    deallocate(fileout);
    return 0;
}
