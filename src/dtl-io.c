#include "dtl-io.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "dtl-dtype.h"
#include "dtl-error.h"

/* === Tables =================================================================================== */

size_t
dtl_io_table_get_num_rows(struct dtl_io_table *table) {
    assert(table != NULL);
    assert(table->get_num_rows != NULL);

    return table->get_num_rows(table);
}

size_t
dtl_io_table_get_num_columns(struct dtl_io_table *table) {
    assert(table != NULL);
    assert(table->get_num_columns != NULL);

    return table->get_num_columns(table);
}

char const *
dtl_io_table_get_column_name(struct dtl_io_table *table, size_t col) {
    assert(table != NULL);
    assert(table->get_column_name != NULL);

    return table->get_column_name(table, col);
}

enum dtl_dtype
dtl_io_table_get_column_dtype(struct dtl_io_table *table, size_t col) {
    assert(table != NULL);
    assert(table->get_column_dtype != NULL);

    return table->get_column_dtype(table, col);
}

enum dtl_status
dtl_io_table_get_column_data(
    struct dtl_io_table *table, size_t col, void *dest, size_t offset, size_t size, struct dtl_error **error
) {
    assert(table != NULL);
    assert(table->get_column_data != NULL);

    return table->get_column_data(table, col, dest, offset, size, error);
}

void
dtl_io_table_destroy(struct dtl_io_table *table) {
    if (table != NULL) {
        assert(table->destroy != NULL);
        table->destroy(table);
    }
}

/* === Importers ================================================================================ */

struct dtl_io_table *
dtl_io_importer_import_table(struct dtl_io_importer *importer, char const *name, struct dtl_error **error) {
    assert(importer != NULL);
    assert(importer->import_table != NULL);
    assert(name != NULL);

    return importer->import_table(importer, name, error);
}

/* === Exporters ================================================================================ */

enum dtl_status
dtl_io_exporter_export_table(struct dtl_io_exporter *exporter, char const *name, struct dtl_io_table *table, struct dtl_error **error) {
    assert(exporter != NULL);
    assert(exporter->export_table != NULL);
    assert(name != NULL);
    assert(table != NULL);

    return exporter->export_table(exporter, name, table, error);
}

/* === Tracers ================================================================================== */

void
dtl_io_tracer_write_manifest(struct dtl_io_tracer *tracer, struct dtl_io_trace_manifest *manifest) {
    assert(tracer != NULL);
    assert(tracer->write_manifest != NULL);
    assert(manifest != NULL);

    tracer->write_manifest(tracer, manifest);
}

void
dtl_io_tracer_write_array(struct dtl_io_tracer *tracer, uint64_t id, enum dtl_dtype dtype, size_t length, void *data) {
    assert(tracer != NULL);
    assert(id != 0);
    assert(data != NULL || length == 0);

    tracer->write_array(tracer, id, dtype, length, data);
}
