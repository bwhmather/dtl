#include "dtl-eval.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dtl-ast-to-ir.h"
#include "dtl-ast.h"
#include "dtl-bool-array.h"
#include "dtl-dtype.h"
#include "dtl-error.h"
#include "dtl-index-array.h"
#include "dtl-io.h"
#include "dtl-ir-viz.h"
#include "dtl-ir.h"
#include "dtl-location.h"
#include "dtl-parser.h"
#include "dtl-schema.h"
#include "dtl-tokenizer.h"
#include "dtl-value.h"

/* === Context ================================================================================== */

struct dtl_eval_context_import {
    char const *name;
    struct dtl_io_table *table;
};

struct dtl_eval_context_export {
    char const *name;
    struct dtl_schema *schema;
    struct dtl_ir_ref *expressions;
};

struct dtl_eval_context_trace {
    struct dtl_location start;
    struct dtl_location end;
    struct dtl_schema *schema;
    struct dtl_ir_ref *expressions;
};

struct dtl_eval_context {
    struct dtl_io_importer *importer;
    struct dtl_io_exporter *exporter;
    struct dtl_io_tracer *tracer;

    struct dtl_ir_graph *graph;

    size_t num_imports;
    struct dtl_eval_context_import *imports;

    size_t num_exports;
    struct dtl_eval_context_export *exports;

    size_t num_traces;
    struct dtl_eval_context_trace *traces;

    struct dtl_value *values;
};

/* --- Load ------------------------------------------------------------------------------------- */

static size_t
dtl_eval_context_load_index(struct dtl_eval_context *context, struct dtl_ir_ref expression) {
    size_t index;
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_INDEX);
    index = dtl_ir_ref_to_index(context->graph, expression);
    return dtl_value_get_index(&context->values[index]);
}

static void *
dtl_eval_context_load_bool_array(struct dtl_eval_context *context, struct dtl_ir_ref expression) {
    size_t index;
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_BOOL_ARRAY);
    index = dtl_ir_ref_to_index(context->graph, expression);
    return dtl_value_get_bool_array(&context->values[index]);
}

static int64_t *
dtl_eval_context_load_int64_array(struct dtl_eval_context *context, struct dtl_ir_ref expression) {
    size_t index;
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_INT64_ARRAY);
    index = dtl_ir_ref_to_index(context->graph, expression);
    return dtl_value_get_int64_array(&context->values[index]);
}

/*
static size_t *
dtl_eval_context_load_index_array(struct dtl_eval_context *context, struct dtl_ir_ref expression) {
    size_t index;
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_INDEX_ARRAY);
    index = dtl_ir_ref_to_index(context->graph, expression);
    return dtl_value_get_index_array(&context->values[index]);
}

static double *
dtl_eval_context_load_double_array(struct dtl_eval_context *context, struct dtl_ir_ref expression) {
    size_t index;
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_DOUBLE_ARRAY);
    index = dtl_ir_ref_to_index(context->graph, expression);
    return dtl_value_get_double_array(&context->values[index]);
}

static char **
dtl_eval_context_load_string_array(struct dtl_eval_context *context, struct dtl_ir_ref expression) {
    size_t index;
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_STRING_ARRAY);
    index = dtl_ir_ref_to_index(context->graph, expression);
    return dtl_value_get_string_array(&context->values[index]);
}
*/

/* --- Store ------------------------------------------------------------------------------------ */

static void
dtl_eval_context_store_index(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    size_t value
) {
    size_t index;
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_INDEX);
    index = dtl_ir_ref_to_index(context->graph, expression);
    dtl_value_set_index(&context->values[index], value);
}

static void
dtl_eval_context_store_bool_array(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    void *array
) {
    size_t index;
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_BOOL_ARRAY);
    index = dtl_ir_ref_to_index(context->graph, expression);
    dtl_value_take_bool_array(&context->values[index], array);
}

static void
dtl_eval_context_store_int64_array(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    int64_t *array
) {
    size_t index;
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_INT64_ARRAY);
    index = dtl_ir_ref_to_index(context->graph, expression);
    dtl_value_take_int64_array(&context->values[index], array);
}

static void
dtl_eval_context_store_index_array(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    size_t *array
) {
    size_t index;
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_INDEX_ARRAY);
    index = dtl_ir_ref_to_index(context->graph, expression);
    dtl_value_take_index_array(&context->values[index], array);
}

