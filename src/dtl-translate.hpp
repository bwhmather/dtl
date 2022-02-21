#pragma once

#include "dtl-ast.hpp"
#include "dtl-ir.hpp"

namespace dtl {
namespace translate {

dtl::ir::Program ast_to_ir(dtl::ast::Script script);

}  /* namespace translate */
}  /* namespace dtl */
