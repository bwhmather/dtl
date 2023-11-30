#include "dtl-ast-find-imports.hpp"

#include <string>
#include <vector>

#include "dtl-ast-visit-children.hpp"
#include "dtl-ast.hpp"

namespace dtl {
namespace ast {

// Note that this returns a vector rather than an unordered set.  Opening files
// in the same order as they are imported makes error reporting more predicable.
std::vector<std::string>
find_imports(dtl::ast::Script& script) {
    std::vector<std::string> imports;
    visit_children(script, [&](const Node& node) {
        if (node.type() == Type::IMPORT_EXPRESSION) {
            const auto& expression = static_cast<const ImportExpression&>(node);
            imports.push_back(expression.location->value);
        }
    });
    return imports;
}

} /* namespace ast */
} /* namespace dtl */
