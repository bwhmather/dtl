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

class Node {
  public:
    Location start;
    Location end;

    virtual ~Node() {}
};

class String;
typedef std::variant<String> Literal;

class UnqualifiedColumnName;
class QualifiedColumnName;
typedef std::variant<
    UnqualifiedColumnName,
    QualifiedColumnName>
    ColumnName;

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

class TableName;

class DistinctClause;

class WildcardColumnBinding;
class ImplicitColumnBinding;
class AliasedColumnBinding;
typedef std::variant<
    WildcardColumnBinding,
    ImplicitColumnBinding,
    AliasedColumnBinding>
    ColumnBinding;

class ImplicitTableBinding;
class AliasedTableBinding;
typedef std::variant<ImplicitTableBinding, AliasedTableBinding> TableBinding;

class FromClause;

class JoinOnConstraint;
class JoinUsingConstraint;
typedef std::variant<JoinOnConstraint, JoinUsingConstraint> JoinConstraint;

class JoinClause;

class WhereClause;

class GroupByClause;

class SelectExpression;
class ImportExpression;
class TableReferenceExpression;
typedef std::variant<
    SelectExpression,
    ImportExpression,
    TableReferenceExpression>
    TableExpression;

class AssignmentStatement;
class UpdateStatement;
class DeleteStatement;
class InsertStatement;
class ExportStatement;
class BeginStatement;

class Script;

/* === Literals ============================================================= */

class String final : public Node {
  public:
    std::string value;
};

/* === Columns ============================================================== */

class UnqualifiedColumnName final : public Node {
  public:
    std::string column_name;
};

class QualifiedColumnName final : public Node {
  public:
    std::string table_name;
    std::string column_name;
};

/* === Expressions ========================================================== */

class ColumnReferenceExpression final : public Node {
  public:
    dtl::unique_variant_ptr_t<const ColumnName> name;
};

class LiteralExpression final : public Node {
  public:
    dtl::unique_variant_ptr_t<const Literal> value;
};

class FunctionCallExpression final : public Node {
  public:
    std::string name;
    std::vector<dtl::unique_variant_ptr_t<const Expression>> arguments;
};

class EqualToExpression final : public Node {
  public:
    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class LessThanExpression final : public Node {
  public:
    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class LessThanEqualExpression final : public Node {
  public:
    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class GreaterThanExpression final : public Node {
  public:
    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class GreaterThanEqualExpression final : public Node {
  public:
    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class AddExpression final : public Node {
  public:
    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class SubtractExpression final : public Node {
  public:
    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class MultiplyExpression final : public Node {
  public:
    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

class DivideExpression final : public Node {
  public:
    dtl::unique_variant_ptr_t<const Expression> left;
    dtl::unique_variant_ptr_t<const Expression> right;
};

/* === Tables =============================================================== */

class TableName final : public Node {
  public:
    std::string table_name;
};

/* === Distinct ============================================================= */

class DistinctClause final : public Node {
  public:
    bool consecutive;
};

/* === Column Bindings ====================================================== */

class WildcardColumnBinding final : public Node {
  public:
};

class ImplicitColumnBinding final : public Node {
  public:
    dtl::unique_variant_ptr_t<const Expression> expression;
};

class AliasedColumnBinding final : public Node {
  public:
    dtl::unique_variant_ptr_t<const Expression> expression;
    std::string alias;
};

/* === From ================================================================= */

class ImplicitTableBinding final : public Node {
  public:
    dtl::unique_variant_ptr_t<const TableExpression> expression;
};

class AliasedTableBinding final : public Node {
  public:
    dtl::unique_variant_ptr_t<const TableExpression> expression;
    std::string alias;
};

class FromClause final : public Node {
  public:
    dtl::unique_variant_ptr_t<const TableBinding> binding;
};

/* === Joins ================================================================ */

class JoinOnConstraint final : public Node {
  public:
    dtl::unique_variant_ptr_t<const Expression> predicate;
};

class JoinUsingConstraint final : public Node {
  public:
    std::vector<std::unique_ptr<const UnqualifiedColumnName>> columns;
};

class JoinClause final : public Node {
  public:
    dtl::unique_variant_ptr_t<const TableBinding> binding;
    dtl::unique_variant_ptr_t<const JoinConstraint> constraint;
};

/* === Filtering ============================================================ */

class WhereClause final : public Node {
  public:
    dtl::unique_variant_ptr_t<const Expression> predicate;
};

/* === Grouping ============================================================= */

class GroupByClause final : public Node {
  public:
    bool consecutive;
    std::vector<dtl::unique_variant_ptr_t<const Expression>> pattern;
};

/* === Table Expressions ==================================================== */

class SelectExpression final : public Node {
  public:
    std::unique_ptr<const DistinctClause> distinct; /* nullable */
    std::vector<dtl::unique_variant_ptr_t<const ColumnBinding>> columns;
    std::unique_ptr<const FromClause> source;
    std::vector<std::unique_ptr<const JoinClause>> joins;
    std::unique_ptr<const WhereClause> where;      /* nullable */
    std::unique_ptr<const GroupByClause> group_by; /* nullable */
};

class ImportExpression final : public Node {
  public:
    std::unique_ptr<const String> location;
};

class TableReferenceExpression final : public Node {
  public:
    std::string name;
};

/* === Statements =========================================================== */

class AssignmentStatement final : public Node {
  public:
    std::unique_ptr<const TableName> target;
    dtl::unique_variant_ptr_t<const TableExpression> expression;
};

class UpdateStatement final : public Node {
  public:
    /* TODO */
};

class DeleteStatement final : public Node {
  public:
    /* TODO */
};

class InsertStatement final : public Node {
  public:
    /* TODO */
};

class ExportStatement final : public Node {
  public:
    std::unique_ptr<const String> location;
    dtl::unique_variant_ptr_t<const TableExpression> expression;
};

class BeginStatement final : public Node {
  public:
    std::string text;
};

typedef std::variant<
    AssignmentStatement,
    UpdateStatement,
    DeleteStatement,
    InsertStatement,
    ExportStatement,
    BeginStatement>
    Statement;

/* === Scripts ============================================================== */

class Script final : public Node {
  public:
    std::vector<dtl::unique_variant_ptr_t<const Statement>> statements;
};

} /* namespace ast */
} /* namespace dtl */
