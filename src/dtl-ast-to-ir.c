#include "dtl-ast-to-ir.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "dtl-ast.h"
#include "dtl-dtype.h"
#include "dtl-error.h"
#include "dtl-ir.h"
#include "dtl-location.h"
#include "dtl-schema.h"

/* === Error Handling =========================================================================== */

static void
dtl_ast_to_ir_shrink_error(struct dtl_error **error, struct dtl_ast_node *node) {
    struct dtl_location start;
    struct dtl_location end;

    assert(error != NULL);
    assert(node != NULL);

    start = dtl_ast_node_get_start(node);
    end = dtl_ast_node_get_end(node);

    dtl_error_shrink_location(*error, start, end);
}

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
    scope = realloc(scope, sizeof(struct dtl_ast_to_ir_scope) + sizeof(struct dtl_ast_to_ir_scope_column) * scope->num_columns);

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
    size_t i;

    assert(scope != NULL);
    assert(name != NULL);

    for (i = 0; i < scope->num_columns; i++) {
        if (scope->columns[i].name != name) {
            continue;
        }
        if (scope->columns[i].namespace != namespace) {
            continue;
        }

        return scope->columns[i].expression;
    }

    return DTL_IR_NULL_REF;
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

struct dtl_ast_to_ir_export {
    char const *name;
    struct dtl_schema *schema;
    struct dtl_ir_ref *expressions;
};

struct dtl_ast_to_ir_context {
    struct dtl_ir_graph *graph;
    struct dtl_schema *(*import_callback)(char const *, struct dtl_error **, void *);
    void (*export_callback)(char const *, struct dtl_schema *, struct dtl_ir_ref *, void *);
    void (*trace_callback)(struct dtl_location, struct dtl_location, struct dtl_schema *, struct dtl_ir_ref *, void *);
    void *user_data;

    struct dtl_ast_to_ir_scope *globals;

    size_t num_exports;
    struct dtl_ast_to_ir_export *exports;
};

static void
dtl_ast_to_ir_context_trace(
    struct dtl_ast_to_ir_context *context,
    struct dtl_location start,
    struct dtl_location end,
    struct dtl_ast_to_ir_scope *scope
) {
    struct dtl_schema *schema;
    struct dtl_ir_ref *expressions;
    size_t i;
    char const *column_name;
    // char const *column_namespace; TODO
    struct dtl_ir_ref column_expression;
    enum dtl_dtype column_dtype;

    assert(context != NULL);
    assert(start.filename == end.filename);
    assert(scope != NULL);

    if (context->trace_callback == NULL) {
        return;
    }

    schema = dtl_schema_create();
    expressions = calloc(scope->num_columns, sizeof(struct dtl_ir_ref));

    // TODO deduplicate where namespaced and unnamespaced versions of same column exist.
    for (i = 0; i < scope->num_columns; i++) {
        column_name = scope->columns[i].name;
        // column_namespace = scope->columns[i].namespace; TODO
        column_expression = scope->columns[i].expression;
        column_dtype = dtl_ir_expression_get_dtype(context->graph, column_expression);

        schema = dtl_schema_add_column(schema, column_name, column_dtype);
        expressions[i] = column_expression;
    }

    context->trace_callback(start, end, schema, expressions, context->user_data);
}

static void
dtl_ast_to_ir_context_export_table(
    struct dtl_ast_to_ir_context *context,
    char const *path,
    struct dtl_ast_to_ir_scope *table
) {
    struct dtl_schema *schema;
    struct dtl_ir_ref *expressions;
    size_t i;
    char const *column_name;
    char const *column_namespace;
    struct dtl_ir_ref column_expression;
    enum dtl_dtype column_dtype;
    struct dtl_ast_to_ir_export *candidate_export;
    struct dtl_ast_to_ir_export *export = NULL;

    schema = dtl_schema_create();
    expressions = calloc(table->num_columns, sizeof(struct dtl_ir_ref));

    for (i = 0; i < table->num_columns; i++) {
        column_name = table->columns[i].name;
        column_namespace = table->columns[i].namespace;
        column_expression = table->columns[i].expression;
        column_dtype = dtl_ir_expression_get_dtype(context->graph, column_expression);

        assert(column_namespace == NULL); // TODO might be safe to skip if not NULL.

        schema = dtl_schema_add_column(schema, column_name, column_dtype);
        expressions[i] = column_expression;
    }

    for (i = 0; i < context->num_exports; i++) {
        candidate_export = &context->exports[i];

        if (candidate_export->name == path) { // Name _should_ be interned.
            export = candidate_export;

            dtl_schema_destroy(export->schema);
            free(export->expressions);

            break;
        }
    }
    if (export == NULL) {
        context->num_exports += 1;
        context->exports = realloc(context->exports, sizeof(struct dtl_ast_to_ir_export) * context->num_exports);

        export = &context->exports[context->num_exports - 1];
    }

    export->name = path;
    export->schema = schema;
    export->expressions = expressions;
}

/* === Helpers ================================================================================== */

