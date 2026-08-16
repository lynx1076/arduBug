#ifndef VECTOR_H
#define VECTOR_H

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>

#define VEC_INITIAL_CAP       8

typedef struct {
  void* memory;
  size_t count;
  size_t capacity;
  unsigned int elSize;
} Vec;

#define VEC_EMPTY      (Vec){NULL, 0, 0, 0}

/**
 * Initalize a vector object
 * Returns 0 on success, -1 on failure
 */
int vec_init(Vec* vec, unsigned int elSize);

/**
 * Check if vector object is initialized
 * Only works if vector is defined as VEC_EMPTY
 */
bool vec_is_init(Vec* vec);

/**
 * Free the vector object
 */
void vec_free(Vec* vec);

/**
 * Add the element onto the back of the vector
 */
int vec_push(Vec* vec, void* el);

/**
 * Add the element at the index and shift all other data
 */
int vec_push_to(Vec* vec, size_t index, void* el);

/**
 * Remove the element at the back of the vector
 */
int vec_pop(Vec* vec, void* el);

/**
 * Remove the element at the index and shift all other data
 */
int vec_pop_at(Vec* vec, size_t index, void* el);

/**
 * Get a pointer to the element at the index
 * Returns NULL if the index is invalid
 */
void* vec_get(Vec* vec, size_t index);

/**
 * Sets the count to 0
 * The capacity updating funciton is called afterwards
 */
int vec_clear(Vec* vec);

#endif
