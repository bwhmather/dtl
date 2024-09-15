#include "dtl-index-array.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

size_t *
dtl_index_array_create(size_t size) {
    return calloc(size, sizeof(size_t));
}

void
dtl_index_array_destroy(size_t *array, size_t size) {
    (void)size;
    free(array);
}

void
dtl_index_array_set(size_t *array, size_t index, size_t value) {
    assert(array != NULL);
    array[index] = value;
}

size_t
dtl_index_array_get(size_t *array, size_t index) {
    assert(array != NULL);
    return array[index];
}