static struct dtl_ir_ref
dtl_ast_to_ir_scope_shape(struct dtl_ast_to_ir_context *context, struct dtl_ast_to_ir_scope *scope) {
    struct dtl_ir_ref first_column;

    assert(scope != NULL);
    assert(scope->num_columns > 0);

    first_column = scope->columns[0].expression;
    assert(dtl_ir_is_array_expression(context->graph, first_column));

    return dtl_ir_array_expression_get_shape(context->graph, first_column);
}

/* === Compilation ============================================================================== */

static struct dtl_ir_ref
dtl_ast_to_ir_compile_expression(
    struct dtl_ast_to_ir_context *context,
    struct dtl_ast_to_ir_scope *scope,
    struct dtl_ast_node *expression_node,
    struct dtl_error **error
);

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_compile_table_expression(
    struct dtl_ast_to_ir_context *context, struct dtl_ast_node *expression, struct dtl_error **error
);

static char const *
dtl_ast_to_ir_expression_name(struct dtl_ast_node *expression_node) {
    struct dtl_ast_node *reference_node;
    struct dtl_ast_node *column_name_node;

    assert(dtl_ast_node_is_expression(expression_node));

    if (dtl_ast_node_is_column_reference_expression(expression_node)) {
        reference_node = dtl_ast_column_reference_expression_node_get_name(expression_node);

        if (dtl_ast_node_is_unqualified_column_name(reference_node)) {
            column_name_node = dtl_ast_unqualified_column_name_get_column_name(reference_node);
            return dtl_ast_name_node_get_value(column_name_node);
        }

        if (dtl_ast_node_is_qualified_column_name(reference_node)) {
            column_name_node = dtl_ast_qualified_column_name_get_column_name(reference_node);
            return dtl_ast_name_node_get_value(column_name_node);
        }

        assert(false); // Unreachable.
    }

    return NULL;
}

static struct dtl_ir_ref
dtl_ast_to_ir_compile_column_reference_expression(
    struct dtl_ast_to_ir_context *context,
    struct dtl_ast_to_ir_scope *scope,
    struct dtl_ast_node *expression_node,
    struct dtl_error **error
) {
    struct dtl_ast_node *reference_node;
    struct dtl_ast_node *table_name_node;
    struct dtl_ast_node *column_name_node;
    struct dtl_ir_ref referenced_expression;
    char const *name;
    char const *namespace;

    assert(context != NULL);
    assert(dtl_ast_node_is_column_reference_expression(expression_node));

    reference_node = dtl_ast_column_reference_expression_node_get_name(expression_node);

    if (dtl_ast_node_is_unqualified_column_name(reference_node)) {
        column_name_node = dtl_ast_unqualified_column_name_get_column_name(reference_node);
        name = dtl_ast_name_node_get_value(column_name_node);
        namespace = NULL;
    }

    if (dtl_ast_node_is_qualified_column_name(reference_node)) {
        column_name_node = dtl_ast_qualified_column_name_get_column_name(reference_node);
        table_name_node = dtl_ast_qualified_column_name_get_table_name(reference_node);
        name = dtl_ast_name_node_get_value(column_name_node);
        namespace = dtl_ast_name_node_get_value(table_name_node);
    }

    name = dtl_ir_graph_intern(context->graph, name);
    namespace = dtl_ir_graph_intern(context->graph, namespace);

    referenced_expression = dtl_ast_to_ir_scope_lookup(scope, name, namespace);
    if (dtl_ir_ref_is_null(referenced_expression)) {
        if (namespace != NULL) {
            dtl_set_error(error, dtl_error_create("Could not resolve column '%s.%s'", namespace, name));
            dtl_ast_to_ir_shrink_error(error, expression_node);
        } else {
            dtl_set_error(error, dtl_error_create("Could not resolve column '%s'", name));
            dtl_ast_to_ir_shrink_error(error, expression_node);
        }
        return DTL_IR_NULL_REF;
    }

    return referenced_expression;
}

static struct dtl_ir_ref
dtl_ast_to_ir_compile_equal_to_expression(
    struct dtl_ast_to_ir_context *context,
    struct dtl_ast_to_ir_scope *scope,
    struct dtl_ast_node *expression_node,
    struct dtl_error **error
) {
    struct dtl_ast_node *left_node;
    struct dtl_ir_ref left_expression;
    struct dtl_ast_node *right_node;
    struct dtl_ir_ref right_expression;
    enum dtl_dtype left_dtype;
    enum dtl_dtype right_dtype;
    struct dtl_ir_ref left_shape;
    struct dtl_ir_ref right_shape;

    assert(context != NULL);
    assert(dtl_ast_node_is_equal_to_expression(expression_node));

    left_node = dtl_ast_equal_to_expression_node_get_left(expression_node);
    left_expression = dtl_ast_to_ir_compile_expression(context, scope, left_node, error);
    if (dtl_ir_ref_is_null(left_expression)) {
        return DTL_IR_NULL_REF;
    }

    right_node = dtl_ast_equal_to_expression_node_get_right(expression_node);
    right_expression = dtl_ast_to_ir_compile_expression(context, scope, right_node, error);
    if (dtl_ir_ref_is_null(right_expression)) {
        return DTL_IR_NULL_REF;
    }

    left_dtype = dtl_ir_expression_get_dtype(context->graph, left_expression);
    right_dtype = dtl_ir_expression_get_dtype(context->graph, right_expression);
    if (left_dtype != right_dtype) {
        dtl_set_error(error, dtl_error_create("mismatched shapes"));
        dtl_ast_to_ir_shrink_error(error, expression_node);
        return DTL_IR_NULL_REF;
    }

    left_shape = dtl_ir_array_expression_get_shape(context->graph, left_expression);
    right_shape = dtl_ir_array_expression_get_shape(context->graph, right_expression);
    if (!dtl_ir_ref_equal(context->graph, left_shape, right_shape)) {
        dtl_set_error(error, dtl_error_create("mismatched shapes"));
        dtl_ast_to_ir_shrink_error(error, expression_node);
        return DTL_IR_NULL_REF;
    }

    return dtl_ir_equal_to_expression_create(
        context->graph, left_shape, left_expression, right_expression
    );
}

