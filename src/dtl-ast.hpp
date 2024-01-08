#pragma once

#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "dtl-location.hpp"
#include "dtl-variant.tpp"

namespace dtl {
namespace ast {

struct Node {
    Location start;
    Location end;
};

struct String;
typedef std::variant<String> Literal;

struct UnqualifiedColumnName;
struct QualifiedColumnName;
typedef std::variant<
    UnqualifiedColumnName,
    QualifiedColumnName>
    ColumnName;

struct ColumnReferenceExpression;
struct LiteralExpression;
struct FunctionCallExpression;
struct EqualToExpression;
struct LessThanExpression;
struct LessThanEqualExpression;
struct GreaterThanExpression;
struct GreaterThanEqualExpression;
struct AddExpression;
struct SubtractExpression;
struct MultiplyExpression;
struct DivideExpression;
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

struct TableName;

struct DistinctClause;

struct WildcardColumnBinding;
struct ImplicitColumnBinding;
struct AliasedColumnBinding;
typedef std::variant<
    WildcardColumnBinding,
    ImplicitColumnBinding,
    AliasedColumnBinding>
    ColumnBinding;

struct ImplicitTableBinding;
struct AliasedTableBinding;
typedef std::variant<ImplicitTableBinding, AliasedTableBinding> TableBinding;

struct FromClause;

struct JoinOnConstraint;
struct JoinUsingConstraint;
typedef std::variant<JoinOnConstraint, JoinUsingConstraint> JoinConstraint;

struct JoinClause;

struct WhereClause;

struct GroupByClause;

struct SelectExpression;
struct ImportExpression;
struct TableReferenceExpression;
typedef std::variant<
    SelectExpression,
    ImportExpression,
    TableReferenceExpression>
    TableExpression;

struct AssignmentStatement;
struct UpdateStatement;
struct DeleteStatement;
struct InsertStatement;
struct ExportStatement;
struct BeginStatement;

struct Script;

/* === Literals ============================================================= */

struct String final : public Node {
    std::string value;
};

/* === Columns ============================================================== */

struct UnqualifiedColumnName final : public Node {
    std::string column_name;
};

struct QualifiedColumnName final : public Node {
    std::string table_name;
    std::string column_name;
};

/* === Expressions ========================================================== */

struct ColumnReferenceExpression final : public Node {
    dtl::unique_variant_ptr<const ColumnName> name;
};

struct LiteralExpression final : public Node {
    dtl::unique_variant_ptr<const Literal> value;
};

struct FunctionCallExpression final : public Node {
    std::string name;
    std::vector<dtl::unique_variant_ptr<const Expression>> arguments;
};

struct EqualToExpression final : public Node {
    dtl::unique_variant_ptr<const Expression> left;
    dtl::unique_variant_ptr<const Expression> right;
};

struct LessThanExpression final : public Node {
    dtl::unique_variant_ptr<const Expression> left;
    dtl::unique_variant_ptr<const Expression> right;
};

struct LessThanEqualExpression final : public Node {
    dtl::unique_variant_ptr<const Expression> left;
    dtl::unique_variant_ptr<const Expression> right;
};

struct GreaterThanExpression final : public Node {
    dtl::unique_variant_ptr<const Expression> left;
    dtl::unique_variant_ptr<const Expression> right;
};

struct GreaterThanEqualExpression final : public Node {
    dtl::unique_variant_ptr<const Expression> left;
    dtl::unique_variant_ptr<const Expression> right;
};

struct AddExpression final : public Node {
    dtl::unique_variant_ptr<const Expression> left;
    dtl::unique_variant_ptr<const Expression> right;
};

struct SubtractExpression final : public Node {
    dtl::unique_variant_ptr<const Expression> left;
    dtl::unique_variant_ptr<const Expression> right;
};

struct MultiplyExpression final : public Node {
    dtl::unique_variant_ptr<const Expression> left;
    dtl::unique_variant_ptr<const Expression> right;
};

struct DivideExpression final : public Node {
    dtl::unique_variant_ptr<const Expression> left;
    dtl::unique_variant_ptr<const Expression> right;
};

/* === Tables =============================================================== */

struct TableName final : public Node {
    std::string table_name;
};

/* === Distinct ============================================================= */

struct DistinctClause final : public Node {
    bool consecutive;
};

/* === Column Bindings ====================================================== */

struct WildcardColumnBinding final : public Node {
};

struct ImplicitColumnBinding final : public Node {
    dtl::unique_variant_ptr<const Expression> expression;
};

struct AliasedColumnBinding final : public Node {
    dtl::unique_variant_ptr<const Expression> expression;
    std::string alias;
};

/* === From ================================================================= */

struct ImplicitTableBinding final : public Node {
    dtl::unique_variant_ptr<const TableExpression> expression;
};

struct AliasedTableBinding final : public Node {
    dtl::unique_variant_ptr<const TableExpression> expression;
    std::string alias;
};

struct FromClause final : public Node {
    dtl::unique_variant_ptr<const TableBinding> binding;
};

/* === Joins ================================================================ */

struct JoinOnConstraint final : public Node {
    dtl::unique_variant_ptr<const Expression> predicate;
};

struct JoinUsingConstraint final : public Node {
    std::vector<std::unique_ptr<const UnqualifiedColumnName>> columns;
};

struct JoinClause final : public Node {
    dtl::unique_variant_ptr<const TableBinding> binding;
    dtl::unique_variant_ptr<const JoinConstraint> constraint;
};

/* === Filtering ============================================================ */

struct WhereClause final : public Node {
    dtl::unique_variant_ptr<const Expression> predicate;
};

/* === Grouping ============================================================= */

struct GroupByClause final : public Node {
    bool consecutive;
    std::vector<dtl::unique_variant_ptr<const Expression>> pattern;
};

/* === Table Expressions ==================================================== */

struct SelectExpression final : public Node {
    std::unique_ptr<const DistinctClause> distinct; /* nullable */
    std::vector<dtl::unique_variant_ptr<const ColumnBinding>> columns;
    std::unique_ptr<const FromClause> source;
    std::vector<std::unique_ptr<const JoinClause>> joins;
    std::unique_ptr<const WhereClause> where;      /* nullable */
    std::unique_ptr<const GroupByClause> group_by; /* nullable */
};

struct ImportExpression final : public Node {
    std::unique_ptr<const String> location;
};

struct TableReferenceExpression final : public Node {
    std::string name;
};

/* === Statements =========================================================== */

struct AssignmentStatement final : public Node {
    std::unique_ptr<const TableName> target;
    dtl::unique_variant_ptr<const TableExpression> expression;
};

struct UpdateStatement final : public Node {
    /* TODO */
};

struct DeleteStatement final : public Node {
    /* TODO */
};

struct InsertStatement final : public Node {
    /* TODO */
};

struct ExportStatement final : public Node {
    std::unique_ptr<const String> location;
    dtl::unique_variant_ptr<const TableExpression> expression;
};

struct BeginStatement final : public Node {
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

struct Script final : public Node {
    std::vector<dtl::unique_variant_ptr<const Statement>> statements;
};

} /* namespace ast */
} /* namespace dtl */
