#include "dtl-ir-dependencies.hpp"

#include <functional>

#include "dtl-ir.hpp"

namespace dtl {
namespace ir {

class ImmediateDependencyVisitor : public ExpressionVisitor {
    std::function<void(const Expression& expression)> m_callback;

  public:
    ImmediateDependencyVisitor(
        std::function<void(const Expression& expression)> callback
    ) : m_callback(callback) {}

    void
    visit_import_shape_expression(
        const ImportShapeExpression& expression
    ) override final {
        (void) expression;
    }
    void
    visit_where_shape_expression(
        const WhereShapeExpression& expression
    ) override final {
        m_callback(*expression.mask);
    }

    void
    visit_join_shape_expression(
        const JoinShapeExpression& expression
    ) override final {
        m_callback(*expression.shape_a);
        m_callback(*expression.shape_b);
    }

    void
    visit_import_expression(
        const ImportExpression& expression
    ) override final {
        (void) expression;
    }

    void
    visit_where_expression(
        const WhereExpression& expression
    ) override final {
        m_callback(*expression.source);
        m_callback(*expression.mask);
    }

    void
    visit_pick_expression(
        const PickExpression& expression
    ) override final {
        m_callback(*expression.source);
        m_callback(*expression.indexes);
    }

    void
    visit_index_expression(
        const IndexExpression& expression
    ) override final {
        m_callback(*expression.source);
    }

    void
    visit_join_left_expression(
        const JoinLeftExpression& expression
    ) override final {
        m_callback(*expression.left);
        m_callback(*expression.right);
    }

    void
    visit_join_right_expression(
        const JoinRightExpression& expression
    ) override final {
        m_callback(*expression.left);
        m_callback(*expression.right);
    }

    void
    visit_add_expression(
        const AddExpression& expression
    ) override final {
        m_callback(*expression.left);
        m_callback(*expression.right);
    }

    void
    visit_subtract_expression(
        const SubtractExpression& expression
    ) override final {
        m_callback(*expression.left);
        m_callback(*expression.right);
    }

    void
    visit_multiply_expression(
        const MultiplyExpression& expression
    ) override final {
        m_callback(*expression.left);
        m_callback(*expression.right);
    }

    void
    visit_divide_expression(
        const DivideExpression& expression
    ) override final {
        m_callback(*expression.left);
        m_callback(*expression.right);
    }
};

void
for_each_direct_dependency(
    const Expression& root_expression,
    std::function<void(const Expression&)> callback
) {
    ImmediateDependencyVisitor visitor(callback);
    root_expression.accept(visitor);
}

void
visit_direct_dependencies(
    const Expression& root_expression,
    ExpressionVisitor& visitor
) {
    for_each_direct_dependency(
        root_expression,
        [&](const Expression& expression) {
            expression.accept(visitor);
        }
    );
}

} /* namespace ir */
} /* namespace dtl */

