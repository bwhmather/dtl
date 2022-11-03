#include "dtl-ir-to-cmd.hpp"

#include <cassert>
#include <memory>
#include <vector>

#include "dtl-ast.hpp"
#include "dtl-ir.hpp"
#include "dtl-ir-dependencies.hpp"
#include "dtl-ir-defaulted-visitor.tpp"

namespace dtl {
namespace ir {

void
for_each_dependency(
    const std::vector<std::shared_ptr<const Expression>>& roots,
    std::function<void(const Expression&)> callback
) {


    // A vector of all unyielded expressions in approximate reverse dependency
    // order.  After traversing the graph, the last root will be the first
    // expression in the list, followed by its dependencies, followed by the
    // next root, and so on.
    std::vector<const Expression*> expressions;
    std::unordered_map<const Expression*, int> refcounts;

    {
        // Traverse and build list of all reachable expressions plus mapping to
        // refcounts.
        std::vector<const Expression*> queue;

        // Enqueue roots with a refcount of zero.
        for (auto&& root : roots) {
            queue.push_back(&*root);
            refcounts[&*root] = 0;
        }

        while (queue.size()) {
            auto next = queue.back();
            queue.pop_back();

            expressions.push_back(next);

            for_each_direct_dependency(
                *next,
                [&](const Expression& dependency) {
                    // Queue the dependency for traversal.
                    if (!refcounts.contains(&dependency)) {
                        queue.push_back(&dependency);
                        refcounts[&dependency] = 0;
                    }

                    refcounts[&dependency]++;
                }
            );
        }
    }

    // Yield all reachable expressions in dependency order.
    while (expressions.size()) {
        // Find and remove next expression with a refcount of zero.  Worst case
        // is O(n^2), but typical case should be closer to O(n) as expressions
        // should be roughly in order.
        auto unblocked = [&](const Expression* expression) {
            return refcounts[expression] == 0;
        };

        auto cursor = std::find_if(
            expressions.rbegin(), expressions.rend(), unblocked
        );
        assert(cursor != expressions.rend());

        const Expression& expression = **cursor;
        expressions.erase(cursor.base() - 1);

        // Decrement refcount of all dependencies.
        for_each_direct_dependency(
            expression,
            [&](const Expression& dependency) {
                refcounts[&dependency]--;
            }
        );

        // Call callback on removed expression.
        callback(expression);
    }
}

void
visit_dependencies(
    const std::vector<std::shared_ptr<const Expression>>& roots,
    ExpressionVisitor& visitor
) {
    for_each_dependency(
        roots,
        [&](const Expression& expression) {
            expression.accept(visitor);
        }
    );
}

class ExpressionToCommandVisitor :
    public DefaultedShapeExpressionVisitorMixin<
        DefaultedArrayExpressionVisitorMixin<
            ExpressionVisitor
        >
    >
{
    std::vector<std::unique_ptr<const dtl::cmd::Command> > m_commands;

  public:
    void
    visit_shape_expression(
        const ShapeExpression& expression
    ) override {
        auto command = std::make_unique<dtl::cmd::EvaluateShapeCommand>(
            expression.get_ptr()
        );

        m_commands.push_back(std::move(command));
    }

    void visit_array_expression(
        const ArrayExpression& expression
    ) override {
        auto command = std::make_unique<dtl::cmd::EvaluateArrayCommand>(
            expression.get_ptr()
        );

        m_commands.push_back(std::move(command));
    }

    std::vector<std::unique_ptr<const dtl::cmd::Command> >
    result(void) {
        return std::move(m_commands);
    }
};

std::vector<std::unique_ptr<const dtl::cmd::Command> >
to_cmd(std::vector<std::shared_ptr<const dtl::ir::Expression> > roots) {
    ExpressionToCommandVisitor visitor;
    visit_dependencies(roots, visitor);
    return visitor.result();
}

}  /* namespace ir */
}  /* namespace dtl */
