#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "ulib.h"

void array_basic_test() {
  u_array_t *arr = u_array_new(sizeof(int));

  u_array_push(int, arr, 1);
  u_array_push(int, arr, 2);

  assert(u_array_get(int, arr, 0) == 1);
  assert(u_array_get(int, arr, 1) == 2);

  for (int i = 0; i < 100; i++) {
    u_array_push(int, arr, i);
  }

  assert(u_array_get(int, arr, 101) == 99);
  assert(u_array_length(arr) == 102);

  u_array_free(arr);
}

void array_pointer_test() {
  char *s1 = "value 1";
  char *s2 = "value 2";

  u_array_t *arr = u_array_new(sizeof(char *));

  u_array_push(char *, arr, s1);
  u_array_push(char *, arr, s2);

  assert(strcmp(u_array_get(char *, arr, 0), "value 1") == 0);
  assert(strcmp(u_array_get(char *, arr, 1), "value 2") == 0);

  u_array_free(arr);
}

typedef struct struct_test_t struct_test_t;
struct struct_test_t {
  int x;
  int y;
};

void array_struct_test_stack(u_array_t *arr) {
  struct_test_t s3 = { .x = 5, .y = 6 };
  u_array_push(struct_test_t, arr, s3);
}

void array_struct_test() {
  struct_test_t s1 = { .x = 1, .y = 2 };
  struct_test_t s2 = { .x = 3, .y = 4 };

  u_array_t *arr = u_array_new(sizeof(struct_test_t));

  u_array_push(struct_test_t, arr, s1);
  u_array_push(struct_test_t, arr, s2);

  assert(u_array_get(struct_test_t, arr, 0).x == 1);
  assert(u_array_get(struct_test_t, arr, 0).y == 2);
  assert(u_array_get(struct_test_t, arr, 1).x == 3);
  assert(u_array_get(struct_test_t, arr, 1).y == 4);

  array_struct_test_stack(arr);

  assert(u_array_get(struct_test_t, arr, 2).x == 5);
  assert(u_array_get(struct_test_t, arr, 2).y == 6);

  // Ensure that copying happens
  struct_test_t remove_test = u_array_get(struct_test_t, arr, 0);
  u_array_remove(arr, 0);
  assert(remove_test.x == 1);

  u_array_free(arr);
}

void array_remove_test() {
  u_array_t *arr = u_array_new();
  u_array_push(int, arr, 1);
  u_array_push(int, arr, 2);
  u_array_push(int, arr, 3);
  u_array_push(int, arr, 4);
  u_array_push(int, arr, 5);

  u_array_remove(arr, 0);

  assert(u_array_get(int, arr, 0) == 2);
  assert(u_array_get(int, arr, 3) == 5);
  assert(u_array_length(arr) == 4);

  u_array_fast_remove(arr, 0);

  assert(u_array_get(int, arr, 0) == 5);
  assert(u_array_get(int, arr, 2) == 4);
  assert(u_array_length(arr) == 3);
}

#define run_test(name)\
  printf("  %s\n", #name);\
  name();

int main(int argc, char **argv) {
  printf("Running tests...\n");

  run_test(array_basic_test);
  run_test(array_pointer_test);
  run_test(array_struct_test);
  run_test(array_remove_test);

  printf("All tests passed...\n");

  return 0;
}