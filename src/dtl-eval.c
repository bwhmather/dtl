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
#include "dtl-dtype.h"
#include "dtl-error.h"
#include "dtl-io.h"
#include "dtl-ir-viz.h"
#include "dtl-ir.h"
#include "dtl-location.h"
#include "dtl-parser.h"
#include "dtl-tokenizer.h"
#include "dtl-value.h"

/* === Context ================================================================================== */

struct dtl_eval_context_column {
    char const *table_name;
    char const *column_name;
    struct dtl_ir_ref shape;
    struct dtl_ir_ref value;
};

struct dtl_eval_context {
    struct dtl_io_importer *importer;
    struct dtl_io_exporter *exporter;

    struct dtl_ir_graph *graph;

    size_t num_columns;
    struct dtl_eval_context_column *columns;

    union dtl_value *values;
};

/* --- Load ------------------------------------------------------------------------------------- */

static union dtl_value
dtl_eval_context_load_value(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression
) {
    size_t index = dtl_ir_ref_to_index(context->graph, expression);
    return context->values[index];
}

static size_t
dtl_eval_context_load_index(struct dtl_eval_context *context, struct dtl_ir_ref expression) {
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_INDEX);
    return dtl_eval_context_load_value(context, expression).as_index;
}

static bool *
dtl_eval_context_load_bool_array(struct dtl_eval_context *context, struct dtl_ir_ref expression) {
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_BOOL_ARRAY);
    return dtl_eval_context_load_value(context, expression).as_bool_array;
}

static int64_t *
dtl_eval_context_load_int_array(struct dtl_eval_context *context, struct dtl_ir_ref expression) {
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_INT_ARRAY);
    return dtl_eval_context_load_value(context, expression).as_int_array;
}

static struct dtl_io_table *
dtl_eval_context_load_table(struct dtl_eval_context *context, struct dtl_ir_ref expression) {
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_TABLE);
    return dtl_eval_context_load_value(context, expression).as_table;
}

/* --- Store ------------------------------------------------------------------------------------ */

static void
dtl_eval_context_store_value(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    union dtl_value value
) {
    size_t index = dtl_ir_ref_to_index(context->graph, expression);
    context->values[index] = value;
}

static void
dtl_eval_context_store_index(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    size_t index
) {
    dtl_eval_context_store_value(context, expression, (union dtl_value){.as_index = index});
}

static void
dtl_eval_context_store_bool_array(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    bool *array
) {
    dtl_eval_context_store_value(context, expression, (union dtl_value){.as_bool_array = array});
}

static void
dtl_eval_context_store_int_array(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    int64_t *array
) {
    dtl_eval_context_store_value(context, expression, (union dtl_value){.as_int_array = array});
}

static void
dtl_eval_context_store_table(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_io_table *table
) {
    dtl_eval_context_store_value(context, expression, (union dtl_value){.as_table = table});
}

/* --- Clear ------------------------------------------------------------------------------------ */

static void
dtl_eval_context_clear(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression
) {
    size_t index = dtl_ir_ref_to_index(context->graph, expression);
    context->values[index].as_int = 0;
}

/* === Compilation ============================================================================== */

static void
dtl_eval_ast_to_ir_column_callback(
    char const *table_name,
    char const *column_name,
    struct dtl_ir_ref column_expression,
    void *user_data
) {
    struct dtl_eval_context *context = (struct dtl_eval_context *)user_data;
    struct dtl_ir_graph *graph;
    size_t column_index;

    assert(context != NULL);

    graph = context->graph;
    assert(graph != NULL);

    assert(dtl_ir_is_array_expression(graph, column_expression));

    context->num_columns += 1;
    context->columns = realloc(context->columns, sizeof(struct dtl_eval_context_column) * context->num_columns);

    column_index = context->num_columns - 1;
    context->columns[column_index].table_name = table_name;
    context->columns[column_index].column_name = column_name;
    context->columns[column_index].shape = dtl_ir_array_expression_get_shape(graph, column_expression);
    context->columns[column_index].value = column_expression;
}

