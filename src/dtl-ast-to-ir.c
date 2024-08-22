#include "dtl-ast-to-ir.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "dtl-ast.h"
#include "dtl-dtype.h"
#include "dtl-error.h"
#include "dtl-io.h"
#include "dtl-ir.h"
#include "dtl-location.h"

/* === Scopes =================================================================================== */

struct dtl_ast_to_ir_scope_column {
    char const *name;
    char const *namespace;
    struct dtl_ir_ref expression;
};

struct dtl_ast_to_ir_scope {
    size_t num_columns;
    struct dtl_ast_to_ir_scope_column columns[];
};

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_scope_create(void) {
    return calloc(1, sizeof(struct dtl_ast_to_ir_scope));
}

static void
dtl_ast_to_ir_scope_destroy(struct dtl_ast_to_ir_scope *scope) {
    free(scope);
}

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_scope_dup(struct dtl_ast_to_ir_scope *src) {
    size_t size;
    struct dtl_ast_to_ir_scope *tgt;

    assert(src != NULL);

    size = (sizeof(struct dtl_ast_to_ir_scope) + sizeof(struct dtl_ast_to_ir_scope_column) * src->num_columns);
    tgt = calloc(1, size);
    memcpy(tgt, src, size);
    return tgt;
}

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_scope_add_unsafe(
    struct dtl_ast_to_ir_scope *scope,
    char const *name,
    char const *namespace,
    struct dtl_ir_ref expression
) {
    assert(scope != NULL);
    assert(name != NULL);

    scope->num_columns += 1;
    scope = realloc(
        scope,
        sizeof(struct dtl_ast_to_ir_scope) +
            sizeof(struct dtl_ast_to_ir_scope_column) * (scope->num_columns)
    );
    scope->columns[scope->num_columns - 1] = (struct dtl_ast_to_ir_scope_column){
        .name = name,
        .namespace = namespace,
        .expression = expression,
    };

    return scope;
}

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_scope_add(
    struct dtl_ast_to_ir_scope *scope,
    char const *name,
    char const *namespace,
    struct dtl_ir_ref expression
) {
    size_t i;

    assert(scope != NULL);
    assert(name != NULL);

    for (i = 0; i < scope->num_columns; i++) {
        // We assume that all strings have been interned.
        if (scope->columns[i].name != name) {
            continue;
        }
        if (scope->columns[i].namespace != namespace) {
            continue;
        }
        scope->columns[i].expression = expression;
        return scope;
    }

    return dtl_ast_to_ir_scope_add_unsafe(scope, name, namespace, expression);
}

static struct dtl_ir_ref
dtl_ast_to_ir_scope_lookup(
    struct dtl_ast_to_ir_scope *scope,
    char const *name,
    char const *namespace
) {
    (void)scope;
    (void)name;
    (void)namespace;
    return (struct dtl_ir_ref){0}; // TODO
}

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_scope_filter(
    struct dtl_ast_to_ir_scope *scope,
    bool (*should_filter)(char const *name, char const *namespace, struct dtl_ir_ref, void *),
    void *user_data
) {
    size_t read_index;
    size_t write_index;
    struct dtl_ast_to_ir_scope_column column;

    assert(scope != NULL);

    write_index = 0;
    for (read_index = 0; read_index < scope->num_columns; read_index++) {
        column = scope->columns[read_index];
        if (should_filter(column.name, column.namespace, column.expression, user_data)) {
            continue;
        }
        scope->columns[write_index] = column;
        write_index += 1;
    }
    scope->num_columns = write_index;
    return scope;
}

static bool
dtl_ast_to_ir_scope_filter_namespace_predicate(
    char const *name, char const *namespace, struct dtl_ir_ref expression, void *user_data
) {
    char const *target_namespace;

    (void)name;
    (void)expression;

    target_namespace = (char const *)user_data;

    // We assume that all strings have been interned.
    return namespace == target_namespace;
}

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_scope_filter_namespace(struct dtl_ast_to_ir_scope *scope, char const *namespace) {
    assert(scope != NULL);

    return dtl_ast_to_ir_scope_filter(
        scope, dtl_ast_to_ir_scope_filter_namespace_predicate, (void *)namespace
    );
}

