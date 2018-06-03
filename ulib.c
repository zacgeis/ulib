#include <stdio.h>
#include <string.h>

#include "ulib.h"

// UTILS

static inline void u_memcpy(void *dest, void *src, int count) {
  char *cdest = (char *) dest;
  char *csrc = (char *) src;

  for (int i = 0; i < count; i++) {
    *(cdest + i) = *(csrc + i);
  }
}

// ARRAY

u_array_t *_u_array_new(size_t object_size) {
  u_array_t *arr = malloc(sizeof(u_array_t));

  arr->object_size = object_size;
  arr->capacity = MIN_ARRAY_CAPACITY;
  arr->buffer = malloc(arr->object_size * arr->capacity);
  arr->count = 0;

  return arr;
}

void u_array_free(u_array_t *arr) {
  free(arr->buffer);
  free(arr);
}

void *_u_array_get(u_array_t *arr, int i) {
  return arr->buffer + (arr->object_size * i);
}

int u_array_count(u_array_t *arr) {
  return arr->count;
}

void *_u_array_push(u_array_t *arr) {
  arr->count += 1;
  if (arr->count > arr->capacity) {
    arr->capacity *= 2;
    arr->buffer = realloc(arr->buffer, arr->object_size * arr->capacity);
  }
  return _u_array_get(arr, arr->count - 1);
}

void u_array_pop(u_array_t *arr) {
  arr->count -= 1;
  if (arr->capacity > MIN_ARRAY_CAPACITY && arr->count < (arr->capacity / 2)) {
    arr->capacity /= 2;
    arr->buffer = realloc(arr->buffer, arr->object_size * arr->capacity);
  }
}

void u_array_remove(u_array_t *arr, int i) {
  if (i < arr->count - 1) {
    u_memcpy(_u_array_get(arr, i), _u_array_get(arr, i + 1), arr->object_size * (arr->count - 1 - i));
  }
  u_array_pop(arr);
}

void u_array_fast_remove(u_array_t *arr, int i) {
  if (i < arr->count - 1) {
    u_memcpy(_u_array_get(arr, i), _u_array_get(arr, arr->count - 1), arr->object_size);
  }
  u_array_pop(arr);
}

void u_array_clear(u_array_t *arr) {
  arr->count = 0;
  arr->capacity = MIN_ARRAY_CAPACITY;
  arr->buffer = realloc(arr->buffer, arr->object_size * arr->capacity);
}

// QUEUE

u_queue_t *_u_queue_new(size_t object_size) {
  u_queue_t *queue = malloc(sizeof(u_queue_t));

  queue->object_size = object_size;
  queue->inbox = _u_array_new(object_size);
  queue->outbox = _u_array_new(object_size);

  return queue;
}

void u_queue_free(u_queue_t *queue) {
  u_array_free(queue->inbox);
  u_array_free(queue->outbox);

  free(queue);
}

int u_queue_count(u_queue_t *queue) {
  return u_array_count(queue->inbox) + u_array_count(queue->outbox);
}

static void _u_queue_shift_inbox(u_queue_t *queue) {
    int inbox_count = u_array_count(queue->inbox);
    int outbox_i = 0;
    for (int inbox_i = inbox_count - 1; inbox_i >= 0; inbox_i--) {
      // Needed for capacity and count setting
      // The value is then set by memcpy
      _u_array_push(queue->outbox);
      void *dest = _u_array_get(queue->outbox, outbox_i);
      void *src = _u_array_get(queue->inbox, inbox_i);
      u_memcpy(dest, src, queue->object_size);
      outbox_i++;
    }
    u_array_clear(queue->inbox);
}

void *_u_queue_peek(u_queue_t *queue) {
  if (u_array_count(queue->outbox) == 0) {
    _u_queue_shift_inbox(queue);
  }
  return _u_array_get(queue->outbox, u_array_count(queue->outbox) - 1);
}

void u_queue_remove(u_queue_t *queue) {
  if (u_array_count(queue->outbox) == 0) {
    _u_queue_shift_inbox(queue);
  } else {
    u_array_pop(queue->outbox);
  }
}

