#include <stdio.h>
#include <string.h>

#include "ulib.h"

static inline void u_memcpy(void *dest, void *src, int length) {
  char *cdest = (char *) dest;
  char *csrc = (char *) src;

  for (int i = 0; i < length; i++) {
    *(cdest + i) = *(csrc + i);
  }
}

u_array_t *u_array_new(size_t object_size) {
  u_array_t *arr = malloc(sizeof(u_array_t));

  arr->object_size = object_size;
  arr->capacity = MIN_ARRAY_CAPACITY;
  arr->buffer = malloc(arr->object_size * arr->capacity);
  arr->length = 0;

  return arr;
}

void u_array_free(u_array_t *arr) {
  free(arr->buffer);
  free(arr);
}

void *_u_array_get(u_array_t *arr, int i) {
  return arr->buffer + (arr->object_size * i);
}

int u_array_length(u_array_t *arr) {
  return arr->length;
}

void *_u_array_push(u_array_t *arr) {
  arr->length += 1;
  if (arr->length > arr->capacity) {
    arr->capacity *= 2;
    arr->buffer = realloc(arr->buffer, arr->object_size * arr->capacity);
  }
  return _u_array_get(arr, arr->length - 1);
}

void u_array_pop(u_array_t *arr) {
  arr->length -= 1;
  if (arr->capacity > MIN_ARRAY_CAPACITY && arr->length < (arr->capacity / 2)) {
    arr->capacity /= 2;
    arr->buffer = realloc(arr->buffer, arr->object_size * arr->capacity);
  }
}

void u_array_remove(u_array_t *arr, int i) {
  if (i < arr->length - 1) {
    u_memcpy(_u_array_get(arr, i), _u_array_get(arr, i + 1), arr->object_size * (arr->length - 1 - i));
  }
  u_array_pop(arr);
}

void u_array_fast_remove(u_array_t *arr, int i) {
  if (i < arr->length - 1) {
    u_memcpy(_u_array_get(arr, i), _u_array_get(arr, arr->length - 1), arr->object_size);
  }
  u_array_pop(arr);
}