/* --- Clear ------------------------------------------------------------------------------------ */

static void
dtl_eval_context_clear(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression
) {
    // TODO context values array is _sort of_ type erased.  This breaks that property.
    size_t index;
    struct dtl_value *value;

    index = dtl_ir_ref_to_index(context->graph, expression);
    value = &context->values[index];

    switch (dtl_ir_expression_get_dtype(context->graph, expression)) {
    case DTL_DTYPE_BOOL:
        dtl_value_clear_bool(value);
        break;
    case DTL_DTYPE_INT64:
        dtl_value_clear_int64(value);
        break;
    case DTL_DTYPE_DOUBLE:
        dtl_value_clear_double(value);
        break;
    case DTL_DTYPE_STRING:
        dtl_value_clear_string(value);
        break;
    case DTL_DTYPE_INDEX:
        dtl_value_clear_index(value);
        break;
    case DTL_DTYPE_BOOL_ARRAY:
        free(value->as_bool_array);
        value->as_bool_array = NULL;
        break;
    case DTL_DTYPE_INT64_ARRAY:
        free(value->as_int64_array);
        value->as_int64_array = NULL;
        break;
    case DTL_DTYPE_DOUBLE_ARRAY:
        free(value->as_double_array);
        value->as_double_array = NULL;
        break;
    case DTL_DTYPE_STRING_ARRAY:
        assert(false);
        break;
    case DTL_DTYPE_INDEX_ARRAY:
        free(value->as_index_array);
        value->as_index_array = NULL;
        break;
    }
}

/* === Compilation ============================================================================== */

static struct dtl_schema *
dtl_eval_ast_to_ir_import_callback(
    char const *table_name,
    struct dtl_error **error,
    void *user_data
) {
    struct dtl_eval_context *context = (struct dtl_eval_context *)user_data;
    size_t i;
    struct dtl_eval_context_import *candidate_import;
    struct dtl_eval_context_import *import = NULL;

    assert(table_name != NULL);
    assert(context != NULL);

    for (i = 0; i < context->num_imports; i++) {
        candidate_import = &context->imports[i];

        if (candidate_import->name == table_name) { // Name _should_ be interned.
            import = candidate_import;
            break;
        }
    }
    if (import == NULL) {
        context->imports = realloc(
            context->imports, sizeof(struct dtl_eval_context_import) * context->num_imports + 1
        );
        import = &context->imports[context->num_imports];

        import->name = table_name;
        import->table = dtl_io_importer_import_table(context->importer, table_name, error);
        if (import->table == NULL) {
            return NULL;
        }

        context->num_imports += 1;
    }

    return dtl_io_table_get_schema(import->table);
}

static void
dtl_eval_ast_to_ir_export_callback(
    char const *table_name,
    struct dtl_schema *schema,
    struct dtl_ir_ref *expressions,
    void *user_data
) {
    struct dtl_eval_context *context = (struct dtl_eval_context *)user_data;
    struct dtl_eval_context_export *export;

    assert(context != NULL);
    assert(schema != NULL);

    context->exports = realloc(context->exports, sizeof(struct dtl_eval_context_export) * (context->num_exports + 1));
    context->num_exports += 1;

    export = &context->exports[context->num_exports - 1];
    export->name = table_name;
    export->schema = schema;
    export->expressions = expressions;
}

static void
dtl_eval_ast_to_ir_trace_callback(
    struct dtl_location start,
    struct dtl_location end,
    struct dtl_schema *schema,
    struct dtl_ir_ref *expressions,
    void *user_data
) {
    struct dtl_eval_context *context = (struct dtl_eval_context *)user_data;
    struct dtl_eval_context_trace *trace;

    assert(context != NULL);

    context->traces = realloc(context->traces, sizeof(struct dtl_eval_context_trace) * (context->num_traces + 1));
    context->num_traces += 1;

    trace = &context->traces[context->num_traces - 1];
    trace->start = start;
    trace->end = end;
    trace->schema = schema;
    trace->expressions = expressions;
}

/* === Operations =============================================================================== */

/* --- Import Operations ------------------------------------------------------------------------ */