static struct dtl_ir_ref
dtl_ast_to_ir_compile_less_than_expression(
    struct dtl_ast_to_ir_context *context,
    struct dtl_ast_to_ir_scope *scope,
    struct dtl_ast_node *expression_node,
    struct dtl_error **error
) {
    struct dtl_ast_node *left_node;
    struct dtl_ir_ref left_expression;
    struct dtl_ast_node *right_node;
    struct dtl_ir_ref right_expression;
    enum dtl_dtype left_dtype;
    enum dtl_dtype right_dtype;
    struct dtl_ir_ref left_shape;
    struct dtl_ir_ref right_shape;

    assert(context != NULL);
    assert(dtl_ast_node_is_less_than_expression(expression_node));

    left_node = dtl_ast_less_than_expression_node_get_left(expression_node);
    left_expression = dtl_ast_to_ir_compile_expression(context, scope, left_node, error);
    if (dtl_ir_ref_is_null(left_expression)) {
        return DTL_IR_NULL_REF;
    }

    right_node = dtl_ast_less_than_expression_node_get_right(expression_node);
    right_expression = dtl_ast_to_ir_compile_expression(context, scope, right_node, error);
    if (dtl_ir_ref_is_null(right_expression)) {
        return DTL_IR_NULL_REF;
    }

    left_dtype = dtl_ir_expression_get_dtype(context->graph, left_expression);
    right_dtype = dtl_ir_expression_get_dtype(context->graph, right_expression);
    if (left_dtype != right_dtype) {
        dtl_set_error(error, dtl_error_create("mismatched shapes"));
        dtl_ast_to_ir_shrink_error(error, expression_node);
        return DTL_IR_NULL_REF;
    }

    left_shape = dtl_ir_array_expression_get_shape(context->graph, left_expression);
    right_shape = dtl_ir_array_expression_get_shape(context->graph, right_expression);
    if (!dtl_ir_ref_equal(context->graph, left_shape, right_shape)) {
        dtl_set_error(error, dtl_error_create("mismatched shapes"));
        dtl_ast_to_ir_shrink_error(error, expression_node);
        return DTL_IR_NULL_REF;
    }

    return dtl_ir_less_than_expression_create(
        context->graph, left_shape, left_expression, right_expression
    );
}

static struct dtl_ir_ref
dtl_ast_to_ir_compile_less_than_or_equal_to_expression(
    struct dtl_ast_to_ir_context *context,
    struct dtl_ast_to_ir_scope *scope,
    struct dtl_ast_node *expression_node,
    struct dtl_error **error
) {
    struct dtl_ast_node *left_node;
    struct dtl_ir_ref left_expression;
    struct dtl_ast_node *right_node;
    struct dtl_ir_ref right_expression;
    enum dtl_dtype left_dtype;
    enum dtl_dtype right_dtype;
    struct dtl_ir_ref left_shape;
    struct dtl_ir_ref right_shape;

    assert(context != NULL);
    assert(dtl_ast_node_is_less_than_or_equal_to_expression(expression_node));

    left_node = dtl_ast_less_than_or_equal_to_expression_node_get_left(expression_node);
    left_expression = dtl_ast_to_ir_compile_expression(context, scope, left_node, error);
    if (dtl_ir_ref_is_null(left_expression)) {
        return DTL_IR_NULL_REF;
    }

    right_node = dtl_ast_less_than_or_equal_to_expression_node_get_right(expression_node);
    right_expression = dtl_ast_to_ir_compile_expression(context, scope, right_node, error);
    if (dtl_ir_ref_is_null(right_expression)) {
        return DTL_IR_NULL_REF;
    }

    left_dtype = dtl_ir_expression_get_dtype(context->graph, left_expression);
    right_dtype = dtl_ir_expression_get_dtype(context->graph, right_expression);
    if (left_dtype != right_dtype) {
        dtl_set_error(error, dtl_error_create("mismatched shapes"));
        dtl_ast_to_ir_shrink_error(error, expression_node);
        return DTL_IR_NULL_REF;
    }

    left_shape = dtl_ir_array_expression_get_shape(context->graph, left_expression);
    right_shape = dtl_ir_array_expression_get_shape(context->graph, right_expression);
    if (!dtl_ir_ref_equal(context->graph, left_shape, right_shape)) {
        dtl_set_error(error, dtl_error_create("mismatched shapes"));
        dtl_ast_to_ir_shrink_error(error, expression_node);
        return DTL_IR_NULL_REF;
    }

    return dtl_ir_less_than_or_equal_to_expression_create(
        context->graph, left_shape, left_expression, right_expression
    );
}

