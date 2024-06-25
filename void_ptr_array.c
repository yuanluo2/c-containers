#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

typedef void(*GenericArray_RemoveElemFunc)(void*);
void GenericArray_RemoveElemFunc_Default(void*) {}

typedef struct GenericArray {
    void* data;
    size_t capacity;
    size_t length;
    size_t elemSize;

    GenericArray_RemoveElemFunc removeElemFunc;
} GenericArray;

#define GenericArray_At(arrPtr, index) \
    ((char*)((arrPtr)->data) + (index) * (arrPtr)->elemSize)

#define GenericArray_Capacity(arrPtr)   ((arrPtr)->capacity)
#define GenericArray_Length(arrPtr)     ((arrPtr)->length)
#define GenericArray_Data(arrPtr)       ((arrPtr)->data)
#define GenericArray_Clear(arrPtr)      GenericArray_Length(arrPtr) = 0;
#define GenericArray_IsEmpty(arrPtr)    (GenericArray_Length(arrPtr) == 0)
#define GenericArray_Front(arrPtr)      GenericArray_At(arrPtr, 0)
#define GenericArray_Back(arrPtr)       GenericArray_At(arrPtr, GenericArray_Length(arrPtr) - 1)

#define GenericArray_ForEach(arrPtr, cursor) \
    for (cursor = 0; cursor < GenericArray_Length(arrPtr); ++cursor)

#define GenericArray_ForEachReverse(arrPtr, cursor) \
    for (cursor = GenericArray_Length(arrPtr) - 1; cursor >= 0; --cursor)

GenericArray* GenericArray_CreateNew(size_t capacity, size_t elemSize, GenericArray_RemoveElemFunc func) {
    if (elemSize == 0) {
        return NULL;
    }

    GenericArray* arr = (GenericArray*)malloc(sizeof(GenericArray));
    if (arr == NULL) {
        return NULL;
    }

    arr->removeElemFunc = (func == NULL ? GenericArray_RemoveElemFunc_Default : func);
    arr->elemSize = elemSize;
    arr->length = 0;
    arr->capacity = (capacity == 0 ? 15 : capacity);

    if ((arr->data = malloc(arr->capacity * elemSize)) == NULL) {
        free(arr);
        return NULL;
    }

    return arr;
}

void GenericArray_Destroy(GenericArray* arr) {
    size_t i;
    GenericArray_ForEach(arr, i) {
        arr->removeElemFunc(GenericArray_At(arr, i));
    }

    free(arr->data);
    free(arr);
}

int GenericArray_ExpandCapacity(GenericArray* arr, size_t newCapacity) {
    void* temp = realloc(arr->data, newCapacity * arr->elemSize);
    if (temp == NULL) {
        return 0;
    }

    arr->data = temp;
    arr->capacity = newCapacity;
    return 1;
}

void* GenericArray_PushBack(GenericArray* arr) {
    if (arr->length == arr->capacity) {
        if (!GenericArray_ExpandCapacity(arr, 2 * arr->capacity)) {
            return NULL;
        }
    }

    arr->length += 1;
    return (void*)GenericArray_At(arr, arr->length - 1);
}

void GenericArray_Remove(GenericArray* arr, size_t index) {
    if (index >= GenericArray_Length(arr)) {
        return;
    }

    arr->removeElemFunc(GenericArray_At(arr, index));

    size_t i;
    for (i = index; i < GenericArray_Length(arr); ++i) {
        memcpy(GenericArray_At(arr, i), GenericArray_At(arr, i + 1), arr->elemSize);
    }

    arr->length -= 1;
    arr->removeElemFunc(GenericArray_At(arr, arr->length));
}

void GenericArray_PopFront(GenericArray* arr) {
    GenericArray_Remove(arr, 0);
}

void GenericArray_PopBack(GenericArray* arr) {
    GenericArray_Remove(arr, GenericArray_Length(arr) - 1);
}

int main() {
    GenericArray* arr = GenericArray_CreateNew(5, sizeof(long long), NULL);

    long long i;
    long long* data;
    for (i = 0; i < 1000000000; ++i) {
        data = GenericArray_PushBack(arr);
        *data = i * i;        
    }

    // GenericArray_PopFront(arr);
    // GenericArray_Remove(arr, 3);

    // GenericArray_ForEach(arr, i) {
    //     printf("%lld\n", *(long long*)GenericArray_At(arr, i));
    // }

    printf("%lld\n", *(long long*)GenericArray_Back(arr));
    GenericArray_Destroy(arr);
    return 0;
}
