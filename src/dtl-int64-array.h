#pragma once

#include <stddef.h>
#include <stdint.h>

int64_t *
dtl_int64_array_create(size_t size);

void
dtl_int64_array_destroy(int64_t *array, size_t size);

void
dtl_int64_array_set(int64_t *array, size_t index, int64_t value);

int64_t
dtl_int64_array_get(int64_t *array, size_t index);
