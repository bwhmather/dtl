#include "dtl-ir-to-cmd.hpp"

#include <cassert>
#include <memory>
#include <vector>

#include "dtl-ast.hpp"
#include "dtl-ir-defaulted-visitor.tpp"
#include "dtl-ir-dependencies.hpp"
#include "dtl-ir.hpp"

namespace dtl {
namespace ir {

void
for_each_dependency(
    const std::vector<dtl::variant_ptr<const Expression>>& roots,
    std::function<void(dtl::variant_ptr<const Expression>)> callback
) {
    // A vector of all unyielded expressions in approximate reverse dependency
    // order.  After traversing the graph, the last root will be the first
    // expression in the list, followed by its dependencies, followed by the
    // next root, and so on.
    std::vector<dtl::variant_ptr<const Expression>> expressions;
    std::unordered_map<dtl::variant_ptr<const Expression>, int> refcounts;

    {
        // Traverse and build list of all reachable expressions plus mapping to
        // refcounts.
        std::vector<dtl::variant_ptr<const Expression>> queue;

        // Enqueue roots with a refcount of zero.
        for (auto&& root : roots) {
            queue.push_back(root);
            refcounts[root] = 0;
        }

        while (queue.size()) {
            auto next = queue.back();
            queue.pop_back();

            expressions.push_back(next);

            for_each_direct_dependency(
                next, [&](dtl::variant_ptr<const Expression> dependency) {
                    // Queue the dependency for traversal.
                    if (!refcounts.contains(dependency)) {
                        queue.push_back(dependency);
                        refcounts[dependency] = 0;
                    }

                    refcounts[dependency]++;
                }
            );
        }
    }

    // Yield all reachable expressions in dependency order.
    while (expressions.size()) {
        // Find and remove next expression with a refcount of zero.  Worst case
        // is O(n^2), but typical case should be closer to O(n) as expressions
        // should be roughly in order.
        auto unblocked = [&](dtl::variant_ptr<const Expression> expression) {
            return refcounts[expression] == 0;
        };

        auto cursor =
            std::find_if(expressions.rbegin(), expressions.rend(), unblocked);
        assert(cursor != expressions.rend());

        dtl::variant_ptr<const Expression> expression = *cursor;
        expressions.erase(cursor.base() - 1);

        // Decrement refcount of all dependencies.
        for_each_direct_dependency(
            expression,
            [&](dtl::variant_ptr<const Expression> dependency) { refcounts[dependency]--; }
        );

        // Call callback on removed expression.
        callback(expression);
    }
}

std::vector<std::unique_ptr<const dtl::cmd::Command>>
to_cmd(std::vector<dtl::variant_ptr<const dtl::ir::Expression>>& roots) {
    std::vector<std::unique_ptr<const dtl::cmd::Command>> commands;

    for_each_dependency(
        roots, [&](dtl::variant_ptr<const Expression> base_dependency) {
            if (auto dependency = dtl::get_if<dtl::variant_ptr<const ShapeExpression>>(base_dependency)) {
                auto command = std::make_unique<dtl::cmd::EvaluateShapeCommand>(
                    dtl::acquire(dependency)
                );

                commands.push_back(std::move(command));
            }

            if (auto dependency = dtl::get_if<dtl::variant_ptr<const ArrayExpression>>(base_dependency)) {
                auto command = std::make_unique<dtl::cmd::EvaluateArrayCommand>(
                    dtl::acquire(dependency)
                );

                commands.push_back(std::move(command));
            }
        }
    );

    return commands;
}

} /* namespace ir */
} /* namespace dtl */
