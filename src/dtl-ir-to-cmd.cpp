#include "dtl-ir-to-cmd.hpp"

#include <memory>
#include <vector>

#include "dtl-ast.hpp"
#include "dtl-ir.hpp"

namespace dtl {
namespace ir {

std::vector<std::unique_ptr<const dtl::cmd::Command> >
to_cmd(std::vector<std::shared_ptr<const dtl::ir::Expression> > roots) {
    std::vector<std::unique_ptr<const dtl::cmd::Command> > result;
    return result;
}

}  /* namespace ir */
}  /* namespace dtl */
