#include "vector.h"
#include "result.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>


result vec_init(Vec* vec, unsigned int elSize) {
  void* memory = malloc(elSize * VEC_INITIAL_CAP);
  if (memory == NULL) return r_EMEM;

  vec->memory = memory;
  vec->capacity = VEC_INITIAL_CAP;
  vec->elSize = elSize;
  vec->count = 0;

  return r_ENONE;
}

void vec_free(Vec* vec) {
  free(vec->memory);
  *vec = (Vec){0};
}

result vec_updatr_capacity(Vec* vec) {
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
  result res;

  res = vec_updatr_capacity(vec);
  if (res != r_ENONE) return res;

  memcpy((char*)vec->memory + vec->count * vec->elSize, el, vec->elSize);
  vec->count++;

  return r_ENONE;
}

result vec_pop(Vec* vec, void* el) {
  result res;

  memcpy(el, (char*)vec->memory + (vec->count - 1) * vec->elSize, vec->elSize);
  vec->count--;
  
  res = vec_updatr_capacity(vec);
  if (res != r_ENONE) return res;

  return r_ENONE;
}

void* vec_get(Vec* vec, size_t index) {
  if (index >= vec->count) return NULL;
  return (char*)vec->memory + index * vec->elSize;
}

result vec_clear(Vec* vec) {
  vec->count = 0;
  return vec_updatr_capacity(vec);
}

