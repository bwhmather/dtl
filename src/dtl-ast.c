#include "dtl-ast.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "dtl-ast.h"
#include "dtl-location.h"

#define DTL_AST_CLASS_MASK 0x78000000

#define DTL_AST_CLASS_NONE 0
#define DTL_AST_CLASS_LITERAL (1 << 27)
#define DTL_AST_CLASS_COLUMN_NAME (2 << 27)
#define DTL_AST_CLASS_EXPRESSION (3 << 27)
#define DTL_AST_CLASS_COLUMN_BINDING (4 << 27)
#define DTL_AST_CLASS_TABLE_BINDING (5 << 27)
#define DTL_AST_CLASS_DISTINCT_CLAUSE (6 << 27)
#define DTL_AST_CLASS_JOIN_CONSTRAINT (7 << 27)
#define DTL_AST_CLASS_TABLE_EXPRESSION (8 << 27)
#define DTL_AST_CLASS_STATEMENT (9 << 27)

enum dtl_ast_node_type {
    DTL_AST_NAME = 1,

    DTL_AST_TABLE_NAME,
    DTL_AST_COLUMN_BINDING_LIST,
    DTL_AST_FROM_CLAUSE,
    DTL_AST_JOIN_CLAUSE,
    DTL_AST_JOIN_CLAUSE_LIST,
    DTL_AST_WHERE_CLAUSE,
    DTL_AST_GROUP_BY_CLAUSE,
    DTL_AST_GROUP_CONSECUTIVE_BY_CLAUSE,
    DTL_AST_EXPRESSION_LIST,
    DTL_AST_STATEMENT_LIST,
    DTL_AST_SCRIPT,

    DTL_AST_INT_LITERAL = DTL_AST_CLASS_LITERAL,
    DTL_AST_STRING_LITERAL,

    DTL_AST_UNQUALIFIED_COLUMN_NAME = DTL_AST_CLASS_COLUMN_NAME,
    DTL_AST_QUALIFIED_COLUMN_NAME,

    DTL_AST_COLUMN_REFERENCE_EXPRESSION = DTL_AST_CLASS_EXPRESSION,
    DTL_AST_LITERAL_EXPRESSION,
    DTL_AST_FUNCTION_CALL_EXPRESSION,
    DTL_AST_EQUAL_TO_EXPRESSION,
    DTL_AST_LESS_THAN_EXPRESSION,
    DTL_AST_LESS_THAN_OR_EQUAL_EXPRESSION,
    DTL_AST_GREATER_THAN_EXPRESSION,
    DTL_AST_GREATER_THAN_OR_EQUAL_EXPRESSION,
    DTL_AST_ADD_EXPRESSION,
    DTL_AST_SUBTRACT_EXPRESSION,
    DTL_AST_MULTIPLY_EXPRESSION,
    DTL_AST_DIVIDE_EXPRESSION,

    DTL_AST_WILDCARD_COLUMN_BINDING = DTL_AST_CLASS_COLUMN_BINDING,
    DTL_AST_IMPLICIT_COLUMN_BINDING,
    DTL_AST_ALIASED_COLUMN_BINDING,

    DTL_AST_IMPLICIT_TABLE_BINDING = DTL_AST_CLASS_TABLE_BINDING,
    DTL_AST_ALIASED_TABLE_BINDING,

    DTL_AST_DISTINCT_CLAUSE = DTL_AST_CLASS_DISTINCT_CLAUSE,
    DTL_AST_DISTINCT_CONSECUTIVE_CLAUSE,

    DTL_AST_JOIN_ON_CONSTRAINT = DTL_AST_CLASS_JOIN_CONSTRAINT,
    DTL_AST_JOIN_USING_CONSTRAINT,

    DTL_AST_SELECT_EXPRESSION = DTL_AST_CLASS_TABLE_EXPRESSION,
    DTL_AST_IMPORT_EXPRESSION,
    DTL_AST_TABLE_REFERENCE_EXPRESSION,

    DTL_AST_ASSIGNMENT_STATEMENT = DTL_AST_CLASS_STATEMENT,
    //    DTL_AST_UPDATE_STATEMENT,  // TODO
    //    DTL_AST_DELETE_STATEMENT,  // TODO
    //    DTL_AST_INSERT_STATEMENT,  // TODO
    DTL_AST_EXPORT_STATEMENT,
};

struct dtl_ast_node {
    enum dtl_ast_node_type type;
    bool has_children;
    bool has_data;

    struct dtl_location start;
    struct dtl_location end;
};

struct dtl_ast_container_node {
    struct dtl_ast_node base;

    size_t num_children;
    struct dtl_ast_node *children[];
};

struct dtl_ast_data_node {
    struct dtl_ast_node base;

    size_t size;
    char data[];
};

/* === Generic ================================================================================== */

void
dtl_ast_node_update_bounds(
    struct dtl_ast_node *node, struct dtl_location start, struct dtl_location end
) {
    assert(node != NULL);
    if (!dtl_location_is_null(start)) {
        if (dtl_location_is_null(node->start) || start.offset < node->start.offset) {
            node->start = start;
        }
    }

    if (!dtl_location_is_null(end)) {
        if (dtl_location_is_null(node->end) || end.offset > node->end.offset) {
            node->end = end;
        }
    }
}

struct dtl_location
dtl_ast_node_get_start(struct dtl_ast_node *node) {
    assert(node != NULL);

    return node->start;
}

struct dtl_location
dtl_ast_node_get_end(struct dtl_ast_node *node) {
    assert(node != NULL);

    return node->end;
}

static void
dtl_ast_node_init(struct dtl_ast_node *node, enum dtl_ast_node_type type) {
    node->type = type;
    node->start = (struct dtl_location){
        .offset = SIZE_MAX,
        .lineno = SIZE_MAX,
        .column = SIZE_MAX
    };
    node->end = (struct dtl_location){
        .offset = SIZE_MAX,
        .lineno = SIZE_MAX,
        .column = SIZE_MAX
    };
}