static bool
dtl_ast_to_ir_scope_pick_namespace_predicate(
    char const *name, char const *namespace, struct dtl_ir_ref expression, void *user_data
) {
    char const *target_namespace;

    (void)name;
    (void)expression;

    target_namespace = (char const *)user_data;

    return namespace != target_namespace;
}

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_scope_pick_namespace(struct dtl_ast_to_ir_scope *scope, char const *namespace) {
    assert(scope != NULL);

    return dtl_ast_to_ir_scope_filter(scope, dtl_ast_to_ir_scope_pick_namespace_predicate, (void *)namespace);
}

/* === Contexts ================================================================================= */

struct dtl_ast_to_ir_context {
    struct dtl_ir_graph *graph;
    struct dtl_io_importer *importer;
    void (*column_callback)(char const *, char const *, struct dtl_ir_ref, void *);
    void (*trace_callback)(struct dtl_location, struct dtl_location, char const *, struct dtl_ir_ref, void *);
    void *user_data;

    struct dtl_ast_to_ir_scope *globals;
};

static void
dtl_ast_to_ir_context_trace_statement(
    struct dtl_ast_to_ir_context *context,
    struct dtl_location start,
    struct dtl_location end,
    struct dtl_ast_to_ir_scope *table
) {
    // TODO
    (void)context;
    (void)start;
    (void)end;
    (void)table;
}

static void
dtl_ast_to_ir_context_export_table(
    struct dtl_ast_to_ir_context *context,
    char const *path,
    struct dtl_ast_to_ir_scope *table
) {
    size_t i;
    char const *column_name;
    char const *column_namespace;
    struct dtl_ir_ref column_expression;

    for (i = 0; i < table->num_columns; i++) {
        column_name = table->columns[i].name;
        column_namespace = table->columns[i].namespace;
        column_expression = table->columns[i].expression;

        assert(column_namespace == NULL); // TODO might be safe to skip if not NULL.

        context->column_callback(path, column_name, column_expression, context->user_data);
    }
}

/* === Compilation ============================================================================== */

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_compile_select_expression(
    struct dtl_ast_to_ir_context *context, struct dtl_ast_node *expression, struct dtl_error **error
) {
    assert(context != NULL);
    assert(expression != NULL);
    assert(dtl_ast_node_is_select_expression(expression));

    (void)error;

    assert(false);
}

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_compile_import_expression(
    struct dtl_ast_to_ir_context *context,
    struct dtl_ast_node *expression,
    struct dtl_error **error
) {
    struct dtl_ast_node *path_expression;
    char const *path;
    struct dtl_io_table *io_table;
    struct dtl_ir_ref table;
    struct dtl_ir_ref table_shape;
    struct dtl_ast_to_ir_scope *table_scope;
    size_t i;
    char const *column_name;
    enum dtl_dtype column_dtype;
    struct dtl_ir_ref column;

    assert(context != NULL);
    assert(expression != NULL);
    assert(dtl_ast_node_is_import_expression(expression));

    path_expression = dtl_ast_import_expression_node_get_path(expression);
    assert(dtl_ast_node_is_string_literal(path_expression));

    path = dtl_ast_string_literal_node_get_value(path_expression);
    path = dtl_ir_graph_intern(context->graph, path);
    assert(path != NULL);

    io_table = dtl_io_importer_import_table(context->importer, path, error);
    if (io_table == NULL) {
        return NULL;
    }

    table = dtl_ir_open_table_expression_create(context->graph, path);
    table_shape = dtl_ir_table_shape_expression_create(context->graph, table);

    table_scope = dtl_ast_to_ir_scope_create();
    for (i = 0; i < dtl_io_table_get_num_columns(io_table); i++) {
        column_name = dtl_io_table_get_column_name(io_table, i);
        assert(column_name != NULL); // TODO validate properly.

        column_dtype = dtl_io_table_get_column_dtype(io_table, i);
        assert(dtl_dtype_is_array_type(column_dtype));

        column = dtl_ir_read_column_expression_create(
            context->graph, column_dtype, table_shape, table, column_name
        );
        // Refresh column name to get the interned string attached to the graph.  Graph outlives
        // table so this is safe.
        column_name = dtl_ir_read_column_expression_get_column_name(context->graph, column);

        table_scope = dtl_ast_to_ir_scope_add(table_scope, column_name, NULL, column);
    }

    dtl_io_table_destroy(io_table);

    return table_scope;
}

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_compile_table_reference_expression(
    struct dtl_ast_to_ir_context *context, struct dtl_ast_node *expression, struct dtl_error **error
) {
    struct dtl_ast_node *name_node;
    char const *table_name;
    struct dtl_ast_to_ir_scope *result;
    size_t i;

    (void)error;

    assert(context != NULL);
    assert(expression != NULL);
    assert(dtl_ast_node_is_table_reference_expression(expression));

    name_node = dtl_ast_table_reference_expression_node_get_name(expression);
    assert(dtl_ast_node_is_name(name_node));

    table_name = dtl_ast_name_node_get_value(name_node);
    table_name = dtl_ir_graph_intern(context->graph, table_name);
    assert(table_name != NULL);

    result = dtl_ast_to_ir_scope_dup(context->globals);
    result = dtl_ast_to_ir_scope_pick_namespace(result, table_name);

    for (i = 0; i < result->num_columns; i++) {
        result->columns[i].namespace = NULL;
    }

    return result;
}

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_compile_table_expression(
    struct dtl_ast_to_ir_context *context, struct dtl_ast_node *expression, struct dtl_error **error
) {
    assert(context != NULL);
    assert(expression != NULL);
    assert(dtl_ast_node_is_table_expression(expression));

    if (dtl_ast_node_is_select_expression(expression)) {
        return dtl_ast_to_ir_compile_select_expression(context, expression, error);
    }

    if (dtl_ast_node_is_import_expression(expression)) {
        return dtl_ast_to_ir_compile_import_expression(context, expression, error);
    }

    if (dtl_ast_node_is_table_reference_expression(expression)) {
        return dtl_ast_to_ir_compile_table_reference_expression(context, expression, error);
    }

    assert(false);
}

