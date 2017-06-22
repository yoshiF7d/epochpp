#include "HashTable.h"

/*hash table : open adressing*/
/*method : Hopscotch hasing*/

#define NEIGHBOR_LENGTH 8

typedef struct st_hashtable{
	ArrayList list;
	char *infolist;
	int count;
	int size;
}HashTable_Sub;

typedef struct st_hashtable_content{
	char *key;
	void *content;
}HashTableContent_Sub;

typedef struct st_hashtable_content *HashTableContent;

int bitcount(int b);
char *bitprint(int b, int length);
void bitprint2(int b, int length);
void bitprint3(int b, int length);

ArrayList HashTable_gather(HashTable ht);

HashTableContent HashTableContent_create(char *key,void *content);
void HashTableContent_delete_imp(HashTableContent htc, void (*delete)(void*),int mode);
void HashTableContent_delete_init(void (*delete)(void*));
void HashTableContent_delete(void *htc);
//int HashTableContent_equals(void *a, void *b);

char *HashTableContent_getKey(HashTableContent htc);
void *HashTableContent_getContent(HashTableContent htc);

char *HashTableContent_toString_imp(HashTableContent htc, char *(*toString)(void*),int mode);
char *HashTableContent_toString(void *htc);
void HashTableContent_toString_init(char *(*toString)(void*));
void HashTableContent_toString_finish();

#define CAST_HTC(content) ((HashTableContent)(content))

HashTable HashTable_create(int size){
	int i;
	HashTable table = allocate(sizeof(HashTable_Sub));
	table->list = ArrayList_create(size);
	table->infolist = allocate(size);
	for(i=0;i<size;i++){table->infolist[i] = 0;}
	table->size = size;
	table->count = 0;
	return table;
}
 
void HashTable_delete(HashTable table, void (*delete)(void*)){
	int i;
	HashTableContent_delete_init(delete);
	ArrayList_delete(table->list,HashTableContent_delete);
	table->list = NULL;
	deallocate(table->infolist);
	table->infolist = NULL;	
	deallocate(table);
}

char *printch2(void *c){return (char*)c;}

HashTable HashTable_resize(HashTable ht, int size){
	int i,index,count;
	char *key;
	HashTableContent htc;
	
	ArrayList list = HashTable_gather(ht);
	count = ht->count;
	ArrayList_delete(ht->list,NULL);
	deallocate(ht->infolist);
	ht->list = ArrayList_create(size);
	ht->infolist = allocate(size);
	for(i=0;i<size;i++){ht->infolist[i]=0;}
	//HashTableContent_toString_init(printch2);
	ht->count = 0; ht->size = size;
	HashTableContent_delete_init(NULL);
	for(i=0;i<count;i++){
		htc = ArrayList_getContent(list,i);
		if(htc){
			HashTable_append(ht,htc->key,htc->content);
			HashTableContent_delete(htc);
		}
	}
	//HashTableContent_toString_finish();
	ArrayList_delete(list,NULL);
	return ht;
}

ArrayList HashTable_gather(HashTable ht){
	int i,k=0;
	void *content;
	ArrayList list = ArrayList_create(ht->count);
	//printf("ht->count : %d\n",ht->count);
	for(i=0;i<ht->size;i++){
		if((content = ArrayList_getContent(ht->list,i))){
			ArrayList_setContent(list,k++,content);
		}
	}
	//printf("k : %d\n",k);
	return list;
}


#define CHUNKSIZE 2
unsigned long HashTable_getHash(char *key){
	int i,j;
	int length = strlen(key);
	unsigned long sum1 = 0;
	unsigned long sum2 = 0; 
	
	for(i=0;i<length;i++){
		if(i%CHUNKSIZE==0){
			sum2 += sum1;
			sum1=key[i];
		}
		else{
			sum1 <<= 8;		/*1 byte*/
			sum1 |= key[i];
		}
	}
	sum2 += sum1;
	return sum2;
}

#define rollindex(index) ((index<0) ? hashtable->size + (index) : (index))
#define onebit(n) (1<<(n))
#define alterinfo(info,a,b) {info &= ~a; info |= b;}

