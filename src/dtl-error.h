#pragma once

#include "dtl-location.h"

struct dtl_error {
    struct dtl_location start;
    struct dtl_location end;
    char *message;
};

enum dtl_status {
    DTL_STATUS_OK = 0,
    DTL_STATUS_ERROR,
};

struct dtl_error *
dtl_error_create(char const *message, ...) __attribute__((format(printf, 1, 2)));

void
dtl_error_destroy(struct dtl_error *);

void
dtl_error_set_location(struct dtl_error *error, struct dtl_location start, struct dtl_location end);

void
dtl_set_error(struct dtl_error **error, struct dtl_error *value);

void
dtl_clear_error(struct dtl_error **error);
