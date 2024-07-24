#pragma once

#include "dtl-io.h"

void
dtl_eval(
    char const *source,
    struct dtl_io_importer *importer,
    struct dtl_io_exporter *exporter,
    struct dtl_io_tracer *tracer
);
