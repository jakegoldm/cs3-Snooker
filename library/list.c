#include "list.h"
#include <assert.h>
#include <math.h>
#include <stdlib.h>

typedef struct list {
  size_t capacity;
  size_t length;
  void **array;
  void (*freer)(void *);
} list_t;

list_t *list_init(size_t initial_size, free_func_t freer) {
  initial_size = initial_size ? initial_size : 1;
  list_t *list = malloc(sizeof(list_t));
  assert(list != NULL);
  list->array = malloc(initial_size * sizeof(void *));
  assert(list->array != NULL);
  list->length = 0;
  list->capacity = initial_size;
  list->freer = freer;
  return list;
}

void list_free(list_t *list) {
  if (list->freer != NULL) {
    for (size_t i = 0; i < list->length; i++) {
      list->freer(list->array[i]);
    }
  }
  free(list->array);
  free(list);
}

void resize(list_t *list) {
  if (list->length >= list->capacity) {
    list->array = realloc(list->array, 2 * list->capacity * sizeof(void *));
    list->capacity *= 2;
  }
}

size_t list_size(list_t *list) { return list->length; }

void *list_get(list_t *list, int index) {
  assert(abs(index) < (int)list->length);
  if (index < 0) {
    index = (int)list->length + index;
  }
  return list->array[index];
}

void list_add(list_t *list, void *value) {
  resize(list);
  assert(list->length < list->capacity && value != NULL);
  list->array[list->length] = value;
  list->length++;
}

void list_add_idx(list_t *list, void *value, size_t index) {
  assert(list->length < list->capacity && index <= list->length &&
         value != NULL);
  for (int i = list->length - 1; i >= index; i--) {
    list->array[i + 1] = list->array[i];
  }
  list->array[index] = value;
  list->length++;
}

void *list_remove(list_t *list, size_t index) {
  assert(list->length && index < list->length);
  void *v = list->array[index];
  for (size_t i = index; i < list->length - 1; i++) {
    list->array[i] = list->array[i + 1];
  }
  list->array[list->length - 1] = NULL;
  list->length--;
  return v;
}