void
dtl_ast_node_destroy(struct dtl_ast_node *node) {
    struct dtl_ast_container_node *container_node;
    size_t i;

    if (node == NULL) {
        return;
    }

    if (node->has_children) {
        container_node = (struct dtl_ast_container_node *)node;
        for (i = 0; i < container_node->num_children; i++) {
            dtl_ast_node_destroy(container_node->children[i]);
        }
    }

    free(node);
}

/* --- Empty Nodes ------------------------------------------------------------------------------ */

static struct dtl_ast_node *
dtl_ast_node_create_empty(int tag) {
    struct dtl_ast_node *node;

    node = calloc(1, sizeof(struct dtl_ast_node));
    dtl_ast_node_init(node, tag);

    return node;
}

/* --- Container Nodes -------------------------------------------------------------------------- */

static struct dtl_ast_node *
dtl_ast_node_create_with_children(int tag) {
    struct dtl_ast_container_node *container_node;

    container_node = calloc(1, sizeof(struct dtl_ast_container_node));
    dtl_ast_node_init(&container_node->base, tag);
    container_node->base.has_children = true;

    return &container_node->base;
}

static struct dtl_ast_node *
dtl_ast_node_append_child(struct dtl_ast_node *node, struct dtl_ast_node *child) {
    struct dtl_ast_container_node *container_node;

    assert(node != NULL);
    assert(dtl_ast_node_has_children(node));

    container_node = (struct dtl_ast_container_node *)node;

    container_node->num_children += 1;
    container_node = realloc(
        container_node,
        sizeof(struct dtl_ast_container_node) +
            container_node->num_children * sizeof(struct dtl_ast_node *)
    );

    container_node->children[container_node->num_children - 1] = child;

    if (child != NULL) {
        dtl_ast_node_update_bounds(
            &container_node->base,
            dtl_ast_node_get_start(child),
            dtl_ast_node_get_end(child)
        );
    }

    return &container_node->base;
}

bool
dtl_ast_node_has_children(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->has_children;
}

size_t
dtl_ast_node_get_num_children(struct dtl_ast_node *node) {
    struct dtl_ast_container_node *container_node;

    assert(node != NULL);
    assert(dtl_ast_node_has_children(node));

    container_node = (struct dtl_ast_container_node *)node;

    return container_node->num_children;
}

struct dtl_ast_node *
dtl_ast_node_get_child(struct dtl_ast_node *node, size_t index) {
    struct dtl_ast_container_node *container_node;

    assert(node != NULL);
    assert(dtl_ast_node_has_children(node));
    assert(index < dtl_ast_node_get_num_children(node));

    container_node = (struct dtl_ast_container_node *)node;

    return container_node->children[index];
}

/* --- Data Nodes ------------------------------------------------------------------------------- */

static struct dtl_ast_node *
dtl_ast_node_create_with_data(int tag, size_t size) {
    struct dtl_ast_data_node *data_node;

    data_node = calloc(1, sizeof(struct dtl_ast_data_node));
    dtl_ast_node_init(&data_node->base, tag);
    data_node->size = size;

    return &data_node->base;
}

bool
dtl_ast_node_has_data(struct dtl_ast_node *node) {
    assert(node != NULL);

    return node->has_data;
}

size_t
dtl_ast_node_get_data_size(struct dtl_ast_node *node) {
    struct dtl_ast_data_node *data_node;

    assert(node != NULL);
    assert(dtl_ast_node_has_data(node));

    data_node = (struct dtl_ast_data_node *)node;

    return data_node->size;
}

void *
dtl_ast_node_get_data(struct dtl_ast_node *node) {
    struct dtl_ast_data_node *data_node;

    assert(node != NULL);
    assert(dtl_ast_node_has_data(node));

    data_node = (struct dtl_ast_data_node *)node;

    return data_node->data;
}

/* --- Names ------------------------------------------------------------------------------------ */

struct dtl_ast_name_node {
    struct dtl_ast_node base;

    char value[];
};

bool
dtl_ast_node_is_name(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_NAME;
}

struct dtl_ast_node *
dtl_ast_name_node_create(char const *value) {
    size_t len;
    struct dtl_ast_name_node *name_node;

    assert(value != NULL);

    len = strlen(value);

    name_node = calloc(1, sizeof(struct dtl_ast_name_node) + len + 1);

    dtl_ast_node_init(&name_node->base, DTL_AST_NAME);
    memcpy(name_node->value, value, len + 1);

    return &name_node->base;
}

char const *
dtl_ast_name_node_get_value(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_name(node));
    return ((struct dtl_ast_name_node *)node)->value;
}

/* --- Table Names ------------------------------------------------------------------------------ */

struct dtl_ast_table_name_node {
    struct dtl_ast_node base;

    struct dtl_ast_node *name;
};

bool
dtl_ast_node_is_table_name(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_TABLE_NAME;
}

struct dtl_ast_node *
dtl_ast_table_name_node_create(struct dtl_ast_node *name) {
    struct dtl_ast_table_name_node *table_name_node;

    assert(dtl_ast_node_is_name(name));

    table_name_node = calloc(1, sizeof(struct dtl_ast_table_name_node));

    dtl_ast_node_init(&table_name_node->base, DTL_AST_TABLE_NAME);
    table_name_node->name = name;

    return &table_name_node->base;
}

struct dtl_ast_node *
dtl_ast_table_name_node_get_value(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_table_name(node));

    return ((struct dtl_ast_table_name_node *)node)->name;
}

/* --- Column Binding Lists --------------------------------------------------------------------- */

bool
dtl_ast_node_is_column_binding_list(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_COLUMN_BINDING_LIST;
}

