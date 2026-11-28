#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stdbool.h>
#include <stddef.h>

// Linked list node contained in an individual hash table bucket
typedef struct HashTableLink {
    const char* key;
    void* value;
    struct HashTableLink* next;
} HashTableLink;

typedef struct HashTable {
    size_t numBuckets;
    HashTableLink** buckets; // array of HashTableLink pointers
} HashTable;

HashTable* createHashTable(size_t numBuckets);
void destroyHashTable(HashTable* ht);
void printHashTable(HashTable* ht);

void* getElement(HashTable* ht, const char* key);
bool insertElement(HashTable* ht, const char* key, void* value);
void* replaceElement(HashTable* ht, const char* key, void* value);
void* deleteElement(HashTable* ht, const char* key);

#endif