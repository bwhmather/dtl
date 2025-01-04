#pragma once

#include "dtl-error.h"
#include "dtl-io.h"

struct dtl_io_tracer *
dtl_io_duckdb_tracer_create(char const *path, struct dtl_error **error);

enum dtl_status
dtl_io_duckdb_tracer_destroy(struct dtl_io_tracer *, struct dtl_error **error);
