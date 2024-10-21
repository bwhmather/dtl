#pragma once

#include "dtl-error.h"
#include "dtl-io.h"

enum dtl_status
dtl_eval(
    char const *source,
    char const *filename,
    struct dtl_io_importer *importer,
    struct dtl_io_exporter *exporter,
    struct dtl_io_tracer *tracer,
    struct dtl_error **error
);
