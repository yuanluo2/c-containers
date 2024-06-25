#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

typedef void(*GenericDoublyList_RemoveElemFunc)(void*);
void GenericDoublyList_RemoveElemFunc_Default(void*) {}

typedef struct GenericDoublyListNode {
    struct GenericDoublyListNode* prev;
    struct GenericDoublyListNode* next;
} GenericDoublyListNode;

typedef struct GenericDoublyList {
    GenericDoublyListNode* head;
    GenericDoublyListNode* tail;
    size_t length;
    size_t elemSize;

    GenericDoublyList_RemoveElemFunc removeElemFunc;
} GenericDoublyList;

#define GenericDoublyList_NodePrev(nodePtr)   ((nodePtr)->prev)
#define GenericDoublyList_NodeNext(nodePtr)   ((nodePtr)->next)
#define GenericDoublyList_NodeData(nodePtr)   ((nodePtr) + 1)

#define GenericDoublyList_Length(listPtr)     ((listPtr)->length)
#define GenericDoublyList_Front(listPtr)      ((listPtr)->head)
#define GenericDoublyList_Back(listPtr)       ((listPtr)->tail)
#define GenericDoublyList_IsEmpty(listPtr)    (GenericDoublyList_Length(listPtr) == 0)

#define GenericDoublyList_ForEach(listPtr, nodePtr) \
    for ((nodePtr) = (listPtr)->head; (nodePtr) != NULL; (nodePtr) = (nodePtr)->next)

#define GenericDoublyList_ForEachReverse(listPtr, nodePtr) \
    for ((nodePtr) = (listPtr)->tail; (nodePtr) != NULL; (nodePtr) = (nodePtr)->prev)

GenericDoublyList* GenericDoublyList_CreateNew(size_t elemSize, GenericDoublyList_RemoveElemFunc func) {
    if (elemSize == 0) {
        return NULL;
    }

    GenericDoublyList* list = (GenericDoublyList*)malloc(sizeof(GenericDoublyList));
    if (list == NULL) {
        return NULL;
    }

    list->removeElemFunc = (func == NULL ? GenericDoublyList_RemoveElemFunc_Default : func);
    list->head = list->tail = NULL;
    list->length = 0;
    list->elemSize = elemSize;
    return list;
}

void GenericDoublyList_Clear(GenericDoublyList* list) {
    GenericDoublyListNode* node = list->head;

    while (list->head != NULL) {
        node = node->next;
        list->removeElemFunc(GenericDoublyList_NodeData(list->head));
        free(list->head);
        list->head = node;
    }

    list->length = 0;
}

void GenericDoublyList_Destroy(GenericDoublyList* list) {
    GenericDoublyList_Clear(list);
    free(list);
}

void* GenericDoublyList_PushBack(GenericDoublyList* list) {
    GenericDoublyListNode* node = (GenericDoublyListNode*)malloc(sizeof(GenericDoublyListNode) + list->elemSize);
    if (node == NULL) {
        return NULL;
    }

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
    return (void*)(node + 1);
}

void* GenericDoublyList_PushFront(GenericDoublyList* list) {
    GenericDoublyListNode* node = (GenericDoublyListNode*)malloc(sizeof(GenericDoublyListNode) + list->elemSize);
    if (node == NULL) {
        return NULL;
    }

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
    return (void*)(node + 1);
}

GenericDoublyListNode* GenericDoublyList_RemoveNode(GenericDoublyList* list, GenericDoublyListNode* node, int isBackOrder) {
    GenericDoublyListNode* retNode;

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

    list->removeElemFunc(GenericDoublyList_NodeData(node));
    free(node);
    list->length -= 1;
    return retNode;
}

void GenericDoublyList_PopFront(GenericDoublyList* list) {
    if (GenericDoublyList_IsEmpty(list)) {
        return;
    }

    GenericDoublyList_RemoveNode(list, list->head, 1);
}

void GenericDoublyList_PopBack(GenericDoublyList* list) {
    if (GenericDoublyList_IsEmpty(list)) {
        return;
    }

    GenericDoublyList_RemoveNode(list, list->tail, 1);
}

int main() {
    GenericDoublyList* list = GenericDoublyList_CreateNew(sizeof(int), NULL);

    int i;
    int* ptr;
    for (i = 0; i < 10; ++i) {
        ptr = GenericDoublyList_PushBack(list);
        *ptr = i * i;
    }

    GenericDoublyListNode* node;
    for (node = list->head; node != NULL;) {
        if (*(int*)GenericDoublyList_NodeData(node) % 2 != 0) {
            node = GenericDoublyList_RemoveNode(list, node, 1);
        }
        else {
            node = node->next;
        }
    }

    GenericDoublyList_ForEach(list, node) {
        printf("%d\n", *(int*)GenericDoublyList_NodeData(node));
    }

    GenericDoublyList_Destroy(list);
    return 0;
}
