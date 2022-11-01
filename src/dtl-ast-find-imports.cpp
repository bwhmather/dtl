#include "dtl-ast-find-imports.hpp"

#include <string>
#include <vector>

#include "dtl-ast.hpp"
#include "dtl-ast-defaulted-visitor.tpp"
#include "dtl-ast-visit-children.hpp"

namespace dtl {
namespace ast {

class FindImportsVisitor :
    public dtl::ast::DefaultedNodeVisitorMixin<NodeVisitor>
{
  public:
    std::vector<std::string> imports;

    void visit_import_expression(const ImportExpression& expr) override final {
        imports.push_back(expr.location->value);
    };

    void visit_node(const Node& node) override final {
        visit_children(node, *this);
    }
};

// Note that this returns a vector rather than an unordered set.  Opening files
// in the same order as they are imported makes error reporting more predicable.
std::vector<std::string> find_imports(dtl::ast::Script& script) {
    FindImportsVisitor visitor;
    script.accept(visitor);
    return std::move(visitor.imports);
}

}  /* namespace ast */
}  /* namespace dtl */
