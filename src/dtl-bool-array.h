#pragma once

#include <stdbool.h>
#include <stddef.h>

void *
dtl_bool_array_create(size_t size);

void
dtl_bool_array_destroy(void *array, size_t size);

void
dtl_bool_array_set(void *array, size_t index, bool value);

bool
dtl_bool_array_get(void *array, size_t index);
