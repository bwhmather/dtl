#include "dtl-array.h"

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bool
dtl_array_get_bool(void const *data, size_t index) {
    uint8_t const *bytes = data;
    return bytes[index >> 3] & (1 << (index & 7));
}

void
dtl_array_set_bool(void *data, size_t index, bool value) {
    uint8_t *bytes = data;
    if (value) {
        bytes[index >> 3] |= (1 << (index & 7));
    } else {
        bytes[index >> 3] &= ~(1 << (index & 7));
    }
}

int64_t
dtl_array_get_int64(void const *data, size_t index) {
    int64_t const *values = data;
    return values[index];
}

void
dtl_array_set_int64(void *data, size_t index, int64_t value) {
    int64_t *values = data;
    values[index] = value;
}

double
dtl_array_get_double(void const *data, size_t index) {
    double const *values = data;
    return values[index];
}

void
dtl_array_set_double(void *data, size_t index, double value) {
    double *values = data;
    values[index] = value;
}

char const *
dtl_array_get_string(void const *data, size_t index) {
    char const *const *values = data;
    return values[index];
}

void
dtl_array_set_string(void *data, size_t index, char const *value) {
    char const **values = data;
    values[index] = value;
}

// TODO decide byte representation.

size_t
dtl_array_get_index(void const *data, size_t index) {
    size_t const *values = data;
    return values[index];
}

void
dtl_array_set_index(void *data, size_t index, size_t value) {
    size_t *values = data;
    values[index] = value;
}
