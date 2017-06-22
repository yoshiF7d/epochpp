#include "HashTable.h"

/*hash table : open adressing*/
/*method : Hopscotch hasing*/

#define NEIGHBOR_LENGTH 8

typedef struct st_hashtable{
	ArrayList list;
	char *infolist;
	int count;
	int size;
	char *(*getkey)(void*);
}HashTable_Sub;

typedef struct st_hashtable_content *HashTableContent;

int bitcount(int b);
char *bitprint(int b, int length);
void bitprint2(int b, int length);
void bitprint3(int b, int length);

ArrayList HashTable_gather(HashTable ht);
char *HashTable_default_getkey(void* c){return c;}
HashTable HashTable_create(int size){
	int i;
	HashTable table = allocate(sizeof(HashTable_Sub));
	table->list = ArrayList_create(size);
	table->infolist = allocate(size);
	for(i=0;i<size;i++){table->infolist[i] = 0;}
	table->size = size;
	table->count = 0;
	table->getkey = HashTable_default_getkey;
	return table;
}

HashTable HashTable_create2(int size, char *(*getkey)(void*)){
	int i;
	HashTable table = allocate(sizeof(HashTable_Sub));
	table->list = ArrayList_create(size);
	table->infolist = allocate(size);
	for(i=0;i<size;i++){table->infolist[i] = 0;}
	table->size = size;
	table->count = 0;
	table->getkey = getkey;
	return table;
}
 
void HashTable_delete(HashTable table, void (*delete)(void*)){
	int i;
	ArrayList_delete(table->list,delete);
	table->list = NULL;
	deallocate(table->infolist);
	table->infolist = NULL;
	table->getkey = NULL;
	deallocate(table);
}

char *printch2(void *c){return (char*)c;}

HashTable HashTable_resize(HashTable ht, int size){
	int i,index,count;
	char *key;
	void *content;
	
	ArrayList list = HashTable_gather(ht);
	count = ht->count;
	ArrayList_delete(ht->list,NULL);
	deallocate(ht->infolist);
	ht->list = ArrayList_create(size);
	ht->infolist = allocate(size);
	for(i=0;i<size;i++){ht->infolist[i]=0;}
	//HashTableContent_toString_init(printch2);
	ht->count = 0; ht->size = size;
	for(i=0;i<count;i++){
		content = ArrayList_getContent(list,i);
		if(content){
			HashTable_append(ht,ht->getkey(content),content);
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
			sum1 = key[i];
		}else{
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
	unsigned int index = HashTable_getHash(key)%hashtable->size;
	unsigned int index_sav = index;
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
		ArrayList_setContent(hashtable->list,index,content);
		hashtable->count++;
		info = &hashtable->infolist[index_sav];
		*info |= 1<< i;
		//printf("success\ti:%d\tindex:%d\t",i,index_sav%hashtable->size); bitprint2(*info,8);
	}else if(flag == resize){
		fprintf(stderr,"HashTable_append : resize operation\n");
		fprintf(stderr,"key : %s\n",key);
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
	void *content;
	unsigned int index = HashTable_getHash(key)%hashtable->size;
	//printf("key : %s\tindex :  %d\n",key,index); 
	content = ArrayList_getContent(hashtable->list,index);
	if(!content){goto unregistered;}
	info = &hashtable->infolist[index];
	s = 1;
	//bitprint2(*info,8); 
	for(i=0;i<NEIGHBOR_LENGTH;i++,index++){
		if(index == hashtable->size){index = 0;}
		if(!(content = ArrayList_getContent(hashtable->list,index))){s <<= 1;continue;}
		if(!(*info & s)){s <<= 1;continue;}
		//printf("%s\n",htc->key);
		if(!strcmp(hashtable->getkey(content),key)){
			return content;
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
	max = 0;
	void *content;
	for(i=0;i<hashtable->size;i++){
		content = ArrayList_getContent(hashtable->list,i);
		if(content){
			if(max < (temp = bitcount(hashtable->infolist[i]))){max = temp;}
		}
		printf("%d\t",i);
		//if(content){printf("%s\t",toString(content));}
		bitprint3(hashtable->infolist[i],8);
		printf("\t%lx\t",(unsigned long)content);
		putchar('\n');
	}
	printf("toString adress : %lx\n",(unsigned long)toString );
	printf("number of slots : %d\n",hashtable->size);
	printf("number of contents : %d\n",hashtable->count);
	printf("load factor : %f\n",hashtable->count/(double)hashtable->size);
	printf("max collision %d\n",max);	
}

void HashTable_enum(HashTable hashtable, void (*func)(void*)){
	int i;
	void *content;
	for(i=0;i<hashtable->size;i++){
		content = ArrayList_get(hashtable->list,i);
		if(content){func(content);}
	}
}

void HashTable_enumSorted(HashTable hashtable, void (*func)(void*), int (*compare)(void*,void*)){
	int i;
	void *content;
	LinkedList list=NULL;
	for(i=0;i<hashtable->size;i++){
		content = ArrayList_get(hashtable->list,i);
		if(content){
			list = LinkedList_append(list,content);
		}
	}
	//printf("length : %d\n",LinkedList_getLength(list));
	list=LinkedList_sort(list,compare);
	//printf("length : %d\n",LinkedList_getLength(list));
	LinkedList_enum(list,func);
	LinkedList_deleteRoot(list,NULL);
}
