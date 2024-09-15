#pragma once

#include <stddef.h>

size_t *
dtl_index_array_create(size_t size);

void
dtl_index_array_destroy(size_t *array, size_t size);

void
dtl_index_array_set(size_t *array, size_t index, size_t value);

size_t
dtl_index_array_get(size_t *array, size_t index);
