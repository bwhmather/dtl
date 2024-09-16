#include "dtl-value.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "dtl-bool-array.h"
#include "dtl-double-array.h"
#include "dtl-dtype.h"
#include "dtl-index-array.h"
#include "dtl-int64-array.h"
#include "dtl-io.h"
#include "dtl-string-array.h"

/* --- Booleans --------------------------------------------------------------------------------- */

void
dtl_value_set_bool(struct dtl_value *value, bool b) {
    assert(value != NULL);
    assert(value->dtype == 0);

#ifndef NDEBUG
    value->dtype = DTL_DTYPE_BOOL;
#endif

    value->as_bool = b;
}

bool
dtl_value_get_bool(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == DTL_DTYPE_BOOL);

    return value->as_bool;
}

void
dtl_value_clear_bool(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == 0 || value->dtype == DTL_DTYPE_BOOL);

#ifndef NDEBUG
    value->dtype = 0;
#endif

    value->as_bool = false;
}

/* --- Integers --------------------------------------------------------------------------------- */

void
dtl_value_set_int64(struct dtl_value *value, int64_t i) {
    assert(value != NULL);
    assert(value->dtype == 0);

#ifndef NDEBUG
    value->dtype = DTL_DTYPE_INT64;
#endif

    value->as_int64 = i;
}

int64_t
dtl_value_get_int64(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == DTL_DTYPE_INT64);

    return value->as_int64;
}

void
dtl_value_clear_int64(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == 0 || value->dtype == DTL_DTYPE_INT64);

#ifndef NDEBUG
    value->dtype = 0;
#endif

    value->as_int64 = false;
}

/* --- Doubles ---------------------------------------------------------------------------------- */

void
dtl_value_set_double(struct dtl_value *value, int64_t d) {
    assert(value != NULL);
    assert(value->dtype == 0);

#ifndef NDEBUG
    value->dtype = DTL_DTYPE_DOUBLE;
#endif

    value->as_double = d;
}

int64_t
dtl_value_get_double(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == DTL_DTYPE_DOUBLE);

    return value->as_double;
}

void
dtl_value_clear_double(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == 0 || value->dtype == DTL_DTYPE_DOUBLE);

#ifndef NDEBUG
    value->dtype = 0;
#endif

    value->as_double = false;
}

/* --- Strings ---------------------------------------------------------------------------------- */

void
dtl_value_set_string(struct dtl_value *value, char const *s) {
    assert(value != NULL);
    assert(value->dtype == 0);
    assert(s != NULL);

#ifndef NDEBUG
    value->dtype = DTL_DTYPE_STRING;
#endif

    value->as_string = strdup(s);
}

void
dtl_value_take_string(struct dtl_value *value, char *s) {
    assert(value != NULL);
    assert(value->dtype == 0);
    assert(s != NULL);

#ifndef NDEBUG
    value->dtype = DTL_DTYPE_STRING;
#endif

    value->as_string = s;
}

char const *
dtl_value_get_string(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == DTL_DTYPE_STRING);

    return value->as_string;
}

void
dtl_value_clear_string(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == 0 || value->dtype == DTL_DTYPE_STRING);

#ifndef NDEBUG
    value->dtype = 0;
#endif

    free(value->as_string);
    value->as_string = NULL;
}

/* --- Indexes ---------------------------------------------------------------------------------- */

void
dtl_value_set_index(struct dtl_value *value, size_t index) {
    assert(value != NULL);
    assert(value->dtype == 0);

#ifndef NDEBUG
    value->dtype = DTL_DTYPE_INDEX;
#endif

    value->as_index = index;
}

int64_t
dtl_value_get_index(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == DTL_DTYPE_INDEX);

    return value->as_index;
}

void
dtl_value_clear_index(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == 0 || value->dtype == DTL_DTYPE_INDEX);

#ifndef NDEBUG
    value->dtype = 0;
#endif

    value->as_index = 0;
}

/* --- Tables ----------------------------------------------------------------------------------- */

void
dtl_value_take_table(struct dtl_value *value, struct dtl_io_table *table) {
    assert(value != NULL);
    assert(value->dtype == 0);

#ifndef NDEBUG
    value->dtype = DTL_DTYPE_TABLE;
#endif

    value->as_table = table;
}

struct dtl_io_table *
dtl_value_get_table(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == DTL_DTYPE_TABLE);

    return value->as_table;
}

void
dtl_value_clear_table(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == 0 || value->dtype == DTL_DTYPE_TABLE);

#ifndef NDEBUG
    value->dtype = 0;
#endif

    dtl_io_table_destroy(value->as_table);
    value->as_table = NULL;
}