struct dtl_ast_node *
dtl_ast_column_binding_list_node_create(void) {
    struct dtl_ast_node *node;

    node = dtl_ast_node_create_with_children(DTL_AST_COLUMN_BINDING_LIST);

    return node;
}

struct dtl_ast_node *
dtl_ast_column_binding_list_node_append(struct dtl_ast_node *node, struct dtl_ast_node *binding) {
    assert(dtl_ast_node_is_column_binding_list(node));
    assert(dtl_ast_node_is_column_binding(binding));

    node = dtl_ast_node_append_child(node, binding);

    return node;
}

size_t
dtl_ast_column_binding_list_node_get_num_bindings(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_column_binding_list(node));
    return dtl_ast_node_get_num_children(node);
}

struct dtl_ast_node *
dtl_ast_column_binding_list_node_get_binding(struct dtl_ast_node *node, size_t index) {
    assert(dtl_ast_node_is_column_binding_list(node));
    return dtl_ast_node_get_child(node, index);
}

/* --- From Clauses ----------------------------------------------------------------------------- */

bool
dtl_ast_node_is_from_clause(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_FROM_CLAUSE;
}

struct dtl_ast_node *
dtl_ast_from_clause_node_create(struct dtl_ast_node *table_binding) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_table_binding(table_binding));

    node = dtl_ast_node_create_with_children(DTL_AST_FROM_CLAUSE);
    node = dtl_ast_node_append_child(node, table_binding);

    return node;
}

struct dtl_ast_node *
dtl_ast_from_clause_node_get_table_binding(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_from_clause(node));
    return dtl_ast_node_get_child(node, 0);
}

/* --- Join Clauses ----------------------------------------------------------------------------- */

bool
dtl_ast_node_is_join_clause(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_JOIN_CLAUSE;
}

struct dtl_ast_node *
dtl_ast_join_clause_node_create(struct dtl_ast_node *table_binding, struct dtl_ast_node *constraint) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_table_binding(table_binding));
    assert(dtl_ast_node_is_join_constraint(constraint));

    node = dtl_ast_node_create_with_children(DTL_AST_JOIN_CLAUSE);
    node = dtl_ast_node_append_child(node, table_binding);
    node = dtl_ast_node_append_child(node, constraint);

    return node;
}

struct dtl_ast_node *
dtl_ast_join_clause_node_get_table_binding(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_join_clause(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_join_clause_node_get_constraint(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_join_clause(node));
    return dtl_ast_node_get_child(node, 1);
}

/* --- Join Clause Lists ------------------------------------------------------------------------ */

bool
dtl_ast_node_is_join_clause_list(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_JOIN_CLAUSE_LIST;
}

struct dtl_ast_node *
dtl_ast_join_clause_list_node_create(struct dtl_ast_node *head) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_join_clause(head));

    node = dtl_ast_node_create_with_children(DTL_AST_JOIN_CLAUSE_LIST);
    node = dtl_ast_node_append_child(node, head);

    return node;
}

struct dtl_ast_node *
dtl_ast_join_clause_list_node_append(struct dtl_ast_node *list, struct dtl_ast_node *clause) {
    assert(dtl_ast_node_is_join_clause_list(list));
    assert(dtl_ast_node_is_join_clause(clause));

    return dtl_ast_node_append_child(list, clause);
}

size_t
dtl_ast_join_clause_list_node_get_num_clauses(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_join_clause_list(node));
    return dtl_ast_node_get_num_children(node);
}

struct dtl_ast_node *
dtl_ast_join_clause_list_node_get_clause(struct dtl_ast_node *node, size_t index) {
    assert(dtl_ast_node_is_join_clause_list(node));
    return dtl_ast_node_get_child(node, index);
}

/* --- Where Clauses ---------------------------------------------------------------------------- */

bool
dtl_ast_node_is_where_clause(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_WHERE_CLAUSE;
}

struct dtl_ast_node *
dtl_ast_where_clause_node_create(struct dtl_ast_node *predicate) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression(predicate));

    node = dtl_ast_node_create_with_children(DTL_AST_WHERE_CLAUSE);
    node = dtl_ast_node_append_child(node, predicate);

    return node;
}

struct dtl_ast_node *
dtl_ast_where_clause_node_get_predicate(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_where_clause(node));
    return dtl_ast_node_get_child(node, 0);
}

/* --- Group By Clauses ------------------------------------------------------------------------- */

bool
dtl_ast_node_is_group_by_clause(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_GROUP_BY_CLAUSE;
}

struct dtl_ast_node *
dtl_ast_group_by_clause_node_create(struct dtl_ast_node *pattern) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression_list(pattern));

    node = dtl_ast_node_create_with_children(DTL_AST_GROUP_BY_CLAUSE);
    node = dtl_ast_node_append_child(node, pattern);

    return node;
}

struct dtl_ast_node *
dtl_ast_group_by_clause_node_get_pattern(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_group_by_clause(node));
    return dtl_ast_node_get_child(node, 0);
}

/* --- Group Consecutive By Clauses ------------------------------------------------------------- */

bool
dtl_ast_node_is_group_consecutive_by_clause(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_GROUP_CONSECUTIVE_BY_CLAUSE;
}

struct dtl_ast_node *
dtl_ast_group_consecutive_by_clause_node_create(struct dtl_ast_node *expression_list) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression_list(expression_list));

    node = dtl_ast_node_create_with_children(DTL_AST_GROUP_CONSECUTIVE_BY_CLAUSE);
    node = dtl_ast_node_append_child(node, expression_list);

    return node;
}

struct dtl_ast_node *
dtl_ast_group_consecutive_by_clause_node_get_expression_list(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_group_consecutive_by_clause(node));
    return dtl_ast_node_get_child(node, 0);
}

/* --- Expression Lists ------------------------------------------------------------------------- */

