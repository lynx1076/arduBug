#include "vector.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>


int vec_init(Vec* vec, unsigned int elSize) {
  void* memory = malloc(elSize * VEC_INITIAL_CAP);
  if (memory == NULL) return -1;

  vec->memory = memory;
  vec->capacity = VEC_INITIAL_CAP;
  vec->elSize = elSize;
  vec->count = 0;

  return 0;
}

void vec_free(Vec* vec) {
  free(vec->memory);
  *vec = (Vec){0};
}

int vec_update_capacity(Vec* vec) {
  size_t new_cap;

  if (vec->count < vec->capacity) {
    if (vec->count > (vec->capacity / 2) - VEC_INITIAL_CAP) return 0; 
    new_cap = vec->capacity / 2;
  } else {
    new_cap = vec->capacity * 2;
  }

  void* oldMem = vec->memory;
  vec->memory = realloc(vec->memory, new_cap * vec->elSize);
  if (vec->memory == NULL) {
    vec->memory = oldMem;
    return -1;
  }

  vec->capacity = new_cap;

  return 0;
}

int vec_push(Vec* vec, void* el) {
  if (vec_update_capacity(vec)) return -1;

  memcpy((char*)vec->memory + vec->count * vec->elSize, el, vec->elSize);
  vec->count++;

  return 0;
}

int vec_pop(Vec* vec, void* el) {
  memcpy(el, (char*)vec->memory + (vec->count - 1) * vec->elSize, vec->elSize);
  vec->count--;
  if (vec_update_capacity(vec)) return -1;

  return 0;
}

void* vec_get(Vec* vec, size_t index) {
  if (index >= vec->count) return NULL;
  return (char*)vec->memory + index * vec->elSize;
}

int vec_clear(Vec* vec) {
  vec->count = 0;
  return vec_update_capacity(vec);
}

