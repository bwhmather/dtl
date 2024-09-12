#include "dtl-string-interner.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <xxhash.h>

static const size_t DEFAULT_CAPACITY = 128;

struct dtl_string_interner {
    int seed;
    char **strings;
    size_t size;
    size_t capacity;
};

static uint64_t
dtl_string_interner_hash_string(struct dtl_string_interner const *self, char const *string) {
    size_t length;

    assert(self != NULL);

    if (string == NULL) {
        return 0;
    }

    length = strlen(string);
    return XXH64(string, length, (XXH64_hash_t)self->seed);
}

static size_t
dtl_string_interner_wrap(struct dtl_string_interner *self, size_t cursor) {
    assert(self != NULL);

    if (cursor >= self->capacity) {
        cursor -= self->capacity;
    }
    assert(cursor < self->capacity);

    return cursor;
}

static size_t
dtl_string_interner_diff(struct dtl_string_interner *self, size_t target, size_t cursor) {
    assert(self != NULL);
    assert(target < self->capacity);
    assert(cursor < self->capacity);

    if (cursor >= target) {
        return cursor - target;
    } else {
        return target + (self->capacity - cursor - 1);
    }
}

static size_t
dtl_string_interner_target(struct dtl_string_interner *self, char const *string) {
    uint64_t hash;

    assert(self != NULL);
    assert(string != NULL);

    hash = dtl_string_interner_hash_string(self, string);
    return hash % self->capacity;
}

struct dtl_string_interner *
dtl_string_interner_create(void) {
    char **strings;
    struct dtl_string_interner *self;

    strings = calloc(DEFAULT_CAPACITY, sizeof(char *));
    if (strings == NULL) {
        return NULL;
    }

    self = calloc(1, sizeof(struct dtl_string_interner));
    if (self == NULL) {
        free(strings);
        return NULL;
    }

    self->strings = strings;
    self->capacity = DEFAULT_CAPACITY;
    self->seed = rand();
    return self;
}

void
dtl_string_interner_destroy(struct dtl_string_interner *self) {
    size_t i;

    if (self == NULL) {
        return;
    }

    for (i = 0; i < self->capacity; i++) {
        if (self->strings[i] == NULL) {
            continue;
        }
        assert(self->size > 0);
        free(self->strings[i]);
        self->strings[i] = NULL;
        self->size -= 1;
    }

    assert(self->size == 0);

    free(self->strings);
    self->strings = NULL;

    self->capacity = 0;

    free(self);
}

char const *
dtl_string_interner_intern(struct dtl_string_interner *self, char const *input_string) {
    char **old_strings;
    size_t old_capacity;
    size_t old_cursor;
    size_t cursor;
    char *displaced_string = NULL;
    size_t displaced_target = 0;
    size_t displaced_diff;
    size_t input_target;
    size_t input_diff;
    char *candidate_string;
    size_t candidate_target;
    size_t candidate_diff;
    char *result_string;

    assert(self != NULL);

    if (input_string == NULL) {
        return NULL;
    }

    // Resize (if necessary).
    if (self->size > (self->capacity / 10) * 7) {
        old_strings = self->strings;
        old_capacity = self->capacity;

        self->capacity *= 2;
        self->strings = calloc(self->capacity, sizeof(char *));

        for (old_cursor = 0; old_cursor < old_capacity; old_cursor++) {
            displaced_string = old_strings[old_cursor];
            if (displaced_string == NULL) {
                continue;
            }
            displaced_target = dtl_string_interner_target(self, displaced_string);

            cursor = displaced_target;
            while (displaced_string != NULL) {
                candidate_string = self->strings[cursor];
                candidate_target = cursor;
                if (candidate_string != NULL) {
                    candidate_target = dtl_string_interner_target(self, candidate_string);
                }

                candidate_diff = dtl_string_interner_diff(self, candidate_target, cursor);
                displaced_diff = dtl_string_interner_diff(self, displaced_target, cursor);
                if (displaced_diff >= candidate_diff) {
                    self->strings[cursor] = displaced_string;
                    displaced_string = candidate_string;
                    displaced_target = candidate_target;
                }

                cursor = dtl_string_interner_wrap(self, cursor + 1);
            }
        }
        free(old_strings);
    }

    input_target = dtl_string_interner_target(self, input_string);
    cursor = input_target;

    // Find current position of input string or new insertion point.
    while (true) {
        candidate_string = self->strings[cursor];

        if (candidate_string == NULL) {
            break;
        }

        if (strcmp(input_string, candidate_string) == 0) {
            return candidate_string;
        }

        candidate_target = dtl_string_interner_target(self, candidate_string);

        candidate_diff = dtl_string_interner_diff(self, candidate_target, cursor);
        input_diff = dtl_string_interner_diff(self, input_target, cursor);
        if (candidate_diff > input_diff) {
            break;
        }

        cursor = dtl_string_interner_wrap(self, cursor + 1);
    }

    // Replace entry at insertion point with copy of input string.
    if (candidate_string != NULL) {
        displaced_string = candidate_string;
        displaced_target = candidate_target;
    }
    result_string = strdup(input_string);
    self->strings[cursor] = result_string;
    self->size += 1;

    // Displace previous entry at insertion point (if any) to right.
    while (displaced_string != NULL) {
        candidate_string = self->strings[cursor];
        candidate_target = cursor;
        if (candidate_string != NULL) {
            dtl_string_interner_target(self, candidate_string);
        }

        candidate_diff = dtl_string_interner_diff(self, candidate_target, cursor);
        displaced_diff = dtl_string_interner_diff(self, displaced_target, cursor);
        if (displaced_diff > candidate_diff) {
            self->strings[cursor] = displaced_string;
            displaced_string = candidate_string;
            displaced_target = candidate_target;
        }

        cursor = dtl_string_interner_wrap(self, cursor + 1);
    }

    return result_string;
}
