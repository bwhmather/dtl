#pragma once

#include "dtl-ast.hpp"

namespace dtl {
namespace ast {

class DefaultedLiteralVisitorMixin : public LiteralVisitor {
  public:
    virtual void visit_string(String& string) override;

    virtual void visit_literal(Literal& literal) = 0;
};

class DefaultedColumnNameVisitorMixin : public ColumnNameVisitor{
  public:
    virtual void visit_unqualified_column_name(UnqualifiedColumnName& column_name) override;
    virtual void visit_qualified_column_name(QualifiedColumnName& column_name) override;

    virtual void visit_column_name(ColumnName& column_name) = 0;
};

class DefaultedExpressionVisitorMixin : public ExpressionVisitor {
  public:
    virtual void visit_column_reference_expression(ColumnReferenceExpression& expr) override;
    virtual void visit_literal_expression(LiteralExpression& expr) override;
    virtual void visit_function_call_expression(FunctionCallExpression& expr) override;
    virtual void visit_add_expression(AddExpression& expr) override;
    virtual void visit_subtract_expression(SubtractExpression& expr) override;
    virtual void visit_multiply_expression(MultiplyExpression& expr) override;
    virtual void visit_divide_expression(DivideExpression& expr) override;

    virtual void visit_expression(Expression& expr) = 0;
};

class DefaultedColumnBindingVisitorMixin : public ColumnBindingVisitor {
  public:
    virtual void visit_wildcard_column_binding(WildcardColumnBinding& binding) override;
    virtual void visit_implicit_column_binding(ImplicitColumnBinding& binding) override;
    virtual void visit_aliased_column_binding(AliasedColumnBinding& binding) override;

    virtual void visit_column_binding(ColumnBinding& binding) = 0;
};

class DefaultedTableBindingVisitorMixin : public TableBindingVisitor {
  public:
    virtual void visit_implicit_table_binding(ImplicitTableBinding& binding) override;
    virtual void visit_aliased_table_binding(AliasedTableBinding& binding) override;

    virtual void visit_table_binding(TableBinding& binding) = 0;
};

class DefaultedJoinConstraintVisitorMixin : public JoinConstraintVisitor {
  public:
    virtual void visit_join_on_constraint(JoinOnConstraint& constraint) override;
    virtual void visit_join_using_constraint(JoinUsingConstraint& constraint) override;

    virtual void visit_join_constraint(JoinConstraint& constraint) = 0;
};

class DefaultedTableExpressionVisitorMixin : public TableExpressionVisitor {
  public:
    virtual void visit_select_expression(SelectExpression& expr) override;
    virtual void visit_import_expression(ImportExpression& expr) override;
    virtual void visit_table_reference_expression(TableReferenceExpression& expr) override;

    virtual void visit_table_expression(TableExpression& expr) = 0;
};

class DefaultedStatementVisitorMixin : public StatementVisitor {
  public:
    virtual void visit_assignment_statement(AssignmentStatement& statement) override;
    virtual void visit_update_statement(UpdateStatement& statement) override;
    virtual void visit_delete_statement(DeleteStatement& statement) override;
    virtual void visit_insert_statement(InsertStatement& statement) override;
    virtual void visit_export_statement(ExportStatement& statement) override;

    virtual void visit_statement(Statement& statement) = 0;
};

class DefaultedNodeVisitorMixin :
    public NodeVisitor,
    public DefaultedColumnNameVisitorMixin,
    public DefaultedLiteralVisitorMixin,
    public DefaultedExpressionVisitorMixin,
    public DefaultedColumnBindingVisitorMixin,
    public DefaultedTableBindingVisitorMixin,
    public DefaultedJoinConstraintVisitorMixin,
    public DefaultedTableExpressionVisitorMixin,
    public DefaultedStatementVisitorMixin
{
  public:
    virtual void visit_table_name(TableName& table_name) override;
    virtual void visit_distinct_clause(DistinctClause& clause) override;
    virtual void visit_from_clause(FromClause& clause) override;
    virtual void visit_join_clause(JoinClause& clause) override;
    virtual void visit_where_clause(WhereClause& clause) override;
    virtual void visit_group_by_clause(GroupByClause& clause) override;
    virtual void visit_script(Script& script) override;

    virtual void visit_column_name(ColumnName& column_name) override;
    virtual void visit_literal(Literal& literal) override;
    virtual void visit_expression(Expression& expr) override;
    virtual void visit_column_binding(ColumnBinding& binding) override;
    virtual void visit_table_binding(TableBinding& binding) override;
    virtual void visit_join_constraint(JoinConstraint& constraint) override;
    virtual void visit_table_expression(TableExpression& expr) override;
    virtual void visit_statement(Statement& statement) override;

    virtual void visit_node(Node& node) = 0;
};

} /* namespace ast */
} /* namespace dtl */

