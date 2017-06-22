#include "ArrayList.h"

typedef struct st_arraylist{
	void **array;
	int size;
	int count;	
}ArrayList_Sub;

ArrayList ArrayList_create(int size){
	int i;
	ArrayList list = allocate(sizeof(ArrayList_Sub));
	list->array = allocate(size*sizeof(void*));
	for(i=0;i<size;i++){list->array[i] = NULL;}
	list->size = size;
	list->count = 0;
	return list;
}
void ArrayList_delete(ArrayList list, void (*delete)(void *)){
	int i;
	if(delete){
		for(i=0;i<list->size;i++){
			if(list->array[i]){
				delete(list->array[i]);
				list->array[i] = NULL;
			}
		}
	}
	deallocate(list->array);
	deallocate(list);
}

void ArrayList_remove(ArrayList list, int k, void (*delete)(void*)){
	int i=k;
	if(k > abs(list->size)){
		fprintf(stderr,"ArrayList_getContent : invalid index\n");
		return;
	}
	if(k < 0){i = list->size + k;}
	if(list->array[i]){
		if(delete){delete(list->array[i]);}
		list->array[i]=NULL;
		list->count--;
	}
}

void ArrayList_removeContent(ArrayList list, void *content, void (*delete)(void*)){
	int i;
	for(i=0;i<list->size;i++){
		if(content==list->array[i]){
			if(list->array[i] && delete){
				delete(list->array[i]);
				list->array[i] = NULL;
				list->count--;
			}
		}
	}
}

void ArrayList_removeContentRange(ArrayList list, int s, int e, void *content, void (*delete)(void*)){
	int i;
	for(i=s;i<e;i++){
		if(content==list->array[i]){
			if(list->array[i] && delete){
				delete(list->array[i]);
				list->array[i] = NULL;
				list->count--;
			}
		}
	}
}

void ArrayList_removeEquals(ArrayList list, void *content, int (*equals)(void*,void*), void (*delete)(void*)){
	int i;
	for(i=0;i<list->size;i++){
		if(equals(content,list->array[i])){
			if(list->array[i] && delete){
				delete(list->array[i]);
				list->array[i] = NULL;
				list->count--;
			}
		}
	}
}

void ArrayList_removeEqualsRange(ArrayList list, int s, int e, void *content, int (*equals)(void*,void*), void (*delete)(void*)){
	int i;
	for(i=s;i<=e;i++){
		if(equals(content,list->array[i])){
			if(list->array[i] && delete){
				delete(list->array[i]);
				list->array[i] = NULL;
				list->count--;
			}
		}
	}
}

void ArrayList_removeDupilcateEquals(ArrayList list, int (*equals)(void*, void*), void (*delete)(void*)){
	int i,j;
	for(i=0;i<list->size;i++){
		for(j=i+1;j<list->size;j++){
			if(list->array[i] && list->array[j]){
				if(equals(list->array[i],list->array[j])){
					if(delete){
						delete(list->array[j]); list->array[j]=NULL;
						list->count--;
					}
				}
			}
		}
	}
}

int ArrayList_findContent(ArrayList list, void *content){
	int i=0;
	for(i=0;i<list->size;i++){
		if(list->array[i] == content){
			return i;
		}
	}
	return -1;
}

int ArrayList_findContentRange(ArrayList list, int s, int e, void *content){
	int i=0;
	for(i=s;i<=e;i++){
		if(list->array[i] == content){
			return i;
		}
	}
	return -1;
}

ArrayList ArrayList_findContentList(ArrayList list, void *content){
	ArrayList nlist = ArrayList_create(list->size);
	int i=0;
	for(i=0;i<list->size;i++){
		if(list->array[i] == content){
			ArrayList_append(nlist,list->array[i]);
		}
	}
	return nlist;
}

int ArrayList_findEquals(ArrayList list, void *content, int (*equals)(void*,void*)){
	int i;
	for(i=0;i<list->size;i++){
		if(equals(list->array[i],content)){
			return i;
		}
	}
	return -1;
}

int ArrayList_findEqualsRange(ArrayList list, int s, int e, void *content, int (*equals)(void*,void*)){
	int i=0;
	for(i=s;i<=e;i++){
		if(equals(list->array[i],content)){
			return i;
		}
	}
	return -1;
}


ArrayList ArrayList_findEqualsList(ArrayList list, void *content, int (*equals)(void*,void*)){
	ArrayList nlist = ArrayList_create(list->size);
	int i=0;
	for(i=0;i<list->size;i++){
		if(equals(list->array[i],content)){
			ArrayList_append(nlist,list->array[i]);
		}
	}
	return nlist;
}

ArrayList ArrayList_gather(ArrayList list){
	int i,j;
	for(i=0;i<list->size;i++){
		if(!list->array[i]){
			for(j=i+1;j<list->size;j++){
				if(list->array[j]){
					list->array[i] = list->array[j];
					list->array[j] = NULL;
					break;
				}
			}
		}
	}
	return list;
}

ArrayList ArrayList_resize(ArrayList list, int size){
	if(size < list->size){fprintf(stderr,"ArrayList_resize : size down operation is disabled\n");return list;}
	else{
		list->array = reallocate(list->array,size);
		memset(list->array[list->size],0,(size - list->size)*sizeof(void*));
		list->size = size;
		return list;
	}
}

ArrayList ArrayList_append(ArrayList list, void *content){
	int i;
	if(!list){list = ArrayList_create(DEFAULT_BLOCK_SIZE);}
	for(i=0;i<list->size;i++){
		if(!list->array[i]){
			list->count++;
			list->array[i] = content; 
			return list;
		}
	}
	printf("ArrayList_append : resize\n");
	list = ArrayList_resize(list,list->size+DEFAULT_BLOCK_SIZE);
	return ArrayList_append(list,content);
}

void ArrayList_swap(ArrayList list, int i, int j){
	void *temp;
	if(i > abs(list->size) || j > abs(list->size)){
		fprintf(stderr,"ArrayList_swap : invalid index\n");
		return;
	}
	if(i < 0){i = list->size+i;}
	if(j < 0){j = list->size+j;}
	temp = list->array[i];
	list->array[i] = list->array[j];
	list->array[j] = temp;
}

void *ArrayList_getContent(ArrayList list, int k){
	int i = k;
	if(k > abs(list->size)){
		fprintf(stderr,"ArrayList_getContent : invalid index\n");
		return NULL;
	}
	if(k < 0){i = list->size + k;}
	return list->array[i];
}

void ArrayList_setContent(ArrayList list, int k, void *content){
	int i = k;
	if(k > abs(list->size)){
		fprintf(stderr,"ArrayList_getContent : invalid index\n");
		return;
	}
	if(k < 0){i = list->size + k;}
	if(list->array[i] && content==NULL){list->count--;}
	if(list->array[i]==NULL && content){list->count++;}
	list->array[i] = content;
}

int ArrayList_getSize(ArrayList list){
	return list->size;
}

int ArrayList_getCount(ArrayList list){
	return list->count;
}

void ArrayList_print(ArrayList list, char *toString(void*)){
	int i;
	for(i=0;i<list->size;i++){printf("%s\n",toString(list->array[i]));}
}

void ArrayList_enum(ArrayList list, void func(void*)){
	int i;
	for(i=0;i<list->size;i++){
		func(list->array[i]);
	}
}

void ArrayList_enumGathered(ArrayList list, void func(void*)){
	int i;
	for(i=0;i<list->count;i++){
		func(list->array[i]);
	}
}