static struct dtl_ir_ref
dtl_ast_to_ir_compile_greater_than_expression(
    struct dtl_ast_to_ir_context *context,
    struct dtl_ast_to_ir_scope *scope,
    struct dtl_ast_node *expression_node,
    struct dtl_error **error
) {
    struct dtl_ast_node *left_node;
    struct dtl_ir_ref left_expression;
    struct dtl_ast_node *right_node;
    struct dtl_ir_ref right_expression;
    enum dtl_dtype left_dtype;
    enum dtl_dtype right_dtype;
    struct dtl_ir_ref left_shape;
    struct dtl_ir_ref right_shape;

    assert(context != NULL);
    assert(dtl_ast_node_is_greater_than_expression(expression_node));

    left_node = dtl_ast_greater_than_expression_node_get_left(expression_node);
    left_expression = dtl_ast_to_ir_compile_expression(context, scope, left_node, error);
    if (dtl_ir_ref_is_null(left_expression)) {
        return DTL_IR_NULL_REF;
    }

    right_node = dtl_ast_greater_than_expression_node_get_right(expression_node);
    right_expression = dtl_ast_to_ir_compile_expression(context, scope, right_node, error);
    if (dtl_ir_ref_is_null(right_expression)) {
        return DTL_IR_NULL_REF;
    }

    left_dtype = dtl_ir_expression_get_dtype(context->graph, left_expression);
    right_dtype = dtl_ir_expression_get_dtype(context->graph, right_expression);
    if (left_dtype != right_dtype) {
        dtl_set_error(error, dtl_error_create("mismatched shapes"));
        dtl_ast_to_ir_shrink_error(error, expression_node);
        return DTL_IR_NULL_REF;
    }

    left_shape = dtl_ir_array_expression_get_shape(context->graph, left_expression);
    right_shape = dtl_ir_array_expression_get_shape(context->graph, right_expression);
    if (!dtl_ir_ref_equal(context->graph, left_shape, right_shape)) {
        dtl_set_error(error, dtl_error_create("mismatched shapes"));
        dtl_ast_to_ir_shrink_error(error, expression_node);
        return DTL_IR_NULL_REF;
    }

    return dtl_ir_greater_than_expression_create(
        context->graph, left_shape, left_expression, right_expression
    );
}

static struct dtl_ir_ref
dtl_ast_to_ir_compile_greater_than_or_equal_to_expression(
    struct dtl_ast_to_ir_context *context,
    struct dtl_ast_to_ir_scope *scope,
    struct dtl_ast_node *expression_node,
    struct dtl_error **error
) {
    struct dtl_ast_node *left_node;
    struct dtl_ir_ref left_expression;
    struct dtl_ast_node *right_node;
    struct dtl_ir_ref right_expression;
    enum dtl_dtype left_dtype;
    enum dtl_dtype right_dtype;
    struct dtl_ir_ref left_shape;
    struct dtl_ir_ref right_shape;

    assert(context != NULL);
    assert(dtl_ast_node_is_greater_than_or_equal_to_expression(expression_node));

    left_node = dtl_ast_greater_than_or_equal_to_expression_node_get_left(expression_node);
    left_expression = dtl_ast_to_ir_compile_expression(context, scope, left_node, error);
    if (dtl_ir_ref_is_null(left_expression)) {
        return DTL_IR_NULL_REF;
    }

    right_node = dtl_ast_greater_than_or_equal_to_expression_node_get_right(expression_node);
    right_expression = dtl_ast_to_ir_compile_expression(context, scope, right_node, error);
    if (dtl_ir_ref_is_null(right_expression)) {
        return DTL_IR_NULL_REF;
    }

    left_dtype = dtl_ir_expression_get_dtype(context->graph, left_expression);
    right_dtype = dtl_ir_expression_get_dtype(context->graph, right_expression);
    if (left_dtype != right_dtype) {
        dtl_set_error(error, dtl_error_create("mismatched shapes"));
        dtl_ast_to_ir_shrink_error(error, expression_node);
        return DTL_IR_NULL_REF;
    }

    left_shape = dtl_ir_array_expression_get_shape(context->graph, left_expression);
    right_shape = dtl_ir_array_expression_get_shape(context->graph, right_expression);
    if (!dtl_ir_ref_equal(context->graph, left_shape, right_shape)) {
        dtl_set_error(error, dtl_error_create("mismatched shapes"));
        dtl_ast_to_ir_shrink_error(error, expression_node);
        return DTL_IR_NULL_REF;
    }

    return dtl_ir_greater_than_or_equal_to_expression_create(
        context->graph, left_shape, left_expression, right_expression
    );
}

