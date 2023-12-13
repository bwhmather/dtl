#pragma once

#include <functional>

#include "dtl-ast.hpp"

namespace dtl {
namespace ast {

void
visit_string(
    const String& string,
    std::function<void(const Node&)> callback);

void
visit_literal(
    variant_ptr_t<const Literal> literal,
    std::function<void(const Node&)> callback);

void
visit_unqualified_column_name(
    const UnqualifiedColumnName& column_name,
    std::function<void(const Node&)> callback);

void
visit_qualified_column_name(
    const QualifiedColumnName& column_name,
    std::function<void(const Node&)> callback);

void
visit_column_name(
    variant_ptr_t<const ColumnName> base_column_name,
    std::function<void(const Node&)> callback);

void
visit_column_reference_expression(
    const ColumnReferenceExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_literal_expression(
    const LiteralExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_function_call_expression(
    const FunctionCallExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_equal_to_expression(
    const EqualToExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_less_than_expression(
    const LessThanExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_less_than_equal_expression(
    const LessThanEqualExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_greater_than_expression(
    const GreaterThanExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_greater_than_equal_expression(
    const GreaterThanEqualExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_add_expression(
    const AddExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_subtract_expression(
    const SubtractExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_multiply_expression(
    const MultiplyExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_divide_expression(
    const DivideExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_expression(
    variant_ptr_t<const Expression> base_expression,
    std::function<void(const Node&)> callback);
void
visit_wildcard_column_binding(
    const WildcardColumnBinding& binding,
    std::function<void(const Node&)> callback);

void
visit_implicit_column_binding(
    const ImplicitColumnBinding& binding,
    std::function<void(const Node&)> callback);

void
visit_aliased_column_binding(
    const AliasedColumnBinding& binding,
    std::function<void(const Node&)> callback);

void
visit_column_binding(
    variant_ptr_t<const ColumnBinding> base_binding,
    std::function<void(const Node&)> callback);

void
visit_implicit_table_binding(
    const ImplicitTableBinding& binding,
    std::function<void(const Node&)> callback);

void
visit_aliased_table_binding(
    const AliasedTableBinding& binding,
    std::function<void(const Node&)> callback);

void
visit_table_binding(
    variant_ptr_t<const TableBinding> base_binding,
    std::function<void(const Node&)> callback);

void
visit_from_clause(
    const FromClause& clause,
    std::function<void(const Node&)> callback);

void
visit_join_on_constraint(
    const JoinOnConstraint& constraint,
    std::function<void(const Node&)> callback);

void
visit_join_using_constraint(
    const JoinUsingConstraint& constraint,
    std::function<void(const Node&)> callback);

void
visit_join_constraint(
    variant_ptr_t<const JoinConstraint> base_constraint,
    std::function<void(const Node&)> callback);

void
visit_table_name(
    const TableName& table_name,
    std::function<void(const Node&)> callback);

void
visit_distinct_clause(
    const DistinctClause& clause,
    std::function<void(const Node&)> callback);

void
visit_join_clause(
    const JoinClause& clause,
    std::function<void(const Node&)> callback);

void
visit_where_clause(
    const WhereClause& clause,
    std::function<void(const Node&)> callback);

void
visit_group_by_clause(
    const GroupByClause& clause,
    std::function<void(const Node&)> callback);

void
visit_select_expression(
    const SelectExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_import_expression(
    const ImportExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_table_reference_expression(
    const TableReferenceExpression& expression,
    std::function<void(const Node&)> callback);

void
visit_table_expression(
    dtl::variant_ptr_t<const TableExpression> base_expression,
    std::function<void(const Node&)> callback);

void
visit_assignment_statement(
    const AssignmentStatement& statement,
    std::function<void(const Node&)> callback);

void
visit_update_statement(
    const UpdateStatement& statement,
    std::function<void(const Node&)> callback);

void
visit_delete_statement(
    const DeleteStatement& statement,
    std::function<void(const Node&)> callback);

void
visit_insert_statement(
    const InsertStatement& statement,
    std::function<void(const Node&)> callback);

void
visit_export_statement(
    const ExportStatement& statement,
    std::function<void(const Node&)> callback);

void
visit_begin_statement(
    const BeginStatement& statement,
    std::function<void(const Node&)> callback);

void
visit_statement(
    dtl::variant_ptr_t<const Statement> base_statement,
    std::function<void(const Node&)> callback);

void
visit_script(
    const Script& script,
    std::function<void(const Node&)> callback);

} /* namespace ast */
} /* namespace dtl */
