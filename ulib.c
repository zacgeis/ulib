#include <stdio.h>
#include <string.h>

#include "ulib.h"

// UTILS

static inline void u_memcpy(void *dest, void *src, int length) {
  char *cdest = (char *) dest;
  char *csrc = (char *) src;

  for (int i = 0; i < length; i++) {
    *(cdest + i) = *(csrc + i);
  }
}

// ARRAY

u_array_t *_u_array_new(size_t object_size) {
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

void u_array_clear(u_array_t *arr) {
  arr->length = 0;
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

int u_queue_length(u_queue_t *queue) {
  return u_array_length(queue->inbox) + u_array_length(queue->outbox);
}

static void _u_queue_shift_inbox(u_queue_t *queue) {
    int inbox_length = u_array_length(queue->inbox);
    int outbox_i = 0;
    for (int inbox_i = inbox_length - 1; inbox_i >= 0; inbox_i--) {
      // Needed for capacity and length setting
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
  if (u_array_length(queue->outbox) == 0) {
    _u_queue_shift_inbox(queue);
  }
  return _u_array_get(queue->outbox, u_array_length(queue->outbox) - 1);
}

void u_queue_remove(u_queue_t *queue) {
  if (u_array_length(queue->outbox) == 0) {
    _u_queue_shift_inbox(queue);
  } else {
    u_array_pop(queue->outbox);
  }
}

// LIST

u_list_t *_u_list_new(size_t object_size) {
  u_list_t *list = malloc(sizeof(u_list_t));

  list->object_size = object_size;
  list->length = 0;
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

  list->length++;

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

  list->length++;

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

  list->length--;
}

int u_list_length(u_list_t *list) {
  return list->length;
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
