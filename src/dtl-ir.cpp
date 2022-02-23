#include "dtl-ir.hpp"

namespace dtl {
namespace ir {

/* === Expressions ========================================================== */

void ImportExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_import_expression(*this);
}

void WhereExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_where_expression(*this);
}

void PickExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_pick_expression(*this);
}

void IndexExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_index_expression(*this);
}

void JoinLeftExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_join_left_expression(*this);
}

void JoinRightExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_join_right_expression(*this);
}

void AddExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_add_expression(*this);
}

void SubtractExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_subtract_expression(*this);
}

void MultiplyExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_multiply_expression(*this);
}

void DivideExpression::accept(ExpressionVisitor& visitor) const {
    visitor.visit_divide_expression(*this);
}

}  /* namespace ir */
}  /* namespace dtl */
