#pragma once

#include <stddef.h>
#include <stdint.h>

#include "dtl-dtype.h"
#include "dtl-error.h"
#include "dtl-location.h"
#include "dtl-schema.h"
#include "dtl-value.h"

/* === Tables =================================================================================== */

struct dtl_io_table {
    struct dtl_schema *schema;
    size_t (*get_num_rows)(struct dtl_io_table *table);
    enum dtl_status (*read_column_data)(struct dtl_io_table *table, size_t col_index, struct dtl_value *out, struct dtl_error **error);
    void (*destroy)(struct dtl_io_table *);
};

struct dtl_schema *
dtl_io_table_get_schema(struct dtl_io_table *table);

size_t
dtl_io_table_get_num_rows(struct dtl_io_table *table);

enum dtl_status
dtl_io_table_read_column_data(struct dtl_io_table *table, size_t col_index, struct dtl_value *out, struct dtl_error **error);

void
dtl_io_table_destroy(struct dtl_io_table *);

/* === Importers ================================================================================ */

struct dtl_io_importer {
    struct dtl_io_table *(*import_table)(struct dtl_io_importer *, char const *, struct dtl_error **);
};

struct dtl_io_table *
dtl_io_importer_import_table(struct dtl_io_importer *, char const *, struct dtl_error **);

/* === Exporters ================================================================================ */

struct dtl_io_exporter {
    enum dtl_status (*export_table)(struct dtl_io_exporter *, char const *, struct dtl_schema *, size_t, struct dtl_value **, struct dtl_error **);
};

enum dtl_status
dtl_io_exporter_export_table(struct dtl_io_exporter *, char const *, struct dtl_schema *, size_t, struct dtl_value **, struct dtl_error **);

/* === Tracers ================================================================================== */

struct dtl_io_tracer {
    enum dtl_status (*record_source)(struct dtl_io_tracer *, char const *source, char const *filename, struct dtl_error **);
    enum dtl_status (*record_input)(struct dtl_io_tracer *, char const *, struct dtl_schema *, uint64_t *, struct dtl_error **);
    enum dtl_status (*record_output)(struct dtl_io_tracer *, char const *, struct dtl_schema *, uint64_t *, struct dtl_error **);
    enum dtl_status (*record_trace)(struct dtl_io_tracer *, struct dtl_location start, struct dtl_location end, struct dtl_schema *, uint64_t *, struct dtl_error **);
    enum dtl_status (*record_mapping)(struct dtl_io_tracer *, uint64_t src_array, uint64_t tgt_array, uint64_t src_index_array, uint64_t tgt_index_array, struct dtl_error **);
    enum dtl_status (*record_value)(struct dtl_io_tracer *, uint64_t id, enum dtl_dtype, size_t, struct dtl_value *, struct dtl_error **);
};

enum dtl_status
dtl_io_tracer_record_source(struct dtl_io_tracer *, char const *source, char const *filename, struct dtl_error **);

enum dtl_status
dtl_io_tracer_record_input(struct dtl_io_tracer *, char const *, struct dtl_schema *, uint64_t *, struct dtl_error **);

enum dtl_status
dtl_io_tracer_record_output(struct dtl_io_tracer *, char const *, struct dtl_schema *, uint64_t *, struct dtl_error **);

enum dtl_status
dtl_io_tracer_record_trace(struct dtl_io_tracer *, struct dtl_location start, struct dtl_location end, struct dtl_schema *, uint64_t *, struct dtl_error **);

enum dtl_status
dtl_io_tracer_record_mapping(struct dtl_io_tracer *tracer, uint64_t src_array, uint64_t tgt_array, uint64_t src_index_array, uint64_t tgt_index_array, struct dtl_error **);

enum dtl_status
dtl_io_tracer_record_value(struct dtl_io_tracer *, uint64_t id, enum dtl_dtype, size_t, struct dtl_value *, struct dtl_error **);
