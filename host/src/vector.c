#include "vector.h"
#include "result.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


int vec_init(Vec* vec, unsigned int elSize) {
  if (vec == NULL) RES_RETURN(r_ENULL_PTR, -1);

  void* memory = malloc(elSize * VEC_INITIAL_CAP);
  if (memory == NULL) return r_EMEM;

  vec->memory = memory;
  vec->capacity = VEC_INITIAL_CAP;
  vec->elSize = elSize;
  vec->count = 0;

  RES_RETURN(r_ENONE, 0);
}

void vec_free(Vec* vec) {
  if (vec == NULL) return;
  
  free(vec->memory);
  *vec = VEC_EMPTY;
}

static int vec_update_capacity(Vec* vec) {
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
  if (vec == NULL) RES_RETURN(r_ENULL_PTR, -1);
  if (el == NULL) RES_RETURN(r_ENULL_PTR, -1);

  if (vec_update_capacity(vec)) return -1;

  memcpy((char*)vec->memory + vec->count * vec->elSize, el, vec->elSize);
  vec->count++;

  RES_RETURN(r_ENONE, 0);
}

int vec_push_to(Vec* vec, size_t index, void* el) {
  if (vec == NULL) RES_RETURN(r_ENULL_PTR, -1);
  if (el == NULL) RES_RETURN(r_ENULL_PTR, -1);
  if (index > vec->count) RES_RETURN(r_EBOUNDS, -1);

  if (vec_update_capacity(vec)) return -1;

  char* base = (char*)vec->memory;

  if (index < vec->count) memmove(base + (index + 1) * vec->elSize, base + index * vec->elSize, (vec->count - index) * vec->elSize);
  memcpy((char*)vec->memory + index * vec->elSize, el, vec->elSize);
  vec->count++;

  RES_RETURN(r_ENONE, 0);
}

int vec_pop(Vec* vec, void* el) {
  if (vec == NULL) RES_RETURN(r_ENULL_PTR, -1);

  if (el) memcpy(el, (char*)vec->memory + (vec->count - 1) * vec->elSize, vec->elSize);
  vec->count--;
 
  RES_RETURN(r_ENONE, 0);
}

int vec_pop_at(Vec* vec, size_t index, void* el) {
  if (vec == NULL) RES_RETURN(r_ENULL_PTR, -1);
  if (index >= vec->count) RES_RETURN(r_EBOUNDS, -1);

  char* base = (char*)vec->memory;

  if (el) memcpy(el, base + index * vec->elSize, vec->elSize);
  if (index < vec->count - 1) memmove(base + index * vec->elSize, (char*)vec->memory + (index + 1) * vec->elSize, (vec->count - index - 1) * vec->elSize);
  vec->count--;
 
  RES_RETURN(r_ENONE, 0);
}

void* vec_get(Vec* vec, size_t index) {
  if (vec == NULL) RES_RETURN(r_ENULL_PTR, NULL);

  if (index >= vec->count) RES_RETURN(r_EBOUNDS, NULL);
  RES_RETURN(r_ENONE, (char*)vec->memory + index * vec->elSize);
}

int vec_clear(Vec* vec) {
  if (vec == NULL) RES_RETURN(r_ENULL_PTR, -1);

  vec->count = 0;
  if (vec_update_capacity(vec)) return -1;

  RES_RETURN(r_ENONE, 0);
}

