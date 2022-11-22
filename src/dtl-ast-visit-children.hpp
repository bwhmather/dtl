#pragma once

#include <functional>

#include "dtl-ast.hpp"

namespace dtl {
namespace ast {

void
visit_children(const Node& node, std::function<void(const Node&)> callback);

void
visit_children(const Node& node, NodeVisitor& visitor);

} /* namespace ast */
} /* namespace dtl */
