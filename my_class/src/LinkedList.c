#include "LinkedList.h"

LinkedList LinkedList_create(void *content){
	LinkedList list = allocate(sizeof(LinkedList_Sub));
	list->content = content;
	list->next = NULL;
	return list;
}

void LinkedList_delete(LinkedList list, void (*delete)(void *content)){
	if(delete && list->content){delete(list->content);}
	deallocate(list);
}

LinkedList LinkedList_removeIndex(LinkedList root, void (*delete)(void *content),int k){
	int c,n,i;
	LinkedList pre=root,rtn = root;
	if(!root){return rtn;}
	if(k > abs(n = LinkedList_getLength(root))){
		fprintf(stderr,"LinkedList_delete : %d th element does not exist.\n",k);
	}else if(k==0){
		LinkedList_deleteRoot(root,delete);
	}else if(k < 0){
		c = n - k;
	}else{
		c = k;
	}
	for(i=0;i<c;i++){pre = root; root = root->next;}
	if(pre==rtn){rtn = pre->next;}
	else{rtn = pre;}
	pre->next = root->next;
	if(root->content && delete){
		delete(root->content);
	}
	deallocate(root);
	return rtn;
}

LinkedList LinkedList_remove(LinkedList root, LinkedList list, void (*delete)(void *content)){
	LinkedList p,prev,rtn = root;
	if(root==list){rtn = root->next;}
	for(prev=p=root; p!=list && p; p=p->next){
		prev = p;
	}
	if(!p){return root;}
	prev->next = p->next;
	LinkedList_delete(p,delete);
	return rtn;
}

LinkedList LinkedList_removeSubList(LinkedList root, LinkedList list, void (*delete)(void *content)){
	LinkedList p,p2,prev,rtn;
	p2 = list;
	rtn = root;
	for(p=prev=root; p; p=p->next){
		if(p==p2){
			if(prev == p){
				rtn = prev = p->next;
			}else{
				prev->next = p->next;
			}
			LinkedList_delete(p,delete);
			p2 = p2->next;
		}else{
			prev = p;
		}
	}
	return rtn;
}

LinkedList LinkedList_removeEquals(LinkedList list, void *content, int (*equals)(void*, void*), void (*delete)(void*)){
	LinkedList p,pre;
	for(pre=p=list; p; p=p->next){
	  loophead:
		if(equals(content, p->content)){
			if(pre==p){
				list = p->next;
				LinkedList_delete(p,delete);
				pre = p = list;
				goto loophead;
			}else{
				p->next = pre->next;
				LinkedList_delete(p,delete);
				p = pre->next;
				goto loophead;
			}
		}
		pre = p;
	}
	return list;
}

LinkedList LinkedList_removeDuplicateEquals(LinkedList list, int (*equals)(void*, void*), void (*delete)(void*)){
	LinkedList p,q,qpre;
	for(p=list; p; p=p->next){
		for(qpre=p,q=p->next; q; q=q->next){
			if(equals(p->content,q->content)){
				qpre->next=q->next;
				LinkedList_delete(q,delete);
				break;
			}
		qpre=q;
		}
	}
	return list;
}

void LinkedList_deleteRoot(LinkedList root, void (*delete)(void *content)){
	if(!root){return;}
	if(root->next){LinkedList_deleteRoot(root->next,delete);}
	if(root->content && delete){delete(root->content); root->content = NULL;}
	deallocate(root);
}

void LinkedList_unwrapRoot(LinkedList root){
	if(root->next) LinkedList_unwrapRoot(root->next);
	deallocate(root);
}

LinkedList LinkedList_skip(LinkedList list, int k){
	int i;
	for(i=0;i<k && list;list=list->next,i++){}
	return list;
}

LinkedList LinkedList_increment(LinkedList list){
	return list = list->next;
}

LinkedList LinkedList_decrement(LinkedList list){
	fprintf(stderr,"LinkedList_decrement is not provided in \"LinkedList.h\"\n");
	return NULL;
}

LinkedList LinkedList_append(LinkedList root, void *content){
	LinkedList list = root;
	if(!list){return LinkedList_create(content);}
	while(list->next){list = list->next;}
	list->next = LinkedList_create(content);
	return root;
}