static void
dtl_eval_ast_to_ir_trace_callback(
    struct dtl_location start,
    struct dtl_location end,
    char const *column_name,
    struct dtl_ir_ref column_expression,
    void *user_data
) {
    (void)start;
    (void)end;
    (void)column_name;
    (void)column_expression;
    (void)user_data;
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
    struct dtl_io_table *table;
    size_t table_size;

    (void)error;

    assert(context != NULL);
    assert(dtl_ir_is_table_shape_expression(context->graph, expression));

    table_expression = dtl_ir_table_shape_expression_get_table(context->graph, expression);
    table = dtl_eval_context_load_table(context, table_expression);

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
    char const *path;
    struct dtl_io_table *table;

    assert(context != NULL);
    assert(dtl_ir_is_open_table_expression(context->graph, expression));

    path = dtl_ir_open_table_expression_get_path(context->graph, expression);
    table = dtl_io_importer_import_table(context->importer, path, error);
    if (table == NULL) {
        return DTL_STATUS_ERROR;
    }

    dtl_eval_context_store_table(context, expression, table);
    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_eval_read_column_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref shape_expression;
    size_t shape;
    struct dtl_ir_ref table_expression;
    struct dtl_io_table *table;
    char const *column_name;
    int64_t *data;
    size_t i;
    enum dtl_status status;

    assert(context != NULL);
    assert(dtl_ir_is_read_column_expression(context->graph, expression));

    shape_expression = dtl_ir_array_expression_get_shape(context->graph, expression);
    shape = dtl_eval_context_load_index(context, shape_expression);

    table_expression = dtl_ir_read_column_expression_get_table(context->graph, expression);
    table = dtl_eval_context_load_table(context, table_expression);

    column_name = dtl_ir_read_column_expression_get_column_name(context->graph, expression);

    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_INT_ARRAY); // TODO
    data = calloc(shape, sizeof(uint64_t));

    for (i = 0; i < dtl_io_table_get_num_columns(table); i++) {
        if (strcmp(dtl_io_table_get_column_name(table, i), column_name) == 0) {
            status = dtl_io_table_get_column_data(table, i, data, 0, shape, error);
            if (status != DTL_STATUS_OK) {
                return status;
            }

            break;
        }
    }

    dtl_eval_context_store_int_array(context, expression, data);
    return DTL_STATUS_OK;
}

/* --- Export Operations ------------------------------------------------------------------------ */

struct dtl_eval_export_table {
    struct dtl_io_table base;

    struct dtl_eval_context *context;

    size_t num_columns;
    size_t columns[]; // Array of indexes into the context columns array.
};

static size_t
dtl_eval_export_table_get_num_rows(struct dtl_io_table *table) {
    struct dtl_eval_export_table *eval_table;
    struct dtl_eval_context *context;
    size_t context_column_index;
    struct dtl_eval_context_column *column;

    assert(table != NULL);
    eval_table = (struct dtl_eval_export_table *)table;

    context = eval_table->context;
    assert(context != NULL);

    if (eval_table->num_columns == 0) {
        return 0;
    }

    context_column_index = eval_table->columns[0];

    assert(context_column_index < context->num_columns);
    column = &context->columns[context_column_index];

    assert(dtl_ir_expression_get_dtype(context->graph, column->shape) == DTL_DTYPE_INDEX);
    return dtl_eval_context_load_index(context, column->shape);
}

static size_t
dtl_eval_export_table_get_num_columns(struct dtl_io_table *table) {
    struct dtl_eval_export_table *eval_table;

    assert(table != NULL);
    eval_table = (struct dtl_eval_export_table *)table;

    return eval_table->num_columns;
}

