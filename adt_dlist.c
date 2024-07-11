#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

typedef struct DListNode DListNode;
typedef struct DList DList;

typedef void (*DList_ElemDestroyFunc) (void* elem);

struct DListNode {
    DListNode* prev;
    DListNode* next;
    void* data;
};

struct DList {
    DListNode* head;
    DListNode* tail;
    size_t length;

    DList_ElemDestroyFunc elemDestroy;
};

#define DList_NodePrev(nodePtr)   ((nodePtr)->prev)
#define DList_NodeNext(nodePtr)   ((nodePtr)->next)
#define DList_NodeData(nodePtr)   ((nodePtr)->data)

#define DList_Front(listPtr)     ((listPtr)->head)
#define DList_Back(listPtr)      ((listPtr)->tail)
#define DList_Length(listPtr)    ((listPtr)->length)
#define DList_IsEmpty(listPtr)   (DList_Length(listPtr) == 0)

#define DList_ForEach(listPtr, nodePtr) \
	for ((nodePtr) = (listPtr)->head; (nodePtr) != NULL; (nodePtr) = (nodePtr)->next)
	
#define DList_ForEachReverse(listPtr, nodePtr) \
	for ((nodePtr) = (listPtr)->tail; (nodePtr) != NULL; (nodePtr) = (nodePtr)->prev)

void DList_DefaultElemDestroyFunc(void* elem) {}

DList* DList_CreateNew(DList_ElemDestroyFunc func) {
    DList* list = (DList*)malloc(sizeof(DList));

    list->head = list->tail = NULL;
    list->length = 0;
    list->elemDestroy = (func == NULL ? DList_DefaultElemDestroyFunc : func);

    return list;
}

void DList_Clear(DList* list) {
    DListNode* node = list->head;

    while (list->head != NULL) {
        node = node->next;
        list->elemDestroy(list->head->data);
        free(list->head);
        list->head = node;
    }

    list->length = 0;
}

void DList_Destroy(DList* list) {
    DList_Clear(list);
    free(list);
}

int DList_PushBack(DList* list, void* elem) {
    DListNode* node = (DListNode*)malloc(sizeof(DListNode));
    if (node == NULL) {
        return 0;
    }

    node->data = elem;

    if (list->head == NULL) {
        node->prev = node->next = NULL;
        list->head = list->tail = node;
    }
    else {
        node->prev = list->tail;
        node->next = NULL;
        list->tail->next = node;
        list->tail = list->tail->next;
    }

    list->length += 1;
    return 1;
}

int DList_PushFront(DList* list, void* elem) {
    DListNode* node = (DListNode*)malloc(sizeof(DListNode));
    if (node == NULL) {
        return 0;
    }

    node->data = elem;

    if (list->head == NULL) {
        node->prev = node->next = NULL;
        list->head = list->tail = node;
    }
    else {
        node->prev = NULL;
        node->next = list->head;
        list->head = node;
    }

    list->length += 1;
    return 1;
}

DListNode* DList_DeleteNode(DList* list, DListNode* node, int isBackOrder) {
    DListNode* retNode;

    if (list->head == list->tail) {
        list->head = list->tail = NULL;
        retNode = NULL;
    }
    else {
        if (node == list->head) {
            list->head = node->next;
            list->head->prev = NULL;
            retNode = isBackOrder ? list->head : NULL;
        }
        else if (node == list->tail) {
            list->tail = node->prev;
            list->tail->next = NULL;
            retNode = isBackOrder ? NULL : list->tail;
        }
        else {
            node->prev->next = node->next;
            node->next->prev = node->prev;
            retNode = isBackOrder ? node->next : node->prev;
        }
    }

    list->elemDestroy(node->data);
    free(node);
    list->length -= 1;
    return retNode;
}

void DList_DeleteAll(DList* list, void* elem, size_t elmeSize) {
    DListNode* node = list->head;

    while (node != NULL) {
        if (memcmp(node->data, elem, elmeSize) == 0) {
            node = DList_DeleteNode(list, node, 1);
        }
        else {
            node = node->next;
        }
    }
}

void DList_PopBack(DList* list) {
    (void)DList_DeleteNode(list, list->tail, 1);
}

void DList_PopFront(DList* list) {
    (void)DList_DeleteNode(list, list->head, 1);
}

int main() {
    DList* list = DList_CreateNew(free);

    int i = 0;
    int* data;
    for (i = 0; i < 5; ++i) {
        data = malloc(sizeof(int));
        *data = i * i;

        DList_PushBack(list, data);
    }

    data = malloc(sizeof(int));
    *data = 16;
    DList_PushBack(list, data);

    data = malloc(sizeof(int));
    *data = 25;
    DList_PushBack(list, data);

    int removeData = 1;
    DList_DeleteAll(list, &removeData, sizeof(int));

    DListNode* node;
    DList_ForEach(list, node) {
        printf("%d\n", *(int*)DList_NodeData(node));
    }

    DList_Destroy(list);
    return 0;
}
