#pragma once

#include "dtl-ir.hpp"

namespace dtl {
namespace ir {

template <typename T>
class DefaultedShapeExpressionVisitorMixin : public T {
  public:
    virtual void
    visit_import_shape_expression(
        const ImportShapeExpression& expression) override {
        return visit_shape_expression(expression);
    }

    virtual void
    visit_join_shape_expression(
        const JoinShapeExpression& expression) override {
        return visit_shape_expression(expression);
    }

    virtual void
    visit_where_shape_expression(
        const WhereShapeExpression& expression) override {
        return visit_shape_expression(expression);
    }

    virtual void
    visit_shape_expression(const ShapeExpression& expression) = 0;
};

template <typename T>
class DefaultedArrayExpressionVisitorMixin : public T {
  public:
    virtual void
    visit_import_expression(const ImportExpression& expression) override {
        return visit_array_expression(expression);
    }

    virtual void
    visit_where_expression(const WhereExpression& expression) override {
        return visit_array_expression(expression);
    }

    virtual void
    visit_pick_expression(const PickExpression& expression) override {
        return visit_array_expression(expression);
    }

    virtual void
    visit_index_expression(const IndexExpression& expression) override {
        return visit_array_expression(expression);
    }

    virtual void
    visit_join_left_expression(const JoinLeftExpression& expression) override {
        return visit_array_expression(expression);
    }

    virtual void
    visit_join_right_expression(
        const JoinRightExpression& expression) override {
        return visit_array_expression(expression);
    }

    virtual void
    visit_add_expression(const AddExpression& expression) override {
        return visit_array_expression(expression);
    }

    virtual void
    visit_subtract_expression(const SubtractExpression& expression) override {
        return visit_array_expression(expression);
    }

    virtual void
    visit_multiply_expression(const MultiplyExpression& expression) override {
        return visit_array_expression(expression);
    }

    virtual void
    visit_divide_expression(const DivideExpression& expression) override {
        return visit_array_expression(expression);
    }

    virtual void
    visit_array_expression(const ArrayExpression& expression) = 0;
};

template <typename T>
class DefaultedExpressionVisitorMixin :
    public DefaultedShapeExpressionVisitorMixin<
        DefaultedArrayExpressionVisitorMixin<T>> {
  public:
    virtual void
    visit_shape_expression(const ShapeExpression& expression) override {
        visit_expression(expression);
    }

    virtual void
    visit_array_expression(const ArrayExpression& expression) override {
        visit_expression(expression);
    }

    virtual void
    visit_expression(const Expression& expression) = 0;
};

} /* namespace ir */
} /* namespace dtl */
