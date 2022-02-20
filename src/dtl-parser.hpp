#pragma once

#include <memory>

#include "dtl-ast.hpp"
#include "dtl-tokenizer.hpp"

namespace dtl {
namespace parser {

std::unique_ptr<dtl::ast::Script> parse(dtl::tokenizer::Tokenizer& tokenizer);


}  /* namespace parser */
}  /* namespace dtl */

