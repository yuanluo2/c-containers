#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define DEFAULT_HASH_TABLE_BUCKET_MAX_LEN   256

typedef unsigned int (*GenericHashTable_HashFunc) (void*);
typedef int (*GenericHashTable_CompareFunc) (void*, void*);
typedef void(*GenericHashTable_RemoveKeyElemFunc)(void*);
typedef void(*GenericHashTable_RemoveValueElemFunc)(void*);

void GenericHashTable_RemoveKeyElemFunc_Default(void*) {}
void GenericHashTable_RemoveValueElemFunc_Default(void*) {}

typedef struct GenericHashNode {
    struct GenericHashNode* next;
} GenericHashNode;

typedef struct GenericHashTable {
    GenericHashNode** bucket;
    size_t bucketSize;
    size_t length;
    size_t keyElemSize;
    size_t valueElemSize;

    GenericHashTable_HashFunc hashFunc;
    GenericHashTable_CompareFunc compareFunc;
    GenericHashTable_RemoveKeyElemFunc removeKeyElemFunc;
    GenericHashTable_RemoveValueElemFunc removeValueElemFunc;
} GenericHashTable;

#define GenericHashNode_Key(hashTablePtr, nodePtr) \
    (void*)((char*)(nodePtr + 1))

#define GenericHashNode_Value(hashTablePtr, nodePtr) \
    (void*)((char*)(nodePtr + 1) + (hashTablePtr)->keyElemSize)

#define GenericHashTable_Length(hashTablePtr) \
    ((hashTablePtr)->length)

#define GenericHashTable_BucketSize(hashTablePtr) \
    ((hashTablePtr)->bucketSize)   

#define GenericHashTable_CreateNode(hashTablePtr) \
    (GenericHashNode*)malloc(sizeof(GenericHashNode) + ht->keyElemSize + ht->valueElemSize)


void GenericHashTable_RemoveNode(GenericHashTable* ht, GenericHashNode* node) {
    ht->removeKeyElemFunc(GenericHashNode_Key(ht, node));
    ht->removeValueElemFunc(GenericHashNode_Value(ht, node));
    free(node);
}

GenericHashTable* GenericHashTable_CreateNew(size_t bucketSize,
                                            size_t keyElemSize, 
                                            size_t valueElemSize, 
                                            GenericHashTable_HashFunc hashFunc, 
                                            GenericHashTable_CompareFunc compareFunc,
                                            GenericHashTable_RemoveKeyElemFunc removeKeyElemFunc,
                                            GenericHashTable_RemoveValueElemFunc removeValueElemFunc) 
{
    if (keyElemSize == 0 || valueElemSize == 0 || hashFunc == NULL || compareFunc == NULL) {
        return NULL;
    }

    GenericHashTable* ht = (GenericHashTable*)malloc(sizeof(GenericHashTable));
    if (ht == NULL) {
        return NULL;
    }

    ht->bucketSize = (bucketSize == 0 ? DEFAULT_HASH_TABLE_BUCKET_MAX_LEN : bucketSize);
    ht->bucket = (GenericHashNode**)malloc(ht->bucketSize * sizeof(GenericHashNode*));
    if (ht->bucket == NULL) {
        free(ht);
        return NULL;
    }

    size_t i;
    for (i = 0; i < ht->bucketSize; ++i) {
        ht->bucket[i] = NULL;
    }

    ht->length = 0;
    ht->keyElemSize = keyElemSize;
    ht->valueElemSize = valueElemSize;
    ht->hashFunc = hashFunc;
    ht->compareFunc = compareFunc;
    ht->removeKeyElemFunc = (removeKeyElemFunc == NULL ? GenericHashTable_RemoveKeyElemFunc_Default : removeKeyElemFunc);
    ht->removeValueElemFunc = (removeValueElemFunc == NULL ? GenericHashTable_RemoveValueElemFunc_Default : removeValueElemFunc);
    return ht;
}

void GenericHashTable_Destroy(GenericHashTable* ht) {
    size_t i;
    GenericHashNode* node;

    for (i = 0; i < ht->bucketSize; ++i) {
        if (ht->bucket[i] != NULL) {
            node = ht->bucket[i];

            while (ht->bucket[i] != NULL) {
                node = node->next;
                GenericHashTable_RemoveNode(ht, ht->bucket[i]);
                ht->bucket[i] = node;
            }
        }
    }
}

GenericHashNode* GenericHashTable_Search(GenericHashTable* ht, void* key) {
    GenericHashNode* node;

    for (node = ht->bucket[ht->hashFunc(key)]; node != NULL; node = node->next) {
        if (ht->compareFunc(key, GenericHashNode_Key(ht, node)) != 0) {
            return node;
        }
    }

    return NULL;
}

void GenericHashTable_Set(GenericHashTable* ht, GenericHashNode* node) {
    GenericHashNode* findNode;
    unsigned int hashValue;

    if ((findNode = GenericHashTable_Search(ht, GenericHashNode_Key(ht, node))) == NULL) {
        hashValue = ht->hashFunc(GenericHashNode_Key(ht, node));
        node->next = ht->bucket[hashValue];
        ht->bucket[hashValue] = node;
        
        ht->length += 1;
    }
    else {
        ht->removeKeyElemFunc(GenericHashNode_Key(ht, findNode));
        ht->removeValueElemFunc(GenericHashNode_Value(ht, findNode));

        memcpy(GenericHashNode_Key(ht, findNode), GenericHashNode_Key(ht, node), ht->keyElemSize);
        memcpy(GenericHashNode_Value(ht, findNode), GenericHashNode_Value(ht, node), ht->valueElemSize);
    }
}

/* usage. */
int compare(void* left, void* right) {
    return !strcmp((const char*)left, (const char*)right);
}

unsigned int hash(void* data) {   /* from K & R. */
    const char* s = (const char*)data;
    unsigned hashval;

    for (hashval = 0; *s != '\0'; ++s) {
        hashval = *s + 31 * hashval;
    }

    return hashval % 101;
}

#define CHAR_BUF_MAX_LEN 20

void create_my_hash_node(GenericHashTable*ht, const char* key, const char* value) {
    GenericHashNode* node = GenericHashTable_CreateNode(ht);
    snprintf((char*)GenericHashNode_Key(ht, node), CHAR_BUF_MAX_LEN, "%s", key);
    snprintf((char*)GenericHashNode_Value(ht, node), CHAR_BUF_MAX_LEN, "%s", value);

    GenericHashTable_Set(ht, node);
}

int main() {
    GenericHashTable* hashTable = GenericHashTable_CreateNew(101,
                                                            CHAR_BUF_MAX_LEN * sizeof(char), 
                                                            CHAR_BUF_MAX_LEN * sizeof(char),
                                                            hash,
                                                            compare,
                                                            NULL,
                                                            NULL);

    create_my_hash_node(hashTable, "Bjarne", "Stroustrup");
    create_my_hash_node(hashTable, "a", "b");
    create_my_hash_node(hashTable, "c", "d");
    create_my_hash_node(hashTable, "e", "f");
    create_my_hash_node(hashTable, "Dennis", "Ritchie");

    GenericHashNode* temp;
    if ((temp = GenericHashTable_Search(hashTable, "Dennis")) == NULL) {
        printf("not found\n");
    }
    else {
        printf("%s\n", (const char*)GenericHashNode_Value(hashTable, temp));
    }

    GenericHashTable_Destroy(hashTable);
}
