#include "dtl-ast.hpp"

#include <memory>
#include <stddef.h>
#include <string>
#include <vector>

namespace dtl {
namespace ast {

/* === Literals ============================================================= */

Type String::type() const {
    return Type::STRING;
}

/* === Columns ============================================================== */

Type UnqualifiedColumnName::type() const {
    return Type::UNQUALIFIED_COLUMN_NAME;
}

Type QualifiedColumnName::type() const {
    return Type::QUALIFIED_COLUMN_NAME;
}

/* === Expressions ========================================================== */

Type ColumnReferenceExpression::type() const {
    return Type::COLUMN_REFERENCE_EXPRESSION;
}

Type LiteralExpression::type() const {
    return Type::LITERAL_EXPRESSION;
}

Type FunctionCallExpression::type() const {
    return Type::FUNCTION_CALL_EXPRESSION;
}

Type EqualToExpression::type() const {
    return Type::EQUAL_TO_EXPRESSION;
}

Type LessThanExpression::type() const {
    return Type::LESS_THAN_EXPRESSION;
}

Type LessThanEqualExpression::type() const {
    return Type::LESS_THAN_EQUAL_EXPRESSION;
}

Type GreaterThanExpression::type() const {
    return Type::GREATER_THAN_EXPRESSION;
}

Type GreaterThanEqualExpression::type() const {
    return Type::GREATER_THAN_EQUAL_EXPRESSION;
}

Type AddExpression::type() const {
    return Type::ADD_EXPRESSION;
}

Type SubtractExpression::type() const {
    return Type::SUBTRACT_EXPRESSION;
}

Type MultiplyExpression::type() const {
    return Type::MULTIPLY_EXPRESSION;
}

Type DivideExpression::type() const {
    return Type::DIVIDE_EXPRESSION;
}

/* === Tables =============================================================== */

Type TableName::type() const {
    return Type::TABLE_NAME;
}

/* === Distinct ============================================================= */

Type DistinctClause::type() const {
    return Type::DISTINCT_CLAUSE;
}

/* === Column Bindings ====================================================== */

Type WildcardColumnBinding::type() const {
    return Type::WILDCARD_COLUMN_BINDING;
}

Type ImplicitColumnBinding::type() const {
    return Type::IMPLICIT_COLUMN_BINDING;
}

Type AliasedColumnBinding::type() const {
    return Type::ALIASED_COLUMN_BINDING;
}

/* === From ================================================================= */

Type ImplicitTableBinding::type() const {
    return Type::IMPLICIT_TABLE_BINDING;
}

Type AliasedTableBinding::type() const {
    return Type::ALIASED_TABLE_BINDING;
}

Type FromClause::type() const {
    return Type::FROM_CLAUSE;
}

/* === Joins ================================================================ */

Type JoinOnConstraint::type() const {
    return Type::JOIN_ON_CONSTRAINT;
}

Type JoinUsingConstraint::type() const {
    return Type::JOIN_USING_CONSTRAINT;
}

Type JoinClause::type() const {
    return Type::JOIN_CLAUSE;
}

/* === Filtering ============================================================ */

Type WhereClause::type() const {
    return Type::WHERE_CLAUSE;
}

/* === Grouping ============================================================= */

Type GroupByClause::type() const {
    return Type::GROUP_BY_CLAUSE;
}

/* === Table Expressions ==================================================== */

Type SelectExpression::type() const {
    return Type::SELECT_EXPRESSION;
}

Type ImportExpression::type() const {
    return Type::IMPORT_EXPRESSION;
}

Type TableReferenceExpression::type() const {
    return Type::TABLE_REFERENCE_EXPRESSION;
}

/* === Statements =========================================================== */

Type AssignmentStatement::type() const {
    return Type::ASSIGNMENT_STATEMENT;
}

Type UpdateStatement::type() const {
    return Type::UPDATE_STATEMENT;
}

Type DeleteStatement::type() const {
    return Type::DELETE_STATEMENT;
}

Type InsertStatement::type() const {
    return Type::INSERT_STATEMENT;
}

Type ExportStatement::type() const {
    return Type::EXPORT_STATEMENT;
}

Type BeginStatement::type() const {
    return Type::BEGIN_STATEMENT;
}

/* === Scripts ============================================================== */

Type Script::type() const {
    return Type::SCRIPT;
}


/* === Literals ============================================================= */

void
Literal::accept(NodeVisitor& visitor) const {
    this->accept(static_cast<LiteralVisitor&>(visitor));
}

void
String::accept(LiteralVisitor& visitor) const {
    visitor.visit_string(*this);
}

/* === Columns ============================================================== */

void
ColumnName::accept(NodeVisitor& visitor) const {
    this->accept(static_cast<ColumnNameVisitor&>(visitor));
}

void
UnqualifiedColumnName::accept(ColumnNameVisitor& visitor) const {
    visitor.visit_unqualified_column_name(*this);
}

void
QualifiedColumnName::accept(ColumnNameVisitor& visitor) const {
    visitor.visit_qualified_column_name(*this);
}

/* === Expressions ========================================================== */

void
Expression::accept(NodeVisitor& visitor) const {
    this->accept(static_cast<ExpressionVisitor&>(visitor));
}

void
ColumnReferenceExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_column_reference_expression(*this);
}

void
LiteralExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_literal_expression(*this);
}

