#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

typedef struct Array Array;
typedef void (*Array_ElemDestroyFunc) (void* elem);

struct Array {
    void** data;
    size_t capacity;
    size_t length;

    Array_ElemDestroyFunc elemDestroy;
};

#define Array_At(arrPtr, index)   ((arrPtr)->data[(index)])
#define Array_Capacity(arrPtr)    ((arrPtr)->capacity)
#define Array_Length(arrPtr)      ((arrPtr)->length)
#define Array_IsEmpty(arrPtr)     (Array_Length(arrPtr) == 0)
#define Array_Front(arrPtr)       Array_At(arrPtr, 0)
#define Array_Back(arrPtr)        Array_At(arrPtr, Array_Length(arrPtr) - 1)

#define Array_ForEach(arrPtr, cursor) \
    for (cursor = 0; cursor < Array_Length(arrPtr); ++cursor)

#define Array_ForEachReverse(arrPtr, cursor) \
    for (cursor = Array_Length(arrPtr) - 1; cursor >= 0; --cursor)

void Array_DefaultElemDestroyFunc(void* elem) {}

Array* Array_CreateNew(size_t capacity, Array_ElemDestroyFunc func) {
    Array* arr = (Array*)malloc(sizeof(Array));
    if (arr == NULL) {
        return NULL;
    }

    arr->length = 0;
    arr->elemDestroy = (func == NULL ? Array_DefaultElemDestroyFunc : func);
    arr->capacity = (capacity != 0) ? capacity : 5;
    arr->data = (void**)malloc(arr->capacity * sizeof(void*));

    if (arr->data == NULL) {
        free(arr);
        arr = NULL;
    }

    return arr;
}

void Array_Destroy(Array* arr) {
    size_t i;
    for (i = 0; i < Array_Length(arr); ++i) {
        arr->elemDestroy(Array_At(arr, i));
    }

    free(arr->data);
    free(arr);
}

int Array_ExpandCapacity(Array* arr, size_t newCapacity) {
    void** temp = (void**)realloc(arr->data, newCapacity * sizeof(void*));
    if (temp == NULL) {
        return 0;
    }

    arr->data = temp;
    arr->capacity = newCapacity;
    return 1;
}

int Array_PushBack(Array* arr, void* elem) {
    if (arr->capacity == arr->length) {
        if (!Array_ExpandCapacity(arr, 2 * arr->capacity)) {
            return 0;
        }
    }

    arr->data[arr->length] = elem;
    arr->length += 1;
    return 1;
}

void Array_PopBack(Array* arr) {
    if (Array_IsEmpty(arr)) {
        return;
    }

    arr->length -= 1;
}

void Array_Remove(Array* arr, size_t index) {
    size_t i;

    if (index >= Array_Length(arr)) {
        return;
    }

    for (i = index; i < Array_Length(arr); ++i) {
        Array_At(arr, i) = Array_At(arr, i + 1);
    }

    arr->length -= 1;
}

int main() {
    Array* arr = Array_CreateNew(0, NULL);

    Array_PushBack(arr, "using your power");
    Array_PushBack(arr, "never turn back");
    Array_PushBack(arr, "do what you should do");

    Array_Remove(arr, 1);

    size_t i;
    Array_ForEach(arr, i) {
        printf("%s\n", (char*)Array_At(arr, i));
    }

    Array_Destroy(arr);
    return 0;
}
