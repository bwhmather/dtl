#include "dtl-ir.hpp"

namespace dtl {
namespace ir {

/* === Expressions ========================================================== */

std::shared_ptr<const Expression>
Expression::get_ptr() const {
    return shared_from_this();
}

/* --- Shape Expressions ---------------------------------------------------- */

void
ShapeExpression::accept(ExpressionVisitor& visitor) const {
    this->accept(static_cast<ShapeExpressionVisitor&>(visitor));
}

std::shared_ptr<const ShapeExpression>
ShapeExpression::get_ptr() const {
    auto expr_ptr = this->shared_from_this();
    return std::static_pointer_cast<const ShapeExpression>(expr_ptr);
}

void
ImportShapeExpression::accept(ShapeExpressionVisitor& visitor) const {
    visitor.visit_import_shape_expression(*this);
}

std::shared_ptr<const ImportShapeExpression>
ImportShapeExpression::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const ImportShapeExpression>(expr_ptr);
}

void
WhereShapeExpression::accept(ShapeExpressionVisitor& visitor) const {
    visitor.visit_where_shape_expression(*this);
}

std::shared_ptr<const WhereShapeExpression>
WhereShapeExpression::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const WhereShapeExpression>(expr_ptr);
}

void
JoinShapeExpression::accept(ShapeExpressionVisitor& visitor) const {
    visitor.visit_join_shape_expression(*this);
}

std::shared_ptr<const JoinShapeExpression>
JoinShapeExpression::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const JoinShapeExpression>(expr_ptr);
}

/* --- Array Expressions ---------------------------------------------------- */

void
ArrayExpression::accept(ExpressionVisitor& visitor) const {
    this->accept(static_cast<ArrayExpressionVisitor&>(visitor));
}

std::shared_ptr<const ArrayExpression>
ArrayExpression::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const ArrayExpression>(expr_ptr);
}

void
ImportExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_import_expression(*this);
}

std::shared_ptr<const ImportExpression>
ImportExpression::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const ImportExpression>(expr_ptr);
}

void
WhereExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_where_expression(*this);
}

std::shared_ptr<const WhereExpression>
WhereExpression::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const WhereExpression>(expr_ptr);
}

void
PickExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_pick_expression(*this);
}

std::shared_ptr<const PickExpression>
PickExpression::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const PickExpression>(expr_ptr);
}

void
IndexExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_index_expression(*this);
}

std::shared_ptr<const IndexExpression>
IndexExpression::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const IndexExpression>(expr_ptr);
}

void
JoinLeftExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_join_left_expression(*this);
}

std::shared_ptr<const JoinLeftExpression>
JoinLeftExpression::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const JoinLeftExpression>(expr_ptr);
}

void
JoinRightExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_join_right_expression(*this);
}

std::shared_ptr<const JoinRightExpression>
JoinRightExpression::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const JoinRightExpression>(expr_ptr);
}

void
AddExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_add_expression(*this);
}

std::shared_ptr<const AddExpression>
AddExpression::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const AddExpression>(expr_ptr);
}

void
SubtractExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_subtract_expression(*this);
}

std::shared_ptr<const SubtractExpression>
SubtractExpression::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const SubtractExpression>(expr_ptr);
}

void
MultiplyExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_multiply_expression(*this);
}

std::shared_ptr<const MultiplyExpression>
MultiplyExpression::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const MultiplyExpression>(expr_ptr);
}

void
DivideExpression::accept(ArrayExpressionVisitor& visitor) const {
    visitor.visit_divide_expression(*this);
}

std::shared_ptr<const DivideExpression>
DivideExpression::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const DivideExpression>(expr_ptr);
}

/* === Tables =============================================================== */

std::shared_ptr<const Table>
Table::get_ptr() const {
    return shared_from_this();
}

void
TraceTable::accept(TableVisitor& visitor) const {
    visitor.visit_trace_table(*this);
}

std::shared_ptr<const TraceTable>
TraceTable::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const TraceTable>(expr_ptr);
}

void
ExportTable::accept(TableVisitor& visitor) const {
    visitor.visit_export_table(*this);
}

std::shared_ptr<const ExportTable>
ExportTable::get_ptr() const {
    auto expr_ptr = shared_from_this();
    return std::static_pointer_cast<const ExportTable>(expr_ptr);
}

} /* namespace ir */
} /* namespace dtl */
