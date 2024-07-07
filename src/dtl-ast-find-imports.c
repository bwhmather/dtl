#include "dtl-ast-find-imports.h"

#include <stddef.h>

#include "dtl-ast.h"

void
dtl_ast_find_imports(struct dtl_ast_node *node, void (*callback)(struct dtl_ast_node *, void *), void *user_data) {
    if (node == NULL) {
        return;
    }

    switch (node->type) {
    case DTL_AST_NAME:
        // Intentionally left blank.
        break;

    case DTL_AST_STRING:
        // Intentionally left blank.
        break;

    case DTL_AST_TABLE_NAME:
        // Intentionally left blank.
        break;

    case DTL_AST_DISTINCT_CLAUSE:
        // Intentionally left blank.
        break;

    case DTL_AST_DISTINCT_CONSECUTIVE_CLAUSE:
        // Intentionally left blank.
        break;

    case DTL_AST_COLUMN_BINDING_LIST:
        for (size_t i = 0; i < node->children_length; i++) {
            dtl_ast_find_imports(dtl_ast_node_get_child(node, i), callback, user_data);
        }
        break;

    case DTL_AST_FROM_CLAUSE:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Source.
        break;

    case DTL_AST_JOIN_CLAUSE:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Table binding.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 1), callback, user_data); // Join constraint.
        break;

    case DTL_AST_JOIN_CLAUSE_LIST:
        for (size_t i = 0; i < node->children_length; i++) {
            dtl_ast_find_imports(dtl_ast_node_get_child(node, i), callback, user_data);
        }
        break;

    case DTL_AST_WHERE_CLAUSE:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Predicate.
        break;

    case DTL_AST_GROUP_BY_CLAUSE:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Pattern.
        break;

    case DTL_AST_GROUP_CONSECUTIVE_BY_CLAUSE:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Pattern.
        break;

    case DTL_AST_EXPRESSION_LIST:
        for (size_t i = 0; i < node->children_length; i++) {
            dtl_ast_find_imports(dtl_ast_node_get_child(node, i), callback, user_data);
        }
        break;

    case DTL_AST_STATEMENT_LIST:
        for (size_t i = 0; i < node->children_length; i++) {
            dtl_ast_find_imports(dtl_ast_node_get_child(node, i), callback, user_data);
        }
        break;

    case DTL_AST_SCRIPT:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Statement list.
        break;

    case DTL_AST_INT_LITERAL:
        // Intentionally left blank.
        break;

    case DTL_AST_STRING_LITERAL:
        // Intentionally left blank.
        break;

    case DTL_AST_UNQUALIFIED_COLUMN_NAME:
        // Intentionally left blank.
        break;

    case DTL_AST_QUALIFIED_COLUMN_NAME:
        // Intentionally left blank.
        break;

    case DTL_AST_COLUMN_REFERENCE_EXPRESSION:
        // Intentionally left blank.
        break;

    case DTL_AST_LITERAL_EXPRESSION:
        // Intentionally left blank.
        break;

    case DTL_AST_FUNCTION_CALL_EXPRESSION:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 1), callback, user_data); // Argument list.
        break;

    case DTL_AST_EQUAL_TO_EXPRESSION:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Left.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 1), callback, user_data); // Right.
        break;

    case DTL_AST_LESS_THAN_EXPRESSION:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Left.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 1), callback, user_data); // Right.
        break;

    case DTL_AST_LESS_THAN_OR_EQUAL_EXPRESSION:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Left.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 1), callback, user_data); // Right.
        break;

    case DTL_AST_GREATER_THAN_EXPRESSION:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Left.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 1), callback, user_data); // Right.
        break;

    case DTL_AST_GREATER_THAN_OR_EQUAL_EXPRESSION:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Left.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 1), callback, user_data); // Right.
        break;

    case DTL_AST_ADD_EXPRESSION:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Left.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 1), callback, user_data); // Right.
        break;

    case DTL_AST_SUBTRACT_EXPRESSION:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Left.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 1), callback, user_data); // Right.
        break;

    case DTL_AST_MULTIPLY_EXPRESSION:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Left.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 1), callback, user_data); // Right.
        break;

    case DTL_AST_DIVIDE_EXPRESSION:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Left.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 1), callback, user_data); // Right.
        break;

    case DTL_AST_WILDCARD_COLUMN_BINDING:
        // Intentionally left blank.
        break;

    case DTL_AST_IMPLICIT_COLUMN_BINDING:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Expression.
        break;

    case DTL_AST_ALIASED_COLUMN_BINDING:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Expression.
        break;

    case DTL_AST_IMPLICIT_TABLE_BINDING:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Expression.
        // Alias ignored.
        break;

    case DTL_AST_ALIASED_TABLE_BINDING:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Expression.
        // Alias ignored.
        break;

    case DTL_AST_JOIN_ON_CONSTRAINT:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Predicate.
        break;

    case DTL_AST_JOIN_USING_CONSTRAINT:
        // Intentionally left blank.
        break;

    case DTL_AST_SELECT_EXPRESSION:
        // Distinct clause not traversed.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 1), callback, user_data); // Columns.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 2), callback, user_data); // Source.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 3), callback, user_data); // Joins.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 4), callback, user_data); // Where.
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 5), callback, user_data); // Group by.
        break;

    case DTL_AST_IMPORT_EXPRESSION:
        callback(node, user_data);
        break;

    case DTL_AST_TABLE_REFERENCE_EXPRESSION:
        // Intentionally left blank.
        break;

    case DTL_AST_ASSIGNMENT_STATEMENT:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Expression.
        // Binding ignored.
        break;

        // TODO
        //    case DTL_AST_UPDATE_STATEMENT:
        //        break;
        //    case DTL_AST_DELETE_STATEMENT:
        //        break;
        //    case DTL_AST_INSERT_STATEMENT:
        //        break;

    case DTL_AST_EXPORT_STATEMENT:
        dtl_ast_find_imports(dtl_ast_node_get_child(node, 0), callback, user_data); // Expression.
        // Name ignored.
        break;
    }
}
