#pragma once

#include <stddef.h>
#include <vector>
#include <string>
#include <memory>

#include "dtl-location.hpp"

namespace dtl {
namespace ast {

class LiteralVisitor;
class ColumnNameVisitor;
class ExpressionVisitor;
class ColumnBindingVisitor;
class TableBindingVisitor;
class JoinConstraintVisitor;
class TableExpressionVisitor;
class StatementVisitor;
class NodeVisitor;

class TableExpression;

class Node {
  public:
    Location start;
    Location end;

    virtual ~Node() {}

  private:
    friend class NodeVisitor;
    virtual void accept(NodeVisitor& visitor) = 0;
};

/* === Literals ============================================================= */

class Literal : public Node {
  private:
    void accept(NodeVisitor& visitor) override final;

    friend class LiteralVisitor;
    virtual void accept(LiteralVisitor& visitor) = 0;
};

class String final : public Literal {
  public:
    std::string value;

  private:
    void accept(LiteralVisitor& visitor) override final;
};

/* === Columns ============================================================== */

class ColumnName : public Node {
  private:
    void accept(NodeVisitor& visitor) override final;

    friend class ColumnNameVisitor;
    virtual void accept(ColumnNameVisitor& visitor) = 0;
};

class UnqualifiedColumnName final : public ColumnName {
  public:
    std::string column_name;

  private:
    void accept(ColumnNameVisitor& visitor) override final;
};

class QualifiedColumnName final : public ColumnName {
  public:
    std::string table_name;
    std::string column_name;

  private:
    void accept(ColumnNameVisitor& visitor) override final;
};

class Expression : public Node {
  private:
    void accept(NodeVisitor& visitor) override final;

    friend class ExpressionVisitor;
    virtual void accept(ExpressionVisitor& visitor) = 0;
};

class ColumnReferenceExpression final : public Expression {
  public:
    std::unique_ptr<ColumnName> name;

  private:
    void accept(ExpressionVisitor& visitor) override final;
};

class LiteralExpression final : public Expression {
  public:
    std::unique_ptr<Literal> value;

  private:
    void accept(ExpressionVisitor& visitor) override final;
};

class FunctionCallExpression final : public Expression {
  public:
    std::string name;
    std::vector<std::unique_ptr<Expression>> arguments;

  private:
    void accept(ExpressionVisitor& visitor) override final;
};

class AddExpression final : public Expression {
  public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

  private:
    void accept(ExpressionVisitor& visitor) override final;
};

class SubtractExpression final : public Expression {
  public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

  private:
    void accept(ExpressionVisitor& visitor) override final;
};

class MultiplyExpression final : public Expression {
  public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

  private:
    void accept(ExpressionVisitor& visitor) override final;
};

class DivideExpression final : public Expression {
  public:
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;

  private:
    void accept(ExpressionVisitor& visitor) override final;
};

/* === Tables =============================================================== */

class TableName final : public Node {
  public:
    std::string table_name;

  private:
    void accept(NodeVisitor& visitor) override final;
};

/* === Distinct ============================================================= */

class DistinctClause final : public Node {
  public:
    bool consecutive;

  private:
    void accept(NodeVisitor& visitor) override final;
};

/* === Column Bindings ====================================================== */

class ColumnBinding : public Node {
  private:
    void accept(NodeVisitor& visitor) override final;

    friend class ColumnBindingVisitor;
    virtual void accept(ColumnBindingVisitor& visitor) = 0;
};

class WildcardColumnBinding final : public ColumnBinding {
  private:
    void accept(ColumnBindingVisitor& visitor) override final;
};

class ImplicitColumnBinding final : public ColumnBinding {
  public:
    std::unique_ptr<Expression> expression;

  private:
    void accept(ColumnBindingVisitor& visitor) override final;
};

class AliasedColumnBinding final : public ColumnBinding {
  public:
    std::unique_ptr<Expression> expression;
    std::string alias;

  private:
    void accept(ColumnBindingVisitor& visitor) override final;
};

/* === From ================================================================= */

class TableBinding : public Node {
  private:
    void accept(NodeVisitor& visitor) override final;

    friend class TableBindingVisitor;
    virtual void accept(TableBindingVisitor& visitor) = 0;
};

class ImplicitTableBinding final : public TableBinding {
  public:
    std::unique_ptr<TableExpression> expression;

