#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define HASHMAP_DEFAULT_BUCKET_SIZE 101

typedef struct HashMapNode HashMapNode;
typedef struct HashMap HashMap;
typedef unsigned int HashType;

typedef int (*HashMap_CompareFunc) (void* left, void* right);   /* return 0 means equal. */
typedef HashType (*HashMap_KeyHashFunc) (void* key);

typedef void (*HashMap_KeyDestroyFunc) (void* key);
typedef void (*HashMap_ValueDestroyFunc) (void* value);

struct HashMapNode {
    HashMapNode* next;
    void* key;
    void* value;
};

struct HashMap {
    HashMapNode* bucket[HASHMAP_DEFAULT_BUCKET_SIZE];
    size_t length;

    HashMap_CompareFunc compare;
    HashMap_KeyHashFunc hash;
    HashMap_KeyDestroyFunc keyDestroy;
    HashMap_ValueDestroyFunc valueDestroy;
};

void HashMap_DefaultKeyDestroyFunc(void* key) {}
void HashMap_DefaultValueDestroyFunc(void* value) {}

HashMap* HashMap_CreateNew(HashMap_CompareFunc compare, 
                    HashMap_KeyHashFunc hash, 
                    HashMap_KeyDestroyFunc keyDestroy, 
                    HashMap_ValueDestroyFunc valueDestroy) {
    if (compare == NULL || hash == NULL) {
        return NULL;
    }

    HashMap* hm = (HashMap*)malloc(sizeof(HashMap));
    if (hm == NULL) {
        return NULL;
    }

    hm->length = 0;
    hm->compare = compare;
    hm->hash = hash;
    hm->keyDestroy = (keyDestroy == NULL ? HashMap_DefaultKeyDestroyFunc : keyDestroy);
    hm->valueDestroy = (valueDestroy == NULL ? HashMap_DefaultValueDestroyFunc : valueDestroy);

    size_t i;
    for (i = 0; i < HASHMAP_DEFAULT_BUCKET_SIZE; ++i) {
        hm->bucket[i] = NULL;
    }

    return hm;
}

void HashMap_Destroy(HashMap* hm) {
    size_t i;
    HashMapNode* node;

    for (i = 0; i < HASHMAP_DEFAULT_BUCKET_SIZE; ++i) {
        if (hm->bucket[i] != NULL) {
            node = hm->bucket[i];

            while (hm->bucket[i] != NULL) {
                node = node->next;

                hm->keyDestroy(hm->bucket[i]->key);
                hm->valueDestroy(hm->bucket[i]->value);
                free(hm->bucket[i]);

                hm->bucket[i] = node;
            }
        }
    }

    free(hm);
}

HashMapNode* HashMap_Find(HashMap* hm, void* key) {
    HashType hashval = hm->hash(key);
    HashMapNode* node;

    for (node = hm->bucket[hashval]; node != NULL; node = node->next) {
        if (hm->compare(node->key, key) == 0) {
            return node;
        }
    }

    return NULL;
}

int HashMap_Insert(HashMap* hm, void* key, void* value) {
    HashMapNode* findNode;
    HashType hashValue;

    if ((findNode = HashMap_Find(hm, key)) == NULL) {
        findNode = (HashMapNode*)malloc(sizeof(HashMapNode));
        if (findNode == NULL) {
            return 0;
        }

        findNode->key = key;
        findNode->value = value;

        hashValue = hm->hash(key);
        findNode->next = hm->bucket[hashValue];
        hm->bucket[hashValue] = findNode;
        
        hm->length += 1;
    }
    else {
        hm->keyDestroy(findNode->key);
        hm->valueDestroy(findNode->value);

        findNode->key = key;
        findNode->value = value;
    }

    return 1;
}

void HashMap_Remove(HashMap* hm, void* key) {
    HashType hashval = hm->hash(key);
    HashMapNode* node;
    HashMapNode* prev;

    if (hm->bucket[hashval] == NULL) {
        return;
    }

    if (hm->compare(hm->bucket[hashval]->key, key) == 0) {
        node = hm->bucket[hashval];
        hm->bucket[hashval] = node->next;
    }
    else {
        prev = hm->bucket[hashval];
        node = prev->next;

        while (node != NULL) {
            if (hm->compare(node->key, key) == 0) {
                prev->next = node->next;
                break;
            }
            else {
                prev = node;
                node = node->next;
            }
        }
    }

    hm->length -= 1;
    hm->keyDestroy(node->key);
    hm->valueDestroy(node->value);
    free(node);
}

HashType hash_c_style_str(void* key) {
    const char* str = (const char*)key;
    HashType hashval = 0;

    for (; *str != '\0' ; ++str)
        hashval = *str + hashval * 31;

    return hashval % HASHMAP_DEFAULT_BUCKET_SIZE;
}

int compare_c_style_str(void* left, void* right) {
    return strcmp((const char*)(left), (const char*)(right));
}

int main() {
    HashMap* hm = HashMap_CreateNew(compare_c_style_str, hash_c_style_str, NULL, NULL);

    HashMap_Insert(hm, "abc", "def");
    HashMap_Insert(hm, "ock", "dlcma");
    HashMap_Insert(hm, "d3q", "lcke");
    HashMap_Insert(hm, "fzc", "dddz");

    /* find. */
    HashMapNode* node = HashMap_Find(hm, "abc");
    if (node != NULL) {
        printf("find %s -> %s\n", "abc", (const char*)node->value);
    }
    else {
        printf("not found\n");
    }

    /* traverse. */
    printf("\ntraverse: \n");

    size_t i;
    for (i = 0; i < HASHMAP_DEFAULT_BUCKET_SIZE; ++i) {
        if (hm->bucket[i] != NULL) {
            node = hm->bucket[i];

            while (node != NULL) {
                printf("  {'%s': '%s'}\n", (const char*)node->key, (const char*)node->value);
                node = node->next;
            }
        }
    }

    HashMap_Destroy(hm);
}
