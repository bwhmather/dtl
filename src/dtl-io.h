#pragma once

#include <stdint.h>

#include "dtl-dtype.h"
#include "dtl-location.h"

/* === Tables =================================================================================== */

struct dtl_io_table {
    size_t (*get_num_rows)(struct dtl_io_table *);
    size_t (*get_num_columns)(struct dtl_io_table *);
    char const *(*get_column_name)(struct dtl_io_table *, size_t);
    enum dtl_dtype (*get_column_dtype)(struct dtl_io_table *, size_t);
    void (*get_column_data)(struct dtl_io_table *, size_t, void *, size_t, size_t);
    void (*destroy)(struct dtl_io_table *);
};

size_t
dtl_io_table_get_num_rows(struct dtl_io_table *);

size_t
dtl_io_table_get_num_columns(struct dtl_io_table *);

char const *
dtl_io_table_get_column_name(struct dtl_io_table *, size_t);

enum dtl_dtype
dtl_io_table_get_column_dtype(struct dtl_io_table *, size_t);

void
dtl_io_table_get_column_data(struct dtl_io_table *, size_t col, void *dest, size_t offset, size_t size);

void
dtl_io_table_destroy(struct dtl_io_table *);

/* === Importers ================================================================================ */

struct dtl_io_importer {
    struct dtl_io_table *(*import_table)(struct dtl_io_importer *, char const *);
};

struct dtl_io_table *
dtl_io_importer_import_table(struct dtl_io_importer *, char const *);

/* === Exporters ================================================================================ */

struct dtl_io_exporter {
    void (*export_table)(struct dtl_io_exporter *, char const *, struct dtl_io_table *);
};

void
dtl_io_exporter_export_table(struct dtl_io_exporter *, char const *, struct dtl_io_table *);

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
