#include "dtl-ast.hpp"

#include <stddef.h>
#include <vector>
#include <string>
#include <memory>

namespace dtl {
namespace ast {

/* === Literals ============================================================= */

void Literal::accept(NodeVisitor& visitor) {
    this->accept(static_cast<LiteralVisitor&>(visitor));
}

void String::accept(LiteralVisitor& visitor) {
    visitor.visit_string(*this);
}

/* === Columns ============================================================== */

void ColumnName::accept(NodeVisitor& visitor) {
    this->accept(static_cast<ColumnNameVisitor&>(visitor));
}

void UnqualifiedColumnName::accept(ColumnNameVisitor& visitor) {
    visitor.visit_unqualified_column_name(*this);
}

void QualifiedColumnName::accept(ColumnNameVisitor& visitor) {
    visitor.visit_qualified_column_name(*this);
}

void Expression::accept(NodeVisitor& visitor) {
    this->accept(static_cast<ExpressionVisitor&>(visitor));
}

void ColumnReferenceExpression::accept(ExpressionVisitor& visitor) {
    visitor.visit_column_reference_expression(*this);
}

void LiteralExpression::accept(ExpressionVisitor& visitor) {
    visitor.visit_literal_expression(*this);
}

void FunctionCallExpression::accept(ExpressionVisitor& visitor) {
    visitor.visit_function_call_expression(*this);
}

void AddExpression::accept(ExpressionVisitor& visitor) {
    visitor.visit_add_expression(*this);
}

void SubtractExpression::accept(ExpressionVisitor& visitor) {
    visitor.visit_subtract_expression(*this);
}

void MultiplyExpression::accept(ExpressionVisitor& visitor) {
    visitor.visit_multiply_expression(*this);
}

void DivideExpression::accept(ExpressionVisitor& visitor) {
    visitor.visit_divide_expression(*this);
}

/* === Tables =============================================================== */

void TableName::accept(NodeVisitor& visitor) {
    visitor.visit_table_name(*this);
}

/* === Distinct ============================================================= */

void DistinctClause::accept(NodeVisitor& visitor) {
    visitor.visit_distinct_clause(*this);
}

/* === Column Bindings ====================================================== */

void ColumnBinding::accept(NodeVisitor& visitor) {
    this->accept(static_cast<ColumnBindingVisitor&>(visitor));
}

void WildcardColumnBinding::accept(ColumnBindingVisitor& visitor) {
    visitor.visit_wildcard_column_binding(*this);
}

void ImplicitColumnBinding::accept(ColumnBindingVisitor& visitor) {
    visitor.visit_implicit_column_binding(*this);
}

void AliasedColumnBinding::accept(ColumnBindingVisitor& visitor) {
    visitor.visit_aliased_column_binding(*this);
}

/* === From ================================================================= */

void TableBinding::accept(NodeVisitor& visitor) {
    this->accept(static_cast<TableBindingVisitor&>(visitor));
}

void ImplicitTableBinding::accept(TableBindingVisitor& visitor) {
    visitor.visit_implicit_table_binding(*this);
}

void AliasedTableBinding::accept(TableBindingVisitor& visitor) {
    visitor.visit_aliased_table_binding(*this);
}

void FromClause::accept(NodeVisitor& visitor) {
    visitor.visit_from_clause(*this);
}

/* === Joins ================================================================ */

void JoinConstraint::accept(NodeVisitor& visitor) {
    this->accept(static_cast<JoinConstraintVisitor&>(visitor));
}

void JoinOnConstraint::accept(JoinConstraintVisitor& visitor) {
    visitor.visit_join_on_constraint(*this);
}

void JoinUsingConstraint::accept(JoinConstraintVisitor& visitor) {
    visitor.visit_join_using_constraint(*this);
}

void JoinClause::accept(NodeVisitor& visitor) {
    visitor.visit_join_clause(*this);
}

/* === Filtering ============================================================ */

void WhereClause::accept(NodeVisitor& visitor) {
    visitor.visit_where_clause(*this);
}

/* === Grouping ============================================================= */

void GroupByClause::accept(NodeVisitor& visitor) {
    visitor.visit_group_by_clause(*this);
}

/* === Table Expressions ==================================================== */

void TableExpression::accept(NodeVisitor& visitor) {
    this->accept(static_cast<TableExpressionVisitor&>(visitor));
}

void SelectExpression::accept(TableExpressionVisitor& visitor) {
    visitor.visit_select_expression(*this);
}

void ImportExpression::accept(TableExpressionVisitor& visitor) {
    visitor.visit_import_expression(*this);
}

void TableReferenceExpression::accept(TableExpressionVisitor& visitor) {
    visitor.visit_table_reference_expression(*this);
}

/* === Statements =========================================================== */

void Statement::accept(NodeVisitor& visitor) {
    this->accept(static_cast<StatementVisitor&>(visitor));
}

void AssignmentStatement::accept(StatementVisitor& visitor) {
    visitor.visit_assignment_statement(*this);
}

void UpdateStatement::accept(StatementVisitor& visitor) {
    visitor.visit_update_statement(*this);
}

void DeleteStatement::accept(StatementVisitor& visitor) {
    visitor.visit_delete_statement(*this);
}

void InsertStatement::accept(StatementVisitor& visitor) {
    visitor.visit_insert_statement(*this);
}

void ExportStatement::accept(StatementVisitor& visitor) {
    visitor.visit_export_statement(*this);
}

/* === Scripts ============================================================== */

void Script::accept(NodeVisitor& visitor) {
    visitor.visit_script(*this);
}

} /* namespace ast */
} /* namespace dtl */

