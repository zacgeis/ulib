#ifndef ULIB_H_
#define ULIB_H_

#include <stdlib.h>

// ARRAY

#define MIN_ARRAY_CAPACITY 8

typedef struct u_array_t u_array_t;
struct u_array_t {
  char *buffer; // use char * instead of void * for assignment
  int capacity;
  int length;
  size_t object_size;
};

#define u_array_new(type) _u_array_new(sizeof(type))
u_array_t *_u_array_new(size_t object_size);
void u_array_free(u_array_t *arr);
#define u_array_get_v(type, arr, i) (* ((type *) _u_array_get(arr, i)))
#define u_array_get_p(type, arr, i) ((type *) _u_array_get(arr, i))
void *_u_array_get(u_array_t *arr, int i);
#define u_array_push(type, arr, val) (* ((type *) _u_array_push(arr))) = val;
void *_u_array_push(u_array_t *arr);
void u_array_pop(u_array_t *arr);
void u_array_remove(u_array_t *arr, int i);
void u_array_fast_remove(u_array_t *arr, int i);
int u_array_length(u_array_t *arr);
void u_array_clear(u_array_t *arr);

// QUEUE

typedef struct u_queue_t u_queue_t;
struct u_queue_t {
  u_array_t *inbox;
  u_array_t *outbox;
  int object_size;
};

#define u_queue_new(type) _u_queue_new(sizeof(type))
u_queue_t *_u_queue_new(size_t object_size);
void u_queue_free(u_queue_t *queue);
int u_queue_length(u_queue_t *queue);
#define u_queue_add(type, queue, val) (* ((type *) _u_array_push(queue->inbox))) = val;
void *_u_queue_peek(u_queue_t *queue);
#define u_queue_peek_v(type, arr) (* ((type *) _u_queue_peek(queue)))
#define u_queue_peek_p(type, arr) ((type *) _u_queue_peek(queue))
void u_queue_remove(u_queue_t *queue);

// LIST

typedef struct u_list_node_t u_list_node_t;
struct u_list_node_t {
  u_list_node_t *prev;
  u_list_node_t *next;
  void *val;
};

typedef struct u_list_t u_list_t;
struct u_list_t {
  u_list_node_t *head;
  u_list_node_t *tail;
  int length;
};

#define u_list_new(type) _u_list_new(sizeof(type))
u_list_t *_u_list_new(size_t object_size);
void u_list_free(u_list_t *list);
#define u_list_prepend(type, list, val) (* (type *) _u_list_prepend(list)) = val
void* _u_list_prepend(u_list_t *list);
#define u_list_append(type, list, val) (* (type *) _u_list_append(list)) = val
void* _u_list_append(u_list_t *list);
void u_list_remove(u_list_t *list, u_list_node_t *node);
int u_list_length(u_list_t *list);
u_list_node_t *u_list_head(u_list_t *list);
u_list_node_t *u_list_tail(u_list_t *list);

u_list_node_t *u_list_node_new(size_t object_size);
void u_list_node_free(u_list_node_t *list_node);
u_list_node_t *u_list_node_next(u_list_node_t *cur);
u_list_node_t *u_list_node_prev(u_list_node_t *cur);
#define u_list_node_value_v(type, node) (* (type *) _u_list_node_value(node))
#define u_list_node_value_p(type, node) (type *) _u_list_node_value(node)
void *_u_list_node_value(u_list_node_t *node);

// HASHTABLE

#endif