static struct dtl_ir_ref
dtl_ast_to_ir_compile_add_expression(
    struct dtl_ast_to_ir_context *context,
    struct dtl_ast_to_ir_scope *scope,
    struct dtl_ast_node *expression_node,
    struct dtl_error **error
) {
    struct dtl_ast_node *left_node;
    struct dtl_ir_ref left_expression;
    struct dtl_ast_node *right_node;
    struct dtl_ir_ref right_expression;
    enum dtl_dtype left_dtype;
    enum dtl_dtype right_dtype;
    struct dtl_ir_ref left_shape;
    struct dtl_ir_ref right_shape;

    assert(context != NULL);
    assert(dtl_ast_node_is_add_expression(expression_node));

    left_node = dtl_ast_add_expression_node_get_left(expression_node);
    left_expression = dtl_ast_to_ir_compile_expression(context, scope, left_node, error);
    if (dtl_ir_ref_is_null(left_expression)) {
        return DTL_IR_NULL_REF;
    }

    right_node = dtl_ast_add_expression_node_get_right(expression_node);
    right_expression = dtl_ast_to_ir_compile_expression(context, scope, right_node, error);
    if (dtl_ir_ref_is_null(right_expression)) {
        return DTL_IR_NULL_REF;
    }

    left_dtype = dtl_ir_expression_get_dtype(context->graph, left_expression);
    right_dtype = dtl_ir_expression_get_dtype(context->graph, right_expression);
    if (left_dtype != right_dtype) {
        dtl_set_error(error, dtl_error_create("mismatched shapes"));
        dtl_ast_to_ir_shrink_error(error, expression_node);
        return DTL_IR_NULL_REF;
    }

    left_shape = dtl_ir_array_expression_get_shape(context->graph, left_expression);
    right_shape = dtl_ir_array_expression_get_shape(context->graph, right_expression);
    if (!dtl_ir_ref_equal(context->graph, left_shape, right_shape)) {
        dtl_set_error(error, dtl_error_create("mismatched shapes"));
        dtl_ast_to_ir_shrink_error(error, expression_node);
        return DTL_IR_NULL_REF;
    }

    return dtl_ir_add_expression_create(
        context->graph, left_dtype, left_shape, left_expression, right_expression
    );
}

