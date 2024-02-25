#pragma once

#include <stddef.h>

struct dtl_location {
    /* Byte offset of the beginning character in the string. */
    size_t offset;

    /* One-indexed line number of the character in the file */
    size_t lineno;

    /* One-indexed column offset of the character in the file */
    size_t column;
};
