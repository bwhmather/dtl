#pragma once

#include <functional>

#include "dtl-ir.hpp"

namespace dtl {
namespace ir {

void
for_each_direct_dependency(
    const Expression& node,
    std::function<void(const Expression&)> callback
);

void
visit_direct_dependencies(
    const Expression& node,
    ExpressionVisitor& visitor
);

} /* namespace ir */
} /* namespace dtl */

