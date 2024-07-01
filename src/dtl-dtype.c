#include "dtl-dtype.h"

bool
dtl_get_bool(void const *data, size_t index) {
    uint8_t const *bytes = data;
    return bytes[index >> 3] & (1 << (index & 7));
}

void
dtl_set_bool(void *data, size_t index, bool value) {
    uint8_t *bytes = data;
    if (value) {
        bytes[index >> 3] |= (1 << (index & 7));
    } else {
        bytes[index >> 3] &= ~(1 << (index & 7));
    }
}

uint64_t
dtl_get_int(void const *data, size_t index) {
    uint64_t const *values = data;
    return values[index];
}

void
dtl_set_int(void *data, size_t index, uint64_t value) {
    uint64_t *values = data;
    values[index] = value;
}

double
dtl_get_double(void const *data, size_t index) {
    double const *values = data;
    return values[index];
}

void
dtl_set_double(void *data, size_t index, double value) {
    double *values = data;
    values[index] = value;
}

char const *
dtl_get_text(void const *data, size_t index) {
    char const *const *values = data;
    return values[index];
}

void
dtl_set_text(void *data, size_t index, char const *value) {
    char const **values = data;
    values[index] = value;
}

// TODO decide byte representation.

size_t
dtl_get_index(void const *data, size_t index) {
    size_t const *values = data;
    return values[index];
}

void
dtl_set_index(void *data, size_t index, size_t value) {
    size_t *values = data;
    values[index] = value;
}
