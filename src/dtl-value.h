#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

union dtl_value {
    bool as_bool;
    int64_t as_int64;
    double as_double;
    char const *as_string;
    void *as_pointer;
    size_t as_index;
    struct dtl_io_table *as_table;

    bool *as_bool_array;
    int64_t *as_int64_array;
    double *as_double_array;
    char **as_string_array;
    void **as_pointer_array;
    size_t *as_index_array;
};
