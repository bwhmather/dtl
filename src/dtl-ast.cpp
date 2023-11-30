#include "dtl-ast.hpp"

#include <memory>
#include <stddef.h>
#include <string>
#include <vector>

namespace dtl {
namespace ast {

/* === Literals ============================================================= */

Type
String::type() const {
    return Type::STRING;
}

/* === Columns ============================================================== */

Type
UnqualifiedColumnName::type() const {
    return Type::UNQUALIFIED_COLUMN_NAME;
}

Type
QualifiedColumnName::type() const {
    return Type::QUALIFIED_COLUMN_NAME;
}

/* === Expressions ========================================================== */

Type
ColumnReferenceExpression::type() const {
    return Type::COLUMN_REFERENCE_EXPRESSION;
}

Type
LiteralExpression::type() const {
    return Type::LITERAL_EXPRESSION;
}

Type
FunctionCallExpression::type() const {
    return Type::FUNCTION_CALL_EXPRESSION;
}

Type
EqualToExpression::type() const {
    return Type::EQUAL_TO_EXPRESSION;
}

Type
LessThanExpression::type() const {
    return Type::LESS_THAN_EXPRESSION;
}

Type
LessThanEqualExpression::type() const {
    return Type::LESS_THAN_EQUAL_EXPRESSION;
}

Type
GreaterThanExpression::type() const {
    return Type::GREATER_THAN_EXPRESSION;
}

Type
GreaterThanEqualExpression::type() const {
    return Type::GREATER_THAN_EQUAL_EXPRESSION;
}

Type
AddExpression::type() const {
    return Type::ADD_EXPRESSION;
}

Type
SubtractExpression::type() const {
    return Type::SUBTRACT_EXPRESSION;
}

Type
MultiplyExpression::type() const {
    return Type::MULTIPLY_EXPRESSION;
}

Type
DivideExpression::type() const {
    return Type::DIVIDE_EXPRESSION;
}

/* === Tables =============================================================== */

Type
TableName::type() const {
    return Type::TABLE_NAME;
}

/* === Distinct ============================================================= */

Type
DistinctClause::type() const {
    return Type::DISTINCT_CLAUSE;
}

/* === Column Bindings ====================================================== */

Type
WildcardColumnBinding::type() const {
    return Type::WILDCARD_COLUMN_BINDING;
}

Type
ImplicitColumnBinding::type() const {
    return Type::IMPLICIT_COLUMN_BINDING;
}

Type
AliasedColumnBinding::type() const {
    return Type::ALIASED_COLUMN_BINDING;
}

/* === From ================================================================= */

Type
ImplicitTableBinding::type() const {
    return Type::IMPLICIT_TABLE_BINDING;
}

Type
AliasedTableBinding::type() const {
    return Type::ALIASED_TABLE_BINDING;
}

Type
FromClause::type() const {
    return Type::FROM_CLAUSE;
}

/* === Joins ================================================================ */

Type
JoinOnConstraint::type() const {
    return Type::JOIN_ON_CONSTRAINT;
}

Type
JoinUsingConstraint::type() const {
    return Type::JOIN_USING_CONSTRAINT;
}

Type
JoinClause::type() const {
    return Type::JOIN_CLAUSE;
}

/* === Filtering ============================================================ */

Type
WhereClause::type() const {
    return Type::WHERE_CLAUSE;
}

/* === Grouping ============================================================= */

Type
GroupByClause::type() const {
    return Type::GROUP_BY_CLAUSE;
}

/* === Table Expressions ==================================================== */

Type
SelectExpression::type() const {
    return Type::SELECT_EXPRESSION;
}

Type
ImportExpression::type() const {
    return Type::IMPORT_EXPRESSION;
}

Type
TableReferenceExpression::type() const {
    return Type::TABLE_REFERENCE_EXPRESSION;
}

/* === Statements =========================================================== */

Type
AssignmentStatement::type() const {
    return Type::ASSIGNMENT_STATEMENT;
}

Type
UpdateStatement::type() const {
    return Type::UPDATE_STATEMENT;
}

Type
DeleteStatement::type() const {
    return Type::DELETE_STATEMENT;
}

Type
InsertStatement::type() const {
    return Type::INSERT_STATEMENT;
}

Type
ExportStatement::type() const {
    return Type::EXPORT_STATEMENT;
}

Type
BeginStatement::type() const {
    return Type::BEGIN_STATEMENT;
}

/* === Scripts ============================================================== */

Type
Script::type() const {
    return Type::SCRIPT;
}

} /* namespace ast */
} /* namespace dtl */
