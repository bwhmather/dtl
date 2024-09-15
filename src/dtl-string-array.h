#pragma once

#include <stddef.h>

char **
dtl_string_array_create(size_t size);

void
dtl_string_array_destroy(char **array, size_t size);

void
dtl_string_array_set(char **array, size_t index, char const *value);

void
dtl_string_array_take(char **array, size_t index, char *value);

char const *
dtl_string_array_get(char **array, size_t index);
