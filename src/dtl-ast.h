#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "dtl-location.h"

struct dtl_ast_node;

/* === Generic ================================================================================== */

void
dtl_ast_node_update_bounds(struct dtl_ast_node *, struct dtl_location start, struct dtl_location end);

struct dtl_location
dtl_ast_node_get_start(struct dtl_ast_node *);

struct dtl_location
dtl_ast_node_get_end(struct dtl_ast_node *);

void
dtl_ast_node_destroy(struct dtl_ast_node *node);

/* --- Container Nodes -------------------------------------------------------------------------- */

bool
dtl_ast_node_has_children(struct dtl_ast_node *);

size_t
dtl_ast_node_get_num_children(struct dtl_ast_node *node);

struct dtl_ast_node *
dtl_ast_node_get_child(struct dtl_ast_node *node, size_t index);

/* --- Data Nodes ------------------------------------------------------------------------------- */

bool
dtl_ast_node_has_data(struct dtl_ast_node *);

size_t
dtl_ast_node_get_data_size(struct dtl_ast_node *);

void *
dtl_ast_node_get_data(struct dtl_ast_node *);

/* === Type Spectific =========================================================================== */

/* --- Names ------------------------------------------------------------------------------------ */

bool
dtl_ast_node_is_name(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_name_node_create(char const *source);

char const *
dtl_ast_name_node_get_value(struct dtl_ast_node *);

/* --- Table Names ------------------------------------------------------------------------------ */

bool
dtl_ast_node_is_table_name(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_table_name_node_create(struct dtl_ast_node *name);

struct dtl_ast_node *
dtl_ast_table_name_node_get_value(struct dtl_ast_node *);

/* --- Column Binding Lists --------------------------------------------------------------------- */

bool
dtl_ast_node_is_column_binding_list(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_column_binding_list_node_create(void);

struct dtl_ast_node *
dtl_ast_column_binding_list_node_append(struct dtl_ast_node *, struct dtl_ast_node *);

size_t
dtl_ast_column_binding_list_node_get_num_bindings(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_column_binding_list_node_get_binding(struct dtl_ast_node *, size_t index);

/* --- From Clauses ----------------------------------------------------------------------------- */

bool
dtl_ast_node_is_from_clause(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_from_clause_node_create(struct dtl_ast_node *table_binding);

struct dtl_ast_node *
dtl_ast_from_clause_node_get_table_binding(struct dtl_ast_node *);

/* --- Join Clauses ----------------------------------------------------------------------------- */

bool
dtl_ast_node_is_join_clause(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_join_clause_node_create(struct dtl_ast_node *table_binding, struct dtl_ast_node *constraint);

struct dtl_ast_node *
dtl_ast_join_clause_node_get_table_binding(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_join_clause_node_get_constraint(struct dtl_ast_node *);

/* --- Join Clause Lists ------------------------------------------------------------------------ */

bool
dtl_ast_node_is_join_clause_list(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_join_clause_list_node_create(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_join_clause_list_node_append(struct dtl_ast_node *list, struct dtl_ast_node *clause);

size_t
dtl_ast_join_clause_list_node_get_num_clauses(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_join_clause_list_node_get_clause(struct dtl_ast_node *, size_t index);

/* --- Where Clauses ---------------------------------------------------------------------------- */

bool
dtl_ast_node_is_where_clause(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_where_clause_node_create(struct dtl_ast_node *expression);

struct dtl_ast_node *
dtl_ast_where_clause_node_get_predicate(struct dtl_ast_node *);

/* --- Group By Clauses ------------------------------------------------------------------------- */

bool
dtl_ast_node_is_group_by_clause(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_group_by_clause_node_create(struct dtl_ast_node *pattern);

struct dtl_ast_node *
dtl_ast_group_by_clause_node_get_pattern(struct dtl_ast_node *);

/* --- Group Consecutive By Clauses ------------------------------------------------------------- */

bool
dtl_ast_node_is_group_consecutive_by_clause(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_group_consecutive_by_clause_node_create(struct dtl_ast_node *expression_list);

struct dtl_ast_node *
dtl_ast_group_consecutive_by_clause_node_get_expression_list(struct dtl_ast_node *);

/* --- Expression Lists ------------------------------------------------------------------------- */

bool
dtl_ast_node_is_expression_list(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_expression_list_node_create(struct dtl_ast_node *head);

struct dtl_ast_node *
dtl_ast_expression_list_node_append(struct dtl_ast_node *node, struct dtl_ast_node *expression);

size_t
dtl_ast_expression_list_node_get_num_expressions(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_expression_list_node_get_expression(struct dtl_ast_node *, size_t index);

/* --- Statement Lists -------------------------------------------------------------------------- */

bool
dtl_ast_node_is_statement_list(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_statement_list_node_create(void);

struct dtl_ast_node *
dtl_ast_statement_list_node_append(struct dtl_ast_node *, struct dtl_ast_node *);

size_t
dtl_ast_statement_list_node_get_num_statements(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_statement_list_node_get_statement(struct dtl_ast_node *, size_t index);

/* --- Scripts ---------------------------------------------------------------------------------- */

bool
dtl_ast_node_is_script(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_script_node_create(struct dtl_ast_node *statements);

struct dtl_ast_node *
dtl_ast_script_node_get_statements(struct dtl_ast_node *);

/* === Literals ================================================================================= */

bool
dtl_ast_node_is_literal(struct dtl_ast_node *);

/* --- Integer Literals ------------------------------------------------------------------------- */

bool
dtl_ast_node_is_int_literal(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_int_literal_node_create(int64_t);

int64_t
dtl_ast_int_literal_node_get_value(struct dtl_ast_node *);

/* --- String Literals -------------------------------------------------------------------------- */

bool
dtl_ast_node_is_string_literal(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_string_literal_node_create(char const *);

char const *
dtl_ast_string_literal_node_get_value(struct dtl_ast_node *);

/* === Column Names ============================================================================= */

bool
dtl_ast_node_is_column_name(struct dtl_ast_node *);

/* --- Unqualified Column Names ----------------------------------------------------------------- */

bool
dtl_ast_node_is_unqualified_column_name(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_unqualified_column_name_node_create(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_unqualified_column_name_get_column_name(struct dtl_ast_node *);

/* --- Qualified Column Names ------------------------------------------------------------------- */

bool
dtl_ast_node_is_qualified_column_name(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_qualified_column_name_node_create(struct dtl_ast_node *, struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_qualified_column_name_get_table_name(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_qualified_column_name_get_column_name(struct dtl_ast_node *);

/* === Expressions ============================================================================== */

bool
dtl_ast_node_is_expression(struct dtl_ast_node *);

/* --- Column Reference Expressions ------------------------------------------------------------- */

bool
dtl_ast_node_is_column_reference_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_column_reference_expression_node_create(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_column_reference_expression_node_get_name(struct dtl_ast_node *);

/* --- Literal Expressions ---------------------------------------------------------------------- */

bool
dtl_ast_node_is_literal_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_literal_expression_node_create(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_literal_expression_node_get_value(struct dtl_ast_node *);

/* --- Function Call Expressions ---------------------------------------------------------------- */

bool
dtl_ast_node_is_function_call_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_function_call_expression_node_create(struct dtl_ast_node *, struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_function_call_expression_node_get_function(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_function_call_expression_node_get_arguments(struct dtl_ast_node *);

/* ---  Equal-to Expressions -------------------------------------------------------------------- */

bool
dtl_ast_node_is_equal_to_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_equal_to_expression_node_create(struct dtl_ast_node *, struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_equal_to_expression_node_get_left(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_equal_to_expression_node_get_right(struct dtl_ast_node *);

/* --- Less-than Expressions -------------------------------------------------------------------- */

bool
dtl_ast_node_is_less_than_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_less_than_expression_node_create(struct dtl_ast_node *, struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_less_than_expression_node_get_left(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_less_than_expression_node_get_right(struct dtl_ast_node *);

/* --- Less-than-or-equal Expressions ----------------------------------------------------------- */

bool
dtl_ast_node_is_less_than_or_equal_to_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_less_than_or_equal_to_expression_node_create(struct dtl_ast_node *, struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_less_than_or_equal_to_expression_node_get_left(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_less_than_or_equal_to_expression_node_get_right(struct dtl_ast_node *);

/* --- Greater-than Expressions ----------------------------------------------------------------- */

bool
dtl_ast_node_is_greater_than_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_greater_than_expression_node_create(struct dtl_ast_node *, struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_greater_than_expression_node_get_left(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_greater_than_expression_node_get_right(struct dtl_ast_node *);

/* --- Greater-than-or-equal Expressions -------------------------------------------------------- */

bool
dtl_ast_node_is_greater_than_or_equal_to_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_greater_than_or_equal_to_expression_node_create(struct dtl_ast_node *, struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_greater_than_or_equal_to_expression_node_get_left(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_greater_than_or_equal_to_expression_node_get_right(struct dtl_ast_node *);

/* --- Add Expressions -------------------------------------------------------------------------- */

bool
dtl_ast_node_is_add_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_add_expression_node_create(struct dtl_ast_node *, struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_add_expression_node_get_left(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_add_expression_node_get_right(struct dtl_ast_node *);

/* --- Subtract Expressions --------------------------------------------------------------------- */

bool
dtl_ast_node_is_subtract_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_subtract_expression_node_create(struct dtl_ast_node *, struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_subtract_expression_node_get_left(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_subtract_expression_node_get_right(struct dtl_ast_node *);

/* --- Multiply Expressions --------------------------------------------------------------------- */

bool
dtl_ast_node_is_multiply_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_multiply_expression_node_create(struct dtl_ast_node *, struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_multiply_expression_node_get_left(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_multiply_expression_node_get_right(struct dtl_ast_node *);

/* --- Divide Expressions ----------------------------------------------------------------------- */

bool
dtl_ast_node_is_divide_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_divide_expression_node_create(struct dtl_ast_node *, struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_divide_expression_node_get_left(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_divide_expression_node_get_right(struct dtl_ast_node *);

/* === Column Bindings ========================================================================== */

bool
dtl_ast_node_is_column_binding(struct dtl_ast_node *);

/* --- Wildcard Column Bindings ----------------------------------------------------------------- */

bool
dtl_ast_node_is_wildcard_column_binding(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_wildcard_column_binding_node_create(void);

/* --- Implicit Column Bindings ----------------------------------------------------------------- */

bool
dtl_ast_node_is_implicit_column_binding(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_implicit_column_binding_node_create(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_implicit_column_binding_node_get_expression(struct dtl_ast_node *);

/* --- Explicit Column Bindings ----------------------------------------------------------------- */

bool
dtl_ast_node_is_aliased_column_binding(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_aliased_column_binding_node_create(struct dtl_ast_node *expression, struct dtl_ast_node *alias);

struct dtl_ast_node *
dtl_ast_aliased_column_binding_node_get_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_aliased_column_binding_node_get_alias(struct dtl_ast_node *);

/* === Table Bindings =========================================================================== */

bool
dtl_ast_node_is_table_binding(struct dtl_ast_node *);

/* --- Implicit Table Bindings ------------------------------------------------------------------ */

bool
dtl_ast_node_is_implicit_table_binding(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_implicit_table_binding_node_create(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_implicit_table_binding_node_get_expression(struct dtl_ast_node *);

/* --- Explicit Table Bindings ------------------------------------------------------------------ */

bool
dtl_ast_node_is_aliased_table_binding(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_aliased_table_binding_node_create(struct dtl_ast_node *, struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_aliased_table_binding_node_get_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_aliased_table_binding_node_get_alias(struct dtl_ast_node *);

/* === Distinct Clauses ========================================================================= */

bool
dtl_ast_node_is_distinct_clause(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_distinct_clause_node_create(void);

bool
dtl_ast_node_is_distinct_consecutive_clause(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_distinct_consecutive_clause_node_create(void);

/* === Join Constraints ========================================================================= */

bool
dtl_ast_node_is_join_constraint(struct dtl_ast_node *);

/* --- Join-on Constraints ---------------------------------------------------------------------- */

bool
dtl_ast_node_is_join_on_constraint(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_join_on_constraint_node_create(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_join_on_constraint_node_get_predicate(struct dtl_ast_node *);

/* --- Join-using Constraints ------------------------------------------------------------------- */

bool
dtl_ast_node_is_join_using_constraint(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_join_using_constraint_node_create(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_join_using_constraint_node_append(struct dtl_ast_node *, struct dtl_ast_node *);

size_t
dtl_ast_join_using_constraint_node_get_num_names(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_join_using_constraint_node_get_name(struct dtl_ast_node *, size_t);

/* === Table Expressions ======================================================================== */

bool
dtl_ast_node_is_table_expression(struct dtl_ast_node *);

/* --- Select Expressions ----------------------------------------------------------------------- */

bool
dtl_ast_node_is_select_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_select_expression_node_create(
    struct dtl_ast_node *distinct,
    struct dtl_ast_node *columns,
    struct dtl_ast_node *source,
    struct dtl_ast_node *joins,
    struct dtl_ast_node *where,
    struct dtl_ast_node *group_by
);

struct dtl_ast_node *
dtl_ast_select_expression_node_get_distinct_clause(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_select_expression_node_get_columns(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_select_expression_node_get_source(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_select_expression_node_get_join_clauses(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_select_expression_node_get_where_clause(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_select_expression_node_get_group_by_clause(struct dtl_ast_node *);

/* --- Import Expressions ----------------------------------------------------------------------- */

bool
dtl_ast_node_is_import_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_import_expression_node_create(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_import_expression_node_get_path(struct dtl_ast_node *);

/* --- Table Reference Expressions -------------------------------------------------------------- */

bool
dtl_ast_node_is_table_reference_expression(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_table_reference_expression_node_create(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_table_reference_expression_node_get_name(struct dtl_ast_node *);

/* === Statements =============================================================================== */

bool
dtl_ast_node_is_statement(struct dtl_ast_node *);

/* --- Assignment Statements -------------------------------------------------------------------- */

bool
dtl_ast_node_is_assignment_statement(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_assignment_statement_node_create(struct dtl_ast_node *expression, struct dtl_ast_node *name);

struct dtl_ast_node *
dtl_ast_assignment_statement_node_get_table_expression(struct dtl_ast_node *node);

struct dtl_ast_node *
dtl_ast_assignment_statement_node_get_name(struct dtl_ast_node *node);

/* --- Export Statements ------------------------------------------------------------------------ */

bool
dtl_ast_node_is_export_statement(struct dtl_ast_node *);

struct dtl_ast_node *
dtl_ast_export_statement_node_create(struct dtl_ast_node *expression, struct dtl_ast_node *path);

struct dtl_ast_node *
dtl_ast_export_statement_node_get_table_expression(struct dtl_ast_node *node);

struct dtl_ast_node *
dtl_ast_export_statement_node_get_path(struct dtl_ast_node *node);