static enum dtl_status
dtl_eval_table_shape_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref table_expression;
    size_t table_index;
    struct dtl_io_table *table;
    size_t table_size;

    (void)error;

    assert(context != NULL);
    assert(dtl_ir_is_table_shape_expression(context->graph, expression));

    table_expression = dtl_ir_table_shape_expression_get_table(context->graph, expression);
    table_index = dtl_eval_context_load_index(context, table_expression);

    assert(table_index < context->num_imports);
    table = context->imports[table_index].table;

    table_size = dtl_io_table_get_num_rows(table);

    dtl_eval_context_store_index(context, expression, table_size);
    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_eval_open_table_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    size_t i;
    char const *path;

    (void)error;

    assert(context != NULL);
    assert(dtl_ir_is_open_table_expression(context->graph, expression));

    path = dtl_ir_open_table_expression_get_path(context->graph, expression);
    for (i = 0; i < context->num_imports; i++) {
        if (path == context->imports[i].name) { // Interned.
            dtl_eval_context_store_index(context, expression, i);
            return DTL_STATUS_OK;
        }
    }

    assert(false); // Should not be possible.
}

static enum dtl_status
dtl_eval_read_column_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref table_expression;
    size_t table_index;
    struct dtl_io_table *table;
    struct dtl_schema *schema;
    char const *column_name;
    size_t i;
    struct dtl_value value = {0};
    enum dtl_status status;

    assert(context != NULL);
    assert(dtl_ir_is_read_column_expression(context->graph, expression));

    table_expression = dtl_ir_read_column_expression_get_table(context->graph, expression);
    table_index = dtl_eval_context_load_index(context, table_expression);

    assert(table_index < context->num_imports);
    table = context->imports[table_index].table;

    schema = dtl_io_table_get_schema(table);

    column_name = dtl_ir_read_column_expression_get_column_name(context->graph, expression);

    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_INT64_ARRAY); // TODO

    for (i = 0; i < dtl_schema_get_num_columns(schema); i++) {
        if (strcmp(dtl_schema_get_column_name(schema, i), column_name) == 0) {
            status = dtl_io_table_read_column_data(table, i, &value, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }

            break;
        }
    }

    dtl_eval_context_store_int64_array(context, expression, value.as_int64_array);
    return DTL_STATUS_OK;
}

/* --- Filtering Operations ---------------------------------------------------------------------- */

