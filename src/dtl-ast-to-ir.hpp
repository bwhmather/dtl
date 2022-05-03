#pragma once

#include "dtl-ast.hpp"
#include "dtl-io.hpp"
#include "dtl-ir.hpp"

namespace dtl {
namespace ast {

dtl::ir::Program to_ir(dtl::ast::Script& script, dtl::io::Importer& importer);

}  /* namespace ast */
}  /* namespace dtl */
