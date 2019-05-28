#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "list.h"

#define INC_FACTOR 2

struct list {
    void **data;
    size_t size;
    size_t capacity;
    FreeFunc free;
};

List *list_init(size_t initial_size, FreeFunc freer) {
    List *res = malloc(sizeof(List));
    assert(res != NULL);
    initial_size = initial_size == 0 ? 1 : initial_size;
    res->data = calloc(initial_size, sizeof(void *));
    res->size = 0;
    res->capacity = initial_size;
    res->free = freer;
    return res;
}

void list_free(List *list) {
    assert (list != NULL);

    if (list->free != NULL) {
        for (size_t i = 0; i < list->size; i++) {
            list->free(list->data[i]);
        }
    }

    free(list->data);
    free(list);
}

size_t list_size(List *list) {
    assert (list != NULL);

    return list->size;
}

void *list_get(List *list, size_t index) {
    assert(list != NULL && index < list->size);

    return list->data[index];
}

void *list_remove(List *list, size_t index) {
    assert(list != NULL && index < list->size);

    size_t size = list_size(list);
    void *res = list->data[index];
    void **tmp = calloc(size - index - 1, sizeof(void *));
    memcpy(tmp, list->data + index + 1, (size - index - 1) * sizeof(void *));
    memcpy(list->data + index, tmp, (size - index - 1) * sizeof(void *));
    list->size--;
    free(tmp);
    return res;
}

void list_add(List *list, void *value) {
    assert(list != NULL && value != NULL);
    size_t size = list_size(list);

    if (size == list->capacity) {
        list->data = realloc(list->data, INC_FACTOR * sizeof(void *) * size);
        assert(list->data != NULL);
        list->capacity *= INC_FACTOR;
    }

    list->data[list->size++] = value;
}
