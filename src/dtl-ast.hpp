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

    virtual void accept(NodeVisitor& visitor) const = 0;
};

/* === Literals ============================================================= */

class Literal : public Node {
  public:
    void accept(NodeVisitor& visitor) const override final;
    virtual void accept(LiteralVisitor& visitor) const = 0;
};

class String final : public Literal {
  public:
    std::string value;

    void accept(LiteralVisitor& visitor) const override final;
};

/* === Columns ============================================================== */

class ColumnName : public Node {
  public:
    void accept(NodeVisitor& visitor) const override final;
    virtual void accept(ColumnNameVisitor& visitor) const = 0;
};

struct UnqualifiedColumnNameArgs {
    Location start;
    Location end;
    std::string column_name;
};

class UnqualifiedColumnName final : public ColumnName {
  public:
    std::string column_name;

    void accept(ColumnNameVisitor& visitor) const override final;
};

class QualifiedColumnName final : public ColumnName {
  public:
    std::string table_name;
    std::string column_name;

    void accept(ColumnNameVisitor& visitor) const override final;
};

class Expression : public Node {
  public:
    void accept(NodeVisitor& visitor) const override final;
    virtual void accept(ExpressionVisitor& visitor) const = 0;
};

class ColumnReferenceExpression final : public Expression {
  public:
    std::unique_ptr<const ColumnName> name;

    void accept(ExpressionVisitor& visitor) const override final;
};

class LiteralExpression final : public Expression {
  public:
    std::unique_ptr<const Literal> value;

    void accept(ExpressionVisitor& visitor) const override final;
};

class FunctionCallExpression final : public Expression {
  public:
    std::string name;
    std::vector<std::unique_ptr<const Expression>> arguments;

    void accept(ExpressionVisitor& visitor) const override final;
};

class EqualToExpression final : public Expression {
  public:
    std::unique_ptr<const Expression> left;
    std::unique_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

class LessThanExpression final : public Expression {
  public:
    std::unique_ptr<const Expression> left;
    std::unique_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

class LessThanEqualExpression final : public Expression {
  public:
    std::unique_ptr<const Expression> left;
    std::unique_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

class GreaterThanExpression final : public Expression {
  public:
    std::unique_ptr<const Expression> left;
    std::unique_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

class GreaterThanEqualExpression final : public Expression {
  public:
    std::unique_ptr<const Expression> left;
    std::unique_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

class AddExpression final : public Expression {
  public:
    std::unique_ptr<const Expression> left;
    std::unique_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

class SubtractExpression final : public Expression {
  public:
    std::unique_ptr<const Expression> left;
    std::unique_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

class MultiplyExpression final : public Expression {
  public:
    std::unique_ptr<const Expression> left;
    std::unique_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

class DivideExpression final : public Expression {
  public:
    std::unique_ptr<const Expression> left;
    std::unique_ptr<const Expression> right;

    void accept(ExpressionVisitor& visitor) const override final;
};

/* === Tables =============================================================== */

class TableName final : public Node {
  public:
    std::string table_name;

    void accept(NodeVisitor& visitor) const override final;
};

/* === Distinct ============================================================= */

class DistinctClause final : public Node {
  public:
    bool consecutive;

    void accept(NodeVisitor& visitor) const override final;
};

/* === Column Bindings ====================================================== */

class ColumnBinding : public Node {
  public:
    void accept(NodeVisitor& visitor) const override final;
    virtual void accept(ColumnBindingVisitor& visitor) const = 0;
};

class WildcardColumnBinding final : public ColumnBinding {
  public:
    void accept(ColumnBindingVisitor& visitor) const override final;
};

class ImplicitColumnBinding final : public ColumnBinding {
  public:
    std::unique_ptr<const Expression> expression;

    void accept(ColumnBindingVisitor& visitor) const override final;
};

class AliasedColumnBinding final : public ColumnBinding {
  public:
    std::unique_ptr<const Expression> expression;
    std::string alias;