bool
dtl_ast_node_is_expression_list(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_EXPRESSION_LIST;
}

struct dtl_ast_node *
dtl_ast_expression_list_node_create(struct dtl_ast_node *head) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression(head));

    node = dtl_ast_node_create_with_children(DTL_AST_EXPRESSION_LIST);
    node = dtl_ast_node_append_child(node, head);

    return node;
}

struct dtl_ast_node *
dtl_ast_expression_list_node_append(struct dtl_ast_node *node, struct dtl_ast_node *expression) {
    assert(dtl_ast_node_is_expression_list(node));
    assert(dtl_ast_node_is_expression(expression));

    return dtl_ast_node_append_child(node, expression);
}

size_t
dtl_ast_expression_list_node_get_num_expressions(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_expression_list(node));
    return dtl_ast_node_get_num_children(node);
}

struct dtl_ast_node *
dtl_ast_expression_list_node_get_expression(struct dtl_ast_node *node, size_t index) {
    assert(dtl_ast_node_is_expression_list(node));
    return dtl_ast_node_get_child(node, index);
}

/* --- Statement Lists -------------------------------------------------------------------------- */

bool
dtl_ast_node_is_statement_list(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_STATEMENT_LIST;
}

struct dtl_ast_node *
dtl_ast_statement_list_node_create(void) {
    struct dtl_ast_node *node;

    node = dtl_ast_node_create_with_children(DTL_AST_STATEMENT_LIST);

    return node;
}

struct dtl_ast_node *
dtl_ast_statement_list_node_append(struct dtl_ast_node *node, struct dtl_ast_node *statement) {
    assert(dtl_ast_node_is_statement_list(node));
    assert(dtl_ast_node_is_statement(statement));

    return dtl_ast_node_append_child(node, statement);
}

size_t
dtl_ast_statement_list_node_get_num_statements(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_statement_list(node));
    return dtl_ast_node_get_num_children(node);
}

struct dtl_ast_node *
dtl_ast_statement_list_node_get_statement(struct dtl_ast_node *node, size_t index) {
    assert(dtl_ast_node_is_statement_list(node));
    return dtl_ast_node_get_child(node, index);
}

/* --- Scripts ---------------------------------------------------------------------------------- */

bool
dtl_ast_node_is_script(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_SCRIPT;
}

struct dtl_ast_node *
dtl_ast_script_node_create(struct dtl_ast_node *statements) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_statement_list(statements));

    node = dtl_ast_node_create_with_children(DTL_AST_SCRIPT);
    node = dtl_ast_node_append_child(node, statements);

    return node;
}

struct dtl_ast_node *
dtl_ast_script_node_get_statements(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_script(node));
    return dtl_ast_node_get_child(node, 0);
}

/* === Literals ================================================================================= */

bool
dtl_ast_node_is_literal(struct dtl_ast_node *node) {
    assert(node != NULL);
    return (node->type & DTL_AST_CLASS_MASK) == DTL_AST_CLASS_LITERAL;
}

/* --- Integer Literals ------------------------------------------------------------------------- */

struct dtl_ast_int_literal_node {
    struct dtl_ast_node base;

    int64_t value;
};

bool
dtl_ast_node_is_int_literal(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_INT_LITERAL;
}

struct dtl_ast_node *
dtl_ast_int_literal_node_create(int64_t value) {
    struct dtl_ast_int_literal_node *int_literal_node;

    int_literal_node = calloc(1, sizeof(struct dtl_ast_int_literal_node));

    dtl_ast_node_init(&int_literal_node->base, DTL_AST_INT_LITERAL);

    int_literal_node->value = value;

    return &int_literal_node->base;
}

int64_t
dtl_ast_int_literal_node_get_value(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_int_literal(node));
    return ((struct dtl_ast_int_literal_node *)node)->value;
}

/* --- String Literals -------------------------------------------------------------------------- */

struct dtl_ast_string_literal_node {
    struct dtl_ast_node base;

    char value[];
};

bool
dtl_ast_node_is_string_literal(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_STRING_LITERAL;
}

struct dtl_ast_node *
dtl_ast_string_literal_node_create(char const *value) {
    size_t len;
    struct dtl_ast_name_node *string_literal_node;

    assert(value != NULL);

    len = strlen(value);

    string_literal_node = calloc(1, sizeof(struct dtl_ast_string_literal_node) + len + 1);

    dtl_ast_node_init(&string_literal_node->base, DTL_AST_STRING_LITERAL);
    memcpy(string_literal_node->value, value, len + 1);

    return &string_literal_node->base;
}

char const *
dtl_ast_string_literal_node_get_value(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_string_literal(node));

    return ((struct dtl_ast_string_literal_node *)node)->value;
}

/* === Column Names ============================================================================= */

bool
dtl_ast_node_is_column_name(struct dtl_ast_node *node) {
    assert(node != NULL);
    return (node->type & DTL_AST_CLASS_MASK) == DTL_AST_CLASS_COLUMN_NAME;
}

/* --- Unqualified Column Names ----------------------------------------------------------------- */

bool
dtl_ast_node_is_unqualified_column_name(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_UNQUALIFIED_COLUMN_NAME;
}

struct dtl_ast_node *
dtl_ast_unqualified_column_name_node_create(struct dtl_ast_node *column_name) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_name(column_name));

    node = dtl_ast_node_create_with_children(DTL_AST_UNQUALIFIED_COLUMN_NAME);
    node = dtl_ast_node_append_child(node, column_name);

    return node;
}

struct dtl_ast_node *
dtl_ast_unqualified_column_name_get_column_name(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_unqualified_column_name(node));
    return dtl_ast_node_get_child(node, 0);
}

/* --- Qualified Column Names ------------------------------------------------------------------- */

bool
dtl_ast_node_is_qualified_column_name(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_QUALIFIED_COLUMN_NAME;
}