HashTable HashTable_append(HashTable hashtable, char *key, void *content){
	if(!key || !content){return hashtable;}
	HashTableContent htc = NULL;
	unsigned long index = HashTable_getHash(key)%hashtable->size;
	unsigned long index_sav = index;
	int i,j,k;
	char *info;
	unsigned int s,t; /*size of s must be bigger than size of htc->info*/
	enum {none,success,swap,resize}flag = none;
	i=0;
	//printf("HashTable_append : key : %s\tindex : %d\n",key,index);
	if(hashtable->count == hashtable->size){flag = resize;}
	/*search for empty slot*/
	while(flag != resize && ArrayList_getContent(hashtable->list,index)){
		index++; i++;
		if(index == hashtable->size){index = 0;}
		if(i == hashtable->size){flag = resize; break;}
	}
	/*list[index] is the empty slot (have to use ArrayList_getContent to refer)*/
	while(flag != resize){
		if(i < NEIGHBOR_LENGTH){flag = success; break;}
		/*empty slot is at further than neighbor length*/
		j=1;
		while(flag != swap && j<NEIGHBOR_LENGTH){
			//htc = ArrayList_getContent(hashtable->list,rollindex(index-j)); /*needless. shouldn't be NULL because list[index-j] is the first non NULL element looked from left*/
			info = &hashtable->infolist[rollindex(index-j)];
			t = s = onebit(j);
			//printf("index : %d\tj : %d\n",index, j);
			//printf("%s\n",htc->key);
			//printf("info\t"); bitprint2(*info,8);
			//printf("s\t"); bitprint2(s,8);
			j++; k=1; s >>=1;
			while(s){
				if(*info & s){
					alterinfo(*info,s,t);
					ArrayList_swap(hashtable->list,rollindex(index-k),index);
					//printf("i : %d\tj : %d\tSWAP(%d,%d)\n",i,j,rollindex(index-k),index);
					//HashTable_print(hashtable,NULL); printf("\n");
					i -= k; index = rollindex(index-k);
					flag = swap;
					break;
				}
				s >>= 1; k++;
			}
		}
		if(flag == swap){flag = none;}
		else{flag = resize;}
	}
	if(flag == success){
		htc = HashTableContent_create(key,content);
		ArrayList_setContent(hashtable->list,index,htc);
		hashtable->count++;
		info = &hashtable->infolist[index_sav];
		*info |= 1<< i;
		//printf("success\ti:%d\tindex:%d\t",i,index_sav%hashtable->size); bitprint2(*info,8);
	}else if(flag == resize){
		//printf("HashTable_append : resize operation is invoked\n");		
		HashTable_resize(hashtable,2*hashtable->size);
		HashTable_append(hashtable,key,content);
	}else{
		printf("HashTable_append : something unexpected happend\n");
	}
	//HashTable_print(hashtable,NULL); printf("\n");		
	return hashtable;
}

#undef rollindex

void *HashTable_find(HashTable hashtable, char *key){
	char *info,i,s;
	HashTableContent htc;
	unsigned long index = HashTable_getHash(key)%hashtable->size;
	//printf("key : %s\tindex :  %d\n",key,index); 
	htc = ArrayList_getContent(hashtable->list,index);
	if(!htc){goto unregistered;}
	info = &hashtable->infolist[index];
	s = 1;
	//bitprint2(*info,8); 
	for(i=0;i<NEIGHBOR_LENGTH;i++,index++){
		if(index == hashtable->size){index = 0;}
		if(!(htc = ArrayList_getContent(hashtable->list,index))){s <<= 1;continue;}
		if(!(*info & s)){s <<= 1;continue;}
		//printf("%s\n",htc->key);
		if(!strcmp(htc->key,key)){
			return htc->content;
		}
		//bitprint2(s,8);
		s <<= 1;
	}
  unregistered:
	//printf("HashTable_find : key \"%s\" is unregistered\n",key);
	return NULL;
}

int bitcount(int b){
	int s,k=0;
	for(s=1;s;s<<=1){
		if(s & b){k++;}
	}
	return k;
}

#define MIN(a,b) ((a) > (b) ? (b) : (a))
char *bitprint(int b, int length){
	int s,k,n;
	unsigned int u;
	n = MIN(8*sizeof(int),length)+1;
	char *buf = allocate(n);
	for(s=1,k=0;s>0&&k<length-1;k++,s<<=1){}
	u = s; /*to tell gcc to do [plain right shift]. not [arithmetic right shift] */
	/*really dangerous operation because it relies on compiler behavior*/
	/*if [arithmetic right shift] is used, below code will become [infinite loop]*/
	
	for(k=0;u&&k<length;k++,u>>=1){
		if(u & b){buf[k]='1';}
		else{buf[k]='0';}
	}
	buf[n-1] = '\0';
	return buf;
}
#undef MIN
void bitprint2(int b, int length){
	int s,k;
	for(s=1,k=0;s>0&&k<length;k++,s<<=1){
		if(s & b){putchar('1');}
		else{putchar('0');}
	}
	putchar('\n');
}

void bitprint3(int b, int length){
	int s,k;
	for(s=1,k=0;s>0&&k<length;k++,s<<=1){
		if(s & b){putchar('1');}
		else{putchar('0');}
	}
}



