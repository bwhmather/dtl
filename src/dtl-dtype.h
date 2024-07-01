#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum dtl_dtype {
    DTL_DTYPE_BOOL,
    DTL_DTYPE_INT,
    DTL_DTYPE_DOUBLE,
    DTL_DTYPE_TEXT,
    DTL_DTYPE_BYTES,
    DTL_DTYPE_INDEX,
};

bool
dtl_get_bool(void const *data, size_t index);
void
dtl_set_bool(void *data, size_t index, bool value);

uint64_t
dtl_get_int(void const *data, size_t index);
void
dtl_set_int(void *data, size_t index, uint64_t value);

double
dtl_get_double(void const *data, size_t index);
void
dtl_set_double(void *data, size_t index, double value);

char const *
dtl_get_text(void const *data, size_t index);
void
dtl_set_text(void *data, size_t index, char const *value);

// TODO decide byte representation.

size_t
dtl_get_index(void const *data, size_t index);
void
dtl_set_index(void *data, size_t index, size_t value);