LinkedList LinkedList_prepend(LinkedList root, void *content){
	LinkedList list = LinkedList_create(content);
	if(!root){return list;}
	list->next = root;
	return list;
}

LinkedList LinkedList_concatenate(LinkedList root, LinkedList root2){
	if(root){LinkedList_tail(root)->next = root2;}
	else{root = root2;}
	return root;
}

LinkedList LinkedList_duplicate(LinkedList root, void *(*duplicate)(void*)){
	LinkedList p,newlist=NULL;
	if(!root){return NULL;}
	
	for(p=root;p;p=p->next){
		newlist=LinkedList_append(newlist,duplicate(p->content));
	}
	return newlist;
}

void *LinkedList_duplicate_func_mod(void *content, size_t size){
	void *p=NULL;
	static size_t sizeimp;
	
	if(!content && size>0){sizeimp = size;}
	else if(content && !size){
		if(sizeimp==0){printf("LinkedList_duplicate_func : size is not set yet\n"); return NULL;}
		p = allocate(sizeimp);
		memcpy(p,content,sizeimp);
	}
	return  p;
}
void *LinkedList_duplicate_func(void *content){return LinkedList_duplicate_func_mod(content,0);}
void LinkedList_duplicate_func_setSize(size_t size){LinkedList_duplicate_func_mod(NULL,size);}


LinkedList LinkedList_insertListBehind(LinkedList root, LinkedList current, LinkedList list){
	if(!root){return NULL;}
	if(!current){return LinkedList_concatenate(list,root);}
	list = LinkedList_concatenate(list,current->next);
	current->next=list;	
	return root;
}

LinkedList LinkedList_find(LinkedList list, void *content){
	LinkedList s=list;
	while(s){
		if(s->content == content){break;}
		s = s->next;
	}
	return s;
}
LinkedList LinkedList_select(LinkedList list, int (*crit)(void*)){
	LinkedList s;
	for(s=list;s;s=s->next){
		if(crit(s->content)){return s;}
	}
	return NULL;
}

LinkedList LinkedList_findEquals(LinkedList list, void *content, int (*equals)(void*, void*)){
	LinkedList s=list;
	while(s){
		if(equals(s->content,content)){break;}
		s = s->next;
	}
	return s;
}

int LinkedList_locate(LinkedList list, LinkedList list2){
	LinkedList s=list;
	int i=0;
	while(s){
		if(s == list2){return i;}
		s = s->next;
		i++;
	}
	return -1;
}

int LinkedList_locateContent(LinkedList list, void *content){
	LinkedList s=list;
	int i=0;
	while(s){
		if(s->content == content){return i;}
		s = s->next;
		i++;
	}
	return -1;
}

int LinkedList_isExist(LinkedList list, void *content){
	LinkedList s=list;
	while(s){
		if(s->content == content){return 1;}
		s = s->next;
	}
	return 0;
}

LinkedList LinkedList_search(LinkedList list, int (*equals)(void*,void*), void *content){
	static LinkedList ll_pre;
	LinkedList ll_cur = NULL;
	LinkedList ll_rtn = NULL;
	
	if(!list){
		if(!ll_pre){return NULL;}
		ll_rtn = ll_cur  = ll_pre;
	}
	else{ll_rtn = ll_cur = list;}
	
	while(!equals(ll_cur->content,content)){
		ll_cur = LinkedList_increment(ll_cur);
		if(!ll_cur->next){return ll_rtn;}
	}
	
	ll_rtn = ll_pre = ll_cur->next;
	ll_cur->next = NULL;
	
	return ll_rtn;
}

LinkedList LinkedList_setIndex(LinkedList root, int k, void *content){
	int i;
	LinkedList list = root;
	
	for(i=0;i<k;i++){
		list = list->next;
		if(list == NULL) {return NULL;}
	}
	list->content = content;
	return root;
}

void LinkedList_set(LinkedList list,void *content){
	if(list == NULL) {return;}
	list->content = content;
}

void LinkedList_setNext(LinkedList list, LinkedList next){list->next = next;}

void LinkedList_swapContent(LinkedList n1, LinkedList n2){
	void *temp;
	temp = n1->content;
	n1->content = n2->content;
	n2->content = temp;
}

