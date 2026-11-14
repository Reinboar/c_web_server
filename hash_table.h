#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>
#include <stddef.h>

// Linked list node contained in an individual hash table bucket
typedef struct HashTableLink {
    const char* key;
    char* value;
    struct HashTableLink* next;
} HashTableLink;

typedef struct HashTable {
    size_t numBuckets;
    HashTableLink** buckets; // array of HashTableLink pointers
} HashTable;

HashTable* createHashTable(size_t numBuckets);
void destroyHashTable(HashTable* ht);
void printHashTable(HashTable* ht);

char* getElement(HashTable* ht, const char* key);
bool insertElement(HashTable* ht, const char* key, char* value);
char* replaceElement(HashTable* ht, const char* key, char* value);
char* deleteElement(HashTable* ht, const char* key);

#endif