#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct dtl_location {
    char const *filename;

    /* Byte offset of the beginning character in the string. */
    size_t offset;

    /* One-indexed line number of the character in the file */
    size_t lineno;

    /* One-indexed column offset of the character in the file */
    size_t column;
};

static const struct dtl_location DTL_LOCATION_NULL = {.filename = NULL, .offset = SIZE_MAX, .lineno = SIZE_MAX, .column = SIZE_MAX};

bool
dtl_location_is_null(struct dtl_location location);