  private:
    void accept(TableBindingVisitor& visitor) override final;
};

class AliasedTableBinding final : public TableBinding {
  public:
    std::unique_ptr<TableExpression> expression;
    std::string alias;

  private:
    void accept(TableBindingVisitor& visitor) override final;
};

class FromClause final : public Node {
  public:
    std::unique_ptr<TableBinding> binding;

  private:
    void accept(NodeVisitor& visitor) override final;
};

/* === Joins ================================================================ */

class JoinConstraint : public Node {
  private:
    void accept(NodeVisitor& visitor) override final;

    friend class JoinConstraintVisitor;
    virtual void accept(JoinConstraintVisitor& visitor) = 0;
};

class JoinOnConstraint final : public JoinConstraint {
  public:
    std::unique_ptr<Expression> predicate;

  private:
    void accept(JoinConstraintVisitor& visitor) override final;
};

class JoinUsingConstraint final : public JoinConstraint {
  public:
    std::vector<std::unique_ptr<UnqualifiedColumnName>> columns;

  private:
    void accept(JoinConstraintVisitor& visitor) override final;
};

class JoinClause final : public Node {
  public:
    std::unique_ptr<TableBinding> binding;
    std::unique_ptr<JoinConstraint> constraint;

  private:
    void accept(NodeVisitor& visitor) override final;
};

/* === Filtering ============================================================ */

class WhereClause final : public Node {
  public:
    std::unique_ptr<Expression> predicate;

  private:
    void accept(NodeVisitor& visitor) override final;
};

/* === Grouping ============================================================= */

class GroupByClause final : public Node {
  public:
    bool consecutive;
    std::vector<std::unique_ptr<Expression>> pattern;

  private:
    void accept(NodeVisitor& visitor) override final;
};

/* === Table Expressions ==================================================== */

class TableExpression : public Node {
  private:
    void accept(NodeVisitor& visitor) override final;

    friend class TableExpressionVisitor;
    virtual void accept(TableExpressionVisitor& visitor) = 0;
};

class SelectExpression final : public TableExpression {
  public:
    std::unique_ptr<DistinctClause> distinct;  /* nullable */
    std::vector<std::unique_ptr<ColumnBinding>> columns;
    std::unique_ptr<FromClause> source;
    std::vector<std::unique_ptr<JoinClause>> joins;
    std::unique_ptr<WhereClause> where;  /* nullable */
    std::unique_ptr<GroupByClause> group_by;  /* nullable */

  private:
    void accept(TableExpressionVisitor& visitor) override final;
};

class ImportExpression final : public TableExpression {
  public:
    std::unique_ptr<String> location;

  private:
    void accept(TableExpressionVisitor& visitor) override final;
};

class TableReferenceExpression final : public TableExpression {
  public:
    std::string name;

  private:
    void accept(TableExpressionVisitor& visitor) override final;
};

/* === Statements =========================================================== */

class Statement : public Node {
  private:
    void accept(NodeVisitor& visitor) override final;

    friend class StatementVisitor;
    virtual void accept(StatementVisitor& visitor) = 0;
};

class AssignmentStatement final : public Statement {
  public:
    std::unique_ptr<TableName> target;
    std::unique_ptr<TableExpression> expression;

  private:
    void accept(StatementVisitor& visitor) override final;
};

class UpdateStatement final : public Statement {
  private:
    /* TODO */
    void accept(StatementVisitor& visitor) override final;
};

class DeleteStatement final : public Statement {
  private:
    /* TODO */
    void accept(StatementVisitor& visitor) override final;
};

class InsertStatement final : public Statement {
  private:
    /* TODO */
    void accept(StatementVisitor& visitor) override final;
};

class ExportStatement final : public Statement {
  public:
    std::unique_ptr<String> location;
    std::unique_ptr<TableExpression> expression;

  private:
    void accept(StatementVisitor& visitor) override final;
};

/* === Scripts ============================================================== */

class Script final : public Node {
  public:
    std::vector<std::unique_ptr<Statement>> statements;

  private:
    void accept(NodeVisitor& visitor) override final;
};

/* === Visitors ============================================================= */

class LiteralVisitor {
  public:
    virtual ~LiteralVisitor() {}