static enum dtl_status
dtl_eval_where_shape_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref mask_expression;
    void *mask_data;
    struct dtl_ir_ref mask_shape_expression;
    size_t mask_shape;
    size_t shape;

    (void)error;

    assert(dtl_ir_is_where_shape_expression(context->graph, expression));

    mask_expression = dtl_ir_where_shape_expression_get_mask(context->graph, expression);
    mask_data = dtl_eval_context_load_bool_array(context, mask_expression);

    mask_shape_expression = dtl_ir_array_expression_get_shape(context->graph, mask_expression);
    mask_shape = dtl_eval_context_load_index(context, mask_shape_expression);

    shape = dtl_bool_array_sum(mask_data, mask_shape);

    dtl_eval_context_store_index(context, expression, shape);
    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_eval_where_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref shape_expression;
    size_t shape;
    struct dtl_ir_ref mask_expression;
    void *mask_data;
    struct dtl_ir_ref source_expression;
    int64_t *source_data;
    int64_t *data;
    size_t cursor;
    size_t i;

    (void)error;

    assert(dtl_ir_is_where_expression(context->graph, expression));

    shape_expression = dtl_ir_array_expression_get_shape(context->graph, expression);
    shape = dtl_eval_context_load_index(context, shape_expression);

    mask_expression = dtl_ir_where_expression_get_mask(context->graph, expression);
    mask_data = dtl_eval_context_load_bool_array(context, mask_expression);

    source_expression = dtl_ir_where_expression_get_source(context->graph, expression);
    source_data = dtl_eval_context_load_int64_array(context, source_expression); // TODO

    data = calloc(shape, sizeof(int64_t)); // TODO

    cursor = 0;
    for (i = 0; i < shape; i++) {
        if (dtl_bool_array_get(mask_data, i)) {
            data[cursor] = source_data[i];
            cursor += 1;
        }
    }

    dtl_eval_context_store_int64_array(context, expression, data);
    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_eval_join_shape_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref left_shape_expression;
    size_t left_shape;
    struct dtl_ir_ref right_shape_expression;
    size_t right_shape;
    size_t shape;

    (void)error;

    assert(dtl_ir_is_join_shape_expression(context->graph, expression));

    left_shape_expression = dtl_ir_join_shape_expression_get_left(context->graph, expression);
    left_shape = dtl_eval_context_load_index(context, left_shape_expression);

    right_shape_expression = dtl_ir_join_shape_expression_get_right(context->graph, expression);
    right_shape = dtl_eval_context_load_index(context, right_shape_expression);

    shape = left_shape * right_shape;

    dtl_eval_context_store_index(context, expression, shape);
    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_eval_join_left_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref shape_expression;
    size_t shape;
    struct dtl_ir_ref left_shape_expression;
    size_t left_shape;
    struct dtl_ir_ref right_shape_expression;
    size_t right_shape;
    size_t *output;
    size_t left_index;
    size_t right_index;

    (void)error;

    shape_expression = dtl_ir_array_expression_get_shape(context->graph, expression);
    shape = dtl_eval_context_load_index(context, shape_expression);

    left_shape_expression = dtl_ir_join_left_expression_left_shape(context->graph, expression);
    left_shape = dtl_eval_context_load_index(context, left_shape_expression);

    right_shape_expression = dtl_ir_join_left_expression_right_shape(context->graph, expression);
    right_shape = dtl_eval_context_load_index(context, right_shape_expression);

    assert(left_shape * right_shape == shape);

    output = dtl_index_array_create(shape);

    for (left_index = 0; left_index < left_shape; left_index++) {
        for (right_index = 0; right_index < right_shape; right_index++) {
            dtl_index_array_set(output, left_shape * right_index + left_index, left_index);
        }
    }

    dtl_eval_context_store_index_array(context, expression, output);
    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_eval_join_right_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref shape_expression;
    size_t shape;
    struct dtl_ir_ref left_shape_expression;
    size_t left_shape;
    struct dtl_ir_ref right_shape_expression;
    size_t right_shape;
    size_t *output;
    size_t left_index;
    size_t right_index;

    (void)error;

    shape_expression = dtl_ir_array_expression_get_shape(context->graph, expression);
    shape = dtl_eval_context_load_index(context, shape_expression);

    left_shape_expression = dtl_ir_join_left_expression_left_shape(context->graph, expression);
    left_shape = dtl_eval_context_load_index(context, left_shape_expression);

    right_shape_expression = dtl_ir_join_left_expression_right_shape(context->graph, expression);
    right_shape = dtl_eval_context_load_index(context, right_shape_expression);

    assert(left_shape * right_shape == shape);

    output = dtl_index_array_create(shape);

    for (left_index = 0; left_index < left_shape; left_index++) {
        for (right_index = 0; right_index < right_shape; right_index++) {
            dtl_index_array_set(output, left_shape * right_index + left_index, right_index);
        }
    }

    dtl_eval_context_store_index_array(context, expression, output);
    return DTL_STATUS_OK;
}

/* --- Binary Operations ------------------------------------------------------------------------ */

