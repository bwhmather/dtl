#include "dtl-int64-array.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

int64_t *
dtl_int64_array_create(size_t size) {
    return calloc(size, sizeof(int64_t));
}

void
dtl_int64_array_destroy(int64_t *array, size_t size) {
    (void)size;
    free(array);
}

void
dtl_int64_array_set(int64_t *array, size_t index, int64_t value) {
    assert(array != NULL);
    array[index] = value;
}

int64_t
dtl_int64_array_get(int64_t *array, size_t index) {
    assert(array != NULL);
    return array[index];
}

int64_t *
dtl_int64_array_copy(int64_t *array, size_t size) {
    int64_t *dest;
    assert(array != NULL);
    dest = calloc(size, sizeof(int64_t));
    memcpy(dest, array, size * sizeof(int64_t));
    return dest;
}
