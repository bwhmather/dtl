#include "dtl-error.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "dtl-location.h"

struct dtl_error *
dtl_error_create(char const *message) {
    struct dtl_error *result;

    assert(message != NULL);

    result = calloc(1, sizeof(struct dtl_error));
    result->message = strdup(message);
    result->start = DTL_LOCATION_NULL;
    result->end = DTL_LOCATION_NULL;

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
    error->start = start;
    error->end = end;
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
