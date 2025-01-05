#include "dtl-io-duckdb.h"

#include <duckdb.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "dtl-dtype.h"
#include "dtl-error.h"
#include "dtl-io.h"
#include "dtl-location.h"
#include "dtl-schema.h"
#include "dtl-value.h"

struct dtl_io_duckdb_tracer {
    struct dtl_io_tracer base;

    duckdb_database db;
    duckdb_connection con;

    duckdb_appender source_appender;
    duckdb_appender trace_appender;
    duckdb_appender mapping_appender;
    duckdb_appender input_appender;
    duckdb_appender output_appender;
};

static enum dtl_status
dtl_io_duckdb_tracer_record_source(
    struct dtl_io_tracer *base_tracer,
    char const *text,
    char const *filename,
    struct dtl_error **error
) {
    duckdb_state state = DuckDBSuccess;
    struct dtl_io_duckdb_tracer *tracer = (struct dtl_io_duckdb_tracer *)base_tracer;

    state |= duckdb_append_varchar(tracer->source_appender, text);
    state |= duckdb_append_varchar(tracer->source_appender, filename);
    state |= duckdb_appender_end_row(tracer->source_appender);
    if (state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Could not append source details: %s", duckdb_appender_error(tracer->source_appender)));
        return DTL_STATUS_ERROR;
    }

    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_io_duckdb_tracer_record_input(
    struct dtl_io_tracer *base_tracer,
    char const *input_name,
    struct dtl_schema *schema,
    uint64_t *array_ids,
    struct dtl_error **error
) {
    duckdb_state state = DuckDBSuccess;
    struct dtl_io_duckdb_tracer *tracer = (struct dtl_io_duckdb_tracer *)base_tracer;
    size_t i;
    char const *column_name;

    for (i = 0; i < dtl_schema_get_num_columns(schema); i++) {
        column_name = dtl_schema_get_column_name(schema, i);

        state |= duckdb_append_varchar(tracer->input_appender, input_name);
        state |= duckdb_append_varchar(tracer->input_appender, column_name);
        state |= duckdb_append_int64(tracer->input_appender, array_ids[i]);
        state |= duckdb_appender_end_row(tracer->input_appender);
    }

    if (state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Could not append input details: %s", duckdb_appender_error(tracer->input_appender)));
        return DTL_STATUS_ERROR;
    }

    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_io_duckdb_tracer_record_output(
    struct dtl_io_tracer *base_tracer,
    char const *output_name,
    struct dtl_schema *schema,
    uint64_t *array_ids,
    struct dtl_error **error
) {
    duckdb_state state = DuckDBSuccess;
    struct dtl_io_duckdb_tracer *tracer = (struct dtl_io_duckdb_tracer *)base_tracer;
    size_t i;
    char const *column_name;

    for (i = 0; i < dtl_schema_get_num_columns(schema); i++) {
        column_name = dtl_schema_get_column_name(schema, i);

        state |= duckdb_append_varchar(tracer->output_appender, output_name);
        state |= duckdb_append_varchar(tracer->output_appender, column_name);
        state |= duckdb_append_int64(tracer->output_appender, array_ids[i]);
        state |= duckdb_appender_end_row(tracer->output_appender);
    }

    if (state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Could not append output details: %s", duckdb_appender_error(tracer->output_appender)));
        return DTL_STATUS_ERROR;
    }

    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_io_duckdb_tracer_record_trace(
    struct dtl_io_tracer *base_tracer,
    struct dtl_location start,
    struct dtl_location end,
    struct dtl_schema *schema,
    uint64_t *array_ids,
    struct dtl_error **error
) {
    duckdb_state state = DuckDBSuccess;
    struct dtl_io_duckdb_tracer *tracer = (struct dtl_io_duckdb_tracer *)base_tracer;
    size_t i;
    char const *column_name;

    for (i = 0; i < dtl_schema_get_num_columns(schema); i++) {
        column_name = dtl_schema_get_column_name(schema, i);

        state |= duckdb_append_varchar(tracer->trace_appender, start.filename);
        state |= duckdb_append_int64(tracer->trace_appender, start.offset);
        state |= duckdb_append_int64(tracer->trace_appender, end.offset);
        state |= duckdb_append_varchar(tracer->trace_appender, column_name);
        state |= duckdb_append_int64(tracer->trace_appender, array_ids[i]);
        state |= duckdb_appender_end_row(tracer->trace_appender);
    }

    if (state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Could not append trace details: %s", duckdb_appender_error(tracer->trace_appender)));
        return DTL_STATUS_ERROR;
    }

    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_io_duckdb_tracer_record_mapping(
    struct dtl_io_tracer *base_tracer,
    uint64_t src_array_id,
    uint64_t tgt_array_id,
    uint64_t src_index_array_id,
    uint64_t tgt_index_array_id,
    struct dtl_error **error
) {
    struct dtl_io_duckdb_tracer *tracer = (struct dtl_io_duckdb_tracer *)base_tracer;

    (void)tracer;
    (void)src_array_id;
    (void)tgt_array_id;
    (void)src_index_array_id;
    (void)tgt_index_array_id;
    (void)error;

    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_io_duckdb_tracer_record_value(
    struct dtl_io_tracer *base_tracer,
    uint64_t id,
    enum dtl_dtype dtype,
    size_t size,
    struct dtl_value *value,
    struct dtl_error **error
) {
    struct dtl_io_duckdb_tracer *tracer = (struct dtl_io_duckdb_tracer *)base_tracer;

    (void)tracer;
    (void)id;
    (void)dtype;
    (void)size;
    (void)value;
    (void)error;

    return DTL_STATUS_OK;
}

struct dtl_io_tracer *
dtl_io_duckdb_tracer_create(char const *path, struct dtl_error **error) {
    duckdb_database db;
    duckdb_connection db_con;
    duckdb_result db_result;
    duckdb_state db_state;
    char *errstr = NULL;
    struct dtl_io_duckdb_tracer *tracer = NULL;

    tracer = calloc(1, sizeof(struct dtl_io_duckdb_tracer));
    if (tracer == NULL) {
        dtl_set_error(error, dtl_error_create("Couldn't allocate memory for tracer"));
        return NULL;
    }

    tracer->base.record_source = dtl_io_duckdb_tracer_record_source;
    tracer->base.record_input = dtl_io_duckdb_tracer_record_input;
    tracer->base.record_output = dtl_io_duckdb_tracer_record_output;
    tracer->base.record_trace = dtl_io_duckdb_tracer_record_trace;
    tracer->base.record_mapping = dtl_io_duckdb_tracer_record_mapping;
    tracer->base.record_value = dtl_io_duckdb_tracer_record_value;

    db_state = duckdb_open_ext(path, &db, NULL, &errstr);
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Couldn't open %s: %s", path, errstr));
        duckdb_free(errstr);
        goto cleanup;
    }

    db_state = duckdb_connect(db, &db_con);
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Failed to open connection to duckdb"));
        duckdb_close(&db);
        goto cleanup;
    }

    // --- Source files --------------------------------------------------------------------------------------
    db_state = duckdb_query(
        db_con,
        "CREATE TABLE source (\n"
        "    text VARCHAR NOT NULL,\n"
        "    filename VARCHAR NOT NULL\n"
        ");",
        &db_result
    );
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Failed to create source file table: %s", duckdb_result_error(&db_result)));
        duckdb_destroy_result(&db_result);
        goto cleanup;
    }

    db_state = duckdb_appender_create(db_con, NULL, "source", &tracer->source_appender);
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Failed to create source appender"));
        goto cleanup;
    }

    // --- Inputs --------------------------------------------------------------------------------------------
    db_state = duckdb_query(
        db_con,
        "CREATE TABLE input (\n"
        "    input_name VARCHAR NOT NULL,\n"
        "    column_name VARCHAR NOT NULL,\n"
        "    expression INT NOT NULL,\n"
        ");",
        &db_result
    );
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Failed to create input table: %s", duckdb_result_error(&db_result)));
        duckdb_destroy_result(&db_result);
        goto cleanup;
    }

    db_state = duckdb_appender_create(db_con, NULL, "input", &tracer->input_appender);
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Failed to create input appender"));
        goto cleanup;
    }

    // --- Outputs -------------------------------------------------------------------------------------------

    db_state = duckdb_query(
        db_con,
        "CREATE TABLE output (\n"
        "    output_name VARCHAR NOT NULL,\n"
        "    column_name VARCHAR NOT NULL,\n"
        "    expression INT NOT NULL,\n"
        ");",
        &db_result
    );
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Failed to create output table: %s", duckdb_result_error(&db_result)));
        duckdb_destroy_result(&db_result);
        goto cleanup;
    }

    db_state = duckdb_appender_create(db_con, NULL, "output", &tracer->output_appender);
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Failed to create output appender"));
        goto cleanup;
    }

    // --- Traces --------------------------------------------------------------------------------------------

    db_state = duckdb_query(
        db_con,
        "CREATE TABLE trace(\n"
        "    filename VARCHAR NOT NULL,\n"
        "    start_offset INT NOT NULL,\n"
        "    end_offset INT NOT NULL,\n"
        "    column_name VARCHAR NOT NULL,\n"
        "    expression INT NOT NULL\n"
        ");",
        &db_result
    );
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Failed to create trace table: %s", duckdb_result_error(&db_result)));
        duckdb_destroy_result(&db_result);
        goto cleanup;
    }

    db_state = duckdb_appender_create(db_con, NULL, "trace", &tracer->trace_appender);
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Failed to create trace appender"));
        goto cleanup;
    }

    // --- Mappings ------------------------------------------------------------------------------------------

    db_state = duckdb_query(
        db_con,
        "CREATE TABLE mapping (\n"
        "    src_expression INT NOT NULL,\n" // Reference to expression table by index.
        "    tgt_expression INT NOT NULL,\n" // Reference to expression table by index.
        "    src_mapping_expression INT\n,"  // Reference to expression table by index.
        "    tgt_mapping_expression INT\n"   // Reference to expression table by index.
        ");",
        &db_result
    );
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Failed to create mapping table: %s", duckdb_result_error(&db_result)));
        duckdb_destroy_result(&db_result);
        goto cleanup;
    }

    db_state = duckdb_appender_create(db_con, NULL, "mapping", &tracer->mapping_appender);
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Failed to create mapping appender"));
        goto cleanup;
    }

    return &tracer->base;