static char const *
dtl_eval_export_table_get_column_name(struct dtl_io_table *table, size_t table_column_index) {
    struct dtl_eval_export_table *eval_table;
    struct dtl_eval_context *context;
    size_t context_column_index;
    struct dtl_eval_context_column *column;

    assert(table != NULL);
    eval_table = (struct dtl_eval_export_table *)table;

    context = eval_table->context;
    assert(context != NULL);

    assert(table_column_index < eval_table->num_columns);
    context_column_index = eval_table->columns[table_column_index];

    assert(context_column_index < context->num_columns);
    column = &context->columns[context_column_index];

    return column->column_name;
}

static enum dtl_dtype
dtl_eval_export_table_get_column_dtype(struct dtl_io_table *table, size_t table_column_index) {
    struct dtl_eval_export_table *eval_table;
    struct dtl_eval_context *context;
    size_t context_column_index;
    struct dtl_eval_context_column *column;
    enum dtl_dtype dtype;

    assert(table != NULL);
    eval_table = (struct dtl_eval_export_table *)table;

    context = eval_table->context;
    assert(context != NULL);

    assert(table_column_index < eval_table->num_columns);
    context_column_index = eval_table->columns[table_column_index];

    assert(context_column_index < context->num_columns);
    column = &context->columns[context_column_index];

    dtype = dtl_ir_expression_get_dtype(context->graph, column->value);
    assert(dtl_dtype_is_array_type(dtype));

    return dtype;
}

static enum dtl_status
dtl_eval_export_table_get_column_data(
    struct dtl_io_table *table,
    size_t table_column_index,
    void *dest,
    size_t offset,
    size_t size,
    struct dtl_error **error
) {
    struct dtl_eval_export_table *eval_table;
    struct dtl_eval_context *context;
    size_t context_column_index;
    struct dtl_eval_context_column *column;
    enum dtl_dtype dtype;
    size_t num_rows;
    union dtl_value value;

    (void)error;

    assert(table != NULL);
    eval_table = (struct dtl_eval_export_table *)table;

    context = eval_table->context;
    assert(context != NULL);

    assert(table_column_index < eval_table->num_columns);
    context_column_index = eval_table->columns[table_column_index];

    assert(context_column_index < context->num_columns);
    column = &context->columns[context_column_index];

    assert(dtl_ir_expression_get_dtype(context->graph, column->shape) == DTL_DTYPE_INDEX);
    num_rows = dtl_eval_context_load_index(context, column->shape);

    assert(SIZE_MAX - size > offset);
    assert(offset + size <= num_rows);

    dtype = dtl_ir_expression_get_dtype(context->graph, column->value);
    assert(dtl_dtype_is_array_type(dtype));

    value = dtl_eval_context_load_value(context, column->value);

    switch (dtype) {
    case DTL_DTYPE_BOOL_ARRAY:
        assert(false); // TODO
        break;
    case DTL_DTYPE_INT_ARRAY:
        memcpy(dest, value.as_int_array + offset, sizeof(int64_t) * size);
        break;
    case DTL_DTYPE_DOUBLE_ARRAY:
        memcpy(dest, value.as_double_array + offset, sizeof(double) * size);
        break;
    case DTL_DTYPE_TEXT_ARRAY:
        memcpy(dest, value.as_text_array + offset, sizeof(char *) * size);
        break;
    case DTL_DTYPE_BYTES_ARRAY:
        memcpy(dest, value.as_pointer_array + offset, sizeof(void *) * size);
        break;
    case DTL_DTYPE_INDEX_ARRAY:
        memcpy(dest, value.as_index_array + offset, sizeof(size_t) * size);
        break;

    default:
        assert(false);
    }

    return DTL_STATUS_OK;
}

static void
dtl_eval_export_table_destroy(struct dtl_io_table *table) {
    (void)table;
    // Intentionally left blank.
}