    virtual void visit_string(String& string) = 0;

    void visit(Literal& literal);
};

class ColumnNameVisitor {
  public:
    virtual ~ColumnNameVisitor() {}

    virtual void visit_unqualified_column_name(UnqualifiedColumnName& column_name) = 0;
    virtual void visit_qualified_column_name(QualifiedColumnName& column_name) = 0;

    void visit(ColumnName& column_name);
};

class ExpressionVisitor {
  public:
    virtual ~ExpressionVisitor() {}

    virtual void visit_column_reference_expression(ColumnReferenceExpression& expr) = 0;
    virtual void visit_literal_expression(LiteralExpression& expr) = 0;
    virtual void visit_function_call_expression(FunctionCallExpression& expr) = 0;
    virtual void visit_add_expression(AddExpression& expr) = 0;
    virtual void visit_subtract_expression(SubtractExpression& expr) = 0;
    virtual void visit_multiply_expression(MultiplyExpression& expr) = 0;
    virtual void visit_divide_expression(DivideExpression& expr) = 0;

    void visit(Expression& expression);
};

class ColumnBindingVisitor {
  public:
    virtual ~ColumnBindingVisitor() {}

    virtual void visit_wildcard_column_binding(WildcardColumnBinding& binding) = 0;
    virtual void visit_implicit_column_binding(ImplicitColumnBinding& binding) = 0;
    virtual void visit_aliased_column_binding(AliasedColumnBinding& binding) = 0;

    void visit(ColumnBinding& binding);
};

class TableBindingVisitor {
  public:
    virtual ~TableBindingVisitor() {}

    virtual void visit_implicit_table_binding(ImplicitTableBinding& binding) = 0;
    virtual void visit_aliased_table_binding(AliasedTableBinding& binding) = 0;

    void visit(TableBinding& binding);
};

class JoinConstraintVisitor {
  public:
    virtual ~JoinConstraintVisitor() {}

    virtual void visit_join_on_constraint(JoinOnConstraint& constraint) = 0;
    virtual void visit_join_using_constraint(JoinUsingConstraint& constraint) = 0;

    void visit(JoinConstraint& constraint);
};

class TableExpressionVisitor {
  public:
    virtual ~TableExpressionVisitor() {}

    virtual void visit_select_expression(SelectExpression& expr) = 0;
    virtual void visit_import_expression(ImportExpression& expr) = 0;
    virtual void visit_table_reference_expression(TableReferenceExpression& expr) = 0;

    void visit(TableExpression& expr);
};

class StatementVisitor {
  public:
    virtual ~StatementVisitor() {}

    virtual void visit_assignment_statement(AssignmentStatement& statement) = 0;
    virtual void visit_update_statement(UpdateStatement& statement) = 0;
    virtual void visit_delete_statement(DeleteStatement& statement) = 0;
    virtual void visit_insert_statement(InsertStatement& statement) = 0;
    virtual void visit_export_statement(ExportStatement& statement) = 0;

    void visit(Statement& statement);
};

class NodeVisitor :
    public ColumnNameVisitor,
    public LiteralVisitor,
    public ExpressionVisitor,
    public ColumnBindingVisitor,
    public TableBindingVisitor,
    public JoinConstraintVisitor,
    public TableExpressionVisitor,
    public StatementVisitor
{
  public:
    virtual void visit_table_name(TableName& table_name) = 0;
    virtual void visit_distinct_clause(DistinctClause& clause) = 0;
    virtual void visit_from_clause(FromClause& clause) = 0;
    virtual void visit_join_clause(JoinClause& clause) = 0;
    virtual void visit_where_clause(WhereClause& clause) = 0;
    virtual void visit_group_by_clause(GroupByClause& clause) = 0;
    virtual void visit_script(Script& script) = 0;

    using ColumnNameVisitor::visit;
    using LiteralVisitor::visit;
    using ExpressionVisitor::visit;
    using ColumnBindingVisitor::visit;
    using TableBindingVisitor::visit;
    using JoinConstraintVisitor::visit;
    using TableExpressionVisitor::visit;
    using StatementVisitor::visit;
    void visit(Node& node);
};

} /* namespace ast */
} /* namespace dtl */