static enum dtl_status
dtl_eval_equal_to_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref shape_expression;
    size_t shape;
    struct dtl_ir_ref left_expression;
    int64_t *left_data;
    struct dtl_ir_ref right_expression;
    int64_t *right_data;

    (void)error;

    assert(dtl_ir_is_equal_to_expression(context->graph, expression));
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_BOOL_ARRAY);

    shape_expression = dtl_ir_array_expression_get_shape(context->graph, expression);
    shape = dtl_eval_context_load_index(context, shape_expression);

    left_expression = dtl_ir_equal_to_expression_left(context->graph, expression);
    left_data = dtl_eval_context_load_int64_array(context, left_expression);

    right_expression = dtl_ir_equal_to_expression_right(context->graph, expression);
    right_data = dtl_eval_context_load_int64_array(context, right_expression);

    void *data = dtl_bool_array_create(shape);

    for (size_t i = 0; i < shape; i++) {
        dtl_bool_array_set(data, i, left_data[i] == right_data[i]);
    }

    dtl_eval_context_store_bool_array(context, expression, data);
    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_eval_less_than_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref shape_expression;
    size_t shape;
    struct dtl_ir_ref left_expression;
    int64_t *left_data;
    struct dtl_ir_ref right_expression;
    int64_t *right_data;

    (void)error;

    assert(dtl_ir_is_less_than_expression(context->graph, expression));
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_BOOL_ARRAY);

    shape_expression = dtl_ir_array_expression_get_shape(context->graph, expression);
    shape = dtl_eval_context_load_index(context, shape_expression);

    left_expression = dtl_ir_less_than_expression_left(context->graph, expression);
    left_data = dtl_eval_context_load_int64_array(context, left_expression);

    right_expression = dtl_ir_less_than_expression_right(context->graph, expression);
    right_data = dtl_eval_context_load_int64_array(context, right_expression);

    void *data = dtl_bool_array_create(shape);

    for (size_t i = 0; i < shape; i++) {
        dtl_bool_array_set(data, i, left_data[i] < right_data[i]);
    }

    dtl_eval_context_store_bool_array(context, expression, data);
    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_eval_less_than_or_equal_to_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref shape_expression;
    size_t shape;
    struct dtl_ir_ref left_expression;
    int64_t *left_data;
    struct dtl_ir_ref right_expression;
    int64_t *right_data;

    (void)error;

    assert(dtl_ir_is_less_than_or_equal_to_expression(context->graph, expression));
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_BOOL_ARRAY);

    shape_expression = dtl_ir_array_expression_get_shape(context->graph, expression);
    shape = dtl_eval_context_load_index(context, shape_expression);

    left_expression = dtl_ir_less_than_or_equal_to_expression_left(context->graph, expression);
    left_data = dtl_eval_context_load_int64_array(context, left_expression);

    right_expression = dtl_ir_less_than_or_equal_to_expression_right(context->graph, expression);
    right_data = dtl_eval_context_load_int64_array(context, right_expression);

    void *data = dtl_bool_array_create(shape);

    for (size_t i = 0; i < shape; i++) {
        dtl_bool_array_set(data, i, left_data[i] <= right_data[i]);
    }

    dtl_eval_context_store_bool_array(context, expression, data);
    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_eval_greater_than_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref shape_expression;
    size_t shape;
    struct dtl_ir_ref left_expression;
    int64_t *left_data;
    struct dtl_ir_ref right_expression;
    int64_t *right_data;

    (void)error;

    assert(dtl_ir_is_greater_than_expression(context->graph, expression));
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_BOOL_ARRAY);

    shape_expression = dtl_ir_array_expression_get_shape(context->graph, expression);
    shape = dtl_eval_context_load_index(context, shape_expression);

    left_expression = dtl_ir_greater_than_expression_left(context->graph, expression);
    left_data = dtl_eval_context_load_int64_array(context, left_expression);

    right_expression = dtl_ir_greater_than_expression_right(context->graph, expression);
    right_data = dtl_eval_context_load_int64_array(context, right_expression);

    void *data = dtl_bool_array_create(shape);

    for (size_t i = 0; i < shape; i++) {
        dtl_bool_array_set(data, i, left_data[i] > right_data[i]);
    }

    dtl_eval_context_store_bool_array(context, expression, data);
    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_eval_greater_than_or_equal_to_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref shape_expression;
    size_t shape;
    struct dtl_ir_ref left_expression;
    int64_t *left_data;
    struct dtl_ir_ref right_expression;
    int64_t *right_data;

    (void)error;

    assert(dtl_ir_is_greater_than_or_equal_to_expression(context->graph, expression));
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_BOOL_ARRAY);

    shape_expression = dtl_ir_array_expression_get_shape(context->graph, expression);
    shape = dtl_eval_context_load_index(context, shape_expression);

    left_expression = dtl_ir_greater_than_or_equal_to_expression_left(context->graph, expression);
    left_data = dtl_eval_context_load_int64_array(context, left_expression);

    right_expression = dtl_ir_greater_than_or_equal_to_expression_right(context->graph, expression);
    right_data = dtl_eval_context_load_int64_array(context, right_expression);

    void *data = dtl_bool_array_create(shape);

    for (size_t i = 0; i < shape; i++) {
        dtl_bool_array_set(data, i, left_data[i] >= right_data[i]);
    }

    dtl_eval_context_store_bool_array(context, expression, data);
    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_eval_add_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref shape_expression;
    size_t shape;
    struct dtl_ir_ref left_expression;
    int64_t *left_data;
    struct dtl_ir_ref right_expression;
    int64_t *right_data;

    (void)error;

    assert(dtl_ir_is_add_expression(context->graph, expression));
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_INT64_ARRAY); // TODO

    shape_expression = dtl_ir_array_expression_get_shape(context->graph, expression);
    shape = dtl_eval_context_load_index(context, shape_expression);

    left_expression = dtl_ir_add_expression_left(context->graph, expression);
    left_data = dtl_eval_context_load_int64_array(context, left_expression);

    right_expression = dtl_ir_add_expression_right(context->graph, expression);
    right_data = dtl_eval_context_load_int64_array(context, right_expression);

    int64_t *data = calloc(shape, sizeof(int64_t));

    for (size_t j = 0; j < shape; j++) {
        data[j] = left_data[j] + right_data[j];
    }

    dtl_eval_context_store_int64_array(context, expression, data);
    return DTL_STATUS_OK;
}

