#include "dtl-error.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "dtl-location.h"

struct dtl_error *
dtl_error_create(char const *message, ...) {
    struct dtl_error *result;
    va_list args;
    int size;

    assert(message != NULL);

    result = calloc(1, sizeof(struct dtl_error));
    result->start = DTL_LOCATION_NULL;
    result->end = DTL_LOCATION_NULL;

    va_start(args, message);
    size = vasprintf(&result->message, message, args);
    va_end(args);
    if (size < 0) {
        abort(); // Not a recoverable error.
    }
    return result;
}

void
dtl_error_destroy(struct dtl_error *error) {
    if (error == NULL) {
        return;
    }

    free(error->message);
    free(error);
}

void
dtl_error_set_location(struct dtl_error *error, struct dtl_location start, struct dtl_location end) {
    assert(error != NULL);
    assert(start.filename == end.filename);

    error->start = start;
    error->end = end;
}

void
dtl_error_shrink_location(struct dtl_error *error, struct dtl_location start, struct dtl_location end) {
    assert(error != NULL);
    assert(start.filename == end.filename);

    if (error->start.filename != start.filename || error->start.offset < start.offset) {
        error->start = start;
    }

    if (error->end.filename != end.filename || error->end.offset > end.offset) {
        error->end = end;
    }
}

void
dtl_set_error(struct dtl_error **error, struct dtl_error *value) {
    if (*error == value) {
        return;
    }

    if (*error != NULL) {
        dtl_error_destroy(*error);
    }

    *error = value;
}

void
dtl_clear_error(struct dtl_error **error) {
    if (*error != NULL) {
        dtl_error_destroy(*error);
    }
    *error = NULL;
}