static struct dtl_ir_ref
dtl_ast_to_ir_compile_expression(
    struct dtl_ast_to_ir_context *context,
    struct dtl_ast_to_ir_scope *scope,
    struct dtl_ast_node *expression_node,
    struct dtl_error **error
) {
    assert(context != NULL);
    assert(dtl_ast_node_is_expression(expression_node));

    if (dtl_ast_node_is_column_reference_expression(expression_node)) {
        return dtl_ast_to_ir_compile_column_reference_expression(context, scope, expression_node, error);
    }

    if (dtl_ast_node_is_literal_expression(expression_node)) {
        assert(false);
    }

    if (dtl_ast_node_is_function_call_expression(expression_node)) {
        assert(false);
    }

    if (dtl_ast_node_is_equal_to_expression(expression_node)) {
        return dtl_ast_to_ir_compile_equal_to_expression(context, scope, expression_node, error);
    }

    if (dtl_ast_node_is_less_than_expression(expression_node)) {
        return dtl_ast_to_ir_compile_less_than_expression(context, scope, expression_node, error);
    }

    if (dtl_ast_node_is_less_than_or_equal_to_expression(expression_node)) {
        return dtl_ast_to_ir_compile_less_than_or_equal_to_expression(context, scope, expression_node, error);
    }

    if (dtl_ast_node_is_greater_than_expression(expression_node)) {
        return dtl_ast_to_ir_compile_greater_than_expression(context, scope, expression_node, error);
    }

    if (dtl_ast_node_is_greater_than_or_equal_to_expression(expression_node)) {
        return dtl_ast_to_ir_compile_greater_than_or_equal_to_expression(context, scope, expression_node, error);
    }

    if (dtl_ast_node_is_add_expression(expression_node)) {
        return dtl_ast_to_ir_compile_add_expression(context, scope, expression_node, error);
    }

    if (dtl_ast_node_is_subtract_expression(expression_node)) {
        assert(false);
    }

    if (dtl_ast_node_is_multiply_expression(expression_node)) {
        assert(false);
    }

    if (dtl_ast_node_is_divide_expression(expression_node)) {
        assert(false);
    }

    assert(false); // Not implemented.
}

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_compile_join_clause(
    struct dtl_ast_to_ir_context *context,
    struct dtl_ast_node *join_clause_node,
    struct dtl_ast_to_ir_scope *left_scope,
    struct dtl_error **error
) {
    struct dtl_ast_node *binding_node;
    struct dtl_ast_node *table_node;
    struct dtl_ast_node *table_name_node;
    char const *join_table_name;
    struct dtl_ast_to_ir_scope *right_scope;
    struct dtl_ir_ref left_shape;
    struct dtl_ir_ref right_shape;
    struct dtl_ir_ref full_shape;
    struct dtl_ir_ref full_left_index;
    struct dtl_ir_ref full_right_index;
    struct dtl_ast_to_ir_scope *full_scope;
    char const *binding_name;
    char const *binding_namespace;
    struct dtl_ir_ref binding_expression;
    struct dtl_ast_node *constraint_node;
    struct dtl_ast_node *predicate_node;
    struct dtl_ir_ref shape;
    struct dtl_ir_ref mask;
    struct dtl_ir_ref left_index;
    struct dtl_ir_ref right_index;
    struct dtl_ast_to_ir_scope *output_scope;
    size_t i;

    binding_node = dtl_ast_join_clause_node_get_table_binding(join_clause_node);

    if (dtl_ast_node_is_implicit_table_binding(binding_node)) {
        table_node = dtl_ast_implicit_table_binding_node_get_expression(binding_node);
        join_table_name = "TODO";
    } else {
        assert(dtl_ast_node_is_aliased_table_binding(binding_node));

        table_node = dtl_ast_aliased_table_binding_node_get_expression(binding_node);
        table_name_node = dtl_ast_aliased_table_binding_node_get_alias(binding_node);
        join_table_name = dtl_ast_name_node_get_value(table_name_node);
    }
    (void)join_table_name; // TODO

    // This is the scope representing the table we're joining against.
    right_scope = dtl_ast_to_ir_compile_table_expression(context, table_node, error);
    if (right_scope == NULL) {
        dtl_ast_to_ir_scope_destroy(left_scope);
        return NULL;
    }

    // Create a full, unfiltered scope that we can run the predicate against.
    left_shape = dtl_ast_to_ir_scope_shape(context, left_scope);
    right_shape = dtl_ast_to_ir_scope_shape(context, right_scope);

    full_shape = dtl_ir_join_shape_expression_create(
        context->graph, left_shape, right_shape
    );

    full_left_index = dtl_ir_join_left_expression_create(
        context->graph, full_shape, left_shape, right_shape
    );
    full_right_index = dtl_ir_join_right_expression_create(
        context->graph, full_shape, left_shape, right_shape
    );

    // Do not trace the full join scope!  We'd like to be able to optimise it away.  For tracing, we
    // should generate a new table containing only the rows which evaluate to true.
    full_scope = dtl_ast_to_ir_scope_create();

    for (i = 0; i < left_scope->num_columns; i++) {
        binding_name = left_scope->columns[i].name;
        binding_namespace = left_scope->columns[i].namespace;
        binding_expression = left_scope->columns[i].expression;

        binding_expression = dtl_ir_pick_expression_create(
            context->graph,
            dtl_ir_expression_get_dtype(context->graph, binding_expression),
            full_shape,
            binding_expression,
            full_left_index
        );

        full_scope = dtl_ast_to_ir_scope_add(
            full_scope, binding_name, binding_namespace, binding_expression
        );
    }

    for (i = 0; i < right_scope->num_columns; i++) {
        binding_name = right_scope->columns[i].name;
        binding_namespace = right_scope->columns[i].namespace;
        binding_expression = right_scope->columns[i].expression;

        binding_expression = dtl_ir_pick_expression_create(
            context->graph,
            dtl_ir_expression_get_dtype(context->graph, binding_expression),
            full_shape,
            binding_expression,
            full_right_index
        );

        full_scope = dtl_ast_to_ir_scope_add(
            full_scope, binding_name, binding_namespace, binding_expression
        );
    }

    constraint_node = dtl_ast_join_clause_node_get_constraint(join_clause_node);

    output_scope = full_scope;

    if (constraint_node != NULL && dtl_ast_node_is_join_on_constraint(constraint_node)) {
        predicate_node = dtl_ast_join_on_constraint_node_get_predicate(constraint_node);

        mask = dtl_ast_to_ir_compile_expression(
            context, full_scope, predicate_node, error
        );
        if (dtl_ir_ref_is_null(mask)) {
            dtl_ast_to_ir_scope_destroy(left_scope);
            dtl_ast_to_ir_scope_destroy(right_scope);
            dtl_ast_to_ir_scope_destroy(full_scope);

            return NULL;
        }

        shape = dtl_ir_where_shape_expression_create(context->graph, mask);

        left_index = dtl_ir_where_expression_create(
            context->graph,
            DTL_DTYPE_INDEX_ARRAY,
            shape,
            full_left_index,
            mask
        );
        right_index = dtl_ir_where_expression_create(
            context->graph,
            DTL_DTYPE_INDEX_ARRAY,
            shape,
            full_right_index,
            mask
        );

        output_scope = dtl_ast_to_ir_scope_create();

        for (i = 0; i < left_scope->num_columns; i++) {
            binding_name = left_scope->columns[i].name;
            binding_namespace = left_scope->columns[i].namespace;
            binding_expression = left_scope->columns[i].expression;

            binding_expression = dtl_ir_pick_expression_create(
                context->graph,
                dtl_ir_expression_get_dtype(context->graph, binding_expression),
                shape,
                binding_expression,
                left_index
            );

            output_scope = dtl_ast_to_ir_scope_add(
                output_scope, binding_name, binding_namespace, binding_expression
            );
        }

        for (i = 0; i < right_scope->num_columns; i++) {
            binding_name = right_scope->columns[i].name;
            binding_namespace = right_scope->columns[i].namespace;
            binding_expression = right_scope->columns[i].expression;

            binding_expression = dtl_ir_pick_expression_create(
                context->graph,
                dtl_ir_expression_get_dtype(context->graph, binding_expression),
                shape,
                binding_expression,
                right_index
            );

            output_scope = dtl_ast_to_ir_scope_add(
                output_scope, binding_name, binding_namespace, binding_expression
            );
        }

        dtl_ast_to_ir_scope_destroy(full_scope);
    }

    dtl_ast_to_ir_scope_destroy(right_scope);
    dtl_ast_to_ir_scope_destroy(left_scope);

    return output_scope;
}

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_compile_select_expression(
    struct dtl_ast_to_ir_context *context, struct dtl_ast_node *select_node, struct dtl_error **error
) {
    struct dtl_ast_node *from_node;
    struct dtl_ast_node *table_binding_node;
    struct dtl_ast_node *source_node;
    struct dtl_ast_to_ir_scope *source_scope;
    struct dtl_ast_node *bindings_list_node;
    struct dtl_ast_node *where_clause_node;
    struct dtl_ast_node *predicate_node;
    struct dtl_ir_ref mask_expr;
    struct dtl_ir_ref where_shape;
    struct dtl_ir_ref column_expr;
    enum dtl_dtype column_dtype;
    struct dtl_ast_node *binding_node;
    struct dtl_ast_node *binding_expression_node;
    struct dtl_ast_node *binding_name_node;
    struct dtl_ast_to_ir_scope *output_scope;
    size_t i;
    char const *binding_name;
    char const *binding_namespace;
    struct dtl_ir_ref binding_expression;

    assert(context != NULL);
    assert(select_node != NULL);
    assert(dtl_ast_node_is_select_expression(select_node));

    // Compile source expression.
    from_node = dtl_ast_select_expression_node_get_source(select_node);
    table_binding_node = dtl_ast_from_clause_node_get_table_binding(from_node);
    if (dtl_ast_node_is_implicit_table_binding(table_binding_node)) {
        source_node = dtl_ast_implicit_table_binding_node_get_expression(table_binding_node);
        source_scope = dtl_ast_to_ir_compile_table_expression(context, source_node, error);
        if (source_scope == NULL) {
            return NULL;
        }
    } else {
        // TODO aliased table bindings.
        assert(false);
    }

    // Add source expression name to source scope bindings.
    // TODO

    // Compile join clauses.
    struct dtl_ast_node *join_clause_list_node;
    struct dtl_ast_node *join_clause_node;

    join_clause_list_node = dtl_ast_select_expression_node_get_join_clauses(select_node);
    if (join_clause_list_node != NULL) {
        assert(dtl_ast_node_is_join_clause_list(join_clause_list_node));

        for (i = 0; i < dtl_ast_join_clause_list_node_get_num_clauses(join_clause_list_node); i++) {
            join_clause_node = dtl_ast_join_clause_list_node_get_clause(join_clause_list_node, i);

            source_scope = dtl_ast_to_ir_compile_join_clause(context, join_clause_node, source_scope, error);
            if (source_scope == NULL) {
                return NULL;
            }
        }
    }

    // Compile where clause.
    where_clause_node = dtl_ast_select_expression_node_get_where_clause(select_node);
    if (where_clause_node != NULL) {
        assert(dtl_ast_node_is_where_clause(where_clause_node));

        predicate_node = dtl_ast_where_clause_node_get_predicate(where_clause_node);

        mask_expr = dtl_ast_to_ir_compile_expression(context, source_scope, predicate_node, error);
        if (dtl_ir_ref_is_null(mask_expr)) {
            return NULL;
        }

        where_shape = dtl_ir_where_shape_expression_create(context->graph, mask_expr);

        for (i = 0; i < source_scope->num_columns; i++) {
            column_expr = source_scope->columns[i].expression;

            column_dtype = dtl_ir_expression_get_dtype(context->graph, column_expr);
            column_expr = dtl_ir_where_expression_create(
                context->graph, column_dtype, where_shape, column_expr, mask_expr
            );

            source_scope->columns[i].expression = column_expr;
        }
    }

    // Compile column bindings.
    bindings_list_node = dtl_ast_select_expression_node_get_columns(select_node);
    assert(dtl_ast_node_is_column_binding_list(bindings_list_node));

    output_scope = dtl_ast_to_ir_scope_create();

    for (i = 0; i < dtl_ast_column_binding_list_node_get_num_bindings(bindings_list_node); i++) {
        binding_node = dtl_ast_column_binding_list_node_get_binding(bindings_list_node, i);

        if (dtl_ast_node_is_wildcard_column_binding(binding_node)) {
            assert(false); // TODO not implemented.
            continue;
        }

        if (dtl_ast_node_is_implicit_column_binding(binding_node)) {
            binding_expression_node = dtl_ast_implicit_column_binding_node_get_expression(binding_node);
            binding_expression = dtl_ast_to_ir_compile_expression(
                context, source_scope, binding_expression_node, error
            );
            if (dtl_ir_ref_is_null(binding_expression)) {
                return NULL;
            }

            binding_name = dtl_ast_to_ir_expression_name(binding_expression_node);
            if (binding_name == NULL) {
                dtl_set_error(error, dtl_error_create("column has no obvious name"));
                dtl_ast_to_ir_shrink_error(error, binding_expression_node);
                return NULL;
            }
            binding_name = dtl_ir_graph_intern(context->graph, binding_name);
            binding_namespace = NULL;

            output_scope = dtl_ast_to_ir_scope_add(
                output_scope, binding_name, binding_namespace, binding_expression
            );
            continue;
        }

        if (dtl_ast_node_is_aliased_column_binding(binding_node)) {
            binding_expression_node = dtl_ast_aliased_column_binding_node_get_expression(binding_node);
            binding_expression = dtl_ast_to_ir_compile_expression(
                context, source_scope, binding_expression_node, error
            );
            if (dtl_ir_ref_is_null(binding_expression)) {
                return NULL;
            }

            binding_name_node = dtl_ast_aliased_column_binding_node_get_alias(binding_node);
            binding_name = dtl_ast_name_node_get_value(binding_name_node);
            binding_name = dtl_ir_graph_intern(context->graph, binding_name);
            binding_namespace = NULL;

            output_scope = dtl_ast_to_ir_scope_add(
                output_scope, binding_name, binding_namespace, binding_expression
            );
            continue;
        }
    }

    dtl_ast_to_ir_scope_destroy(source_scope);

    dtl_ast_to_ir_context_trace(
        context, dtl_ast_node_get_start(select_node), dtl_ast_node_get_end(select_node), output_scope
    );

    return output_scope;
}

