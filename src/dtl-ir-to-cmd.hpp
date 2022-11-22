#pragma once

#include <memory>
#include <vector>

#include "dtl-cmd.hpp"
#include "dtl-ir.hpp"

namespace dtl {
namespace ir {

std::vector<std::unique_ptr<const dtl::cmd::Command>>
to_cmd(std::vector<std::shared_ptr<const dtl::ir::Expression>> roots);

} /* namespace ir */
} /* namespace dtl */
