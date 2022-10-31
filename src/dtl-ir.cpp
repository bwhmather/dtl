#include "dtl-ir.hpp"

namespace dtl {
namespace ir {

/* === Expressions ========================================================== */

/* --- Shape Expressions ---------------------------------------------------- */

void ShapeExpression::accept(ExpressionVisitor& visitor) const {
    this->accept(static_cast<ShapeExpressionVisitor&>(visitor));
}

void ImportShapeExpression::accept(ShapeExpressionVisitor& visitor) const {
    visitor.visit_import_shape_expression(*this);
}

void WhereShapeExpression::accept(ShapeExpressionVisitor& visitor) const {
    visitor.visit_where_shape_expression(*this);
}

void JoinShapeExpression::accept(ShapeExpressionVisitor& visitor) const {
    visitor.visit_join_shape_expression(*this);
}

/* --- Array Expressions ---------------------------------------------------- */

void ArrayExpression::accept(ExpressionVisitor& visitor) const {
    this->accept(static_cast<ArrayExpressionVisitor&>(visitor));
}

void ImportExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_import_expression(*this);
}

void WhereExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_where_expression(*this);
}

void PickExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_pick_expression(*this);
}

void IndexExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_index_expression(*this);
}

void JoinLeftExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_join_left_expression(*this);
}

void JoinRightExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_join_right_expression(*this);
}

void AddExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_add_expression(*this);
}

void SubtractExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_subtract_expression(*this);
}

void MultiplyExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_multiply_expression(*this);
}

void DivideExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_divide_expression(*this);
}

/* === Tables =============================================================== */

void TraceTable::accept(TableVisitor& visitor) const {
    visitor.visit_trace_table(*this);
}

void ExportTable::accept(TableVisitor& visitor) const {
    visitor.visit_export_table(*this);
}

}  /* namespace ir */
}  /* namespace dtl */