static enum dtl_status
dtl_ast_to_ir_compile_assignment_statement(
    struct dtl_ast_to_ir_context *context, struct dtl_ast_node *statement, struct dtl_error **error
) {
    struct dtl_ast_node *table_expression;
    struct dtl_ast_node *table_name_node;
    struct dtl_ast_node *name_node;
    char const *table_name;
    struct dtl_ast_to_ir_scope *expression_scope;
    size_t i;
    char const *column_name;
    struct dtl_ir_ref column_expression;

    assert(context != NULL);
    assert(statement != NULL);
    assert(dtl_ast_node_is_assignment_statement(statement));

    table_expression = dtl_ast_node_get_child(statement, 0);
    assert(dtl_ast_node_is_table_expression(table_expression));

    table_name_node = dtl_ast_assignment_statement_node_get_name(statement);
    assert(dtl_ast_node_is_table_name(table_name_node));

    name_node = dtl_ast_table_name_node_get_value(table_name_node);
    assert(dtl_ast_node_is_name(name_node));

    table_name = dtl_ast_name_node_get_value(name_node);
    table_name = dtl_ir_graph_intern(context->graph, table_name);
    assert(table_name != NULL);

    expression_scope = dtl_ast_to_ir_compile_table_expression(context, table_expression, error);
    if (expression_scope == NULL) {
        return DTL_STATUS_ERROR;
    }

    expression_scope = dtl_ast_to_ir_scope_pick_namespace(expression_scope, NULL);

    dtl_ast_to_ir_context_trace_statement(
        context, dtl_ast_node_get_start(statement), dtl_ast_node_get_end(statement), expression_scope
    );

    context->globals = dtl_ast_to_ir_scope_filter_namespace(context->globals, table_name);
    for (i = 0; i < expression_scope->num_columns; i++) {
        column_name = expression_scope->columns[i].name;
        column_expression = expression_scope->columns[i].expression;

        context->globals = dtl_ast_to_ir_scope_add_unsafe(
            context->globals, column_name, table_name, column_expression
        );
    }

    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_ast_to_ir_compile_export_statement(
    struct dtl_ast_to_ir_context *context, struct dtl_ast_node *statement, struct dtl_error **error
) {
    struct dtl_ast_node *expression;
    struct dtl_ast_node *path_expression;
    char const *path;
    struct dtl_ast_to_ir_scope *expression_scope;

    assert(context != NULL);
    assert(statement != NULL);
    assert(dtl_ast_node_is_export_statement(statement));

    expression = dtl_ast_node_get_child(statement, 0);
    assert(dtl_ast_node_is_table_expression(expression));

    path_expression = dtl_ast_node_get_child(statement, 1);
    assert(dtl_ast_node_is_string_literal(path_expression));

    path = dtl_ast_string_literal_node_get_value(path_expression);
    path = dtl_ir_graph_intern(context->graph, path);
    assert(path != NULL);

    expression_scope = dtl_ast_to_ir_compile_table_expression(context, expression, error);
    if (expression_scope == NULL) {
        return DTL_STATUS_ERROR;
    }
    expression_scope = dtl_ast_to_ir_scope_pick_namespace(expression_scope, NULL);

    dtl_ast_to_ir_context_trace_statement(
        context, dtl_ast_node_get_start(statement), dtl_ast_node_get_end(statement), expression_scope
    );

    dtl_ast_to_ir_context_export_table(context, path, expression_scope);

    return DTL_STATUS_OK;
}

static enum dtl_status
dtl_ast_to_ir_compile_statement(
    struct dtl_ast_to_ir_context *context,
    struct dtl_ast_node *statement,
    struct dtl_error **error
) {
    assert(context != NULL);
    assert(statement != NULL);
    assert(dtl_ast_node_is_statement(statement));

    if (dtl_ast_node_is_assignment_statement(statement)) {
        return dtl_ast_to_ir_compile_assignment_statement(context, statement, error);
    }

    //    if (dtl_ast_node_is_update_statement(statement)) {
    //        dtl_ast_to_ir_compile_update_statement(context, statement);
    //        return;
    //    }
    //
    //    if (dtl_ast_node_is_delete_statement(statement)) {
    //        dtl_ast_to_ir_compile_delete_statement(context, statement);
    //        return;
    //    }
    //
    //    if (dtl_ast_node_is_insert_statement(statement)) {
    //        dtl_ast_to_ir_compile_insert_statement(context, statement);
    //        return;
    //    }

    if (dtl_ast_node_is_export_statement(statement)) {
        return dtl_ast_to_ir_compile_export_statement(context, statement, error);
    }
    //    if (dtl_ast_node_is_begin_statement(statement)) {
    //        dtl_ast_to_ir_compile_begin_statement(context, statement);
    //        return;
    //    }

    assert(false);
}

enum dtl_status
dtl_ast_to_ir(
    struct dtl_ast_node *root,
    struct dtl_ir_graph *graph,
    struct dtl_io_importer *importer,
    void (*column_callback)(char const *, char const *, struct dtl_ir_ref, void *),
    void (*trace_callback)(struct dtl_location, struct dtl_location, char const *, struct dtl_ir_ref, void *),
    void *user_data,
    struct dtl_error **error
) {
    struct dtl_ast_to_ir_context *context;
    size_t i;
    struct dtl_ast_node *statements;
    struct dtl_ast_node *statement;
    enum dtl_status status;

    context = calloc(1, sizeof(struct dtl_ast_to_ir_context));
    context->graph = graph;
    context->importer = importer;
    context->column_callback = column_callback;
    context->trace_callback = trace_callback;
    context->user_data = user_data;
    context->globals = dtl_ast_to_ir_scope_create();

    //    dtl_ast_find_imports();
    //    dtl_ast_find_imports(root, void (*callback)(struct dtl_ast_node *, void *), void *user_data) {

    statements = dtl_ast_script_node_get_statements(root);
    for (i = 0; i < dtl_ast_statement_list_node_get_num_statements(statements); i++) {
        statement = dtl_ast_statement_list_node_get_statement(statements, i);
        status = dtl_ast_to_ir_compile_statement(context, statement, error);
        if (status != DTL_STATUS_OK) {
            return status;
        }
    }

    return DTL_STATUS_OK;
}
