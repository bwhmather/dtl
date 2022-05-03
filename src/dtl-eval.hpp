#pragma once

#include "dtl-ast.hpp"
#include "dtl-io.hpp"
#include "dtl-ir.hpp"

namespace dtl {
namespace eval {

void eval(dtl::ir::Program program, dtl::io::Importer& importer, dtl::io::Exporter& exporter);

}  /* namespace eval */
}  /* namespace dtl */
