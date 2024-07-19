#include "dtl-ast-to-ir.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "dtl-ast-find-imports.h"
#include "dtl-ast-node.h"
#include "dtl-ast.h"
#include "dtl-dtype.h"
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
dtl_ast_to_ir_scope_strip_namespaces(struct dtl_ast_to_ir_scope *scope) {
    size_t read_index;
    size_t write_index;
    struct dtl_ast_to_ir_scope_column column;

    assert(scope != NULL);

    write_index = 0;
    for (read_index = 0; read_index < scope->num_columns; read_index++) {
        column = scope->columns[read_index];
        if (column.namespace != NULL) {
            continue;
        }
        scope->columns[write_index] = column;
        write_index += 1;
    }
    scope->num_columns = write_index;
    return scope;
}

/* === Contexts ================================================================================= */

struct dtl_ast_to_ir_context {
    struct dtl_ir_graph *graph;
    struct dtl_io_importer *importer;
    void (*table_callback)(char const *, char const *, struct dtl_ir_ref, void *);
    void (*trace_callback)(struct dtl_location, struct dtl_location, char const *, struct dtl_ir_ref, void *);
    void *user_data;
};

void
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

void
dtl_ast_to_ir_context_export_table(
    struct dtl_ast_to_ir_context *context,
    char const *path,
    struct dtl_ast_to_ir_scope *table
) {
    (void)context;
    (void)path;
    (void)table;
}

/* === Compilation ============================================================================== */

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_compile_select_expression(
    struct dtl_ast_to_ir_context *context, struct dtl_ast_node *expression
) {
    assert(context != NULL);
    assert(expression != NULL);
    assert(dtl_ast_node_is_select_expression(expression));

    assert(false);
}

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_compile_import_expression(
    struct dtl_ast_to_ir_context *context, struct dtl_ast_node *expression
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

    io_table = dtl_io_importer_import_table(context->importer, path);

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
    struct dtl_ast_to_ir_context *context, struct dtl_ast_node *expression
) {
    assert(context != NULL);
    assert(expression != NULL);
    assert(dtl_ast_node_is_table_reference_expression(expression));

    assert(false);
}

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_compile_table_expression(
    struct dtl_ast_to_ir_context *context, struct dtl_ast_node *expression
) {
    assert(context != NULL);
    assert(expression != NULL);
    assert(dtl_ast_node_is_table_expression(expression));

    if (dtl_ast_node_is_select_expression(expression)) {
        return dtl_ast_to_ir_compile_select_expression(context, expression);
    }

    if (dtl_ast_node_is_import_expression(expression)) {
        return dtl_ast_to_ir_compile_import_expression(context, expression);
    }

    if (dtl_ast_node_is_table_reference_expression(expression)) {
        return dtl_ast_to_ir_compile_table_reference_expression(context, expression);
    }

    assert(false);
}

static void
dtl_ast_to_ir_compile_assignment_statement(
    struct dtl_ast_to_ir_context *context, struct dtl_ast_node *statement
) {
    struct dtl_ast_node *expression;
    struct dtl_ast_node *name;
    struct dtl_ast_to_ir_scope *expression_scope;

    assert(context != NULL);
    assert(statement != NULL);
    assert(dtl_ast_node_is_assignment_statement(statement));

    expression = dtl_ast_node_get_child(statement, 0);
    assert(dtl_ast_node_is_table_expression(expression));

    name = dtl_ast_node_get_child(statement, 1);
    assert(dtl_ast_node_is_table_name(name));

    expression_scope = dtl_ast_to_ir_compile_table_expression(context, expression);
    expression_scope = dtl_ast_to_ir_scope_strip_namespaces(expression_scope);

    dtl_ast_to_ir_context_trace_statement(
        context, dtl_ast_node_get_start(statement), dtl_ast_node_get_end(statement), expression_scope
    );

    // Add to globals.
    // Filter out old values with matching namespace.
    // Set num columns to end.
    // Append columns from result scope.
}

static void
dtl_ast_to_ir_compile_export_statement(
    struct dtl_ast_to_ir_context *context, struct dtl_ast_node *statement
) {
    struct dtl_ast_node *expression;
    struct dtl_ast_node *path;
    struct dtl_ast_to_ir_scope *expression_scope;

    assert(context != NULL);
    assert(statement != NULL);
    assert(dtl_ast_node_is_export_statement(statement));

    expression = dtl_ast_node_get_child(statement, 0);
    assert(dtl_ast_node_is_table_expression(expression));

    path = dtl_ast_node_get_child(statement, 1);
    assert(dtl_ast_node_is_string_literal(path));

    expression_scope = dtl_ast_to_ir_compile_table_expression(context, expression);
    expression_scope = dtl_ast_to_ir_scope_strip_namespaces(expression_scope);

    dtl_ast_to_ir_context_trace_statement(
        context, dtl_ast_node_get_start(statement), dtl_ast_node_get_end(statement), expression_scope
    );

    // TODO export_table()
}

static void
dtl_ast_to_ir_compile_statement(struct dtl_ast_to_ir_context *context, struct dtl_ast_node *statement) {
    assert(context != NULL);
    assert(statement != NULL);
    assert(dtl_ast_node_is_statement(statement));

    if (dtl_ast_node_is_assignment_statement(statement)) {
        dtl_ast_to_ir_compile_assignment_statement(context, statement);
        return;
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
        dtl_ast_to_ir_compile_export_statement(context, statement);
        return;
    }
    //    if (dtl_ast_node_is_begin_statement(statement)) {
    //        dtl_ast_to_ir_compile_begin_statement(context, statement);
    //        return;
    //    }

    assert(false);
}

void
dtl_ast_to_ir(
    struct dtl_ast_node *root,
    struct dtl_ir_graph *graph,
    struct dtl_io_importer *importer,
    void (*table_callback)(char const *, char const *, struct dtl_ir_ref, void *),
    void (*trace_callback)(struct dtl_location, struct dtl_location, char const *, struct dtl_ir_ref, void *),
    void *user_data
) {
    struct dtl_ast_to_ir_context *context;
    size_t i;
    struct dtl_ast_node *statement;

    context = calloc(1, sizeof(struct dtl_ast_to_ir_context));
    context->graph = graph;
    context->importer = importer;
    context->table_callback = table_callback;
    context->trace_callback = trace_callback;
    context->user_data = user_data;

    //    dtl_ast_find_imports();
    //    dtl_ast_find_imports(root, void (*callback)(struct dtl_ast_node *, void *), void *user_data) {

    for (i = 0; i < dtl_ast_node_get_num_children(root); i++) {
        statement = dtl_ast_node_get_child(root, i);
        dtl_ast_to_ir_compile_statement(context, statement);
    }
}
