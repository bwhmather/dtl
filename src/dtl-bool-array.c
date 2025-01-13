#include "dtl-bool-array.h"

#include <stdbit.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

void *
dtl_bool_array_create(size_t size) {
    size_t num_chunks = ((size + 63) / 64);
    return calloc(num_chunks, sizeof(uint64_t));
}

void
dtl_bool_array_destroy(void *array, size_t size) {
    (void)size;

    free(array);
}

void
dtl_bool_array_set(void *array, size_t index, bool value) {
    uint64_t *chunks = array;
    if (value) {
        chunks[index / 64] |= ((uint64_t)1) << (index % 64);
    } else {
        chunks[index / 64] &= ~(((uint64_t)1) << (index % 64));
    }
}

bool
dtl_bool_array_get(void const *array, size_t index) {
    uint64_t const *chunks = array;
    return chunks[index / 64] & (((uint64_t)1) << (index % 64));
}

static inline void
dtl_bool_array_fix_padding(void *array, size_t size) {
    uint64_t *chunks = array;
    int remainder = 64 - (size % 64);

    if (remainder) {
        chunks[((size + 63) / 64) - 1] &= 0xffffffff >> remainder;
    }
}

void
dtl_bool_array_and(void const *restrict a, void const *restrict b, size_t size, void *restrict out) {
    uint64_t const *a_chunks = a;
    uint64_t const *b_chunks = b;
    uint64_t *out_chunks = out;
    size_t i;

    for (i = 0; i < ((size + 63) / 64); i++) {
        out_chunks[i] = a_chunks[i] & b_chunks[i];
    }
}

void
dtl_bool_array_or(void const *restrict a, void const *restrict b, size_t size, void *restrict out) {
    uint64_t const *a_chunks = a;
    uint64_t const *b_chunks = b;
    uint64_t *out_chunks = out;
    size_t i;

    for (i = 0; i < ((size + 63) / 64); i++) {
        out_chunks[i] = a_chunks[i] | b_chunks[i];
    }
}

void
dtl_bool_array_not(void const *restrict in, size_t size, void *restrict out) {
    uint64_t const *in_chunks = in;
    uint64_t *out_chunks = out;
    size_t i;

    for (i = 0; i < ((size + 63) / 64); i++) {
        out_chunks[i] = ~in_chunks[i];
    }

    dtl_bool_array_fix_padding(out, size);
}

void
dtl_bool_array_maskk(void const *restrict array, void const *restrict mask, size_t size, void *restrict out) {
    size_t cursor = 0;
    size_t i;

    for (i = 0; i < size; i++) {
        if (dtl_bool_array_get(mask, i)) {
            dtl_bool_array_set(out, cursor, dtl_bool_array_get(array, i));
            cursor++;
        }
    }
}

void
dtl_bool_array_pick(void const *restrict array, size_t const *restrict indexes, size_t size, void *restrict out) {
    size_t i;

    for (i = 0; i < size; i++) {
        dtl_bool_array_set(out, i, dtl_bool_array_get(array, indexes[i]));
    }
}

uint64_t
dtl_bool_array_sum(void const *restrict array, size_t size) {
    uint64_t const *chunks = array;
    size_t i;
    uint64_t result = 0;

    for (i = 0; i < ((size + 63) / 64); i++) {
        result += stdc_count_ones_ull(chunks[i]);
    }

    return result;
}
