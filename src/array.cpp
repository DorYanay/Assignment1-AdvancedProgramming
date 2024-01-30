#include "array.h"

#include <stdlib.h>

#define ARRAY_MAXZIE 255

int array_create(array* arr)
{
    arr->size = 0;
    arr->data = (char**)calloc(ARRAY_MAXZIE, sizeof(char*));
    if (arr->data == NULL) {
        return 1;
    }
    return 0;
}

int array_size(array* arr)
{
    return arr->size;
}

char** array_data(array* arr)
{
    return arr->data;
}

char* array_pull(array* arr) 
{
    if (arr->size == 0) {
        return NULL;
    }
    
    char* el = arr->data[arr->size - 1];
    
    arr->data[arr->size - 1] = NULL;
    arr->size--;

    return el;
}

void array_push(array* arr, char* value)
{
    arr->data[arr->size] = value;
    arr->size++;
}

char* array_get(array* arr, int index)
{
    if (arr->size < index) {
        return NULL;
    }

    return arr->data[index];
}

void array_set(array* arr, int index, char* value)
{
    if (index < arr->size) {
        arr->data[index] = value;
    } else if (index == arr->size) {
        array_push(arr, value);
    }
}

void array_free(array* arr)
{
    free(arr->data);
}