/* === Tracing ================================================================================== */

static enum dtl_status
dtl_eval_tracing_record_import_metadata(struct dtl_eval_context *context, struct dtl_error **error) {
    (void)context;
    (void)error;

    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_eval_tracing_record_export_metadata(struct dtl_eval_context *context, struct dtl_error **error) {
    size_t i;
    size_t j;
    struct dtl_eval_context_export *export;
    uint64_t *array_ids;
    enum dtl_status status;

    if (context->tracer == NULL) {
        return DTL_STATUS_OK;
    }

    for (i = 0; i < context->num_exports; i++) {
        export = &context->exports[context->num_exports - 1];

        array_ids = calloc(dtl_schema_get_num_columns(export->schema), sizeof(uint64_t));
        for (j = 0; j < dtl_schema_get_num_columns(export->schema); j++) {
            array_ids[j] = dtl_ir_ref_to_index(context->graph, export->expressions[j]);
        }

        status = dtl_io_tracer_record_output(context->tracer, export->name, export->schema, array_ids, error);
        if (status != DTL_STATUS_OK) {
            return status;
        }
        free(array_ids);
    }

    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_eval_tracing_record_trace_metadata(struct dtl_eval_context *context, struct dtl_error **error) {
    size_t i;
    size_t j;
    struct dtl_eval_context_trace *trace;
    uint64_t *array_ids;
    enum dtl_status status;

    if (context->tracer == NULL) {
        return DTL_STATUS_OK;
    }

    for (i = 0; i < context->num_traces; i++) {
        trace = &context->traces[context->num_traces - 1];

        array_ids = calloc(dtl_schema_get_num_columns(trace->schema), sizeof(uint64_t));
        for (j = 0; j < dtl_schema_get_num_columns(trace->schema); j++) {
            array_ids[j] = dtl_ir_ref_to_index(context->graph, trace->expressions[j]);
        }

        status = dtl_io_tracer_record_trace(context->tracer, trace->start, trace->end, trace->schema, array_ids, error);
        if (status != DTL_STATUS_OK) {
            return status;
        }
        free(array_ids);
    }

    return DTL_STATUS_OK;
}

static void *
dtl_eval_tracing_mark_dependencies(struct dtl_eval_context *context) {
    size_t i;
    size_t j;
    struct dtl_eval_context_trace *trace;
    void *mask;

    if (context->tracer == NULL) {
        return NULL;
    }

    mask = dtl_bool_array_create(dtl_ir_graph_get_size(context->graph));

    for (i = 0; i < context->num_traces; i++) {
        trace = &context->traces[context->num_traces - 1];

        for (j = 0; j < dtl_schema_get_num_columns(trace->schema); j++) {
            dtl_bool_array_set(mask, dtl_ir_ref_to_index(context->graph, trace->expressions[j]), true);
        }
    }

    return mask;
}

/* === Eval ===================================================================================== */

enum dtl_status
dtl_eval(
    char const *source,
    char const *filename,
    struct dtl_io_importer *importer,
    struct dtl_io_exporter *exporter,
    struct dtl_io_tracer *tracer,
    struct dtl_error **error
) {
    struct dtl_tokenizer *tokenizer;
    int parse_result;
    enum dtl_status status;
    struct dtl_ast_node *root;
    struct dtl_ir_graph *graph;
    struct dtl_eval_context context;

    status = dtl_io_tracer_record_source(tracer, source, filename, error);
    if (status != DTL_STATUS_OK) {
        return status;
    }

    // === Parse Source Code =======================================================================

    tokenizer = dtl_tokenizer_create(source, filename);
    parse_result = dtl_parser_parse(tokenizer, &root, error);
    if (parse_result != 0) {
        return DTL_STATUS_ERROR;
    }
    dtl_tokenizer_destroy(tokenizer);

    // === Compile AST to List of Tables Referencing IR Expressions ================================

    graph = dtl_ir_graph_create();
    context = (struct dtl_eval_context){
        .importer = importer,
        .exporter = exporter,
        .tracer = tracer,
        .graph = graph,
    };
    status = dtl_ast_to_ir(
        root,
        graph,
        dtl_eval_ast_to_ir_import_callback,
        dtl_eval_ast_to_ir_export_callback,
        dtl_eval_ast_to_ir_trace_callback,
        &context,
        error
    );
    if (status != DTL_STATUS_OK) {
        dtl_ir_graph_destroy(graph);
        dtl_ast_node_destroy(root);
        return status;
    }

    dtl_ast_node_destroy(root);
    dtl_ir_viz(stderr, graph);

    // === Generate Mappings =======================================================================

    // Generate initial mappings for all reachable expressions pairs.
    // TODO

    // Merge mappings between expressions that aren't in the roots list.
    // TODO

    // === Optimise IR =============================================================================

    // Optimise regular joins to iteration on one side and lookup on index on other.
    // TODO.

    // Deduplicate IR expressions.
    // TODO.

    // Drop unreachable IR expressions.
    // TODO

    // After this point the expression graph is frozen.  We no longer need to update roots.

    // === Compile Reachable Expressions to Command List ===========================================
    // TODO

    // === Inject Commands to Export Tables ========================================================
    // TODO

    // === Setup Tracing ===========================================================================
    status = dtl_eval_tracing_record_import_metadata(&context, error);
    if (status != DTL_STATUS_OK) {
        return status;
    }

    status = dtl_eval_tracing_record_export_metadata(&context, error);
    if (status != DTL_STATUS_OK) {
        return status;
    }

    status = dtl_eval_tracing_record_trace_metadata(&context, error);
    if (status != DTL_STATUS_OK) {
        return status;
    }

    void *traced_expressions = dtl_eval_tracing_mark_dependencies(&context);

    // === Inject Commands to Collect Arrays After Use =============================================
    // TODO

    // === Evaluate the Command List ===============================================================
    size_t num_expressions = dtl_ir_graph_get_size(graph);

    context.values = calloc(num_expressions, sizeof(struct dtl_value));
    for (size_t i = 0; i < num_expressions; i++) {
        struct dtl_ir_ref expression = dtl_ir_index_to_ref(graph, i);

        if (dtl_ir_is_table_shape_expression(graph, expression)) {
            status = dtl_eval_table_shape_expression(&context, expression, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }
            continue;
        }

        if (dtl_ir_is_where_shape_expression(graph, expression)) {
            status = dtl_eval_where_shape_expression(&context, expression, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }
            continue;
        }

        if (dtl_ir_is_join_shape_expression(graph, expression)) {
            status = dtl_eval_join_shape_expression(&context, expression, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }
            continue;
        }

        if (dtl_ir_is_int64_constant_expression(graph, expression)) {
            assert(false); // Not implemented.
        }

        if (dtl_ir_is_double_constant_expression(graph, expression)) {
            assert(false); // Not implemented.
        }

        if (dtl_ir_is_open_table_expression(graph, expression)) {
            status = dtl_eval_open_table_expression(&context, expression, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }
            continue;
        }

        if (dtl_ir_is_read_column_expression(graph, expression)) {
            status = dtl_eval_read_column_expression(&context, expression, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }
            continue;
        }

        if (dtl_ir_is_where_expression(graph, expression)) {
            status = dtl_eval_where_expression(&context, expression, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }
            continue;
        }

        if (dtl_ir_is_pick_expression(graph, expression)) {
            assert(false); // Not implemented.
        }

        if (dtl_ir_is_index_expression(graph, expression)) {
            assert(false); // Not implemented.
        }

        if (dtl_ir_is_join_left_expression(graph, expression)) {
            status = dtl_eval_join_left_expression(&context, expression, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }
            continue;
        }

        if (dtl_ir_is_join_right_expression(graph, expression)) {
            status = dtl_eval_join_right_expression(&context, expression, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }
            continue;
        }

        if (dtl_ir_is_equal_to_expression(graph, expression)) {
            status = dtl_eval_equal_to_expression(&context, expression, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }
            continue;
        }

        if (dtl_ir_is_less_than_expression(graph, expression)) {
            status = dtl_eval_less_than_expression(&context, expression, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }
            continue;
        }

        if (dtl_ir_is_less_than_or_equal_to_expression(graph, expression)) {
            status = dtl_eval_less_than_or_equal_to_expression(&context, expression, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }
            continue;
        }

        if (dtl_ir_is_greater_than_expression(graph, expression)) {
            status = dtl_eval_greater_than_expression(&context, expression, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }
            continue;
        }

        if (dtl_ir_is_greater_than_or_equal_to_expression(graph, expression)) {
            status = dtl_eval_greater_than_or_equal_to_expression(&context, expression, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }
            continue;
        }

        if (dtl_ir_is_add_expression(graph, expression)) {
            status = dtl_eval_add_expression(&context, expression, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }
            continue;
        }

        if (dtl_ir_is_subtract_expression(graph, expression)) {
            assert(false); // Not implemented.
        }

        if (dtl_ir_is_multiply_expression(graph, expression)) {
            assert(false); // Not implemented.
        }
        if (dtl_ir_is_divide_expression(graph, expression)) {
            assert(false); // Not implemented.
        }
        assert(false);
    }

    for (size_t i = 0; i < num_expressions; i++) {
        assert(context.tracer != NULL);
        assert(traced_expressions != NULL);
        if (traced_expressions == NULL || !dtl_bool_array_get(traced_expressions, i)) {
            continue;
        }

        struct dtl_ir_ref expression = dtl_ir_index_to_ref(graph, i);

        struct dtl_ir_ref shape_expression = dtl_ir_array_expression_get_shape(context.graph, expression);
        size_t num_rows = dtl_eval_context_load_index(&context, shape_expression);

        status = dtl_io_tracer_record_value(
            context.tracer,
            i,
            dtl_ir_expression_get_dtype(context.graph, expression),
            num_rows,
            &context.values[i],
            error
        );
        if (status != DTL_STATUS_OK) {
            return status;
        }
    }

    for (size_t i = 0; i < context.num_exports; i++) {
        struct dtl_eval_context_export *export;
        size_t num_cols;
        size_t num_rows;
        struct dtl_ir_ref shape_expression;
        struct dtl_value **values;
        size_t index;

        export = &context.exports[i];

        num_cols = dtl_schema_get_num_columns(export->schema);
        num_rows = 0;
        if (num_cols > 0) {
            shape_expression = dtl_ir_array_expression_get_shape(context.graph, export->expressions[0]);
            num_rows = dtl_eval_context_load_index(&context, shape_expression);
        }

        values = calloc(num_cols, sizeof(struct dtl_value *));
        for (size_t j = 0; j < num_cols; j++) {
            index = dtl_ir_ref_to_index(context.graph, export->expressions[j]);
            values[j] = &context.values[index];
        }

        status = dtl_io_exporter_export_table(
            exporter,
            export->name,
            export->schema,
            num_rows,
            values,
            error
        );
        if (status != DTL_STATUS_OK) {
            return status;
        }

        free(values);
    }

    // TODO
    for (size_t i = 0; i < num_expressions; i++) {
        struct dtl_ir_ref expression = dtl_ir_index_to_ref(graph, i);
        dtl_eval_context_clear(&context, expression);
    }
    free(context.values);

    for (size_t i = 0; i < context.num_imports; i++) {
        dtl_io_table_destroy(context.imports[i].table);
    }
    free(context.imports);

    for (size_t i = 0; i < context.num_exports; i++) {
        dtl_schema_destroy(context.exports[i].schema);
        free(context.exports[i].expressions);
    }

    for (size_t i = 0; i < context.num_traces; i++) {
        dtl_schema_destroy(context.traces[i].schema);
        free(context.traces[i].expressions);
    }

    dtl_ir_graph_destroy(graph);

    return DTL_STATUS_OK;
}
