#pragma once

#include <stddef.h>

double *
dtl_double_array_create(size_t size);

void
dtl_double_array_destroy(double *array, size_t size);

void
dtl_double_array_set(double *array, size_t index, double value);

double
dtl_double_array_get(double *array, size_t index);
