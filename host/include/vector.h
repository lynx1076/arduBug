#ifndef VECTOR_H
#define VECTOR_H

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


/**
 * Initalize a vector object
 * Returns 0 on success, -1 on failure
 */
int vec_init(Vec* vec, unsigned int elSize);

/**
 * Free the vector object
 */
void vec_free(Vec* vec);

/**
 * Double the capacity of the vector if insufficient
 * Half the capacity if the element count is less than half the capacity
 * Returns 0 on success, -1 on failure
 */
int vec_update_capacity(Vec* vec);

/**
 * Add the element onto the back of the vector
 * If the capacity is insufficient to support another element, the capacity updating funciton is called
 * Returns 0 on success, -1 on failure
 */
int vec_push(Vec* vec, void* el);

/**
 * Remove the element at the back of the vector
 * The capacity updating funciton is called afterwards
 * Returns 0 on success, -1 on failure
 */
int vec_pop(Vec* vec, void* el);

/**
 * Get a pointer to the element at the index
 * Returns NULL if the index is invalid
 */
void* vec_get(Vec* vec, size_t index);

/**
 * Sets the count to 0
 * The capacity updating funciton is called afterwards
 * Returns 0 on success, -1 on failure
 */
int vec_clear(Vec* vec);

#endif
