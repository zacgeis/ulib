#ifndef ULIB_H_
#define ULIB_H_

#include <stdlib.h>

#define MIN_ARRAY_CAPACITY 8

typedef struct u_array_t u_array_t;
struct u_array_t {
  char *buffer; // use char * instead of void * for assignment
  int capacity;
  int length;
  size_t object_size;
};

u_array_t *u_array_new();
void u_array_free(u_array_t *arr);
#define u_array_get(type, arr, i) (* ((type *) _u_array_get(arr, i)))
void *_u_array_get(u_array_t *arr, int i);
#define u_array_push(type, arr, val) (* ((type *) _u_array_push(arr))) = val;
void *_u_array_push(u_array_t *arr);
void u_array_pop(u_array_t *arr);
void u_array_remove(u_array_t *arr, int i);
void u_array_fast_remove(u_array_t *arr, int i);
int u_array_length(u_array_t *arr);

typedef struct u_queue_t u_queue_t;
struct u_queue_t {
  u_array_t *inbox;
  u_array_t *outbox;
};

u_queue_t *u_queue_new();
void u_queue_free(u_queue_t *queue);
int u_queue_length(u_queue_t *queue);
void *_u_queue_peek(u_queue_t *queue);
void u_queue_remove(u_queue_t *queue);

#endif
