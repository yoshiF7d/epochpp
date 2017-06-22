#ifndef ARRAY_LIST
#define ARRAY_LIST

#include <stdio.h>
#include "utilities.h"

typedef struct st_arraylist *ArrayList;
#define DEFAULT_BLOCK_SIZE 64

ArrayList ArrayList_create(int size);
void ArrayList_delete(ArrayList list, void (*delete)(void*));

ArrayList ArrayList_append(ArrayList list, void *content);
void ArrayList_remove(ArrayList list, int k, void (*delete)(void*));

void ArrayList_removeContent(ArrayList list, void *content, void (*delete)(void*));
void ArrayList_removeContentRange(ArrayList list, int s, int e, void *content, void (*delete)(void*));
void ArrayList_removeEquals(ArrayList list, void *content, int (*equals)(void*,void*), void (*delete)(void*));
void ArrayList_removeEqualsRange(ArrayList list, int s, int e, void *content, int (*equals)(void*,void*), void (*delete)(void*));
void ArrayList_removeDupilcateEquals(ArrayList list, int (*equals)(void*, void*),void (*delete)(void*));

int ArrayList_findContent(ArrayList list, void *content);
int ArrayList_findContentRange(ArrayList list, int s, int e, void *content);
ArrayList ArrayList_findList(ArrayList list, void *content);

int ArrayList_findEquals(ArrayList list, void *content, int (*equals)(void*,void*));
int ArrayList_findEqualsRange(ArrayList list, int s, int e, void *content, int (*equals)(void*,void*));
ArrayList ArrayList_findEqualsList(ArrayList list, void *content, int (*equals)(void*,void*));

ArrayList ArrayList_gather(ArrayList list);
void ArrayList_swap(ArrayList list, int i, int j);
ArrayList ArrayList_resize(ArrayList list, int size);

void ArrayList_setContent(ArrayList list, int k, void *content);
int ArrayList_getSize(ArrayList list);
int ArrayList_getCount(ArrayList list);
void *ArrayList_getContent(ArrayList list, int k);
#define ArrayList_get(list,k) ArrayList_getContent(list,k)

void ArrayList_print(ArrayList list, char *toString(void*));
void ArrayList_enum(ArrayList list, void func(void*));
void ArrayList_enumGathered(ArrayList list, void func(void*));

#endif