/*http://www.geeksforgeeks.org/merge-sort-for-linked-list/*/
LinkedList LinkedList_sort_merge(LinkedList a, LinkedList b, int (*compare)(void *c1, void *c2));
void LinkedList_sort_split(LinkedList source, LinkedList *front, LinkedList *back);

LinkedList LinkedList_sort(LinkedList root, int compare(void *c1, void *c2)){
	/*marge sort*/
	LinkedList list=root;
	LinkedList a,b;
	if((list==NULL) || (list->next==NULL)){return list;}
	LinkedList_sort_split(list,&a,&b);
	a = LinkedList_sort(a,compare);
	b = LinkedList_sort(b,compare);
	
	return LinkedList_sort_merge(a,b,compare);
}

LinkedList LinkedList_sort_merge(LinkedList a, LinkedList b, int (*compare)(void *c1, void *c2)){
	LinkedList result = NULL;
	if(a == NULL){return b;}
	else if(b == NULL){return a;}
	
	if(compare(LinkedList_get(a),LinkedList_get(b))>0){
		result = b; result->next = LinkedList_sort_merge(a,b->next,compare);
	}else{
		result = a; result->next = LinkedList_sort_merge(a->next,b,compare);
	}
	return result;
}

void LinkedList_sort_split(LinkedList source, LinkedList *front, LinkedList *back){
	LinkedList fast,slow;
	if(source == NULL || source->next == NULL){
		*front = source; 
		*back = NULL;
	}else{
		slow = source;
		fast = source-> next;
		while(fast != NULL){
			fast = fast->next;
			if(fast != NULL){
				slow = slow->next;
				fast = fast->next;
			}
		}
		*front = source;
		*back = slow->next;
		slow->next = NULL;
	}
}

int LinkedList_count(LinkedList list,int (* equals)(void*,void*),void *content){
	int count=0;
	while(list){
		if(equals(list->content,content)){count++;}
		list = list->next;
	}
	//printf("%d\n",count);
	return count;
}

int LinkedList_getLength(LinkedList root){
	LinkedList list = root;
	int length = 0;
	for(list=root,length=0;list;list=list->next){length++;}
	return length;
}

void *LinkedList_get(LinkedList list){
	return list ? list->content : NULL;
}
void *LinkedList_getIndex(LinkedList root, int k){
	int i;
	LinkedList list = root;
	for(i=0,list=root;list;list=list->next,i++){
		if(i==k){return list->content;}
	}
	return NULL;
}

LinkedList LinkedList_getNext(LinkedList list){return list->next;}
LinkedList LinkedList_getPrevious(LinkedList root, LinkedList list){
	LinkedList p,prev;
	for(p=prev=root;p!=list;p=p->next){
		prev=p;
	}
	return prev;
}

LinkedList LinkedList_cut(LinkedList list, LinkedList list2){
	LinkedList p;
	for(p=list;p;p=p->next){
		if(p->next==list2){p->next=NULL;}
	}
	return list;
}

/*
void *LinkedList_toArray(LinkedList root){
	int i,n;
	void *array_p;
	void *ptr;
	size_t size = root->interface.size;
	
	LinkedList list=root;
	n = LinkedList_getLength(root);
	array_p = allocate(n*size);
	//printf("memory : %lx is allocated\n",(long unsigned int)array_p);
	//printf("size : %lx\n",(long unsigned int)size);
	//printf("size of ptr : %lx\n",sizeof(array_p));
	ptr = array_p;
	for(i=0;i<n;i++){
		//printf("memcpy : %lx - > %lx\n",(long unsigned int)list->content,(long unsigned int)ptr);
		memcpy(ptr,list->content,size);
		list = list->next;
		ptr = (char*)ptr + size; 
		//char型は一バイトだからchar*にキャストすればバイト単位の足し算ができる。
	}
	LinkedList_deleteRoot(root);
	return array_p;
}
*/

void LinkedList_print(LinkedList root, char *(*toString)(void *a)){
	LinkedList list = root;
	
	if(!root){printf("{null}\n"); return;}
	
	while(list){
		printf("{%s}",toString(list->content));
		list = list->next;
		if(list){printf("->");}
	}
	printf("\n");
}

