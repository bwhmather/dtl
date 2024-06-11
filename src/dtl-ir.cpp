#include "dtl-ir.hpp"

#include <memory>

#include "dtl-variant.tpp"

namespace dtl {
namespace ir {

/* === Expressions ========================================================== */

DType
expression_dtype(dtl::variant_ptr<const ArrayExpression> expression) {
    auto base_expression = dtl::cast<const dtl::ir::BaseArrayExpression*>(expression);
    return base_expression->dtype;
}

dtl::shared_variant_ptr<const ShapeExpression>
expression_shape(dtl::variant_ptr<const ArrayExpression> expression) {
    auto base_expression = dtl::cast<const dtl::ir::BaseArrayExpression*>(expression);
    return base_expression->shape;
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