static enum dtl_status
dtl_eval_export_table(
    struct dtl_eval_context *context,
    struct dtl_io_exporter *exporter,
    char const *table_name,
    struct dtl_error **error
) {
    struct dtl_eval_export_table *eval_table;
    size_t i;
    struct dtl_eval_context_column *context_column;
    enum dtl_status status;

    assert(context != NULL);
    assert(table_name != NULL);

    // Overallocated.  Could be shared.
    eval_table = malloc(sizeof(struct dtl_eval_export_table) + context->num_columns * sizeof(size_t));
    eval_table->base.get_num_rows = dtl_eval_export_table_get_num_rows;
    eval_table->base.get_num_columns = dtl_eval_export_table_get_num_columns;
    eval_table->base.get_column_name = dtl_eval_export_table_get_column_name;
    eval_table->base.get_column_dtype = dtl_eval_export_table_get_column_dtype;
    eval_table->base.get_column_data = dtl_eval_export_table_get_column_data;
    eval_table->base.destroy = dtl_eval_export_table_destroy;

    eval_table->context = context;

    eval_table->num_columns = 0;
    for (i = 0; i < context->num_columns; i++) {
        context_column = &context->columns[i];
        if (strcmp(context_column->table_name, table_name) != 0) {
            continue;
        }
        eval_table->num_columns += 1;
        eval_table->columns[eval_table->num_columns - 1] = i;
    }

    status = dtl_io_exporter_export_table(exporter, table_name, &eval_table->base, error);

    free(eval_table);

    return status;
}

/* --- Filtering Operations ---------------------------------------------------------------------- */

