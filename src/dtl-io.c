#include "dtl-io.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>

#include "dtl-dtype.h"
#include "dtl-error.h"
#include "dtl-location.h"
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

enum dtl_status
dtl_io_tracer_record_source(
    struct dtl_io_tracer *tracer,
    char const *source,
    char const *filename,
    struct dtl_error **error
) {
    assert(tracer != NULL);
    assert(source != NULL);
    assert(filename != NULL);
    assert(error != NULL);

    if (tracer->record_source != NULL) {
        return tracer->record_source(tracer, source, filename, error);
    }
    return DTL_STATUS_OK;
}

enum dtl_status
dtl_io_tracer_record_input(
    struct dtl_io_tracer *tracer,
    char const *name,
    struct dtl_schema *schema,
    uint64_t *array_ids,
    struct dtl_error **error
) {
    assert(tracer != NULL);
    assert(name != NULL);
    assert(schema != NULL);
    assert(array_ids != NULL);
    assert(error != NULL);

    if (tracer->record_input != NULL) {
        return tracer->record_input(tracer, name, schema, array_ids, error);
    }
    return DTL_STATUS_OK;
}

enum dtl_status
dtl_io_tracer_record_output(
    struct dtl_io_tracer *tracer,
    char const *name,
    struct dtl_schema *schema,
    uint64_t *array_ids,
    struct dtl_error **error
) {
    assert(tracer != NULL);
    assert(name != NULL);
    assert(schema != NULL);
    assert(array_ids != NULL);
    assert(error != NULL);

    if (tracer->record_output != NULL) {
        return tracer->record_output(tracer, name, schema, array_ids, error);
    }
    return DTL_STATUS_OK;
}

enum dtl_status
dtl_io_tracer_record_trace(
    struct dtl_io_tracer *tracer,
    struct dtl_location start,
    struct dtl_location end,
    struct dtl_schema *schema,
    uint64_t *array_ids,
    struct dtl_error **error
) {
    assert(tracer != NULL);
    assert(!dtl_location_is_null(start));
    assert(!dtl_location_is_null(end));
    assert(start.filename == end.filename);
    assert(start.offset < end.offset);
    assert(schema != NULL);
    assert(array_ids != NULL);

    if (tracer->record_trace != NULL) {
        return tracer->record_trace(tracer, start, end, schema, array_ids, error);
    }
    return DTL_STATUS_OK;
}

enum dtl_status
dtl_io_tracer_record_mapping(
    struct dtl_io_tracer *tracer,
    uint64_t src_array_id,
    uint64_t tgt_array_id,
    uint64_t src_index_array_id,
    uint64_t tgt_index_array_id,
    struct dtl_error **error
) {
    assert(tracer != NULL);
    assert(src_array_id != 0);
    assert(tgt_array_id != 0);

    if (tracer->record_mapping != NULL) {
        return tracer->record_mapping(tracer, src_array_id, tgt_array_id, src_index_array_id, tgt_index_array_id, error);
    }
    return DTL_STATUS_OK;
}

enum dtl_status
dtl_io_tracer_record_value(
    struct dtl_io_tracer *tracer,
    uint64_t id,
    enum dtl_dtype dtype,
    size_t size,
    struct dtl_value *value,
    struct dtl_error **error
) {
    assert(tracer != NULL);
    assert(id != 0);
    assert(dtl_dtype_is_array_type(dtype) || size == 0);
    assert(value != NULL);
    assert(error != NULL);

    if (tracer->record_value != NULL) {
        return tracer->record_value(tracer, id, dtype, size, value, error);
    }
    return DTL_STATUS_OK;
}