void
FunctionCallExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_function_call_expression(*this);
}

void
EqualToExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_equal_to_expression(*this);
}

void
LessThanExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_less_than_expression(*this);
}

void
LessThanEqualExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_less_than_equal_expression(*this);
}

void
GreaterThanExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_greater_than_expression(*this);
}

void
GreaterThanEqualExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_greater_than_equal_expression(*this);
}

void
AddExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_add_expression(*this);
}

void
SubtractExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_subtract_expression(*this);
}

void
MultiplyExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_multiply_expression(*this);
}

void
DivideExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_divide_expression(*this);
}

/* === Tables =============================================================== */

void
TableName::accept(NodeVisitor& visitor) const {
    visitor.visit_table_name(*this);
}

/* === Distinct ============================================================= */

void
DistinctClause::accept(NodeVisitor& visitor) const {
    visitor.visit_distinct_clause(*this);
}

/* === Column Bindings ====================================================== */

void
ColumnBinding::accept(NodeVisitor& visitor) const {
    this->accept(static_cast<ColumnBindingVisitor&>(visitor));
}

void
WildcardColumnBinding::accept(ColumnBindingVisitor& visitor) const {
    visitor.visit_wildcard_column_binding(*this);
}

void
ImplicitColumnBinding::accept(ColumnBindingVisitor& visitor) const {
    visitor.visit_implicit_column_binding(*this);
}

void
AliasedColumnBinding::accept(ColumnBindingVisitor& visitor) const {
    visitor.visit_aliased_column_binding(*this);
}

/* === From ================================================================= */

void
TableBinding::accept(NodeVisitor& visitor) const {
    this->accept(static_cast<TableBindingVisitor&>(visitor));
}

void
ImplicitTableBinding::accept(TableBindingVisitor& visitor) const {
    visitor.visit_implicit_table_binding(*this);
}

void
AliasedTableBinding::accept(TableBindingVisitor& visitor) const {
    visitor.visit_aliased_table_binding(*this);
}

void
FromClause::accept(NodeVisitor& visitor) const {
    visitor.visit_from_clause(*this);
}

/* === Joins ================================================================ */

void
JoinConstraint::accept(NodeVisitor& visitor) const {
    this->accept(static_cast<JoinConstraintVisitor&>(visitor));
}

void
JoinOnConstraint::accept(JoinConstraintVisitor& visitor) const {
    visitor.visit_join_on_constraint(*this);
}

void
JoinUsingConstraint::accept(JoinConstraintVisitor& visitor) const {
    visitor.visit_join_using_constraint(*this);
}

void
JoinClause::accept(NodeVisitor& visitor) const {
    visitor.visit_join_clause(*this);
}

/* === Filtering ============================================================ */

void
WhereClause::accept(NodeVisitor& visitor) const {
    visitor.visit_where_clause(*this);
}

/* === Grouping ============================================================= */

void
GroupByClause::accept(NodeVisitor& visitor) const {
    visitor.visit_group_by_clause(*this);
}

/* === Table Expressions ==================================================== */

void
TableExpression::accept(NodeVisitor& visitor) const {
    this->accept(static_cast<TableExpressionVisitor&>(visitor));
}

void
SelectExpression::accept(TableExpressionVisitor& visitor) const {
    visitor.visit_select_expression(*this);
}

void
ImportExpression::accept(TableExpressionVisitor& visitor) const {
    visitor.visit_import_expression(*this);
}

void
TableReferenceExpression::accept(TableExpressionVisitor& visitor) const {
    visitor.visit_table_reference_expression(*this);
}

/* === Statements =========================================================== */

void
Statement::accept(NodeVisitor& visitor) const {
    this->accept(static_cast<StatementVisitor&>(visitor));
}

void
AssignmentStatement::accept(StatementVisitor& visitor) const {
    visitor.visit_assignment_statement(*this);
}

void
UpdateStatement::accept(StatementVisitor& visitor) const {
    visitor.visit_update_statement(*this);
}

void
DeleteStatement::accept(StatementVisitor& visitor) const {
    visitor.visit_delete_statement(*this);
}

void
InsertStatement::accept(StatementVisitor& visitor) const {
    visitor.visit_insert_statement(*this);
}

void
ExportStatement::accept(StatementVisitor& visitor) const {
    visitor.visit_export_statement(*this);
}

void
BeginStatement::accept(StatementVisitor& visitor) const {
    visitor.visit_begin_statement(*this);
}

/* === Scripts ============================================================== */

void
Script::accept(NodeVisitor& visitor) const {
    visitor.visit_script(*this);
}

} /* namespace ast */
} /* namespace dtl */
