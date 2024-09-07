#include "dtl-ast-find-imports.h"

#include <stddef.h>

#include "dtl-ast.h"

void
dtl_ast_find_imports(struct dtl_ast_node *node, void (*callback)(struct dtl_ast_node *, void *), void *user_data) {
    if (node == NULL) {
        return;
    }

    if (dtl_ast_node_is_name(node)) {
        // Intentionally left blank.
        return;
    }

    if (dtl_ast_node_is_string_literal(node)) {
        // Intentionally left blank.
        return;
    }

    if (dtl_ast_node_is_table_name(node)) {
        // Intentionally left blank.
        return;
    }

    if (dtl_ast_node_is_distinct_clause(node)) {
        // Intentionally left blank.
        return;
    }

    if (dtl_ast_node_is_distinct_consecutive_clause(node)) {
        // Intentionally left blank.
        return;
    }

    if (dtl_ast_node_is_column_binding_list(node)) {
        for (size_t i = 0; i < dtl_ast_column_binding_list_node_get_num_bindings(node); i++) {
            dtl_ast_find_imports(dtl_ast_column_binding_list_node_get_binding(node, i), callback, user_data);
        }
        return;
    }

    if (dtl_ast_node_is_from_clause(node)) {
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Source.
        return;
    }

    if (dtl_ast_node_is_join_clause(node)) {
        dtl_ast_find_imports(dtl_ast_join_clause_node_get_table_binding(node), callback, user_data);
        dtl_ast_find_imports(dtl_ast_join_clause_node_get_constraint(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_join_clause_list(node)) {
        for (size_t i = 0; i < dtl_ast_join_clause_list_node_get_num_clauses(node); i++) {
            dtl_ast_find_imports(dtl_ast_join_clause_list_node_get_clause(node, i), callback, user_data);
        }
        return;
    }

    if (dtl_ast_node_is_where_clause(node)) {
        dtl_ast_find_imports(dtl_ast_where_clause_node_get_predicate(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_group_by_clause(node)) {
        dtl_ast_find_imports(dtl_ast_group_by_clause_node_get_pattern(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_group_consecutive_by_clause(node)) {
        dtl_ast_find_imports(dtl_ast_group_consecutive_by_clause_node_get_expression_list(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_expression_list(node)) {
        for (size_t i = 0; i < dtl_ast_expression_list_node_get_num_expressions(node); i++) {
            dtl_ast_find_imports(dtl_ast_expression_list_node_get_expression(node, i), callback, user_data);
        }
        return;
    }

    if (dtl_ast_node_is_statement_list(node)) {
        for (size_t i = 0; i < dtl_ast_statement_list_node_get_num_statements(node); i++) {
            dtl_ast_find_imports(dtl_ast_statement_list_node_get_statement(node, i), callback, user_data);
        }
        return;
    }

    if (dtl_ast_node_is_script(node)) {
        dtl_ast_find_imports(dtl_ast_script_node_get_statements(node), callback, user_data); // Statement list.
        return;
    }

    if (dtl_ast_node_is_int_literal(node)) {
        // Intentionally left blank.
        return;
    }

    if (dtl_ast_node_is_string_literal(node)) {
        // Intentionally left blank.
        return;
    }

    if (dtl_ast_node_is_unqualified_column_name(node)) {
        // Intentionally left blank.
        return;
    }

    if (dtl_ast_node_is_qualified_column_name(node)) {
        // Intentionally left blank.
        return;
    }

    if (dtl_ast_node_is_column_reference_expression(node)) {
        // Intentionally left blank.
        return;
    }

    if (dtl_ast_node_is_literal_expression(node)) {
        // Intentionally left blank.
        return;
    }

    if (dtl_ast_node_is_function_call_expression(node)) {
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 1), callback, user_data); // Argument list.
        return;
    }

    if (dtl_ast_node_is_equal_to_expression(node)) {
        dtl_ast_find_imports(dtl_ast_equal_to_expression_node_get_left(node), callback, user_data);
        dtl_ast_find_imports(dtl_ast_equal_to_expression_node_get_right(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_less_than_expression(node)) {
        dtl_ast_find_imports(dtl_ast_less_than_expression_node_get_left(node), callback, user_data);
        dtl_ast_find_imports(dtl_ast_less_than_expression_node_get_right(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_less_than_or_equal_to_expression(node)) {
        dtl_ast_find_imports(dtl_ast_less_than_or_equal_to_expression_node_get_left(node), callback, user_data);
        dtl_ast_find_imports(dtl_ast_less_than_or_equal_to_expression_node_get_right(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_greater_than_expression(node)) {
        dtl_ast_find_imports(dtl_ast_greater_than_expression_node_get_left(node), callback, user_data);
        dtl_ast_find_imports(dtl_ast_greater_than_expression_node_get_right(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_greater_than_or_equal_to_expression(node)) {
        dtl_ast_find_imports(dtl_ast_greater_than_or_equal_to_expression_node_get_left(node), callback, user_data);
        dtl_ast_find_imports(dtl_ast_greater_than_or_equal_to_expression_node_get_right(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_add_expression(node)) {
        dtl_ast_find_imports(dtl_ast_add_expression_node_get_left(node), callback, user_data);
        dtl_ast_find_imports(dtl_ast_add_expression_node_get_right(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_subtract_expression(node)) {
        dtl_ast_find_imports(dtl_ast_subtract_expression_node_get_left(node), callback, user_data);
        dtl_ast_find_imports(dtl_ast_subtract_expression_node_get_right(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_multiply_expression(node)) {
        dtl_ast_find_imports(dtl_ast_multiply_expression_node_get_left(node), callback, user_data);
        dtl_ast_find_imports(dtl_ast_multiply_expression_node_get_right(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_divide_expression(node)) {
        dtl_ast_find_imports(dtl_ast_divide_expression_node_get_left(node), callback, user_data);
        dtl_ast_find_imports(dtl_ast_divide_expression_node_get_right(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_wildcard_column_binding(node)) {
        // Intentionally left blank.
        return;
    }

    if (dtl_ast_node_is_implicit_column_binding(node)) {
        dtl_ast_find_imports(dtl_ast_implicit_column_binding_node_get_expression(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_aliased_column_binding(node)) {
        dtl_ast_find_imports(dtl_ast_aliased_column_binding_node_get_expression(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_implicit_table_binding(node)) {
        dtl_ast_find_imports(dtl_ast_implicit_table_binding_node_get_expression(node), callback, user_data);
        // Alias ignored.
        return;
    }

    if (dtl_ast_node_is_aliased_table_binding(node)) {
        dtl_ast_find_imports(dtl_ast_aliased_table_binding_node_get_expression(node), callback, user_data);
        // Alias ignored.
        return;
    }

    if (dtl_ast_node_is_join_on_constraint(node)) {
        dtl_ast_find_imports(dtl_ast_join_on_constraint_node_get_predicate(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_join_using_constraint(node)) {
        // Intentionally left blank.
        return;
    }

    if (dtl_ast_node_is_select_expression(node)) {
        // Distinct clause not traversed.
        dtl_ast_find_imports(dtl_ast_select_expression_node_get_columns(node), callback, user_data);
        dtl_ast_find_imports(dtl_ast_select_expression_node_get_source(node), callback, user_data);
        dtl_ast_find_imports(dtl_ast_select_expression_node_get_join_clauses(node), callback, user_data);
        dtl_ast_find_imports(dtl_ast_select_expression_node_get_where_clause(node), callback, user_data);
        dtl_ast_find_imports(dtl_ast_select_expression_node_get_group_by_clause(node), callback, user_data);
        return;
    }

    if (dtl_ast_node_is_import_expression(node)) {
        callback(node, user_data);
        return;
    }

    if (dtl_ast_node_is_table_reference_expression(node)) {
        // Intentionally left blank.
        return;
    }

    if (dtl_ast_node_is_assignment_statement(node)) {
        dtl_ast_find_imports(dtl_ast_assignment_statement_node_get_table_expression(node), callback, user_data);
        // Binding ignored.
        return;
    }
    // TODO
    //    case DTL_AST_UPDATE_STATEMENT:
    //        return;
    //    case DTL_AST_DELETE_STATEMENT:
    //        return;
    //    case DTL_AST_INSERT_STATEMENT:
    //        return;
    if (dtl_ast_node_is_export_statement(node)) {
        dtl_ast_find_imports(dtl_ast_export_statement_node_get_table_expression(node), callback, user_data);
        // Name ignored.
        return;
    }
}