cleanup:

    return NULL;
}

enum dtl_status
dtl_io_duckdb_tracer_destroy(struct dtl_io_tracer *base_tracer, struct dtl_error **error) {
    struct dtl_io_duckdb_tracer *tracer = (struct dtl_io_duckdb_tracer *)base_tracer;
    duckdb_state db_state;
    enum dtl_status result = DTL_STATUS_ERROR;

    db_state = duckdb_appender_close(tracer->mapping_appender);
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Error flushing mappings: %s", duckdb_appender_error(tracer->mapping_appender)));
        goto cleanup;
    }

    db_state = duckdb_appender_close(tracer->trace_appender);
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Error flushing traces: %s", duckdb_appender_error(tracer->trace_appender)));
        goto cleanup;
    }

    db_state = duckdb_appender_close(tracer->output_appender);
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Error flushing outputs: %s", duckdb_appender_error(tracer->output_appender)));
        goto cleanup;
    }

    db_state = duckdb_appender_close(tracer->input_appender);
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Error flushing inputs: %s", duckdb_appender_error(tracer->input_appender)));
        goto cleanup;
    }

    db_state = duckdb_appender_close(tracer->source_appender);
    if (db_state == DuckDBError) {
        dtl_set_error(error, dtl_error_create("Error flushing sources: %s", duckdb_appender_error(tracer->source_appender)));
        goto cleanup;
    }

    result = DTL_STATUS_OK;

cleanup:
    duckdb_appender_destroy(&tracer->mapping_appender);
    duckdb_appender_destroy(&tracer->trace_appender);
    duckdb_appender_destroy(&tracer->output_appender);
    duckdb_appender_destroy(&tracer->input_appender);
    duckdb_appender_destroy(&tracer->source_appender);

    duckdb_disconnect(&tracer->con);
    duckdb_close(&tracer->db);

    return result;
}