char *LinkedList_vprintf(void *x){return (char*)x;}

void LinkedList_enum(LinkedList root, void (func)(void *)){
	int n;
	LinkedList list = root;
	
	if(!root){return;}
	
	while(list){
		func(list->content);
		list = list->next;
	}
}

void *LinkedList_toArray(LinkedList root, size_t size){
	void *array=NULL,*p;
	LinkedList list = root;
	p = array = allocate(size*LinkedList_getLength(list));
	for(list=root;list;list=list->next,p = (char*)p + size){
		memcpy(p,LinkedList_get(list),size);
	}
	return array;
}

LinkedList LinkedList_tail(LinkedList list){
	LinkedList tail=NULL;
	while(list){tail = list; list = list->next;}
	return tail;
}

int LinkedList_isTail(LinkedList list){
	if(list->next){return 0;}
	else {return 1;}
}

void LinkedList_test(LinkedList list, FILE *fp){
	int*k,*buf;
	LinkedList s;
	clock_t start,end;
	fp = fopen("test.txt","w");
	
	start = clock();
	for(s=list;s;s=s->next){
		k = s->content;
		fprintf(fp,"%d\n",*k);
	}
	end = clock();
	printf(KRED "no func call : %f msec\n" KNRM,1000*(double)(end-start)/CLOCKS_PER_SEC);
}

StringList StringList_init(){
	StringList sl = LinkedList_create(NULL);
	return sl;
}

StringList StringList_append(StringList list, char *string){
	return LinkedList_append(list,string);
}

char *StringList_copy(StringList sl,char *str){
	char *str1 = allocate(strlen(str)+1);
	if(!str1){perror("StringList_pass : malloc error\n");}
	strcpy(str1,str);
	if(!sl->content){LinkedList_set(sl,str1);}
	else{LinkedList_append(sl,str1);}
	return str1;
}

char *StringList_pass(StringList sl,char *str){
	if(!sl->content){LinkedList_set(sl,str);}
	else{LinkedList_append(sl,str);}
	return str;
}

char *StringList_collect_mod(char c, int release){
	static StringList list;
	char *str=NULL;
	if(release){
		str = LinkedList_toArray(list,sizeof(char));
		LinkedList_deleteRoot(list,deallocate);
		list =NULL;
	}else{
		list = LinkedList_append(list,String_ncopy(&c,1));
	}
	return str;
}

void StringList_collect(char c){StringList_collect_mod(c,0);}
char *StringList_release(){return StringList_collect_mod(0,1);}

void StringList_finish(StringList sl){
	LinkedList_deleteRoot(sl,deallocate);
}

void StringList_unwrap(StringList sl){
	LinkedList_deleteRoot(sl,NULL);
}

int StringList_length(StringList list){
	int count=0;
	for(;list;list=list->next){count++;}
	return count;
}

char **StringList_toArray(StringList list){
	int i,len;
	char **array;
	len = StringList_length(list);
	array = allocate(len*sizeof(char*));
	for(i=0;i<len;i++,list++){
		array[i] = String_copy((char*)(list->content));
	}
	return array;
}

StringList StringList_splitString(char *string, char **separators, int separatorsCount,int repeatedSeparators){
	int i,imin,len,*exist;
	char *p,*p2,*last,*pmin;
	StringList list=NULL;
	
	exist = allocate(separatorsCount*sizeof(int));
	for(i=0;i<separatorsCount;i++){exist[i] = 1;}
	last = strchr(string,'\0');
	for(p=string;p<last;p=p2){
		for(i=0,pmin=last,imin=-1;i<separatorsCount;i++){
			if(exist[i]){
			p2=strstr(p,separators[i]);
				if(p2){
					if(p2 < pmin){
						imin = i;
						pmin = p2;
					}
				}else{
					exist[i] = 0;
				}
			}
		}
		if(imin<0){
			p2 = last;
		}else{
			len = strlen(separators[imin]);
			for(i=0;i<len;i++){pmin[i] = '\0';}
			p2 = &pmin[i];
		}
		if(*p || !repeatedSeparators){list=StringList_append(list,p);}
	}
	deallocate(exist);
	return list;
}