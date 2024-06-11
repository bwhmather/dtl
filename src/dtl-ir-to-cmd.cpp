#include "dtl-ir-to-cmd.hpp"

#include <algorithm>
#include <functional>
#include <memory>
#include <unordered_set>
#include <utility>
#include <vector>

#include "dtl-cmd.hpp"
#include "dtl-ir-dependencies.hpp"
#include "dtl-ir.hpp"
#include "dtl-variant.tpp"

namespace dtl {
namespace ir {

void
for_each_dependency(
    const std::vector<dtl::variant_ptr<const Expression>>& roots,
    std::function<void(dtl::variant_ptr<const Expression>)> callback
) {
    std::unordered_set<dtl::variant_ptr<const Expression>> visited;
    std::vector<dtl::variant_ptr<const Expression>> stack(roots);
    std::vector<bool> expanded(roots.size());

    while (stack.size()) {
        dtl::variant_ptr<const Expression> next = stack.back();

        if (visited.contains(next)) {
            // Ignore
            stack.pop_back();
            expanded.pop_back();
            continue;
        }

        if (!expanded.back()) {
            // Expand
            expanded.back() = true;

            for_each_direct_dependency(
                next, [&](dtl::variant_ptr<const Expression> dependency) {
                    stack.push_back(dependency);
                    expanded.push_back(false);
                }
            );
            continue;
        }

        stack.pop_back();
        expanded.pop_back();

        visited.insert(next);
        callback(next);
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
