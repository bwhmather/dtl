#include "dtl-ir-dependencies.hpp"

#include <functional>
#include <stdexcept>

#include "dtl-ir.hpp"
#include "dtl-variant.tpp"

namespace dtl {
namespace ir {

void
for_each_direct_dependency(
    dtl::variant_ptr<const Expression> base_expression,
    std::function<void(dtl::variant_ptr<const Expression>)> callback
) {
    if (dtl::get_if<const ImportShapeExpression*>(base_expression)) {
        return;
    }

    if (auto expression = dtl::get_if<const WhereShapeExpression*>(base_expression)) {
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->mask)));
        return;
    }

    if (auto expression = dtl::get_if<const JoinShapeExpression*>(base_expression)) {
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->shape_a)));
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->shape_b)));
        return;
    }

    if (dtl::get_if<const ImportExpression*>(base_expression)) {
        return;
    }

    if (auto expression = dtl::get_if<const WhereExpression*>(base_expression)) {
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->source)));
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->mask)));
        return;
    }

    if (auto expression = dtl::get_if<const PickExpression*>(base_expression)) {
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->source)));
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->indexes)));
        return;
    }

    if (auto expression = dtl::get_if<const IndexExpression*>(base_expression)) {
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->source)));
        return;
    }

    if (auto expression = dtl::get_if<const JoinLeftExpression*>(base_expression)) {
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->left)));
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->right)));
        return;
    }

    if (auto expression = dtl::get_if<const JoinRightExpression*>(base_expression)) {
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->left)));
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->right)));
        return;
    }

    if (auto expression = dtl::get_if<const AddExpression*>(base_expression)) {
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->left)));
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->right)));
        return;
    }

    if (auto expression = dtl::get_if<const SubtractExpression*>(base_expression)) {
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->left)));
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->right)));
        return;
    }

    if (auto expression = dtl::get_if<const MultiplyExpression*>(base_expression)) {
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->left)));
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->right)));
        return;
    }

    if (auto expression = dtl::get_if<const DivideExpression*>(base_expression)) {
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->left)));
        callback(dtl::cast<dtl::variant_ptr<const Expression>>(dtl::borrow(expression->right)));
        return;
    }

    throw std::logic_error("Not implemented");
}

} /* namespace ir */
} /* namespace dtl */
