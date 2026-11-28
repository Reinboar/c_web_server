#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "hash_table.h"

HashTable* createHashTable(size_t numBuckets) {
    HashTable* ht = calloc(1, sizeof(HashTable));
    ht->numBuckets = numBuckets;
    ht->buckets = calloc(numBuckets, sizeof(HashTableLink*));
    return ht;
}

// Follows the chain of a single hash table bucket and deallocates each link in it
void destroyHashTableBucket(HashTableLink* htl) {
    HashTableLink* curLink = htl;
    HashTableLink* prevLink = NULL;
    while (curLink != NULL) {
        if (curLink->value != NULL) {
            free(curLink->value);
        }
        prevLink = curLink;
        curLink = curLink->next;
        free(prevLink);
    }
}

void destroyHashTable(HashTable* ht) {
    if (!ht)
        return;
    for (int i = 0; i < ht->numBuckets; i++) {
        destroyHashTableBucket(ht->buckets[i]);
    }
    free(ht);
}

void printHashTable(HashTable* ht) {
    for (int i = 0; i < ht->numBuckets; i++) {
        HashTableLink* curLink = ht->buckets[i];
        while (curLink) {
            printf("bucket: %d, %s : %s\n", i, curLink->key, curLink->value);
            curLink = curLink->next;
        }
    }
}

HashTableLink* createHashTableLink(const char* key, void* value) {
    HashTableLink* htl = calloc(1,sizeof(HashTableLink));
    htl->key = key;
    htl->value = value;
    htl->next = NULL;
    return htl;
}

// Calculates the bucket index that the given key should be stored in
//   on the given hash table
unsigned int calcBucketIndex(HashTable* ht, const char* key) {
    int keyLen = strlen(key);
    char resultIndex = 0;
    for (int i = 0; i < keyLen; i++) {
        char c = key[i];
        resultIndex += c;
    }
    return resultIndex % ht->numBuckets;
}

// Returns NULL if an element with the given key does not exist
HashTableLink* getLink(HashTable* ht, const char* key) {
    if (!ht)
        return NULL;
    unsigned int bucketIndex = calcBucketIndex(ht, key);
    HashTableLink* curLink = ht->buckets[bucketIndex];
    while (curLink) {
        if (strcmp(curLink->key, key) == 0) {
            return curLink;
        }
        curLink = curLink->next;
    }
    return NULL;
}

// Returns NULL if an element with the given key does not exist
void* getElement(HashTable* ht, const char* key) {
    if (!ht)
        return NULL;
    HashTableLink* link = getLink(ht, key);
    if (link)
        return link->value;
    return NULL;
}

// Inserts a new element into a hash table with the given key and value
// Returns false if NULL is given as the hash table or if an element with
//   given key already exists. Returns true otherwise
bool insertElement(HashTable* ht, const char* key, void* value) {
    if (!ht)
        return false;
    unsigned int bucketIndex = calcBucketIndex(ht, key);
    HashTableLink* curLink = ht->buckets[bucketIndex];
    if (curLink) {
        while (curLink && curLink->next) {
            if (strcmp(curLink->key, key) == 0)
                return false;
            curLink = curLink->next;
        }
        curLink->next = createHashTableLink(key, value);
    } else {
        ht->buckets[bucketIndex] = createHashTableLink(key, value);
    }
    return true;
}

// Replaces the value of the given key with a new value
// Returns the old value at the given key, NULL if the element does not exist
void* replaceElement(HashTable* ht, const char* key, void* value) {
    if (!ht)
        return NULL;
    HashTableLink* link = getLink(ht, key);
    if (link) {
        void* oldValue = link->value;
        link->value = value;
        return oldValue;
    } else {
        insertElement(ht, key, value);
        return NULL;
    }
}

// Deletes the element in the hash table at the given key
// Returns the old value of the given key, returns NULL if the element does not exist
void* deleteElement(HashTable* ht, const char* key) {
    if (!ht)
        return NULL;
    unsigned int bucketIndex = calcBucketIndex(ht, key);
    HashTableLink* curLink = ht->buckets[bucketIndex];
    HashTableLink* prevLink = NULL;
    while (curLink) {
        if (strcmp(curLink->key, key) == 0) {
            void* oldValue = curLink->value;
            if (prevLink) { // Relink the previous and next links together if this is not the first link in the bucket
                prevLink->next = curLink->next;
            } else { // Set the first link in the bucket to the next link, if curlink is the first link in the bucket
                ht->buckets[bucketIndex] = curLink->next;
            }
            free(curLink);
            return oldValue;
        }
        prevLink = curLink;
        curLink = curLink->next;
    }
    return NULL;
}