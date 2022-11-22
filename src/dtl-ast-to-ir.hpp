#pragma once

#include "dtl-ast.hpp"
#include "dtl-io.hpp"
#include "dtl-ir.hpp"

namespace dtl {
namespace ast {

std::vector<std::shared_ptr<const dtl::ir::Table>>
to_ir(dtl::ast::Script& script, dtl::io::Importer& importer);

} /* namespace ast */
} /* namespace dtl */
