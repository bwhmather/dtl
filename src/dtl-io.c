#include "dtl-io.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "dtl-dtype.h"
#include "dtl-error.h"
#include "dtl-schema.h"
#include "dtl-value.h"

/* === Tables =================================================================================== */

struct dtl_schema *
dtl_io_table_get_schema(struct dtl_io_table *table) {
    assert(table != NULL);
    assert(table->schema != NULL);

    return table->schema;
}

size_t
dtl_io_table_get_num_rows(struct dtl_io_table *table) {
    assert(table != NULL);
    assert(table->get_num_rows != NULL);

    return table->get_num_rows(table);
}

enum dtl_status
dtl_io_table_read_column_data(
    struct dtl_io_table *table,
    size_t col_index,
    struct dtl_value *out,
    struct dtl_error **error
) {
    assert(table != NULL);
    assert(out != NULL);
    assert(col_index < dtl_schema_get_num_columns(table->schema));
    assert(table->read_column_data != NULL);

    return table->read_column_data(table, col_index, out, error);
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
dtl_io_exporter_export_table(
    struct dtl_io_exporter *exporter,
    char const *name,
    struct dtl_schema *schema,
    size_t num_rows,
    struct dtl_value **values,
    struct dtl_error **error
) {
    assert(exporter != NULL);
    assert(exporter->export_table != NULL);
    assert(name != NULL);
    assert(schema != NULL);
    assert(values != NULL);

    return exporter->export_table(exporter, name, schema, num_rows, values, error);
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