    void accept(ColumnBindingVisitor& visitor) const override final;
};

/* === From ================================================================= */

class TableBinding : public Node {
  public:
    void accept(NodeVisitor& visitor) const override final;
    virtual void accept(TableBindingVisitor& visitor) const = 0;
};

class ImplicitTableBinding final : public TableBinding {
  public:
    std::unique_ptr<const TableExpression> expression;

    void accept(TableBindingVisitor& visitor) const override final;
};

class AliasedTableBinding final : public TableBinding {
  public:
    std::unique_ptr<const TableExpression> expression;
    std::string alias;

    void accept(TableBindingVisitor& visitor) const override final;
};

class FromClause final : public Node {
  public:
    std::unique_ptr<const TableBinding> binding;

    void accept(NodeVisitor& visitor) const override final;
};

/* === Joins ================================================================ */

class JoinConstraint : public Node {
  public:
    void accept(NodeVisitor& visitor) const override final;
    virtual void accept(JoinConstraintVisitor& visitor) const = 0;
};

class JoinOnConstraint final : public JoinConstraint {
  public:
    std::unique_ptr<const Expression> predicate;

    void accept(JoinConstraintVisitor& visitor) const override final;
};

class JoinUsingConstraint final : public JoinConstraint {
  public:
    std::vector<std::unique_ptr<const UnqualifiedColumnName>> columns;

    void accept(JoinConstraintVisitor& visitor) const override final;
};

class JoinClause final : public Node {
  public:
    std::unique_ptr<const TableBinding> binding;
    std::unique_ptr<const JoinConstraint> constraint;

    void accept(NodeVisitor& visitor) const override final;
};

/* === Filtering ============================================================ */

class WhereClause final : public Node {
  public:
    std::unique_ptr<const Expression> predicate;

    void accept(NodeVisitor& visitor) const override final;
};

/* === Grouping ============================================================= */

class GroupByClause final : public Node {
  public:
    bool consecutive;
    std::vector<std::unique_ptr<const Expression>> pattern;

    void accept(NodeVisitor& visitor) const override final;
};

/* === Table Expressions ==================================================== */

class TableExpression : public Node {
  public:
    void accept(NodeVisitor& visitor) const override final;
    virtual void accept(TableExpressionVisitor& visitor) const = 0;
};

class SelectExpression final : public TableExpression {
  public:
    std::unique_ptr<const DistinctClause> distinct;  /* nullable */
    std::vector<std::unique_ptr<const ColumnBinding>> columns;
    std::unique_ptr<const FromClause> source;
    std::vector<std::unique_ptr<const JoinClause>> joins;
    std::unique_ptr<const WhereClause> where;  /* nullable */
    std::unique_ptr<const GroupByClause> group_by;  /* nullable */

    void accept(TableExpressionVisitor& visitor) const override final;
};

class ImportExpression final : public TableExpression {
  public:
    std::unique_ptr<const String> location;

    void accept(TableExpressionVisitor& visitor) const override final;
};

class TableReferenceExpression final : public TableExpression {
  public:
    std::string name;

    void accept(TableExpressionVisitor& visitor) const override final;
};

/* === Statements =========================================================== */

class Statement : public Node {
  public:
    void accept(NodeVisitor& visitor) const override final;
    virtual void accept(StatementVisitor& visitor) const = 0;
};

class AssignmentStatement final : public Statement {
  public:
    std::unique_ptr<const TableName> target;
    std::unique_ptr<const TableExpression> expression;

    void accept(StatementVisitor& visitor) const override final;
};

class UpdateStatement final : public Statement {
  public:
    /* TODO */
    void accept(StatementVisitor& visitor) const override final;
};

class DeleteStatement final : public Statement {
  public:
    /* TODO */
    void accept(StatementVisitor& visitor) const override final;
};

class InsertStatement final : public Statement {
  public:
    /* TODO */
    void accept(StatementVisitor& visitor) const override final;
};

class ExportStatement final : public Statement {
  public:
    std::unique_ptr<const String> location;
    std::unique_ptr<const TableExpression> expression;

    void accept(StatementVisitor& visitor) const override final;
};

class BeginStatement final : public Statement {
  public:
    std::string text;

    void accept(StatementVisitor& visitor) const override final;
};

/* === Scripts ============================================================== */

class Script final : public Node {
  public:
    std::vector<std::unique_ptr<const Statement>> statements;

