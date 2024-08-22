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

struct dtl_eval_context_column {
    char const *table_name;
    char const *column_name;
    struct dtl_ir_ref shape;
    struct dtl_ir_ref value;
};

struct dtl_eval_context {
    struct dtl_ir_graph *graph;

    size_t num_columns;
    struct dtl_eval_context_column *columns;

    union dtl_value *values;
};

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
    return context->values[dtl_ir_ref_to_index(context->graph, column->shape)].as_index;
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
    num_rows = context->values[dtl_ir_ref_to_index(context->graph, column->shape)].as_index;

    assert(SIZE_MAX - size > offset);
    assert(offset + size <= num_rows);

    dtype = dtl_ir_expression_get_dtype(context->graph, column->value);
    assert(dtl_dtype_is_array_type(dtype));

    value = context->values[dtl_ir_ref_to_index(context->graph, column->value)];

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
dtl_export_table_destroy(struct dtl_io_table *table) {
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

    for (i = 0; i < context->num_columns; i++) {
        context_column = &context->columns[i];
        if (strcmp(context_column->table_name, table_name) != 0) {
            continue;
        }
        eval_table->num_columns += 1;
        eval_table->columns[eval_table->num_columns -= 1] = i;
    }

    status = dtl_io_exporter_export_table(exporter, table_name, &eval_table->base, error);

    free(eval_table);

    return status;
}

void
dtl_eval(
    char const *source,
    struct dtl_io_importer *importer,
    struct dtl_io_exporter *exporter,
    struct dtl_io_tracer *tracer
) {
    struct dtl_tokenizer *tokenizer;
    int parse_result;
    struct dtl_ast_node *root;
    struct dtl_ir_graph *graph;
    struct dtl_eval_context context;
    struct dtl_error *error;

    (void)exporter;
    (void)tracer;

    // === Parse Source Code =======================================================================

    tokenizer = dtl_tokenizer_create(source);
    parse_result = dtl_parser_parse(tokenizer, &root);
    assert(parse_result == 0);
    dtl_tokenizer_destroy(tokenizer);

    // === Compile AST to List of Tables Referencing IR Expressions ================================

    graph = dtl_ir_graph_create(1024, 1024);
    context = (struct dtl_eval_context){
        .graph = graph,
    };
    dtl_ast_to_ir(
        root, graph, importer,
        dtl_eval_ast_to_ir_column_callback,
        dtl_eval_ast_to_ir_trace_callback,
        &context,
        &error
    );

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
    // TODO
}
