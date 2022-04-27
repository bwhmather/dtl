#pragma once

#include "dtl-ast.hpp"
#include "dtl-ir.hpp"

namespace dtl {
namespace ast {

dtl::ir::Program to_ir(dtl::ast::Script script);

}  /* namespace ast */
}  /* namespace dtl */
