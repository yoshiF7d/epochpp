#ifndef HASH_TABLE
#define HASH_TABLE

#include "LinkedList.h"
#include "ArrayList.h"
#include "utilities.h"

/*hash table : open adressing*/
/*hash function : https://www.google.co.jp/webhp?sourceid=chrome-instant&ion=1&espv=2&ie=UTF-8#q=hash%20function%20example*/

typedef struct st_hashtable *HashTable;

HashTable HashTable_create(int size);
HashTable HashTable_create2(int size, char *(*getkey)(void*));
HashTable HashTable_create3(int size, unsigned long (*getHash)(void*));
void HashTable_delete(HashTable hashtable, void (*delete)(void*));

HashTable HashTable_resize(HashTable ht, int size);
void *HashTable_find(HashTable hashtable, char *key);
void *HashTable_find2(HashTable hashtable, int id);
#define HashTable_get(hashtable,key) HashTable_find(hashtable,key)

unsigned long HashTable_getHash(char *key);
HashTable HashTable_append(HashTable hashtable, char *key, void *content);
HashTable HashTable_append2(HashTable hashtable, void *content);

void HashTable_print(HashTable hashtable, char *(*toString)(void*));
void HashTable_enum(HashTable hashtable, void (*func)(void*));
void HashTable_enumSorted(HashTable hashtable, void (*func)(void*), int (*compare)(void*,void*));
#endif
