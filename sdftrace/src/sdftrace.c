#include "utilities.h"
#include "Data.h"
#include <dirent.h>

typedef struct st_list{
	Data data;
	Data id;
	int index;
	struct st_list *next;
}List_Sub;
typedef List_Sub *List;

List List_append(List list, Data data, Data id, int index){
	List s;
	if(!list){
		list = allocate(sizeof(List_Sub));
		list->data = data;
		list->id = id;
		list->index = index;
		list->next = NULL;
	}else{
		for(s=list;s->next;s=s->next){}
		s->next = allocate(sizeof(List_Sub));
		s->next->data = data;
		s->next->id = id;
		s->next->index = index;
		s->next->next = NULL;
	}	
	return list;
}

void List_fprint(List list, FILE *fp, int frac){
	List s;
	Data data;
	int i,j,k,r,c;
	if(!fp){return;}
	r = Data_getRow(list->data);
	c = Data_getColumn(list->data);
	for(s=list;s;s=s->next){
		data = Data_create(r,c+1);
		for(i=0;i<r;i++){
			Data_translate(s->id,i,0,data,i,0);
			for(j=0;j<c;j++){Data_translate(s->data,i,j,data,i,j+1);}
		}	
		Data_delete(s->id);
		Data_delete(s->data);
		s->id = NULL;
		s->data = data;
		Data_sort(s->data,0);
	}
	for(i=0;i<r;i++){
		if(i%frac!=0){continue;}
		for(s=list;s;s=s->next){
			//fprintf(stderr,KCYN "%d" KNRM,s->index);
			fprintf(fp,"%d ",s->index);
			for(k=0;k<c;k++){
				fprintf(fp,"%e ",Data_get(s->data,i,k+1));
			}
			fprintf(fp,"\n");
		}
		fprintf(fp,"\n");
	}
}

void List_fprintm(List list,FILE *fp){
	List s;
	int i,j,k,r,c;
	if(!fp){return;}
	r = Data_getRow(list->data);
	c = Data_getColumn(list->data);
	fprintf(fp,"{");
	for(i=0;i<r;i++){
		fprintf(fp,"{");
		for(s=list,j=0;s;s=s->next,j++){
			fprintf(fp,"{%d",j);
			for(k=0;k<c;k++){
				fprintf(fp,",%e",Data_get(s->data,i,k));
			}
			if(s->next){fprintf(fp,"},\n");}
			else{fprintf(fp,"}\n");}	
		}
		if(i<r-1){fprintf(fp,"},\n");}
		else{fprintf(fp,"}\n");}
	}
	fprintf(fp,"}\n");
}

void List_sort_split(List source, List *front, List *back){
	List fast, slow;
	if(source==NULL||source->next==NULL){
		*front=source;
		*back=NULL;
	}else{
		slow=source;
		fast=source->next;
		while(fast!=NULL){
			fast=fast->next;
			if(fast!=NULL){
				slow=slow->next;
				fast=fast->next;
			}
		}
		*front=source;
		*back=slow->next;
		slow->next=NULL;
	}
}

List List_sort_merge(List a, List b){
	List result=NULL;
	if(a==NULL){return b;}
	else if(b==NULL){return a;}

	if(a->index > b->index){
		result=b; result->next=List_sort_merge(a,b->next);
	}else{
		result=a; result->next=List_sort_merge(a->next,b);
	}
	return result;
}

List List_sort(List root){
	List list=root;
	List a,b;
	if((list==NULL)||(list->next==NULL)){return list;}
	List_sort_split(root,&a,&b);
	a=List_sort(a);
	b=List_sort(b);
	return List_sort_merge(a,b);
}

void List_delete(List list){
	List s,next;
	for(s=list;s;s=next){
		Data_delete(s->id);
		Data_delete(s->data);
		next = s->next;
		deallocate(s);
	}
}

