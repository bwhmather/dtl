#pragma once

#include "dtl-ast.hpp"

namespace dtl {
namespace ast {

template <typename T>
class DefaultedLiteralVisitorMixin : public T {
  public:
    virtual void visit_string(String& string) override {
        return visit_literal(string);
    }

    virtual void visit_literal(Literal& literal) = 0;
};

template <typename T>
class DefaultedColumnNameVisitorMixin : public T {
  public:
    virtual void visit_unqualified_column_name(UnqualifiedColumnName& column_name) override {
        return visit_column_name(column_name);
    }
    virtual void visit_qualified_column_name(QualifiedColumnName& column_name) override {
        return visit_column_name(column_name);
    }

    virtual void visit_column_name(ColumnName& column_name) = 0;
};

template <typename T>
class DefaultedExpressionVisitorMixin : public T {
  public:
    virtual void visit_column_reference_expression(ColumnReferenceExpression& expr) override {
        visit_expression(expr);
    }
    virtual void visit_literal_expression(LiteralExpression& expr) override {
        visit_expression(expr);
    }
    virtual void visit_function_call_expression(FunctionCallExpression& expr) override {
        visit_expression(expr);
    }
    virtual void visit_equal_to_expression(EqualToExpression& expr) override {
        visit_expression(expr);
    }
    virtual void visit_less_than_expression(LessThanExpression& expr) override {
        visit_expression(expr);
    }
    virtual void visit_less_than_equal_expression(LessThanEqualExpression& expr) override {
        visit_expression(expr);
    }
    virtual void visit_greater_than_expression(GreaterThanExpression& expr) override {
        visit_expression(expr);
    }
    virtual void visit_greater_than_equal_expression(GreaterThanEqualExpression& expr) override {
        visit_expression(expr);
    }
    virtual void visit_add_expression(AddExpression& expr) override {
        visit_expression(expr);
    }
    virtual void visit_subtract_expression(SubtractExpression& expr) override {
        visit_expression(expr);
    }
    virtual void visit_multiply_expression(MultiplyExpression& expr) override {
        visit_expression(expr);
    }
    virtual void visit_divide_expression(DivideExpression& expr) override {
        visit_expression(expr);
    }

    virtual void visit_expression(Expression& expr) = 0;
};

template <typename T>
class DefaultedColumnBindingVisitorMixin : public T {
  public:
    virtual void visit_wildcard_column_binding(WildcardColumnBinding& binding) override {
        visit_column_binding(binding);
    }
    virtual void visit_implicit_column_binding(ImplicitColumnBinding& binding) override {
        visit_column_binding(binding);
    }
    virtual void visit_aliased_column_binding(AliasedColumnBinding& binding) override {
        visit_column_binding(binding);
    }

    virtual void visit_column_binding(ColumnBinding& binding) = 0;
};

template <typename T>
class DefaultedTableBindingVisitorMixin : public T {
  public:
    virtual void visit_implicit_table_binding(ImplicitTableBinding& binding) override {
        visit_table_binding(binding);
    }
    virtual void visit_aliased_table_binding(AliasedTableBinding& binding) override {
        visit_table_binding(binding);
    }

    virtual void visit_table_binding(TableBinding& binding) = 0;
};

template <typename T>
class DefaultedJoinConstraintVisitorMixin : public T {
  public:
    virtual void visit_join_on_constraint(JoinOnConstraint& constraint) override {
        visit_join_constraint(constraint);
    }
    virtual void visit_join_using_constraint(JoinUsingConstraint& constraint) override {
        visit_join_constraint(constraint);
    }

    virtual void visit_join_constraint(JoinConstraint& constraint) = 0;
};

template <typename T>
class DefaultedTableExpressionVisitorMixin : public T {
  public:
    virtual void visit_select_expression(SelectExpression& expr) override {
        visit_table_expression(expr);
    }
    virtual void visit_import_expression(ImportExpression& expr) override {
        visit_table_expression(expr);
    }
    virtual void visit_table_reference_expression(TableReferenceExpression& expr) override {
        visit_table_expression(expr);
    }

    virtual void visit_table_expression(TableExpression& expr) = 0;
};

template <typename T>
class DefaultedStatementVisitorMixin : public T {
  public:
    virtual void visit_assignment_statement(AssignmentStatement& statement) override {
        visit_statement(statement);
    }
    virtual void visit_update_statement(UpdateStatement& statement) override {
        visit_statement(statement);
    }
    virtual void visit_delete_statement(DeleteStatement& statement) override {
        visit_statement(statement);
    }
    virtual void visit_insert_statement(InsertStatement& statement) override {
        visit_statement(statement);
    }
    virtual void visit_export_statement(ExportStatement& statement) override {
        visit_statement(statement);
    }
    virtual void visit_begin_statement(BeginStatement& statement) override {
        visit_statement(statement);
    }

    virtual void visit_statement(Statement& statement) = 0;
};


template <typename T>
class DefaultedNodeVisitorMixin :
    public DefaultedLiteralVisitorMixin<
        DefaultedColumnNameVisitorMixin<
            DefaultedExpressionVisitorMixin<
                DefaultedColumnBindingVisitorMixin<
                    DefaultedTableBindingVisitorMixin<
                        DefaultedJoinConstraintVisitorMixin<
                            DefaultedTableExpressionVisitorMixin<
                                DefaultedStatementVisitorMixin<
                                    T
                                >
                            >
                        >
                    >
                >
            >
        >
    >
{
  public:
    virtual void visit_table_name(TableName& table_name) override {
        visit_node(table_name);
    }
    virtual void visit_distinct_clause(DistinctClause& clause) override {
        visit_node(clause);
    }
    virtual void visit_from_clause(FromClause& clause) override {
        visit_node(clause);
    }
    virtual void visit_join_clause(JoinClause& clause) override {
        visit_node(clause);
    }
    virtual void visit_where_clause(WhereClause& clause) override {
        visit_node(clause);
    }
    virtual void visit_group_by_clause(GroupByClause& clause) override {
        visit_node(clause);
    }
    virtual void visit_script(Script& script) override {
        visit_node(script);
    }

    virtual void visit_column_name(ColumnName& column_name) override {
        visit_node(column_name);
    }
    virtual void visit_literal(Literal& literal) override {
        visit_node(literal);
    }
    virtual void visit_expression(Expression& expr) override {
        visit_node(expr);
    }
    virtual void visit_column_binding(ColumnBinding& binding) override {
        visit_node(binding);
    }
    virtual void visit_table_binding(TableBinding& binding) override {
        visit_node(binding);
    }
    virtual void visit_join_constraint(JoinConstraint& constraint) override {
        visit_node(constraint);
    }
    virtual void visit_table_expression(TableExpression& expr) override {
        visit_node(expr);
    }
    virtual void visit_statement(Statement& statement) override {
        visit_node(statement);
    }

    virtual void visit_node(Node& node) = 0;
};

} /* namespace ast */
} /* namespace dtl */