static struct dtl_ast_to_ir_scope *
dtl_ast_to_ir_compile_import_expression(
    struct dtl_ast_to_ir_context *context,
    struct dtl_ast_node *expression,
    struct dtl_error **error
) {
    struct dtl_ast_node *path_expression;
    char const *path;
    struct dtl_schema *schema;
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

    // Note that we do not free the schema.
    schema = context->import_callback(path, error, context->user_data);
    if (schema == NULL) {
        dtl_ast_to_ir_shrink_error(error, path_expression);
        return NULL;
    }

    table = dtl_ir_open_table_expression_create(context->graph, path);
    table_shape = dtl_ir_table_shape_expression_create(context->graph, table);

    table_scope = dtl_ast_to_ir_scope_create();

    for (i = 0; i < dtl_schema_get_num_columns(schema); i++) {
        column_name = dtl_schema_get_column_name(schema, i);
        assert(column_name != NULL); // TODO validate properly.

        column_dtype = dtl_schema_get_column_dtype(schema, i);
        assert(dtl_dtype_is_array_type(column_dtype));

        column = dtl_ir_read_column_expression_create(
            context->graph, column_dtype, table_shape, table, column_name
        );
        // Refresh column name to get the interned string attached to the graph.  Graph outlives
        // table so this is safe.
        column_name = dtl_ir_read_column_expression_get_column_name(context->graph, column);

        table_scope = dtl_ast_to_ir_scope_add(table_scope, column_name, NULL, column);
    }

    dtl_ast_to_ir_context_trace(
        context, dtl_ast_node_get_start(expression), dtl_ast_node_get_end(expression), table_scope
    );

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

    dtl_ast_to_ir_context_trace(
        context, dtl_ast_node_get_start(expression), dtl_ast_node_get_end(expression), result
    );

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

    dtl_ast_to_ir_context_trace(
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

    dtl_ast_to_ir_scope_destroy(expression_scope);

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

    dtl_ast_to_ir_context_trace(
        context, dtl_ast_node_get_start(statement), dtl_ast_node_get_end(statement), expression_scope
    );

    dtl_ast_to_ir_context_export_table(context, path, expression_scope);

    dtl_ast_to_ir_scope_destroy(expression_scope);

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
    struct dtl_schema *(*import_callback)(char const *, struct dtl_error **, void *),
    void (*export_callback)(char const *, struct dtl_schema *, struct dtl_ir_ref *, void *),
    void (*trace_callback)(struct dtl_location, struct dtl_location, struct dtl_schema *, struct dtl_ir_ref *, void *),
    void *user_data,
    struct dtl_error **error
) {
    struct dtl_ast_to_ir_context *context;
    size_t i;
    struct dtl_ast_node *statements;
    struct dtl_ast_node *statement;
    enum dtl_status status = DTL_STATUS_OK;

    context = calloc(1, sizeof(struct dtl_ast_to_ir_context));
    context->graph = graph;
    context->import_callback = import_callback;
    context->export_callback = export_callback;
    context->trace_callback = trace_callback;
    context->user_data = user_data;
    context->globals = dtl_ast_to_ir_scope_create();
    context->num_exports = 0;
    context->exports = NULL;

    statements = dtl_ast_script_node_get_statements(root);
    for (i = 0; i < dtl_ast_statement_list_node_get_num_statements(statements); i++) {
        statement = dtl_ast_statement_list_node_get_statement(statements, i);
        status = dtl_ast_to_ir_compile_statement(context, statement, error);
        if (status != DTL_STATUS_OK) {
            break;
        }
    }

    for (i = 0; i < context->num_exports; i++) {
        context->export_callback(
            context->exports[i].name,
            context->exports[i].schema,
            context->exports[i].expressions, context->user_data
        );
    }

    dtl_ast_to_ir_scope_destroy(context->globals);
    free(context->exports);
    free(context);

    return status;
}