struct dtl_ast_node *
dtl_ast_qualified_column_name_node_create(struct dtl_ast_node *table_name, struct dtl_ast_node *column_name) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_name(table_name));
    assert(dtl_ast_node_is_name(column_name));

    node = dtl_ast_node_create_with_children(DTL_AST_QUALIFIED_COLUMN_NAME);
    node = dtl_ast_node_append_child(node, table_name);
    node = dtl_ast_node_append_child(node, column_name);

    return node;
}

struct dtl_ast_node *
dtl_ast_qualified_column_name_get_table_name(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_qualified_column_name(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_qualified_column_name_get_column_name(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_qualified_column_name(node));
    return dtl_ast_node_get_child(node, 1);
}

/* === Expressions ============================================================================== */

bool
dtl_ast_node_is_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return (node->type & DTL_AST_CLASS_MASK) == DTL_AST_CLASS_EXPRESSION;
}

/* --- Column Reference Expressions ------------------------------------------------------------- */

bool
dtl_ast_node_is_column_reference_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_COLUMN_REFERENCE_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_column_reference_expression_node_create(struct dtl_ast_node *name) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_column_name(name));

    node = dtl_ast_node_create_with_children(DTL_AST_COLUMN_REFERENCE_EXPRESSION);
    node = dtl_ast_node_append_child(node, name);

    return node;
}

struct dtl_ast_node *
dtl_ast_column_reference_expression_node_get_name(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_column_reference_expression(node));
    return dtl_ast_node_get_child(node, 0);
}

/* --- Literal Expressions ---------------------------------------------------------------------- */

bool
dtl_ast_node_is_literal_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_LITERAL_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_literal_expression_node_create(struct dtl_ast_node *value) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_literal(value));

    node = dtl_ast_node_create_with_children(DTL_AST_LITERAL_EXPRESSION);
    node = dtl_ast_node_append_child(node, value);

    return node;
}

struct dtl_ast_node *
dtl_ast_literal_expression_node_get_value(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_literal_expression(node));
    return dtl_ast_node_get_child(node, 0);
}

/* --- Function Call Expressions ---------------------------------------------------------------- */

bool
dtl_ast_node_is_function_call_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_FUNCTION_CALL_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_function_call_expression_node_create(struct dtl_ast_node *function, struct dtl_ast_node *arguments) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_name(function));
    assert(dtl_ast_node_is_expression_list(arguments));

    node = dtl_ast_node_create_with_children(DTL_AST_FUNCTION_CALL_EXPRESSION);
    node = dtl_ast_node_append_child(node, function);
    node = dtl_ast_node_append_child(node, arguments);

    return node;
}

struct dtl_ast_node *
dtl_ast_function_call_expression_node_get_function(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_function_call_expression(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_function_call_expression_node_get_arguments(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_function_call_expression(node));
    return dtl_ast_node_get_child(node, 1);
}

/* ---  Equal-to Expressions -------------------------------------------------------------------- */

bool
dtl_ast_node_is_equal_to_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_EQUAL_TO_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_equal_to_expression_node_create(struct dtl_ast_node *left, struct dtl_ast_node *right) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression(left));
    assert(dtl_ast_node_is_expression(right));

    node = dtl_ast_node_create_with_children(DTL_AST_EQUAL_TO_EXPRESSION);
    node = dtl_ast_node_append_child(node, left);
    node = dtl_ast_node_append_child(node, right);

    return node;
}

struct dtl_ast_node *
dtl_ast_equal_to_expression_node_get_left(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_equal_to_expression(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_equal_to_expression_node_get_right(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_equal_to_expression(node));
    return dtl_ast_node_get_child(node, 1);
}

/* --- Less-than Expressions -------------------------------------------------------------------- */

bool
dtl_ast_node_is_less_than_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_LESS_THAN_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_less_than_expression_node_create(struct dtl_ast_node *left, struct dtl_ast_node *right) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression(left));
    assert(dtl_ast_node_is_expression(right));

    node = dtl_ast_node_create_with_children(DTL_AST_LESS_THAN_EXPRESSION);
    node = dtl_ast_node_append_child(node, left);
    node = dtl_ast_node_append_child(node, right);

    return node;
}

struct dtl_ast_node *
dtl_ast_less_than_expression_node_get_left(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_less_than_expression(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_less_than_expression_node_get_right(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_less_than_expression(node));
    return dtl_ast_node_get_child(node, 1);
}

/* --- Less-than Expressions -------------------------------------------------------------------- */

bool
dtl_ast_node_is_less_than_or_equal_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_LESS_THAN_OR_EQUAL_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_less_than_or_equal_expression_node_create(struct dtl_ast_node *left, struct dtl_ast_node *right) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression(left));
    assert(dtl_ast_node_is_expression(right));

    node = dtl_ast_node_create_with_children(DTL_AST_LESS_THAN_OR_EQUAL_EXPRESSION);
    node = dtl_ast_node_append_child(node, left);
    node = dtl_ast_node_append_child(node, right);

    return node;
}

struct dtl_ast_node *
dtl_ast_less_than_or_equal_expression_node_get_left(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_less_than_or_equal_expression(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_less_than_or_equal_expression_node_get_right(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_less_than_or_equal_expression(node));
    return dtl_ast_node_get_child(node, 1);
}

/* --- Greater-than Expressions ----------------------------------------------------------------- */

bool
dtl_ast_node_is_greater_than_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_GREATER_THAN_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_greater_than_expression_node_create(struct dtl_ast_node *left, struct dtl_ast_node *right) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression(left));
    assert(dtl_ast_node_is_expression(right));

    node = dtl_ast_node_create_with_children(DTL_AST_GREATER_THAN_EXPRESSION);
    node = dtl_ast_node_append_child(node, left);
    node = dtl_ast_node_append_child(node, right);

    return node;
}