static enum dtl_status
dtl_eval_where_shape_expression(
    struct dtl_eval_context *context,
    struct dtl_ir_ref expression,
    struct dtl_error **error
) {
    struct dtl_ir_ref mask_expression;
    bool *mask_data;
    struct dtl_ir_ref mask_shape_expression;
    size_t mask_shape;
    size_t shape;
    size_t i;

    (void)error;

    assert(dtl_ir_is_where_shape_expression(context->graph, expression));

    mask_expression = dtl_ir_where_shape_expression_get_mask(context->graph, expression);
    mask_data = dtl_eval_context_load_bool_array(context, mask_expression);

    mask_shape_expression = dtl_ir_array_expression_get_shape(context->graph, mask_expression);
    mask_shape = dtl_eval_context_load_index(context, mask_shape_expression);

    shape = 0;
    for (i = 0; i < mask_shape; i++) {
        if (mask_data[i]) {
            shape++;
        }
    }

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
    bool *mask_data;
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
    source_data = dtl_eval_context_load_int_array(context, source_expression); // TODO

    data = calloc(shape, sizeof(int64_t)); // TODO

    cursor = 0;
    for (i = 0; i < shape; i++) {
        if (mask_data[i]) {
            data[cursor] = source_data[i];
            cursor += 1;
        }
    }

    dtl_eval_context_store_int_array(context, expression, data);
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
    left_data = dtl_eval_context_load_int_array(context, left_expression);

    right_expression = dtl_ir_equal_to_expression_right(context->graph, expression);
    right_data = dtl_eval_context_load_int_array(context, right_expression);

    bool *data = calloc(shape, sizeof(bool));

    for (size_t i = 0; i < shape; i++) {
        data[i] = left_data[i] == right_data[i];
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
    left_data = dtl_eval_context_load_int_array(context, left_expression);

    right_expression = dtl_ir_less_than_expression_right(context->graph, expression);
    right_data = dtl_eval_context_load_int_array(context, right_expression);

    bool *data = calloc(shape, sizeof(bool));

    for (size_t i = 0; i < shape; i++) {
        data[i] = left_data[i] < right_data[i];
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
    left_data = dtl_eval_context_load_int_array(context, left_expression);

    right_expression = dtl_ir_less_than_or_equal_to_expression_right(context->graph, expression);
    right_data = dtl_eval_context_load_int_array(context, right_expression);

    bool *data = calloc(shape, sizeof(bool));

    for (size_t i = 0; i < shape; i++) {
        data[i] = left_data[i] <= right_data[i];
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
    left_data = dtl_eval_context_load_int_array(context, left_expression);

    right_expression = dtl_ir_greater_than_expression_right(context->graph, expression);
    right_data = dtl_eval_context_load_int_array(context, right_expression);

    bool *data = calloc(shape, sizeof(bool));

    for (size_t i = 0; i < shape; i++) {
        data[i] = left_data[i] > right_data[i];
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
    left_data = dtl_eval_context_load_int_array(context, left_expression);

    right_expression = dtl_ir_greater_than_or_equal_to_expression_right(context->graph, expression);
    right_data = dtl_eval_context_load_int_array(context, right_expression);

    bool *data = calloc(shape, sizeof(bool));

    for (size_t i = 0; i < shape; i++) {
        data[i] = left_data[i] >= right_data[i];
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
    assert(dtl_ir_expression_get_dtype(context->graph, expression) == DTL_DTYPE_INT_ARRAY); // TODO

    shape_expression = dtl_ir_array_expression_get_shape(context->graph, expression);
    shape = dtl_eval_context_load_index(context, shape_expression);

    left_expression = dtl_ir_add_expression_left(context->graph, expression);
    left_data = dtl_eval_context_load_int_array(context, left_expression);

    right_expression = dtl_ir_add_expression_right(context->graph, expression);
    right_data = dtl_eval_context_load_int_array(context, right_expression);

    int64_t *data = calloc(shape, sizeof(uint64_t));

    for (size_t j = 0; j < shape; j++) {
        data[j] = left_data[j] + right_data[j];
    }

    dtl_eval_context_store_int_array(context, expression, data);
    return DTL_STATUS_OK;
}

/* === Eval ===================================================================================== */

enum dtl_status
dtl_eval(
    char const *source,
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

    (void)tracer;

    // === Parse Source Code =======================================================================

    tokenizer = dtl_tokenizer_create(source);
    parse_result = dtl_parser_parse(tokenizer, &root);
    assert(parse_result == 0);
    dtl_tokenizer_destroy(tokenizer);

    // === Compile AST to List of Tables Referencing IR Expressions ================================

    graph = dtl_ir_graph_create(1024, 1024);
    context = (struct dtl_eval_context){
        .importer = importer,
        .exporter = exporter,
        .graph = graph,
    };
    status = dtl_ast_to_ir(
        root, graph, importer,
        dtl_eval_ast_to_ir_column_callback,
        dtl_eval_ast_to_ir_trace_callback,
        &context,
        error
    );
    if (status != DTL_STATUS_OK) {
        return status;
    }

    dtl_ir_viz(stderr, graph);

    // === Optimise IR =============================================================================

    // Optimise regular joins to iteration on one side and lookup on index on other.
    // TODO.

    // Deduplicate IR expressions.
    // TODO.

    // Drop unreachable IR expressions.
    // TODO

    // After this point the expression graph is frozen.  We no longer need to update mappings.

    // === Generate Mappings =======================================================================

    // Generate initial mappings for all reachable expressions pairs.
    // TODO

    // Merge mappings between expressions that aren't in the roots list.
    // TODO

    // === Compile Reachable Expressions to Command List ===========================================
    // TODO

    // === Inject Commands to Export Tables ========================================================
    // TODO

    // === Setup Tracing ===========================================================================
    // TODO

    // === Inject Commands to Collect Arrays After Use =============================================
    // TODO

    // === Evaluate the Command List ===============================================================
    size_t num_expressions = dtl_ir_graph_get_size(graph);

    context.values = calloc(num_expressions, sizeof(union dtl_value));
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
            assert(false); // Not implemented.
        }

        if (dtl_ir_is_int_constant_expression(graph, expression)) {
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
            assert(false); // Not implemented.
        }

        if (dtl_ir_is_join_right_expression(graph, expression)) {
            assert(false); // Not implemented.
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

    // TODO
    dtl_eval_export_table(&context, exporter, "output", error);

    return DTL_STATUS_OK;
}
