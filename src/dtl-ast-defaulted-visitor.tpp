#pragma once

#include "dtl-ast.hpp"

namespace dtl {
namespace ast {

template <typename T>
class DefaultedLiteralVisitorMixin : public T {
  public:
    virtual void
    visit_string(const String& string) override {
        return visit_literal(string);
    }

    virtual void
    visit_literal(const Literal& literal) = 0;
};

template <typename T>
class DefaultedColumnNameVisitorMixin : public T {
  public:
    virtual void
    visit_unqualified_column_name(
        const UnqualifiedColumnName& column_name) override {
        return visit_column_name(column_name);
    }

    virtual void
    visit_qualified_column_name(
        const QualifiedColumnName& column_name) override {
        return visit_column_name(column_name);
    }

    virtual void
    visit_column_name(const ColumnName& column_name) = 0;
};

template <typename T>
class DefaultedExpressionVisitorMixin : public T {
  public:
    virtual void
    visit_column_reference_expression(
        const ColumnReferenceExpression& expr) override {
        visit_expression(expr);
    }

    virtual void
    visit_literal_expression(const LiteralExpression& expr) override {
        visit_expression(expr);
    }

    virtual void
    visit_function_call_expression(
        const FunctionCallExpression& expr) override {
        visit_expression(expr);
    }

    virtual void
    visit_equal_to_expression(const EqualToExpression& expr) override {
        visit_expression(expr);
    }

    virtual void
    visit_less_than_expression(const LessThanExpression& expr) override {
        visit_expression(expr);
    }

    virtual void
    visit_less_than_equal_expression(
        const LessThanEqualExpression& expr) override {
        visit_expression(expr);
    }

    virtual void
    visit_greater_than_expression(const GreaterThanExpression& expr) override {
        visit_expression(expr);
    }

    virtual void
    visit_greater_than_equal_expression(
        const GreaterThanEqualExpression& expr) override {
        visit_expression(expr);
    }

    virtual void
    visit_add_expression(const AddExpression& expr) override {
        visit_expression(expr);
    }

    virtual void
    visit_subtract_expression(const SubtractExpression& expr) override {
        visit_expression(expr);
    }

    virtual void
    visit_multiply_expression(const MultiplyExpression& expr) override {
        visit_expression(expr);
    }

    virtual void
    visit_divide_expression(const DivideExpression& expr) override {
        visit_expression(expr);
    }

    virtual void
    visit_expression(const Expression& expr) = 0;
};

template <typename T>
class DefaultedColumnBindingVisitorMixin : public T {
  public:
    virtual void
    visit_wildcard_column_binding(
        const WildcardColumnBinding& binding) override {
        visit_column_binding(binding);
    }

    virtual void
    visit_implicit_column_binding(
        const ImplicitColumnBinding& binding) override {
        visit_column_binding(binding);
    }

    virtual void
    visit_aliased_column_binding(const AliasedColumnBinding& binding) override {
        visit_column_binding(binding);
    }

    virtual void
    visit_column_binding(const ColumnBinding& binding) = 0;
};

template <typename T>
class DefaultedTableBindingVisitorMixin : public T {
  public:
    virtual void
    visit_implicit_table_binding(const ImplicitTableBinding& binding) override {
        visit_table_binding(binding);
    }

    virtual void
    visit_aliased_table_binding(const AliasedTableBinding& binding) override {
        visit_table_binding(binding);
    }

    virtual void
    visit_table_binding(const TableBinding& binding) = 0;
};

template <typename T>
class DefaultedJoinConstraintVisitorMixin : public T {
  public:
    virtual void
    visit_join_on_constraint(const JoinOnConstraint& constraint) override {
        visit_join_constraint(constraint);
    }

    virtual void
    visit_join_using_constraint(
        const JoinUsingConstraint& constraint) override {
        visit_join_constraint(constraint);
    }

    virtual void
    visit_join_constraint(const JoinConstraint& constraint) = 0;
};

template <typename T>
class DefaultedTableExpressionVisitorMixin : public T {
  public:
    virtual void
    visit_select_expression(const SelectExpression& expr) override {
        visit_table_expression(expr);
    }

    virtual void
    visit_import_expression(const ImportExpression& expr) override {
        visit_table_expression(expr);
    }

    virtual void
    visit_table_reference_expression(
        const TableReferenceExpression& expr) override {
        visit_table_expression(expr);
    }

    virtual void
    visit_table_expression(const TableExpression& expr) = 0;
};

template <typename T>
class DefaultedStatementVisitorMixin : public T {
  public:
    virtual void
    visit_assignment_statement(const AssignmentStatement& statement) override {
        visit_statement(statement);
    }

    virtual void
    visit_update_statement(const UpdateStatement& statement) override {
        visit_statement(statement);
    }

    virtual void
    visit_delete_statement(const DeleteStatement& statement) override {
        visit_statement(statement);
    }

    virtual void
    visit_insert_statement(const InsertStatement& statement) override {
        visit_statement(statement);
    }

    virtual void
    visit_export_statement(const ExportStatement& statement) override {
        visit_statement(statement);
    }

    virtual void
    visit_begin_statement(const BeginStatement& statement) override {
        visit_statement(statement);
    }

    virtual void
    visit_statement(const Statement& statement) = 0;
};

template <typename T>
class DefaultedNodeVisitorMixin :
    public DefaultedLiteralVisitorMixin<DefaultedColumnNameVisitorMixin<
        DefaultedExpressionVisitorMixin<DefaultedColumnBindingVisitorMixin<
            DefaultedTableBindingVisitorMixin<
                DefaultedJoinConstraintVisitorMixin<
                    DefaultedTableExpressionVisitorMixin<
                        DefaultedStatementVisitorMixin<T>>>>>>>> {
  public:
    virtual void
    visit_table_name(const TableName& table_name) override {
        visit_node(table_name);
    }

    virtual void
    visit_distinct_clause(const DistinctClause& clause) override {
        visit_node(clause);
    }

    virtual void
    visit_from_clause(const FromClause& clause) override {
        visit_node(clause);
    }

    virtual void
    visit_join_clause(const JoinClause& clause) override {
        visit_node(clause);
    }

    virtual void
    visit_where_clause(const WhereClause& clause) override {
        visit_node(clause);
    }

    virtual void
    visit_group_by_clause(const GroupByClause& clause) override {
        visit_node(clause);
    }

    virtual void
    visit_script(const Script& script) override {
        visit_node(script);
    }

    virtual void
    visit_column_name(const ColumnName& column_name) override {
        visit_node(column_name);
    }

    virtual void
    visit_literal(const Literal& literal) override {
        visit_node(literal);
    }

    virtual void
    visit_expression(const Expression& expr) override {
        visit_node(expr);
    }

    virtual void
    visit_column_binding(const ColumnBinding& binding) override {
        visit_node(binding);
    }

    virtual void
    visit_table_binding(const TableBinding& binding) override {
        visit_node(binding);
    }

    virtual void
    visit_join_constraint(const JoinConstraint& constraint) override {
        visit_node(constraint);
    }

    virtual void
    visit_table_expression(const TableExpression& expr) override {
        visit_node(expr);
    }

    virtual void
    visit_statement(const Statement& statement) override {
        visit_node(statement);
    }

    virtual void
    visit_node(const Node& node) = 0;
};

} /* namespace ast */
} /* namespace dtl */