    void accept(NodeVisitor& visitor) const override final;
};

/* === Visitors ============================================================= */

class LiteralVisitor {
  public:
    virtual ~LiteralVisitor() {}

    virtual void visit_string(const String& string) = 0;

    void visit(Literal& literal);
};

class ColumnNameVisitor {
  public:
    virtual ~ColumnNameVisitor() {}

    virtual void visit_unqualified_column_name(const UnqualifiedColumnName& column_name) = 0;
    virtual void visit_qualified_column_name(const QualifiedColumnName& column_name) = 0;
};

class ExpressionVisitor {
  public:
    virtual ~ExpressionVisitor() {}

    virtual void visit_column_reference_expression(const ColumnReferenceExpression& expr) = 0;
    virtual void visit_literal_expression(const LiteralExpression& expr) = 0;
    virtual void visit_function_call_expression(const FunctionCallExpression& expr) = 0;
    virtual void visit_equal_to_expression(const EqualToExpression& expr) = 0;
    virtual void visit_less_than_expression(const LessThanExpression& expr) = 0;
    virtual void visit_less_than_equal_expression(const LessThanEqualExpression& expr) = 0;
    virtual void visit_greater_than_expression(const GreaterThanExpression& expr) = 0;
    virtual void visit_greater_than_equal_expression(const GreaterThanEqualExpression& expr) = 0;
    virtual void visit_add_expression(const AddExpression& expr) = 0;
    virtual void visit_subtract_expression(const SubtractExpression& expr) = 0;
    virtual void visit_multiply_expression(const MultiplyExpression& expr) = 0;
    virtual void visit_divide_expression(const DivideExpression& expr) = 0;
};

class ColumnBindingVisitor {
  public:
    virtual ~ColumnBindingVisitor() {}

    virtual void visit_wildcard_column_binding(const WildcardColumnBinding& binding) = 0;
    virtual void visit_implicit_column_binding(const ImplicitColumnBinding& binding) = 0;
    virtual void visit_aliased_column_binding(const AliasedColumnBinding& binding) = 0;
};

class TableBindingVisitor {
  public:
    virtual ~TableBindingVisitor() {}

    virtual void visit_implicit_table_binding(const ImplicitTableBinding& binding) = 0;
    virtual void visit_aliased_table_binding(const AliasedTableBinding& binding) = 0;
};

class JoinConstraintVisitor {
  public:
    virtual ~JoinConstraintVisitor() {}

    virtual void visit_join_on_constraint(const JoinOnConstraint& constraint) = 0;
    virtual void visit_join_using_constraint(const JoinUsingConstraint& constraint) = 0;
};

class TableExpressionVisitor {
  public:
    virtual ~TableExpressionVisitor() {}

    virtual void visit_select_expression(const SelectExpression& expr) = 0;
    virtual void visit_import_expression(const ImportExpression& expr) = 0;
    virtual void visit_table_reference_expression(const TableReferenceExpression& expr) = 0;
};

class StatementVisitor {
  public:
    virtual ~StatementVisitor() {}

    virtual void visit_assignment_statement(const AssignmentStatement& statement) = 0;
    virtual void visit_update_statement(const UpdateStatement& statement) = 0;
    virtual void visit_delete_statement(const DeleteStatement& statement) = 0;
    virtual void visit_insert_statement(const InsertStatement& statement) = 0;
    virtual void visit_export_statement(const ExportStatement& statement) = 0;
    virtual void visit_begin_statement(const BeginStatement& statement) = 0;
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
    virtual void visit_table_name(const TableName& table_name) = 0;
    virtual void visit_distinct_clause(const DistinctClause& clause) = 0;
    virtual void visit_from_clause(const FromClause& clause) = 0;
    virtual void visit_join_clause(const JoinClause& clause) = 0;
    virtual void visit_where_clause(const WhereClause& clause) = 0;
    virtual void visit_group_by_clause(const GroupByClause& clause) = 0;
    virtual void visit_script(const Script& script) = 0;
};

} /* namespace ast */
} /* namespace dtl */

