#include "dtl-double-array.h"

#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

double *
dtl_double_array_create(size_t size) {
    return calloc(size, sizeof(double));
}

void
dtl_double_array_destroy(double *array, size_t size) {
    (void)size;
    free(array);
}

void
dtl_double_array_set(double *array, size_t index, double value) {
    assert(array != NULL);
    array[index] = value;
}

double
dtl_double_array_get(double *array, size_t index) {
    assert(array != NULL);
    return array[index];
}
