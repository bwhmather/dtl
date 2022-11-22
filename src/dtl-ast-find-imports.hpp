#pragma once

#include <string>
#include <vector>

#include "dtl-ast.hpp"

namespace dtl {
namespace ast {

// Note that this returns a vector rather than an unordered set.  Opening files
// in the same order as they are imported makes error reporting more predicable.
std::vector<std::string>
find_imports(dtl::ast::Script& script);

} /* namespace ast */
} /* namespace dtl */