void HashTable_print(HashTable hashtable, char *(*toString)(void*)){
	int i,max,temp;
	HashTableContent htc;
	HashTableContent_toString_init(toString);
	max = 0;
	for(i=0;i<hashtable->size;i++){
		htc = ArrayList_getContent(hashtable->list,i);
		if(htc){
			if(max < (temp = bitcount(hashtable->infolist[i]))){max = temp;}
		}
		printf("%d\t%s\t",i,HashTableContent_toString(htc));  bitprint3(hashtable->infolist[i],8); putchar('\n');
	}
	HashTableContent_toString_finish();
	printf("number of slots : %d\n",hashtable->size);
	printf("number of contents : %d\n",hashtable->count);
	printf("load factor : %f\n",hashtable->count/(double)hashtable->size);
	printf("max collision %d\n",max);	
}

void HashTable_enum(HashTable hashtable, void (*func)(void*)){
	int i;
	HashTableContent htc;
	for(i=0;i<hashtable->size;i++){
		htc = ArrayList_get(hashtable->list,i);
		if(htc){func(HashTableContent_getContent(htc));}
	}
}

int HashTable_enumSortedByKey_compare(void *a,void *b){
	HashTableContent htc1,htc2;
	htc1 = a; htc2 = b;
	return strcmp(htc1->key,htc2->key);
}

void HashTable_enumSortedByKey(HashTable hashtable, void (*func)(void*)){
	int i;
	LinkedList s,list = NULL;
	HashTableContent htc;
	for(i=0;i<hashtable->size;i++){
		htc = ArrayList_get(hashtable->list,i);
		if(htc){
			list = LinkedList_append(list,htc);
		}
	}
	list = LinkedList_sort(list,HashTable_enumSortedByKey_compare);
	for(s=list;s;s=LinkedList_increment(s)){
		htc = LinkedList_get(s);
		LinkedList_set(s,htc->content);
	}
	LinkedList_enum(list,func);
	LinkedList_deleteRoot(list,NULL);
}

HashTableContent HashTableContent_create(char *key,void *content){
	HashTableContent htc = allocate(sizeof(HashTableContent_Sub));
	htc->key = String_copy(key);
	htc->content = content;
	return htc;
}

char *HashTableContent_getKey(HashTableContent htc){return htc->key;}
void *HashTableContent_getContent(HashTableContent htc){return htc->content;}

void HashTableContent_delete_imp(HashTableContent htc, void (*delete)(void*),int mode){
	static void (*delete_imp)(void*);	
	if(mode == 0){delete_imp = delete;}
	else{
		if(delete_imp && htc){delete_imp(htc->content);}
		//printf("htc : %x\n",htc);
		if(htc){deallocate(htc->key);}
		deallocate(htc);
	}
}

void HashTableContent_delete_init(void (*delete)(void*)){
	HashTableContent_delete_imp(NULL,delete,0);
}

void HashTableContent_delete(void *htc){
	HashTableContent_delete_imp((HashTableContent)htc , NULL ,1);
}

/*
int HashTableContent_equals(void *a, void *b){
	if(!strcmp(CAST_HTC(a))->key,(CAST_HTC(b))->key){
		return 1;
	}else{
		return 0;
	}
}
*/

char *HashTableContent_toString_imp(HashTableContent htc, char *(*toString)(void*),int mode){
	static char *(*toString_imp)(void*);
	static StringList lead;
	char *buf = NULL,*buf1 = NULL;
	if(mode == 0){
		toString_imp = toString;
		lead = StringList_init();
	}else if(mode == 1){
		if(htc){
			if(toString_imp){buf1 = toString_imp(htc->content);} 
			else{buf1 = "null";}
			//buf = allocate(strlen(htc->key)+strlen(buf1)+2);
			//sprintf(buf,"%s,%s",htc->key,buf1);
			buf = allocate(strlen(htc->key)+1+strlen(buf1)+1+8+1);
			sprintf(buf,"%s",htc->key);
		}else{
			buf1 = "null";
			buf = allocate(strlen(buf1)+1+8+1);
			sprintf(buf,"%s",buf1);
		}
		StringList_pass(lead,buf);
	}else{
		StringList_finish(lead);
	}
	return buf;
}

char *HashTableContent_toString(void *htc){
	return HashTableContent_toString_imp(CAST_HTC(htc),NULL,1);
}

void HashTableContent_toString_init(char *(*toString)(void*)){
	HashTableContent_toString_imp(NULL,toString,0);
}

void HashTableContent_toString_finish(){
	HashTableContent_toString_imp(NULL,NULL,2);
}


