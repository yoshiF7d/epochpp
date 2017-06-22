#ifndef LINKEDLIST_H
#define LINKEDLIST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utilities.h"

typedef struct st_linkedlist{
	void *content;
	struct st_linkedlist *next;
}LinkedList_Sub;
typedef struct st_linkedlist *LinkedList;
typedef LinkedList StringList;

LinkedList LinkedList_create(void *content);
void LinkedList_delete(LinkedList list, void (*delete)(void *content));

LinkedList LinkedList_removeIndex(LinkedList root,void (*delete)(void *content), int k);
LinkedList LinkedList_remove(LinkedList root, LinkedList list,void (*delete)(void *content));
LinkedList LinkedList_removeSubList(LinkedList root, LinkedList list, void (*delete)(void *content));
LinkedList LinkedList_removeEquals(LinkedList list, void *content, int (*equals)(void*, void*), void (*delete)(void*));
LinkedList LinkedList_removeDuplicateEquals(LinkedList list, int (*equals)(void*, void*), void (*delete)(void*));

void LinkedList_deleteRoot(LinkedList root, void (*delete)(void *content));
void LinkedList_unwrapRoot(LinkedList root);

LinkedList LinkedList_skip(LinkedList list, int k);
LinkedList LinkedList_increment(LinkedList list);
LinkedList LinkedList_decrement(LinkedList list);
LinkedList LinkedList_append(LinkedList root, void *content);
LinkedList LinkedList_prepend(LinkedList root, void *content);
LinkedList LinkedList_concatenate(LinkedList root, LinkedList root2);
LinkedList LinkedList_duplicate(LinkedList root, void *(*duplicate)(void*));
void *LinkedList_duplicate_func(void *content);
void LinkedList_duplicate_func_setSize(size_t size);

LinkedList LinkedList_insert(LinkedList root, int k, void *content);
LinkedList LinkedList_insertList(LinkedList root, int k, LinkedList list);
LinkedList LinkedList_insertListBehind(LinkedList root, LinkedList current, LinkedList list);

LinkedList LinkedList_select(LinkedList list, int (*crit)(void*));
LinkedList LinkedList_find(LinkedList list, void *content);
LinkedList LinkedList_findEquals(LinkedList list, void *content, int (*equals)(void*, void*));
LinkedList LinkedList_search(LinkedList list,int (* equals)(void*,void*), void *content);
int LinkedList_isExist(LinkedList list, void *content);
int LinkedList_locate(LinkedList list, LinkedList list2);

LinkedList LinkedList_setIndex(LinkedList root, int k, void *content);
void LinkedList_set(LinkedList list, void *content);
void LinkedList_setNext(LinkedList list, LinkedList next);
LinkedList LinkedList_cut(LinkedList list, LinkedList list2);

void LinkedList_swapContent(LinkedList n1, LinkedList n2); 
LinkedList LinkedList_sort(LinkedList root, int compare(void *c1, void *c2));

void *LinkedList_getIndex(LinkedList root, int k); /*get 1st one (root) : LinkedList_getIndex(root,0)*/
void *LinkedList_get(LinkedList list);
LinkedList LinkedList_getNext(LinkedList list);
LinkedList LinkedList_getPrevious(LinkedList root, LinkedList list);
#define LinkedList_length(root) LinkedList_getLength(root)
int LinkedList_getLength(LinkedList root);

int LinkedList_count(LinkedList list,int (* equals)(void*,void*),void *content);
char *LinkedList_print_func(void *x);
void LinkedList_print(LinkedList root, char *(*toString)(void *a));
void LinkedList_enum(LinkedList root, void (func)(void *));
void *LinkedList_toArray(LinkedList root, size_t size);

LinkedList LinkedList_tail(LinkedList list);
int LinkedList_isTail(LinkedList list);

void LinkedList_test(LinkedList list, FILE *fp);

/*use StringList_pass if you want deallocate strings with StringList_finsih.*/
/*use StringList_copy if you want to keep strings after StringList_finish.*/

StringList StringList_init();
StringList StringList_splitString(char *string, char **separators, int separatorsCount,int repeatedSeparators);

StringList StringList_append(StringList list, char *string);
char *StringList_pass(StringList sl,char *str);
char *StringList_copy(StringList sl,char *str);
void StringList_finish(StringList sl);
void StringList_unwrap(StringList sl);
int StringList_length(StringList list);
char **StringList_toArray(StringList list);
void StringList_collect(char c); /*not reentrant*/
char *StringList_release(); /*not reentrant*/
//void *LinkedList_toArray(LinkedList root);

#endif
