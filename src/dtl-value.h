#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "dtl-dtype.h"

struct dtl_value {
#ifndef NDEBUG
    enum dtl_dtype dtype;
#endif

    union {
        bool as_bool;
        int64_t as_int64;
        double as_double;
        char *as_string;
        size_t as_index;
        struct dtl_io_table *as_table;

        bool *as_bool_array;
        int64_t *as_int64_array;
        double *as_double_array;
        char **as_string_array;
        size_t *as_index_array;
    };
};

/* --- Booleans --------------------------------------------------------------------------------- */

void
dtl_value_set_bool(struct dtl_value *value, bool b);

bool
dtl_value_get_bool(struct dtl_value *value);

void
dtl_value_clear_bool(struct dtl_value *value);

/* --- Integers --------------------------------------------------------------------------------- */

void
dtl_value_set_int64(struct dtl_value *value, int64_t i);

int64_t
dtl_value_get_int64(struct dtl_value *value);

void
dtl_value_clear_int64(struct dtl_value *value);

/* --- Doubles ---------------------------------------------------------------------------------- */

void
dtl_value_set_double(struct dtl_value *value, int64_t i);

int64_t
dtl_value_get_double(struct dtl_value *value);

void
dtl_value_clear_double(struct dtl_value *value);

/* --- Strings ---------------------------------------------------------------------------------- */

void
dtl_value_set_string(struct dtl_value *value, char const *s);

void
dtl_value_take_string(struct dtl_value *value, char *s);

char const *
dtl_value_get_string(struct dtl_value *value);

void
dtl_value_clear_string(struct dtl_value *value);

/* --- Indexes ---------------------------------------------------------------------------------- */

void
dtl_value_set_index(struct dtl_value *value, size_t index);

int64_t
dtl_value_get_index(struct dtl_value *value);

void
dtl_value_clear_index(struct dtl_value *value);

/* --- Tables ----------------------------------------------------------------------------------- */

void
dtl_value_take_table(struct dtl_value *value, struct dtl_io_table *table);

struct dtl_io_table *
dtl_value_get_table(struct dtl_value *value);

void
dtl_value_clear_table(struct dtl_value *value);

/* --- Boolean Arrays --------------------------------------------------------------------------- */

void
dtl_value_take_bool_array(struct dtl_value *value, void *bool_array);

void *
dtl_value_get_bool_array(struct dtl_value *value);

void
dtl_value_clear_bool_array(struct dtl_value *value, size_t size);

/* --- Integer Arrays --------------------------------------------------------------------------- */

void
dtl_value_take_int64_array(struct dtl_value *value, int64_t *int64_array);

int64_t *
dtl_value_get_int64_array(struct dtl_value *value);

void
dtl_value_clear_int64_array(struct dtl_value *value, size_t size);

/* --- Double Arrays ---------------------------------------------------------------------------- */

void
dtl_value_take_double_array(struct dtl_value *value, double *double_array);

double *
dtl_value_get_double_array(struct dtl_value *value);

void
dtl_value_clear_double_array(struct dtl_value *value, size_t size);

/* --- String Arrays ---------------------------------------------------------------------------- */

void
dtl_value_take_string_array(struct dtl_value *value, char **string_array);

char **
dtl_value_get_string_array(struct dtl_value *value);

void
dtl_value_clear_string_array(struct dtl_value *value, size_t size);

/* --- Index Arrays ----------------------------------------------------------------------------- */

void
dtl_value_take_index_array(struct dtl_value *value, size_t *index_array);

size_t *
dtl_value_get_index_array(struct dtl_value *value);

void
dtl_value_clear_index_array(struct dtl_value *value, size_t size);