int getindex(char *filename){
	int i,res=0,exp=1;
	//fprintf(stderr,"%s\n",filename);
	//for(i=0;filename[i];i++){}
	//for(i--;i>=0;i--,exp*=10){
	//	res += exp*(filename[i]-0x30);
	//}
	res = atoi(filename);
	//fprintf(stderr,KYEL "%d\n" KNRM,res);
	return res;
}

int main(int argc, char *argv[]){
	char dirsdf[256],command[256],specname[256],specname2[256],filename[256],filegrid[256],filesdf[256];
	char dirid[256],dirgrid[256],fileout[256],fileid[256];
	FILE *fp = stdout;
	DIR *dp; 
	struct dirent *entry;
	List list=NULL, s;
	int i, mini=0,maxi=-1,frac=1;
	
	if(argc >= 3){
		strncpy(dirsdf,argv[1],256);
		strncpy(specname,argv[2],256);
	}else{
		fprintf(stderr,"usage : %s sdfdir specname [outputfile] [every] [min] [max]\n",argv[0]);
	}
	
	if(argc >= 5){frac = atoi(argv[4]);}
	if(argc >= 6){mini = atoi(argv[5]);}
	if(argc >= 7){maxi = atoi(argv[6]);}
	
	for(i=0;;i++){
		if(specname[i]=='/'){
			specname2[i] = '_';
		}else{
			specname2[i] = specname[i];
		}
		if(specname[i]=='\0'){break;}	
	}
	
	snprintf(command,256,"%s/phase",dirsdf);
	mkdir(command,0777);
	fprintf(stderr,"%s\n",command);
	snprintf(command,256,"%s/phase/%s",dirsdf,specname2);
	mkdir(command,0777);
	fprintf(stderr,"%s\n",command);
	snprintf(dirgrid,256,"%s/phase/%s/grid",dirsdf,specname2);
	mkdir(dirgrid,0777);
	fprintf(stderr,"%s\n",dirgrid);
	snprintf(dirid,256,"%s/phase/%s/id",dirsdf,specname2);
	mkdir(dirid,0777);
	fprintf(stderr,"%s\n",dirid);

	if((dp=opendir(dirsdf))==NULL){fprintf(stderr,"%s",argv[0]); return -1;}
	while((entry=readdir(dp))!=NULL){
		if(entry->d_name[0]=='.'){continue;}
		if(strcmp(getFileExtension(entry->d_name),".sdf")==0){
			snprintf(filename,strlen(entry->d_name)-3,"%s",entry->d_name);
			snprintf(filegrid,256,"%s/%s.bmat",dirgrid,filename);
			snprintf(fileid,256,"%s/%s.bmat",dirid,filename);
			i=getindex(filename);
			if((maxi > 0 && i > maxi) || i < mini){continue;}
			fprintf(stderr,"%d\n",i);
			if(access(filegrid,F_OK)==-1){
				snprintf(command,256,"sdf2bmat -cni -v grid/%s -f %s %s/%s",specname,filegrid,dirsdf,entry->d_name);
				fprintf(stderr,"%s\n",command);
				system(command);
			}
			if(access(fileid,F_OK)==-1){
				snprintf(command,256,"sdf2bmat -cni -v id/%s -f %s %s/%s",specname,fileid,dirsdf,entry->d_name);
				fprintf(stderr,"%s\n",command);
				system(command);
			}
			list = List_append(list,Data_input(filegrid),Data_input(fileid),i);
		}
	}
	closedir(dp);
	
	if(argc >= 4){
		strncpy(fileout,argv[3],256);
		if((fp = fopen(fileout,"w"))==NULL){perror("fileout");}
	}
	List_sort(list);		
	//List_fprintm(list,fp);
	List_fprint(list,fp,frac);
	//fprintf(stderr,"end1\n");
	if(argc >= 4){fclose(fp);}
	//fprintf(stderr,"end2\n");
	List_delete(list);
	//fprintf(stderr,"end3\n");
	return 0;
}