/* --- Boolean Arrays --------------------------------------------------------------------------- */

void
dtl_value_take_bool_array(struct dtl_value *value, void *bool_array) {
    assert(value != NULL);
    assert(value->dtype == 0);
    assert(bool_array != NULL);

#ifndef NDEBUG
    value->dtype = DTL_DTYPE_BOOL_ARRAY;
#endif

    value->as_bool_array = bool_array;
}

void *
dtl_value_get_bool_array(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == DTL_DTYPE_BOOL_ARRAY);

    return value->as_bool_array;
}

void
dtl_value_clear_bool_array(struct dtl_value *value, size_t size) {
    assert(value != NULL);
    assert(value->dtype == 0 || value->dtype == DTL_DTYPE_BOOL_ARRAY);

#ifndef NDEBUG
    value->dtype = 0;
#endif

    dtl_bool_array_destroy(value, size);
    value->as_bool_array = NULL;
}

/* --- Integer Arrays --------------------------------------------------------------------------- */

void
dtl_value_take_int64_array(struct dtl_value *value, int64_t *int64_array) {
    assert(value != NULL);
    assert(value->dtype == 0);
    assert(int64_array != NULL);

#ifndef NDEBUG
    value->dtype = DTL_DTYPE_INT64_ARRAY;
#endif

    value->as_int64_array = int64_array;
}

int64_t *
dtl_value_get_int64_array(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == DTL_DTYPE_INT64_ARRAY);

    return value->as_int64_array;
}

void
dtl_value_clear_int64_array(struct dtl_value *value, size_t size) {
    assert(value != NULL);
    assert(value->dtype == 0 || value->dtype == DTL_DTYPE_INT64_ARRAY);

#ifndef NDEBUG
    value->dtype = 0;
#endif

    dtl_int64_array_destroy(value->as_int64_array, size);
    value->as_int64_array = NULL;
}

/* --- Double Arrays ---------------------------------------------------------------------------- */

void
dtl_value_take_double_array(struct dtl_value *value, double *double_array) {
    assert(value != NULL);
    assert(value->dtype == 0);
    assert(double_array != NULL);

#ifndef NDEBUG
    value->dtype = DTL_DTYPE_DOUBLE_ARRAY;
#endif

    value->as_double_array = double_array;
}

double *
dtl_value_get_double_array(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == DTL_DTYPE_DOUBLE_ARRAY);

    return value->as_double_array;
}

void
dtl_value_clear_double_array(struct dtl_value *value, size_t size) {
    assert(value != NULL);
    assert(value->dtype == 0 || value->dtype == DTL_DTYPE_DOUBLE_ARRAY);

#ifndef NDEBUG
    value->dtype = 0;
#endif

    dtl_double_array_destroy(value->as_double_array, size);
    value->as_double_array = NULL;
}

/* --- String Arrays ---------------------------------------------------------------------------- */

void
dtl_value_take_string_array(struct dtl_value *value, char **string_array) {
    assert(value != NULL);
    assert(value->dtype == 0);
    assert(string_array != NULL);

#ifndef NDEBUG
    value->dtype = DTL_DTYPE_STRING_ARRAY;
#endif

    value->as_string_array = string_array;
}

char **
dtl_value_get_string_array(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == DTL_DTYPE_STRING_ARRAY);

    return value->as_string_array;
}

void
dtl_value_clear_string_array(struct dtl_value *value, size_t size) {
    assert(value != NULL);
    assert(value->dtype == 0 || value->dtype == DTL_DTYPE_STRING_ARRAY);

#ifndef NDEBUG
    value->dtype = 0;
#endif

    dtl_string_array_destroy(value->as_string_array, size);
    value->as_string_array = NULL;
}

/* --- Index Arrays ----------------------------------------------------------------------------- */

void
dtl_value_take_index_array(struct dtl_value *value, size_t *index_array) {
    assert(value != NULL);
    assert(value->dtype == 0);
    assert(index_array != NULL);

#ifndef NDEBUG
    value->dtype = DTL_DTYPE_INDEX_ARRAY;
#endif

    value->as_index_array = index_array;
}

size_t *
dtl_value_get_index_array(struct dtl_value *value) {
    assert(value != NULL);
    assert(value->dtype == DTL_DTYPE_INDEX_ARRAY);

    return value->as_index_array;
}

void
dtl_value_clear_index_array(struct dtl_value *value, size_t size) {
    assert(value != NULL);
    assert(value->dtype == 0 || value->dtype == DTL_DTYPE_INDEX_ARRAY);

#ifndef NDEBUG
    value->dtype = 0;
#endif

    dtl_index_array_destroy(value->as_index_array, size);
    value->as_index_array = NULL;
}
