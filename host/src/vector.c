#include "vector.h"
#include "result.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <time.h>


result vec_init(Vec* vec, unsigned int elSize) {
  if (vec_is_init(vec)) return r_EDOUBLE_INIT;
  void* memory = malloc(elSize * VEC_INITIAL_CAP);
  if (memory == NULL) return r_EMEM;

  vec->memory = memory;
  vec->capacity = VEC_INITIAL_CAP;
  vec->elSize = elSize;
  vec->count = 0;

  return r_ENONE;
}

bool vec_is_init(Vec* vec) {
  if (vec == NULL) return false;
  return vec->memory != NULL;
}

void vec_free(Vec* vec) {
  if (!vec_is_init(vec)) return;
  free(vec->memory);
  *vec = VEC_ZERO;
}

result vec_update_capacity(Vec* vec) {
  if (!vec_is_init(vec)) return r_ENOT_INIT;
  size_t new_cap;

  if (vec->count < vec->capacity) {
    if (vec->capacity <= VEC_INITIAL_CAP || vec->count > vec->capacity / 4) {
      return r_ENONE;
    }
    new_cap = vec->capacity / 2;
  } else {
    new_cap = vec->capacity * 2;
  }

  size_t new_size = new_cap * vec->elSize;
  void* new_mem = realloc(vec->memory, new_size);
  if (new_mem == NULL) {
    return r_EMEM;
  }

  vec->memory = new_mem;
  vec->capacity = new_cap;

  return r_ENONE;
}

result vec_push(Vec* vec, void* el) {
  if (!vec_is_init(vec)) return r_ENOT_INIT;
  result res;

  res = vec_update_capacity(vec);
  if (res != r_ENONE) return res;

  memcpy((char*)vec->memory + vec->count * vec->elSize, el, vec->elSize);
  vec->count++;

  return r_ENONE;
}

result vec_pop(Vec* vec, void* el) {
  if (!vec_is_init(vec)) return r_ENOT_INIT;
  result res;

  memcpy(el, (char*)vec->memory + (vec->count - 1) * vec->elSize, vec->elSize);
  vec->count--;
  
  res = vec_update_capacity(vec);
  if (res != r_ENONE) return res;

  return r_ENONE;
}

void* vec_get(Vec* vec, size_t index) {
  if (!vec_is_init(vec)) return NULL;
  if (index >= vec->count) return NULL;
  return (char*)vec->memory + index * vec->elSize;
}

result vec_clear(Vec* vec) {
  if (!vec_is_init(vec)) return r_ENOT_INIT;
  vec->count = 0;
  return vec_update_capacity(vec);
}