// LIST

u_list_t *_u_list_new(size_t object_size) {
  u_list_t *list = malloc(sizeof(u_list_t));

  list->object_size = object_size;
  list->count = 0;
  list->head = NULL;
  list->tail = NULL;

  return list;
}

void u_list_free(u_list_t *list) {
  u_list_node_t *current = list->head;
  while (current != NULL) {
    u_list_node_t *previous = current;
    current = current->next;
    u_list_node_free(previous);
  }
  free(list);
}

void* _u_list_prepend(u_list_t *list) {
  u_list_node_t *new_node = u_list_node_new(list->object_size);

  if (list->head == NULL && list->tail == NULL) {
    list->head = new_node;
    list->tail = new_node;
  } else {
    list->head->prev = new_node;
    new_node->next = list->head;
    list->head = new_node;
  }

  list->count++;

  return new_node->val;
}

void* _u_list_append(u_list_t *list) {
  u_list_node_t *new_node = u_list_node_new(list->object_size);

  if (list->head == NULL && list->tail == NULL) {
    list->head = new_node;
    list->tail = new_node;
  } else {
    list->tail->next = new_node;
    new_node->prev = list->tail;
    list->tail = new_node;
  }

  list->count++;

  return new_node->val;
}

void u_list_remove(u_list_t *list, u_list_node_t *node) {
  if (list->head == node && list->tail == node) {
    list->head = NULL;
    list->tail = NULL;
  } else if (list->head == node) {
    list->head = list->head->next;
    list->head->prev = NULL;
  } else if (list->tail == node) {
    list->tail = list->tail->prev;
    list->tail->next = NULL;
  } else {
    node->next->prev = node->prev;
    node->prev->next = node->next;
  }

  u_list_node_free(node);

  list->count--;
}

int u_list_count(u_list_t *list) {
  return list->count;
}

u_list_node_t *u_list_head(u_list_t *list) {
  return list->head;
}

u_list_node_t *u_list_tail(u_list_t *list) {
  return list->tail;
}


u_list_node_t *u_list_node_new(size_t object_size) {
  u_list_node_t *node = malloc(sizeof(u_list_node_t));

  node->val = malloc(sizeof(object_size));
  node->prev = NULL;
  node->next = NULL;

  return node;
}

void u_list_node_free(u_list_node_t *node) {
  free(node->val);
  free(node);
}

u_list_node_t *u_list_node_next(u_list_node_t *cur) {
  return cur->next;
}

u_list_node_t *u_list_node_prev(u_list_node_t *cur) {
  return cur->prev;
}

void *_u_list_node_value(u_list_node_t *node) {
  return node->val;
}

// HASH TABLE

u_hash_table_t *_u_hash_table_new(size_t object_size) {
  u_hash_table_t *table = malloc(sizeof(u_hash_table_t));

  table->_bucket_count = 4;
  table->buckets = malloc(sizeof(u_list_t *) * table->_bucket_count);
  for (int i = 0; i < table->_bucket_count; i++) {
    *(table->buckets + i) = u_list_new(u_hash_table_entry_t *);
  }
  table->count = 0;
  table->object_size = object_size;

  return table;
}

void _u_hash_table_free_bucket(u_list_t *bucket) {
  u_list_node_t *current = bucket->head;
  while (current != NULL) {
    u_list_node_t *previous = current;
    current = current->next;
    free(u_list_node_value_v(u_hash_table_entry_t *, previous));
  }
  u_list_free(bucket);
}

void u_hash_table_free(u_hash_table_t *table) {
  for (int i = 0; i < table->_bucket_count; i++) {
    _u_hash_table_free_bucket(*(table->buckets + i));
  }
  free(table->buckets);
  free(table);
}

void *_u_hash_table_put(u_hash_table_t *table, char *key) {

}

void *_u_hash_table_get(u_hash_table_t *table, char *key) {

}

void u_hash_table_remove(u_hash_table_t *table, char *key) {

}

int u_hash_table_count(u_hash_table_t *table) {
  return table->count;
}
