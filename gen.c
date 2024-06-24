/**
 * author: yuanluo2
 * 
 * brief: 
 * This program is a code generator to generate specific data type data stuctures or algorithms, written in ansi C.
 * 
 * background:
 * I write this program to just generate specific type data structures or algorithms in c, because I really disklike
 * the macro trick (hard to debug) or void* trick (performance issue) to simulate generic, linux kernel's intrusive 
 * trick is really good, but it can't avoid the casting like void*, and by this intrusive way, you can lie: hold 
 * different type, which could lead to some tricky problems. Simulate generics in C seems not a easy thing, so why not
 * think back, just using specific data type? 
 * 
 * One day, I finally gived up on a horrible problem, a very long macro to simulate C++'s template for generic usage,
 * code is so difficult to understand, really hard to debug, and wasted me a lot of time. After that, I can't tolerant 
 * the ways using macros to do such things anymore.
 * 
 * That's why I have to write this program, because using a code generator could get and see the real code (not preprocessor), 
 * type safe, no need to do cast, easy to modify, easy to debug. Though the generated codes is not the best, but 
 * they suits for most cases, I can easily test them, push them to my projects, that's enough.
 * 
 * usage:
 * just look at the example function below.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

typedef struct String {
    char* data;
    size_t capacity;
    size_t length;
} String;

#define String_At(strPtr, index)   ((strPtr)->data[(index)])
#define String_Capacity(strPtr)    ((strPtr)->capacity)
#define String_Length(strPtr)      ((strPtr)->length)
#define String_Data(strPtr)        ((strPtr)->data)
#define String_IsEmpty(strPtr)     (String_Length(strPtr) == 0)

String* String_CreateNew(size_t capacity) {
    String* str;
    
    if ((str = (String*)malloc(sizeof(String))) == NULL) {
        return NULL;
    }

    str->length = 0;
    str->capacity = (capacity != 0) ? capacity : 5;

    if ((str->data = (char*)malloc(str->capacity * sizeof(char))) == NULL) {
        free(str);
        str = NULL;
    }

    return str;
}

String* String_Create_CStyle(const char* cstr) {
    if (cstr == NULL) {
        return String_CreateNew(0);
    }

    size_t len = strlen(cstr);
    size_t capacity = (size_t)(1.5 * len);
    String* str = String_CreateNew(capacity);
    if (str == NULL) {
        return NULL;
    }

    str->capacity = capacity;
    str->length = len;
    memcpy(str->data, cstr, (len + 1) * sizeof(char));   /* contain '\0'. */
    return str;
}

void String_Destroy(String* str) {
    free(str->data);
    free(str);
}

int String_ExpandCapacity(String* str, size_t newCapacity) {
    char* temp = (char*)realloc(str->data, newCapacity * sizeof(char));
    if (temp == NULL) {
        return 0;
    }

    str->data = temp;
    str->capacity = newCapacity;
    return 1;
}

int String_Append_CStyle(String* str, const char* cstr, size_t cstrLen) {
    size_t newCapacity = str->length + cstrLen + 1;

    if (newCapacity > str->capacity) {
        if (!String_ExpandCapacity(str, (size_t)(1.5 * newCapacity))) {
            return 0;
        }
    }

    memcpy(str->data + str->length, cstr, cstrLen * sizeof(char));
    str->length += cstrLen;
    str->data[str->length] = '\0';
    return 1;
}

int String_Replace(String* str, const char* pattern, size_t patternLen, const char* target, size_t targetLen) {
    const char* patternPos = NULL;
    const char* cursor = str->data;
    String* temp = String_CreateNew((size_t)(1.5 * String_Length(str)));
    if (temp == NULL) {
        return 0;
    }

    while (1) {
        patternPos = strstr(cursor, pattern);

        if (patternPos != NULL) {
            if (!String_Append_CStyle(temp, cursor, (size_t)(patternPos - cursor))) {
                String_Destroy(temp);
                return 0;
            }
            
            if (!String_Append_CStyle(temp, target, targetLen)) {
                String_Destroy(temp);
                return 0;
            }

            cursor = patternPos + patternLen;
        }
        else {
            size_t strLeftLen = String_Length(str) - (size_t)(cursor - String_Data(str));
            if (!String_Append_CStyle(temp, cursor, strLeftLen)) {
                String_Destroy(temp);
                return 0;
            }

            free(str->data);
            str->data = temp->data;
            str->length = temp->length;
            str->capacity = temp->capacity;
            return 1;
        }
    }
}

typedef struct ReplaceTable {
    const char* pattern;
    const char* target;
} ReplaceTable;

void read_all_file_content(FILE* f, String* str) {
    #define LOAD_LINE_MAX_LEN 2048

    char line[LOAD_LINE_MAX_LEN];
    size_t len;

    while (!feof(f)) {
        len = fread(line, sizeof(char), LOAD_LINE_MAX_LEN, f);
        String_Append_CStyle(str, line, len);
    }

    #undef LOAD_LINE_MAX_LEN
}

void replace_file_content_then_write_to_file(const char* templateFilePath, const char* targetFilePath, const ReplaceTable* rt, size_t replaceTableLen) {
    size_t i;
    FILE* templateFile = fopen(templateFilePath, "r");
    FILE* targetFile = fopen(targetFilePath, "w");
    String* fileContent = String_CreateNew(1024);

    read_all_file_content(templateFile, fileContent);

    for (i = 0; i < replaceTableLen; ++i) {
        String_Replace(fileContent, rt[i].pattern, strlen(rt[i].pattern), rt[i].target, strlen(rt[i].target));
    }

    fprintf(targetFile, "%s", String_Data(fileContent));

    fclose(templateFile);
    fclose(targetFile);
    String_Destroy(fileContent);
}

void create_array(const char* targetFilePath, const char* elementType, const char* arrayTypeName) {
    const ReplaceTable rt[] = {
        "@ElementType", elementType,
        "@ArrayTypeName", arrayTypeName
    };

    replace_file_content_then_write_to_file("template_array.txt", targetFilePath, rt, sizeof(rt) / sizeof(ReplaceTable));
}

void create_doubly_linked_list(const char* targetFilePath, const char* elementType, const char* listNodeTypeName, const char* listTypeName) {
    const ReplaceTable rt[] = {
        "@ElementType", elementType,
        "@ListNodeTypeName", listNodeTypeName,
        "@ListTypeName", listTypeName
    };

    replace_file_content_then_write_to_file("template_doubly_linked_list.txt", targetFilePath, rt, sizeof(rt) / sizeof(ReplaceTable));
}

void example(void) {
    create_array("array_int.c", "int", "ArrayInt");
    create_array("stack_int.c", "int", "StackInt");  /* stack based on array. */
    create_doubly_linked_list("dlist_int.c", "int", "ListNodeInt", "ListInt");
    create_doubly_linked_list("stack_int.c", "int", "StackNodeInt", "StackInt");  /* stack based on doubly linked list. */
    create_doubly_linked_list("queue_int.c", "int", "QueueNodeInt", "QueueInt");  /* queue based on doubly linked list. */
}

int main() {
    create_doubly_linked_list("dlist_int.c", "int", "ListNodeInt", "ListInt");
    return 0;
}
