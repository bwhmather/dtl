#pragma once

#include "dtl-io.h"

struct dtl_io_importer *
dtl_io_filesystem_importer_create(char const *root);

struct dtl_io_exporter *
dtl_io_filesystem_exporter_create(char const *root);

struct dtl_io_tracer *
dtl_io_filesystem_tracer_create(char const *root);