struct dtl_ast_node *
dtl_ast_greater_than_expression_node_get_left(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_greater_than_expression(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_greater_than_expression_node_get_right(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_greater_than_expression(node));
    return dtl_ast_node_get_child(node, 1);
}

/* --- Greater-than-or-equal Expressions -------------------------------------------------------- */

bool
dtl_ast_node_is_greater_than_or_equal_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_GREATER_THAN_OR_EQUAL_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_greater_than_or_equal_expression_node_create(struct dtl_ast_node *left, struct dtl_ast_node *right) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression(left));
    assert(dtl_ast_node_is_expression(right));

    node = dtl_ast_node_create_with_children(DTL_AST_GREATER_THAN_OR_EQUAL_EXPRESSION);
    node = dtl_ast_node_append_child(node, left);
    node = dtl_ast_node_append_child(node, right);

    return node;
}

struct dtl_ast_node *
dtl_ast_greater_than_or_equal_expression_node_get_left(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_greater_than_or_equal_expression(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_greater_than_or_equal_expression_node_get_right(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_greater_than_or_equal_expression(node));
    return dtl_ast_node_get_child(node, 1);
}

/* --- Add Expressions -------------------------------------------------------------------------- */

bool
dtl_ast_node_is_add_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_ADD_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_add_expression_node_create(struct dtl_ast_node *left, struct dtl_ast_node *right) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression(left));
    assert(dtl_ast_node_is_expression(right));

    node = dtl_ast_node_create_with_children(DTL_AST_ADD_EXPRESSION);
    node = dtl_ast_node_append_child(node, left);
    node = dtl_ast_node_append_child(node, right);

    return node;
}

struct dtl_ast_node *
dtl_ast_add_expression_node_get_left(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_add_expression(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_add_expression_node_get_right(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_add_expression(node));
    return dtl_ast_node_get_child(node, 1);
}

/* --- Subtract Expressions --------------------------------------------------------------------- */

bool
dtl_ast_node_is_subtract_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_SUBTRACT_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_subtract_expression_node_create(struct dtl_ast_node *left, struct dtl_ast_node *right) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression(left));
    assert(dtl_ast_node_is_expression(right));

    node = dtl_ast_node_create_with_children(DTL_AST_SUBTRACT_EXPRESSION);
    node = dtl_ast_node_append_child(node, left);
    node = dtl_ast_node_append_child(node, right);

    return node;
}

struct dtl_ast_node *
dtl_ast_subtract_expression_node_get_left(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_subtract_expression(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_subtract_expression_node_get_right(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_subtract_expression(node));
    return dtl_ast_node_get_child(node, 1);
}

/* --- Multiply Expressions --------------------------------------------------------------------- */

bool
dtl_ast_node_is_multiply_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_MULTIPLY_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_multiply_expression_node_create(struct dtl_ast_node *left, struct dtl_ast_node *right) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression(left));
    assert(dtl_ast_node_is_expression(right));

    node = dtl_ast_node_create_with_children(DTL_AST_MULTIPLY_EXPRESSION);
    node = dtl_ast_node_append_child(node, left);
    node = dtl_ast_node_append_child(node, right);

    return node;
}

struct dtl_ast_node *
dtl_ast_multiply_expression_node_get_left(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_multiply_expression(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_multiply_expression_node_get_right(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_multiply_expression(node));
    return dtl_ast_node_get_child(node, 1);
}

/* --- Divide Expressions ----------------------------------------------------------------------- */

bool
dtl_ast_node_is_divide_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_DIVIDE_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_divide_expression_node_create(struct dtl_ast_node *left, struct dtl_ast_node *right) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression(left));
    assert(dtl_ast_node_is_expression(right));

    node = dtl_ast_node_create_with_children(DTL_AST_DIVIDE_EXPRESSION);
    node = dtl_ast_node_append_child(node, left);
    node = dtl_ast_node_append_child(node, right);

    return node;
}

struct dtl_ast_node *
dtl_ast_divide_expression_node_get_left(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_divide_expression(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_divide_expression_node_get_right(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_divide_expression(node));
    return dtl_ast_node_get_child(node, 1);
}

/* === Column Bindings ========================================================================== */

bool
dtl_ast_node_is_column_binding(struct dtl_ast_node *node) {
    assert(node != NULL);
    return (node->type & DTL_AST_CLASS_MASK) == DTL_AST_CLASS_COLUMN_BINDING;
}

/* --- Wildcard Column Bindings ----------------------------------------------------------------- */

bool
dtl_ast_node_is_wildcard_column_binding(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_WILDCARD_COLUMN_BINDING;
}

struct dtl_ast_node *
dtl_ast_wildcard_column_binding_node_create(void) {
    struct dtl_ast_node *node;

    node = dtl_ast_node_create_empty(DTL_AST_WILDCARD_COLUMN_BINDING);

    return node;
}

/* --- Implicit Column Bindings ----------------------------------------------------------------- */

bool
dtl_ast_node_is_implicit_column_binding(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_IMPLICIT_COLUMN_BINDING;
}

struct dtl_ast_node *
dtl_ast_implicit_column_binding_node_create(struct dtl_ast_node *expression) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression(expression));

    node = dtl_ast_node_create_with_children(DTL_AST_IMPLICIT_COLUMN_BINDING);
    node = dtl_ast_node_append_child(node, expression);

    return node;
}

struct dtl_ast_node *
dtl_ast_implicit_column_binding_node_get_expression(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_implicit_column_binding(node));
    return dtl_ast_node_get_child(node, 0);
}

/* --- Explicit Column Bindings ----------------------------------------------------------------- */

bool
dtl_ast_node_is_aliased_column_binding(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_ALIASED_COLUMN_BINDING;
}

