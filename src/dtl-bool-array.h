#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void *
dtl_bool_array_create(size_t size);

void
dtl_bool_array_destroy(void *array, size_t size);

void
dtl_bool_array_set(void *array, size_t index, bool value);

bool
dtl_bool_array_get(void const *array, size_t index);

void
dtl_bool_array_and(void const *restrict a, void const *restrict b, size_t size, void *restrict out);

void
dtl_bool_array_or(void const *restrict a, void const *restrict b, size_t size, void *restrict out);

void
dtl_bool_array_not(void const *restrict in, size_t size, void *restrict out);

void
dtl_bool_array_maskk(void const *restrict array, void const *restrict mask, size_t size, void *restrict out);

void
dtl_bool_array_pick(void const *restrict array, size_t const *restrict indexes, size_t size, void *restrict out);

uint64_t
dtl_bool_array_sum(void const *restrict array, size_t size);
