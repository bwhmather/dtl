#include "dtl-bool-array.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void *
dtl_bool_array_create(size_t size) {
    size_t num_chunks = ((size + 63) / 64);
    return calloc(num_chunks, sizeof(uint64_t));
}

void
dtl_bool_array_destroy(void *array, size_t size) {
    (void)size;

    free(array);
}

void
dtl_bool_array_set(void *array, size_t index, bool value) {
    uint64_t *chunks = array;
    if (value) {
        chunks[index / 64] |= ((uint64_t)1) << (index % 64);
    } else {
        chunks[index / 64] &= ~(((uint64_t)1) << (index % 64));
    }
}

bool
dtl_bool_array_get(void *array, size_t index) {
    uint64_t *chunks = array;
    return chunks[index / 64] & (((uint64_t)1) << (index % 64));
}