struct dtl_ast_node *
dtl_ast_aliased_column_binding_node_create(struct dtl_ast_node *expression, struct dtl_ast_node *alias) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression(expression));
    assert(dtl_ast_node_is_name(alias));

    node = dtl_ast_node_create_with_children(DTL_AST_ALIASED_COLUMN_BINDING);
    node = dtl_ast_node_append_child(node, expression);
    node = dtl_ast_node_append_child(node, alias);

    return node;
}

struct dtl_ast_node *
dtl_ast_aliased_column_binding_node_get_expression(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_aliased_column_binding(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_aliased_column_binding_node_get_alias(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_aliased_column_binding(node));
    return dtl_ast_node_get_child(node, 1);
}

/* === Table Bindings =========================================================================== */

bool
dtl_ast_node_is_table_binding(struct dtl_ast_node *node) {
    assert(node != NULL);
    return (node->type & DTL_AST_CLASS_MASK) == DTL_AST_CLASS_TABLE_BINDING;
}

/* --- Implicit Table Bindings ------------------------------------------------------------------ */

bool
dtl_ast_node_is_implicit_table_binding(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_IMPLICIT_TABLE_BINDING;
}

struct dtl_ast_node *
dtl_ast_implicit_table_binding_node_create(struct dtl_ast_node *table_expression) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_table_expression(table_expression));

    node = dtl_ast_node_create_with_children(DTL_AST_IMPLICIT_TABLE_BINDING);
    node = dtl_ast_node_append_child(node, table_expression);

    return node;
}

struct dtl_ast_node *
dtl_ast_implicit_table_binding_node_get_expression(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_implicit_table_binding(node));
    return dtl_ast_node_get_child(node, 0);
}

/* --- Explicit Table Bindings ------------------------------------------------------------------ */

bool
dtl_ast_node_is_aliased_table_binding(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_ALIASED_TABLE_BINDING;
}

struct dtl_ast_node *
dtl_ast_aliased_table_binding_node_create(
    struct dtl_ast_node *table_expression, struct dtl_ast_node *alias
) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_table_expression(table_expression));
    assert(dtl_ast_node_is_name(alias));

    node = dtl_ast_node_create_with_children(DTL_AST_ALIASED_TABLE_BINDING);
    node = dtl_ast_node_append_child(node, table_expression);
    node = dtl_ast_node_append_child(node, alias);

    return node;
}

struct dtl_ast_node *
dtl_ast_aliased_table_binding_node_get_expression(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_aliased_table_binding(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_aliased_table_binding_node_get_alias(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_aliased_table_binding(node));
    return dtl_ast_node_get_child(node, 1);
}

/* === Distinct Clauses ========================================================================= */

bool
dtl_ast_node_is_distinct_clause(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_DISTINCT_CLAUSE;
}

struct dtl_ast_node *
dtl_ast_distinct_clause_node_create(void) {
    struct dtl_ast_node *node;

    node = dtl_ast_node_create_empty(DTL_AST_DISTINCT_CLAUSE);

    return node;
}

bool
dtl_ast_node_is_distinct_consecutive_clause(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_DISTINCT_CONSECUTIVE_CLAUSE;
}

struct dtl_ast_node *
dtl_ast_distinct_consecutive_clause_node_create(void) {
    struct dtl_ast_node *node;

    node = dtl_ast_node_create_empty(DTL_AST_DISTINCT_CONSECUTIVE_CLAUSE);

    return node;
}

/* === Join Constraints ========================================================================= */

bool
dtl_ast_node_is_join_constraint(struct dtl_ast_node *node) {
    assert(node != NULL);
    return (node->type & DTL_AST_CLASS_MASK) == DTL_AST_CLASS_JOIN_CONSTRAINT;
}

/* --- Join-on Constraints ---------------------------------------------------------------------- */

bool
dtl_ast_node_is_join_on_constraint(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_JOIN_ON_CONSTRAINT;
}

struct dtl_ast_node *
dtl_ast_join_on_constraint_node_create(struct dtl_ast_node *predicate) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_expression(predicate));

    node = dtl_ast_node_create_with_children(DTL_AST_JOIN_ON_CONSTRAINT);
    node = dtl_ast_node_append_child(node, predicate);

    return node;
}

struct dtl_ast_node *
dtl_ast_join_on_constraint_node_get_predicate(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_join_on_constraint(node));
    return dtl_ast_node_get_child(node, 0);
}

/* --- Join-using Constraints ------------------------------------------------------------------- */

bool
dtl_ast_node_is_join_using_constraint(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_JOIN_USING_CONSTRAINT;
}

struct dtl_ast_node *
dtl_ast_join_using_constraint_node_create(struct dtl_ast_node *child) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_unqualified_column_name(child));

    node = dtl_ast_node_create_with_children(DTL_AST_JOIN_USING_CONSTRAINT);
    node = dtl_ast_node_append_child(node, child);

    return node;
}

struct dtl_ast_node *
dtl_ast_join_using_constraint_node_append(struct dtl_ast_node *node, struct dtl_ast_node *child) {
    assert(dtl_ast_node_is_join_using_constraint(node));
    assert(dtl_ast_node_is_unqualified_column_name(child));

    return dtl_ast_node_append_child(node, child);
}

size_t
dtl_ast_join_using_constraint_node_get_num_names(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_join_using_constraint(node));
    return dtl_ast_node_get_num_children(node);
}

struct dtl_ast_node *
dtl_ast_join_using_constraint_node_get_name(struct dtl_ast_node *node, size_t index) {
    assert(dtl_ast_node_is_join_using_constraint(node));
    return dtl_ast_node_get_child(node, index);
}

/* === Table Expressions ======================================================================== */

bool
dtl_ast_node_is_table_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return (node->type & DTL_AST_CLASS_MASK) == DTL_AST_CLASS_TABLE_EXPRESSION;
}

/* --- Select Expressions ----------------------------------------------------------------------- */

