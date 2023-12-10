#pragma once

#include <memory>
#include <stddef.h>
#include <string>
#include <type_traits>
#include <variant>
#include <vector>

#include "dtl-location.hpp"
#include "dtl-variant.tpp"

namespace dtl {
namespace ast {

class TableExpression;

enum class Type {
    // Literals.
    LITERAL = 0x0100,
    STRING = 1,

    // Columns.
    COLUMN_NAME = 0x0200,
    UNQUALIFIED_COLUMN_NAME,
    QUALIFIED_COLUMN_NAME,

    // Expressions.
    EXPRESSION = 0x0300,
    COLUMN_REFERENCE_EXPRESSION,
    LITERAL_EXPRESSION,
    FUNCTION_CALL_EXPRESSION,
    EQUAL_TO_EXPRESSION,
    LESS_THAN_EXPRESSION,
    LESS_THAN_EQUAL_EXPRESSION,
    GREATER_THAN_EXPRESSION,
    GREATER_THAN_EQUAL_EXPRESSION,
    ADD_EXPRESSION,
    SUBTRACT_EXPRESSION,
    MULTIPLY_EXPRESSION,
    DIVIDE_EXPRESSION,

    // Tables.
    TABLE_NAME = 0x0400,

    // Distinct.
    DISTINCT_CLAUSE = 0x0500,

    // Column Bindings.
    COLUMN_BINDING = 0x0600,
    WILDCARD_COLUMN_BINDING,
    IMPLICIT_COLUMN_BINDING,
    ALIASED_COLUMN_BINDING,

    // From.
    TABLE_BINDING = 0x0700,
    IMPLICIT_TABLE_BINDING,
    ALIASED_TABLE_BINDING,
    FROM_CLAUSE,

    // Joins.
    JOIN_CONSTRAINT = 0x0800,
    JOIN_ON_CONSTRAINT,
    JOIN_USING_CONSTRAINT,
    JOIN_CLAUSE = 0x0900,

    // Filtering.
    WHERE_CLAUSE = 0x0A00,

    // Grouping.
    GROUP_BY_CLAUSE = 0x0B00,
    // Table Expressions.
    TABLE_EXPRESSION = 0x0C00,
    SELECT_EXPRESSION,
    IMPORT_EXPRESSION,
    TABLE_REFERENCE_EXPRESSION,

    // Statements.
    STATEMENT = 0x0D00,
    ASSIGNMENT_STATEMENT,
    UPDATE_STATEMENT,
    DELETE_STATEMENT,
    INSERT_STATEMENT,
    EXPORT_STATEMENT,
    BEGIN_STATEMENT,

    // Scripts.
    SCRIPT = 0x0E000,
};

class Node {
  public:
    Location start;
    Location end;

    virtual Type
    type() const = 0;

    virtual ~Node() {}
};

/* === Literals ============================================================= */

class String final : public Node {
  public:
    Type
    type() const override final;

    std::string value;
};

typedef std::variant<String> Literal;

/* === Columns ============================================================== */

class UnqualifiedColumnName final : public Node {
  public:
    Type
    type() const override final;

    std::string column_name;
};

class QualifiedColumnName final : public Node {
  public:
    Type
    type() const override final;

    std::string table_name;
    std::string column_name;
};

typedef std::variant<
    UnqualifiedColumnName,
    QualifiedColumnName>
    ColumnName;

/* === Expressions ========================================================== */

class ColumnReferenceExpression;
class LiteralExpression;
class FunctionCallExpression;
class EqualToExpression;
class LessThanExpression;
class LessThanEqualExpression;
class GreaterThanExpression;
class GreaterThanEqualExpression;
class AddExpression;
class SubtractExpression;
class MultiplyExpression;
class DivideExpression;

typedef std::variant<
    ColumnReferenceExpression,
    LiteralExpression,
    FunctionCallExpression,
    EqualToExpression,
    LessThanExpression,
    LessThanEqualExpression,
    GreaterThanExpression,
    GreaterThanEqualExpression,
    AddExpression,
    SubtractExpression,
    MultiplyExpression,
    DivideExpression>
    Expression;

class ColumnReferenceExpression final : public Node {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const ColumnName> name;
};

class LiteralExpression final : public Node {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const Literal> value;
};

class FunctionCallExpression final : public Node {
  public:
    Type
    type() const override final;

