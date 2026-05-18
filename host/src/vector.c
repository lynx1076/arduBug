#include "vector.h"
#include "result.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <time.h>


int vec_init(Vec* vec, unsigned int elSize) {
  if (vec_is_init(vec)) RES_RETURN(r_EDOUBLE_INIT, -1);
  void* memory = malloc(elSize * VEC_INITIAL_CAP);
  if (memory == NULL) return r_EMEM;

  vec->memory = memory;
  vec->capacity = VEC_INITIAL_CAP;
  vec->elSize = elSize;
  vec->count = 0;

  RES_RETURN(r_ENONE, 0);
}

bool vec_is_init(Vec* vec) {
  if (vec == NULL) return false;
  return vec->memory != NULL;
}

void vec_free(Vec* vec) {
  free(vec->memory);
  *vec = VEC_EMPTY;
}

int vec_update_capacity(Vec* vec) {
  if (!vec_is_init(vec)) RES_RETURN(r_ENOT_INIT, -1);
  size_t new_cap;

  if (vec->count >= vec->capacity) {
    new_cap = vec->capacity * 2;
  } else {
    RES_RETURN(r_ENONE, 0);
  }

  size_t new_size = new_cap * vec->elSize;
  void* new_mem = realloc(vec->memory, new_size);
  if (new_mem == NULL) {
    RES_RETURN(r_EMEM, -1);
  }

  vec->memory = new_mem;
  vec->capacity = new_cap;

  RES_RETURN(r_ENONE, 0);
}

int vec_push(Vec* vec, void* el) {
  if (!vec_is_init(vec)) RES_RETURN(r_ENOT_INIT, -1);

  if (vec_update_capacity(vec)) return -1;

  memcpy((char*)vec->memory + vec->count * vec->elSize, el, vec->elSize);
  vec->count++;

  RES_RETURN(r_ENONE, 0);
}

int vec_pop(Vec* vec, void* el) {
  if (!vec_is_init(vec)) RES_RETURN(r_ENOT_INIT, -1);

  memcpy(el, (char*)vec->memory + (vec->count - 1) * vec->elSize, vec->elSize);
  vec->count--;
 
  RES_RETURN(r_ENONE, 0);
}

void* vec_get(Vec* vec, size_t index) {
  if (!vec_is_init(vec)) RES_RETURN(r_ENOT_INIT, NULL);
  if (index >= vec->count) RES_RETURN(r_EBOUNDS, NULL);
  RES_RETURN(r_ENONE, (char*)vec->memory + index * vec->elSize);
}

int vec_clear(Vec* vec) {
  if (!vec_is_init(vec)) RES_RETURN(r_ENOT_INIT, -1);
  vec->count = 0;
  if (vec_update_capacity(vec)) return -1;

  RES_RETURN(r_ENONE, 0);
}