bool
dtl_ast_node_is_select_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_SELECT_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_select_expression_node_create(
    struct dtl_ast_node *distinct_clause,
    struct dtl_ast_node *columns,
    struct dtl_ast_node *source,
    struct dtl_ast_node *join_clauses,
    struct dtl_ast_node *where_clause,
    struct dtl_ast_node *group_by_clause
) {
    struct dtl_ast_node *node;

    assert(distinct_clause == NULL || dtl_ast_node_is_distinct_clause(distinct_clause));
    assert(dtl_ast_node_is_column_binding_list(columns));
    assert(dtl_ast_node_is_from_clause(source));
    assert(join_clauses == NULL || dtl_ast_node_is_join_clause_list(join_clauses));
    assert(where_clause == NULL || dtl_ast_node_is_where_clause(where_clause));
    assert(group_by_clause == NULL || dtl_ast_node_is_group_by_clause(group_by_clause));

    node = dtl_ast_node_create_with_children(DTL_AST_SELECT_EXPRESSION);
    node = dtl_ast_node_append_child(node, distinct_clause);
    node = dtl_ast_node_append_child(node, columns);
    node = dtl_ast_node_append_child(node, source);
    node = dtl_ast_node_append_child(node, join_clauses);
    node = dtl_ast_node_append_child(node, where_clause);
    node = dtl_ast_node_append_child(node, group_by_clause);

    return node;
}

struct dtl_ast_node *
dtl_ast_select_expression_node_get_distinct_clause(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_select_expression(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_select_expression_node_get_columns(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_select_expression(node));
    return dtl_ast_node_get_child(node, 1);
}

struct dtl_ast_node *
dtl_ast_select_expression_node_get_source(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_select_expression(node));
    return dtl_ast_node_get_child(node, 2);
}

struct dtl_ast_node *
dtl_ast_select_expression_node_get_join_clauses(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_select_expression(node));
    return dtl_ast_node_get_child(node, 3);
}

struct dtl_ast_node *
dtl_ast_select_expression_node_get_where_clause(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_select_expression(node));
    return dtl_ast_node_get_child(node, 4);
}

struct dtl_ast_node *
dtl_ast_select_expression_node_get_group_by_clause(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_select_expression(node));
    return dtl_ast_node_get_child(node, 5);
}

/* --- Import Expressions ----------------------------------------------------------------------- */

bool
dtl_ast_node_is_import_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_IMPORT_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_import_expression_node_create(struct dtl_ast_node *path) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_string_literal(path));

    node = dtl_ast_node_create_with_children(DTL_AST_IMPORT_EXPRESSION);
    node = dtl_ast_node_append_child(node, path);

    return node;
}

struct dtl_ast_node *
dtl_ast_import_expression_node_get_path(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_import_expression(node));
    return dtl_ast_node_get_child(node, 0);
}

/* --- Table Reference Expressions -------------------------------------------------------------- */

bool
dtl_ast_node_is_table_reference_expression(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_TABLE_REFERENCE_EXPRESSION;
}

struct dtl_ast_node *
dtl_ast_table_reference_expression_node_create(struct dtl_ast_node *name) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_name(name));

    node = dtl_ast_node_create_with_children(DTL_AST_TABLE_REFERENCE_EXPRESSION);
    node = dtl_ast_node_append_child(node, name);

    return node;
}

struct dtl_ast_node *
dtl_ast_table_reference_expression_node_get_name(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_table_reference_expression(node));
    return dtl_ast_node_get_child(node, 0);
    ;
}

/* === Statements =============================================================================== */

bool
dtl_ast_node_is_statement(struct dtl_ast_node *node) {
    assert(node != NULL);
    return (node->type & DTL_AST_CLASS_MASK) == DTL_AST_CLASS_STATEMENT;
}

/* --- Assignment Statements -------------------------------------------------------------------- */

bool
dtl_ast_node_is_assignment_statement(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_ASSIGNMENT_STATEMENT;
}

struct dtl_ast_node *
dtl_ast_assignment_statement_node_create(struct dtl_ast_node *table_expression, struct dtl_ast_node *name) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_table_expression(table_expression));
    assert(dtl_ast_node_is_table_name(name));

    node = dtl_ast_node_create_with_children(DTL_AST_ASSIGNMENT_STATEMENT);
    node = dtl_ast_node_append_child(node, table_expression);
    node = dtl_ast_node_append_child(node, name);

    return node;
}

struct dtl_ast_node *
dtl_ast_assignment_statement_node_get_table_expression(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_assignment_statement(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_assignment_statement_node_get_name(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_assignment_statement(node));
    return dtl_ast_node_get_child(node, 1);
}

/* --- Export Statements ------------------------------------------------------------------------ */

bool
dtl_ast_node_is_export_statement(struct dtl_ast_node *node) {
    assert(node != NULL);
    return node->type == DTL_AST_EXPORT_STATEMENT;
}

struct dtl_ast_node *
dtl_ast_export_statement_node_create(struct dtl_ast_node *expression, struct dtl_ast_node *path) {
    struct dtl_ast_node *node;

    assert(dtl_ast_node_is_table_expression(expression));
    assert(dtl_ast_node_is_string_literal(path));

    node = dtl_ast_node_create_with_children(DTL_AST_EXPORT_STATEMENT);
    node = dtl_ast_node_append_child(node, expression);
    node = dtl_ast_node_append_child(node, path);

    return node;
}

struct dtl_ast_node *
dtl_ast_export_statement_node_get_table_expression(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_export_statement(node));
    return dtl_ast_node_get_child(node, 0);
}

struct dtl_ast_node *
dtl_ast_export_statement_node_get_path(struct dtl_ast_node *node) {
    assert(dtl_ast_node_is_export_statement(node));
    return dtl_ast_node_get_child(node, 1);
}