    std::string name;
    std::vector<dtl::unique_variant_ptr_t<const Expression>> arguments;
};

class EqualToExpression final : public Node {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class LessThanExpression final : public Node {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class LessThanEqualExpression final : public Node {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class GreaterThanExpression final : public Node {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class GreaterThanEqualExpression final : public Node {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class AddExpression final : public Node {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class SubtractExpression final : public Node {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class MultiplyExpression final : public Node {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class DivideExpression final : public Node {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

/* === Tables =============================================================== */

class TableName final : public Node {
  public:
    Type
    type() const override final;

    std::string table_name;
};

/* === Distinct ============================================================= */

class DistinctClause final : public Node {
  public:
    Type
    type() const override final;

    bool consecutive;
};

/* === Column Bindings ====================================================== */

class WildcardColumnBinding final : public Node {
  public:
    Type
    type() const override final;
};

class ImplicitColumnBinding final : public Node {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const Expression> expression;
};

class AliasedColumnBinding final : public Node {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const Expression> expression;
    std::string alias;
};

typedef std::variant<
    WildcardColumnBinding,
    ImplicitColumnBinding,
    AliasedColumnBinding>
    ColumnBinding;

/* === From ================================================================= */

class TableBinding : public Node {};

class ImplicitTableBinding final : public TableBinding {
  public:
    Type
    type() const override final;

    std::unique_ptr<const TableExpression> expression;
};

class AliasedTableBinding final : public TableBinding {
  public:
    Type
    type() const override final;

    std::unique_ptr<const TableExpression> expression;
    std::string alias;
};

class FromClause final : public Node {
  public:
    Type
    type() const override final;

    std::unique_ptr<const TableBinding> binding;
};

/* === Joins ================================================================ */

class JoinConstraint : public Node {};

class JoinOnConstraint final : public JoinConstraint {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const Expression> predicate;
};

class JoinUsingConstraint final : public JoinConstraint {
  public:
    Type
    type() const override final;

    std::vector<std::unique_ptr<const UnqualifiedColumnName>> columns;
};

class JoinClause final : public Node {
  public:
    Type
    type() const override final;

    std::unique_ptr<const TableBinding> binding;
    std::unique_ptr<const JoinConstraint> constraint;
};

/* === Filtering ============================================================ */

class WhereClause final : public Node {
  public:
    Type
    type() const override final;

    dtl::unique_variant_ptr_t<const Expression> predicate;
};

/* === Grouping ============================================================= */

class GroupByClause final : public Node {
  public:
    Type
    type() const override final;

    bool consecutive;
    std::vector<dtl::unique_variant_ptr_t<const Expression>> pattern;
};

/* === Table Expressions ==================================================== */

class TableExpression : public Node {};

class SelectExpression final : public TableExpression {
  public:
    Type
    type() const override final;

    std::unique_ptr<const DistinctClause> distinct; /* nullable */
    std::vector<dtl::unique_variant_ptr_t<const ColumnBinding>> columns;
    std::unique_ptr<const FromClause> source;
    std::vector<std::unique_ptr<const JoinClause>> joins;
    std::unique_ptr<const WhereClause> where;      /* nullable */
    std::unique_ptr<const GroupByClause> group_by; /* nullable */
};

class ImportExpression final : public TableExpression {
  public:
    Type
    type() const override final;

    std::unique_ptr<const String> location;
};

class TableReferenceExpression final : public TableExpression {
  public:
    Type
    type() const override final;

    std::string name;
};

/* === Statements =========================================================== */

class Statement : public Node {};

class AssignmentStatement final : public Statement {
  public:
    Type
    type() const override final;

    std::unique_ptr<const TableName> target;
    std::unique_ptr<const TableExpression> expression;
};

class UpdateStatement final : public Statement {
  public:
    Type
    type() const override final;

    /* TODO */
};

class DeleteStatement final : public Statement {
  public:
    Type
    type() const override final;

    /* TODO */
};

class InsertStatement final : public Statement {
  public:
    Type
    type() const override final;

    /* TODO */
};

class ExportStatement final : public Statement {
  public:
    Type
    type() const override final;

    std::unique_ptr<const String> location;
    std::unique_ptr<const TableExpression> expression;
};

class BeginStatement final : public Statement {
  public:
    Type
    type() const override final;

    std::string text;
};

/* === Scripts ============================================================== */

class Script final : public Node {
  public:
    Type
    type() const override final;

    std::vector<std::unique_ptr<const Statement>> statements;
};

} /* namespace ast */
} /* namespace dtl */
