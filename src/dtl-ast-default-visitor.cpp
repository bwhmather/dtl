#include "dtl-ast-default-visitor.hpp"

#include <stddef.h>
#include <vector>
#include <string>
#include <memory>

#include "dtl-ast.hpp"

namespace dtl {
namespace ast {

/* === Literals ============================================================= */

void DefaultedLiteralVisitorMixin::visit_string(String& string) {
    return visit_literal(string);
}

/* === Column names ========================================================= */

void DefaultedColumnNameVisitorMixin::visit_unqualified_column_name(
    UnqualifiedColumnName& column_name
) {
    return visit_column_name(column_name);
}

void DefaultedColumnNameVisitorMixin::visit_qualified_column_name(
    QualifiedColumnName& column_name
) {
    return visit_column_name(column_name);
}

/* === Expressions ========================================================== */

void DefaultedExpressionVisitorMixin::visit_column_reference_expression(
    ColumnReferenceExpression& expr
) {
    return visit_expression(expr);
}

void DefaultedExpressionVisitorMixin::visit_literal_expression(
    LiteralExpression& expr
) {
    return visit_expression(expr);
}

void DefaultedExpressionVisitorMixin::visit_function_call_expression(
    FunctionCallExpression& expr
) {
    return visit_expression(expr);
}

void DefaultedExpressionVisitorMixin::visit_add_expression(
    AddExpression& expr
) {
    return visit_expression(expr);
}

void DefaultedExpressionVisitorMixin::visit_subtract_expression(
    SubtractExpression& expr
) {
    return visit_expression(expr);
}

void DefaultedExpressionVisitorMixin::visit_multiply_expression(
    MultiplyExpression& expr
) {
    return visit_expression(expr);
}

void DefaultedExpressionVisitorMixin::visit_divide_expression(
    DivideExpression& expr
) {
    return visit_expression(expr);
}

/* === Column Bindings ====================================================== */

void DefaultedColumnBindingVisitorMixin::visit_wildcard_column_binding(
    WildcardColumnBinding& binding
) {
    return visit_column_binding(binding);
}

void DefaultedColumnBindingVisitorMixin::visit_implicit_column_binding(
    ImplicitColumnBinding& binding
) {
    return visit_column_binding(binding);
}

void DefaultedColumnBindingVisitorMixin::visit_aliased_column_binding(
    AliasedColumnBinding& binding
) {
    return visit_column_binding(binding);
}

/* === Table Bindings ======================================================= */

void DefaultedTableBindingVisitorMixin::visit_implicit_table_binding(
    ImplicitTableBinding& binding
) {
    return visit_table_binding(binding);
}

void DefaultedTableBindingVisitorMixin::visit_aliased_table_binding(
    AliasedTableBinding& binding
) {
    return visit_table_binding(binding);
}

/* === Join Constraints ===================================================== */

void DefaultedJoinConstraintVisitorMixin::visit_join_on_constraint(
    JoinOnConstraint& constraint
) {
    return visit_join_constraint(constraint);
}

void DefaultedJoinConstraintVisitorMixin::visit_join_using_constraint(
    JoinUsingConstraint& constraint
) {
    return visit_join_constraint(constraint);
}

/* === Table Expressions ==================================================== */

void DefaultedTableExpressionVisitorMixin::visit_select_expression(
    SelectExpression& expr
) {
    return visit_table_expression(expr);
}

void DefaultedTableExpressionVisitorMixin::visit_import_expression(
    ImportExpression& expr
) {
    return visit_table_expression(expr);
}

void DefaultedTableExpressionVisitorMixin::visit_table_reference_expression(
    TableReferenceExpression& expr
) {
    return visit_table_expression(expr);
}

/* === Statements =========================================================== */

void DefaultedStatementVisitorMixin::visit_assignment_statement(
    AssignmentStatement& statement
) {
    return visit_statement(statement);
}

void DefaultedStatementVisitorMixin::visit_update_statement(
    UpdateStatement& statement
) {
    return visit_statement(statement);
}

void DefaultedStatementVisitorMixin::visit_delete_statement(
    DeleteStatement& statement
) {
    return visit_statement(statement);
}

void DefaultedStatementVisitorMixin::visit_insert_statement(
    InsertStatement& statement
) {
    return visit_statement(statement);
}

void DefaultedStatementVisitorMixin::visit_export_statement(
    ExportStatement& statement
) {
    return visit_statement(statement);
}

/* === Nodes ================================================================ */

void DefaultedNodeVisitorMixin::visit_column_name(ColumnName& column_name) {
    return visit_node(column_name);
}

void DefaultedNodeVisitorMixin::visit_literal(Literal& literal) {
    return visit_node(literal);
}

void DefaultedNodeVisitorMixin::visit_expression(Expression& expr) {
    return visit_node(expr);
}

void DefaultedNodeVisitorMixin::visit_column_binding(ColumnBinding& binding) {
    return visit_node(binding);
}

void DefaultedNodeVisitorMixin::visit_table_binding(TableBinding& binding) {
    return visit_node(binding);
}

void DefaultedNodeVisitorMixin::visit_join_constraint(JoinConstraint& constraint) {
    return visit_node(constraint);
}

void DefaultedNodeVisitorMixin::visit_table_expression(TableExpression& expr) {
    return visit_node(expr);
}

void DefaultedNodeVisitorMixin::visit_statement(Statement& statement) {
    return visit_node(statement);
}

void DefaultedNodeVisitorMixin::visit_table_name(TableName& table_name) {
    return visit_node(table_name);
}

void DefaultedNodeVisitorMixin::visit_distinct_clause(DistinctClause& clause) {
    return visit_node(clause);
}

void DefaultedNodeVisitorMixin::visit_from_clause(FromClause& clause) {
    return visit_node(clause);
}

void DefaultedNodeVisitorMixin::visit_join_clause(JoinClause& clause) {
    return visit_node(clause);
}

void DefaultedNodeVisitorMixin::visit_where_clause(WhereClause& clause) {
    return visit_node(clause);
}

void DefaultedNodeVisitorMixin::visit_group_by_clause(GroupByClause& clause) {
    return visit_node(clause);
}

void DefaultedNodeVisitorMixin::visit_script(Script& script) {
    return visit_node(script);
}

} /* namespace ast */
} /* namespace dtl */

