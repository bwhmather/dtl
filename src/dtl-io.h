#pragma once

#include <stddef.h>
#include <stdint.h>

#include "dtl-dtype.h"
#include "dtl-error.h"
#include "dtl-location.h"
#include "dtl-value.h"

/* === Schema =================================================================================== */

struct dtl_io_schema;

struct dtl_io_schema *
dtl_io_schema_create(void);

struct dtl_io_schema *
dtl_io_schema_add_column(struct dtl_io_schema *schema, char const *name, enum dtl_dtype dtype);

size_t
dtl_io_schema_get_num_columns(struct dtl_io_schema *schema);

char const *
dtl_io_schema_get_column_name(struct dtl_io_schema *schema, size_t index);

enum dtl_dtype
dtl_io_schema_get_column_dtype(struct dtl_io_schema *schema, size_t index);

void
dtl_io_schema_destroy(struct dtl_io_schema *schema);

/* === Tables =================================================================================== */

struct dtl_io_table {
    struct dtl_io_schema *schema;
    size_t (*get_num_rows)(struct dtl_io_table *table);
    enum dtl_status (*read_column_data)(struct dtl_io_table *table, size_t col_index, struct dtl_value *out, struct dtl_error **error);
    void (*destroy)(struct dtl_io_table *);
};

struct dtl_io_schema *
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
    enum dtl_status (*export_table)(struct dtl_io_exporter *, char const *, struct dtl_io_table *, struct dtl_error **);
};

enum dtl_status
dtl_io_exporter_export_table(struct dtl_io_exporter *, char const *, struct dtl_io_table *, struct dtl_error **);

/* === Tracers ================================================================================== */

struct dtl_io_trace_column {
    char const *name;
    uint64_t array;
};

struct dtl_io_trace_snapshot {
    struct dtl_location start;
    struct dtl_location end;

    struct dtl_io_trace_column *columns;
    size_t num_columns;
};

struct dtl_io_trace_mapping {
    // The identifiers of the two arrays that this mapping connects.
    uint64_t src_array;
    uint64_t tgt_array;

    // TODO more efficient representations for common cases.
    // Pair of arrays forming a table that maps indexes in the source array into
    // indexes in the target array.
    uint64_t src_index_array;
    uint64_t tgt_index_array;
};

struct dtl_io_trace_manifest {
    char const *source;

    struct dtl_io_trace_snapshot *snapshots;
    size_t num_snapshots;

    struct dtl_io_trace_mapping *mappings;
    size_t num_mappings;
};

struct dtl_io_tracer {
    void (*write_manifest)(struct dtl_io_tracer *, struct dtl_io_trace_manifest *);
    void (*write_array)(struct dtl_io_tracer *, uint64_t, enum dtl_dtype, size_t, void *);
};

void
dtl_io_tracer_write_manifest(struct dtl_io_tracer *, struct dtl_io_trace_manifest *);

void
dtl_io_tracer_write_array(struct dtl_io_tracer *, uint64_t, enum dtl_dtype, size_t, void *);
